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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BASE_BUNDLE_INSTALLER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BASE_BUNDLE_INSTALLER_H

#include <string>

#include "nocopyable.h"

#include "appexecfwk_errors.h"
#include "inner_bundle_info.h"
#include "install_param.h"
#include "bundle_data_mgr.h"

namespace OHOS {
namespace AppExecFwk {

class BaseBundleInstaller {
public:
    BaseBundleInstaller();
    virtual ~BaseBundleInstaller();

protected:
    enum class InstallerState {
        INSTALL_START,
        INSTALL_BUNDLE_CHECKED = 5,
        INSTALL_PARSED = 10,
        INSTALL_SIGNATURE_CHECKED = 15,
        INSTALL_PERMS_REQ = 20,
        INSTALL_CREATDIR = 40,
        INSTALL_EXTRACTED = 60,
        INSTALL_RENAMED = 80,
        INSTALL_INFO_SAVED = 90,
        INSTALL_SUCCESS = 100,
        INSTALL_FAILED,
    };

    /**
     * @brief The main function for system and normal bundle install.
     * @param bundlePath Indicates the path for storing the HAP file of the application
     *                   to install or update.
     * @param installParam Indicates the install parameters.
     * @param appType Indicates the application type.
     * @return Returns ERR_OK if the application install successfully; returns error code otherwise.
     */
    ErrCode InstallBundle(
        const std::string &bundlePath, const InstallParam &installParam, const Constants::AppType appType);
    /**
     * @brief The main function for uninstall a bundle.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @return Returns ERR_OK if the application uninstall successfully; returns error code otherwise.
     */
    ErrCode UninstallBundle(const std::string &bundleName, const InstallParam &installParam);
    /**
     * @brief The main function for uninstall a module in a specific bundle.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param modulePackage Indicates the module package of the module to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @return Returns ERR_OK if the application uninstall successfully; returns error code otherwise.
     */
    ErrCode UninstallBundle(
        const std::string &bundleName, const std::string &modulePackage, const InstallParam &installParam);
    /**
     * @brief Update the installer state.
     * @attention This function changes the base class state only.
     * @param state Indicates the state to be updated to.
     * @return
     */
    virtual void UpdateInstallerState(const InstallerState state);
    /**
     * @brief Get the installer state.
     * @return The current state of the installer object.
     */
    inline InstallerState GetInstallerState()
    {
        return state_;
    }
    /**
     * @brief Get the installer state.
     * @param state Indicates the state to be updated to.
     * @return
     */
    inline void SetInstallerState(InstallerState state)
    {
        state_ = state;
    }

private:
    /**
     * @brief The real procedure for system and normal bundle install.
     * @param bundlePath Indicates the path for storing the HAP file of the application
     *                   to install or update.
     * @param installParam Indicates the install parameters.
     * @param appType Indicates the application type.
     * @return Returns ERR_OK if the bundle install successfully; returns error code otherwise.
     */
    ErrCode ProcessBundleInstall(
        const std::string &bundlePath, const InstallParam &installParam, const Constants::AppType appType);
    /**
     * @brief The real procedure function for uninstall a bundle.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @return Returns ERR_OK if the bundle uninstall successfully; returns error code otherwise.
     */
    ErrCode ProcessBundleUninstall(const std::string &bundleName, const InstallParam &installParam);
    /**
     * @brief The real procedure for uninstall a module in a specific bundle.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param modulePackage Indicates the module package of the module to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @return Returns ERR_OK if the module uninstall successfully; returns error code otherwise.
     */
    ErrCode ProcessBundleUninstall(
        const std::string &bundleName, const std::string &modulePackage, const InstallParam &installParam);
    /**
     * @brief The process of installing a new bundle.
     * @param info Indicates the InnerBundleInfo parsed from the config.json in the HAP package.
     * @return Returns ERR_OK if the new bundle install successfully; returns error code otherwise.
     */
    ErrCode ProcessBundleInstallStatus(InnerBundleInfo &info);
    /**
     * @brief The process of updating an exist bundle.
     * @param oldInfo Indicates the exist InnerBundleInfo object get from the database.
     * @param newInfo Indicates the InnerBundleInfo object parsed from the config.json in the HAP package.
     * @param isReplace Indicates whether there is the replace flag in the install flag.
     * @return Returns ERR_OK if the bundle updating successfully; returns error code otherwise.
     */
    ErrCode ProcessBundleUpdateStatus(InnerBundleInfo &oldInfo, InnerBundleInfo &newInfo, bool isReplace);
    /**
     * @brief Remove a whole bundle.
     * @param info Indicates the InnerBundleInfo object of a bundle.
     * @return Returns ERR_OK if the bundle removed successfully; returns error code otherwise.
     */
    ErrCode RemoveBundle(InnerBundleInfo &info);
    /**
     * @brief Create the code and data directories of a bundle.
     * @param info Indicates the InnerBundleInfo object of a bundle.
     * @return Returns ERR_OK if the bundle directories created successfully; returns error code otherwise.
     */
    ErrCode CreateBundleAndDataDir(InnerBundleInfo &info) const;
    /**
     * @brief Extract the code to temporilay directory and rename it.
     * @param info Indicates the InnerBundleInfo object of a bundle.
     * @return Returns ERR_OK if the bundle extract and renamed successfully; returns error code otherwise.
     */
    ErrCode ExtractModuleAndRename(InnerBundleInfo &info);
    /**
     * @brief Remove the code and data directories of a bundle.
     * @param info Indicates the InnerBundleInfo object of a bundle.
     * @param isUninstall Indicates that whether the remove is in an uninstall process.
     * @return Returns ERR_OK if the bundle directories removed successfully; returns error code otherwise.
     */
    ErrCode RemoveBundleAndDataDir(InnerBundleInfo &info, bool isUninstall) const;
    /**
     * @brief Remove the code and data directories of a module in a bundle.
     * @param info Indicates the InnerBundleInfo object of a bundle.
     * @param modulePackage Indicates the module to be removed.
     * @return Returns ERR_OK if the bundle directories removed successfully; returns error code otherwise.
     */
    ErrCode RemoveModuleAndDataDir(InnerBundleInfo &info, const std::string &modulePackage) const;
    /**
     * @brief Parse the bundle config.json file.
     * @param bundleFilePath Indicates the HAP file path.
     * @param InnerBundleInfo Indicates the InnerBundleInfo object of a bundle.
     * @return Returns ERR_OK if the bundle parsed successfully; returns error code otherwise.
     */
    ErrCode ParseBundleInfo(const std::string &bundleFilePath, InnerBundleInfo &info) const;
    /**
     * @brief Remove the current installing module directory.
     * @param info Indicates the InnerBundleInfo object of a bundle under installing.
     * @return Returns ERR_OK if the module directory removed successfully; returns error code otherwise.
     */
    ErrCode RemoveModuleDir(InnerBundleInfo &info) const;
    /**
     * @brief Extract files of the current installing module package.
     * @param info Indicates the InnerBundleInfo object of a bundle under installing.
     * @return Returns ERR_OK if the module files extraced successfully; returns error code otherwise.
     */
    ErrCode ExtractModuleFiles(InnerBundleInfo &info);
    /**
     * @brief Create the data directories of current installing module package.
     * @param info Indicates the InnerBundleInfo object of a bundle under installing.
     * @return Returns ERR_OK if the module directory created successfully; returns error code otherwise.
     */
    ErrCode CreateModuleDataDir(InnerBundleInfo &info) const;
    /**
     * @brief Remove the data directories of current installing module package.
     * @param info Indicates the InnerBundleInfo object of a bundle under installing.
     * @return Returns ERR_OK if the module directory removed successfully; returns error code otherwise.
     */
    ErrCode RemoveModuleDataDir(InnerBundleInfo &info) const;
    /**
     * @brief Rename the directory of current installing module package.
     * @param info Indicates the InnerBundleInfo object of a bundle under installing.
     * @return Returns ERR_OK if the module directory renamed successfully; returns error code otherwise.
     */
    ErrCode RenameModuleDir(InnerBundleInfo &info) const;
    /**
     * @brief Modify the install directory path for different install type.
     * @param info Indicates the InnerBundleInfo object of a bundle under installing.
     * @return Returns true if the path set successfully; returns false otherwise.
     */
    bool ModifyInstallDirByHapType(InnerBundleInfo &info);
    /**
     * @brief Update the bundle paths in the InnerBundleInfo object.
     * @param info Indicates the InnerBundleInfo object of a bundle under installing.
     * @param baseDataPath Indicates the data file paths.
     * @return Returns true if the path set successfully; returns false otherwise.
     */
    bool UpdateBundlePaths(InnerBundleInfo &info, const std::string baseDataPath) const;
    /**
     * @brief The process of install a new module package.
     * @param newInfo Indicates the InnerBundleInfo object parsed from the config.json in the HAP package.
     * @param oldInfo Indicates the exist InnerBundleInfo object get from the database.
     * @return Returns ERR_OK if the new module install successfully; returns error code otherwise.
     */
    ErrCode ProcessNewModuleInstall(InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo);
    /**
     * @brief The process of updating an exist module package.
     * @param newInfo Indicates the InnerBundleInfo object parsed from the config.json in the HAP package.
     * @param oldInfo Indicates the exist InnerBundleInfo object get from the database.
     * @return Returns ERR_OK if the module updating successfully; returns error code otherwise.
     */
    ErrCode ProcessModuleUpdate(InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo);

private:
    InstallerState state_ = InstallerState::INSTALL_START;
    std::shared_ptr<BundleDataMgr> dataMgr_ = nullptr;  // this pointer will get when public functions called
    std::string bundleName_;
    std::string moduleTmpDir_;
    std::string modulePath_;
    std::string baseCodePath_;
    std::string baseDataPath_;
    std::string modulePackage_;
    std::string mainAbility_;
    bool isAppExist_ = false;

    DISALLOW_COPY_AND_MOVE(BaseBundleInstaller);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BASE_BUNDLE_INSTALLER_H