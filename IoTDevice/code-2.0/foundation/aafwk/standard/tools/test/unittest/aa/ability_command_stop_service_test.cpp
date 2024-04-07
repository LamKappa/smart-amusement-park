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

#define protected public
#include "ability_command.h"
#undef protected
#include "mock_ability_manager_stub.h"
#define private public
#include "ability_manager_client.h"
#undef private
#include "ability_manager_interface.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;

class AaCommandStopServiceTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void MakeMockObjects() const;

    std::string cmd_ = "stop-service";
};

void AaCommandStopServiceTest::SetUpTestCase()
{}

void AaCommandStopServiceTest::TearDownTestCase()
{}

void AaCommandStopServiceTest::SetUp()
{
    // reset optind to 0
    optind = 0;

    // make mock objects
    MakeMockObjects();
}

void AaCommandStopServiceTest::TearDown()
{}

void AaCommandStopServiceTest::MakeMockObjects() const
{
    // mock a stub
    auto managerStubPtr = sptr<IRemoteObject>(new MockAbilityManagerStub());

    // set the mock stub
    auto managerClientPtr = AbilityManagerClient::GetInstance();
    managerClientPtr->remoteObject_ = managerStubPtr;
}

/**
 * @tc.number: Aa_Command_StopService_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_0100, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service xxx" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_0200, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -x" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_0300, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-x",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option '-x'.\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_0400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -xxx" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_0400, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option '-xxx'.\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_0500
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service --x" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_0500, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"--x",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option '--x'.\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_0600
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service --xxx" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_0600, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"--xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option '--xxx'.\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_0700
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -h" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_0700, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-h",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_0800
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service --help" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_0800, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"--help",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_0900
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_0900, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-d",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), "error: option '-d' requires a value.\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_1000
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d <device-id>" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_1000, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-d",
        (char *)STRING_DEVICE.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(),
        HELP_MSG_NO_ABILITY_NAME_OPTION + "\n" + HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_1100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d <device-id> -a" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_1100, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-d",
        (char *)STRING_DEVICE.c_str(),
        (char *)"-a",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), "error: option '-a' requires a value.\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_1200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d <device-id> -a <ability-name>" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_1200, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-d",
        (char *)STRING_DEVICE.c_str(),
        (char *)"-a",
        (char *)STRING_ABILITY_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_1300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d <device-id> -b" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_1300, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-d",
        (char *)STRING_DEVICE.c_str(),
        (char *)"-b",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), "error: option '-b' requires a value.\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_1400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d <device-id> -b <bundle-name>" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_1400, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-d",
        (char *)STRING_DEVICE.c_str(),
        (char *)"-b",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_ABILITY_NAME_OPTION + "\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_1500
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d <device-id> -a <ability-name> -b" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_1500, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-d",
        (char *)STRING_DEVICE.c_str(),
        (char *)"-a",
        (char *)STRING_ABILITY_NAME.c_str(),
        (char *)"-b",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), "error: option '-b' requires a value.\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_1600
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d <device-id> -a <ability-name> -b <bundle-name>" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_1600, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-d",
        (char *)STRING_DEVICE.c_str(),
        (char *)"-a",
        (char *)STRING_ABILITY_NAME.c_str(),
        (char *)"-b",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), STRING_STOP_SERVICE_ABILITY_OK + "\n");
}

/**
 * @tc.number: Aa_Command_StopService_1700
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -a" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_1700, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-a",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), "error: option '-a' requires a value.\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_1800
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -a <ability-name> -b" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_1800, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-a",
        (char *)STRING_ABILITY_NAME.c_str(),
        (char *)"-b",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), "error: option '-b' requires a value.\n" + HELP_MSG_STOP_SERVICE);
}

/**
 * @tc.number: Aa_Command_StopService_1900
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -a <ability-name> -b <bundle-name>" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_1900, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-a",
        (char *)STRING_ABILITY_NAME.c_str(),
        (char *)"-b",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), STRING_STOP_SERVICE_ABILITY_OK + "\n");
}

/**
 * @tc.number: Aa_Command_StopService_2000
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d <device-id> -a <ability-name> -b <bundle-name>" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_2000, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-d",
        (char *)STRING_DEVICE.c_str(),
        (char *)"-a",
        (char *)STRING_ABILITY_NAME_INVALID.c_str(),
        (char *)"-b",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(
        cmd.ExecCommand(), STRING_STOP_SERVICE_ABILITY_NG + "\n" + cmd.messageMap_.at(RESOLVE_ABILITY_ERR) + "\n");
}

/**
 * @tc.number: Aa_Command_StopService_2100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d <device-id> -a <ability-name> -b <bundle-name>" command.
 */
HWTEST_F(AaCommandStopServiceTest, Aa_Command_StopService_2100, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-d",
        (char *)STRING_DEVICE.c_str(),
        (char *)"-a",
        (char *)STRING_ABILITY_NAME.c_str(),
        (char *)"-b",
        (char *)STRING_BUNDLE_NAME_INVALID.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), STRING_STOP_SERVICE_ABILITY_NG + "\n" + cmd.messageMap_.at(RESOLVE_APP_ERR) + "\n");
}
