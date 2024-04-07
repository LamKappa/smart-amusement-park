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

#include "ability_lifecycle.h"
#include "mock_lifecycle_observer.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using Want = OHOS::AAFwk::Want;

class LifeCycleTest : public testing::Test {
public:
    LifeCycleTest() : lifeCycle_(nullptr)
    {}
    ~LifeCycleTest()
    {}
    std::shared_ptr<LifeCycle> lifeCycle_;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void LifeCycleTest::SetUpTestCase(void)
{}

void LifeCycleTest::TearDownTestCase(void)
{}

void LifeCycleTest::SetUp(void)
{
    lifeCycle_ = std::make_shared<LifeCycle>();
}

void LifeCycleTest::TearDown(void)
{}


/**
 * @tc.number: AaFwk_LifeCycle_GetLifecycleState_0100
 * @tc.name: GetLifecycleState
 * @tc.desc: Determine whether the return value of getlifecycle state is equal to undefined.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_GetLifecycleState_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_GetLifecycleState_0100 start";

    LifeCycle::Event state = lifeCycle_->GetLifecycleState();
    EXPECT_EQ(LifeCycle::Event::UNDEFINED, state);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_GetLifecycleState_0100 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_AddObserver_0100
 * @tc.name: AddObserver
 * @tc.desc: Determine whether the return value of getlifecycle state is equal to undefined.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_AddObserver_0100, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_AddObserver_0100 start";

    lifeCycle_->AddObserver(nullptr);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_AddObserver_0100 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_AddObserver_0200
 * @tc.name: AddObserver
 * @tc.desc: Determine whether the return value of getlifecycle state is equal to undefined.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_AddObserver_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_AddObserver_0200 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_AddObserver_0200 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_RemoveObserver_0100
 * @tc.name: RemoveObserver
 * @tc.desc: Test the removeobserver exception state.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_RemoveObserver_0100, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_RemoveObserver_0100 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
    lifeCycle_->RemoveObserver(nullptr);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_RemoveObserver_0100 end";
}
/**
 * @tc.number: AaFwk_LifeCycle_RemoveObserver_0200
 * @tc.name: RemoveObserver
 * @tc.desc: Test whether addobserver is added and confirm whether removeobserver is deleted.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_RemoveObserver_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_RemoveObserver_0200 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
    lifeCycle_->RemoveObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_RemoveObserver_0200 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_want_0100
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_want_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0100 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    Want want;
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::UNDEFINED, want);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0100 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_want_0200
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_want_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0200 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    Want want;
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE, want);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0200 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_want_0300
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_want_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0300 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    Want want;
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_BACKGROUND, want);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0300 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_want_0400
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_want_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0400 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    Want want;
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_FOREGROUND, want);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0400 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_want_0500
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_want_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0500 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    Want want;
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_INACTIVE, want);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0500 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_want_0600
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_want_0600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0600 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    Want want;
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_START, want);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_006 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_want_0700
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_want_0700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0700 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    Want want;
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_STOP, want);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_want_0700 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_0100
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0100 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::UNDEFINED);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0100 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_0200
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0200 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0200 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_0300
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0300 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_BACKGROUND);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0300 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_0400
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0400 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_FOREGROUND);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0400 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_0500
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0500 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_INACTIVE);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0500 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_0600
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_0600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0600 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_START);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0600 end";
}

/**
 * @tc.number: AaFwk_LifeCycle_DispatchLifecycle_event_0700
 * @tc.name: DispatchLifecycle
 * @tc.desc: Determine whether the current action is correct through dispatchlifecycle.
 */
HWTEST_F(LifeCycleTest, AaFwk_LifeCycle_DispatchLifecycle_event_0700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0700 start";

    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    lifeCycle_->AddObserver(observer);
    lifeCycle_->DispatchLifecycle(LifeCycle::Event::ON_STOP);

    GTEST_LOG_(INFO) << "AaFwk_LifeCycle_DispatchLifecycle_event_0700 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS
