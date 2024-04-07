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

#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include "system_ability_definition.h"
#include "ability_info.h"
#include "application_info.h"
#include "ability_manager_service.h"
#include "ability_manager_errors.h"
#include "gtest/gtest.h"
#include "mock_bundle_manager.h"
#include "want.h"
#include "sa_mgr_client.h"
#include "appmgr_test_service.h"
#include "module_test_dump_util.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using OHOS::MTUtil::MTDumpUtil;

namespace OHOS {
namespace AAFwk {
namespace {
#define SLEEP(milli) std::this_thread::sleep_for(std::chrono::milliseconds(milli))

const std::string NAME_BUNDLE_MGR_SERVICE = "BundleMgrService";
static std::shared_ptr<AbilityManagerService> g_abilityMs = nullptr;
static std::shared_ptr<AppManagerTestService> g_appTestService = nullptr;

static const ElementName G_TESTABILITY1("device", "com.ix.hiMusic", "MainAbility1");
static const ElementName G_TESTABILITY2("device", "com.ix.hiMusic", "MainAbility2");
static const ElementName G_TESTABILITY3("device", "com.ix.hiRadio", "MainAbility3");
static const ElementName G_TESTABILITY4("device", "com.ix.hiRadio", "MainAbility4");
static const ElementName G_LAUNCHABILITY("device", "com.ix.hiworld", "LaunchAbility");
static const ElementName G_TESTBLOCKACTIVEABILITY("device", "com.ix.musicService", "ServiceAbility");
static const ElementName G_TESTABILITY5("device", "com.ix.hiRadio", "MainAbility5");
static const ElementName G_TESTABILITY6("device", "com.ix.hiRadio", "MainAbility6");
}  // namespace

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

std::string ShowDump(const std::vector<std::string> &strVec)
{
    std::string ret;
    for (const auto &str : strVec) {
        ret += str + "\n";
    }
    return ret;
}

class DumpModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void StartAllAbilities();
    bool SearchAbilityNameFromStackInfo(const std::string &abilityName, const std::vector<AbilityRecordInfo> &vec);

    Want want11;
    Want want22;
    Want want33;
    Want want44;
    Want want55;
    Want wantLauncher;
};

bool DumpModuleTest::SearchAbilityNameFromStackInfo(
    const std::string &abilityName, const std::vector<AbilityRecordInfo> &vec)
{
    auto iter = std::find_if(vec.begin(), vec.end(), [&abilityName](const AbilityRecordInfo &abilityRecordInfo) {
        return (abilityRecordInfo.mainName == abilityName);
    });
    return ((iter == vec.end()) ? false : true);
}

void DumpModuleTest::SetUpTestCase()
{}

void DumpModuleTest::TearDownTestCase()
{}

void DumpModuleTest::SetUp()
{
    OHOS::sptr<OHOS::IRemoteObject> bundleObject = new BundleMgrService();
    OHOS::DelayedSingleton<SaMgrClient>::GetInstance()->RegisterSystemAbility(
        OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);

    g_abilityMs = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance();
    g_abilityMs->OnStart();
    WaitUntilTaskFinished();

    want11.SetElement(G_TESTABILITY1);
    want22.SetElement(G_TESTABILITY2);
    want33.SetElement(G_TESTABILITY3);
    want44.SetElement(G_TESTABILITY4);
    want55.SetElement(G_TESTABILITY6);
    wantLauncher.SetElement(G_LAUNCHABILITY);
    StartAllAbilities();
}

void DumpModuleTest::StartAllAbilities()
{
    wantLauncher.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    g_abilityMs->StartAbility(wantLauncher);
    WaitUntilTaskFinished();
    auto topAbilityLuncher = g_abilityMs->GetStackManager()->GetCurrentTopAbility();
    if (topAbilityLuncher) {
        topAbilityLuncher->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }

    g_abilityMs->StartAbility(want11);
    WaitUntilTaskFinished();
    auto topAbility11 = g_abilityMs->GetStackManager()->GetCurrentTopAbility();
    if (topAbility11) {
        topAbility11->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }

    g_abilityMs->StartAbility(want22);
    WaitUntilTaskFinished();
    auto topAbility22 = g_abilityMs->GetStackManager()->GetCurrentTopAbility();
    if (topAbility22) {
        topAbility22->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }

    g_abilityMs->StartAbility(want33);
    WaitUntilTaskFinished();
    auto topAbility33 = g_abilityMs->GetStackManager()->GetCurrentTopAbility();
    if (topAbility33) {
        topAbility33->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }

    g_abilityMs->StartAbility(want44);
    WaitUntilTaskFinished();
    auto topAbility44 = g_abilityMs->GetStackManager()->GetCurrentTopAbility();
    if (topAbility44) {
        topAbility44->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }

    g_abilityMs->StartAbility(want55);
    WaitUntilTaskFinished();
    auto topAbility55 = g_abilityMs->GetStackManager()->GetCurrentTopAbility();
    if (topAbility55) {
        topAbility55->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }
}

void DumpModuleTest::TearDown()
{
    g_abilityMs->OnStop();
    OHOS::DelayedSingleton<AbilityManagerService>::DestroyInstance();
}

/*
 * Feature: Aafwk
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: test AbilityManagerService DumpState
 * EnvConditions: System running normally
 * CaseDescription: DumpState to show info of stacks respectively
 */
HWTEST_F(DumpModuleTest, dump_module_test_001, TestSize.Level2)
{
    std::vector<std::string> dumpInfo;
    g_abilityMs->DumpState("--stack 1", dumpInfo);
    std::vector<Want> abilitiesStarted = {
        want55,
        want44,
        want33,
        want22,
        want11,
    };
    std::vector<std::string> abilityNames;
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, abilityNames);
    ASSERT_EQ(abilitiesStarted.size(), abilityNames.size());
    for (unsigned int i = 0; i < abilityNames.size(); ++i) {
        ASSERT_EQ(0, abilitiesStarted[i].GetElement().GetAbilityName().compare(abilityNames[i]));
    }

    dumpInfo.clear();

    g_abilityMs->DumpState("--stack 0", dumpInfo);
    abilitiesStarted = {
        wantLauncher,
    };
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, abilityNames);
    ASSERT_EQ(abilitiesStarted.size(), abilityNames.size());
    for (unsigned int i = 0; i < abilityNames.size(); ++i) {
        ASSERT_EQ(0, abilitiesStarted[i].GetElement().GetAbilityName().compare(abilityNames[i]));
    }

    dumpInfo.clear();

    g_abilityMs->DumpState("--stack 1 abc", dumpInfo);
    abilitiesStarted = {
        want55,
        want44,
        want33,
        want22,
        want11,
    };
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, abilityNames);
    ASSERT_EQ(abilitiesStarted.size(), abilityNames.size());
    for (unsigned int i = 0; i < abilityNames.size(); ++i) {
        ASSERT_EQ(0, abilitiesStarted[i].GetElement().GetAbilityName().compare(abilityNames[i]));
    }

    dumpInfo.clear();

    g_abilityMs->DumpState(" --stack 1", dumpInfo);
    abilitiesStarted = {
        want55,
        want44,
        want33,
        want22,
        want11,
    };
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, abilityNames);
    ASSERT_EQ(abilitiesStarted.size(), abilityNames.size());
    for (unsigned int i = 0; i < abilityNames.size(); ++i) {
        ASSERT_EQ(0, abilitiesStarted[i].GetElement().GetAbilityName().compare(abilityNames[i]));
    }
}

/*
 * Feature: Aafwk
 * Function: DumpStack
 * SubFunction: NA
 * FunctionPoints: test AbilityStackManager DumpStack
 * EnvConditions: System running normally
 * CaseDescription: DumpStack directly to show info of stacks respectively
 */
HWTEST_F(DumpModuleTest, dump_module_test_002, TestSize.Level2)
{
    std::vector<std::string> dumpInfo;
    g_abilityMs->GetStackManager()->DumpStack(1, dumpInfo);
    std::vector<Want> abilitiesStarted = {
        want55,
        want44,
        want33,
        want22,
        want11,
    };
    std::vector<std::string> abilityNames;
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, abilityNames);

    GTEST_LOG_(INFO) << "abilitiesStarted.size() = " << abilitiesStarted.size();
    GTEST_LOG_(INFO) << "abilityNames.size() = " << abilityNames.size();

    ASSERT_EQ(abilitiesStarted.size(), abilityNames.size());
    for (unsigned int i = 0; i < abilityNames.size(); ++i) {
        ASSERT_EQ(0, abilitiesStarted[i].GetElement().GetAbilityName().compare(abilityNames[i]));
    }

    dumpInfo.clear();

    g_abilityMs->GetStackManager()->DumpStack(0, dumpInfo);
    abilitiesStarted = {
        wantLauncher,
    };
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, abilityNames);
    ASSERT_EQ(abilitiesStarted.size(), abilityNames.size());
    for (unsigned int i = 0; i < abilityNames.size(); ++i) {
        ASSERT_EQ(0, abilitiesStarted[i].GetElement().GetAbilityName().compare(abilityNames[i]));
    }
}

/*
 * Feature: Aafwk
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: testAbilityManagerService DumpState
 * EnvConditions: System running normally
 * CaseDescription: Handle wrong args about missionStack
 */
HWTEST_F(DumpModuleTest, dump_module_test_003, TestSize.Level2)
{
    std::string expectedResult1 = "error: invalid argument, please see 'ability dump -h'.";
    std::string expectedResult2 = "Invalid stack number, please see ability dump stack-list.";
    std::vector<std::string> dumpInfo;

    g_abilityMs->DumpState("--stack", dumpInfo);
    EXPECT_EQ(1UL, dumpInfo.size());
    EXPECT_EQ(dumpInfo[0], expectedResult1);

    dumpInfo.clear();

    g_abilityMs->DumpState("--sta ck", dumpInfo);
    EXPECT_EQ(0UL, dumpInfo.size());

    dumpInfo.clear();

    g_abilityMs->DumpState("--stack1", dumpInfo);
    EXPECT_EQ(0UL, dumpInfo.size());

    dumpInfo.clear();

    g_abilityMs->DumpState("--stack a", dumpInfo);
    EXPECT_EQ(2UL, dumpInfo.size());
    EXPECT_EQ(dumpInfo[1], expectedResult2);

    dumpInfo.clear();

    g_abilityMs->DumpState("--stack 10", dumpInfo);
    EXPECT_EQ(2UL, dumpInfo.size());
    EXPECT_EQ(dumpInfo[1], expectedResult2);

    dumpInfo.clear();

    g_abilityMs->DumpState(" ", dumpInfo);
    EXPECT_EQ(0UL, dumpInfo.size());

    dumpInfo.clear();

    g_abilityMs->DumpState("", dumpInfo);
    EXPECT_EQ(0UL, dumpInfo.size());

    dumpInfo.clear();

    g_abilityMs->DumpState("--", dumpInfo);
    EXPECT_EQ(0UL, dumpInfo.size());

    dumpInfo.clear();

    g_abilityMs->DumpState("-", dumpInfo);
    EXPECT_EQ(0UL, dumpInfo.size());

    dumpInfo.clear();

    g_abilityMs->DumpState("-ss", dumpInfo);
    EXPECT_EQ(0UL, dumpInfo.size());

    dumpInfo.clear();

    g_abilityMs->DumpState("!.. --stack 1", dumpInfo);
    EXPECT_EQ(0UL, dumpInfo.size());

    dumpInfo.clear();

    g_abilityMs->GetStackManager()->DumpStack(10, dumpInfo);
    // index 0 :"User ID #"
    // index 2 :"Invalid stack number, please see ability dump stack-list."
    EXPECT_EQ(2UL, dumpInfo.size());
    EXPECT_EQ(dumpInfo[0], "User ID #0");
    EXPECT_EQ(dumpInfo[1], expectedResult2);
}

/*
 * Feature: Aafwk
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: test AbilityManagerService DumpState
 * EnvConditions: System running normally
 * CaseDescription: DumpState to show info of the top ability
 */
HWTEST_F(DumpModuleTest, dump_module_test_004, TestSize.Level2)
{
    std::string args("--top");
    std::vector<std::string> dumpInfo;
    g_abilityMs->DumpState(args, dumpInfo);
    std::vector<std::string> abilityNames;
    int abilityNum = MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, abilityNames);
    ASSERT_EQ(1, abilityNum);
    ASSERT_EQ("MainAbility6", abilityNames[0]);
}

/*
 * Feature: Aafwk
 * Function: DumpTopAbility
 * SubFunction: NA
 * FunctionPoints: test AbilityStackManager DumpTopAbility
 * EnvConditions: System running normally
 * CaseDescription: DumpTopAbility directly to show info of the top ability
 */
HWTEST_F(DumpModuleTest, dump_module_test_005, TestSize.Level2)
{
    std::vector<std::string> dumpInfo;
    std::vector<std::string> abilityNames;
    g_abilityMs->GetStackManager()->DumpTopAbility(dumpInfo);
    int abilityNum = MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, abilityNames);
    ASSERT_EQ(1, abilityNum);
    ASSERT_EQ("MainAbility6", abilityNames[0]);
}

/*
 * Feature: Aafwk
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: test AbilityManagerService DumpState
 * EnvConditions: System running normally
 * CaseDescription: DumpState to show all dump info
 */
