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

#include "start_via_asan.h"
#include <gtest/gtest.h>
#include "properties.h"
#include "app_log_wrapper.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

class AmsStartViaAsanTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AmsStartViaAsanTest::SetUpTestCase()
{}

void AmsStartViaAsanTest::TearDownTestCase()
{}

void AmsStartViaAsanTest::SetUp()
{}

void AmsStartViaAsanTest::TearDown()
{}

/*
 * Feature: StartViaAsan
 * Function: ASAN
 * SubFunction: IsAsanVersion
 * FunctionPoints: Use interface GetSysPara to get wrap.appName attribute.
 * EnvConditions: NA
 * CaseDescription: Judge whether the wrap attribute exists in the environment.
 */
HWTEST_F(AmsStartViaAsanTest, IsAsanVersion_001, TestSize.Level0)
{
    APP_LOGD("IsAsanVersion_001 start");
    std::string appName = "com.ohos.hiworld";
    bool result = DelayedSingleton<StartViaAsan>::GetInstance()->IsAsanVersion(appName);
    EXPECT_EQ(result, false);

    APP_LOGD("IsAsanVersion_001 end");
}

/*
 * Feature: StartViaAsan
 * Function: ASAN
 * SubFunction: GetAsanStartMsg
 * FunctionPoints: Add wrap attribute to AppSpawnStartMsg object
 * EnvConditions: NA
 * CaseDescription: Add the wrap attribute to the arg field of the AppSpawnStartMsg object
 */
HWTEST_F(AmsStartViaAsanTest, GetAsanStartMsg_001, TestSize.Level0)
{
    APP_LOGD("GetAsanStartMsg_001 start");
    std::string appName = "com.ohos.hiworld";
    AppSpawnStartMsg startMsg = {appName, "clsName", "funcName", "soPath"};
    DelayedSingleton<StartViaAsan>::GetInstance()->GetAsanStartMsg(startMsg);
    EXPECT_EQ(startMsg.arg, "wrap." + appName);
    APP_LOGD("GetAsanStartMsg_001 end");
}