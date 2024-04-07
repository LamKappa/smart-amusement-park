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

#include "connection_record.h"

#include "ability_manager_errors.h"
#include "ability_manager_service.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
int64_t ConnectionRecord::connectRecordId = 0;

ConnectionRecord::ConnectionRecord(const sptr<IRemoteObject> &callerToken,
    const std::shared_ptr<AbilityRecord> &targetService, const sptr<IAbilityConnection> &connCallback)
    : state_(ConnectionState::INIT),
      callerToken_(callerToken),
      targetService_(targetService),
      connCallback_(connCallback)
{
    recordId_ = connectRecordId++;
}

ConnectionRecord::~ConnectionRecord()
{}

std::shared_ptr<ConnectionRecord> ConnectionRecord::CreateConnectionRecord(const sptr<IRemoteObject> &callerToken,
    const std::shared_ptr<AbilityRecord> &targetService, const sptr<IAbilityConnection> &connCallback)
{
    std::shared_ptr<ConnectionRecord> connRecord =
        std::make_shared<ConnectionRecord>(callerToken, targetService, connCallback);
    if (connRecord == nullptr) {
        HILOG_ERROR("%{public}s failed to create connection record.", __func__);
        return nullptr;
    }
    connRecord->SetConnectState(ConnectionState::INIT);
    return connRecord;
}

void ConnectionRecord::SetConnectState(const ConnectionState &state)
{
    state_ = state;
}

ConnectionState ConnectionRecord::GetConnectState() const
{
    return state_;
}

sptr<IRemoteObject> ConnectionRecord::GetToken() const
{
    return callerToken_;
}

std::shared_ptr<AbilityRecord> ConnectionRecord::GetAbilityRecord() const
{
    return targetService_;
}

sptr<IAbilityConnection> ConnectionRecord::GetAbilityConnectCallback() const
{
    return connCallback_;
}

void ConnectionRecord::ClearConnCallBack()
{
    if (connCallback_) {
        connCallback_.clear();
    }
}

int ConnectionRecord::DisconnectAbility()
{
    if (state_ != ConnectionState::CONNECTED) {
        HILOG_INFO("The connection has not established.");
        return INVALID_CONNECTION_STATE;
    }

    /* set state to Disconnecting */
    SetConnectState(ConnectionState::DISCONNECTING);
    if (targetService_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    int connectNums = targetService_->GetConnectRecordList().size();
    if (connectNums == 1) {
        /* post timeout task to eventhandler */
        std::shared_ptr<AbilityEventHandler> handler =
            DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
        if (handler == nullptr) {
            HILOG_ERROR("fail to get AbilityEventHandler");
        } else {
            std::string taskName("DisconnectTimeout_");
            taskName += std::to_string(recordId_);
            auto disconnectTask = [connectionRecord = shared_from_this()]() {
                HILOG_ERROR("Disconnect ability timeout");
                connectionRecord->DisconnectTimeout();
            };
            handler->PostTask(disconnectTask, taskName, AbilityManagerService::DISCONNECT_TIMEOUT);
        }
        /* schedule disconnect to target ability */
        targetService_->DisconnectAbility();
    } else {
        SetConnectState(ConnectionState::DISCONNECTED);
    }

    return ERR_OK;
}

void ConnectionRecord::CompleteConnect(int resultCode)
{
    if (targetService_ == nullptr) {
        HILOG_ERROR("%{public}s record is nullptr", __func__);
        return;
    }
    if (resultCode == ERR_OK) {
        SetConnectState(ConnectionState::CONNECTED);
        targetService_->SetAbilityState(AbilityState::ACTIVE);
    }
    const AppExecFwk::AbilityInfo &abilityInfo = targetService_->GetAbilityInfo();
    AppExecFwk::ElementName element(abilityInfo.deviceId, abilityInfo.bundleName, abilityInfo.name);
    auto remoteObject = targetService_->GetConnRemoteObject();
    if (connCallback_) {
        connCallback_->OnAbilityConnectDone(element, remoteObject, resultCode);
    }
    HILOG_INFO("%{public}s, result: %{public}d. connectstate:%{public}d.", __func__, resultCode, state_);
}

void ConnectionRecord::CompleteDisconnect(int resultCode)
{
    if (resultCode == ERR_OK) {
        SetConnectState(ConnectionState::DISCONNECTED);
    }
    if (targetService_ == nullptr) {
        return;
    }
    const AppExecFwk::AbilityInfo &abilityInfo = targetService_->GetAbilityInfo();
    AppExecFwk::ElementName element(abilityInfo.deviceId, abilityInfo.bundleName, abilityInfo.name);
    if (connCallback_) {
        connCallback_->OnAbilityDisconnectDone(element, resultCode);
    }
    HILOG_INFO("%{public}s, result: %{public}d. connectstate:%{public}d.", __func__, resultCode, state_);
}

void ConnectionRecord::ScheduleDisconnectAbilityDone()
{
    if (state_ != ConnectionState::DISCONNECTING) {
        HILOG_ERROR("fail to schedule disconnect ability done, current state is not disconnecting.");
        return;
    }

    std::shared_ptr<AbilityEventHandler> handler =
        DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
    } else {
        std::string taskName = std::string("DisconnectTimeout_") + std::to_string(recordId_);
        handler->RemoveTask(taskName);
    }

    CompleteDisconnect(ERR_OK);
}

void ConnectionRecord::ScheduleConnectAbilityDone()
{
    if (state_ != ConnectionState::CONNECTING) {
        HILOG_ERROR("fail to schedule connect ability done, current state is not connecting.");
        return;
    }
    std::shared_ptr<AbilityEventHandler> handler =
        DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
    } else {
        std::string taskName = std::string("ConnectTimeout_") + std::to_string(recordId_);
        handler->RemoveTask(taskName);
    }

    CompleteConnect(ERR_OK);
}

void ConnectionRecord::DisconnectTimeout()
{
    if (targetService_ == nullptr) {
        HILOG_ERROR("target service ability is nullptr.");
        return;
    }
    /* force to disconnect */
    /* so scheduler target service disconnect done */
    DelayedSingleton<AbilityManagerService>::GetInstance()->ScheduleDisconnectAbilityDone(targetService_->GetToken());
}

std::string ConnectionRecord::ConvertConnectionState(const ConnectionState &state) const
{
    switch (state) {
        case ConnectionState::INIT:
            return "INIT";
        case ConnectionState::CONNECTING:
            return "CONNECTING";
        case ConnectionState::CONNECTED:
            return "CONNECTED";
        case ConnectionState::DISCONNECTING:
            return "DISCONNECTING";
        case ConnectionState::DISCONNECTED:
            return "DISCONNECTED";
        default:
            return "INVALIDSTATE";
    }
}

void ConnectionRecord::Dump(std::vector<std::string> &info) const
{
    info.emplace_back("     > " + GetAbilityRecord()->GetAbilityInfo().bundleName + "/" +
                      GetAbilityRecord()->GetAbilityInfo().name + "   connectionState #" +
                      ConvertConnectionState(GetConnectState()));
}
}  // namespace AAFwk
}  // namespace OHOS