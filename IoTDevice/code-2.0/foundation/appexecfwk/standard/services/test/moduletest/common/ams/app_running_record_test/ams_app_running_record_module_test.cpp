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
#include "app_running_record.h"
#include <gtest/gtest.h>
#include <vector>
#include "iremote_object.h"
#include "app_record_id.h"
#include "app_scheduler_proxy.h"
#include "app_scheduler_host.h"
#include "app_mgr_service_inner.h"
#include "mock_application.h"
#include "ability_info.h"
#include "application_info.h"
#include "mock_ability_token.h"

using namespace testing::ext;
using OHOS::iface_cast;
using OHOS::IRemoteObject;
using OHOS::sptr;
using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;

namespace OHOS {
namespace AppExecFwk {

class AmsAppRunningRecordModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::string GetTestAppName(const unsigned long num) const
    {
        if (num < appName_.size()) {
            return appName_[num];
        }
        return "";
    }

    std::string GetTestAbilityName(const unsigned long num) const
    {
        if (num < abilityName_.size()) {
            return abilityName_[num];
        }
        return "";
    }

    void CheckLaunchApplication(const sptr<MockApplication> &mockApplication, const unsigned long index,
        std::shared_ptr<AppRunningRecord> record, const std::string &testPoint) const
    {
        ASSERT_TRUE(record != nullptr) << "record is nullptr!";
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);
        record->SetApplicationClient(client);

        std::string applicationName(GetTestAppName(index));
        ApplicationInfo info;
        info.name = applicationName;
        std::string processInfoName(GetTestAppName(index));
        pid_t pidId = 123;
        ProcessInfo processInfo(processInfoName, pidId);

        AppLaunchData launchData;
        launchData.SetApplicationInfo(info);
        launchData.SetProcessInfo(processInfo);

        EXPECT_CALL(*mockApplication, ScheduleLaunchApplication(_))
            .Times(1)
            .WillOnce(Invoke(mockApplication.GetRefPtr(), &MockApplication::LaunchApplication));
        record->LaunchApplication();
        mockApplication->Wait();

        bool isEqual = mockApplication->CompareAppLaunchData(launchData);
        EXPECT_EQ(isEqual, true) << testPoint << ",fail";
    }

    void CheckAppRunningRecording(const std::shared_ptr<ApplicationInfo> appInfo,
        const std::shared_ptr<AbilityInfo> abilityInfo, const std::shared_ptr<AppRunningRecord> record, const int index,
        RecordQueryResult &result) const
    {
        ASSERT_TRUE(service_ != nullptr) << "init service fail!";
        ASSERT_TRUE(appInfo != nullptr) << "appInfo is nullptr!";
        ASSERT_TRUE(abilityInfo != nullptr) << "abilityInfo is nullptr!";
        ASSERT_TRUE(record != nullptr) << "record is nullptr!";
        auto abilityRecord = record->GetAbilityRunningRecord(GetTestAbilityName(index));
        int32_t id = record->GetRecordId();
        auto name = record->GetName();
        sptr<IRemoteObject> token = abilityRecord->GetToken();
        auto abilityName = abilityRecord->GetName();
        std::string processName = GetTestAppName(index);

        auto appRecordFromServ =
            service_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, processName, 0, result);
        EXPECT_TRUE(appRecordFromServ);
        auto abilityRecordFromServ = appRecordFromServ->GetAbilityRunningRecord(GetTestAbilityName(index));
        int32_t idFromServ = appRecordFromServ->GetRecordId();
        sptr<IRemoteObject> tokenFromServ = abilityRecordFromServ->GetToken();
        auto nameFromServ = appRecordFromServ->GetName();
        auto abilityNameFromServ = abilityRecordFromServ->GetName();
        ASSERT_TRUE(result.appExists) << "result is wrong!";
        EXPECT_TRUE(id == idFromServ) << "fail, RecordId is not equal!";
        EXPECT_TRUE(tokenFromServ.GetRefPtr() == token.GetRefPtr()) << "fail, token is not equal!";
        EXPECT_EQ(name, nameFromServ) << "fail, app record name is not equal!";
        EXPECT_EQ(abilityName, abilityNameFromServ) << "fail, app record name is not equal!";
    }

    std::unique_ptr<AppMgrServiceInner> service_;

    sptr<MockAbilityToken> GetMockToken() const
    {
        return mockToken_;
    }

