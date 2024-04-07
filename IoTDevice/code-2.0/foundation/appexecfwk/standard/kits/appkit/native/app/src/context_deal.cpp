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

#include "context_deal.h"
#include "file_ex.h"
#include "directory_ex.h"
#include "iservice_registry.h"
#include "app_log_wrapper.h"
#include "ability_manager_interface.h"
#include "ability_manager_client.h"
#include "system_ability_definition.h"
#include "sys_mgr_client.h"

#define MODE 0771
namespace OHOS {
namespace AppExecFwk {

/**
 * Called when getting the ProcessInfo
 *
 * @return ProcessInfo
 */
std::shared_ptr<ProcessInfo> ContextDeal::GetProcessInfo() const
{
    return processInfo_;
}

/**
 * Called when setting the ProcessInfo
 *
 * @param info ProcessInfo instance
 */
void ContextDeal::SetProcessInfo(const std::shared_ptr<ProcessInfo> &info)
{
    if (info == nullptr) {
        APP_LOGE("ContextDeal::SetProcessInfo failed, info is empty");
        return;
    }
    processInfo_ = info;
}

/**
 * @brief Obtains information about the current application. The returned application information includes basic
 * information such as the application name and application permissions.
 *
 * @return Returns the ApplicationInfo for the current application.
 */
std::shared_ptr<ApplicationInfo> ContextDeal::GetApplicationInfo() const
{
    return applicationInfo_;
}

/**
 * @brief Set ApplicationInfo
 *
 * @param info ApplicationInfo instance.
 */
void ContextDeal::SetApplicationInfo(const std::shared_ptr<ApplicationInfo> &info)
{
    if (info == nullptr) {
        APP_LOGE("ContextDeal::SetApplicationInfo failed, info is empty");
        return;
    }
    applicationInfo_ = info;
}

/**
 * @brief Obtains the Context object of the application.
 *
 * @return Returns the Context object of the application.
 */
std::shared_ptr<Context> ContextDeal::GetApplicationContext() const
{
    return appContext_;
}

/**
 * @brief Set ApplicationContext
 *
 * @param context ApplicationContext instance.
 */
void ContextDeal::SetApplicationContext(const std::shared_ptr<Context> &context)
{
    if (context == nullptr) {
        APP_LOGE("ContextDeal::SetApplicationContext failed, context is empty");
        return;
    }
    appContext_ = context;
}

/**
 * @brief Obtains the path of the package containing the current ability. The returned path contains the resources,
 *  source code, and configuration files of a module.
 *
 * @return Returns the path of the package file.
 */
std::string ContextDeal::GetBundleCodePath()
{
    return (applicationInfo_ != nullptr) ? applicationInfo_->codePath : "";
}

/**
 * @brief SetBundleCodePath
 *
 * @param Returns string path
 */
void ContextDeal::SetBundleCodePath(std::string &path)
{
    path_ = path;
}

/**
 * @brief Obtains information about the current ability.
 * The returned information includes the class name, bundle name, and other information about the current ability.
 *
 * @return Returns the AbilityInfo object for the current ability.
 */
const std::shared_ptr<AbilityInfo> ContextDeal::GetAbilityInfo()
{
    return abilityInfo_;
}

/**
 * @brief Set AbilityInfo
 *
 * @param info AbilityInfo instance.
 */
void ContextDeal::SetAbilityInfo(const std::shared_ptr<AbilityInfo> &info)
{
    if (info == nullptr) {
        APP_LOGE("ContextDeal::SetAbilityInfo failed, info is empty");
        return;
    }
    abilityInfo_ = info;
}

/**
 * @brief Obtains the Context object of the ability.
 *
 * @return Returns the Context object of the ability.
 */
std::shared_ptr<Context> ContextDeal::GetContext()
{
    return abilityContext_;
}

/**
 * @brief Set Ability context
 *
 * @param context Ability object
 */
void ContextDeal::SetContext(const std::shared_ptr<Context> &context)
{
    if (context == nullptr) {
        APP_LOGE("ContextDeal::SetContext failed, context is empty");
        return;
    }
    abilityContext_ = context;
}

/**
 * @brief Obtains an IBundleMgr instance.
 * You can use this instance to obtain information about the application bundle.
 *
 * @return Returns an IBundleMgr instance.
 */
sptr<IBundleMgr> ContextDeal::GetBundleManager() const
{
    auto bundleObj =
        OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleObj == nullptr) {
        APP_LOGE("failed to get bundle manager service");
        return nullptr;
    }

    APP_LOGI("get bundle manager proxy success.");
    return iface_cast<IBundleMgr>(bundleObj);
}

/**
 * @brief Obtains a resource manager.
 *
 * @return Returns a ResourceManager object.
 */
std::shared_ptr<Global::Resource::ResourceManager> ContextDeal::GetResourceManager() const
{
    return resourceManager_;
}

/**
 * @brief Set Profile instance.
 *
 * @param Profile instance.
 */
void ContextDeal::SetProfile(const std::shared_ptr<Profile> &profile)
{
    if (profile == nullptr) {
        APP_LOGE("ContextDeal::SetProfile failed, profile is nullptr");
        return;
    }
    profile_ = profile;
}

/**
 * @brief Obtains an Profile instance.
 *
 * @return Returns an Profile instance.
 */
std::shared_ptr<Profile> ContextDeal::GetProfile() const
{
    return profile_;
}

/**
 * @brief Deletes the specified private file associated with the application.
 *
 * @param fileName Indicates the name of the file to delete. The file name cannot contain path separators.
 *
 * @return Returns true if the file is deleted successfully; returns false otherwise.
 */
bool ContextDeal::DeleteFile(const std::string &fileName)
{
    std::string path = GetDataDir() + "/" + fileName;
    return OHOS::RemoveFile(path);
}

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
bool ContextDeal::StopAbility(const AAFwk::Want &want)
{
    return false;
}

/**
 * @brief Obtains the application-specific cache directory on the device's internal storage. The system
 * automatically deletes files from the cache directory if disk space is required elsewhere on the device.
 * Older files are always deleted first.
 *
 * @return Returns the application-specific cache directory.
 */
std::string ContextDeal::GetCacheDir()
{
    return (applicationInfo_ != nullptr) ? applicationInfo_->cacheDir : "";
}

/**
 * @brief Obtains the application-specific code-cache directory on the device's internal storage.
 * The system will delete any files stored in this location both when your specific application is upgraded,
 * and when the entire platform is upgraded.
 *
 * @return Returns the application-specific code-cache directory.
 */
std::string ContextDeal::GetCodeCacheDir()
{
    return (applicationInfo_ != nullptr) ? (applicationInfo_->dataDir + "/" + "code_cache") : "";
}

/**
 * @brief Obtains the local database path.
 * If the local database path does not exist, the system creates one and returns the created path.
 *
 * @return Returns the local database file.
 */
std::string ContextDeal::GetDatabaseDir()
{
    return (applicationInfo_ != nullptr) ? applicationInfo_->dataBaseDir : "";
}

/**
 * @brief Obtains the absolute path where all private data files of this application are stored.
 *
 * @return Returns the absolute path storing all private data files of this application.
 */
std::string ContextDeal::GetDataDir()
{
    return (applicationInfo_ != nullptr) ? applicationInfo_->dataDir : "";
}

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
std::string ContextDeal::GetDir(const std::string &name, int mode)
{
    if (applicationInfo_ == nullptr) {
        APP_LOGE("ContextDeal::GetDir failed, applicationInfo_ == nullptr");
        return "";
    }
    std::string dir = applicationInfo_->dataDir + "/" + name;
    if (!OHOS::FileExists(dir)) {
        APP_LOGI("ContextDeal::GetDir File is not exits");
        OHOS::ForceCreateDirectory(dir);
        OHOS::ChangeModeDirectory(dir, mode);
    }
    return dir;
}

/**
 * @brief Obtains the absolute path to the application-specific cache directory
 * on the primary external or shared storage device.
 *
 * @return Returns the absolute path to the application-specific cache directory on the external or
 * shared storage device; returns null if the external or shared storage device is temporarily unavailable.
 */
std::string ContextDeal::GetExternalCacheDir()
{
    return "";
}

/**
 * @brief Obtains the absolute path to the directory for storing files for the application on the
 * primary external or shared storage device.
 *
 * @param type Indicates the type of the file directory to return
 *
 * @return Returns the absolute path to the application file directory on the external or shared storage
 * device; returns null if the external or shared storage device is temporarily unavailable.
 */
std::string ContextDeal::GetExternalFilesDir(std::string &type)
{
    return "";
}

/**
 * @brief Obtains the directory for storing files for the application on the device's internal storage.
 *
 * @return Returns the application file directory.
 */
std::string ContextDeal::GetFilesDir()
{
    return (applicationInfo_ != nullptr) ? (applicationInfo_->dataDir + "/" + "files") : "";
}

/**
 * @brief Obtains the absolute path which app created and will be excluded from automatic backup to remote storage.
 * The returned path maybe changed if the application is moved to an adopted storage device.
 *
 * @return The path of the directory holding application files that will not be automatically backed up to remote
 * storage.
 */
std::string ContextDeal::GetNoBackupFilesDir()
{
    std::string dir = applicationInfo_->dataDir + "no_backup";
    if (!OHOS::FileExists(dir)) {
        APP_LOGI("ContextDeal::GetDir GetNoBackupFilesDir is not exits");
        OHOS::ForceCreateDirectory(dir);
        OHOS::ChangeModeDirectory(dir, MODE);
    }
    return dir;
}

/**
 * @brief Checks whether the calling process for inter-process communication has the given permission.
 * The calling process is not the current process.
 *
 * @param permission Indicates the permission to check. This parameter cannot be null.
 *
 * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the calling process has the permission;
 * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
 */
int ContextDeal::VerifyCallingPermission(const std::string &permission)
{
    return 0;
}

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
int ContextDeal::VerifySelfPermission(const std::string &permission)
{
    return 0;
}

/**
 * @brief Obtains the bundle name of the current ability.
 *
 * @return Returns the bundle name of the current ability.
 */
std::string ContextDeal::GetBundleName()
{
    return (applicationInfo_ != nullptr) ? applicationInfo_->bundleName : "";
}

/**
 * @brief Obtains the path of the OHOS Ability Package (HAP} containing this ability.
 *
 * @return Returns the path of the HAP containing this ability.
 */
std::string ContextDeal::GetBundleResourcePath()
{
    return (abilityInfo_ != nullptr) ? abilityInfo_->resourcePath : "";
}

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
void ContextDeal::StartAbility(const AAFwk::Want &want, int requestCode)
{
    APP_LOGI("ContextDeal::StartAbility is called");
}

/**
 * @brief Remove permissions for all users who have access to specific permissions
 *
 * @param permission Indicates the permission to unauth. This parameter cannot be null.
 * @param uri Indicates the URI to unauth. This parameter cannot be null.
 * @param uid Indicates the UID of the unauth to check.
 *
 */
void ContextDeal::UnauthUriPermission(const std::string &permission, const Uri &uri, int uid)
{}

/**
 * @brief Obtains an ability manager.
 * The ability manager provides information about running processes and memory usage of an application.
 *
 * @return Returns an IAbilityManager instance.
 */
sptr<AAFwk::IAbilityManager> ContextDeal::GetAbilityManager()
{
    auto remoteObject = OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
    if (remoteObject == nullptr) {
        APP_LOGE("failed to get ability manager service");
        return nullptr;
    }

    APP_LOGI("get bundle ability proxy success.");
    return iface_cast<AAFwk::IAbilityManager>(remoteObject);
}

/**
 * @brief Obtains the type of this application.
 *
 * @return Returns system if this application is a system application;
 * returns normal if it is released in OHOS AppGallery;
 * returns other if it is released by a third-party vendor;
 * returns an empty string if the query fails.
 */
std::string ContextDeal::GetAppType()
{
    sptr<IBundleMgr> ptr = GetBundleManager();
    if (ptr == nullptr) {
        APP_LOGE("GetAppType failed to get bundle manager service");
        return "";
    }

    return ptr->GetAppType(applicationInfo_->bundleName);
}

/**
 * @brief Destroys another ability you had previously started by calling Ability.startAbilityForResult
 * (ohos.aafwk.content.Want, int, ohos.aafwk.ability.startsetting.AbilityStartSetting) with the same requestCode passed.
 *
 * @param requestCode Indicates the request code passed for starting the ability.
 *
 */
void ContextDeal::TerminateAbility(int requestCode)
{}

/**
 * @brief Confirms with the permission management module to check whether a request prompt is required for granting a
 * certain permission. You need to call the current method to check whether a prompt is required before calling
 * requestPermissionsFromUser(java.lang.String[],int) to request a permission. If a prompt is not required, permission
 * request will not be initiated.
 *
 * @param requestCode Indicates the permission to be queried. This parameter cannot be null.
 *
 * @return Returns true if the current application does not have the permission and the user does not turn off further
 * requests; returns false if the current application already has the permission, the permission is rejected by the
 * system, or the permission is denied by the user and the user has turned off further requests.
 */
bool ContextDeal::CanRequestPermission(const std::string &permission)
{
    return false;
}

/**
 * @brief When there is a remote call to check whether the remote has permission, otherwise check whether it has
 * permission
 *
 * @param permissions Indicates the list of permissions to be requested. This parameter cannot be null.
 * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the current process has the permission;
 * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
 */
int ContextDeal::VerifyCallingOrSelfPermission(const std::string &permission)
{
    return 0;
}

/**
 * @brief Query whether the application of the specified PID and UID has been granted a certain permission
 *
 * @param permissions Indicates the list of permissions to be requested. This parameter cannot be null.
 * @param pid Process id
 * @param uid
 * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the current process has the permission;
 * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
 */
int ContextDeal::VerifyPermission(const std::string &permission, int pid, int uid)
{
    return 0;
}

/**
 * @brief Obtains the distributed file path.
 * If the distributed file path does not exist, the system creates one and returns the created path. This method is
 * applicable only to the context of an ability rather than that of an application.
 *
 * @return Returns the distributed file.
 */
std::string ContextDeal::GetDistributedDir()
{
    return "";
}
/**
 * @brief Sets the pattern of this Context based on the specified pattern ID.
 *
 * @param patternId Indicates the resource ID of the pattern to set.
 */
void ContextDeal::SetPattern(int patternId)
{}

/**
 * @brief Obtains the Context object of this ability.
 *
 * @return Returns the Context object of this ability.
 */
std::shared_ptr<Context> ContextDeal::GetAbilityPackageContext()
{
    return nullptr;
}

/**
 * @brief Obtains the HapModuleInfo object of the application.
 *
 * @return Returns the HapModuleInfo object of the application.
 */
std::shared_ptr<HapModuleInfo> ContextDeal::GetHapModuleInfo()
{
    sptr<IBundleMgr> ptr = GetBundleManager();
    if (ptr == nullptr) {
        APP_LOGE("GetHapModuleInfo failed to get bundle manager service");
        return nullptr;
    }

    if (abilityInfo_ == nullptr) {
        APP_LOGE("GetHapModuleInfo failed for abilityInfo_ is nullptr");
        return nullptr;
    }
    HapModuleInfo hapModuleInfo;
    ptr->GetHapModuleInfo(*abilityInfo_.get(), hapModuleInfo);
    return std::make_shared<HapModuleInfo>(hapModuleInfo);
}

/**
 * @brief Obtains the name of the current process.
 *
 * @return Returns the current process name.
 */
std::string ContextDeal::GetProcessName()
{
    return (processInfo_ != nullptr) ? processInfo_->GetProcessName() : "";
}

/**
 * @brief Obtains the bundle name of the ability that called the current ability.
 * You can use the obtained bundle name to check whether the calling ability is allowed to receive the data you will
 * send. If you did not use Ability.startAbilityForResult(ohos.aafwk.content.Want, int,
 * ohos.aafwk.ability.startsetting.AbilityStartSetting) to start the calling ability, null is returned.
 *
 * @return Returns the bundle name of the calling ability; returns null if no calling ability is available.
 */
std::string ContextDeal::GetCallingBundle()
{
    return "";
}

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
void ContextDeal::RequestPermissionsFromUser(std::vector<std::string> &permissions, int requestCode)
{}

/**
 * @brief Starts a new ability with special ability start setting.
 *
 * @param want Indicates the Want containing information about the target ability to start.
 * @param requestCode Indicates the request code returned after the ability is started. You can define the request code
 * to identify the results returned by abilities. The value ranges from 0 to 65535.
 * @param abilityStartSetting Indicates the special start setting used in starting ability.
 *
 */
void ContextDeal::StartAbility(const Want &want, int requestCode, const AbilityStartSetting &abilityStartSetting)
{}

/**
 * @brief Destroys the current ability.
 *
 */
void ContextDeal::TerminateAbility()
{}

/**
 * @brief Connects the current ability to an ability
 *
 * @param want Indicates the want containing information about the ability to connect
 *
 * @param conn Indicates the callback object when the target ability is connected.
 *
 * @return True means success and false means failure
 */
bool ContextDeal::ConnectAbility(const Want &want, const sptr<AAFwk::IAbilityConnection> &conn)
{
    return false;
}

/**
 * @brief Disconnects the current ability from an ability
 *
 * @param conn Indicates the IAbilityConnection callback object passed by connectAbility after the connection
 *              is set up. The IAbilityConnection object uniquely identifies a connection between two abilities.
 */
void ContextDeal::DisconnectAbility(const sptr<AAFwk::IAbilityConnection> &conn)
{}

sptr<IRemoteObject> ContextDeal::GetToken()
{
    return nullptr;
}

/**
 * @brief init the ResourceManager for ContextDeal.
 *
 * @param the ResourceManager has been inited.
 *
 */
void ContextDeal::initResourceManager(const std::shared_ptr<Global::Resource::ResourceManager> &resourceManager)
{
    resourceManager_ = resourceManager;
}

/**
 * @brief Obtains information about the caller of this ability.
 *
 * @return Returns the caller information.
 */
Uri ContextDeal::GetCaller()
{
    Uri uri(uriString_);
    return uri;
}

/**
 * @brief SerUriString
 */
void ContextDeal::SerUriString(const std::string &uri)
{
    uriString_ = uri;
}

}  // namespace AppExecFwk
}  // namespace OHOS