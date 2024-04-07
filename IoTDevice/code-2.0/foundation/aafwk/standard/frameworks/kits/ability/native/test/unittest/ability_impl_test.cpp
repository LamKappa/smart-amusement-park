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

#include "ability_impl.h"
#include "ability.h"
#include "ability_state.h"
#include "app_log_wrapper.h"
#include "context_deal.h"
#include "foundation/distributedschedule/dmsfwk/services/dtbschedmgr/include/uri.h"
#include "mock_ability_token.h"
#include "mock_page_ability.h"
#include "mock_ability_impl.h"
#include "mock_ability_lifecycle_callbacks.h"
#include "ohos_application.h"
#include "page_ability_impl.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

class AbilityImplTest : public testing::Test {
public:
    AbilityImplTest() : AbilityImpl_(nullptr), MocKPageAbility_(nullptr)
    {}
    ~AbilityImplTest()
    {}
    std::shared_ptr<AbilityImpl> AbilityImpl_;
    std::shared_ptr<MockPageAbility> MocKPageAbility_;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AbilityImplTest::SetUpTestCase(void)
{}

void AbilityImplTest::TearDownTestCase(void)
{}

void AbilityImplTest::SetUp(void)
{
    AbilityImpl_ = std::make_shared<AbilityImpl>();
    MocKPageAbility_ = std::make_shared<MockPageAbility>();
}

void AbilityImplTest::TearDown(void)
{
    // delete AbilityImpl_;
    // delete MocKPageAbility_;

    // AbilityImpl_ = nullptr;
    // MocKPageAbility_ = nullptr;
}

/*
 * Feature: AbilityImpl
 * Function: Init
 * SubFunction: NA
 * FunctionPoints: Init
 * EnvConditions: NA
 * CaseDescription: Validate when normally entering a string
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Init_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Init_001 start";

    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = std::make_shared<Ability>();
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);
            EXPECT_EQ(mockAbilityimpl->GetToken(), record->GetToken());
            EXPECT_EQ(mockAbilityimpl->GetAbility(), ability);
            EXPECT_EQ(mockAbilityimpl->GetCurrentState(), AAFwk::ABILITY_STATE_INITIAL);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Init_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: Start
 * SubFunction: NA
 * FunctionPoints: Start
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::Start
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Start_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Start_001 start";

    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                contextDeal->SetAbilityInfo(abilityInfo);
                ability->AttachBaseContext(contextDeal);
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);
                Want want;
                mockAbilityimpl->ImplStart(want);
                EXPECT_EQ(MockPageAbility::Event::ON_START, pMocKPageAbility->state_);
                EXPECT_EQ(AAFwk::ABILITY_STATE_INACTIVE, mockAbilityimpl->GetCurrentState());
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Start_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: Start
 * SubFunction: NA
 * FunctionPoints: Start
 * EnvConditions: NA
 * CaseDescription: Test the abnormal behavior of the AbilityImpl::Start
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Start_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Start_002 start";

    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = nullptr;
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

            Want want;
            mockAbilityimpl->ImplStart(want);

            mockAbilityimpl->SetlifecycleState(AAFwk::ABILITY_STATE_SUSPENDED);
            EXPECT_EQ(AAFwk::ABILITY_STATE_SUSPENDED, mockAbilityimpl->GetCurrentState());
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Start_002 end";
}

/*
 * Feature: AbilityImpl
 * Function: Stop
 * SubFunction: NA
 * FunctionPoints: Stop
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::Stop
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Stop_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Stop_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                mockAbilityimpl->ImplStop();

                EXPECT_EQ(MockPageAbility::Event::ON_STOP, pMocKPageAbility->state_);
                EXPECT_EQ(AAFwk::ABILITY_STATE_INITIAL, mockAbilityimpl->GetCurrentState());
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Stop_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: Stop
 * SubFunction: NA
 * FunctionPoints: Stop
 * EnvConditions: NA
 * CaseDescription: Test the abnormal behavior of the AbilityImpl::Stop
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Stop_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Stop_002 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = nullptr;
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

            mockAbilityimpl->ImplStop();
            mockAbilityimpl->SetlifecycleState(AAFwk::ABILITY_STATE_SUSPENDED);

            EXPECT_EQ(AAFwk::ABILITY_STATE_SUSPENDED, mockAbilityimpl->GetCurrentState());
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Stop_002 end";
}

/*
 * Feature: AbilityImpl
 * Function: Active
 * SubFunction: NA
 * FunctionPoints: Active
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::Active
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Active_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Active_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                AbilityLifecycleExecutor AbilityLifecycleExecutor_;
                mockAbilityimpl->ImplActive();

                EXPECT_EQ(MockPageAbility::Event::ON_ACTIVE, pMocKPageAbility->state_);
                EXPECT_EQ(AAFwk::ABILITY_STATE_ACTIVE, mockAbilityimpl->GetCurrentState());
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Active_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: Active
 * SubFunction: NA
 * FunctionPoints: Active
 * EnvConditions: NA
 * CaseDescription: Test the abnormal behavior of the AbilityImpl::Active
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Active_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Active_002 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = nullptr;
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

            AbilityLifecycleExecutor AbilityLifecycleExecutor_;
            mockAbilityimpl->ImplActive();
            mockAbilityimpl->SetlifecycleState(AAFwk::ABILITY_STATE_SUSPENDED);

            EXPECT_EQ(AAFwk::ABILITY_STATE_SUSPENDED, mockAbilityimpl->GetCurrentState());
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Active_002 end";
}

/*
 * Feature: AbilityImpl
 * Function: Inactive
 * SubFunction: NA
 * FunctionPoints: Inactive
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::Inactive
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Inactive_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Inactive_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                AbilityLifecycleExecutor AbilityLifecycleExecutor_;
                mockAbilityimpl->ImplInactive();

                EXPECT_EQ(MockPageAbility::Event::ON_INACTIVE, pMocKPageAbility->state_);
                EXPECT_EQ(AAFwk::ABILITY_STATE_INACTIVE, mockAbilityimpl->GetCurrentState());
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Inactive_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: Inactive
 * SubFunction: NA
 * FunctionPoints: Inactive
 * EnvConditions: NA
 * CaseDescription: Test the abnormal behavior of the AbilityImpl::Inactive
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Inactive_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Inactive_002 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = nullptr;
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

            AbilityLifecycleExecutor AbilityLifecycleExecutor_;
            mockAbilityimpl->ImplInactive();
            mockAbilityimpl->SetlifecycleState(AAFwk::ABILITY_STATE_SUSPENDED);

            EXPECT_EQ(AAFwk::ABILITY_STATE_SUSPENDED, mockAbilityimpl->GetCurrentState());
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Inactive_002 end";
}

/*
 * Feature: AbilityImpl
 * Function: Foreground
 * SubFunction: NA
 * FunctionPoints: Foreground
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::Foreground
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Foreground_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Foreground_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = nullptr;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                Want want;
                AbilityLifecycleExecutor AbilityLifecycleExecutor_;
                mockAbilityimpl->ImplForeground(want);

                EXPECT_EQ(MockPageAbility::Event::ON_FOREGROUND, pMocKPageAbility->state_);
                EXPECT_EQ(AAFwk::ABILITY_STATE_INACTIVE, mockAbilityimpl->GetCurrentState());
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Foreground_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: Foreground
 * SubFunction: NA
 * FunctionPoints: Foreground
 * EnvConditions: NA
 * CaseDescription: Test the abnormal behavior of the AbilityImpl::Foreground
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Foreground_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Foreground_002 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = nullptr;
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

            Want want;
            AbilityLifecycleExecutor AbilityLifecycleExecutor_;
            mockAbilityimpl->ImplForeground(want);
            mockAbilityimpl->SetlifecycleState(AAFwk::ABILITY_STATE_SUSPENDED);

            EXPECT_EQ(AAFwk::ABILITY_STATE_SUSPENDED, mockAbilityimpl->GetCurrentState());
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Foreground_002 end";
}

/*
 * Feature: AbilityImpl
 * Function: Background
 * SubFunction: NA
 * FunctionPoints: Background
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::Background
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Background_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Background_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                AbilityLifecycleExecutor AbilityLifecycleExecutor_;
                mockAbilityimpl->ImplBackground();

                EXPECT_EQ(MockPageAbility::Event::ON_BACKGROUND, pMocKPageAbility->state_);
                EXPECT_EQ(AAFwk::ABILITY_STATE_BACKGROUND, mockAbilityimpl->GetCurrentState());
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Background_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: Background
 * SubFunction: NA
 * FunctionPoints: Background
 * EnvConditions: NA
 * CaseDescription: Test the abnormal behavior of the AbilityImpl::Background
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Background_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Background_002 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = nullptr;
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

            mockAbilityimpl->ImplBackground();
            mockAbilityimpl->SetlifecycleState(AAFwk::ABILITY_STATE_SUSPENDED);

            EXPECT_EQ(AAFwk::ABILITY_STATE_SUSPENDED, mockAbilityimpl->GetCurrentState());
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Background_002 end";
}

/*
 * Feature: AbilityImpl
 * Function: DisoatcgSaveAbilityState
 * SubFunction: NA
 * FunctionPoints: DisoatcgSaveAbilityState
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::DisoatcgSaveAbilityState
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_DispatchSaveAbilityState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DispatchSaveAbilityState_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = std::make_shared<Ability>();
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

            PacMap outState;
            AbilityLifecycleExecutor AbilityLifecycleExecutor_;
            mockAbilityimpl->DispatchSaveAbilityState(outState);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DispatchSaveAbilityState_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: DisoatcgSaveAbilityState
 * SubFunction: NA
 * FunctionPoints: DisoatcgSaveAbilityState
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::DisoatcgSaveAbilityState
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_DispatchSaveAbilityState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DispatchSaveAbilityState_002 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = nullptr;
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

            PacMap outState;
            mockAbilityimpl->DispatchSaveAbilityState(outState);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DispatchSaveAbilityState_002 end";
}

/*
 * Feature: AbilityImpl
 * Function: DispatchRestoreAbilityState
 * SubFunction: NA
 * FunctionPoints: DispatchRestoreAbilityState
 * EnvConditions: NA
 * CaseDescription: Test the abnormal behavior of the AbilityImpl::DispatchRestoreAbilityState
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_DispatchRestoreAbilityState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DispatchRestoreAbilityState_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = std::make_shared<Ability>();
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

            PacMap inState;

            mockAbilityimpl->DispatchRestoreAbilityState(inState);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DispatchRestoreAbilityState_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: DispatchRestoreAbilityState
 * SubFunction: NA
 * FunctionPoints: DispatchRestoreAbilityState
 * EnvConditions: NA
 * CaseDescription: Test the abnormal behavior of the AbilityImpl::DispatchRestoreAbilityState
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_DispatchRestoreAbilityState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DispatchRestoreAbilityState_002 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = nullptr;
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

            PacMap inState;
            mockAbilityimpl->DispatchRestoreAbilityState(inState);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DispatchRestoreAbilityState_002 end";
}

/*
 * Feature: AbilityImpl
 * Function: ConnectAbility
 * SubFunction: NA
 * FunctionPoints: ConnectAbility
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::ConnectAbility
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_ConnectAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_ConnectAbility_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                Want want;

                AbilityLifecycleExecutor AbilityLifecycleExecutor_;
                mockAbilityimpl->ConnectAbility(want);

                EXPECT_EQ(MockPageAbility::Event::ON_ACTIVE, pMocKPageAbility->state_);
                EXPECT_EQ(AAFwk::ABILITY_STATE_ACTIVE, mockAbilityimpl->GetCurrentState());
                EXPECT_EQ(nullptr, mockAbilityimpl->ConnectAbility(want));
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_ConnectAbility_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: ConnectAbility
 * SubFunction: NA
 * FunctionPoints: ConnectAbility
 * EnvConditions: NA
 * CaseDescription: Test the abnormal behavior of the AbilityImpl::ConnectAbility
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_ConnectAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_ConnectAbility_002 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = nullptr;
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

            Want want;

            AbilityLifecycleExecutor AbilityLifecycleExecutor_;
            mockAbilityimpl->ConnectAbility(want);
            mockAbilityimpl->SetlifecycleState(AAFwk::ABILITY_STATE_SUSPENDED);

            EXPECT_EQ(AAFwk::ABILITY_STATE_SUSPENDED, mockAbilityimpl->GetCurrentState());
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_ConnectAbility_002 end";
}

/*
 * Feature: AbilityImpl
 * Function: CommandAbility
 * SubFunction: NA
 * FunctionPoints: CommandAbility
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::CommandAbility
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_CommandAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_CommandAbility_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                Want want;
                bool restart = true;
                int startId = 1;
                AbilityLifecycleExecutor AbilityLifecycleExecutor_;
                mockAbilityimpl->CommandAbility(want, restart, startId);

                EXPECT_EQ(MockPageAbility::Event::ON_ACTIVE, pMocKPageAbility->state_);
                EXPECT_EQ(AAFwk::ABILITY_STATE_ACTIVE, mockAbilityimpl->GetCurrentState());
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_CommandAbility_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: CommandAbility
 * SubFunction: NA
 * FunctionPoints: CommandAbility
 * EnvConditions: NA
 * CaseDescription: Test the abnormal behavior of the AbilityImpl::CommandAbility
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_CommandAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_CommandAbility_002 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability = nullptr;
            std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
            AbilityImpl_->Init(application, record, ability, handler, token, contextDeal);

            Want want;

            bool restart = true;
            int startId = 1;

            AbilityLifecycleExecutor AbilityLifecycleExecutor_;
            AbilityImpl_->CommandAbility(want, restart, startId);
            mockAbilityimpl->SetlifecycleState(AAFwk::ABILITY_STATE_SUSPENDED);

            EXPECT_EQ(AAFwk::ABILITY_STATE_SUSPENDED, mockAbilityimpl->MockGetCurrentState());
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_CommandAbility_002 end";
}

/*
 * Feature: AbilityImpl
 * Function: GetCUrrentState
 * SubFunction: NA
 * FunctionPoints: GetCUrrentState
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::GetCUrrentState
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_GetCurrentState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_GetCurrentState_001 start";

    AbilityImpl abilityimpl;

    EXPECT_EQ(AAFwk::ABILITY_STATE_INITIAL, abilityimpl.GetCurrentState());

    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_GetCurrentState_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: DoKeyDown
 * SubFunction: NA
 * FunctionPoints: DoKeyDown
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::DoKeyDown
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_DoKeyDown_001, TestSize.Level1)
{

    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DoKeyDown_001 start";

    int keyCode = 0;
    KeyEvent keyEvent;

    EXPECT_EQ(false, AbilityImpl_->DoKeyDown(keyCode, keyEvent));

    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DoKeyDown_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: DoKeyUp
 * SubFunction: NA
 * FunctionPoints: DoKeyUp
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::DoKeyUp
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_DoKeyUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DoKeyUp_001 start";
    int keyCode = 0;
    KeyEvent keyEvent;

    EXPECT_EQ(false, AbilityImpl_->DoKeyUp(keyCode, keyEvent));
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DoKeyUp_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: DoTouchEvvent
 * SubFunction: NA
 * FunctionPoints: DoTouchEvvent
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::DoTouchEvvent
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_DoTouchEvent_001, TestSize.Level1)
{

    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DoTouchEvent_001 start";
    TouchEvent touchEvent;

    EXPECT_EQ(false, AbilityImpl_->DoTouchEvent(touchEvent));
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_DoTouchEvent_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: SendResult
 * SubFunction: NA
 * FunctionPoints: SendResult
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::SendResult
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_SendResult_001, TestSize.Level1)
{

    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_SendResult_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);

                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                int requestCode = 0;
                int resultCode = 0;
                Want resultData;

                mockAbilityimpl->SendResult(requestCode, resultCode, resultData);
                EXPECT_EQ(MockPageAbility::Event::ON_ACTIVE, pMocKPageAbility->state_);
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_SendResult_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: NewWant
 * SubFunction: NA
 * FunctionPoints: NewWant
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::NewWant
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_NewWant_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_NewWant_001 start";

    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);

                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                Want want;
                mockAbilityimpl->NewWant(want);
                EXPECT_EQ(1, pMocKPageAbility->onNewWantCalled_);
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_NewWant_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: GetFileTypes
 * SubFunction: NA
 * FunctionPoints: GetFileTypes
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::GetFileTypes
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_GetFileTypes_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_GetFileTypes_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                Uri uri("nullptr");

                std::string mimeTypeFilter("string1");

                std::vector<std::string> result = mockAbilityimpl->GetFileTypes(uri, mimeTypeFilter);
                int count = result.size();
                EXPECT_EQ(count, 0);
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_GetFileTypes_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: OpenFile
 * SubFunction: NA
 * FunctionPoints: OpenFile
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::OpenFile
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_OpenFile_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_OpenFile_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                Uri uri("\nullptr");
                std::string mode;
                int index = mockAbilityimpl->OpenFile(uri, mode);

                EXPECT_EQ(-1, index);
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_OpenFile_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: Insert
 * SubFunction: NA
 * FunctionPoints: Insert
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::Insert
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Insert_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Insert_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                Uri uri("\nullptr");

                ValuesBucket numerical;
                int index = mockAbilityimpl->Insert(uri, numerical);

                EXPECT_EQ(-1, index);
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Insert_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: Update
 * SubFunction: NA
 * FunctionPoints: Update
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::Update
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Update_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Update_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                Uri uri("\nullptr");

                ValuesBucket numerical;
                DataAbilityPredicates predicates;
                int index = mockAbilityimpl->Update(uri, numerical, predicates);

                EXPECT_EQ(-1, index);
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Update_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: Delate
 * SubFunction: NA
 * FunctionPoints: Delate
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::Delate
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Delete_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Delete_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                Uri uri("\nullptr");

                DataAbilityPredicates predicates;
                int index = mockAbilityimpl->Delete(uri, predicates);

                EXPECT_EQ(-1, index);
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Delete_001 end";
}

/*
 * Feature: AbilityImpl
 * Function: Query
 * SubFunction: NA
 * FunctionPoints: Query
 * EnvConditions: NA
 * CaseDescription: Test the normal behavior of the AbilityImpl::Query
 */
HWTEST_F(AbilityImplTest, AaFwk_AbilityImpl_Query_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Query_001 start";
    std::shared_ptr<MockAbilityimpl> mockAbilityimpl = std::make_shared<MockAbilityimpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "pageAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    EXPECT_NE(token, nullptr);
    if (token != nullptr) {
        std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
        std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
        sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
        EXPECT_NE(abilityThread, nullptr);
        if (abilityThread != nullptr) {
            std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
            std::shared_ptr<Ability> ability;
            MockPageAbility *pMocKPageAbility = new (std::nothrow) MockPageAbility();
            EXPECT_NE(pMocKPageAbility, nullptr);
            if (pMocKPageAbility != nullptr) {
                ability.reset(pMocKPageAbility);
                std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
                mockAbilityimpl->Init(application, record, ability, handler, token, contextDeal);

                Uri uri("\nullptr");
                std::vector<std::string> columns;
                columns.push_back("string1");
                columns.push_back("string2");
                columns.push_back("string3");
                DataAbilityPredicates predicates;

                EXPECT_EQ(nullptr, mockAbilityimpl->Query(uri, columns, predicates));
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityImpl_Query_001 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS