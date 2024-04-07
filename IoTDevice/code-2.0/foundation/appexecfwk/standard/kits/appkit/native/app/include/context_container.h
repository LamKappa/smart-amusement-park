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

#ifndef FOUNDATION_APPEXECFWK_OHOS_CONTEXT_CONTAINER_H
#define FOUNDATION_APPEXECFWK_OHOS_CONTEXT_CONTAINER_H

#include "context_deal.h"

namespace OHOS {
namespace AppExecFwk {

class ContextContainer : public Context {
public:
    ContextContainer() = default;
    virtual ~ContextContainer() = default;

    /**
     * Attaches a Context object to the current ability.
     * Generally, this method is called after Ability is loaded to provide the application context for the current
     * ability.
     *
     * @param base Indicates a Context object.
     */
    void AttachBaseContext(const std::shared_ptr<Context> &base);

    /**
     * Called when getting the ProcessInfo
     *
     * @return ProcessInfo
     */
    std::shared_ptr<ProcessInfo> GetProcessInfo() const override;

    /**
     * @brief Obtains information about the current application. The returned application information includes basic
     * information such as the application name and application permissions.
     *
     * @return Returns the ApplicationInfo for the current application.
     */
    std::shared_ptr<ApplicationInfo> GetApplicationInfo() const override;

    /**
     * @brief Obtains the Context object of the application.
     *
     * @return Returns the Context object of the application.
     */
    std::shared_ptr<Context> GetApplicationContext() const override;

    /**
     * @brief Obtains the path of the package containing the current ability. The returned path contains the resources,
     *  source code, and configuration files of a module.
     *
     * @return Returns the path of the package file.
     */
    virtual std::string GetBundleCodePath() override;

    /**
     * @brief Obtains information about the current ability.
     * The returned information includes the class name, bundle name, and other information about the current ability.
     *
     * @return Returns the AbilityInfo object for the current ability.
     */
    virtual const std::shared_ptr<AbilityInfo> GetAbilityInfo() override;

    /**
     * @brief Obtains the Context object of the application.
     *
     * @return Returns the Context object of the application.
     */
    std::shared_ptr<Context> GetContext() override;

    /**
     * @brief Obtains an IBundleMgr instance.
     * You can use this instance to obtain information about the application bundle.
     *
     * @return Returns an IBundleMgr instance.
     */
    sptr<IBundleMgr> GetBundleManager() const override;

    /**
     * @brief Obtains a resource manager.
     *
     * @return Returns a ResourceManager object.
     */
    std::shared_ptr<Global::Resource::ResourceManager> GetResourceManager() const override;

    /**
     * @brief Deletes the specified private file associated with the application.
     *
     * @param fileName Indicates the name of the file to delete. The file name cannot contain path separators.
     *
     * @return Returns true if the file is deleted successfully; returns false otherwise.
     */
    bool DeleteFile(const std::string &fileName) override;

    /**
     * @brief Obtains the application-specific cache directory on the device's internal storage. The system
     * automatically deletes files from the cache directory if disk space is required elsewhere on the device.
     * Older files are always deleted first.
     *
     * @return Returns the application-specific cache directory.
     */
    std::string GetCacheDir() override;

    /**
     * @brief Obtains the application-specific code-cache directory on the device's internal storage.
     * The system will delete any files stored in this location both when your specific application is upgraded,
     * and when the entire platform is upgraded.
     *
     * @return Returns the application-specific code-cache directory.
     */
    std::string GetCodeCacheDir() override;

    /**
     * @brief Obtains the local database path.
     * If the local database path does not exist, the system creates one and returns the created path.
     *
     * @return Returns the local database file.
     */
    std::string GetDatabaseDir() override;

    /**
     * @brief Obtains the absolute path where all private data files of this application are stored.
     *
     * @return Returns the absolute path storing all private data files of this application.
     */
    std::string GetDataDir() override;

    /**
     * @brief Obtains the directory for storing custom data files of the application.
     * You can use the returned File object to create and access files in this directory. The files
     * can be accessible only by the current application.
     *
     * @param name Indicates the name of the directory to retrieve. This directory is created as part
     * of your application data.
     * @param mode Indicates the file operating mode. The value can be 0 or a combination of MODE_PRIVATE.
     *
     * @return Returns a File object for the requested directory.
     */
    std::string GetDir(const std::string &name, int mode) override;

    /**
     * @brief Obtains the absolute path to the application-specific cache directory
     * on the primary external or shared storage device.
     *
     * @return Returns the absolute path to the application-specific cache directory on the external or
     * shared storage device; returns null if the external or shared storage device is temporarily unavailable.
     */
    std::string GetExternalCacheDir() override;

    /**
     * @brief Obtains the absolute path to the directory for storing files for the application on the
     * primary external or shared storage device.
     *
     * @param type Indicates the type of the file directory to return
     *
     * @return Returns the absolute path to the application file directory on the external or shared storage
     * device; returns null if the external or shared storage device is temporarily unavailable.
     */
    std::string GetExternalFilesDir(std::string &type) override;

    /**
     * @brief Obtains the directory for storing files for the application on the device's internal storage.
     *
     * @return Returns the application file directory.
     */
    std::string GetFilesDir() override;

    /**
     * @brief Obtains the absolute path which app created and will be excluded from automatic backup to remote storage.
     * The returned path maybe changed if the application is moved to an adopted storage device.
     *
     * @return The path of the directory holding application files that will not be automatically backed up to remote
     * storage.
     */
    std::string GetNoBackupFilesDir() override;

    /**
     * @brief Checks whether the calling process for inter-process communication has the given permission.
     * The calling process is not the current process.
     *
     * @param permission Indicates the permission to check. This parameter cannot be null.
     *
     * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the calling process has the permission;
     * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
     */
    int VerifyCallingPermission(const std::string &permission) override;

    /**
     * @brief Checks whether the current process has the given permission.
     * You need to call requestPermissionsFromUser(java.lang.std::string[],int) to request a permission only
     * if the current process does not have the specific permission.
     *
     * @param permission Indicates the permission to check. This parameter cannot be null.
     *
     * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the current process has the permission;
     * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
     */
    int VerifySelfPermission(const std::string &permission) override;

    /**
     * @brief Obtains the bundle name of the current ability.
     *
     * @return Returns the bundle name of the current ability.
     */
    std::string GetBundleName() override;

    /**
     * @brief Obtains the path of the OHOS Ability Package (HAP} containing this ability.
     *
     * @return Returns the path of the HAP containing this ability.
     */
    std::string GetBundleResourcePath() override;

    /**
     * @brief Remove permissions for all users who have access to specific permissions
     *
     * @param permission Indicates the permission to unauth. This parameter cannot be null.
     * @param uri Indicates the URI to unauth. This parameter cannot be null.
     * @param uid Indicates the UID of the unauth to check.
     *
     */
    void UnauthUriPermission(const std::string &permission, const Uri &uri, int uid) override;

    /**
     * @brief Obtains an ability manager.
     * The ability manager provides information about running processes and memory usage of an application.
     *
     * @return Returns an IAbilityManager instance.
     */
    sptr<AAFwk::IAbilityManager> GetAbilityManager() override;

    /**
     * @brief Obtains the type of this application.
     *
     * @return Returns system if this application is a system application;
     * returns normal if it is released in OHOS AppGallery;
     * returns other if it is released by a third-party vendor;
     * returns an empty string if the query fails.
     */
    std::string GetAppType() override;

    /**
     * @brief Confirms with the permission management module to check whether a request prompt is required for granting
     * a certain permission. You need to call the current method to check whether a prompt is required before calling
     * requestPermissionsFromUser(java.lang.String[],int) to request a permission. If a prompt is not required,
     * permission request will not be initiated.
     *
     * @param requestCode Indicates the permission to be queried. This parameter cannot be null.
     *
     * @return Returns true if the current application does not have the permission and the user does not turn off
     * further requests; returns false if the current application already has the permission, the permission is rejected
     * by the system, or the permission is denied by the user and the user has turned off further requests.
     */
    bool CanRequestPermission(const std::string &permission) override;

    /**
     * @brief When there is a remote call to check whether the remote has permission, otherwise check whether it has
     * permission
     *
     * @param permissions Indicates the list of permissions to be requested. This parameter cannot be null.
     * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the current process has the permission;
     * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
     */
    int VerifyCallingOrSelfPermission(const std::string &permission) override;

    /**
     * @brief Query whether the application of the specified PID and UID has been granted a certain permission
     *
     * @param permissions Indicates the list of permissions to be requested. This parameter cannot be null.
     * @param pid Process id
     * @param uid
     * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the current process has the permission;
     * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
     */
    int VerifyPermission(const std::string &permission, int pid, int uid) override;

    /**
     * @brief Obtains the distributed file path.
     * If the distributed file path does not exist, the system creates one and returns the created path. This method is
     * applicable only to the context of an ability rather than that of an application.
     *
     * @return Returns the distributed file.
     */
    std::string GetDistributedDir() override;

    /**
     * @brief Sets the pattern of this Context based on the specified pattern ID.
     *
     * @param patternId Indicates the resource ID of the pattern to set.
     */
    void SetPattern(int patternId) override;

    /**
     * @brief Obtains the Context object of this ability.
     *
     * @return Returns the Context object of this ability.
     */
    std::shared_ptr<Context> GetAbilityPackageContext() override;

    /**
     * @brief Obtains the HapModuleInfo object of the application.
     *
     * @return Returns the HapModuleInfo object of the application.
     */
    std::shared_ptr<HapModuleInfo> GetHapModuleInfo() override;

    /**
     * @brief Obtains the name of the current process.
     *
     * @return Returns the current process name.
     */
    std::string GetProcessName() override;

    /**
     * @brief Requests certain permissions from the system.
     * This method is called for permission request. This is an asynchronous method. When it is executed,
     * the Ability.onRequestPermissionsFromUserResult(int, String[], int[]) method will be called back.
     *
     * @param permissions Indicates the list of permissions to be requested. This parameter cannot be null.
     * @param requestCode Indicates the request code to be passed to the Ability.onRequestPermissionsFromUserResult(int,
     * String[], int[]) callback method. This code cannot be a negative number.
     *
     */
    void RequestPermissionsFromUser(std::vector<std::string> &permissions, int requestCode) override;

    /**
     * @brief Creates a Context object for an application with the given bundle name.
     *
     * @param bundleName Indicates the bundle name of the application.
     *
     * @param flag  Indicates the flag for creating a Context object. It can be 0, any of
     * the following values, or any combination of the following values: CONTEXT_IGNORE_SECURITY,
     * CONTEXT_INCLUDE_CODE, and CONTEXT_RESTRICTED. The value 0 indicates that there is no restriction
     * on creating contexts for applications.
     *
     * @return Returns a Context object created for the specified application.
     */
    std::shared_ptr<Context> CreateBundleContext(std::string bundleName, int flag);

    /**
     * @brief Obtains information about the caller of this ability.
     *
     * @return Returns the caller information.
     */
    Uri GetCaller() override;

    /**
     * @brief InitResourceManager
     * 
     * @param bundleInfo  BundleInfo
     */
    void InitResourceManager(BundleInfo &bundleInfo, std::shared_ptr<ContextDeal> &deal);

private:
    std::shared_ptr<Context> baseContext_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_CONTEXT_CONTAINER_H
