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
#define protected public
#include "ability_manager_service.h"
#include "ability_event_handler.h"
#undef private
#undef protected

#include "system_ability_definition.h"
#include "ability_manager_errors.h"
#include "ability_scheduler.h"
#include "bundlemgr/mock_bundle_manager.h"
#include "sa_mgr_client.h"
#include "mock_ability_connect_callback.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"

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

#define SLEEP(milli) std::this_thread::sleep_for(std::chrono::seconds(milli))

namespace {
const std::string NAME_BUNDLE_MGR_SERVICE = "BundleMgrService";
static int32_t g_windowToken = 0;
}  // namespace

class AbilityManagerServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    int StartAbility(const Want &want);
    static constexpr int TEST_WAIT_TIME = 100000;

public:
    std::shared_ptr<AbilityManagerService> abilityMs_;
    AbilityRequest abilityRequest_;
    std::shared_ptr<AbilityRecord> abilityRecord_;
};

int AbilityManagerServiceTest::StartAbility(const Want &want)
{
    int ref = -1;
    auto topAbility = abilityMs_->GetStackManager()->GetCurrentTopAbility();
    if (topAbility) {
        topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }
    ref = abilityMs_->StartAbility(want);
    WaitUntilTaskFinished();
    return ref;
}

void AbilityManagerServiceTest::SetUpTestCase()
{}

void AbilityManagerServiceTest::TearDownTestCase()
{}

void AbilityManagerServiceTest::SetUp()
{
    auto bundleObject = new BundleMgrService();
    OHOS::DelayedSingleton<SaMgrClient>::GetInstance()->RegisterSystemAbility(
        OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);

    abilityMs_ = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance();
    abilityMs_->OnStart();
    WaitUntilTaskFinished();
    if (abilityRecord_ == nullptr) {
        abilityRequest_.appInfo.bundleName = "data.client.bundle";
        abilityRequest_.abilityInfo.name = "ClientAbility";
        abilityRequest_.abilityInfo.type = AbilityType::DATA;
        abilityRecord_ = AbilityRecord::CreateAbilityRecord(abilityRequest_);
    }
}

void AbilityManagerServiceTest::TearDown()
{
    abilityMs_->OnStop();
    OHOS::DelayedSingleton<AbilityManagerService>::DestroyInstance();
    if (abilityRecord_) {
        abilityRecord_.reset();
    }
    abilityMs_.reset();
}

/*
 * Feature: AbilityManagerService
 * Function: StartAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService StartAbility
 * EnvConditions: NA
 * CaseDescription: Verify that the result of StartAbility is failed if the param of StartAbility is illegal.
 */
HWTEST_F(AbilityManagerServiceTest, Interface_001, TestSize.Level0)
{
    Want want;
    want.AddEntity(Want::ENTITY_HOME);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(RESOLVE_ABILITY_ERR, result);
}

/*
 * Feature: AbilityManagerService
 * Function: StartAbility QueryServiceState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService StartAbility
 * EnvConditions: NA
 * CaseDescription: Verify that the result of StartAbility is successful if the param of StartAbility is normal.
 */
HWTEST_F(AbilityManagerServiceTest, Interface_002, TestSize.Level0)
{
    Want want;
    ElementName element("device", "com.ix.hiMusic", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, abilityMs_->QueryServiceState());
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    EXPECT_EQ(topAbility->GetAbilityInfo().bundleName, "com.ix.hiMusic");
}

/*
 * Feature: AbilityManagerService
 * Function: StartAbility QueryServiceState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService StartAbility
 * EnvConditions: NA
 * CaseDescription: Verify the singleton startup mode, start multiple times, and do not recreate
 */
HWTEST_F(AbilityManagerServiceTest, Interface_003, TestSize.Level0)
{
    Want want;
    ElementName element("device", "com.ix.hiMusic", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    EXPECT_EQ(topAbility->GetAbilityInfo().bundleName, "com.ix.hiMusic");

    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    Want want1;
    ElementName element1("device", "com.ix.hiMusic", "MusicSAbility");
    want1.SetElement(element1);
    auto result1 = StartAbility(want1);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);
    stackManager = abilityMs_->GetStackManager();
    topAbility = stackManager->GetCurrentTopAbility();
    auto abilityId = topAbility->GetRecordId();

    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    Want want2;
    ElementName element2("device", "com.ix.hiMusic", "MusicSAbility");
    want2.SetElement(element2);
    auto result2 = StartAbility(want2);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result2);
    stackManager = abilityMs_->GetStackManager();
    topAbility = stackManager->GetCurrentTopAbility();

    EXPECT_EQ(topAbility->GetRecordId(), abilityId);
}

/*
 * Feature: AbilityManagerService
 * Function: StartAbility QueryServiceState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService StartAbility
 * EnvConditions: NA
 * CaseDescription: Verify the standard startup mode, start several times, and create a new record
 */
HWTEST_F(AbilityManagerServiceTest, Interface_004, TestSize.Level0)
{

    Want want;
    ElementName element("device", "com.ix.hiMusic", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    auto id = topAbility->GetRecordId();
    EXPECT_EQ(topAbility->GetAbilityInfo().bundleName, "com.ix.hiMusic");

    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    Want want1;
    ElementName element1("device", "com.ix.hiMusic", "MusicAbility");
    want1.SetElement(element1);
    auto result1 = StartAbility(want1);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);
    stackManager = abilityMs_->GetStackManager();
    topAbility = stackManager->GetCurrentTopAbility();
    auto abilityId = topAbility->GetRecordId();

    EXPECT_NE(abilityId, id);
}

/*
 * Feature: AbilityManagerService
 * Function: StartAbility QueryServiceState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService StartAbility
 * EnvConditions: NA
 * CaseDescription: Verify the singletop startup mode, start several times, and create a new record
 */
HWTEST_F(AbilityManagerServiceTest, Interface_005, TestSize.Level0)
{
    Want want;
    ElementName element("device", "com.ix.hiMusic", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    EXPECT_EQ(topAbility->GetAbilityInfo().bundleName, "com.ix.hiMusic");

    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    Want want1;
    ElementName element1("device", "com.ix.hiMusic", "MusicTopAbility");
    want1.SetElement(element1);
    auto result1 = StartAbility(want1);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);
    stackManager = abilityMs_->GetStackManager();
    topAbility = stackManager->GetCurrentTopAbility();
    auto abilityId = topAbility->GetRecordId();

    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    Want want2;
    ElementName element2("device", "com.ix.hiMusic", "MusicTopAbility");
    want2.SetElement(element2);
    auto result2 = StartAbility(want2);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result2);
    stackManager = abilityMs_->GetStackManager();
    topAbility = stackManager->GetCurrentTopAbility();

    EXPECT_EQ(topAbility->GetRecordId(), abilityId);
}

/*
 * Feature: AbilityManagerService
 * Function: StartAbility QueryServiceState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService StartAbility
 * EnvConditions: NA
 * CaseDescription: Verify that service ability started successfully
 */
HWTEST_F(AbilityManagerServiceTest, Interface_006, TestSize.Level0)
{
    Want want;
    ElementName element("device", "com.ix.musicService", "MusicService");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto serviceMap = abilityMs_->connectManager_->GetServiceMap();
    EXPECT_EQ(1, static_cast<int>(serviceMap.size()));
}

/*
 * Feature: AbilityManagerService
 * Function: StartAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService StartAbility
 * EnvConditions: NA
 * CaseDescription: Top ability is not active, so enqueue ability for waiting. StartAbility failed
 */
HWTEST_F(AbilityManagerServiceTest, Interface_007, TestSize.Level0)
{

    Want want;
    ElementName element("device", "com.ix.hiMusic", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();

    Want want1;
    ElementName element1("device", "com.ix.hiMusic", "MusicAbility");
    want1.SetElement(element1);
    auto result1 = abilityMs_->StartAbility(want1);
    WaitUntilTaskFinished();
    EXPECT_EQ(START_ABILITY_WAITING, result1);
}

/*
 * Feature: AbilityManagerService
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService TerminateAbility
 * EnvConditions: NA
 * CaseDescription: Failed to verify terminate ability
 */
HWTEST_F(AbilityManagerServiceTest, Interface_008, TestSize.Level0)
{

    Want want;
    ElementName element("device", "com.ix.hiData", "DataAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_INVALID_VALUE, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    sptr<IRemoteObject> token = nullptr;
    if (topAbility) {
        token = topAbility->GetToken();
    }

    sptr<IRemoteObject> nullToekn = nullptr;
    auto result1 = abilityMs_->TerminateAbility(nullToekn, -1, &want);
    WaitUntilTaskFinished();
    EXPECT_EQ(ERR_INVALID_VALUE, result1);

    std::shared_ptr<AbilityRecord> ability = nullptr;
    sptr<IRemoteObject> toekn1 = new Token(ability);
    auto result2 = abilityMs_->TerminateAbility(toekn1, -1, &want);
    WaitUntilTaskFinished();
    EXPECT_EQ(ERR_INVALID_VALUE, result2);

    auto result3 = abilityMs_->TerminateAbility(token, -1, &want);
    WaitUntilTaskFinished();
    EXPECT_EQ(ERR_INVALID_VALUE, result3);
}

/*
 * Feature: AbilityManagerService
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService TerminateAbility
 * EnvConditions: NA
 * CaseDescription: Verification service terminate ability failure
 */
HWTEST_F(AbilityManagerServiceTest, Interface_009, TestSize.Level0)
{
    Want want;
    ElementName element("device", "com.ix.musicService", "MusicService");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);

    auto serviceMap = abilityMs_->connectManager_->GetServiceMap();
    EXPECT_EQ(1, static_cast<int>(serviceMap.size()));
    for (auto &it : serviceMap) {
        EXPECT_EQ(it.first, element.GetURI());
    }
    auto service = serviceMap.at(element.GetURI());
    service->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    auto result1 = abilityMs_->TerminateAbility(service->GetToken(), -1, &want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);
}

/*
 * Feature: AbilityManagerService
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService TerminateAbility
 * EnvConditions: NA
 * CaseDescription: Verification service terminate ability failure
 */
HWTEST_F(AbilityManagerServiceTest, Interface_010, TestSize.Level0)
{
    Want want;
    ElementName element("device", "com.ix.hiMusic", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    EXPECT_EQ(topAbility->GetAbilityInfo().bundleName, "com.ix.hiMusic");

    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    Want want1;
    ElementName element1("device", "com.ix.hiMusic", "MusicSAbility");
    want1.SetElement(element1);
    auto result1 = StartAbility(want1);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);
    stackManager = abilityMs_->GetStackManager();
    topAbility = stackManager->GetCurrentTopAbility();

    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    Want want2;
    ElementName element2("device", "com.ix.hiRadio", "RadioAbility");
    want2.SetElement(element2);
    auto result2 = StartAbility(want2);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result2);
    stackManager = abilityMs_->GetStackManager();
    topAbility = stackManager->GetCurrentTopAbility();

    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    auto result3 = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result3);
    topAbility = stackManager->GetCurrentTopAbility();

    auto result4 = abilityMs_->TerminateAbility(topAbility->GetToken(), -1, &want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result4);

    topAbility = stackManager->GetCurrentTopAbility();
    EXPECT_EQ(topAbility->GetAbilityInfo().bundleName, "com.ix.hiRadio");

    auto result5 = abilityMs_->TerminateAbility(topAbility->GetToken(), -1, &want2);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result5);

    topAbility = stackManager->GetCurrentTopAbility();
    EXPECT_EQ(topAbility->GetAbilityInfo().bundleName, "com.ix.hiMusic");
}

/*
 * Feature: AbilityManagerService
 * Function: AttachAbilityThread
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService StartAbility
 * EnvConditions: NA
 * CaseDescription: Attachabilitythread failed due to empty token or scheduler
 */
HWTEST_F(AbilityManagerServiceTest, Interface_011, TestSize.Level0)
{

    Want wantLuncher;
    ElementName elementLun("device", "com.ix.hiworld", "LauncherAbility");
    wantLuncher.SetElement(elementLun);
    abilityMs_->StartAbility(wantLuncher);
    WaitUntilTaskFinished();
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    EXPECT_EQ(topAbility->GetAbilityInfo().bundleName, "com.ix.hiworld");

    OHOS::sptr<IAbilityScheduler> scheduler = new AbilityScheduler();
    OHOS::sptr<IAbilityScheduler> nullScheduler = nullptr;
    EXPECT_EQ(abilityMs_->AttachAbilityThread(nullScheduler, topAbility->GetToken()), OHOS::ERR_INVALID_VALUE);

    std::shared_ptr<AbilityRecord> record = nullptr;
    OHOS::sptr<Token> nullToken = new Token(record);
    EXPECT_EQ(abilityMs_->AttachAbilityThread(scheduler, nullToken), OHOS::ERR_INVALID_VALUE);

    EXPECT_EQ(abilityMs_->AttachAbilityThread(scheduler, topAbility->GetToken()), OHOS::ERR_OK);
}

/*
 * Feature: AbilityManagerService
 * Function: AbilityTransitionDone
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService StartAbility
 * EnvConditions: NA
 * CaseDescription: AbilityTransitionDone failed due to empty token or scheduler
 */
