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

#include "installd_client.h"

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "installd_death_recipient.h"
#include "system_ability_definition.h"
#include "system_ability_helper.h"

namespace OHOS {
namespace AppExecFwk {

ErrCode InstalldClient::CreateBundleDir(const std::string &bundleDir)
{
    if (!GetInstalldProxy()) {
        return ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR;
    }
    return installdProxy_->CreateBundleDir(bundleDir);
}

ErrCode InstalldClient::RemoveBundleDir(const std::string &bundleDir)
{
    if (!GetInstalldProxy()) {
        return ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR;
    }
    return installdProxy_->RemoveBundleDir(bundleDir);
}

ErrCode InstalldClient::RemoveBundleDataDir(const std::string &bundleDataDir)
{
    if (!GetInstalldProxy()) {
        return ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR;
    }
    return installdProxy_->RemoveBundleDir(bundleDataDir);
}

ErrCode InstalldClient::ExtractModuleFiles(const std::string &srcModulePath, const std::string &targetPath)
{
    if (!GetInstalldProxy()) {
        return ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR;
    }
    return installdProxy_->ExtractModuleFiles(srcModulePath, targetPath);
}

ErrCode InstalldClient::RemoveModuleDir(const std::string &moduleDir)
{
    if (!GetInstalldProxy()) {
        return ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR;
    }
    return installdProxy_->RemoveModuleDir(moduleDir);
}

ErrCode InstalldClient::RenameModuleDir(const std::string &oldPath, const std::string &newPath)
{
    if (!GetInstalldProxy()) {
        return ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR;
    }
    return installdProxy_->RenameModuleDir(oldPath, newPath);
}

ErrCode InstalldClient::CreateBundleDataDir(const std::string &bundleDir, const int uid, const int gid)
{
    if (!GetInstalldProxy()) {
        return ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR;
    }
    return installdProxy_->CreateBundleDataDir(bundleDir, uid, gid);
}

ErrCode InstalldClient::CreateModuleDataDir(
    const std::string &ModuleDir, const std::vector<std::string> &abilityDirs, const int uid, const int gid)
{
    if (!GetInstalldProxy()) {
        return ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR;
    }
    return installdProxy_->CreateModuleDataDir(ModuleDir, abilityDirs, uid, gid);
}

ErrCode InstalldClient::RemoveModuleDataDir(const std::string &moduleDataDir)
{
    if (!GetInstalldProxy()) {
        return ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR;
    }
    return installdProxy_->RemoveModuleDataDir(moduleDataDir);
}

ErrCode InstalldClient::CleanBundleDataDir(const std::string &bundleDir)
{
    if (!GetInstalldProxy()) {
        return ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR;
    }
    return installdProxy_->CleanBundleDataDir(bundleDir);
}

void InstalldClient::ResetInstalldProxy()
{
    if ((installdProxy_ != nullptr) && (installdProxy_->AsObject() != nullptr)) {
        installdProxy_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    installdProxy_ = nullptr;
}

bool InstalldClient::GetInstalldProxy()
{
    if (!installdProxy_) {
        APP_LOGD("try to get installd proxy");
        std::lock_guard<std::mutex> lock(mutex_);
        if (!installdProxy_) {
            sptr<IInstalld> tempProxy =
                iface_cast<IInstalld>(SystemAbilityHelper::GetSystemAbility(INSTALLD_SERVICE_ID));
            if ((!tempProxy) || (!tempProxy->AsObject())) {
                APP_LOGE("the installd proxy or remote object is null");
                return false;
            }
            recipient_ = new (std::nothrow) InstalldDeathRecipient();
            tempProxy->AsObject()->AddDeathRecipient(recipient_);
            installdProxy_ = tempProxy;
        }
    }
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS
