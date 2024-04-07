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
#include "ability_scheduler_proxy.h"
#include "ability_scheduler_stub.h"
#include "ability_scheduler_mock.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace AAFwk {
class AbilitySchedulerProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    sptr<AbilitySchedulerProxy> abilitySchedulerProxy_;
    sptr<AbilitySchedulerMock> mock_;
    sptr<AbilitySchedulerRecipient> abilitySchedulerRecipient_;
};

void AbilitySchedulerProxyTest::SetUpTestCase(void)
{}
void AbilitySchedulerProxyTest::TearDownTestCase(void)
{}
void AbilitySchedulerProxyTest::TearDown(void)
{}

void AbilitySchedulerProxyTest::SetUp(void)
{
    mock_ = new AbilitySchedulerMock();
    abilitySchedulerProxy_ = new AbilitySchedulerProxy(mock_);
    OHOS::AAFwk::AbilitySchedulerRecipient::RemoteDiedHandler callbake;
    abilitySchedulerRecipient_ = new AbilitySchedulerRecipient(callbake);
}

/*
 * Feature: AbilitySchedulerProxy
 * Function: AbilitySchedulerProxy
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify AbilitySchedulerProxy is create success
 */
HWTEST_F(AbilitySchedulerProxyTest, ability_scheduler_proxy_operating_001, TestSize.Level0)
{
    EXPECT_NE(abilitySchedulerProxy_, nullptr);
}

/*
 * Feature: AbilitySchedulerProxy
 * Function: AbilitySchedulerRecipient
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify AbilitySchedulerRecipient is create success
 */
HWTEST_F(AbilitySchedulerProxyTest, ability_scheduler_proxy_operating_002, TestSize.Level0)
{
    EXPECT_NE(abilitySchedulerRecipient_, nullptr);
}

/*
 * Feature: AbilitySchedulerProxy
 * Function: ScheduleAbilityTransaction
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerProxy ScheduleAbilityTransaction
 * EnvConditions: NA
 * CaseDescription: verify ScheduleAbilityTransaction Normal case
 */
HWTEST_F(AbilitySchedulerProxyTest, ability_scheduler_proxy_operating_003, TestSize.Level0)
{
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AbilitySchedulerMock::InvokeSendRequest));
    Want want;
    want.SetFlags(10);
    LifeCycleStateInfo info;
    abilitySchedulerProxy_->ScheduleAbilityTransaction(want, info);

    EXPECT_EQ(IAbilityScheduler::SCHEDULE_ABILITY_TRANSACTION, mock_->code_);
}

/*
 * Feature: AbilitySchedulerProxy
 * Function: ScheduleAbilityTransaction
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerProxy ScheduleAbilityTransaction
 * EnvConditions: NA
 * CaseDescription: verify ScheduleAbilityTransaction Return value exception
 */
HWTEST_F(AbilitySchedulerProxyTest, ability_scheduler_proxy_operating_004, TestSize.Level0)
{
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AbilitySchedulerMock::InvokeErrorSendRequest));
    Want want;
    want.SetFlags(10);
    LifeCycleStateInfo info;
    abilitySchedulerProxy_->ScheduleAbilityTransaction(want, info);

    EXPECT_EQ(IAbilityScheduler::SCHEDULE_ABILITY_TRANSACTION, mock_->code_);
}

/*
 * Feature: AbilitySchedulerProxy
 * Function: SendResult
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerProxy SendResult
 * EnvConditions: NA
 * CaseDescription: verify SendResult Normal case
 */
HWTEST_F(AbilitySchedulerProxyTest, ability_scheduler_proxy_operating_005, TestSize.Level0)
{
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AbilitySchedulerMock::InvokeSendRequest));
    Want want;
    want.SetFlags(10);
    abilitySchedulerProxy_->SendResult(9, -1, want);

    EXPECT_EQ(IAbilityScheduler::SEND_RESULT, mock_->code_);
}

/*
 * Feature: AbilitySchedulerProxy
 * Function: SendResult
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerProxy SendResult
 * EnvConditions: NA
 * CaseDescription: verify SendResult Return value exception
 */
HWTEST_F(AbilitySchedulerProxyTest, ability_scheduler_proxy_operating_006, TestSize.Level0)
{
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AbilitySchedulerMock::InvokeErrorSendRequest));
    Want want;
    want.SetFlags(10);
    abilitySchedulerProxy_->SendResult(9, -1, want);

    EXPECT_EQ(IAbilityScheduler::SEND_RESULT, mock_->code_);
}

/*
 * Feature: AbilitySchedulerProxy
 * Function: ScheduleConnectAbility
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerProxy ScheduleConnectAbility
 * EnvConditions: NA
 * CaseDescription: verify ScheduleConnectAbility Normal case
 */
HWTEST_F(AbilitySchedulerProxyTest, ability_scheduler_proxy_operating_007, TestSize.Level0)
{
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AbilitySchedulerMock::InvokeSendRequest));
    Want want;
    want.SetFlags(10);
    abilitySchedulerProxy_->ScheduleConnectAbility(want);

    EXPECT_EQ(IAbilityScheduler::SCHEDULE_ABILITY_CONNECT, mock_->code_);
}

/*
 * Feature: AbilitySchedulerProxy
 * Function: ScheduleConnectAbility
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerProxy ScheduleConnectAbility
 * EnvConditions: NA
 * CaseDescription: verify ScheduleConnectAbility Return value exception
 */
HWTEST_F(AbilitySchedulerProxyTest, ability_scheduler_proxy_operating_008, TestSize.Level0)
{
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AbilitySchedulerMock::InvokeErrorSendRequest));
    Want want;
    want.SetFlags(10);
    abilitySchedulerProxy_->ScheduleConnectAbility(want);

    EXPECT_EQ(IAbilityScheduler::SCHEDULE_ABILITY_CONNECT, mock_->code_);
}

/*
 * Feature: AbilitySchedulerProxy
 * Function: ScheduleDisconnectAbility
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerProxy ScheduleDisconnectAbility
 * EnvConditions: NA
 * CaseDescription: verify ScheduleDisconnectAbility Normal case
 */
HWTEST_F(AbilitySchedulerProxyTest, ability_scheduler_proxy_operating_009, TestSize.Level0)
{
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AbilitySchedulerMock::InvokeSendRequest));
    Want want;
    want.SetFlags(10);
    abilitySchedulerProxy_->ScheduleDisconnectAbility(want);

    EXPECT_EQ(IAbilityScheduler::SCHEDULE_ABILITY_DISCONNECT, mock_->code_);
}

/*
 * Feature: AbilitySchedulerProxy
 * Function: ScheduleDisconnectAbility
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerProxy ScheduleDisconnectAbility
 * EnvConditions: NA
 * CaseDescription: verify ScheduleDisconnectAbility Return value exception
 */
HWTEST_F(AbilitySchedulerProxyTest, ability_scheduler_proxy_operating_010, TestSize.Level0)
{
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AbilitySchedulerMock::InvokeErrorSendRequest));
    Want want;
    want.SetFlags(10);
    abilitySchedulerProxy_->ScheduleDisconnectAbility(want);

    EXPECT_EQ(IAbilityScheduler::SCHEDULE_ABILITY_DISCONNECT, mock_->code_);
}

/*
 * Feature: AbilitySchedulerProxy
 * Function: ScheduleCommandAbility
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerProxy ScheduleCommandAbility
 * EnvConditions: NA
 * CaseDescription: verify ScheduleCommandAbility Normal case
 */
HWTEST_F(AbilitySchedulerProxyTest, ability_scheduler_proxy_operating_011, TestSize.Level0)
{
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AbilitySchedulerMock::InvokeSendRequest));
    Want want;
    want.SetFlags(10);
    abilitySchedulerProxy_->ScheduleCommandAbility(want, false, 1);

    EXPECT_EQ(IAbilityScheduler::SCHEDULE_ABILITY_COMMAND, mock_->code_);
}

/*
 * Feature: AbilitySchedulerProxy
 * Function: ScheduleCommandAbility
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerProxy ScheduleCommandAbility
 * EnvConditions: NA
 * CaseDescription: verify ScheduleCommandAbility Return value exception
 */
HWTEST_F(AbilitySchedulerProxyTest, ability_scheduler_proxy_operating_012, TestSize.Level0)
{
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AbilitySchedulerMock::InvokeErrorSendRequest));
    Want want;
    want.SetFlags(10);
    abilitySchedulerProxy_->ScheduleCommandAbility(want, false, 1);

    EXPECT_EQ(IAbilityScheduler::SCHEDULE_ABILITY_COMMAND, mock_->code_);
}
}  // namespace AAFwk
}  // namespace OHOS