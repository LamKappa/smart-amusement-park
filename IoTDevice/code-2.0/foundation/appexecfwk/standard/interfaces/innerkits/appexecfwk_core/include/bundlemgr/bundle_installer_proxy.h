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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_INSTALLER_PROXY_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_INSTALLER_PROXY_H

#include "iremote_proxy.h"

#include "bundle_installer_interface.h"
#include "status_receiver_interface.h"

namespace OHOS {
namespace AppExecFwk {

class BundleInstallerProxy : public IRemoteProxy<IBundleInstaller> {
public:
    explicit BundleInstallerProxy(const sptr<IRemoteObject> &object);
    virtual ~BundleInstallerProxy() override;
    /**
     * @brief Installs an application through the proxy object.
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
     * @brief Uninstalls an application through the proxy object.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the uninstall result.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool Uninstall(const std::string &bundleName, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver) override;
    /**
     * @brief Uninstalls a module in an application through the proxy object.
     * @param bundleName Indicates the bundle name of the module to uninstall.
     * @param modulePackage Indicates the module package of the module to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the uninstall result.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool Uninstall(const std::string &bundleName, const std::string &modulePackage,
        const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver) override;

private:
    static inline BrokerDelegator<BundleInstallerProxy> delegator_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_INSTALLER_PROXY_H