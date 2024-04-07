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

#ifndef FOUNDATION_APPEXECFWK_OHOS_CONTEXT_H
#define FOUNDATION_APPEXECFWK_OHOS_CONTEXT_H

#include <memory>

#include "bundle_mgr_interface.h"
#include "ability_manager_interface.h"
#include "ability_info.h"
#include "process_info.h"
#include "resource_manager.h"
#include "ability_start_setting.h"
#include "dummy_hap_module_info.h"
#include "hap_module_info.h"

namespace OHOS {
namespace AppExecFwk {

using Want = OHOS::AAFwk::Want;

#define OHOS_PERMISSIONS_REQUEST_KEY "HAS_CURRENT_PERMISSIONS_REQUEST_KEY"
#define OHOS_PERMISSIONS_REQUEST_RESULT_KEY "HAS_CURRENT_PERMISSIONS_RESULT_KEY"
#define OHOS_PERMISSIONS_REQUEST_BUNDLE_NAME_KEY "HAS_CURRENT_PERMISSIONS_REQUEST_BUNDLE_NAME_KEY"
#define OHOS_PERMISSIONS_REQUEST_USER_ID_KEY "HAS_CURRENT_PERMISSIONS_REQUEST_USER_ID_KEY"

class DataAbilityHelper;
class IAbilityManager;
class Context {
public:
    Context() = default;
    virtual ~Context() = default;

    /**
     * The value 0 indicates that there is no restriction on creating contexts for applications.
     */
    int MODE_PRIVATE = 0x0000;

    /**
     * static final int CONTEXT_INCLUDE_CODE
     * Indicates the flag used with createBundleContext(java.lang.String,int) for creating a Context
     * object that includes the application code.
     *
     * The value is 0x00000001.
     *
     * @since 3
     */
    int CONTEXT_INCLUDE_CODE = 0x00000001;

    /**
     * static final int CONTEXT_IGNORE_SECURITY
     * Indicates the flag used with createBundleContext(java.lang.String,int) for creating a Context
     * object that can always be loaded with all security restrictions ignored.
     *
     * The value is 0x00000002.
     *
     * @since 3
     */
    int CONTEXT_IGNORE_SECURITY = 0x00000002;

    /**
     * static final int CONTEXT_RESTRICTED
     * Indicates the flag used with createBundleContext(java.lang.String,int) for creating a Context
     * object in which particular features may be disabled.
     *
     * The value is 0x00000004.
     *
     * @since 3
     */
    int CONTEXT_RESTRICTED = 0x00000004;

    int CONTEXT_RESOUCE_ONLY = 0x00000008;

    /**
     * Called when getting the ProcessInfo
     *
     * @return ProcessInfo
     */
    virtual std::shared_ptr<ProcessInfo> GetProcessInfo() const = 0;

    /**
     * @brief Obtains information about the current application. The returned application information includes basic
     * information such as the application name and application permissions.
     *
     * @return Returns the ApplicationInfo for the current application.
     */
    virtual std::shared_ptr<ApplicationInfo> GetApplicationInfo() const = 0;

    /**
     * @brief Obtains the Context object of the application.
     *
     * @return Returns the Context object of the application.
     */
    virtual std::shared_ptr<Context> GetApplicationContext() const = 0;

    /**
     * @brief Obtains the path of the package containing the current ability. The returned path contains the resources,
     *  source code, and configuration files of a module.
     *
     * @return Returns the path of the package file.
     */
    virtual std::string GetBundleCodePath() = 0;

    /**
     * @brief Obtains information about the current ability.
     * The returned information includes the class name, bundle name, and other information about the current ability.
     *
     * @return Returns the AbilityInfo object for the current ability.
     */
    virtual const std::shared_ptr<AbilityInfo> GetAbilityInfo() = 0;

    /**
     * @brief Obtains the Context object of the application.
     *
     * @return Returns the Context object of the application.
     */
    virtual std::shared_ptr<Context> GetContext() = 0;

    /**
     * @brief Obtains an IBundleMgr instance.
     * You can use this instance to obtain information about the application bundle.
     *
     * @return Returns an IBundleMgr instance.
     */
    virtual sptr<IBundleMgr> GetBundleManager() const = 0;

    /**
     * @brief Obtains a resource manager.
     *
     * @return Returns a ResourceManager object.
     */
    virtual std::shared_ptr<Global::Resource::ResourceManager> GetResourceManager() const = 0;

    /**
     * @brief Deletes the specified private file associated with the application.
     *
     * @param fileName Indicates the name of the file to delete. The file name cannot contain path separators.
     *
     * @return Returns true if the file is deleted successfully; returns false otherwise.
     */
    virtual bool DeleteFile(const std::string &fileName) = 0;

    /**
     * @brief Destroys another ability that uses the AbilityInfo.AbilityType.SERVICE template.
     * The current ability using either the AbilityInfo.AbilityType.SERVICE or AbilityInfo.AbilityType.PAGE
     * template can call this method to destroy another ability that uses the AbilityInfo.AbilityType.SERVICE
     * template. The current ability itself can be destroyed by calling the terminateAbility() method.
     *
     * @param want Indicates the Want containing information about the ability to destroy.
     *
     * @return Returns true if the ability is destroyed successfully; returns false otherwise.
     */
    virtual bool StopAbility(const AAFwk::Want &want) = 0;

    /**
     * @brief Obtains the application-specific cache directory on the device's internal storage. The system
     * automatically deletes files from the cache directory if disk space is required elsewhere on the device.
     * Older files are always deleted first.
     *
     * @return Returns the application-specific cache directory.
     */
    virtual std::string GetCacheDir() = 0;

    /**
     * @brief Obtains the application-specific code-cache directory on the device's internal storage.
     * The system will delete any files stored in this location both when your specific application is upgraded,
     * and when the entire platform is upgraded.
     *
     * @return Returns the application-specific code-cache directory.
     */
    virtual std::string GetCodeCacheDir() = 0;

    /**
     * @brief Obtains the local database path.
     * If the local database path does not exist, the system creates one and returns the created path.
     *
     * @return Returns the local database file.
     */
    virtual std::string GetDatabaseDir() = 0;

    /**
     * @brief Obtains the absolute path where all private data files of this application are stored.
     *
     * @return Returns the absolute path storing all private data files of this application.
     */
    virtual std::string GetDataDir() = 0;

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
    virtual std::string GetDir(const std::string &name, int mode) = 0;

    /**
     * @brief Obtains the absolute path to the application-specific cache directory
     * on the primary external or shared storage device.
     *
     * @return Returns the absolute path to the application-specific cache directory on the external or
     * shared storage device; returns null if the external or shared storage device is temporarily unavailable.
     */
    virtual std::string GetExternalCacheDir() = 0;

    /**
     * @brief Obtains the absolute path to the directory for storing files for the application on the
     * primary external or shared storage device.
     *
     * @param type Indicates the type of the file directory to return
     *
     * @return Returns the absolute path to the application file directory on the external or shared storage
     * device; returns null if the external or shared storage device is temporarily unavailable.
     */
    virtual std::string GetExternalFilesDir(std::string &type) = 0;

    /**
     * @brief Obtains the directory for storing files for the application on the device's internal storage.
     *
     * @return Returns the application file directory.
     */
    virtual std::string GetFilesDir() = 0;

    /**
     * @brief Obtains the absolute path which app created and will be excluded from automatic backup to remote storage.
     * The returned path maybe changed if the application is moved to an adopted storage device.
     *
     * @return The path of the directory holding application files that will not be automatically backed up to remote
     * storage.
     */
    virtual std::string GetNoBackupFilesDir() = 0;

    /**
     * @brief Checks whether the calling process for inter-process communication has the given permission.
     * The calling process is not the current process.
     *
     * @param permission Indicates the permission to check. This parameter cannot be null.
     *
     * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the calling process has the permission;
     * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
     */
    virtual int VerifyCallingPermission(const std::string &permission) = 0;

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
    virtual int VerifySelfPermission(const std::string &permission) = 0;

    /**
     * @brief Obtains the bundle name of the current ability.
     *
     * @return Returns the bundle name of the current ability.
     */
    virtual std::string GetBundleName() = 0;

    /**
     * @brief Obtains the path of the OHOS Ability Package (HAP} containing this ability.
     *
     * @return Returns the path of the HAP containing this ability.
     */
    virtual std::string GetBundleResourcePath() = 0;

    /**
     * @brief Starts a new ability.
     * An ability using the AbilityInfo.AbilityType.SERVICE or AbilityInfo.AbilityType.PAGE template uses this method
     * to start a specific ability. The system locates the target ability from installed abilities based on the value
     * of the want parameter and then starts it. You can specify the ability to start using the want parameter.
     *
     * @param want Indicates the Want containing information about the target ability to start.
     *
     * @param requestCode Indicates the request code returned after the ability using the AbilityInfo.AbilityType.PAGE
     * template is started. You can define the request code to identify the results returned by abilities. The value
     * ranges from 0 to 65535. This parameter takes effect only on abilities using the AbilityInfo.AbilityType.PAGE
     * template.
     *
     */
    virtual void StartAbility(const AAFwk::Want &want, int requestCode) = 0;

    /**
     * @brief Remove permissions for all users who have access to specific permissions
     *
     * @param permission Indicates the permission to unauth. This parameter cannot be null.
     * @param uri Indicates the URI to unauth. This parameter cannot be null.
     * @param uid Indicates the UID of the unauth to check.
     *
     */
    virtual void UnauthUriPermission(const std::string &permission, const Uri &uri, int uid) = 0;

    /**
     * @brief Obtains an ability manager.
     * The ability manager provides information about running processes and memory usage of an application.
     *
     * @return Returns an IAbilityManager instance.
     */
    virtual sptr<AAFwk::IAbilityManager> GetAbilityManager() = 0;

    /**
     * @brief Obtains the type of this application.
     *
     * @return Returns system if this application is a system application;
     * returns normal if it is released in OHOS AppGallery;
     * returns other if it is released by a third-party vendor;
     * returns an empty string if the query fails.
     */
    virtual std::string GetAppType() = 0;

    /**
     * @brief Destroys another ability you had previously started by calling Ability.startAbilityForResult
     * (ohos.aafwk.content.Want, int, ohos.aafwk.ability.startsetting.AbilityStartSetting) with the same requestCode
     * passed.
     *
     * @param requestCode Indicates the request code passed for starting the ability.
     *
     */
    virtual void TerminateAbility(int requestCode) = 0;

    /**
     * @brief Destroys the current ability.
     *
     */
    virtual void TerminateAbility() = 0;

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
    virtual bool CanRequestPermission(const std::string &permission) = 0;

    /**
     * @brief When there is a remote call to check whether the remote has permission, otherwise check whether it has
     * permission
     *
     * @param permissions Indicates the list of permissions to be requested. This parameter cannot be null.
     * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the current process has the permission;
     * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
     */
    virtual int VerifyCallingOrSelfPermission(const std::string &permission) = 0;

    /**
     * @brief Query whether the application of the specified PID and UID has been granted a certain permission
     *
     * @param permissions Indicates the list of permissions to be requested. This parameter cannot be null.
     * @param pid Process id
     * @param uid
     * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the current process has the permission;
     * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
     */
    virtual int VerifyPermission(const std::string &permission, int pid, int uid) = 0;

    /**
     * @brief Obtains the distributed file path.
     * If the distributed file path does not exist, the system creates one and returns the created path. This method is
     * applicable only to the context of an ability rather than that of an application.
     *
     * @return Returns the distributed file.
     */
    virtual std::string GetDistributedDir() = 0;

    /**
     * @brief Sets the pattern of this Context based on the specified pattern ID.
     *
     * @param patternId Indicates the resource ID of the pattern to set.
     */
    virtual void SetPattern(int patternId) = 0;

    /**
     * @brief Obtains the Context object of this ability.
     *
     * @return Returns the Context object of this ability.
     */
    virtual std::shared_ptr<Context> GetAbilityPackageContext() = 0;

    /**
     * @brief Obtains the HapModuleInfo object of the application.
     *
     * @return Returns the HapModuleInfo object of the application.
     */
    virtual std::shared_ptr<HapModuleInfo> GetHapModuleInfo() = 0;

    /**
     * @brief Obtains the name of the current process.
     *
     * @return Returns the current process name.
     */
    virtual std::string GetProcessName() = 0;

    /**
     * @brief Obtains the bundle name of the ability that called the current ability.
     * You can use the obtained bundle name to check whether the calling ability is allowed to receive the data you will
     * send. If you did not use Ability.startAbilityForResult(ohos.aafwk.content.Want, int,
     * ohos.aafwk.ability.startsetting.AbilityStartSetting) to start the calling ability, null is returned.
     *
     * @return Returns the bundle name of the calling ability; returns null if no calling ability is available.
     */
    virtual std::string GetCallingBundle() = 0;

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
    virtual void RequestPermissionsFromUser(std::vector<std::string> &permissions, int requestCode) = 0;

    /**
     * @brief Starts a new ability with special ability start setting.
     *
     * @param want Indicates the Want containing information about the target ability to start.
     * @param requestCode Indicates the request code returned after the ability is started. You can define the request
     * code to identify the results returned by abilities. The value ranges from 0 to 65535.
     * @param abilityStartSetting Indicates the special start setting used in starting ability.
     *
     */
    virtual void StartAbility(const Want &want, int requestCode, const AbilityStartSetting &abilityStartSetting) = 0;

    /**
     * @brief Connects the current ability to an ability using the AbilityInfo.AbilityType.SERVICE template.
     *
     * @param want Indicates the want containing information about the ability to connect
     *
     * @param conn Indicates the callback object when the target ability is connected.
     *
     * @return True means success and false means failure
     */
    virtual bool ConnectAbility(const Want &want, const sptr<AAFwk::IAbilityConnection> &conn) = 0;

    /**
     * @brief Disconnects the current ability from an ability
     *
     * @param conn Indicates the IAbilityConnection callback object passed by connectAbility after the connection
     *              is set up. The IAbilityConnection object uniquely identifies a connection between two abilities.
     */
    virtual void DisconnectAbility(const sptr<AAFwk::IAbilityConnection> &conn) = 0;

    /**
     * @brief Obtains information about the caller of this ability.
     *
     * @return Returns the caller information.
     */
    virtual Uri GetCaller() = 0;

    friend DataAbilityHelper;

protected:
    virtual sptr<IRemoteObject> GetToken() = 0;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_CONTEXT_H
