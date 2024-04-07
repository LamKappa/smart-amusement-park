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
#include "lifecycle_deal.h"
#include "ability_scheduler_mock.h"

using namespace testing::ext;

namespace OHOS {
namespace AAFwk {
class LifecycleDealTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<LifecycleDeal> lifecycleDeal_;
    sptr<AbilitySchedulerMock> abilityScheduler_;
};

void LifecycleDealTest::SetUpTestCase(void)
{}
void LifecycleDealTest::TearDownTestCase(void)
{}
void LifecycleDealTest::TearDown()
{}

void LifecycleDealTest::SetUp()
{
    lifecycleDeal_ = std::make_shared<LifecycleDeal>();
    abilityScheduler_ = new AbilitySchedulerMock();
}

/*
 * Feature: LifecycleDeal
 * Function: Activate
 * SubFunction: NA
 * FunctionPoints: LifecycleDeal Activate
 * EnvConditions:NA
 * CaseDescription: Verify activate operation and call mock once
 */
HWTEST_F(LifecycleDealTest, LifecycleDeal_oprator_001, TestSize.Level0)
{
    LifeCycleStateInfo val;
    EXPECT_CALL(*abilityScheduler_, ScheduleAbilityTransaction(::testing::_, ::testing::_))
        .Times(1)
        .WillOnce(testing::SaveArg<1>(&val));

    const Want want;
    CallerInfo caller;
    caller.deviceId = "device";
    caller.bundleName = "bundle";
    caller.abilityName = "LifecycleDealTest";

    LifeCycleStateInfo info;
    info.caller = caller;
    lifecycleDeal_->Activate(want, info);
    lifecycleDeal_->SetScheduler(abilityScheduler_);
    lifecycleDeal_->Activate(want, info);

    EXPECT_EQ(val.caller.deviceId, caller.deviceId);
    EXPECT_EQ(val.caller.bundleName, caller.bundleName);
    EXPECT_EQ(val.caller.abilityName, caller.abilityName);
}

/*
 * Feature: LifecycleDeal
 * Function: Inactivate
 * SubFunction: NA
 * FunctionPoints: LifecycleDeal Inactivate
 * EnvConditions:NA
 * CaseDescription: Verify Inactivate operation and call mock once
 */
HWTEST_F(LifecycleDealTest, LifecycleDeal_oprator_002, TestSize.Level0)
{
    LifeCycleStateInfo val;
    EXPECT_CALL(*abilityScheduler_, ScheduleAbilityTransaction(::testing::_, ::testing::_))
        .Times(1)
        .WillOnce(testing::SaveArg<1>(&val));

    const Want want;
    CallerInfo caller;
    caller.deviceId = "device";
    caller.bundleName = "bundle";
    caller.abilityName = "LifecycleDealTest";

    LifeCycleStateInfo info;
    info.caller = caller;
    lifecycleDeal_->Inactivate(want, info);
    lifecycleDeal_->SetScheduler(abilityScheduler_);
    lifecycleDeal_->Inactivate(want, info);

    EXPECT_EQ(val.caller.deviceId, caller.deviceId);
    EXPECT_EQ(val.caller.bundleName, caller.bundleName);
    EXPECT_EQ(val.caller.abilityName, caller.abilityName);
}

/*
 * Feature: LifecycleDeal
 * Function: MoveToBackground
 * SubFunction: NA
 * FunctionPoints: LifecycleDeal MoveToBackground
 * EnvConditions:NA
 * CaseDescription: Verify MoveToBackground operation and call mock once
 */
HWTEST_F(LifecycleDealTest, LifecycleDeal_oprator_003, TestSize.Level0)
{
    LifeCycleStateInfo val;
    EXPECT_CALL(*abilityScheduler_, ScheduleAbilityTransaction(::testing::_, ::testing::_))
        .Times(1)
        .WillOnce(testing::SaveArg<1>(&val));

    const Want want;
    CallerInfo caller;
    caller.deviceId = "device";
    caller.bundleName = "bundle";
    caller.abilityName = "LifecycleDealTest";

    LifeCycleStateInfo info;
    info.caller = caller;
    lifecycleDeal_->MoveToBackground(want, info);
    lifecycleDeal_->SetScheduler(abilityScheduler_);
    lifecycleDeal_->MoveToBackground(want, info);

    EXPECT_EQ(val.caller.deviceId, caller.deviceId);
    EXPECT_EQ(val.caller.bundleName, caller.bundleName);
    EXPECT_EQ(val.caller.abilityName, caller.abilityName);
}

/*
 * Feature: LifecycleDeal
 * Function: ConnectAbility
 * SubFunction: NA
 * FunctionPoints: LifecycleDeal ConnectAbility
 * EnvConditions:NA
 * CaseDescription: Verify ConnectAbility operation and call mock once
 */
HWTEST_F(LifecycleDealTest, LifecycleDeal_oprator_004, TestSize.Level0)
{
    EXPECT_CALL(*abilityScheduler_, ScheduleConnectAbility(::testing::_)).Times(1);
    const Want want;
    lifecycleDeal_->ConnectAbility(want);
    lifecycleDeal_->SetScheduler(abilityScheduler_);
    lifecycleDeal_->ConnectAbility(want);
}

/*
 * Feature: LifecycleDeal
 * Function: DisconnectAbility
 * SubFunction: NA
 * FunctionPoints: LifecycleDeal DisconnectAbility
 * EnvConditions:NA
 * CaseDescription: Verify DisconnectAbility operation and call mock once
 */
HWTEST_F(LifecycleDealTest, LifecycleDeal_oprator_005, TestSize.Level0)
{
    EXPECT_CALL(*abilityScheduler_, ScheduleDisconnectAbility(::testing::_)).Times(1);

    const Want want;
    lifecycleDeal_->DisconnectAbility(want);
    lifecycleDeal_->SetScheduler(abilityScheduler_);
    lifecycleDeal_->DisconnectAbility(want);
}

/*
 * Feature: LifecycleDeal
 * Function: Terminate
 * SubFunction: NA
 * FunctionPoints: LifecycleDeal Terminate
 * EnvConditions:NA
 * CaseDescription: Verify Terminate operation and call mock once
 */
HWTEST_F(LifecycleDealTest, LifecycleDeal_oprator_006, TestSize.Level0)
{
    EXPECT_CALL(*abilityScheduler_, ScheduleAbilityTransaction(::testing::_, ::testing::_)).Times(1);

    const Want want;
    CallerInfo caller;
    caller.deviceId = "device";
    caller.bundleName = "bundle";
    caller.abilityName = "LifecycleDealTest";

    LifeCycleStateInfo info;
    info.caller = caller;
    lifecycleDeal_->Activate(want, info);
    lifecycleDeal_->SetScheduler(abilityScheduler_);
    lifecycleDeal_->Activate(want, info);
}

/*
 * Feature: LifecycleDeal
 * Function: CommandAbility
 * SubFunction: NA
 * FunctionPoints: LifecycleDeal CommandAbility
 * EnvConditions:NA
 * CaseDescription: Verify CommandAbility operation and call mock once
 */
HWTEST_F(LifecycleDealTest, LifecycleDeal_oprator_007, TestSize.Level0)
{
    EXPECT_CALL(*abilityScheduler_, ScheduleCommandAbility(::testing::_, ::testing::_, ::testing::_)).Times(1);
    const Want want;
    LifeCycleStateInfo info;
    lifecycleDeal_->CommandAbility(want, false, 1);
    lifecycleDeal_->SetScheduler(abilityScheduler_);
    lifecycleDeal_->CommandAbility(want, false, 1);
}
}  // namespace AAFwk
}  // namespace OHOS