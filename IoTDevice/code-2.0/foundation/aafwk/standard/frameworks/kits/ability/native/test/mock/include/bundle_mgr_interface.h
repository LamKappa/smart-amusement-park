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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_BUNDLEMGR_INCLUDE_MOCK_BUNDLE_MGR_INTERFACE_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_BUNDLEMGR_INCLUDE_MOCK_BUNDLE_MGR_INTERFACE_H

#include "ability_info.h"
#include "application_info.h"
#include "iremote_broker.h"
#include "iremote_object.h"

#include "bundle_info.h"
#include "hap_module_info.h"
#include "ohos/aafwk/content/want.h"
#include "permission_def.h"

using OHOS::AAFwk::Want;

namespace OHOS {
namespace AppExecFwk {
enum class DumpFlag {
    DUMP_BUNDLE_LIST = 1,
    DUMP_ALL_BUNDLE_INFO,
    DUMP_BUNDLE_INFO,
};

enum class InstallFlag {
    NORMAL = 0,
    // Allow to replace the existing bundle when the new version isn't lower than the old one.
    // If the bundle does not exist, just like normal flag.
    REPLACE_EXISTING = 1,
};

enum class InstallLocation {
    INTERNAL_ONLY = 1,
    PREFER_EXTERNAL = 2,
};

struct InstallParam : public Parcelable {
    InstallFlag installFlag = InstallFlag::NORMAL;
    InstallLocation installLocation = InstallLocation::INTERNAL_ONLY;
    int userId = -1;
    // Is keep user data while uninstall.
    bool isKeepData = false;

    // the parcel object function is not const.
    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static InstallParam *Unmarshalling(Parcel &parcel);
};
class IBundleStatusCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.BundleStatusCallback");

    virtual void OnBundleStateChanged(const uint8_t installType, const int32_t resultCode, const std::string &resultMsg,
        const std::string &bundleName) = 0;

    enum class Message {
        ON_BUNDLE_STATE_CHANGED,
    };
};

class ICleanCacheCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.CleanCacheCallback");

    virtual void OnCleanCacheFinished(bool succeeded) = 0;

    enum class Message {
        ON_CLEAN_CACHE_CALLBACK,
    };
};

class IStatusReceiver : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.StatusReceiver");

    virtual void OnStatusNotify(const int progress) = 0;
    virtual void OnFinished(const int32_t resultCode, const std::string &resultMsg) = 0;

    enum class Message {
        ON_STATUS_NOTIFY,
        ON_FINISHED,
    };

    enum {
        SUCCESS = 0,
        ERR_INSTALL_INTERNAL_ERROR,
        ERR_INSTALL_PARSE_FAILED,
        ERR_INSTALL_VERSION_DOWNGRADE,
        ERR_INSTALL_VERIFICATION_FAILED,
        ERR_INSTALL_NO_SIGNATURE_INFO,
        ERR_INSTALL_UPDATE_INCOMPATIBLE,
        ERR_INSTALL_INVALID_BUNDLE_FILE,
        ERR_INSTALL_MISSING_INSTALLED_BUNDLE,
        ERR_INSTALL_ALREADY_EXIST,
        ERR_INSTALL_PARSE_UNEXPECTED,
        ERR_INSTALL_PARSE_MISSING_BUNDLE,
        ERR_INSTALL_PARSE_MISSING_ABILITY,
        ERR_INSTALL_PARSE_NO_PROFILE,
        ERR_INSTALL_PARSE_BAD_PROFILE,
        ERR_INSTALL_PARSE_PROFILE_PROP_TYPE_ERROR,
        ERR_INSTALL_PARSE_PROFILE_MISSING_PROP,
        ERR_UNINSTALL_INVALID_NAME,
        ERR_UNKNOW,
    };
};

class IBundleInstaller : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.BundleInstaller");

    virtual bool Install(const std::string &bundleFilePath, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver) = 0;
    virtual bool Uninstall(const std::string &bundleName, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver) = 0;

    enum class Message {
        INSTALL,
        UNINSTALL,
    };
};

class IBundleMgr : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.aafwk.BundleMgr");
    virtual bool GetApplicationInfo(
        const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo) = 0;
    virtual bool GetApplicationInfos(
        const ApplicationFlag flag, const int userId, std::vector<ApplicationInfo> &appInfos) = 0;
    virtual bool GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo) = 0;
    virtual bool GetBundleInfos(const BundleFlag flag, std::vector<BundleInfo> &bundleInfos) = 0;
    virtual int GetUidByBundleName(const std::string &bundleName, const int userId) = 0;
    virtual bool GetBundleNamesForUid(const int uid, std::vector<std::string> &bundleNames) = 0;
    virtual bool GetBundleGids(const std::string &bundleName, std::vector<int> &gids) = 0;
    virtual std::string GetAppType(const std::string &bundleName) = 0;
    virtual bool CheckIsSystemAppByUid(const int uid) = 0;
    virtual bool GetBundleInfosByMetaData(const std::string &metaData, std::vector<BundleInfo> &bundleInfos) = 0;
    virtual bool QueryAbilityInfo(const Want &want, AbilityInfo &abilityInfo) = 0;
    virtual bool QueryAbilityInfoByUri(const std::string &uri, AbilityInfo &abilityInfo) = 0;
    virtual bool QueryKeepAliveBundleInfos(std::vector<BundleInfo> &bundleInfos) = 0;
    virtual std::string GetAbilityLabel(const std::string &bundleName, const std::string &className) = 0;
    // obtains information about an application bundle contained in an OHOS Ability Package (HAP).
    virtual bool GetBundleArchiveInfo(
        const std::string &hapFilePath, const BundleFlag flag, BundleInfo &bundleInfo) = 0;
    virtual bool GetHapModuleInfo(const std::string &hapFilePath, HapModuleInfo &hapModuleInfo) = 0;
    // obtains the Want for starting the main ability of an application based on the given bundle name.
    virtual bool GetLaunchWantForBundle(const std::string &bundleName, Want &want) = 0;
    // checks whether the publickeys of two bundles are the same.
    virtual int CheckPublicKeys(const std::string &firstBundleName, const std::string &secondBundleName) = 0;
    // checks whether a specified bundle has been granted a specific permission.
    virtual int CheckPermission(const std::string &bundleName, const std::string &permission) = 0;
    virtual bool GetPermissionDef(const std::string &permissionName, PermissionDef &permissionDef) = 0;
    virtual bool GetAllPermissionGroupDefs(std::vector<PermissionDef> &permissionDefs) = 0;
    virtual bool GetAppsGrantedPermissions(
        const std::vector<std::string> &permissions, std::vector<std::string> &appNames) = 0;
    virtual bool HasSystemCapability(const std::string &capName) = 0;
    virtual bool GetSystemAvailableCapabilities(std::vector<std::string> &systemCaps) = 0;
    virtual bool IsSafeMode() = 0;
    // clears cache data of a specified application.
    virtual bool CleanBundleCacheFiles(
        const std::string &bundleName, const sptr<ICleanCacheCallback> &cleanCacheCallback) = 0;
    virtual bool CleanBundleDataFiles(const std::string &bundleName) = 0;
    virtual bool RegisterBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback) = 0;
    virtual bool ClearBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback) = 0;
    // unregister callback of all application
    virtual bool UnregisterBundleStatusCallback() = 0;
    virtual bool DumpInfos(const DumpFlag flag, const std::string &bundleName, std::string &result) = 0;
    virtual sptr<IBundleInstaller> GetBundleInstaller() = 0;
    virtual bool CanRequestPermission(
        const std::string &bundleName, const std::string &permissionName, const int userId) = 0;
    virtual bool RequestPermissionFromUser(
        const std::string &bundleName, const std::string &permission, const int userId) = 0;
    enum class Message {
        QUERY_ABILITY_INFO,
        GET_APPLICATION_INFO,
    };
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_BUNDLEMGR_INCLUDE_MOCK_BUNDLE_MGR_INTERFACE_H