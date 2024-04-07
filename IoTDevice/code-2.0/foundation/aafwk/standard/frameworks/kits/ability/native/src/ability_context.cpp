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

#include "ability_context.h"
#include "ability_manager_client.h"
#include "app_log_wrapper.h"
#include "resource_manager.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {
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
void AbilityContext::StartAbility(const AAFwk::Want &want, int requestCode)
{
    APP_LOGI("AbilityContext::StartAbility called");

    AppExecFwk::AbilityType type = GetAbilityInfoType();
    if (type != AppExecFwk::AbilityType::PAGE && type != AppExecFwk::AbilityType::SERVICE) {
        APP_LOGE("AbilityContext::StartAbility AbilityType = %{public}d", type);
        return;
    }

    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want, token_, requestCode);
    if (err != ERR_OK) {
        APP_LOGE("AbilityContext::StartAbility is failed %{public}d", err);
    }
}

/**
 * @brief Starts a new ability with special ability start setting.
 *
 * @param want Indicates the Want containing information about the target ability to start.
 * @param requestCode Indicates the request code returned after the ability is started. You can define the request code
 * to identify the results returned by abilities. The value ranges from 0 to 65535.
 * @param abilityStartSetting Indicates the special start setting used in starting ability.
 *
 */
void AbilityContext::StartAbility(const Want &want, int requestCode, const AbilityStartSetting &abilityStartSetting)
{
    AppExecFwk::AbilityType type = GetAbilityInfoType();
    if (AppExecFwk::AbilityType::PAGE != type && AppExecFwk::AbilityType::SERVICE != type) {
        APP_LOGE("AbilityContext::StartAbility AbilityType = %{public}d", type);
        return;
    }
}

/**
 * @brief Destroys another ability you had previously started by calling Ability.startAbilityForResult
 * (ohos.aafwk.content.Want, int, ohos.aafwk.ability.startsetting.AbilityStartSetting) with the same requestCode passed.
 *
 * @param requestCode Indicates the request code passed for starting the ability.
 *
 */
void AbilityContext::TerminateAbility(int requestCode)
{
    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->TerminateAbility(token_, requestCode);
    if (err != ERR_OK) {
        APP_LOGE("AbilityContext::TerminateAbility is failed %{public}d", err);
    }
}

/**
 * @brief Destroys the current ability.
 *
 */
void AbilityContext::TerminateAbility()
{
    std::shared_ptr<AbilityInfo> info = GetAbilityInfo();
    if (info == nullptr) {
        APP_LOGE("AbilityContext::TerminateAbility info == nullptr");
        return;
    }

    ErrCode err = ERR_OK;

    switch (info->type) {
        case AppExecFwk::AbilityType::PAGE:
            err = AAFwk::AbilityManagerClient::GetInstance()->TerminateAbility(token_, resultCode_, &resultWant_);
            break;
        case AppExecFwk::AbilityType::SERVICE:
            err = AAFwk::AbilityManagerClient::GetInstance()->TerminateAbility(token_, -1, nullptr);
            break;
        default:
            APP_LOGE("AbilityContext::TerminateAbility info type error is %{public}d", info->type);
            break;
    }

    if (err != ERR_OK) {
        APP_LOGE("AbilityContext::TerminateAbility is failed %{public}d", err);
    }
}  // namespace AppExecFwk

/**
 * @brief Obtains the bundle name of the ability that called the current ability.
 * You can use the obtained bundle name to check whether the calling ability is allowed to receive the data you will
 * send. If you did not use Ability.startAbilityForResult(ohos.aafwk.content.Want, int,
 * ohos.aafwk.ability.startsetting.AbilityStartSetting) to start the calling ability, null is returned.
 *
 * @return Returns the bundle name of the calling ability; returns null if no calling ability is available.
 */
std::string AbilityContext::GetCallingBundle()
{
    return callingBundleName_;
}

/**
 * @brief Obtains the ohos.bundle.ElementName object of the current ability.
 *
 * @return Returns the ohos.bundle.ElementName object of the current ability.
 */
std::shared_ptr<ElementName> AbilityContext::GetElementName()
{
    std::shared_ptr<AbilityInfo> info = GetAbilityInfo();
    if (info == nullptr) {
        APP_LOGE("AbilityContext::GetElementName info == nullptr");
        return nullptr;
    }

    std::shared_ptr<ElementName> elementName = std::make_shared<ElementName>();
    if (elementName == nullptr) {
        APP_LOGE("AbilityContext::GetElementName elementName == nullptr");
        return nullptr;
    }
    elementName->SetAbilityName(info->name);
    elementName->SetBundleName(info->bundleName);
    elementName->SetDeviceID(info->deviceId);
    return elementName;
}

/**
 * @brief Obtains the ElementName of the ability that called the current ability.
 *
 * @return Returns the ElementName of the calling ability; returns null if no calling ability is available.
 */
std::shared_ptr<ElementName> AbilityContext::GetCallingAbility()
{
    std::shared_ptr<ElementName> elementName = std::make_shared<ElementName>();

    if (elementName == nullptr) {
        APP_LOGE("AbilityContext::GetElementName elementName == nullptr");
        return nullptr;
    }
    elementName->SetAbilityName(callingAbilityName_);
    elementName->SetBundleName(callingBundleName_);
    elementName->SetDeviceID(callingDeviceId_);
    return elementName;
}

/**
 * @brief Connects the current ability to an ability
 *
 * @param want Indicates the want containing information about the ability to connect
 *
 * @param conn Indicates the callback object when the target ability is connected.
 *
 * @return True means success and false means failure
 */
bool AbilityContext::ConnectAbility(const Want &want, const sptr<AAFwk::IAbilityConnection> &conn)
{
    APP_LOGD("AbilityContext::ConnectAbility called");

    AppExecFwk::AbilityType type = GetAbilityInfoType();
    if (AppExecFwk::AbilityType::PAGE != type && AppExecFwk::AbilityType::SERVICE != type) {
        APP_LOGE("AbilityContext::ConnectAbility AbilityType = %{public}d", type);
        return false;
    }

    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, conn, token_);
    bool value = ((ret == ERR_OK) ? true : false);
    if (!value) {
        APP_LOGE("AbilityContext::ConnectAbility ErrorCode = %{public}d", ret);
    }
    return value;
}

/**
 *
 * @param conn Indicates the IAbilityConnection callback object passed by connectAbility after the connection
 *              is set up. The IAbilityConnection object uniquely identifies a connection between two abilities.
 */
void AbilityContext::DisconnectAbility(const sptr<AAFwk::IAbilityConnection> &conn)
{
    APP_LOGD("AbilityContext::DisconnectAbility called");

    AppExecFwk::AbilityType type = GetAbilityInfoType();
    if (AppExecFwk::AbilityType::PAGE != type && AppExecFwk::AbilityType::SERVICE != type) {
        APP_LOGE("AbilityContext::DisconnectAbility AbilityType = %{public}d", type);
        return;
    }

    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(conn);
    if (ret != ERR_OK) {
        APP_LOGE("AbilityContext::DisconnectAbility error");
    }
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
bool AbilityContext::StopAbility(const AAFwk::Want &want)
{
    AppExecFwk::AbilityType type = GetAbilityInfoType();
    if (AppExecFwk::AbilityType::PAGE != type && AppExecFwk::AbilityType::SERVICE != type) {
        APP_LOGE("AbilityContext::StopAbility AbilityType = %{public}d", type);
        return false;
    }

    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->StopServiceAbility(want);
    if (err != ERR_OK) {
        APP_LOGE("AbilityContext::StopAbility is failed %{public}d", err);
        return false;
    }

    return true;
}

sptr<IRemoteObject> AbilityContext::GetToken()
{
    return token_;
}

/**
 * @brief Obtains information about the current application. The returned application information includes basic
 * information such as the application name and application permissions.
 *
 * @return Returns the ApplicationInfo for the current application.
 */
std::shared_ptr<ApplicationInfo> AbilityContext::GetApplicationInfo() const
{
    return ContextContainer::GetApplicationInfo();
}

/**
 * @brief Obtains the application-specific cache directory on the device's internal storage. The system
 * automatically deletes files from the cache directory if disk space is required elsewhere on the device.
 * Older files are always deleted first.
 *
 * @return Returns the application-specific cache directory.
 */
std::string AbilityContext::GetCacheDir()
{
    return ContextContainer::GetCacheDir();
}

/**
 * @brief Obtains the application-specific code-cache directory on the device's internal storage.
 * The system will delete any files stored in this location both when your specific application is upgraded,
 * and when the entire platform is upgraded.
 *
 * @return Returns the application-specific code-cache directory.
 */
std::string AbilityContext::GetCodeCacheDir()
{
    return ContextContainer::GetCodeCacheDir();
}

/**
 * @brief Obtains the local database path.
 * If the local database path does not exist, the system creates one and returns the created path.
 *
 * @return Returns the local database file.
 */
std::string AbilityContext::GetDatabaseDir()
{
    return ContextContainer::GetDatabaseDir();
}

/**
 * @brief Obtains the absolute path where all private data files of this application are stored.
 *
 * @return Returns the absolute path storing all private data files of this application.
 */
std::string AbilityContext::GetDataDir()
{
    return ContextContainer::GetDataDir();
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
std::string AbilityContext::GetDir(const std::string &name, int mode)
{
    return ContextContainer::GetDir(name, mode);
}

/**
 * @brief Obtains an IBundleMgr instance.
 * You can use this instance to obtain information about the application bundle.
 *
 * @return Returns an IBundleMgr instance.
 */
sptr<IBundleMgr> AbilityContext::GetBundleManager() const
{
    return ContextContainer::GetBundleManager();
}

/**
 * @brief Obtains the path of the package containing the current ability. The returned path contains the resources,
 *  source code, and configuration files of a module.
 *
 * @return Returns the path of the package file.
 */
std::string AbilityContext::GetBundleCodePath()
{
    return ContextContainer::GetBundleCodePath();
}

/**
 * @brief Obtains the bundle name of the current ability.
 *
 * @return Returns the bundle name of the current ability.
 */
std::string AbilityContext::GetBundleName()
{
    return ContextContainer::GetBundleName();
}

/**
 * @brief Obtains the path of the OHOS Ability Package (HAP} containing this ability.
 *
 * @return Returns the path of the HAP containing this ability.
 */
std::string AbilityContext::GetBundleResourcePath()
{
    return ContextContainer::GetBundleResourcePath();
}

/**
 * @brief Obtains the Context object of the application.
 *
 * @return Returns the Context object of the application.
 */
std::shared_ptr<Context> AbilityContext::GetApplicationContext() const
{
    return ContextContainer::GetApplicationContext();
}

/**
 * @brief Obtains the Context object of the application.
 *
 * @return Returns the Context object of the application.
 */
std::shared_ptr<Context> AbilityContext::GetContext()
{
    return ContextContainer::GetContext();
}

/**
 * @brief Obtains an ability manager.
 * The ability manager provides information about running processes and memory usage of an application.
 *
 * @return Returns an IAbilityManager instance.
 */
sptr<AAFwk::IAbilityManager> AbilityContext::GetAbilityManager()
{
    return ContextContainer::GetAbilityManager();
}

/**
 * Called when getting the ProcessInfo
 *
 * @return ProcessInfo
 */
std::shared_ptr<ProcessInfo> AbilityContext::GetProcessInfo() const
{
    return ContextContainer::GetProcessInfo();
}

/**
 * @brief Obtains the type of this application.
 *
 * @return Returns system if this application is a system application;
 * returns normal if it is released in official AppGallery;
 * returns other if it is released by a third-party vendor;
 * returns an empty string if the query fails.
 */
std::string AbilityContext::GetAppType()
{
    return ContextContainer::GetAppType();
}

/**
 * @brief Obtains information about the current ability.
 * The returned information includes the class name, bundle name, and other information about the current ability.
 *
 * @return Returns the AbilityInfo object for the current ability.
 */
const std::shared_ptr<AbilityInfo> AbilityContext::GetAbilityInfo()
{
    return ContextContainer::GetAbilityInfo();
}

/**
 * @brief Obtains the HapModuleInfo object of the application.
 *
 * @return Returns the HapModuleInfo object of the application.
 */
std::shared_ptr<HapModuleInfo> AbilityContext::GetHapModuleInfo()
{
    return ContextContainer::GetHapModuleInfo();
}

/**
 * @brief Get Current Ability Type
 *
 * @return Current Ability Type
 */
AppExecFwk::AbilityType AbilityContext::GetAbilityInfoType()
{
    std::shared_ptr<AbilityInfo> info = GetAbilityInfo();
    if (info == nullptr) {
        APP_LOGE("AbilityContext::GetAbilityInfoType info == nullptr");
        return AppExecFwk::AbilityType::UNKNOWN;
    }

    return info->type;
}

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
std::shared_ptr<Context> AbilityContext::CreateBundleContext(std::string bundleName, int flag)
{
    return ContextContainer::CreateBundleContext(bundleName, flag);
}

/**
 * @brief Obtains a resource manager.
 *
 * @return Returns a ResourceManager object.
 */
std::shared_ptr<Global::Resource::ResourceManager> AbilityContext::GetResourceManager() const
{
    std::shared_ptr<Context> appcontext = GetApplicationContext();
    if (appcontext == nullptr) {
        APP_LOGE("AbilityContext::GetResourceManager appcontext is nullptr");
        return nullptr;
    }

    std::shared_ptr<Global::Resource::ResourceManager> resourceManager = appcontext->GetResourceManager();
    if (resourceManager == nullptr) {
        APP_LOGE("AbilityContext::GetResourceManager resourceManager is nullptr");
        return nullptr;
    }

    return resourceManager;
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
int AbilityContext::VerifySelfPermission(const std::string &permission)
{
    if (permission.empty()) {
        APP_LOGE("VerifySelfPermission permission invalid");
        return AppExecFwk::Constants::PERMISSION_NOT_GRANTED;
    }

    std::string bundle_name = GetBundleName();
    if (bundle_name.empty()) {
        APP_LOGE("VerifySelfPermission failed to get bundle name error");
        return AppExecFwk::Constants::PERMISSION_NOT_GRANTED;
    }

    sptr<IBundleMgr> ptr = GetBundleManager();
    if (ptr == nullptr) {
        APP_LOGE("VerifySelfPermission failed to get bundle manager service");
        return AppExecFwk::Constants::PERMISSION_NOT_GRANTED;
    }
    return ptr->CheckPermission(bundle_name, permission);
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
int AbilityContext::VerifyCallingPermission(const std::string &permission)
{
    if (permission.empty()) {
        APP_LOGE("VerifyCallingPermission permission invalid");
        return AppExecFwk::Constants::PERMISSION_NOT_GRANTED;
    }

    std::string bundle_name = GetCallingBundle();
    if (bundle_name.empty()) {
        APP_LOGE("VerifyCallingPermission failed to get bundle name by uid");
        return AppExecFwk::Constants::PERMISSION_NOT_GRANTED;
    }

    sptr<IBundleMgr> ptr = GetBundleManager();
    if (ptr == nullptr) {
        APP_LOGE("VerifyCallingPermission failed to get bundle manager service");
        return AppExecFwk::Constants::PERMISSION_NOT_GRANTED;
    }
    return ptr->CheckPermission(bundle_name, permission);
}

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
bool AbilityContext::CanRequestPermission(const std::string &permission)
{
    if (permission.empty()) {
        APP_LOGE("CanRequestPermission permission invalid");
        return true;
    }

    std::string bundle_name = GetBundleName();
    if (bundle_name.empty()) {
        APP_LOGE("CanRequestPermission failed to get bundle name error");
        return true;
    }

    sptr<IBundleMgr> ptr = GetBundleManager();
    if (ptr == nullptr) {
        APP_LOGE("CanRequestPermission failed to get bundle manager service");
        return true;
    }

    return ptr->CanRequestPermission(bundle_name, permission, 0);
}

/**
 * @brief When there is a remote call to check whether the remote has permission, otherwise check whether it has
 * permission
 *
 * @param permissions Indicates the list of permissions to be requested. This parameter cannot be null.
 * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the current process has the permission;
 * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
 */
int AbilityContext::VerifyCallingOrSelfPermission(const std::string &permission)
{
    return VerifySelfPermission(permission);
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
int AbilityContext::VerifyPermission(const std::string &permission, int pid, int uid)
{
    if (permission.empty()) {
        APP_LOGE("VerifyPermission permission invalid");
        return AppExecFwk::Constants::PERMISSION_NOT_GRANTED;
    }

    sptr<IBundleMgr> ptr = GetBundleManager();
    if (ptr == nullptr) {
        APP_LOGE("VerifyPermission failed to get bundle manager service");
        return AppExecFwk::Constants::PERMISSION_NOT_GRANTED;
    }

    std::string bundle_name;
    if (!ptr->GetBundleNameForUid(uid, bundle_name)) {
        APP_LOGE("VerifyPermission failed to get bundle name by uid");
        return AppExecFwk::Constants::PERMISSION_NOT_GRANTED;
    }

    return ptr->CheckPermission(bundle_name, permission);
}

/**
 * @brief Requests certain permissions from the system.
 * This method is called for permission request. This is an asynchronous method. When it is executed,
 * the Ability.onRequestPermissionsFromUserResult(int, String[], int[]) method will be called back.
 *
 * @param permissions Indicates the list of permissions to be requested. This parameter cannot be null.
 * @param requestCode Indicates the request code to be passed to the Ability.onRequestPermissionsFromUserResult(int,
 * String[], int[]) callback method. This code cannot be a negative number.
 */
void AbilityContext::RequestPermissionsFromUser(std::vector<std::string> &permissions, int requestCode)
{
    if (permissions.size() == 0) {
        APP_LOGE("AbilityContext::RequestPermissionsFromUser permissions is empty");
        return;
    }

    if (requestCode < 0) {
        APP_LOGE("AbilityContext::RequestPermissionsFromUser requestCode should be >= 0");
        return;
    }

    AAFwk::Want want;
    want.SetElementName("com.ohos.systemui", "com.ohos.systemdialog.MainAbility");
    want.SetParam(OHOS_PERMISSIONS_REQUEST_KEY, permissions);
    want.SetParam(OHOS_PERMISSIONS_REQUEST_USER_ID_KEY, 0);
    want.SetParam(OHOS_PERMISSIONS_REQUEST_BUNDLE_NAME_KEY, GetBundleName());

    StartAbility(want, requestCode);
}

/* @brief Deletes the specified private file associated with the application.
 *
 * @param fileName Indicates the name of the file to delete. The file name cannot contain path separators.
 *
 * @return Returns true if the file is deleted successfully; returns false otherwise.
 */
bool AbilityContext::DeleteFile(const std::string &fileName)
{
    return ContextContainer::DeleteFile(fileName);
}

/**
 * @brief Set deviceId/bundleName/abilityName of the calling ability
 *
 * @param deviceId deviceId of the calling ability
 *
 * @param deviceId bundleName of the calling ability
 *
 * @param deviceId abilityName of the calling ability
 */
void AbilityContext::SetCallingContext(
    const std::string &deviceId, const std::string &bundleName, const std::string &abilityName)
{
    callingDeviceId_ = deviceId;
    callingBundleName_ = bundleName;
    callingAbilityName_ = abilityName;
}

/**
 * @brief Obtains information about the caller of this ability.
 *
 * @return Returns the caller information.
 */
Uri AbilityContext::GetCaller()
{
    return ContextContainer::GetCaller();
}

/**
 * Attaches a Context object to the current ability.
 * Generally, this method is called after Ability is loaded to provide the application context for the current
 * ability.
 *
 * @param base Indicates a Context object.
 */
void AbilityContext::AttachBaseContext(const std::shared_ptr<Context> &base)
{
    ContextContainer::AttachBaseContext(base);
}

/**
 * @brief Obtains the absolute path to the application-specific cache directory
 * on the primary external or shared storage device.
 *
 * @return Returns the absolute path to the application-specific cache directory on the external or
 * shared storage device; returns null if the external or shared storage device is temporarily unavailable.
 */
std::string AbilityContext::GetExternalCacheDir()
{
    return ContextContainer::GetExternalCacheDir();
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
std::string AbilityContext::GetExternalFilesDir(std::string &type)
{
    return ContextContainer::GetExternalFilesDir(type);
}

/**
 * @brief Obtains the directory for storing files for the application on the device's internal storage.
 *
 * @return Returns the application file directory.
 */
std::string AbilityContext::GetFilesDir()
{
    return ContextContainer::GetFilesDir();
}

/**
 * @brief Obtains the absolute path which app created and will be excluded from automatic backup to remote storage.
 * The returned path maybe changed if the application is moved to an adopted storage device.
 *
 * @return The path of the directory holding application files that will not be automatically backed up to remote
 * storage.
 */
std::string AbilityContext::GetNoBackupFilesDir()
{
    return ContextContainer::GetNoBackupFilesDir();
}

/**
 * @brief Remove permissions for all users who have access to specific permissions
 *
 * @param permission Indicates the permission to unauth. This parameter cannot be null.
 * @param uri Indicates the URI to unauth. This parameter cannot be null.
 * @param uid Indicates the UID of the unauth to check.
 *
 */
void AbilityContext::UnauthUriPermission(const std::string &permission, const Uri &uri, int uid)
{
    ContextContainer::UnauthUriPermission(permission, uri, uid);
}

/**
 * @brief Obtains the distributed file path.
 * If the distributed file path does not exist, the system creates one and returns the created path. This method is
 * applicable only to the context of an ability rather than that of an application.
 *
 * @return Returns the distributed file.
 */
std::string AbilityContext::GetDistributedDir()
{
    return ContextContainer::GetDistributedDir();
}

/**
 * @brief Sets the pattern of this Context based on the specified pattern ID.
 *
 * @param patternId Indicates the resource ID of the pattern to set.
 */
void AbilityContext::SetPattern(int patternId)
{
    ContextContainer::SetPattern(patternId);
}

/**
 * @brief Obtains the Context object of this ability.
 *
 * @return Returns the Context object of this ability.
 */
std::shared_ptr<Context> AbilityContext::GetAbilityPackageContext()
{
    return ContextContainer::GetAbilityPackageContext();
}

/**
 * @brief Obtains the name of the current process.
 *
 * @return Returns the current process name.
 */
std::string AbilityContext::GetProcessName()
{
    return ContextContainer::GetProcessName();
}

/**
 * @brief InitResourceManager
 *
 * @param bundleInfo  BundleInfo
 */
void AbilityContext::InitResourceManager(BundleInfo &bundleInfo, std::shared_ptr<ContextDeal> &deal)
{
    ContextContainer::InitResourceManager(bundleInfo, deal);
}
}  // namespace AppExecFwk
}  // namespace OHOS
