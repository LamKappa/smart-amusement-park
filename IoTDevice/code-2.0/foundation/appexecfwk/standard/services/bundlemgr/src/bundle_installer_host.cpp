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

#include "bundle_installer_host.h"

#include "ipc_types.h"
#include "string_ex.h"

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "bundle_permission_mgr.h"

namespace OHOS {
namespace AppExecFwk {
namespace {

const std::string INSTALL_THREAD = "install_thread";
const std::string GET_MANAGER_FAIL = "fail to get bundle installer manager";

}  // namespace

BundleInstallerHost::BundleInstallerHost()
{
    APP_LOGI("create bundle installer host instance");
}

BundleInstallerHost::~BundleInstallerHost()
{
    APP_LOGI("destroy bundle installer host instance");
}

bool BundleInstallerHost::Init()
{
    APP_LOGD("begin to init");
    auto installRunner = EventRunner::Create(INSTALL_THREAD);
    if (!installRunner) {
        APP_LOGE("create install runner fail");
        return false;
    }
    manager_ = std::make_shared<BundleInstallerManager>(installRunner);
    APP_LOGD("init successfully");
    return true;
}

int BundleInstallerHost::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_LOGI("bundle installer host onReceived message, the message code is %{public}u", code);
    std::u16string descripter = GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        APP_LOGE("fail to write reply message in bundle mgr host due to the reply is nullptr");
        return OBJECT_NULL;
    }

    switch (code) {
        case static_cast<uint32_t>(IBundleInstaller::Message::INSTALL):
            HandleInstallMessage(data);
            break;
        case static_cast<uint32_t>(IBundleInstaller::Message::UNINSTALL):
            HandleUninstallMessage(data);
            break;
        case static_cast<uint32_t>(IBundleInstaller::Message::UNINSTALL_MODULE):
            HandleUninstallModuleMessage(data);
            break;
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return NO_ERROR;
}

void BundleInstallerHost::HandleInstallMessage(Parcel &data)
{
    APP_LOGD("handle install message");
    std::string bundlePath = Str16ToStr8(data.ReadString16());
    std::unique_ptr<InstallParam> installParam(data.ReadParcelable<InstallParam>());
    if (!installParam) {
        APP_LOGE("ReadParcelable<InstallParam> failed");
        return;
    }
    sptr<IRemoteObject> object = data.ReadParcelable<IRemoteObject>();
    sptr<IStatusReceiver> statusReceiver = iface_cast<IStatusReceiver>(object);

    Install(bundlePath, *installParam, statusReceiver);
    APP_LOGD("handle install message finished");
}

void BundleInstallerHost::HandleUninstallMessage(Parcel &data)
{
    APP_LOGD("handle uninstall message");
    std::string bundleName = Str16ToStr8(data.ReadString16());
    std::unique_ptr<InstallParam> installParam(data.ReadParcelable<InstallParam>());
    if (!installParam) {
        APP_LOGE("ReadParcelable<InstallParam> failed");
        return;
    }
    sptr<IRemoteObject> object = data.ReadParcelable<IRemoteObject>();
    sptr<IStatusReceiver> statusReceiver = iface_cast<IStatusReceiver>(object);

    Uninstall(bundleName, *installParam, statusReceiver);
    APP_LOGD("handle uninstall message finished");
}

void BundleInstallerHost::HandleUninstallModuleMessage(Parcel &data)
{
    APP_LOGD("handle uninstall module message");
    std::string bundleName = Str16ToStr8(data.ReadString16());
    std::string modulePackage = Str16ToStr8(data.ReadString16());
    std::unique_ptr<InstallParam> installParam(data.ReadParcelable<InstallParam>());
    if (!installParam) {
        APP_LOGE("ReadParcelable<InstallParam> failed");
        return;
    }
    sptr<IRemoteObject> object = data.ReadParcelable<IRemoteObject>();
    sptr<IStatusReceiver> statusReceiver = iface_cast<IStatusReceiver>(object);

    Uninstall(bundleName, modulePackage, *installParam, statusReceiver);
    APP_LOGD("handle uninstall message finished");
}

bool BundleInstallerHost::Install(
    const std::string &bundleFilePath, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    if (installParam.userId == Constants::INVALID_USERID) {
        APP_LOGE("userId invalid");
        return false;
    }
    if (!CheckBundleInstallerManager(statusReceiver)) {
        APP_LOGE("statusReceiver invalid");
        return false;
    }
    if (!BundlePermissionMgr::CheckCallingPermission(Constants::PERMISSION_INSTALL_BUNDLE)) {
        APP_LOGE("install permission denied");
        statusReceiver->OnFinished(ERR_APPEXECFWK_INSTALL_PERMISSION_DENIED, "");
        return false;
    }
    manager_->CreateInstallTask(bundleFilePath, installParam, statusReceiver);
    return true;
}

bool BundleInstallerHost::Uninstall(
    const std::string &bundleName, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    if (installParam.userId == Constants::INVALID_USERID) {
        APP_LOGE("userId invalid");
        return false;
    }
    if (!CheckBundleInstallerManager(statusReceiver)) {
        APP_LOGE("statusReceiver invalid");
        return false;
    }
    if (!BundlePermissionMgr::CheckCallingPermission(Constants::PERMISSION_INSTALL_BUNDLE)) {
        APP_LOGE("uninstall permission denied");
        statusReceiver->OnFinished(ERR_APPEXECFWK_UNINSTALL_PERMISSION_DENIED, "");
        return false;
    }
    manager_->CreateUninstallTask(bundleName, installParam, statusReceiver);
    return true;
}

bool BundleInstallerHost::Uninstall(const std::string &bundleName, const std::string &modulePackage,
    const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    if (installParam.userId == Constants::INVALID_USERID) {
        APP_LOGE("userId invalid");
        return false;
    }
    if (!CheckBundleInstallerManager(statusReceiver)) {
        APP_LOGE("statusReceiver invalid");
        return false;
    }
    if (!BundlePermissionMgr::CheckCallingPermission(Constants::PERMISSION_INSTALL_BUNDLE)) {
        APP_LOGE("uninstall permission denied");
        statusReceiver->OnFinished(ERR_APPEXECFWK_UNINSTALL_PERMISSION_DENIED, "");
        return false;
    }
    manager_->CreateUninstallTask(bundleName, modulePackage, installParam, statusReceiver);
    return true;
}

bool BundleInstallerHost::CheckBundleInstallerManager(const sptr<IStatusReceiver> &statusReceiver) const
{
    if (!statusReceiver) {
        APP_LOGE("the receiver is nullptr");
        return false;
    }
    if (!manager_) {
        APP_LOGE("the bundle installer manager is nullptr");
        statusReceiver->OnFinished(ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR, GET_MANAGER_FAIL);
        return false;
    }
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS