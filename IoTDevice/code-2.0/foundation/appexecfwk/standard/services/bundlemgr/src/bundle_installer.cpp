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

#include "bundle_installer.h"

#include <cinttypes>

#include "app_log_wrapper.h"
#include "bundle_installer_manager.h"

namespace OHOS {
namespace AppExecFwk {

BundleInstaller::BundleInstaller(const int64_t installerId, const std::shared_ptr<EventHandler> &handler,
    const sptr<IStatusReceiver> &statusReceiver)
    : installerId_(installerId), handler_(handler), statusReceiver_(statusReceiver)
{
    APP_LOGI("create bundle installer instance, the installer id is %{public}" PRId64 "", installerId_);
}

BundleInstaller::~BundleInstaller()
{
    APP_LOGI("destroy bundle installer instance, the installer id is %{public}" PRId64 "", installerId_);
}

void BundleInstaller::Install(const std::string &bundleFilePath, const InstallParam &installParam)
{
    auto resultCode = InstallBundle(bundleFilePath, installParam, Constants::AppType::THIRD_PARTY_APP);
    statusReceiver_->OnFinished(resultCode, "");
    SendRemoveEvent();
}

void BundleInstaller::Uninstall(const std::string &bundleName, const InstallParam &installParam)
{
    auto resultCode = UninstallBundle(bundleName, installParam);
    statusReceiver_->OnFinished(resultCode, "");
    SendRemoveEvent();
}

void BundleInstaller::Uninstall(
    const std::string &bundleName, const std::string &modulePackage, const InstallParam &installParam)
{
    auto resultCode = UninstallBundle(bundleName, modulePackage, installParam);
    statusReceiver_->OnFinished(resultCode, "");
    SendRemoveEvent();
}

void BundleInstaller::UpdateInstallerState(const InstallerState state)
{
    APP_LOGI("UpdateInstallerState in bundleInstaller state %{public}d", state);
    SetInstallerState(state);
    if (statusReceiver_) {
        statusReceiver_->OnStatusNotify(static_cast<int>(state));
    }
}

void BundleInstaller::SendRemoveEvent() const
{
    if (auto handler = handler_.lock()) {
        uint32_t eventId = static_cast<uint32_t>(BundleInstallerManager::REMOVE_BUNDLE_INSTALLER);
        handler->SendEvent(InnerEvent::Get(eventId, installerId_));
    } else {
        APP_LOGE("fail to remove %{public}" PRId64 " installer due to handler is expired", installerId_);
    }
}

}  // namespace AppExecFwk
}  // namespace OHOS