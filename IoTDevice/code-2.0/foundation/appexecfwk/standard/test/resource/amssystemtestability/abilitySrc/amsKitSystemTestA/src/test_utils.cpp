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
#include "test_utils.h"
#include <iostream>
#include <numeric>
#include <sstream>
#include <regex>
#include "common_event_data.h"
#include "common_event_manager.h"
namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;
bool TestUtils::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    Want want;
    want.SetAction(eventName);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}

Want TestUtils::MakeWant(
    std::string deviceId, std::string abilityName, std::string bundleName, std::map<std::string, std::string> params)
{
    ElementName element(deviceId, bundleName, abilityName);
    Want want;
    want.SetElement(element);
    for (const auto &param : params) {
        want.SetParam(param.first, param.second);
    }
    return want;
}

std::string TestUtils::ApplicationInfoToString(std::shared_ptr<ApplicationInfo> applicationInfo)
{
    std::stringstream sstream;
    sstream << "name:" << applicationInfo->name << ",";
    sstream << "bundleName:" << applicationInfo->bundleName << ",";
    sstream << "description:" << applicationInfo->description << ",";
    sstream << "iconPath:" << applicationInfo->iconPath << ",";
    sstream << "label:" << applicationInfo->label << ",";
    sstream << "deviceId:" << applicationInfo->deviceId << ",";
    sstream << "signatureKey:" << applicationInfo->signatureKey << ",";
    sstream << "isSystemApp:" << std::boolalpha << applicationInfo->isSystemApp << ",";
    sstream << "isLauncherApp:" << applicationInfo->isLauncherApp << ",";
    sstream << "supportedModes:" << applicationInfo->supportedModes << ",";
    sstream << "process:" << applicationInfo->process << ",";
    std::string permissions =
        std::accumulate(applicationInfo->permissions.begin(), applicationInfo->permissions.end(), std::string(","));
    sstream << "permissions:" << permissions << ",";
    std::string moduleSourceDirs = std::accumulate(
        applicationInfo->moduleSourceDirs.begin(), applicationInfo->moduleSourceDirs.end(), std::string(","));
    sstream << "moduleSourceDirs:" << moduleSourceDirs << ",";
    sstream << "entryDir:" << applicationInfo->entryDir << ",";
    sstream << "codePath:" << applicationInfo->codePath << ",";
    sstream << "dataDir:" << applicationInfo->dataDir << ",";
    sstream << "dataBaseDir:" << applicationInfo->dataBaseDir << ",";
    sstream << "cacheDir:" << applicationInfo->cacheDir;
    return sstream.str();
}

std::string TestUtils::AbilityInfoToString(std::shared_ptr<AbilityInfo> abilityInfo)
{
    std::stringstream sstream;
    sstream << "name:" << abilityInfo->name << ",";
    sstream << "label:" << abilityInfo->label << ",";
    sstream << "description:" << abilityInfo->description << ",";
    sstream << "iconPath:" << abilityInfo->iconPath << ",";
    sstream << "visible:" << std::boolalpha << abilityInfo->visible << ",";
    sstream << "kind:" << abilityInfo->kind << ",";
    auto type = static_cast<std::underlying_type<AppExecFwk::AbilityType>::type>(abilityInfo->type);
    sstream << "type:" << type << ",";
    auto orientation =
        static_cast<std::underlying_type<AppExecFwk::DisplayOrientation>::type>(abilityInfo->orientation);
    sstream << "orientation:" << orientation << ",";
    auto launchMode = static_cast<std::underlying_type<AppExecFwk::LaunchMode>::type>(abilityInfo->launchMode);
    sstream << "launchMode:" << launchMode << ",";
    std::string permissions =
        std::accumulate(abilityInfo->permissions.begin(), abilityInfo->permissions.end(), std::string(","));
    sstream << "permissions:" << permissions << ",";
    sstream << "process:" << abilityInfo->process << ",";
    std::string deviceTypes =
        std::accumulate(abilityInfo->deviceTypes.begin(), abilityInfo->deviceTypes.end(), std::string(","));
    sstream << "deviceTypes:" << deviceTypes << ",";
    std::string deviceCapabilities = std::accumulate(
        abilityInfo->deviceCapabilities.begin(), abilityInfo->deviceCapabilities.end(), std::string(","));
    sstream << "deviceCapabilities:" << deviceCapabilities << ",";
    sstream << "uri:" << abilityInfo->uri << ",";
    sstream << "package:" << abilityInfo->package << ",";
    sstream << "bundleName:" << abilityInfo->bundleName << ",";
    sstream << "moduleName:" << abilityInfo->moduleName << ",";
    sstream << "applicationName:" << abilityInfo->applicationName << ",";
    sstream << "deviceId:" << abilityInfo->deviceId << ",";
    sstream << "codePath:" << abilityInfo->codePath << ",";
    sstream << "resourcePath:" << abilityInfo->resourcePath << ",";
    sstream << "libPath:" << abilityInfo->libPath;
    return sstream.str();
}

std::string TestUtils::ProcessInfoToString(std::shared_ptr<ProcessInfo> processInfo)
{
    std::stringstream sstream;
    sstream << "pid:" << processInfo->GetPid() << ",";
    sstream << "processName:" << processInfo->GetProcessName();
    return sstream.str();
}

std::vector<std::string> TestUtils::split(const std::string &in, const std::string &delim)
{
    std::regex reg{delim};
    return std::vector<std::string>{
        std::sregex_token_iterator(in.begin(), in.end(), reg, -1), std::sregex_token_iterator()};
}
}  // namespace AppExecFwk
}  // namespace OHOS