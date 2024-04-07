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
    GTEST_LOG_(INFO) << "MockAbilityManagerService::StartAbility called";
    curstate_ = AbilityLifeCycleState::ABILITY_STATE_INITIAL;
    switch (requestCode) {
        case AbilityLifeCycleState::ABILITY_STATE_INITIAL:
            curstate_ = AbilityLifeCycleState::ABILITY_STATE_INITIAL;
            break;
        case AbilityLifeCycleState::ABILITY_STATE_INACTIVE:
            curstate_ = AbilityLifeCycleState::ABILITY_STATE_INACTIVE;
            break;
        case AbilityLifeCycleState::ABILITY_STATE_ACTIVE:
            curstate_ = AbilityLifeCycleState::ABILITY_STATE_ACTIVE;
            break;
        case AbilityLifeCycleState::ABILITY_STATE_BACKGROUND:
            curstate_ = AbilityLifeCycleState::ABILITY_STATE_BACKGROUND;
            break;
        case AbilityLifeCycleState::ABILITY_STATE_SUSPENDED:
            curstate_ = AbilityLifeCycleState::ABILITY_STATE_SUSPENDED;
            break;
        default:
            break;
    }

    if (abilityScheduler_ != nullptr) {
        want_ = want;
        want_.SetElementName("BundleName", "abilityName");
        LifeCycleStateInfo stateInfo;
        stateInfo.state = curstate_;
        abilityScheduler_->ScheduleAbilityTransaction(want_, stateInfo);
    }

    return 0;
}

int MockAbilityManagerService::StartAbility(const Want &want, const sptr<IRemoteObject> &callerToken, int requestCode)
{
    return 0;
}

int MockAbilityManagerService::TerminateAbility(
    const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant)
{
    GTEST_LOG_(INFO) << "MockAbilityManagerService::TerminateAbility";

    if (abilityScheduler_ != nullptr) {
        LifeCycleStateInfo stateInfo;
        stateInfo.state = AbilityLifeCycleState::ABILITY_STATE_INITIAL;
        curstate_ = AbilityLifeCycleState::ABILITY_STATE_INITIAL;
        abilityScheduler_->ScheduleAbilityTransaction(want_, stateInfo);

        int ceode = 250;
        abilityScheduler_->SendResult(ceode, resultCode, *resultWant);
    }
    return 0;
}

int MockAbilityManagerService::ConnectAbility(
    const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken)
{
    if (abilityScheduler_ != nullptr) {
        PacMap inState;
        abilityScheduler_->ScheduleSaveAbilityState(inState);
        abilityScheduler_->ScheduleRestoreAbilityState(inState);
    }
    return 0;
}

int MockAbilityManagerService::DisconnectAbility(const sptr<IAbilityConnection> &connect)
{
    GTEST_LOG_(INFO) << "MockAbilityManagerService::DisconnectAbility";
    AbilityLifeCycleState lifeState = AbilityLifeCycleState::ABILITY_STATE_ACTIVE;

    if (abilityScheduler_ != nullptr) {
        LifeCycleStateInfo stateInfo;
        stateInfo.state = lifeState;
        stateInfo.isNewWant = true;
        want_.SetElementName("BundleName", "abilityName");
        curstate_ = AbilityLifeCycleState::ABILITY_STATE_ACTIVE;
        abilityScheduler_->ScheduleAbilityTransaction(want_, stateInfo);
    }
    return 0;
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
    GTEST_LOG_(INFO) << "MockAbilityManagerService::AbilityTransitionDone called";
    EXPECT_EQ(curstate_, state);
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

int MockAbilityManagerService::TerminateAbilityByCaller(const sptr<IRemoteObject> &callerToken, int requestCode)
{
    return 0;
}

int MockAbilityManagerService::TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId)
{
    return 0;
}

int MockAbilityManagerService::StopServiceAbility(const Want &want)
{
    return 0;
}

int MockAbilityManagerService::GetRecentMissions(
    const int32_t numMax, const int32_t flags, std::vector<RecentMissionInfo> &recentList)
{
    return 0;
}

int MockAbilityManagerService::GetMissionSnapshot(const int32_t missionId, MissionSnapshotInfo &snapshot)
{
    return 0;
}

int MockAbilityManagerService::RemoveMission(int id)
{
    return 0;
}

int MockAbilityManagerService::RemoveStack(int id)
{
    return 0;
}

sptr<IAbilityScheduler> MockAbilityManagerService::AcquireDataAbility(
    const Uri &uri, bool tryBind, const sptr<IRemoteObject> &callerToken)
{
    if (abilityScheduler_ != nullptr) {
        return abilityScheduler_;
    }
    return nullptr;
}

ErrCode MockAbilityManagerService::ReleaseDataAbility(
    sptr<IAbilityScheduler> dataAbilityScheduler, const sptr<IRemoteObject> &callerToken)
{
    if (abilityScheduler_ != nullptr) {
        abilityScheduler_ = nullptr;
        return 0;
    }
    return -1;
}
}  // namespace AAFwk
}  // namespace OHOS
