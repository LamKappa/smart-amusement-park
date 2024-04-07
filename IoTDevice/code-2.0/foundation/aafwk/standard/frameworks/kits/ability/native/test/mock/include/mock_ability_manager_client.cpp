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

#include "mock_ability_manager_client.h"
#include <gtest/gtest.h>
#include "ability_manager_client.h"

namespace OHOS {
namespace AAFwk {
std::shared_ptr<AbilityManagerClient> AbilityManagerClient::instance_ = nullptr;
std::mutex AbilityManagerClient::mutex_;

std::shared_ptr<AbilityManagerClient> AbilityManagerClient::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<AbilityManagerClient>();
        }
    }
    return instance_;
}

AbilityManagerClient::AbilityManagerClient()
{
    // log
}

AbilityManagerClient::~AbilityManagerClient()
{}

ErrCode AbilityManagerClient::AttachAbilityThread(
    const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token)
{
    GTEST_LOG_(INFO) << "Mock AbilityManagerClient::AttachAbilityThread called";
    return -1;
}

ErrCode AbilityManagerClient::AbilityTransitionDone(const sptr<IRemoteObject> &token, int state)
{
    return -1;
}

ErrCode AbilityManagerClient::ScheduleConnectAbilityDone(
    const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject)
{
    return -1;
}

ErrCode AbilityManagerClient::ScheduleDisconnectAbilityDone(const sptr<IRemoteObject> &token)
{
    return -1;
}

ErrCode AbilityManagerClient::ScheduleCommandAbilityDone(const sptr<IRemoteObject> &token)
{
    return -1;
}

void AbilityManagerClient::AddWindowInfo(const sptr<IRemoteObject> &token, int32_t windowToken)
{
    return;
}

ErrCode AbilityManagerClient::StartAbility(const Want &want, int requestCode)
{
    return -1;
}

ErrCode AbilityManagerClient::TerminateAbility(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant)
{
    return -1;
}

ErrCode AbilityManagerClient::TerminateAbility(const sptr<IRemoteObject> &callerToken, int requestCode)
{
    GTEST_LOG_(INFO) << "Mock AbilityManagerClient::TerminateAbility called";
    return -1;
}

ErrCode AbilityManagerClient::TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId)
{
    return -1;
}

ErrCode AbilityManagerClient::ConnectAbility(
    const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken)
{
    return -1;
}

ErrCode AbilityManagerClient::DisconnectAbility(const sptr<IAbilityConnection> &connect)
{
    return -1;
}

sptr<IAbilityScheduler> AbilityManagerClient::AcquireDataAbility(
    const Uri &uri, bool tryBind, const sptr<IRemoteObject> &callerToken)
{
    GTEST_LOG_(INFO) << "Mock AcquireDataAbility called";
    sptr<IAbilityScheduler> dataScheduler = new (std::nothrow) OHOS::AppExecFwk::MockAbilityThread();
    return dataScheduler;
}

ErrCode AbilityManagerClient::ReleaseDataAbility(
    sptr<IAbilityScheduler> dataAbilityScheduler, const sptr<IRemoteObject> &callerToken)
{
    GTEST_LOG_(INFO) << "Mock ReleaseDataAbility called";
    return ERR_OK;
}

ErrCode AbilityManagerClient::DumpState(const std::string &args, std::vector<std::string> &state)
{
    return -1;
}

ErrCode AbilityManagerClient::Connect()
{
    return -1;
}

ErrCode AbilityManagerClient::GetAllStackInfo(StackInfo &stackInfo)
{
    return -1;
}

ErrCode AbilityManagerClient::StopServiceAbility(const Want &want)
{
    return -1;
}
}  // namespace AAFwk
}  // namespace OHOS
