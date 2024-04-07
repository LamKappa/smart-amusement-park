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

#include <unistd.h>

#include "iremote_object.h"
#include "refbase.h"

#include "app_launch_data.h"
#include "app_log_wrapper.h"
#include "app_mgr_service_inner.h"
#include "mock_ability_token.h"
#include "mock_app_scheduler.h"
#include "mock_app_spawn_client.h"

using namespace testing::ext;
using testing::_;
using testing::Return;
using testing::SetArgReferee;

namespace OHOS {
namespace AppExecFwk {
namespace {

const uint32_t CYCLE_NUMBER = 10;

}  // namespace

struct TestApplicationPreRunningRecord {
    TestApplicationPreRunningRecord(
        const std::shared_ptr<AppRunningRecord> &appRecord, const sptr<MockAppScheduler> &mockAppScheduler)
        : appRecord_(appRecord), mockAppScheduler_(mockAppScheduler)
    {}
    sptr<IRemoteObject> GetToken(const std::string &abilityName) const
    {
        auto abilityRecord = appRecord_->GetAbilityRunningRecord(abilityName);
        return abilityRecord ? abilityRecord->GetToken() : nullptr;
    }

    std::shared_ptr<AbilityRunningRecord> GetAbility(const std::string &abilityName) const
    {
        return appRecord_->GetAbilityRunningRecord(abilityName);
    }

    virtual ~TestApplicationPreRunningRecord()
    {}

