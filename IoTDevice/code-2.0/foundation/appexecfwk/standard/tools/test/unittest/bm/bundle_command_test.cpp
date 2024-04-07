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

#define private public
#include "bundle_command.h"
#undef private
#include "mock_bundle_mgr_host.h"
#include "mock_bundle_installer_host.h"
#include "iremote_object.h"
#include "iremote_broker.h"
#include "bundle_installer_interface.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

class BmCommandTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void MakeMockObjects();
    void SetMockObjects(BundleManagerShellCommand &cmd) const;

    sptr<IBundleMgr> mgrProxyPtr_;
    sptr<IBundleInstaller> installerProxyPtr_;
};

void BmCommandTest::SetUpTestCase()
{}

void BmCommandTest::TearDownTestCase()
{}

void BmCommandTest::SetUp()
{
    // reset optind to 0
    optind = 0;

    // make mock objects
    MakeMockObjects();
}

void BmCommandTest::TearDown()
{}

void BmCommandTest::MakeMockObjects()
{
    // mock a mgr host
    auto mgrHostPtr = sptr<IRemoteObject>(new MockBundleMgrHost());
    // mock a mgr proxy
    mgrProxyPtr_ = iface_cast<IBundleMgr>(mgrHostPtr);

    // mock a installer host
    auto installerHostPtr = sptr<IRemoteObject>(new MockBundleInstallerHost());
    // mock a installer proxy
    installerProxyPtr_ = iface_cast<IBundleInstaller>(installerHostPtr);
}

void BmCommandTest::SetMockObjects(BundleManagerShellCommand &cmd) const
{
    // set the mock mgr proxy
    cmd.bundleMgrProxy_ = mgrProxyPtr_;

    // set the mock installer proxy
    cmd.bundleInstallerProxy_ = installerProxyPtr_;
}

/**
 * @tc.number: Bm_Command_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm" command.
 */
HWTEST_F(BmCommandTest, Bm_Command_0100, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG);
}

/**
 * @tc.number: Bm_Command_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm xxx" command.
 */
HWTEST_F(BmCommandTest, Bm_Command_0200, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), cmd.GetCommandErrorMsg() + HELP_MSG);
}

/**
 * @tc.number: Bm_Command_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm -xxx" command.
 */
HWTEST_F(BmCommandTest, Bm_Command_0300, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"-xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), cmd.GetCommandErrorMsg() + HELP_MSG);
}

/**
 * @tc.number: Bm_Command_0400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm --xxx" command.
 */
HWTEST_F(BmCommandTest, Bm_Command_0400, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"--xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), cmd.GetCommandErrorMsg() + HELP_MSG);
}

/**
 * @tc.number: Bm_Command_0500
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm help" command.
 */
HWTEST_F(BmCommandTest, Bm_Command_0500, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"help",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG);
}
