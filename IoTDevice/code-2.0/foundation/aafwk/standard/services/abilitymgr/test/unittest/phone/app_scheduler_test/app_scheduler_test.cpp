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

#define private public
#define protected public
#include "app_scheduler.h"
#undef private
#undef protected

#include "app_state_call_back_mock.h"
#include "ability_record.h"
#include "element_name.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
class AppSchedulerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static AbilityRequest GenerateAbilityRequest(const std::string &deviceName, const std::string &abilityName,
        const std::string &appName, const std::string &bundleName);

    std::shared_ptr<AppStateCallbackMock> appStateMock_ = std::make_shared<AppStateCallbackMock>();
};

void AppSchedulerTest::SetUpTestCase(void)
{}
void AppSchedulerTest::TearDownTestCase(void)
{}
void AppSchedulerTest::SetUp()
{}
void AppSchedulerTest::TearDown()
{}

AbilityRequest AppSchedulerTest::GenerateAbilityRequest(const std::string &deviceName, const std::string &abilityName,
    const std::string &appName, const std::string &bundleName)
{
    ElementName element(deviceName, abilityName, bundleName);
    Want want;
    want.SetElement(element);

    AbilityInfo abilityInfo;
    abilityInfo.applicationName = appName;
    ApplicationInfo appinfo;
    appinfo.name = appName;

    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = abilityInfo;
    abilityRequest.appInfo = appinfo;

    return abilityRequest;
}

/*
 * Feature: AppScheduler
 * Function: Init
 * SubFunction: NA
 * FunctionPoints: AppSchedulerTest Init
 * EnvConditions:NA
 * CaseDescription: Appstatecallback is nullptr causes init to fail
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_001, TestSize.Level0)
{
    std::shared_ptr<AppStateCallbackMock> appStateMock;
    EXPECT_EQ(false, DelayedSingleton<AppScheduler>::GetInstance()->Init(appStateMock));
}

/*
 * Feature: AppScheduler
 * Function: Init
 * SubFunction: NA
 * FunctionPoints: AppScheduler Init
 * EnvConditions:NA
 * CaseDescription: Verify init success
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_002, TestSize.Level0)
{
    EXPECT_EQ(true, DelayedSingleton<AppScheduler>::GetInstance()->Init(appStateMock_));
}

/*
 * Feature: AppScheduler
 * Function: LoadAbility
 * SubFunction: NA
 * FunctionPoints: AppScheduler LoadAbility
 * EnvConditions:NA
 * CaseDescription: Verify the normal process of loadability
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_003, TestSize.Level0)
{
    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.First";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();

    std::string preDeviceName = "device";
    std::string preAbilityName = "SecondAbility";
    std::string preAppName = "SecondApp";
    std::string preBundleName = "com.ix.Second";
    auto preAbilityReq = GenerateAbilityRequest(preDeviceName, preAbilityName, preAppName, preBundleName);
    auto preRecord = AbilityRecord::CreateAbilityRecord(preAbilityReq);
    auto pretoken = preRecord->GetToken();

    EXPECT_EQ((int)ERR_OK,
        DelayedSingleton<AppScheduler>::GetInstance()->LoadAbility(
            token, pretoken, record->GetAbilityInfo(), record->GetApplicationInfo()));
}

/*
 * Feature: AppScheduler
 * Function: LoadAbility
 * SubFunction: NA
 * FunctionPoints: AppScheduler LoadAbility
 * EnvConditions:NA
 * CaseDescription: Verify the fail process of loadability
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_004, TestSize.Level0)
{
    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.First";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();

    std::string preDeviceName = "device";
    std::string preAbilityName = "SecondAbility";
    std::string preAppName = "SecondApp";
    std::string preBundleName = "com.ix.Second";
    auto preAbilityReq = GenerateAbilityRequest(preDeviceName, preAbilityName, preAppName, preBundleName);
    auto preRecord = AbilityRecord::CreateAbilityRecord(preAbilityReq);
    auto pretoken = preRecord->GetToken();
    DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_ = nullptr;
    EXPECT_NE((int)ERR_OK,
        DelayedSingleton<AppScheduler>::GetInstance()->LoadAbility(
            token, pretoken, record->GetAbilityInfo(), record->GetApplicationInfo()));
}

/*
 * Feature: AppScheduler
 * Function: Init
 * SubFunction: NA
 * FunctionPoints: AppScheduler Init
 * EnvConditions:NA
 * CaseDescription: Verify appmgrclient_ Is nullptr causes init to fail
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_005, TestSize.Level0)
{
    EXPECT_EQ(false, DelayedSingleton<AppScheduler>::GetInstance()->Init(appStateMock_));
}

/*
 * Feature: AppScheduler
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: AppScheduler TerminateAbility
 * EnvConditions:NA
 * CaseDescription: Verify appmgrclient_ Is nullptr causes TerminateAbility to fail
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_006, TestSize.Level0)
{
    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.First";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();

    EXPECT_NE((int)ERR_OK, DelayedSingleton<AppScheduler>::GetInstance()->TerminateAbility(token));
}

/*
 * Feature: AppScheduler
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: AppScheduler TerminateAbility
 * EnvConditions:NA
 * CaseDescription: Verify appmgrclient_ Is not nullptr causes TerminateAbility to success
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_007, TestSize.Level0)
{
    DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_ = std::make_unique<AppExecFwk::AppMgrClient>();

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.First";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();

    EXPECT_EQ((int)ERR_OK, DelayedSingleton<AppScheduler>::GetInstance()->TerminateAbility(token));
}

/*
 * Feature: AppScheduler
 * Function: MoveToForground
 * SubFunction: NA
 * FunctionPoints: AppScheduler MoveToForground
 * EnvConditions:NA
 * CaseDescription: Verify appmgrclient_ Is null causes movetoforground to be invalid
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_008, TestSize.Level0)
{
    DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_ = nullptr;

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.First";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();

    DelayedSingleton<AppScheduler>::GetInstance()->MoveToForground(token);
}

/*
 * Feature: AppScheduler
 * Function: MoveToForground
 * SubFunction: NA
 * FunctionPoints: AppScheduler MoveToForground
 * EnvConditions:NA
 * CaseDescription: Verify the normal process of movetoforground
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_009, TestSize.Level0)
{
    DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_ = std::make_unique<AppExecFwk::AppMgrClient>();
    DelayedSingleton<AppScheduler>::GetInstance()->Init(appStateMock_);
    EXPECT_CALL(*appStateMock_, OnAbilityRequestDone(::testing::_, ::testing::_)).Times(1);

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.First";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();

    DelayedSingleton<AppScheduler>::GetInstance()->MoveToForground(token);
}

/*
 * Feature: AppScheduler
 * Function: MoveToBackground
 * SubFunction: NA
 * FunctionPoints: AppScheduler MoveToBackground
 * EnvConditions:NA
 * CaseDescription: Verify appmgrclient_ Is null causes OnAbilityRequestDone to be invalid
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_010, TestSize.Level0)
{
    DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_ = nullptr;

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.First";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();

    DelayedSingleton<AppScheduler>::GetInstance()->MoveToBackground(token);
}

/*
 * Feature: AppScheduler
 * Function: MoveToBackground GetAbilityState
 * SubFunction: NA
 * FunctionPoints: AppScheduler MoveToBackground and GetAbilityState
 * EnvConditions:NA
 * CaseDescription: Verify appmgrclient_ Is not nullptr causes onabilityrequestdone invoke
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_011, TestSize.Level0)
{
    DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_ = std::make_unique<AppExecFwk::AppMgrClient>();
    DelayedSingleton<AppScheduler>::GetInstance()->Init(appStateMock_);
    EXPECT_CALL(*appStateMock_, OnAbilityRequestDone(::testing::_, ::testing::_)).Times(1);

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.First";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();

    DelayedSingleton<AppScheduler>::GetInstance()->MoveToBackground(token);

    EXPECT_EQ(
        AppAbilityState::ABILITY_STATE_BACKGROUND, DelayedSingleton<AppScheduler>::GetInstance()->GetAbilityState());
}

/*
 * Feature: AppScheduler
 * Function: ConvertToAppAbilityState
 * SubFunction: NA
 * FunctionPoints: AppScheduler ConvertToAppAbilityState
 * EnvConditions:NA
 * CaseDescription: Verify ConvertToAppAbilityState result
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_012, TestSize.Level0)
{
    EXPECT_EQ(AppAbilityState::ABILITY_STATE_FOREGROUND,
        DelayedSingleton<AppScheduler>::GetInstance()->ConvertToAppAbilityState(
            static_cast<int>(AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND)));

    EXPECT_EQ(AppAbilityState::ABILITY_STATE_BACKGROUND,
        DelayedSingleton<AppScheduler>::GetInstance()->ConvertToAppAbilityState(
            static_cast<int>(AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND)));

    EXPECT_EQ(AppAbilityState::ABILITY_STATE_UNDEFINED,
        DelayedSingleton<AppScheduler>::GetInstance()->ConvertToAppAbilityState(
            static_cast<int>(AppExecFwk::AbilityState::ABILITY_STATE_BEGIN)));
}

/*
 * Feature: AppScheduler
 * Function: ConvertToAppAbilityState
 * SubFunction: NA
 * FunctionPoints: AppScheduler ConvertToAppAbilityState
 * EnvConditions:NA
 * CaseDescription: Verify ConvertToAppAbilityState result
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_013, TestSize.Level0)
{
    DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_ = nullptr;
    EXPECT_EQ(false, DelayedSingleton<AppScheduler>::GetInstance()->Init(appStateMock_));
}

/*
 * Feature: AppScheduler
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: AppScheduler AbilityBehaviorAnalysis
 * EnvConditions:NA
 * CaseDescription: Verify appmgrclient_ Is not nullptr causes AbilityBehaviorAnalysis to success
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_014, TestSize.Level0)
{
    DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_ = std::make_unique<AppExecFwk::AppMgrClient>();

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.First";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();
    const int32_t visibility = 1;
    const int32_t perceptibility = 1;
    const int32_t connectionState = 1;

    DelayedSingleton<AppScheduler>::GetInstance()->AbilityBehaviorAnalysis(
        token, nullptr, visibility, perceptibility, connectionState);

    auto pretoken = record->GetToken();
    DelayedSingleton<AppScheduler>::GetInstance()->AbilityBehaviorAnalysis(
        token, pretoken, visibility, perceptibility, connectionState);

    const int32_t visibility_1 = 0;
    DelayedSingleton<AppScheduler>::GetInstance()->AbilityBehaviorAnalysis(
        token, token, visibility_1, perceptibility, connectionState);

    const int32_t perceptibility_1 = 0;
    DelayedSingleton<AppScheduler>::GetInstance()->AbilityBehaviorAnalysis(
        token, token, visibility_1, perceptibility_1, connectionState);

    const int32_t connectionState_1 = 0;
    DelayedSingleton<AppScheduler>::GetInstance()->AbilityBehaviorAnalysis(
        token, token, visibility_1, perceptibility_1, connectionState_1);
}

/*
 * Feature: AppScheduler
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: AppScheduler AbilityBehaviorAnalysis
 * EnvConditions:NA
 * CaseDescription: Verify appmgrclient_ Is nullptr causes AbilityBehaviorAnalysis to fail
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_015, TestSize.Level0)
{
    DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_ = nullptr;

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.First";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();
    const int32_t visibility = 0;
    const int32_t perceptibility = 1;
    const int32_t connectionState = 1;

    DelayedSingleton<AppScheduler>::GetInstance()->AbilityBehaviorAnalysis(
        token, nullptr, visibility, perceptibility, connectionState);
}

/*
 * Feature: AppScheduler
 * Function: KillProcessByAbilityToken
 * SubFunction: NA
 * FunctionPoints: AppScheduler KillProcessByAbilityToken
 * EnvConditions:NA
 * CaseDescription: Verify appmgrclient_ Is not nullptr causes KillProcessByAbilityToken to success
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_016, TestSize.Level0)
{
    DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_ = std::make_unique<AppExecFwk::AppMgrClient>();

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.First";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();

    DelayedSingleton<AppScheduler>::GetInstance()->KillProcessByAbilityToken(token);
}

/*
 * Feature: AppScheduler
 * Function: KillProcessByAbilityToken
 * SubFunction: NA
 * FunctionPoints: AppScheduler KillProcessByAbilityToken
 * EnvConditions:NA
 * CaseDescription: Verify appmgrclient_ Is nullptr causes KillProcessByAbilityToken to fail
 */
HWTEST_F(AppSchedulerTest, AppScheduler_oprator_017, TestSize.Level0)
{
    DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_ = nullptr;

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.First";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();

    DelayedSingleton<AppScheduler>::GetInstance()->KillProcessByAbilityToken(token);
}
}  // namespace AAFwk
}  // namespace OHOS