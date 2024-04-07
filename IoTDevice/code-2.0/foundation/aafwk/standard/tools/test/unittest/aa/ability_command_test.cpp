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

class AaCommandTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void MakeMockObjects() const;
};

void AaCommandTest::SetUpTestCase()
{}

void AaCommandTest::TearDownTestCase()
{}

void AaCommandTest::SetUp()
{
    // reset optind to 0
    optind = 0;

    // make mock objects
    MakeMockObjects();
}

void AaCommandTest::TearDown()
{}

void AaCommandTest::MakeMockObjects() const
{
    // mock a stub
    auto managerStubPtr = sptr<IRemoteObject>(new MockAbilityManagerStub());

    // set the mock stub
    auto managerClientPtr = AbilityManagerClient::GetInstance();
    managerClientPtr->remoteObject_ = managerStubPtr;
}

/**
 * @tc.number: Aa_Command_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa" command.
 */
HWTEST_F(AaCommandTest, Aa_Command_0100, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG);
}

/**
 * @tc.number: Aa_Command_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa xxx" command.
 */
HWTEST_F(AaCommandTest, Aa_Command_0200, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), cmd.GetCommandErrorMsg() + HELP_MSG);
}

/**
 * @tc.number: Aa_Command_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa -xxx" command.
 */
HWTEST_F(AaCommandTest, Aa_Command_0300, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"-xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), cmd.GetCommandErrorMsg() + HELP_MSG);
}

/**
 * @tc.number: Aa_Command_0400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa --xxx" command.
 */
HWTEST_F(AaCommandTest, Aa_Command_0400, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"--xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), cmd.GetCommandErrorMsg() + HELP_MSG);
}

/**
 * @tc.number: Aa_Command_0500
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa help" command.
 */
HWTEST_F(AaCommandTest, Aa_Command_0500, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"help",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    AbilityManagerShellCommand cmd(argc, argv);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG);
}
