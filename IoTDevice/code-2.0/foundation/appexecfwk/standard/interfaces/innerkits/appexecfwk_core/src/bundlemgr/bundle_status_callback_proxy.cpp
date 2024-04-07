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

#include "bundle_status_callback_proxy.h"

#include "ipc_types.h"
#include "parcel.h"
#include "string_ex.h"

#include "appexecfwk_errors.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
namespace {

std::string TransformResult(ErrCode resultCode)
{
    switch (resultCode) {
        case ERR_OK:
            return "SUCCESS";
        case ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR:
            return "ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR";
        case ERR_APPEXECFWK_INSTALL_HOST_INSTALLER_FAILED:
            return "ERR_APPEXECFWK_INSTALL_HOST_INSTALLER_FAILED";
        case ERR_APPEXECFWK_INSTALL_PARSE_FAILED:
            return "ERR_APPEXECFWK_INSTALL_PARSE_FAILED";
        case ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE:
            return "ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE";
        case ERR_APPEXECFWK_INSTALL_VERIFICATION_FAILED:
            return "ERR_APPEXECFWK_INSTALL_VERIFICATION_FAILED";
        case ERR_APPEXECFWK_INSTALL_NO_SIGNATURE_INFO:
            return "ERR_APPEXECFWK_INSTALL_NO_SIGNATURE_INFO";
        case ERR_APPEXECFWK_INSTALL_UPDATE_INCOMPATIBLE:
            return "ERR_APPEXECFWK_INSTALL_UPDATE_INCOMPATIBLE";
        case ERR_APPEXECFWK_INSTALL_PARAM_ERROR:
            return "ERR_APPEXECFWK_INSTALL_PARAM_ERROR";
        case ERR_APPEXECFWK_INSTALL_PERMISSION_DENIED:
            return "ERR_APPEXECFWK_INSTALL_PERMISSION_DENIED";
        case ERR_APPEXECFWK_INSTALL_ENTRY_ALREADY_EXIST:
            return "ERR_APPEXECFWK_INSTALL_ENTRY_ALREADY_EXIST";
        case ERR_APPEXECFWK_INSTALL_STATE_ERROR:
            return "ERR_APPEXECFWK_INSTALL_STATE_ERROR";
        case ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID:
            return "ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID";
        case ERR_APPEXECFWK_INSTALL_INVALID_HAP_NAME:
            return "ERR_APPEXECFWK_INSTALL_INVALID_HAP_NAME";
        case ERR_APPEXECFWK_INSTALL_INVALID_BUNDLE_FILE:
            return "ERR_APPEXECFWK_INSTALL_INVALID_BUNDLE_FILE";
        case ERR_APPEXECFWK_INSTALL_GENERATE_UID_ERROR:
            return "ERR_APPEXECFWK_INSTALL_GENERATE_UID_ERROR";
        case ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR:
            return "ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR";
        case ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR:
            return "ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR";
        case ERR_APPEXECFWK_INSTALL_ALREADY_EXIST:
            return "ERR_APPEXECFWK_INSTALL_ALREADY_EXIST";
        case ERR_APPEXECFWK_PARSE_UNEXPECTED:
            return "ERR_APPEXECFWK_PARSE_UNEXPECTED";
        case ERR_APPEXECFWK_PARSE_MISSING_BUNDLE:
            return "ERR_APPEXECFWK_PARSE_MISSING_BUNDLE";
        case ERR_APPEXECFWK_PARSE_MISSING_ABILITY:
            return "ERR_APPEXECFWK_PARSE_MISSING_ABILITY";
        case ERR_APPEXECFWK_PARSE_NO_PROFILE:
            return "ERR_APPEXECFWK_PARSE_NO_PROFILE";
        case ERR_APPEXECFWK_PARSE_BAD_PROFILE:
            return "ERR_APPEXECFWK_PARSE_BAD_PROFILE";
        case ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR:
            return "ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR";
        case ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP:
            return "ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP";
        case ERR_APPEXECFWK_PARSE_PERMISSION_ERROR:
            return "ERR_APPEXECFWK_PARSE_PERMISSION_ERROR";
        case ERR_APPEXECFWK_INSTALLD_PARAM_ERROR:
            return "ERR_APPEXECFWK_INSTALLD_PARAM_ERROR";
        case ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR:
            return "ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR";
        case ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED:
            return "ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED";
        case ERR_APPEXECFWK_INSTALLD_CREATE_DIR_EXIST:
            return "ERR_APPEXECFWK_INSTALLD_CREATE_DIR_EXIST";
        case ERR_APPEXECFWK_INSTALLD_CHOWN_FAILED:
            return "ERR_APPEXECFWK_INSTALLD_CHOWN_FAILED";
        case ERR_APPEXECFWK_INSTALLD_REMOVE_DIR_FAILED:
            return "ERR_APPEXECFWK_INSTALLD_REMOVE_DIR_FAILED";
        case ERR_APPEXECFWK_INSTALLD_EXTRACT_FILES_FAILED:
            return "ERR_APPEXECFWK_INSTALLD_EXTRACT_FILES_FAILED";
        case ERR_APPEXECFWK_INSTALLD_RNAME_DIR_FAILED:
            return "ERR_APPEXECFWK_INSTALLD_RNAME_DIR_FAILED";
        case ERR_APPEXECFWK_INSTALLD_CLEAN_DIR_FAILED:
            return "ERR_APPEXECFWK_INSTALLD_CLEAN_DIR_FAILED";
        case ERR_APPEXECFWK_UNINSTALL_SYSTEM_APP_ERROR:
            return "ERR_APPEXECFWK_UNINSTALL_SYSTEM_APP_ERROR";
        case ERR_APPEXECFWK_UNINSTALL_KILLING_APP_ERROR:
            return "ERR_APPEXECFWK_UNINSTALL_KILLING_APP_ERROR";
        case ERR_APPEXECFWK_UNINSTALL_INVALID_NAME:
            return "ERR_APPEXECFWK_UNINSTALL_INVALID_NAME";
        case ERR_APPEXECFWK_UNINSTALL_PARAM_ERROR:
            return "ERR_APPEXECFWK_UNINSTALL_PARAM_ERROR";
        case ERR_APPEXECFWK_UNINSTALL_PERMISSION_DENIED:
            return "ERR_APPEXECFWK_UNINSTALL_PERMISSION_DENIED";
        case ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR:
            return "ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR";
        case ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE:
            return "ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE";
        case ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_MODULE:
            return "ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_MODULE";
        default:
            return "";
    }
}

}  // namespace

BundleStatusCallbackProxy::BundleStatusCallbackProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IBundleStatusCallback>(object)
{
    APP_LOGI("create bundle status callback proxy instance");
}

BundleStatusCallbackProxy::~BundleStatusCallbackProxy()
{
    APP_LOGI("destroy bundle status callback proxy instance");
}

void BundleStatusCallbackProxy::OnBundleStateChanged(
    const uint8_t installType, const int32_t resultCode, const std::string &resultMsg, const std::string &bundleName)
{
    APP_LOGI("bundle state changed %{public}s", bundleName.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteUint8(installType)) {
        APP_LOGE("fail to call OnBundleStateChanged, for write installType failed");
        return;
    }
    if (!data.WriteInt32(resultCode)) {
        APP_LOGE("fail to call OnBundleStateChanged, for write resultCode failed");
        return;
    }
    std::string msg = TransformResult(resultCode);
    if (!data.WriteString16(Str8ToStr16(msg))) {
        APP_LOGE("fail to call OnBundleStateChanged, for write resultMsg failed");
        return;
    }
    if (!data.WriteString16(Str8ToStr16(bundleName))) {
        APP_LOGE("fail to call OnBundleStateChanged, for write bundleName failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        APP_LOGE("fail to call OnBundleStateChanged, for Remote() is nullptr");
        return;
    }

    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(IBundleStatusCallback::Message::ON_BUNDLE_STATE_CHANGED), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGW("fail to call OnBundleStateChanged, for transact is failed, error code is: %{public}d", ret);
    }
}

}  // namespace AppExecFwk
}  // namespace OHOS