HWTEST_F(AbilityManagerServiceTest, Interface_012, TestSize.Level0)
{

    Want wantLuncher;
    ElementName elementLun("device", "com.ix.hiworld", "LauncherAbility");
    wantLuncher.SetElement(elementLun);
    abilityMs_->StartAbility(wantLuncher);
    WaitUntilTaskFinished();
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    EXPECT_EQ(topAbility->GetAbilityInfo().bundleName, "com.ix.hiworld");
    OHOS::sptr<Token> nullToken = nullptr;
    auto res = abilityMs_->AbilityTransitionDone(nullToken, OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(res, OHOS::ERR_INVALID_VALUE);

    std::shared_ptr<AbilityRecord> record = nullptr;
    OHOS::sptr<Token> token = new Token(record);
    auto res1 = abilityMs_->AbilityTransitionDone(token, OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(res1, OHOS::ERR_INVALID_VALUE);
}

/*
 * Feature: AbilityManagerService
 * Function: SetStackManager and GetStackManager
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService SetStackManager and GetStackManager
 * EnvConditions: NA
 * CaseDescription: Verify set and get
 */
HWTEST_F(AbilityManagerServiceTest, Interface_013, TestSize.Level0)
{

    abilityMs_->SetStackManager(0);
    EXPECT_NE(nullptr, abilityMs_->GetStackManager());
}

/*
 * Feature: AbilityManagerService
 * Function: DumpWaittingAbilityQueue
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DumpWaittingAbilityQueue
 * EnvConditions: NA
 * CaseDescription: Verify dumpwaittingabilityqueue result
 */
HWTEST_F(AbilityManagerServiceTest, Interface_014, TestSize.Level0)
{

    std::string dump;
    abilityMs_->DumpWaittingAbilityQueue(dump);
    EXPECT_EQ(false, (dump.find("User ID #0") != string::npos));
}

/*
 * Feature: AbilityManagerService
 * Function: OnAbilityRequestDone
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnAbilityRequestDone
 * EnvConditions: NA
 * CaseDescription: OnAbilityRequestDone failed due to empty token
 */
HWTEST_F(AbilityManagerServiceTest, Interface_015, TestSize.Level0)
{

    Want wantLuncher;
    ElementName elementLun("device", "com.ix.hiworld", "LauncherAbility");
    wantLuncher.SetElement(elementLun);
    abilityMs_->StartAbility(wantLuncher);
    WaitUntilTaskFinished();
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    OHOS::sptr<Token> nullToken = nullptr;

    abilityMs_->OnAbilityRequestDone(nullToken, 2);
    EXPECT_EQ(topAbility->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVE);

    abilityMs_->OnAbilityRequestDone(topAbility->GetToken(), 2);
    EXPECT_EQ(topAbility->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVATING);
}

/*
 * Feature: AbilityManagerService
 * Function: RemoveMission
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService RemoveMission
 * EnvConditions: NA
 * CaseDescription: 1.MissionId < 0, RemoveMission failed
 *                  2.MissionId no exist, RemoveMission failed
 *                  3.Top mission is launcher, RemoveMission failed
 *                  4.The current mission cannot be removed
 *                  5.current mission is launcher, remove default mission is success
 */
HWTEST_F(AbilityManagerServiceTest, Interface_016, TestSize.Level0)
{

    Want wantLuncher;
    ElementName elementLun("device", "com.ix.hiworld", "LauncherAbility");
    wantLuncher.SetElement(elementLun);
    StartAbility(wantLuncher);
    WaitUntilTaskFinished();

    EXPECT_TRUE(abilityMs_->GetStackManager() != nullptr);
    EXPECT_TRUE(abilityMs_->GetStackManager()->GetTopMissionRecord() != nullptr);
    EXPECT_TRUE(abilityMs_->GetStackManager()->GetTopMissionRecord()->GetTopAbilityRecord() != nullptr);
    auto launcherWant = abilityMs_->GetStackManager()->GetTopMissionRecord()->GetTopAbilityRecord()->GetWant();

    EXPECT_EQ(abilityMs_->RemoveMission(-1), OHOS::ERR_INVALID_VALUE);

    EXPECT_EQ(abilityMs_->RemoveMission(100), REMOVE_MISSION_ID_NOT_EXIST);

    auto topMissionId = abilityMs_->GetStackManager()->GetTopMissionRecord()->GetMissionRecordId();
    EXPECT_EQ(abilityMs_->RemoveMission(topMissionId), REMOVE_MISSION_LAUNCHER_DENIED);
    GTEST_LOG_(INFO) << "topMissionId " << topMissionId;
    abilityMs_->GetStackManager()->GetCurrentTopAbility()->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    Want want;
    want.AddEntity(Want::ENTITY_HOME);
    ElementName element("device", "com.ix.music", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);

    topMissionId = abilityMs_->GetStackManager()->GetTopMissionRecord()->GetMissionRecordId();
    GTEST_LOG_(INFO) << "topMissionId " << topMissionId;
    EXPECT_FALSE(abilityMs_->GetStackManager()->IsLauncherMission(topMissionId));
    EXPECT_EQ(abilityMs_->RemoveMission(topMissionId), REMOVE_MISSION_ACTIVE_DENIED);

    auto musicAbility = abilityMs_->GetStackManager()->GetCurrentTopAbility();
    musicAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(abilityMs_->RemoveMission(topMissionId), REMOVE_MISSION_ACTIVE_DENIED);

    auto result1 = StartAbility(launcherWant);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);

    EXPECT_EQ(musicAbility->GetAbilityState(), OHOS::AAFwk::AbilityState::INACTIVATING);
    EXPECT_NE(topMissionId, abilityMs_->GetStackManager()->GetTopMissionRecord()->GetMissionRecordId());
    musicAbility->SetAbilityState(OHOS::AAFwk::AbilityState::MOVING_BACKGROUND);

    EXPECT_EQ(abilityMs_->RemoveMission(topMissionId), 0);
    WaitUntilTaskFinished();
}

/*
 * Feature: AbilityManagerService
 * Function: RemoveMission
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService RemoveMission
 * EnvConditions: window visible is true, ability state is inactive
 * CaseDescription: Verify RemoveMission operation fail
 */
HWTEST_F(AbilityManagerServiceTest, Interface_017, TestSize.Level0)
{

    Want wantLuncher;
    ElementName elementLun("device", "com.ix.hiworld", "LauncherAbility");
    wantLuncher.SetElement(elementLun);
    abilityMs_->StartAbility(wantLuncher);
    WaitUntilTaskFinished();
    auto launcherWant = abilityMs_->GetStackManager()->GetTopMissionRecord()->GetTopAbilityRecord()->GetWant();

    auto topMissionId = abilityMs_->GetStackManager()->GetTopMissionRecord()->GetMissionRecordId();
    EXPECT_EQ(abilityMs_->RemoveMission(topMissionId), REMOVE_MISSION_LAUNCHER_DENIED);
    GTEST_LOG_(INFO) << "topMissionId " << topMissionId;
    abilityMs_->GetStackManager()->GetCurrentTopAbility()->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    Want want;
    want.AddEntity(Want::ENTITY_HOME);
    ElementName element("device", "com.ix.music", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);

    auto topAbility = abilityMs_->GetStackManager()->GetCurrentTopAbility();
    abilityMs_->AddWindowInfo(topAbility->GetToken(), ++g_windowToken);
    topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    topMissionId = abilityMs_->GetStackManager()->GetTopMissionRecord()->GetMissionRecordId();

    auto result1 = abilityMs_->StartAbility(launcherWant);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);

    topAbility->GetWindowInfo()->isVisible_ = true;
    topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);

    EXPECT_EQ(abilityMs_->RemoveMission(topMissionId), REMOVE_MISSION_ACTIVE_DENIED);
    WaitUntilTaskFinished();
}