HWTEST_F(DumpModuleTest, dump_module_test_006, TestSize.Level2)
{
    std::string args("--all");
    std::vector<std::string> dumpInfo;
    g_abilityMs->DumpState(args, dumpInfo);
    std::vector<Want> abilitiesStarted = {
        want55,
        want44,
        want33,
        want22,
        want11,
        wantLauncher,
    };
    std::vector<std::string> abilityNames;
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, abilityNames);
    ASSERT_EQ(abilitiesStarted.size(), abilityNames.size());
    for (unsigned int i = 0; i < abilityNames.size(); ++i) {
        GTEST_LOG_(INFO) << "abilitiesStarted = " << abilitiesStarted[i].GetElement().GetAbilityName();
        GTEST_LOG_(INFO) << "abilityNames = " << abilityNames[i];
        ASSERT_EQ(0, abilitiesStarted[i].GetElement().GetAbilityName().compare(abilityNames[i]));
    }

    dumpInfo.clear();
    g_abilityMs->GetStackManager()->Dump(dumpInfo);
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, abilityNames);
    ASSERT_EQ(abilitiesStarted.size(), abilityNames.size());
    for (unsigned int i = 0; i < abilityNames.size(); ++i) {
        ASSERT_EQ(0, abilitiesStarted[i].GetElement().GetAbilityName().compare(abilityNames[i]));
    }
}

/*
 * Feature: Aafwk
 * Function: DumpTopAbility
 * SubFunction: NA
 * FunctionPoints: test AbilityStackManager DumpTopAbility
 * EnvConditions: System running normally
 * CaseDescription: Dump directly to show all dump info
 */
HWTEST_F(DumpModuleTest, dump_module_test_007, TestSize.Level2)
{
    std::vector<std::string> dumpInfo;
    std::vector<Want> abilitiesStarted = {want55, want44, want33, want22, want11, wantLauncher};
    std::vector<std::string> abilityNames;
    g_abilityMs->GetStackManager()->Dump(dumpInfo);
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, abilityNames);
    ASSERT_EQ(abilitiesStarted.size(), abilityNames.size());
    for (unsigned int i = 0; i < abilityNames.size(); ++i) {
        GTEST_LOG_(INFO) << "abilitiesStarted = " << abilitiesStarted[i].GetElement().GetAbilityName();
        GTEST_LOG_(INFO) << "abilityNames = " << abilityNames[i];
        ASSERT_EQ(0, abilitiesStarted[i].GetElement().GetAbilityName().compare(abilityNames[i]));
    }
}

/*
 * Feature: Aafwk
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: test AbilityManagerService DumpState
 * EnvConditions: System running normally
 * CaseDescription: DumpState to show info of missionRecords respectively
 */
HWTEST_F(DumpModuleTest, dump_module_test_008, TestSize.Level2)
{

    std::string args("--mission");
    std::vector<std::string> dumpInfo;
    std::vector<std::string> abilityNames;

    ASSERT_TRUE(g_abilityMs);
    auto stackMgr = g_abilityMs->GetStackManager();
    ASSERT_TRUE(stackMgr);
    auto missionRecord = stackMgr->GetTopMissionRecord();
    ASSERT_TRUE(missionRecord);
    int id = missionRecord->GetMissionRecordId();
    args += " ";
    args += std::to_string(id);
    GTEST_LOG_(INFO) << "args = " << args;

    g_abilityMs->DumpState(args, dumpInfo);

    std::vector<Want> abilitiesStarted = {
        want55,
        want44,
        want33,
        want22,
        want11,
    };

    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, abilityNames);
    ASSERT_EQ(abilitiesStarted.size(), abilityNames.size());
    for (unsigned int i = 0; i < abilityNames.size(); ++i) {
        ASSERT_EQ(0, abilitiesStarted[i].GetElement().GetAbilityName().compare(abilityNames[i]));
    }
}

/*
 * Feature: Aafwk
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: test AbilityManagerService DumpState
 * EnvConditions: System running normally
 * CaseDescription: Handle wrong args about missionRecord
 */
HWTEST_F(DumpModuleTest, dump_module_test_09, TestSize.Level2)
{
    std::string args;
    std::vector<std::string> dumpInfo;
    std::string expectedResult1 = "error: invalid argument, please see 'ability dump -h'.";
    std::string expectedResult2 = "error: invalid mission number, please see 'ability dump --stack-list'.";

    args = "--mission 10";
    g_abilityMs->DumpState(args, dumpInfo);
    EXPECT_EQ(2UL, dumpInfo.size());
    EXPECT_EQ(dumpInfo[1], expectedResult2);

    dumpInfo.clear();

    args = "--mission a";
    g_abilityMs->DumpState(args, dumpInfo);
    EXPECT_EQ(2UL, dumpInfo.size());
    EXPECT_EQ(dumpInfo[1], expectedResult2);

    dumpInfo.clear();

    args = "--mission";
    g_abilityMs->DumpState(args, dumpInfo);
    EXPECT_EQ(1UL, dumpInfo.size());
    EXPECT_EQ(dumpInfo[0], expectedResult1);

    dumpInfo.clear();

    args = "--mission1";
    g_abilityMs->DumpState(args, dumpInfo);
    EXPECT_EQ(0UL, dumpInfo.size());

    dumpInfo.clear();

    args = "--miss ion";
    g_abilityMs->DumpState(args, dumpInfo);
    EXPECT_EQ(0UL, dumpInfo.size());

    dumpInfo.clear();

    args = "-m 1";
    g_abilityMs->DumpState(args, dumpInfo);
    EXPECT_EQ(2UL, dumpInfo.size());
}

/*
 * Feature: Aafwk
 * Function: GetAllStackInfo
 * SubFunction: NA
 * FunctionPoints: test AbilityManagerService GetAllStackInfo
 * EnvConditions: System running normally
 * CaseDescription: Get info of all stacks
 */

HWTEST_F(DumpModuleTest, dump_module_test_010, TestSize.Level2)
{
    std::string args;
    StackInfo stackInfo;
    g_abilityMs->GetAllStackInfo(stackInfo);

    int findFlag = 0;
    for (auto &stackInfo : stackInfo.missionStackInfos) {
        for (auto &missionRecord : stackInfo.missionRecords) {

            if (SearchAbilityNameFromStackInfo(G_TESTABILITY1.GetAbilityName(), missionRecord.abilityRecordInfos)) {
                ++findFlag;
            }
            if (SearchAbilityNameFromStackInfo(G_TESTABILITY2.GetAbilityName(), missionRecord.abilityRecordInfos)) {
                ++findFlag;
            }
            if (SearchAbilityNameFromStackInfo(G_TESTABILITY3.GetAbilityName(), missionRecord.abilityRecordInfos)) {
                ++findFlag;
            }
            if (SearchAbilityNameFromStackInfo(G_TESTABILITY4.GetAbilityName(), missionRecord.abilityRecordInfos)) {
                ++findFlag;
            }
            if (SearchAbilityNameFromStackInfo(G_TESTABILITY6.GetAbilityName(), missionRecord.abilityRecordInfos)) {
                ++findFlag;
            }
            if (SearchAbilityNameFromStackInfo(G_LAUNCHABILITY.GetAbilityName(), missionRecord.abilityRecordInfos)) {
                ++findFlag;
            }
        }
    }

    ASSERT_EQ(6, findFlag);
}

/*
 * Feature: Aafwk
 * Function: DumpWaittingAbilityQueue
 * SubFunction: NA
 * FunctionPoints: test AbilityStackManager DumpWaittingAbilityQueue
 * EnvConditions: System running normally
 * CaseDescription: Dump info of abilities in waiting queue
 */
HWTEST_F(DumpModuleTest, dump_module_test_011, TestSize.Level2)
{
    Want want1;
    Want want2;
    std::string expectResult("The waitting ability queue is empty.");

    std::string args("--waitting-queue");
    std::vector<std::string> result;
    std::string waitingQueueResult;

    g_abilityMs->DumpState(args, result);
    auto stackMgr = g_abilityMs->GetStackManager();
    ASSERT_TRUE(stackMgr);
    g_abilityMs->DumpWaittingAbilityQueue(waitingQueueResult);
    ASSERT_EQ(waitingQueueResult, expectResult);

    result.clear();
    waitingQueueResult.clear();

    want1.SetElement(G_TESTABILITY1);
    want2.SetElement(G_TESTABILITY5);

    int ref = g_abilityMs->StartAbility(want1);
    EXPECT_EQ(ref, 0);
    int refwant2 = g_abilityMs->StartAbility(want2);
    EXPECT_EQ(refwant2, START_ABILITY_WAITING);

    g_abilityMs->DumpState(args, result);
    ASSERT_TRUE(stackMgr);
    stackMgr->DumpWaittingAbilityQueue(waitingQueueResult);
    EXPECT_NE(std::string::npos, waitingQueueResult.find("com.ix.hiRadio"));
}

}  // namespace AAFwk
}  // namespace OHOS
