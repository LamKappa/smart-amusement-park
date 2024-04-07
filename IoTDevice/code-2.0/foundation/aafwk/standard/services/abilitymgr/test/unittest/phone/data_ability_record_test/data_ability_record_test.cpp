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
#include "data_ability_record.h"
#undef private
#undef protected

#include "ability_scheduler_mock.h"

using namespace testing::ext;
using namespace testing;
using namespace std::chrono;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
class DataAbilityRecordTest : public testing::TestWithParam<OHOS::AAFwk::AbilityState> {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

protected:
    sptr<AbilitySchedulerMock> abilitySchedulerMock_;
    AbilityRequest abilityRequest_;
    std::shared_ptr<AbilityRecord> abilityRecord_;
    OHOS::AAFwk::AbilityState abilityState_;
};

void DataAbilityRecordTest::SetUpTestCase(void)
{}
void DataAbilityRecordTest::TearDownTestCase(void)
{}

void DataAbilityRecordTest::SetUp(void)
{
    if (abilitySchedulerMock_ == nullptr) {
        abilitySchedulerMock_ = new AbilitySchedulerMock();
    }

    abilityRequest_.appInfo.bundleName = "com.data_ability.hiworld";
    abilityRequest_.appInfo.name = "com.data_ability.hiworld";
    abilityRequest_.abilityInfo.name = "DataAbilityHiworld";
    abilityRequest_.abilityInfo.type = AbilityType::DATA;

    if (abilityRecord_ == nullptr) {
        OHOS::AppExecFwk::AbilityInfo abilityInfo;
        OHOS::AppExecFwk::ApplicationInfo applicationInfo;
        const Want want;
        abilityRecord_ = std::make_shared<AbilityRecord>(want, abilityInfo, applicationInfo);
        abilityRecord_->Init();
    }
    abilityState_ = INITIAL;
}

void DataAbilityRecordTest::TearDown(void)
{
    abilitySchedulerMock_.clear();
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: NA
 * FunctionPoints: Normal flow
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify normal flow.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_Flow_001, TestSize.Level1)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_Flow_001 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    abilityState_ = ACTIVE;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_OK);
    EXPECT_NE(dataAbilityRecord->GetToken(), nullptr);
    EXPECT_NE(dataAbilityRecord->GetAbilityRecord()->GetToken(), nullptr);
    EXPECT_EQ(dataAbilityRecord->GetRequest().appInfo.name, abilityRequest_.appInfo.name);

    HILOG_INFO("AaFwk_DataAbilityRecord_Flow_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: StartLoading
 * FunctionPoints: The parameter of function Startloading.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function StartLoading called twice.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_StartLoading_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_StartLoading_001 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_ALREADY_EXISTS);

    HILOG_INFO("AaFwk_DataAbilityRecord_StartLoading_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: StartLoading
 * FunctionPoints: The parameter of function Startloading.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function StartLoading request parameter without app name
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_StartLoading_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_StartLoading_002 start.");

    // clear app name
    abilityRequest_.appInfo.name = "";
    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_INVALID_VALUE);

    HILOG_INFO("AaFwk_DataAbilityRecord_StartLoading_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: WaitForLoaded
 * FunctionPoints: The parameter of function WaitForLoaded.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function WaitForLoaded, before call WaitForLoaded not startloading.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_WaitForLoaded_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_WaitForLoaded_001 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    std::mutex mutex;
    system_clock::duration timeout = 800ms;

    EXPECT_EQ(dataAbilityRecord->WaitForLoaded(mutex, timeout), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_WaitForLoaded_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: WaitForLoaded
 * FunctionPoints: The parameter of function WaitForLoaded.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function WaitForLoaded wait state timeout.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_WaitForLoaded_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_WaitForLoaded_002 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    std::mutex mutex;
    system_clock::duration timeout = 800ms;

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->WaitForLoaded(mutex, timeout), ERR_TIMED_OUT);

    HILOG_INFO("AaFwk_DataAbilityRecord_WaitForLoaded_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: WaitForLoaded
 * FunctionPoints: The parameter of function WaitForLoaded.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function WaitForLoaded ability has loaded.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_WaitForLoaded_003, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_WaitForLoaded_003 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    std::mutex mutex;
    system_clock::duration timeout = 800ms;

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    abilityState_ = ACTIVE;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->WaitForLoaded(mutex, timeout), ERR_OK);

    HILOG_INFO("AaFwk_DataAbilityRecord_WaitForLoaded_003 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Attach
 * FunctionPoints: The parameter of function Attach.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function Attach request parameter without call StartLoading.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_Attach_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_Attach_001 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_Attach_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Attach
 * FunctionPoints: The parameter of function Attach.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function Attach request parameter with nullptr
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_Attach_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_Attach_002 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->Attach(nullptr), ERR_INVALID_DATA);

    HILOG_INFO("AaFwk_DataAbilityRecord_Attach_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Attach
 * FunctionPoints: The parameter of function Attach.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function Attach called twice
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_Attach_003, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_Attach_003 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);

    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_Attach_003 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: OnTransitionDone
 * FunctionPoints: The parameter of function OnTransitionDone.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function OnTransitionDone without call StartLoading and Attach.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_OnTransitionDone_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_OnTransitionDone_001 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_OnTransitionDone_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: OnTransitionDone
 * FunctionPoints: The parameter of function OnTransitionDone.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function OnTransitionDone request parameter without call Attach.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_OnTransitionDone_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_OnTransitionDone_002 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_OnTransitionDone_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: OnTransitionDone
 * FunctionPoints: The parameter of function OnTransitionDone.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function OnTransitionDone request parameter state
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_OnTransitionDone_003, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_OnTransitionDone_003 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);

    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);

    abilityState_ = INITIAL;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_INVALID_STATE);

    abilityState_ = ACTIVE;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_INVALID_STATE);

    abilityState_ = BACKGROUND;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_INVALID_STATE);

    abilityState_ = SUSPENDED;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_INVALID_STATE);

    abilityState_ = INACTIVATING;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_INVALID_STATE);

    abilityState_ = ACTIVATING;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_INVALID_STATE);

    abilityState_ = MOVING_BACKGROUND;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_INVALID_STATE);

    abilityState_ = TERMINATING;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_INVALID_STATE);

    abilityState_ = INACTIVE;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_OnTransitionDone_003 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: OnTransitionDone
 * FunctionPoints: The parameter of function OnTransitionDone.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function OnTransitionDone ability state changed to inactive, and call OnTransitionDone.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_OnTransitionDone_004, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_OnTransitionDone_004 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);

    abilityState_ = ACTIVE;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_OnTransitionDone_004 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: AddClient
 * FunctionPoints: The parameter of function AddClient.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function OnTransitionDone with nullptr.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_AddClient_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_AddClient_001 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    EXPECT_EQ(dataAbilityRecord->AddClient(nullptr, true), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_AddClient_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: AddClient
 * FunctionPoints: The parameter of function AddClient.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function AddClient, before call AddClient not startloading and not attach.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_AddClient_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_AddClient_002 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->AddClient(client, true), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_AddClient_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: AddClient
 * FunctionPoints: The parameter of function AddClient.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function AddClient, before call AddClient not attach.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_AddClient_003, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_AddClient_003 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->AddClient(client, true), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_AddClient_003 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: AddClient
 * FunctionPoints: The parameter of function AddClient.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function AddClient ability state is not inactive.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_AddClient_004, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_AddClient_004 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->AddClient(client, true), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_AddClient_004 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: AddClient
 * FunctionPoints: The parameter of function AddClient.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function AddClient has add one client, and get count exact.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_AddClient_005, TestSize.Level1)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_AddClient_005 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    abilityState_ = ACTIVE;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->AddClient(client, true), ERR_OK);
    unsigned int count = 1;
    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), count);

    HILOG_INFO("AaFwk_DataAbilityRecord_AddClient_005 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: RemoveClient
 * FunctionPoints: The parameter of function RemoveClient.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function RemoveClient with nullptr.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_RemoveClient_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_001 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    EXPECT_EQ(dataAbilityRecord->RemoveClient(nullptr), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: RemoveClient
 * FunctionPoints: The parameter of function RemoveClient.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function RemoveClient, before call RemoveClient not startloading and not attach.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_RemoveClient_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_002 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->RemoveClient(client), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: RemoveClient
 * FunctionPoints: The parameter of function RemoveClient.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function RemoveClient ability state is not attach.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_RemoveClient_003, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_003 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->RemoveClient(client), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_003 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: RemoveClient
 * FunctionPoints: The parameter of function RemoveClient.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function RemoveClient ability state is not inactive.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_RemoveClient_004, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_004 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->RemoveClient(client), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_004 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: RemoveClient
 * FunctionPoints: The parameter of function RemoveClient.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function RemoveClient return ok when not add client.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_RemoveClient_005, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_005 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    abilityState_ = ACTIVE;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->RemoveClient(client), ERR_OK);

    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_005 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: RemoveClient
 * FunctionPoints: Add and remove client.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function add one client and remove it, client count is exact.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_RemoveClient_006, TestSize.Level1)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_006 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    abilityState_ = ACTIVE;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->AddClient(client, true), ERR_OK);
    unsigned int count = 1;
    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), count);
    EXPECT_EQ(dataAbilityRecord->RemoveClient(client), ERR_OK);
    count = 0;
    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), count);

    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_006 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: RemoveClient
 * FunctionPoints: Add and remove client.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function add the same client twice and remove it twice.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_RemoveClient_007, TestSize.Level1)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_007 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    abilityState_ = ACTIVE;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_OK);

    // first add client
    EXPECT_EQ(dataAbilityRecord->AddClient(client, true), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), (uint32_t)1);

    // second add client
    EXPECT_EQ(dataAbilityRecord->AddClient(client, true), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), (uint32_t)2);

    // first remove client
    EXPECT_EQ(dataAbilityRecord->RemoveClient(client), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), (uint32_t)1);

    // second remove client
    EXPECT_EQ(dataAbilityRecord->RemoveClient(client), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), (uint32_t)0);

    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClient_007 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: RemoveClients
 * FunctionPoints: The parameter of function RemoveClients.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function RemoveClients, before call RemoveClients not startloading and not attach.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_RemoveClients_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClients_001 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->RemoveClients(client), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClients_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: RemoveClients
 * FunctionPoints: The parameter of function RemoveClients.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function RemoveClients ability state is not attach.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_RemoveClients_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClients_002 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->RemoveClients(client), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClients_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: RemoveClients
 * FunctionPoints: The parameter of function RemoveClients.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function RemoveClients ability state is not inactive.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_RemoveClients_003, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClients_003 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->RemoveClients(client), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClients_003 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: RemoveClients
 * FunctionPoints: The parameter of function RemoveClients.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function RemoveClients return ok when not add client.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_RemoveClients_005, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClients_005 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    abilityState_ = ACTIVE;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->RemoveClients(client), ERR_OK);

    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClients_005 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: RemoveClients
 * FunctionPoints: The parameter of function RemoveClients.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function RemoveClients, add the same client twice and remove them once.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_RemoveClients_006, TestSize.Level1)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClients_006 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    abilityState_ = ACTIVE;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_OK);

    // first add client
    EXPECT_EQ(dataAbilityRecord->AddClient(client, true), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), (uint32_t)1);

    // second add client
    EXPECT_EQ(dataAbilityRecord->AddClient(client, true), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), (uint32_t)2);

    // remove the same client
    EXPECT_EQ(dataAbilityRecord->RemoveClients(client), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), (uint32_t)0);

    HILOG_INFO("AaFwk_DataAbilityRecord_RemoveClients_005 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: GetClientCount
 * FunctionPoints: The parameter of function GetClientCount.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function GetClientCount, before call GetClientCount not startloading and not attach.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_GetClientCount_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_GetClientCount_001 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), (uint32_t)0);

    HILOG_INFO("AaFwk_DataAbilityRecord_GetClientCount_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: GetClientCount
 * FunctionPoints: The parameter of function GetClientCount.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function GetClientCount ability state is not attach.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_GetClientCount_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_GetClientCount_002 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), (uint32_t)0);

    HILOG_INFO("AaFwk_DataAbilityRecord_GetClientCount_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: GetClientCount
 * FunctionPoints: The parameter of function GetClientCount.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function GetClientCount ability state is not inactive.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_GetClientCount_003, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_GetClientCount_003 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);
    auto client = Token::GetAbilityRecordByToken(abilityRecord_->GetToken());

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->GetClientCount(client), (uint32_t)0);

    HILOG_INFO("AaFwk_DataAbilityRecord_GetClientCount_003 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: KillBoundClientProcesses
 * FunctionPoints: The parameter of function KillBoundClientProcesses.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function KillBoundClientProcesses,
 * before call KillBoundClientProcesses not startloading and not attach.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_KillBoundClientProcesses_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_KillBoundClientProcesses_001 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);

    EXPECT_EQ(dataAbilityRecord->KillBoundClientProcesses(), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_KillBoundClientProcesses_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: KillBoundClientProcesses
 * FunctionPoints: The parameter of function KillBoundClientProcesses.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function KillBoundClientProcesses ability state is not attach.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_KillBoundClientProcesses_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_KillBoundClientProcesses_002 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->KillBoundClientProcesses(), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_KillBoundClientProcesses_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: KillBoundClientProcesses
 * FunctionPoints: The parameter of function KillBoundClientProcesses.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function KillBoundClientProcesses ability state is not inactive.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_KillBoundClientProcesses_003, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_KillBoundClientProcesses_003 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->KillBoundClientProcesses(), ERR_INVALID_STATE);

    HILOG_INFO("AaFwk_DataAbilityRecord_KillBoundClientProcesses_003 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: KillBoundClientProcesses
 * FunctionPoints: The parameter of function KillBoundClientProcesses.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function KillBoundClientProcesses ability state is not inactive.
 */
HWTEST_F(DataAbilityRecordTest, AaFwk_DataAbilityRecord_KillBoundClientProcesses_004, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityRecord_KillBoundClientProcesses_004 start.");

    std::unique_ptr<DataAbilityRecord> dataAbilityRecord = std::make_unique<DataAbilityRecord>(abilityRequest_);

    EXPECT_EQ(dataAbilityRecord->StartLoading(), ERR_OK);
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_EQ(dataAbilityRecord->Attach(abilitySchedulerMock_), ERR_OK);
    abilityState_ = ACTIVE;
    EXPECT_EQ(dataAbilityRecord->OnTransitionDone(abilityState_), ERR_OK);
    EXPECT_EQ(dataAbilityRecord->KillBoundClientProcesses(), ERR_OK);

    HILOG_INFO("AaFwk_DataAbilityRecord_KillBoundClientProcesses_004 end.");
}
}  // namespace AAFwk
}  // namespace OHOS