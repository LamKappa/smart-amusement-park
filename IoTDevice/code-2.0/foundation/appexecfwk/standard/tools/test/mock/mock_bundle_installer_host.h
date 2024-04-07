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

#ifndef FOUNDATION_APPEXECFWK_TOOLS_TEST_MOCK_MOCK_BUNDLE_INSTALLER_HOST_H
#define FOUNDATION_APPEXECFWK_TOOLS_TEST_MOCK_MOCK_BUNDLE_INSTALLER_HOST_H

#include "gmock/gmock.h"

#include "app_log_wrapper.h"
#include "bundle_installer_host.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string MSG_SUCCESS = "[SUCCESS]";
const std::string STRING_BUNDLE_PATH = "path";
const std::string STRING_BUNDLE_NAME = "name";
const std::string STRING_MODULE_NAME = "module";
}  // namespace

class MockBundleInstallerHost : public BundleInstallerHost {
public:
    bool Install(const std::string &bundleFilePath, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver);

    bool Uninstall(
        const std::string &bundleName, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver);

    bool Uninstall(const std::string &bundleName, const std::string &modulePackage, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver);
};

}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_TOOLS_TEST_MOCK_MOCK_BUNDLE_INSTALLER_HOST_H
