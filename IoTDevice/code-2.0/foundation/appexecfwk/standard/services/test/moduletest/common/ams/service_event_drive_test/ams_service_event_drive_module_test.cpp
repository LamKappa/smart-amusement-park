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
#include "semaphore_ex.h"
#include "app_log_wrapper.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using OHOS::Semaphore;

namespace {

const uint32_t CYCLE_NUMBER = 10000;

}

enum class AmsInnerState {
    STATE_NO_OPERATION,
    STATE_ABILITY_LOADED,
    STATE_APPLICATION_FOREGROUNDED,
    STATE_APPLICATION_BACKGROUNDED,
    STATE_APPLICATION_TERMINATED,
    STATE_APPLICATION_BACK_TO_FORE,
    STATE_APPLICATION_FORE_TO_BACK,
    STATE_APPLICATION_BACK_TO_TER,
    STATE_APPLICATION_FORE_TO_TER,
    STATE_APPLICATION_TER_TO_FORE,
    STATE_APPLICATION_TER_TO_BACK,
};

class MockedAppMgrServiceInner : public AppMgrServiceInner {
public:
    MockedAppMgrServiceInner() : lock_(0)
    {}
    ~MockedAppMgrServiceInner()
    {}

    void ApplicationForegrounded([[maybe_unused]] const int32_t recordId) override
    {
        if (lastState_ == AmsInnerState::STATE_APPLICATION_BACKGROUNDED ||
            lastState_ == AmsInnerState::STATE_APPLICATION_FORE_TO_BACK ||
            lastState_ == AmsInnerState::STATE_APPLICATION_TER_TO_BACK) {
            state_ = AmsInnerState::STATE_APPLICATION_BACK_TO_FORE;
        } else if (lastState_ == AmsInnerState::STATE_APPLICATION_TERMINATED ||
                   lastState_ == AmsInnerState::STATE_APPLICATION_BACK_TO_TER ||
                   lastState_ == AmsInnerState::STATE_APPLICATION_FORE_TO_TER) {
            state_ = AmsInnerState::STATE_APPLICATION_TER_TO_FORE;
        } else {
            state_ = AmsInnerState::STATE_APPLICATION_FOREGROUNDED;
        }

        lastState_ = AmsInnerState::STATE_APPLICATION_FOREGROUNDED;
        Post();
    }

    void ApplicationBackgrounded([[maybe_unused]] const int32_t recordId) override
    {
        if (lastState_ == AmsInnerState::STATE_APPLICATION_FOREGROUNDED ||
            lastState_ == AmsInnerState::STATE_APPLICATION_BACK_TO_FORE ||
            lastState_ == AmsInnerState::STATE_APPLICATION_TER_TO_FORE) {
            state_ = AmsInnerState::STATE_APPLICATION_FORE_TO_BACK;
        } else if (lastState_ == AmsInnerState::STATE_APPLICATION_TERMINATED ||
                   lastState_ == AmsInnerState::STATE_APPLICATION_BACK_TO_TER ||
                   lastState_ == AmsInnerState::STATE_APPLICATION_FORE_TO_TER) {
            state_ = AmsInnerState::STATE_APPLICATION_TER_TO_BACK;
        } else {
            state_ = AmsInnerState::STATE_APPLICATION_BACKGROUNDED;
        }

        lastState_ = AmsInnerState::STATE_APPLICATION_BACKGROUNDED;
        Post();
    }

    void ApplicationTerminated([[maybe_unused]] const int32_t recordId) override
    {
        if (lastState_ == AmsInnerState::STATE_APPLICATION_FOREGROUNDED ||
            lastState_ == AmsInnerState::STATE_APPLICATION_BACK_TO_FORE ||
            lastState_ == AmsInnerState::STATE_APPLICATION_TER_TO_FORE) {
            state_ = AmsInnerState::STATE_APPLICATION_FORE_TO_TER;
        } else if (lastState_ == AmsInnerState::STATE_APPLICATION_BACKGROUNDED ||
                   lastState_ == AmsInnerState::STATE_APPLICATION_FORE_TO_BACK ||
                   lastState_ == AmsInnerState::STATE_APPLICATION_TER_TO_BACK) {
            state_ = AmsInnerState::STATE_APPLICATION_BACK_TO_TER;
        } else {
            state_ = AmsInnerState::STATE_APPLICATION_TERMINATED;
        }

        lastState_ = AmsInnerState::STATE_APPLICATION_TERMINATED;
        Post();
    }

    void Wait()
    {
        lock_.Wait();
    }

    void SetWaitNum(const int waitNum)
    {
        count_ = waitNum;
        currentCount_ = waitNum;
    }

    AmsInnerState GetInnerServiceState() const
    {
        return state_;
    }

    AmsInnerState SetInnerServiceState(const AmsInnerState &state)
    {
        state_ = state;
        return state_;
    }

    int32_t OpenAppSpawnConnection() override
    {
        return 0;
    }

private:
    Semaphore lock_ = {0};
    int32_t count_ = 1;
    int32_t currentCount_ = 1;
    AmsInnerState state_ = AmsInnerState::STATE_NO_OPERATION;
    AmsInnerState lastState_ = AmsInnerState::STATE_NO_OPERATION;

    void Post()
    {
        if (currentCount_ > 1) {
            currentCount_--;
        } else {
            lock_.Post();
            currentCount_ = count_;
        }
    }
};

class AmsServiceEventDriveModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<AppMgrService> appMgrService_;
};

void AmsServiceEventDriveModuleTest::SetUpTestCase()
{}

void AmsServiceEventDriveModuleTest::TearDownTestCase()
{}

void AmsServiceEventDriveModuleTest::SetUp()
{
    appMgrService_ = std::make_shared<AppMgrService>();
}

void AmsServiceEventDriveModuleTest::TearDown()
{
    appMgrService_.reset();
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application from background to foreground.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_001, TestSize.Level2)
{
    APP_LOGI("AmsServiceEventDrive_001 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 2 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationBackgrounded(recordId);
        appMgrService_->ApplicationForegrounded(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_BACK_TO_FORE, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_001 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application from foreground to background.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_002, TestSize.Level2)
{
    APP_LOGI("AmsServiceEventDrive_002 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 2 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationForegrounded(recordId);
        appMgrService_->ApplicationBackgrounded(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_FORE_TO_BACK, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_002 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application from background to terminate.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_003, TestSize.Level1)
{
    APP_LOGI("AmsServiceEventDrive_003 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 2 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationBackgrounded(recordId);
        appMgrService_->ApplicationTerminated(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_BACK_TO_TER, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_003 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application from foreground to terminated.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_004, TestSize.Level2)
{
    APP_LOGI("AmsServiceEventDrive_004 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 2 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationForegrounded(recordId);
        appMgrService_->ApplicationTerminated(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_FORE_TO_TER, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_004 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application from terminate to foreground.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_005, TestSize.Level1)
{
    APP_LOGI("AmsServiceEventDrive_005 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 2 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationTerminated(recordId);
        appMgrService_->ApplicationForegrounded(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_TER_TO_FORE, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_005 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application from terminate to background.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_006, TestSize.Level2)
{
    APP_LOGI("AmsServiceEventDrive_006 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 2 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationTerminated(recordId);
        appMgrService_->ApplicationBackgrounded(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_TER_TO_BACK, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_006 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application among terminate, background and foreground.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_007, TestSize.Level1)
{
    APP_LOGI("AmsServiceEventDrive_007 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 3 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationTerminated(recordId);
        appMgrService_->ApplicationBackgrounded(recordId);
        appMgrService_->ApplicationForegrounded(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_BACK_TO_FORE, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_007 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application among terminate, background and foreground.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_008, TestSize.Level1)
{
    APP_LOGI("AmsServiceEventDrive_008 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 3 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationTerminated(recordId);
        appMgrService_->ApplicationForegrounded(recordId);
        appMgrService_->ApplicationBackgrounded(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_FORE_TO_BACK, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_008 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application among terminate, background and foreground.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_009, TestSize.Level1)
{
    APP_LOGI("AmsServiceEventDrive_009 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 3 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationForegrounded(recordId);
        appMgrService_->ApplicationTerminated(recordId);
        appMgrService_->ApplicationBackgrounded(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_TER_TO_BACK, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_009 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application among terminate, background and foreground.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_010, TestSize.Level1)
{
    APP_LOGI("AmsServiceEventDrive_010 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 3 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationForegrounded(recordId);
        appMgrService_->ApplicationBackgrounded(recordId);
        appMgrService_->ApplicationTerminated(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_BACK_TO_TER, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_010 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application among terminate, background and foreground.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_011, TestSize.Level1)
{
    APP_LOGI("AmsServiceEventDrive_011 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 3 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationBackgrounded(recordId);
        appMgrService_->ApplicationForegrounded(recordId);
        appMgrService_->ApplicationTerminated(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_FORE_TO_TER, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_011 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application among terminate, background and foreground.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_012, TestSize.Level1)
{
    APP_LOGI("AmsServiceEventDrive_012 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 3 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationBackgrounded(recordId);
        appMgrService_->ApplicationTerminated(recordId);
        appMgrService_->ApplicationForegrounded(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_TER_TO_FORE, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_012 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application from terminate to terminate.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_013, TestSize.Level1)
{
    APP_LOGI("AmsServiceEventDrive_013 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 2 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationTerminated(recordId);
        appMgrService_->ApplicationTerminated(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_TERMINATED, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_013 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application from backgrounded to backgrounded.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_014, TestSize.Level1)
{
    APP_LOGI("AmsServiceEventDrive_014 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 2 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationBackgrounded(recordId);
        appMgrService_->ApplicationBackgrounded(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_BACKGROUNDED, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_014 end");
}

/*
 * Feature: AppMgrService
 * Function: EventDrive
 * SubFunction: NA
 * FunctionPoints: AppMgrService event drive program model
 * EnvConditions: NA
 * CaseDescription: Verify if post application from terminate to terminate.
 */
HWTEST_F(AmsServiceEventDriveModuleTest, AmsServiceEventDrive_015, TestSize.Level1)
{
    APP_LOGI("AmsServiceEventDrive_015 start");
    std::shared_ptr<MockedAppMgrServiceInner> innerService = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService_->SetInnerService(innerService);
    appMgrService_->OnStart();
    int32_t recordId = 0;
    int32_t waitCount = 2 * CYCLE_NUMBER;
    innerService->SetWaitNum(waitCount);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        appMgrService_->ApplicationForegrounded(recordId);
        appMgrService_->ApplicationForegrounded(recordId);
    }
    innerService->Wait();
    EXPECT_EQ(AmsInnerState::STATE_APPLICATION_FOREGROUNDED, innerService->GetInnerServiceState());
    APP_LOGI("AmsServiceEventDrive_015 end");
}