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

#include "ability_record.h"

#include <vector>
#include <singleton.h>

#include "errors.h"
#include "hilog_wrapper.h"

#include "ability_event_handler.h"
#include "ability_manager_service.h"
#include "ability_scheduler_stub.h"

namespace OHOS {
namespace AAFwk {
int64_t AbilityRecord::abilityRecordId = 0;
int64_t AbilityRecord::g_abilityRecordEventId_ = 0;
const std::map<AbilityState, std::string> AbilityRecord::stateToStrMap = {
    std::map<AbilityState, std::string>::value_type(INITIAL, "INITIAL"),
    std::map<AbilityState, std::string>::value_type(INACTIVE, "INACTIVE"),
    std::map<AbilityState, std::string>::value_type(ACTIVE, "ACTIVE"),
    std::map<AbilityState, std::string>::value_type(BACKGROUND, "BACKGROUND"),
    std::map<AbilityState, std::string>::value_type(SUSPENDED, "SUSPENDED"),
    std::map<AbilityState, std::string>::value_type(INACTIVATING, "INACTIVATING"),
    std::map<AbilityState, std::string>::value_type(ACTIVATING, "ACTIVATING"),
    std::map<AbilityState, std::string>::value_type(MOVING_BACKGROUND, "MOVING_BACKGROUND"),
    std::map<AbilityState, std::string>::value_type(TERMINATING, "TERMINATING"),
};
const std::map<AbilityLifeCycleState, AbilityState> AbilityRecord::convertStateMap = {
    std::map<AbilityLifeCycleState, AbilityState>::value_type(ABILITY_STATE_INITIAL, INITIAL),
    std::map<AbilityLifeCycleState, AbilityState>::value_type(ABILITY_STATE_INACTIVE, INACTIVE),
    std::map<AbilityLifeCycleState, AbilityState>::value_type(ABILITY_STATE_ACTIVE, ACTIVE),
    std::map<AbilityLifeCycleState, AbilityState>::value_type(ABILITY_STATE_BACKGROUND, BACKGROUND),
    std::map<AbilityLifeCycleState, AbilityState>::value_type(ABILITY_STATE_SUSPENDED, SUSPENDED),
};

Token::Token(std::weak_ptr<AbilityRecord> abilityRecord) : abilityRecord_(abilityRecord)
{}

Token::~Token()
{}

std::shared_ptr<AbilityRecord> Token::GetAbilityRecordByToken(const sptr<IRemoteObject> &token)
{
    if (token == nullptr) {
        return nullptr;
    }
    return (static_cast<Token *>(token.GetRefPtr()))->GetAbilityRecord();
}

std::shared_ptr<AbilityRecord> Token::GetAbilityRecord() const
{
    return abilityRecord_.lock();
}

AbilityRecord::AbilityRecord(const Want &want, const AppExecFwk::AbilityInfo &abilityInfo,
    const AppExecFwk::ApplicationInfo &applicationInfo, int requestCode)
    : want_(want), abilityInfo_(abilityInfo), applicationInfo_(applicationInfo), requestCode_(requestCode)
{
    recordId_ = abilityRecordId++;
    currentState_ = AbilityState::INITIAL;
}

AbilityRecord::~AbilityRecord()
{
    if (scheduler_ != nullptr && schedulerDeathRecipient_ != nullptr) {
        auto object = scheduler_->AsObject();
        if (object != nullptr) {
            object->RemoveDeathRecipient(schedulerDeathRecipient_);
        }
    }
}

std::shared_ptr<AbilityRecord> AbilityRecord::CreateAbilityRecord(const AbilityRequest &abilityRequest)
{
    std::shared_ptr<AbilityRecord> abilityRecord = std::make_shared<AbilityRecord>(
        abilityRequest.want, abilityRequest.abilityInfo, abilityRequest.appInfo, abilityRequest.requestCode);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("failed to create new ability record");
        return nullptr;
    }
    if (!abilityRecord->Init()) {
        HILOG_ERROR("failed to init new ability record");
        return nullptr;
    }
    return abilityRecord;
}

bool AbilityRecord::Init()
{
    lifecycleDeal_ = std::make_unique<LifecycleDeal>();
    if (lifecycleDeal_ == nullptr) {
        HILOG_ERROR("failed to create new lifecycle deal");
        return false;
    }
    token_ = new (std::nothrow) Token(weak_from_this());
    if (token_ == nullptr) {
        HILOG_ERROR("failed to create new token");
        return false;
    }

    if (applicationInfo_.isLauncherApp) {
        isLauncherAbility_ = true;
    }
    return true;
}

int AbilityRecord::LoadAbility()
{
    HILOG_INFO("%s", __func__);
    startTime_ = SystemTimeMillis();
    if (token_ == nullptr) {
        HILOG_ERROR("token is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::string appName = applicationInfo_.name;
    if (appName.empty()) {
        HILOG_ERROR("app name is empty");
        return ERR_INVALID_VALUE;
    }

    if (abilityInfo_.type != AppExecFwk::AbilityType::DATA) {
        if (isKernalSystemAbility) {
            SendEvent(AbilityManagerService::LOAD_TIMEOUT_MSG, AbilityManagerService::SYSTEM_UI_TIMEOUT);
        } else {
            SendEvent(AbilityManagerService::LOAD_TIMEOUT_MSG, AbilityManagerService::LOAD_TIMEOUT);
        }
    }
    sptr<Token> callerToken_ = nullptr;
    if (!callerList_.empty()) {
        callerToken_ = callerList_.back()->GetCaller()->GetToken();
    }
    return DelayedSingleton<AppScheduler>::GetInstance()->LoadAbility(
        token_, callerToken_, abilityInfo_, applicationInfo_);
}

int AbilityRecord::TerminateAbility()
{
    HILOG_INFO("%s", __func__);
    return DelayedSingleton<AppScheduler>::GetInstance()->TerminateAbility(token_);
}

void AbilityRecord::SetMissionRecord(const std::shared_ptr<MissionRecord> &missionRecord)
{
    missionRecord_ = missionRecord;
    if (missionRecord) {
        lifeCycleStateInfo_.missionId = missionRecord->GetMissionRecordId();
    }
}

std::shared_ptr<MissionRecord> AbilityRecord::GetMissionRecord() const
{
    return missionRecord_.lock();
}

const AppExecFwk::AbilityInfo &AbilityRecord::GetAbilityInfo() const
{
    return abilityInfo_;
}

const AppExecFwk::ApplicationInfo &AbilityRecord::GetApplicationInfo() const
{
    return applicationInfo_;
}

AbilityState AbilityRecord::GetAbilityState() const
{
    return currentState_;
}

void AbilityRecord::SetAbilityState(AbilityState state)
{
    currentState_ = state;
}

void AbilityRecord::SetScheduler(const sptr<IAbilityScheduler> &scheduler)
{
    HILOG_INFO("%s", __func__);
    if (lifecycleDeal_ == nullptr) {
        HILOG_ERROR("lifecycleDeal_ is nullptr");
        return;
    }
    if (scheduler != nullptr) {
        if (scheduler_ != nullptr && schedulerDeathRecipient_ != nullptr) {
            auto schedulerObject = scheduler_->AsObject();
            if (schedulerObject != nullptr) {
                schedulerObject->RemoveDeathRecipient(schedulerDeathRecipient_);
            }
        }
        if (schedulerDeathRecipient_ == nullptr) {
            schedulerDeathRecipient_ =
                new AbilitySchedulerRecipient(std::bind(&AbilityRecord::OnSchedulerDied, this, std::placeholders::_1));
        }
        isReady_ = true;
        scheduler_ = scheduler;
        lifecycleDeal_->SetScheduler(scheduler);
        auto schedulerObject = scheduler_->AsObject();
        if (schedulerObject != nullptr) {
            schedulerObject->AddDeathRecipient(schedulerDeathRecipient_);
        }
    } else {
        HILOG_ERROR("scheduler is nullptr");
        isReady_ = false;
    }
}

sptr<Token> AbilityRecord::GetToken() const
{
    return token_;
}

void AbilityRecord::SetPreAbilityRecord(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    preAbilityRecord_ = abilityRecord;
}

std::shared_ptr<AbilityRecord> AbilityRecord::GetPreAbilityRecord() const
{
    return preAbilityRecord_.lock();
}

void AbilityRecord::SetNextAbilityRecord(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    nextAbilityRecord_ = abilityRecord;
}

std::shared_ptr<AbilityRecord> AbilityRecord::GetNextAbilityRecord() const
{
    return nextAbilityRecord_.lock();
}

void AbilityRecord::SetBackAbilityRecord(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    backAbilityRecord_ = abilityRecord;
}

std::shared_ptr<AbilityRecord> AbilityRecord::GetBackAbilityRecord() const
{
    return backAbilityRecord_.lock();
}

void AbilityRecord::SetEventId(int64_t eventId)
{
    eventId_ = eventId;
}

int64_t AbilityRecord::GetEventId() const
{
    return eventId_;
}

bool AbilityRecord::IsReady() const
{
    return isReady_;
}

bool AbilityRecord::IsWindowAttached() const
{
    return isWindowAttached_;
}

bool AbilityRecord::IsLauncherAbility() const
{
    return isLauncherAbility_;
}

bool AbilityRecord::IsTerminating() const
{
    return isTerminating_;
}

bool AbilityRecord::IsForceTerminate() const
{
    return isForceTerminate_;
}
void AbilityRecord::SetForceTerminate(bool flag)
{
    isForceTerminate_ = flag;
}

void AbilityRecord::SetTerminatingState()
{
    isTerminating_ = true;
}

bool AbilityRecord::IsNewWant() const
{
    return lifeCycleStateInfo_.isNewWant;
}

void AbilityRecord::SetIsNewWant(bool isNewWant)
{
    lifeCycleStateInfo_.isNewWant = isNewWant;
}

bool AbilityRecord::IsCreateByConnect() const
{
    return isCreateByConnect;
}

void AbilityRecord::SetCreateByConnectMode()
{
    isCreateByConnect = true;
}

void AbilityRecord::Activate()
{
    HILOG_INFO("%{public}s", __func__);
    if (lifecycleDeal_ == nullptr) {
        HILOG_ERROR("lifecycleDeal_ is nullptr");
        return;
    }

    SendEvent(AbilityManagerService::ACTIVE_TIMEOUT_MSG, AbilityManagerService::ACTIVE_TIMEOUT);

    // schedule active after updating AbilityState and sending timeout message to avoid ability async callback
    // earlier than above actions.
    currentState_ = AbilityState::ACTIVATING;
    lifecycleDeal_->Activate(want_, lifeCycleStateInfo_);

    // update ability state to appMgr service when restart
    if (IsNewWant()) {
        sptr<Token> preToken = nullptr;
        if (GetPreAbilityRecord()) {
            preToken = GetPreAbilityRecord()->GetToken();
        }
        DelayedSingleton<AppScheduler>::GetInstance()->AbilityBehaviorAnalysis(token_, preToken, 1, 1, 1);
    }
}

void AbilityRecord::ProcessActivate()
{
    std::string element = GetWant().GetElement().GetURI();
    HILOG_DEBUG("ability record: %{public}s", element.c_str());

    if (isReady_) {
        if (currentState_ == AbilityState::BACKGROUND) {
            HILOG_DEBUG("MoveToForground, %{public}s", element.c_str());
            DelayedSingleton<AppScheduler>::GetInstance()->MoveToForground(token_);
        } else {
            HILOG_DEBUG("Activate %{public}s", element.c_str());
            Activate();
        }
    } else {
        LoadAbility();
    }
}

void AbilityRecord::Inactivate()
{
    HILOG_INFO("%{public}s", __func__);
    if (lifecycleDeal_ == nullptr) {
        HILOG_ERROR("lifecycleDeal_ is nullptr");
        return;
    }

    SendEvent(AbilityManagerService::INACTIVE_TIMEOUT_MSG, AbilityManagerService::INACTIVE_TIMEOUT);

    // schedule inactive after updating AbilityState and sending timeout message to avoid ability async callback
    // earlier than above actions.
    currentState_ = AbilityState::INACTIVATING;
    lifecycleDeal_->Inactivate(want_, lifeCycleStateInfo_);
}

void AbilityRecord::MoveToBackground(const Closure &task)
{
    HILOG_INFO("%{public}s", __func__);
    if (lifecycleDeal_ == nullptr) {
        HILOG_ERROR("lifecycleDeal_ is nullptr");
        return;
    }
    std::shared_ptr<AbilityEventHandler> handler =
        DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr || task == nullptr) {
        // handler is nullptr means couldn't send timeout message. But still need to notify ability to inactive.
        // so don't return here.
        HILOG_ERROR("handler is nullptr or task is nullptr.");
    } else {
        g_abilityRecordEventId_++;
        eventId_ = g_abilityRecordEventId_;
        // eventId_ is a unique id of the task.
        handler->PostTask(task, std::to_string(eventId_), AbilityManagerService::BACKGROUND_TIMEOUT);
    }
    // schedule background after updating AbilityState and sending timeout message to avoid ability async callback
    // earlier than above actions.
    currentState_ = AbilityState::MOVING_BACKGROUND;
    lifecycleDeal_->MoveToBackground(want_, lifeCycleStateInfo_);
}

void AbilityRecord::Terminate(const Closure &task)
{
    HILOG_INFO("terminate ability : %{public}s", __func__);
    if (lifecycleDeal_ == nullptr) {
        HILOG_ERROR("lifecycleDeal_ is nullptr");
        return;
    }
    std::shared_ptr<AbilityEventHandler> handler =
        DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr || task == nullptr) {
        // handler is nullptr means couldn't send timeout message. But still need to notify ability to inactive.
        // so don't return here.
        HILOG_ERROR("handler is nullptr or task is nullptr.");
    } else {
        g_abilityRecordEventId_++;
        eventId_ = g_abilityRecordEventId_;
        // eventId_ is a unique id of the task.
        handler->PostTask(task, std::to_string(eventId_), AbilityManagerService::TERMINATE_TIMEOUT);
    }
    // schedule background after updating AbilityState and sending timeout message to avoid ability async callback
    // earlier than above actions.
    currentState_ = AbilityState::TERMINATING;
    lifecycleDeal_->Terminate(want_, lifeCycleStateInfo_);
}

void AbilityRecord::ConnectAbility()
{
    HILOG_INFO("%{public}s", __func__);
    if (lifecycleDeal_ == nullptr) {
        HILOG_ERROR("lifecycleDeal_ is nullptr");
        return;
    }
    lifecycleDeal_->ConnectAbility(want_);
}

void AbilityRecord::DisconnectAbility()
{
    HILOG_INFO("%{public}s", __func__);
    if (lifecycleDeal_ == nullptr) {
        HILOG_ERROR("lifecycleDeal_ is nullptr");
        return;
    }
    lifecycleDeal_->DisconnectAbility(want_);
}

void AbilityRecord::CommandAbility()
{
    HILOG_INFO("%{public}s", __func__);
    if (lifecycleDeal_ == nullptr) {
        HILOG_ERROR("lifecycleDeal_ is nullptr");
        return;
    }
    lifecycleDeal_->CommandAbility(want_, false, startId_);
}

void AbilityRecord::SetWant(const Want &want)
{
    want_ = want;
}

const Want &AbilityRecord::GetWant() const
{
    return want_;
}

int AbilityRecord::GetRequestCode() const
{
    return requestCode_;
}

void AbilityRecord::SetResult(const std::shared_ptr<AbilityResult> &result)
{
    result_ = result;
}

std::shared_ptr<AbilityResult> AbilityRecord::GetResult() const
{
    return result_;
}

void AbilityRecord::SendResult()
{
    HILOG_INFO("%{public}s", __func__);
    if (scheduler_ == nullptr) {
        HILOG_ERROR("scheduler_ is nullptr");
        return;
    }
    if (result_ == nullptr) {
        HILOG_ERROR("result_ is nullptr");
        return;
    }
    scheduler_->SendResult(result_->requestCode_, result_->resultCode_, result_->resultWant_);
    // reset result to avoid send result next time
    result_.reset();
}

void AbilityRecord::SendResultToCallers()
{
    for (auto caller : GetCallerRecordList()) {
        std::shared_ptr<AbilityRecord> callerAbilityRecord = caller->GetCaller();
        if (callerAbilityRecord != nullptr && callerAbilityRecord->GetResult() != nullptr) {
            callerAbilityRecord->SendResult();
        }
    }
}

void AbilityRecord::SaveResultToCallers(const int resultCode, const Want *resultWant)
{
    for (auto caller : GetCallerRecordList()) {
        std::shared_ptr<AbilityRecord> callerAbilityRecord = caller->GetCaller();
        if (callerAbilityRecord != nullptr) {
            callerAbilityRecord->SetResult(
                std::make_shared<AbilityResult>(caller->GetRequestCode(), resultCode, *resultWant));
        }
    }
}

void AbilityRecord::AddConnectRecordToList(const std::shared_ptr<ConnectionRecord> &connRecord)
{
    if (connRecord == nullptr) {
        HILOG_ERROR("%{public}s: connRecord is null, can't be added to list", __func__);
        return;
    }
    auto it = std::find(connRecordList_.begin(), connRecordList_.end(), connRecord);
    // found it
    if (it != connRecordList_.end()) {
        HILOG_DEBUG(
            "%{public}s: found it in list, so no need to add same connection(%{public}p)", __func__, connRecord.get());
        return;
    }
    // no found then add new connection to list
    HILOG_DEBUG("%{public}s: no found in list, so add new connection(%{public}p) to list", __func__, connRecord.get());
    connRecordList_.push_back(connRecord);
}

std::list<std::shared_ptr<ConnectionRecord>> AbilityRecord::GetConnectRecordList() const
{
    return connRecordList_;
}

void AbilityRecord::RemoveConnectRecordFromList(const std::shared_ptr<ConnectionRecord> &connRecord)
{
    if (connRecord == nullptr) {
        HILOG_ERROR("%{public}s: connRecord is null, can't be removed from list", __func__);
        return;
    }
    connRecordList_.remove(connRecord);
    HILOG_INFO("%{public}s: remove member(%{public}p) from list", __func__, connRecord.get());
}

void AbilityRecord::AddCallerRecord(const sptr<IRemoteObject> &callerToken, int requestCode)
{
    HILOG_INFO("%{public}s", __func__);
    auto abilityRecord = Token::GetAbilityRecordByToken(callerToken);
    if (!abilityRecord) {
        HILOG_ERROR("caller ability record is nullptr");
        return;
    }

    auto isExist = [&abilityRecord](const std::shared_ptr<CallerRecord> &callerRecord) {
        return (callerRecord->GetCaller() == abilityRecord);
    };

    auto record = std::find_if(callerList_.begin(), callerList_.end(), isExist);
    if (record != callerList_.end()) {
        callerList_.erase(record);
    }

    callerList_.emplace_back(std::make_shared<CallerRecord>(requestCode, abilityRecord));

    lifeCycleStateInfo_.caller.requestCode = requestCode;
    lifeCycleStateInfo_.caller.deviceId = abilityRecord->GetAbilityInfo().deviceId;
    lifeCycleStateInfo_.caller.bundleName = abilityRecord->GetAbilityInfo().bundleName;
    lifeCycleStateInfo_.caller.abilityName = abilityRecord->GetAbilityInfo().name;
    HILOG_INFO("%{public}s, caller %{public}s, %{public}s, %{public}s",
        __func__,
        abilityRecord->GetAbilityInfo().deviceId.c_str(),
        abilityRecord->GetAbilityInfo().bundleName.c_str(),
        abilityRecord->GetAbilityInfo().name.c_str());
}

std::list<std::shared_ptr<CallerRecord>> AbilityRecord::GetCallerRecordList() const
{
    return callerList_;
}

void AbilityRecord::AddWindowInfo(int windowToken)
{
    windowInfo_ = std::make_shared<WindowInfo>(windowToken);
}

void AbilityRecord::RemoveWindowInfo()
{
    windowInfo_.reset();
}

bool AbilityRecord::IsConnectListEmpty()
{
    return connRecordList_.empty();
}

std::shared_ptr<WindowInfo> AbilityRecord::GetWindowInfo() const
{
    return windowInfo_;
}

std::shared_ptr<ConnectionRecord> AbilityRecord::GetConnectingRecord() const
{
    auto connect =
        std::find_if(connRecordList_.begin(), connRecordList_.end(), [](std::shared_ptr<ConnectionRecord> record) {
            return record->GetConnectState() == ConnectionState::CONNECTING;
        });
    return (connect != connRecordList_.end()) ? *connect : nullptr;
}

std::list<std::shared_ptr<ConnectionRecord>> AbilityRecord::GetConnectingRecordList()
{
    std::list<std::shared_ptr<ConnectionRecord>> connectingList;
    for (auto record : connRecordList_) {
        if (record && record->GetConnectState() == ConnectionState::CONNECTING) {
            connectingList.push_back(record);
        }
    }
    return connectingList;
}

std::shared_ptr<ConnectionRecord> AbilityRecord::GetDisconnectingRecord() const
{
    auto connect =
        std::find_if(connRecordList_.begin(), connRecordList_.end(), [](std::shared_ptr<ConnectionRecord> record) {
            return record->GetConnectState() == ConnectionState::DISCONNECTING;
        });
    return (connect != connRecordList_.end()) ? *connect : nullptr;
}

int64_t AbilityRecord::SystemTimeMillis()
{
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (int64_t)((t.tv_sec) * NANOSECONDS + t.tv_nsec) / MICROSECONDS;
}

void AbilityRecord::GetAbilityTypeString(std::string &typeStr)
{
    AppExecFwk::AbilityType type = GetAbilityInfo().type;
    switch (type) {
        case AppExecFwk::AbilityType::PAGE: {
            typeStr = "PAGE";
            break;
        }
        case AppExecFwk::AbilityType::SERVICE: {
            typeStr = "SERVICE";
            break;
        }
        // for config.json type
        case AppExecFwk::AbilityType::DATA: {
            typeStr = "DATA";
            break;
        }
        default: {
            typeStr = "UNKNOWN";
            break;
        }
    }
}

std::string AbilityRecord::ConvertAbilityState(const AbilityState &state)
{
    auto it = stateToStrMap.find(state);
    if (it != stateToStrMap.end()) {
        return it->second;
    }
    return "INVALIDSTATE";
}

int AbilityRecord::ConvertLifeCycleToAbilityState(const AbilityLifeCycleState &state)
{
    auto it = convertStateMap.find(state);
    if (it != convertStateMap.end()) {
        return it->second;
    }
    return -1;
}

void AbilityRecord::Dump(std::vector<std::string> &info)
{
    std::string dumpInfo = "      AbilityRecord ID #" + std::to_string(recordId_);
    info.push_back(dumpInfo);
    dumpInfo = "        app name [" + GetAbilityInfo().applicationName + "]";
    info.push_back(dumpInfo);
    dumpInfo = "        main name [" + GetAbilityInfo().name + "]";
    info.push_back(dumpInfo);
    dumpInfo = "        bundle name [" + GetAbilityInfo().bundleName + "]";
    info.push_back(dumpInfo);
    // get ability type(unknown/page/service/provider)
    std::string typeStr;
    GetAbilityTypeString(typeStr);
    dumpInfo = "        ability type [" + typeStr + "]";
    info.push_back(dumpInfo);
    std::shared_ptr<AbilityRecord> preAbility = GetPreAbilityRecord();
    if (preAbility == nullptr) {
        dumpInfo = "        previous ability app name [NULL]" + LINE_SEPARATOR;
        dumpInfo += "        previous ability file name [NULL]";
    } else {
        dumpInfo =
            "        previous ability app name [" + preAbility->GetAbilityInfo().applicationName + "]" + LINE_SEPARATOR;
        dumpInfo += "        previous ability file name [" + preAbility->GetAbilityInfo().name + "]";
    }
    info.push_back(dumpInfo);
    std::shared_ptr<AbilityRecord> nextAbility = GetNextAbilityRecord();
    if (nextAbility == nullptr) {
        dumpInfo = "        next ability app name [NULL]" + LINE_SEPARATOR;
        dumpInfo += "        next ability file name [NULL]";
    } else {
        dumpInfo =
            "        next ability app name [" + nextAbility->GetAbilityInfo().applicationName + "]" + LINE_SEPARATOR;
        dumpInfo += "        next ability main name [" + nextAbility->GetAbilityInfo().name + "]";
    }
    info.push_back(dumpInfo);
    dumpInfo = "        state #" + AbilityRecord::ConvertAbilityState(GetAbilityState()) + "  start time [" +
               std::to_string(startTime_) + "]";
    info.push_back(dumpInfo);
    dumpInfo = "        ready #" + std::to_string(isReady_) + "  window attached #" +
               std::to_string(isWindowAttached_) + "  launcher #" + std::to_string(isLauncherAbility_);
    info.push_back(dumpInfo);
}

void AbilityRecord::SetStartTime()
{
    if (startTime_ == 0) {
        startTime_ = SystemTimeMillis();
    }
}

int64_t AbilityRecord::GetStartTime() const
{
    return startTime_;
}

void AbilityRecord::DumpService(std::vector<std::string> &info) const
{
    info.emplace_back("    AbilityRecord ID #" + std::to_string(GetRecordId()) + "   state #" +
                      AbilityRecord::ConvertAbilityState(GetAbilityState()) + "   start time [" +
                      std::to_string(GetStartTime()) + "]");
    info.emplace_back("    main name [" + GetAbilityInfo().name + "]");
    info.emplace_back("    bundle name [" + GetAbilityInfo().bundleName + "]");
    info.emplace_back("    ability type [SERVICE]");
    info.emplace_back("    Connections: " + std::to_string(connRecordList_.size()));

    for (auto &&conn : connRecordList_) {
        if (conn) {
            conn->Dump(info);
        }
    }
}

void AbilityRecord::GetAbilityRecordInfo(AbilityRecordInfo &recordInfo)
{
    recordInfo.elementName = want_.GetElement().GetURI();
    recordInfo.id = recordId_;
    recordInfo.appName = abilityInfo_.applicationName;
    recordInfo.mainName = abilityInfo_.name;
    recordInfo.abilityType = static_cast<int32_t>(abilityInfo_.type);

    std::shared_ptr<AbilityRecord> preAbility = GetPreAbilityRecord();
    if (preAbility) {
        recordInfo.previousAppName = preAbility->GetAbilityInfo().applicationName;
        recordInfo.previousMainName = preAbility->GetAbilityInfo().name;
    }

    std::shared_ptr<AbilityRecord> nextAbility = GetNextAbilityRecord();
    if (nextAbility) {
        recordInfo.nextAppName = nextAbility->GetAbilityInfo().applicationName;
        recordInfo.nextMainName = nextAbility->GetAbilityInfo().name;
    }

    recordInfo.state = static_cast<AbilityState>(currentState_);
    recordInfo.startTime = std::to_string(startTime_);
    recordInfo.ready = isReady_;
    recordInfo.windowAttached = isWindowAttached_;
    recordInfo.lanucher = isLauncherAbility_;
}

void AbilityRecord::OnSchedulerDied(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    if (scheduler_ == nullptr) {
        HILOG_ERROR("BUG: remote death notifies to a unready ability.");
        return;
    }

    auto object = remote.promote();
    if (!object) {
        HILOG_ERROR("Ability on scheduler died: null object.");
        return;
    }

    if (object != scheduler_->AsObject()) {
        HILOG_ERROR("Ability on scheduler died: scheduler is not matches with remote.");
        return;
    }

    isReady_ = false;
    if (scheduler_ != nullptr && schedulerDeathRecipient_ != nullptr) {
        auto schedulerObject = scheduler_->AsObject();
        if (schedulerObject != nullptr) {
            schedulerObject->RemoveDeathRecipient(schedulerDeathRecipient_);
        }
    }
    scheduler_.clear();
    if (lifecycleDeal_ == nullptr) {
        HILOG_ERROR("lifecycleDeal_ is nullptr");
        return;
    }
    lifecycleDeal_->SetScheduler(nullptr);

    auto abilityManagerService = DelayedSingleton<AbilityManagerService>::GetInstance();
    if (!abilityManagerService) {
        HILOG_ERROR("Ability on scheduler died: failed to get ams.");
        return;
    }

    auto handler = abilityManagerService->GetEventHandler();
    if (!handler) {
        HILOG_ERROR("Ability on scheduler died: failed to get ams handler.");
        return;
    }

    HILOG_INFO("Ability on scheduler died: '%{public}s'", abilityInfo_.name.c_str());
    auto task = [abilityManagerService, ability = shared_from_this()]() {
        abilityManagerService->OnAbilityDied(ability);
    };
    handler->PostTask(task);
}

void AbilityRecord::SetConnRemoteObject(const sptr<IRemoteObject> &remoteObject)
{
    connRemoteObject_ = remoteObject;
}

sptr<IRemoteObject> AbilityRecord::GetConnRemoteObject() const
{
    return connRemoteObject_;
}

void AbilityRecord::AddStartId()
{
    startId_++;
}
int AbilityRecord::GetStartId() const
{
    return startId_;
}

void AbilityRecord::SetIsUninstallAbility()
{
    isUninstall = true;
}

bool AbilityRecord::IsUninstallAbility() const
{
    return isUninstall;
}

void AbilityRecord::SetKernalSystemAbility()
{
    isKernalSystemAbility = true;
}

bool AbilityRecord::IsKernalSystemAbility() const
{
    return isKernalSystemAbility;
}

void AbilityRecord::SetLauncherRoot()
{
    isLauncherRoot_ = true;
}
bool AbilityRecord::IsLauncherRoot() const
{
    return isLauncherRoot_;
}

void AbilityRecord::SendEvent(uint32_t msg, uint32_t timeOut)
{
    auto handler = DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (!handler) {
        HILOG_ERROR("fail to get AbilityEventHandler");
        return;
    }

    g_abilityRecordEventId_++;
    eventId_ = g_abilityRecordEventId_;
    handler->SendEvent(msg, eventId_, timeOut);
}
}  // namespace AAFwk
}  // namespace OHOS
