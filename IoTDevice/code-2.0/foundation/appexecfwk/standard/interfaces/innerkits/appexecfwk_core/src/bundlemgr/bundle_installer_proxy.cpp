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

#include "bundle_installer_proxy.h"

#include "ipc_types.h"
#include "parcel.h"
#include "string_ex.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

BundleInstallerProxy::BundleInstallerProxy(const sptr<IRemoteObject> &object) : IRemoteProxy<IBundleInstaller>(object)
{
    APP_LOGI("create bundle installer proxy instance");
}

BundleInstallerProxy::~BundleInstallerProxy()
{
    APP_LOGI("destroy bundle installer proxy instance");
}

bool BundleInstallerProxy::Install(
    const std::string &bundlePath, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to dump due to write MessageParcel fail");
        return false;
    }
    if (!data.WriteString16(Str8ToStr16(bundlePath))) {
        APP_LOGE("fail to install, for write parcel bundle path failed");
        return false;
    }
    if (!data.WriteParcelable(&installParam)) {
        APP_LOGE("fail to install, for write parcel install param failed");
        return false;
    }
    if (!statusReceiver) {
        APP_LOGE("fail to install, for statusReceiver is nullptr");
        return false;
    }
    if (!data.WriteParcelable(statusReceiver->AsObject())) {
        APP_LOGE("fail to install, for write parcel status receiver failed");
        return false;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        APP_LOGE("fail to install, for Remote() is nullptr");
        return false;
    }

    int32_t ret = remote->SendRequest(static_cast<int32_t>(IBundleInstaller::Message::INSTALL), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGE("fail to install, for transact is failed and error code is: %{public}d", ret);
        return false;
    }
    return true;
}

bool BundleInstallerProxy::Uninstall(
    const std::string &bundleName, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to dump due to write MessageParcel fail");
        return false;
    }
    if (!data.WriteString16(Str8ToStr16(bundleName))) {
        APP_LOGE("fail to uninstall, for write parcel bundle name failed");
        return false;
    }
    if (!data.WriteParcelable(&installParam)) {
        APP_LOGE("fail to install, for write parcel install param failed");
        return false;
    }
    if (!statusReceiver) {
        APP_LOGE("fail to uninstall, for statusReceiver is nullptr");
        return false;
    }
    if (!data.WriteParcelable(statusReceiver->AsObject())) {
        APP_LOGE("fail to uninstall, for write parcel status receiver failed");
        return false;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        APP_LOGE("fail to uninstall, for Remote() is nullptr");
        return false;
    }

    int32_t ret = remote->SendRequest(static_cast<int32_t>(IBundleInstaller::Message::UNINSTALL), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGE("fail to uninstall, for transact is failed and error code is: %{public}d", ret);
        return false;
    }
    return true;
}

bool BundleInstallerProxy::Uninstall(const std::string &bundleName, const std::string &modulePackage,
    const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to dump due to write MessageParcel fail");
        return false;
    }
    if (!data.WriteString16(Str8ToStr16(bundleName))) {
        APP_LOGE("fail to uninstall, for write parcel bundle name failed");
        return false;
    }
    if (!data.WriteString16(Str8ToStr16(modulePackage))) {
        APP_LOGE("fail to uninstall, for write parcel module package failed");
        return false;
    }
    if (!data.WriteParcelable(&installParam)) {
        APP_LOGE("fail to install, for write parcel install param failed");
        return false;
    }
    if (!statusReceiver) {
        APP_LOGE("fail to uninstall, for statusReceiver is nullptr");
        return false;
    }
    if (!data.WriteParcelable(statusReceiver->AsObject())) {
        APP_LOGE("fail to uninstall, for write parcel status receiver failed");
        return false;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        APP_LOGE("fail to uninstall, for Remote() is nullptr");
        return false;
    }

    int32_t ret =
        remote->SendRequest(static_cast<int32_t>(IBundleInstaller::Message::UNINSTALL_MODULE), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGE("fail to uninstall, for transact is failed and error code is: %{public}d", ret);
        return false;
    }
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS