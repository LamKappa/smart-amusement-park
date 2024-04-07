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
#include <ipc_skeleton.h>
#include <iremote_object.h>
#include <iremote_stub.h>
#include "system_ability_definition.h"

#define private public
#define protected public
#include "ability_manager_service.h"
#undef private
#undef protected

#include "ability_record.h"
#include "ability_scheduler.h"
#include "datetime_ex.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "sa_mgr_client.h"
#include "string_ex.h"
#include "ability_manager_errors.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
namespace {
const std::string NAME_BUNDLE_MGR_SERVICE = "BundleMgrService";
const std::string NAME_ABILITY_MGR_SERVICE = "UTAbilityManagerService";
static std::shared_ptr<OHOS::AAFwk::AbilityManagerService> g_aams = nullptr;
static int32_t g_windowToken = 0;
AbilityRequest launcherAbilityRequest_;
AbilityRequest launcherAAbilityRequest_;
AbilityRequest otherLauncherAbilityRequest_;
AbilityRequest musicAbilityRequest_;
AbilityRequest musicTopAbilityRequest_;
AbilityRequest musicSAbilityRequest_;
AbilityRequest radioAbilityRequest_;
AbilityRequest radioTopAbilityRequest_;
}  // namespace

AbilityRequest GenerateAbilityRequest(const std::string &deviceName, const std::string &abilityName,
    const std::string &appName, const std::string &bundleName)
{
    ElementName element(deviceName, bundleName, abilityName);
    Want want;
    want.SetElement(element);

    AbilityRequest abilityRequest;
    abilityRequest.want = want;

    return abilityRequest;
}

static void WaitUntilTaskFinished()
{
    const uint32_t maxRetryTime = 1000;
    const uint32_t sleepTime = 1000;
    uint32_t count = 0;
    auto handler = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };
    if (handler->PostTask(f)) {
        while (!taskCalled.load()) {
            ++count;
            if (count >= maxRetryTime) {
                break;
            }
            usleep(sleepTime);
        }
    }
}

class TerminateAbilityTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static void init();
    void SetUp();
    void TearDown();
    bool StartAbility(
        const AbilityRequest &request, OHOS::sptr<Token> &token, OHOS::sptr<AbilityScheduler> &abilityScheduler);
    void TerminateAbility(
        const OHOS::sptr<Token> &curToken, const OHOS::sptr<Token> &preToken, const Want *resultWant = nullptr);
};

void TerminateAbilityTest::init()
{
    launcherAbilityRequest_ = GenerateAbilityRequest("device", "LauncherAbility", "launcher", "com.ix.hiworld");

    launcherAAbilityRequest_ = GenerateAbilityRequest("device", "LauncherAAbility", "launcher", "com.ix.hiworld");

    otherLauncherAbilityRequest_ =
        GenerateAbilityRequest("device", "OtherLauncherAbility", "launcher", "com.ix.hiworld");

    musicAbilityRequest_ = GenerateAbilityRequest("device", "MusicAbility", "music", "com.ix.hiMusic");

    musicTopAbilityRequest_ = GenerateAbilityRequest("device", "MusicTopAbility", "music", "com.ix.hiMusic");

    musicSAbilityRequest_ = GenerateAbilityRequest("device", "MusicSAbility", "music", "com.ix.hiMusic");

    radioAbilityRequest_ = GenerateAbilityRequest("device", "RadioAbility", "radio", "com.ix.hiRadio");

    radioTopAbilityRequest_ = GenerateAbilityRequest("device", "RadioTopAbility", "radio", "com.ix.hiRadio");
}

void TerminateAbilityTest::SetUpTestCase(void)
{}

void TerminateAbilityTest::TearDownTestCase(void)
{}

void TerminateAbilityTest::SetUp(void)
{
    OHOS::sptr<OHOS::IRemoteObject> bundleObject = new BundleMgrService();
    OHOS::DelayedSingleton<SaMgrClient>::GetInstance()->RegisterSystemAbility(
        OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);
    g_aams = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance();
    g_aams->OnStart();
    WaitUntilTaskFinished();
    init();
}

void TerminateAbilityTest::TearDown(void)
{
    g_aams->OnStop();
    OHOS::DelayedSingleton<AbilityManagerService>::DestroyInstance();
}

bool TerminateAbilityTest::StartAbility(
    const AbilityRequest &request, OHOS::sptr<Token> &token, OHOS::sptr<AbilityScheduler> &abilityScheduler)
{
    if (g_aams == nullptr) {
        GTEST_LOG_(ERROR) << "stackManager is nullptr ";
        return false;
    }
    std::shared_ptr<AbilityStackManager> stackManager = g_aams->GetStackManager();
    if (stackManager == nullptr) {
        GTEST_LOG_(ERROR) << "stackManager is nullptr ";
        return false;
    }
    bool previousExist = false;
    std::shared_ptr<AbilityRecord> last = stackManager->GetCurrentTopAbility();
    if (last != nullptr) {
        last->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
        previousExist = true;
    }

    if (g_aams->StartAbility(request.want) != 0) {
        GTEST_LOG_(ERROR) << "fail to StartAbility";
        return false;
    }

    std::shared_ptr<AbilityRecord> abilityRecord = stackManager->GetCurrentTopAbility();
    if (abilityRecord == nullptr) {
        GTEST_LOG_(ERROR) << "new ability is nullptr";
        return false;
    }
    token = abilityRecord->GetToken();
    if (token == nullptr) {
        GTEST_LOG_(ERROR) << "new token is nullptr";
        return false;
    }
    GTEST_LOG_(INFO) << "start ability: " << abilityRecord.get() << " token: " << token.GetRefPtr();
    abilityScheduler = new AbilityScheduler();
    if (g_aams->AttachAbilityThread(abilityScheduler, token) != 0) {
        GTEST_LOG_(ERROR) << "fail to AttachAbilityThread";
        return false;
    }
    g_aams->AddWindowInfo(token, ++g_windowToken);
    g_aams->AbilityTransitionDone(token, OHOS::AAFwk::AbilityState::ACTIVE);
    WaitUntilTaskFinished();
    return true;
}

void TerminateAbilityTest::TerminateAbility(
    const OHOS::sptr<Token> &curToken, const OHOS::sptr<Token> &preToken, const Want *resultWant)
{
    EXPECT_EQ(g_aams->TerminateAbility(curToken, -1, resultWant), 0);
    if (Token::GetAbilityRecordByToken(curToken)->GetAbilityState() != OHOS::AAFwk::AbilityState::INACTIVATING) {
        EXPECT_EQ(g_aams->AbilityTransitionDone(curToken, OHOS::AAFwk::AbilityState::INITIAL), 0);
        WaitUntilTaskFinished();
        return;
    }
    EXPECT_EQ(g_aams->AbilityTransitionDone(curToken, OHOS::AAFwk::AbilityState::INACTIVE), 0);
    WaitUntilTaskFinished();
    EXPECT_EQ(g_aams->AbilityTransitionDone(preToken, OHOS::AAFwk::AbilityState::ACTIVE), 0);
    WaitUntilTaskFinished();
    EXPECT_EQ(g_aams->AbilityTransitionDone(curToken, OHOS::AAFwk::AbilityState::BACKGROUND), 0);
    WaitUntilTaskFinished();
    EXPECT_EQ(g_aams->AbilityTransitionDone(curToken, OHOS::AAFwk::AbilityState::INITIAL), 0);
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityManagerService::TerminateAbility
 * FunctionPoints: param check.
 * EnvConditions: Launcher has started.
 * CaseDescription: verify TerminateAbility parameters. TerminateAbility fail if token is nullptr.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_001, TestSize.Level0)
{
    EXPECT_NE(g_aams->TerminateAbility(nullptr, -1, nullptr), 0);
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility GetAbilityFromTerminateList
 * FunctionPoints: Don't terminate ability on terminating.
 * EnvConditions: Launcher has started.
 * CaseDescription: Terminate ability on terminating should return ERR_OK.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_002, TestSize.Level0)
{
    OHOS::sptr<Token> token0;
    OHOS::sptr<AbilityScheduler> scheduler0;
    EXPECT_TRUE(StartAbility(musicAbilityRequest_, token0, scheduler0));
    OHOS::sptr<Token> token;
    OHOS::sptr<AbilityScheduler> scheduler;
    EXPECT_TRUE(StartAbility(musicTopAbilityRequest_, token, scheduler));
    WaitUntilTaskFinished();
    EXPECT_EQ(g_aams->TerminateAbility(token, -1, nullptr), 0);
    EXPECT_EQ(g_aams->TerminateAbility(token, -1, nullptr), 0);
    std::shared_ptr<AbilityRecord> abilityRecord = Token::GetAbilityRecordByToken(token0);
    EXPECT_TRUE(abilityRecord != nullptr);
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(g_aams->TerminateAbility(token0, -1, nullptr), 0);
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility
 * FunctionPoints: Can't terminate launcher root ability.
 * EnvConditions:Launcher has started.
 * CaseDescription: Can't terminate launcher root ability.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_003, TestSize.Level0)
{
    // AbilityManagerService starts one launcher ability in default.
    // Unable terminate root launcher ability.
    std::shared_ptr<AbilityStackManager> stackManager = g_aams->GetStackManager();
    EXPECT_TRUE(stackManager != nullptr);
    std::shared_ptr<AbilityRecord> launcherAbilityRecord = nullptr;
    OHOS::sptr<Token> launcherToken = nullptr;
    if (stackManager && stackManager->GetCurrentTopAbility().get()) {
        launcherAbilityRecord.reset(stackManager->GetCurrentTopAbility().get());
        launcherToken = launcherAbilityRecord->GetToken();
    }

    EXPECT_NE(g_aams->TerminateAbility(launcherToken, -1, nullptr), 0);
    WaitUntilTaskFinished();
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility TerminateAbilityLocked GetAbilityFromTerminateList
 * DispatchTerminate CompleteTerminate
 * AbilityRecord::Terminate AppScheduler::Terminate
 * FunctionPoints: Remove the only one AbilityRecord and its MissionRecord. Active caller AbilityRecord.
 * EnvConditions: start LauncherAbility and TestAbility.
 * CaseDescription: Terminate the only one AbilityRecord of MissionRecord.
 * 1. Remove AbilityRecord and MissionRecord from the stack.
 * 2. Jump to Launcher. Launcher receives result and is active.
 * 3. AbilityRecord and and MissionRecord are destroyed.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_004, TestSize.Level0)
{
    OHOS::sptr<Token> launcherToken;
    OHOS::sptr<AbilityScheduler> launcherScheduler;
    EXPECT_TRUE(StartAbility(launcherAbilityRequest_, launcherToken, launcherScheduler));
    std::shared_ptr<AbilityRecord> launcherAbilityRecord = Token::GetAbilityRecordByToken(launcherToken);
    EXPECT_TRUE(launcherAbilityRecord != nullptr);

    OHOS::sptr<Token> testToken;
    OHOS::sptr<AbilityScheduler> testScheduler;
    EXPECT_TRUE(StartAbility(musicAbilityRequest_, testToken, testScheduler));
    EXPECT_TRUE(testToken != nullptr);
    std::shared_ptr<AbilityRecord> testAbilityRecord = Token::GetAbilityRecordByToken(testToken);
    EXPECT_TRUE(testAbilityRecord != nullptr);
    std::shared_ptr<MissionRecord> missionRecord = testAbilityRecord->GetMissionRecord();
    EXPECT_TRUE(missionRecord != nullptr);
    WaitUntilTaskFinished();
    Want want;
    std::string key("key");
    int resultValue = 4;
    want.SetParam(key, resultValue);
    TerminateAbility(testToken, launcherToken, &want);

    // verify result
    std::shared_ptr<AbilityResult> serverResult = launcherAbilityRecord->GetResult();
    EXPECT_TRUE(serverResult == nullptr);
    AbilityResult result = launcherScheduler->GetResult();
    EXPECT_NE(result.resultWant_.GetIntParam(key, 0), resultValue);
    // last launcherAbilityRecord
    EXPECT_EQ(g_aams->TerminateAbility(launcherToken, -1, nullptr), TERMINATE_LAUNCHER_DENIED);
    WaitUntilTaskFinished();
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility TerminateAbilityLocked GetAbilityFromTerminateList
 * DispatchTerminate CompleteTerminate
 * AbilityRecord::Terminate AppScheduler::Terminate
 * FunctionPoints: TerminateAbility
 * EnvConditions:
 * CaseDescription: Terminate the only one non-active AbilityRecord of MissionRecord in default stack.
 * 1. destroy AbilityRecord
 * 2. destroy MissionRecord
 * 3. send result to caller
 * 4. caller is in background
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_005, TestSize.Level0)
{
    OHOS::sptr<Token> launcherToken;
    OHOS::sptr<AbilityScheduler> launcherScheduler;
    EXPECT_TRUE(StartAbility(launcherAbilityRequest_, launcherToken, launcherScheduler));
    std::shared_ptr<AbilityRecord> launcherAbilityRecord = Token::GetAbilityRecordByToken(launcherToken);
    EXPECT_TRUE(launcherAbilityRecord != nullptr);

    OHOS::sptr<Token> testToken;
    OHOS::sptr<AbilityScheduler> testScheduler;
    EXPECT_TRUE(StartAbility(musicAbilityRequest_, testToken, testScheduler));
    WaitUntilTaskFinished();
    EXPECT_TRUE(testToken != nullptr);
    std::shared_ptr<AbilityRecord> testAbilityRecord = Token::GetAbilityRecordByToken(testToken);
    EXPECT_TRUE(testAbilityRecord != nullptr);
    std::shared_ptr<MissionRecord> missionRecord = testAbilityRecord->GetMissionRecord();
    EXPECT_TRUE(missionRecord != nullptr);

    testAbilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::BACKGROUND);
    Want want;
    std::string key("key");
    int resultValue = 5;
    want.SetParam(key, resultValue);
    TerminateAbility(testToken, launcherToken, &want);

    EXPECT_NE(launcherAbilityRecord->GetAbilityState(), OHOS::AAFwk::AbilityState::BACKGROUND);
    // clear launcherAbilityRecord
    EXPECT_EQ(g_aams->TerminateAbility(launcherToken, -1, nullptr), TERMINATE_LAUNCHER_DENIED);
    WaitUntilTaskFinished();
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility TerminateAbilityLocked GetAbilityFromTerminateList
 * AbilityRecord::Terminate AppScheduler::Terminate
 * FunctionPoints: Remove AbilityRecord. Active caller AbilityRecord.
 * EnvConditions: start TestAbilityA and TestAbilityB.
 * CaseDescription: defaultStack has more than 2 abilities. Terminate top ACTIVE ability.
 * 1. Remove AbilityRecord from the stack.
 * 2. Jump to caller. caller receives result and is active.
 * 3. AbilityRecord is destroyed.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_006, TestSize.Level0)
{
    OHOS::sptr<Token> tokenA;
    OHOS::sptr<AbilityScheduler> schedulerA;
    EXPECT_TRUE(StartAbility(musicAbilityRequest_, tokenA, schedulerA));
    std::shared_ptr<AbilityRecord> testAbilityRecordA = Token::GetAbilityRecordByToken(tokenA);
    EXPECT_TRUE(testAbilityRecordA != nullptr);

    OHOS::sptr<Token> tokenB;
    OHOS::sptr<AbilityScheduler> schedulerB;
    EXPECT_TRUE(StartAbility(musicTopAbilityRequest_, tokenB, schedulerB));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> testAbilityRecordB = Token::GetAbilityRecordByToken(tokenB);
    EXPECT_TRUE(testAbilityRecordB != nullptr);
    std::shared_ptr<MissionRecord> missionRecord = testAbilityRecordB->GetMissionRecord();
    EXPECT_TRUE(missionRecord != nullptr);

    Want want;
    std::string key("key");
    int resultValue = 0;
    want.SetParam(key, resultValue);
    TerminateAbility(tokenB, tokenA, &want);
    // verify result
    std::shared_ptr<AbilityResult> serverResult = testAbilityRecordA->GetResult();
    EXPECT_TRUE(serverResult == nullptr);
    AbilityResult result = schedulerA->GetResult();
    EXPECT_EQ(result.resultWant_.GetIntParam(key, 0), resultValue);
    // caller is active
    EXPECT_EQ(testAbilityRecordA->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVE);
    // clear launcherAbilityRecord
    EXPECT_EQ(g_aams->TerminateAbility(tokenA, -1, nullptr), 0);
    WaitUntilTaskFinished();
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility TerminateAbilityLocked
 * DispatchTerminate CompleteTerminate
 * AbilityRecord::Terminate AppScheduler::Terminate
 * FunctionPoints: Remove AbilityRecord. Active caller AbilityRecord.
 * EnvConditions: start LauncherAbilityA and LauncherAbilityB.
 * CaseDescription: launcherStack has more than 2 abilities. Terminate top ACTIVE ability.
 * 1. Remove AbilityRecord from the stack.
 * 2. Jump to caller. caller receives result and is active.
 * 3. AbilityRecord is destroyed.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_007, TestSize.Level0)
{
    OHOS::sptr<Token> launcherTokenA;
    OHOS::sptr<AbilityScheduler> launcherSchedulerA;
    EXPECT_TRUE(StartAbility(launcherAbilityRequest_, launcherTokenA, launcherSchedulerA));
    std::shared_ptr<AbilityRecord> launcherAbilityRecordA = Token::GetAbilityRecordByToken(launcherTokenA);
    EXPECT_TRUE(launcherAbilityRecordA != nullptr);

    OHOS::sptr<Token> launcherTokenB;
    OHOS::sptr<AbilityScheduler> launcherSchedulerB;
    EXPECT_TRUE(StartAbility(otherLauncherAbilityRequest_, launcherTokenB, launcherSchedulerB));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> launcherAbilityRecordB = Token::GetAbilityRecordByToken(launcherTokenB);
    EXPECT_TRUE(launcherAbilityRecordB != nullptr);
    std::shared_ptr<MissionRecord> missionRecord = launcherAbilityRecordB->GetMissionRecord();
    EXPECT_TRUE(missionRecord != nullptr);

    Want want;
    std::string key("key");
    int resultValue = 7;
    want.SetParam(key, resultValue);
    TerminateAbility(launcherTokenB, launcherTokenA, &want);
    // verify result
    std::shared_ptr<AbilityResult> serverResult = launcherAbilityRecordA->GetResult();
    EXPECT_TRUE(serverResult == nullptr);
    AbilityResult result = launcherSchedulerA->GetResult();
    EXPECT_NE(result.resultWant_.GetIntParam(key, 0), resultValue);
    // caller is active
    EXPECT_EQ(launcherAbilityRecordA->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVE);
    // clear launcherAbilityRecord
    EXPECT_EQ(g_aams->TerminateAbility(launcherTokenA, -1, nullptr), TERMINATE_LAUNCHER_DENIED);
    WaitUntilTaskFinished();
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility TerminateAbilityLocked
 * DispatchTerminate CompleteTerminate
 * AbilityRecord::Terminate AppScheduler::Terminate
 * FunctionPoints: Terminate top non-active ability.
 * EnvConditions: start TestAbilityA and TestAbilityB.
 * CaseDescription: default stack has more than 2 abilities. Terminate top non-active ability.
 * 1. remove AbilityRecord from the stack.
 * 2. caller receives result.
 * 3. caller is background.
 * 4. AbilityRecord is destroyed.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_008, TestSize.Level0)
{
    OHOS::sptr<Token> tokenA;
    OHOS::sptr<AbilityScheduler> schedulerA;
    EXPECT_TRUE(StartAbility(musicAbilityRequest_, tokenA, schedulerA));
    std::shared_ptr<AbilityRecord> testAbilityRecordA = Token::GetAbilityRecordByToken(tokenA);
    EXPECT_TRUE(testAbilityRecordA != nullptr);

    OHOS::sptr<Token> tokenB;
    OHOS::sptr<AbilityScheduler> schedulerB;
    EXPECT_TRUE(StartAbility(musicTopAbilityRequest_, tokenB, schedulerB));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> testAbilityRecordB = Token::GetAbilityRecordByToken(tokenB);
    EXPECT_TRUE(testAbilityRecordB != nullptr);
    std::shared_ptr<MissionRecord> missionRecord = testAbilityRecordB->GetMissionRecord();
    EXPECT_TRUE(missionRecord != nullptr);

    testAbilityRecordB->SetAbilityState(OHOS::AAFwk::AbilityState::BACKGROUND);
    TerminateAbility(tokenB, tokenA, nullptr);
    // caller is active
    EXPECT_NE(testAbilityRecordA->GetAbilityState(), OHOS::AAFwk::AbilityState::BACKGROUND);
    // clear launcherAbilityRecord
    EXPECT_EQ(g_aams->TerminateAbility(tokenA, -1, nullptr), 0);
    WaitUntilTaskFinished();
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility TerminateAbilityLocked
 * DispatchTerminate CompleteTerminate
 * AbilityRecord::Terminate AppScheduler::Terminate
 * FunctionPoints: Terminate top non-active ability.
 * EnvConditions: start LauncherAbilityA and LauncherAbilityB.
 * CaseDescription: launcherStack has more than 2 abilities. Terminate top non-active ability.
 * 1. remove AbilityRecord from the stack.
 * 2. caller receives result.
 * 3. caller is background.
 * 4. AbilityRecord is destroyed.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_009, TestSize.Level0)
{
    OHOS::sptr<Token> launcherTokenA;
    OHOS::sptr<AbilityScheduler> launcherSchedulerA;
    EXPECT_TRUE(StartAbility(launcherAbilityRequest_, launcherTokenA, launcherSchedulerA));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> launcherAbilityRecordA = Token::GetAbilityRecordByToken(launcherTokenA);
    EXPECT_TRUE(launcherAbilityRecordA != nullptr);

    OHOS::sptr<Token> launcherTokenB;
    OHOS::sptr<AbilityScheduler> launcherSchedulerB;
    EXPECT_TRUE(StartAbility(otherLauncherAbilityRequest_, launcherTokenB, launcherSchedulerB));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> launcherAbilityRecordB = Token::GetAbilityRecordByToken(launcherTokenB);
    EXPECT_TRUE(launcherAbilityRecordB != nullptr);
    std::shared_ptr<MissionRecord> missionRecord = launcherAbilityRecordB->GetMissionRecord();
    EXPECT_TRUE(missionRecord != nullptr);

    launcherAbilityRecordB->SetAbilityState(OHOS::AAFwk::AbilityState::BACKGROUND);
    TerminateAbility(launcherTokenB, launcherTokenA, nullptr);
    // verify result. Has no result.
    std::shared_ptr<AbilityResult> result = launcherAbilityRecordA->GetResult();
    EXPECT_TRUE(result == nullptr);
    // caller is active
    EXPECT_NE(launcherAbilityRecordA->GetAbilityState(), OHOS::AAFwk::AbilityState::BACKGROUND);
    // clear launcherAbilityRecord
    EXPECT_EQ(g_aams->TerminateAbility(launcherTokenA, -1, nullptr), TERMINATE_LAUNCHER_DENIED);
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility TerminateAbilityLocked
 * DispatchTerminate CompleteTerminate
 * AbilityRecord::Terminate AppScheduler::Terminate
 * FunctionPoints: Terminate non-top ability.
 * EnvConditions: start TestAbilityA TestAbilityB TestAbilityC.
 * CaseDescription: default stack has more than 2 abilities. Terminate non-top ability.
 * 1. Remove AbilityRecord from the stack.
 * 2. caller receives result.
 * 3. caller is in background.
 * 4. AbilityRecord is destroyed.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_010, TestSize.Level0)
{
    OHOS::sptr<Token> tokenA;
    OHOS::sptr<AbilityScheduler> schedulerA;
    EXPECT_TRUE(StartAbility(musicAbilityRequest_, tokenA, schedulerA));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> testAbilityRecordA = Token::GetAbilityRecordByToken(tokenA);
    EXPECT_TRUE(testAbilityRecordA != nullptr);

    OHOS::sptr<Token> tokenB;
    OHOS::sptr<AbilityScheduler> schedulerB;
    EXPECT_TRUE(StartAbility(musicTopAbilityRequest_, tokenB, schedulerB));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> testAbilityRecordB = Token::GetAbilityRecordByToken(tokenB);
    EXPECT_TRUE(testAbilityRecordB != nullptr);

    OHOS::sptr<Token> tokenC;
    OHOS::sptr<AbilityScheduler> schedulerC;
    EXPECT_TRUE(StartAbility(musicSAbilityRequest_, tokenC, schedulerC));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> testAbilityRecordC = Token::GetAbilityRecordByToken(tokenC);
    EXPECT_TRUE(testAbilityRecordC != nullptr);

    Want want;
    std::string key("key");
    int resultValue = 10;
    want.SetParam(key, resultValue);
    EXPECT_EQ(g_aams->TerminateAbility(tokenB, -1, &musicAbilityRequest_.want), 0);
    EXPECT_NE(g_aams->AbilityTransitionDone(tokenA, OHOS::AAFwk::AbilityState::ACTIVE), 0);
    // clear testAbilityRecordC testAbilityRecordA
    EXPECT_EQ(g_aams->TerminateAbility(tokenC, -1, nullptr), 0);
    EXPECT_EQ(g_aams->TerminateAbility(tokenA, -1, nullptr), 0);
    WaitUntilTaskFinished();
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility TerminateAbilityLocked
 * DispatchTerminate CompleteTerminate
 * AbilityRecord::Terminate AppScheduler::Terminate
 * FunctionPoints: Terminate non-top ability.
 * EnvConditions: start LauncherAbilityA and LauncherAbilityB LauncherAbilityC.
 * CaseDescription: launcherStack has more than 2 abilities. Terminate non-top ability.
 * 1. Remove AbilityRecord from the stack.
 * 2. caller receives result.
 * 3. caller is background.
 * 4. AbilityRecord is destroyed.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_011, TestSize.Level0)
{
    OHOS::sptr<Token> launcherTokenA;
    OHOS::sptr<AbilityScheduler> schedulerA;
    EXPECT_TRUE(StartAbility(launcherAbilityRequest_, launcherTokenA, schedulerA));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> launcherAbilityRecordA = Token::GetAbilityRecordByToken(launcherTokenA);
    EXPECT_TRUE(launcherAbilityRecordA != nullptr);

    OHOS::sptr<Token> launcherTokenB;
    OHOS::sptr<AbilityScheduler> schedulerB;
    EXPECT_TRUE(StartAbility(otherLauncherAbilityRequest_, launcherTokenB, schedulerB));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> launcherAbilityRecordB = Token::GetAbilityRecordByToken(launcherTokenB);
    EXPECT_TRUE(launcherAbilityRecordB != nullptr);

    OHOS::sptr<Token> launcherTokenC;
    OHOS::sptr<AbilityScheduler> schedulerC;
    EXPECT_TRUE(StartAbility(launcherAAbilityRequest_, launcherTokenC, schedulerC));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> launcherAbilityRecordC = Token::GetAbilityRecordByToken(launcherTokenC);
    EXPECT_TRUE(launcherAbilityRecordC != nullptr);

    Want want;
    std::string key("key");
    int resultValue = 11;
    want.SetParam(key, resultValue);
    EXPECT_EQ(g_aams->TerminateAbility(launcherTokenB, -1, &launcherAbilityRequest_.want), 0);
    EXPECT_NE(g_aams->AbilityTransitionDone(launcherTokenA, OHOS::AAFwk::AbilityState::ACTIVE), 0);
    EXPECT_EQ(g_aams->TerminateAbility(launcherTokenC, -1, &want), 0);
    EXPECT_EQ(g_aams->TerminateAbility(launcherTokenA, -1, &want), TERMINATE_LAUNCHER_DENIED);
    WaitUntilTaskFinished();
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility TerminateAbilityLocked
 * DispatchTerminate CompleteTerminate
 * AbilityRecord::Terminate AppScheduler::Terminate
 * FunctionPoints: Terminate timeout handle.
 * EnvConditions: start TestAbilityA and TestAbilityB.
 * CaseDescription: TestAbilityA start TestAbilityB. TestAbilityB terminate timeout.
 * 1. Remove AbilityRecordB from the stack.
 * 2. Caller TestAbilityA receives.
 * 3. Caller is active.
 * 4. AbilityRecordB is destroyed.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_012, TestSize.Level2)
{
    OHOS::sptr<Token> tokenA;
    OHOS::sptr<AbilityScheduler> schedulerA;
    EXPECT_TRUE(StartAbility(musicAbilityRequest_, tokenA, schedulerA));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> testAbilityRecordA = Token::GetAbilityRecordByToken(tokenA);
    EXPECT_TRUE(testAbilityRecordA != nullptr);

    OHOS::sptr<Token> tokenB;
    OHOS::sptr<AbilityScheduler> schedulerB;
    EXPECT_TRUE(StartAbility(musicTopAbilityRequest_, tokenB, schedulerB));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> testAbilityRecordB = Token::GetAbilityRecordByToken(tokenB);
    EXPECT_TRUE(testAbilityRecordB != nullptr);

    Want resultWant;
    std::string key("key");
    int resultValue = 13;
    resultWant.SetParam(key, resultValue);
    EXPECT_EQ(g_aams->TerminateAbility(tokenB, -1, &resultWant), 0);
    EXPECT_EQ(g_aams->AbilityTransitionDone(tokenB, OHOS::AAFwk::AbilityState::INACTIVE), 0);
    WaitUntilTaskFinished();
    EXPECT_EQ(g_aams->AbilityTransitionDone(tokenA, OHOS::AAFwk::AbilityState::ACTIVE), 0);
    WaitUntilTaskFinished();
    EXPECT_EQ(g_aams->AbilityTransitionDone(tokenB, OHOS::AAFwk::AbilityState::BACKGROUND), 0);
    WaitUntilTaskFinished();

    // AbilityTransitionDone TERMINATE timeout
    sleep(AbilityManagerService::TERMINATE_TIMEOUT / OHOS::MICROSEC_TO_NANOSEC + 1);
    // verify result
    std::shared_ptr<AbilityResult> serverResult = testAbilityRecordB->GetResult();
    EXPECT_TRUE(serverResult == nullptr);
    AbilityResult result = schedulerA->GetResult();
    EXPECT_NE(result.resultWant_.GetIntParam(key, 0), resultValue);
    // clear testAbilityRecordC testAbilityRecordA
    EXPECT_EQ(g_aams->TerminateAbility(tokenA, -1, nullptr), 0);
    WaitUntilTaskFinished();
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility TerminateAbilityLocked
 * DispatchTerminate CompleteTerminate
 * AbilityRecord::Terminate AppScheduler::Terminate
 * FunctionPoints: Background timeout handle.
 * EnvConditions: start TestAbilityA and TestAbilityB.
 * CaseDescription: TestAbilityA start TestAbilityB. TestAbilityB Background timeout.
 * 1. Remove AbilityRecordB from the stack.
 * 2. Caller TestAbilityA receives.
 * 3. Caller is active.
 * 4. AbilityRecordB is destroyed.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_013, TestSize.Level2)
{
    OHOS::sptr<Token> tokenA;
    OHOS::sptr<AbilityScheduler> schedulerA;
    EXPECT_TRUE(StartAbility(musicAbilityRequest_, tokenA, schedulerA));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> testAbilityRecordA = Token::GetAbilityRecordByToken(tokenA);
    EXPECT_TRUE(testAbilityRecordA != nullptr);

    OHOS::sptr<Token> tokenB;
    OHOS::sptr<AbilityScheduler> schedulerB;
    EXPECT_TRUE(StartAbility(musicTopAbilityRequest_, tokenB, schedulerB));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> testAbilityRecordB = Token::GetAbilityRecordByToken(tokenB);
    EXPECT_TRUE(testAbilityRecordB != nullptr);

    Want resultWant;
    std::string key("key");
    int resultValue = 14;
    resultWant.SetParam(key, resultValue);
    EXPECT_EQ(g_aams->TerminateAbility(tokenB, -1, &resultWant), 0);
    EXPECT_EQ(g_aams->AbilityTransitionDone(tokenB, OHOS::AAFwk::AbilityState::INACTIVE), 0);
    WaitUntilTaskFinished();
    EXPECT_EQ(g_aams->AbilityTransitionDone(tokenA, OHOS::AAFwk::AbilityState::ACTIVE), 0);

    // AbilityTransitionDone BACKGROUND timeout
    sleep(AbilityManagerService::BACKGROUND_TIMEOUT / OHOS::MICROSEC_TO_NANOSEC + 1);
    EXPECT_EQ(g_aams->AbilityTransitionDone(tokenB, OHOS::AAFwk::AbilityState::INITIAL), 0);
    WaitUntilTaskFinished();
    // verify result
    std::shared_ptr<AbilityResult> serverResult = testAbilityRecordB->GetResult();
    EXPECT_TRUE(serverResult == nullptr);
    AbilityResult result = schedulerA->GetResult();
    EXPECT_NE(result.resultWant_.GetIntParam(key, 0), resultValue);
    // clear testAbilityRecordC testAbilityRecordA
    EXPECT_EQ(g_aams->TerminateAbility(tokenA, -1, nullptr), 0);
    WaitUntilTaskFinished();
}

/*
 * Feature: TerminateAbility
 * Function: TerminateAbility
 * SubFunction: AbilityStackManager::TerminateAbility TerminateAbilityLocked
 * DispatchTerminate CompleteTerminate
 * AbilityRecord::Terminate AppScheduler::Terminate
 * FunctionPoints: Terminate ability on inactive.
 * EnvConditions: start TestAbilityA and TestAbilityB.
 * CaseDescription:  TestAbilityA starts TestAbilityB. Terminate TestAbilityA when it's on inactive.
 * 1. Remove AbilityRecordA from the stack.
 * 2. caller TestAbilityA receives result.
 * 3. AbilityRecordA is destroyed.
 */
HWTEST_F(TerminateAbilityTest, AAFWK_AbilityMS_TerminateAbility_014, TestSize.Level0)
{
    OHOS::sptr<Token> token;
    OHOS::sptr<AbilityScheduler> scheduler;
    EXPECT_TRUE(StartAbility(musicAbilityRequest_, token, scheduler));
    std::shared_ptr<AbilityRecord> testAbilityRecord = Token::GetAbilityRecordByToken(token);
    EXPECT_TRUE(testAbilityRecord != nullptr);

    OHOS::sptr<Token> tokenA;
    OHOS::sptr<AbilityScheduler> schedulerA;
    EXPECT_TRUE(StartAbility(musicTopAbilityRequest_, tokenA, schedulerA));
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityRecord> testAbilityRecordA = Token::GetAbilityRecordByToken(tokenA);
    EXPECT_TRUE(testAbilityRecordA != nullptr);

    EXPECT_EQ(g_aams->StartAbility(musicSAbilityRequest_.want), 0);
    WaitUntilTaskFinished();
    std::shared_ptr<AbilityStackManager> stackManager = g_aams->GetStackManager();
    EXPECT_TRUE(stackManager != nullptr);
    std::shared_ptr<AbilityRecord> testAbilityRecordB = stackManager->GetCurrentTopAbility();
    OHOS::sptr<Token> tokenB = testAbilityRecordB->GetToken();
    g_aams->AddWindowInfo(tokenB, ++g_windowToken);
    EXPECT_EQ(g_aams->AttachAbilityThread(new AbilityScheduler(), tokenB), 0);
    EXPECT_EQ(g_aams->AbilityTransitionDone(tokenA, OHOS::AAFwk::AbilityState::INACTIVE), 0);

    EXPECT_EQ(g_aams->TerminateAbility(tokenA, -1, &(musicSAbilityRequest_.want)), 0);
    EXPECT_EQ(g_aams->AbilityTransitionDone(tokenB, OHOS::AAFwk::AbilityState::ACTIVE), 0);
    WaitUntilTaskFinished();
    EXPECT_EQ(g_aams->AbilityTransitionDone(tokenA, OHOS::AAFwk::AbilityState::BACKGROUND), 0);
    WaitUntilTaskFinished();
    EXPECT_EQ(g_aams->AbilityTransitionDone(tokenA, OHOS::AAFwk::AbilityState::INITIAL), 0);
    WaitUntilTaskFinished();
    // clear testAbilityRecordB testAbilityRecord
    EXPECT_EQ(g_aams->TerminateAbility(tokenB, -1, nullptr), 0);
    EXPECT_EQ(g_aams->TerminateAbility(token, -1, nullptr), 0);
}
}  // namespace AAFwk
}  // namespace OHOS