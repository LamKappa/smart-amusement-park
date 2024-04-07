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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_HAP_MODULE_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_HAP_MODULE_INFO_H

#include <string>

#include "parcel.h"
#include "ability_info.h"

namespace OHOS {
namespace AppExecFwk {

// configuration information about an module
struct HapModuleInfo : public Parcelable {
    std::string name;        // module.package in config.json
    std::string moduleName;  // module.name in config.json
    std::string description;
    std::string iconPath;
    std::string label;
    std::string backgroundImg;
    int supportedModes = 0;

    std::vector<std::string> reqCapabilities;
    std::vector<std::string> deviceTypes;
    std::vector<AbilityInfo> abilityInfos;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static HapModuleInfo *Unmarshalling(Parcel &parcel);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_HAP_MODULE_INFO_H
