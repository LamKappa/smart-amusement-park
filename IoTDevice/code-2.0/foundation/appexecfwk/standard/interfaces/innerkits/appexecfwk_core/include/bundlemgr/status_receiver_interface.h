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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_STATUS_RECEIVER_INTERFACE_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_STATUS_RECEIVER_INTERFACE_H

#include "iremote_broker.h"

namespace OHOS {
namespace AppExecFwk {

class IStatusReceiver : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.StatusReceiver");

    /**
     * @brief Called when install status changed, with the percentage of installation progress
     * @param progress Indicates the percentage of the installation progress.
     */
    virtual void OnStatusNotify(const int progress) = 0;
    /**
     * @brief Called when an application is installed, updated, or uninstalled.
     * @param resultCode Indicates the status code returned for the application installation, update, or uninstallation
     *                   result.
     * @param resultMsg Indicates the result message returned with the status code.
     */
    virtual void OnFinished(const int32_t resultCode, const std::string &resultMsg) = 0;

    enum class Message {
        ON_STATUS_NOTIFY,
        ON_FINISHED,
    };

    enum {
        SUCCESS = 0,
        ERR_INSTALL_INTERNAL_ERROR,
        ERR_INSTALL_HOST_INSTALLER_FAILED,
        ERR_INSTALL_PARSE_FAILED,
        ERR_INSTALL_VERSION_DOWNGRADE,
        ERR_INSTALL_VERIFICATION_FAILED,
        ERR_INSTALL_NO_SIGNATURE_INFO,
        ERR_INSTALL_UPDATE_INCOMPATIBLE,
        ERR_INSTALL_PARAM_ERROR,
        ERR_INSTALL_PERMISSION_DENIED,
        ERR_INSTALL_ENTRY_ALREADY_EXIST,
        ERR_INSTALL_STATE_ERROR,
        ERR_INSTALL_FILE_PATH_INVALID,
        ERR_INSTALL_INVALID_HAP_NAME,
        ERR_INSTALL_INVALID_BUNDLE_FILE,
        ERR_INSTALL_GENERATE_UID_ERROR,
        ERR_INSTALL_INSTALLD_SERVICE_ERROR,
        ERR_INSTALL_BUNDLE_MGR_SERVICE_ERROR,
        ERR_INSTALL_ALREADY_EXIST,

        ERR_INSTALL_PARSE_UNEXPECTED,
        ERR_INSTALL_PARSE_MISSING_BUNDLE,
        ERR_INSTALL_PARSE_MISSING_ABILITY,
        ERR_INSTALL_PARSE_NO_PROFILE,
        ERR_INSTALL_PARSE_BAD_PROFILE,
        ERR_INSTALL_PARSE_PROFILE_PROP_TYPE_ERROR,
        ERR_INSTALL_PARSE_PROFILE_MISSING_PROP,
        ERR_INSTALL_PARSE_PERMISSION_ERROR,

        ERR_INSTALLD_PARAM_ERROR,
        ERR_INSTALLD_GET_PROXY_ERROR,
        ERR_INSTALLD_CREATE_DIR_FAILED,
        ERR_INSTALLD_CREATE_DIR_EXIST,
        ERR_INSTALLD_CHOWN_FAILED,
        ERR_INSTALLD_REMOVE_DIR_FAILED,
        ERR_INSTALLD_EXTRACT_FILES_FAILED,
        ERR_INSTALLD_RNAME_DIR_FAILED,
        ERR_INSTALLD_CLEAN_DIR_FAILED,

        ERR_UNINSTALL_SYSTEM_APP_ERROR,
        ERR_UNINSTALL_KILLING_APP_ERROR,
        ERR_UNINSTALL_INVALID_NAME,
        ERR_UNINSTALL_PARAM_ERROR,
        ERR_UNINSTALL_PERMISSION_DENIED,
        ERR_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR,
        ERR_UNINSTALL_MISSING_INSTALLED_BUNDLE,
        ERR_UNINSTALL_MISSING_INSTALLED_MODULE,
        ERR_UNKNOWN,
    };
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_STATUS_RECEIVER_INTERFACE_H