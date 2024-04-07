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

#include "bundle_mgr_host.h"

#include <cinttypes>

#include "datetime_ex.h"
#include "ipc_types.h"
#include "string_ex.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {
namespace {

const int32_t LIMIT_PARCEL_SIZE = 1024;

void SplitString(const std::string &source, std::vector<std::string> &strings)
{
    int splitSize = (source.size() / LIMIT_PARCEL_SIZE);
    if ((source.size() % LIMIT_PARCEL_SIZE) != 0) {
        splitSize++;
    }
    APP_LOGD("the dump string split into %{public}d size", splitSize);
    for (int i = 0; i < splitSize; i++) {
        int32_t start = LIMIT_PARCEL_SIZE * i;
        strings.emplace_back(source.substr(start, LIMIT_PARCEL_SIZE));
    }
}

}  // namespace

int BundleMgrHost::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_LOGD("bundle mgr host onReceived message, the message code is %{public}u", code);
    std::u16string descripter = BundleMgrHost::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        APP_LOGE("fail to write reply message in bundle mgr host due to the reply is nullptr");
        return OBJECT_NULL;
    }

    ErrCode errCode = ERR_OK;
    switch (code) {
        case static_cast<uint32_t>(IBundleMgr::Message::GET_APPLICATION_INFO):
            errCode = HandleGetApplicationInfo(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_APPLICATION_INFOS):
            errCode = HandleGetApplicationInfos(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_BUNDLE_INFO):
            errCode = HandleGetBundleInfo(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_BUNDLE_INFOS):
            errCode = HandleGetBundleInfos(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_BUNDLE_NAME_FOR_UID):
            errCode = HandleGetBundleNameForUid(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_BUNDLE_GIDS):
            errCode = HandleGetBundleGids(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_BUNDLE_INFOS_BY_METADATA):
            errCode = HandleGetBundleInfosByMetaData(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::QUERY_ABILITY_INFO):
            errCode = HandleQueryAbilityInfo(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::QUERY_ABILITY_INFO_BY_URI):
            errCode = HandleQueryAbilityInfoByUri(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::QUERY_KEEPALIVE_BUNDLE_INFOS):
            errCode = HandleQueryKeepAliveBundleInfos(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_ABILITY_LABEL):
            errCode = HandleGetAbilityLabel(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::CHECK_IS_SYSTEM_APP_BY_UID):
            errCode = HandleCheckIsSystemAppByUid(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_BUNDLE_ARCHIVE_INFO):
            errCode = HandleGetBundleArchiveInfo(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_HAP_MODULE_INFO):
            errCode = HandleGetHapModuleInfo(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_LAUNCH_WANT_FOR_BUNDLE):
            errCode = HandleGetLaunchWantForBundle(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::CHECK_PUBLICKEYS):
            errCode = HandleGetApplicationInfo(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::CHECK_PERMISSION):
            errCode = HandleCheckPermission(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_PERMISSION_DEF):
            errCode = HandleGetPermissionDef(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_ALL_PERMISSION_GROUP_DEFS):
            errCode = HandleGetAllPermissionGroupDefs(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_APPS_GRANTED_PERMISSIONS):
            errCode = HandleGetAppsGrantedPermissions(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::HAS_SYSTEM_CAPABILITY):
            errCode = HandleHasSystemCapability(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_SYSTEM_AVAILABLE_CAPABILITIES):
            errCode = HandleGetSystemAvailableCapabilities(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::IS_SAFE_MODE):
            errCode = HandleIsSafeMode(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::CLEAN_BUNDLE_CACHE_FILES):
            errCode = HandleCleanBundleCacheFiles(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::CLEAN_BUNDLE_DATA_FILES):
            errCode = HandleCleanBundleDataFiles(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::REGISTER_BUNDLE_STATUS_CALLBACK):
            errCode = HandleRegisterBundleStatusCallback(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::CLEAR_BUNDLE_STATUS_CALLBACK):
            errCode = HandleClearBundleStatusCallback(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::UNREGISTER_BUNDLE_STATUS_CALLBACK):
            errCode = HandleUnregisterBundleStatusCallback(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::DUMP_INFOS):
            errCode = HandleDumpInfos(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::GET_BUNDLE_INSTALLER):
            errCode = HandleGetBundleInstaller(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::CAN_REQUEST_PERMISSION):
            errCode = HandleCanRequestPermission(data, reply);
            break;
        case static_cast<uint32_t>(IBundleMgr::Message::REQUEST_PERMISSION_FROM_USER):
            errCode = HandleRequestPermissionFromUser(data, reply);
            break;
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    // if ERR_OK return ipc NO_ERROR, else return ipc UNKNOW_ERROR
    return (errCode == ERR_OK) ? NO_ERROR : UNKNOWN_ERROR;
}

