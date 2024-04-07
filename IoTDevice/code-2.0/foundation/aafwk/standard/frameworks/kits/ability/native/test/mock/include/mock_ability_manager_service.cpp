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

#include "mock_ability_manager_service.h"
#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <string>
#include <unistd.h>

using OHOS::AppExecFwk::ElementName;

namespace OHOS {
namespace AAFwk {
MockAbilityManagerService::MockAbilityManagerService() : abilityScheduler_(nullptr)
{
    abilityScheduler_ = nullptr;
}

MockAbilityManagerService::~MockAbilityManagerService()
{}

int MockAbilityManagerService::StartAbility(const Want &want, int requestCode)
{
    AbilityLifeCycleState state = AbilityLifeCycleState::ABILITY_STATE_INITIAL;
    switch (requestCode) {
        // Test code, representing the life cycle: Ability_ STATE_ INITIAL
        case RequestCode::E_STATE_INITIAL:
            state = AbilityLifeCycleState::ABILITY_STATE_INITIAL;
            break;
        // Test code, representing the life cycle: ABILITY_STATE_INACTIVE
        case RequestCode::E_STATE_INACTIVE:
            state = AbilityLifeCycleState::ABILITY_STATE_INACTIVE;
            break;
        // Test code, representing the life cycle: ABILITY_STATE_ACTIVE
        case RequestCode::E_STATE_ACTIVE:
            state = AbilityLifeCycleState::ABILITY_STATE_ACTIVE;
            break;
        // Test code, representing the life cycle: ABILITY_STATE_BACKGROUND
        case RequestCode::E_STATE_BACKGROUND:
            state = AbilityLifeCycleState::ABILITY_STATE_BACKGROUND;
            break;
        // Test code, representing the life cycle: ABILITY_STATE_SUSPENDED
        case RequestCode::E_STATE_SUSPENDED:
            state = AbilityLifeCycleState::ABILITY_STATE_SUSPENDED;
            break;
        default:
            break;
    }

    if (abilityScheduler_ != nullptr) {
        want_ = want;
        want_.SetElementName("BundleName", "abilityName");
        LifeCycleStateInfo stateInfo;
        stateInfo.state = state;
        abilityScheduler_->ScheduleAbilityTransaction(want_, stateInfo);
    }

    return 0;
}

int MockAbilityManagerService::TerminateAbility(
    const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant)
{
    GTEST_LOG_(INFO) << "MockAbilityManagerService::TerminateAbility";

    if (abilityScheduler_ != nullptr) {
        LifeCycleStateInfo stateInfo;
        stateInfo.state = AbilityLifeCycleState::ABILITY_STATE_INITIAL;
        abilityScheduler_->ScheduleAbilityTransaction(want_, stateInfo);

        int ceode = 250;
        abilityScheduler_->SendResult(ceode, resultCode, *resultWant);
    }
    return 0;
}

int MockAbilityManagerService::ConnectAbility(
    const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken)
{
    GTEST_LOG_(INFO) << "MockAbilityManagerService::ConnectAbility";
    return ERR_OK;
}

int MockAbilityManagerService::DisconnectAbility(const sptr<IAbilityConnection> &connect)
{
    GTEST_LOG_(INFO) << "MockAbilityManagerService::DisconnectAbility";
    return ERR_OK;
}

int MockAbilityManagerService::AttachAbilityThread(
    const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token)
{
    abilityScheduler_ = scheduler;
    EXPECT_NE(nullptr, token);
    return 0;
}

void MockAbilityManagerService::DumpState(const std::string &args, std::vector<std::string> &info)
{}

int MockAbilityManagerService::AbilityTransitionDone(const sptr<IRemoteObject> &token, int state)
{
    return 0;
}

int MockAbilityManagerService::ScheduleConnectAbilityDone(
    const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject)
{
    return 0;
}

int MockAbilityManagerService::ScheduleDisconnectAbilityDone(const sptr<IRemoteObject> &token)
{
    return 0;
}

int MockAbilityManagerService::ScheduleCommandAbilityDone(const sptr<IRemoteObject> &token)
{
    return 0;
}

void MockAbilityManagerService::AddWindowInfo(const sptr<IRemoteObject> &token, int32_t windowToken)
{}

int MockAbilityManagerService::TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId)
{
    GTEST_LOG_(INFO) << "MockAbilityManagerService::TerminateAbilityResult";
    return ERR_OK;
}

int MockAbilityManagerService::StopServiceAbility(const Want &want)
{
    GTEST_LOG_(INFO) << "MockAbilityManagerService::StopServiceAbility";
    return ERR_OK;
}

int MockAbilityManagerService::RemoveMission(int id)
{
    return 0;
}

int MockAbilityManagerService::RemoveStack(int id)
{
    return 0;
}

int MockAbilityManagerService::MoveMissionToTop(int32_t missionId)
{
    return 0;
}

int MockAbilityManagerService::KillProcess(const std::string &bundleName)
{
    return 0;
}

int MockAbilityManagerService::UninstallApp(const std::string &bundleName)
{
    return 0;
}
}  // namespace AAFwk
}  // namespace OHOS
