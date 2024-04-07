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
#include "ams_mgr_scheduler.h"
#undef private

#include "app_log_wrapper.h"
#include "mock_app_mgr_service_inner.h"
#include "mock_ability_token.h"
#include "app_state_callback_host.h"

using namespace testing;
using namespace testing::ext;
using testing::_;
using testing::Return;

namespace OHOS {
namespace AppExecFwk {

class AmsMgrSchedulerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

public:
protected:
    static const std::string GetTestAppName()
    {
        return "test_app_name";
    }
    static const std::string GetTestAbilityName()
    {
        return "test_ability_name";
    }

    std::shared_ptr<MockAppMgrServiceInner> GetMockAppMgrServiceInner();
    std::shared_ptr<AMSEventHandler> GetAmsEventHandler();

private:
    std::shared_ptr<MockAppMgrServiceInner> mockAppMgrServiceInner_;
    std::shared_ptr<AMSEventHandler> amsEventHandler_;
};

void AmsMgrSchedulerTest::SetUpTestCase()
{}

void AmsMgrSchedulerTest::TearDownTestCase()
{}

void AmsMgrSchedulerTest::SetUp()
{}

void AmsMgrSchedulerTest::TearDown()
{
    amsEventHandler_.reset();
    mockAppMgrServiceInner_.reset();
}

std::shared_ptr<MockAppMgrServiceInner> AmsMgrSchedulerTest::GetMockAppMgrServiceInner()
{
    if (!mockAppMgrServiceInner_) {
        mockAppMgrServiceInner_ = std::make_shared<MockAppMgrServiceInner>();
    }
    return mockAppMgrServiceInner_;
}

std::shared_ptr<AMSEventHandler> AmsMgrSchedulerTest::GetAmsEventHandler()
{
    if (!amsEventHandler_) {
        auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
        amsEventHandler_ =
            std::make_shared<AMSEventHandler>(EventRunner::Create("AmsMgrSchedulerTest"), mockAppMgrServiceInner);
    }
    return amsEventHandler_;
}

/*
 * Feature: AMS
 * Function: AmsMgrScheduler
 * SubFunction: LoadAbility
 * FunctionPoints: Act normal
 * EnvConditions: Mobile that can run ohos test framework.
 * CaseDescription: Verify the function LoadAbility can works.
 */
HWTEST_F(AmsMgrSchedulerTest, AmsMgrScheduler_001, TestSize.Level0)
{
    APP_LOGD("AmsMgrScheduler_001 start.");

    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);

    sptr<IRemoteObject> token = new MockAbilityToken();
    sptr<IRemoteObject> preToken = new MockAbilityToken();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    std::shared_ptr<ApplicationInfo> applicationInfo = std::make_shared<ApplicationInfo>();
    applicationInfo->name = GetTestAppName();

    EXPECT_CALL(*mockAppMgrServiceInner, LoadAbility(_, _, _, _))
        .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner.get(), &MockAppMgrServiceInner::Post));
    amsMgrScheduler->LoadAbility(token, preToken, abilityInfo, applicationInfo);
    mockAppMgrServiceInner->Wait();

    APP_LOGD("AmsMgrScheduler_001 end.");
}

/*
 * Feature: AMS
 * Function: AmsMgrScheduler
 * SubFunction: LoadAbility
 * FunctionPoints: Check params
 * EnvConditions: Mobile that can run ohos test framework.
 * CaseDescription: Verify the function LoadAbility can check appInfo and abilityInfo.
 */
HWTEST_F(AmsMgrSchedulerTest, AmsMgrScheduler_002, TestSize.Level0)
{
    APP_LOGD("AmsMgrScheduler_002 start.");

    auto mockAppMgrServiceInner = std::make_shared<MockAppMgrServiceInner>();
    auto eventRunner = EventRunner::Create("AmsMgrSchedulerTest");
    auto amsEventHandler = std::make_shared<AMSEventHandler>(eventRunner, mockAppMgrServiceInner);
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);

    sptr<IRemoteObject> token = new MockAbilityToken();
    sptr<IRemoteObject> preToken = new MockAbilityToken();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    std::shared_ptr<ApplicationInfo> applicationInfo = std::make_shared<ApplicationInfo>();
    applicationInfo->name = GetTestAppName();

    // check token parameter
    EXPECT_CALL(*mockAppMgrServiceInner, LoadAbility(_, _, _, _)).Times(0);
    amsMgrScheduler->LoadAbility(token, preToken, nullptr, applicationInfo);

    // check pretoken parameter
    EXPECT_CALL(*mockAppMgrServiceInner, LoadAbility(_, _, _, _)).Times(0);
    amsMgrScheduler->LoadAbility(token, preToken, abilityInfo, nullptr);

    APP_LOGD("AmsMgrScheduler_002 end.");
}

