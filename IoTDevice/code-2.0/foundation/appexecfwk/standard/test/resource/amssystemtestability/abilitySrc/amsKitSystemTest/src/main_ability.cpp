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

#include "main_ability.h"
#include "app_log_wrapper.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

void MainAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("MainAbility::Init");
    Ability::Init(abilityInfo, application, handler, token);
}

void MainAbility::OnStart(const Want &want)
{
    APP_LOGI("MainAbility::onStart");
    SubscribeEvent();
    Ability::OnStart(want);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_LIFECYCLE, Ability::GetState(), "onStart");
}

void MainAbility::OnStop()
{
    APP_LOGI("MainAbility::OnStop");
    Ability::OnStop();
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_LIFECYCLE, Ability::GetState(), "OnStop");
}

void MainAbility::OnActive()
{
    APP_LOGI("MainAbility::OnActive");
    Ability::OnActive();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_LIFECYCLE, Ability::GetState(), "OnActive");
}

void MainAbility::OnInactive()
{
    APP_LOGI("MainAbility::OnInactive");
    Ability::OnInactive();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_LIFECYCLE, Ability::GetState(), "OnInactive");
}

void MainAbility::OnBackground()
{
    APP_LOGI("MainAbility::OnBackground");
    Ability::OnBackground();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_LIFECYCLE, Ability::GetState(), "OnBackground");
}

void MainAbility::OnForeground(const Want &want)
{
    APP_LOGI("MainAbility::OnForeground");
    Ability::OnForeground(want);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_LIFECYCLE, Ability::GetState(), "OnForeground");
}

void MainAbility::OnAbilityResult(int requestCode, int resultCode, const Want &resultData)
{
    APP_LOGI("MainAbility::OnAbilityResult");
    Ability::OnAbilityResult(requestCode, resultCode, resultData);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_ON_ABILITY_RESULT, 0, resultData.ToUri());
}

void MainAbility::OnBackPressed()
{
    APP_LOGI("MainAbility::OnBackPressed");
    Ability::OnBackPressed();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_ON_BACK_PRESSED, 0, "");
}

void MainAbility::OnNewWant(const Want &want)
{
    APP_LOGI("MainAbility::OnNewWant");
    Ability::OnNewWant(want);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_ON_NEW_WANT, 0, want.ToUri());
}

void MainAbility::SubscribeEvent()
{
    std::vector<std::string> eventList = {
        g_EVENT_REQU_FIRST,
    };
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber = std::make_shared<FirstEventSubscriber>(subscribeInfo, this);
    CommonEventManager::SubscribeCommonEvent(subscriber);
}

void FirstEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("FirstEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("FirstEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("FirstEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (std::strcmp(eventName.c_str(), g_EVENT_REQU_FIRST.c_str()) == 0) {
        auto target = data.GetData();
        auto handle = 0;
        auto api = 1;
        auto code = 2;
        auto caseInfo = TestUtils::split(target, "_");
        auto paramMinSize = 3;
        if (caseInfo.size() < static_cast<unsigned int>(paramMinSize)) {
            return;
        }
        if (mapTestFunc_.find(caseInfo[handle]) != mapTestFunc_.end()) {
            mapTestFunc_[caseInfo[handle]](std::stoi(caseInfo[api]), std::stoi(caseInfo[code]), data.GetCode());
        } else {
            APP_LOGI("OnReceiveEvent: CommonEventData error(%{public}s)", target.c_str());
        }
    }
}

void MainAbility::TestAbility(int apiIndex, int caseIndex, int code)
{
    APP_LOGI("MainAbility::TestAbility");
    if (mapCase_.find(apiIndex) != mapCase_.end()) {
        if (caseIndex < (int)mapCase_[apiIndex].size()) {
            mapCase_[apiIndex][caseIndex](code);
        }
    }
}

// get current ability name
void MainAbility::GetAbilityNameCase1(int code)
{
    std::string abilityName = Ability::GetAbilityName();
    bool result = abilityName == "MainAbility";
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, std::to_string(result));
}

// get AbilityPackage object
void MainAbility::GetAbilityPackageCase1(int code)
{
    auto abilityPackage = Ability::GetAbilityPackage();
    bool result = abilityPackage != nullptr;
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, std::to_string(result));
}

// get want from empty Want
void MainAbility::GetWantCase1(int code)
{
    Want want;
    auto getWant = Ability::GetWant();
    bool result = getWant == nullptr;
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, std::to_string(result));
}

// set and get want
void MainAbility::GetWantCase2(int code)
{
    std::string action = "action";
    Want want;
    Want setWant;
    setWant.SetAction(action);
    Ability::SetWant(setWant);
    bool result = Ability::GetWant()->GetAction() == action;
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, std::to_string(result));
}

// get want repeatedly
void MainAbility::GetWantCase3(int code)
{
    std::string action = "action";
    Want want;
    bool result = true;
    std::string tmpAction;
    for (int i = 0; i < pressureTimes; i++) {
        Want setWant;
        tmpAction = action + std::to_string(i);
        setWant.SetAction(tmpAction);
        Ability::SetWant(setWant);
        result = result && Ability::GetWant()->GetAction() == tmpAction;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, std::to_string(result));
}

void MainAbility::GetWindowCase1(int code)
{
    APP_LOGI("MainAbility::GetWindowCase1");
    bool result = true;
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, std::to_string(result));
}

void MainAbility::DumpCase1(int code)
{
    APP_LOGI("MainAbility::DumpCase1");
    std::string dumpInfo;
    Ability::Dump(dumpInfo);
    bool result = true;
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, std::to_string(result));
}

// set empty Want
void MainAbility::SetWantCase1(int code)
{
    std::string empty;
    Want want;
    Want setWant;
    Ability::SetWant(setWant);
    bool result = Ability::GetWant()->GetAction() == empty;
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, std::to_string(result));
}

// set want repeatedly
void MainAbility::SetWantCase2(int code)
{
    std::string action = "action";
    Want want;
    std::string tmpAction;
    for (int i = 0; i < pressureTimes; i++) {
        Want setWant;
        tmpAction = action + std::to_string(i);
        setWant.SetAction(tmpAction);
        Ability::SetWant(setWant);
    }
    bool result = Ability::GetWant()->GetAction() == tmpAction;
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, std::to_string(result));
}

REGISTER_AA(MainAbility)
}  // namespace AppExecFwk
}  // namespace OHOS
