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

#include "bundle_mgr_service_event_handler.h"

#include <future>

#include "app_log_wrapper.h"
#include "bundle_mgr_service.h"
#include "bundle_scanner.h"
#include "perf_profile.h"
#include "system_bundle_installer.h"

namespace OHOS {
namespace AppExecFwk {

BMSEventHandler::BMSEventHandler(const std::shared_ptr<EventRunner> &runner) : EventHandler(runner)
{
    APP_LOGI("instance is created");
}

BMSEventHandler::~BMSEventHandler()
{
    APP_LOGI("instance is destroyed");
}

void BMSEventHandler::ProcessEvent(const InnerEvent::Pointer &event)
{
    switch (event->GetInnerEventId()) {
        case BUNDLE_SCAN_START: {
            auto future = std::async(std::launch::async, [this] {
                ProcessSystemBundleInstall(Constants::AppType::SYSTEM_APP);
                ProcessSystemBundleInstall(Constants::AppType::THIRD_SYSTEM_APP);
            });
            future.get();
            SetAllInstallFlag();
            break;
        }
        case BUNDLE_SCAN_FINISHED:
            break;
        case BMS_START_FINISHED:
            break;
        default:
            APP_LOGE("the eventId is not supported");
            break;
    }
}

void BMSEventHandler::ProcessSystemBundleInstall(Constants::AppType appType) const
{
    APP_LOGI("scan thread start");
    auto scanner = std::make_unique<BundleScanner>();
    if (!scanner) {
        APP_LOGE("make scanner failed");
        return;
    }
    std::string scanDir = (appType == Constants::AppType::SYSTEM_APP) ? Constants::SYSTEM_APP_SCAN_PATH
                                                                      : Constants::THIRD_SYSTEM_APP_SCAN_PATH;
    APP_LOGI("scanDir %{public}s", scanDir.c_str());
    std::list<std::string> bundleList = scanner->Scan(scanDir);
    for (const auto &item : bundleList) {
        SystemBundleInstaller installer(item);
        APP_LOGI("scan item %{public}s", item.c_str());
        if (!installer.InstallSystemBundle(appType)) {
            APP_LOGW("Install System app:%{public}s error", item.c_str());
        }
    }
    PerfProfile::GetInstance().Dump();
}

void BMSEventHandler::SetAllInstallFlag() const
{
    auto dataMgr = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return;
    }
    dataMgr->SetAllInstallFlag(true);
}

}  // namespace AppExecFwk
}  // namespace OHOS
