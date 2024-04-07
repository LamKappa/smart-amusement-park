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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_JSON_SERIALIZER_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_JSON_SERIALIZER_H

#include "nlohmann/json.hpp"
#include "bundle_info.h"

namespace OHOS {
namespace AppExecFwk {

/*
 * form_json and to_json is global static overload method, which need callback by json library,
 * and can not rename this function, so don't named according UpperCamelCase style
 */
void to_json(nlohmann::json &jsonObject, const AbilityInfo &abilityInfo);
void from_json(const nlohmann::json &jsonObject, AbilityInfo &abilityInfo);
void to_json(nlohmann::json &jsonObject, const ApplicationInfo &applicationInfo);
void from_json(const nlohmann::json &jsonObject, ApplicationInfo &applicationInfo);
void to_json(nlohmann::json &jsonObject, const BundleInfo &bundleInfo);
void from_json(const nlohmann::json &jsonObject, BundleInfo &bundleInfo);
void to_json(nlohmann::json &jsonObject, const ModuleInfo &moduleInfo);
void from_json(const nlohmann::json &jsonObject, ModuleInfo &moduleInfo);

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_JSON_SERIALIZER_H
