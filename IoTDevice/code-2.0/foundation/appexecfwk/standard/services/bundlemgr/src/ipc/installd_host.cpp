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

#include "ipc/installd_host.h"

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "parcel_macro.h"

namespace OHOS {
namespace AppExecFwk {

InstalldHost::InstalldHost()
{
    APP_LOGI("installd host instance is created");
}

InstalldHost::~InstalldHost()
{
    APP_LOGI("installd host instance is destroyed");
}

int InstalldHost::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_LOGD(
        "installd host receives message from client, code = %{public}d, flags = %{public}d", code, option.GetFlags());
    std::u16string descripter = InstalldHost::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        APP_LOGE("installd host fail to write reply message due to the reply is nullptr");
        return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
    }
    switch (code) {
        case static_cast<uint32_t>(IInstalld::Message::CREATE_BUNDLE_DIR): {
            if (!HandleCreateBundleDir(data, reply)) {
                return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
            }
            break;
        }
        case static_cast<uint32_t>(IInstalld::Message::REMOVE_BUNDLE_DIR): {
            if (!HandleRemoveBundleDir(data, reply)) {
                return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
            }
            break;
        }
        case static_cast<uint32_t>(IInstalld::Message::EXTRACT_MODULE_FILES): {
            if (!HandleExtractModuleFiles(data, reply)) {
                return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
            }
            break;
        }
        case static_cast<uint32_t>(IInstalld::Message::REMOVE_MODULE_DIR): {
            if (!HandleRemoveModuleDir(data, reply)) {
                return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
            }
            break;
        }
        case static_cast<uint32_t>(IInstalld::Message::RENAME_MODULE_DIR): {
            if (!HandleRenameModuleDir(data, reply)) {
                return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
            }
            break;
        }
        case static_cast<uint32_t>(IInstalld::Message::CREATE_BUNDLE_DATA_DIR): {
            if (!HandleCreateBundleDataDir(data, reply)) {
                return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
            }
            break;
        }
        case static_cast<uint32_t>(IInstalld::Message::REMOVE_BUNDLE_DATA_DIR): {
            if (!HandleRemoveBundleDataDir(data, reply)) {
                return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
            }
            break;
        }
        case static_cast<uint32_t>(IInstalld::Message::CREATE_MODULE_DATA_DIR): {
            if (!HandleCreateModuleDataDir(data, reply)) {
                return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
            }
            break;
        }
        case static_cast<uint32_t>(IInstalld::Message::REMOVE_MODULE_DATA_DIR): {
            if (!HandleRemoveModuleDataDir(data, reply)) {
                return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
            }
            break;
        }
        case static_cast<uint32_t>(IInstalld::Message::CLEAN_BUNDLE_DATA_DIR): {
            if (!HandleCleanBundleDataDir(data, reply)) {
                return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
            }
            break;
        }
        default:
            APP_LOGW("installd host receives unknown code, code = %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    APP_LOGD("installd host finish to process message from client");
    return NO_ERROR;
}

bool InstalldHost::HandleCreateBundleDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleDir = data.ReadString();
    APP_LOGI("bundleName %{public}s", bundleDir.c_str());
    ErrCode result = CreateBundleDir(bundleDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRemoveBundleDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleDir = data.ReadString();
    ErrCode result = RemoveBundleDir(bundleDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleExtractModuleFiles(MessageParcel &data, MessageParcel &reply)
{
    std::string srcModulePath = data.ReadString();
    std::string targetPath = data.ReadString();
    APP_LOGI("extract module %{public}s", targetPath.c_str());
    ErrCode result = ExtractModuleFiles(srcModulePath, targetPath);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRenameModuleDir(MessageParcel &data, MessageParcel &reply)
{
    std::string oldPath = data.ReadString();
    std::string newPath = data.ReadString();
    APP_LOGI("rename moduleDir %{public}s", oldPath.c_str());
    ErrCode result = RenameModuleDir(oldPath, newPath);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRemoveModuleDir(MessageParcel &data, MessageParcel &reply)
{
    std::string moduleDir = data.ReadString();
    APP_LOGI("remove moduleDir %{public}s", moduleDir.c_str());
    ErrCode result = RemoveModuleDir(moduleDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleCreateBundleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleDir = data.ReadString();
    int uid = data.ReadInt32();
    int gid = data.ReadInt32();
    ErrCode result = CreateBundleDataDir(bundleDir, uid, gid);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRemoveBundleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleDataDir = data.ReadString();
    ErrCode result = RemoveBundleDataDir(bundleDataDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleCreateModuleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleDir = data.ReadString();
    std::vector<std::string> abilityDirs;
    if (!data.ReadStringVector(&abilityDirs)) {
        return false;
    }
    int uid = data.ReadInt32();
    int gid = data.ReadInt32();
    ErrCode result = CreateModuleDataDir(bundleDir, abilityDirs, uid, gid);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRemoveModuleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleDataDir = data.ReadString();
    ErrCode result = RemoveBundleDataDir(bundleDataDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleCleanBundleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleDir = data.ReadString();
    ErrCode result = CleanBundleDataDir(bundleDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS
