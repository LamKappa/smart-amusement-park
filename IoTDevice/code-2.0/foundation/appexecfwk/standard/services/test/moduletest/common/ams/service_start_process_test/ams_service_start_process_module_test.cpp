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

namespace {

const int CYCLE_NUMBER = 10000;

}  // namespace

class AppMgrServiceInnerMock : public AppMgrServiceInner {
public:
    int32_t OpenAppSpawnConnection() override
    {
        return 0;
    }
};

class AmsServiceStartModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AmsServiceStartModuleTest::SetUpTestCase()
{}

void AmsServiceStartModuleTest::TearDownTestCase()
{}

void AmsServiceStartModuleTest::SetUp()
{}

void AmsServiceStartModuleTest::TearDown()
{}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: AppMgrService startup
 * EnvConditions: NA
 * CaseDescription: Verify if AppMgrService startup 10000 times.
 */
HWTEST_F(AmsServiceStartModuleTest, AmsStartupMoretimes_001, TestSize.Level1)
{
    APP_LOGI("AmsStartupMoretimes_001 start");
    std::shared_ptr<AppMgrService> appMgrService = std::make_shared<AppMgrService>();
    ASSERT_TRUE(appMgrService.get() != nullptr);
    std::shared_ptr<AppMgrServiceInnerMock> innerService = std::make_shared<AppMgrServiceInnerMock>();
    appMgrService->SetInnerService(innerService);
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);

    for (int i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService->OnStart();
        EXPECT_EQ(ServiceRunningState::STATE_RUNNING, appMgrService->QueryServiceState().serviceRunningState);
    }

    appMgrService->OnStop();
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    APP_LOGI("AmsStartupMoretimes_001 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: AppMgrService startup
 * EnvConditions: NA
 * CaseDescription: Verify if AppMgrService stop 10000 times.
 */
HWTEST_F(AmsServiceStartModuleTest, AmsStartupMoretimes_002, TestSize.Level1)
{
    APP_LOGI("AmsStartupMoretimes_002 start");
    std::shared_ptr<AppMgrService> appMgrService = std::make_shared<AppMgrService>();
    ASSERT_TRUE(appMgrService.get() != nullptr);
    std::shared_ptr<AppMgrServiceInnerMock> innerService = std::make_shared<AppMgrServiceInnerMock>();
    appMgrService->SetInnerService(innerService);
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);

    for (int i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService->OnStop();
        EXPECT_EQ(ServiceRunningState::STATE_NOT_START, appMgrService->QueryServiceState().serviceRunningState);
    }

    appMgrService->OnStart();
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, appMgrService->QueryServiceState().serviceRunningState);
    APP_LOGI("AmsStartupMoretimes_002 end");
}