/*
 * Feature: AbilityManagerService
 * Function: GetRecentMissions
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService GetRecentMissions
 * EnvConditions: NA
 * CaseDescription: Verify GetRecentMissions operation
 */
HWTEST_F(AbilityManagerServiceTest, Interface_018, TestSize.Level0)
{

    Want want;
    want.AddEntity(Want::ENTITY_HOME);
    ElementName element("device", "com.ix.music", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);

    abilityMs_->GetStackManager()->GetCurrentTopAbility()->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    Want want1;
    want1.AddEntity(Want::ENTITY_HOME);
    ElementName element1("device", "com.ix.radio", "RadioAbility");
    want1.SetElement(element1);
    auto result1 = StartAbility(want1);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);

    std::vector<RecentMissionInfo> info;
    auto res = abilityMs_->GetRecentMissions(-1, 1, info);
    EXPECT_EQ(OHOS::ERR_INVALID_VALUE, res);

    auto res1 = abilityMs_->GetRecentMissions(INT_MAX, -1, info);
    EXPECT_EQ(OHOS::ERR_INVALID_VALUE, res1);

    auto res2 = abilityMs_->GetRecentMissions(INT_MAX, 1, info);
    EXPECT_EQ(OHOS::ERR_OK, res2);
    EXPECT_EQ(static_cast<int>(info.size()), 1);
    EXPECT_EQ(info[0].runingState, -1);
    EXPECT_EQ(info[0].missionDescription.label, "app label");
    EXPECT_EQ(info[0].missionDescription.iconPath, "icon path");
    EXPECT_EQ(info[0].baseWant.GetElement().GetAbilityName(), want.GetElement().GetAbilityName());
    EXPECT_EQ(info[0].baseWant.GetElement().GetBundleName(), want.GetElement().GetBundleName());
    EXPECT_EQ(info[0].baseWant.GetElement().GetDeviceID(), want.GetElement().GetDeviceID());

    EXPECT_EQ(info[0].baseAbility.GetAbilityName(), want.GetElement().GetAbilityName());
    EXPECT_EQ(info[0].baseAbility.GetBundleName(), want.GetElement().GetBundleName());
    EXPECT_EQ(info[0].baseAbility.GetDeviceID(), want.GetElement().GetDeviceID());

    EXPECT_EQ(info[0].topAbility.GetAbilityName(), want1.GetElement().GetAbilityName());
    EXPECT_EQ(info[0].topAbility.GetBundleName(), want1.GetElement().GetBundleName());
    EXPECT_EQ(info[0].topAbility.GetDeviceID(), want1.GetElement().GetDeviceID());
}

/*
 * Feature: AbilityManagerService
 * Function: RemoveStack
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService RemoveStack
 * EnvConditions: NA
 * CaseDescription: 1.stackid < 0, RemoveStack failed
 *                  2.stackid no exist, RemoveStack failed
 *                  3.don't allow remove luncher mission stack
 */
HWTEST_F(AbilityManagerServiceTest, Interface_019, TestSize.Level0)
{

    EXPECT_EQ(abilityMs_->RemoveStack(-1), OHOS::ERR_INVALID_VALUE);
    EXPECT_EQ(abilityMs_->RemoveStack(INT_MAX), REMOVE_STACK_ID_NOT_EXIST);
    EXPECT_EQ(abilityMs_->RemoveStack(0), REMOVE_STACK_LAUNCHER_DENIED);
}

/*
 * Feature: AbilityManagerService
 * Function: RemoveStack
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService RemoveStack
 * EnvConditions: NA
 * CaseDescription: remove default stack success
 */
HWTEST_F(AbilityManagerServiceTest, Interface_020, TestSize.Level0)
{

    Want want;
    ElementName element("device", "com.ix.music", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManage = abilityMs_->GetStackManager();
    stackManage->GetCurrentTopAbility()->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    Want want1;
    ElementName element1("device", "com.ix.music", "MusicAbility");
    want1.SetElement(element1);
    auto result1 = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);

    EXPECT_EQ(stackManage->GetCurrentMissionStack()->GetMissionRecordCount(), 1);
    EXPECT_EQ(abilityMs_->RemoveStack(1), REMOVE_MISSION_ACTIVE_DENIED);

    stackManage->GetCurrentTopAbility()->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    auto launcherWant = abilityMs_->GetStackManager()->GetTopMissionRecord()->GetTopAbilityRecord()->GetWant();
    auto result2 = abilityMs_->StartAbility(launcherWant);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result2);
}

/*
 * Feature: AbilityManagerService
 * Function: ConnectAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService ConnectAbility
 * EnvConditions: NA
 * CaseDescription: Verify the following:
 * 1.callback is nullptr, connectAbility failed
 * 2.ability type is page, connectAbility failed
 * 3.ability type is service and callback is not nullptr, connectAbility success
 */
HWTEST_F(AbilityManagerServiceTest, Interface_021, TestSize.Level0)
{

    Want want;
    ElementName element("device", "com.ix.musicService", "MusicService");
    want.SetElement(element);
    OHOS::sptr<IAbilityConnection> callback = new AbilityConnectCallback();
    ;
    auto result = abilityMs_->ConnectAbility(want, nullptr, nullptr);
    EXPECT_EQ(result, ERR_INVALID_VALUE);

    Want want1;
    ElementName element1("device", "com.ix.hiMusic", "MusicAbility");
    want1.SetElement(element1);
    auto result1 = abilityMs_->ConnectAbility(want1, callback, nullptr);
    EXPECT_EQ(result1, TARGET_ABILITY_NOT_SERVICE);

    auto result2 = abilityMs_->ConnectAbility(want, callback, nullptr);
    EXPECT_EQ(result2, ERR_OK);
}

/*
 * Feature: AbilityManagerService
 * Function: DisconnectAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService DisconnectAbility
 * EnvConditions: NA
 * CaseDescription: Verify the following:
 * 1.callback is nullptr, disconnect ability failed
 * 2.connect ability is not connected, connectAbility failed
 */
HWTEST_F(AbilityManagerServiceTest, Interface_022, TestSize.Level0)
{

    Want want;
    ElementName element("device", "com.ix.musicService", "MusicService");
    want.SetElement(element);
    OHOS::sptr<IAbilityConnection> callback = new AbilityConnectCallback();
    ;
    auto result = abilityMs_->ConnectAbility(want, callback, nullptr);
    EXPECT_EQ(result, ERR_OK);

    auto serviceMap = abilityMs_->connectManager_->GetServiceMap();
    auto service = serviceMap.at(element.GetURI());
    service->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    auto result1 = abilityMs_->DisconnectAbility(nullptr);
    EXPECT_EQ(result1, ERR_INVALID_VALUE);

    auto result2 = abilityMs_->DisconnectAbility(callback);
    EXPECT_EQ(result2, INVALID_CONNECTION_STATE);
}

/*
 * Feature: AbilityManagerService
 * Function: ScheduleConnectAbilityDone
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService ScheduleConnectAbilityDone
 * EnvConditions: NA
 * CaseDescription: Verify the following:
 * 1.token is nullptr, ScheduleConnectAbilityDone failed
 * 2.ability record is nullptr, ScheduleConnectAbilityDone failed
 * 2.ability type is not service, ScheduleConnectAbilityDone failed
 */
HWTEST_F(AbilityManagerServiceTest, Interface_023, TestSize.Level0)
{

    Want want;
    ElementName element("device", "com.ix.musicService", "MusicService");
    want.SetElement(element);
    OHOS::sptr<IAbilityConnection> callback = new AbilityConnectCallback();
    auto result = abilityMs_->ConnectAbility(want, callback, nullptr);
    WaitUntilTaskFinished();
    EXPECT_EQ(result, ERR_OK);
    auto serviceMap = abilityMs_->connectManager_->GetServiceMap();
    auto service = serviceMap.at(element.GetURI());
    service->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    const sptr<IRemoteObject> nulltToken = nullptr;
    auto result1 = abilityMs_->ScheduleConnectAbilityDone(nulltToken, callback->AsObject());
    WaitUntilTaskFinished();
    EXPECT_EQ(result1, ERR_INVALID_VALUE);

    std::shared_ptr<AbilityRecord> ability = nullptr;
    const sptr<IRemoteObject> token = new Token(ability);
    auto result2 = abilityMs_->ScheduleConnectAbilityDone(token, callback->AsObject());
    WaitUntilTaskFinished();
    EXPECT_EQ(result2, ERR_INVALID_VALUE);

    Want want1;
    want1.AddEntity(Want::ENTITY_HOME);
    ElementName element1("device", "com.ix.radio", "RadioAbility");
    want1.SetElement(element1);
    auto result3 = StartAbility(want1);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result3);
    auto topAbility = abilityMs_->GetStackManager()->GetCurrentTopAbility();

    auto result4 = abilityMs_->ScheduleConnectAbilityDone(topAbility->GetToken(), callback->AsObject());
    WaitUntilTaskFinished();
    EXPECT_EQ(result4, TARGET_ABILITY_NOT_SERVICE);

    auto result5 = abilityMs_->ScheduleConnectAbilityDone(service->GetToken(), callback->AsObject());
    WaitUntilTaskFinished();
    EXPECT_EQ(result5, ERR_OK);
}

/*
 * Feature: AbilityManagerService
 * Function: ScheduleDisconnectAbilityDone
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService ScheduleDisconnectAbilityDone
 * EnvConditions: NA
 * CaseDescription: Verify the following:
 * 1.token is nullptr, ScheduleDisconnectAbilityDone failed
 * 2.ability record is nullptr, ScheduleDisconnectAbilityDone failed
 * 2.ability type is not service, ScheduleDisconnectAbilityDone failed
 */
HWTEST_F(AbilityManagerServiceTest, Interface_024, TestSize.Level0)
{

    Want want;
    ElementName element("device", "com.ix.musicService", "MusicService");
    want.SetElement(element);
    OHOS::sptr<IAbilityConnection> callback = new AbilityConnectCallback();
    ;
    auto result = abilityMs_->ConnectAbility(want, callback, nullptr);
    WaitUntilTaskFinished();
    EXPECT_EQ(result, ERR_OK);
    auto serviceMap = abilityMs_->connectManager_->GetServiceMap();
    auto service = serviceMap.at(element.GetURI());
    service->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    const sptr<IRemoteObject> nulltToken = nullptr;
    auto result1 = abilityMs_->ScheduleDisconnectAbilityDone(nulltToken);
    WaitUntilTaskFinished();
    EXPECT_EQ(result1, ERR_INVALID_VALUE);

    std::shared_ptr<AbilityRecord> ability = nullptr;
    const sptr<IRemoteObject> token = new Token(ability);
    auto result2 = abilityMs_->ScheduleDisconnectAbilityDone(token);
    WaitUntilTaskFinished();
    EXPECT_EQ(result2, ERR_INVALID_VALUE);

    Want want1;
    want1.AddEntity(Want::ENTITY_HOME);
    ElementName element1("device", "com.ix.radio", "RadioAbility");
    want1.SetElement(element1);
    auto result3 = StartAbility(want1);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result3);
    auto topAbility = abilityMs_->GetStackManager()->GetCurrentTopAbility();

    auto result4 = abilityMs_->ScheduleDisconnectAbilityDone(topAbility->GetToken());
    WaitUntilTaskFinished();
    EXPECT_EQ(result4, TARGET_ABILITY_NOT_SERVICE);

    auto result5 = abilityMs_->ScheduleDisconnectAbilityDone(service->GetToken());
    WaitUntilTaskFinished();
    EXPECT_EQ(result5, CONNECTION_NOT_EXIST);
}

/*
 * Feature: AbilityManagerService
 * Function: ScheduleCommandAbilityDone
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService ScheduleCommandAbilityDone
 * EnvConditions: NA
 * CaseDescription: Verify the following:
 * 1.token is nullptr, ScheduleCommandAbilityDone failed
 * 2.ability record is nullptr, ScheduleCommandAbilityDone failed
 * 2.ability type is not service, ScheduleCommandAbilityDone failed
 */
HWTEST_F(AbilityManagerServiceTest, Interface_025, TestSize.Level0)
{

    Want want;
    ElementName element("device", "com.ix.musicService", "MusicService");
    want.SetElement(element);
    OHOS::sptr<IAbilityConnection> callback = new AbilityConnectCallback();
    ;
    auto result = abilityMs_->ConnectAbility(want, callback, nullptr);
    WaitUntilTaskFinished();
    EXPECT_EQ(result, ERR_OK);
    auto serviceMap = abilityMs_->connectManager_->GetServiceMap();
    auto service = serviceMap.at(element.GetURI());
    service->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    const sptr<IRemoteObject> nulltToken = nullptr;
    auto result1 = abilityMs_->ScheduleCommandAbilityDone(nulltToken);
    WaitUntilTaskFinished();
    EXPECT_EQ(result1, ERR_INVALID_VALUE);

    std::shared_ptr<AbilityRecord> ability = nullptr;
    const sptr<IRemoteObject> token = new Token(ability);
    auto result2 = abilityMs_->ScheduleCommandAbilityDone(token);
    WaitUntilTaskFinished();
    EXPECT_EQ(result2, ERR_INVALID_VALUE);

    Want want1;
    want1.AddEntity(Want::ENTITY_HOME);
    ElementName element1("device", "com.ix.radio", "RadioAbility");
    want1.SetElement(element1);
    auto result3 = StartAbility(want1);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result3);
    auto topAbility = abilityMs_->GetStackManager()->GetCurrentTopAbility();

    auto result4 = abilityMs_->ScheduleCommandAbilityDone(topAbility->GetToken());
    WaitUntilTaskFinished();
    EXPECT_EQ(result4, TARGET_ABILITY_NOT_SERVICE);

    auto result5 = abilityMs_->ScheduleCommandAbilityDone(service->GetToken());
    WaitUntilTaskFinished();
    EXPECT_EQ(result5, ERR_OK);
}

