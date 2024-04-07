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

const int MainAbilityBCode = 300;

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
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB_LIFECYCLE, MainAbilityBCode, "onStart");
}

void MainAbility::OnStop()
{
    APP_LOGI("MainAbility::OnStop");
    Ability::OnStop();
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB_LIFECYCLE, MainAbilityBCode, "OnStop");
}

void MainAbility::OnActive()
{
    APP_LOGI("MainAbility::OnActive");
    Ability::OnActive();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB_LIFECYCLE, MainAbilityBCode, "OnActive");
}

void MainAbility::OnInactive()
{
    APP_LOGI("MainAbility::OnInactive");
    Ability::OnInactive();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB_LIFECYCLE, MainAbilityBCode, "OnInactive");
}

void MainAbility::OnBackground()
{
    APP_LOGI("MainAbility::OnBackground");
    Ability::OnBackground();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB_LIFECYCLE, MainAbilityBCode, "OnBackground");
}

void MainAbility::OnForeground(const Want &want)
{
    APP_LOGI("MainAbility::OnForeground");
    Ability::OnForeground(want);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB_LIFECYCLE, MainAbilityBCode, "OnForeground");
}

void MainAbility::OnAbilityResult(int requestCode, int resultCode, const Want &resultData)
{
    APP_LOGI("MainAbility::OnAbilityResult");
    Ability::OnAbilityResult(requestCode, resultCode, resultData);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB_LIFECYCLE, 0, resultData.ToUri());
}

void MainAbility::OnBackPressed()
{
    APP_LOGI("MainAbility::OnBackPressed");
    Ability::OnBackPressed();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB_LIFECYCLE, 0, "");
}

void MainAbility::OnNewWant(const Want &want)
{
    APP_LOGI("MainAbility::OnNewWant");
    Ability::OnNewWant(want);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB_LIFECYCLE, 0, want.ToUri());
}

MainAbility::~MainAbility()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void MainAbility::SubscribeEvent()
{
    std::vector<std::string> eventList = {
        g_EVENT_REQU_FIRSTB,
    };
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<FirstEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility = this;
    CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void FirstEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("FirstEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("FirstEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("FirstEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (std::strcmp(eventName.c_str(), g_EVENT_REQU_FIRSTB.c_str()) == 0) {
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

void MainAbility::TestAbility(int apiIndex, int caseIndex, int code)
{
    APP_LOGI("MainAbility::TestAbility");
    if (mapCase_.find(apiIndex) != mapCase_.end()) {
        if (caseIndex < (int)mapCase_[apiIndex].size()) {
            mapCase_[apiIndex][caseIndex](code);
        }
    }
}

void MainAbility::GetApplicationInfoCase1(int code)
{
    return;
}
void MainAbility::GetApplicationInfoCase2(int code)
{
    return;
}
void MainAbility::GetApplicationInfoCase3(int code)
{
    auto appInfo = AbilityContext::GetApplicationInfo();
    string appInfoStr = "";
    string result = "";
    if (appInfo != nullptr) {
        appInfoStr = TestUtils::ApplicationInfoToString(appInfo);
        result = appInfo->name;
    }

    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, result);
}

void MainAbility::GetCacheDirCase1(int code)
{
    return;
}
void MainAbility::GetCacheDirCase2(int code)
{
    return;
}
void MainAbility::GetCacheDirCase3(int code)
{
    string cacheDir = AbilityContext::GetCacheDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, cacheDir);
}

void MainAbility::GetDatabaseDirCase1(int code)
{
    return;
}
void MainAbility::GetDatabaseDirCase2(int code)
{
    return;
}
void MainAbility::GetDatabaseDirCase3(int code)
{
    string databaseDir = AbilityContext::GetDatabaseDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, databaseDir);
}

void MainAbility::GetDataDirCase1(int code)
{
    return;
}

void MainAbility::GetDataDirCase2(int code)
{
    return;
}

void MainAbility::GetDataDirCase3(int code)
{
    string dataDir = AbilityContext::GetDataDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, dataDir);
}

void MainAbility::GetDirCase1(int code)
{
    return;
}

void MainAbility::GetDirCase2(int code)
{
    return;
}

void MainAbility::GetDirCase3(int code)
{
    string name = "getDir";
    string dir = AbilityContext::GetDir(name, 0);
    dir = AbilityContext::GetDir(name, 0);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, dir);
}

void MainAbility::GetNoBackupFilesDirCase1(int code)
{
    return;
}

void MainAbility::GetBundleManagerCase1(int code)
{
    return;
}

void MainAbility::VerifyCallingPermissionCase1(int code)
{
    return;
}

void MainAbility::VerifyPermissionCase1(int code)
{
    return;
}

void MainAbility::VerifySelfPermissionCase1(int code)
{
    return;
}

void MainAbility::GetBundleCodePathCase1(int code)
{
    return;
}

void MainAbility::GetBundleCodePathCase2(int code)
{
    return;
}

void MainAbility::GetBundleCodePathCase3(int code)
{
    string bundleCodePath = AbilityContext::GetBundleCodePath();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, bundleCodePath);
}

void MainAbility::GetBundleNameCase1(int code)
{
    return;
}

void MainAbility::GetBundleNameCase2(int code)
{
    return;
}

void MainAbility::GetBundleNameCase3(int code)
{
    string bundleName = AbilityContext::GetBundleName();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, bundleName);
    return;
}

void MainAbility::GetBundleResourcePathCase1(int code)
{
    return;
}

void MainAbility::GetBundleResourcePathCase2(int code)
{
    return;
}

void MainAbility::GetBundleResourcePathCase3(int code)
{
    string bundleResourcePath = AbilityContext::GetBundleResourcePath();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, bundleResourcePath);
}

void MainAbility::CanRequestPermissionCase1(int code)
{
    return;
}

void MainAbility::GetCallingAbilityCase1(int code)
{
    return;
}

void MainAbility::GetCallingAbilityCase2(int code)
{
    auto callingAbility = AbilityContext::GetCallingAbility();
    string result = "";
    if (callingAbility != nullptr)
        result = callingAbility->GetURI();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, result);
    return;
}

void MainAbility::GetCallingAbilityCase3(int code)
{
    string targetBundle = "com.ohos.amsst.AppKitA";
    string targetAbility = "SecondAbility";
    Want want;
    want.SetElementName(targetBundle, targetAbility);
    want.SetParam("operator", 0);
    StartAbility(want);
}

void MainAbility::GetContextCase1(int code)
{
    return;
}

void MainAbility::GetAbilityManagerCase1(int code)
{
    return;
}

void MainAbility::GetProcessInfoCase1(int code)
{
    return;
}

void MainAbility::GetProcessInfoCase2(int code)
{
    return;
}

void MainAbility::GetProcessInfoCase3(int code)
{
    auto processInfo = AbilityContext::GetProcessInfo();
    string processInfoStr = "";
    string result = "";
    if (processInfo != nullptr) {
        processInfoStr = TestUtils::ProcessInfoToString(processInfo);
        result = processInfo->GetProcessName();
    }

    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, result);
}

void MainAbility::GetAppTypeCase1(int code)
{
    return;
}

void MainAbility::GetAppTypeCase2(int code)
{
    return;
}

void MainAbility::GetAppTypeCase3(int code)
{
    string appType = AbilityContext::GetAppType();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, appType);
}

void MainAbility::GetCallingBundleCase1(int code)
{
    return;
}

void MainAbility::GetCallingBundleCase2(int code)
{
    string callingBundle = AbilityContext::GetCallingBundle();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, callingBundle);
}

void MainAbility::GetCallingBundleCase3(int code)
{
    string targetBundle = "com.ohos.amsst.AppKitA";
    string targetAbility = "SecondAbility";
    Want want;
    want.SetElementName(targetBundle, targetAbility);
    want.SetParam("operator", 0);
    StartAbility(want);
}

void MainAbility::StartAbilityCase1(int code)
{
    return;
}

void MainAbility::StartAbilityCase2(int code)
{
    return;
}

void MainAbility::TerminateAbilityCase1(int code)
{
    return;
}

void MainAbility::GetElementNameCase1(int code)
{
    return;
}

void MainAbility::GetElementNameCase2(int code)
{
    return;
}

void MainAbility::GetElementNameCase3(int code)
{
    auto elementName = AbilityContext::GetElementName();
    string result = "";
    if (elementName != nullptr)
        result = elementName->GetBundleName();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, result);
}

void MainAbility::GetHapModuleInfoCase1(int code)
{
    return;
}

void MainAbility::GetHapModuleInfoCase2(int code)
{
    return;
}

void MainAbility::GetHapModuleInfoCase3(int code)
{
    auto elementName = AbilityContext::GetHapModuleInfo();
    string result = "";
    if (elementName != nullptr)
        result = elementName->moduleName;
    TestUtils::PublishEvent(g_EVENT_RESP_FIRSTB, code, result);
}

void MainAbility::GetCodeCacheDirCase1(int code)
{}
void MainAbility::GetCodeCacheDirCase2(int code)
{}
void MainAbility::GetCodeCacheDirCase3(int code)
{
    string codeCacheDir = AbilityContext::GetCodeCacheDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, codeCacheDir);
}

void MainAbility::GetApplicationContextCase1(int code)
{}

REGISTER_AA(MainAbility)
}  // namespace AppExecFwk
}  // namespace OHOS
