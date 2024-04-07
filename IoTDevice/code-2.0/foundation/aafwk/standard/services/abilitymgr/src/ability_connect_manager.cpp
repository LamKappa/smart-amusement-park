/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ability_connect_manager.h"

#include <algorithm>

#include "hilog_wrapper.h"

#include "ability_manager_errors.h"
#include "ability_manager_service.h"
#include "ability_connect_callback_stub.h"

namespace OHOS {
namespace AAFwk {

AbilityConnectManager::AbilityConnectManager()
{}

AbilityConnectManager::~AbilityConnectManager()
{}

int AbilityConnectManager::StartAbility(const AbilityRequest &abilityRequest)
{
    HILOG_INFO("%{public}s,called", __func__);
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    return StartAbilityLocked(abilityRequest);
}

int AbilityConnectManager::TerminateAbility(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("%{public}s,called", __func__);
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    return TerminateAbilityLocked(token);
}

int AbilityConnectManager::TerminateAbility(const std::shared_ptr<AbilityRecord> &caller, int requestCode)
{
    HILOG_INFO("%{public}s called, %{public}d", __func__, __LINE__);
    std::lock_guard<std::recursive_mutex> guard(Lock_);

    std::shared_ptr<AbilityRecord> targetAbility = nullptr;
    std::for_each(serviceMap_.begin(),
        serviceMap_.end(),
        [&targetAbility, &caller, requestCode](ServiceMapType::reference service) {
            auto callerList = service.second->GetCallerRecordList();
            for (auto &it : callerList) {
                if (it->GetCaller() == caller && it->GetRequestCode() == requestCode) {
                    targetAbility = service.second;
                    break;
                }
            }
        });

    if (!targetAbility) {
        return ERR_INVALID_VALUE;
    }

    return TerminateAbilityLocked(targetAbility->GetToken());
}

int AbilityConnectManager::StopServiceAbility(const AbilityRequest &abilityRequest)
{
    HILOG_INFO("%{public}s,called", __func__);
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    return StopServiceAbilityLocked(abilityRequest);
}

int AbilityConnectManager::TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId)
{
    HILOG_INFO("%{public}s called, startId:%{public}d", __func__, startId);
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    return TerminateAbilityResultLocked(token, startId);
}

int AbilityConnectManager::StartAbilityLocked(const AbilityRequest &abilityRequest)
{
    HILOG_INFO("%{public}s, ability_name:%{public}s", __func__, abilityRequest.want.GetElement().GetURI().c_str());

    std::shared_ptr<AbilityRecord> targetService;
    bool isLoadedAbility = false;
    GetOrCreateServiceRecord(abilityRequest, false, targetService, isLoadedAbility);
    if (targetService == nullptr) {
        HILOG_ERROR("%{public}s,target service record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    targetService->AddCallerRecord(abilityRequest.callerToken, abilityRequest.requestCode);

    if (!isLoadedAbility) {
        LoadAbility(targetService);
    } else if (targetService->GetAbilityState() == AbilityState::ACTIVE) {
        // It may have been started through connect
        CommandAbility(targetService);
    } else {
        HILOG_ERROR("%{public}s,target service is already activing", __func__);
        return START_SERVICE_ABILITY_ACTIVING;
    }

    sptr<Token> token = targetService->GetToken();
    sptr<Token> preToken = nullptr;
    if (targetService->GetPreAbilityRecord()) {
        preToken = targetService->GetPreAbilityRecord()->GetToken();
    }
    DelayedSingleton<AppScheduler>::GetInstance()->AbilityBehaviorAnalysis(token, preToken, 0, 1, 1);
    return ERR_OK;
}

int AbilityConnectManager::TerminateAbilityLocked(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("%{public}s,called", __func__);
    auto abilityRecord = GetServiceRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("serviceAbility is null");
        return ERR_INVALID_VALUE;
    }

    if (abilityRecord->IsTerminating()) {
        HILOG_INFO("ability is on terminating");
        return ERR_OK;
    }

    if (abilityRecord->GetConnectRecordList().empty()) {
        HILOG_INFO("service ability has no any connection, and not started , need terminate.");
        auto timeoutTask = [abilityRecord, connectManager = shared_from_this()]() {
            HILOG_WARN("disconnect ability terminate timeout.");
            connectManager->HandleStopTimeoutTask(abilityRecord);
        };
        abilityRecord->Terminate(timeoutTask);
    } else {
        HILOG_WARN("%{public}s, target service has been connected. It cannot be stopped", __func__);
        return TERMINATE_SERVICE_IS_CONNECTED;
    }

    return ERR_OK;
}

int AbilityConnectManager::TerminateAbilityResultLocked(const sptr<IRemoteObject> &token, int startId)
{
    HILOG_INFO("%{public}s called, startId:%{public}d", __func__, startId);

    if (token == nullptr) {
        HILOG_ERROR("%{public}s failed, token is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    auto abilityRecord = Token::GetAbilityRecordByToken(token);
    if (!abilityRecord) {
        HILOG_ERROR("%{public}s, ability record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    if (abilityRecord->GetStartId() != startId) {
        HILOG_ERROR("%{public}s, Start id not equal", __func__);
        return TERMINATE_ABILITY_RESULT_FAILED;
    }

    return TerminateAbilityLocked(token);
}

int AbilityConnectManager::StopServiceAbilityLocked(const AbilityRequest &abilityRequest)
{
    HILOG_INFO("%{public}s,called", __func__);
    AppExecFwk::ElementName element(
        abilityRequest.abilityInfo.deviceId, abilityRequest.abilityInfo.bundleName, abilityRequest.abilityInfo.name);
    auto abilityRecord = GetServiceRecordByElementName(element.GetURI());
    if (!abilityRecord) {
        HILOG_ERROR("%{public}s,target service record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    if (abilityRecord->IsTerminating()) {
        HILOG_INFO("ability is on terminating");
        return ERR_OK;
    }

    if (abilityRecord->GetConnectRecordList().empty()) {
        HILOG_INFO("service ability has no any connection, and no started , need terminate.");
        auto timeoutTask = [abilityRecord, connectManager = shared_from_this()]() {
            HILOG_WARN("disconnect ability terminate timeout.");
            connectManager->HandleStopTimeoutTask(abilityRecord);
        };
        abilityRecord->Terminate(timeoutTask);
    } else {
        HILOG_WARN("%{public}s, target service has been connected. It cannot be stopped", __func__);
        return TERMINATE_SERVICE_IS_CONNECTED;
    }

    return ERR_OK;
}

void AbilityConnectManager::GetOrCreateServiceRecord(const AbilityRequest &abilityRequest,
    const bool isCreatedByConnect, std::shared_ptr<AbilityRecord> &targetService, bool &isLoadedAbility)
{
    AppExecFwk::ElementName element(
        abilityRequest.abilityInfo.deviceId, abilityRequest.abilityInfo.bundleName, abilityRequest.abilityInfo.name);
    auto serviceMapIter = serviceMap_.find(element.GetURI());
    if (serviceMapIter == serviceMap_.end()) {
        targetService = AbilityRecord::CreateAbilityRecord(abilityRequest);
        if (isCreatedByConnect && targetService != nullptr) {
            targetService->SetCreateByConnectMode();
        }
        serviceMap_.emplace(element.GetURI(), targetService);
        isLoadedAbility = false;
    } else {
        targetService = serviceMapIter->second;
        if (targetService != nullptr) {
            // want may be changed for the same ability.
            targetService->SetWant(abilityRequest.want);
        }
        isLoadedAbility = true;
    }
}

void AbilityConnectManager::GetConnectRecordListFromMap(
    const sptr<IAbilityConnection> &connect, std::list<std::shared_ptr<ConnectionRecord>> &connectRecordList)
{
    auto connectMapIter = connectMap_.find(connect->AsObject());
    if (connectMapIter != connectMap_.end()) {
        connectRecordList = connectMapIter->second;
    }
}

int AbilityConnectManager::ConnectAbilityLocked(const AbilityRequest &abilityRequest,
    const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s, ability_name:%{public}s", __func__, abilityRequest.want.GetElement().GetURI().c_str());
    std::lock_guard<std::recursive_mutex> guard(Lock_);

    // 1. get target service ability record, and check whether it has been loaded.
    std::shared_ptr<AbilityRecord> targetService;
    bool isLoadedAbility = false;
    GetOrCreateServiceRecord(abilityRequest, true, targetService, isLoadedAbility);
    if (targetService == nullptr) {
        HILOG_ERROR("%{public}s,target service record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    // 2. get target connectRecordList, and check whether this callback has been connected.
    ConnectListType connectRecordList;
    GetConnectRecordListFromMap(connect, connectRecordList);
    bool isCallbackConnected = !connectRecordList.empty();

    // 3. If this service ability and callback has been connected, There is no need to connect repeatedly
    if (isLoadedAbility && (isCallbackConnected) && IsAbilityConnected(targetService, connectRecordList)) {
        HILOG_ERROR("%{public}s, service and callback was connected", __func__);
        return ERR_OK;
    }

    // 4. Other cases , need to connect the service ability
    auto connectRecord = ConnectionRecord::CreateConnectionRecord(callerToken, targetService, connect);
    if (connectRecord == nullptr) {
        HILOG_ERROR("%{public}s failed to create connection record.", __func__);
        return ERR_INVALID_VALUE;
    }
    connectRecord->SetConnectState(ConnectionState::CONNECTING);
    targetService->AddConnectRecordToList(connectRecord);
    connectRecordList.push_back(connectRecord);
    if (isCallbackConnected) {
        RemoveConnectDeathRecipient(connect);
        connectMap_.erase(connectMap_.find(connect->AsObject()));
    }
    AddConnectDeathRecipient(connect);
    connectMap_.emplace(connect->AsObject(), connectRecordList);

    // 5. load or connect ability
    if (!isLoadedAbility) {
        LoadAbility(targetService);
    } else if (targetService->GetAbilityState() == AbilityState::ACTIVE) {
        // this service ability has not first connect
        if (targetService->GetConnectRecordList().size() > 1) {
            if (eventHandler_ != nullptr) {
                auto task = [connectRecord]() { connectRecord->CompleteConnect(ERR_OK); };
                eventHandler_->PostTask(task);
            }
        } else {
            ConnectAbility(targetService);
        }
    } else {
        HILOG_ERROR("%{public}s,target service is already activing", __func__);
    }

    auto token = targetService->GetToken();
    auto preToken = iface_cast<Token>(connectRecord->GetToken());
    DelayedSingleton<AppScheduler>::GetInstance()->AbilityBehaviorAnalysis(token, preToken, 0, 1, 1);
    return ERR_OK;
}

int AbilityConnectManager::DisconnectAbilityLocked(const sptr<IAbilityConnection> &connect)
{
    HILOG_INFO("disconnect ability with connect");
    std::lock_guard<std::recursive_mutex> guard(Lock_);

    // 1. check whether callback was connected.
    ConnectListType connectRecordList;
    GetConnectRecordListFromMap(connect, connectRecordList);
    if (connectRecordList.empty()) {
        HILOG_ERROR("can't find the connect list from connect map by callback.");
        return CONNECTION_NOT_EXIST;
    }

    // 2. schedule disconnect to target service
    for (auto &connectRecord : connectRecordList) {
        if (connectRecord) {
            int ret = connectRecord->DisconnectAbility();
            if (ret != ERR_OK) {
                HILOG_ERROR("disconnect ability fail , ret = %{public}d.", ret);
                return ret;
            }
            HILOG_INFO("disconnect ability ,connect record id %{public}d", connectRecord->GetRecordId());
        }
    }

    // 3. target servie has another connection, this record callback disconnected directly.
    if (eventHandler_ != nullptr) {
        auto task = [connectRecordList, connectManager = shared_from_this()]() {
            connectManager->HandleDisconnectTask(connectRecordList);
        };
        eventHandler_->PostTask(task);
    }

    return ERR_OK;
}

int AbilityConnectManager::AttachAbilityThreadLocked(
    const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token)
{
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    auto abilityRecord = GetServiceRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("abilityRecord is null");
        return ERR_INVALID_VALUE;
    }
    if (eventHandler_ != nullptr) {
        int recordId = abilityRecord->GetRecordId();
        std::string taskName = std::string("LoadTimeout_") + std::to_string(recordId);
        eventHandler_->RemoveTask(taskName);
        eventHandler_->RemoveEvent(AbilityManagerService::LOAD_TIMEOUT_MSG, abilityRecord->GetEventId());
    }
    std::string element = abilityRecord->GetWant().GetElement().GetURI();
    HILOG_INFO("%{public}s, ability: %{public}s", __func__, element.c_str());
    abilityRecord->SetScheduler(scheduler);

    DelayedSingleton<AppScheduler>::GetInstance()->MoveToForground(token);
    return ERR_OK;
}

void AbilityConnectManager::OnAbilityRequestDone(const sptr<IRemoteObject> &token, const int32_t state)
{
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    auto abilitState = DelayedSingleton<AppScheduler>::GetInstance()->ConvertToAppAbilityState(state);
    auto abilityRecord = GetServiceRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("abilityRecord is null");
        return;
    }
    std::string element = abilityRecord->GetWant().GetElement().GetURI();
    HILOG_INFO("%{public}s, ability: %{public}s", __func__, element.c_str());

    if (abilitState == AppAbilityState::ABILITY_STATE_FOREGROUND) {
        abilityRecord->Inactivate();
    } else if (abilitState == AppAbilityState::ABILITY_STATE_BACKGROUND) {
        DelayedSingleton<AppScheduler>::GetInstance()->TerminateAbility(token);
        RemoveServiceAbility(abilityRecord);
    }
}

int AbilityConnectManager::AbilityTransitionDone(const sptr<IRemoteObject> &token, int state)
{
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    auto abilityRecord = GetServiceRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("transition done abilityRecord is null");
        return ERR_INVALID_VALUE;
    }

    std::string element = abilityRecord->GetWant().GetElement().GetURI();
    int targetState = AbilityRecord::ConvertLifeCycleToAbilityState(static_cast<AbilityLifeCycleState>(state));
    std::string abilityState = AbilityRecord::ConvertAbilityState(static_cast<AbilityState>(targetState));
    HILOG_INFO("%{public}s, ability: %{public}s, state: %{public}s", __func__, element.c_str(), abilityState.c_str());

    switch (state) {
        case AbilityState::INACTIVE: {
            return DispatchInactive(abilityRecord, state);
        }
        case AbilityState::INITIAL: {
            return DispatchTerminate(abilityRecord);
        }
        default: {
            HILOG_WARN("don't support transiting state: %{public}d", state);
            return ERR_INVALID_VALUE;
        }
    }
}

int AbilityConnectManager::ScheduleConnectAbilityDoneLocked(
    const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject)
{
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    if (token == nullptr) {
        HILOG_ERROR("%{public}s, token or remoteObject is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    auto abilityRecord = Token::GetAbilityRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("%{public}s, ability record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    std::string element = abilityRecord->GetWant().GetElement().GetURI();
    HILOG_DEBUG("%{public}s, ability: %{public}s", __func__, element.c_str());

    if (abilityRecord->GetAbilityState() != AbilityState::INACTIVE &&
        abilityRecord->GetAbilityState() != AbilityState::ACTIVE) {
        HILOG_ERROR("%{public}s, ability record state is not inactive ,state：%{public}d",
            __func__,
            abilityRecord->GetAbilityState());
        return INVALID_CONNECTION_STATE;
    }

    abilityRecord->SetConnRemoteObject(remoteObject);
    // There may be multiple callers waiting for the connection result
    auto connectRecordList = abilityRecord->GetConnectRecordList();
    for (auto &connectRecord : connectRecordList) {
        connectRecord->ScheduleConnectAbilityDone();
    }

    return ERR_OK;
}

int AbilityConnectManager::ScheduleDisconnectAbilityDoneLocked(const sptr<IRemoteObject> &token)
{
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    std::shared_ptr<AbilityRecord> abilityRecord = GetServiceRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("can't find the service ability by token when scheduler disconnect.");
        return CONNECTION_NOT_EXIST;
    }

    std::shared_ptr<ConnectionRecord> connect = abilityRecord->GetDisconnectingRecord();
    if (connect == nullptr) {
        HILOG_ERROR("can't find any disconnecting record by token when scheduler disconnect.");
        return CONNECTION_NOT_EXIST;
    }

    if (abilityRecord->GetAbilityState() != AbilityState::ACTIVE) {
        HILOG_ERROR("%{public}s, the service ability state is not active ,state：%{public}d",
            __func__,
            abilityRecord->GetAbilityState());
        return INVALID_CONNECTION_STATE;
    }

    std::string element = abilityRecord->GetWant().GetElement().GetURI();
    HILOG_INFO("disconnect ability done with service %{public}s", element.c_str());

    // complete disconnect and remove record from conn map
    connect->ScheduleDisconnectAbilityDone();
    abilityRecord->RemoveConnectRecordFromList(connect);
    if (abilityRecord->IsConnectListEmpty() && abilityRecord->GetStartId() == 0) {
        HILOG_INFO("service ability has no any connection, and not started , need terminate.");
        auto timeoutTask = [abilityRecord, connectManager = shared_from_this()]() {
            HILOG_WARN("disconnect ability terminate timeout.");
            connectManager->HandleStopTimeoutTask(abilityRecord);
        };
        abilityRecord->Terminate(timeoutTask);
    }
    RemoveConnectionRecordFromMap(connect);

    return ERR_OK;
}

int AbilityConnectManager::ScheduleCommandAbilityDoneLocked(const sptr<IRemoteObject> &token)
{
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    if (token == nullptr) {
        HILOG_ERROR("%{public}s, token or remoteObject is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    auto abilityRecord = Token::GetAbilityRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("%{public}s, ability record is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::string element = abilityRecord->GetWant().GetElement().GetURI();
    HILOG_DEBUG("%{public}s, ability: %{public}s", __func__, element.c_str());

    if (abilityRecord->GetAbilityState() != AbilityState::INACTIVE &&
        abilityRecord->GetAbilityState() != AbilityState::ACTIVE) {
        HILOG_ERROR("%{public}s, ability record state is not inactive ,state：%{public}d",
            __func__,
            abilityRecord->GetAbilityState());
        return INVALID_CONNECTION_STATE;
    }
    // complete command and pop waiting start ability from queue.
    CompleteCommandAbility(abilityRecord);

    return ERR_OK;
}

void AbilityConnectManager::CompleteCommandAbility(std::shared_ptr<AbilityRecord> abilityRecord)
{
    if (!abilityRecord) {
        HILOG_ERROR("%{public}s, ability record is nullptr", __func__);
        return;
    }

    if (eventHandler_) {
        int recordId = abilityRecord->GetRecordId();
        std::string taskName = std::string("CommandTimeout_") + std::to_string(recordId) + std::string("_") +
                               std::to_string(abilityRecord->GetStartId());
        eventHandler_->RemoveTask(taskName);
    }

    abilityRecord->SetAbilityState(AbilityState::ACTIVE);
}

std::shared_ptr<AbilityRecord> AbilityConnectManager::GetServiceRecordByElementName(const std::string &element)
{
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    auto mapIter = serviceMap_.find(element);
    if (mapIter != serviceMap_.end()) {
        return mapIter->second;
    }
    return nullptr;
}

std::shared_ptr<AbilityRecord> AbilityConnectManager::GetServiceRecordByToken(const sptr<IRemoteObject> &token)
{
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    auto IsMatch = [token](auto service) {
        sptr<IRemoteObject> srcToken = service.second->GetToken();
        return srcToken == token;
    };
    auto serviceRecord = std::find_if(serviceMap_.begin(), serviceMap_.end(), IsMatch);
    if (serviceRecord != serviceMap_.end()) {
        return serviceRecord->second;
    }
    return nullptr;
}

std::list<std::shared_ptr<ConnectionRecord>> AbilityConnectManager::GetConnectRecordListByCallback(
    sptr<IAbilityConnection> callback)
{
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    std::list<std::shared_ptr<ConnectionRecord>> connectList;
    auto connectMapIter = connectMap_.find(callback->AsObject());
    if (connectMapIter != connectMap_.end()) {
        connectList = connectMapIter->second;
    }
    return connectList;
}

void AbilityConnectManager::RemoveAll()
{
    serviceMap_.clear();
    connectMap_.clear();
}

void AbilityConnectManager::LoadAbility(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    if (abilityRecord == nullptr) {
        HILOG_ERROR("abilityRecord is nullptr.");
        return;
    }

    PostTimeOutTask(abilityRecord, AbilityManagerService::LOAD_TIMEOUT_MSG);

    sptr<Token> token = abilityRecord->GetToken();
    sptr<Token> perToken = nullptr;
    if (abilityRecord->IsCreateByConnect()) {
        perToken = iface_cast<Token>(abilityRecord->GetConnectingRecord()->GetToken());
    } else {
        auto callerList = abilityRecord->GetCallerRecordList();
        if (!callerList.empty()) {
            perToken = callerList.back()->GetCaller()->GetToken();
        }
    }
    DelayedSingleton<AppScheduler>::GetInstance()->LoadAbility(
        token, perToken, abilityRecord->GetAbilityInfo(), abilityRecord->GetApplicationInfo());

    abilityRecord->SetStartTime();
}

void AbilityConnectManager::PostTimeOutTask(const std::shared_ptr<AbilityRecord> &abilityRecord, uint32_t messageId)
{
    if (abilityRecord == nullptr || eventHandler_ == nullptr) {
        HILOG_ERROR("abilityRecord or eventHandler_ is nullptr.");
        return;
    }
    if (messageId != AbilityConnectManager::LOAD_TIMEOUT_MSG &&
        messageId != AbilityConnectManager::CONNECT_TIMEOUT_MSG) {
        HILOG_ERROR("timeout task messageId is error.");
        return;
    }

    int recordId;
    std::string taskName;
    int resultCode;
    uint32_t delayTime;
    if (messageId == AbilityManagerService::LOAD_TIMEOUT_MSG) {
        // first load ability, There is at most one connect record.
        recordId = abilityRecord->GetRecordId();
        taskName = std::string("LoadTimeout_") + std::to_string(recordId);
        resultCode = LOAD_ABILITY_TIMEOUT;
        delayTime = AbilityManagerService::LOAD_TIMEOUT;
    } else {
        auto connectRecord = abilityRecord->GetConnectingRecord();
        if (connectRecord == nullptr) {
            HILOG_ERROR("timeout task connectRecord is nullptr.");
            return;
        }
        recordId = connectRecord->GetRecordId();
        taskName = std::string("ConnectTimeout_") + std::to_string(recordId);
        resultCode = CONNECTION_TIMEOUT;
        delayTime = AbilityManagerService::CONNECT_TIMEOUT;
    }

    auto timeoutTask = [abilityRecord, connectManager = shared_from_this(), resultCode]() {
        HILOG_WARN("connect or load ability timeout.");
        connectManager->HandleStartTimeoutTask(abilityRecord, resultCode);
    };

    eventHandler_->PostTask(timeoutTask, taskName, delayTime);
}

void AbilityConnectManager::HandleStartTimeoutTask(const std::shared_ptr<AbilityRecord> &abilityRecord, int resultCode)
{
    HILOG_DEBUG("complete connect or load ability timeout.");
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    if (!abilityRecord) {
        HILOG_ERROR("the target ability recorc is nullptr.");
        return;
    }
    auto connectingList = abilityRecord->GetConnectingRecordList();
    for (auto &connectRecord : connectingList) {
        if (connectRecord == nullptr) {
            HILOG_WARN("connectRecord is nullptr.");
            continue;
        }
        connectRecord->CompleteConnect(resultCode);
        abilityRecord->RemoveConnectRecordFromList(connectRecord);
        RemoveConnectionRecordFromMap(connectRecord);
    }
    if (resultCode == LOAD_ABILITY_TIMEOUT) {
        HILOG_DEBUG("load time out , remove target service record from services map.");
        RemoveServiceAbility(abilityRecord);
    }
}

void AbilityConnectManager::HandleStopTimeoutTask(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    HILOG_DEBUG("complete stop ability timeout start.");
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    if (!abilityRecord) {
        HILOG_ERROR("the target ability recorc is nullptr.");
        return;
    }
    TerminateDone(abilityRecord);
    HILOG_DEBUG("complete stop ability timeout end.");
}

void AbilityConnectManager::HandleDisconnectTask(const ConnectListType &connectlist)
{
    HILOG_DEBUG("complete disconnect ability.");
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    for (auto &connectRecord : connectlist) {
        if (!connectRecord) {
            continue;
        }
        auto targetService = connectRecord->GetAbilityRecord();
        if (targetService && connectRecord->GetConnectState() == ConnectionState::DISCONNECTED &&
            targetService->GetConnectRecordList().size() > 1) {
            HILOG_WARN("this record complete disconnect directly. recordId:%{public}d", connectRecord->GetRecordId());
            connectRecord->CompleteDisconnect(ERR_OK);
            targetService->RemoveConnectRecordFromList(connectRecord);
            RemoveConnectionRecordFromMap(connectRecord);
        };
    }
}

int AbilityConnectManager::DispatchInactive(const std::shared_ptr<AbilityRecord> &abilityRecord, int state)
{
    if (eventHandler_ == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
        return ERR_INVALID_VALUE;
    }
    if (abilityRecord->GetAbilityState() != AbilityState::INACTIVATING) {
        HILOG_ERROR("ability transition life state error. expect %{public}d, actual %{public}d callback %{public}d",
            AbilityState::INACTIVATING,
            abilityRecord->GetAbilityState(),
            state);
        return ERR_INVALID_VALUE;
    }
    eventHandler_->RemoveEvent(AbilityManagerService::INACTIVE_TIMEOUT_MSG, abilityRecord->GetEventId());

    // complete inactive
    abilityRecord->SetAbilityState(AbilityState::INACTIVE);
    if (abilityRecord->IsCreateByConnect()) {
        ConnectAbility(abilityRecord);
    } else {
        CommandAbility(abilityRecord);
    }

    return ERR_OK;
}

int AbilityConnectManager::DispatchTerminate(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    // remove terminate timeout task
    if (eventHandler_ != nullptr) {
        eventHandler_->RemoveTask(std::to_string(abilityRecord->GetEventId()));
    }
    // complete terminate
    TerminateDone(abilityRecord);
    return ERR_OK;
}

void AbilityConnectManager::ConnectAbility(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    if (abilityRecord == nullptr) {
        HILOG_ERROR("abilityRecord is nullptr.");
        return;
    }
    PostTimeOutTask(abilityRecord, AbilityConnectManager::CONNECT_TIMEOUT_MSG);
    abilityRecord->ConnectAbility();
}

void AbilityConnectManager::CommandAbility(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    if (eventHandler_ != nullptr) {
        // first connect ability, There is at most one connect record.
        int recordId = abilityRecord->GetRecordId();
        abilityRecord->AddStartId();
        std::string taskName = std::string("CommandTimeout_") + std::to_string(recordId) + std::string("_") +
                               std::to_string(abilityRecord->GetStartId());
        auto timeoutTask = [abilityRecord, connectManager = shared_from_this()]() {
            HILOG_ERROR("command ability timeout. %{public}s", abilityRecord->GetAbilityInfo().name.c_str());
        };
        eventHandler_->PostTask(timeoutTask, taskName, AbilityManagerService::COMMAND_TIMEOUT);
        // scheduling command ability
        abilityRecord->CommandAbility();
    }
}

void AbilityConnectManager::TerminateDone(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    if (abilityRecord->GetAbilityState() != AbilityState::TERMINATING) {
        std::string expect = AbilityRecord::ConvertAbilityState(AbilityState::TERMINATING);
        std::string actual = AbilityRecord::ConvertAbilityState(abilityRecord->GetAbilityState());
        HILOG_ERROR(
            "transition life state error. expect %{public}s, actual %{public}s", expect.c_str(), actual.c_str());
        return;
    }
    DelayedSingleton<AppScheduler>::GetInstance()->MoveToBackground(abilityRecord->GetToken());
}

bool AbilityConnectManager::IsAbilityConnected(const std::shared_ptr<AbilityRecord> &abilityRecord,
    const std::list<std::shared_ptr<ConnectionRecord>> &connectRecordList)
{
    auto isMatch = [abilityRecord](auto connectRecord) -> bool {
        if (abilityRecord == nullptr || connectRecord == nullptr) {
            return false;
        }
        if (abilityRecord != connectRecord->GetAbilityRecord()) {
            return false;
        }
        return true;
    };
    return std::any_of(connectRecordList.begin(), connectRecordList.end(), isMatch);
}

void AbilityConnectManager::RemoveConnectionRecordFromMap(const std::shared_ptr<ConnectionRecord> &connection)
{
    for (auto &connectCallback : connectMap_) {
        auto &connectList = connectCallback.second;
        auto connectRecord = std::find(connectList.begin(), connectList.end(), connection);
        if (connectRecord != connectList.end()) {
            HILOG_INFO(
                "%{public}s: remove connrecord(%{public}d) from maplist", __func__, (*connectRecord)->GetRecordId());
            connectList.remove(connection);
            if (connectList.empty()) {
                HILOG_INFO("%{public}s: remove connlist from map ", __func__);
                sptr<IAbilityConnection> connect = iface_cast<IAbilityConnection>(connectCallback.first);
                RemoveConnectDeathRecipient(connect);
                connectMap_.erase(connectCallback.first);
            }
            return;
        }
    }
}

void AbilityConnectManager::RemoveServiceAbility(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    if (!abilityRecord) {
        return;
    }
    const AppExecFwk::AbilityInfo &abilityInfo = abilityRecord->GetAbilityInfo();
    std::string element = abilityInfo.deviceId + "/" + abilityInfo.bundleName + "/" + abilityInfo.name;
    HILOG_INFO("%{public}s: remove service(%{public}s) from map ", __func__, element.c_str());
    auto it = serviceMap_.find(element);
    if (it != serviceMap_.end()) {
        HILOG_INFO("%{public}s: remove service(%{public}s) from map ", __func__, element.c_str());
        serviceMap_.erase(it);
    }
}

void AbilityConnectManager::AddConnectDeathRecipient(const sptr<IAbilityConnection> &connect)
{
    if (!connect || !connect->AsObject()) {
        HILOG_ERROR("%{public}s add death recipient,the object is nullptr.", __func__);
        return;
    }
    auto it = recipientMap_.find(connect->AsObject());
    if (it != recipientMap_.end()) {
        HILOG_ERROR("%{public}s this death recipient has been added.", __func__);
        return;
    } else {
        sptr<IRemoteObject::DeathRecipient> deathRecipient = new AbilityConnectCallbackRecipient(
            std::bind(&AbilityConnectManager::OnCallBackDied, this, std::placeholders::_1));
        connect->AsObject()->AddDeathRecipient(deathRecipient);
        recipientMap_.emplace(connect->AsObject(), deathRecipient);
    }
}

void AbilityConnectManager::RemoveConnectDeathRecipient(const sptr<IAbilityConnection> &connect)
{
    if (!connect || !connect->AsObject()) {
        HILOG_ERROR("%{public}s add death recipient,the object is nullptr.", __func__);
        return;
    }
    auto it = recipientMap_.find(connect->AsObject());
    if (it != recipientMap_.end()) {
        it->first->RemoveDeathRecipient(it->second);
        recipientMap_.erase(it);
        return;
    }
}

void AbilityConnectManager::OnCallBackDied(const wptr<IRemoteObject> &remote)
{
    auto object = remote.promote();
    if (!object) {
        HILOG_ERROR("Ability on scheduler died: null object.");
        return;
    }
    if (eventHandler_) {
        auto task = [object, connectManager = shared_from_this()]() { connectManager->HandleCallBackDiedTask(object); };
        eventHandler_->PostTask(task, TASK_ON_CALLBACK_DIED);
    }
}

void AbilityConnectManager::HandleCallBackDiedTask(const sptr<IRemoteObject> &connect)
{
    HILOG_INFO("%{public}s,called", __func__);
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    if (!connect) {
        HILOG_ERROR("%{public}s handle on call back died is nullptr.", __func__);
        return;
    }
    auto it = connectMap_.find(connect);
    if (it != connectMap_.end()) {
        ConnectListType connectRecordList = it->second;
        for (auto &connRecord : connectRecordList) {
            connRecord->ClearConnCallBack();
        }
    } else {
        HILOG_INFO("%{public}s died object can't find from conn map.", __func__);
        return;
    }
    sptr<IAbilityConnection> object = iface_cast<IAbilityConnection>(connect);
    DisconnectAbilityLocked(object);
}

void AbilityConnectManager::OnAbilityDied(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    HILOG_INFO("%{public}s,called", __func__);
    if (!abilityRecord) {
        HILOG_ERROR("%{public}s Ability on scheduler died: null object..", __func__);
        return;
    }
    if (abilityRecord->GetAbilityInfo().type != AbilityType::SERVICE) {
        HILOG_DEBUG("ability type is not service");
        return;
    }
    if (eventHandler_) {
        auto task = [abilityRecord, connectManager = shared_from_this()]() {
            connectManager->HandleAbilityDiedTask(abilityRecord);
        };
        eventHandler_->PostTask(task, TASK_ON_ABILITY_DIED);
    }
}

void AbilityConnectManager::HandleAbilityDiedTask(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    HILOG_INFO("%{public}s,called", __func__);
    std::lock_guard<std::recursive_mutex> guard(Lock_);
    if (!abilityRecord) {
        HILOG_ERROR("%{public}s Ability on scheduler died: null object..", __func__);
        return;
    }
    if (!GetServiceRecordByToken(abilityRecord->GetToken())) {
        HILOG_ERROR("%{public}s died ability record is not exist in service map", __func__);
        return;
    }
    ConnectListType connlist = abilityRecord->GetConnectRecordList();
    for (auto &connectRecord : connlist) {
        HILOG_WARN("this record complete disconnect directly. recordId:%{public}d", connectRecord->GetRecordId());
        connectRecord->CompleteDisconnect(ERR_OK);
        abilityRecord->RemoveConnectRecordFromList(connectRecord);
        RemoveConnectionRecordFromMap(connectRecord);
    }

    RemoveServiceAbility(abilityRecord);
}

void AbilityConnectManager::DumpState(std::vector<std::string> &info, const std::string &args) const
{
    if (!args.empty()) {
        auto it = std::find_if(serviceMap_.begin(), serviceMap_.end(), [&args](const auto &service) {
            return service.first.compare(args) == 0;
        });
        if (it != serviceMap_.end()) {
            info.emplace_back("uri [ " + it->first + " ]");
            it->second->DumpService(info);
        } else {
            info.emplace_back(args + ": Nothing to dump.");
        }
    } else {
        info.emplace_back("serviceAbilityRecords:");
        for (auto &&service : serviceMap_) {
            info.emplace_back("  uri [" + service.first + "]");
            service.second->DumpService(info);
        }
    }
}
}  // namespace AAFwk
}  // namespace OHOS