/*
 * Feature: AbilityManagerService
 * Function: GetAllStackInfo
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService GetAllStackInfo
 * EnvConditions: NA
 * CaseDescription: Verify getallstackenfo results
 */
HWTEST_F(AbilityManagerServiceTest, Interface_026, TestSize.Level0)
{
    EXPECT_TRUE(abilityMs_ != nullptr);
    Want want;
    ElementName element("device", "com.ix.hiMusic", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    Want want1;
    ElementName element1("device", "com.ix.hiMusic", "MusicTopAbility");
    want1.SetElement(element1);
    auto result1 = StartAbility(want1);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);
    stackManager = abilityMs_->GetStackManager();
    topAbility = stackManager->GetCurrentTopAbility();
    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    Want want2;
    ElementName element2("device", "com.ix.hiMusic", "MusicTopAbility");
    want2.SetElement(element2);
    auto result2 = StartAbility(want2);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result2);
    stackManager = abilityMs_->GetStackManager();
    topAbility = stackManager->GetCurrentTopAbility();

    StackInfo info;
    stackManager->GetAllStackInfo(info);
    EXPECT_EQ(static_cast<int>(info.missionStackInfos.size()), 2);
}

/*
 * Feature: AbilityManagerService
 * Function: StopServiceAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService StopServiceAbility
 * EnvConditions: NA
 * CaseDescription: Verify StopServiceAbility results
 */
HWTEST_F(AbilityManagerServiceTest, Interface_027, TestSize.Level0)
{

    Want want;
    ElementName element("device", "com.ix.musicService", "MusicService");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto serviceMap = abilityMs_->connectManager_->GetServiceMap();
    EXPECT_EQ(1, static_cast<int>(serviceMap.size()));
    for (auto &it : serviceMap) {
        EXPECT_EQ(it.first, element.GetURI());
    }
    auto service = serviceMap.at(element.GetURI());
    service->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    Want want1;
    ElementName element1("device", "com.ix.hiMusic", "MusicAbility");
    want1.SetElement(element1);
    auto result1 = abilityMs_->StopServiceAbility(want1);
    WaitUntilTaskFinished();
    EXPECT_EQ(TARGET_ABILITY_NOT_SERVICE, result1);

    auto result2 = abilityMs_->StopServiceAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(ERR_OK, result2);
}

/*
 * Feature: AbilityManagerService
 * Function: MoveMissionToTop
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService MoveMissionToTop
 * EnvConditions: NA
 * CaseDescription: Verify MoveMissionToTop results
 */
HWTEST_F(AbilityManagerServiceTest, Interface_028, TestSize.Level0)
{

    Want want;
    ElementName element("device", "com.ix.hiMusic", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    auto mission = topAbility->GetMissionRecord();
    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    Want want1;
    ElementName element1("device", "com.ix.hiMusic", "MusicSAbility");
    want1.SetElement(element1);
    auto result1 = StartAbility(want1);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);
    stackManager = abilityMs_->GetStackManager();
    topAbility = stackManager->GetCurrentTopAbility();
    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    abilityMs_->MoveMissionToTop(mission->GetMissionRecordId());
    WaitUntilTaskFinished();
    stackManager = abilityMs_->GetStackManager();
    topAbility = stackManager->GetCurrentTopAbility();
    auto topMission = topAbility->GetMissionRecord();
    EXPECT_EQ(topMission, mission);
}

/*
 * Feature: AbilityManagerService
 * Function: KillProcess
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService KillProcess
 * EnvConditions: NA
 * CaseDescription: Verify KillProcess Failure situation
 */
HWTEST_F(AbilityManagerServiceTest, Interface_029, TestSize.Level0)
{

    abilityMs_->currentStackManager_ = nullptr;
    auto result = abilityMs_->KillProcess("bundle");
    EXPECT_EQ(ERR_NO_INIT, result);
}

/*
 * Feature: AbilityManagerService
 * Function: UninstallApp
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService UninstallApp
 * EnvConditions: NA
 * CaseDescription: Verify UninstallApp Failure situation
 */
HWTEST_F(AbilityManagerServiceTest, Interface_030, TestSize.Level0)
{

    abilityMs_->currentStackManager_ = nullptr;
    auto result = abilityMs_->UninstallApp("bundle");
    EXPECT_EQ(ERR_NO_INIT, result);
}

/*
 * Feature: AbilityManagerService
 * Function: AcquireDataAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService AcquireDataAbility
 * EnvConditions: NA
 * CaseDescription: Verify function AcquireDataAbility return nullptr when AbilityManagerService not
 * dataAbilityManager_.
 */
HWTEST_F(AbilityManagerServiceTest, AbilityManagerService_AcquireDataAbility_001, TestSize.Level0)
{
    OHOS::Uri dataAbilityUri("dataability:///data.bundle.DataAbility");

    // assert ability record
    ASSERT_TRUE(abilityRecord_);

    EXPECT_EQ(abilityMs_->AcquireDataAbility(dataAbilityUri, true, abilityRecord_->GetToken()), nullptr);
}

/*
 * Feature: AbilityManagerService
 * Function: AcquireDataAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService AcquireDataAbility
 * EnvConditions: NA
 * CaseDescription: Verify function AcquireDataAbility return nullptr when AbilityManagerService not iBundleManager_.
 */
HWTEST_F(AbilityManagerServiceTest, AbilityManagerService_AcquireDataAbility_002, TestSize.Level0)
{
    OHOS::Uri dataAbilityUri("dataability:///data.bundle.DataAbility");
    // assert ability record
    ASSERT_TRUE(abilityRecord_);
    EXPECT_EQ(abilityMs_->AcquireDataAbility(dataAbilityUri, true, abilityRecord_->GetToken()), nullptr);
}

/*
 * Feature: AbilityManagerService
 * Function: AcquireDataAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService AcquireDataAbility
 * EnvConditions: NA
 * CaseDescription: Verify function AcquireDataAbility return nullptr when uri not start with 'dataablity'
 */
