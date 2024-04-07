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

#include "bundle_permission_mgr.h"

#include "ipc_skeleton.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_mgr_service.h"

namespace OHOS {
namespace AppExecFwk {

using namespace OHOS::Security;

namespace {
const std::string HOS_NORMAL_APP = "hos_normal_app";
const std::string HOS_SYSTEM_APP = "hos_system_app";
// convert the Permission::PermissionDef struct to
// AppExecFwk::PermissionDef struct that can be used in IPC process
bool ConvertPermissionDef(const Permission::PermissionDef &permDef, PermissionDef &permissionDef)
{
    permissionDef.permissionName = permDef.permissionName;
    permissionDef.bundleName = permDef.bundleName;
    permissionDef.grantMode = permDef.grantMode;
    permissionDef.availableScope = permDef.availableScope;
    permissionDef.label = permDef.label;
    permissionDef.labelId = permDef.labelId;
    permissionDef.description = permDef.description;
    permissionDef.descriptionId = permDef.descriptionId;
    return true;
}

// Convert from the struct DefPermission that parsed from config.json
bool ConvertPermissionDef(
    Permission::PermissionDef &permDef, const DefPermission &defPermission, const std::string &bundleName)
{
    permDef.permissionName = defPermission.name;
    permDef.bundleName = bundleName;
    permDef.grantMode = [&defPermission]() -> int {
        if (defPermission.grantMode ==
            ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_GRANTMODE_SYSTEM_GRANT) {
            return Permission::GrantMode::SYSTEM_GRANT;
        }
        return Permission::GrantMode::USER_GRANT;
    }();
    permDef.availableScope = [&defPermission]() -> int {
        unsigned flag = 0;
        if (std::find(defPermission.availableScope.begin(),
                      defPermission.availableScope.end(),
                      ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE_SIGNATURE) !=
                      defPermission.availableScope.end()) {
            flag |= Permission::AvailableScope::AVAILABLE_SCOPE_SIGNATURE;
        }
        if (std::find(defPermission.availableScope.begin(),
                      defPermission.availableScope.end(),
                      ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE_RESTRICTED) !=
                      defPermission.availableScope.end()) {
            flag |= Permission::AvailableScope::AVAILABLE_SCOPE_RESTRICTED;
        }
        if (flag == 0) {
            return Permission::AvailableScope::AVAILABLE_SCOPE_ALL;
        }
        return flag;
    }();
    permDef.label = defPermission.label;
    permDef.labelId = defPermission.labelId;
    permDef.description = defPermission.description;
    permDef.descriptionId = defPermission.descriptionId;
    return true;
}

}  // namespace

bool BundlePermissionMgr::InitPermissions()
{
    // need load all system defined permissions here on first start up.
    return true;
}

int BundlePermissionMgr::AddDefPermissions(const InnerBundleInfo &innerBundleInfo)
{
    const auto bundleName = innerBundleInfo.GetBundleName();
    const auto defPermissions = innerBundleInfo.GetDefPermissions();
    int ret = Permission::RET_FAILED;
    if (!defPermissions.empty()) {
        std::vector<Permission::PermissionDef> permList;
        for (const auto &defPermission : defPermissions) {
            Permission::PermissionDef perm;
            APP_LOGI("add defPermission %{public}s", defPermission.name.c_str());
            ConvertPermissionDef(perm, defPermission, bundleName);
            permList.emplace_back(perm);
        }
        if (!permList.empty()) {
            ret = Permission::PermissionKit::AddDefPermissions(permList);
        }
    }
    return ret;
}

bool BundlePermissionMgr::CheckPermissionAuthorization(
    const Permission::PermissionDef &permissionDef, const InnerBundleInfo &innerBundleInfo)
{
    APP_LOGI("availableScope type is %{public}d", permissionDef.availableScope);
    if (permissionDef.availableScope == Permission::TypeAvailableScope::AVAILABLE_SCOPE_ALL) {
        return true;
    }
    if (permissionDef.availableScope != Permission::TypeAvailableScope::AVAILABLE_SCOPE_SIGNATURE) {
        return false;
    }
    auto appFeature = innerBundleInfo.GetAppFeature();
    APP_LOGI("appFeature is %{public}s", appFeature.c_str());
    APP_LOGI("permission name is %{public}s", permissionDef.permissionName.c_str());
    auto dataMgr = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (!dataMgr) {
        APP_LOGE("Get dataMgr shared_ptr nullptr");
        return false;
    }
    if (appFeature == HOS_NORMAL_APP) {
        std::string provisionId;
        bool result = dataMgr->GetProvisionId(permissionDef.bundleName, provisionId);
        if (result && (provisionId == innerBundleInfo.GetProvisionId())) {
            APP_LOGI("provisionId is same");
            return true;
        }
    } else if (appFeature == HOS_SYSTEM_APP) {
        std::string oldAppFeature;
        bool result = dataMgr->GetAppFeature(permissionDef.bundleName, oldAppFeature);
        APP_LOGI("oldAppFeature %{public}s", oldAppFeature.c_str());
        if (result && (oldAppFeature == HOS_SYSTEM_APP)) {
            APP_LOGI("appFeature is same");
            return true;
        }
    }
    return false;
}

int BundlePermissionMgr::AddAndGrantedReqPermissions(const InnerBundleInfo &innerBundleInfo)
{
    auto reqPermissions = innerBundleInfo.GetReqPermissions();
    std::vector<std::string> userPermList;
    std::vector<std::string> systemPermList;
    std::vector<std::string> grantPermList;
    for (const auto &reqPermission : reqPermissions) {
        APP_LOGI("add permission %{public}s", reqPermission.name.c_str());
        Permission::PermissionDef permDef;
        int ret = Permission::PermissionKit::GetDefPermission(reqPermission.name, permDef);
        if (ret != Permission::RET_SUCCESS) {
            APP_LOGE("get permission def failed");
            continue;
        }
        if (permDef.grantMode == Permission::GrantMode::USER_GRANT) {
            userPermList.emplace_back(reqPermission.name);
            continue;
        }
        if (permDef.grantMode == Permission::GrantMode::SYSTEM_GRANT) {
            systemPermList.emplace_back(reqPermission.name);
            if (CheckPermissionAuthorization(permDef, innerBundleInfo)) {
                grantPermList.emplace_back(reqPermission.name);
            }
            continue;
        }
    }
    std::string bundleName = innerBundleInfo.GetBundleName();
    APP_LOGI("add permission %{public}s %{public}zu  %{public}zu  %{public}zu",
        bundleName.c_str(),
        userPermList.size(),
        systemPermList.size(),
        grantPermList.size());
    if (!userPermList.empty()) {
        auto ret = AddUserGrantedReqPermissions(bundleName, userPermList, Constants::DEFAULT_USERID);
        if (ret != Permission::RET_SUCCESS) {
            APP_LOGE("AddUserGrantedReqPermissions failed");
        }
    }
    if (!systemPermList.empty()) {
        auto ret = AddSystemGrantedReqPermissions(bundleName, systemPermList);
        if (ret != Permission::RET_SUCCESS) {
            APP_LOGE("AddUserGrantedReqPermissions failed");
        }
    }
    for (const auto &perm : grantPermList) {
        auto ret = GrantReqPermissions(bundleName, perm);
        if (ret != Permission::RET_SUCCESS) {
            APP_LOGE("GrantReqPermissions failed");
        }
    }
    return Permission::RET_SUCCESS;
}

bool BundlePermissionMgr::InstallPermissions(const InnerBundleInfo &innerBundleInfo)
{
    int ret = AddDefPermissions(innerBundleInfo);
    if (ret != Permission::RET_SUCCESS) {
        APP_LOGE("AddDefPermissions ret %{public}d", ret);
    }
    ret = AddAndGrantedReqPermissions(innerBundleInfo);
    if (ret != Permission::RET_SUCCESS) {
        APP_LOGE("AddUserGrantedReqPermissions ret %{public}d", ret);
    }
    return true;
}

bool BundlePermissionMgr::UpdatePermissions(const InnerBundleInfo &innerBundleInfo)
{
    // at current time the update permissions process is same as installation process.
    return InstallPermissions(innerBundleInfo);
}

bool BundlePermissionMgr::UninstallPermissions(const InnerBundleInfo &innerBundleInfo)
{
    auto bundleName = innerBundleInfo.GetBundleName();
    auto ret = RemoveDefPermissions(bundleName);
    if (ret != Permission::RET_SUCCESS) {
        APP_LOGE("RemoveDefPermissions ret %{public}d", ret);
    }
    int userId = innerBundleInfo.GetUserId();
    ret = RemoveUserGrantedReqPermissions(bundleName, userId);
    if (ret != Permission::RET_SUCCESS) {
        APP_LOGE("RemoveUserGrantedReqPermissions ret %{public}d", ret);
    }
    ret = RemoveSystemGrantedReqPermissions(bundleName);
    if (ret != Permission::RET_SUCCESS) {
        APP_LOGE("RemoveSystemGrantedReqPermissions ret %{public}d", ret);
    }
    return true;
}

int BundlePermissionMgr::VerifyPermission(
    const std::string &bundleName, const std::string &permissionName, const int userId)
{
    APP_LOGI(
        "VerifyPermission bundleName %{public}s, permission %{public}s", bundleName.c_str(), permissionName.c_str());
    return Permission::PermissionKit::VerifyPermission(bundleName, permissionName, userId);
}

bool BundlePermissionMgr::CanRequestPermission(
    const std::string &bundleName, const std::string &permissionName, const int userId)
{
    APP_LOGI("CanRequestPermission bundleName %{public}s, permission %{public}s",
        bundleName.c_str(),
        permissionName.c_str());
    return Permission::PermissionKit::CanRequestPermission(bundleName, permissionName, userId);
}

bool BundlePermissionMgr::RequestPermissionFromUser(
    const std::string &bundleName, const std::string &permissionName, const int userId)
{
    APP_LOGI("RequestPermissionFromUser bundleName %{public}s, permission %{public}s",
        bundleName.c_str(),
        permissionName.c_str());
    return (Permission::PermissionKit::GrantUserGrantedPermission(bundleName, permissionName, userId) ==
            Permission::RET_SUCCESS);
}

bool BundlePermissionMgr::GetPermissionDef(const std::string &permissionName, PermissionDef &permissionDef)
{
    APP_LOGI("GetPermissionDef permission %{public}s", permissionName.c_str());
    Permission::PermissionDef permDef;
    int ret = Permission::PermissionKit::GetDefPermission(permissionName, permDef);
    if (ret != Permission::RET_SUCCESS) {
        APP_LOGE("get permission def failed");
        return false;
    }
    return ConvertPermissionDef(permDef, permissionDef);
}

bool BundlePermissionMgr::CheckCallingPermission(const std::string &permissionName)
{
    int32_t uid = IPCSkeleton::GetCallingUid();
    if (uid >= Constants::ROOT_UID && uid < Constants::BASE_SYS_UID) {
        APP_LOGE("check permission true for system uid %{public}d", uid);
        return true;
    }
    auto dataMgr = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (!dataMgr) {
        APP_LOGE("Get dataMgr shared_ptr nullptr");
        return false;
    }
    std::string bundleName;
    bool result = dataMgr->GetBundleNameForUid(uid, bundleName);
    if (!result) {
        APP_LOGE("cannot get bundle name by uid %{public}d", uid);
        return false;
    }
    // now if the application is Launcher, pass it through for installing permission
    {
        APP_LOGI(
            "get app bundleName %{public}s and permissionName %{public}s", bundleName.c_str(), permissionName.c_str());
        ApplicationInfo appInfo;
        bool ret = dataMgr->GetApplicationInfo(bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO,
                                            Constants::DEFAULT_USERID, appInfo);
        if (ret && appInfo.isLauncherApp && (permissionName == Constants::PERMISSION_INSTALL_BUNDLE)) {
            APP_LOGE("launcher app %{public}s pass through", bundleName.c_str());
            return true;
        }
    }
    int ret = VerifyPermission(bundleName, permissionName, Constants::DEFAULT_USERID);
    APP_LOGI("permission = %{public}s, uid = %{public}d, ret = %{public}d", permissionName.c_str(), uid, ret);
    return ret == Permission::PermissionState::PERMISSION_GRANTED;
}

int BundlePermissionMgr::GrantReqPermissions(const std::string &bundleName, const std::string &permissionName)
{
    APP_LOGI(
        "GrantReqPermissions bundleName %{public}s, permission %{public}s", bundleName.c_str(), permissionName.c_str());
    return Permission::PermissionKit::GrantSystemGrantedPermission(bundleName, permissionName);
}

int BundlePermissionMgr::AddUserGrantedReqPermissions(
    const std::string &bundleName, const std::vector<std::string> &permList, const int userId)
{
    APP_LOGI("AddUserGrantedReqPermissions bundleName %{public}s %{public}zu", bundleName.c_str(), permList.size());
    return Permission::PermissionKit::AddUserGrantedReqPermissions(bundleName, permList, userId);
}

int BundlePermissionMgr::AddSystemGrantedReqPermissions(
    const std::string &bundleName, const std::vector<std::string> &permList)
{
    APP_LOGI("AddSystemGrantedReqPermissions bundleName %{public}s %{public}zu", bundleName.c_str(), permList.size());
    return Permission::PermissionKit::AddSystemGrantedReqPermissions(bundleName, permList);
}

int BundlePermissionMgr::RemoveDefPermissions(const std::string &bundleName)
{
    APP_LOGI("RemoveDefPermissions bundleName %{public}s", bundleName.c_str());
    return Permission::PermissionKit::RemoveDefPermissions(bundleName);
}

int BundlePermissionMgr::RemoveUserGrantedReqPermissions(const std::string &bundleName, const int userId)
{
    APP_LOGI("RemoveUserGrantedReqPermissions bundleName %{public}s", bundleName.c_str());
    return Permission::PermissionKit::RemoveUserGrantedReqPermissions(bundleName, userId);
}

int BundlePermissionMgr::RemoveSystemGrantedReqPermissions(const std::string &bundleName)
{
    APP_LOGI("RemoveSystemGrantedReqPermissions bundleName %{public}s", bundleName.c_str());
    return Permission::PermissionKit::RemoveSystemGrantedReqPermissions(bundleName);
}

}  // namespace AppExecFwk
}  // namespace OHOS