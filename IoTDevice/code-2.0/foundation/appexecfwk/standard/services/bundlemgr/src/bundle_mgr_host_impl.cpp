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

#include "bundle_mgr_host_impl.h"

#include <future>

#include "json_serializer.h"
#include "app_log_wrapper.h"
#include "bundle_mgr_service.h"
#include "bundle_util.h"
#include "bundle_parser.h"
#include "installd_client.h"
#include "bundle_permission_mgr.h"

namespace OHOS {
namespace AppExecFwk {

bool BundleMgrHostImpl::GetApplicationInfo(
    const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->GetApplicationInfo(appName, flag, userId, appInfo);
}

bool BundleMgrHostImpl::GetApplicationInfos(
    const ApplicationFlag flag, const int userId, std::vector<ApplicationInfo> &appInfos)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->GetApplicationInfos(flag, userId, appInfos);
}

bool BundleMgrHostImpl::GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->GetBundleInfo(bundleName, flag, bundleInfo);
}

bool BundleMgrHostImpl::GetBundleInfos(const BundleFlag flag, std::vector<BundleInfo> &bundleInfos)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->GetBundleInfos(flag, bundleInfos);
}

bool BundleMgrHostImpl::GetBundleNameForUid(const int uid, std::string &bundleName)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->GetBundleNameForUid(uid, bundleName);
}

bool BundleMgrHostImpl::GetBundleGids(const std::string &bundleName, std::vector<int> &gids)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->GetBundleGids(bundleName, gids);
}

bool BundleMgrHostImpl::CheckIsSystemAppByUid(const int uid)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->CheckIsSystemAppByUid(uid);
}

bool BundleMgrHostImpl::GetBundleInfosByMetaData(const std::string &metaData, std::vector<BundleInfo> &bundleInfos)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->GetBundleInfosByMetaData(metaData, bundleInfos);
}

bool BundleMgrHostImpl::QueryAbilityInfo(const Want &want, AbilityInfo &abilityInfo)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->QueryAbilityInfo(want, abilityInfo);
}

bool BundleMgrHostImpl::QueryAbilityInfoByUri(const std::string &abilityUri, AbilityInfo &abilityInfo)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->QueryAbilityInfoByUri(abilityUri, abilityInfo);
}

bool BundleMgrHostImpl::QueryKeepAliveBundleInfos(std::vector<BundleInfo> &bundleInfos)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->QueryKeepAliveBundleInfos(bundleInfos);
}

std::string BundleMgrHostImpl::GetAbilityLabel(const std::string &bundleName, const std::string &className)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return Constants::EMPTY_STRING;
    }
    return dataMgr->GetAbilityLabel(bundleName, className);
}

bool BundleMgrHostImpl::GetBundleArchiveInfo(
    const std::string &hapFilePath, const BundleFlag flag, BundleInfo &bundleInfo)
{
    std::string realPath;
    auto ret = BundleUtil::CheckFilePath(hapFilePath, realPath);
    if (ret != ERR_OK) {
        APP_LOGE("GetBundleArchiveInfo file path %{public}s invalid", hapFilePath.c_str());
        return false;
    }
    InnerBundleInfo info;
    BundleParser bundleParser;
    ret = bundleParser.Parse(realPath, info);
    if (ret != ERR_OK) {
        APP_LOGE("parse bundle info failed, error: %{public}d", ret);
        return false;
    }
    info.GetBundleInfo(flag, bundleInfo);
    return true;
}

bool BundleMgrHostImpl::GetHapModuleInfo(const AbilityInfo &abilityInfo, HapModuleInfo &hapModuleInfo)
{
    if (abilityInfo.bundleName.empty() || abilityInfo.package.empty()) {
        APP_LOGE("fail to GetHapModuleInfo due to params empty");
        return false;
    }
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->GetHapModuleInfo(abilityInfo, hapModuleInfo);
}

bool BundleMgrHostImpl::GetLaunchWantForBundle(const std::string &bundleName, Want &want)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->GetLaunchWantForBundle(bundleName, want);
}

int BundleMgrHostImpl::CheckPublicKeys(const std::string &firstBundleName, const std::string &secondBundleName)
{
    return -1;
}

int BundleMgrHostImpl::CheckPermission(const std::string &bundleName, const std::string &permission)
{
    if (bundleName.empty() || permission.empty()) {
        APP_LOGE("fail to CheckPermission due to params empty");
        return Constants::PERMISSION_NOT_GRANTED;
    }
    return BundlePermissionMgr::VerifyPermission(bundleName, permission, Constants::DEFAULT_USERID);
}

bool BundleMgrHostImpl::GetPermissionDef(const std::string &permissionName, PermissionDef &permissionDef)
{
    if (permissionName.empty()) {
        APP_LOGE("fail to GetPermissionDef due to params empty");
        return false;
    }
    return BundlePermissionMgr::GetPermissionDef(permissionName, permissionDef);
}

bool BundleMgrHostImpl::GetAllPermissionGroupDefs(std::vector<PermissionDef> &permissionDefs)
{
    return true;
}

bool BundleMgrHostImpl::GetAppsGrantedPermissions(
    const std::vector<std::string> &permissions, std::vector<std::string> &appNames)
{
    return true;
}

bool BundleMgrHostImpl::HasSystemCapability(const std::string &capName)
{
    return true;
}

bool BundleMgrHostImpl::GetSystemAvailableCapabilities(std::vector<std::string> &systemCaps)
{
    return true;
}

