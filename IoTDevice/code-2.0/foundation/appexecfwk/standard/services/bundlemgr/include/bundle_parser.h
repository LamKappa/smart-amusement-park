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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PARSER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PARSER_H

#include <string>

#include "appexecfwk_errors.h"
#include "inner_bundle_info.h"

namespace OHOS {
namespace AppExecFwk {

class BundleParser {
public:
    /**
     * @brief Parse bundle by the path name, then save in innerBundleInfo info.
     * @param pathName Indicates the path of Bundle.
     * @param innerBundleInfo Indicates the obtained InnerBundleInfo object.
     * @return Returns ERR_OK if the bundle successfully parsed; returns ErrCode otherwise.
     */
    ErrCode Parse(const std::string &pathName, InnerBundleInfo &innerBundleInfo) const;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PARSER_H
