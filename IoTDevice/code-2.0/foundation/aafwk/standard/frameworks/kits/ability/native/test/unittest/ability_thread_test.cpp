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
#include <functional>
#include "ability_thread.h"
#include "ability_state.h"
#include "ability_loader.h"
#include "app_log_wrapper.h"
#include "ability_impl_factory.h"
#include "ability.h"
#include "ability_impl.h"
#include "context_deal.h"
#include "mock_page_ability.h"
#include "mock_service_ability.h"
#include "mock_ability_token.h"
#include "mock_ability_lifecycle_callbacks.h"
#include "mock_ability_impl.h"
#include "mock_ability_thread.h"
#include "mock_data_ability.h"
#include "ohos_application.h"
#include "page_ability_impl.h"
#include "uri.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

REGISTER_AA(MockDataAbility)
REGISTER_AA(MockPageAbility)
REGISTER_AA(MockServiceAbility)

class AbilityThreadTest : public testing::Test {
public:
    AbilityThreadTest() : abilitythread_(nullptr)
    {}
    ~AbilityThreadTest()
    {
        abilitythread_ = nullptr;
    }
    AbilityThread *abilitythread_;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AbilityThreadTest::SetUpTestCase(void)
{}

void AbilityThreadTest::TearDownTestCase(void)
{}

void AbilityThreadTest::SetUp(void)
{
    GTEST_LOG_(INFO) << "AbilityThreadTest SetUp";
}

void AbilityThreadTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "AbilityThreadTest TearDown";
}

/**
 * @tc.number: AaFwk_AbilityThread_ScheduleSaveAbilityState_0100
 * @tc.name: ScheduleSaveAbilityState
 * @tc.desc: Simulate successful test cases
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_ScheduleSaveAbilityState_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleSaveAbilityState_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            abilitythread->Attach(application, abilityRecord, mainRunner);

            PacMap state;
            abilitythread->ScheduleSaveAbilityState(state);

            sleep(1);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleSaveAbilityState_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_ScheduleSaveAbilityState_0200
 * @tc.name: ScheduleSaveAbilityState
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_ScheduleSaveAbilityState_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleSaveAbilityState_0200 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        PacMap state;
        abilitythread->ScheduleSaveAbilityState(state);
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleSaveAbilityState_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_ScheduleRestoreAbilityState_0100
 * @tc.name: ScheduleRestoreAbilityState
 * @tc.desc: Simulate successful test cases
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_ScheduleRestoreAbilityState_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleRestoreAbilityState_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            abilitythread->Attach(application, abilityRecord, mainRunner);
            PacMap state;

            abilitythread->ScheduleRestoreAbilityState(state);

            sleep(1);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleRestoreAbilityState_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_ScheduleRestoreAbilityState_0200
 * @tc.name: ScheduleRestoreAbilityState
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_ScheduleRestoreAbilityState_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleRestoreAbilityState_0200 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        PacMap state;
        abilitythread->ScheduleSaveAbilityState(state);
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleRestoreAbilityState_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_Attach_3_Param_0100
 * @tc.name: Attach
 * @tc.desc: Simulate successful test cases
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_Attach_3_Param_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Attach_3_Param_0100 start";
    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            abilitythread->Attach(application, abilityRecord, mainRunner);
            sleep(1);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Attach_3_Param_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_Attach_3_Param_0200
 * @tc.name: Attach
 * @tc.desc: Simulate successful test cases
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_Attach_3_Param_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Attach_3_Param_0200 start";
    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = nullptr;
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            abilitythread->Attach(application, abilityRecord, mainRunner);

            sleep(1);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Attach_3_Param_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_Attach_2_Param_0100
 * @tc.name: Attach
 * @tc.desc: Simulate successful test cases
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_Attach_2_Param_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Attach_2_Param_0100 start";
    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            abilitythread->Attach(application, abilityRecord);

            sleep(1);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Attach_2_Param_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_Attach_2_Param_0200
 * @tc.name: Attach
 * @tc.desc: Simulate successful test cases
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_Attach_2_Param_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Attach_2_Param_0200 start";
    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = nullptr;
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);

            abilitythread->Attach(application, abilityRecord);

            sleep(1);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Attach_2_Param_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_ScheduleAbilityTransaction_0100
 * @tc.name: ScheduleAbilityTransaction
 * @tc.desc: Simulate successful test cases
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_ScheduleAbilityTransaction_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleAbilityTransaction_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            abilitythread->Attach(application, abilityRecord, mainRunner);

            Want want;
            LifeCycleStateInfo lifeCycleStateInfo;
            abilitythread->ScheduleAbilityTransaction(want, lifeCycleStateInfo);

            sleep(1);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleAbilityTransaction_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_ScheduleAbilityTransaction_0200
 * @tc.name: ScheduleAbilityTransaction
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_ScheduleAbilityTransaction_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleAbilityTransaction_0200 start";
    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            abilitythread->Attach(application, abilityRecord, nullptr);

            Want want;
            LifeCycleStateInfo lifeCycleStateInfo;
            abilitythread->ScheduleAbilityTransaction(want, lifeCycleStateInfo);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleAbilityTransaction_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_ScheduleConnectAbility_0100
 * @tc.name: ScheduleConnectAbility
 * @tc.desc: Simulate successful test cases
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_ScheduleConnectAbility_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleConnectAbility_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            abilitythread->Attach(application, abilityRecord, mainRunner);

            Want want;
            abilitythread->ScheduleConnectAbility(want);

            sleep(1);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleConnectAbility_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_ScheduleConnectAbility_0200
 * @tc.name: ScheduleConnectAbility
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_ScheduleConnectAbility_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleConnectAbility_0200 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        Want want;
        abilitythread->ScheduleConnectAbility(want);
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleConnectAbility_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_ScheduleDisconnectAbility_0100
 * @tc.name: ScheduleDisconnectAbility
 * @tc.desc: Simulate successful test cases
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_ScheduleDisconnectAbility_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleDisconnectAbility_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            abilitythread->Attach(application, abilityRecord, mainRunner);

            Want want;
            abilitythread->ScheduleDisconnectAbility(want);

            sleep(1);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleDisconnectAbility_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_ScheduleDisconnectAbility_0200
 * @tc.name: ScheduleDisconnectAbility
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_ScheduleDisconnectAbility_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleDisconnectAbility_0200 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityImpl> abilityimpl = std::make_shared<AbilityImpl>();

        Want want;
        abilitythread->ScheduleDisconnectAbility(want);
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleDisconnectAbility_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_ScheduleCommandAbility_0100
 * @tc.name: ScheduleCommandAbility
 * @tc.desc: Simulate successful test cases
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_ScheduleCommandAbility_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleCommandAbility_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockServiceAbility";
        abilityInfo->type = AbilityType::SERVICE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            std::shared_ptr<AbilityImpl> abilityimpl = std::make_shared<AbilityImpl>();
            abilitythread->Attach(application, abilityRecord, mainRunner);

            Want want;
            bool restart = true;
            int startId = 0;

            abilitythread->ScheduleCommandAbility(want, restart, startId);

            sleep(1);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleCommandAbility_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_ScheduleCommandAbility_0200
 * @tc.name: ScheduleCommandAbility
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_ScheduleCommandAbility_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleCommandAbility_0200 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        Want want;
        bool restart = true;
        int startId = 0;

        abilitythread->ScheduleCommandAbility(want, restart, startId);
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_ScheduleCommandAbility_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_SendResult_0100
 * @tc.name: SendResult
 * @tc.desc: Simulate successful test cases
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_SendResult_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_SendResult_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            std::shared_ptr<AbilityImpl> abilityimpl = std::make_shared<AbilityImpl>();
            abilitythread->Attach(application, abilityRecord, mainRunner);

            int requestCode = 0;
            int resultCode = 0;
            Want want;
            abilitythread->SendResult(requestCode, resultCode, want);

            sleep(1);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_SendResult_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_SendResult_0200
 * @tc.name: SendResult
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_SendResult_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_SendResult_0200 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityImpl> abilityimpl = std::make_shared<AbilityImpl>();

        int requestCode = 0;
        int resultCode = 0;
        Want want;
        abilitythread->SendResult(requestCode, resultCode, want);
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_SendResult_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_AbilityThreadMain_0100
 * @tc.name: AbilityThreadMain
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_AbilityThreadMain_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_AbilityThreadMain_0100 start";
    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);

            abilitythread->AbilityThreadMain(application, abilityRecord, mainRunner);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_AbilityThreadMain_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_AbilityThreadMain_0200
 * @tc.name: AbilityThreadMain
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_AbilityThreadMain_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_AbilityThreadMain_0200 start";
    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = nullptr;
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);

            abilitythread->AbilityThreadMain(application, abilityRecord, mainRunner);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_AbilityThreadMain_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_AbilityThreadMain_0300
 * @tc.name: AbilityThreadMain
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_AbilityThreadMain_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_AbilityThreadMain_0300 start";
    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);

            abilitythread->AbilityThreadMain(application, abilityRecord);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_AbilityThreadMain_0300 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_AbilityThreadMain_0400
 * @tc.name: AbilityThreadMain
 * @tc.desc: Validate when normally entering a string
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_AbilityThreadMain_0400, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_AbilityThreadMain_0400 start";
    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockPageAbility";
        abilityInfo->type = AbilityType::PAGE;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = nullptr;
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);

            abilitythread->AbilityThreadMain(application, abilityRecord);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_AbilityThreadMain_0400 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS