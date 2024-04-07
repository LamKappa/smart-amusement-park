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
#include <iremote_object.h>
#include <iremote_stub.h>
#include "system_ability_definition.h"
#define private public
#define protected public
#include "ability_manager_service.h"
#undef private
#undef protected
#include "bundlemgr/mock_bundle_manager.h"
#include "sa_mgr_client.h"
#include "appmgr_test_service.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
static void WaitUntilTaskFinished()
{
    const uint32_t maxRetryCount = 1000;
    const uint32_t sleepTime = 1000;
    uint32_t count = 0;
    auto handler = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };
    if (handler->PostTask(f)) {
        while (!taskCalled.load()) {
            ++count;
            if (count >= maxRetryCount) {
                break;
            }
            usleep(sleepTime);
        }
    }
}

namespace {
const std::string NAME_BUNDLE_MGR_SERVICE = "BundleMgrService";
static ElementName g_testAbility1("device", "com.ix.hiMusic", "MusicAbility");
static ElementName g_testAbility2("device", "com.ix.hiMusic", "MusicTopAbility");
static ElementName g_testAbility3("device", "com.ix.hiMusic", "MusicSAbility");
static ElementName g_testAbility4("device", "com.ix.hiRadio", "RadioAbility");
static ElementName g_testAbility5("device", "com.ix.hiRadio", "RadioTopAbility");
static ElementName g_launcherAbility("device", "com.ix.hiWord", "LauncherAbility");
static std::shared_ptr<AbilityManagerService> g_abilityMs = nullptr;
static std::shared_ptr<AppManagerTestService> g_appTestService = nullptr;
}  // namespace

bool IsTestAbility1Exist(const std::string &state)
{
    return std::string::npos != state.find("MusicAbility");
}

bool IsTestAbility2Exist(const std::string &state)
{
    return std::string::npos != state.find("MusicTopAbility");
}

bool IsTestAbility3Exist(const std::string &state)
{
    return std::string::npos != state.find("MusicSAbility");
}

bool IsTestAbility4Exist(const std::string &state)
{
    return std::string::npos != state.find("RadioAbility");
}

bool IsTestAbility5Exist(const std::string &state)
{
    return std::string::npos != state.find("RadioTopAbility");
}

bool IsLaunchAbilityExist(const std::string &state)
{
    return std::string::npos != state.find("LauncherAbility");
}

class AbilityDumpTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void StartAbilityes();
    void startAbility1();
    void startAbility2();
    void startAbility3();
    void startAbility4();
    void startAbility5();
    void startAbility6();

public:
    static constexpr int TEST_WAIT_TIME = 100000;
};

void AbilityDumpTest::SetUpTestCase()
{}

void AbilityDumpTest::TearDownTestCase()
{}

void AbilityDumpTest::SetUp()
{
    OHOS::sptr<OHOS::IRemoteObject> bundleObject = new BundleMgrService();
    OHOS::DelayedSingleton<SaMgrClient>::GetInstance()->RegisterSystemAbility(
        OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);

    g_abilityMs = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance();
    g_appTestService = OHOS::DelayedSingleton<AppManagerTestService>::GetInstance();
    g_abilityMs->OnStart();
    WaitUntilTaskFinished();
    g_appTestService->Start();

    StartAbilityes();
}

void AbilityDumpTest::TearDown()
{
    OHOS::DelayedSingleton<AbilityManagerService>::DestroyInstance();
}

void AbilityDumpTest::StartAbilityes()
{
    startAbility6();
    startAbility1();
    startAbility2();
    startAbility3();
    startAbility4();
    startAbility5();
}

void AbilityDumpTest::startAbility1()
{
    Want want;
    want.SetElement(g_testAbility1);
    g_abilityMs->StartAbility(want);
    WaitUntilTaskFinished();
    auto topAbility = g_abilityMs->GetStackManager()->GetCurrentTopAbility();
    if (topAbility) {
        topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }
}

void AbilityDumpTest::startAbility2()
{
    Want want;
    want.SetElement(g_testAbility2);
    g_abilityMs->StartAbility(want);
    WaitUntilTaskFinished();
    auto topAbility = g_abilityMs->GetStackManager()->GetCurrentTopAbility();
    if (topAbility) {
        topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }
}

void AbilityDumpTest::startAbility3()
{
    Want want;
    want.SetElement(g_testAbility3);
    g_abilityMs->StartAbility(want);
    WaitUntilTaskFinished();
    auto topAbility = g_abilityMs->GetStackManager()->GetCurrentTopAbility();
    if (topAbility) {
        topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }
}

void AbilityDumpTest::startAbility4()
{
    Want want;
    want.SetElement(g_testAbility4);
    g_abilityMs->StartAbility(want);
    WaitUntilTaskFinished();
    auto topAbility = g_abilityMs->GetStackManager()->GetCurrentTopAbility();
    if (topAbility) {
        topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }
}

void AbilityDumpTest::startAbility5()
{
    Want want;
    want.SetElement(g_testAbility5);
    g_abilityMs->StartAbility(want);
    WaitUntilTaskFinished();
    auto topAbility = g_abilityMs->GetStackManager()->GetCurrentTopAbility();
    if (topAbility) {
        topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }
}

