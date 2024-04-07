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

#include "bundle_mgr_proxy.h"

#include "ipc_types.h"
#include "parcel.h"
#include "string_ex.h"

#include "appexecfwk_errors.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {

BundleMgrProxy::BundleMgrProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBundleMgr>(impl)
{
    APP_LOGI("create bundle mgr proxy instance");
}

BundleMgrProxy::~BundleMgrProxy()
{
    APP_LOGI("destroy create bundle mgr proxy instance");
}

bool BundleMgrProxy::GetApplicationInfo(
    const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo)
{
    APP_LOGI("begin to GetApplicationInfo of %{public}s", appName.c_str());
    if (appName.empty()) {
        APP_LOGE("fail to GetApplicationInfo due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetApplicationInfo due to write MessageParcel fail");
        return false;
    }
    if (!data.WriteString(appName)) {
        APP_LOGE("fail to GetApplicationInfo due to write appName fail");
        return false;
    }
    if (!data.WriteInt32(static_cast<int>(flag))) {
        APP_LOGE("fail to GetApplicationInfo due to write flag fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        APP_LOGE("fail to GetApplicationInfo due to write userId fail");
        return false;
    }

    if (!GetParcelableInfo<ApplicationInfo>(IBundleMgr::Message::GET_APPLICATION_INFO, data, appInfo)) {
        APP_LOGE("fail to GetApplicationInfo from server");
        return false;
    }
    return true;
}

bool BundleMgrProxy::GetApplicationInfos(
    const ApplicationFlag flag, const int userId, std::vector<ApplicationInfo> &appInfos)
{
    APP_LOGD("begin to get GetApplicationInfos of specific userId id %{private}d", userId);
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetApplicationInfo due to write MessageParcel fail");
        return false;
    }
    if (!data.WriteInt32(static_cast<int>(flag))) {
        APP_LOGE("fail to GetApplicationInfo due to write flag fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        APP_LOGE("fail to GetApplicationInfos due to write userId error");
        return false;
    }

    if (!GetParcelableInfos<ApplicationInfo>(IBundleMgr::Message::GET_APPLICATION_INFOS, data, appInfos)) {
        APP_LOGE("fail to GetApplicationInfos from server");
        return false;
    }
    return true;
}

bool BundleMgrProxy::GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo)
{
    APP_LOGI("begin to get bundle info of %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("fail to GetBundleInfo due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetBundleInfo due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("fail to GetBundleInfo due to write bundleName fail");
        return false;
    }
    if (!data.WriteInt32(static_cast<int>(flag))) {
        APP_LOGE("fail to GetBundleInfo due to write flag fail");
        return false;
    }

    if (!GetParcelableInfo<BundleInfo>(IBundleMgr::Message::GET_BUNDLE_INFO, data, bundleInfo)) {
        APP_LOGE("fail to GetBundleInfo from server");
        return false;
    }
    return true;
}

bool BundleMgrProxy::GetBundleInfos(const BundleFlag flag, std::vector<BundleInfo> &bundleInfos)
{
    APP_LOGD("begin to get bundle infos");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetBundleInfos due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(static_cast<int>(flag))) {
        APP_LOGE("fail to GetBundleInfos due to write flag fail");
        return false;
    }

    if (!GetParcelableInfos<BundleInfo>(IBundleMgr::Message::GET_BUNDLE_INFOS, data, bundleInfos)) {
        APP_LOGE("fail to GetBundleInfos from server");
        return false;
    }
    return true;
}

int BundleMgrProxy::GetUidByBundleName(const std::string &bundleName, const int userId)
{
    APP_LOGI("begin to get uid of %{public}s", bundleName.c_str());
    BundleInfo bundleInfo;
    int uid = Constants::INVALID_UID;
    bool ret = GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    if (ret) {
        uid = bundleInfo.uid;
        APP_LOGD("get bundle uid success");
    } else {
        APP_LOGE("can not get bundleInfo's uid");
    }
    return uid;
}

