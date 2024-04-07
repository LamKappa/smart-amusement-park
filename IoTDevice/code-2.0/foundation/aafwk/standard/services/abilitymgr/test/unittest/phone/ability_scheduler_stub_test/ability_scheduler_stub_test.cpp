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
#include "ability_schedule_stub_mock.h"

using namespace testing::ext;

namespace OHOS {
namespace AAFwk {
class AbilitySchedulerStubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void WriteInterfaceToken(MessageParcel &data);
    sptr<AbilitySchedulerStubMock> stub_;
};

void AbilitySchedulerStubTest::SetUpTestCase(void)
{}
void AbilitySchedulerStubTest::TearDownTestCase(void)
{}
void AbilitySchedulerStubTest::TearDown(void)
{}

void AbilitySchedulerStubTest::SetUp(void)
{
    stub_ = new AbilitySchedulerStubMock();
}
void AbilitySchedulerStubTest::WriteInterfaceToken(MessageParcel &data)
{
    data.WriteInterfaceToken(AbilitySchedulerStub::GetDescriptor());
}

/*
 * Feature: AbilitySchedulerStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerStub OnRemoteRequest
 * EnvConditions: code is SCHEDULE_ABILITY_TRANSACTION
 * CaseDescription: Verify the normal process of onremoterequest
 */
HWTEST_F(AbilitySchedulerStubTest, AbilitySchedulerStub_001, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    Want want;
    LifeCycleStateInfo stateInfo;
    WriteInterfaceToken(data);
    data.WriteParcelable(&want);
    data.WriteParcelable(&stateInfo);
    auto res = stub_->OnRemoteRequest(IAbilityScheduler::SCHEDULE_ABILITY_TRANSACTION, data, reply, option);
    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilitySchedulerStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerStub OnRemoteRequest
 * EnvConditions: code is SCHEDULE_ABILITY_TRANSACTION
 * CaseDescription: Verifying stateinfo is nullptr causes onremoterequest to fail
 */
HWTEST_F(AbilitySchedulerStubTest, AbilitySchedulerStub_002, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    Want want;
    WriteInterfaceToken(data);
    data.WriteParcelable(&want);
    data.WriteParcelable(nullptr);
    auto res = stub_->OnRemoteRequest(IAbilityScheduler::SCHEDULE_ABILITY_TRANSACTION, data, reply, option);
    EXPECT_NE(res, NO_ERROR);
}

/*
 * Feature: AbilitySchedulerStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerStub OnRemoteRequest
 * EnvConditions: code is SEND_RESULT
 * CaseDescription: Verify the normal process of onremoterequest
 */
HWTEST_F(AbilitySchedulerStubTest, AbilitySchedulerStub_003, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    Want want;
    WriteInterfaceToken(data);
    data.WriteInt32(1);
    data.WriteInt32(1);
    data.WriteParcelable(&want);

    auto res = stub_->OnRemoteRequest(IAbilityScheduler::SEND_RESULT, data, reply, option);
    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilitySchedulerStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerStub OnRemoteRequest
 * EnvConditions: code is SEND_RESULT
 * CaseDescription: Verifying want is nullptr causes onremoterequest to fail
 */
HWTEST_F(AbilitySchedulerStubTest, AbilitySchedulerStub_004, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WriteInterfaceToken(data);
    data.WriteInt32(1);
    data.WriteParcelable(nullptr);

    auto res = stub_->OnRemoteRequest(IAbilityScheduler::SEND_RESULT, data, reply, option);
    EXPECT_NE(res, NO_ERROR);
}

/*
 * Feature: AbilitySchedulerStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerStub OnRemoteRequest
 * EnvConditions: code is SCHEDULE_ABILITY_CONNECT
 * CaseDescription: Verify the normal and failed conditions of onremoterequest
 */
HWTEST_F(AbilitySchedulerStubTest, AbilitySchedulerStub_005, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    WriteInterfaceToken(data);
    data.WriteParcelable(nullptr);

    auto res = stub_->OnRemoteRequest(IAbilityScheduler::SCHEDULE_ABILITY_CONNECT, data, reply, option);
    EXPECT_NE(res, NO_ERROR);

    Want want;
    WriteInterfaceToken(data);
    data.WriteParcelable(&want);
    auto res1 = stub_->OnRemoteRequest(IAbilityScheduler::SCHEDULE_ABILITY_CONNECT, data, reply, option);
    EXPECT_EQ(res1, NO_ERROR);
}

/*
 * Feature: AbilitySchedulerStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerStub OnRemoteRequest
 * EnvConditions: code is SCHEDULE_ABILITY_DISCONNECT
 * CaseDescription: Verify the normal conditions of onremoterequest
 */
HWTEST_F(AbilitySchedulerStubTest, AbilitySchedulerStub_006, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WriteInterfaceToken(data);
    auto res = stub_->OnRemoteRequest(IAbilityScheduler::SCHEDULE_ABILITY_DISCONNECT, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_VALUE);
}

/*
 * Feature: AbilitySchedulerStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerStub OnRemoteRequest
 * EnvConditions: code is SCHEDULE_SAVE_ABILITY_STATE
 * CaseDescription: Verify the failed conditions of onremoterequest
 */
HWTEST_F(AbilitySchedulerStubTest, AbilitySchedulerStub_007, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WriteInterfaceToken(data);
    auto res = stub_->OnRemoteRequest(IAbilityScheduler::SCHEDULE_SAVE_ABILITY_STATE, data, reply, option);
    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilitySchedulerStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilitySchedulerStub OnRemoteRequest
 * EnvConditions: code is default
 * CaseDescription: Verify the normal conditions of onremoterequest
 */
HWTEST_F(AbilitySchedulerStubTest, AbilitySchedulerStub_008, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WriteInterfaceToken(data);
    auto res = stub_->OnRemoteRequest(INT_MAX, data, reply, option);
    EXPECT_NE(res, NO_ERROR);
}
}  // namespace AAFwk
}  // namespace OHOS