/*
 * Feature: AMS
 * Function: AmsMgrScheduler
 * SubFunction: UpdateAbilityState
 * FunctionPoints: Act normal
 * EnvConditions: Mobile that can run ohos test framework.
 * CaseDescription: Verify the function UpdateAbilityState can works.
 */
HWTEST_F(AmsMgrSchedulerTest, AmsMgrScheduler_003, TestSize.Level0)
{
    APP_LOGD("AmsMgrScheduler_003 start.");

    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);

    sptr<IRemoteObject> token = new MockAbilityToken();
    AbilityState abilityState = AbilityState::ABILITY_STATE_BEGIN;

    EXPECT_CALL(*mockAppMgrServiceInner, UpdateAbilityState(_, _))
        .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner.get(), &MockAppMgrServiceInner::Post));
    amsMgrScheduler->UpdateAbilityState(token, abilityState);
    mockAppMgrServiceInner->Wait();

    APP_LOGD("AmsMgrScheduler_003 end.");
}

/*
 * Feature: AMS
 * Function: AmsMgrScheduler
 * SubFunction: TerminateAbility
 * FunctionPoints: Act normal
 * EnvConditions: Mobile that can run ohos test framework.
 * CaseDescription: Verify the function TerminateAbility can works.
 */
HWTEST_F(AmsMgrSchedulerTest, AmsMgrScheduler_004, TestSize.Level0)
{
    APP_LOGD("AmsMgrScheduler_004 start.");

    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);
    sptr<IRemoteObject> token = new MockAbilityToken();

    EXPECT_CALL(*mockAppMgrServiceInner, TerminateAbility(_))
        .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner.get(), &MockAppMgrServiceInner::Post));
    amsMgrScheduler->TerminateAbility(token);
    mockAppMgrServiceInner->Wait();

    APP_LOGD("AmsMgrScheduler_004 end.");
}

/*
 * Feature: AMS
 * Function: AmsMgrScheduler
 * SubFunction: RegisterAppStateCallback
 * FunctionPoints: Act normal
 * EnvConditions: Mobile that can run ohos test framework.
 * CaseDescription: Verify the function RegisterAppStateCallback can works.
 */
HWTEST_F(AmsMgrSchedulerTest, AmsMgrScheduler_005, TestSize.Level0)
{
    APP_LOGD("AmsMgrScheduler_005 start.");

    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);

    sptr<AppStateCallbackHost> appStateCallbackHost = new AppStateCallbackHost();
    EXPECT_CALL(*mockAppMgrServiceInner, RegisterAppStateCallback(_))
        .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner.get(), &MockAppMgrServiceInner::Post));
    amsMgrScheduler->RegisterAppStateCallback(appStateCallbackHost);
    mockAppMgrServiceInner->Wait();

    APP_LOGD("AmsMgrScheduler_005 end.");
}

/*
 * Feature: AMS
 * Function: AmsMgrScheduler
 * SubFunction: Reset
 * FunctionPoints: Act normal
 * EnvConditions: Mobile that can run ohos test framework.
 * CaseDescription: Verify the function Reset can works.
 */
HWTEST_F(AmsMgrSchedulerTest, AmsMgrScheduler_006, TestSize.Level0)
{
    APP_LOGD("AmsMgrScheduler_006 start.");

    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);

    sptr<AppStateCallbackHost> appStateCallbackHost = new AppStateCallbackHost();
    EXPECT_CALL(*mockAppMgrServiceInner, StopAllProcess())
        .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner.get(), &MockAppMgrServiceInner::Post));
    amsMgrScheduler->Reset();
    mockAppMgrServiceInner->Wait();

    APP_LOGD("AmsMgrScheduler_006 end.");
}

/*
 * Feature: AMS
 * Function: AmsMgrScheduler
 * SubFunction: IsReady
 * FunctionPoints: Check Params
 * EnvConditions: Mobile that can run ohos test framework.
 * CaseDescription: Verify the function IsReady can check params.
 */
HWTEST_F(AmsMgrSchedulerTest, AmsMgrScheduler_007, TestSize.Level0)
{
    APP_LOGD("AmsMgrScheduler_007 start.");

    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();

    // act normal
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);
    EXPECT_EQ(true, amsMgrScheduler->IsReady());

    // check params AppMgrServiceInner
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler2 = std::make_unique<AmsMgrScheduler>(nullptr, amsEventHandler);
    EXPECT_EQ(false, amsMgrScheduler2->IsReady());

    // check params AMSEventHandler
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler3 =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, nullptr);
    EXPECT_EQ(false, amsMgrScheduler3->IsReady());

    APP_LOGD("AmsMgrScheduler_007 end.");
}

/*
 * Feature: AMS
 * Function: KillApplication
 * SubFunction: IsReady
 * FunctionPoints: Check Params
 * EnvConditions: Mobile that can run ohos test framework.
 * CaseDescription: Kill apps by name
 */
HWTEST_F(AmsMgrSchedulerTest, AmsMgrScheduler_008, TestSize.Level0)
{
    APP_LOGD("AmsMgrScheduler_008 start.");

    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();

    EXPECT_CALL(*mockAppMgrServiceInner, KillApplication(_)).Times(1).WillOnce(Return(ERR_OK));

    // check params AppMgrServiceInner
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler2 = std::make_unique<AmsMgrScheduler>(nullptr, amsEventHandler);
    EXPECT_EQ(false, amsMgrScheduler2->IsReady());

    EXPECT_EQ(ERR_INVALID_OPERATION, amsMgrScheduler2->KillApplication(GetTestAppName()));

    // check params AMSEventHandler
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler3 =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, nullptr);
    EXPECT_EQ(false, amsMgrScheduler3->IsReady());

    EXPECT_EQ(ERR_INVALID_OPERATION, amsMgrScheduler3->KillApplication(GetTestAppName()));

    // act normal
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler4 =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);
    EXPECT_EQ(true, amsMgrScheduler4->IsReady());

    EXPECT_EQ(ERR_OK, amsMgrScheduler4->KillApplication(GetTestAppName()));

    APP_LOGD("AmsMgrScheduler_008 end.");
}

/*
 * Feature: AMS
 * Function: AbilityBehaviorAnalysis
 * SubFunction: IsReady
 * FunctionPoints: Check Params
 * EnvConditions: Mobile that can run ohos test framework.
 * CaseDescription: Optimize based on visibility and perception
 */
HWTEST_F(AmsMgrSchedulerTest, AmsMgrScheduler_009, TestSize.Level0)
{
    APP_LOGD("AmsMgrScheduler_009 start.");

    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();
    auto amsEventHandler = GetAmsEventHandler();
    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler =
        std::make_unique<AmsMgrScheduler>(mockAppMgrServiceInner, amsEventHandler);
    EXPECT_EQ(true, amsMgrScheduler->IsReady());

    EXPECT_CALL(*mockAppMgrServiceInner, AbilityBehaviorAnalysis(_, _, _, _, _))
        .Times(1)
        .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner.get(), &MockAppMgrServiceInner::Post));

    sptr<IRemoteObject> token;
    sptr<IRemoteObject> preToken;
    int32_t visibility = 0;
    int32_t perceptibility = 0;
    int32_t connectionState = 0;

    amsMgrScheduler->AbilityBehaviorAnalysis(token, preToken, visibility, perceptibility, connectionState);

    mockAppMgrServiceInner->Wait();

    mockAppMgrServiceInner.reset();
    amsEventHandler.reset();

    APP_LOGD("AmsMgrScheduler_009 end.");
}

/*
 * Feature: AMS
 * Function: AbilityBehaviorAnalysis
 * SubFunction: IsReady
 * FunctionPoints: Check Params
 * EnvConditions: Mobile that can run ohos test framework.
 * CaseDescription: Optimize based on visibility and perception
 */
HWTEST_F(AmsMgrSchedulerTest, AmsMgrScheduler_010, TestSize.Level0)
{
    APP_LOGD("AmsMgrScheduler_010 start.");

    auto mockAppMgrServiceInner = GetMockAppMgrServiceInner();

    std::unique_ptr<AmsMgrScheduler> amsMgrScheduler = std::make_unique<AmsMgrScheduler>(nullptr, nullptr);
    EXPECT_EQ(false, amsMgrScheduler->IsReady());

    EXPECT_CALL(*mockAppMgrServiceInner, AbilityBehaviorAnalysis(_, _, _, _, _)).Times(0);

    sptr<IRemoteObject> token;
    sptr<IRemoteObject> preToken;
    int32_t visibility = 0;
    int32_t perceptibility = 0;
    int32_t connectionState = 0;

    amsMgrScheduler->AbilityBehaviorAnalysis(token, preToken, visibility, perceptibility, connectionState);

    APP_LOGD("AmsMgrScheduler_010 end.");
}

}  // namespace AppExecFwk
}  // namespace OHOS
