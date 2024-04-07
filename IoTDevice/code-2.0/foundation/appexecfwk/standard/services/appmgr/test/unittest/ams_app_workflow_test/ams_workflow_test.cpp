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

#include "app_mgr_service_inner.h"
#include <unistd.h>
#include <gtest/gtest.h>
#include "iremote_object.h"
#include "refbase.h"
#include "app_launch_data.h"
#include "app_log_wrapper.h"
#include "mock_ability_token.h"
#include "mock_app_scheduler.h"
#include "mock_app_spawn_client.h"

using namespace testing::ext;
using testing::_;
using testing::Return;
using testing::SetArgReferee;
namespace OHOS {
namespace AppExecFwk {
struct TestApplicationPreRecord {
    TestApplicationPreRecord(
        const std::shared_ptr<AppRunningRecord> &appRecord, const sptr<MockAppScheduler> &mockAppScheduler)
        : appRecord_(appRecord), mockAppScheduler_(mockAppScheduler)
    {}

    std::shared_ptr<AbilityRunningRecord> GetAbility(const sptr<IRemoteObject> &token) const
    {
        return appRecord_->GetAbilityRunningRecordByToken(token);
    }

    virtual ~TestApplicationPreRecord()
    {}

    std::shared_ptr<AppRunningRecord> appRecord_;
    sptr<MockAppScheduler> mockAppScheduler_;
};

pid_t g_mockPid = 0;
class AmsWorkFlowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    AbilityInfo CreateAbilityInfo(const std::string &ability, const std::string &app) const;
    ApplicationInfo CreateApplication(const std::string &app) const;
    sptr<MockAppScheduler> AddApplicationClient(const std::shared_ptr<AppRunningRecord> &appRecord) const;
    TestApplicationPreRecord CreateTestApplicationRecord(const std::string &ability, const sptr<IRemoteObject> &token,
        const std::string &app, const AbilityState abilityState, const ApplicationState appState) const;

protected:
    std::unique_ptr<AppMgrServiceInner> serviceInner_;
};

void AmsWorkFlowTest::SetUpTestCase()
{}

void AmsWorkFlowTest::TearDownTestCase()
{}

void AmsWorkFlowTest::SetUp()
{
    serviceInner_ = std::make_unique<AppMgrServiceInner>();
}

void AmsWorkFlowTest::TearDown()
{
    g_mockPid = 0;
}

AbilityInfo AmsWorkFlowTest::CreateAbilityInfo(const std::string &ability, const std::string &app) const
{
    AbilityInfo abilityInfo;
    abilityInfo.name = "test_ability" + ability;
    abilityInfo.applicationName = "test_app" + app;
    return abilityInfo;
}

ApplicationInfo AmsWorkFlowTest::CreateApplication(const std::string &app) const
{
    ApplicationInfo appInfo;
    appInfo.name = "test_app" + app;
    return appInfo;
}

sptr<MockAppScheduler> AmsWorkFlowTest::AddApplicationClient(const std::shared_ptr<AppRunningRecord> &appRecord) const
{
    if (appRecord->GetApplicationClient()) {
        return nullptr;
    }
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    appRecord->SetApplicationClient(client);
    return mockAppScheduler;
}

// create one application that include one ability, and set state
TestApplicationPreRecord AmsWorkFlowTest::CreateTestApplicationRecord(const std::string &ability,
    const sptr<IRemoteObject> &token, const std::string &app, const AbilityState abilityState,
    const ApplicationState appState) const
{
    RecordQueryResult result;
    AbilityInfo abilityInfo = CreateAbilityInfo(ability, app);
    ApplicationInfo appInfo = CreateApplication(app);

    auto appRecord = serviceInner_->GetOrCreateAppRunningRecord(token,
        std::make_shared<ApplicationInfo>(appInfo),
        std::make_shared<AbilityInfo>(abilityInfo),
        appInfo.name,
        0,
        result);
    if (!result.appExists) {
        appRecord->GetPriorityObject()->SetPid(g_mockPid++);
    }
    EXPECT_NE(appRecord, nullptr);
    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_NE(abilityRecord, nullptr);
    abilityRecord->SetState(abilityState);
    appRecord->SetState(appState);
    sptr<MockAppScheduler> mockAppScheduler = AddApplicationClient(appRecord);

    TestApplicationPreRecord testAppPreRecord(appRecord, mockAppScheduler);
    return testAppPreRecord;
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: BackKey
 * CaseDescription: when only one ability on foreground, previous is another app, simulate press back key
 */
HWTEST_F(AmsWorkFlowTest, BackKey_001, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest BackKey_001 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    TestApplicationPreRecord appB = CreateTestApplicationRecord(
        "B", tokenB, "B", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_CALL(*(appB.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(appB.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appB.GetAbility(tokenB)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appB.appRecord_->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest BackKey_001 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: BackKey
 * CaseDescription: when only one ability on foreground, previous ability in same app, simulate press back key
 */
HWTEST_F(AmsWorkFlowTest, BackKey_002, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest BackKey_002 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(0);

    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);

    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appA.GetAbility(tokenB)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest BackKey_002 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: BackKey
 * CaseDescription: when two ability on foreground, previous ability in another app, simulate press back key
 */
HWTEST_F(AmsWorkFlowTest, BackKey_003, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest BackKey_003 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenC = new MockAbilityToken();
    TestApplicationPreRecord appC = CreateTestApplicationRecord(
        "C", tokenC, "C", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_CALL(*(appC.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenC, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(appC.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appC.GetAbility(tokenC)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appC.appRecord_->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenB)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest BackKey_003 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: BackKey
 * CaseDescription: when two ability on foreground, previous is 2 ability in another app, simulate press back key
 */
HWTEST_F(AmsWorkFlowTest, BackKey_004, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest BackKey_004 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenC = new MockAbilityToken();
    TestApplicationPreRecord appC = CreateTestApplicationRecord(
        "C", tokenC, "C", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    sptr<IRemoteObject> tokenD = new MockAbilityToken();
    CreateTestApplicationRecord(
        "D", tokenD, "C", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_CALL(*(appC.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenC, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(appC.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(tokenD, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appC.GetAbility(tokenC)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appC.GetAbility(tokenD)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appC.appRecord_->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenB)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest BackKey_004 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: BackKey
 * CaseDescription: when only one ability on foreground, previous is another app, simulate press back key and exit app
 */
HWTEST_F(AmsWorkFlowTest, BackKey_005, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest BackKey_005 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    TestApplicationPreRecord appB = CreateTestApplicationRecord(
        "B", tokenB, "B", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_CALL(*(appB.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleCleanAbility(_)).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(appB.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(tokenA);
    serviceInner_->AbilityTerminated(tokenA);
    serviceInner_->ApplicationTerminated(appA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appB.GetAbility(tokenB)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appB.appRecord_->GetState());
    EXPECT_EQ(appA.GetAbility(tokenA), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest BackKey_005 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: BackKey
 * CaseDescription: when two ability on foreground, previous is another app, simulate press back key and exit
 */
HWTEST_F(AmsWorkFlowTest, BackKey_006, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest BackKey_006 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenC = new MockAbilityToken();
    TestApplicationPreRecord appC = CreateTestApplicationRecord(
        "C", tokenC, "C", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_CALL(*(appC.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleCleanAbility(_)).Times(2);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenC, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(appC.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(tokenA);
    serviceInner_->AbilityTerminated(tokenA);
    serviceInner_->TerminateAbility(tokenB);
    serviceInner_->AbilityTerminated(tokenB);
    serviceInner_->ApplicationTerminated(appA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appC.GetAbility(tokenC)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appC.appRecord_->GetState());
    EXPECT_EQ(appA.GetAbility(tokenA), nullptr);
    EXPECT_EQ(appA.GetAbility(tokenB), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest BackKey_006 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: BackKey
 * CaseDescription: when two ability on foreground, previous is 2 abiltiy in another app,
 *                  simulate press back key and exit
 */
HWTEST_F(AmsWorkFlowTest, BackKey_007, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest BackKey_007 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenC = new MockAbilityToken();
    TestApplicationPreRecord appC = CreateTestApplicationRecord(
        "C", tokenC, "C", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    sptr<IRemoteObject> tokenD = new MockAbilityToken();
    CreateTestApplicationRecord(
        "D", tokenD, "C", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_CALL(*(appC.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleCleanAbility(_)).Times(2);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenC, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(appC.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(tokenD, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(tokenA);
    serviceInner_->AbilityTerminated(tokenA);
    serviceInner_->TerminateAbility(tokenB);
    serviceInner_->AbilityTerminated(tokenB);
    serviceInner_->ApplicationTerminated(appA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appC.GetAbility(tokenC)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appC.GetAbility(tokenD)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appC.appRecord_->GetState());
    EXPECT_EQ(appA.GetAbility(tokenA), nullptr);
    EXPECT_EQ(appA.GetAbility(tokenB), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest BackKey_007 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ScreenOff
 * CaseDescription: when only one ability on foreground, simulate screenoff
 */
HWTEST_F(AmsWorkFlowTest, ScreenOff_001, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest ScreenOff_001 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest ScreenOff_001 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ScreenOff
 * CaseDescription: when multiple ability on foreground, simulate screenoff
 */
HWTEST_F(AmsWorkFlowTest, ScreenOff_002, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest ScreenOff_002 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenB)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest ScreenOff_002 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ScreenOff
 * CaseDescription: when one ability on foreground, another ability in same app is background, simulate screenoff
 */
HWTEST_F(AmsWorkFlowTest, ScreenOff_003, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest ScreenOff_003 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenB)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest ScreenOff_003 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ScreenOff
 * CaseDescription: when only one ability on foreground, simulate screenoff and exit
 */
HWTEST_F(AmsWorkFlowTest, ScreenOff_004, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest ScreenOff_004 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleCleanAbility(_)).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(tokenA);
    serviceInner_->AbilityTerminated(tokenA);
    serviceInner_->ApplicationTerminated(appA.appRecord_->GetRecordId());

    EXPECT_EQ(appA.GetAbility(tokenA), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest ScreenOff_004 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ScreenOff
 * CaseDescription: when multiple ability on foreground, simulate screenoff and exit
 */
HWTEST_F(AmsWorkFlowTest, ScreenOff_005, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest ScreenOff_005 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleCleanAbility(_)).Times(2);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(tokenA);
    serviceInner_->AbilityTerminated(tokenA);
    serviceInner_->TerminateAbility(tokenB);
    serviceInner_->AbilityTerminated(tokenB);
    serviceInner_->ApplicationTerminated(appA.appRecord_->GetRecordId());

    EXPECT_EQ(appA.GetAbility(tokenA), nullptr);
    EXPECT_EQ(appA.GetAbility(tokenB), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest ScreenOff_005 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ScreenOff
 * CaseDescription: when one ability on foreground, another ability in same app is background,
 *                  simulate screenoff and exit
 */
HWTEST_F(AmsWorkFlowTest, ScreenOff_006, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest ScreenOff_006 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleCleanAbility(_)).Times(2);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(tokenA);
    serviceInner_->AbilityTerminated(tokenA);
    serviceInner_->TerminateAbility(tokenB);
    serviceInner_->AbilityTerminated(tokenB);
    serviceInner_->ApplicationTerminated(appA.appRecord_->GetRecordId());

    EXPECT_EQ(appA.GetAbility(tokenA), nullptr);
    EXPECT_EQ(appA.GetAbility(tokenB), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest ScreenOff_006 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ScreenOn
 * CaseDescription: when only one ability on background, simulate screen on
 */
HWTEST_F(AmsWorkFlowTest, ScreenOn_001, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest ScreenOn_001 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(appA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest ScreenOn_001 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ScreenOn
 * CaseDescription: when multiple abilities on backgroud, previous is one ability, simulate screen on
 */
HWTEST_F(AmsWorkFlowTest, ScreenOn_002, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest ScreenOn_002 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(appA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenB)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest ScreenOn_002 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ScreenOn
 * CaseDescription: when multiple abilities on backgroud, all abilities are previous, simulate screen on
 */
HWTEST_F(AmsWorkFlowTest, ScreenOn_003, TestSize.Level0)
{
    APP_LOGI("AmsWorkFlowTest ScreenOn_003 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(appA.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_FOREGROUND);

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appA.GetAbility(tokenB)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appA.appRecord_->GetState());
    APP_LOGI("AmsWorkFlowTest ScreenOn_003 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ChangeAbility
 * CaseDescription: when one ability on foreground, request to load another ability of the same Application
 */
HWTEST_F(AmsWorkFlowTest, ChangeAbility_001, TestSize.Level0)
{
    APP_LOGD("AmsWorkFlowTest ChangeAbility_001 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_FOREGROUND);

    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);

    std::shared_ptr<AbilityRunningRecord> abilityA = appA.GetAbility(tokenA);
    std::shared_ptr<AbilityRunningRecord> abilityB = appA.GetAbility(tokenB);
    EXPECT_EQ(abilityA->GetAbilityInfo()->applicationName, abilityB->GetAbilityInfo()->applicationName);

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appA.GetAbility(tokenB)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appA.appRecord_->GetState());

    auto abilities = appA.appRecord_->GetAbilities();
    EXPECT_NE(nullptr, abilities[tokenA]);
    EXPECT_NE(nullptr, abilities[tokenB]);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ChangeAbility
 * CaseDescription: when two ability on foreground, request to load another ability of the same Application
 */
HWTEST_F(AmsWorkFlowTest, ChangeAbility_002, TestSize.Level0)
{
    APP_LOGD("AmsWorkFlowTest ChangeAbility_001 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenC = new MockAbilityToken();
    CreateTestApplicationRecord(
        "C", tokenC, "A", AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_FOREGROUND);

    serviceInner_->UpdateAbilityState(tokenC, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);

    std::shared_ptr<AbilityRunningRecord> abilityA = appA.GetAbility(tokenA);
    std::shared_ptr<AbilityRunningRecord> abilityB = appA.GetAbility(tokenB);
    std::shared_ptr<AbilityRunningRecord> abilityC = appA.GetAbility(tokenC);
    EXPECT_EQ(abilityA->GetAbilityInfo()->applicationName, abilityB->GetAbilityInfo()->applicationName);
    EXPECT_EQ(abilityC->GetAbilityInfo()->applicationName, abilityB->GetAbilityInfo()->applicationName);

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appA.GetAbility(tokenC)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenB)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appA.appRecord_->GetState());

    auto abilities = appA.appRecord_->GetAbilities();
    EXPECT_NE(nullptr, abilities[tokenA]);
    EXPECT_NE(nullptr, abilities[tokenB]);
    EXPECT_NE(nullptr, abilities[tokenC]);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ChangeAbility
 * CaseDescription: when one ability on foreground, request to load another ability of the another Application
 */
HWTEST_F(AmsWorkFlowTest, ChangeAbility_003, TestSize.Level0)
{
    APP_LOGD("AmsWorkFlowTest ChangeAbility_001 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    TestApplicationPreRecord appB = CreateTestApplicationRecord(
        "B", tokenB, "B", AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    EXPECT_CALL(*(appB.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(appB.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());

    std::shared_ptr<AbilityRunningRecord> abilityA = appA.GetAbility(tokenA);
    std::shared_ptr<AbilityRunningRecord> abilityB = appB.GetAbility(tokenB);
    EXPECT_NE(abilityA->GetAbilityInfo()->applicationName, abilityB->GetAbilityInfo()->applicationName);

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appB.GetAbility(tokenB)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appB.appRecord_->GetState());

    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, appA.appRecord_->GetState());

    auto abilitiesA = appA.appRecord_->GetAbilities();
    auto abilitiesB = appB.appRecord_->GetAbilities();
    EXPECT_NE(nullptr, abilitiesA[tokenA]);
    EXPECT_NE(nullptr, abilitiesB[tokenB]);
    pid_t pidA = appA.appRecord_->GetPriorityObject()->GetPid();
    pid_t pidB = appB.appRecord_->GetPriorityObject()->GetPid();
    EXPECT_NE(pidA, pidB);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ChangeAbility
 * CaseDescription: when two ability on foreground, request to load another ability of the another Application
 */
HWTEST_F(AmsWorkFlowTest, ChangeAbility_004, TestSize.Level0)
{
    APP_LOGD("AmsWorkFlowTest ChangeAbility_004 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    CreateTestApplicationRecord(
        "B", tokenB, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenC = new MockAbilityToken();
    TestApplicationPreRecord appC = CreateTestApplicationRecord(
        "C", tokenC, "C", AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    EXPECT_CALL(*(appC.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenC, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(appC.appRecord_->GetRecordId());

    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());

    std::shared_ptr<AbilityRunningRecord> abilityA = appA.GetAbility(tokenA);
    std::shared_ptr<AbilityRunningRecord> abilityB = appA.GetAbility(tokenB);
    std::shared_ptr<AbilityRunningRecord> abilityC = appC.GetAbility(tokenC);
    EXPECT_EQ(abilityA->GetAbilityInfo()->applicationName, abilityB->GetAbilityInfo()->applicationName);
    EXPECT_NE(abilityC->GetAbilityInfo()->applicationName, abilityB->GetAbilityInfo()->applicationName);

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appC.GetAbility(tokenC)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appC.appRecord_->GetState());

    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenB)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, appA.appRecord_->GetState());

    auto abilitiesA = appA.appRecord_->GetAbilities();
    auto abilitiesC = appC.appRecord_->GetAbilities();
    EXPECT_NE(nullptr, abilitiesA[tokenA]);
    EXPECT_NE(nullptr, abilitiesA[tokenB]);
    EXPECT_NE(nullptr, abilitiesC[tokenC]);

    pid_t pidA = appA.appRecord_->GetPriorityObject()->GetPid();
    pid_t pidC = appC.appRecord_->GetPriorityObject()->GetPid();
    EXPECT_NE(pidA, pidC);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: WorkFlow
 * FunctionPoints: ChangeAbility
 * CaseDescription: when a application on background, request to load another ability of the same Application
 */
HWTEST_F(AmsWorkFlowTest, ChangeAbility_005, TestSize.Level0)
{
    APP_LOGD("AmsWorkFlowTest ChangeAbility_004 start");
    sptr<IRemoteObject> tokenA = new MockAbilityToken();
    TestApplicationPreRecord appA = CreateTestApplicationRecord(
        "A", tokenA, "A", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    sptr<IRemoteObject> tokenB = new MockAbilityToken();
    TestApplicationPreRecord appB = CreateTestApplicationRecord(
        "B", tokenB, "B", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    sptr<IRemoteObject> tokenC = new MockAbilityToken();
    CreateTestApplicationRecord("C", tokenC, "B", AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    EXPECT_CALL(*(appA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(appB.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);

    serviceInner_->UpdateAbilityState(tokenC, AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(tokenB, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationForegrounded(appB.appRecord_->GetRecordId());

    serviceInner_->UpdateAbilityState(tokenA, AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(appA.appRecord_->GetRecordId());

    std::shared_ptr<AbilityRunningRecord> abilityB = appB.GetAbility(tokenB);
    std::shared_ptr<AbilityRunningRecord> abilityC = appB.GetAbility(tokenC);
    EXPECT_EQ(abilityC->GetAbilityInfo()->applicationName, abilityB->GetAbilityInfo()->applicationName);

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, appB.GetAbility(tokenC)->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appB.GetAbility(tokenB)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appB.appRecord_->GetState());

    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, appA.GetAbility(tokenA)->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, appA.appRecord_->GetState());

    auto abilitiesA = appA.appRecord_->GetAbilities();
    auto abilitiesB = appB.appRecord_->GetAbilities();
    EXPECT_NE(nullptr, abilitiesA[tokenA]);
    EXPECT_NE(nullptr, abilitiesB[tokenB]);
    EXPECT_NE(nullptr, abilitiesB[tokenC]);
}
}  // namespace AppExecFwk
}  // namespace OHOS
