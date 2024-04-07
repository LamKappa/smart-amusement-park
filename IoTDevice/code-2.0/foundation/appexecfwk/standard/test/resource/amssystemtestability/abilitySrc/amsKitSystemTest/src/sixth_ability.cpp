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

#include "sixth_ability.h"
#include <iostream>
#include <numeric>
#include <sstream>
#include "app_log_wrapper.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

namespace {
const int cycleCount = 1000;
}  // namespace

#define APPREGISTERABILITYLIFECYCALLBACK(onAbilityFunctionName, getAbilityCountFunction, expected, code) \
    auto callback = std::make_shared<KitTestFirstLifecycleCallbacks>();                                  \
    Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback);                              \
    std::shared_ptr<Ability> ability = std::make_shared<Ability>();                                      \
    Ability::GetApplication()->onAbilityFunctionName(ability);                                           \
    bool result = (callback->GetCallbackCount().getAbilityCountFunction() == expected);                  \
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));                     \
    Ability::GetApplication()->UnregisterAbilityLifecycleCallbacks(callback)

#define APPUNREGISTERABILITYLIFECYCALLBACK(onAbilityFunctionName, getAbilityCountFunction, expected, code) \
    auto callback = std::make_shared<KitTestFirstLifecycleCallbacks>();                                    \
    Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback);                                \
    Ability::GetApplication()->UnregisterAbilityLifecycleCallbacks(callback);                              \
    std::shared_ptr<Ability> ability = std::make_shared<Ability>();                                        \
    Ability::GetApplication()->onAbilityFunctionName(ability);                                             \
    bool result = (callback->GetCallbackCount().getAbilityCountFunction() == expected);                    \
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result))

void SixthAbility::SubscribeEvent(const vector_conststr &eventList)
{
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber = std::make_shared<KitTestSixEventSubscriber>(subscribeInfo);
    subscriber->sixthAbility_ = this;
    CommonEventManager::SubscribeCommonEvent(subscriber);
}

void SixthAbility::ApplicationStByCode(int apiIndex, int caseIndex, int code)
{
    APP_LOGI("SixthAbility::ApplicationStByCode");
    if (mapStKitFunc_.find(apiIndex) != mapStKitFunc_.end() &&
        static_cast<int>(mapStKitFunc_[apiIndex].size()) > caseIndex) {
        mapStKitFunc_[apiIndex][caseIndex](code);
    } else {
        APP_LOGI("ApplicationStByCode error");
    }
}

// KitTest Start
// RegisterAbilityLifecycleCallbacks ST case
void SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase1(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase1");
    APPREGISTERABILITYLIFECYCALLBACK(OnAbilityStart, GetOnAbilityStartCount, 1, code);
}

void SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase2(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase2");
    APPREGISTERABILITYLIFECYCALLBACK(OnAbilityInactive, GetOnAbilityInactiveCount, 1, code);
}

void SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase3(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase3");
    APPREGISTERABILITYLIFECYCALLBACK(OnAbilityBackground, GetOnAbilityBackgroundCount, 1, code);
}

void SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase4(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase4");
    APPREGISTERABILITYLIFECYCALLBACK(OnAbilityForeground, GetOnAbilityForegroundCount, 1, code);
}

void SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase5(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase5");
    APPREGISTERABILITYLIFECYCALLBACK(OnAbilityActive, GetOnAbilityActiveCount, 1, code);
}

void SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase6(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase5");
    APPREGISTERABILITYLIFECYCALLBACK(OnAbilityStop, GetOnAbilityStopCount, 1, code);
}

void SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase7(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase7");
    auto callback = std::make_shared<KitTestFirstLifecycleCallbacks>();
    for (int i = 0; i < cycleCount; i++) {
        Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback);
    }
    std::shared_ptr<Ability> ability = std::make_shared<Ability>();
    Ability::GetApplication()->OnAbilityStart(ability);
    bool result = (callback->GetCallbackCount().GetOnAbilityStartCount() == cycleCount);
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
    Ability::GetApplication()->UnregisterAbilityLifecycleCallbacks(callback);
}

void SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase8(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterAbilityLifecycleCallbacksCase8");
    auto callback1 = std::make_shared<KitTestFirstLifecycleCallbacks>();
    auto callback2 = std::make_shared<KitTestSecondLifecycleCallbacks>();
    Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback1);
    Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback2);
    std::shared_ptr<Ability> ability = std::make_shared<Ability>();
    Ability::GetApplication()->OnAbilityStart(ability);
    bool result = ((callback1->GetCallbackCount().GetOnAbilityStartCount() == 1) &&
                   (callback2->GetCallbackCount().GetOnAbilityStartCount() == 1));
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
    Ability::GetApplication()->UnregisterAbilityLifecycleCallbacks(callback1);
    Ability::GetApplication()->UnregisterAbilityLifecycleCallbacks(callback2);
}

// UnregisterAbilityLifecycleCallbacks
void SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase1(int code)
{
    APP_LOGI("SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase1");
    APPUNREGISTERABILITYLIFECYCALLBACK(OnAbilityStart, GetOnAbilityStartCount, 0, code);
}

void SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase2(int code)
{
    APP_LOGI("SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase2");
    APPUNREGISTERABILITYLIFECYCALLBACK(OnAbilityInactive, GetOnAbilityInactiveCount, 0, code);
}

void SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase3(int code)
{
    APP_LOGI("SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase3");
    APPUNREGISTERABILITYLIFECYCALLBACK(OnAbilityBackground, GetOnAbilityBackgroundCount, 0, code);
}

void SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase4(int code)
{
    APP_LOGI("SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase4");
    APPUNREGISTERABILITYLIFECYCALLBACK(OnAbilityForeground, GetOnAbilityForegroundCount, 0, code);
}

void SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase5(int code)
{
    APP_LOGI("SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase5");
    APPUNREGISTERABILITYLIFECYCALLBACK(OnAbilityActive, GetOnAbilityActiveCount, 0, code);
}

void SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase6(int code)
{
    APP_LOGI("SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase6");
    APPUNREGISTERABILITYLIFECYCALLBACK(OnAbilityStop, GetOnAbilityStopCount, 0, code);
}

void SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase7(int code)
{
    APP_LOGI("SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase7");
    auto callback = std::make_shared<KitTestFirstLifecycleCallbacks>();
    Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback);
    for (int i = 0; i < cycleCount; i++) {
        Ability::GetApplication()->UnregisterAbilityLifecycleCallbacks(callback);
    }
    std::shared_ptr<Ability> ability = std::make_shared<Ability>();
    Ability::GetApplication()->OnAbilityStart(ability);
    bool result = (callback->GetCallbackCount().GetOnAbilityStartCount() == 0);
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
}

void SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase8(int code)
{
    APP_LOGI("SixthAbility::SkillUnregisterAbilityLifecycleCallbacksCase8");
    auto callback1 = std::make_shared<KitTestFirstLifecycleCallbacks>();
    auto callback2 = std::make_shared<KitTestSecondLifecycleCallbacks>();
    Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback1);
    Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback2);
    Ability::GetApplication()->UnregisterAbilityLifecycleCallbacks(callback1);
    Ability::GetApplication()->UnregisterAbilityLifecycleCallbacks(callback2);
    std::shared_ptr<Ability> ability = std::make_shared<Ability>();
    Ability::GetApplication()->OnAbilityStart(ability);
    bool result = ((callback1->GetCallbackCount().GetOnAbilityStartCount() == 0) &&
                   (callback2->GetCallbackCount().GetOnAbilityStartCount() == 0));
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
}

// DispatchAbilitySavedState ST case
void SixthAbility::SkillDispatchAbilitySavedStateCase1(int code)
{
    APP_LOGI("SixthAbility::SkillDispatchAbilitySavedStateCase1");
    auto callback = std::make_shared<KitTestFirstLifecycleCallbacks>();
    Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback);
    PacMap outState;
    Ability::GetApplication()->OnAbilitySaveState(outState);
    bool result = (callback->GetCallbackCount().GetOnAbilitySaveStateCount() == 1);
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
    Ability::GetApplication()->UnregisterAbilityLifecycleCallbacks(callback);
}

void SixthAbility::SkillDispatchAbilitySavedStateCase2(int code)
{
    APP_LOGI("SixthAbility::SkillDispatchAbilitySavedStateCase2");
    auto callback = std::make_shared<KitTestFirstLifecycleCallbacks>();
    for (int i = 0; i < cycleCount; i++) {
        Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback);
    }
    PacMap outState;
    Ability::GetApplication()->OnAbilitySaveState(outState);
    bool result = (callback->GetCallbackCount().GetOnAbilitySaveStateCount() == cycleCount);
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
    Ability::GetApplication()->UnregisterAbilityLifecycleCallbacks(callback);
}

// RegisterElementsCallbacks
void SixthAbility::SkillRegisterElementsCallbacksCase1(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterElementsCallbacksCase1");
    auto callback = std::make_shared<KitTestFirstElementsCallback>();
    Ability::GetApplication()->RegisterElementsCallbacks(callback);
    Configuration configuration;
    Ability::GetApplication()->OnConfigurationUpdated(configuration);
    bool result = (callback->GetCallbackCount().GetOnConfigurationCount() == 1);
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
    Ability::GetApplication()->UnregisterElementsCallbacks(callback);
}

void SixthAbility::SkillRegisterElementsCallbacksCase2(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterElementsCallbacksCase2");
    auto callback = std::make_shared<KitTestFirstElementsCallback>();
    Ability::GetApplication()->RegisterElementsCallbacks(callback);
    Ability::GetApplication()->OnMemoryLevel(1);
    bool result = (callback->GetCallbackCount().GetOnMemoryLevelCount() == 1);
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
    Ability::GetApplication()->UnregisterElementsCallbacks(callback);
}

void SixthAbility::SkillRegisterElementsCallbacksCase3(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterElementsCallbacksCase3");
    auto callback = std::make_shared<KitTestFirstElementsCallback>();
    for (int i = 0; i < cycleCount; i++) {
        Ability::GetApplication()->RegisterElementsCallbacks(callback);
    }
    Configuration configuration;
    Ability::GetApplication()->OnConfigurationUpdated(configuration);
    bool result = (callback->GetCallbackCount().GetOnConfigurationCount() == cycleCount);
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
    Ability::GetApplication()->UnregisterElementsCallbacks(callback);
}

void SixthAbility::SkillRegisterElementsCallbacksCase4(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterElementsCallbacksCase4");
    auto callback1 = std::make_shared<KitTestSecondElementsCallback>();
    auto callback2 = std::make_shared<KitTestFirstElementsCallback>();
    Ability::GetApplication()->RegisterElementsCallbacks(callback1);
    Ability::GetApplication()->RegisterElementsCallbacks(callback2);
    Ability::GetApplication()->OnMemoryLevel(1);
    bool result = ((callback1->GetCallbackCount().GetOnMemoryLevelCount() == 1) &&
                   (callback2->GetCallbackCount().GetOnMemoryLevelCount() == 1));
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
    Ability::GetApplication()->UnregisterElementsCallbacks(callback1);
    Ability::GetApplication()->UnregisterElementsCallbacks(callback2);
}

// UnregisterElementsCallbacks
void SixthAbility::SkillUnregisterElementsCallbacksCase1(int code)
{
    APP_LOGI("SixthAbility::SkillUnregisterElementsCallbacksCase1");
    auto callback = std::make_shared<KitTestFirstElementsCallback>();
    Ability::GetApplication()->RegisterElementsCallbacks(callback);
    Ability::GetApplication()->UnregisterElementsCallbacks(callback);
    Configuration configuration;
    Ability::GetApplication()->OnConfigurationUpdated(configuration);
    bool result = (callback->GetCallbackCount().GetOnConfigurationCount() == 0);
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
}

void SixthAbility::SkillUnregisterElementsCallbacksCase2(int code)
{
    APP_LOGI("SixthAbility::SkillUnregisterElementsCallbacksCase2");
    auto callback = std::make_shared<KitTestFirstElementsCallback>();
    Ability::GetApplication()->RegisterElementsCallbacks(callback);
    Ability::GetApplication()->UnregisterElementsCallbacks(callback);
    Ability::GetApplication()->OnMemoryLevel(1);
    bool result = (callback->GetCallbackCount().GetOnMemoryLevelCount() == 0);
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
}

void SixthAbility::SkillUnregisterElementsCallbacksCase3(int code)
{
    APP_LOGI("SixthAbility::SkillRegisterElementsCallbacksCase3");
    auto callback = std::make_shared<KitTestFirstElementsCallback>();
    Ability::GetApplication()->RegisterElementsCallbacks(callback);
    for (int i = 0; i < cycleCount; i++) {
        Ability::GetApplication()->UnregisterElementsCallbacks(callback);
    }
    Configuration configuration;
    Ability::GetApplication()->OnConfigurationUpdated(configuration);
    bool result = (callback->GetCallbackCount().GetOnConfigurationCount() == 0);
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
}

void SixthAbility::SkillUnregisterElementsCallbacksCase4(int code)
{
    APP_LOGI("SixthAbility::SkillUnregisterElementsCallbacksCase4");
    auto callback1 = std::make_shared<KitTestSecondElementsCallback>();
    auto callback2 = std::make_shared<KitTestFirstElementsCallback>();
    Ability::GetApplication()->RegisterElementsCallbacks(callback1);
    Ability::GetApplication()->RegisterElementsCallbacks(callback2);
    Ability::GetApplication()->UnregisterElementsCallbacks(callback1);
    Ability::GetApplication()->UnregisterElementsCallbacks(callback2);
    Ability::GetApplication()->OnMemoryLevel(1);
    bool result = ((callback1->GetCallbackCount().GetOnMemoryLevelCount() == 0) &&
                   (callback2->GetCallbackCount().GetOnMemoryLevelCount() == 0));
    TestUtils::PublishEvent(g_respPageSixthAbilityST, code, std::to_string(result));
}

// KitTest End
void SixthAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("SixthAbility::Init");
    Ability::Init(abilityInfo, application, handler, token);
    auto callback = std::make_shared<KitTestThirdLifecycleCallbacks>();
    Ability::GetApplication()->RegisterAbilityLifecycleCallbacks(callback);
}

void SixthAbility::OnStart(const Want &want)
{
    APP_LOGI("SixthAbility::onStart");
    GetWantInfo(want);
    Ability::OnStart(want);
    SubscribeEvent(g_requPageSixthAbilitySTVector);
    std::string eventData = GetAbilityName() + g_abilityStateOnStart;
    TestUtils::PublishEvent(g_respPageSixthAbilityST, 0, eventData);
}

void SixthAbility::OnStop()
{
    APP_LOGI("SixthAbility::onStop");
    Ability::OnStop();
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    std::string eventData = GetAbilityName() + g_abilityStateOnStop;
    TestUtils::PublishEvent(g_respPageSixthAbilityST, 0, eventData);
}

void SixthAbility::OnActive()
{
    APP_LOGI("SixthAbility::OnActive");
    Ability::OnActive();
    std::string startBundleName = this->Split(targetBundle_, ",");
    std::string startAabilityName = this->Split(targetAbility_, ",");
    if (!startBundleName.empty() && !startAabilityName.empty()) {
        Want want;
        want.SetElementName(startBundleName, startAabilityName);
        want.SetParam("shouldReturn", shouldReturn_);
        if (!targetBundle_.empty() && !targetAbility_.empty()) {
            want.SetParam("targetBundle", targetBundle_);
            want.SetParam("targetAbility", targetAbility_);
        }
        StartAbility(want);
    }
    if (std::string::npos != shouldReturn_.find(GetAbilityName())) {
        TerminateAbility();
    }
    Clear();
    std::string eventData = GetAbilityName() + g_abilityStateOnActive;
    TestUtils::PublishEvent(g_respPageSixthAbilityST, 0, eventData);
}

void SixthAbility::OnInactive()
{
    APP_LOGI("SixthAbility::OnInactive");
    Ability::OnInactive();
    std::string eventData = GetAbilityName() + g_abilityStateOnInactive;
    TestUtils::PublishEvent(g_respPageSixthAbilityST, 0, eventData);
}

void SixthAbility::OnBackground()
{
    APP_LOGI("SixthAbility::OnBackground");
    Ability::OnBackground();
    std::string eventData = GetAbilityName() + g_abilityStateOnBackground;
    TestUtils::PublishEvent(g_respPageSixthAbilityST, 0, eventData);
}

void SixthAbility::OnForeground(const Want &want)
{
    APP_LOGI("SixthAbility::OnForeground");
    GetWantInfo(want);
    Ability::OnForeground(want);
    std::string eventData = GetAbilityName() + g_abilityStateOnForeground;
    TestUtils::PublishEvent(g_respPageSixthAbilityST, 0, eventData);
}

void SixthAbility::OnNewWant(const Want &want)
{
    APP_LOGI("SixthAbility::OnNewWant");
    GetWantInfo(want);
    Ability::OnNewWant(want);
    std::string eventData = GetAbilityName() + g_abilityStateOnNewWant;
    TestUtils::PublishEvent(g_respPageSixthAbilityST, 0, eventData);
}

void SixthAbility::Clear()
{
    shouldReturn_ = "";
    targetBundle_ = "";
    targetAbility_ = "";
}

void SixthAbility::GetWantInfo(const Want &want)
{
    Want mWant(want);
    shouldReturn_ = mWant.GetStringParam("shouldReturn");
    targetBundle_ = mWant.GetStringParam("targetBundle");
    targetAbility_ = mWant.GetStringParam("targetAbility");
}

std::string SixthAbility::Split(std::string &str, std::string delim)
{
    std::string result;
    if (!str.empty()) {
        size_t index = str.find(delim);
        if (index != std::string::npos) {
            result = str.substr(0, index);
            str = str.substr(index + delim.size());
        } else {
            result = str;
            str = "";
        }
    }
    return result;
}

void KitTestSixEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (g_requPageSixthAbilityST == eventName) {
        auto target = data.GetData();
        vector_str splitResult = TestUtils::split(target, "_");
        auto keyMap = splitResult.at(0);
        if (mapTestFunc_.find(keyMap) != mapTestFunc_.end() && splitResult.size() >= 3) {
            auto apiIndex = atoi(splitResult.at(1).c_str());
            auto caseIndex = atoi(splitResult.at(2).c_str());
            mapTestFunc_[keyMap](apiIndex, caseIndex, data.GetCode());
        } else {
            if (keyMap == "TerminateAbility") {
                KitTerminateAbility();
            } else {
                APP_LOGI("OnReceiveEvent: CommonEventData error(%{public}s)", target.c_str());
            }
        }
    }
}

void KitTestSixEventSubscriber::ApplicationStByCode(int apiIndex, int caseIndex, int code)
{
    if (sixthAbility_ != nullptr) {
        sixthAbility_->ApplicationStByCode(apiIndex, caseIndex, code);
    }
}

void KitTestSixEventSubscriber::KitTerminateAbility()
{
    if (sixthAbility_ != nullptr) {
        sixthAbility_->TerminateAbility();
    }
}

// Record the number of callback function exec
int CallbackCount::GetOnAbilityStartCount()
{
    return onAbilityStartCount;
}

int CallbackCount::GetOnAbilityInactiveCount()
{
    return onAbilityInactiveCount;
}

int CallbackCount::GetOnAbilityBackgroundCount()
{
    return onAbilityBackgroundCount;
}

int CallbackCount::GetOnAbilityForegroundCount()
{
    return onAbilityForegroundCount;
}

int CallbackCount::GetOnAbilityActiveCount()
{
    return onAbilityActiveCount;
}

int CallbackCount::GetOnAbilityStopCount()
{
    return onAbilityStopCount;
}

int CallbackCount::GetOnAbilitySaveStateCount()
{
    return onAbilitySaveStateCount;
}

int CallbackCount::GetOnConfigurationCount()
{
    return onConfigurationCount;
}

int CallbackCount::GetOnMemoryLevelCount()
{
    return onMemoryLevelCount;
}

void CallbackCount::SetOnAbilityStartCount()
{
    onAbilityStartCount++;
}

void CallbackCount::SetOnAbilityInactiveCount()
{
    onAbilityInactiveCount++;
}

void CallbackCount::SetOnAbilityBackgroundCount()
{
    onAbilityBackgroundCount++;
}

void CallbackCount::SetOnAbilityForegroundCount()
{
    onAbilityForegroundCount++;
}

void CallbackCount::SetOnAbilityActiveCount()
{
    onAbilityActiveCount++;
}

void CallbackCount::SetOnAbilityStopCount()
{
    onAbilityStopCount++;
}

void CallbackCount::SetOnAbilitySaveStateCount()
{
    onAbilitySaveStateCount++;
}

void CallbackCount::SetOnConfigurationCount()
{
    onConfigurationCount++;
}

void CallbackCount::SetOnMemoryLevelCount()
{
    onMemoryLevelCount++;
}

// The Lifecycle callback function class (First)
void KitTestFirstLifecycleCallbacks::OnAbilityStart(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestFirstLifecycleCallbacks::OnAbilityStart");
    callbackCount_.SetOnAbilityStartCount();
}

void KitTestFirstLifecycleCallbacks::OnAbilityInactive(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestFirstLifecycleCallbacks::OnAbilityInactive");
    callbackCount_.SetOnAbilityInactiveCount();
}

void KitTestFirstLifecycleCallbacks::OnAbilityBackground(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestFirstLifecycleCallbacks::OnAbilityBackground");
    callbackCount_.SetOnAbilityBackgroundCount();
}

void KitTestFirstLifecycleCallbacks::OnAbilityForeground(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestFirstLifecycleCallbacks::OnAbilityForeground");
    callbackCount_.SetOnAbilityForegroundCount();
}

void KitTestFirstLifecycleCallbacks::OnAbilityActive(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestFirstLifecycleCallbacks::OnAbilityActive");
    callbackCount_.SetOnAbilityActiveCount();
}

void KitTestFirstLifecycleCallbacks::OnAbilityStop(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestFirstLifecycleCallbacks::OnAbilityStop");
    callbackCount_.SetOnAbilityStopCount();
}

void KitTestFirstLifecycleCallbacks::OnAbilitySaveState(const PacMap &outState)
{
    APP_LOGI("KitTestFirstLifecycleCallbacks::OnAbilitySaveState");
    callbackCount_.SetOnAbilitySaveStateCount();
}

CallbackCount KitTestFirstLifecycleCallbacks::GetCallbackCount()
{
    return callbackCount_;
}

// The Lifecycle callback function class (Second)
void KitTestSecondLifecycleCallbacks::OnAbilityStart(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestSecondLifecycleCallbacks::OnAbilityStart");
    callbackCount_.SetOnAbilityStartCount();
}

void KitTestSecondLifecycleCallbacks::OnAbilityInactive(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestSecondLifecycleCallbacks::OnAbilityInactive");
    callbackCount_.SetOnAbilityInactiveCount();
}

void KitTestSecondLifecycleCallbacks::OnAbilityBackground(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestSecondLifecycleCallbacks::OnAbilityBackground");
    callbackCount_.SetOnAbilityBackgroundCount();
}

void KitTestSecondLifecycleCallbacks::OnAbilityForeground(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestSecondLifecycleCallbacks::OnAbilityForeground");
    callbackCount_.SetOnAbilityForegroundCount();
}

void KitTestSecondLifecycleCallbacks::OnAbilityActive(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestSecondLifecycleCallbacks::OnAbilityActive");
    callbackCount_.SetOnAbilityActiveCount();
}

void KitTestSecondLifecycleCallbacks::OnAbilityStop(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestSecondLifecycleCallbacks::OnAbilityStop");
    callbackCount_.SetOnAbilityStopCount();
}

void KitTestSecondLifecycleCallbacks::OnAbilitySaveState(const PacMap &outState)
{
    APP_LOGI("KitTestSecondLifecycleCallbacks::OnAbilitySaveState");
    callbackCount_.SetOnAbilitySaveStateCount();
}

CallbackCount KitTestSecondLifecycleCallbacks::GetCallbackCount()
{
    return callbackCount_;
}

// The Lifecycle callback function class (Third)
void KitTestThirdLifecycleCallbacks::OnAbilityStart(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestThirdLifecycleCallbacks::OnAbilityStart");
    std::string eventData = ability->GetAbilityName() + g_onAbilityStart;
    TestUtils::PublishEvent(g_respPageSixthAbilityLifecycleCallbacks, 0, eventData);
}

void KitTestThirdLifecycleCallbacks::OnAbilityInactive(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestThirdLifecycleCallbacks::OnAbilityInactive");
    std::string eventData = ability->GetAbilityName() + g_onAbilityInactive;
    TestUtils::PublishEvent(g_respPageSixthAbilityLifecycleCallbacks, 0, eventData);
}

void KitTestThirdLifecycleCallbacks::OnAbilityBackground(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestThirdLifecycleCallbacks::OnAbilityBackground");
    std::string eventData = ability->GetAbilityName() + g_onAbilityBackground;
    TestUtils::PublishEvent(g_respPageSixthAbilityLifecycleCallbacks, 0, eventData);
}

void KitTestThirdLifecycleCallbacks::OnAbilityForeground(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestThirdLifecycleCallbacks::OnAbilityForeground");
    std::string eventData = ability->GetAbilityName() + g_onAbilityForeground;
    TestUtils::PublishEvent(g_respPageSixthAbilityLifecycleCallbacks, 0, eventData);
}

void KitTestThirdLifecycleCallbacks::OnAbilityActive(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestThirdLifecycleCallbacks::OnAbilityActive");
    std::string eventData = ability->GetAbilityName() + g_onAbilityActive;
    TestUtils::PublishEvent(g_respPageSixthAbilityLifecycleCallbacks, 0, eventData);
}

void KitTestThirdLifecycleCallbacks::OnAbilityStop(const std::shared_ptr<Ability> &ability)
{
    APP_LOGI("KitTestThirdLifecycleCallbacks::OnAbilityStop");
    std::string eventData = ability->GetAbilityName() + g_onAbilityStop;
    TestUtils::PublishEvent(g_respPageSixthAbilityLifecycleCallbacks, 0, eventData);
}

void KitTestThirdLifecycleCallbacks::OnAbilitySaveState(const PacMap &outState)
{
    APP_LOGI("KitTestThirdLifecycleCallbacks::OnAbilitySaveState");
    std::string eventData = g_onAbilitySaveState;
    TestUtils::PublishEvent(g_respPageSixthAbilityLifecycleCallbacks, 0, eventData);
}

CallbackCount KitTestThirdLifecycleCallbacks::GetCallbackCount()
{
    return callbackCount_;
}

// The Elements callback function class (First)
void KitTestFirstElementsCallback::OnConfigurationUpdated(
    const std::shared_ptr<Ability> &ability, const Configuration &config)
{
    APP_LOGI("KitTestFirstElementsCallback::OnConfigurationUpdated");
    callbackCount_.SetOnConfigurationCount();
}

void KitTestFirstElementsCallback::OnMemoryLevel(int level)
{
    APP_LOGI("KitTestFirstElementsCallback::OnMemoryLevel");
    callbackCount_.SetOnMemoryLevelCount();
}

CallbackCount KitTestFirstElementsCallback::GetCallbackCount()
{
    return callbackCount_;
}

// The Elements callback function class (Second)
void KitTestSecondElementsCallback::OnConfigurationUpdated(
    const std::shared_ptr<Ability> &ability, const Configuration &config)
{
    APP_LOGI("KitTestSecondElementsCallback::OnConfigurationUpdated");
    callbackCount_.SetOnConfigurationCount();
}

void KitTestSecondElementsCallback::OnMemoryLevel(int level)
{
    APP_LOGI("KitTestSecondElementsCallback::OnMemoryLevel");
    callbackCount_.SetOnMemoryLevelCount();
}

CallbackCount KitTestSecondElementsCallback::GetCallbackCount()
{
    return callbackCount_;
}

REGISTER_AA(SixthAbility)
}  // namespace AppExecFwk
}  // namespace OHOS