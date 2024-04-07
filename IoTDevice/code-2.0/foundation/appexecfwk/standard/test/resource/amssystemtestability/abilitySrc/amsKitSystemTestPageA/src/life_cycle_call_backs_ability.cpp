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

#include "life_cycle_call_backs_ability.h"
#include "app_log_wrapper.h"
#include "base_ability.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;
void ServiceLifecycleCallbacks::DoTask()
{
    if (!LifecycleCallbacksAbility::sequenceNumber_.empty()) {
        switch ((CaseIndex)std::stoi(LifecycleCallbacksAbility::sequenceNumber_)) {
            case CaseIndex::TWO:
                PostTask<ServiceLifecycleCallbacks>();
                break;
            case CaseIndex::THREE:
                DoWhile<ServiceLifecycleCallbacks>();
                break;
            case CaseIndex::SIX:
                break;
            default:
                break;
        }
    }
}
void ServiceLifecycleCallbacks::OnAbilityStart(const std::shared_ptr<Ability> &ability)
{
    DoTask();
    TestUtils::PublishEvent(EVENT_RESP_LIFECYCLE_CALLBACK, 0, "OnAbilityStart");
}

void ServiceLifecycleCallbacks::OnAbilityInactive(const std::shared_ptr<Ability> &ability)
{
    DoTask();
    TestUtils::PublishEvent(EVENT_RESP_LIFECYCLE_CALLBACK, 0, "OnAbilityInactive");
}

void ServiceLifecycleCallbacks::OnAbilityBackground(const std::shared_ptr<Ability> &ability)
{
    DoTask();
    TestUtils::PublishEvent(EVENT_RESP_LIFECYCLE_CALLBACK, 0, "OnAbilityBackground");
}

void ServiceLifecycleCallbacks::OnAbilityForeground(const std::shared_ptr<Ability> &ability)
{
    DoTask();
    TestUtils::PublishEvent(EVENT_RESP_LIFECYCLE_CALLBACK, 0, "OnAbilityForeground");
}

void ServiceLifecycleCallbacks::OnAbilityActive(const std::shared_ptr<Ability> &ability)
{
    DoTask();
    TestUtils::PublishEvent(EVENT_RESP_LIFECYCLE_CALLBACK, 0, "OnAbilityActive");
}

void ServiceLifecycleCallbacks::OnAbilityStop(const std::shared_ptr<Ability> &ability)
{
    DoTask();
    TestUtils::PublishEvent(EVENT_RESP_LIFECYCLE_CALLBACK, 0, "OnAbilityStop");
}

void ServiceLifecycleCallbacks::OnAbilitySaveState(const PacMap &outState)
{
    DoTask();
    TestUtils::PublishEvent(EVENT_RESP_LIFECYCLE_CALLBACK, 0, "OnAbilitySaveState");
}

LifecycleCallbacksAbility::~LifecycleCallbacksAbility()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void LifecycleCallbacksAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("LifecycleCallbacksAbility::Init called.");

    Ability::Init(abilityInfo, application, handler, token);

    SubscribeEvent();
    lifecycleCallbacks_ = std::make_shared<ServiceLifecycleCallbacks>();
    Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(lifecycleCallbacks_);
}

std::string LifecycleCallbacksAbility::GetNoFromWantInfo(const Want &want)
{
    Want wantImpl(want);
    return wantImpl.GetStringParam("No.");
}

void LifecycleCallbacksAbility::OnStart(const Want &want)
{
    APP_LOGI("LifecycleCallbacksAbility::OnStart");

    want_ = want;
    Ability::OnStart(want);
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_CALL_BACKS_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnStart");

    sequenceNumber_ = GetNoFromWantInfo(want);
    if (!LifecycleCallbacksAbility::sequenceNumber_.empty()) {
        switch ((CaseIndex)std::stoi(LifecycleCallbacksAbility::sequenceNumber_)) {
            case CaseIndex::FOUR:
                StopAbility(want);
                break;
            case CaseIndex::FIVE:
                TerminateAbility();
                break;
            default:
                break;
        }
    }
}

void LifecycleCallbacksAbility::StopSelfAbility()
{
    TerminateAbility();
}

void LifecycleCallbacksEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("LifecycleCallbacksEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("LifecycleCallbacksEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("LifecycleCallbacksEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());

    auto eventName = data.GetWant().GetAction();
    if (std::strcmp(eventName.c_str(), APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME.c_str()) == 0) {
        auto target = data.GetData();
        auto func = mapTestFunc_.find(target);
        if (func != mapTestFunc_.end()) {
            func->second();
        } else {
            APP_LOGI(
                "LifecycleCallbacksEventSubscriber::OnReceiveEvent: CommonEventData error(%{public}s)", target.c_str());
        }
    }
}

bool LifecycleCallbacksAbility::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<LifecycleCallbacksEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility = *this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}
void LifecycleCallbacksAbility::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    APP_LOGI("LifecycleCallbacksAbility::OnCommand");

    Ability::OnCommand(want, restart, startId);
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_CALL_BACKS_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnCommand");
}
void LifecycleCallbacksAbility::OnNewWant(const Want &want)
{
    APP_LOGI("LifecycleCallbacksAbility::OnNewWant");

    Ability::OnNewWant(want);
}
void LifecycleCallbacksAbility::OnStop()
{
    APP_LOGI("LifecycleCallbacksAbility::OnStop");

    Ability::OnStop();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_CALL_BACKS_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INITIAL, "OnStop");
}
void LifecycleCallbacksAbility::OnActive()
{
    APP_LOGI("LifecycleCallbacksAbility::OnActive");

    Ability::OnActive();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_CALL_BACKS_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnActive");
}
void LifecycleCallbacksAbility::OnInactive()
{
    APP_LOGI("LifecycleCallbacksAbility::OnInactive");

    Ability::OnInactive();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_CALL_BACKS_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnInactive");
}
void LifecycleCallbacksAbility::OnBackground()
{
    APP_LOGI("LifecycleCallbacksAbility::OnBackground");

    Ability::OnBackground();
    TestUtils::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_RESP_EVENT_NAME,
        AbilityLifecycleExecutor::LifecycleState::BACKGROUND,
        "OnBackground");
}

void LifecycleCallbacksAbility::OnForeground(const Want &want)
{
    APP_LOGI("LifecycleCallbacksAbility::OnForeground");
    Ability::OnForeground(want);
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_CALL_BACKS_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnForeground");
}

REGISTER_AA(LifecycleCallbacksAbility)
}  // namespace AppExecFwk
}  // namespace OHOS