int BundleMgrHost::GetUidByBundleName([[maybe_unused]] const std::string &bundleName, const int userId)
{
    APP_LOGD("need not impl for host interface");
    return Constants::INVALID_UID;
}

std::string BundleMgrHost::GetAppType([[maybe_unused]] const std::string &bundleName)
{
    APP_LOGD("need not impl for host interface");
    return Constants::EMPTY_STRING;
}

ErrCode BundleMgrHost::HandleGetApplicationInfo(Parcel &data, Parcel &reply)
{
    std::string name = data.ReadString();
    ApplicationFlag flag = static_cast<ApplicationFlag>(data.ReadInt32());
    int userId = data.ReadInt32();
    APP_LOGI("name %{public}s, flag %{public}d, userId %{public}d", name.c_str(), flag, userId);

    ApplicationInfo info;
    bool ret = GetApplicationInfo(name, flag, userId, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetApplicationInfos(Parcel &data, Parcel &reply)
{
    ApplicationFlag flag = static_cast<ApplicationFlag>(data.ReadInt32());
    int userId = data.ReadInt32();
    std::vector<ApplicationInfo> infos;
    bool ret = GetApplicationInfos(flag, userId, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleInfo(Parcel &data, Parcel &reply)
{
    std::string name = data.ReadString();
    BundleFlag flag = static_cast<BundleFlag>(data.ReadInt32());
    APP_LOGI("name %{public}s, flag %{public}d", name.c_str(), flag);
    BundleInfo info;
    bool ret = GetBundleInfo(name, flag, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleInfos(Parcel &data, Parcel &reply)
{
    BundleFlag flag = static_cast<BundleFlag>(data.ReadInt32());

    std::vector<BundleInfo> infos;
    bool ret = GetBundleInfos(flag, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleNameForUid(Parcel &data, Parcel &reply)
{
    int uid = data.ReadInt32();
    std::string name;
    bool ret = GetBundleNameForUid(uid, name);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteString(name)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleGids(Parcel &data, Parcel &reply)
{
    std::string name = data.ReadString();

    std::vector<int> gids;
    bool ret = GetBundleGids(name, gids);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteInt32Vector(gids)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleInfosByMetaData(Parcel &data, Parcel &reply)
{
    std::string metaData = data.ReadString();

    std::vector<BundleInfo> infos;
    bool ret = GetBundleInfosByMetaData(metaData, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryAbilityInfo(Parcel &data, Parcel &reply)
{
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        APP_LOGE("ReadParcelable<want> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    AbilityInfo info;
    bool ret = QueryAbilityInfo(*want, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryAbilityInfoByUri(Parcel &data, Parcel &reply)
{
    std::string abilityUri = data.ReadString();
    AbilityInfo info;
    bool ret = QueryAbilityInfoByUri(abilityUri, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryKeepAliveBundleInfos(Parcel &data, Parcel &reply)
{
    std::vector<BundleInfo> infos;
    bool ret = QueryKeepAliveBundleInfos(infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAbilityLabel(Parcel &data, Parcel &reply)
{
    std::string bundleName = data.ReadString();
    std::string className = data.ReadString();

    APP_LOGI("bundleName %{public}s, className %{public}s", bundleName.c_str(), className.c_str());
    BundleInfo info;
    std::string label = GetAbilityLabel(bundleName, className);
    if (!reply.WriteString16(Str8ToStr16(label))) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleCheckIsSystemAppByUid(Parcel &data, Parcel &reply)
{
    int uid = data.ReadInt32();
    bool ret = CheckIsSystemAppByUid(uid);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleArchiveInfo(Parcel &data, Parcel &reply)
{
    std::string hapFilePath = data.ReadString();
    BundleFlag flag = static_cast<BundleFlag>(data.ReadInt32());
    APP_LOGI("name %{public}s, flag %{public}d", hapFilePath.c_str(), flag);

    BundleInfo info;
    bool ret = GetBundleArchiveInfo(hapFilePath, flag, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetHapModuleInfo(Parcel &data, Parcel &reply)
{
    std::unique_ptr<AbilityInfo> abilityInfo(data.ReadParcelable<AbilityInfo>());
    if (!abilityInfo) {
        APP_LOGE("ReadParcelable<abilityInfo> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    HapModuleInfo info;
    bool ret = GetHapModuleInfo(*abilityInfo, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetLaunchWantForBundle(Parcel &data, Parcel &reply)
{
    std::string bundleName = data.ReadString();
    APP_LOGI("name %{public}s", bundleName.c_str());

    Want want;
    bool ret = GetLaunchWantForBundle(bundleName, want);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&want)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleCheckPublicKeys(Parcel &data, Parcel &reply)
{
    std::string firstBundleName = data.ReadString();
    std::string secondBundleName = data.ReadString();

    APP_LOGI(
        "firstBundleName %{public}s, secondBundleName %{public}s", firstBundleName.c_str(), secondBundleName.c_str());
    int ret = CheckPublicKeys(firstBundleName, secondBundleName);
    if (!reply.WriteInt32(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleCheckPermission(Parcel &data, Parcel &reply)
{
    std::string bundleName = data.ReadString();
    std::string permission = data.ReadString();

    APP_LOGI("bundleName %{public}s, permission %{public}s", bundleName.c_str(), permission.c_str());
    int ret = CheckPermission(bundleName, permission);
    if (!reply.WriteInt32(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetPermissionDef(Parcel &data, Parcel &reply)
{
    std::string permissionName = data.ReadString();
    APP_LOGI("name %{public}s", permissionName.c_str());

    PermissionDef permissionDef;
    bool ret = GetPermissionDef(permissionName, permissionDef);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&permissionDef)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAllPermissionGroupDefs(Parcel &data, Parcel &reply)
{
    std::vector<PermissionDef> permissionDefs;
    bool ret = GetAllPermissionGroupDefs(permissionDefs);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(permissionDefs, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAppsGrantedPermissions(Parcel &data, Parcel &reply)
{
    std::vector<std::string> permissions;
    if (data.ReadStringVector(&permissions)) {
        APP_LOGE("read failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    std::vector<std::string> appNames;
    bool ret = GetAppsGrantedPermissions(permissions, appNames);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteStringVector(appNames)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleHasSystemCapability(Parcel &data, Parcel &reply)
{
    std::string capName = data.ReadString();

    bool ret = HasSystemCapability(capName);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetSystemAvailableCapabilities(Parcel &data, Parcel &reply)
{
    std::vector<std::string> caps;
    bool ret = GetSystemAvailableCapabilities(caps);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteStringVector(caps)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleIsSafeMode(Parcel &data, Parcel &reply)
{
    bool ret = IsSafeMode();
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleCleanBundleCacheFiles(Parcel &data, Parcel &reply)
{
    std::string bundleName = data.ReadString();
    sptr<IRemoteObject> object = data.ReadParcelable<IRemoteObject>();
    sptr<ICleanCacheCallback> cleanCacheCallback = iface_cast<ICleanCacheCallback>(object);

    bool ret = CleanBundleCacheFiles(bundleName, cleanCacheCallback);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleCleanBundleDataFiles(Parcel &data, Parcel &reply)
{
    std::string bundleName = data.ReadString();

    bool ret = CleanBundleDataFiles(bundleName);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleRegisterBundleStatusCallback(Parcel &data, Parcel &reply)
{
    std::string bundleName = data.ReadString();
    sptr<IRemoteObject> object = data.ReadParcelable<IRemoteObject>();
    sptr<IBundleStatusCallback> BundleStatusCallback = iface_cast<IBundleStatusCallback>(object);

    bool ret = false;
    if (bundleName.empty() || !BundleStatusCallback) {
        APP_LOGE("Get BundleStatusCallback failed");
    } else {
        BundleStatusCallback->SetBundleName(bundleName);
        ret = RegisterBundleStatusCallback(BundleStatusCallback);
    }
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleClearBundleStatusCallback(Parcel &data, Parcel &reply)
{
    sptr<IRemoteObject> object = data.ReadParcelable<IRemoteObject>();
    sptr<IBundleStatusCallback> BundleStatusCallback = iface_cast<IBundleStatusCallback>(object);

    bool ret = ClearBundleStatusCallback(BundleStatusCallback);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleUnregisterBundleStatusCallback(Parcel &data, Parcel &reply)
{
    bool ret = UnregisterBundleStatusCallback();
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleDumpInfos(Parcel &data, Parcel &reply)
{
    DumpFlag flag = static_cast<DumpFlag>(data.ReadInt32());
    std::string bundleName = data.ReadString();

    std::string result;
    bool ret = DumpInfos(flag, bundleName, result);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        std::vector<std::string> dumpInfos;
        SplitString(result, dumpInfos);
        if (!reply.WriteStringVector(dumpInfos)) {
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleIsApplicationEnabled(Parcel &data, Parcel &reply)
{
    std::string bundleName = data.ReadString();
    bool ret = IsApplicationEnabled(bundleName);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleSetApplicationEnabled(Parcel &data, Parcel &reply)
{
    std::string bundleName = data.ReadString();
    bool isEnable = data.ReadBool();
    bool ret = SetApplicationEnabled(bundleName, isEnable);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleCanRequestPermission(Parcel &data, Parcel &reply)
{
    std::string bundleName = data.ReadString();
    std::string permission = data.ReadString();
    int32_t userId = data.ReadInt32();

    APP_LOGI("bundleName %{public}s, permission %{public}s", bundleName.c_str(), permission.c_str());
    bool ret = CanRequestPermission(bundleName, permission, userId);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleRequestPermissionFromUser(Parcel &data, Parcel &reply)
{
    std::string bundleName = data.ReadString();
    std::string permission = data.ReadString();
    int32_t userId = data.ReadInt32();

    APP_LOGI("bundleName %{public}s, permission %{public}s", bundleName.c_str(), permission.c_str());
    bool ret = RequestPermissionFromUser(bundleName, permission, userId);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleInstaller(Parcel &data, Parcel &reply)
{
    sptr<IBundleInstaller> installer = GetBundleInstaller();
    if (!installer) {
        APP_LOGE("bundle installer is nullptr");
        return ERR_APPEXECFWK_INSTALL_HOST_INSTALLER_FAILED;
    }

    if (!reply.WriteParcelable(installer->AsObject())) {
        APP_LOGE("failed to reply bundle installer to client, for write parcel error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

template<typename T>
bool BundleMgrHost::WriteParcelableVector(std::vector<T> &parcelableVector, Parcel &reply)
{
    if (!reply.WriteInt32(parcelableVector.size())) {
        APP_LOGE("write ParcelableVector failed");
        return false;
    }

    for (auto &parcelable : parcelableVector) {
        if (!reply.WriteParcelable(&parcelable)) {
            APP_LOGE("write ParcelableVector failed");
            return false;
        }
    }
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS
