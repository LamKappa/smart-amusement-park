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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PERMISSION_MGR_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PERMISSION_MGR_H

#include "inner_bundle_info.h"
#include "permission_def.h"
#include "permission/permission_kit.h"

namespace OHOS {
namespace AppExecFwk {

class BundlePermissionMgr {
public:
    /**
     * @brief Initialize the system defined permissions on first start up.
     * @return Returns true if the permissions initialized successfully; returns false otherwise.
     */
    static bool InitPermissions();
    /**
     * @brief Handle the permissions in installation progress.
     * @param innerBundleInfo Indicates the current installing inner bundle information.
     * @return Returns true if the permissions install successfully; returns false otherwise.
     */
    static bool InstallPermissions(const InnerBundleInfo &innerBundleInfo);
    /**
     * @brief Handle the permissions in updating progress.
     * @param innerBundleInfo Indicates the current installing inner bundle information.
     * @return Returns true if the permissions updating successfully; returns false otherwise.
     */
    static bool UpdatePermissions(const InnerBundleInfo &innerBundleInfo);
    /**
     * @brief Handle the permissions in uninstall progress.
     * @param innerBundleInfo Indicates the current installing inner bundle information.
     * @return Returns true if the permissions uninstall successfully; returns false otherwise.
     */
    static bool UninstallPermissions(const InnerBundleInfo &innerBundleInfo);
    /**
     * @brief Check the permission whether granted for calling process.
     * @param permissionName Indicates the permission name.
     * @return Returns true if the permissions has been granted; returns false otherwise.
     */
    static bool CheckCallingPermission(const std::string &permissionName);
    /**
     * @brief Verify whether a specified bundle has been granted a specific permission.
     * @param bundleName Indicates the name of the bundle to check.
     * @param permission Indicates the permission to check.
     * @param userId Indicates the userId of the bundle.
     * @return Returns 0 if the bundle has the permission; returns -1 otherwise.
     */
    static int VerifyPermission(const std::string &bundleName, const std::string &permissionName, const int userId);
    /**
     * @brief Obtains detailed information about a specified permission.
     * @param permissionName Indicates the name of the permission.
     * @param permissionDef Indicates the object containing detailed information about the given permission.
     * @return Returns true if the PermissionDef object is successfully obtained; returns false otherwise.
     */
    static bool GetPermissionDef(const std::string &permissionName, PermissionDef &permissionDef);
    /**
     * @brief Confirms with the permission management module to check whether a request prompt is required for granting
     * a certain permission.
     * @param bundleName Indicates the name of the bundle.
     * @param permission Indicates the permission to quest.
     * @param userId Indicates the userId of the bundle.
     * @return Returns true if the current application does not have the permission and the user does not turn off
     * further requests; returns false if the current application already has the permission, the permission is rejected
     * by the system, or the permission is denied by the user and the user has turned off further requests.
     */
    static bool CanRequestPermission(
        const std::string &bundleName, const std::string &permissionName, const int userId);
    /**
     * @brief Requests a certain permission from user.
     * @param bundleName Indicates the name of the bundle.
     * @param permission Indicates the permission to request.
     * @param userId Indicates the userId of the bundle.
     * @return Returns true if the permission request successfully; returns false otherwise.
     */
    static bool RequestPermissionFromUser(
        const std::string &bundleName, const std::string &permissionName, const int userId);

private:
    /**
     * @brief Add the defPermissions to permission kit.
     * @param innerBundleInfo Indicates the current installing inner bundle information.
     * @return Returns 0 if the defPermissions add successfully; returns -1 otherwise.
     */
    static int AddDefPermissions(const InnerBundleInfo &innerBundleInfo);
    /**
     * @brief Add and grant the reqPermissions to permission kit.
     * @param innerBundleInfo Indicates the current installing inner bundle information.
     * @return Returns 0 if the reqPermissions add and grant successfully; returns -1 otherwise.
     */
    static int AddAndGrantedReqPermissions(const InnerBundleInfo &innerBundleInfo);
    /**
     * @brief Grant a reqPermission from permission kit.
     * @param bundleName Indicates the name of the bundle.
     * @param permissionName Indicates the permission.
     * @return Returns 0 if the reqPermission grant successfully; returns -1 otherwise.
     */
    static int GrantReqPermissions(const std::string &bundleName, const std::string &permissionName);
    /**
     * @brief Add user granted reqPermissions to permission kit.
     * @param bundleName Indicates the name of the bundle to add.
     * @param permList Indicates the list of reqPermission to add.
     * @param userId Indicates the userId of the bundle.
     * @return Returns 0 if the reqPermissions add successfully; returns -1 otherwise.
     */
    static int AddUserGrantedReqPermissions(
        const std::string &bundleName, const std::vector<std::string> &permList, const int userId);
    /**
     * @brief Add system granted reqPermissions to permission kit.
     * @param bundleName Indicates the name of the bundle to add.
     * @param permList Indicates the list of reqPermission to add.
     * @return Returns 0 if the reqPermissions add successfully; returns -1 otherwise.
     */
    static int AddSystemGrantedReqPermissions(const std::string &bundleName, const std::vector<std::string> &permList);
    /**
     * @brief Check whether a permission need to be granted.
     * @param permissionDef Indicates the definition of a permission.
     * @param innerBundleInfo Indicates the current installing inner bundle information.
     * @return Returns true if the permission need to be granted; returns false otherwise.
     */
    static bool CheckPermissionAuthorization(
        const Security::Permission::PermissionDef &permissionDef, const InnerBundleInfo &innerBundleInfo);
    /**
     * @brief Remove the defPermissions from permission kit.
     * @param innerBundleInfo Indicates the current uninstalling inner bundle information.
     * @return Returns 0 if the defPermissions removed successfully; returns -1 otherwise.
     */
    static int RemoveDefPermissions(const std::string &bundleName);
    /**
     * @brief Remove user granted reqPermissions from permission kit.
     * @param bundleName Indicates the name of the bundle to remove.
     * @param userId Indicates the userId of the bundle.
     * @return Returns 0 if the reqPermissions removed successfully; returns -1 otherwise.
     */
    static int RemoveUserGrantedReqPermissions(const std::string &bundleName, const int userId);
    /**
     * @brief Remove system granted reqPermissions from permission kit.
     * @param bundleName Indicates the name of the bundle to remove.
     * @return Returns 0 if the reqPermissions removed successfully; returns -1 otherwise.
     */
    static int RemoveSystemGrantedReqPermissions(const std::string &bundleName);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PERMISSION_MGR_H