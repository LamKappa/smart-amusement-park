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
#include "app_log_wrapper.h"
#include "app_record_id.h"
#include "app_scheduler_host.h"
#include "ability_info.h"
#include "application_info.h"
#include "app_running_record.h"
#include "mock_ability_token.h"

using namespace testing::ext;
namespace OHOS {
namespace AppExecFwk {
namespace {

const std::string APP_RECORD_NAME = "App_Name_Z";
const std::string ABILITY_RECORD_NAME = "Ability_Name_Z";

// schedule phase
const int NONE_SCHEDULED = 0;
const int FOREGROUND_SCHEDULED = 1 << 0;
const int BACKGROUND_SCHEDULED = 1 << 1;
const int TERMINATE_SCHEDULED = 1 << 2;
const int SHRINK_MEMORY_SCHEDULED = 1 << 3;
const int LOW_MEMORY_SCHEDULED = 1 << 4;
const int LAUNCH_APPLICATION_SCHEDULED = 1 << 5;
const int LAUNCH_ABILITY_SCHEDULED = 1 << 6;
const int CLEAN_ABILITY_SCHEDULED = 1 << 7;
const int PROFILE_CHANGED_SCHEDULED = 1 << 8;
const int SCHEDULE_CONFIGURATION_UPDATED = 1 << 9;
const int ABILITY_RUNNING_RECORD_NUM = 1000;

}  // namespace

class MockedSchedulerBase {
public:
    MockedSchedulerBase()
    {
        Reset();
    }

    virtual ~MockedSchedulerBase()
    {}

    virtual void Reset()
    {
        scheduled_ = NONE_SCHEDULED;
    }

    bool IsScheduled(const int scheduledPhase) const
    {
        return (scheduled_ & scheduledPhase) > 0;
    }

protected:
    unsigned int scheduled_ = NONE_SCHEDULED;
};

class MockedApplication : public AppSchedulerHost, public MockedSchedulerBase {
public:
    void ScheduleForegroundApplication() override
    {
        scheduled_ |= FOREGROUND_SCHEDULED;
    }
    void ScheduleBackgroundApplication() override
    {
        scheduled_ |= BACKGROUND_SCHEDULED;
    }
    void ScheduleTerminateApplication() override
    {
        scheduled_ |= TERMINATE_SCHEDULED;
    }
    void ScheduleShrinkMemory(const int) override
    {
        scheduled_ |= SHRINK_MEMORY_SCHEDULED;
    }
    void ScheduleLowMemory() override
    {
        scheduled_ |= LOW_MEMORY_SCHEDULED;
    }
    void ScheduleLaunchApplication(const AppLaunchData &) override
    {
        scheduled_ |= LAUNCH_APPLICATION_SCHEDULED;
        appLaunchTime++;
    }
    void ScheduleLaunchAbility(const AbilityInfo &, const sptr<IRemoteObject> &) override
    {
        scheduled_ |= LAUNCH_ABILITY_SCHEDULED;
        abilityLaunchTime++;
    }
    void ScheduleCleanAbility(const sptr<IRemoteObject> &) override
    {
        scheduled_ |= CLEAN_ABILITY_SCHEDULED;
    }
    void ScheduleProfileChanged(const Profile &) override
    {
        scheduled_ |= PROFILE_CHANGED_SCHEDULED;
    }
    void ScheduleConfigurationUpdated(const Configuration &config)
    {
        scheduled_ |= SCHEDULE_CONFIGURATION_UPDATED;
    }
    void ScheduleProcessSecurityExit()
    {}
    int GetAppLaunchTime() const
    {
        return appLaunchTime;
    }
    int GetAbilityLaunchTime() const
    {
        return abilityLaunchTime;
    }
    void Reset() override
    {
        MockedSchedulerBase::Reset();
        abilityLaunchTime = 0;
        appLaunchTime = 0;
    }

private:
    int abilityLaunchTime = 0;
    int appLaunchTime = 0;
};

class AmsAbilityRunningRecordModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<AppRunningRecord> QueryAppRunningRecord();
    sptr<IAppScheduler> QueryMockedAppSchedulerClient();

protected:
    sptr<MockedApplication> mockedAppClient_;
    sptr<IAppScheduler> client_;
    std::shared_ptr<AppRunningRecord> caseAppRunningRecord_;
};

void AmsAbilityRunningRecordModuleTest::SetUpTestCase()
{}

void AmsAbilityRunningRecordModuleTest::TearDownTestCase()
{}

void AmsAbilityRunningRecordModuleTest::SetUp()
{
    mockedAppClient_ = new (std::nothrow) MockedApplication();
}

void AmsAbilityRunningRecordModuleTest::TearDown()
{
    caseAppRunningRecord_.reset();
}

sptr<IAppScheduler> AmsAbilityRunningRecordModuleTest::QueryMockedAppSchedulerClient()
{
    if (!client_) {
        client_ = iface_cast<IAppScheduler>(mockedAppClient_.GetRefPtr());
    }
    return client_;
}

std::shared_ptr<AppRunningRecord> AmsAbilityRunningRecordModuleTest::QueryAppRunningRecord()
{
    if (caseAppRunningRecord_ == nullptr) {
        auto appInfo = std::make_shared<ApplicationInfo>();
        appInfo->name = APP_RECORD_NAME;
        appInfo->bundleName = APP_RECORD_NAME;  // specify process condition
        caseAppRunningRecord_.reset(
            new (std::nothrow) AppRunningRecord(appInfo, AppRecordId::Create(), appInfo->bundleName));
        caseAppRunningRecord_->SetApplicationClient(QueryMockedAppSchedulerClient());
    }
    return caseAppRunningRecord_;
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Create AbilityRunningRecord succeed
 * EnvConditions: NA
 * CaseDescription: Verify the function creating two same names of AbilityRunningRecord.
 */
HWTEST_F(AmsAbilityRunningRecordModuleTest, AddAbilityRunningRecord_001, TestSize.Level0)
{
    APP_LOGI("AddAbilityRunningRecord_001 start");
    auto appRunningRecord = QueryAppRunningRecord();
    auto caseAbilityInfo = std::make_shared<AbilityInfo>();
    caseAbilityInfo->name = ABILITY_RECORD_NAME;
    sptr<IRemoteObject> token = new MockAbilityToken();
    auto caseAbilityRunningRecord = appRunningRecord->AddAbility(token, caseAbilityInfo);

    EXPECT_TRUE(caseAbilityRunningRecord != nullptr);
    EXPECT_EQ(caseAbilityRunningRecord, appRunningRecord->GetAbilityRunningRecordByToken(token));
    auto abilityRunningRecordWithSameName = appRunningRecord->AddAbility(token, caseAbilityInfo);
    appRunningRecord->ClearAbility(caseAbilityRunningRecord);
    EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token) == nullptr);
    APP_LOGI("AddAbilityRunningRecord_001 end");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Create AbilityRunningRecord succeed
 * EnvConditions: NA
 * CaseDescription: Verify the function creating more AbilityRunningRecords.
 */
HWTEST_F(AmsAbilityRunningRecordModuleTest, AddAbilityRunningRecord_002, TestSize.Level0)
{
    APP_LOGI("AddAbilityRunningRecord_002 start");
    int i;
    auto appRunningRecord = QueryAppRunningRecord();

    for (i = 0; i < ABILITY_RUNNING_RECORD_NUM; i++) {
        auto caseAbilityInfo = std::make_shared<AbilityInfo>();
        caseAbilityInfo->name = ABILITY_RECORD_NAME + "_" + std::to_string(i);
        sptr<IRemoteObject> token = new MockAbilityToken();
        auto caseAbilityRunningRecord = appRunningRecord->AddAbility(token, caseAbilityInfo);
        EXPECT_TRUE(caseAbilityRunningRecord != nullptr);
        EXPECT_EQ(caseAbilityRunningRecord, appRunningRecord->GetAbilityRunningRecordByToken(token));
        EXPECT_EQ(caseAbilityRunningRecord->GetState(), AbilityState::ABILITY_STATE_BEGIN);
    }
    APP_LOGI("AddAbilityRunningRecord_002 end");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update AbilityRunningRecord succeed
 * EnvConditions: NA
 * CaseDescription: Verify the function updating more AbilityRunningRecords.
 */
HWTEST_F(AmsAbilityRunningRecordModuleTest, UpdateAbilityRunningRecord_001, TestSize.Level1)
{
    APP_LOGI("UpdateAbilityRunningRecord_001 start");
    int i;
    auto appRunningRecord = QueryAppRunningRecord();

    for (i = 0; i < ABILITY_RUNNING_RECORD_NUM; i++) {
        auto caseAbilityInfo = std::make_shared<AbilityInfo>();
        caseAbilityInfo->name = ABILITY_RECORD_NAME + "_" + std::to_string(i);
        sptr<IRemoteObject> token = new MockAbilityToken();
        auto caseAbilityRunningRecord = appRunningRecord->AddAbility(token, caseAbilityInfo);
        EXPECT_TRUE(caseAbilityRunningRecord != nullptr);
        EXPECT_EQ(caseAbilityRunningRecord, appRunningRecord->GetAbilityRunningRecordByToken(token));
        caseAbilityRunningRecord->SetState(AbilityState::ABILITY_STATE_BACKGROUND);
        appRunningRecord->SetState(ApplicationState::APP_STATE_FOREGROUND);
        appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_FOREGROUND);
        EXPECT_EQ(caseAbilityRunningRecord->GetState(), AbilityState::ABILITY_STATE_FOREGROUND);
        appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_BACKGROUND);
        EXPECT_EQ(caseAbilityRunningRecord->GetState(), AbilityState::ABILITY_STATE_BACKGROUND);
    }
    APP_LOGI("UpdateAbilityRunningRecord_001 end");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update AbilityRunningRecord succeed
 * EnvConditions: NA
 * CaseDescription: Verify the function updating an illegal state of AbilityRunningRecord.
 */
HWTEST_F(AmsAbilityRunningRecordModuleTest, UpdateAbilityRunningRecord_002, TestSize.Level0)
{
    APP_LOGI("UpdateAbilityRunningRecord_002 start");
    int i;
    auto appRunningRecord = QueryAppRunningRecord();

    for (i = 0; i < ABILITY_RUNNING_RECORD_NUM; i++) {
        auto caseAbilityInfo = std::make_shared<AbilityInfo>();
        caseAbilityInfo->name = ABILITY_RECORD_NAME + "_" + std::to_string(i);
        sptr<IRemoteObject> token = new MockAbilityToken();
        auto caseAbilityRunningRecord = appRunningRecord->AddAbility(token, caseAbilityInfo);
        EXPECT_TRUE(caseAbilityRunningRecord != nullptr);
        EXPECT_EQ(caseAbilityRunningRecord, appRunningRecord->GetAbilityRunningRecordByToken(token));
        appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_END);
        EXPECT_EQ(caseAbilityRunningRecord->GetState(), AbilityState::ABILITY_STATE_BEGIN);
    }
    APP_LOGI("UpdateAbilityRunningRecord_002 end");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Update AbilityRunningRecord succeed
 * EnvConditions: NA
 * CaseDescription: Verify the function updating more AbilityRunningRecords.
 */
HWTEST_F(AmsAbilityRunningRecordModuleTest, UpdateAbilityRunningRecord_003, TestSize.Level1)
{
    APP_LOGI("UpdateAbilityRunningRecord_003 start");
    int i;
    auto appRunningRecord = QueryAppRunningRecord();

    for (i = 0; i < ABILITY_RUNNING_RECORD_NUM; i++) {
        auto caseAbilityInfo = std::make_shared<AbilityInfo>();
        caseAbilityInfo->name = ABILITY_RECORD_NAME + "_" + std::to_string(i);
        sptr<IRemoteObject> token = new MockAbilityToken();
        auto caseAbilityRunningRecord = appRunningRecord->AddAbility(token, caseAbilityInfo);
        EXPECT_TRUE(caseAbilityRunningRecord != nullptr);
        EXPECT_EQ(caseAbilityRunningRecord, appRunningRecord->GetAbilityRunningRecordByToken(token));
        caseAbilityRunningRecord->SetState(AbilityState::ABILITY_STATE_FOREGROUND);
        appRunningRecord->SetState(ApplicationState::APP_STATE_BACKGROUND);
        appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_BACKGROUND);
        EXPECT_EQ(caseAbilityRunningRecord->GetState(), AbilityState::ABILITY_STATE_BACKGROUND);
        appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_READY);
        EXPECT_EQ(caseAbilityRunningRecord->GetState(), AbilityState::ABILITY_STATE_BACKGROUND);
    }
    APP_LOGI("UpdateAbilityRunningRecord_003 end");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Clear AbilityRunningRecord succeed
 * EnvConditions: NA
 * CaseDescription: Verify the function clearing more AbilityRunningRecords.
 */
HWTEST_F(AmsAbilityRunningRecordModuleTest, ClearAbilityRunningRecord_001, TestSize.Level1)
{
    APP_LOGI("ClearAbilityRunningRecord_001 start");
    int i;
    auto appRunningRecord = QueryAppRunningRecord();

    for (i = 0; i < ABILITY_RUNNING_RECORD_NUM; i++) {
        auto caseAbilityInfo = std::make_shared<AbilityInfo>();
        caseAbilityInfo->name = ABILITY_RECORD_NAME + "_" + std::to_string(i);
        sptr<IRemoteObject> token = new MockAbilityToken();
        auto caseAbilityRunningRecord = appRunningRecord->AddAbility(token, caseAbilityInfo);
        EXPECT_TRUE(caseAbilityRunningRecord != nullptr);
        EXPECT_EQ(caseAbilityRunningRecord, appRunningRecord->GetAbilityRunningRecordByToken(token));
    }
    for (i = 0; i < ABILITY_RUNNING_RECORD_NUM; i++) {
        auto caseAbilityInfo = std::make_shared<AbilityInfo>();
        caseAbilityInfo->name = ABILITY_RECORD_NAME + "_" + std::to_string(i);
        sptr<IRemoteObject> token = new MockAbilityToken();
        auto caseAbilityRunningRecord = appRunningRecord->GetAbilityRunningRecordByToken(token);
        appRunningRecord->ClearAbility(caseAbilityRunningRecord);
        EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token) == nullptr);
    }
    APP_LOGI("ClearAbilityRunningRecord_001 end");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Clear AbilityRunningRecord succeed
 * EnvConditions: NA
 * CaseDescription: Verify the function clearing all AbilityRunningRecords, the AbilityRunningRecord is null.
 */
HWTEST_F(AmsAbilityRunningRecordModuleTest, ClearAbilityRunningRecord_002, TestSize.Level1)
{
    APP_LOGI("ClearAbilityRunningRecord_002 start");
    int i;
    auto appRunningRecord = QueryAppRunningRecord();

    for (i = 0; i < ABILITY_RUNNING_RECORD_NUM; i++) {
        auto caseAbilityInfo = std::make_shared<AbilityInfo>();
        caseAbilityInfo->name = ABILITY_RECORD_NAME + "_" + std::to_string(i);
        sptr<IRemoteObject> token = new MockAbilityToken();
        auto caseAbilityRunningRecord = appRunningRecord->AddAbility(token, caseAbilityInfo);
        EXPECT_TRUE(caseAbilityRunningRecord != nullptr);
        EXPECT_EQ(caseAbilityRunningRecord, appRunningRecord->GetAbilityRunningRecordByToken(token));
        appRunningRecord->ClearAbility(caseAbilityRunningRecord);
        EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token) == nullptr);
    }
    if (i == ABILITY_RUNNING_RECORD_NUM) {
        auto abilityMap = appRunningRecord->GetAbilities();
        EXPECT_TRUE(abilityMap.empty());
    }
    APP_LOGI("ClearAbilityRunningRecord_002 end");
}

/*
 * Feature: AMS
 * Function: AbilityRunningRecord
 * SubFunction: NA
 * FunctionPoints: Add ,update and Clear AbilityRunningRecord succeed
 * EnvConditions: NA
 * CaseDescription: Verify the function Add ,update and clear more AbilityRunningRecords.
 */
HWTEST_F(AmsAbilityRunningRecordModuleTest, OperateAbilityRunningRecord_001, TestSize.Level1)
{
    APP_LOGI("OperateAbilityRunningRecord_001 start");
    int i;
    auto appRunningRecord = QueryAppRunningRecord();

    for (i = 0; i < ABILITY_RUNNING_RECORD_NUM; i++) {
        auto caseAbilityInfo = std::make_shared<AbilityInfo>();
        caseAbilityInfo->name = ABILITY_RECORD_NAME + "_" + std::to_string(i);
        sptr<IRemoteObject> token = new MockAbilityToken();
        auto caseAbilityRunningRecord = appRunningRecord->AddAbility(token, caseAbilityInfo);
        EXPECT_TRUE(caseAbilityRunningRecord != nullptr);
        EXPECT_EQ(caseAbilityRunningRecord, appRunningRecord->GetAbilityRunningRecordByToken(token));
    }
    for (i = 0; i < ABILITY_RUNNING_RECORD_NUM; i++) {
        auto caseAbilityInfo = std::make_shared<AbilityInfo>();
        caseAbilityInfo->name = ABILITY_RECORD_NAME + "_" + std::to_string(i);
        sptr<IRemoteObject> token = new MockAbilityToken();
        auto caseAbilityRunningRecord = appRunningRecord->GetAbilityRunningRecordByToken(token);
        appRunningRecord->ClearAbility(caseAbilityRunningRecord);
        EXPECT_TRUE(appRunningRecord->GetAbilityRunningRecordByToken(token) == nullptr);
    }
    APP_LOGI("OperateAbilityRunningRecord_001 end");
}

}  // namespace AppExecFwk
}  // namespace OHOS
