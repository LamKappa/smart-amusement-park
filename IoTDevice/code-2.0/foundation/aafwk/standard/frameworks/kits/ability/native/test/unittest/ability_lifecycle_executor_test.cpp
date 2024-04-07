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

#include "ability_lifecycle_executor.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

class AbilityLifecycleExecutorTest : public testing::Test {
public:
    AbilityLifecycleExecutorTest() : abilityLifecycleExecutor_(nullptr)
    {}
    ~AbilityLifecycleExecutorTest()
    {}
    std::shared_ptr<AbilityLifecycleExecutor> abilityLifecycleExecutor_;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AbilityLifecycleExecutorTest::SetUpTestCase(void)
{}

void AbilityLifecycleExecutorTest::TearDownTestCase(void)
{}

void AbilityLifecycleExecutorTest::SetUp(void)
{
    abilityLifecycleExecutor_ = std::make_shared<AbilityLifecycleExecutor>();
}

void AbilityLifecycleExecutorTest::TearDown(void)
{}

// AaFwk_AbilityLifecycleExecutor_GetState

/**
 * @tc.number: AaFwk_AbilityLifecycleExecutor_GetState_0100
 * @tc.name: GetState
 * @tc.desc: Verify that the return value of getstate is UNINITIALIZED. 
 */
HWTEST_F(AbilityLifecycleExecutorTest, AaFwk_AbilityLifecycleExecutor_GetState_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityLifecycleExecutor_GetState_0100 start";

    AbilityLifecycleExecutor::LifecycleState state =
        (AbilityLifecycleExecutor::LifecycleState)abilityLifecycleExecutor_->GetState();
    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::UNINITIALIZED, state);

    GTEST_LOG_(INFO) << "AaFwk_AbilityLifecycleExecutor_GetState_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0100
 * @tc.name: DispatchLifecycleState
 * @tc.desc: Test whether attachbasecontext is called normally, 
 *           and verify whether the return value of getdatabasedir is ACTIVE. 
 */
HWTEST_F(AbilityLifecycleExecutorTest, AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0100 start";

    AbilityLifecycleExecutor::LifecycleState targetState = AbilityLifecycleExecutor::LifecycleState::ACTIVE;
    abilityLifecycleExecutor_->DispatchLifecycleState(targetState);
    AbilityLifecycleExecutor::LifecycleState state =
        (AbilityLifecycleExecutor::LifecycleState)abilityLifecycleExecutor_->GetState();
    EXPECT_EQ(targetState, state);

    GTEST_LOG_(INFO) << "AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0200
 * @tc.name: DispatchLifecycleState
 * @tc.desc: Test whether attachbasecontext is called normally, 
 *           and verify whether the return value of getdatabasedir is BACKGROUND. 
 */
HWTEST_F(AbilityLifecycleExecutorTest, AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0200 start";

    AbilityLifecycleExecutor::LifecycleState targetState = AbilityLifecycleExecutor::LifecycleState::BACKGROUND;
    abilityLifecycleExecutor_->DispatchLifecycleState(targetState);
    AbilityLifecycleExecutor::LifecycleState state =
        (AbilityLifecycleExecutor::LifecycleState)abilityLifecycleExecutor_->GetState();
    EXPECT_EQ(targetState, state);

    GTEST_LOG_(INFO) << "AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0300
 * @tc.name: DispatchLifecycleState
 * @tc.desc: Test whether attachbasecontext is called normally, 
 *           and verify whether the return value of getdatabasedir is INACTIVE. 
 */
HWTEST_F(AbilityLifecycleExecutorTest, AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0300 start";

    AbilityLifecycleExecutor::LifecycleState targetState = AbilityLifecycleExecutor::LifecycleState::INACTIVE;
    abilityLifecycleExecutor_->DispatchLifecycleState(targetState);
    AbilityLifecycleExecutor::LifecycleState state =
        (AbilityLifecycleExecutor::LifecycleState)abilityLifecycleExecutor_->GetState();
    EXPECT_EQ(targetState, state);

    GTEST_LOG_(INFO) << "AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0300 end";
}

/**
 * @tc.number: AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0400
 * @tc.name: DispatchLifecycleState
 * @tc.desc: Test whether attachbasecontext is called normally, 
 *           and verify whether the return value of getdatabasedir is INITIAL. 
 */
HWTEST_F(AbilityLifecycleExecutorTest, AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0400 start";

    AbilityLifecycleExecutor::LifecycleState targetState = AbilityLifecycleExecutor::LifecycleState::INITIAL;
    abilityLifecycleExecutor_->DispatchLifecycleState(targetState);
    AbilityLifecycleExecutor::LifecycleState state =
        (AbilityLifecycleExecutor::LifecycleState)abilityLifecycleExecutor_->GetState();
    EXPECT_EQ(targetState, state);

    GTEST_LOG_(INFO) << "AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0400 end";
}

/**
 * @tc.number: AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0500
 * @tc.name: DispatchLifecycleState
 * @tc.desc: Test whether attachbasecontext is called normally, 
 *           and verify whether the return value of getdatabasedir is UNINITIALIZED. 
 */
HWTEST_F(AbilityLifecycleExecutorTest, AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0500 start";

    AbilityLifecycleExecutor::LifecycleState targetState = AbilityLifecycleExecutor::LifecycleState::UNINITIALIZED;
    abilityLifecycleExecutor_->DispatchLifecycleState(targetState);
    AbilityLifecycleExecutor::LifecycleState state =
        (AbilityLifecycleExecutor::LifecycleState)abilityLifecycleExecutor_->GetState();
    EXPECT_EQ(targetState, state);

    GTEST_LOG_(INFO) << "AaFwk_AbilityLifecycleExecutor_DispatchLifecycleState_0500 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS