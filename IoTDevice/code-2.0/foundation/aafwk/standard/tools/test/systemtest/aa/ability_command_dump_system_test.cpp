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

#include <gtest/gtest.h>

#include "ability_command.h"
#include "tool_system_test.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;

namespace {
const std::string STRING_PAGE_ABILITY_BUNDLE_PATH = "/data/test/resource/aa/pageAbilityBundleForDump.hap";
const std::string STRING_PAGE_ABILITY_BUNDLE_NAME = "com.ohos.tools.pageAbilityBundleForDump";

const std::string STRING_DATA_ABILITY_BUNDLE_PATH = "/data/test/resource/aa/dataAbilityBundleForDump.hap";
const std::string STRING_DATA_ABILITY_BUNDLE_NAME = "com.ohos.tools.dataAbilityBundleForDump";

const std::string STRING_SERVICE_ABILITY_BUNDLE_PATH = "/data/test/resource/aa/serviceAbilityBundleForStart.hap";
const std::string STRING_SERVICE_ABILITY_BUNDLE_NAME = "com.ohos.tools.serviceAbilityBundleForStart";
}  // namespace

class AaCommandDumpSystemTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AaCommandDumpSystemTest::SetUpTestCase()
{}

void AaCommandDumpSystemTest::TearDownTestCase()
{}

void AaCommandDumpSystemTest::SetUp()
{
    // reset optind to 0
    optind = 0;
}

void AaCommandDumpSystemTest::TearDown()
{}

/**
 * @tc.number: Aa_Command_Dump_SystemTest_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa dump -a" command.
 */
HWTEST_F(AaCommandDumpSystemTest, Aa_Command_Dump_SystemTest_0100, Function | MediumTest | Level1)
{
    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_PAGE_ABILITY_BUNDLE_NAME);

    // install the bundle
    ToolSystemTest::InstallBundle(STRING_PAGE_ABILITY_BUNDLE_PATH, true);

    // dump the abilities
    std::string command = "aa dump -a";
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_NE(commandResult, "");

    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_PAGE_ABILITY_BUNDLE_NAME);
}

/**
 * @tc.number: Aa_Command_Dump_SystemTest_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa dump -d" command.
 */
HWTEST_F(AaCommandDumpSystemTest, Aa_Command_Dump_SystemTest_0200, Function | MediumTest | Level1)
{
    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_DATA_ABILITY_BUNDLE_NAME);

    // install the bundle
    ToolSystemTest::InstallBundle(STRING_DATA_ABILITY_BUNDLE_PATH, true);

    // dump the abilities
    std::string command = "aa dump -d";
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_NE(commandResult, "");

    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_DATA_ABILITY_BUNDLE_NAME);
}

/**
 * @tc.number: Aa_Command_Dump_SystemTest_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa dump -e" command.
 */
HWTEST_F(AaCommandDumpSystemTest, Aa_Command_Dump_SystemTest_0300, Function | MediumTest | Level1)
{
    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_SERVICE_ABILITY_BUNDLE_NAME);

    // install the bundle
    ToolSystemTest::InstallBundle(STRING_SERVICE_ABILITY_BUNDLE_PATH, true);

    // dump the abilities
    std::string command = "aa dump -d";
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_NE(commandResult, "");

    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_SERVICE_ABILITY_BUNDLE_NAME);
}