bool BundleMgrProxy::GetBundleNameForUid(const int uid, std::string &bundleName)
{
    APP_LOGI("begin to GetBundleNameForUid of %{public}d", uid);
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetBundleNameForUid due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(uid)) {
        APP_LOGE("fail to GetBundleNameForUid due to write uid fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::GET_BUNDLE_NAME_FOR_UID, data, reply)) {
        APP_LOGE("fail to GetBundleNameForUid from server");
        return false;
    }
    if (!reply.ReadBool()) {
        APP_LOGE("reply result false");
        return false;
    }
    bundleName = reply.ReadString();
    return true;
}

bool BundleMgrProxy::GetBundleGids(const std::string &bundleName, std::vector<int> &gids)
{
    APP_LOGI("begin to GetBundleGids of %{public}s", bundleName.c_str());
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetBundleGids due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("fail to GetBundleGids due to write bundleName fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::GET_BUNDLE_GIDS, data, reply)) {
        APP_LOGE("fail to GetBundleGids from server");
        return false;
    }

    if (!reply.ReadInt32Vector(&gids)) {
        APP_LOGE("fail to GetBundleGids from reply");
        return false;
    }
    return true;
}

std::string BundleMgrProxy::GetAppType(const std::string &bundleName)
{
    APP_LOGI("begin to GetAppType of %{public}s", bundleName.c_str());
    BundleInfo bundleInfo;
    bool ret = GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    if (ret) {
        bool isSystemApp = bundleInfo.applicationInfo.isSystemApp;
        APP_LOGD("get GetAppType success");
        if (isSystemApp) {
            return Constants::SYSTEM_APP;
        } else {
            return Constants::THIRD_PARTY_APP;
        }
    }
    APP_LOGE("can not GetAppType");
    return Constants::EMPTY_STRING;
}

bool BundleMgrProxy::CheckIsSystemAppByUid(const int uid)
{
    APP_LOGI("begin to CheckIsSystemAppByUid of %{public}d", uid);
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to CheckIsSystemAppByUid due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(uid)) {
        APP_LOGE("fail to CheckIsSystemAppByUid due to write uid fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::CHECK_IS_SYSTEM_APP_BY_UID, data, reply)) {
        APP_LOGE("fail to CheckIsSystemAppByUid from server");
        return false;
    }
    return reply.ReadBool();
}

bool BundleMgrProxy::GetBundleInfosByMetaData(const std::string &metaData, std::vector<BundleInfo> &bundleInfos)
{
    APP_LOGI("begin to GetBundleInfosByMetaData of %{public}s", metaData.c_str());
    if (metaData.empty()) {
        APP_LOGE("fail to GetBundleInfosByMetaData due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetBundleInfosByMetaData due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(metaData)) {
        APP_LOGE("fail to GetBundleInfosByMetaData due to write metaData fail");
        return false;
    }

    if (!GetParcelableInfos<BundleInfo>(IBundleMgr::Message::GET_BUNDLE_INFOS_BY_METADATA, data, bundleInfos)) {
        APP_LOGE("fail to GetBundleInfosByMetaData from server");
        return false;
    }
    return true;
}

bool BundleMgrProxy::QueryAbilityInfo(const Want &want, AbilityInfo &abilityInfo)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to QueryAbilityInfo due to write MessageParcel fail");
        return false;
    }
    if (!data.WriteParcelable(&want)) {
        APP_LOGE("fail to QueryAbilityInfo due to write want fail");
        return false;
    }

    if (!GetParcelableInfo<AbilityInfo>(IBundleMgr::Message::QUERY_ABILITY_INFO, data, abilityInfo)) {
        APP_LOGE("fail to query ability info from server");
        return false;
    }
    return true;
}

bool BundleMgrProxy::QueryAbilityInfoByUri(const std::string &abilityUri, AbilityInfo &abilityInfo)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to QueryAbilityInfoByUri due to write MessageParcel fail");
        return false;
    }
    if (!data.WriteString(abilityUri)) {
        APP_LOGE("fail to QueryAbilityInfoByUri due to write abilityUri fail");
        return false;
    }

    if (!GetParcelableInfo<AbilityInfo>(IBundleMgr::Message::QUERY_ABILITY_INFO_BY_URI, data, abilityInfo)) {
        APP_LOGE("fail to QueryAbilityInfoByUri from server");
        return false;
    }
    return true;
}

bool BundleMgrProxy::QueryKeepAliveBundleInfos(std::vector<BundleInfo> &bundleInfos)
{
    APP_LOGI("begin to QueryKeepAliveBundleInfos");

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to QueryKeepAliveBundleInfos due to write InterfaceToken fail");
        return false;
    }

    if (!GetParcelableInfos<BundleInfo>(IBundleMgr::Message::QUERY_KEEPALIVE_BUNDLE_INFOS, data, bundleInfos)) {
        APP_LOGE("fail to QueryKeepAliveBundleInfos from server");
        return false;
    }
    return true;
}

std::string BundleMgrProxy::GetAbilityLabel(const std::string &bundleName, const std::string &className)
{
    APP_LOGI("begin to get bundle info of %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("fail to GetAbilityLabel due to params empty");
        return Constants::EMPTY_STRING;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetAbilityLabel due to write InterfaceToken fail");
        return Constants::EMPTY_STRING;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("fail to GetAbilityLabel due to write bundleName fail");
        return Constants::EMPTY_STRING;
    }
    if (!data.WriteString(className)) {
        APP_LOGE("fail to GetAbilityLabel due to write className fail");
        return Constants::EMPTY_STRING;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::GET_ABILITY_LABEL, data, reply)) {
        APP_LOGE("fail to GetAbilityLabel from server");
        return Constants::EMPTY_STRING;
    }
    return reply.ReadString();
}

bool BundleMgrProxy::GetBundleArchiveInfo(const std::string &hapFilePath, const BundleFlag flag, BundleInfo &bundleInfo)
{
    APP_LOGI("begin to GetBundleArchiveInfo of %{public}s", hapFilePath.c_str());
    if (hapFilePath.empty()) {
        APP_LOGE("fail to GetBundleArchiveInfo due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetBundleArchiveInfo due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(hapFilePath)) {
        APP_LOGE("fail to GetBundleArchiveInfo due to write hapFilePath fail");
        return false;
    }
    if (!data.WriteInt32(static_cast<int>(flag))) {
        APP_LOGE("fail to GetBundleArchiveInfo due to write flag fail");
        return false;
    }

    if (!GetParcelableInfo<BundleInfo>(IBundleMgr::Message::GET_BUNDLE_ARCHIVE_INFO, data, bundleInfo)) {
        APP_LOGE("fail to GetBundleArchiveInfo from server");
        return false;
    }
    return true;
}

bool BundleMgrProxy::GetHapModuleInfo(const AbilityInfo &abilityInfo, HapModuleInfo &hapModuleInfo)
{
    APP_LOGI("begin to GetHapModuleInfo of %{public}s", abilityInfo.package.c_str());
    if (abilityInfo.bundleName.empty() || abilityInfo.package.empty()) {
        APP_LOGE("fail to GetHapModuleInfo due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetHapModuleInfo due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteParcelable(&abilityInfo)) {
        APP_LOGE("fail to GetHapModuleInfo due to write abilityInfo fail");
        return false;
    }

    if (!GetParcelableInfo<HapModuleInfo>(IBundleMgr::Message::GET_HAP_MODULE_INFO, data, hapModuleInfo)) {
        APP_LOGE("fail to GetHapModuleInfo from server");
        return false;
    }
    return true;
}

bool BundleMgrProxy::GetLaunchWantForBundle(const std::string &bundleName, Want &want)
{
    APP_LOGI("begin to GetLaunchWantForBundle of %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("fail to GetHapModuleInfo due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetLaunchWantForBundle due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("fail to GetLaunchWantForBundle due to write bundleName fail");
        return false;
    }

    if (!GetParcelableInfo<Want>(IBundleMgr::Message::GET_LAUNCH_WANT_FOR_BUNDLE, data, want)) {
        APP_LOGE("fail to GetLaunchWantForBundle from server");
        return false;
    }
    return true;
}

int BundleMgrProxy::CheckPublicKeys(const std::string &firstBundleName, const std::string &secondBundleName)
{
    APP_LOGI(
        "begin to CheckPublicKeys of %{public}s and %{public}s", firstBundleName.c_str(), secondBundleName.c_str());
    if (firstBundleName.empty() || secondBundleName.empty()) {
        APP_LOGE("fail to CheckPublicKeys due to params empty");
        return Constants::SIGNATURE_UNKNOWN_BUNDLE;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetBundleInfo due to write MessageParcel fail");
        return Constants::SIGNATURE_UNKNOWN_BUNDLE;
    }
    if (!data.WriteString(firstBundleName)) {
        APP_LOGE("fail to GetBundleInfo due to write firstBundleName fail");
        return Constants::SIGNATURE_UNKNOWN_BUNDLE;
    }
    if (!data.WriteString(secondBundleName)) {
        APP_LOGE("fail to GetBundleInfo due to write secondBundleName fail");
        return Constants::SIGNATURE_UNKNOWN_BUNDLE;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::CHECK_PUBLICKEYS, data, reply)) {
        APP_LOGE("fail to CheckPublicKeys from server");
        return Constants::SIGNATURE_UNKNOWN_BUNDLE;
    }
    return reply.ReadInt32();
}

int BundleMgrProxy::CheckPermission(const std::string &bundleName, const std::string &permission)
{
    APP_LOGI("begin to CheckPublicKeys of %{public}s and %{public}s", bundleName.c_str(), permission.c_str());
    if (bundleName.empty() || permission.empty()) {
        APP_LOGE("fail to CheckPermission due to params empty");
        return Constants::PERMISSION_NOT_GRANTED;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to CheckPermission due to write InterfaceToken fail");
        return Constants::PERMISSION_NOT_GRANTED;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("fail to CheckPermission due to write bundleName fail");
        return Constants::PERMISSION_NOT_GRANTED;
    }
    if (!data.WriteString(permission)) {
        APP_LOGE("fail to CheckPermission due to write permission fail");
        return Constants::PERMISSION_NOT_GRANTED;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::CHECK_PERMISSION, data, reply)) {
        APP_LOGE("fail to CheckPermission from server");
        return Constants::PERMISSION_NOT_GRANTED;
    }
    return reply.ReadInt32();
}

bool BundleMgrProxy::GetPermissionDef(const std::string &permissionName, PermissionDef &permissionDef)
{
    APP_LOGI("begin to GetPermissionDef of %{public}s", permissionName.c_str());
    if (permissionName.empty()) {
        APP_LOGE("fail to GetPermissionDef due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetPermissionDef due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(permissionName)) {
        APP_LOGE("fail to GetPermissionDef due to write permissionName fail");
        return false;
    }

    if (!GetParcelableInfo<PermissionDef>(IBundleMgr::Message::GET_PERMISSION_DEF, data, permissionDef)) {
        APP_LOGE("fail to GetPermissionDef from server");
        return false;
    }
    return true;
}

bool BundleMgrProxy::GetAllPermissionGroupDefs(std::vector<PermissionDef> &permissionDefs)
{
    APP_LOGI("begin to GetPermissionDefs");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetAllPermissionGroupDefs due to write InterfaceToken fail");
        return false;
    }

    if (!GetParcelableInfos<PermissionDef>(IBundleMgr::Message::GET_ALL_PERMISSION_GROUP_DEFS, data, permissionDefs)) {
        APP_LOGE("fail to GetAllPermissionGroupDefs from server");
        return false;
    }
    return true;
}

bool BundleMgrProxy::GetAppsGrantedPermissions(
    const std::vector<std::string> &permissions, std::vector<std::string> &appNames)
{
    APP_LOGI("begin to GetAppsGrantedPermissions");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetAppsGrantedPermissions due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteStringVector(permissions)) {
        APP_LOGE("fail to GetAppsGrantedPermissions due to write permissions fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::GET_APPS_GRANTED_PERMISSIONS, data, reply)) {
        APP_LOGE("fail to GetAppsGrantedPermissions from server");
        return false;
    }

    if (!reply.ReadStringVector(&appNames)) {
        APP_LOGE("fail to GetAppsGrantedPermissions from reply");
        return false;
    }

    return true;
}

bool BundleMgrProxy::HasSystemCapability(const std::string &capName)
{
    APP_LOGI("begin to HasSystemCapability of %{public}s", capName.c_str());
    if (capName.empty()) {
        APP_LOGE("fail to HasSystemCapability due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to HasSystemCapability due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(capName)) {
        APP_LOGE("fail to HasSystemCapability due to write capName fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::HAS_SYSTEM_CAPABILITY, data, reply)) {
        APP_LOGE("fail to HasSystemCapability from server");
        return false;
    }
    return reply.ReadBool();
}

bool BundleMgrProxy::GetSystemAvailableCapabilities(std::vector<std::string> &systemCaps)
{
    APP_LOGI("begin to GetSystemAvailableCapabilities");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetSystemAvailableCapabilities due to write InterfaceToken fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::GET_SYSTEM_AVAILABLE_CAPABILITIES, data, reply)) {
        APP_LOGE("fail to GetSystemAvailableCapabilities from server");
        return false;
    }

    if (!reply.ReadStringVector(&systemCaps)) {
        APP_LOGE("fail to GetSystemAvailableCapabilities from reply");
        return false;
    }

    return true;
}

bool BundleMgrProxy::IsSafeMode()
{
    APP_LOGI("begin to IsSafeMode");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to IsSafeMode due to write InterfaceToken fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::IS_SAFE_MODE, data, reply)) {
        APP_LOGE("fail to IsSafeMode from server");
        return false;
    }
    return reply.ReadBool();
}

bool BundleMgrProxy::CleanBundleCacheFiles(
    const std::string &bundleName, const sptr<ICleanCacheCallback> &cleanCacheCallback)
{
    APP_LOGI("begin to CleanBundleCacheFiles of %{public}s", bundleName.c_str());
    if (bundleName.empty() || !cleanCacheCallback) {
        APP_LOGE("fail to CleanBundleCacheFiles due to params error");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to CleanBundleCacheFiles due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("fail to CleanBundleCacheFiles due to write bundleName fail");
        return false;
    }

    if (!data.WriteParcelable(cleanCacheCallback->AsObject())) {
        APP_LOGE("fail to CleanBundleCacheFiles, for write parcel failed");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::CLEAN_BUNDLE_CACHE_FILES, data, reply)) {
        APP_LOGE("fail to CleanBundleCacheFiles from server");
        return false;
    }
    return reply.ReadBool();
}

bool BundleMgrProxy::CleanBundleDataFiles(const std::string &bundleName)
{
    APP_LOGI("begin to CleanBundleDataFiles of %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("fail to CleanBundleDataFiles due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to CleanBundleDataFiles due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("fail to CleanBundleDataFiles due to write hapFilePath fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::CLEAN_BUNDLE_DATA_FILES, data, reply)) {
        APP_LOGE("fail to CleanBundleDataFiles from server");
        return false;
    }
    return reply.ReadBool();
}

bool BundleMgrProxy::RegisterBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback)
{
    APP_LOGI("begin to RegisterBundleStatusCallback");
    if (!bundleStatusCallback || bundleStatusCallback->GetBundleName().empty()) {
        APP_LOGE("fail to RegisterBundleStatusCallback");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to RegisterBundleStatusCallback due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(bundleStatusCallback->GetBundleName())) {
        APP_LOGE("fail to RegisterBundleStatusCallback due to write bundleName fail");
        return false;
    }
    if (!data.WriteParcelable(bundleStatusCallback->AsObject())) {
        APP_LOGE("fail to RegisterBundleStatusCallback, for write parcel failed");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::REGISTER_BUNDLE_STATUS_CALLBACK, data, reply)) {
        APP_LOGE("fail to RegisterBundleStatusCallback from server");
        return false;
    }
    return reply.ReadBool();
}

