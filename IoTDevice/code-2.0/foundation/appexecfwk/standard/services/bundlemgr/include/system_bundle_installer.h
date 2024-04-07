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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_SYSTEM_BUNDLE_INSTALLER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_SYSTEM_BUNDLE_INSTALLER_H

#include "base_bundle_installer.h"

namespace OHOS {
namespace AppExecFwk {

class SystemBundleInstaller : public BaseBundleInstaller {
public:
    SystemBundleInstaller(const std::string &filePath);
    virtual ~SystemBundleInstaller() override;
    /**
     * @brief Install system and system vendor bundles.
     * @param appType Indicates the bundle type.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    bool InstallSystemBundle(Constants::AppType appType);

private:
    const std::string filePath_;

    DISALLOW_COPY_AND_MOVE(SystemBundleInstaller);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_SYSTEM_BUNDLE_INSTALLER_H