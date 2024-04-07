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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_UTIL_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_UTIL_H

#include <string>

#include "appexecfwk_errors.h"

namespace OHOS {
namespace AppExecFwk {

class BundleUtil {
public:
    /**
     * @brief Check whether a file is valid HAP file.
     * @param bundlePath Indicates the HAP file path.
     * @return Returns ERR_OK if the file checked successfully; returns error code otherwise.
     */
    static ErrCode CheckFilePath(const std::string &bundlePath, std::string &realPath);
    /**
     * @brief Check whether a file is the specific type file.
     * @param fileName Indicates the file path.
     * @param extensionName Indicates the type to be checked.
     * @return Returns true if the file type checked successfully; returns false otherwise.
     */
    static bool CheckFileType(const std::string &fileName, const std::string &extensionName);
    /**
     * @brief Check whether a file name is valid.
     * @param fileName Indicates the file path.
     * @return Returns true if the file name checked successfully; returns false otherwise.
     */
    static bool CheckFileName(const std::string &fileName);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_UTIL_H
