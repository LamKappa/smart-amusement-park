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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_ABILITY_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_ABILITY_INFO_H

#include <string>

#include "parcel.h"
#include "application_info.h"

namespace OHOS {
namespace AppExecFwk {

enum class AbilityType {
    UNKNOWN = 0,
    PAGE,
    SERVICE,
    DATA,
};

enum class DisplayOrientation {
    UNSPECIFIED = 0,
    LANDSCAPE,
    PORTRAIT,
    FOLLOWRECENT,
};

enum class LaunchMode {
    SINGLETON = 0,
    SINGLETOP,
    STANDARD,  // support more than one instance
};

// configuration information about an ability
struct AbilityInfo : public Parcelable {
    std::string name;  // ability name, only the main class name
    std::string label;
    std::string description;
    std::string iconPath;
    bool visible = false;
    std::string kind;  // ability category
    AbilityType type = AbilityType::UNKNOWN;
    DisplayOrientation orientation = DisplayOrientation::UNSPECIFIED;
    LaunchMode launchMode = LaunchMode::STANDARD;
    std::vector<std::string> permissions;

    std::string process;
    std::vector<std::string> deviceTypes;
    std::vector<std::string> deviceCapabilities;
    std::string uri;
    ApplicationInfo applicationInfo;
    bool isLauncherAbility = false;
    bool isNativeAbility = false;

    // set when install
    std::string package;  // the "module.package" in config.json
    std::string bundleName;
    std::string moduleName;       // the "module.name" in config.json
    std::string applicationName;  // the "bundlename" in config.json
    std::string deviceId;         // should auto-get self device id
    std::string codePath;         // ability main code path with name
    std::string resourcePath;     // resource path for resource init
    std::string libPath;          // ability library path without name, libPath->libDir

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static AbilityInfo *Unmarshalling(Parcel &parcel);
    void Dump(std::string prefix, int fd);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_ABILITY_INFO_H