    std::shared_ptr<AppRunningRecord> appRecord_;
    sptr<MockAppScheduler> mockAppScheduler_;
    static pid_t g_pid;
};

pid_t TestApplicationPreRunningRecord::g_pid = 0;

class AmsAppServiceFlowModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    sptr<MockAppScheduler> TestCreateApplicationClient(const std::shared_ptr<AppRunningRecord> &appRecord) const;
    TestApplicationPreRunningRecord TestCreateApplicationRecordAndSetState(const std::string &abilityName,
        const std::string &appName, const AbilityState abilityState, const ApplicationState appState) const;

protected:
    std::unique_ptr<AppMgrServiceInner> serviceInner_;
};

void AmsAppServiceFlowModuleTest::SetUpTestCase()
{}

void AmsAppServiceFlowModuleTest::TearDownTestCase()
{}

void AmsAppServiceFlowModuleTest::SetUp()
{
    serviceInner_.reset(new (std::nothrow) AppMgrServiceInner());
}

void AmsAppServiceFlowModuleTest::TearDown()
{}

sptr<MockAppScheduler> AmsAppServiceFlowModuleTest::TestCreateApplicationClient(
    const std::shared_ptr<AppRunningRecord> &appRecord) const
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
TestApplicationPreRunningRecord AmsAppServiceFlowModuleTest::TestCreateApplicationRecordAndSetState(
    const std::string &abilityName, const std::string &appName, const AbilityState abilityState,
    const ApplicationState appState) const
{
    RecordQueryResult result;
    AbilityInfo abilityInfo;
    ApplicationInfo appInfo;

    appInfo.name = appName;
    appInfo.bundleName = appName;  // specify process condition
    abilityInfo.name = abilityName;
    sptr<IRemoteObject> token = new (std::nothrow) MockAbilityToken();
    auto appRecord = serviceInner_->GetOrCreateAppRunningRecord(token,
        std::make_shared<ApplicationInfo>(appInfo),
        std::make_shared<AbilityInfo>(abilityInfo),
        appInfo.bundleName,
        0,
        result);
    EXPECT_NE(appRecord, nullptr);
    if (!result.appExists && appRecord) {
        appRecord->GetPriorityObject()->SetPid(TestApplicationPreRunningRecord::g_pid++);
    }
    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_NE(abilityRecord, nullptr);
    abilityRecord->SetState(abilityState);
    appRecord->SetState(appState);
    sptr<MockAppScheduler> mockAppScheduler = TestCreateApplicationClient(appRecord);
    TestApplicationPreRunningRecord testAppPreRecord(appRecord, mockAppScheduler);
    return testAppPreRecord;
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: BackKey
 * CaseDescription: when two abilities on foreground, previous ability in another app, simulate press back key.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_BackKey_001, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_BackKey_001 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    TestCreateApplicationRecordAndSetState("abilityA2",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_FOREGROUND,
        ApplicationState::APP_STATE_FOREGROUND);
    // The previous app and ability
    TestApplicationPreRunningRecord testAppB = TestCreateApplicationRecordAndSetState(
        "abilityB1", "appB", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    TestCreateApplicationRecordAndSetState("abilityB2",
        testAppB.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);

    // simulate press back key
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB1"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testAppB.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA2"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppB.GetAbility("abilityB1")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppB.GetAbility("abilityB2")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppB.appRecord_->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppA.GetAbility("abilityA1")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppA.GetAbility("abilityA2")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppA.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_BackKey_001 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: BackKey
 * CaseDescription: when two abilities on foreground, previous ability in another app, simulate press back key twice.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_BackKey_002, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_BackKey_002 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    TestApplicationPreRunningRecord testAppB = TestCreateApplicationRecordAndSetState(
        "abilityB1", "appB", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    TestApplicationPreRunningRecord testappC = TestCreateApplicationRecordAndSetState(
        "abilityC1", "appC", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    // simulate press back key
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB1"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testAppB.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppA.appRecord_->GetRecordId());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppB.GetAbility("abilityB1")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppB.appRecord_->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppA.GetAbility("abilityA1")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppA.appRecord_->GetState());

    EXPECT_CALL(*(testappC.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    // simulate press back key again
    serviceInner_->UpdateAbilityState(testappC.GetToken("abilityC1"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testappC.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppB.appRecord_->GetRecordId());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testappC.GetAbility("abilityC1")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testappC.appRecord_->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppA.GetAbility("abilityA1")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppA.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_BackKey_002 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: BackKey
 * CaseDescription: app have three abilities , previous ability in same app, simulate press back key.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_BackKey_003, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_BackKey_003 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    TestCreateApplicationRecordAndSetState("abilityA2",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_FOREGROUND);
    TestCreateApplicationRecordAndSetState("abilityA3",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_FOREGROUND);

    // simulate press back key
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA2"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA3"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppA.GetAbility("abilityA1")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppA.GetAbility("abilityA2")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppA.GetAbility("abilityA3")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppA.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_BackKey_003 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: BackKey
 * CaseDescription:app with two abilities on foreground, previous ability in another app, simulate press back and exit.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_BackKey_004, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_BackKey_004 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    TestCreateApplicationRecordAndSetState("abilityA2",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_FOREGROUND,
        ApplicationState::APP_STATE_FOREGROUND);
    // The previous app and ability
    TestApplicationPreRunningRecord testAppB = TestCreateApplicationRecordAndSetState(
        "abilityB1", "appB", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    TestCreateApplicationRecordAndSetState("abilityB2",
        testAppB.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleCleanAbility(_)).Times(2);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);

    // simulate press back key
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB2"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB1"), AbilityState::ABILITY_STATE_FOREGROUND);

    serviceInner_->ApplicationForegrounded(testAppB.appRecord_->GetRecordId());

    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA2"), AbilityState::ABILITY_STATE_BACKGROUND);

    serviceInner_->ApplicationBackgrounded(testAppA.appRecord_->GetRecordId());

    serviceInner_->TerminateAbility(testAppA.GetToken("abilityA1"));
    serviceInner_->AbilityTerminated(testAppA.GetToken("abilityA1"));
    serviceInner_->TerminateAbility(testAppA.GetToken("abilityA2"));
    serviceInner_->AbilityTerminated(testAppA.GetToken("abilityA2"));
    serviceInner_->ApplicationTerminated(testAppA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppB.GetAbility("abilityB1")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppB.GetAbility("abilityB2")->GetState());

    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppB.appRecord_->GetState());

    EXPECT_EQ(testAppA.GetAbility("abilityA1"), nullptr);
    EXPECT_EQ(testAppA.GetAbility("abilityA2"), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, testAppA.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_BackKey_004 end");
}

/*
 * Feature: AMS
 *     AR_AMS_ENV04_003
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: BackKey
 * CaseDescription: Three applications,only one on foreground, previous ability in another app,
                    simulate press back key twice.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_BackKey_005, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_BackKey_005 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    TestApplicationPreRunningRecord testAppB = TestCreateApplicationRecordAndSetState(
        "abilityB1", "appB", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    TestApplicationPreRunningRecord testappC = TestCreateApplicationRecordAndSetState(
        "abilityC1", "appC", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleCleanAbility(_)).Times(1);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);

    // simulate press back key, AppA to backgroud and exit.
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB1"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testAppB.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppA.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(testAppA.GetToken("abilityA1"));
    serviceInner_->AbilityTerminated(testAppA.GetToken("abilityA1"));
    serviceInner_->ApplicationTerminated(testAppA.appRecord_->GetRecordId());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppB.GetAbility("abilityB1")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppB.appRecord_->GetState());
    EXPECT_EQ(testAppA.GetAbility("abilityA1"), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, testAppA.appRecord_->GetState());

    EXPECT_CALL(*(testappC.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleCleanAbility(_)).Times(1);
    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);
    // simulate press back key again
    serviceInner_->UpdateAbilityState(testappC.GetToken("abilityC1"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testappC.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppB.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(testAppB.GetToken("abilityB1"));
    serviceInner_->AbilityTerminated(testAppB.GetToken("abilityB1"));
    serviceInner_->ApplicationTerminated(testAppB.appRecord_->GetRecordId());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testappC.GetAbility("abilityC1")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testappC.appRecord_->GetState());
    EXPECT_EQ(testAppB.GetAbility("abilityB1"), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, testAppB.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_BackKey_005 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: ScreenOff
 * CaseDescription: when two applications with two abilities on foreground, simulate press screenoff key.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_ScreenOff_001, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOff_001 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    // The previous app and ability
    TestApplicationPreRunningRecord testAppB = TestCreateApplicationRecordAndSetState(
        "abilityB1", "appB", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleCleanAbility(_)).Times(1);
    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleCleanAbility(_)).Times(1);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);

    // simulate press screenOff key
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppB.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(testAppB.GetToken("abilityB1"));
    serviceInner_->AbilityTerminated(testAppB.GetToken("abilityB1"));
    serviceInner_->ApplicationTerminated(testAppB.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppA.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(testAppA.GetToken("abilityA1"));
    serviceInner_->AbilityTerminated(testAppA.GetToken("abilityA1"));
    serviceInner_->ApplicationTerminated(testAppA.appRecord_->GetRecordId());

    EXPECT_EQ(testAppA.GetAbility("abilityA1"), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, testAppA.appRecord_->GetState());
    EXPECT_EQ(testAppB.GetAbility("abilityB1"), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, testAppB.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOff_001 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: ScreenOff
 * CaseDescription: when two applications with two abilities on foreground, simulate press screenoff key.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_ScreenOff_002, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOff_002 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    TestCreateApplicationRecordAndSetState("abilityA2",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_FOREGROUND,
        ApplicationState::APP_STATE_FOREGROUND);
    // The previous app and ability
    TestApplicationPreRunningRecord testAppB = TestCreateApplicationRecordAndSetState(
        "abilityB1", "appB", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    TestCreateApplicationRecordAndSetState("abilityB2",
        testAppB.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);

    // simulate press screenOff key
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB2"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppB.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA2"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppB.GetAbility("abilityB1")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppB.GetAbility("abilityB2")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppB.appRecord_->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppA.GetAbility("abilityA1")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppA.GetAbility("abilityA2")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppA.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOff_002 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: ScreenOff
 * CaseDescription: when two applications with two abilities on foreground, simulate press screenoff key.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_ScreenOff_003, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOff_003 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    TestCreateApplicationRecordAndSetState("abilityA2",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_FOREGROUND);
    // The previous app and ability
    TestApplicationPreRunningRecord testAppB = TestCreateApplicationRecordAndSetState(
        "abilityB1", "appB", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleCleanAbility(_)).Times(2);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);
    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleCleanAbility(_)).Times(1);
    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);

    // simulate press screenOff key
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppB.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(testAppB.GetToken("abilityB1"));
    serviceInner_->AbilityTerminated(testAppB.GetToken("abilityB1"));
    serviceInner_->ApplicationTerminated(testAppB.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppA.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(testAppA.GetToken("abilityA1"));
    serviceInner_->AbilityTerminated(testAppA.GetToken("abilityA1"));
    serviceInner_->TerminateAbility(testAppA.GetToken("abilityA2"));
    serviceInner_->AbilityTerminated(testAppA.GetToken("abilityA2"));
    serviceInner_->ApplicationTerminated(testAppA.appRecord_->GetRecordId());

    EXPECT_EQ(testAppA.GetAbility("abilityA1"), nullptr);
    EXPECT_EQ(testAppA.GetAbility("abilityA2"), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, testAppA.appRecord_->GetState());
    EXPECT_EQ(testAppB.GetAbility("abilityB1"), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, testAppB.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOff_003 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: ScreenOff
 * CaseDescription: when two applications with two abilities on foreground, simulate press screenoff key.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_ScreenOff_004, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOff_004 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    TestCreateApplicationRecordAndSetState("abilityA2",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_FOREGROUND);
    // The previous app and ability
    TestApplicationPreRunningRecord testAppB = TestCreateApplicationRecordAndSetState(
        "abilityB1", "appB", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    TestCreateApplicationRecordAndSetState("abilityB2",
        testAppB.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleCleanAbility(_)).Times(2);
    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);

    // simulate press screenOff key
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppB.appRecord_->GetRecordId());
    serviceInner_->TerminateAbility(testAppB.GetToken("abilityB1"));
    serviceInner_->AbilityTerminated(testAppB.GetToken("abilityB1"));
    serviceInner_->TerminateAbility(testAppB.GetToken("abilityB2"));
    serviceInner_->AbilityTerminated(testAppB.GetToken("abilityB2"));
    serviceInner_->ApplicationTerminated(testAppB.appRecord_->GetRecordId());
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppA.appRecord_->GetRecordId());

    EXPECT_EQ(testAppB.GetAbility("abilityB1"), nullptr);
    EXPECT_EQ(testAppB.GetAbility("abilityB2"), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_TERMINATED, testAppB.appRecord_->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppA.GetAbility("abilityA1")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppA.GetAbility("abilityA2")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppA.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOff_004 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: ScreenOn
 * CaseDescription: when an application with three abilities on foreground, simulate press screenon key.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_ScreenOn_001, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOn_001 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    TestCreateApplicationRecordAndSetState("abilityA2",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_BACKGROUND);
    TestCreateApplicationRecordAndSetState("abilityA3",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);

    // simulate press ScreenOn key
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testAppA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppA.GetAbility("abilityA1")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppA.GetAbility("abilityA2")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppA.GetAbility("abilityA3")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppA.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOn_001 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: ScreenOn
 * CaseDescription: when an application with three abilities on foreground, simulate press screenon key.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_ScreenOn_002, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOn_002 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    TestCreateApplicationRecordAndSetState("abilityA2",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_BACKGROUND);
    TestCreateApplicationRecordAndSetState("abilityA3",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);

    // simulate press ScreenOn key
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA2"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA3"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testAppA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppA.GetAbility("abilityA1")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppA.GetAbility("abilityA2")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppA.GetAbility("abilityA3")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppA.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOn_002 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: ScreenOn
 * CaseDescription: when an application with two abilities on foreground, simulate press screenon key.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_ScreenOn_003, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOn_003 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    TestCreateApplicationRecordAndSetState("abilityA2",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_BACKGROUND);
    TestApplicationPreRunningRecord testAppB = TestCreateApplicationRecordAndSetState(
        "abilityB1", "appB", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    TestCreateApplicationRecordAndSetState("abilityB2",
        testAppB.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    EXPECT_CALL(*(testAppB.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);

    // simulate press ScreenOn key
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA2"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB1"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(testAppB.GetToken("abilityB2"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testAppA.appRecord_->GetRecordId());
    serviceInner_->ApplicationForegrounded(testAppB.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppA.GetAbility("abilityA1")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppA.GetAbility("abilityA2")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppA.appRecord_->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppB.GetAbility("abilityB1")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppB.GetAbility("abilityB2")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppB.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOn_003 end");
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ServiceFlow
 * FunctionPoints: ScreenOn
 * CaseDescription: an application with two abilities on foreground, simulate press screenon and screenoff 1000 times.
 */
HWTEST_F(AmsAppServiceFlowModuleTest, ServiceFlow_ScreenOnAndOff_001, TestSize.Level0)
{
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOnAndOff_001 start");
    TestApplicationPreRunningRecord testAppA = TestCreateApplicationRecordAndSetState(
        "abilityA1", "appA", AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    TestCreateApplicationRecordAndSetState("abilityA2",
        testAppA.appRecord_->GetName(),
        AbilityState::ABILITY_STATE_BACKGROUND,
        ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleForegroundApplication()).Times(CYCLE_NUMBER + 1);
    EXPECT_CALL(*(testAppA.mockAppScheduler_), ScheduleBackgroundApplication()).Times(CYCLE_NUMBER);

    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        // simulate press ScreenOn key
        serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_FOREGROUND);
        serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA2"), AbilityState::ABILITY_STATE_FOREGROUND);
        serviceInner_->ApplicationForegrounded(testAppA.appRecord_->GetRecordId());

        // simulate press ScreenOff key
        serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_BACKGROUND);
        serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA2"), AbilityState::ABILITY_STATE_BACKGROUND);
        serviceInner_->ApplicationBackgrounded(testAppA.appRecord_->GetRecordId());
    }

    // simulate press ScreenOn key
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA1"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->UpdateAbilityState(testAppA.GetToken("abilityA2"), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testAppA.appRecord_->GetRecordId());

    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppA.GetAbility("abilityA1")->GetState());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppA.GetAbility("abilityA2")->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppA.appRecord_->GetState());
    APP_LOGI("AmsAppServiceFlowModuleTest ServiceFlow_ScreenOnAndOff_001 end");
}

}  // namespace AppExecFwk
}  // namespace OHOS
