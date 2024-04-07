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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INSTALLD_CLIENT_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INSTALLD_CLIENT_H

#include <memory>
#include <mutex>
#include <string>

#include "nocopyable.h"
#include "singleton.h"

#include "appexecfwk_errors.h"
#include "ipc/installd_interface.h"

namespace OHOS {
namespace AppExecFwk {

class InstalldClient : public DelayedSingleton<InstalldClient> {
public:
    /**
     * @brief Create a bundle code directory through an installd proxy object.
     * @param bundleDir Indicates the bundle code directory path that to be created.
     * @return Returns ERR_OK if the bundle directory created successfully; returns error code otherwise.
     */
    ErrCode CreateBundleDir(const std::string &bundleDir);
    /**
     * @brief Remove a bundle code directory.
     * @param bundleDir Indicates the bundle code directory path that to be removed.
     * @return Returns ERR_OK if the bundle directory removed successfully; returns error code otherwise.
     */
    ErrCode RemoveBundleDir(const std::string &bundleDir);
    /**
     * @brief Remove a bundle data directory.
     * @param bundleDir Indicates the bundle data directory path that to be removed.
     * @return Returns ERR_OK if the bundle data directory removed successfully; returns error code otherwise.
     */
    ErrCode RemoveBundleDataDir(const std::string &bundleDataDir);
    /**
     * @brief Extract the files of a HAP module to the code directory.
     * @param srcModulePath Indicates the HAP file path.
     * @param targetPath Indicates the code directory path that the HAP to be extracted to.
     * @return Returns ERR_OK if the HAP file extracted successfully; returns error code otherwise.
     */
    ErrCode ExtractModuleFiles(const std::string &srcModulePath, const std::string &targetPath);
    /**
     * @brief Remove a module directory.
     * @param moduleDir Indicates the module directory to be removed.
     * @return Returns ERR_OK if the module directory removed successfully; returns error code otherwise.
     */
    ErrCode RemoveModuleDir(const std::string &moduleDir);
    /**
     * @brief Rename the module directory from temporaily path to the real path.
     * @param oldPath Indicates the old path name.
     * @param newPath Indicates the new path name.
     * @return Returns ERR_OK if the module directory renamed successfully; returns error code otherwise.
     */
    ErrCode RenameModuleDir(const std::string &oldPath, const std::string &newPath);
    /**
     * @brief Create a bundle data directory.
     * @param bundleDir Indicates the bundle data directory path that to be created.
     * @param uid Indicates uid to be set to the directory.
     * @param gid Indicates gid to be set to the directory.
     * @return Returns ERR_OK if the bundle data directory created successfully; returns error code otherwise.
     */
    ErrCode CreateBundleDataDir(const std::string &bundleDir, const int uid, const int gid);
    /**
     * @brief Create a module and it's abilities data directory.
     * @param bundleDir Indicates the module data directory path that to be created.
     * @param abilityDirs Indicates the abilities data directory name that to be created.
     * @param uid Indicates uid to be set to the directory.
     * @param gid Indicates gid to be set to the directory.
     * @return Returns ERR_OK if the data directories created successfully; returns error code otherwise.
     */
    ErrCode CreateModuleDataDir(
        const std::string &ModuleDir, const std::vector<std::string> &abilityDirs, const int uid, const int gid);
    /**
     * @brief Remove a module data directory.
     * @param bundleDir Indicates the module data directory path that to be removed.
     * @return Returns ERR_OK if the module data directory removed successfully; returns error code otherwise.
     */
    ErrCode RemoveModuleDataDir(const std::string &moduleDataDir);
    /**
     * @brief Clean all files in a bundle data directory.
     * @param bundleDir Indicates the data directory path that to be cleaned.
     * @return Returns ERR_OK if the data directory cleaned successfully; returns error code otherwise.
     */
    ErrCode CleanBundleDataDir(const std::string &bundleDir);
    /**
     * @brief Reset the installd proxy object when installd service died.
     * @return
     */
    void ResetInstalldProxy();

private:
    /**
     * @brief Get the installd proxy object.
     * @return Returns true if the installd proxy object got successfully; returns false otherwise.
     */
    bool GetInstalldProxy();

private:
    std::mutex mutex_;
    sptr<IInstalld> installdProxy_;
    sptr<IRemoteObject::DeathRecipient> recipient_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INSTALLD_CLIENT_H