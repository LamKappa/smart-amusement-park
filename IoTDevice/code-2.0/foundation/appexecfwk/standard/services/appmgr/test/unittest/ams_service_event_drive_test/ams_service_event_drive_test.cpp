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

// redefine private and protected since testcase need to invoke and test private function
#define private public
#define protected public
#include "app_mgr_service.h"
#undef private
#undef protected
#include <gtest/gtest.h>
#include "app_log_wrapper.h"
#include "iremote_object.h"
#include "errors.h"
#include "mock_app_mgr_service_inner.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using OHOS::iface_cast;
using OHOS::IRemoteObject;
using OHOS::sptr;
using testing::_;
using testing::InvokeWithoutArgs;

class AmsServiceEventDriveTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    std::shared_ptr<MockAppMgrServiceInner> GetMockAppMgrServiceInner();
    std::shared_ptr<AMSEventHandler> GetAmsEventHandler();

protected:
    std::shared_ptr<AppMgrService> appMgrService_;
};

void AmsServiceEventDriveTest::SetUpTestCase()
{}

void AmsServiceEventDriveTest::TearDownTestCase()
{}

void AmsServiceEventDriveTest::SetUp()
{
    appMgrService_ = std::make_shared<AppMgrService>();
}

void AmsServiceEventDriveTest::TearDown()
{}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post AttachApplication task success
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_001, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_001 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    innerService->SetWaitCount(2);

    EXPECT_CALL(*innerService, AddAppDeathRecipient(_, _))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));
    EXPECT_CALL(*innerService, AttachApplication(_, _))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    sptr<IRemoteObject> client;
    appMgrService_->AttachApplication(client);
    innerService->Wait();

    APP_LOGI("ams_service_event_drive_test_001 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post ApplicationForegrounded task success
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_002, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_002 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    std::unique_ptr<AppMgrService> appMgrService = std::make_unique<AppMgrService>();
    appMgrService->SetInnerService(innerService);
    appMgrService->OnStart();

    EXPECT_CALL(*innerService, ApplicationForegrounded(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    int32_t recordId = 0;
    appMgrService->ApplicationForegrounded(recordId);
    innerService->Wait();

    APP_LOGI("ams_service_event_drive_test_002 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post ApplicationBackgrounded task success
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_003, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_003 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();

    EXPECT_CALL(*innerService, ApplicationBackgrounded(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    int32_t recordId = 0;
    appMgrService_->ApplicationBackgrounded(recordId);
    innerService->Wait();

    APP_LOGI("ams_service_event_drive_test_003 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post ApplicationTerminated task success
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_004, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_004 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();

    EXPECT_CALL(*innerService, ApplicationTerminated(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    int32_t recordId = 0;
    appMgrService_->ApplicationTerminated(recordId);
    innerService->Wait();

    APP_LOGI("ams_service_event_drive_test_004 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post AbilityCleaned task success
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_005, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_005 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();

    EXPECT_CALL(*innerService, AbilityTerminated(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    sptr<IRemoteObject> token;
    appMgrService_->AbilityCleaned(token);
    innerService->Wait();

    APP_LOGI("ams_service_event_drive_test_005 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post ClearUpApplicationData task success
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_006, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_006 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();

    EXPECT_CALL(*innerService, ClearUpApplicationData(_, _, _))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    std::string appName = "appName";
    appMgrService_->ClearUpApplicationData(appName);
    innerService->Wait();

    APP_LOGI("ams_service_event_drive_test_006 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post IsBackgroundRunningRestricted task success
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_007, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_007 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();

    EXPECT_CALL(*innerService, IsBackgroundRunningRestricted(_)).WillOnce(Return(0));

    std::string appName = "appName";
    EXPECT_EQ(0, appMgrService_->IsBackgroundRunningRestricted(appName));

    APP_LOGI("ams_service_event_drive_test_007 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post IsBackgroundRunningRestricted task success
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_008, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_008 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();

    EXPECT_CALL(*innerService, GetAllRunningProcesses(_)).WillOnce(Return(0));

    std::shared_ptr<RunningProcessInfo> runningProcessInfo = std::make_shared<RunningProcessInfo>();
    EXPECT_EQ(0, appMgrService_->GetAllRunningProcesses(runningProcessInfo));

    APP_LOGI("ams_service_event_drive_test_008 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AttachApplication act normal without initialize AppMgrService
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_009, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_009 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);

    EXPECT_CALL(*innerService, AttachApplication(_, _)).Times(0);

    sptr<IRemoteObject> client;
    appMgrService_->AttachApplication(client);

    APP_LOGI("ams_service_event_drive_test_009 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if ApplicationForegrounded act normal without initialize AppMgrService
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_010, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_010 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);

    EXPECT_CALL(*innerService, ApplicationForegrounded(_)).Times(0);

    int32_t recordId = 0;
    appMgrService_->ApplicationForegrounded(recordId);

    APP_LOGI("ams_service_event_drive_test_010 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if ApplicationBackgrounded act normal without initialize AppMgrService
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_011, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_011 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);

    EXPECT_CALL(*innerService, ApplicationBackgrounded(_)).Times(0);

    int32_t recordId = 0;
    appMgrService_->ApplicationBackgrounded(recordId);

    APP_LOGI("ams_service_event_drive_test_011 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if ApplicationTerminated act normal without initialize AppMgrService
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_012, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_012 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);

    EXPECT_CALL(*innerService, ApplicationTerminated(_)).Times(0);

    int32_t recordId = 0;
    appMgrService_->ApplicationTerminated(recordId);

    APP_LOGI("ams_service_event_drive_test_012 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AbilityCleaned act normal without initialize AppMgrService
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_013, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_013 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);

    EXPECT_CALL(*innerService, AbilityTerminated(_)).Times(0);

    sptr<IRemoteObject> token;
    appMgrService_->AbilityCleaned(token);

    APP_LOGI("ams_service_event_drive_test_013 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if ClearUpApplicationData act normal without initialize AppMgrService
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_014, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_014 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);

    EXPECT_CALL(*innerService, ClearUpApplicationData(_, _, _)).Times(0);

    std::string appName = "appName";
    appMgrService_->ClearUpApplicationData(appName);

    APP_LOGI("ams_service_event_drive_test_014 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if IsBackgroundRunningRestricted act normal after AppMgrService stopped
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_015, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_015 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);

    std::string appName = "appName";
    EXPECT_EQ(OHOS::ERR_INVALID_OPERATION, appMgrService_->IsBackgroundRunningRestricted(appName));

    APP_LOGI("ams_service_event_drive_test_015 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if GetAllRunningProcesses act normal after AppMgrService stopped
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_016, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_016 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);

    std::shared_ptr<RunningProcessInfo> runningProcessInfo = std::make_shared<RunningProcessInfo>();
    EXPECT_EQ(OHOS::ERR_INVALID_OPERATION, appMgrService_->GetAllRunningProcesses(runningProcessInfo));

    APP_LOGI("ams_service_event_drive_test_016 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AttachApplication act normal after AppMgrService stopped
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_017, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_017 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    appMgrService_->OnStop();

    EXPECT_CALL(*innerService, AttachApplication(_, _)).Times(0);

    sptr<IRemoteObject> client;
    appMgrService_->AttachApplication(client);

    APP_LOGI("ams_service_event_drive_test_017 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if ApplicationForegrounded act normal after AppMgrService stopped
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_018, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_018 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    appMgrService_->OnStop();

    EXPECT_CALL(*innerService, ApplicationForegrounded(_)).Times(0);

    int32_t recordId = 0;
    appMgrService_->ApplicationForegrounded(recordId);

    APP_LOGI("ams_service_event_drive_test_018 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if ApplicationBackgrounded act normal after AppMgrService stopped
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_019, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_019 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    appMgrService_->OnStop();

    EXPECT_CALL(*innerService, ApplicationBackgrounded(_)).Times(0);

    int32_t recordId = 0;
    appMgrService_->ApplicationBackgrounded(recordId);

    APP_LOGI("ams_service_event_drive_test_019 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if ApplicationTerminated act normal after AppMgrService stopped
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_020, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_020 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    appMgrService_->OnStop();

    EXPECT_CALL(*innerService, ApplicationTerminated(_)).Times(0);

    int32_t recordId = 0;
    appMgrService_->ApplicationTerminated(recordId);

    APP_LOGI("ams_service_event_drive_test_020 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AbilityCleaned act normal after AppMgrService stopped
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_021, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_021 start");
    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    appMgrService_->OnStop();

    EXPECT_CALL(*innerService, AbilityTerminated(_)).Times(0);

    sptr<IRemoteObject> token;
    appMgrService_->AbilityCleaned(token);

    APP_LOGI("ams_service_event_drive_test_021 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if ClearUpApplicationData act normal after AppMgrService stopped
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_022, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_022 start");
    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    appMgrService_->OnStop();

    EXPECT_CALL(*innerService, ClearUpApplicationData(_, _, _)).Times(0);

    std::string appName = "appName";
    appMgrService_->ClearUpApplicationData(appName);

    APP_LOGI("ams_service_event_drive_test_022 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if IsBackgroundRunningRestricted act normal after AppMgrService stopped
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_023, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_023 start");
    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    appMgrService_->OnStop();

    std::string appName = "appName";
    EXPECT_EQ(OHOS::ERR_INVALID_OPERATION, appMgrService_->IsBackgroundRunningRestricted(appName));

    APP_LOGI("ams_service_event_drive_test_023 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if GetAllRunningProcesses act normal after AppMgrService stopped
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_024, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_024 start");
    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    appMgrService_->OnStop();

    appMgrService_->SetInnerService(innerService);

    std::shared_ptr<RunningProcessInfo> runningProcessInfo = std::make_shared<RunningProcessInfo>();
    EXPECT_EQ(OHOS::ERR_INVALID_OPERATION, appMgrService_->GetAllRunningProcesses(runningProcessInfo));

    APP_LOGI("ams_service_event_drive_test_024 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post application from background to foreground task act normal
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_025, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_025 start");
    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t waitCount = 2;
    innerService->SetWaitCount(waitCount);

    EXPECT_CALL(*innerService, ApplicationBackgrounded(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));
    EXPECT_CALL(*innerService, ApplicationForegrounded(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    int32_t recordId = 0;
    appMgrService_->ApplicationBackgrounded(recordId);
    appMgrService_->ApplicationForegrounded(recordId);
    innerService->Wait();
    APP_LOGI("ams_service_event_drive_test_025 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post application from foreground to background task act normal
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_026, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_026 start");
    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t waitCount = 2;
    innerService->SetWaitCount(waitCount);

    EXPECT_CALL(*innerService, ApplicationForegrounded(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));
    EXPECT_CALL(*innerService, ApplicationBackgrounded(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    int32_t recordId = 0;
    appMgrService_->ApplicationForegrounded(recordId);
    appMgrService_->ApplicationBackgrounded(recordId);
    innerService->Wait();
    APP_LOGI("ams_service_event_drive_test_026 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post application from background to terminate task act normal
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_027, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_027 start");
    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t waitCount = 2;
    innerService->SetWaitCount(waitCount);

    EXPECT_CALL(*innerService, ApplicationBackgrounded(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));
    EXPECT_CALL(*innerService, ApplicationTerminated(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    int32_t recordId = 0;
    appMgrService_->ApplicationBackgrounded(recordId);
    appMgrService_->ApplicationTerminated(recordId);
    innerService->Wait();
    APP_LOGI("ams_service_event_drive_test_027 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post application from foreground to terminated task act normal
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_028, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_028 start");
    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t waitCount = 2;
    innerService->SetWaitCount(waitCount);

    EXPECT_CALL(*innerService, ApplicationForegrounded(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));
    EXPECT_CALL(*innerService, ApplicationTerminated(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    int32_t recordId = 0;
    appMgrService_->ApplicationForegrounded(recordId);
    appMgrService_->ApplicationTerminated(recordId);
    innerService->Wait();
    APP_LOGI("ams_service_event_drive_test_028 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post application from terminate to foreground task act normal
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_029, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_029 start");
    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t waitCount = 2;
    innerService->SetWaitCount(waitCount);

    EXPECT_CALL(*innerService, ApplicationTerminated(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));
    EXPECT_CALL(*innerService, ApplicationForegrounded(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    int32_t recordId = 0;
    appMgrService_->ApplicationTerminated(recordId);
    appMgrService_->ApplicationForegrounded(recordId);
    innerService->Wait();
    APP_LOGI("ams_service_event_drive_test_029 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework.
 * CaseDescription: Verify if post application from terminate to background task act normal
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_030, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_030 start");
    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t waitCount = 2;
    innerService->SetWaitCount(waitCount);

    EXPECT_CALL(*innerService, ApplicationTerminated(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));
    EXPECT_CALL(*innerService, ApplicationBackgrounded(_))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    int32_t recordId = 0;
    appMgrService_->ApplicationTerminated(recordId);
    appMgrService_->ApplicationBackgrounded(recordId);
    innerService->Wait();
    APP_LOGI("ams_service_event_drive_test_030 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if post AddAppDeathRecipient task success
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_034, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_034 start");
    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();

    EXPECT_CALL(*innerService, AddAppDeathRecipient(_, _))
        .WillOnce(InvokeWithoutArgs(innerService.get(), &MockAppMgrServiceInner::Post));

    pid_t pid = 1;
    appMgrService_->AddAppDeathRecipient(pid);
    innerService->Wait();
    APP_LOGI("ams_service_event_drive_test_034 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AddAppDeathRecipient act normal without initialize AppMgrService
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_035, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_035 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);

    EXPECT_CALL(*innerService, AddAppDeathRecipient(_, _)).Times(0);

    pid_t pid = 1;
    appMgrService_->AddAppDeathRecipient(pid);

    APP_LOGI("ams_service_event_drive_test_035 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AddAppDeathRecipient act normal after AppMgrService stopped
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_036, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_036 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    appMgrService_->OnStop();

    EXPECT_CALL(*innerService, AddAppDeathRecipient(_, _)).Times(0);

    pid_t pid = 1;
    appMgrService_->AddAppDeathRecipient(pid);

    APP_LOGI("ams_service_event_drive_test_036 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if QueryServiceState act normal
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_037, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_037 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();

    EXPECT_CALL(*innerService, QueryAppSpawnConnectionState())
        .WillRepeatedly(Return(SpawnConnectionState::STATE_CONNECTED));
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, appMgrService_->QueryServiceState().serviceRunningState);
    EXPECT_EQ(SpawnConnectionState::STATE_CONNECTED, appMgrService_->QueryServiceState().connectionState);

    APP_LOGI("ams_service_event_drive_test_037 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if QueryServiceState act normal without initialize AppMgrService
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_038, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_038 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);

    EXPECT_CALL(*innerService, QueryAppSpawnConnectionState()).Times(2);
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService_->QueryServiceState().serviceRunningState);
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService_->QueryServiceState().connectionState);

    APP_LOGI("ams_service_event_drive_test_038 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: EventDrive
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if QueryServiceState act normal after AppMgrService stopped
 */
HWTEST_F(AmsServiceEventDriveTest, EventDrive_039, TestSize.Level0)
{
    APP_LOGI("ams_service_event_drive_test_039 start");

    std::shared_ptr<MockAppMgrServiceInner> innerService = std::make_shared<MockAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    appMgrService_->OnStop();

    EXPECT_CALL(*innerService, QueryAppSpawnConnectionState()).Times(2);
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService_->QueryServiceState().serviceRunningState);
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService_->QueryServiceState().connectionState);

    APP_LOGI("ams_service_event_drive_test_039 end");
}
