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

#include "mock_bundle_mgr.h"
#include "ability_info.h"
#include "application_info.h"
#include "hilog_wrapper.h"
#include "ability_config.h"
using namespace OHOS::AAFwk;

namespace OHOS {
namespace AppExecFwk {

int BundleMgrProxy::QueryWantAbility(
    const AAFwk::Want &__attribute__((unused)) want, std::vector<AbilityInfo> &__attribute__((unused)) abilityInfos)
{
    return 0;
}

bool BundleMgrProxy::QueryAbilityInfo(const AAFwk::Want &want, AbilityInfo &abilityInfo)
{
    ElementName eleName = want.GetElement();
    if (eleName.GetBundleName().empty()) {
        return false;
    }
    abilityInfo.name = eleName.GetAbilityName();
    abilityInfo.bundleName = eleName.GetBundleName();
    abilityInfo.applicationName = eleName.GetAbilityName() + "App";
    if (abilityInfo.bundleName != "com.ix.hiworld") {
        abilityInfo.applicationInfo.isLauncherApp = false;
    }
    return true;
}

bool BundleMgrProxy::GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo)
{
    return true;
}

bool BundleMgrProxy::QueryAbilityInfoByUri(const std::string &uri, AbilityInfo &abilityInfo)
{
    return false;
}

bool BundleMgrProxy::GetApplicationInfo(
    const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo)
{
    if (appName.empty()) {
        return false;
    }
    appInfo.name = "Helloworld";
    appInfo.bundleName = "com.ix.hiworld";
    return true;
}

int BundleMgrStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    return 0;
}

bool BundleMgrService::GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo)
{
    return true;
}

bool BundleMgrService::QueryAbilityInfo(const AAFwk::Want &want, AbilityInfo &abilityInfo)
{
    if (CheckWantEntity(want, abilityInfo)) {
        return true;
    }

    ElementName elementTemp = want.GetElement();
    std::string abilityNameTemp = elementTemp.GetAbilityName();
    std::string bundleNameTemp = elementTemp.GetBundleName();
    abilityInfo.deviceId = elementTemp.GetDeviceID();

    if (bundleNameTemp.empty() || abilityNameTemp.empty()) {
        return false;
    }

    auto fun = abilityInfoMap_.find(bundleNameTemp);
    if (fun != abilityInfoMap_.end()) {
        auto call = fun->second;
        if (call) {
            call(bundleNameTemp, abilityInfo, elementTemp);
            return true;
        }
    }
    if (std::string::npos != elementTemp.GetBundleName().find("service")) {
        abilityInfo.type = AppExecFwk::AbilityType::SERVICE;
    }
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = elementTemp.GetBundleName();
    abilityInfo.deviceId = elementTemp.GetDeviceID();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.name = "hello";
    if (want.HasEntity(Want::ENTITY_HOME) && want.GetAction() == Want::ACTION_HOME) {
        abilityInfo.applicationInfo.isLauncherApp = true;
    } else {
        abilityInfo.applicationInfo.isLauncherApp = false;
        abilityInfo.applicationInfo.iconPath = "icon path";
        abilityInfo.applicationInfo.label = "app label";
    }
    return true;
}

bool BundleMgrService::GetApplicationInfo(
    const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo)
{
    if (appName.empty()) {
        return false;
    }
    return true;
}

bool BundleMgrService::CheckWantEntity(const AAFwk::Want &want, AbilityInfo &abilityInfo)
{
    auto entityVector = want.GetEntities();
    ElementName element = want.GetElement();
    if (entityVector.empty()) {
        return false;
    }

    auto find = false;
    // filter ams onstart
    for (const auto &entity : entityVector) {
        if (entity == Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM && element.GetAbilityName().empty() &&
            element.GetBundleName().empty()) {
            find = true;
            break;
        }
    }

    auto bundleName = element.GetBundleName();
    auto abilityName = element.GetAbilityName();
    if (find || (bundleName == AbilityConfig::SYSTEM_UI_BUNDLE_NAME &&
                    (abilityName == AbilityConfig::SYSTEM_UI_STATUS_BAR ||
                        abilityName == AbilityConfig::SYSTEM_UI_NAVIGATION_BAR))) {
        GTEST_LOG_(INFO) << "QueryAbilityInfo ++> system luncher, find :" << find;
        return true;
    }

    return false;
}

}  // namespace AppExecFwk
}  // namespace OHOS
