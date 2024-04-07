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

#include "second_ability.h"
#include "app_log_wrapper.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

const int SecondAbilityACode = 200;

void SecondAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("SecondAbility::Init");
    Ability::Init(abilityInfo, application, handler, token);
}

SecondAbility::~SecondAbility()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void SecondAbility::OnStart(const Want &want)
{
    APP_LOGI("SecondAbility::onStart");
    SubscribeEvent();
    Ability::OnStart(want);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND_LIFECYCLE, SecondAbilityACode, "onStart");
}

void SecondAbility::OnStop()
{
    APP_LOGI("SecondAbility::OnStop");
    Ability::OnStop();
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND_LIFECYCLE, SecondAbilityACode, "OnStop");
}

void SecondAbility::OnActive()
{
    APP_LOGI("SecondAbility::OnActive");
    Ability::OnActive();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND_LIFECYCLE, SecondAbilityACode, "OnActive");
}

void SecondAbility::OnInactive()
{
    APP_LOGI("SecondAbility::OnInactive");
    Ability::OnInactive();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND_LIFECYCLE, SecondAbilityACode, "OnInactive");
}

void SecondAbility::OnBackground()
{
    APP_LOGI("SecondAbility::OnBackground");
    Ability::OnBackground();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND_LIFECYCLE, SecondAbilityACode, "OnBackground");
}

void SecondAbility::OnForeground(const Want &want)
{
    APP_LOGI("SecondAbility::OnForeground");
    Ability::OnForeground(want);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND_LIFECYCLE, SecondAbilityACode, "OnForeground");
}

void SecondAbility::OnAbilityResult(int requestCode, int resultCode, const Want &resultData)
{
    APP_LOGI("SecondAbility::OnAbilityResult");
    Ability::OnAbilityResult(requestCode, resultCode, resultData);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND_LIFECYCLE, 0, resultData.ToUri());
}

void SecondAbility::OnBackPressed()
{
    APP_LOGI("SecondAbility::OnBackPressed");
    Ability::OnBackPressed();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND_LIFECYCLE, 0, "");
}

void SecondAbility::OnNewWant(const Want &want)
{
    APP_LOGI("SecondAbility::OnNewWant");
    Ability::OnNewWant(want);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND_LIFECYCLE, 0, want.ToUri());
}

void SecondAbility::SubscribeEvent()
{
    std::vector<std::string> eventList = {
        g_EVENT_REQU_SECOND,
    };
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<SecondEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility = this;
    CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void SecondEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("SecondEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("SecondEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("SecondEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (std::strcmp(eventName.c_str(), g_EVENT_REQU_SECOND.c_str()) == 0) {
        auto target = data.GetData();
        auto caseInfo = TestUtils::split(target, "_");
        if (caseInfo.size() < 3) {
            return;
        }
        if (mapTestFunc_.find(caseInfo[0]) != mapTestFunc_.end()) {
            mapTestFunc_[caseInfo[0]](std::stoi(caseInfo[1]), std::stoi(caseInfo[2]), data.GetCode());
        } else {
            APP_LOGI("OnReceiveEvent: CommonEventData error(%{public}s)", target.c_str());
        }
    }
}

void SecondAbility::TestAbility(int apiIndex, int caseIndex, int code)
{
    APP_LOGI("SecondAbility::TestAbility");
    if (mapCase_.find(apiIndex) != mapCase_.end()) {
        if (caseIndex < (int)mapCase_[apiIndex].size()) {
            mapCase_[apiIndex][caseIndex](code);
        }
    }
}

void SecondAbility::GetApplicationInfoCase1(int code)
{}
void SecondAbility::GetApplicationInfoCase2(int code)
{
    auto appInfo = AbilityContext::GetApplicationInfo();
    string appInfoStr = "";
    string result = "";
    if (appInfo != nullptr) {
        appInfoStr = TestUtils::ApplicationInfoToString(appInfo);
        result = appInfo->name;
    }

    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, result);
}
void SecondAbility::GetApplicationInfoCase3(int code)
{}

void SecondAbility::GetCacheDirCase1(int code)
{}
void SecondAbility::GetCacheDirCase2(int code)
{
    string cacheDir = AbilityContext::GetCacheDir();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, cacheDir);
}
void SecondAbility::GetCacheDirCase3(int code)
{}

void SecondAbility::GetDatabaseDirCase1(int code)
{}
void SecondAbility::GetDatabaseDirCase2(int code)
{
    string databaseDir = AbilityContext::GetDatabaseDir();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, databaseDir);
}
void SecondAbility::GetDatabaseDirCase3(int code)
{}

void SecondAbility::GetDataDirCase1(int code)
{}

void SecondAbility::GetDataDirCase2(int code)
{
    string dataDir = AbilityContext::GetDataDir();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, dataDir);
}

void SecondAbility::GetDataDirCase3(int code)
{}

void SecondAbility::GetDirCase1(int code)
{}

void SecondAbility::GetDirCase2(int code)
{
    string name = "getDir";
    string dir = AbilityContext::GetDir(name, 0);
    dir = AbilityContext::GetDir(name, 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, dir);
}

void SecondAbility::GetDirCase3(int code)
{}

void SecondAbility::GetNoBackupFilesDirCase1(int code)
{}

void SecondAbility::GetBundleManagerCase1(int code)
{}

void SecondAbility::VerifyCallingPermissionCase1(int code)
{}

void SecondAbility::VerifyPermissionCase1(int code)
{}

void SecondAbility::VerifySelfPermissionCase1(int code)
{}

void SecondAbility::GetBundleCodePathCase1(int code)
{}

void SecondAbility::GetBundleCodePathCase2(int code)
{
    string bundleCodePath = AbilityContext::GetBundleCodePath();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, bundleCodePath);
}

void SecondAbility::GetBundleCodePathCase3(int code)
{}

void SecondAbility::GetBundleNameCase1(int code)
{}

void SecondAbility::GetBundleNameCase2(int code)
{
    string bundleName = AbilityContext::GetBundleName();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, bundleName);
}

void SecondAbility::GetBundleNameCase3(int code)
{}

void SecondAbility::GetBundleResourcePathCase1(int code)
{}

void SecondAbility::GetBundleResourcePathCase2(int code)
{
    string bundleResourcePath = AbilityContext::GetBundleResourcePath();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, bundleResourcePath);
}

void SecondAbility::GetBundleResourcePathCase3(int code)
{}

void SecondAbility::CanRequestPermissionCase1(int code)
{}

void SecondAbility::GetCallingAbilityCase1(int code)
{
    auto callingAbility = AbilityContext::GetCallingAbility();
    string result = "";
    if (callingAbility != nullptr)
        result = callingAbility->GetURI();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, result);
}

void SecondAbility::GetCallingAbilityCase2(int code)
{
    auto callingAbility = AbilityContext::GetCallingAbility();
    string result = "";
    if (callingAbility != nullptr)
        result = callingAbility->GetURI();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, result);
}

void SecondAbility::GetCallingAbilityCase3(int code)
{
    auto callingAbility = AbilityContext::GetCallingAbility();
    string result = "";
    if (callingAbility != nullptr && (callingTime == 0 || callingTime == code)) {
        callingTime = code;
        result = callingAbility->GetURI();
        TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, result);
    }
}

void SecondAbility::GetContextCase1(int code)
{}

void SecondAbility::GetAbilityManagerCase1(int code)
{}

void SecondAbility::GetProcessInfoCase1(int code)
{}

void SecondAbility::GetProcessInfoCase2(int code)
{
    auto processInfo = AbilityContext::GetProcessInfo();
    string processInfoStr = "";
    string result = "";
    if (processInfo != nullptr) {
        processInfoStr = TestUtils::ProcessInfoToString(processInfo);
        result = processInfo->GetProcessName();
    }

    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, result);
}

void SecondAbility::GetProcessInfoCase3(int code)
{}

void SecondAbility::GetAppTypeCase1(int code)
{}

void SecondAbility::GetAppTypeCase2(int code)
{
    string appType = AbilityContext::GetAppType();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, appType);
}

void SecondAbility::GetAppTypeCase3(int code)
{}

void SecondAbility::GetCallingBundleCase1(int code)
{
    string callingBundle = AbilityContext::GetCallingBundle();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, callingBundle);
}

void SecondAbility::GetCallingBundleCase2(int code)
{
    string callingBundle = AbilityContext::GetCallingBundle();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, callingBundle);
}

void SecondAbility::GetCallingBundleCase3(int code)
{
    string callingBundle = "";
    if (callingTime == 0 || callingTime == code) {
        callingTime = code;
        callingBundle = AbilityContext::GetCallingBundle();
        TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, callingBundle);
    }
}

void SecondAbility::StartAbilityCase1(int code)
{}

void SecondAbility::StartAbilityCase2(int code)
{}

void SecondAbility::TerminateAbilityCase1(int code)
{}

void SecondAbility::GetElementNameCase1(int code)
{}

void SecondAbility::GetElementNameCase2(int code)
{
    auto elementName = AbilityContext::GetElementName();
    string result = "";
    if (elementName != nullptr)
        result = elementName->GetBundleName();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, result);
}

void SecondAbility::GetElementNameCase3(int code)
{}

void SecondAbility::GetHapModuleInfoCase1(int code)
{}

void SecondAbility::GetHapModuleInfoCase2(int code)
{
    auto elementName = AbilityContext::GetHapModuleInfo();
    string result = "";
    if (elementName != nullptr)
        result = elementName->moduleName;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, result);
}

void SecondAbility::GetHapModuleInfoCase3(int code)
{}

void SecondAbility::GetCodeCacheDirCase1(int code)
{}
void SecondAbility::GetCodeCacheDirCase2(int code)
{
    string codeCacheDir = AbilityContext::GetCodeCacheDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, codeCacheDir);
}
void SecondAbility::GetCodeCacheDirCase3(int code)
{}

void SecondAbility::GetApplicationContextCase1(int code)
{}

REGISTER_AA(SecondAbility)
}  // namespace AppExecFwk
}  // namespace OHOS
