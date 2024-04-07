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

#include "lifecycle_deal.h"

#include "ability_record.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
LifecycleDeal::LifecycleDeal()
{}

LifecycleDeal::~LifecycleDeal()
{}

void LifecycleDeal::SetScheduler(const sptr<IAbilityScheduler> &scheduler)
{
    abilityScheduler_ = scheduler;
}

void LifecycleDeal::Activate(const Want &want, LifeCycleStateInfo &stateInfo)
{
    HILOG_INFO("LifecycleDeal Activate");
    if (abilityScheduler_ == nullptr) {
        HILOG_ERROR("abilityScheduler_ is nullptr");
        return;
    }
    HILOG_INFO("%{public}s, caller %{public}s, %{public}s, %{public}s",
        __func__,
        stateInfo.caller.deviceId.c_str(),
        stateInfo.caller.bundleName.c_str(),
        stateInfo.caller.abilityName.c_str());
    stateInfo.state = AbilityLifeCycleState::ABILITY_STATE_ACTIVE;
    abilityScheduler_->ScheduleAbilityTransaction(want, stateInfo);
}

void LifecycleDeal::Inactivate(const Want &want, LifeCycleStateInfo &stateInfo)
{
    HILOG_INFO("LifecycleDeal Inactivate");
    if (abilityScheduler_ == nullptr) {
        HILOG_ERROR("abilityScheduler_ is nullptr");
        return;
    }
    stateInfo.state = AbilityLifeCycleState::ABILITY_STATE_INACTIVE;
    abilityScheduler_->ScheduleAbilityTransaction(want, stateInfo);
}

void LifecycleDeal::MoveToBackground(const Want &want, LifeCycleStateInfo &stateInfo)
{
    HILOG_INFO("LifecycleDeal MoveToBackground");
    if (abilityScheduler_ == nullptr) {
        HILOG_ERROR("abilityScheduler_ is nullptr");
        return;
    }
    stateInfo.state = AbilityLifeCycleState::ABILITY_STATE_BACKGROUND;
    abilityScheduler_->ScheduleAbilityTransaction(want, stateInfo);
}

void LifecycleDeal::ConnectAbility(const Want &want)
{
    HILOG_INFO("LifecycleDeal connect ability");
    if (abilityScheduler_ == nullptr) {
        HILOG_ERROR("abilityScheduler_ is nullptr");
        return;
    }
    abilityScheduler_->ScheduleConnectAbility(want);
}

void LifecycleDeal::DisconnectAbility(const Want &want)
{
    HILOG_INFO("LifecycleDeal disconnect ability");
    if (abilityScheduler_ == nullptr) {
        HILOG_ERROR("abilityScheduler_ is nullptr");
        return;
    }
    abilityScheduler_->ScheduleDisconnectAbility(want);
}

void LifecycleDeal::Terminate(const Want &want, LifeCycleStateInfo &stateInfo)
{
    HILOG_INFO("LifecycleDeal Terminate");
    if (abilityScheduler_ == nullptr) {
        HILOG_ERROR("abilityScheduler_ is nullptr");
        return;
    }
    stateInfo.state = AbilityLifeCycleState::ABILITY_STATE_INITIAL;
    abilityScheduler_->ScheduleAbilityTransaction(want, stateInfo);
}

void LifecycleDeal::CommandAbility(const Want &want, bool reStart, int startId)
{
    HILOG_INFO("LifecycleDeal command ability");
    if (abilityScheduler_ == nullptr) {
        HILOG_ERROR("abilityScheduler_ is nullptr");
        return;
    }
    abilityScheduler_->ScheduleCommandAbility(want, reStart, startId);
}
}  // namespace AAFwk
}  // namespace OHOS
