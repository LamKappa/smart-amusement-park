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

#include "tool_system_test.h"

#include <gtest/gtest.h>
#include <thread>
#include "ability_command.h"
#include "bundle_command.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

std::string ToolSystemTest::ExecuteCommand(const std::string &command)
{
    std::string result = "";
    FILE *file = popen(command.c_str(), "r");

    // wait for services
    std::this_thread::sleep_for(std::chrono::seconds(TIME_DELAY_FOR_SERVICES));

    if (file != nullptr) {
        char commandResult[1024] = {0};

        fgets(commandResult, sizeof(commandResult), file);

        pclose(file);
        file = nullptr;

        result.append(commandResult);
    }

    return result;
}

void ToolSystemTest::InstallBundle(const std::string &bundlePath, const bool checkResult)
{
    // install a bundle
    std::string command = "bm install -p " + bundlePath;
    std::string commandResult = ExecuteCommand(command);

    if (checkResult) {
        EXPECT_EQ(commandResult, STRING_INSTALL_BUNDLE_OK + "\n");
    }
}

void ToolSystemTest::UninstallBundle(const std::string &bundleName, const bool checkResult)
{
    // uninstall a bundle
    std::string command = "bm uninstall -n " + bundleName;
    std::string commandResult = ExecuteCommand(command);

    if (checkResult) {
        EXPECT_EQ(commandResult, STRING_UNINSTALL_BUNDLE_OK + "\n");
    }
}

void ToolSystemTest::StartAbility(
    const std::string &device, const std::string &abilityName, const std::string &bundleName, const bool checkResult)
{
    // start an ability
    std::string command = "aa start -d " + device + " -a " + abilityName + " -b " + bundleName;
    std::string commandResult = ExecuteCommand(command);

    if (checkResult) {
        EXPECT_EQ(commandResult, STRING_START_ABILITY_OK + "\n");
    }
}
