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
#include "ability_thread.h"
#include "ability_context.h"
#include "ability_manager_client.h"
#include "context_deal.h"
#include "mock_serviceability_manager_service.h"
#include "ohos_application.h"
#include "system_ability_definition.h"
#include "sys_mgr_client.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace OHOS;
using namespace AAFwk;

class AbilityContextTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static constexpr int TEST_WAIT_TIME = 500 * 1000;  // 500 ms
public:
    std::unique_ptr<AbilityContext> context_ = nullptr;
};

void AbilityContextTest::SetUpTestCase(void)
{
    OHOS::sptr<OHOS::IRemoteObject> abilityObject = new (std::nothrow) MockServiceAbilityManagerService();

    auto sysMgr = OHOS::DelayedSingleton<SysMrgClient>::GetInstance();
    if (sysMgr == NULL) {
        GTEST_LOG_(ERROR) << "fail to get ISystemAbilityManager";
        return;
    }

    sysMgr->RegisterSystemAbility(OHOS::ABILITY_MGR_SERVICE_ID, abilityObject);
}

void AbilityContextTest::TearDownTestCase(void)
{}

void AbilityContextTest::SetUp(void)
{
    context_ = std::make_unique<AbilityContext>();
}

void AbilityContextTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Ability_Context_ConnectAbility_0100
 * @tc.name: AbilityFwk
 * @tc.desc: When connecting ability, AMS will inform ability to process OnStart in the life cycle, and then inform
 * ability to process onconnect, and the connection is successful
 */
HWTEST_F(AbilityContextTest, AaFwk_Ability_Context_ConnectAbility_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->type = AppExecFwk::AbilityType::SERVICE;
    abilityInfo->name = "DemoAbility";
    std::shared_ptr<AbilityLocalRecord> abilityRecord = std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

    AbilityThread::AbilityThreadMain(application, abilityRecord);

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetAbilityInfo(abilityInfo);
    context_->AttachBaseContext(deal);

    Want want;
    bool ret = context_->ConnectAbility(want, nullptr);
    EXPECT_TRUE(ret);
    usleep(AbilityContextTest::TEST_WAIT_TIME);
}

/**
 * @tc.number: AaFwk_Ability_Context_DisconnectAbility_0100
 * @tc.name: AbilityFwk
 * @tc.desc: AMS notifies the abilityondisconnect event when disconnectservice.
 */
HWTEST_F(AbilityContextTest, AaFwk_Ability_Context_DisconnectAbility_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->type = AppExecFwk::AbilityType::SERVICE;
    abilityInfo->name = "DemoAbility";
    std::shared_ptr<AbilityLocalRecord> abilityRecord = std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

    AbilityThread::AbilityThreadMain(application, abilityRecord);

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetAbilityInfo(abilityInfo);
    context_->AttachBaseContext(deal);

    Want want;
    context_->ConnectAbility(want, nullptr);
    context_->DisconnectAbility(nullptr);
    usleep(AbilityContextTest::TEST_WAIT_TIME);
}

/**
 * @tc.number: AaFwk_Ability_Context_StartAbility_0100
 * @tc.name: AbilityFwk
 * @tc.desc: Starting ability service, AMS will inform ability to perform OnStart lifecycle conversion, and then inform
 * oncommand event.
 */
HWTEST_F(AbilityContextTest, AaFwk_Ability_Context_StartAbility_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->type = AppExecFwk::AbilityType::SERVICE;
    abilityInfo->name = "DemoAbility";
    std::shared_ptr<AbilityLocalRecord> abilityRecord = std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

    AbilityThread::AbilityThreadMain(application, abilityRecord);
    Want want;
    context_->StartAbility(want, -1);
    usleep(AbilityContextTest::TEST_WAIT_TIME);
}

/**
 * @tc.number: AaFwk_Ability_Context_TerminateAbility_0100
 * @tc.name: AbilityFwk
 * @tc.desc: To terminate ability service, AMS will notify ability to perform onbackground lifecycle conversion, and
 * then notify onstop event.
 */
HWTEST_F(AbilityContextTest, AaFwk_Ability_Context_TerminateAbility_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->type = AppExecFwk::AbilityType::SERVICE;
    abilityInfo->name = "DemoAbility";
    std::shared_ptr<AbilityLocalRecord> abilityRecord = std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

    AbilityThread::AbilityThreadMain(application, abilityRecord);
    Want want;
    context_->StartAbility(want, -1);
    usleep(AbilityContextTest::TEST_WAIT_TIME);

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetAbilityInfo(abilityInfo);
    context_->AttachBaseContext(deal);
    context_->TerminateAbility();
    usleep(AbilityContextTest::TEST_WAIT_TIME);
}

/**
 * @tc.number: AaFwk_Ability_Context_TerminateAbility_0200
 * @tc.name: AbilityFwk
 * @tc.desc: When there is no startability, calling terminateability directly will not respond to onbackground and
 * onstop events.
 */
HWTEST_F(AbilityContextTest, AaFwk_Ability_Context_TerminateAbility_0200, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->type = AppExecFwk::AbilityType::SERVICE;
    abilityInfo->name = "DemoAbility";
    std::shared_ptr<AbilityLocalRecord> abilityRecord = std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

    AbilityThread::AbilityThreadMain(application, abilityRecord);

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetAbilityInfo(abilityInfo);
    context_->AttachBaseContext(deal);
    context_->TerminateAbility();
    usleep(AbilityContextTest::TEST_WAIT_TIME);
}

/**
 * @tc.number: AaFwk_Ability_Context_StopService_0100
 * @tc.name: AbilityFwk
 * @tc.desc: To stop ability service, AMS will notify ability to perform onbackground lifecycle conversion, and then
 * notify onstop event.
 */
HWTEST_F(AbilityContextTest, AaFwk_Ability_Context_StopService_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->type = AppExecFwk::AbilityType::SERVICE;
    abilityInfo->name = "DemoAbility";
    std::shared_ptr<AbilityLocalRecord> abilityRecord = std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

    AbilityThread::AbilityThreadMain(application, abilityRecord);

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetAbilityInfo(abilityInfo);
    context_->AttachBaseContext(deal);

    Want want;
    context_->StartAbility(want, -1);
    usleep(AbilityContextTest::TEST_WAIT_TIME);
    bool ret = context_->StopAbility(want);
    EXPECT_TRUE(ret);
    usleep(AbilityContextTest::TEST_WAIT_TIME);
}

/**
 * @tc.number: AaFwk_Ability_Context_StopService_0200
 * @tc.name: AbilityFwk
 * @tc.desc: When there is no startability, calling stop ability directly will not respond to onbackground and onstop
 * events.
 */
HWTEST_F(AbilityContextTest, AaFwk_Ability_Context_StopService_0200, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    sptr<IRemoteObject> abilityToken = sptr<IRemoteObject>(new AbilityThread());
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->type = AppExecFwk::AbilityType::SERVICE;
    abilityInfo->name = "DemoAbility";
    std::shared_ptr<AbilityLocalRecord> abilityRecord = std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

    AbilityThread::AbilityThreadMain(application, abilityRecord);
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetAbilityInfo(abilityInfo);
    context_->AttachBaseContext(deal);

    Want want;
    bool ret = context_->StopAbility(want);
    EXPECT_TRUE(ret);
    usleep(AbilityContextTest::TEST_WAIT_TIME);
}
}  // namespace AppExecFwk
}  // namespace OHOS