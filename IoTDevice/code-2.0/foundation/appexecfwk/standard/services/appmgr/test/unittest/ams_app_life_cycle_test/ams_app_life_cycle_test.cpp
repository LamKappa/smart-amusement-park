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
#define private public
#include "app_mgr_service_inner.h"
#undef private

#include <unistd.h>
#include <gtest/gtest.h>
#include "iremote_object.h"
#include "refbase.h"
#include "app_launch_data.h"
#include "mock_ability_token.h"
#include "mock_app_scheduler.h"
#include "mock_app_spawn_client.h"
#include "mock_app_spawn_socket.h"
#include "mock_iapp_state_callback.h"
#include "mock_bundle_manager.h"

using namespace testing::ext;
using testing::_;
using testing::Return;
using testing::SetArgReferee;
namespace OHOS {
namespace AppExecFwk {
struct TestApplicationPreRecord {
    TestApplicationPreRecord(const std::shared_ptr<AbilityRunningRecord> &firstAbilityRecord,
        const std::shared_ptr<AppRunningRecord> &appRecord, const sptr<MockAppScheduler> &mockAppScheduler)
        : firstAbilityRecord_(firstAbilityRecord), appRecord_(appRecord), mockAppScheduler_(mockAppScheduler)
    {}
    virtual ~TestApplicationPreRecord()
    {}

    std::shared_ptr<AbilityRunningRecord> firstAbilityRecord_;
    std::shared_ptr<AppRunningRecord> appRecord_;
    sptr<MockAppScheduler> mockAppScheduler_ = nullptr;
};
class AmsAppLifeCycleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<AppRunningRecord> StartProcessAndLoadAbility(const sptr<IRemoteObject> &token,
        const sptr<IRemoteObject> &preToken, const std::shared_ptr<AbilityInfo> &abilityInfo,
        const std::shared_ptr<ApplicationInfo> &appInfo, const pid_t newPid) const;

    std::shared_ptr<AppRunningRecord> LoadTestAbility(const sptr<IRemoteObject> &token,
        const sptr<IRemoteObject> &preToken, const std::shared_ptr<AbilityInfo> &abilityInfo,
        const std::shared_ptr<ApplicationInfo> &appInfo);

    sptr<MockAbilityToken> GetMockToken() const;
    std::shared_ptr<AbilityInfo> GetAbilityInfoByIndex(const std::string &index) const;
    std::shared_ptr<ApplicationInfo> GetApplication() const;
    TestApplicationPreRecord PrepareLoadTestAbilityAndApp(const ApplicationState currentAppState) const;
    TestApplicationPreRecord CreateTestApplicationRecord(
        const AbilityState abilityState, const ApplicationState appState) const;
    std::shared_ptr<AppRunningRecord> CreateTestApplicationAndSetState(const ApplicationState appState) const;
    sptr<MockAppScheduler> AddApplicationClient(const std::shared_ptr<AppRunningRecord> &appRecord) const;
    void TestUpdateAbilityStateWhenAbilityIsUnLoaded(const AbilityState changingState) const;
    void TestUpdateAbilityStateWhenAbilityIsCreate(
        const AbilityState changingState, const ApplicationState curAppState) const;
    void TestUpdateAbilityStateToBackgroundWhenAbilityIsReady(const ApplicationState curAppState) const;
    void TestUpdateAbilityStateToBackgroundWhenAbilityIsBackground(const ApplicationState curAppState);
    void TestTerminateAbilityWhenAbilityIsNotBackground(
        const AbilityState curAbilityState, const ApplicationState curAppState) const;
    std::shared_ptr<AbilityRunningRecord> AddNewAbility(
        const std::shared_ptr<AppRunningRecord> &appRecord, const std::string &index) const;

protected:
    std::unique_ptr<AppMgrServiceInner> serviceInner_;
    sptr<MockAbilityToken> mock_token_ = nullptr;
    sptr<BundleMgrService> mockBundleMgr = nullptr;
};

void AmsAppLifeCycleTest::SetUpTestCase()
{}

void AmsAppLifeCycleTest::TearDownTestCase()
{}

void AmsAppLifeCycleTest::SetUp()
{
    serviceInner_.reset(new (std::nothrow) AppMgrServiceInner());
    mock_token_ = new (std::nothrow) MockAbilityToken();
    mockBundleMgr = new (std::nothrow) BundleMgrService();
    serviceInner_->SetBundleManager(mockBundleMgr);
}

void AmsAppLifeCycleTest::TearDown()
{}

std::shared_ptr<AppRunningRecord> AmsAppLifeCycleTest::StartProcessAndLoadAbility(const sptr<IRemoteObject> &token,
    const sptr<IRemoteObject> &preToken, const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<ApplicationInfo> &appInfo, const pid_t newPid) const
{
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(1).WillOnce(DoAll(SetArgReferee<1>(newPid), Return(ERR_OK)));

    serviceInner_->SetAppSpawnClient(mockClientPtr);

    serviceInner_->LoadAbility(token, preToken, abilityInfo, appInfo);
    std::shared_ptr<AppRunningRecord> record = serviceInner_->GetAppRunningRecordByAppName(appInfo->name);
    EXPECT_EQ(record->GetPriorityObject()->GetPid(), newPid);
    return record;
}

sptr<MockAbilityToken> AmsAppLifeCycleTest::GetMockToken() const
{
    return mock_token_;
}

std::shared_ptr<AbilityInfo> AmsAppLifeCycleTest::GetAbilityInfoByIndex(const std::string &index) const
{
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "test_ability" + index;
    abilityInfo->applicationName = "test_app";
    abilityInfo->process = "test_app";
    return abilityInfo;
}

std::shared_ptr<ApplicationInfo> AmsAppLifeCycleTest::GetApplication() const
{
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = "test_app";
    appInfo->bundleName = "test_app";
    return appInfo;
}

// load the first ability and create his parent app, then set app a state
TestApplicationPreRecord AmsAppLifeCycleTest::PrepareLoadTestAbilityAndApp(const ApplicationState currentAppState) const
{
    const pid_t NEW_PID = 123;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);
    EXPECT_NE(appRecord, nullptr);
    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_NE(abilityRecord, nullptr);
    EXPECT_EQ(AbilityState::ABILITY_STATE_CREATE, abilityRecord->GetState());
    appRecord->SetState(currentAppState);
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    appRecord->SetApplicationClient(client);
    TestApplicationPreRecord testAppPreRecord(abilityRecord, appRecord, mockAppScheduler);
    return testAppPreRecord;
}

// create one application that include one ability, and set state
TestApplicationPreRecord AmsAppLifeCycleTest::CreateTestApplicationRecord(
    const AbilityState abilityState, const ApplicationState appState) const
{
    RecordQueryResult result;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();

    auto appRecord = serviceInner_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, "test_app", 0, result);
    EXPECT_NE(appRecord, nullptr);
    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(GetMockToken());
    EXPECT_NE(abilityRecord, nullptr);
    abilityRecord->SetState(abilityState);
    appRecord->SetState(appState);
    sptr<MockAppScheduler> mockAppScheduler = AddApplicationClient(appRecord);

    TestApplicationPreRecord testAppPreRecord(abilityRecord, appRecord, mockAppScheduler);
    return testAppPreRecord;
}

std::shared_ptr<AppRunningRecord> AmsAppLifeCycleTest::CreateTestApplicationAndSetState(
    const ApplicationState appState) const
{
    RecordQueryResult result;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto appRecord =
        serviceInner_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, "AmsAppLifeCycleTest", 0, result);
    EXPECT_NE(appRecord, nullptr);
    EXPECT_EQ(appRecord->GetRecordId(), result.appRecordId);
    appRecord->SetState(appState);
    return appRecord;
}

sptr<MockAppScheduler> AmsAppLifeCycleTest::AddApplicationClient(
    const std::shared_ptr<AppRunningRecord> &appRecord) const
{
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    appRecord->SetApplicationClient(client);
    return mockAppScheduler;
}

void AmsAppLifeCycleTest::TestUpdateAbilityStateWhenAbilityIsUnLoaded(const AbilityState changingState) const
{
    sptr<IRemoteObject> token = GetMockToken();
    serviceInner_->UpdateAbilityState(token, changingState);
    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    EXPECT_EQ(appRecord, nullptr);
}

void AmsAppLifeCycleTest::TestUpdateAbilityStateWhenAbilityIsCreate(
    const AbilityState changingState, const ApplicationState curAppState) const
{
    auto testAppPreRecord = CreateTestApplicationRecord(AbilityState::ABILITY_STATE_CREATE, curAppState);
    auto abilityState = testAppPreRecord.firstAbilityRecord_->GetState();

    serviceInner_->UpdateAbilityState(GetMockToken(), changingState);
    EXPECT_EQ(abilityState, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(curAppState, testAppPreRecord.appRecord_->GetState());
}

void AmsAppLifeCycleTest::TestUpdateAbilityStateToBackgroundWhenAbilityIsReady(const ApplicationState curAppState) const
{
    auto testAppPreRecord = CreateTestApplicationRecord(AbilityState::ABILITY_STATE_READY, curAppState);

    serviceInner_->UpdateAbilityState(GetMockToken(), AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ(AbilityState::ABILITY_STATE_READY, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(curAppState, testAppPreRecord.appRecord_->GetState());
}

void AmsAppLifeCycleTest::TestUpdateAbilityStateToBackgroundWhenAbilityIsBackground(const ApplicationState curAppState)
{
    auto testAppPreRecord = CreateTestApplicationRecord(AbilityState::ABILITY_STATE_BACKGROUND, curAppState);

    serviceInner_->UpdateAbilityState(GetMockToken(), AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(curAppState, testAppPreRecord.appRecord_->GetState());
}

void AmsAppLifeCycleTest::TestTerminateAbilityWhenAbilityIsNotBackground(
    const AbilityState curAbilityState, const ApplicationState curAppState) const
{
    auto testAppPreRecord = CreateTestApplicationRecord(curAbilityState, curAppState);

    serviceInner_->TerminateAbility(GetMockToken());
    EXPECT_EQ(curAbilityState, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(curAppState, testAppPreRecord.appRecord_->GetState());
}

std::shared_ptr<AbilityRunningRecord> AmsAppLifeCycleTest::AddNewAbility(
    const std::shared_ptr<AppRunningRecord> &appRecord, const std::string &index) const
{
    auto newAbilityInfo = GetAbilityInfoByIndex(index);
    sptr<IRemoteObject> newToken = new (std::nothrow) MockAbilityToken();
    serviceInner_->LoadAbility(newToken, nullptr, newAbilityInfo, GetApplication());
    auto newAbilityRecord = appRecord->GetAbilityRunningRecordByToken(newToken);
    EXPECT_NE(newAbilityRecord, nullptr);
    return newAbilityRecord;
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Init
 * FunctionPoints: Init
 * CaseDescription: test application init process
 */
HWTEST_F(AmsAppLifeCycleTest, Init_001, TestSize.Level0)
{
    const pid_t NEW_PID = 123;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto record = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);

    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchApplication(_)).Times(1);
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchAbility(_, _)).Times(1);

    serviceInner_->AttachApplication(NEW_PID, client);
    EXPECT_NE(record->GetApplicationClient(), nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_READY, record->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: LoadAbility
 * CaseDescription: LoadAbility when ability is loaded.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_001, TestSize.Level0)
{
    const pid_t NEW_PID = 123;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);
    EXPECT_NE(appRecord, nullptr);
    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_NE(abilityRecord, nullptr);
    AbilityState abilityState = abilityRecord->GetState();
    ApplicationState appState = appRecord->GetState();

    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    EXPECT_EQ(abilityState, abilityRecord->GetState());
    EXPECT_EQ(appState, appRecord->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: LoadAbility
 * CaseDescription: LoadAbility when ability and application is not created.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_002, TestSize.Level0)
{
    const pid_t NEW_PID = 123;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);
    EXPECT_NE(appRecord, nullptr);
    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_NE(abilityRecord, nullptr);
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());

    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchApplication(_)).Times(1);
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchAbility(_, _)).Times(1);

    serviceInner_->AttachApplication(NEW_PID, client);
    EXPECT_EQ(AbilityState::ABILITY_STATE_READY, abilityRecord->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_READY, appRecord->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: LoadAbility
 * CaseDescription: Load ability when ability is not created but application is create.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_003, TestSize.Level0)
{
    TestApplicationPreRecord testAppRecord = PrepareLoadTestAbilityAndApp(ApplicationState::APP_STATE_CREATE);

    auto newAbilityRecord = AddNewAbility(testAppRecord.appRecord_, "1");
    EXPECT_EQ(AbilityState::ABILITY_STATE_CREATE, newAbilityRecord->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_CREATE, testAppRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: LoadAbility
 * CaseDescription: Load ability when ability is not created but application is ready.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_004, TestSize.Level0)
{
    TestApplicationPreRecord testAppRecord = PrepareLoadTestAbilityAndApp(ApplicationState::APP_STATE_READY);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleLaunchAbility(_, _)).Times(1);
    auto newAbilityRecord = AddNewAbility(testAppRecord.appRecord_, "1");
    EXPECT_EQ(AbilityState::ABILITY_STATE_READY, newAbilityRecord->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_READY, testAppRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: LoadAbility
 * CaseDescription: Load ability when ability is not created but application is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_005, TestSize.Level0)
{
    TestApplicationPreRecord testAppRecord = PrepareLoadTestAbilityAndApp(ApplicationState::APP_STATE_FOREGROUND);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleLaunchAbility(_, _)).Times(1);
    auto newAbilityRecord = AddNewAbility(testAppRecord.appRecord_, "1");
    EXPECT_EQ(AbilityState::ABILITY_STATE_READY, newAbilityRecord->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: LoadAbility
 * CaseDescription: Load ability when ability is not created but application is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_006, TestSize.Level0)
{
    TestApplicationPreRecord testAppRecord = PrepareLoadTestAbilityAndApp(ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleLaunchAbility(_, _)).Times(1);

    auto newAbilityRecord = AddNewAbility(testAppRecord.appRecord_, "1");
    EXPECT_EQ(AbilityState::ABILITY_STATE_READY, newAbilityRecord->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: LoadAbility
 * CaseDescription: Load ability when ability is not created but application is suspended.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_007, TestSize.Level0)
{
    TestApplicationPreRecord testAppRecord = PrepareLoadTestAbilityAndApp(ApplicationState::APP_STATE_SUSPENDED);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleLaunchAbility(_, _)).Times(1);
    auto newAbilityRecord = AddNewAbility(testAppRecord.appRecord_, "1");
    EXPECT_EQ(AbilityState::ABILITY_STATE_READY, newAbilityRecord->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to foreground
 * CaseDescription: Update ability state to foreground when ability is not loaded.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_008, TestSize.Level0)
{
    TestUpdateAbilityStateWhenAbilityIsUnLoaded(AbilityState::ABILITY_STATE_FOREGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to foreground
 * CaseDescription: Update ability state to foreground when ability and app is create.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_009, TestSize.Level0)
{
    TestUpdateAbilityStateWhenAbilityIsCreate(
        AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_CREATE);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to foreground
 * CaseDescription: Update ability state to foreground when ability is create but app is ready.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_010, TestSize.Level0)
{
    TestUpdateAbilityStateWhenAbilityIsCreate(
        AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_READY);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to foreground
 * CaseDescription: Update ability state to foreground when ability is create but app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_011, TestSize.Level0)
{
    TestUpdateAbilityStateWhenAbilityIsCreate(
        AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to foreground
 * CaseDescription: Update ability state to foreground when ability is create but app is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_012, TestSize.Level0)
{
    TestUpdateAbilityStateWhenAbilityIsCreate(
        AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_BACKGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to foreground
 * CaseDescription: Update ability state to foreground when ability and app is ready.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_013, TestSize.Level0)
{
    auto testAppPreRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    EXPECT_CALL(*(testAppPreRecord.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    serviceInner_->UpdateAbilityState(GetMockToken(), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testAppPreRecord.appRecord_->GetRecordId());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppPreRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to foreground
 * CaseDescription: Update ability state to foreground when ability is ready and app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_014, TestSize.Level0)
{
    auto testAppPreRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_FOREGROUND);

    serviceInner_->UpdateAbilityState(GetMockToken(), AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppPreRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to foreground
 * CaseDescription: Update ability state to foreground when ability is ready and app is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_015, TestSize.Level0)
{
    auto testAppPreRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppPreRecord.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    serviceInner_->UpdateAbilityState(GetMockToken(), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testAppPreRecord.appRecord_->GetRecordId());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppPreRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to foreground
 * CaseDescription: Update ability state to foreground when ability and app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_016, TestSize.Level0)
{
    auto testAppPreRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    serviceInner_->UpdateAbilityState(GetMockToken(), AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppPreRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to foreground
 * CaseDescription: Update ability state to foreground when ability is background and app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_017, TestSize.Level0)
{
    auto testAppPreRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_FOREGROUND);

    serviceInner_->UpdateAbilityState(GetMockToken(), AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppPreRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to foreground
 * CaseDescription: Update ability state to foreground when ability and app is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_018, TestSize.Level0)
{
    auto testAppPreRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppPreRecord.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    serviceInner_->UpdateAbilityState(GetMockToken(), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testAppPreRecord.appRecord_->GetRecordId());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppPreRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to foreground
 * CaseDescription: Update ability state to foreground when ability is background and app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_019, TestSize.Level0)
{
    auto testAppPreRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_SUSPENDED);

    EXPECT_CALL(*(testAppPreRecord.mockAppScheduler_), ScheduleForegroundApplication()).Times(1);
    serviceInner_->UpdateAbilityState(GetMockToken(), AbilityState::ABILITY_STATE_FOREGROUND);
    serviceInner_->ApplicationForegrounded(testAppPreRecord.appRecord_->GetRecordId());
    EXPECT_EQ(AbilityState::ABILITY_STATE_FOREGROUND, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppPreRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when ability is not loaded.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_020, TestSize.Level0)
{
    TestUpdateAbilityStateWhenAbilityIsUnLoaded(AbilityState::ABILITY_STATE_BACKGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when ability and app is create.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_021, TestSize.Level0)
{
    TestUpdateAbilityStateWhenAbilityIsCreate(
        AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_CREATE);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when ability is create but app is ready.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_022, TestSize.Level0)
{
    TestUpdateAbilityStateWhenAbilityIsCreate(
        AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_READY);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when ability is create but app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_023, TestSize.Level0)
{
    TestUpdateAbilityStateWhenAbilityIsCreate(
        AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_FOREGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when ability is create but app is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_024, TestSize.Level0)
{
    TestUpdateAbilityStateWhenAbilityIsCreate(
        AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when ability and app is ready.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_025, TestSize.Level0)
{
    TestUpdateAbilityStateToBackgroundWhenAbilityIsReady(ApplicationState::APP_STATE_READY);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when ability is ready and app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_026, TestSize.Level0)
{
    TestUpdateAbilityStateToBackgroundWhenAbilityIsReady(ApplicationState::APP_STATE_FOREGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when ability is ready and app is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_027, TestSize.Level0)
{
    TestUpdateAbilityStateToBackgroundWhenAbilityIsReady(ApplicationState::APP_STATE_BACKGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when ability and app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_028, TestSize.Level0)
{
    auto testAppPreRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    EXPECT_CALL(*(testAppPreRecord.mockAppScheduler_), ScheduleBackgroundApplication()).Times(1);
    serviceInner_->UpdateAbilityState(GetMockToken(), AbilityState::ABILITY_STATE_BACKGROUND);
    serviceInner_->ApplicationBackgrounded(testAppPreRecord.appRecord_->GetRecordId());
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppPreRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when multiple ability.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_029, TestSize.Level0)
{
    auto testAppPreRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    auto newAbilityInfo = GetAbilityInfoByIndex("1");
    sptr<IRemoteObject> newToken = new MockAbilityToken();
    auto newAbilityRecord = testAppPreRecord.appRecord_->AddAbility(newToken, newAbilityInfo);
    newAbilityRecord->SetState(AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_NE(newAbilityRecord, nullptr);

    serviceInner_->UpdateAbilityState(GetMockToken(), AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ(AbilityState::ABILITY_STATE_BACKGROUND, testAppPreRecord.firstAbilityRecord_->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppPreRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when ability is background and app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_030, TestSize.Level0)
{
    TestUpdateAbilityStateToBackgroundWhenAbilityIsBackground(ApplicationState::APP_STATE_FOREGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when ability is background and app is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_031, TestSize.Level0)
{
    TestUpdateAbilityStateToBackgroundWhenAbilityIsBackground(ApplicationState::APP_STATE_BACKGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Update ability state to background
 * CaseDescription: Update ability state to background when ability is background and app is suspended.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_032, TestSize.Level0)
{
    TestUpdateAbilityStateToBackgroundWhenAbilityIsBackground(ApplicationState::APP_STATE_SUSPENDED);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when ability is unload.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_033, TestSize.Level0)
{
    serviceInner_->TerminateAbility(GetMockToken());
    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(GetMockToken());
    EXPECT_EQ(appRecord, nullptr);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when ability and app is create.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_034, TestSize.Level0)
{
    TestTerminateAbilityWhenAbilityIsNotBackground(
        AbilityState::ABILITY_STATE_CREATE, ApplicationState::APP_STATE_CREATE);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when ability is create and app is ready.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_035, TestSize.Level0)
{
    TestTerminateAbilityWhenAbilityIsNotBackground(
        AbilityState::ABILITY_STATE_CREATE, ApplicationState::APP_STATE_READY);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when ability is create and app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_036, TestSize.Level0)
{
    TestTerminateAbilityWhenAbilityIsNotBackground(
        AbilityState::ABILITY_STATE_CREATE, ApplicationState::APP_STATE_FOREGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when ability is create and app is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_037, TestSize.Level0)
{
    TestTerminateAbilityWhenAbilityIsNotBackground(
        AbilityState::ABILITY_STATE_CREATE, ApplicationState::APP_STATE_BACKGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when ability and app is ready.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_038, TestSize.Level0)
{
    TestTerminateAbilityWhenAbilityIsNotBackground(
        AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when ability is ready and app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_039, TestSize.Level0)
{
    TestTerminateAbilityWhenAbilityIsNotBackground(
        AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_FOREGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when ability is ready and app is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_040, TestSize.Level0)
{
    TestTerminateAbilityWhenAbilityIsNotBackground(
        AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_BACKGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when ability and app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_041, TestSize.Level0)
{
    TestTerminateAbilityWhenAbilityIsNotBackground(
        AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when ability is background and app is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_042, TestSize.Level0)
{
    auto testAppRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleLaunchAbility(_, _)).Times(1);
    auto newAbilityRecord = AddNewAbility(testAppRecord.appRecord_, "1");
    newAbilityRecord->SetState(AbilityState::ABILITY_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleCleanAbility(_)).Times(1);
    serviceInner_->TerminateAbility(newAbilityRecord->GetToken());
    serviceInner_->AbilityTerminated(newAbilityRecord->GetToken());
    auto abilityRecord = testAppRecord.appRecord_->GetAbilityRunningRecordByToken(newAbilityRecord->GetToken());
    EXPECT_EQ(nullptr, abilityRecord);
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when ability and app is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_043, TestSize.Level0)
{
    auto testAppRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleCleanAbility(_)).Times(1);
    serviceInner_->TerminateAbility(GetMockToken());
    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);
    serviceInner_->AbilityTerminated(GetMockToken());
    auto abilityRecord = testAppRecord.appRecord_->GetAbilityRunningRecordByToken(GetMockToken());
    EXPECT_EQ(nullptr, abilityRecord);

    serviceInner_->ApplicationTerminated(testAppRecord.appRecord_->GetRecordId());
    auto appRecord = serviceInner_->GetAppRunningRecordByAppName(testAppRecord.appRecord_->GetName());
    EXPECT_EQ(nullptr, appRecord);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when multiple abilities and app is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_044, TestSize.Level0)
{
    auto testAppRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleLaunchAbility(_, _)).Times(1);
    auto newAbilityRecord = AddNewAbility(testAppRecord.appRecord_, "1");
    newAbilityRecord->SetState(AbilityState::ABILITY_STATE_BACKGROUND);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleCleanAbility(_)).Times(1);
    serviceInner_->TerminateAbility(GetMockToken());
    serviceInner_->AbilityTerminated(GetMockToken());
    auto abilityRecord = testAppRecord.appRecord_->GetAbilityRunningRecordByToken(GetMockToken());
    EXPECT_EQ(nullptr, abilityRecord);
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when ability is background and app is suspended.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_045, TestSize.Level0)
{
    auto testAppRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_SUSPENDED);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleCleanAbility(_)).Times(1);
    serviceInner_->TerminateAbility(GetMockToken());
    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);
    serviceInner_->AbilityTerminated(GetMockToken());
    auto abilityRecord = testAppRecord.appRecord_->GetAbilityRunningRecordByToken(GetMockToken());
    EXPECT_EQ(nullptr, abilityRecord);

    serviceInner_->ApplicationTerminated(testAppRecord.appRecord_->GetRecordId());
    auto appRecord = serviceInner_->GetAppRunningRecordByAppName(testAppRecord.appRecord_->GetName());
    EXPECT_EQ(nullptr, appRecord);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Terminate ability
 * CaseDescription: Terminate ability when multiple abilities is backgroung and app is suspended.
 */
HWTEST_F(AmsAppLifeCycleTest, Schedule_046, TestSize.Level0)
{
    auto testAppRecord =
        CreateTestApplicationRecord(AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_SUSPENDED);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleLaunchAbility(_, _)).Times(1);
    auto newAbilityRecord = AddNewAbility(testAppRecord.appRecord_, "1");
    newAbilityRecord->SetState(AbilityState::ABILITY_STATE_BACKGROUND);
    testAppRecord.appRecord_->SetState(ApplicationState::APP_STATE_SUSPENDED);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleCleanAbility(_)).Times(1);
    serviceInner_->TerminateAbility(GetMockToken());
    serviceInner_->AbilityTerminated(GetMockToken());
    auto abilityRecord = testAppRecord.appRecord_->GetAbilityRunningRecordByToken(GetMockToken());
    EXPECT_EQ(nullptr, abilityRecord);
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: ApplicationForegrounded
 * CaseDescription: Verify application can not change to foreground if application is create.
 */
HWTEST_F(AmsAppLifeCycleTest, Process_001, TestSize.Level0)
{
    auto appRecord = CreateTestApplicationAndSetState(ApplicationState::APP_STATE_CREATE);
    int32_t appRecordId = appRecord->GetRecordId();
    EXPECT_TRUE(appRecordId > 0);

    serviceInner_->ApplicationForegrounded(appRecordId);
    auto testAppRecord = serviceInner_->GetAppRunningRecordByAppRecordId(appRecordId);
    EXPECT_NE(nullptr, testAppRecord);
    EXPECT_EQ(ApplicationState::APP_STATE_CREATE, testAppRecord->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: ApplicationForegrounded
 * CaseDescription: Verify application can change to foreground if application is ready.
 */
HWTEST_F(AmsAppLifeCycleTest, Process_002, TestSize.Level0)
{
    auto appRecord = CreateTestApplicationAndSetState(ApplicationState::APP_STATE_READY);
    int32_t appRecordId = appRecord->GetRecordId();
    EXPECT_TRUE(appRecordId > 0);

    serviceInner_->ApplicationForegrounded(appRecordId);
    auto testAppRecord = serviceInner_->GetAppRunningRecordByAppRecordId(appRecordId);
    EXPECT_NE(nullptr, testAppRecord);
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppRecord->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: ApplicationForegrounded
 * CaseDescription: Verify application can not change to foreground if application is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Process_003, TestSize.Level0)
{
    auto appRecord = CreateTestApplicationAndSetState(ApplicationState::APP_STATE_FOREGROUND);
    int32_t appRecordId = appRecord->GetRecordId();
    EXPECT_TRUE(appRecordId > 0);

    serviceInner_->ApplicationForegrounded(appRecordId);
    auto testAppRecord = serviceInner_->GetAppRunningRecordByAppRecordId(appRecordId);
    EXPECT_NE(nullptr, testAppRecord);
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppRecord->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: ApplicationForegrounded
 * CaseDescription: Verify application can change to foreground if application is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Process_004, TestSize.Level0)
{
    auto appRecord = CreateTestApplicationAndSetState(ApplicationState::APP_STATE_BACKGROUND);
    int32_t appRecordId = appRecord->GetRecordId();
    EXPECT_TRUE(appRecordId > 0);

    serviceInner_->ApplicationForegrounded(appRecordId);
    auto testAppRecord = serviceInner_->GetAppRunningRecordByAppRecordId(appRecordId);
    EXPECT_NE(nullptr, testAppRecord);
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppRecord->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: ApplicationForegrounded
 * CaseDescription: Verify application can change to foregrounded if application is not exist.
 */
HWTEST_F(AmsAppLifeCycleTest, Process_005, TestSize.Level0)
{
    int32_t appRecordId = AppRecordId::Create();

    serviceInner_->ApplicationForegrounded(appRecordId);
    auto testAppRecord = serviceInner_->GetAppRunningRecordByAppRecordId(appRecordId);
    EXPECT_EQ(nullptr, testAppRecord);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: ApplicationBackgrounded
 * CaseDescription: Verify application can not change to background if application is create.
 */
HWTEST_F(AmsAppLifeCycleTest, Process_006, TestSize.Level0)
{
    auto appRecord = CreateTestApplicationAndSetState(ApplicationState::APP_STATE_CREATE);
    int32_t appRecordId = appRecord->GetRecordId();
    EXPECT_TRUE(appRecordId > 0);

    serviceInner_->ApplicationBackgrounded(appRecordId);
    auto testAppRecord = serviceInner_->GetAppRunningRecordByAppRecordId(appRecordId);
    EXPECT_NE(nullptr, testAppRecord);
    EXPECT_EQ(ApplicationState::APP_STATE_CREATE, testAppRecord->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: ApplicationBackgrounded
 * CaseDescription: Verify application can change to background if application is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Process_007, TestSize.Level0)
{
    auto appRecord = CreateTestApplicationAndSetState(ApplicationState::APP_STATE_FOREGROUND);
    int32_t appRecordId = appRecord->GetRecordId();
    EXPECT_TRUE(appRecordId > 0);

    serviceInner_->ApplicationBackgrounded(appRecordId);
    auto testAppRecord = serviceInner_->GetAppRunningRecordByAppRecordId(appRecordId);
    EXPECT_NE(nullptr, testAppRecord);
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppRecord->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: ApplicationBackgrounded
 * CaseDescription: Verify application can not change to background if application is ready.
 */
HWTEST_F(AmsAppLifeCycleTest, Process_008, TestSize.Level0)
{
    auto appRecord = CreateTestApplicationAndSetState(ApplicationState::APP_STATE_READY);
    int32_t appRecordId = appRecord->GetRecordId();
    EXPECT_TRUE(appRecordId > 0);

    serviceInner_->ApplicationBackgrounded(appRecord->GetRecordId());
    auto testAppRecord = serviceInner_->GetAppRunningRecordByAppRecordId(appRecord->GetRecordId());
    EXPECT_NE(nullptr, testAppRecord);
    EXPECT_EQ(ApplicationState::APP_STATE_READY, testAppRecord->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: ApplicationBackgrounded
 * CaseDescription: Verify application can not change to background if application is not exist.
 */
HWTEST_F(AmsAppLifeCycleTest, Process_009, TestSize.Level0)
{
    int32_t appRecordId = AppRecordId::Create();

    serviceInner_->ApplicationBackgrounded(appRecordId);
    auto testAppRecord = serviceInner_->GetAppRunningRecordByAppRecordId(appRecordId);
    EXPECT_EQ(nullptr, testAppRecord);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: ApplicationTerminated
 * CaseDescription: Verify application can change to terminate if application is background.
 */
HWTEST_F(AmsAppLifeCycleTest, Process_010, TestSize.Level0)
{
    auto testAppRecord = PrepareLoadTestAbilityAndApp(ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleTerminateApplication()).Times(1);
    serviceInner_->AbilityTerminated(GetMockToken());
    auto abilityRecord = testAppRecord.appRecord_->GetAbilityRunningRecordByToken(GetMockToken());
    EXPECT_EQ(nullptr, abilityRecord);

    serviceInner_->ApplicationTerminated(testAppRecord.appRecord_->GetRecordId());
    auto appRecord = serviceInner_->GetAppRunningRecordByAppRecordId(testAppRecord.appRecord_->GetRecordId());
    EXPECT_EQ(nullptr, appRecord);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: ApplicationTerminated
 * CaseDescription: Verify application can not change to terminate if application is foreground.
 */
HWTEST_F(AmsAppLifeCycleTest, Process_011, TestSize.Level0)
{
    auto appRecord = CreateTestApplicationAndSetState(ApplicationState::APP_STATE_FOREGROUND);
    int32_t appRecordId = appRecord->GetRecordId();
    EXPECT_TRUE(appRecordId > 0);

    serviceInner_->ApplicationTerminated(appRecordId);
    auto testAppRecord = serviceInner_->GetAppRunningRecordByAppRecordId(appRecordId);
    EXPECT_NE(nullptr, testAppRecord);
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppRecord->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: ApplicationTerminated
 * CaseDescription: Verify application can not change to foregrounded if application is not exist.
 */
HWTEST_F(AmsAppLifeCycleTest, Process_012, TestSize.Level0)
{
    int32_t appRecordId = AppRecordId::Create();
    serviceInner_->ApplicationTerminated(appRecordId);
    auto testAppRecord = serviceInner_->GetAppRunningRecordByAppRecordId(appRecordId);
    EXPECT_EQ(nullptr, testAppRecord);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: AppMgrService stop
 * CaseDescription: Verify if AppMgrService stop successfully.
 */
HWTEST_F(AmsAppLifeCycleTest, Stop_001, TestSize.Level0)
{
    const pid_t NEW_PID = 123;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);
    EXPECT_NE(appRecord, nullptr);
    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_NE(abilityRecord, nullptr);
    AbilityState abilityState = abilityRecord->GetState();
    ApplicationState appState = appRecord->GetState();
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    EXPECT_EQ(abilityState, abilityRecord->GetState());
    EXPECT_EQ(appState, appRecord->GetState());

    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, serviceInner_->QueryAppSpawnConnectionState());
    int32_t size = serviceInner_->GetRecordMap().size();
    EXPECT_EQ(1, size);
    serviceInner_->OnStop();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, serviceInner_->QueryAppSpawnConnectionState());
    size = serviceInner_->GetRecordMap().size();
    EXPECT_EQ(0, size);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: AppMgrService stop
 * CaseDescription: Verify if AppMgrService stop successfully.
 */
HWTEST_F(AmsAppLifeCycleTest, Stop_002, TestSize.Level0)
{
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    serviceInner_->SetAppSpawnClient(appSpawnClient);
    serviceInner_->OpenAppSpawnConnection();
    EXPECT_EQ(SpawnConnectionState::STATE_CONNECTED, serviceInner_->QueryAppSpawnConnectionState());
    int32_t size = serviceInner_->GetRecordMap().size();
    EXPECT_EQ(0, size);

    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(1);
    serviceInner_->OnStop();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, serviceInner_->QueryAppSpawnConnectionState());
    size = serviceInner_->GetRecordMap().size();
    EXPECT_EQ(0, size);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: AppMgrService stop
 * CaseDescription: Verify if AppMgrService stop successfully.
 */
HWTEST_F(AmsAppLifeCycleTest, Stop_003, TestSize.Level0)
{
    const pid_t NEW_PID = 123;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);
    EXPECT_NE(appRecord, nullptr);
    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_NE(abilityRecord, nullptr);
    AbilityState abilityState = abilityRecord->GetState();
    ApplicationState appState = appRecord->GetState();
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    EXPECT_EQ(abilityState, abilityRecord->GetState());
    EXPECT_EQ(appState, appRecord->GetState());

    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    serviceInner_->SetAppSpawnClient(appSpawnClient);
    serviceInner_->OpenAppSpawnConnection();
    EXPECT_EQ(SpawnConnectionState::STATE_CONNECTED, serviceInner_->QueryAppSpawnConnectionState());
    int32_t size = serviceInner_->GetRecordMap().size();
    EXPECT_EQ(1, size);

    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(1);
    serviceInner_->OnStop();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, serviceInner_->QueryAppSpawnConnectionState());
    size = serviceInner_->GetRecordMap().size();
    EXPECT_EQ(0, size);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Kill application
 * CaseDescription: Verify if AppMgrService Kill by appname fail.
 */
HWTEST_F(AmsAppLifeCycleTest, KillApplication_001, TestSize.Level0)
{
    int result = serviceInner_->KillApplication("hwei.ss.bb");
    EXPECT_EQ(ERR_OK, result);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Kill application
 * CaseDescription: Verify if AppMgrService Kill by appname successfully.
 */
HWTEST_F(AmsAppLifeCycleTest, KillApplication_002, TestSize.Level0)
{
    const pid_t NEW_PID = 123;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();

    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(1).WillOnce(DoAll(SetArgReferee<1>(NEW_PID), Return(ERR_OK)));

    serviceInner_->SetAppSpawnClient(mockClientPtr);

    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    std::shared_ptr<AppRunningRecord> appRecord = serviceInner_->GetAppRunningRecordByAppName(appInfo->name);
    EXPECT_EQ(appRecord->GetPriorityObject()->GetPid(), NEW_PID);

    pid_t pid = fork();
    if (pid > 0) {
        appRecord->GetPriorityObject()->SetPid(pid);
    }

    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    appRecord->SetApplicationClient(client);
    EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);

    int ret = serviceInner_->KillApplication(abilityInfo->applicationName);
    EXPECT_EQ(ERR_OK, ret);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Kill application
 * CaseDescription: Verify if AppMgrService Kill by pid successfully.
 */
HWTEST_F(AmsAppLifeCycleTest, KillProcessByPid001, TestSize.Level0)
{
    pid_t pid = fork();

    if (pid > 0) {
        int32_t ret = serviceInner_->KillProcessByPid(pid);
        EXPECT_EQ(ERR_OK, ret);
    }

    if (pid == 0) {
        int32_t ret = serviceInner_->KillProcessByPid(pid);
        EXPECT_EQ(-1, ret);
    }
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Kill application
 * CaseDescription: Verify if AppMgrService Kill by pid fail.
 */
HWTEST_F(AmsAppLifeCycleTest, KillProcessByPid002, TestSize.Level0)
{
    int32_t ret = serviceInner_->KillProcessByPid(-1);
    EXPECT_EQ(-1, ret);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Register App State Callback
 * CaseDescription: Verify if AppMgrService Register Callback.
 */
HWTEST_F(AmsAppLifeCycleTest, Callback001, TestSize.Level0)
{
    sptr<MockAppStateCallback> mockCallback(new MockAppStateCallback());
    sptr<IAppStateCallback> callback = iface_cast<IAppStateCallback>(mockCallback);
    serviceInner_->RegisterAppStateCallback(callback);

    EXPECT_CALL(*mockCallback, OnAppStateChanged(_)).Times(2);

    const pid_t NEW_PID = 123;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);
    EXPECT_NE(appRecord, nullptr);
    serviceInner_->OnAppStateChanged(appRecord, ApplicationState::APP_STATE_READY);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Register App State Callback
 * CaseDescription: Verify if AppMgrService Register Callback.
 */
HWTEST_F(AmsAppLifeCycleTest, Callback002, TestSize.Level0)
{
    sptr<MockAppStateCallback> mockCallback(new MockAppStateCallback());
    sptr<IAppStateCallback> callback = iface_cast<IAppStateCallback>(mockCallback);
    serviceInner_->RegisterAppStateCallback(callback);

    EXPECT_CALL(*mockCallback, OnAppStateChanged(_)).Times(0);

    serviceInner_->OnAppStateChanged(nullptr, ApplicationState::APP_STATE_READY);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Register Ability State Callback
 * CaseDescription: Verify if AppMgrService Register Callback.
 */
HWTEST_F(AmsAppLifeCycleTest, Callback003, TestSize.Level0)
{
    sptr<MockAppStateCallback> mockCallback(new MockAppStateCallback());
    sptr<IAppStateCallback> callback = iface_cast<IAppStateCallback>(mockCallback);

    const pid_t NEW_PID = 123;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);
    EXPECT_NE(appRecord, nullptr);
    auto ability = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_NE(ability, nullptr);

    serviceInner_->RegisterAppStateCallback(callback);
    EXPECT_CALL(*mockCallback, OnAbilityRequestDone(_, _)).Times(1);
    serviceInner_->OnAbilityStateChanged(ability, AbilityState::ABILITY_STATE_READY);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Register Ability State Callback
 * CaseDescription: Verify if AppMgrService Register Callback.
 */
HWTEST_F(AmsAppLifeCycleTest, Callback004, TestSize.Level0)
{
    sptr<MockAppStateCallback> mockCallback(new MockAppStateCallback());
    sptr<IAppStateCallback> callback = iface_cast<IAppStateCallback>(mockCallback);
    serviceInner_->RegisterAppStateCallback(callback);
    EXPECT_CALL(*mockCallback, OnAbilityRequestDone(_, _)).Times(0);
    serviceInner_->OnAbilityStateChanged(nullptr, AbilityState::ABILITY_STATE_READY);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Register Ability State Changed
 * CaseDescription: Verify if AppMgrService Ability State Changed Callback.
 */
HWTEST_F(AmsAppLifeCycleTest, AbilityStateChanged001, TestSize.Level0)
{
    sptr<MockAppStateCallback> mockCallback(new MockAppStateCallback());
    sptr<IAppStateCallback> callback = iface_cast<IAppStateCallback>(mockCallback);
    const pid_t NEW_PID = 123;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);
    EXPECT_NE(appRecord, nullptr);
    auto ability = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_NE(ability, nullptr);
    serviceInner_->RegisterAppStateCallback(callback);
    EXPECT_CALL(*mockCallback, OnAbilityRequestDone(_, _)).Times(1);
    serviceInner_->OnAbilityStateChanged(ability, AbilityState::ABILITY_STATE_BEGIN);
    EXPECT_CALL(*mockCallback, OnAbilityRequestDone(_, _)).Times(1);
    serviceInner_->OnAbilityStateChanged(ability, AbilityState::ABILITY_STATE_CREATE);
    EXPECT_CALL(*mockCallback, OnAbilityRequestDone(_, _)).Times(1);
    serviceInner_->OnAbilityStateChanged(ability, AbilityState::ABILITY_STATE_READY);
    EXPECT_CALL(*mockCallback, OnAbilityRequestDone(_, _)).Times(1);
    serviceInner_->OnAbilityStateChanged(ability, AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_CALL(*mockCallback, OnAbilityRequestDone(_, _)).Times(1);
    serviceInner_->OnAbilityStateChanged(ability, AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_CALL(*mockCallback, OnAbilityRequestDone(_, _)).Times(1);
    serviceInner_->OnAbilityStateChanged(ability, AbilityState::ABILITY_STATE_TERMINATED);
    EXPECT_CALL(*mockCallback, OnAbilityRequestDone(_, _)).Times(1);
    serviceInner_->OnAbilityStateChanged(ability, AbilityState::ABILITY_STATE_END);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: Schedule
 * FunctionPoints: Register App State Changed
 * CaseDescription: Verify if AppMgrService App State Changed Callback.
 */
HWTEST_F(AmsAppLifeCycleTest, AppStateChanged001, TestSize.Level0)
{
    sptr<MockAppStateCallback> mockCallback(new MockAppStateCallback());
    sptr<IAppStateCallback> callback = iface_cast<IAppStateCallback>(mockCallback);
    const pid_t NEW_PID = 123;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);
    EXPECT_NE(appRecord, nullptr);
    serviceInner_->RegisterAppStateCallback(callback);
    EXPECT_CALL(*mockCallback, OnAppStateChanged(_)).Times(1);
    serviceInner_->OnAppStateChanged(appRecord, ApplicationState::APP_STATE_BEGIN);
    EXPECT_CALL(*mockCallback, OnAppStateChanged(_)).Times(1);
    serviceInner_->OnAppStateChanged(appRecord, ApplicationState::APP_STATE_CREATE);
    EXPECT_CALL(*mockCallback, OnAppStateChanged(_)).Times(1);
    serviceInner_->OnAppStateChanged(appRecord, ApplicationState::APP_STATE_READY);
    EXPECT_CALL(*mockCallback, OnAppStateChanged(_)).Times(1);
    serviceInner_->OnAppStateChanged(appRecord, ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_CALL(*mockCallback, OnAppStateChanged(_)).Times(1);
    serviceInner_->OnAppStateChanged(appRecord, ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_CALL(*mockCallback, OnAppStateChanged(_)).Times(1);
    serviceInner_->OnAppStateChanged(appRecord, ApplicationState::APP_STATE_SUSPENDED);
    EXPECT_CALL(*mockCallback, OnAppStateChanged(_)).Times(1);
    serviceInner_->OnAppStateChanged(appRecord, ApplicationState::APP_STATE_TERMINATED);
    EXPECT_CALL(*mockCallback, OnAppStateChanged(_)).Times(1);
    serviceInner_->OnAppStateChanged(appRecord, ApplicationState::APP_STATE_END);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: UnsuspendApplication
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: test application state is APP_STATE_BACKGROUND
 */
HWTEST_F(AmsAppLifeCycleTest, Unsuspend_001, TestSize.Level0)
{
    TestApplicationPreRecord testAppRecord = PrepareLoadTestAbilityAndApp(ApplicationState::APP_STATE_FOREGROUND);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleLaunchAbility(_, _)).Times(1);
    auto newAbilityRecord = AddNewAbility(testAppRecord.appRecord_, "1");
    EXPECT_EQ(AbilityState::ABILITY_STATE_READY, newAbilityRecord->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppRecord.appRecord_->GetState());
    testAppRecord.appRecord_->SetState(ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppRecord.appRecord_->GetState());
    testAppRecord.appRecord_->SetState(ApplicationState::APP_STATE_SUSPENDED);
    EXPECT_EQ(ApplicationState::APP_STATE_SUSPENDED, testAppRecord.appRecord_->GetState());
    serviceInner_->UnsuspendApplication(testAppRecord.appRecord_);
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: UnsuspendApplication
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: test application state is APP_STATE_SUSPENDED(apprecord is nullptr)
 */
HWTEST_F(AmsAppLifeCycleTest, Unsuspend_002, TestSize.Level0)
{
    TestApplicationPreRecord testAppRecord = PrepareLoadTestAbilityAndApp(ApplicationState::APP_STATE_FOREGROUND);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleLaunchAbility(_, _)).Times(1);
    auto newAbilityRecord = AddNewAbility(testAppRecord.appRecord_, "1");
    EXPECT_EQ(AbilityState::ABILITY_STATE_READY, newAbilityRecord->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppRecord.appRecord_->GetState());
    testAppRecord.appRecord_->SetState(ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppRecord.appRecord_->GetState());
    testAppRecord.appRecord_->SetState(ApplicationState::APP_STATE_SUSPENDED);
    EXPECT_EQ(ApplicationState::APP_STATE_SUSPENDED, testAppRecord.appRecord_->GetState());
    serviceInner_->UnsuspendApplication(nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_SUSPENDED, testAppRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: suspendApplication
 * FunctionPoints: suspendApplication
 * CaseDescription: test application state is APP_STATE_SUSPENDED
 */
HWTEST_F(AmsAppLifeCycleTest, Suspend_001, TestSize.Level0)
{
    TestApplicationPreRecord testAppRecord = PrepareLoadTestAbilityAndApp(ApplicationState::APP_STATE_FOREGROUND);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleLaunchAbility(_, _)).Times(1);
    auto newAbilityRecord = AddNewAbility(testAppRecord.appRecord_, "1");
    EXPECT_EQ(AbilityState::ABILITY_STATE_READY, newAbilityRecord->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppRecord.appRecord_->GetState());
    testAppRecord.appRecord_->SetState(ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppRecord.appRecord_->GetState());
    testAppRecord.appRecord_->SetState(ApplicationState::APP_STATE_SUSPENDED);
    EXPECT_EQ(ApplicationState::APP_STATE_SUSPENDED, testAppRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: UnsuspendApplication
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: test application state is APP_STATE_BACKGROUND(apprecord is nullptr)
 */
HWTEST_F(AmsAppLifeCycleTest, Suspend_002, TestSize.Level0)
{
    TestApplicationPreRecord testAppRecord = PrepareLoadTestAbilityAndApp(ApplicationState::APP_STATE_FOREGROUND);

    EXPECT_CALL(*(testAppRecord.mockAppScheduler_), ScheduleLaunchAbility(_, _)).Times(1);
    auto newAbilityRecord = AddNewAbility(testAppRecord.appRecord_, "1");
    EXPECT_EQ(AbilityState::ABILITY_STATE_READY, newAbilityRecord->GetState());
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, testAppRecord.appRecord_->GetState());
    testAppRecord.appRecord_->SetState(ApplicationState::APP_STATE_BACKGROUND);
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppRecord.appRecord_->GetState());
    serviceInner_->UnsuspendApplication(nullptr);
    EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, testAppRecord.appRecord_->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: UnsuspendApplication
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: test application state is APP_STATE_BACKGROUND(apprecord is nullptr)
 */
HWTEST_F(AmsAppLifeCycleTest, AbilityBehaviorAnalysis_001, TestSize.Level0)
{
    const pid_t NEW_PID = 1234;
    auto abilityInfo = GetAbilityInfoByIndex("110");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);
    EXPECT_TRUE(appRecord != nullptr);

    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_TRUE(abilityRecord != nullptr);
    EXPECT_EQ(0, abilityRecord->GetVisibility());
    EXPECT_EQ(0, abilityRecord->GetPerceptibility());
    EXPECT_EQ(0, abilityRecord->GetConnectionState());

    EXPECT_TRUE(serviceInner_);
    serviceInner_->AbilityBehaviorAnalysis(token, nullptr, 1, 1, 1);

    auto abilityRecordAfter = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_TRUE(abilityRecordAfter != nullptr);
    EXPECT_EQ(1, abilityRecordAfter->GetVisibility());
    EXPECT_EQ(1, abilityRecordAfter->GetPerceptibility());
    EXPECT_EQ(1, abilityRecordAfter->GetConnectionState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: UnsuspendApplication
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: test application state is APP_STATE_BACKGROUND(apprecord is nullptr)
 */
HWTEST_F(AmsAppLifeCycleTest, AbilityBehaviorAnalysis_002, TestSize.Level0)
{
    const pid_t NEW_PID = 1234;
    auto abilityInfo = GetAbilityInfoByIndex("110");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();

    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);
    EXPECT_TRUE(appRecord != nullptr);

    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_TRUE(abilityRecord != nullptr);
    EXPECT_EQ(0, abilityRecord->GetVisibility());
    EXPECT_EQ(0, abilityRecord->GetPerceptibility());
    EXPECT_EQ(0, abilityRecord->GetConnectionState());

    EXPECT_TRUE(serviceInner_);
    serviceInner_->AbilityBehaviorAnalysis(nullptr, nullptr, 1, 1, 1);

    auto abilityRecordAfter = appRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_TRUE(abilityRecordAfter != nullptr);
    EXPECT_NE(1, abilityRecordAfter->GetVisibility());
    EXPECT_NE(1, abilityRecordAfter->GetPerceptibility());
    EXPECT_NE(1, abilityRecordAfter->GetConnectionState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: UnsuspendApplication
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: test application state is APP_STATE_BACKGROUND(apprecord is nullptr)
 */
HWTEST_F(AmsAppLifeCycleTest, ClearUpApplicationData_001, TestSize.Level0)
{
    const pid_t NEW_PID = 1234;
    auto abilityInfo = GetAbilityInfoByIndex("110");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();

    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, NEW_PID);

    // pid < 0
    appRecord->SetUid(101);
    EXPECT_CALL(*mockBundleMgr, GetUidByBundleName(_, _)).Times(0);
    serviceInner_->ClearUpApplicationData(appRecord->GetBundleName(), appRecord->GetUid(), 0);

    // uid < 0
    EXPECT_CALL(*mockBundleMgr, GetUidByBundleName(_, _)).Times(0);
    serviceInner_->ClearUpApplicationData(appRecord->GetBundleName(), -1, NEW_PID);

    EXPECT_CALL(*mockBundleMgr, GetUidByBundleName(_, _)).Times(1).WillOnce(Return(101));
    EXPECT_CALL(*mockBundleMgr, CheckPermission(_, _)).Times(1);
    EXPECT_CALL(*mockBundleMgr, CleanBundleDataFiles(_)).Times(1).WillOnce(Return(1));

    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    appRecord->SetApplicationClient(client);
    EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);

    serviceInner_->ClearUpApplicationData(appRecord->GetBundleName(), appRecord->GetUid(), NEW_PID);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle
 * SubFunction: ClearUpApplicationData
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: test application state is APP_STATE_BACKGROUND(apprecord is nullptr)
 */
HWTEST_F(AmsAppLifeCycleTest, ClearUpApplicationData_002, TestSize.Level0)
{
    auto abilityInfo = GetAbilityInfoByIndex("110");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();

    int32_t pid = fork();

    if (pid > 0) {
        auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, pid);

        EXPECT_CALL(*mockBundleMgr, GetUidByBundleName(_, _)).Times(1).WillOnce(Return(101));
        EXPECT_CALL(*mockBundleMgr, CheckPermission(_, _)).Times(1);
        EXPECT_CALL(*mockBundleMgr, CleanBundleDataFiles(_)).Times(1).WillOnce(Return(true));

        sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
        appRecord->SetApplicationClient(client);
        EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);
        serviceInner_->ClearUpApplicationData(appRecord->GetBundleName(), appRecord->GetUid(), pid);
    }
}

/*
 * Feature: AMS
 * Function: AppLifeCycle::IsBackgroundRunningRestricted
 * SubFunction: bundleMgr CheckPermission
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: Check if there is background operation permission
 */
HWTEST_F(AmsAppLifeCycleTest, IsBackgroundRunningRestricted_001, TestSize.Level0)
{
    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr);
    EXPECT_CALL(*bundleMgr, CheckPermission(_, _)).Times(1).WillOnce(Return(ERR_OK));
    EXPECT_EQ(ERR_OK, serviceInner_->IsBackgroundRunningRestricted("bundle"));
}

/*
 * Feature: AMS
 * Function: AppLifeCycle::GetOrCreateAppRunningRecord
 * SubFunction: bundleMgr CheckPermission
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: Check if there is background operation permission
 */
HWTEST_F(AmsAppLifeCycleTest, GetOrCreateAppRunningRecord_001, TestSize.Level0)
{
    RecordQueryResult result;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();

    auto appRecord = serviceInner_->GetOrCreateAppRunningRecord(nullptr, appInfo, abilityInfo, "test_app", 100, result);
    EXPECT_FALSE(appRecord);
    appRecord = serviceInner_->GetOrCreateAppRunningRecord(token, nullptr, abilityInfo, "test_app", 100, result);
    EXPECT_FALSE(appRecord);
    appRecord = serviceInner_->GetOrCreateAppRunningRecord(token, appInfo, nullptr, "test_app", 100, result);
    EXPECT_FALSE(appRecord);
    appRecord = serviceInner_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, "test_app", -1, result);
    EXPECT_FALSE(appRecord);
    appRecord = serviceInner_->GetOrCreateAppRunningRecord(
        token, appInfo, abilityInfo, "test_app", std::numeric_limits<int32_t>::max(), result);
    EXPECT_FALSE(appRecord);
    appRecord = serviceInner_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, "", 100, result);
    EXPECT_FALSE(appRecord);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle::GetOrCreateAppRunningRecord
 * SubFunction: bundleMgr CheckPermission
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: Check if there is background operation permission
 */
HWTEST_F(AmsAppLifeCycleTest, GetOrCreateAppRunningRecord_002, TestSize.Level0)
{
    RecordQueryResult result;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();

    // add one app
    auto appRecord =
        serviceInner_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, appInfo->name, 100, result);
    EXPECT_TRUE(appRecord);

    // add again
    RecordQueryResult result1;
    auto appRecord2 =
        serviceInner_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, appInfo->name, 100, result1);
    EXPECT_TRUE(appRecord2);
    EXPECT_TRUE(result1.appExists);
    EXPECT_TRUE(result1.abilityExists);
    EXPECT_EQ(appRecord->GetRecordId(), appRecord2->GetRecordId());

    // same token, new ability,
    RecordQueryResult result2;
    auto abilityInfo2 = GetAbilityInfoByIndex("1");
    auto appRecord3 =
        serviceInner_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo2, appInfo->name, 100, result2);
    EXPECT_TRUE(appRecord3);
    EXPECT_TRUE(result2.appExists);
    EXPECT_TRUE(result2.abilityExists);
    EXPECT_FALSE(appRecord3->GetAbilityRunningRecord(abilityInfo2->name));

    // new token, new ability
    RecordQueryResult result3;
    sptr<IRemoteObject> token2 = new MockAbilityToken();
    auto abilityInfo3 = GetAbilityInfoByIndex("2");
    auto appRecord4 =
        serviceInner_->GetOrCreateAppRunningRecord(token2, appInfo, abilityInfo3, appInfo->name, 100, result3);
    auto abilityRecord = appRecord4->GetAbilityRunningRecord(abilityInfo3->name);

    EXPECT_TRUE(appRecord4);
    EXPECT_TRUE(result3.appExists);
    EXPECT_FALSE(result3.abilityExists);
    EXPECT_TRUE(abilityRecord);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle::GetOrCreateAppRunningRecord
 * SubFunction: bundleMgr CheckPermission
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: Check if there is background operation permission
 */
HWTEST_F(AmsAppLifeCycleTest, GetAppRunningRecordByProcessName_001, TestSize.Level0)
{
    RecordQueryResult result;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();

    auto appRecord =
        serviceInner_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, appInfo->name, 100, result);
    EXPECT_TRUE(appRecord);

    auto appRecordProc = serviceInner_->GetAppRunningRecordByProcessName(appInfo->name, appInfo->name);
    EXPECT_TRUE(appRecordProc);

    EXPECT_EQ(appRecord->GetRecordId(), appRecordProc->GetRecordId());
    EXPECT_TRUE(appRecord->GetName() == appRecordProc->GetName());
    EXPECT_TRUE(appRecord->GetUid() == appRecordProc->GetUid());
    EXPECT_TRUE(appRecord->GetProcessName() == appRecordProc->GetProcessName());

    appRecord->SetState(ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_EQ(ApplicationState::APP_STATE_FOREGROUND, appRecordProc->GetState());
}

/*
 * Feature: AMS
 * Function: AppLifeCycle::GetOrCreateAppRunningRecord
 * SubFunction: bundleMgr CheckPermission
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: Check if there is background operation permission
 */
HWTEST_F(AmsAppLifeCycleTest, RemoveAppFromRecentList_001, TestSize.Level0)
{
    RecordQueryResult result;
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());

    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();
    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, 100);
    appRecord->SetApplicationClient(client);

    EXPECT_TRUE(appRecord);
    int size = serviceInner_->GetRecentAppList().size();
    EXPECT_EQ(size, 1);
    EXPECT_FALSE(result.appExists);

    auto abilityInfo2 = std::make_shared<AbilityInfo>();
    abilityInfo2->name = "test_ability_1";
    abilityInfo2->applicationName = "test_app_1";
    abilityInfo2->process = "test_app_1";

    auto appInfo2 = std::make_shared<ApplicationInfo>();
    appInfo2->name = "test_app_1";
    appInfo2->bundleName = "test_app_1";
    sptr<IRemoteObject> token2 = GetMockToken();

    auto appRecord2 = StartProcessAndLoadAbility(token, nullptr, abilityInfo2, appInfo2, 101);
    appRecord2->SetApplicationClient(client);
    EXPECT_TRUE(appRecord2);
    size = serviceInner_->GetRecentAppList().size();
    EXPECT_EQ(size, 2);
    EXPECT_FALSE(result.appExists);

    // remove the first
    EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);
    serviceInner_->RemoveAppFromRecentList(appRecord->GetName(), appRecord->GetName());

    size = serviceInner_->GetRecentAppList().size();
    EXPECT_EQ(size, 1);

    EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);
    serviceInner_->RemoveAppFromRecentList(appRecord2->GetName(), appRecord2->GetName());
    size = serviceInner_->GetRecentAppList().size();
    EXPECT_EQ(size, 0);
}

/*
 * Feature: AMS
 * Function: AppLifeCycle::GetOrCreateAppRunningRecord
 * SubFunction: bundleMgr CheckPermission
 * FunctionPoints: UnsuspendApplication
 * CaseDescription: Check if there is background operation permission
 */
HWTEST_F(AmsAppLifeCycleTest, GetAbilityRunningRecordByAbilityToken_001, TestSize.Level0)
{
    // RecordQueryResult result;
    auto abilityInfo = GetAbilityInfoByIndex("0");
    auto appInfo = GetApplication();
    sptr<IRemoteObject> token = GetMockToken();

    auto abilityRecord = serviceInner_->GetAbilityRunningRecordByAbilityToken(nullptr);
    EXPECT_FALSE(abilityRecord);

    abilityRecord = serviceInner_->GetAbilityRunningRecordByAbilityToken(token);
    EXPECT_FALSE(abilityRecord);
    auto appRecord = StartProcessAndLoadAbility(token, nullptr, abilityInfo, appInfo, 1234);
    EXPECT_TRUE(appRecord);

    abilityRecord = serviceInner_->GetAbilityRunningRecordByAbilityToken(token);
    sptr<IRemoteObject> token2 = new MockAbilityToken();
    auto abilityRecord2 = serviceInner_->GetAbilityRunningRecordByAbilityToken(token2);
    EXPECT_FALSE(abilityRecord2);
}

}  // namespace AppExecFwk
}  // namespace OHOS