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

const int MainAbilityACode = 100;

void MainAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("MainAbility::Init");
    Ability::Init(abilityInfo, application, handler, token);
}

MainAbility::~MainAbility()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void MainAbility::OnStart(const Want &want)
{
    APP_LOGI("MainAbility::onStart");
    SubscribeEvent();
    Ability::OnStart(want);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_LIFECYCLE, MainAbilityACode, "onStart");
}

void MainAbility::OnStop()
{
    APP_LOGI("MainAbility::OnStop");
    Ability::OnStop();
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_LIFECYCLE, MainAbilityACode, "OnStop");
}

void MainAbility::OnActive()
{
    APP_LOGI("MainAbility::OnActive");
    Ability::OnActive();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_LIFECYCLE, MainAbilityACode, "OnActive");
}

void MainAbility::OnInactive()
{
    APP_LOGI("MainAbility::OnInactive");
    Ability::OnInactive();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_LIFECYCLE, MainAbilityACode, "OnInactive");
}

void MainAbility::OnBackground()
{
    APP_LOGI("MainAbility::OnBackground");
    Ability::OnBackground();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_LIFECYCLE, MainAbilityACode, "OnBackground");
}

void MainAbility::OnForeground(const Want &want)
{
    APP_LOGI("MainAbility::OnForeground");
    Ability::OnForeground(want);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST_LIFECYCLE, MainAbilityACode, "OnForeground");
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
    if (std::strcmp(eventName.c_str(), g_EVENT_REQU_FIRST.c_str()) == 0) {
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
    auto appInfo = AbilityContext::GetApplicationInfo();
    string appInfoStr = "";
    string result = "";
    if (appInfo != nullptr) {
        appInfoStr = TestUtils::ApplicationInfoToString(appInfo);
        result = appInfo->name;
    }

    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}
void MainAbility::GetApplicationInfoCase2(int code)
{
    auto appInfo = AbilityContext::GetApplicationInfo();
    string appInfoStr = "";
    string result = "";
    if (appInfo != nullptr) {
        appInfoStr = TestUtils::ApplicationInfoToString(appInfo);
        result = appInfo->name;
    }

    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
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

    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}

void MainAbility::GetCacheDirCase1(int code)
{
    string cacheDir = AbilityContext::GetCacheDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, cacheDir);
}
void MainAbility::GetCacheDirCase2(int code)
{
    string cacheDir = AbilityContext::GetCacheDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, cacheDir);
}
void MainAbility::GetCacheDirCase3(int code)
{
    string cacheDir = AbilityContext::GetCacheDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, cacheDir);
}

void MainAbility::GetDatabaseDirCase1(int code)
{
    string databaseDir = AbilityContext::GetDatabaseDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, databaseDir);
}
void MainAbility::GetDatabaseDirCase2(int code)
{
    string databaseDir = AbilityContext::GetDatabaseDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, databaseDir);
}
void MainAbility::GetDatabaseDirCase3(int code)
{
    string databaseDir = AbilityContext::GetDatabaseDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, databaseDir);
}

void MainAbility::GetDataDirCase1(int code)
{
    string dataDir = AbilityContext::GetDataDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, dataDir);
}

void MainAbility::GetDataDirCase2(int code)
{
    string dataDir = AbilityContext::GetDataDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, dataDir);
}

void MainAbility::GetDataDirCase3(int code)
{
    string dataDir = AbilityContext::GetDataDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, dataDir);
}

void MainAbility::GetDirCase1(int code)
{
    string name = "getDir";
    string dir = AbilityContext::GetDir(name, 0);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, dir);
}

void MainAbility::GetDirCase2(int code)
{
    string name = "getDir";
    string dir = AbilityContext::GetDir(name, 0);
    dir = AbilityContext::GetDir(name, 0);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, dir);
}

void MainAbility::GetDirCase3(int code)
{
    string name = "getDir";
    string dir = AbilityContext::GetDir(name, 0);
    dir = AbilityContext::GetDir(name, 0);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, dir);
}

void MainAbility::GetNoBackupFilesDirCase1(int code)
{}

void MainAbility::GetBundleManagerCase1(int code)
{
    auto bundleManger = AbilityContext::GetBundleManager();
    int result = 1;
    if (bundleManger == nullptr)
        result = 0;
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, std::to_string(result));
}

void MainAbility::VerifyCallingPermissionCase1(int code)
{}

void MainAbility::VerifyPermissionCase1(int code)
{}

void MainAbility::VerifySelfPermissionCase1(int code)
{}

void MainAbility::GetBundleCodePathCase1(int code)
{
    string bundleCodePath = AbilityContext::GetBundleCodePath();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, bundleCodePath);
}

void MainAbility::GetBundleCodePathCase2(int code)
{
    string bundleCodePath = AbilityContext::GetBundleCodePath();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, bundleCodePath);
}

void MainAbility::GetBundleCodePathCase3(int code)
{
    string bundleCodePath = AbilityContext::GetBundleCodePath();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, bundleCodePath);
}

void MainAbility::GetBundleNameCase1(int code)
{
    string bundleName = AbilityContext::GetBundleName();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, bundleName);
}

void MainAbility::GetBundleNameCase2(int code)
{
    string bundleName = AbilityContext::GetBundleName();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, bundleName);
}

void MainAbility::GetBundleNameCase3(int code)
{
    string bundleName = AbilityContext::GetBundleName();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, bundleName);
}

void MainAbility::GetBundleResourcePathCase1(int code)
{
    string bundleResourcePath = AbilityContext::GetBundleResourcePath();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, bundleResourcePath);
}

void MainAbility::GetBundleResourcePathCase2(int code)
{
    string bundleResourcePath = AbilityContext::GetBundleResourcePath();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, bundleResourcePath);
}

void MainAbility::GetBundleResourcePathCase3(int code)
{
    string bundleResourcePath = AbilityContext::GetBundleResourcePath();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, bundleResourcePath);
}

void MainAbility::CanRequestPermissionCase1(int code)
{}

void MainAbility::GetCallingAbilityCase1(int code)
{
    string targetBundle = "com.ohos.amsst.AppKitA";
    string targetAbility = "SecondAbility";
    Want want;
    want.SetElementName(targetBundle, targetAbility);
    want.SetParam("operator", 0);
    StartAbility(want);
}

void MainAbility::GetCallingAbilityCase2(int code)
{
    string targetBundle1 = "com.ohos.amsst.AppKitA";
    string targetAbility1 = "SecondAbility";
    Want want;
    want.SetElementName(targetBundle1, targetAbility1);
    want.SetParam("operator", 0);
    StartAbility(want);

    string targetBundle2 = "com.ohos.amsst.AppKitB";
    string targetAbility2 = "MainAbility";
    want.SetElementName(targetBundle2, targetAbility2);
    want.SetParam("operator", 0);
    StartAbility(want);
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
    auto context = AbilityContext::GetContext();
    string result = "0";
    if (context != nullptr)
        result = "1";
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}

void MainAbility::GetAbilityManagerCase1(int code)
{
    auto abilityManger = AbilityContext::GetAbilityManager();
    string result = "0";
    if (abilityManger != nullptr)
        result = "1";
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}

void MainAbility::GetProcessInfoCase1(int code)
{
    auto processInfo = AbilityContext::GetProcessInfo();
    string processInfoStr = "";
    string result = "";
    if (processInfo != nullptr) {
        processInfoStr = TestUtils::ProcessInfoToString(processInfo);
        result = processInfo->GetProcessName();
    }

    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}

void MainAbility::GetProcessInfoCase2(int code)
{
    auto processInfo = AbilityContext::GetProcessInfo();
    string processInfoStr = "";
    string result = "";
    if (processInfo != nullptr) {
        processInfoStr = TestUtils::ProcessInfoToString(processInfo);
        result = processInfo->GetProcessName();
    }

    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
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

    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}

void MainAbility::GetAppTypeCase1(int code)
{
    string appType = AbilityContext::GetAppType();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, appType);
}

