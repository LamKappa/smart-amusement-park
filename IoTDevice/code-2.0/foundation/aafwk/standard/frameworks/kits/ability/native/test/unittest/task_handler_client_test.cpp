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

#include "task_handler_client.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

class TaskHandlerClientTest : public testing::Test {
public:
    TaskHandlerClientTest()
    {}
    ~TaskHandlerClientTest()
    {}

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void TaskHandlerClientTest::SetUpTestCase(void)
{}

void TaskHandlerClientTest::TearDownTestCase(void)
{}

void TaskHandlerClientTest::SetUp(void)
{}

void TaskHandlerClientTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_TaskHandlerClient_GetInstance_0100
 * @tc.name: GetInstance
 * @tc.desc: Test TaskHandlerClient::GetInstance is not empty, and test whether it is called normally.
 */
HWTEST_F(TaskHandlerClientTest, AaFwk_TaskHandlerClient_GetInstance_0100, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_TaskHandlerClient_GetInstance_0100 start";

    EXPECT_NE(TaskHandlerClient::GetInstance(), nullptr);
    EXPECT_NE(TaskHandlerClient::GetInstance(), nullptr);

    GTEST_LOG_(INFO) << "AaFwk_TaskHandlerClient_GetInstance_0100 end";
}

/**
 * @tc.number: AaFwk_TaskHandlerClient_CreateRunner_0100
 * @tc.name: CreateRunner
 * @tc.desc: Test whether TaskHandlerClient::CreateRunner is true.
 */
HWTEST_F(TaskHandlerClientTest, AaFwk_TaskHandlerClient_CreateRunner_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_TaskHandlerClient_CreateRunner_0100 start";

    EXPECT_EQ(TaskHandlerClient::GetInstance()->CreateRunner(), true);

    GTEST_LOG_(INFO) << "AaFwk_TaskHandlerClient_CreateRunner_0100 end";
}

/**
 * @tc.number: AaFwk_TaskHandlerClient_PostTask_0100
 * @tc.name: PostTask
 * @tc.desc: Test whether TaskHandlerClient::PostTask is true.
 */
HWTEST_F(TaskHandlerClientTest, AaFwk_TaskHandlerClient_PostTask_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_TaskHandlerClient_PostTask_0100 start";

    auto task = []() { GTEST_LOG_(INFO) << "AaFwk_TaskHandlerClient_PostTask_0100 task called"; };

    EXPECT_EQ(TaskHandlerClient::GetInstance()->PostTask(task, 1000), true);

    GTEST_LOG_(INFO) << "AaFwk_TaskHandlerClient_PostTask_0100 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS