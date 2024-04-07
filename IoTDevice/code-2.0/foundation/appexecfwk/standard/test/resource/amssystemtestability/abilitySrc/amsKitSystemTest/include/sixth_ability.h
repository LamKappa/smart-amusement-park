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

#ifndef _Six_ABILITY_H_
#define _Six_ABILITY_H_
#include <unordered_map>
#include "ability_loader.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "kit_test_common_info.h"
#include "process_info.h"

namespace OHOS {
namespace AppExecFwk {
using vector_str = std::vector<std::string>;
using vector_conststr = std::vector<const std::string>;
using vector_func = std::vector<std::function<void(int)>>;
class KitTestSixEventSubscriber;

class SixthAbility : public Ability {
public:
    void SubscribeEvent(const vector_conststr &eventList);
    void ApplicationStByCode(int apiIndex, int caseIndex, int code);
    // RegisterAbilityLifecycleCallbacks ST case
    void SkillRegisterAbilityLifecycleCallbacksCase1(int code);
    void SkillRegisterAbilityLifecycleCallbacksCase2(int code);
    void SkillRegisterAbilityLifecycleCallbacksCase3(int code);
    void SkillRegisterAbilityLifecycleCallbacksCase4(int code);
    void SkillRegisterAbilityLifecycleCallbacksCase5(int code);
    void SkillRegisterAbilityLifecycleCallbacksCase6(int code);
    void SkillRegisterAbilityLifecycleCallbacksCase7(int code);
    void SkillRegisterAbilityLifecycleCallbacksCase8(int code);

    // UnregisterAbilityLifecycleCallbacks
    void SkillUnregisterAbilityLifecycleCallbacksCase1(int code);
    void SkillUnregisterAbilityLifecycleCallbacksCase2(int code);
    void SkillUnregisterAbilityLifecycleCallbacksCase3(int code);
    void SkillUnregisterAbilityLifecycleCallbacksCase4(int code);
    void SkillUnregisterAbilityLifecycleCallbacksCase5(int code);
    void SkillUnregisterAbilityLifecycleCallbacksCase6(int code);
    void SkillUnregisterAbilityLifecycleCallbacksCase7(int code);
    void SkillUnregisterAbilityLifecycleCallbacksCase8(int code);

    // DispatchAbilitySavedState ST case
    void SkillDispatchAbilitySavedStateCase1(int code);
    void SkillDispatchAbilitySavedStateCase2(int code);

    // RegisterElementsCallbacks
    void SkillRegisterElementsCallbacksCase1(int code);
    void SkillRegisterElementsCallbacksCase2(int code);
    void SkillRegisterElementsCallbacksCase3(int code);
    void SkillRegisterElementsCallbacksCase4(int code);

    // UnregisterElementsCallbacks
    void SkillUnregisterElementsCallbacksCase1(int code);
    void SkillUnregisterElementsCallbacksCase2(int code);
    void SkillUnregisterElementsCallbacksCase3(int code);
    void SkillUnregisterElementsCallbacksCase4(int code);

    std::shared_ptr<KitTestSixEventSubscriber> subscriber;

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

private:
    void Clear();
    void GetWantInfo(const Want &want);
    std::string Split(std::string &str, std::string delim);

    std::string shouldReturn_;
    std::string targetBundle_;
    std::string targetAbility_;
    std::unordered_map<int, vector_func> mapStKitFunc_ = {
        {static_cast<int>(OHOSApplicationApi::RegisterAbilityLifecycleCallbacks),
            {{[this](int code) { SkillRegisterAbilityLifecycleCallbacksCase1(code); }},
                {[this](int code) { SkillRegisterAbilityLifecycleCallbacksCase2(code); }},
                {[this](int code) { SkillRegisterAbilityLifecycleCallbacksCase3(code); }},
                {[this](int code) { SkillRegisterAbilityLifecycleCallbacksCase4(code); }},
                {[this](int code) { SkillRegisterAbilityLifecycleCallbacksCase5(code); }},
                {[this](int code) { SkillRegisterAbilityLifecycleCallbacksCase6(code); }},
                {[this](int code) { SkillRegisterAbilityLifecycleCallbacksCase7(code); }},
                {[this](int code) { SkillRegisterAbilityLifecycleCallbacksCase8(code); }}}},
        {static_cast<int>(OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks),
            {{[this](int code) { SkillUnregisterAbilityLifecycleCallbacksCase1(code); }},
                {[this](int code) { SkillUnregisterAbilityLifecycleCallbacksCase2(code); }},
                {[this](int code) { SkillUnregisterAbilityLifecycleCallbacksCase3(code); }},
                {[this](int code) { SkillUnregisterAbilityLifecycleCallbacksCase4(code); }},
                {[this](int code) { SkillUnregisterAbilityLifecycleCallbacksCase5(code); }},
                {[this](int code) { SkillUnregisterAbilityLifecycleCallbacksCase6(code); }},
                {[this](int code) { SkillUnregisterAbilityLifecycleCallbacksCase7(code); }},
                {[this](int code) { SkillUnregisterAbilityLifecycleCallbacksCase8(code); }}}},
        {static_cast<int>(OHOSApplicationApi::DispatchAbilitySavedState),
            {{[this](int code) { SkillDispatchAbilitySavedStateCase1(code); }},
                {[this](int code) { SkillDispatchAbilitySavedStateCase2(code); }}}},
        {static_cast<int>(OHOSApplicationApi::RegisterElementsCallbacks),
            {{[this](int code) { SkillRegisterElementsCallbacksCase1(code); }},
                {[this](int code) { SkillRegisterElementsCallbacksCase2(code); }},
                {[this](int code) { SkillRegisterElementsCallbacksCase3(code); }},
                {[this](int code) { SkillRegisterElementsCallbacksCase4(code); }}}},
        {static_cast<int>(OHOSApplicationApi::UnregisterElementsCallbacks),
            {{[this](int code) { SkillUnregisterElementsCallbacksCase1(code); }},
                {[this](int code) { SkillUnregisterElementsCallbacksCase2(code); }},
                {[this](int code) { SkillUnregisterElementsCallbacksCase3(code); }},
                {[this](int code) { SkillUnregisterElementsCallbacksCase4(code); }}}},
    };
};

class KitTestSixEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    KitTestSixEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp)
    {
        mapTestFunc_ = {
            {"OHOSApplicationApi",
                [this](int apiIndex, int caseIndex, int code) { ApplicationStByCode(apiIndex, caseIndex, code); }},
        };
        sixthAbility_ = nullptr;
    }
    ~KitTestSixEventSubscriber()
    {
        sixthAbility_ = nullptr;
    }
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data) override;
    void ApplicationStByCode(int apiIndex, int caseIndex, int code);
    void KitTerminateAbility();

    SixthAbility *sixthAbility_;

private:
    std::unordered_map<std::string, std::function<void(int, int, int)>> mapTestFunc_;
};

// Record the number of callback function exec
class CallbackCount {
public:
    int GetOnAbilityStartCount();
    int GetOnAbilityInactiveCount();
    int GetOnAbilityBackgroundCount();
    int GetOnAbilityForegroundCount();
    int GetOnAbilityActiveCount();
    int GetOnAbilityStopCount();
    int GetOnAbilitySaveStateCount();
    int GetOnConfigurationCount();
    int GetOnMemoryLevelCount();

    void SetOnAbilityStartCount();
    void SetOnAbilityInactiveCount();
    void SetOnAbilityBackgroundCount();
    void SetOnAbilityForegroundCount();
    void SetOnAbilityActiveCount();
    void SetOnAbilityStopCount();
    void SetOnAbilitySaveStateCount();
    void SetOnConfigurationCount();
    void SetOnMemoryLevelCount();

private:
    int onAbilityStartCount = 0;
    int onAbilityInactiveCount = 0;
    int onAbilityBackgroundCount = 0;
    int onAbilityForegroundCount = 0;
    int onAbilityActiveCount = 0;
    int onAbilityStopCount = 0;
    int onAbilitySaveStateCount = 0;
    int onConfigurationCount = 0;
    int onMemoryLevelCount = 0;
};

// The Lifecycle callback function class (First)
class KitTestFirstLifecycleCallbacks : public AbilityLifecycleCallbacks {
public:
    virtual void OnAbilityStart(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityInactive(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityBackground(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityForeground(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityActive(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityStop(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilitySaveState(const PacMap &outState) override;
    CallbackCount GetCallbackCount();

private:
    CallbackCount callbackCount_;
};

// The Lifecycle callback function class (Second)
class KitTestSecondLifecycleCallbacks : public AbilityLifecycleCallbacks {
public:
    virtual void OnAbilityStart(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityInactive(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityBackground(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityForeground(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityActive(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityStop(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilitySaveState(const PacMap &outState) override;
    CallbackCount GetCallbackCount();

private:
    CallbackCount callbackCount_;
};

// The Lifecycle callback function class (Third)
class KitTestThirdLifecycleCallbacks : public AbilityLifecycleCallbacks {
public:
    virtual void OnAbilityStart(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityInactive(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityBackground(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityForeground(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityActive(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityStop(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilitySaveState(const PacMap &outState) override;
    CallbackCount GetCallbackCount();

private:
    CallbackCount callbackCount_;
};

// The Elements callback function class (First)
class KitTestFirstElementsCallback : public ElementsCallback {
public:
    virtual void OnConfigurationUpdated(const std::shared_ptr<Ability> &ability, const Configuration &config) override;
    virtual void OnMemoryLevel(int level) override;
    CallbackCount GetCallbackCount();

private:
    CallbackCount callbackCount_;
};

// The Elements callback function class (Second)
class KitTestSecondElementsCallback : public ElementsCallback {
public:
    virtual void OnConfigurationUpdated(const std::shared_ptr<Ability> &ability, const Configuration &config) override;
    virtual void OnMemoryLevel(int level) override;
    CallbackCount GetCallbackCount();

private:
    CallbackCount callbackCount_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // _Six_ABILITY_H_