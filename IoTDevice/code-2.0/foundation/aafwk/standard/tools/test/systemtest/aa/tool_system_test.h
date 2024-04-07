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

#ifndef FOUNDATION_AAFWK_SERVICES_TOOLS_TEST_SYSTEMTEST_AA_TOOL_SYSTEM_TEST_H
#define FOUNDATION_AAFWK_SERVICES_TOOLS_TEST_SYSTEMTEST_AA_TOOL_SYSTEM_TEST_H

#include <iostream>

namespace OHOS {
namespace AAFwk {
namespace {
const int TIME_DELAY_FOR_SERVICES = 1;
}

class ToolSystemTest {
public:
    static std::string ExecuteCommand(const std::string &command);
    static void InstallBundle(const std::string &bundlePath, const bool checkResult = false);
    static void UninstallBundle(const std::string &bundleName, const bool checkResult = false);
    static void StartAbility(const std::string &device, const std::string &abilityName, const std::string &bundleName,
        const bool checkResult = false);
};
}  // namespace AAFwk
}  // namespace OHOS

#endif  // FOUNDATION_AAFWK_SERVICES_TOOLS_TEST_SYSTEMTEST_AA_TOOL_SYSTEM_TEST_H
