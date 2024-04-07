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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_H

#include <memory>
#include <string>

#include "nocopyable.h"

#include "base_bundle_installer.h"
#include "event_handler.h"
#include "status_receiver_interface.h"

namespace OHOS {
namespace AppExecFwk {

class BundleInstaller : public BaseBundleInstaller {
public:
    BundleInstaller(const int64_t installerId, const std::shared_ptr<EventHandler> &handler,
        const sptr<IStatusReceiver> &statusReceiver);
    virtual ~BundleInstaller() override;
    /**
     * @brief Get the installer ID of an installer object.
     * @return Returns the installer ID of this object.
     */
    int64_t GetInstallerId() const
    {
        return installerId_;
    }
    /**
     * @brief Install a bundle using this installer object.
     * @param bundleFilePath Indicates the path for storing the HAP of the bundle to install or update.
     * @param installParam Indicates the install parameters.
     * @return
     */
    void Install(const std::string &bundleFilePath, const InstallParam &installParam);
    /**
     * @brief Uninstall a bundle using this installer object.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @return
     */
    void Uninstall(const std::string &bundleName, const InstallParam &installParam);
    /**
     * @brief Uninstall a module using this installer object.
     * @param bundleName Indicates the bundle name of the module to uninstall.
     * @param modulePackage Indicates the module package of the module to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @return
     */
    void Uninstall(const std::string &bundleName, const std::string &modulePackage, const InstallParam &installParam);
    /**
     * @brief Update the installer state and send status from the StatusReceiver object.
     * @attention This function will send the install status to StatusReceiver.
     * @param state Indicates the state to be updated to.
     * @return
     */
    virtual void UpdateInstallerState(const InstallerState state) override;

private:
    /**
     * @brief Send an event for requesting to remove this bundle installer object.
     * @return
     */
    void SendRemoveEvent() const;

private:
    const int64_t installerId_ = 0;
    const std::weak_ptr<EventHandler> handler_;
    const sptr<IStatusReceiver> statusReceiver_;

    DISALLOW_COPY_AND_MOVE(BundleInstaller);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_H