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

#include "securec.h"
#include "app_log_wrapper.h"
#include "app_spawn_msg_wrapper.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

// this function is only used to mock sleep method so ut can run without delay.
int MockSleep([[maybe_unused]] uint32_t seconds)
{
    return 0;
}

class AmsServiceAppSpawnMsgWrapperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AmsServiceAppSpawnMsgWrapperTest::SetUpTestCase()
{}

void AmsServiceAppSpawnMsgWrapperTest::TearDownTestCase()
{}

void AmsServiceAppSpawnMsgWrapperTest::SetUp()
{}

void AmsServiceAppSpawnMsgWrapperTest::TearDown()
{}

/*
 * Feature: AppMgrService
 * Function: AppSpawnMsgWrapper
 * SubFunction: AssembleMsg
 * FunctionPoints: check message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function AssembleMsg can check the invalid uid.
 */
HWTEST(AmsServiceAppSpawnMsgWrapperTest, AppSpawnMsgWrapper_001, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_msg_wrapper_001 start");

    std::unique_ptr<AppSpawnMsgWrapper> appSpawnMsgWrapper = std::make_unique<AppSpawnMsgWrapper>();

    AppSpawnStartMsg params = {10000, 10000, {10001, 10002}, "processName", "soPath"};
    EXPECT_EQ(true, appSpawnMsgWrapper->AssembleMsg(params));

    APP_LOGI("ams_service_app_spawn_msg_wrapper_001 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnMsgWrapper
 * SubFunction: AssembleMsg
 * FunctionPoints: check message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function AssembleMsg can check the invalid gid.
 */
HWTEST(AmsServiceAppSpawnMsgWrapperTest, AppSpawnMsgWrapper_002, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_msg_wrapper_002 start");

    std::unique_ptr<AppSpawnMsgWrapper> appSpawnMsgWrapper = std::make_unique<AppSpawnMsgWrapper>();

    AppSpawnStartMsg params = {10000, 10000, {10001, 10002}, "processName", "soPath"};
    EXPECT_EQ(true, appSpawnMsgWrapper->AssembleMsg(params));

    APP_LOGI("ams_service_app_spawn_msg_wrapper_002 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnMsgWrapper
 * SubFunction: AssembleMsg
 * FunctionPoints: check message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function AssembleMsg can check gid count which exceeds the max limit.
 */
HWTEST(AmsServiceAppSpawnMsgWrapperTest, AppSpawnMsgWrapper_003, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_msg_wrapper_003 start");

    std::unique_ptr<AppSpawnMsgWrapper> appSpawnMsgWrapper = std::make_unique<AppSpawnMsgWrapper>();
    // gids limit, max 64
    AppSpawnStartMsg params = {10000,
        10000,
        {10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001,
            10001},
        "processName",
        "soPath"};
    EXPECT_EQ(false, appSpawnMsgWrapper->AssembleMsg(params));

    APP_LOGI("ams_service_app_spawn_msg_wrapper_003 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnMsgWrapper
 * SubFunction: AssembleMsg
 * FunctionPoints: check message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function AssembleMsg can check the process name is empty.
 */
HWTEST(AmsServiceAppSpawnMsgWrapperTest, AppSpawnMsgWrapper_004, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_msg_wrapper_004 start");

    std::unique_ptr<AppSpawnMsgWrapper> appSpawnMsgWrapper = std::make_unique<AppSpawnMsgWrapper>();
    AppSpawnStartMsg params = {10000, 10000, {10001, 10001}, "", "soPath"};
    EXPECT_EQ(false, appSpawnMsgWrapper->AssembleMsg(params));

    APP_LOGI("ams_service_app_spawn_msg_wrapper_004 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnMsgWrapper
 * SubFunction: AssembleMsg
 * FunctionPoints: check message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function AssembleMsg can check the process name exceeds the max length.
 */
HWTEST(AmsServiceAppSpawnMsgWrapperTest, AppSpawnMsgWrapper_005, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_msg_wrapper_005 start");

    std::unique_ptr<AppSpawnMsgWrapper> appSpawnMsgWrapper = std::make_unique<AppSpawnMsgWrapper>();
    std::string invalid =
        "InvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalid"
        "InvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInval"
        "id"
        "InvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInval"
        "id";
    AppSpawnStartMsg params = {10000, 10000, {10001, 10001}, invalid, "soPath"};
    EXPECT_EQ(false, appSpawnMsgWrapper->AssembleMsg(params));

    APP_LOGI("ams_service_app_spawn_msg_wrapper_005 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnMsgWrapper
 * SubFunction: AssembleMsg
 * FunctionPoints: check message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function AssembleMsg can check the so path is empty.
 */

HWTEST(AmsServiceAppSpawnMsgWrapperTest, AppSpawnMsgWrapper_006, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_msg_wrapper_006 start");

    std::unique_ptr<AppSpawnMsgWrapper> appSpawnMsgWrapper = std::make_unique<AppSpawnMsgWrapper>();
    AppSpawnStartMsg params = {10000, 10000, {10001, 10001}, "", ""};
    EXPECT_EQ(false, appSpawnMsgWrapper->AssembleMsg(params));

    APP_LOGI("ams_service_app_spawn_msg_wrapper_006 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnMsgWrapper
 * SubFunction: AssembleMsg
 * FunctionPoints: check message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function AssembleMsg can check the so path exceeds the max length.
 */
HWTEST(AmsServiceAppSpawnMsgWrapperTest, AppSpawnMsgWrapper_007, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_msg_wrapper_007 start");

    std::unique_ptr<AppSpawnMsgWrapper> appSpawnMsgWrapper = std::make_unique<AppSpawnMsgWrapper>();
    std::string invalid =
        "InvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalid"
        "InvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInval"
        "id"
        "InvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInvalidInval"
        "id";
    AppSpawnStartMsg params = {10000, 10000, {10001, 10001}, "processName", invalid};
    EXPECT_EQ(false, appSpawnMsgWrapper->AssembleMsg(params));

    APP_LOGI("ams_service_app_spawn_msg_wrapper_007 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnMsgWrapper
 * SubFunction: AssembleMsg
 * FunctionPoints: check message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function AssembleMsg check the valid message.
 */
HWTEST(AmsServiceAppSpawnMsgWrapperTest, AppSpawnMsgWrapper_008, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_msg_wrapper_008 start");

    std::unique_ptr<AppSpawnMsgWrapper> appSpawnMsgWrapper = std::make_unique<AppSpawnMsgWrapper>();
    AppSpawnStartMsg params = {10000, 10000, {10001, 10001}, "processName", "soPath"};
    EXPECT_EQ(true, appSpawnMsgWrapper->AssembleMsg(params));

    APP_LOGI("ams_service_app_spawn_msg_wrapper_008 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnMsgWrapper
 * SubFunction: IsValid & GetMsgLength
 * FunctionPoints: check message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if check the message pass, the length and valid value is right.
 */
HWTEST(AmsServiceAppSpawnMsgWrapperTest, AppSpawnMsgWrapper_009, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_msg_wrapper_009 start");

    std::unique_ptr<AppSpawnMsgWrapper> appSpawnMsgWrapper = std::make_unique<AppSpawnMsgWrapper>();
    AppSpawnStartMsg params = {10000, 10000, {10001, 10001}, "processName", "soPath"};
    EXPECT_EQ(true, appSpawnMsgWrapper->AssembleMsg(params));
    EXPECT_EQ(true, appSpawnMsgWrapper->IsValid());
    int32_t lhs = sizeof(AppSpawnMsg);
    EXPECT_EQ(lhs, appSpawnMsgWrapper->GetMsgLength());

    APP_LOGI("ams_service_app_spawn_msg_wrapper_009 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnMsgWrapper
 * SubFunction: IsValid & GetMsgLength
 * FunctionPoints: check message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if check the message fail, the length and valid value is right.
 */
HWTEST(AmsServiceAppSpawnMsgWrapperTest, AppSpawnMsgWrapper_010, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_msg_wrapper_010 start");

    std::unique_ptr<AppSpawnMsgWrapper> appSpawnMsgWrapper = std::make_unique<AppSpawnMsgWrapper>();
    AppSpawnStartMsg params = {10000, 10000, {10001, 10001}, "", "soPath"};
    EXPECT_EQ(false, appSpawnMsgWrapper->AssembleMsg(params));
    EXPECT_EQ(false, appSpawnMsgWrapper->IsValid());
    int32_t lhs = 0;
    EXPECT_EQ(lhs, appSpawnMsgWrapper->GetMsgLength());

    APP_LOGI("ams_service_app_spawn_msg_wrapper_010 end");
}