bool BundleMgrProxy::ClearBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback)
{
    APP_LOGI("begin to ClearBundleStatusCallback");
    if (!bundleStatusCallback) {
        APP_LOGE("fail to ClearBundleStatusCallback, for bundleStatusCallback is nullptr");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to ClearBundleStatusCallback due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteParcelable(bundleStatusCallback->AsObject())) {
        APP_LOGE("fail to ClearBundleStatusCallback, for write parcel failed");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::CLEAR_BUNDLE_STATUS_CALLBACK, data, reply)) {
        APP_LOGE("fail to CleanBundleCacheFiles from server");
        return false;
    }
    return reply.ReadBool();
}

bool BundleMgrProxy::UnregisterBundleStatusCallback()
{
    APP_LOGI("begin to UnregisterBundleStatusCallback");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to UnregisterBundleStatusCallback due to write InterfaceToken fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::UNREGISTER_BUNDLE_STATUS_CALLBACK, data, reply)) {
        APP_LOGE("fail to UnregisterBundleStatusCallback from server");
        return false;
    }
    return reply.ReadBool();
}

bool BundleMgrProxy::DumpInfos(const DumpFlag flag, const std::string &bundleName, std::string &result)
{
    APP_LOGD("begin to dump");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to dump due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(static_cast<int>(flag))) {
        APP_LOGE("fail to dump due to write flag fail");
        return false;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("fail to dump due to write bundleName fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::DUMP_INFOS, data, reply)) {
        APP_LOGE("fail to dump from server");
        return false;
    }
    if (!reply.ReadBool()) {
        APP_LOGE("readParcelableInfo failed");
        return false;
    }
    std::vector<std::string> dumpInfos;
    if (!reply.ReadStringVector(&dumpInfos)) {
        APP_LOGE("fail to dump from reply");
        return false;
    }
    for (auto &dumpinfo : dumpInfos) {
        result += dumpinfo;
    }
    return true;
}