void MainAbility::GetAppTypeCase2(int code)
{
    string appType = AbilityContext::GetAppType();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, appType);
}

void MainAbility::GetAppTypeCase3(int code)
{
    string appType = AbilityContext::GetAppType();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, appType);
}

void MainAbility::GetCallingBundleCase1(int code)
{
    string targetBundle = "com.ohos.amsst.AppKitA";
    string targetAbility = "SecondAbility";
    Want want;
    want.SetElementName(targetBundle, targetAbility);
    want.SetParam("operator", 0);
    StartAbility(want);
}

void MainAbility::GetCallingBundleCase2(int code)
{
    string targetBundle1 = "com.ohos.amsst.AppKitA";
    string targetAbility1 = "SecondAbility";
    Want want;
    want.SetElementName(targetBundle1, targetAbility1);
    want.SetParam("operator", 0);
    StartAbility(want);

    string targetBundle2 = "com.ohos.amsst.AppKitB";
    string targetAbility2 = "MainAbility";
    want.SetElementName(targetBundle2, targetAbility2);
    want.SetParam("operator", 0);
    StartAbility(want);
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
    std::map<std::string, std::string> params;
    Want want = TestUtils::MakeWant("device", "SecondAbility", "com.ohos.amsst.AppKitA", params);
    AbilityContext::StartAbility(want, 1);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, "startAbility");
}

void MainAbility::StartAbilityCase2(int code)
{
    std::map<std::string, std::string> params;
    Want want = TestUtils::MakeWant("device", "MainAbility", "com.ohos.amsst.AppKitB", params);
    AbilityContext::StartAbility(want, 1);
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, "startAbility");
}

void MainAbility::TerminateAbilityCase1(int code)
{
    AbilityContext::TerminateAbility();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, "TerminateAbility");
}

void MainAbility::GetElementNameCase1(int code)
{
    auto elementName = AbilityContext::GetElementName();
    string result = "";
    if (elementName != nullptr)
        result = elementName->GetBundleName();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}

void MainAbility::GetElementNameCase2(int code)
{
    auto elementName = AbilityContext::GetElementName();
    string result = "";
    if (elementName != nullptr)
        result = elementName->GetBundleName();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}

void MainAbility::GetElementNameCase3(int code)
{
    auto elementName = AbilityContext::GetElementName();
    string result = "";
    if (elementName != nullptr)
        result = elementName->GetBundleName();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}

void MainAbility::GetHapModuleInfoCase1(int code)
{
    auto elementName = AbilityContext::GetHapModuleInfo();
    string result = "";
    if (elementName != nullptr)
        result = elementName->moduleName;
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}

void MainAbility::GetHapModuleInfoCase2(int code)
{
    auto elementName = AbilityContext::GetHapModuleInfo();
    string result = "";
    if (elementName != nullptr)
        result = elementName->moduleName;
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}

void MainAbility::GetHapModuleInfoCase3(int code)
{
    auto elementName = AbilityContext::GetHapModuleInfo();
    string result = "";
    if (elementName != nullptr)
        result = elementName->moduleName;
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}

void MainAbility::GetCodeCacheDirCase1(int code)
{
    string codeCacheDir = AbilityContext::GetCodeCacheDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, codeCacheDir);
}
void MainAbility::GetCodeCacheDirCase2(int code)
{
    string codeCacheDir = AbilityContext::GetCodeCacheDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, codeCacheDir);
}
void MainAbility::GetCodeCacheDirCase3(int code)
{
    string codeCacheDir = AbilityContext::GetCodeCacheDir();
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, codeCacheDir);
}

void MainAbility::GetApplicationContextCase1(int code)
{
    auto elementName = AbilityContext::GetApplicationContext();
    string result = "";
    if (elementName != nullptr)
        result = "1";
    TestUtils::PublishEvent(g_EVENT_RESP_FIRST, code, result);
}

REGISTER_AA(MainAbility)
}  // namespace AppExecFwk
}  // namespace OHOS
