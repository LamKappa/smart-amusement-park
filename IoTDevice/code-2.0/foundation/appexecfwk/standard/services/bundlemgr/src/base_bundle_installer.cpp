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

#include "base_bundle_installer.h"

#include <vector>
#include <unistd.h>

#include "datetime_ex.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "app_log_wrapper.h"
#include "installd_client.h"
#include "perf_profile.h"
#include "system_ability_helper.h"
#include "ability_manager_interface.h"
#include "bundle_constants.h"
#include "bundle_extractor.h"
#include "bundle_mgr_service.h"
#include "bundle_parser.h"
#include "bundle_util.h"
#include "bundle_permission_mgr.h"
#include "bundle_verify_mgr.h"

namespace OHOS {
namespace AppExecFwk {
namespace {

bool KillApplicationProcesses(const std::string &bundleName)
{
    APP_LOGI("kill running processes, app name is %{public}s", bundleName.c_str());
    sptr<AAFwk::IAbilityManager> abilityMgrProxy =
        iface_cast<AAFwk::IAbilityManager>(SystemAbilityHelper::GetSystemAbility(ABILITY_MGR_SERVICE_ID));
    if (!abilityMgrProxy) {
        APP_LOGE("fail to find the app mgr service to kill application");
        return false;
    }
    if (abilityMgrProxy->KillProcess(bundleName) != 0) {
        APP_LOGE("kill application process failed");
        return false;
    }
    return true;
}

bool UninstallApplicationProcesses(const std::string &bundleName)
{
    APP_LOGI("uninstall kill running processes, app name is %{public}s", bundleName.c_str());
    sptr<AAFwk::IAbilityManager> abilityMgrProxy =
        iface_cast<AAFwk::IAbilityManager>(SystemAbilityHelper::GetSystemAbility(ABILITY_MGR_SERVICE_ID));
    if (!abilityMgrProxy) {
        APP_LOGE("fail to find the app mgr service to kill application");
        return false;
    }
    if (abilityMgrProxy->UninstallApp(bundleName) != 0) {
        APP_LOGE("kill application process failed");
        return false;
    }
    return true;
}

class ScopeGuard final {
public:
    using Function = std::function<void()>;
    explicit ScopeGuard(Function fn) : fn_(fn), dismissed_(false)
    {}

    ~ScopeGuard()
    {
        if (!dismissed_) {
            fn_();
        }
    }

    void Dismiss()
    {
        dismissed_ = true;
    }

private:
    Function fn_;
    bool dismissed_;
};

}  // namespace

BaseBundleInstaller::BaseBundleInstaller()
{
    APP_LOGI("base bundle installer instance is created");
}

BaseBundleInstaller::~BaseBundleInstaller()
{
    APP_LOGI("base bundle installer instance is destroyed");
}

ErrCode BaseBundleInstaller::InstallBundle(
    const std::string &bundlePath, const InstallParam &installParam, const Constants::AppType appType)
{
    APP_LOGI("begin to process %{public}s bundle install", bundlePath.c_str());
    PerfProfile::GetInstance().SetBundleInstallStartTime(GetTickCount());

    ErrCode result = ProcessBundleInstall(bundlePath, installParam, appType);
    if (dataMgr_ && !bundleName_.empty() && !modulePackage_.empty()) {
        dataMgr_->NotifyBundleStatus(
            bundleName_, modulePackage_, mainAbility_, result, isAppExist_ ? NotifyType::UPDATE : NotifyType::INSTALL);
    }

    PerfProfile::GetInstance().SetBundleInstallEndTime(GetTickCount());
    APP_LOGI("finish to process %{public}s bundle install", bundlePath.c_str());
    return result;
}

ErrCode BaseBundleInstaller::UninstallBundle(const std::string &bundleName, const InstallParam &installParam)
{
    APP_LOGD("begin to process %{public}s bundle uninstall", bundleName.c_str());
    PerfProfile::GetInstance().SetBundleUninstallStartTime(GetTickCount());

    ErrCode result = ProcessBundleUninstall(bundleName, installParam);
    if (dataMgr_) {
        dataMgr_->NotifyBundleStatus(
            bundleName, Constants::EMPTY_STRING, Constants::EMPTY_STRING, result, NotifyType::UNINSTALL_BUNDLE);
    }

    PerfProfile::GetInstance().SetBundleUninstallEndTime(GetTickCount());
    APP_LOGD("finish to process %{public}s bundle uninstall", bundleName.c_str());
    return result;
}

ErrCode BaseBundleInstaller::UninstallBundle(
    const std::string &bundleName, const std::string &modulePackage, const InstallParam &installParam)
{
    APP_LOGD("begin to process %{public}s module in %{public}s uninstall", modulePackage.c_str(), bundleName.c_str());
    PerfProfile::GetInstance().SetBundleUninstallStartTime(GetTickCount());

    ErrCode result = ProcessBundleUninstall(bundleName, modulePackage, installParam);
    if (dataMgr_) {
        dataMgr_->NotifyBundleStatus(
            bundleName, modulePackage, Constants::EMPTY_STRING, result, NotifyType::UNINSTALL_MODULE);
    }

    PerfProfile::GetInstance().SetBundleUninstallEndTime(GetTickCount());
    APP_LOGD("finish to process %{public}s module in %{public}s uninstall", modulePackage.c_str(), bundleName.c_str());
    return result;
}

void BaseBundleInstaller::UpdateInstallerState(const InstallerState state)
{
    APP_LOGI("UpdateInstallerState in BaseBundleInstaller state %{public}d", state);
    SetInstallerState(state);
}

ErrCode BaseBundleInstaller::ProcessBundleInstall(
    const std::string &inBundlePath, const InstallParam &installParam, const Constants::AppType appType)
{
    APP_LOGI("ProcessBundleInstall bundlePath %{public}s", inBundlePath.c_str());
    if (installParam.userId == Constants::INVALID_USERID) {
        APP_LOGE("invalid userId");
        return ERR_APPEXECFWK_INSTALL_PARAM_ERROR;
    }

    std::string bundlePath;
    ErrCode result = BundleUtil::CheckFilePath(inBundlePath, bundlePath);
    if (result != ERR_OK) {
        APP_LOGE("hap file check failed %{public}d", result);
        return result;
    }
    UpdateInstallerState(InstallerState::INSTALL_BUNDLE_CHECKED);

    Security::Verify::HapVerifyResult hapVerifyResult;
    if (!BundleVerifyMgr::HapVerify(bundlePath, hapVerifyResult)) {
        APP_LOGE("hap file verify failed");
        return ERR_APPEXECFWK_INSTALL_NO_SIGNATURE_INFO;
    }

    // parse the single bundle info to get the bundle name.
    InnerBundleInfo newInfo;
    modulePath_ = bundlePath;
    newInfo.SetAppType(appType);
    newInfo.SetUserId(installParam.userId);
    newInfo.SetIsKeepData(installParam.isKeepData);
    auto provisionInfo = hapVerifyResult.GetProvisionInfo();
    newInfo.SetProvisionId(provisionInfo.appId);
    newInfo.SetAppFeature(provisionInfo.bundleInfo.appFeature);

    if (!ModifyInstallDirByHapType(newInfo)) {
        APP_LOGE("modify bundle install dir failed %{public}d", result);
        return ERR_APPEXECFWK_INSTALL_PARAM_ERROR;
    }
    result = ParseBundleInfo(bundlePath, newInfo);
    if (result != ERR_OK) {
        APP_LOGE("bundle parse failed %{public}d", result);
        return result;
    }
    UpdateInstallerState(InstallerState::INSTALL_PARSED);

    bundleName_ = newInfo.GetBundleName();
    modulePackage_ = newInfo.GetCurrentModulePackage();
    mainAbility_ = newInfo.GetMainAbilityName();
    if (modulePackage_.empty()) {
        APP_LOGE("get current package failed %{public}d", result);
        return ERR_APPEXECFWK_INSTALL_PARAM_ERROR;
    }

    // try to get the bundle info to decide use install or update.
    InnerBundleInfo oldInfo;
    dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (!dataMgr_) {
        APP_LOGE("Get dataMgr shared_ptr nullptr");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    isAppExist_ = dataMgr_->GetInnerBundleInfo(bundleName_, Constants::CURRENT_DEVICE_ID, oldInfo);
    if (isAppExist_) {
        APP_LOGI("app is exist");
        bool isReplace = (installParam.installFlag == InstallFlag::REPLACE_EXISTING);
        return ProcessBundleUpdateStatus(oldInfo, newInfo, isReplace);  // app exist, but module may not
    }
    return ProcessBundleInstallStatus(newInfo);
}

ErrCode BaseBundleInstaller::ProcessBundleUninstall(const std::string &bundleName, const InstallParam &installParam)
{
    if (bundleName.empty()) {
        APP_LOGE("uninstall bundle name empty");
        return ERR_APPEXECFWK_UNINSTALL_INVALID_NAME;
    }
    if (installParam.userId == Constants::INVALID_USERID) {
        APP_LOGE("invalid userId");
        return ERR_APPEXECFWK_UNINSTALL_PARAM_ERROR;
    }

    dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (!dataMgr_) {
        APP_LOGE("Get dataMgr shared_ptr nullptr");
        return ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    InnerBundleInfo oldInfo;
    if (!dataMgr_->GetInnerBundleInfo(bundleName, Constants::CURRENT_DEVICE_ID, oldInfo)) {
        APP_LOGE("uninstall bundle info missing");
        return ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE;
    }
    ScopeGuard enableGuard([&] { dataMgr_->EnableBundle(bundleName); });
    if (oldInfo.GetAppType() == Constants::AppType::SYSTEM_APP) {
        APP_LOGE("uninstall system app");
        return ERR_APPEXECFWK_UNINSTALL_SYSTEM_APP_ERROR;
    }

    if (!dataMgr_->UpdateBundleInstallState(bundleName, InstallState::UNINSTALL_START)) {
        APP_LOGE("uninstall already start");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    // kill the bundle process during uninstall.
    if (!UninstallApplicationProcesses(oldInfo.GetApplicationName())) {
        APP_LOGE("can not kill process");
        dataMgr_->UpdateBundleInstallState(bundleName, InstallState::INSTALL_SUCCESS);
        return ERR_APPEXECFWK_UNINSTALL_KILLING_APP_ERROR;
    }
    enableGuard.Dismiss();
    ErrCode result = RemoveBundle(oldInfo);
    if (result != ERR_OK) {
        APP_LOGE("remove whole bundle failed");
        return result;
    }
    APP_LOGD("finish to process %{public}s bundle uninstall", bundleName.c_str());
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ProcessBundleUninstall(
    const std::string &bundleName, const std::string &modulePackage, const InstallParam &installParam)
{
    if (bundleName.empty() || modulePackage.empty()) {
        APP_LOGE("uninstall bundle name or module name empty");
        return ERR_APPEXECFWK_UNINSTALL_INVALID_NAME;
    }
    if (installParam.userId == Constants::INVALID_USERID) {
        APP_LOGE("invalid userId");
        return ERR_APPEXECFWK_UNINSTALL_PARAM_ERROR;
    }

    dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (!dataMgr_) {
        APP_LOGE("Get dataMgr shared_ptr nullptr");
        return ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    InnerBundleInfo oldInfo;
    if (!dataMgr_->GetInnerBundleInfo(bundleName, Constants::CURRENT_DEVICE_ID, oldInfo)) {
        APP_LOGE("uninstall bundle info missing");
        return ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE;
    }
    ScopeGuard enableGuard([&] { dataMgr_->EnableBundle(bundleName); });

    if (oldInfo.GetAppType() == Constants::AppType::SYSTEM_APP) {
        APP_LOGE("uninstall system app");
        return ERR_APPEXECFWK_UNINSTALL_SYSTEM_APP_ERROR;
    }

    bool isModuleExist = oldInfo.FindModule(modulePackage);
    if (!isModuleExist) {
        APP_LOGE("uninstall bundle info missing");
        return ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_MODULE;
    }

    if (!dataMgr_->UpdateBundleInstallState(bundleName, InstallState::UNINSTALL_START)) {
        APP_LOGE("uninstall already start");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    ScopeGuard stateGuard([&] { dataMgr_->UpdateBundleInstallState(bundleName, InstallState::INSTALL_SUCCESS); });
    // kill the bundle process during uninstall.
    if (!UninstallApplicationProcesses(oldInfo.GetApplicationName())) {
        APP_LOGE("can not kill process");
        return ERR_APPEXECFWK_UNINSTALL_KILLING_APP_ERROR;
    }

    // if it is the only module in the bundle
    if (oldInfo.IsOnlyModule(modulePackage)) {
        APP_LOGI("%{public}s is only module", modulePackage.c_str());
        enableGuard.Dismiss();
        stateGuard.Dismiss();
        return RemoveBundle(oldInfo);
    }

    ErrCode result = RemoveModuleAndDataDir(oldInfo, modulePackage);
    if (result != ERR_OK) {
        APP_LOGE("remove module dir failed");
        return result;
    }

    if (!dataMgr_->RemoveModuleInfo(bundleName, modulePackage, oldInfo)) {
        APP_LOGE("RemoveModuleInfo failed");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    APP_LOGI("finish to process %{public}s in %{public}s uninstall", bundleName.c_str(), modulePackage.c_str());
    return ERR_OK;
}

ErrCode BaseBundleInstaller::RemoveBundle(InnerBundleInfo &info)
{
    ErrCode result = RemoveBundleAndDataDir(info, true);
    if (result != ERR_OK) {
        APP_LOGE("remove bundle dir failed");
        dataMgr_->UpdateBundleInstallState(info.GetBundleName(), InstallState::UNINSTALL_FAIL);
        return result;
    }

    if (!dataMgr_->UpdateBundleInstallState(info.GetBundleName(), InstallState::UNINSTALL_SUCCESS)) {
        APP_LOGE("delete inner info failed");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    BundlePermissionMgr::UninstallPermissions(info);
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ProcessBundleInstallStatus(InnerBundleInfo &info)
{
    APP_LOGI("ProcessBundleInstallStatus %{public}s", info.GetBundleName().c_str());
    if (!dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::INSTALL_START)) {
        APP_LOGE("install already start");
        return ERR_APPEXECFWK_INSTALL_STATE_ERROR;
    }
    ScopeGuard stateGuard([&] { dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::INSTALL_FAIL); });
    ErrCode result = CreateBundleAndDataDir(info);
    if (result != ERR_OK) {
        APP_LOGE("create bundle and data dir failed");
        return result;
    }
    UpdateInstallerState(InstallerState::INSTALL_CREATDIR);
    ScopeGuard bundleGuard([&] { RemoveBundleAndDataDir(info, false); });

    result = ExtractModuleAndRename(info);
    if (result != ERR_OK) {
        APP_LOGE("create bundle and data dir failed");
        return result;
    }

    result = CreateModuleDataDir(info);
    if (result != ERR_OK) {
        APP_LOGE("create module data dir failed");
        return result;
    }

    if (!dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::INSTALL_SUCCESS)) {
        APP_LOGE("update bundle %{public}s failed", bundleName_.c_str());
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    if (!dataMgr_->AddInnerBundleInfo(bundleName_, info)) {
        APP_LOGE("add bundle %{public}s info failed", bundleName_.c_str());
        dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::UNINSTALL_START);
        dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::UNINSTALL_SUCCESS);
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    UpdateInstallerState(InstallerState::INSTALL_INFO_SAVED);
    stateGuard.Dismiss();
    bundleGuard.Dismiss();

    BundlePermissionMgr::InstallPermissions(info);
    UpdateInstallerState(InstallerState::INSTALL_SUCCESS);
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ProcessBundleUpdateStatus(
    InnerBundleInfo &oldInfo, InnerBundleInfo &newInfo, bool isReplace)
{
    // this state should always be set when return
    ScopeGuard enableGuard([&] { dataMgr_->EnableBundle(bundleName_); });
    if (oldInfo.GetVersionCode() > newInfo.GetVersionCode()) {
        APP_LOGE("fail to update lower version bundle");
        return ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE;
    }

    if (oldInfo.GetProvisionId() != newInfo.GetProvisionId()) {
        APP_LOGE("the signature of the new bundle is not the same as old one");
        return ERR_APPEXECFWK_INSTALL_UPDATE_INCOMPATIBLE;
    }
    // now there are two cases for updating:
    // 1. bundle exist, hap exist, update hap
    // 2. bundle exist, install new hap
    bool isModuleExist = oldInfo.FindModule(modulePackage_);
    if (isModuleExist && !isReplace) {
        APP_LOGE("fail to install already existing bundle using normal flag");
        return ERR_APPEXECFWK_INSTALL_ALREADY_EXIST;
    }
    if (!dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::UPDATING_START)) {
        APP_LOGE("update already start");
        return ERR_APPEXECFWK_INSTALL_STATE_ERROR;
    }
    // this state should always be set when return
    ScopeGuard stateGuard([&] { dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::INSTALL_SUCCESS); });
    newInfo.RestoreFromOldInfo(oldInfo);
    ErrCode result = isModuleExist ? ProcessModuleUpdate(newInfo, oldInfo) : ProcessNewModuleInstall(newInfo, oldInfo);
    if (result != ERR_OK) {
        APP_LOGE("install module failed %{public}d", result);
        return result;
    }

    BundlePermissionMgr::UpdatePermissions(newInfo);
    UpdateInstallerState(InstallerState::INSTALL_SUCCESS);
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ProcessNewModuleInstall(InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo)
{
    APP_LOGI("ProcessNewModuleInstall %{public}s", newInfo.GetBundleName().c_str());
    if (newInfo.HasEntry() && oldInfo.HasEntry()) {
        APP_LOGE("install more than one entry module");
        return ERR_APPEXECFWK_INSTALL_ENTRY_ALREADY_EXIST;
    }
    ErrCode result = ExtractModuleAndRename(newInfo);
    if (result != ERR_OK) {
        APP_LOGE("extract module and rename failed");
        return result;
    }
    ScopeGuard moduleGuard([&] { RemoveModuleDir(newInfo); });
    result = CreateModuleDataDir(newInfo);
    if (result != ERR_OK) {
        APP_LOGE("create module data dir failed");
        return result;
    }
    ScopeGuard moduleDataGuard([&] { RemoveModuleDataDir(newInfo); });
    if (!dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::UPDATING_SUCCESS)) {
        APP_LOGE("new moduleupdate state failed");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    if (!dataMgr_->AddNewModuleInfo(bundleName_, newInfo, oldInfo)) {
        APP_LOGE(
            "add module %{public}s to innerBundleInfo %{public}s failed", modulePackage_.c_str(), bundleName_.c_str());
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    moduleGuard.Dismiss();
    moduleDataGuard.Dismiss();
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ProcessModuleUpdate(InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo)
{
    APP_LOGI("ProcessModuleUpdate %{public}s", newInfo.GetBundleName().c_str());
    // kill the bundle process during updating
    if (!KillApplicationProcesses(oldInfo.GetApplicationName())) {
        APP_LOGE("fail to kill running application");
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    ErrCode result = ExtractModuleAndRename(newInfo);
    if (result != ERR_OK) {
        APP_LOGE("extract module and rename failed");
        return result;
    }
    if (!dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::UPDATING_SUCCESS)) {
        APP_LOGE("old module update state failed");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    newInfo.RestoreModuleInfo(oldInfo);
    if (!dataMgr_->UpdateInnerBundleInfo(bundleName_, newInfo, oldInfo)) {
        APP_LOGE("update innerBundleInfo %{public}s failed", bundleName_.c_str());
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    return ERR_OK;
}

ErrCode BaseBundleInstaller::CreateBundleAndDataDir(InnerBundleInfo &info) const
{
    auto appCodePath = baseCodePath_ + Constants::PATH_SEPARATOR + bundleName_;
    auto appDataPath = baseDataPath_ + Constants::PATH_SEPARATOR + bundleName_;
    APP_LOGI("create bundle dir %{public}s", appCodePath.c_str());
    ErrCode result = InstalldClient::GetInstance()->CreateBundleDir(appCodePath);
    if (result != ERR_OK) {
        APP_LOGE("fail to create bundle dir, error is %{public}d", result);
        return result;
    }
    info.SetAppCodePath(appCodePath);

    if (!dataMgr_->GenerateUidAndGid(info)) {
        APP_LOGE("fail to gererate uid and gid");
        InstalldClient::GetInstance()->RemoveBundleDir(appCodePath);
        return ERR_APPEXECFWK_INSTALL_GENERATE_UID_ERROR;
    }
    result = InstalldClient::GetInstance()->CreateBundleDataDir(appDataPath, info.GetUid(), info.GetGid());
    if (result != ERR_OK) {
        APP_LOGE("fail to create bundle data dir, error is %{public}d", result);
        InstalldClient::GetInstance()->RemoveBundleDir(appCodePath);
        return result;
    }
    UpdateBundlePaths(info, appDataPath);
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ExtractModuleAndRename(InnerBundleInfo &info)
{
    auto result = ExtractModuleFiles(info);
    if (result != ERR_OK) {
        APP_LOGE("fail to extrace module dir, error is %{public}d", result);
        return result;
    }
    UpdateInstallerState(InstallerState::INSTALL_EXTRACTED);
    result = RenameModuleDir(info);
    if (result != ERR_OK) {
        APP_LOGE("fail to rename module dir, error is %{public}d", result);
        RemoveModuleDir(info);
        return result;
    }
    UpdateInstallerState(InstallerState::INSTALL_RENAMED);
    return ERR_OK;
}

ErrCode BaseBundleInstaller::RemoveBundleAndDataDir(InnerBundleInfo &info, bool isUninstall) const
{
    auto result = InstalldClient::GetInstance()->RemoveBundleDir(info.GetAppCodePath());
    if (result != ERR_OK) {
        APP_LOGE("fail to remove bundle dir, error is %{public}d", result);
        return result;
    }
    if (!info.GetIsKeepData() || !isUninstall) {
        result = InstalldClient::GetInstance()->RemoveBundleDataDir(info.GetBaseDataDir());
        if (result != ERR_OK) {
            APP_LOGE("fail to remove bundle data dir, error is %{public}d", result);
            return result;
        }
    }
    return ERR_OK;
}

ErrCode BaseBundleInstaller::RemoveModuleAndDataDir(InnerBundleInfo &info, const std::string &modulePackage) const
{
    auto moduleDir = info.GetModuleDir(modulePackage);
    auto result = InstalldClient::GetInstance()->RemoveModuleDir(moduleDir);
    if (result != ERR_OK) {
        APP_LOGE("fail to remove module dir, error is %{public}d", result);
        return result;
    }

    if (!info.GetIsKeepData()) {
        auto moduleDataDir = info.GetModuleDataDir(modulePackage);
        result = InstalldClient::GetInstance()->RemoveModuleDataDir(moduleDataDir);
        if (result != ERR_OK) {
            APP_LOGE("fail to remove bundle data dir, error is %{public}d", result);
            return result;
        }
    }
    return ERR_OK;
}

ErrCode BaseBundleInstaller::RemoveModuleDir(InnerBundleInfo &info) const
{
    std::string moduleDir = info.GetAppCodePath() + Constants::PATH_SEPARATOR + modulePackage_;
    APP_LOGI("module dir %{public}s to be removed", moduleDir.c_str());
    return InstalldClient::GetInstance()->RemoveModuleDir(moduleDir);
}

ErrCode BaseBundleInstaller::RemoveModuleDataDir(InnerBundleInfo &info) const
{
    return InstalldClient::GetInstance()->RemoveModuleDataDir(info.GetModuleDataDir(modulePackage_));
}

ErrCode BaseBundleInstaller::ParseBundleInfo(const std::string &bundleFilePath, InnerBundleInfo &info) const
{
    BundleParser bundleParser;
    ErrCode result = bundleParser.Parse(bundleFilePath, info);
    if (result != ERR_OK) {
        APP_LOGE("parse bundle info failed, error: %{public}d", result);
        return result;
    }
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ExtractModuleFiles(InnerBundleInfo &info)
{
    moduleTmpDir_ = info.GetAppCodePath() + Constants::PATH_SEPARATOR + modulePackage_ + Constants::TMP_SUFFIX;
    APP_LOGI("extract module to %{public}s", moduleTmpDir_.c_str());
    auto result = InstalldClient::GetInstance()->ExtractModuleFiles(modulePath_, moduleTmpDir_);
    if (result != ERR_OK) {
        APP_LOGE("extract module files failed, error is %{public}d", result);
        return result;
    }
    return ERR_OK;
}

ErrCode BaseBundleInstaller::RenameModuleDir(InnerBundleInfo &info) const
{
    auto moduleDir = info.GetAppCodePath() + Constants::PATH_SEPARATOR + modulePackage_;
    APP_LOGI("rename module to %{public}s", moduleDir.c_str());
    auto result = InstalldClient::GetInstance()->RenameModuleDir(moduleTmpDir_, moduleDir);
    if (result != ERR_OK) {
        APP_LOGE("rename module dir failed, error is %{public}d", result);
        return result;
    }
    info.AddModuleSrcDir(moduleDir);
    info.AddModuleResPath(moduleDir);
    return ERR_OK;
}

ErrCode BaseBundleInstaller::CreateModuleDataDir(InnerBundleInfo &info) const
{
    auto moduleDataDir = info.GetBaseDataDir() + Constants::PATH_SEPARATOR + modulePackage_;
    auto result = InstalldClient::GetInstance()->CreateModuleDataDir(
        moduleDataDir, info.GetAbilityNames(), info.GetUid(), info.GetGid());
    if (result != ERR_OK) {
        APP_LOGE("create module data dir failed, error is %{public}d", result);
        return result;
    }
    info.AddModuleDataDir(moduleDataDir);
    return ERR_OK;
}

bool BaseBundleInstaller::ModifyInstallDirByHapType(InnerBundleInfo &info)
{
    auto internalPath = Constants::PATH_SEPARATOR + Constants::USER_ACCOUNT_DIR + Constants::FILE_UNDERLINE +
                        std::to_string(info.GetUserId()) + Constants::PATH_SEPARATOR;
    switch (info.GetAppType()) {
        case Constants::AppType::SYSTEM_APP:
            baseCodePath_ = Constants::SYSTEM_APP_INSTALL_PATH + internalPath + Constants::APP_CODE_DIR;
            baseDataPath_ = Constants::SYSTEM_APP_INSTALL_PATH + internalPath + Constants::APP_DATA_DIR;
            break;
        case Constants::AppType::THIRD_SYSTEM_APP:
            baseCodePath_ = Constants::THIRD_SYSTEM_APP_INSTALL_PATH + internalPath + Constants::APP_CODE_DIR;
            baseDataPath_ = Constants::THIRD_SYSTEM_APP_INSTALL_PATH + internalPath + Constants::APP_DATA_DIR;
            break;
        case Constants::AppType::THIRD_PARTY_APP:
            baseCodePath_ = Constants::THIRD_PARTY_APP_INSTALL_PATH + internalPath + Constants::APP_CODE_DIR;
            baseDataPath_ = Constants::THIRD_PARTY_APP_INSTALL_PATH + internalPath + Constants::APP_DATA_DIR;
            break;
        default:
            APP_LOGE("App type error");
            return false;
    }
    return true;
}

bool BaseBundleInstaller::UpdateBundlePaths(InnerBundleInfo &info, const std::string baseDataPath) const
{
    info.SetBaseDataDir(baseDataPath);
    info.SetAppDataDir(baseDataPath + Constants::PATH_SEPARATOR + Constants::DATA_DIR);
    info.SetAppDataBaseDir(baseDataPath + Constants::PATH_SEPARATOR + Constants::DATA_BASE_DIR);
    info.SetAppCacheDir(baseDataPath + Constants::PATH_SEPARATOR + Constants::CACHE_DIR);
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS
