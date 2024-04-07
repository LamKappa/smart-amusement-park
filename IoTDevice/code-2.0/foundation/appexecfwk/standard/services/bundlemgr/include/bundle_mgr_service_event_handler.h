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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_EVENT_HANDLER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_EVENT_HANDLER_H

#include "event_handler.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {

class BundleMgrService;

class BMSEventHandler : public EventHandler {
public:
    explicit BMSEventHandler(const std::shared_ptr<EventRunner> &runner);
    virtual ~BMSEventHandler() override;
    /**
     * @brief Process the event of install system bundles.
     * @param event Indicates the event to be processed.
     * @return
     */
    virtual void ProcessEvent(const InnerEvent::Pointer &event) override;

    enum {
        BUNDLE_SCAN_START = 1,
        BUNDLE_SCAN_FINISHED,
        BMS_START_FINISHED,
    };

private:
    /**
     * @brief Install system and system vendor bundles.
     * @param appType Indicates the bundle type.
     * @return
     */
    void ProcessSystemBundleInstall(Constants::AppType appType) const;

    /**
     * @brief Set the flag indicates that all system and vendor applications installed.
     * @return
     */
    void SetAllInstallFlag() const;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_EVENT_HANDLER_H
