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

#include "bundle_util.h"

#include <unistd.h>

#include "string_ex.h"
#include "directory_ex.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {

ErrCode BundleUtil::CheckFilePath(const std::string &bundlePath, std::string &realPath)
{
    if (!CheckFileName(bundlePath)) {
        APP_LOGE("bundle file path invalid");
        return ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID;
    }
    if (!CheckFileType(bundlePath, Constants::INSTALL_FILE_SUFFIX)) {
        APP_LOGE("file is not hap");
        return ERR_APPEXECFWK_INSTALL_INVALID_HAP_NAME;
    }
    if (!PathToRealPath(bundlePath, realPath)) {
        APP_LOGE("file is not real path");
        return ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID;
    }
    if (access(realPath.c_str(), F_OK) != 0) {
        APP_LOGE("can not access the bundle file path: %{private}s", realPath.c_str());
        return ERR_APPEXECFWK_INSTALL_INVALID_BUNDLE_FILE;
    }
    return ERR_OK;
}

bool BundleUtil::CheckFileType(const std::string &fileName, const std::string &extensionName)
{
    APP_LOGD("path is %{public}s, support suffix is %{public}s", fileName.c_str(), extensionName.c_str());
    if (!CheckFileName(fileName)) {
        return false;
    }

    auto position = fileName.rfind('.');
    if (position == std::string::npos) {
        APP_LOGE("filename no extension name");
        return false;
    }

    std::string suffixStr = fileName.substr(position);
    return LowerStr(suffixStr) == extensionName;
}

bool BundleUtil::CheckFileName(const std::string &fileName)
{
    if (fileName.empty()) {
        APP_LOGE("the file name is empty");
        return false;
    }
    if (fileName.size() > Constants::PATH_MAX_SIZE) {
        APP_LOGE("bundle file path length %{public}zu too long", fileName.size());
        return false;
    }
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS
