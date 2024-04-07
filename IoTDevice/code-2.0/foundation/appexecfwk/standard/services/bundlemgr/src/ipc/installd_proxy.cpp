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

#include "ipc/installd_proxy.h"

#include "ipc_types.h"

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "parcel_macro.h"

namespace OHOS {
namespace AppExecFwk {

InstalldProxy::InstalldProxy(const sptr<IRemoteObject> &object) : IRemoteProxy<IInstalld>(object)
{
    APP_LOGI("installd proxy instance is created");
}

InstalldProxy::~InstalldProxy()
{
    APP_LOGI("installd proxy instance is destroyed");
}

ErrCode InstalldProxy::CreateBundleDir(const std::string &bundleDir)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to send cmd to service due to data.WriteInterfaceToken(GetDescriptor())");
        return false;
    }
    if (!data.WriteString(bundleDir)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!TransactInstalldCmd(IInstalld::Message::CREATE_BUNDLE_DIR, data, reply)) {
        return false;
    }
    return reply.ReadInt32();
}

ErrCode InstalldProxy::RemoveBundleDir(const std::string &bundleDir)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to send cmd to service due to data.WriteInterfaceToken(GetDescriptor())");
        return false;
    }
    if (!data.WriteString(bundleDir)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!TransactInstalldCmd(IInstalld::Message::REMOVE_BUNDLE_DIR, data, reply)) {
        return false;
    }
    return reply.ReadInt32();
}

ErrCode InstalldProxy::ExtractModuleFiles(const std::string &srcModulePath, const std::string &targetPath)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to send cmd to service due to data.WriteInterfaceToken(GetDescriptor())");
        return false;
    }
    if (!data.WriteString(srcModulePath)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!data.WriteString(targetPath)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!TransactInstalldCmd(IInstalld::Message::EXTRACT_MODULE_FILES, data, reply)) {
        return false;
    }
    return reply.ReadInt32();
}

ErrCode InstalldProxy::RemoveModuleDir(const std::string &moduleDir)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to send cmd to service due to data.WriteInterfaceToken(GetDescriptor())");
        return false;
    }
    if (!data.WriteString(moduleDir)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!TransactInstalldCmd(IInstalld::Message::REMOVE_MODULE_DIR, data, reply)) {
        return false;
    }
    return reply.ReadInt32();
}

ErrCode InstalldProxy::RenameModuleDir(const std::string &oldPath, const std::string &newPath)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to send cmd to service due to data.WriteInterfaceToken(GetDescriptor())");
        return false;
    }
    if (!data.WriteString(oldPath)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!data.WriteString(newPath)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!TransactInstalldCmd(IInstalld::Message::RENAME_MODULE_DIR, data, reply)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    return reply.ReadInt32();
}

ErrCode InstalldProxy::CreateBundleDataDir(const std::string &bundleDir, const int uid, const int gid)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to send cmd to service due to data.WriteInterfaceToken(GetDescriptor())");
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!data.WriteString(bundleDir)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!data.WriteInt32(uid)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!data.WriteInt32(gid)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!TransactInstalldCmd(IInstalld::Message::CREATE_BUNDLE_DATA_DIR, data, reply)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    return reply.ReadInt32();
}

ErrCode InstalldProxy::RemoveBundleDataDir(const std::string &bundleDataPath)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to send cmd to service due to data.WriteInterfaceToken(GetDescriptor())");
        return false;
    }
    if (!data.WriteString(bundleDataPath)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!TransactInstalldCmd(IInstalld::Message::REMOVE_BUNDLE_DATA_DIR, data, reply)) {
        return false;
    }
    return reply.ReadInt32();
}

ErrCode InstalldProxy::CreateModuleDataDir(
    const std::string &ModuleDir, const std::vector<std::string> &abilityDirs, const int uid, const int gid)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to send cmd to service due to data.WriteInterfaceToken(GetDescriptor())");
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!data.WriteString(ModuleDir)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!data.WriteStringVector(abilityDirs)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!data.WriteInt32(uid)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!data.WriteInt32(gid)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!TransactInstalldCmd(IInstalld::Message::CREATE_MODULE_DATA_DIR, data, reply)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    return reply.ReadInt32();
}

ErrCode InstalldProxy::RemoveModuleDataDir(const std::string &moduleDataDir)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to send cmd to service due to data.WriteInterfaceToken(GetDescriptor())");
        return false;
    }
    if (!data.WriteString(moduleDataDir)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!TransactInstalldCmd(IInstalld::Message::REMOVE_MODULE_DATA_DIR, data, reply)) {
        return false;
    }
    return reply.ReadInt32();
}

ErrCode InstalldProxy::CleanBundleDataDir(const std::string &bundleDir)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to send cmd to service due to data.WriteInterfaceToken(GetDescriptor())");
        return false;
    }
    if (!data.WriteString(bundleDir)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    if (!TransactInstalldCmd(IInstalld::Message::CLEAN_BUNDLE_DATA_DIR, data, reply)) {
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    return reply.ReadInt32();
}

bool InstalldProxy::TransactInstalldCmd(IInstalld::Message code, MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = Remote();
    MessageOption option(MessageOption::TF_SYNC);
    if (remote == nullptr) {
        APP_LOGE("fail to send %{public}d cmd to service due to remote object is null", code);
        return false;
    }
    int32_t result = remote->SendRequest(static_cast<uint32_t>(code), data, reply, option);
    if (result != OHOS::NO_ERROR) {
        APP_LOGE("fail to send %{public}d cmd to service due to transact error", code);
        return false;
    }
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS