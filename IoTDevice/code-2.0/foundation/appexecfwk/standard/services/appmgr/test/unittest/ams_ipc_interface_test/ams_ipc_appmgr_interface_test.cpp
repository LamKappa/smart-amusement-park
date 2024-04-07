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

#include "app_scheduler_proxy.h"
#include <unistd.h>
#include <gtest/gtest.h>
#include "errors.h"
#include "ipc_types.h"
#include "app_log_wrapper.h"
#include "app_mgr_proxy.h"
#include "app_record_id.h"
#include "mock_application.h"
#include "mock_app_mgr_service.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using OHOS::iface_cast;
using OHOS::sptr;
using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;
using testing::Return;

class AmsIpcAppMgrInterfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AmsIpcAppMgrInterfaceTest::SetUpTestCase()
{}

void AmsIpcAppMgrInterfaceTest::TearDownTestCase()
{}

void AmsIpcAppMgrInterfaceTest::SetUp()
{}

void AmsIpcAppMgrInterfaceTest::TearDown()
{}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: interface
 * CaseDescription: test interface of AttachApplication
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, Interface_001, TestSize.Level0)
{
    APP_LOGD("AppMgrIpcInterfaceTest_AppMgr_001 start");
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);
    sptr<MockApplication> app(new MockApplication());

    EXPECT_CALL(*mockAppMgr, AttachApplication(_))
        .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
    appMgrClient->AttachApplication(app);
    mockAppMgr->Wait();
    APP_LOGD("AppMgrIpcInterfaceTest_AppMgr_001 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: interface
 * CaseDescription: test interface of ApplicationForegrounded
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, Interface_002, TestSize.Level0)
{
    APP_LOGD("AppMgrIpcInterfaceTest_AppMgr_002 start");
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, ApplicationForegrounded(_))
        .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
    auto recordId = AppRecordId::Create();
    appMgrClient->ApplicationForegrounded(recordId);
    mockAppMgr->Wait();
    APP_LOGD("AppMgrIpcInterfaceTest_AppMgr_002 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: interface
 * CaseDescription: test interface of ApplicationBackgrounded
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, Interface_003, TestSize.Level0)
{
    APP_LOGD("AppMgrIpcInterfaceTest_AppMgr_003 start");
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, ApplicationBackgrounded(_))
        .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
    auto recordId = AppRecordId::Create();
    appMgrClient->ApplicationBackgrounded(recordId);
    mockAppMgr->Wait();
    APP_LOGD("AppMgrIpcInterfaceTest_AppMgr_003 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: interface
 * CaseDescription: test interface of ApplicationTerminated
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, Interface_004, TestSize.Level0)
{
    APP_LOGD("AppMgrIpcInterfaceTest_AppMgr_004 start");
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, ApplicationTerminated(_))
        .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
    auto recordId = AppRecordId::Create();
    appMgrClient->ApplicationTerminated(recordId);
    mockAppMgr->Wait();
    APP_LOGD("AppMgrIpcInterfaceTest_AppMgr_004 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: interface
 * CaseDescription: test interface of CheckPermission
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, Interface_005, TestSize.Level0)
{
    APP_LOGD("AppMgrIpcInterfaceTest_AppMgr_005 start");
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, CheckPermission(_, _)).Times(1).WillOnce(Return(OHOS::NO_ERROR));
    auto recordId = AppRecordId::Create();
    int ret = appMgrClient->CheckPermission(recordId, "write");
    EXPECT_EQ(OHOS::NO_ERROR, ret);

    EXPECT_CALL(*mockAppMgr, CheckPermission(_, _)).Times(1).WillOnce(Return(OHOS::NO_ERROR));
    ret = appMgrClient->CheckPermission(recordId, "read");
    EXPECT_EQ(OHOS::NO_ERROR, ret);

    EXPECT_CALL(*mockAppMgr, CheckPermission(_, _)).Times(1).WillOnce(Return(OHOS::ERR_INVALID_STATE));
    ret = appMgrClient->CheckPermission(recordId, "location");
    EXPECT_EQ(OHOS::ERR_INVALID_STATE, ret);

    APP_LOGD("AppMgrIpcInterfaceTest_AppMgr_005 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: interface
 * CaseDescription: test IPC can transact data
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, Interface_006, TestSize.Level0)
{
    APP_LOGD("AppMgrIpcInterfaceTest_AppMgr_006 start");
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, CheckPermission(_, _))
        .Times(1)
        .WillOnce(Invoke(mockAppMgr.GetRefPtr(), &MockAppMgrService::CheckPermissionImpl));
    auto recordId = AppRecordId::Create();
    int ret = appMgrClient->CheckPermission(recordId, "write");
    EXPECT_EQ(0, ret);
    EXPECT_EQ("write", mockAppMgr->GetData());
    APP_LOGD("AppMgrIpcInterfaceTest_AppMgr_006 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: KillApplication interface
 * CaseDescription: test IPC can transact data
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, ClearUpApplicationData_008, TestSize.Level0)
{
    APP_LOGD("ClearUpApplicationData_008 start");

    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, ClearUpApplicationData(_)).Times(1);

    appMgrClient->ClearUpApplicationData("PROCESS");

    APP_LOGD("ClearUpApplicationData_008 end");
}

/*
 * Feature: AMS
 * Function: IPC IsBackgroundRunningRestricted
 * SubFunction: appmgr interface
 * FunctionPoints: Check background operation
 * CaseDescription: test IPC can transact data
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, IsBackgroundRunningRestricted_009, TestSize.Level0)
{
    APP_LOGD("IsBackgroundRunningRestricted_009 start");

    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, IsBackgroundRunningRestricted(_)).Times(1).WillOnce(Return(OHOS::NO_ERROR));

    int32_t ret = appMgrClient->IsBackgroundRunningRestricted("PROCESS");

    EXPECT_EQ(ret, OHOS::NO_ERROR);
    // Returns 32 when the bundle name is empty
    EXPECT_CALL(*mockAppMgr, IsBackgroundRunningRestricted(_)).Times(1).WillOnce(Return(32));
    ret = appMgrClient->IsBackgroundRunningRestricted("");

    EXPECT_EQ(ret, 32);
    APP_LOGD("IsBackgroundRunningRestricted_009 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: KillApplication interface
 * CaseDescription: test IPC can transact data
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, GetAllRunningProcesses_010, TestSize.Level0)
{
    APP_LOGD("GetAllRunningProcesses_009 start");

    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, GetAllRunningProcesses(_)).Times(1).WillOnce(Return(OHOS::ERR_NULL_OBJECT));

    std::shared_ptr<RunningProcessInfo> runningProcessInfo;
    int32_t ret = appMgrClient->GetAllRunningProcesses(runningProcessInfo);
    EXPECT_EQ(ret, OHOS::ERR_NULL_OBJECT);

    EXPECT_CALL(*mockAppMgr, GetAllRunningProcesses(_)).Times(1).WillOnce(Return(OHOS::ERR_NONE));
    ret = appMgrClient->GetAllRunningProcesses(runningProcessInfo);
    EXPECT_EQ(ret, OHOS::ERR_NONE);

    APP_LOGD("GetAllRunningProcesses_009 end");
}