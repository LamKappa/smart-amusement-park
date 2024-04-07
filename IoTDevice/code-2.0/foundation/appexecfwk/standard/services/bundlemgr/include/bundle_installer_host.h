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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_HOST_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_HOST_H

#include <memory>
#include <string>

#include "iremote_stub.h"
#include "nocopyable.h"

#include "bundle_installer_interface.h"
#include "bundle_installer_manager.h"

namespace OHOS {
namespace AppExecFwk {

class BundleInstallerHost : public IRemoteStub<IBundleInstaller> {
public:
    BundleInstallerHost();
    virtual ~BundleInstallerHost() override;

    bool Init();
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    /**
     * @brief Installs an application, the final result will be notified from the statusReceiver object.
     * @attention Notice that the bundleFilePath should be an absolute path.
     * @param bundleFilePath Indicates the path for storing the ohos Ability Package (HAP) of the application
     *                       to install or update.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool Install(const std::string &bundleFilePath, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver) override;
    /**
     * @brief Uninstalls an application, the result will be notified from the statusReceiver object.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the uninstall result.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool Uninstall(const std::string &bundleName, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver) override;
    /**
     * @brief Uninstalls a module in an application, the result will be notified from the statusReceiver object.
     * @param bundleName Indicates the bundle name of the module to uninstall.
     * @param modulePackage Indicates the module package of the module to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the uninstall result.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool Uninstall(const std::string &bundleName, const std::string &modulePackage,
        const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver) override;

private:
    /**
     * @brief Handles the Install function called from a IBundleInstaller proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return
     */
    void HandleInstallMessage(Parcel &data);
    /**
     * @brief Handles the Uninstall bundle function called from a IBundleInstaller proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return
     */
    void HandleUninstallMessage(Parcel &data);
    /**
     * @brief Handles the Uninstall module function called from a IBundleInstaller proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return
     */
    void HandleUninstallModuleMessage(Parcel &data);
    /**
     * @brief Check whether the statusReceiver object is valid.
     * @param statusReceiver Indicates the IStatusReceiver object.
     * @return Returns true if the object is valid; returns false otherwise.
     */
    bool CheckBundleInstallerManager(const sptr<IStatusReceiver> &statusReceiver) const;

private:
    std::shared_ptr<BundleInstallerManager> manager_;

    DISALLOW_COPY_AND_MOVE(BundleInstallerHost);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_HOST_H