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

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

class AppMgrServiceInnerMock : public AppMgrServiceInner {
public:
    virtual int32_t OpenAppSpawnConnection() override
    {
        return 0;
    }
};

class AmsServiceStartupTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AmsServiceStartupTest::SetUpTestCase()
{}

void AmsServiceStartupTest::TearDownTestCase()
{}

void AmsServiceStartupTest::SetUp()
{}

void AmsServiceStartupTest::TearDown()
{}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: StartUp
 * FunctionPoints: AppMgrService startup
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AppMgrService startup successfully
 */
HWTEST_F(AmsServiceStartupTest, Startup_001, TestSize.Level0)
{
    APP_LOGI("ams_service_startup_001 start");
    std::shared_ptr<AppMgrService> appMgrService = std::make_shared<AppMgrService>();
    std::shared_ptr<AppMgrServiceInnerMock> innerService = std::make_shared<AppMgrServiceInnerMock>();
    appMgrService->SetInnerService(innerService);
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStart();
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStop();
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    APP_LOGI("ams_service_startup_001 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: StartUp
 * FunctionPoints: AppMgrService startup again
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if start up repeatly system act normal
 */
HWTEST_F(AmsServiceStartupTest, Startup_002, TestSize.Level0)
{
    APP_LOGI("ams_service_startup_002 start");
    std::shared_ptr<AppMgrService> appMgrService = std::make_shared<AppMgrService>();
    std::shared_ptr<AppMgrServiceInnerMock> innerService = std::make_shared<AppMgrServiceInnerMock>();
    appMgrService->SetInnerService(innerService);
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStart();
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStart();
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStop();
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    APP_LOGI("ams_service_startup_002 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: StartUp
 * FunctionPoints: AppMgrService stop without start
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if stop without start system act normal
 */
HWTEST_F(AmsServiceStartupTest, Startup_003, TestSize.Level0)
{
    APP_LOGI("ams_service_startup_003 start");
    std::shared_ptr<AppMgrService> appMgrService = std::make_shared<AppMgrService>();
    std::shared_ptr<AppMgrServiceInnerMock> innerService = std::make_shared<AppMgrServiceInnerMock>();
    appMgrService->SetInnerService(innerService);
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStop();
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    APP_LOGI("ams_service_startup_003 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: StartUp
 * FunctionPoints: AppMgrService stop repeatly
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if stop repeatly system act normal
 */
HWTEST_F(AmsServiceStartupTest, Startup_004, TestSize.Level0)
{
    APP_LOGI("ams_service_startup_004 start");
    std::shared_ptr<AppMgrService> appMgrService = std::make_shared<AppMgrService>();
    std::shared_ptr<AppMgrServiceInnerMock> innerService = std::make_shared<AppMgrServiceInnerMock>();
    appMgrService->SetInnerService(innerService);
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStart();
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStop();
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStop();
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    APP_LOGI("ams_service_startup_004 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: StartUp
 * FunctionPoints: AppMgrService start and stop repeatly
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if start and stop repeatly system act normal
 */
HWTEST_F(AmsServiceStartupTest, Startup_005, TestSize.Level0)
{
    APP_LOGI("ams_service_startup_005 start");
    std::shared_ptr<AppMgrService> appMgrService = std::make_shared<AppMgrService>();
    std::shared_ptr<AppMgrServiceInnerMock> innerService = std::make_shared<AppMgrServiceInnerMock>();
    appMgrService->SetInnerService(innerService);
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStart();
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStop();
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStart();
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, appMgrService->QueryServiceState().serviceRunningState);
    appMgrService->OnStop();
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    APP_LOGI("ams_service_startup_005 end");
}