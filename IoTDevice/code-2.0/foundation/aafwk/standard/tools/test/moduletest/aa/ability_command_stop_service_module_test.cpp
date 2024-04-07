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

class AaCommandStopServiceModuleTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void MakeMockObjects() const;

    std::string cmd_ = "stop-service";
};

void AaCommandStopServiceModuleTest::SetUpTestCase()
{}

void AaCommandStopServiceModuleTest::TearDownTestCase()
{}

void AaCommandStopServiceModuleTest::SetUp()
{
    // reset optind to 0
    optind = 0;

    // make mock objects
    MakeMockObjects();
}

void AaCommandStopServiceModuleTest::TearDown()
{}

void AaCommandStopServiceModuleTest::MakeMockObjects() const
{
    // mock a stub
    auto managerStubPtr = sptr<IRemoteObject>(new MockAbilityManagerStub());

    // set the mock stub
    auto managerClientPtr = AbilityManagerClient::GetInstance();
    managerClientPtr->remoteObject_ = managerStubPtr;
}

/**
 * @tc.number: Aa_Command_StopService_ModuleTest_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d <device-id> -a <ability-name> -b <bundle-name>" command.
 */
HWTEST_F(AaCommandStopServiceModuleTest, Aa_Command_StopService_ModuleTest_0100, Function | MediumTest | Level1)
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
 * @tc.number: Aa_Command_StopService_ModuleTest_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d <device-id> -a <ability-name> -b <bundle-name>" command.
 */
HWTEST_F(AaCommandStopServiceModuleTest, Aa_Command_StopService_ModuleTest_0200, Function | MediumTest | Level1)
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
 * @tc.number: Aa_Command_StopService_ModuleTest_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "aa stop-service -d <device-id> -a <ability-name> -b <bundle-name>" command.
 */
HWTEST_F(AaCommandStopServiceModuleTest, Aa_Command_StopService_ModuleTest_0300, Function | MediumTest | Level1)
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