void AbilityDumpTest::startAbility6()
{
    Want want;
    want.SetElement(g_launcherAbility);
    want.AddEntity(Want::ENTITY_HOME);
    g_abilityMs->StartAbility(want);
    WaitUntilTaskFinished();
    auto topAbility = g_abilityMs->GetStackManager()->GetCurrentTopAbility();
    if (topAbility) {
        topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: DumpState when args empty
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_001, TestSize.Level2)
{
    std::string args;
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);

    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility1Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility2Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility3Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility4Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility5Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsLaunchAbilityExist));
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: DumpState when args empty
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_002, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Ability_Dump_002 start";
    std::string args("--top");
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);

    EXPECT_NE(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility5Exist));
    GTEST_LOG_(INFO) << "Ability_Dump_002 end";
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: DumpState when args fail
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_003, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Ability_Dump_003 start";
    std::string expectResult = "error: invalid argument, please see 'ability dump -h'.";
    std::string args("--stack");
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);
    EXPECT_EQ(1UL, result.size());
    EXPECT_EQ(result[0], expectResult);
    GTEST_LOG_(INFO) << "Ability_Dump_003 end";
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: Verify DumpState --stack 0
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_004, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Ability_Dump_004 start";
    std::string args("--stack 0");
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility1Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility2Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility3Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility4Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility5Exist));
    GTEST_LOG_(INFO) << "Ability_Dump_004 end";
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: Verify DumpState --stack 1
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_005, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Ability_Dump_005 start";
    std::string args("--stack 1");
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);

    EXPECT_NE(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility1Exist));
    EXPECT_NE(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility2Exist));
    EXPECT_NE(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility3Exist));
    EXPECT_NE(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility4Exist));
    EXPECT_NE(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility5Exist));
    GTEST_LOG_(INFO) << "Ability_Dump_005 end";
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: Verify DumpState --stack 10
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_006, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Ability_Dump_006 start";
    std::string expectResult = "Invalid stack number, please see ability dump stack-list.";
    std::string args("--stack 100");
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);
    auto isExist = std::find_if(
        result.begin(), result.end(), [&expectResult](const std::string &str) -> bool { return str == expectResult; });
    EXPECT_TRUE(isExist != result.end());
    GTEST_LOG_(INFO) << "Ability_Dump_006 end";
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: Verify DumpState --stack-list
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_007, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Ability_Dump_007 start";
    std::string stack1("  MissionStack ID #1 [ #5 #4 #3 #2 #1 ]");
    std::string args("--stack-list");
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);

    // Because the user ID is static, it will increase all the time
    std::for_each(result.begin(), result.end(), [](std::string &s) { GTEST_LOG_(INFO) << s.c_str(); });
    int sum = result.size();
    EXPECT_NE(sum, 0);
    GTEST_LOG_(INFO) << "Ability_Dump_007 end";
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: Verify DumpState --mission fail
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_008, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Ability_Dump_008 start";
    std::string expectResult = "error: invalid argument, please see 'ability dump -h'.";
    std::string args("--mission");
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);
    EXPECT_EQ(result[0], expectResult);

    GTEST_LOG_(INFO) << "Ability_Dump_008 end";
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: Verify DumpState --mission 0
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_009, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Ability_Dump_009 start";
    std::string args("--mission 0");
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility2Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility3Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility4Exist));

    GTEST_LOG_(INFO) << "Ability_Dump_009 end";
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: Verify DumpState --mission 1
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_010, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Ability_Dump_010 start";
    std::string args("--mission 1");
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);

    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsLaunchAbilityExist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility1Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility2Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility3Exist));
    EXPECT_EQ(result.end(), std::find_if(result.begin(), result.end(), IsTestAbility4Exist));
    GTEST_LOG_(INFO) << "Ability_Dump_010 end";
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: Verify DumpState --mission 100 cause invalid
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_011, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Ability_Dump_011 start";
    std::string expectResult = "error: invalid mission number, please see 'ability dump --stack-list'.";
    std::string args("--mission 100");
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);
    EXPECT_EQ(result[1], expectResult);
    GTEST_LOG_(INFO) << "Ability_Dump_011 end";
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: Verify DumpState --waitting-queue
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_012, TestSize.Level2)
{
    Want want1;
    Want want2;
    std::string expectResult("The waitting ability queue is empty.");

    want1.SetElement(g_testAbility1);
    want2.SetElement(g_testAbility5);

    GTEST_LOG_(INFO) << "Ability_Dump_012 start";
    g_abilityMs->StartAbility(want1);
    g_abilityMs->StartAbility(want2);

    std::string args("--waitting-queue");
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);
    EXPECT_NE(result.end(), std::find_if(result.begin(), result.end(), [](const std::string &state) {
        return std::string::npos != state.find("RadioTopAbility");
    }));
    usleep(TEST_WAIT_TIME * 6);
    result.clear();
    g_abilityMs->DumpState(args, result);
    EXPECT_NE(result[0], expectResult);

    GTEST_LOG_(INFO) << "Ability_Dump_012 end";
}

/*
 * Feature: AbilityManagerService
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpState
 * EnvConditions: NA
 * CaseDescription: Verify DumpState when args invalid
 */
HWTEST_F(AbilityDumpTest, Ability_Dump_013, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Ability_Dump_013 start";
    std::string args("x");
    std::vector<std::string> result;
    g_abilityMs->DumpState(args, result);
    int size = result.size();
    EXPECT_EQ(size, 0);
    GTEST_LOG_(INFO) << "Ability_Dump_013 end";
}
}  // namespace AAFwk
}  // namespace OHOS