private:
    std::vector<std::string> appName_ = {
        "test_app_name1", "test_app_name2", "test_app_name3", "test_app_name4", "test_app_name5"};
    std::vector<std::string> abilityName_ = {
        "test_ability_name1", "test_ability_name2", "test_ability_name3", "test_ability_name4", "test_ability_name5"};
    sptr<MockAbilityToken> mockToken_;
};

void AmsAppRunningRecordModuleTest::SetUpTestCase()
{}

void AmsAppRunningRecordModuleTest::TearDownTestCase()
{}

void AmsAppRunningRecordModuleTest::SetUp()
{
    service_.reset(new (std::nothrow) AppMgrServiceInner());
    mockToken_ = new (std::nothrow) MockAbilityToken();
}

void AmsAppRunningRecordModuleTest::TearDown()
{}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: ApprunningRecord
 * FunctionPoints: run application and use Iapplication LaunchApplication and
 *                 ScheduleForegroundRunning interface to control application,
 *                 check whether function is good or not
 * EnvConditions: system running normally
 * CaseDescription: 1. start application
 *                  2. use Iapplication LaunchApplication and ScheduleForegroundRunning interface
 */
HWTEST_F(AmsAppRunningRecordModuleTest, ApplicationStart_001, TestSize.Level0)
{
    // init AppRunningRecord
    unsigned long index = 0L;
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName(index);
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName(index);
    std::string processName = GetTestAppName(index);
    RecordQueryResult result;
    auto record = service_->GetOrCreateAppRunningRecord(GetMockToken(), appInfo, abilityInfo, processName, 0, result);
    ASSERT_TRUE(record != nullptr) << ",create apprunningrecord fail!";
    ASSERT_FALSE(result.appExists) << ",result is wrong!";

    // check apprunningrecord
    int32_t id = record->GetRecordId();
    CheckAppRunningRecording(appInfo, abilityInfo, record, index, result);

    // LaunchApplication
    sptr<MockApplication> mockApplication(new MockApplication());
    std::string testPoint = "ApplicationStart_001";
    CheckLaunchApplication(mockApplication, index, record, testPoint);

    EXPECT_CALL(*mockApplication, ScheduleForegroundApplication())
        .Times(1)
        .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
    // application enter in foreground and check the result
    record->ScheduleForegroundRunning();
    mockApplication->Wait();

    // update application state and check the state
    record->SetState(ApplicationState::APP_STATE_FOREGROUND);
    RecordQueryResult newResult;
    auto newRecord =
        service_->GetOrCreateAppRunningRecord(GetMockToken(), appInfo, abilityInfo, processName, 0, newResult);
    EXPECT_TRUE(newRecord);
    auto stateFromRec = newRecord->GetState();
    ASSERT_TRUE(newResult.appExists) << "fail, app is not exist!";
    EXPECT_EQ(stateFromRec, ApplicationState::APP_STATE_FOREGROUND);

    // clear apprunningrecord
    record->SetState(ApplicationState::APP_STATE_BACKGROUND);
    service_->ApplicationTerminated(id);
    record = service_->GetAppRunningRecordByAppRecordId(id);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: ApprunningRecord
 * FunctionPoints: run multi application at the same time, then use Iapplication
 *                 LaunchApplication and ScheduleForegroundRunning interface to control
 *                 application, check whether function is good or not
 * EnvConditions: system running normally
 * CaseDescription: 1. start 5 application at the same time
 *                  2. use Iapplication LaunchApplication and ScheduleForegroundRunning interface
 */
HWTEST_F(AmsAppRunningRecordModuleTest, MultiApplicationStart_002, TestSize.Level1)
{
    unsigned long count = 5;
    for (unsigned long i = 0; i < count; i++) {
        // init AppRunningRecord
        auto abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = GetTestAbilityName(i);
        std::string processName = GetTestAppName(i);
        auto appInfo = std::make_shared<ApplicationInfo>();
        appInfo->name = GetTestAppName(i);
        RecordQueryResult result;
        auto record =
            service_->GetOrCreateAppRunningRecord(GetMockToken(), appInfo, abilityInfo, processName, 0, result);
        ASSERT_TRUE(record != nullptr) << "create apprunningrecord fail!";
        ASSERT_FALSE(result.appExists) << "result is wrong!";

        // check abilityrunningrecord & apprunningrecord
        int32_t id = record->GetRecordId();
        CheckAppRunningRecording(appInfo, abilityInfo, record, i, result);

        // LaunchApplication
        sptr<MockApplication> mockApplication(new MockApplication());
        std::string testPoint = "multiApplicationStart_002";
        CheckLaunchApplication(mockApplication, i, record, testPoint);

        // clear apprunningrecord
        record->SetState(ApplicationState::APP_STATE_BACKGROUND);
        service_->ApplicationTerminated(id);
        record = service_->GetAppRunningRecordByAppRecordId(id);
        EXPECT_EQ(nullptr, record);
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: ApprunningRecord
 * FunctionPoints: run application, then use Iapplication LaunchApplication and ScheduleTrimMemory
 *                 interface to control application,check whether function is good or not
 * EnvConditions: system running normally
 * CaseDescription: 1. start application
 *                  2. use Iapplication LaunchApplication and ScheduleTrimMemory interface
 */
HWTEST_F(AmsAppRunningRecordModuleTest, ScheduleTrimMemory_003, TestSize.Level1)
{
    ASSERT_TRUE(service_ != nullptr) << "init service fail!";

    // init AppRunningRecord
    unsigned long index = 0;
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName(index);
    std::string processName = GetTestAppName(index);
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName(index);
    RecordQueryResult result;
    auto record = service_->GetOrCreateAppRunningRecord(GetMockToken(), appInfo, abilityInfo, processName, 0, result);
    ASSERT_TRUE(record != nullptr) << "create apprunningrecord fail!";
    ASSERT_FALSE(result.appExists) << "result is wrong!";

    // LaunchApplication
    sptr<MockApplication> mockApplication(new MockApplication());
    std::string testPoint = "ScheduleTrimMemory_003";
    CheckLaunchApplication(mockApplication, index, record, testPoint);

    EXPECT_CALL(*mockApplication, ScheduleShrinkMemory(_))
        .Times(1)
        .WillOnce(Invoke(mockApplication.GetRefPtr(), &MockApplication::ShrinkMemory));
    // set ScheduleTrimMemory API
    record->ScheduleTrimMemory();
    mockApplication->Wait();
    int level = mockApplication->GetShrinkLevel();
    EXPECT_EQ(level, 0) << "fail, level is wrong!";

    // clear apprunningrecord
    record->SetState(ApplicationState::APP_STATE_BACKGROUND);
    int32_t id = record->GetRecordId();
    service_->ApplicationTerminated(id);
    record = service_->GetAppRunningRecordByAppRecordId(id);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: ApprunningRecord
 * FunctionPoints: run application, then use Iapplication LaunchApplication and LowMemoryWarning
 *                 interface to control application,check whether function is good or not
 * EnvConditions: system running normally
 * CaseDescription: 1. start application
 *                  2. use Iapplication LaunchApplication and LowMemoryWarning interface
 */
HWTEST_F(AmsAppRunningRecordModuleTest, LowMemoryWarning_004, TestSize.Level1)
{
    ASSERT_TRUE(service_ != nullptr) << "init service fail!";

    // init AppRunningRecord
    unsigned long index = 0;
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName(index);
    std::string processName = GetTestAppName(index);
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName(index);
    RecordQueryResult result;
    auto record = service_->GetOrCreateAppRunningRecord(GetMockToken(), appInfo, abilityInfo, processName, 0, result);
    ASSERT_TRUE(record != nullptr) << "create apprunningrecord fail!";
    ASSERT_FALSE(result.appExists) << "result is wrong!";

    // LaunchApplication
    sptr<MockApplication> mockApplication(new MockApplication());
    std::string testPoint = "LowMemoryWarning_004";
    CheckLaunchApplication(mockApplication, index, record, testPoint);

    EXPECT_CALL(*mockApplication, ScheduleLowMemory())
        .Times(1)
        .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
    // set LowMemoryWarning
    record->LowMemoryWarning();
    mockApplication->Wait();

    // clear apprunningrecord
    record->SetState(ApplicationState::APP_STATE_BACKGROUND);
    int32_t id = record->GetRecordId();
    service_->ApplicationTerminated(id);
    record = service_->GetAppRunningRecordByAppRecordId(id);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: ApprunningRecord
 * FunctionPoints: run application and quit repeatly, then check whether function is good or not
 * EnvConditions: system running normally
 * CaseDescription: 1. start application
 *                  2. use Iapplication LaunchApplication and ScheduleForegroundRunning interface
 *                  3. use ScheduleBackgroundRunning interface
 *                  4. use ScheduleTerminate and OnApplicationTerminated to stop application
 *                  5. repeat 1~4 10000 times
 */
HWTEST_F(AmsAppRunningRecordModuleTest, ApplicationStartAndQuit_005, TestSize.Level2)
{
    ASSERT_TRUE(service_ != nullptr) << "init service fail!";

    unsigned long index = 0;
    int startCount = 10000;
    for (int i = 0; i < startCount; i++) {
        // init AppRunningRecord
        auto abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = GetTestAbilityName(index);
        std::string processName = GetTestAppName(index);
        auto appInfo = std::make_shared<ApplicationInfo>();
        appInfo->name = GetTestAppName(index);
        RecordQueryResult result;
        auto record =
            service_->GetOrCreateAppRunningRecord(GetMockToken(), appInfo, abilityInfo, processName, 0, result);
        ASSERT_TRUE(record != nullptr) << "create apprunningrecord fail!";

        // LaunchApplication
        sptr<MockApplication> mockApplication(new MockApplication());
        std::string testPoint = "ApplicationStartAndQuit_005";
        CheckLaunchApplication(mockApplication, index, record, testPoint);

        EXPECT_CALL(*mockApplication, ScheduleForegroundApplication())
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
        // set foreground and update foreground state
        record->ScheduleForegroundRunning();
        mockApplication->Wait();
        record->SetState(ApplicationState::APP_STATE_FOREGROUND);
        auto stateFromRec = record->GetState();
        EXPECT_EQ(stateFromRec, ApplicationState::APP_STATE_FOREGROUND);

        EXPECT_CALL(*mockApplication, ScheduleBackgroundApplication())
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
        // set background and update foreground state
        record->ScheduleBackgroundRunning();
        mockApplication->Wait();
        record->SetState(ApplicationState::APP_STATE_BACKGROUND);
        stateFromRec = record->GetState();
        EXPECT_EQ(stateFromRec, ApplicationState::APP_STATE_BACKGROUND);

        EXPECT_CALL(*mockApplication, ScheduleTerminateApplication())
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
        // set application terminate
        record->ScheduleTerminate();
        mockApplication->Wait();

        // clear apprunnningrecord
        int32_t id = record->GetRecordId();
        service_->ApplicationTerminated(id);
        record = service_->GetAppRunningRecordByAppRecordId(id);
        EXPECT_EQ(nullptr, record);
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: ApprunningRecord
 * FunctionPoints: run application, make application foreground and background repeatly,
 *                 then check whether function is good or not
 * EnvConditions: system running normally
 * CaseDescription: 1. start application
 *                  2. use Iapplication LaunchApplication and ScheduleForegroundRunning interface
 *                  3. use ScheduleBackgroundRunning and SetState to update state
 *                  4. repeat 1~4 10000 times
 */
HWTEST_F(AmsAppRunningRecordModuleTest, ApplicationStatusChange_006, TestSize.Level2)
{
    ASSERT_TRUE(service_ != nullptr) << "init service fail!";

    // init AppRunningRecord
    unsigned long index = 0;
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName(index);
    std::string processName = GetTestAppName(index);
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName(index);
    RecordQueryResult result;
    auto record = service_->GetOrCreateAppRunningRecord(GetMockToken(), appInfo, abilityInfo, processName, 0, result);
    ASSERT_TRUE(record != nullptr) << "create apprunningrecord fail!";

    // LaunchApplication
    sptr<MockApplication> mockApplication(new MockApplication());
    std::string testPoint = "ApplicationStatusChange_006";
    CheckLaunchApplication(mockApplication, index, record, testPoint);
    int startCount = 10000;
    for (int i = 0; i < startCount; i++) {
        EXPECT_CALL(*mockApplication, ScheduleForegroundApplication())
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
        // set foreground and update foreground state
        record->ScheduleForegroundRunning();
        mockApplication->Wait();
        record->SetState(ApplicationState::APP_STATE_FOREGROUND);
        auto stateFromRec = record->GetState();
        EXPECT_EQ(stateFromRec, ApplicationState::APP_STATE_FOREGROUND);

        EXPECT_CALL(*mockApplication, ScheduleBackgroundApplication())
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
        // set background and update background state
        record->ScheduleBackgroundRunning();
        mockApplication->Wait();
        record->SetState(ApplicationState::APP_STATE_BACKGROUND);
        stateFromRec = record->GetState();
        EXPECT_EQ(stateFromRec, ApplicationState::APP_STATE_BACKGROUND);
    }
    int32_t id = record->GetRecordId();
    service_->ApplicationTerminated(id);
    auto stateFromRec = record->GetState();
    EXPECT_EQ(stateFromRec, ApplicationState::APP_STATE_TERMINATED);
}

}  // namespace AppExecFwk
}  // namespace OHOS