HWTEST_F(AbilityManagerServiceTest, AbilityManagerService_AcquireDataAbility_003, TestSize.Level0)
{
    OHOS::Uri dataAbilityUri("mydataability:///data.bundle.DataAbility");
    // assert ability record
    ASSERT_TRUE(abilityRecord_);
    EXPECT_EQ(abilityMs_->AcquireDataAbility(dataAbilityUri, true, abilityRecord_->GetToken()), nullptr);
}

/*
 * Feature: AbilityManagerService
 * Function: AcquireDataAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService AcquireDataAbility
 * EnvConditions: NA
 * CaseDescription: Verify function AcquireDataAbility return nullptr when uri start with empty
 */
HWTEST_F(AbilityManagerServiceTest, AbilityManagerService_AcquireDataAbility_004, TestSize.Level0)
{
    OHOS::Uri dataAbilityUri("dataability://");
    EXPECT_EQ(abilityMs_->AcquireDataAbility(dataAbilityUri, true, abilityRecord_->GetToken()), nullptr);
}

/*
 * Feature: AbilityManagerService
 * Function: AcquireDataAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService AcquireDataAbility
 * EnvConditions: NA
 * CaseDescription: Verify function AcquireDataAbility return nullptr when QueryAbilityInfoByUri false.
 */
HWTEST_F(AbilityManagerServiceTest, AbilityManagerService_AcquireDataAbility_005, TestSize.Level0)
{
    OHOS::Uri dataAbilityUri("dataability:///data.bundle.DataAbility");
    EXPECT_EQ(abilityMs_->AcquireDataAbility(dataAbilityUri, true, abilityRecord_->GetToken()), nullptr);
}

/*
 * Feature: AbilityManagerService
 * Function: AcquireDataAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService AcquireDataAbility
 * EnvConditions: NA
 * CaseDescription: Verify function AcquireDataAbility return nullptr when appInfo name empty
 */
HWTEST_F(AbilityManagerServiceTest, AbilityManagerService_AcquireDataAbility_006, TestSize.Level0)
{
    OHOS::Uri dataAbilityUri("dataability:///data.bundle.DataAbility");

    if (abilityRecord_ == nullptr) {
        abilityRequest_.appInfo.bundleName = "data.client.bundle";
        abilityRequest_.abilityInfo.name = "ClientAbility";
        abilityRequest_.abilityInfo.type = AbilityType::DATA;
        abilityRecord_ = AbilityRecord::CreateAbilityRecord(abilityRequest_);
    }

    EXPECT_EQ(abilityMs_->AcquireDataAbility(dataAbilityUri, true, abilityRecord_->GetToken()), nullptr);
}

/*
 * Feature: AbilityManagerService
 * Function: ReleaseDataAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService ReleaseDataAbility
 * EnvConditions: NA
 * CaseDescription: Verify function ReleaseDataAbility
 * return nullptr when AbilityManagerService not dataAbilityManager_.
 */
HWTEST_F(AbilityManagerServiceTest, AbilityManagerService_ReleaseDataAbility_001, TestSize.Level0)
{
    // assert ability record
    ASSERT_TRUE(abilityRecord_);
    EXPECT_EQ(abilityMs_->ReleaseDataAbility(nullptr, nullptr), OHOS::ERR_INVALID_STATE);
}

/*
 * Feature: AbilityManagerService
 * Function: ReleaseDataAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService ReleaseDataAbility
 * EnvConditions: NA
 * CaseDescription: Verify function ReleaseDataAbility
 * return nullptr when AbilityManagerService not dataAbilityManager_.
 */
HWTEST_F(AbilityManagerServiceTest, TerminateAbilityResult_001, TestSize.Level0)
{
    sptr<IRemoteObject> token = nullptr;
    auto result1 = abilityMs_->TerminateAbilityResult(token, -1);
    WaitUntilTaskFinished();
    EXPECT_EQ(ERR_INVALID_VALUE, result1);

    // not service aa
    Want want;
    ElementName element("device", "com.ix.hiMusic", "MusicAbility");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    auto token1 = topAbility->GetToken();
    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    auto result2 = abilityMs_->TerminateAbilityResult(token1, topAbility->GetStartId());
    WaitUntilTaskFinished();
    EXPECT_EQ(TARGET_ABILITY_NOT_SERVICE, result2);

    // an service aa
    Want want2;
    ElementName element2("device", "com.ix.musicService", "MusicService");
    want2.SetElement(element2);
    result = StartAbility(want2);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    stackManager = abilityMs_->GetStackManager();
    auto topAbility2 = stackManager->GetCurrentTopAbility();
    auto token2 = topAbility->GetToken();
    topAbility2->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    auto result3 = abilityMs_->TerminateAbilityResult(token2, -1);
    EXPECT_NE(OHOS::ERR_OK, result3);

    // current stack top is com.ix.hiMusic
    auto stackManagerAfter = abilityMs_->GetStackManager();
    auto topAbilityAfter = stackManagerAfter->GetCurrentTopAbility();
    Want want3 = topAbilityAfter->GetWant();
    EXPECT_EQ(want3.GetElement().GetURI(), want.GetElement().GetURI());
}

/*
 * Feature: AbilityManagerService
 * Function: ReleaseDataAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService ReleaseDataAbility
 * EnvConditions: NA
 * CaseDescription: Verify function ReleaseDataAbility
 * return nullptr when AbilityManagerService not dataAbilityManager_.
 */
HWTEST_F(AbilityManagerServiceTest, startAbility_001, TestSize.Level0)
{
    // first run a service aa
    Want want;
    ElementName element("device", "com.ix.musicService", "MusicService");
    want.SetElement(element);
    auto result = StartAbility(want);
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();

    sptr<IRemoteObject> token = nullptr;
    if (topAbility) {
        topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);
        token = topAbility->GetToken();
    }

    // start other aa
    Want want1;
    ElementName element1("device", "com.ix.hiMusic", "MusicAbility");
    want.SetElement(element1);
    auto result1 = abilityMs_->StartAbility(want, token);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);
}

/*
 * Feature: AbilityManagerService
 * Function: startAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService startAbility
 * EnvConditions: NA
 * CaseDescription: Verify function startAbility
 * return nullptr when AbilityManagerService not dataAbilityManager_.
 */
HWTEST_F(AbilityManagerServiceTest, startAbility_002, TestSize.Level0)
{
    // first run a service aa
    Want want;
    ElementName element("device", "com.ix.musicService", "MusicService");
    want.SetElement(element);
    auto result = StartAbility(want);
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();

    sptr<IRemoteObject> token = nullptr;
    if (topAbility) {
        topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);
        token = topAbility->GetToken();
    }

    // start self
    auto result1 = abilityMs_->StartAbility(want, token);
    EXPECT_NE(OHOS::ERR_OK, result1);
}

/*
 * Feature: AbilityManagerService
 * Function: startAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService startAbility
 * EnvConditions: NA
 * CaseDescription: Verify function startAbility
 * return nullptr when AbilityManagerService not dataAbilityManager_.
 */
HWTEST_F(AbilityManagerServiceTest, startAbility_003, TestSize.Level0)
{
    // first run a service aa
    Want want;
    ElementName element("device", "com.ix.musicService", "MusicService");
    want.SetElement(element);
    auto result = StartAbility(want);
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    sptr<IRemoteObject> token = nullptr;
    if (topAbility) {
        topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);
        token = topAbility->GetToken();
    }

    // start a date aa
    Want want1;
    ElementName element1("device", "com.ix.hiData", "DataAbility");
    want.SetElement(element1);
    auto result1 = abilityMs_->StartAbility(want1, token);
    EXPECT_EQ(RESOLVE_ABILITY_ERR, result1);
}

/*
 * Feature: AbilityManagerService
 * Function: startAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService startAbility
 * EnvConditions: NA
 * CaseDescription: Verify function startAbility
 * return nullptr when AbilityManagerService not dataAbilityManager_.
 */
HWTEST_F(AbilityManagerServiceTest, startAbility_004, TestSize.Level0)
{
    // run a page aa
    Want want;
    ElementName element("device", "com.ix.hiworld", "WorldService");
    want.SetElement(element);
    auto result = StartAbility(want);
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    sptr<IRemoteObject> token = nullptr;
    if (topAbility) {
        topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);
        token = topAbility->GetToken();
    }

    // start a page aa
    Want want1;
    ElementName element1("device", "com.ix.hiMusic", "hiMusic");
    want1.SetElement(element1);
    auto result1 = abilityMs_->StartAbility(want1, token);
    EXPECT_EQ(OHOS::ERR_OK, result1);

    // current stack top is com.ix.hiMusic
    auto stackManagerAfter = abilityMs_->GetStackManager();
    auto topAbilityAfter = stackManagerAfter->GetCurrentTopAbility();
    Want want2 = topAbilityAfter->GetWant();
    EXPECT_EQ(want1.GetElement().GetURI(), want2.GetElement().GetURI());
}

/*
 * Feature: AbilityManagerService
 * Function: startAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService startAbility
 * EnvConditions: NA
 * CaseDescription: Verify function startAbility
 * return nullptr when AbilityManagerService not dataAbilityManager_.
 */
HWTEST_F(AbilityManagerServiceTest, systemDialog_001, TestSize.Level0)
{
    Want want;
    ElementName element("device", "com.ix.hiworld", "WorldService");
    want.SetElement(element);
    auto result = StartAbility(want);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto topAbility = stackManager->GetCurrentTopAbility();
    auto token = topAbility->GetToken();
    topAbility->SetAbilityState(AAFwk::AbilityState::ACTIVE);

    // statrt a dialog
    Want want1;
    ElementName elementdialog1("device", AbilityConfig::SYSTEM_UI_BUNDLE_NAME, AbilityConfig::SYSTEM_DIALOG_NAME);
    want1.SetElement(elementdialog1);
    auto result1 = abilityMs_->StartAbility(want1, token);
    WaitUntilTaskFinished();
    EXPECT_EQ(OHOS::ERR_OK, result1);
    auto dialogAbility = stackManager->GetCurrentTopAbility();
    auto dialogtoken = topAbility->GetToken();

    // current stack top is dialog
    auto stackManagerAfter = abilityMs_->GetStackManager();
    auto topAbilityAfter = stackManagerAfter->GetCurrentTopAbility();
    Want want2 = topAbilityAfter->GetWant();
    EXPECT_EQ(want1.GetElement().GetURI(), want2.GetElement().GetURI());

    auto dialogRecord = stackManager->GetAbilityRecordByToken(dialogtoken);
    EXPECT_EQ(dialogRecord->GetAbilityState(), AAFwk::AbilityState::INACTIVATING);

    // start other dialog
    Want want3;
    ElementName elementdialog2("device2", AbilityConfig::SYSTEM_UI_BUNDLE_NAME, AbilityConfig::SYSTEM_DIALOG_NAME);
    want3.SetElement(elementdialog2);
    auto result2 = abilityMs_->StartAbility(want3);

    EXPECT_NE(OHOS::ERR_OK, result2);
    auto dialogAbility2 = stackManager->GetCurrentTopAbility();
    auto dialogtoken2 = dialogAbility2->GetToken();
    EXPECT_EQ(dialogAbility2->GetAbilityState(), AAFwk::AbilityState::INITIAL);

    // start a luncher aa,should be fail
    Want wantLuncher;
    ElementName elementLun("device", "com.ix.hiworld", "WorldService");
    wantLuncher.SetElement(elementLun);
    auto result3 = abilityMs_->StartAbility(wantLuncher);
    // should be fail
    EXPECT_NE(OHOS::ERR_OK, result3);
    auto topAbilityLun = stackManager->GetCurrentTopAbility();
    EXPECT_EQ(topAbilityLun->GetStartId(), dialogAbility2->GetStartId());
}

/*
 * Feature: AbilityManagerService
 * Function: startAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService startAbility
 * EnvConditions: NA
 * CaseDescription: Verify function startAbility
 * return nullptr when AbilityManagerService not dataAbilityManager_.
 */
HWTEST_F(AbilityManagerServiceTest, systemDialog_002, TestSize.Level0)
{
    // start dialog when device get up
    Want want;
    ElementName elementdialog("device", AbilityConfig::SYSTEM_UI_BUNDLE_NAME, AbilityConfig::SYSTEM_DIALOG_NAME);
    want.SetElement(elementdialog);
    auto result = StartAbility(want);
    EXPECT_EQ(OHOS::ERR_OK, result);
    auto stackManager = abilityMs_->GetStackManager();
    auto dialogAbility = stackManager->GetCurrentTopAbility();
    auto dialogtoken = dialogAbility->GetToken();
    EXPECT_TRUE(dialogAbility->GetAbilityInfo().bundleName == AbilityConfig::SYSTEM_UI_BUNDLE_NAME);

    // other same aa
    Want want1;
    ElementName elementdialog1("device", AbilityConfig::SYSTEM_UI_BUNDLE_NAME, AbilityConfig::SYSTEM_DIALOG_NAME);
    want1.SetElement(elementdialog1);
    auto result1 = StartAbility(want1);
    EXPECT_EQ(OHOS::ERR_OK, result1);

    // start a page aa, waiting
    Want want2;
    ElementName elementdialog2("device", "com.ix.hiMusic", "hiMusic");
    want2.SetElement(elementdialog2);
    auto result2 = StartAbility(want2);
    EXPECT_EQ(OHOS::ERR_OK, result2);
    auto MusicAbility = stackManager->GetCurrentTopAbility();
    EXPECT_EQ(MusicAbility->GetAbilityInfo().bundleName, "com.ix.hiMusic");
}

}  // namespace AAFwk
}  // namespace OHOS