bool BundleMgrHostImpl::IsSafeMode()
{
    return true;
}

bool BundleMgrHostImpl::CleanBundleCacheFiles(
    const std::string &bundleName, const sptr<ICleanCacheCallback> &cleanCacheCallback)
{
    if (!cleanCacheCallback || bundleName.empty()) {
        APP_LOGE("the cleanCacheCallback is nullptr or bundleName empty");
        return false;
    }
    ApplicationInfo applicationInfo;
    if (!GetApplicationInfo(bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, 0, applicationInfo)) {
        APP_LOGE("can not get application info of %{public}s", bundleName.c_str());
        return false;
    }
    auto cacheDir = applicationInfo.cacheDir;
    std::thread([cacheDir, cleanCacheCallback]() {
        auto ret = InstalldClient::GetInstance()->CleanBundleDataDir(cacheDir);
        cleanCacheCallback->OnCleanCacheFinished((ret == ERR_OK) ? true : false);
    }).detach();
    return true;
}

bool BundleMgrHostImpl::CleanBundleDataFiles(const std::string &bundleName)
{
    if (bundleName.empty()) {
        APP_LOGE("the  bundleName empty");
        return false;
    }
    ApplicationInfo applicationInfo;
    if (!GetApplicationInfo(bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, 0, applicationInfo)) {
        APP_LOGE("can not get application info of %{public}s", bundleName.c_str());
        return false;
    }
    if (InstalldClient::GetInstance()->CleanBundleDataDir(applicationInfo.dataDir) != ERR_OK) {
        return false;
    }
    return true;
}

bool BundleMgrHostImpl::RegisterBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback)
{
    if ((!bundleStatusCallback) || (bundleStatusCallback->GetBundleName().empty())) {
        APP_LOGE("the bundleStatusCallback is nullptr or bundleName empty");
        return false;
    }
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->RegisterBundleStatusCallback(bundleStatusCallback);
}

bool BundleMgrHostImpl::ClearBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback)
{
    if (!bundleStatusCallback) {
        APP_LOGE("the bundleStatusCallback is nullptr");
        return false;
    }
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->ClearBundleStatusCallback(bundleStatusCallback);
}

bool BundleMgrHostImpl::UnregisterBundleStatusCallback()
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->UnregisterBundleStatusCallback();
}

bool BundleMgrHostImpl::DumpInfos(const DumpFlag flag, const std::string &bundleName, std::string &result)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    bool ret = false;
    switch (flag) {
        case DumpFlag::DUMP_BUNDLE_LIST: {
            std::vector<std::string> bundleNames;
            ret = dataMgr->GetBundleList(bundleNames);
            if (ret) {
                for (const auto &name : bundleNames) {
                    result.append(name);
                    result.append("\n");
                }
                APP_LOGI("get installed bundles success");
            }
            break;
        }
        case DumpFlag::DUMP_ALL_BUNDLE_INFO: {
            std::vector<BundleInfo> bundleInfos;
            ret = GetBundleInfos(BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfos);
            if (ret) {
                for (const auto &info : bundleInfos) {
                    result.append(info.name);
                    result.append(":\n");
                    nlohmann::json jsonObject = info;
                    result.append(jsonObject.dump(Constants::DUMP_INDENT));
                    result.append("\n");
                }
                APP_LOGI("get all bundle info success");
            }
            break;
        }
        case DumpFlag::DUMP_BUNDLE_INFO: {
            BundleInfo bundleInfo;
            ret = GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
            if (ret) {
                result.append(bundleName);
                result.append(":\n");
                nlohmann::json jsonObject = bundleInfo;
                result.append(jsonObject.dump(Constants::DUMP_INDENT));
                result.append("\n");
                APP_LOGI("get %{public}s bundle info success", bundleName.c_str());
            }
            break;
        }
        default:
            APP_LOGE("dump flag error");
            return false;
    }
    return ret;
}

bool BundleMgrHostImpl::IsApplicationEnabled(const std::string &bundleName)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->IsApplicationEnabled(bundleName);
}

bool BundleMgrHostImpl::SetApplicationEnabled(const std::string &bundleName, bool isEnable)
{
    auto dataMgr = GetDataMgrFromService();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }
    return dataMgr->SetApplicationEnabled(bundleName, isEnable);
}

sptr<IBundleInstaller> BundleMgrHostImpl::GetBundleInstaller()
{
    return DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
}

bool BundleMgrHostImpl::CanRequestPermission(
    const std::string &bundleName, const std::string &permissionName, const int userId)
{
    if (bundleName.empty() || permissionName.empty()) {
        APP_LOGE("fail to CanRequestPermission due to params empty");
        return false;
    }
    return BundlePermissionMgr::CanRequestPermission(bundleName, permissionName, userId);
}

bool BundleMgrHostImpl::RequestPermissionFromUser(
    const std::string &bundleName, const std::string &permissionName, const int userId)
{
    if (bundleName.empty() || permissionName.empty()) {
        APP_LOGE("fail to CanRequestPermission due to params empty");
        return false;
    }
    return BundlePermissionMgr::RequestPermissionFromUser(bundleName, permissionName, userId);
}

const std::shared_ptr<BundleDataMgr> BundleMgrHostImpl::GetDataMgrFromService()
{
    return DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
}

}  // namespace AppExecFwk
}  // namespace OHOS
