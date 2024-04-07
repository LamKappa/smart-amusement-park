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

#ifndef LIFT_CYCLE_CALL_BACKS_ABILITY_H_
#define LIFT_CYCLE_CALL_BACKS_ABILITY_H_

#include "ability_loader.h"
#include "base_ability.h"
#include "common_event.h"
#include "common_event_manager.h"

namespace OHOS {
namespace AppExecFwk {

class ServiceLifecycleCallbacks : public AbilityLifecycleCallbacks {
public:
    ServiceLifecycleCallbacks() = default;
    virtual ~ServiceLifecycleCallbacks() = default;

    virtual void OnAbilityStart(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityInactive(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityBackground(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityForeground(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityActive(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityStop(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilitySaveState(const PacMap &outState) override;

    void DoTask();
};

class LifecycleCallbacksEventSubscriber;
class LifecycleCallbacksAbility : public Ability {
public:
    ~LifecycleCallbacksAbility();
    static std::string sequenceNumber_;
    void StopSelfAbility();

protected:
    virtual void Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
        const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
        const sptr<IRemoteObject> &token) override;
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnForeground(const Want &want) override;
    virtual void OnNewWant(const Want &want) override;
    virtual void OnCommand(const AAFwk::Want &want, bool restart, int startId) override;
    bool SubscribeEvent();
    std::string GetNoFromWantInfo(const Want &want);

private:
    std::shared_ptr<ServiceLifecycleCallbacks> lifecycleCallbacks_ = {};
    Want want_ = {};
    std::shared_ptr<LifecycleCallbacksEventSubscriber> subscriber_ = {};
};
std::string LifecycleCallbacksAbility::sequenceNumber_ = "";

class LifecycleCallbacksEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    LifecycleCallbacksEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp)
        : EventFwk::CommonEventSubscriber(sp)
    {
        mapTestFunc_ = {
            {"StopAbility", [this]() { StopSelfAbility(); }},
        };
    }
    ~LifecycleCallbacksEventSubscriber() = default;
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data);

    Want want = {};
    LifecycleCallbacksAbility mainAbility = {};
    std::unordered_map<std::string, std::function<void()>> mapTestFunc_ = {};

    void StopSelfAbility()
    {
        mainAbility.StopSelfAbility();
    }
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // LIFT_CYCLE_CALL_BACKS_ABILITY_H_