bool BundleMgrProxy::IsApplicationEnabled(const std::string &bundleName)
{
    APP_LOGI("begin to IsApplicationEnabled of %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("fail to IsApplicationEnabled due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to IsApplicationEnabled due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("fail to IsApplicationEnabled due to write bundleName fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::IS_APPLICATION_ENABLED, data, reply)) {
        APP_LOGE("fail to IsApplicationEnabled from server");
        return false;
    }
    return reply.ReadBool();
}

bool BundleMgrProxy::SetApplicationEnabled(const std::string &bundleName, bool isEnable)
{
    APP_LOGI("begin to SetApplicationEnabled of %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("fail to SetApplicationEnabled due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to SetApplicationEnabled due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("fail to SetApplicationEnabled due to write bundleName fail");
        return false;
    }
    if (!data.WriteBool(isEnable)) {
        APP_LOGE("fail to IsApplicationEnabled due to write isEnable fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::SET_APPLICATION_ENABLED, data, reply)) {
        APP_LOGE("fail to SetApplicationEnabled from server");
        return false;
    }
    return reply.ReadBool();
}

sptr<IBundleInstaller> BundleMgrProxy::GetBundleInstaller()
{
    APP_LOGD("begin to get bundle installer");
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetBundleInstaller due to write InterfaceToken fail");
        return nullptr;
    }
    if (!SendTransactCmd(IBundleMgr::Message::GET_BUNDLE_INSTALLER, data, reply)) {
        return nullptr;
    }

    sptr<IRemoteObject> object = reply.ReadParcelable<IRemoteObject>();
    sptr<IBundleInstaller> installer = iface_cast<IBundleInstaller>(object);
    if (!installer) {
        APP_LOGE("bundle installer is nullptr");
    }

    APP_LOGD("get bundle installer success");
    return installer;
}

bool BundleMgrProxy::CanRequestPermission(
    const std::string &bundleName, const std::string &permissionName, const int userId)
{
    APP_LOGI("begin to CanRequestPermission of %{public}s and %{public}s", bundleName.c_str(), permissionName.c_str());
    if (bundleName.empty() || permissionName.empty()) {
        APP_LOGE("fail to CanRequestPermission due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to CanRequestPermission due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("fail to CanRequestPermission due to write bundleName fail");
        return false;
    }
    if (!data.WriteString(permissionName)) {
        APP_LOGE("fail to CanRequestPermission due to write permission fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        APP_LOGE("fail to CanRequestPermission due to write userId fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::CAN_REQUEST_PERMISSION, data, reply)) {
        APP_LOGE("fail to CanRequestPermission from server");
        return false;
    }
    return reply.ReadBool();
}

bool BundleMgrProxy::RequestPermissionFromUser(
    const std::string &bundleName, const std::string &permission, const int userId)
{
    APP_LOGI("begin to RequestPermissionsFromUser of %{public}s", bundleName.c_str());
    if (bundleName.empty() || permission.empty()) {
        APP_LOGE("fail to RequestPermissionsFromUser due to params empty");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to RequestPermissionsFromUser due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("fail to RequestPermissionsFromUser due to write bundleName fail");
        return false;
    }
    if (!data.WriteString(permission)) {
        APP_LOGE("fail to RequestPermissionsFromUser due to write permission fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        APP_LOGE("fail to RequestPermissionsFromUser due to write userId fail");
        return false;
    }

    MessageParcel reply;
    if (!SendTransactCmd(IBundleMgr::Message::CAN_REQUEST_PERMISSION, data, reply)) {
        APP_LOGE("fail to RequestPermissionsFromUser from server");
        return false;
    }
    return reply.ReadBool();
}

template<typename T>
bool BundleMgrProxy::GetParcelableInfo(IBundleMgr::Message code, MessageParcel &data, T &parcelableInfo)
{
    MessageParcel reply;
    if (!SendTransactCmd(code, data, reply)) {
        return false;
    }

    if (!reply.ReadBool()) {
        APP_LOGE("reply result false");
        return false;
    }

    std::unique_ptr<T> info(reply.ReadParcelable<T>());
    if (!info) {
        APP_LOGE("readParcelableInfo failed");
        return false;
    }
    parcelableInfo = *info;
    APP_LOGD("get parcelable info success");
    return true;
}

template<typename T>
bool BundleMgrProxy::GetParcelableInfos(IBundleMgr::Message code, MessageParcel &data, std::vector<T> &parcelableInfos)
{
    MessageParcel reply;
    if (!SendTransactCmd(code, data, reply)) {
        return false;
    }

    if (!reply.ReadBool()) {
        APP_LOGE("readParcelableInfo failed");
        return false;
    }

    int32_t infoSize = reply.ReadInt32();
    for (int32_t i = 0; i < infoSize; i++) {
        std::unique_ptr<T> info(reply.ReadParcelable<T>());
        if (!info) {
            APP_LOGE("Read Parcelable infos failed");
            return false;
        }
        parcelableInfos.emplace_back(*info);
    }
    APP_LOGD("get parcelable infos success");
    return true;
}

bool BundleMgrProxy::SendTransactCmd(IBundleMgr::Message code, MessageParcel &data, MessageParcel &reply)
{
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        APP_LOGE("fail to send transact cmd %{public}d due to remote object", code);
        return false;
    }
    int32_t result = remote->SendRequest(static_cast<uint32_t>(code), data, reply, option);
    if (result != NO_ERROR) {
        APP_LOGE("receive error transact code %{public}d in transact cmd %{public}d", result, code);
        return false;
    }
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS
