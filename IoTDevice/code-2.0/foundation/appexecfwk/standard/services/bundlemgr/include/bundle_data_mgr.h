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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_DATA_MGR_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_DATA_MGR_H

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "ohos/aafwk/content/want.h"

#include "ability_info.h"
#include "application_info.h"
#include "inner_bundle_info.h"
#include "bundle_status_callback_interface.h"
#include "bundle_data_storage_interface.h"

namespace OHOS {
namespace AppExecFwk {

enum class NotifyType { INSTALL, UPDATE, UNINSTALL_BUNDLE, UNINSTALL_MODULE };

enum class InstallState {
    INSTALL_START = 1,
    INSTALL_SUCCESS,
    INSTALL_FAIL,
    UNINSTALL_START,
    UNINSTALL_SUCCESS,
    UNINSTALL_FAIL,
    UPDATING_START,
    UPDATING_SUCCESS,
    UPDATING_FAIL,
};

class BundleDataMgr {
public:
    using Want = OHOS::AAFwk::Want;

    // init state transfer map data.
    BundleDataMgr();
    ~BundleDataMgr();

    /**
     * @brief Boot query persistent storage.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool LoadDataFromPersistentStorage();
    /**
     * @brief Update internal state for whole bundle.
     * @param bundleName Indicates the bundle name.
     * @param state Indicates the install state to be set.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool UpdateBundleInstallState(const std::string &bundleName, const InstallState state);
    /**
     * @brief Add new InnerBundleInfo.
     * @param bundleName Indicates the bundle name.
     * @param info Indicates the InnerBundleInfo object to be save.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool AddInnerBundleInfo(const std::string &bundleName, InnerBundleInfo &info);
    /**
     * @brief Add new module info to an exist InnerBundleInfo.
     * @param bundleName Indicates the bundle name.
     * @param newInfo Indicates the new InnerBundleInfo object.
     * @param oldInfo Indicates the old InnerBundleInfo object.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool AddNewModuleInfo(const std::string &bundleName, const InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo);
    /**
     * @brief Remove module info from an exist InnerBundleInfo.
     * @param bundleName Indicates the bundle name.
     * @param modulePackage Indicates the module Package.
     * @param oldInfo Indicates the old InnerBundleInfo object.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool RemoveModuleInfo(const std::string &bundleName, const std::string &modulePackage, InnerBundleInfo &oldInfo);
    /**
     * @brief Update module info of an exist module.
     * @param bundleName Indicates the bundle name.
     * @param newInfo Indicates the new InnerBundleInfo object.
     * @param oldInfo Indicates the old InnerBundleInfo object.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool UpdateInnerBundleInfo(const std::string &bundleName, const InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo);
    /**
     * @brief Get an InnerBundleInfo if exist (will change the status to DISABLED).
     * @param bundleName Indicates the bundle name.
     * @param deviceId Indicates this device Id corresponding to the bundle name.
     * @param info Indicates the obtained InnerBundleInfo object.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool GetInnerBundleInfo(const std::string &bundleName, const std::string &deviceId, InnerBundleInfo &info);
    /**
     * @brief Generate UID and GID for a bundle.
     * @param info Indicates the InnerBundleInfo object.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool GenerateUidAndGid(InnerBundleInfo &info);
    /**
     * @brief Query the AbilityInfo by the given Want.
     * @param want Indicates the information of the ability.
     * @param abilityInfo Indicates the obtained AbilityInfo object.
     * @return Returns true if the AbilityInfo is successfully obtained; returns false otherwise.
     */
    bool QueryAbilityInfo(const Want &want, AbilityInfo &abilityInfo) const;
    /**
     * @brief Query the AbilityInfo by ability.uri in config.json.
     * @param abilityUri Indicates the uri of the ability.
     * @param abilityInfo Indicates the obtained AbilityInfo object.
     * @return Returns true if the AbilityInfo is successfully obtained; returns false otherwise.
     */
    bool QueryAbilityInfoByUri(const std::string &abilityUri, AbilityInfo &abilityInfo) const;
    /**
     * @brief Obtains the ApplicationInfo based on a given bundle name.
     * @param appName Indicates the application bundle name to be queried.
     * @param flag Indicates the flag used to specify information contained
     *             in the ApplicationInfo object that will be returned.
     * @param userId Indicates the user ID.
     * @param appInfo Indicates the obtained ApplicationInfo object.
     * @return Returns true if the application is successfully obtained; returns false otherwise.
     */
    bool GetApplicationInfo(
        const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo) const;
    /**
     * @brief Obtains information about all installed applications of a specified user.
     * @param flag Indicates the flag used to specify information contained
     *             in the ApplicationInfo objects that will be returned.
     * @param userId Indicates the user ID.
     * @param appInfos Indicates all of the obtained ApplicationInfo objects.
     * @return Returns true if the application is successfully obtained; returns false otherwise.
     */
    bool GetApplicationInfos(
        const ApplicationFlag flag, const int userId, std::vector<ApplicationInfo> &appInfos) const;
    /**
     * @brief Obtains BundleInfo of all bundles available in the system.
     * @param flag Indicates the flag used to specify information contained in the BundleInfo that will be returned.
     * @param bundleInfos Indicates all of the obtained BundleInfo objects.
     * @return Returns true if the BundleInfos is successfully obtained; returns false otherwise.
     */
    bool GetBundleInfos(const BundleFlag flag, std::vector<BundleInfo> &bundleInfos) const;
    /**
     * @brief Obtains the BundleInfo based on a given bundle name.
     * @param bundleName Indicates the application bundle name to be queried.
     * @param flag Indicates the information contained in the BundleInfo object to be returned.
     * @param bundleInfo Indicates the obtained BundleInfo object.
     * @return Returns true if the BundleInfo is successfully obtained; returns false otherwise.
     */
    bool GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo) const;
    /**
     * @brief Obtains the BundleInfo of application bundles based on the specified metaData.
     * @param metaData Indicates the metadata to get in the bundle.
     * @param bundleInfos Indicates all of the obtained BundleInfo objects.
     * @return Returns true if the BundleInfos is successfully obtained; returns false otherwise.
     */
    bool GetBundleInfosByMetaData(const std::string &metaData, std::vector<BundleInfo> &bundleInfos) const;
    /**
     * @brief Obtains the bundle name of a specified application based on the given UID.
     * @param uid Indicates the uid.
     * @param bundleName Indicates the obtained bundle name.
     * @return Returns true if the bundle name is successfully obtained; returns false otherwise.
     */
    bool GetBundleNameForUid(const int uid, std::string &bundleName) const;
    /**
     * @brief Obtains an array of all group IDs associated with a specified bundle.
     * @param bundleName Indicates the bundle name.
     * @param gids Indicates the group IDs associated with the specified bundle.
     * @return Returns true if the gids is successfully obtained; returns false otherwise.
     */
    bool GetBundleGids(const std::string &bundleName, std::vector<int> &gids) const;
    /**
     * @brief Obtains the BundleInfo of all keep-alive applications in the system.
     * @param bundleInfos Indicates all of the obtained BundleInfo objects.
     * @return Returns true if the BundleInfos is successfully obtained; returns false otherwise.
     */
    bool QueryKeepAliveBundleInfos(std::vector<BundleInfo> &bundleInfos) const;
    /**
     * @brief Obtains the label of a specified ability.
     * @param bundleName Indicates the bundle name.
     * @param className Indicates the ability class name.
     * @return Returns the label of the ability if exist; returns empty string otherwise.
     */
    std::string GetAbilityLabel(const std::string &bundleName, const std::string &className) const;
    /**
     * @brief Obtains the Want for starting the main ability of an application based on the given bundle name.
     * @param bundleName Indicates the bundle name.
     * @param want Indicates the obtained launch Want object.
     * @return Returns true if the launch Want object is successfully obtained; returns false otherwise.
     */
    bool GetLaunchWantForBundle(const std::string &bundleName, Want &want) const;
    /**
     * @brief Obtain the HAP module info of a specific ability.
     * @param abilityInfo Indicates the ability.
     * @param hapModuleInfo Indicates the obtained HapModuleInfo object.
     * @return Returns true if the HapModuleInfo is successfully obtained; returns false otherwise.
     */
    bool GetHapModuleInfo(const AbilityInfo &abilityInfo, HapModuleInfo &hapModuleInfo) const;
    /**
     * @brief Check whether the app is system app by it's UID.
     * @param uid Indicates the uid.
     * @return Returns true if the bundle is a system application; returns false otherwise.
     */
    bool CheckIsSystemAppByUid(const int uid) const;
    /**
     * @brief Obtains all bundle names installed.
     * @param bundleNames Indicates the bundle Names.
     * @return Returns true if have bundle installed; returns false otherwise.
     */
    bool GetBundleList(std::vector<std::string> &bundleNames) const;
    /**
     * @brief Set the bundle status disable.
     * @param bundleName Indicates the bundle name.
     * @return Returns true if the bundle status successfully set; returns false otherwise.
     */
    bool DisableBundle(const std::string &bundleName);
    /**
     * @brief Set the bundle status enable.
     * @param bundleName Indicates the bundle name.
     * @return Returns true if the bundle status successfully set; returns false otherwise.
     */
    bool EnableBundle(const std::string &bundleName);
    /**
     * @brief Get whether the application status is enabled.
     * @param bundleName Indicates the bundle name.
     * @return Returns true if the bundle status is enabled; returns false otherwise.
     */
    bool IsApplicationEnabled(const std::string &bundleName) const;
    /**
     * @brief Set the application status.
     * @param bundleName Indicates the bundle name.
     * @param isEnable Indicates the status to set.
     * @return Returns true if the bundle status successfully set; returns false otherwise.
     */
    bool SetApplicationEnabled(const std::string &bundleName, bool isEnable);
    /**
     * @brief Register the bundle status callback function.
     * @param bundleStatusCallback Indicates the callback object that using for notifing the bundle status.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool RegisterBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback);
    /**
     * @brief Clear the specific bundle status callback.
     * @param bundleStatusCallback Indicates the callback to be cleared.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool ClearBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback);
    /**
     * @brief Unregister all the callbacks of status changed.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool UnregisterBundleStatusCallback();
    /**
     * @brief Notify when the installation, update, or uninstall state of an application changes.
     * @param bundleName Indicates the name of the bundle whose state has changed.
     * @param modulePackage Indicates the modulePackage name of the bundle whose state has changed.
     * @param resultCode Indicates the status code returned for the application installation, update, or uninstall
     *  result.
     * @param type Indicates the NotifyType object.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool NotifyBundleStatus(const std::string &bundleName, const std::string &modulePackage,
        const std::string &mainAbility, const ErrCode resultCode, const NotifyType type);
    /**
     * @brief Obtains the provision Id based on a given bundle name.
     * @param bundleName Indicates the application bundle name to be queried.
     * @param provisionId Indicates the provision Id to be returned.
     * @return Returns true if the provision Id is successfully obtained; returns false otherwise.
     */
    bool GetProvisionId(const std::string &bundleName, std::string &provisionId) const;
    /**
     * @brief Obtains the app feature based on a given bundle name.
     * @param bundleName Indicates the application bundle name to be queried.
     * @param provisionId Indicates the app feature to be returned.
     * @return Returns true if the app feature is successfully obtained; returns false otherwise.
     */
    bool GetAppFeature(const std::string &bundleName, std::string &appFeature) const;

    /**
     * @brief Set the flag that indicates whether all applications are installed.
     * @param flag Indicates the flag to be set.
     * @return
     */
    void SetAllInstallFlag(bool flag);

private:
    /**
     * @brief Init transferStates.
     * @return
     */
    void InitStateTransferMap();
    /**
     * @brief Determine whether to delete the data status.
     * @param state Indicates the InstallState object.
     * @return Returns true if state is INSTALL_FAIL，UNINSTALL_FAIL，UNINSTALL_SUCCESS，or UPDATING_FAIL; returns false
     * otherwise.
     */
    bool IsDeleteDataState(const InstallState state) const;
    /**
     * @brief Determine whether it is disable.
     * @param state Indicates the InstallState object.
     * @return Returns true if install state is UPDATING_START or UNINSTALL_START; returns false otherwise.
     */
    bool IsDisableState(const InstallState state) const;
    /**
     * @brief Delete bundle info if InstallState is not INSTALL_FAIL.
     * @param bundleName Indicates the bundle Names.
     * @param state Indicates the InstallState object.
     * @return Returns true if install state is UPDATING_START or UNINSTALL_START; returns false otherwise.
     */
    void DeleteBundleInfo(const std::string &bundleName, const InstallState state);
    /**
     * @brief Determine whether app is installed.
     * @param bundleName Indicates the bundle Names.
     * @return Returns true if install state is INSTALL_SUCCESS; returns false otherwise.
     */
    bool IsAppOrAbilityInstalled(const std::string &bundleName) const;
    /**
     * @brief Restore uid and gid .
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool RestoreUidAndGid();
    /**
     * @brief Recycle uid and gid .
     * @param info Indicates the InnerBundleInfo object.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool RecycleUidAndGid(const InnerBundleInfo &info);

private:
    mutable std::mutex bundleInfoMutex_;
    mutable std::mutex stateMutex_;
    mutable std::mutex uidMapMutex_;
    mutable std::mutex callbackMutex_;
    bool allInstallFlag_ = false;
    // using for generating uid and gid
    std::map<int, std::string> sysUidMap_;
    std::map<int, std::string> sysVendorUidMap_;
    std::map<int, std::string> appUidMap_;
    // use vector because these functions using for IPC, the bundleName may duplicate
    std::vector<sptr<IBundleStatusCallback>> callbackList_;
    // all installed bundles
    // key:bundleName
    // value:deviceId-innerbundleinfo pair
    std::map<std::string, std::map<std::string, InnerBundleInfo>> bundleInfos_;
    // key:bundle name
    std::map<std::string, InstallState> installStates_;
    // current-status:previous-statue pair
    std::multimap<InstallState, InstallState> transferStates_;
    std::shared_ptr<IBundleDataStorage> dataStorage_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_DATA_MGR_H
