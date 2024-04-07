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
#include "bundle_mgr.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "securec.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "installer_callback.h"

using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace {
constexpr int ARGC_SIZE = 4;
constexpr size_t BUF_SIZE = 1024;
constexpr size_t ARGS_SIZE_TWO = 2;
}  // namespace

static OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> GetBundleMgr()
{
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::sptr<OHOS::IRemoteObject> remoteObject =
        systemAbilityManager->GetSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    return OHOS::iface_cast<IBundleMgr>(remoteObject);
}

static void ConvertApplicationInfo(napi_env env, napi_value objAppInfo, const ApplicationInfo &appInfo)
{
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "name", nName));
    HILOG_INFO("ConvertApplicationInfo name=%{public}s.", appInfo.name.c_str());

    napi_value nBundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, appInfo.bundleName.c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "bundleName", nBundleName));
    HILOG_INFO("ConvertApplicationInfo bundleName=%{public}s.", appInfo.bundleName.c_str());

    napi_value nDescription;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, appInfo.description.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "description", nDescription));

    napi_value nDescriptionId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.descriptionId, &nDescriptionId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "descriptionId", nDescriptionId));

    napi_value nIconPath;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.iconPath.c_str(), NAPI_AUTO_LENGTH, &nIconPath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "iconPath", nIconPath));

    napi_value nIconId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.iconId, &nIconId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "iconId", nIconId));

    napi_value nLabel;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.label.c_str(), NAPI_AUTO_LENGTH, &nLabel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "label", nLabel));

    napi_value nLabelId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.labelId, &nLabelId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "labelId", nLabelId));

    napi_value nDeviceId;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.deviceId.c_str(), NAPI_AUTO_LENGTH, &nDeviceId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "deviceId", nDeviceId));

    napi_value nSignatureKey;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, appInfo.signatureKey.c_str(), NAPI_AUTO_LENGTH, &nSignatureKey));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "signatureKey", nSignatureKey));

    napi_value nIsSystemApp;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, appInfo.isSystemApp, &nIsSystemApp));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "isSystemApp", nIsSystemApp));

    napi_value nIsLauncherApp;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, appInfo.isLauncherApp, &nIsLauncherApp));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "isLauncherApp", nIsLauncherApp));

    napi_value nSupportedModes;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.supportedModes, &nSupportedModes));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "supportedModes", nSupportedModes));

    napi_value nProcess;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.process.c_str(), NAPI_AUTO_LENGTH, &nProcess));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "process", nProcess));

    napi_value nEntryDir;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.entryDir.c_str(), NAPI_AUTO_LENGTH, &nEntryDir));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "entryDir", nEntryDir));

    napi_value nCodePath;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.codePath.c_str(), NAPI_AUTO_LENGTH, &nCodePath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "codePath", nCodePath));

    napi_value nDataDir;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.dataDir.c_str(), NAPI_AUTO_LENGTH, &nDataDir));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "dataDir", nDataDir));

    napi_value nDataBaseDir;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, appInfo.dataBaseDir.c_str(), NAPI_AUTO_LENGTH, &nDataBaseDir));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "dataBaseDir", nDataBaseDir));

    napi_value nCacheDir;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.cacheDir.c_str(), NAPI_AUTO_LENGTH, &nCacheDir));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "cacheDir", nCacheDir));

    napi_value nPermissions;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nPermissions));
    for (size_t idx = 0; idx < appInfo.permissions.size(); idx++) {
        napi_value nPermission;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, appInfo.permissions[idx].c_str(), NAPI_AUTO_LENGTH, &nPermission));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nPermissions, idx, nPermission));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "permissions", nPermissions));

    napi_value nModuleSourceDirs;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nModuleSourceDirs));
    for (size_t idx = 0; idx < appInfo.moduleSourceDirs.size(); idx++) {
        napi_value nModuleSourceDir;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, appInfo.moduleSourceDirs[idx].c_str(), NAPI_AUTO_LENGTH, &nModuleSourceDir));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nModuleSourceDirs, idx, nModuleSourceDir));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "moduleSourceDirs", nModuleSourceDirs));

    napi_value nModuleInfos;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nModuleInfos));
    for (size_t idx = 0; idx < appInfo.moduleInfos.size(); idx++) {
        napi_value objModuleInfos;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objModuleInfos));

        napi_value nModuleName;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, appInfo.moduleInfos[idx].moduleName.c_str(), NAPI_AUTO_LENGTH, &nModuleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objModuleInfos, "moduleName", nModuleName));

        napi_value nModuleSourceDir;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(
                env, appInfo.moduleInfos[idx].moduleSourceDir.c_str(), NAPI_AUTO_LENGTH, &nModuleSourceDir));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objModuleInfos, "moduleSourceDir", nModuleSourceDir));

        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nModuleInfos, idx, objModuleInfos));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "moduleInfos", nModuleInfos));
    HILOG_INFO("ConvertApplicationInfo cacheDir=%{public}s.", appInfo.cacheDir.c_str());
}

static void ConvertAbilityInfo(napi_env env, napi_value objAbilityInfo, const AbilityInfo &abilityInfo)
{
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "name", nName));
    HILOG_INFO("ConvertAbilityInfo name=%{public}s.", abilityInfo.name.c_str());

    napi_value nLabel;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.label.c_str(), NAPI_AUTO_LENGTH, &nLabel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "label", nLabel));
    HILOG_INFO("ConvertAbilityInfo label=%{public}s.", abilityInfo.label.c_str());

    napi_value nDescription;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.description.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "description", nDescription));

    napi_value nIconPath;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.iconPath.c_str(), NAPI_AUTO_LENGTH, &nIconPath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "iconPath", nIconPath));

    napi_value nVisible;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, abilityInfo.visible, &nVisible));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "visible", nVisible));

    napi_value nKind;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.kind.c_str(), NAPI_AUTO_LENGTH, &nKind));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "kind", nKind));

    napi_value nPermissions;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nPermissions));
    for (size_t idx = 0; idx < abilityInfo.permissions.size(); idx++) {
        napi_value nPermission;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, abilityInfo.permissions[idx].c_str(), NAPI_AUTO_LENGTH, &nPermission));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nPermissions, idx, nPermission));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "permissions", nPermissions));

    napi_value nDeviceCapabilities;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nDeviceCapabilities));
    for (size_t idx = 0; idx < abilityInfo.deviceCapabilities.size(); idx++) {
        napi_value nDeviceCapability;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(
                env, abilityInfo.deviceCapabilities[idx].c_str(), NAPI_AUTO_LENGTH, &nDeviceCapability));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nDeviceCapabilities, idx, nDeviceCapability));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "deviceCapabilities", nDeviceCapabilities));

    napi_value nDeviceTypes;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nDeviceTypes));
    for (size_t idx = 0; idx < abilityInfo.deviceTypes.size(); idx++) {
        napi_value nDeviceType;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, abilityInfo.deviceTypes[idx].c_str(), NAPI_AUTO_LENGTH, &nDeviceType));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nDeviceTypes, idx, nDeviceType));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "deviceTypes", nDeviceTypes));

    napi_value nProcess;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.process.c_str(), NAPI_AUTO_LENGTH, &nProcess));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "process", nProcess));

    napi_value nUri;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.uri.c_str(), NAPI_AUTO_LENGTH, &nUri));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "uri", nUri));
    HILOG_INFO("ConvertAbilityInfo uri=%{public}s.", abilityInfo.uri.c_str());

    napi_value nBundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.bundleName.c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "bundleName", nBundleName));
    HILOG_INFO("ConvertAbilityInfo bundleName=%{public}s.", abilityInfo.bundleName.c_str());

    napi_value nPackage;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.package.c_str(), NAPI_AUTO_LENGTH, &nPackage));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "package", nPackage));
    HILOG_INFO("ConvertAbilityInfo package=%{public}s.", abilityInfo.package.c_str());

    napi_value nModuleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.moduleName.c_str(), NAPI_AUTO_LENGTH, &nModuleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "moduleName", nModuleName));

    napi_value nApplicationName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.applicationName.c_str(), NAPI_AUTO_LENGTH, &nApplicationName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "applicationName", nApplicationName));

    napi_value nDeviceId;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.deviceId.c_str(), NAPI_AUTO_LENGTH, &nDeviceId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "deviceId", nDeviceId));

    napi_value nCodePath;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.codePath.c_str(), NAPI_AUTO_LENGTH, &nCodePath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "codePath", nCodePath));

    napi_value nResourcePath;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.resourcePath.c_str(), NAPI_AUTO_LENGTH, &nResourcePath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "resourcePath", nResourcePath));

    napi_value nLibPath;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.libPath.c_str(), NAPI_AUTO_LENGTH, &nLibPath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "libPath", nLibPath));

    napi_value nAppInfo;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nAppInfo));
    ConvertApplicationInfo(env, nAppInfo, abilityInfo.applicationInfo);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "applicationInfo", nAppInfo));

    napi_value nType;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(abilityInfo.type), &nType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "type", nType));

    napi_value nOrientation;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(abilityInfo.orientation), &nOrientation));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "orientation", nOrientation));

    napi_value nLaunchMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(abilityInfo.launchMode), &nLaunchMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "launchMode", nLaunchMode));
}

static void ConvertBundleInfo(napi_env env, napi_value objBundleInfo, const BundleInfo &bundleInfo)
{
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, bundleInfo.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "name", nName));

    napi_value nLabel;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, bundleInfo.label.c_str(), NAPI_AUTO_LENGTH, &nLabel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "label", nLabel));

    napi_value nDescription;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, bundleInfo.description.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "description", nDescription));

    napi_value nVendor;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, bundleInfo.vendor.c_str(), NAPI_AUTO_LENGTH, &nVendor));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "vendor", nVendor));

    napi_value nVersionCode;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, bundleInfo.versionCode, &nVersionCode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "versionCode", nVersionCode));

    napi_value nVersionName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, bundleInfo.versionName.c_str(), NAPI_AUTO_LENGTH, &nVersionName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "versionName", nVersionName));

    napi_value nJointUserId;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, bundleInfo.jointUserId.c_str(), NAPI_AUTO_LENGTH, &nJointUserId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "jointUserId", nJointUserId));

    napi_value nIsKeepAlive;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, bundleInfo.isKeepAlive, &nIsKeepAlive));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "isKeepAlive", nIsKeepAlive));

    napi_value nIsNativeApp;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, bundleInfo.isNativeApp, &nIsNativeApp));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "isNativeApp", nIsNativeApp));

    napi_value nMinSdkVersion;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, bundleInfo.minSdkVersion, &nMinSdkVersion));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "minSdkVersion", nMinSdkVersion));

    napi_value nMaxSdkVersion;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, bundleInfo.maxSdkVersion, &nMaxSdkVersion));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "maxSdkVersion", nMaxSdkVersion));

    napi_value nMainEntry;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, bundleInfo.mainEntry.c_str(), NAPI_AUTO_LENGTH, &nMainEntry));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "mainEntry", nMainEntry));

    napi_value nCpuAbi;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, bundleInfo.cpuAbi.c_str(), NAPI_AUTO_LENGTH, &nCpuAbi));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "cpuAbi", nCpuAbi));

    napi_value nAppId;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, bundleInfo.appId.c_str(), NAPI_AUTO_LENGTH, &nAppId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "appId", nAppId));

    napi_value nReleaseType;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, bundleInfo.releaseType.c_str(), NAPI_AUTO_LENGTH, &nReleaseType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "releaseType", nReleaseType));

    napi_value nSeInfo;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, bundleInfo.seInfo.c_str(), NAPI_AUTO_LENGTH, &nSeInfo));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "seInfo", nSeInfo));

    napi_value nEntryModuleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, bundleInfo.entryModuleName.c_str(), NAPI_AUTO_LENGTH, &nEntryModuleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "entryModuleName", nEntryModuleName));

    napi_value nCompatibleVersion;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, bundleInfo.compatibleVersion, &nCompatibleVersion));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "compatibleVersion", nCompatibleVersion));

    napi_value nTargetVersion;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, bundleInfo.targetVersion, &nTargetVersion));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "targetVersion", nTargetVersion));

    napi_value nUid;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, bundleInfo.uid, &nUid));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "uid", nUid));

    napi_value nGid;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, bundleInfo.gid, &nGid));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "gid", nGid));

    napi_value nInstallTime;
    NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, bundleInfo.installTime, &nInstallTime));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "installTime", nInstallTime));

    napi_value nUpdateTime;
    NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, bundleInfo.updateTime, &nUpdateTime));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "updateTime", nUpdateTime));

    napi_value nAppInfo;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nAppInfo));
    ConvertApplicationInfo(env, nAppInfo, bundleInfo.applicationInfo);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "applicationInfo", nAppInfo));

    napi_value nAbilityInfos;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nAbilityInfos));
    for (size_t idx = 0; idx < bundleInfo.abilityInfos.size(); idx++) {
        napi_value objAbilityInfo;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objAbilityInfo));
        ConvertAbilityInfo(env, objAbilityInfo, bundleInfo.abilityInfos[idx]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nAbilityInfos, idx, objAbilityInfo));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "abilityInfos", nAbilityInfos));

    napi_value nReqPermissions;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nReqPermissions));
    for (size_t idx = 0; idx < bundleInfo.reqPermissions.size(); idx++) {
        napi_value nReqPermission;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, bundleInfo.reqPermissions[idx].c_str(), NAPI_AUTO_LENGTH, &nReqPermission));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nReqPermissions, idx, nReqPermission));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "reqPermissions", nReqPermissions));

    napi_value nDefPermissions;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nDefPermissions));
    for (size_t idx = 0; idx < bundleInfo.defPermissions.size(); idx++) {
        napi_value nDefPermission;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, bundleInfo.defPermissions[idx].c_str(), NAPI_AUTO_LENGTH, &nDefPermission));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nDefPermissions, idx, nDefPermission));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "defPermissions", nDefPermissions));

    napi_value nHapModuleNames;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nHapModuleNames));
    for (size_t idx = 0; idx < bundleInfo.hapModuleNames.size(); idx++) {
        napi_value nHapModuleName;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, bundleInfo.hapModuleNames[idx].c_str(), NAPI_AUTO_LENGTH, &nHapModuleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nHapModuleNames, idx, nHapModuleName));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "hapModuleNames", nHapModuleNames));

    napi_value nModuleNames;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nModuleNames));
    for (size_t idx = 0; idx < bundleInfo.moduleNames.size(); idx++) {
        napi_value nModuleName;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, bundleInfo.moduleNames[idx].c_str(), NAPI_AUTO_LENGTH, &nModuleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nModuleNames, idx, nModuleName));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "moduleNames", nModuleNames));

    napi_value nModulePublicDirs;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nModulePublicDirs));
    for (size_t idx = 0; idx < bundleInfo.modulePublicDirs.size(); idx++) {
        napi_value nModulePublicDir;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(
                env, bundleInfo.modulePublicDirs[idx].c_str(), NAPI_AUTO_LENGTH, &nModulePublicDir));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nModulePublicDirs, idx, nModulePublicDir));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "modulePublicDirs", nModulePublicDirs));

    napi_value nModuleDirs;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nModuleDirs));
    for (size_t idx = 0; idx < bundleInfo.moduleDirs.size(); idx++) {
        napi_value nModuleDir;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, bundleInfo.moduleDirs[idx].c_str(), NAPI_AUTO_LENGTH, &nModuleDir));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nModuleDirs, idx, nModuleDir));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "moduleDirs", nModuleDirs));

    napi_value nModuleResPaths;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nModuleResPaths));
    for (size_t idx = 0; idx < bundleInfo.moduleResPaths.size(); idx++) {
        napi_value nModuleResPath;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, bundleInfo.moduleResPaths[idx].c_str(), NAPI_AUTO_LENGTH, &nModuleResPath));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nModuleResPaths, idx, nModuleResPath));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "moduleResPaths", nModuleResPaths));
}

static void InnerGetApplicationInfos(napi_env env, std::vector<OHOS::AppExecFwk::ApplicationInfo> &appInfos)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        HILOG_ERROR("can not get iBundleMgr");
        return;
    }
    iBundleMgr->GetApplicationInfos(ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMS, 0, appInfos);
}

static void ProcessApplicationInfos(
    napi_env env, napi_value result, const std::vector<OHOS::AppExecFwk::ApplicationInfo> &appInfos)
{
    if (appInfos.size() > 0) {
        HILOG_INFO("-----appInfos is not null-----");
        size_t index = 0;
        for (const auto &item : appInfos) {
            HILOG_INFO("name{%s} ", item.name.c_str());
            HILOG_INFO("bundleName{%s} ", item.bundleName.c_str());
            for (const auto &moduleInfo : item.moduleInfos) {
                HILOG_INFO("moduleName{%s} ", moduleInfo.moduleName.c_str());
                HILOG_INFO("bundleName{%s} ", moduleInfo.moduleSourceDir.c_str());
            }
            napi_value objAppInfo;
            NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objAppInfo));
            ConvertApplicationInfo(env, objAppInfo, item);
            NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, objAppInfo));
            index++;
        }
    } else {
        HILOG_INFO("-----appInfos is null-----");
    }
}
/**
 * Promise and async callback
 */
napi_value GetApplicationInfos(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGC_SIZE] = {nullptr};
    napi_value thisArg;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, &data));

    AsyncApplicationInfosCallbackInfo *asyncCallbackInfo = new AsyncApplicationInfosCallbackInfo{
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
    };
    if (argc > 0) {
        HILOG_INFO("GetApplicationInfos asyncCallback.");
        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "GetApplicationInfos", NAPI_AUTO_LENGTH, &resourceName));
        for (size_t i = 0; i < argc; i++) {
            napi_valuetype valuetype;
            NAPI_CALL(env, napi_typeof(env, args[i], &valuetype));
            NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
            NAPI_CALL(env, napi_create_reference(env, args[i], 1, &asyncCallbackInfo->callback[i]));
        }
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncApplicationInfosCallbackInfo *asyncCallbackInfo = (AsyncApplicationInfosCallbackInfo *)data;
                InnerGetApplicationInfos(env, asyncCallbackInfo->appInfos);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncApplicationInfosCallbackInfo *asyncCallbackInfo = (AsyncApplicationInfosCallbackInfo *)data;
                napi_value result;
                napi_create_array(env, &result);
                ProcessApplicationInfos(env, result, asyncCallbackInfo->appInfos);
                napi_value callback;
                napi_value undefined;
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_value callResult;
                napi_call_function(env, undefined, callback, 1, &result, &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                if (asyncCallbackInfo->callback[1] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[1]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, 1, &result));
        return result;
    } else {
        HILOG_INFO("GetApplicationInfos promise.");
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        napi_create_string_latin1(env, "GetApplicationInfos", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncApplicationInfosCallbackInfo *asyncCallbackInfo = (AsyncApplicationInfosCallbackInfo *)data;
                InnerGetApplicationInfos(env, asyncCallbackInfo->appInfos);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("=================load=================");
                AsyncApplicationInfosCallbackInfo *asyncCallbackInfo = (AsyncApplicationInfosCallbackInfo *)data;
                napi_value result;
                napi_create_array(env, &result);
                ProcessApplicationInfos(env, result, asyncCallbackInfo->appInfos);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

// QueryAbilityInfo(want)
static void InnerQueryAbilityInfo(napi_env env, const Want &want, AbilityInfo &abilityInfo)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        HILOG_ERROR("can not get iBundleMgr");
        return;
    }
    iBundleMgr->QueryAbilityInfo(want, abilityInfo);
}

static napi_value ParseWant(napi_env env, Want &want, napi_value args)
{
    napi_status status;
    char buf[BUF_SIZE] = {0};
    size_t len = 0;
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, args, &valueType));
    NAPI_ASSERT(env, valueType == napi_object, "param type mismatch!");
    HILOG_INFO("-----ParseWant type1-----");
    napi_value wantProp = nullptr;
    status = napi_get_named_property(env, args, "want", &wantProp);
    NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
    napi_typeof(env, wantProp, &valueType);
    NAPI_ASSERT(env, valueType == napi_object, "property type mismatch!");
    HILOG_INFO("-----ParseWant want-----");
    // get want action property
    napi_value property = nullptr;
    status = napi_get_named_property(env, wantProp, "action", &property);
    NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
    napi_typeof(env, property, &valueType);
    NAPI_ASSERT(env, valueType == napi_string, "property type mismatch!");
    (void)memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
    napi_get_value_string_utf8(env, property, buf, BUF_SIZE, &len);
    want.SetAction(std::string(buf));
    HILOG_INFO("ParseWantSetAction=%{public}s.", want.GetAction().c_str());
    // get want entities property
    property = nullptr;
    status = napi_get_named_property(env, wantProp, "entities", &property);
    NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
    napi_typeof(env, property, &valueType);
    NAPI_ASSERT(env, valueType == napi_object, "property type mismatch!");
    {
        std::string deviceId;
        std::string bundleName;
        std::string abilityName;

        status = napi_get_named_property(env, wantProp, "elementName", &property);
        NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
        napi_typeof(env, property, &valueType);
        NAPI_ASSERT(env, valueType == napi_object, "property type mismatch!");

        // get elementName:deviceId_ property
        napi_value prop = nullptr;
        status = napi_get_named_property(env, property, "deviceId", &prop);
        NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
        napi_typeof(env, prop, &valueType);
        NAPI_ASSERT(env, valueType == napi_string, "property type mismatch!");
        (void)memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
        NAPI_CALL(env, napi_get_value_string_utf8(env, prop, buf, BUF_SIZE, &len));
        deviceId = std::string(buf);

        // get elementName:bundleName_ property
        prop = nullptr;
        status = napi_get_named_property(env, property, "bundleName", &prop);
        NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
        napi_typeof(env, prop, &valueType);
        NAPI_ASSERT(env, valueType == napi_string, "property type mismatch!");
        (void)memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
        napi_get_value_string_utf8(env, prop, buf, BUF_SIZE, &len);
        bundleName = std::string(buf);
        HILOG_INFO("ParseWant bundleName=%{public}s.", bundleName.c_str());

        // get elementName:abilityName_ property
        prop = nullptr;
        status = napi_get_named_property(env, property, "abilityName", &prop);
        NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
        napi_typeof(env, prop, &valueType);
        NAPI_ASSERT(env, valueType == napi_string, "property type mismatch!");
        (void)memset_s(buf, BUF_SIZE, 0, BUF_SIZE);
        napi_get_value_string_utf8(env, prop, buf, BUF_SIZE, &len);
        abilityName = std::string(buf);
        HILOG_INFO("ParseWant abilityName=%{public}s.", abilityName.c_str());
        want.SetElementName(deviceId, bundleName, abilityName);
    }
    // create result code
    napi_value result;
    status = napi_create_int32(env, 1, &result);
    NAPI_ASSERT(env, status == napi_ok, "napi_create_int32 error!");
    return result;
}
/**
 * Promise and async callback
 */
napi_value QueryAbilityInfo(napi_env env, napi_callback_info info)
{

    HILOG_INFO("QueryAbilityInfo called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGC_SIZE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}zu]", argc);
    Want want;
    ParseWant(env, want, argv[0]);
    HILOG_INFO("After ParseWant action=%{public}s.", want.GetAction().c_str());
    HILOG_INFO("After ParseWant bundleName=%{public}s.", want.GetElement().GetBundleName().c_str());
    HILOG_INFO("After ParseWant abilityName=%{public}s.", want.GetElement().GetAbilityName().c_str());
    AsyncAbilityInfoCallbackInfo *asyncCallbackInfo =
        new AsyncAbilityInfoCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr, .want = want};

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_INFO("QueryAbilityInfo asyncCallback.");
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "QueryAbilityInfo", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncAbilityInfoCallbackInfo *asyncCallbackInfo = (AsyncAbilityInfoCallbackInfo *)data;
                InnerQueryAbilityInfo(env, asyncCallbackInfo->want, asyncCallbackInfo->abilityInfo);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncAbilityInfoCallbackInfo *asyncCallbackInfo = (AsyncAbilityInfoCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                ConvertAbilityInfo(env, result, asyncCallbackInfo->abilityInfo);
                napi_value callback;
                napi_value undefined;
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_value callResult;
                napi_call_function(env, undefined, callback, 1, &result, &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                if (asyncCallbackInfo->callback[1] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[1]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, 1, &result));

        return result;
    } else {
        HILOG_INFO("QueryAbilityInfo promise.");
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        napi_create_string_latin1(env, "QueryAbilityInfo", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncAbilityInfoCallbackInfo *asyncCallbackInfo = (AsyncAbilityInfoCallbackInfo *)data;
                InnerQueryAbilityInfo(env, asyncCallbackInfo->want, asyncCallbackInfo->abilityInfo);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("=================load=================");
                AsyncAbilityInfoCallbackInfo *asyncCallbackInfo = (AsyncAbilityInfoCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                ConvertAbilityInfo(env, result, asyncCallbackInfo->abilityInfo);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

static void InnerGetApplicationInfo(napi_env env, const std::string &bundleName, ApplicationInfo &appInfo)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        HILOG_ERROR("can not get iBundleMgr");
        return;
    }
    iBundleMgr->GetApplicationInfo(bundleName, ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMS, 0, appInfo);
}

static napi_value ParseString(napi_env env, std::string &param, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, args, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    char buf[BUF_SIZE] = {0};
    size_t len = 0;
    napi_get_value_string_utf8(env, args, buf, BUF_SIZE, &len);
    HILOG_INFO("param=%{public}s.", buf);
    param = std::string{buf};
    // create result code
    napi_value result;
    status = napi_create_int32(env, 1, &result);
    NAPI_ASSERT(env, status == napi_ok, "napi_create_int32 error!");
    return result;
}
/**
 * Promise and async callback
 */
napi_value GetApplicationInfo(napi_env env, napi_callback_info info)
{
    HILOG_INFO("NAPI_GetApplicationInfo called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGC_SIZE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}zu]", argc);
    std::string bundleName;
    ParseString(env, bundleName, argv[0]);
    AsyncApplicationInfoCallbackInfo *asyncCallbackInfo = new AsyncApplicationInfoCallbackInfo{
        .env = env, .asyncWork = nullptr, .deferred = nullptr, .bundleName = bundleName};

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_INFO("GetApplicationInfo asyncCallback.");
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetApplicationInfoCallBack", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncApplicationInfoCallbackInfo *asyncCallbackInfo = (AsyncApplicationInfoCallbackInfo *)data;
                InnerGetApplicationInfo(env, asyncCallbackInfo->bundleName, asyncCallbackInfo->appInfo);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncApplicationInfoCallbackInfo *asyncCallbackInfo = (AsyncApplicationInfoCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                HILOG_INFO("appInfo.name=%{public}s.", asyncCallbackInfo->appInfo.name.c_str());
                HILOG_INFO("appInfo.bundleName=%{public}s.", asyncCallbackInfo->appInfo.bundleName.c_str());
                HILOG_INFO("appInfo.description=%{public}s.", asyncCallbackInfo->appInfo.description.c_str());
                HILOG_INFO("appInfo.descriptionId=%{public}d.", asyncCallbackInfo->appInfo.descriptionId);
                ConvertApplicationInfo(env, result, asyncCallbackInfo->appInfo);
                napi_value callback;
                napi_value undefined;
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_value callResult;
                napi_call_function(env, undefined, callback, 1, &result, &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                if (asyncCallbackInfo->callback[1] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[1]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, 1, &result));

        return result;
    } else {
        HILOG_INFO("GetApplicationInfo promise.");
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        napi_create_string_latin1(env, "GetApplicationInfo", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncApplicationInfoCallbackInfo *asyncCallbackInfo = (AsyncApplicationInfoCallbackInfo *)data;
                InnerGetApplicationInfo(env, asyncCallbackInfo->bundleName, asyncCallbackInfo->appInfo);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("=================load=================");
                AsyncApplicationInfoCallbackInfo *asyncCallbackInfo = (AsyncApplicationInfoCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                HILOG_INFO("appInfo.name=%{public}s.", asyncCallbackInfo->appInfo.name.c_str());
                HILOG_INFO("appInfo.bundleName=%{public}s.", asyncCallbackInfo->appInfo.bundleName.c_str());
                HILOG_INFO("appInfo.description=%{public}s.", asyncCallbackInfo->appInfo.description.c_str());
                HILOG_INFO("appInfo.descriptionId=%{public}d.", asyncCallbackInfo->appInfo.descriptionId);
                ConvertApplicationInfo(env, result, asyncCallbackInfo->appInfo);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

static void InnerGetBundleInfos(napi_env env, std::vector<OHOS::AppExecFwk::BundleInfo> &bundleInfos)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        HILOG_ERROR("can not get iBundleMgr");
        return;
    }
    iBundleMgr->GetBundleInfos(BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfos);
}

static void ProcessBundleInfos(
    napi_env env, napi_value result, const std::vector<OHOS::AppExecFwk::BundleInfo> &bundleInfos)
{
    if (bundleInfos.size() > 0) {
        HILOG_INFO("-----bundleInfos is not null-----");
        size_t index = 0;
        for (const auto &item : bundleInfos) {
            HILOG_INFO("name{%s} ", item.name.c_str());
            HILOG_INFO("bundleName{%s} ", item.applicationInfo.bundleName.c_str());
            for (const auto &moduleInfo : item.applicationInfo.moduleInfos) {
                HILOG_INFO("moduleName{%s} ", moduleInfo.moduleName.c_str());
                HILOG_INFO("moduleSourceDir{%s} ", moduleInfo.moduleSourceDir.c_str());
            }
            napi_value objBundleInfo = nullptr;
            napi_create_object(env, &objBundleInfo);
            ConvertBundleInfo(env, objBundleInfo, item);
            napi_set_element(env, result, index, objBundleInfo);
            index++;
        }
    } else {
        HILOG_INFO("-----bundleInfos is null-----");
    }
}
/**
 * Promise and async callback
 */
napi_value GetBundleInfos(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGC_SIZE] = {0};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, &data));
    AsyncBundleInfosCallbackInfo *asyncCallbackInfo = new AsyncBundleInfosCallbackInfo{
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
    };
    if (argc > 0) {
        HILOG_INFO("GetBundleInfo asyncCallback.");
        napi_value resourceName;
        napi_create_string_latin1(env, "GetBundleInfos", NAPI_AUTO_LENGTH, &resourceName);
        for (size_t i = 0; i < argc; i++) {
            napi_valuetype valuetype;
            NAPI_CALL(env, napi_typeof(env, args[i], &valuetype));
            NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
            NAPI_CALL(env, napi_create_reference(env, args[i], 1, &asyncCallbackInfo->callback[i]));
        }

        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncBundleInfosCallbackInfo *asyncCallbackInfo = (AsyncBundleInfosCallbackInfo *)data;
                InnerGetBundleInfos(env, asyncCallbackInfo->bundleInfos);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncBundleInfosCallbackInfo *asyncCallbackInfo = (AsyncBundleInfosCallbackInfo *)data;
                napi_value result;
                napi_create_array(env, &result);
                ProcessBundleInfos(env, result, asyncCallbackInfo->bundleInfos);
                napi_value callback;
                napi_value undefined;
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_value callResult;
                napi_call_function(env, undefined, callback, 1, &result, &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                if (asyncCallbackInfo->callback[1] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[1]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, 1, &result));

        return result;
    } else {
        HILOG_INFO("BundleMgr::GetBundleInfos promise.");
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        napi_create_string_latin1(env, "GetBundleInfos", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncBundleInfosCallbackInfo *asyncCallbackInfo = (AsyncBundleInfosCallbackInfo *)data;
                InnerGetBundleInfos(env, asyncCallbackInfo->bundleInfos);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("=================load=================");
                AsyncBundleInfosCallbackInfo *asyncCallbackInfo = (AsyncBundleInfosCallbackInfo *)data;
                napi_value result;
                napi_create_array(env, &result);
                ProcessBundleInfos(env, result, asyncCallbackInfo->bundleInfos);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}
static void InnerGetBundleInfo(napi_env env, const std::string &bundleName, BundleInfo &bundleInfo)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        HILOG_ERROR("can not get iBundleMgr");
        return;
    }
    bool ret = iBundleMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
    if (!ret) {
        HILOG_INFO("-----bundleInfo is not find-----");
    }
}
/**
 * Promise and async callback
 */
napi_value GetBundleInfo(napi_env env, napi_callback_info info)
{
    HILOG_INFO("NAPI_InnerGetBundleInfo called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGC_SIZE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}zu]", argc);
    std::string bundleName;
    ParseString(env, bundleName, argv[0]);
    AsyncBundleInfoCallbackInfo *asyncCallbackInfo =
        new AsyncBundleInfoCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr, .param = bundleName};

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_INFO("InnerGetBundleInfo asyncCallback.");
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_InnerGetBundleInfo", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncBundleInfoCallbackInfo *asyncCallbackInfo = (AsyncBundleInfoCallbackInfo *)data;
                InnerGetBundleInfo(env, asyncCallbackInfo->param, asyncCallbackInfo->bundleInfo);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncBundleInfoCallbackInfo *asyncCallbackInfo = (AsyncBundleInfoCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                ConvertBundleInfo(env, result, asyncCallbackInfo->bundleInfo);
                napi_value callback;
                napi_value undefined;
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_value callResult;
                napi_call_function(env, undefined, callback, 1, &result, &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                if (asyncCallbackInfo->callback[1] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[1]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, 1, &result));

        return result;
    } else {
        HILOG_INFO("GetBundleinfo promise.");
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        napi_create_string_latin1(env, "GetBundleInfo", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncBundleInfoCallbackInfo *asyncCallbackInfo = (AsyncBundleInfoCallbackInfo *)data;
                InnerGetBundleInfo(env, asyncCallbackInfo->param, asyncCallbackInfo->bundleInfo);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("=================load=================");
                AsyncBundleInfoCallbackInfo *asyncCallbackInfo = (AsyncBundleInfoCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                ConvertBundleInfo(env, result, asyncCallbackInfo->bundleInfo);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

static void InnerGetArchiveInfo(napi_env env, const std::string &hapFilePath, BundleInfo &bundleInfo)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        HILOG_ERROR("can not get iBundleMgr");
        return;
    };
    bool ret = iBundleMgr->GetBundleArchiveInfo(hapFilePath, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
    if (!ret) {
        HILOG_INFO("-----bundleInfo is not find-----");
    }
}
/**
 * Promise and async callback
 */
napi_value GetBundleArchiveInfo(napi_env env, napi_callback_info info)
{
    HILOG_INFO("NAPI_GetBundleArchiveInfo called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGC_SIZE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}zu]", argc);
    std::string hapFilePath;
    ParseString(env, hapFilePath, argv[0]);
    AsyncBundleInfoCallbackInfo *asyncCallbackInfo =
        new AsyncBundleInfoCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr, .param = hapFilePath};

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_INFO("GetBundleArchiveInfo asyncCallback.");
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetBundleArchiveInfo", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncBundleInfoCallbackInfo *asyncCallbackInfo = (AsyncBundleInfoCallbackInfo *)data;
                InnerGetArchiveInfo(env, asyncCallbackInfo->param, asyncCallbackInfo->bundleInfo);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncBundleInfoCallbackInfo *asyncCallbackInfo = (AsyncBundleInfoCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                ConvertBundleInfo(env, result, asyncCallbackInfo->bundleInfo);
                napi_value callback;
                napi_value undefined;
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_value callResult;
                napi_call_function(env, undefined, callback, 1, &result, &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                if (asyncCallbackInfo->callback[1] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[1]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, 1, &result));

        return result;
    } else {
        HILOG_INFO("GetBundleArchiveInfo promise.");
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        napi_create_string_latin1(env, "GetBundleArchiveInfo", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncBundleInfoCallbackInfo *asyncCallbackInfo = (AsyncBundleInfoCallbackInfo *)data;
                InnerGetArchiveInfo(env, asyncCallbackInfo->param, asyncCallbackInfo->bundleInfo);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("=================load=================");
                AsyncBundleInfoCallbackInfo *asyncCallbackInfo = (AsyncBundleInfoCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                ConvertBundleInfo(env, result, asyncCallbackInfo->bundleInfo);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

static void ConvertPermissionDef(napi_env env, napi_value result, const PermissionDef &permissionDef)
{
    napi_value nPermissionName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, permissionDef.permissionName.c_str(), NAPI_AUTO_LENGTH, &nPermissionName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "permissionName", nPermissionName));
    HILOG_INFO("InnerGetPermissionDef name=%{public}s.", permissionDef.permissionName.c_str());

    napi_value nBundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, permissionDef.bundleName.c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "bundleName", nBundleName));
    HILOG_INFO("InnerGetPermissionDef bundleName=%{public}s.", permissionDef.bundleName.c_str());

    napi_value nGrantMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, permissionDef.grantMode, &nGrantMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "grantMode", nGrantMode));

    napi_value nAvailableScope;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, permissionDef.availableScope, &nAvailableScope));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "availableScope", nAvailableScope));

    napi_value nLabel;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, permissionDef.label.c_str(), NAPI_AUTO_LENGTH, &nLabel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "label", nLabel));

    napi_value nLabelId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, permissionDef.labelId, &nLabelId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "labelId", nLabelId));

    napi_value nDescription;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, permissionDef.description.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "description", nDescription));

    napi_value nDescriptionId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, permissionDef.descriptionId, &nDescriptionId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "descriptionId", nDescriptionId));
}

static void InnerGetPermissionDef(napi_env env, const std::string &permissionName, PermissionDef &permissionDef)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        HILOG_ERROR("can not get iBundleMgr");
        return;
    };
    bool ret = iBundleMgr->GetPermissionDef(permissionName, permissionDef);
    if (ret) {
        HILOG_INFO("-----permissionName is not find-----");
    }
}
/**
 * Promise and async callback
 */
napi_value GetPermissionDef(napi_env env, napi_callback_info info)
{
    HILOG_INFO("GetPermissionDef called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGC_SIZE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}zu]", argc);
    std::string permissionName;
    ParseString(env, permissionName, argv[0]);
    AsyncPermissionDefCallbackInfo *asyncCallbackInfo = new AsyncPermissionDefCallbackInfo{
        .env = env, .asyncWork = nullptr, .deferred = nullptr, .permissionName = permissionName};

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_INFO("GetPermissionDef asyncCallback.");
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "GetPermissionDef", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncPermissionDefCallbackInfo *asyncCallbackInfo = (AsyncPermissionDefCallbackInfo *)data;
                HILOG_INFO("asyncCallbackInfo->permissionName=%{public}s.", asyncCallbackInfo->permissionName.c_str());
                InnerGetPermissionDef(env, asyncCallbackInfo->permissionName, asyncCallbackInfo->permissionDef);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncPermissionDefCallbackInfo *asyncCallbackInfo = (AsyncPermissionDefCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                ConvertPermissionDef(env, result, asyncCallbackInfo->permissionDef);
                napi_value callback;
                napi_value undefined;
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_value callResult;
                napi_call_function(env, undefined, callback, 1, &result, &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                if (asyncCallbackInfo->callback[1] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[1]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, 1, &result));

        return result;
    } else {
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        napi_create_string_latin1(env, "GetBundleInfo", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncPermissionDefCallbackInfo *asyncCallbackInfo = (AsyncPermissionDefCallbackInfo *)data;
                InnerGetPermissionDef(env, asyncCallbackInfo->permissionName, asyncCallbackInfo->permissionDef);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("=================load=================");
                AsyncPermissionDefCallbackInfo *asyncCallbackInfo = (AsyncPermissionDefCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                ConvertPermissionDef(env, result, asyncCallbackInfo->permissionDef);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

static void InnerInstall(napi_env env, const std::string &bundleFilePath, std::string &resultMsg)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        HILOG_ERROR("can not get iBundleMgr");
        return;
    }
    auto iBundleInstaller = iBundleMgr->GetBundleInstaller();
    if (!iBundleInstaller) {
        HILOG_ERROR("can not get iBundleInstaller");
        return;
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    OHOS::sptr<InstallerCallback> callback = new InstallerCallback();
    if (!callback) {
        HILOG_ERROR("callback nullptr");
        return;
    }
    iBundleInstaller->Install(bundleFilePath, installParam, callback);
    resultMsg = callback->GetResultMsg();
}
/**
 * Promise and async callback
 */
napi_value Install(napi_env env, napi_callback_info info)
{
    HILOG_INFO("Install called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGC_SIZE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}zu]", argc);
    std::string bundleFilePath;
    ParseString(env, bundleFilePath, argv[0]);
    AsyncInstallCallbackInfo *asyncCallbackInfo =
        new AsyncInstallCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr, .param = bundleFilePath};

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_INFO("Install asyncCallback.");
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "Install", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                InnerInstall(env, asyncCallbackInfo->param, asyncCallbackInfo->resultMsg);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                napi_value nResultMsg;
                napi_create_string_utf8(env, asyncCallbackInfo->resultMsg.c_str(), NAPI_AUTO_LENGTH, &nResultMsg);
                napi_set_named_property(env, result, "installResultMsg", nResultMsg);
                napi_value callback;
                napi_value undefined;
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_value callResult;
                napi_call_function(env, undefined, callback, 1, &result, &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                if (asyncCallbackInfo->callback[1] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[1]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, 1, &result));

        return result;
    } else {
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        napi_create_string_latin1(env, "Install", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                InnerInstall(env, asyncCallbackInfo->param, asyncCallbackInfo->resultMsg);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("=================load=================");
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                napi_value nResultMsg;
                napi_create_string_utf8(env, asyncCallbackInfo->resultMsg.c_str(), NAPI_AUTO_LENGTH, &nResultMsg);
                napi_set_named_property(env, result, "installResultMsg", nResultMsg);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

static void InnerUninstall(napi_env env, const std::string &bundleName, std::string &resultMsg)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        HILOG_ERROR("can not get iBundleMgr");
        return;
    }
    auto iBundleInstaller = iBundleMgr->GetBundleInstaller();
    if (!iBundleInstaller) {
        HILOG_ERROR("can not get iBundleInstaller");
        return;
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    OHOS::sptr<InstallerCallback> callback = new InstallerCallback();
    if (!callback) {
        HILOG_ERROR("callback nullptr");
        return;
    }
    iBundleInstaller->Uninstall(bundleName, installParam, callback);
    resultMsg = callback->GetResultMsg();
}
/**
 * Promise and async callback
 */
napi_value Uninstall(napi_env env, napi_callback_info info)
{
    HILOG_INFO("Uninstall called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGC_SIZE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}zu]", argc);
    std::string bundleName;
    ParseString(env, bundleName, argv[0]);
    AsyncInstallCallbackInfo *asyncCallbackInfo =
        new AsyncInstallCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr, .param = bundleName};

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_INFO("Uninstall asyncCallback.");
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "Uninstall", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                InnerUninstall(env, asyncCallbackInfo->param, asyncCallbackInfo->resultMsg);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                napi_value nResultMsg;
                napi_create_string_utf8(env, asyncCallbackInfo->resultMsg.c_str(), NAPI_AUTO_LENGTH, &nResultMsg);
                napi_set_named_property(env, result, "installResultMsg", nResultMsg);
                napi_value callback;
                napi_value undefined;
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_value callResult;
                napi_call_function(env, undefined, callback, 1, &result, &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                if (asyncCallbackInfo->callback[1] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[1]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, 1, &result));

        return result;
    } else {
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        napi_create_string_latin1(env, "Install", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                InnerUninstall(env, asyncCallbackInfo->param, asyncCallbackInfo->resultMsg);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("=================load=================");
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                napi_value nResultMsg;
                napi_create_string_utf8(env, asyncCallbackInfo->resultMsg.c_str(), NAPI_AUTO_LENGTH, &nResultMsg);
                napi_set_named_property(env, result, "installResultMsg", nResultMsg);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}