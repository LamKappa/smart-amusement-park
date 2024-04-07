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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_INFO_H

#include <string>
#include <vector>

#include "parcel.h"

#include "ability_info.h"
#include "application_info.h"

namespace OHOS {
namespace AppExecFwk {

enum class BundleFlag {
    // get bundle info except abilityInfos
    GET_BUNDLE_DEFAULT = 0x00000000,
    // get bundle info include abilityInfos
    GET_BUNDLE_WITH_ABILITIES = 0x00000001,
};

// configuration information about a bundle
struct BundleInfo : public Parcelable {
    std::string name;         // bundle name
    std::string label;        // name display on screen
    std::string description;  // detail description. When ResourceKit ready replace with descriptionId
    std::string vendor;
    uint32_t versionCode = 0;
    std::string versionName;
    std::string jointUserId;
    int32_t minSdkVersion = -1;  // The min SDK version this app can run on
    int32_t maxSdkVersion = -1;  // The max SDK version this app can run on
    std::string mainEntry;       // entry is path of ability main executable file
    std::string cpuAbi;
    std::string appId;
    int compatibleVersion = 0;
    int targetVersion = 0;
    std::string releaseType;
    int uid = -1;
    int gid = -1;
    std::string seInfo;
    std::string entryModuleName;
    bool isKeepAlive = false;
    bool isNativeApp = false;
    int64_t installTime = 0;    // the installation time is the number of seconds elapsed since January 1,
                                // 1970 00:00:00 UTC. The time will be recalculated if the application is reinstalled
                                // after being uninstalled.
    int64_t updateTime = 0;     // the update time is the number of seconds elapsed since January 1,
                                // 1970 00:00:00 UTC. If the application is installed for the first time, the application
                                // update time is the same as the installation time.
    ApplicationInfo applicationInfo;
    std::vector<AbilityInfo> abilityInfos;
    std::vector<std::string> reqPermissions;
    std::vector<std::string> defPermissions;    // the permissions required for accessing the application.
    std::vector<std::string> hapModuleNames;    // the "module.package" in each config.json
    std::vector<std::string> moduleNames;       // the "module.name" in each config.json
    std::vector<std::string> modulePublicDirs;  // the public paths of all modules of the application.
    std::vector<std::string> moduleDirs;        // the paths of all modules of the application.
    std::vector<std::string> moduleResPaths;    // the paths of all resources paths.

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static BundleInfo *Unmarshalling(Parcel &parcel);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_INFO_H
