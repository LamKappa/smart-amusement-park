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

#include "ability_running_record.h"
#include <gtest/gtest.h>
#include "app_running_record.h"
#include "app_scheduler_host.h"
#include "app_log_wrapper.h"
#include "mock_ability_token.h"
#include "mock_application.h"

using namespace testing::ext;
using testing::_;
namespace OHOS {
namespace AppExecFwk {
class AmsAbilityRunningRecordTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    static const std::string GetTestAppName()
    {
        return "test_app_name";
    }
    static const std::string GetTestAbilityName()
    {
        return "test_ability_name";
    }
    static const std::string GetAnotherTestAbilityName()
    {
        return "another_test_ability_name";
    }
    static const std::string GetProcessName()
    {
        return "AmsAbilityRunningRecordTest";
    }

    std::shared_ptr<AppRunningRecord> GetTestAppRunningRecord();
    sptr<IAppScheduler> GetMockedAppSchedulerClient();

protected:
    sptr<IAppScheduler> client_;
    sptr<MockApplication> mockedAppClient_;
    std::shared_ptr<AppRunningRecord> testAppRecord_;
};

void AmsAbilityRunningRecordTest::SetUpTestCase()
{}

void AmsAbilityRunningRecordTest::TearDownTestCase()
{}

void AmsAbilityRunningRecordTest::SetUp()
{
    mockedAppClient_ = new MockApplication();
}

void AmsAbilityRunningRecordTest::TearDown()
{
    testAppRecord_.reset();
}

sptr<IAppScheduler> AmsAbilityRunningRecordTest::GetMockedAppSchedulerClient()
{
    if (!client_) {
        client_ = iface_cast<IAppScheduler>(mockedAppClient_.GetRefPtr());
    }
    return client_;
}

std::shared_ptr<AppRunningRecord> AmsAbilityRunningRecordTest::GetTestAppRunningRecord()
{
    if (!testAppRecord_) {
        auto appInfo = std::make_shared<ApplicationInfo>();
        appInfo->name = GetTestAppName();
        testAppRecord_ = std::make_shared<AppRunningRecord>(appInfo, AppRecordId::Create(), GetProcessName());
        testAppRecord_->SetApplicationClient(GetMockedAppSchedulerClient());
    }
    return testAppRecord_;
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Create AbilityRunningRecord using correct args.
 * EnvConditions: NA
 * CaseDescription: Verify the function AddAbility can create AbilityRunningRecord add add to AppRunningRecord.
 */
HWTEST_F(AmsAbilityRunningRecordTest, CreateAbilityRunningRecord_001, TestSize.Level0)
{
    APP_LOGD("CreateAbilityRunningRecord_001 start.");
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto appRunningRecord = GetTestAppRunningRecord();
    sptr<IRemoteObject> token = new MockAbilityToken();
    auto abilityRunningRecord = appRunningRecord->AddAbility(token, abilityInfo);

    EXPECT_TRUE(abilityRunningRecord != nullptr);
    EXPECT_EQ(abilityRunningRecord, appRunningRecord->GetAbilityRunningRecordByToken(token));
    APP_LOGD("CreateAbilityRunningRecord_001 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Create AbilityRunningRecord using null args.
 * EnvConditions: NA
 * CaseDescription: Verify the function AddAbility works but does not take effect using nullptr parameter.
 */
HWTEST_F(AmsAbilityRunningRecordTest, CreateAbilityRunningRecord_002, TestSize.Level0)
{
    APP_LOGD("CreateAbilityRunningRecord_002 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    sptr<IRemoteObject> token = new MockAbilityToken();
    auto abilityRunningRecord = appRunningRecord->AddAbility(token, nullptr);
    EXPECT_TRUE(abilityRunningRecord == nullptr);
    APP_LOGD("CreateAbilityRunningRecord_002 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Create AbilityRunningRecord that already exists.
 * EnvConditions: NA
 * CaseDescription: Verify the function AddAbility does not take effect when abilityRunningRecord already exists.
 */
HWTEST_F(AmsAbilityRunningRecordTest, CreateAbilityRunningRecord_003, TestSize.Level0)
{
    APP_LOGD("CreateAbilityRunningRecord_003 start.");
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto appRunningRecord = GetTestAppRunningRecord();
    sptr<IRemoteObject> token = new MockAbilityToken();

    auto abilityRunningRecordFirst = appRunningRecord->AddAbility(token, abilityInfo);
    EXPECT_TRUE(abilityRunningRecordFirst != nullptr);
    EXPECT_EQ(abilityRunningRecordFirst, appRunningRecord->GetAbilityRunningRecordByToken(token));

    auto abilityRunningRecordSecond = appRunningRecord->AddAbility(token, abilityInfo);
    EXPECT_TRUE(abilityRunningRecordSecond == nullptr);
    APP_LOGD("CreateAbilityRunningRecord_003 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update the state of AbilityRunningRecord using correct args.
 * EnvConditions: NA
 * CaseDescription: Verify the function UpdateAbilityState can update the state of AbilityRunningRecord correctly.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_001, TestSize.Level0)
{
    APP_LOGD("UpdateAbilityRunningRecord_001 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();

    auto abilityRunningRecord = appRunningRecord->AddAbility(token, abilityInfo);
    ASSERT_TRUE(abilityRunningRecord != nullptr);
    abilityRunningRecord->SetState(AbilityState::ABILITY_STATE_READY);
    appRunningRecord->SetState(ApplicationState::APP_STATE_READY);

    EXPECT_CALL(*mockedAppClient_, ScheduleForegroundApplication()).Times(1);
    appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_FOREGROUND);
    appRunningRecord->PopForegroundingAbilityTokens();
    EXPECT_EQ(abilityRunningRecord->GetState(), AbilityState::ABILITY_STATE_FOREGROUND) << "execute fail!";

    appRunningRecord->SetState(ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_CALL(*mockedAppClient_, ScheduleBackgroundApplication()).Times(1);
    appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ(abilityRunningRecord->GetState(), AbilityState::ABILITY_STATE_BACKGROUND) << "execute fail!";
    APP_LOGD("UpdateAbilityRunningRecord_001 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update the state of AbilityRunningRecord using incorrect args.
 * EnvConditions: NA
 * CaseDescription: Verify the function UpdateAbilityState works but does not take effect using incorrect value.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_002, TestSize.Level0)
{
    APP_LOGD("UpdateAbilityRunningRecord_002 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();

    auto abilityRunningRecord = appRunningRecord->AddAbility(token, abilityInfo);
    ASSERT_TRUE(abilityRunningRecord != nullptr);

    AbilityState state = abilityRunningRecord->GetState();
    appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_END);
    EXPECT_EQ(abilityRunningRecord->GetState(), state);
    EXPECT_NE(abilityRunningRecord->GetState(), AbilityState::ABILITY_STATE_END);
    APP_LOGD("UpdateAbilityRunningRecord_002 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update the state of AbilityRunningRecord using nullptr.
 * EnvConditions: NA
 * CaseDescription: Verify the function UpdateAbilityState works but does not take effect using nullptr parameter.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_003, TestSize.Level0)
{
    APP_LOGD("UpdateAbilityRunningRecord_003 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    auto abilityRunningRecord = appRunningRecord->AddAbility(token, abilityInfo);
    ASSERT_TRUE(abilityRunningRecord != nullptr);

    AbilityState state = abilityRunningRecord->GetState();

    appRunningRecord->UpdateAbilityState(nullptr, AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(abilityRunningRecord->GetState(), state);
    APP_LOGD("UpdateAbilityRunningRecord_003 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update the state of AbilityRunningRecord that does not exist.
 * EnvConditions: NA
 * CaseDescription: Verify the function UpdateAbilityState cannot change the state of AbilityRunningRecord
 *                  that does not exist.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_004, TestSize.Level0)
{
    APP_LOGD("UpdateAbilityRunningRecord_004 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();

    auto anotherAbilityInfo = std::make_shared<AbilityInfo>();
    anotherAbilityInfo->name = GetAnotherTestAbilityName();
    sptr<IRemoteObject> token2 = new MockAbilityToken();

    auto abilityRunningRecord = appRunningRecord->AddAbility(token, abilityInfo);
    ASSERT_TRUE(abilityRunningRecord != nullptr);

    AbilityState state = abilityRunningRecord->GetState();
    EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token2) == nullptr);
    appRunningRecord->UpdateAbilityState(token2, AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(abilityRunningRecord->GetState(), state);
    APP_LOGD("UpdateAbilityRunningRecord_004 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update one state of AbilityRunningRecords as foreground.
 * EnvConditions: NA
 * CaseDescription: Verify if there is at least one state of AbilityRunningRecords is foreground,
 *                  the state of application should be changed to foreground.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_005, TestSize.Level0)
{
    APP_LOGD("UpdateAbilityRunningRecord_005 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto anotherAbilityInfo = std::make_shared<AbilityInfo>();
    anotherAbilityInfo->name = GetAnotherTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    sptr<IRemoteObject> anotherToken = new MockAbilityToken();

    auto abilityRunningRecord = appRunningRecord->AddAbility(token, abilityInfo);
    auto anotherAbilityRunningRecord = appRunningRecord->AddAbility(anotherToken, anotherAbilityInfo);

    ASSERT_TRUE(abilityRunningRecord != nullptr);
    ASSERT_TRUE(anotherAbilityRunningRecord != nullptr);
    anotherAbilityRunningRecord->SetState(AbilityState::ABILITY_STATE_BACKGROUND);
    appRunningRecord->SetState(ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*mockedAppClient_, ScheduleForegroundApplication()).Times(1);
    appRunningRecord->UpdateAbilityState(anotherToken, AbilityState::ABILITY_STATE_FOREGROUND);
    APP_LOGD("UpdateAbilityRunningRecord_005 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update all states of AbilityRunningRecords as background.
 * EnvConditions: NA
 * CaseDescription: Verify if all states of AbilityRunningRecords are background, the state of application should be
 *                  changed to background.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_006, TestSize.Level0)
{
    APP_LOGD("UpdateAbilityRunningRecord_006 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto anotherAbilityInfo = std::make_shared<AbilityInfo>();
    anotherAbilityInfo->name = GetAnotherTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    sptr<IRemoteObject> anotherToken = new MockAbilityToken();
    auto abilityRunningRecord = appRunningRecord->AddAbility(token, abilityInfo);
    auto anotherAbilityRunningRecord = appRunningRecord->AddAbility(anotherToken, anotherAbilityInfo);
    EXPECT_TRUE(abilityRunningRecord != nullptr);
    EXPECT_TRUE(anotherAbilityRunningRecord != nullptr);
    anotherAbilityRunningRecord->SetState(AbilityState::ABILITY_STATE_FOREGROUND);
    appRunningRecord->SetState(ApplicationState::APP_STATE_FOREGROUND);
    abilityRunningRecord->SetState(AbilityState::ABILITY_STATE_FOREGROUND);

    EXPECT_CALL(*mockedAppClient_, ScheduleBackgroundApplication()).Times(1);
    appRunningRecord->UpdateAbilityState(anotherToken, AbilityState::ABILITY_STATE_FOREGROUND);

    auto abilities = appRunningRecord->GetAbilities();
    for (auto iter = abilities.begin(); iter != abilities.end(); iter++) {
        appRunningRecord->UpdateAbilityState(iter->second->GetToken(), AbilityState::ABILITY_STATE_BACKGROUND);
    }
    APP_LOGD("UpdateAbilityRunningRecord_006 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update all states of AbilityRunningRecords as terminate.
 * EnvConditions: NA
 * CaseDescription: Verify if all states of AbilityRunningRecords are terminate, the state of application should be
 *                  changed to terminate.
 */
HWTEST_F(AmsAbilityRunningRecordTest, UpdateAbilityRunningRecord_007, TestSize.Level0)
{
    APP_LOGD("UpdateAbilityRunningRecord_007 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto anotherAbilityInfo = std::make_shared<AbilityInfo>();
    anotherAbilityInfo->name = GetAnotherTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    sptr<IRemoteObject> anotherToken = new MockAbilityToken();
    auto abilityRunningRecord = appRunningRecord->AddAbility(token, abilityInfo);
    auto anotherAbilityRunningRecord = appRunningRecord->AddAbility(anotherToken, anotherAbilityInfo);
    EXPECT_TRUE(abilityRunningRecord != nullptr);
    ASSERT_TRUE(anotherAbilityRunningRecord != nullptr);
    anotherAbilityRunningRecord->SetState(AbilityState::ABILITY_STATE_BACKGROUND);
    appRunningRecord->SetState(ApplicationState::APP_STATE_BACKGROUND);
    abilityRunningRecord->SetState(AbilityState::ABILITY_STATE_BACKGROUND);

    EXPECT_CALL(*mockedAppClient_, ScheduleTerminateApplication()).Times(1);
    EXPECT_CALL(*mockedAppClient_, ScheduleCleanAbility(_)).Times(2);
    auto abilities = appRunningRecord->GetAbilities();
    for (auto iter = abilities.begin(); iter != abilities.end(); iter++) {
        appRunningRecord->TerminateAbility(iter->second->GetToken());
        appRunningRecord->AbilityTerminated(iter->second->GetToken());
    }
    APP_LOGD("UpdateAbilityRunningRecord_007 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Delete AbilityRunningRecord using correct args.
 * EnvConditions: NA
 * CaseDescription: Verify the function ClearAbility can delete AbilityRunningRecord.
 */
HWTEST_F(AmsAbilityRunningRecordTest, DeleteAbilityRunningRecord_001, TestSize.Level0)
{
    APP_LOGD("DeleteAbilityRunningRecord_001 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    auto abilityRunningRecord = appRunningRecord->AddAbility(token, abilityInfo);

    EXPECT_TRUE(abilityRunningRecord != nullptr);
    ASSERT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token) != nullptr);

    appRunningRecord->ClearAbility(abilityRunningRecord);
    EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token) == nullptr);
    APP_LOGD("DeleteAbilityRunningRecord_001 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Delete AbilityRunningRecord using null args.
 * EnvConditions: NA
 * CaseDescription: Verify the function ClearAbility works but does not take effect using nullptr parameter.
 */
HWTEST_F(AmsAbilityRunningRecordTest, DeleteAbilityRunningRecord_002, TestSize.Level0)
{
    APP_LOGD("DeleteAbilityRunningRecord_002 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    auto abilityRunnningRecord = appRunningRecord->AddAbility(token, abilityInfo);

    EXPECT_TRUE(abilityRunnningRecord != nullptr);
    ASSERT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token) != nullptr);

    appRunningRecord->ClearAbility(nullptr);
    EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token) != nullptr);
    APP_LOGD("DeleteAbilityRunningRecord_002 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Delete AbilityRunningRecord that does not exist.
 * EnvConditions: NA
 * CaseDescription: Verify the function ClearAbility cannot delete AbilityRunningRecord that does not exist.
 */
HWTEST_F(AmsAbilityRunningRecordTest, DeleteAbilityRunningRecord_003, TestSize.Level0)
{
    APP_LOGD("DeleteAbilityRunningRecord_003 start.");
    auto appRunningRecord = GetTestAppRunningRecord();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto anotherAbilityInfo = std::make_shared<AbilityInfo>();
    anotherAbilityInfo->name = GetAnotherTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    sptr<IRemoteObject> anotherToken = new MockAbilityToken();
    auto abilityRunningRecord = appRunningRecord->AddAbility(token, abilityInfo);
    auto anotherAbilityRunningRecord = appRunningRecord->AddAbility(anotherToken, anotherAbilityInfo);

    EXPECT_TRUE(abilityRunningRecord != nullptr);
    EXPECT_TRUE(anotherAbilityRunningRecord != nullptr);
    EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token) != nullptr);
    EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(anotherToken) != nullptr);

    appRunningRecord->ClearAbility(anotherAbilityRunningRecord);
    EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token) != nullptr);
    EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(anotherToken) == nullptr);

    appRunningRecord->ClearAbility(anotherAbilityRunningRecord);
    EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token) != nullptr);
    EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(anotherToken) == nullptr);
    APP_LOGD("DeleteAbilityRunningRecord_003 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: IsSameState
 * FunctionPoints: Check state is same or different.
 * EnvConditions: NA
 * CaseDescription: Verify the function IsSameState judge the exact state value.
 */
HWTEST_F(AmsAbilityRunningRecordTest, IsSameState_001, TestSize.Level0)
{
    APP_LOGD("IsSameState_001 start.");

    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    sptr<IRemoteObject> token = new MockAbilityToken();
    std::shared_ptr<AbilityRunningRecord> abilityRunningRecord =
        std::make_shared<AbilityRunningRecord>(abilityInfo, token);

    abilityRunningRecord->SetState(AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(false, abilityRunningRecord->IsSameState(AbilityState::ABILITY_STATE_BACKGROUND));
    EXPECT_EQ(true, abilityRunningRecord->IsSameState(AbilityState::ABILITY_STATE_FOREGROUND));

    APP_LOGD("IsSameState_001 end.");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Create AbilityRunningRecord using correct args.
 * EnvConditions: NA
 * CaseDescription: Verify the function AddAbility can create AbilityRunningRecord add add to AppRunningRecord.
 */
HWTEST_F(AmsAbilityRunningRecordTest, SetGetAbilityRecord_001, TestSize.Level0)
{
    APP_LOGD("SetGetAbilityRecord_001 start.");

    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    auto appRunningRecord = GetTestAppRunningRecord();
    sptr<IRemoteObject> token = new MockAbilityToken();
    auto abilityRunningRecord = appRunningRecord->AddAbility(token, abilityInfo);

    ASSERT_TRUE(abilityRunningRecord != nullptr);
    abilityRunningRecord->SetVisibility(1);
    abilityRunningRecord->SetPerceptibility(1);
    abilityRunningRecord->SetConnectionState(1);

    EXPECT_EQ(abilityRunningRecord, appRunningRecord->GetAbilityRunningRecordByToken(token));

    auto testRecord = appRunningRecord->GetAbilityRunningRecordByToken(token);
    EXPECT_EQ(1, testRecord->GetVisibility());
    EXPECT_EQ(1, testRecord->GetPerceptibility());
    EXPECT_EQ(1, testRecord->GetConnectionState());

    APP_LOGD("SetGetAbilityRecord_001 end.");
}

}  // namespace AppExecFwk
}  // namespace OHOS
