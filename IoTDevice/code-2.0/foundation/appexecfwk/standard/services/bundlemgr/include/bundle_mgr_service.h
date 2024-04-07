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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_H

#include <memory>

#include "singleton.h"
#include "system_ability.h"

#include "bundle_data_mgr.h"
#include "bundle_installer_host.h"
#include "bundle_mgr_host_impl.h"
#include "bundle_mgr_service_event_handler.h"

namespace OHOS {
namespace AppExecFwk {

class BundleMgrService : public SystemAbility {
    DECLARE_DELAYED_SINGLETON(BundleMgrService);
    DECLEAR_SYSTEM_ABILITY(BundleMgrService);

public:
    /**
     * @brief Start the bundle manager service.
     * @return
     */
    virtual void OnStart() override;
    /**
     * @brief Stop the bundle manager service.
     * @return
     */
    virtual void OnStop() override;
    /**
     * @brief Check whether if the bundle manager service is ready.
     * @return Returns true if the bundle manager service is ready; returns false otherwise.
     */
    bool IsServiceReady() const;
    /**
     * @brief Get a shared pointer to the BundleDataMgr object.
     * @return Returns the pointer of BundleDataMgr object.
     */
    const std::shared_ptr<BundleDataMgr> GetDataMgr() const;
    /**
     * @brief Get a IBundleInstaller object for IPC
     * @return Returns the pointer of IBundleInstaller object.
     */
    sptr<IBundleInstaller> GetBundleInstaller() const;

private:
    /**
     * @brief Initialize the bundle manager service context.
     * @return Returns true if initialized successfully; returns false otherwise.
     */
    bool Init();
    /**
     * @brief Clean the context of this bundle manager service.
     * @return
     */
    void SelfClean();

private:
    bool ready_ = false;
    bool registerToService_ = false;
    bool needToScan_ = false;
    std::shared_ptr<EventRunner> runner_;
    std::shared_ptr<BMSEventHandler> handler_;
    std::shared_ptr<BundleDataMgr> dataMgr_;
    sptr<BundleMgrHostImpl> host_;
    sptr<BundleInstallerHost> installer_;

    DISALLOW_COPY_AND_MOVE(BundleMgrService);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_H
