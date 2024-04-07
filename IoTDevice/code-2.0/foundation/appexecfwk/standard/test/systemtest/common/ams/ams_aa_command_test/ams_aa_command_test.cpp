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

#include <cstdio>
#include <fstream>
#include <sstream>
#include "ability_command.h"
#include "shell_command.h"
#include "hilog_wrapper.h"
#include "module_test_dump_util.h"
#include "system_test_ability_util.h"
#include <gtest/gtest.h>

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::MTUtil;
using namespace OHOS::STABUtil;
using std::string;

namespace {
static const std::string bundleName = "com.ohos.amsst.appA";
static const std::string abilityName = "AmsStAbilityA1";
constexpr int WAIT_LAUNCHER_OK = 25 * 1000;
static std::string launcherBundleName = "com.ix.launcher";
std::string systemUiBundle = "com.ohos.systemui";
}  // namespace

class AmsAACommandTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void ExecuteSystemForResult(const string &cmd, string &result);
    void ReadForResult(const string &cmd, string &result);
    void Clear();
};

void AmsAACommandTest::SetUpTestCase(void)
{
    STAbilityUtil::Install("amsSystemTestA");
}

void AmsAACommandTest::TearDownTestCase(void)
{
    STAbilityUtil::Uninstall("com.ohos.amsst.appA");
}

void AmsAACommandTest::SetUp(void)
{}

void AmsAACommandTest::TearDown(void)
{}

void AmsAACommandTest::ExecuteSystemForResult(const string &cmd, string &result)
{
    result.clear();
    int MAX_SIZE = 1024;
    char buf_ps[MAX_SIZE];
    FILE *ptr;
    string command = "aa " + cmd;
    if (!command.empty() && (ptr = popen(command.c_str(), "r")) != nullptr) {
        while (fgets(buf_ps, MAX_SIZE, ptr) != nullptr) {
            result.append(buf_ps);
        }
        pclose(ptr);
        ptr = nullptr;
    }
}

void AmsAACommandTest::Clear()
{
    STAbilityUtil::KillService("appspawn");
    STAbilityUtil::KillService("installs");
    STAbilityUtil::KillService(launcherBundleName);
    STAbilityUtil::KillService(systemUiBundle);
    STAbilityUtil::KillService("foundation");
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_LAUNCHER_OK));
}

/**
 * @tc.number    : ams_aa_command_test_0100
 * @tc.name      : aa -h
 * @tc.desc      : commands help when aa -h command.
 */
HWTEST_F(AmsAACommandTest, ams_aa_command_test_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0100 start";

    string result;
    const string cmd{"start -h"};
    const string &expectResult = HELP_MSG_START;
    ExecuteSystemForResult(cmd, result);

    EXPECT_EQ(result, expectResult);

    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0100 end";
}

/**
 * @tc.number    : ams_aa_command_test_0200
 * @tc.name      : aa -d <deviceName> -a <abilityName> -b <bundleName>
 * @tc.desc      : start/stop page ability when aa -d -a -b command.
 */
HWTEST_F(AmsAACommandTest, ams_aa_command_test_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0200 start";

    string result;
    const string cmd{"start -d deviceId -a " + abilityName + " -b " + bundleName};
    const string expectResult{"start ability successfully.\n"};
    ExecuteSystemForResult(cmd, result);
    EXPECT_EQ(result, expectResult);

    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0200 end";
}

/**
 * @tc.number    : ams_aa_command_test_0300
 * @tc.name      : aa dump -h
 * @tc.desc      : commands help when aa dump -h command.
 */
HWTEST_F(AmsAACommandTest, ams_aa_command_test_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0300 start";

    string result;
    const string cmd{"dump -h"};
    const string &expectResult = HELP_MSG_DUMP;
    ExecuteSystemForResult(cmd, result);

    EXPECT_EQ(result, expectResult);

    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0300 end";
}

/**
 * @tc.number    : ams_aa_command_test_0400
 * @tc.name      : aa dump -a
 * @tc.desc      : all page abilities when aa -a command.
 */
HWTEST_F(AmsAACommandTest, ams_aa_command_test_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0400 start";

    string dumpInfo, result;
    const string cmd{"dump -a"};
    const string &expectResult = "User ID #0\n  MissionStack ID #0\n    MissionRecord ID #0  bottom app "
                                 "[LauncherAbility]\n      AbilityRecord ID #0\n        app name";
    Clear();
    ExecuteSystemForResult(cmd, result);
    EXPECT_EQ(result.substr(0, expectResult.size()), expectResult);

    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0400 end";
}

/**
 * @tc.number    : ams_aa_command_test_0500
 * @tc.name      : aa dump -s
 * @tc.desc      : commands the ability info of a specificed stack when aa -s command.
 */
HWTEST_F(AmsAACommandTest, ams_aa_command_test_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0500 start";

    string result;
    const string cmd{"dump -s 0"};
    const string &expectResult = "User ID #0\n  MissionStack ID #0\n    MissionRecord ID #0  bottom app "
                                 "[LauncherAbility]\n      AbilityRecord ID #0\n        app name";
    Clear();
    ExecuteSystemForResult(cmd, result);
    EXPECT_EQ(result.substr(0, expectResult.size()), expectResult);

    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0500 end";
}

/**
 * @tc.number    : ams_aa_command_test_0600
 * @tc.name      : aa dump -m
 * @tc.desc      : dump the ability info of a specificed mission when aa -m command.
 */
HWTEST_F(AmsAACommandTest, ams_aa_command_test_0600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0600 start";

    string result;
    const string cmd{"dump -m 0"};
    const string &expectResult = "User ID #0\n    MissionRecord ID #0  bottom app [LauncherAbility]\n      "
                                 "AbilityRecord ID #0\n        app name";
    Clear();
    ExecuteSystemForResult(cmd, result);
    EXPECT_EQ(result.substr(0, expectResult.size()), expectResult);

    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0600 end";
}

/**
 * @tc.number    : ams_aa_command_test_0700
 * @tc.name      : aa dump -l
 * @tc.desc      : dump the mission list of every stack when aa -l command.
 */
HWTEST_F(AmsAACommandTest, ams_aa_command_test_0700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0700 start";
    string result;
    const string cmd{"dump -l"};
    const string &expectResult = "User ID #0\n  MissionStack ID #0 [ #0 ]\n  MissionStack ID #1 [ ]\n";
    Clear();
    ExecuteSystemForResult(cmd, result);
    EXPECT_EQ(result, expectResult);

    GTEST_LOG_(INFO) << "AmsAACommandTest ams_aa_command_test_0700 end";
}