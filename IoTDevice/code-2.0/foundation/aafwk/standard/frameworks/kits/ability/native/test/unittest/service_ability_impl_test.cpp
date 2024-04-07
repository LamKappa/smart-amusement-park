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
#include "app_log_wrapper.h"
#include "context_deal.h"
#include "mock_service_ability.h"
#include "mock_ability_token.h"
#include "service_ability_impl.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using namespace OHOS::AAFwk;

class ServiceAbilityImplTest : public testing::Test {
public:
    ServiceAbilityImplTest() : serviceAbilityImpl_(nullptr)
    {}
    ~ServiceAbilityImplTest()
    {
        serviceAbilityImpl_ = nullptr;
    }
    std::shared_ptr<ServiceAbilityImpl> serviceAbilityImpl_ = nullptr;

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ServiceAbilityImplTest::SetUpTestCase(void)
{}

void ServiceAbilityImplTest::TearDownTestCase(void)
{}

void ServiceAbilityImplTest::SetUp(void)
{
    serviceAbilityImpl_ = std::make_shared<ServiceAbilityImpl>();
}

void ServiceAbilityImplTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0100
 * @tc.name: HandleAbilityTransaction
 * @tc.desc: Test whether handleabilitytransaction is called normally,
 *           and verify that getcurrentstate returns ABILITY_STATE_INITIAL.
 */
HWTEST_F(ServiceAbilityImplTest, AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0100 start";

    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "MockServiceAbility";
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);

    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);

    std::shared_ptr<Ability> ability = std::make_shared<MockServiceAbility>();
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetAbilityInfo(abilityInfo);
    ability->AttachBaseContext(deal);

    serviceAbilityImpl_->Init(application, record, ability, handler, token, deal);
    Want want;
    serviceAbilityImpl_->ConnectAbility(want);

    AAFwk::LifeCycleStateInfo state;
    state.state = ABILITY_STATE_INITIAL;
    GTEST_LOG_(INFO) << "AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_001 midle";
    serviceAbilityImpl_->HandleAbilityTransaction(want, state);
    GTEST_LOG_(INFO) << "AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_001 midle2";
    EXPECT_EQ(ABILITY_STATE_INITIAL, serviceAbilityImpl_->GetCurrentState());

    GTEST_LOG_(INFO) << "AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0100 end";
}

/**
 * @tc.number: AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0200
 * @tc.name: HandleAbilityTransaction
 * @tc.desc: Test whether handleabilitytransaction is called normally,
 *           and verify that getcurrentstate returns ABILITY_STATE_INACTIVE.
 */
HWTEST_F(ServiceAbilityImplTest, AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0200 start";

    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "MockServiceAbility";
    abilityInfo->type = AppExecFwk::AbilityType::SERVICE;
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);

    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);

    std::shared_ptr<Ability> ability = std::make_shared<MockServiceAbility>();
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetAbilityInfo(abilityInfo);
    ability->AttachBaseContext(deal);

    serviceAbilityImpl_->Init(application, record, ability, handler, token, deal);

    Want want;
    AAFwk::LifeCycleStateInfo state;
    state.state = ABILITY_STATE_INACTIVE;
    serviceAbilityImpl_->HandleAbilityTransaction(want, state);
    EXPECT_EQ(ABILITY_STATE_INACTIVE, serviceAbilityImpl_->GetCurrentState());

    GTEST_LOG_(INFO) << "AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0200 end";
}

/**
 * @tc.number: AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0300
 * @tc.name: HandleAbilityTransaction
 * @tc.desc: Test whether handleabilitytransaction is called normally,
 *           and verify that getcurrentstate returns ABILITY_STATE_INITIAL.
 */
HWTEST_F(ServiceAbilityImplTest, AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0300 start";

    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "MockServiceAbility";
    sptr<IRemoteObject> token = nullptr;
    std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);

    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);

    std::shared_ptr<Ability> ability = std::make_shared<MockServiceAbility>();
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetAbilityInfo(abilityInfo);
    ability->AttachBaseContext(deal);

    serviceAbilityImpl_->Init(application, record, ability, handler, token, deal);

    Want want;
    AAFwk::LifeCycleStateInfo state;
    state.state = ABILITY_STATE_INACTIVE;
    serviceAbilityImpl_->HandleAbilityTransaction(want, state);
    EXPECT_EQ(ABILITY_STATE_INITIAL, serviceAbilityImpl_->GetCurrentState());

    GTEST_LOG_(INFO) << "AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0300 end";
}

/**
 * @tc.number: AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0400
 * @tc.name: HandleAbilityTransaction
 * @tc.desc: Test whether handleabilitytransaction is called normally,
 *           and verify that getcurrentstate returns ABILITY_STATE_INACTIVE.
 */
HWTEST_F(ServiceAbilityImplTest, AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0400 start";

    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "MockServiceAbility";
    abilityInfo->type = AppExecFwk::AbilityType::SERVICE;
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);

    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);

    std::shared_ptr<Ability> ability = std::make_shared<MockServiceAbility>();
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetAbilityInfo(abilityInfo);
    ability->AttachBaseContext(deal);

    serviceAbilityImpl_->Init(application, record, ability, handler, token, deal);

    Want want;
    AAFwk::LifeCycleStateInfo state;
    state.state = ABILITY_STATE_INACTIVE;
    serviceAbilityImpl_->HandleAbilityTransaction(want, state);
    state.state = ABILITY_STATE_INITIAL;
    serviceAbilityImpl_->HandleAbilityTransaction(want, state);
    EXPECT_EQ(ABILITY_STATE_INACTIVE, serviceAbilityImpl_->GetCurrentState());

    GTEST_LOG_(INFO) << "AaFwk_ServiceAbilityImpl_HandleAbilityTransaction_0400 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS