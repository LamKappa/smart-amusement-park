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
#include "ability_record.h"
#undef private
#undef protected

#include "ability_scheduler.h"
#include "connection_record.h"
#include "mission_record.h"
#include "mock_ability_connect_callback.h"
#include "ability_connect_callback_stub.h"

using namespace testing::ext;

namespace OHOS {
namespace AAFwk {

class AbilityRecordTest : public testing::TestWithParam<OHOS::AAFwk::AbilityState> {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<AbilityRecord> abilityRecord_;
    std::shared_ptr<AbilityResult> abilityResult_;
    std::shared_ptr<AbilityRequest> abilityRequest_;
};

void AbilityRecordTest::SetUpTestCase(void)
{}
void AbilityRecordTest::TearDownTestCase(void)
{}

void AbilityRecordTest::SetUp(void)
{
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    OHOS::AppExecFwk::ApplicationInfo applicationInfo;
    Want want;
    abilityRecord_ = std::make_shared<AbilityRecord>(want, abilityInfo, applicationInfo);
    abilityResult_ = std::make_shared<AbilityResult>(-1, -1, want);
    abilityRequest_ = std::make_shared<AbilityRequest>();
    abilityRecord_->Init();
}

void AbilityRecordTest::TearDown(void)
{
    abilityRecord_.reset();
}

bool IsTestAbilityExist(const std::string &data)
{
    return std::string::npos != data.find("previous ability app name [NULL]");
}

bool IsTestAbilityExist1(const std::string &data)
{
    return std::string::npos != data.find("test_pre_app");
}

bool IsTestAbilityExist2(const std::string &data)
{
    return std::string::npos != data.find("test_next_app");
}

/*
 * Feature: AbilityRecord
 * Function: GetRecordId
 * SubFunction: GetRecordId
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify create one abilityRecord could through GetRecordId 1
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_GetRecordId, TestSize.Level1)
{
    EXPECT_EQ(abilityRecord_->GetRecordId(), 0);
}

/*
 * Feature: AbilityRecord
 * Function: create AbilityRecord
 * SubFunction: NA
 * FunctionPoints: LoadAbility Activate Inactivate MoveToBackground
 * EnvConditions:NA
 * CaseDescription: LoadAbility Activate Inactivate MoveToBackground UT.
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_UpdateLifeState, TestSize.Level1)
{
    abilityRecord_->LoadAbility();
    EXPECT_EQ(abilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::INITIAL);
    abilityRecord_->Activate();
    EXPECT_EQ(abilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVATING);
    abilityRecord_->Inactivate();
    EXPECT_EQ(abilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::INACTIVATING);
    abilityRecord_->MoveToBackground(nullptr);
    EXPECT_EQ(abilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::MOVING_BACKGROUND);
}

/*
 * Feature: AbilityRecord
 * Function: create AbilityRecord
 * SubFunction: NA
 * FunctionPoints: SetMissionRecord GetMissionRecord
 * EnvConditions:NA
 * CaseDescription: SetMissionRecord GetMissionRecord UT.
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_SetGetMissionRecord, TestSize.Level1)
{
    std::shared_ptr<MissionRecord> missionRecord = std::make_shared<MissionRecord>();
    abilityRecord_->SetMissionRecord(missionRecord);
    EXPECT_EQ(abilityRecord_->GetMissionRecord().get(), missionRecord.get());
}

/*
 * Feature: AbilityRecord
 * Function: create AbilityRecord
 * SubFunction: NA
 * FunctionPoints: SetAbilityInfo GetAbilityInfo
 * EnvConditions:NA
 * CaseDescription: SetAbilityInfo GetAbilityInfo UT.
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_SetGetAbilityInfo, TestSize.Level1)
{
    Want want;
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.applicationName = std::string("TestApp");
    OHOS::AppExecFwk::ApplicationInfo applicationInfo;
    std::shared_ptr<AbilityRecord> abilityRecord = std::make_shared<AbilityRecord>(want, abilityInfo, applicationInfo);
    EXPECT_EQ(abilityRecord->GetAbilityInfo().applicationName, std::string("TestApp"));
}

/*
 * Feature: AbilityRecord
 * Function: create AbilityRecord
 * SubFunction: NA
 * FunctionPoints: SetApplicationInfo GetApplicationInfo
 * EnvConditions:NA
 * CaseDescription: SetApplicationInfo GetApplicationInfo UT.
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_SetGetApplicationInfo, TestSize.Level1)
{
    Want want;
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    OHOS::AppExecFwk::ApplicationInfo applicationInfo;
    applicationInfo.name = "TestApp";
    std::shared_ptr<AbilityRecord> abilityRecord = std::make_shared<AbilityRecord>(want, abilityInfo, applicationInfo);
    EXPECT_EQ(abilityRecord->GetApplicationInfo().name, "TestApp");
}

/*
 * Feature: AbilityRecord
 * Function: create AbilityRecord
 * SubFunction: NA
 * FunctionPoints: SetAbilityState GetAbilityState
 * EnvConditions:NA
 * CaseDescription: SetAbilityState GetAbilityState UT.
 */
HWTEST_P(AbilityRecordTest, AaFwk_AbilityMS_SetGetAbilityState, TestSize.Level1)
{
    OHOS::AAFwk::AbilityState state = GetParam();
    abilityRecord_->SetAbilityState(state);
    EXPECT_EQ(static_cast<int>(state), static_cast<int>(abilityRecord_->GetAbilityState()));
}

INSTANTIATE_TEST_CASE_P(AbilityRecordTestCaseP, AbilityRecordTest,
    testing::Values(OHOS::AAFwk::AbilityState::INITIAL, OHOS::AAFwk::AbilityState::INACTIVE,
        OHOS::AAFwk::AbilityState::ACTIVE, OHOS::AAFwk::AbilityState::BACKGROUND,
        OHOS::AAFwk::AbilityState::SUSPENDED));

/*
 * Feature: AbilityRecord
 * Function: create AbilityRecord
 * SubFunction: NA
 * FunctionPoints: SetAbilityState GetAbilityState
 * EnvConditions:NA
 * CaseDescription: SetAbilityState GetAbilityState UT.
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_SetGetToken, TestSize.Level1)
{
    EXPECT_EQ(Token::GetAbilityRecordByToken(abilityRecord_->GetToken()).get(), abilityRecord_.get());
}

/*
 * Feature: AbilityRecord
 * Function: create AbilityRecord
 * SubFunction: NA
 * FunctionPoints: SetPreAbilityRecord SetNextAbilityRecord GetPreAbilityRecord GetNextAbilityRecord
 * EnvConditions:NA
 * CaseDescription: SetPreAbilityRecord SetNextAbilityRecord GetPreAbilityRecord GetNextAbilityRecord UT.
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_SetGetPreNextAbilityReocrd, TestSize.Level1)
{
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    OHOS::AppExecFwk::ApplicationInfo applicationInfo;
    Want want;
    std::shared_ptr<AbilityRecord> preAbilityRecord =
        std::make_shared<AbilityRecord>(want, abilityInfo, applicationInfo);
    std::shared_ptr<AbilityRecord> nextAbilityRecord =
        std::make_shared<AbilityRecord>(want, abilityInfo, applicationInfo);
    abilityRecord_->SetPreAbilityRecord(preAbilityRecord);
    abilityRecord_->SetNextAbilityRecord(nextAbilityRecord);
    EXPECT_EQ(abilityRecord_->GetPreAbilityRecord().get(), preAbilityRecord.get());
    EXPECT_EQ(abilityRecord_->GetNextAbilityRecord().get(), nextAbilityRecord.get());
}

/*
 * Feature: AbilityRecord
 * Function: create AbilityRecord
 * SubFunction: NA
 * FunctionPoints: SetEventId GetEventId
 * EnvConditions:NA
 * CaseDescription: SetEventId GetEventId UT.
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_SetGetEventId, TestSize.Level1)
{
    int64_t eventId = 1;
    abilityRecord_->SetEventId(eventId);
    EXPECT_EQ(eventId, abilityRecord_->GetEventId());
}

/*
 * Feature: AbilityRecord
 * Function: create AbilityRecord
 * SubFunction: NA
 * FunctionPoints: IsReady
 * EnvConditions:NA
 * CaseDescription: IsReady UT.
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_IsReady, TestSize.Level1)
{
    EXPECT_EQ(false, abilityRecord_->IsReady());
    OHOS::sptr<IAbilityScheduler> scheduler = new AbilityScheduler();
    abilityRecord_->SetScheduler(scheduler);
    EXPECT_EQ(true, abilityRecord_->IsReady());
}

/*
 * Feature: AbilityRecord
 * Function: create AbilityRecord
 * SubFunction: NA
 * FunctionPoints: IsLauncherAbility
 * EnvConditions:NA
 * CaseDescription: IsLauncherAbility UT.
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_IsLauncherAbility, TestSize.Level1)
{
    EXPECT_EQ(false, abilityRecord_->IsLauncherAbility());
    Want launcherWant;
    launcherWant.AddEntity(Want::ENTITY_HOME);
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    OHOS::AppExecFwk::ApplicationInfo applicationInfo;
    std::unique_ptr<AbilityRecord> launcherAbilityRecord =
        std::make_unique<AbilityRecord>(launcherWant, abilityInfo, applicationInfo);
    launcherAbilityRecord->Init();
    EXPECT_EQ(false, launcherAbilityRecord->IsLauncherAbility());
}

/*
 * Feature: AbilityRecord
 * Function: Add connection record to ability record' list
 * SubFunction: NA
 * FunctionPoints: AddConnectRecordToList
 * EnvConditions:NA
 * CaseDescription: AddConnectRecordToList UT.
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_AddConnectRecordToList, TestSize.Level0)
{
    // test1 for input param is null
    abilityRecord_->AddConnectRecordToList(nullptr);
    auto connList = abilityRecord_->GetConnectRecordList();
    EXPECT_EQ(0, static_cast<int>(connList.size()));

    // test2 for adding new connection record to empty list
    OHOS::sptr<IAbilityConnection> callback1 = new AbilityConnectCallback();
    auto newConnRecord1 =
        ConnectionRecord::CreateConnectionRecord(abilityRecord_->GetToken(), abilityRecord_, callback1);
    abilityRecord_->AddConnectRecordToList(newConnRecord1);
    connList = abilityRecord_->GetConnectRecordList();
    EXPECT_EQ(1, static_cast<int>(connList.size()));

    // test3 for adding new connection record to non-empty list
    OHOS::sptr<IAbilityConnection> callback2 = new AbilityConnectCallback();
    auto newConnRecord2 =
        ConnectionRecord::CreateConnectionRecord(abilityRecord_->GetToken(), abilityRecord_, callback2);
    abilityRecord_->AddConnectRecordToList(newConnRecord2);
    connList = abilityRecord_->GetConnectRecordList();
    EXPECT_EQ(2, static_cast<int>(connList.size()));

    // test4 for adding old connection record to non-empty list
    abilityRecord_->AddConnectRecordToList(newConnRecord2);
    connList = abilityRecord_->GetConnectRecordList();
    EXPECT_EQ(2, static_cast<int>(connList.size()));

    // test5 for delete nullptr from list
    abilityRecord_->RemoveConnectRecordFromList(nullptr);
    connList = abilityRecord_->GetConnectRecordList();
    EXPECT_EQ(2, static_cast<int>(connList.size()));

    // test6 for delete no-match member from list
    auto newConnRecord3 =
        ConnectionRecord::CreateConnectionRecord(abilityRecord_->GetToken(), abilityRecord_, callback2);
    abilityRecord_->RemoveConnectRecordFromList(newConnRecord3);
    connList = abilityRecord_->GetConnectRecordList();
    EXPECT_EQ(2, static_cast<int>(connList.size()));

    // test7 for delete match member from list
    abilityRecord_->RemoveConnectRecordFromList(newConnRecord2);
    connList = abilityRecord_->GetConnectRecordList();
    EXPECT_EQ(1, static_cast<int>(connList.size()));

    // test8 for get ability unknown type
    EXPECT_EQ(OHOS::AppExecFwk::AbilityType::UNKNOWN, abilityRecord_->GetAbilityInfo().type);
}

/*
 * Feature: AbilityRecord
 * Function: ConvertAbilityState
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify ConvertAbilityState convert success
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_ConvertAbilityState, TestSize.Level1)
{
    abilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INITIAL);
    EXPECT_EQ(abilityRecord_->ConvertAbilityState(abilityRecord_->GetAbilityState()), "INITIAL");
    abilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);
    EXPECT_EQ(abilityRecord_->ConvertAbilityState(abilityRecord_->GetAbilityState()), "INACTIVE");
    abilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(abilityRecord_->ConvertAbilityState(abilityRecord_->GetAbilityState()), "ACTIVE");
    abilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::BACKGROUND);
    EXPECT_EQ(abilityRecord_->ConvertAbilityState(abilityRecord_->GetAbilityState()), "BACKGROUND");
    abilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::SUSPENDED);
    EXPECT_EQ(abilityRecord_->ConvertAbilityState(abilityRecord_->GetAbilityState()), "SUSPENDED");
    abilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
    EXPECT_EQ(abilityRecord_->ConvertAbilityState(abilityRecord_->GetAbilityState()), "INACTIVATING");
    abilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVATING);
    EXPECT_EQ(abilityRecord_->ConvertAbilityState(abilityRecord_->GetAbilityState()), "ACTIVATING");
    abilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::MOVING_BACKGROUND);
    EXPECT_EQ(abilityRecord_->ConvertAbilityState(abilityRecord_->GetAbilityState()), "MOVING_BACKGROUND");
    abilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::TERMINATING);
    EXPECT_EQ(abilityRecord_->ConvertAbilityState(abilityRecord_->GetAbilityState()), "TERMINATING");
}

/*
 * Feature: AbilityRecord
 * Function: IsTerminating
 * SubFunction: IsTerminating SetTerminatingState
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify IsTerminating SetTerminatingState success
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_IsTerminating, TestSize.Level1)
{
    abilityRecord_->SetTerminatingState();
    EXPECT_EQ(abilityRecord_->IsTerminating(), true);
}

/*
 * Feature: AbilityRecord
 * Function: Activate
 * SubFunction: Activate
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify lifecycleDeal_ is nullptr cause Activate is not call
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_Activate, TestSize.Level1)
{
    abilityRecord_->lifecycleDeal_ = nullptr;
    abilityRecord_->currentState_ = OHOS::AAFwk::AbilityState::INITIAL;
    abilityRecord_->Activate();
    EXPECT_EQ(abilityRecord_->currentState_, OHOS::AAFwk::AbilityState::INITIAL);
    abilityRecord_->lifecycleDeal_ = std::make_unique<LifecycleDeal>();
    abilityRecord_->Activate();
    EXPECT_EQ(abilityRecord_->currentState_, OHOS::AAFwk::AbilityState::ACTIVATING);
}

/*
 * Feature: AbilityRecord
 * Function: Inactivate
 * SubFunction: Inactivate
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify lifecycleDeal_ is nullptr cause Inactivate is not call
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_Inactivate, TestSize.Level1)
{
    abilityRecord_->lifecycleDeal_ = nullptr;
    abilityRecord_->currentState_ = OHOS::AAFwk::AbilityState::INITIAL;
    abilityRecord_->Inactivate();
    EXPECT_EQ(abilityRecord_->currentState_, OHOS::AAFwk::AbilityState::INITIAL);
    abilityRecord_->lifecycleDeal_ = std::make_unique<LifecycleDeal>();
    abilityRecord_->Inactivate();
    EXPECT_EQ(abilityRecord_->currentState_, OHOS::AAFwk::AbilityState::INACTIVATING);
}

/*
 * Feature: AbilityRecord
 * Function: MoveToBackground
 * SubFunction: MoveToBackground
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify lifecycleDeal_ is nullptr cause MoveToBackground is not call
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_MoveToBackground, TestSize.Level1)
{
    abilityRecord_->lifecycleDeal_ = nullptr;
    abilityRecord_->currentState_ = OHOS::AAFwk::AbilityState::INITIAL;
    abilityRecord_->MoveToBackground([]() {

    });
    EXPECT_EQ(abilityRecord_->currentState_, OHOS::AAFwk::AbilityState::INITIAL);
    abilityRecord_->lifecycleDeal_ = std::make_unique<LifecycleDeal>();
    abilityRecord_->MoveToBackground([]() {

    });
    EXPECT_EQ(abilityRecord_->currentState_, OHOS::AAFwk::AbilityState::MOVING_BACKGROUND);
}

/*
 * Feature: AbilityRecord
 * Function: Terminate
 * SubFunction: Terminate
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify lifecycleDeal_ is nullptr cause Terminate is not call
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_Terminate, TestSize.Level1)
{
    abilityRecord_->lifecycleDeal_ = nullptr;
    abilityRecord_->currentState_ = OHOS::AAFwk::AbilityState::INITIAL;
    abilityRecord_->Terminate([]() {

    });
    EXPECT_EQ(abilityRecord_->currentState_, OHOS::AAFwk::AbilityState::INITIAL);
    abilityRecord_->lifecycleDeal_ = std::make_unique<LifecycleDeal>();
    abilityRecord_->Terminate([]() {

    });
    EXPECT_EQ(abilityRecord_->currentState_, OHOS::AAFwk::AbilityState::TERMINATING);
}

/*
 * Feature: AbilityRecord
 * Function: GetWindowInfo
 * SubFunction: GetWindowInfo RemoveWindowInfo AddWindowInfo
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify windowInfo set get remove success
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_GetWindowInfo, TestSize.Level1)
{
    int windowToken = 1;
    abilityRecord_->AddWindowInfo(windowToken);
    EXPECT_NE(abilityRecord_->GetWindowInfo(), nullptr);
    abilityRecord_->RemoveWindowInfo();
    EXPECT_EQ(abilityRecord_->GetWindowInfo(), nullptr);
}

/*
 * Feature: AbilityRecord
 * Function: SetScheduler
 * SubFunction: SetScheduler
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify AbilityRecord SetScheduler success
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_SetScheduler, TestSize.Level1)
{
    OHOS::sptr<IAbilityScheduler> scheduler = new AbilityScheduler();
    abilityRecord_->lifecycleDeal_ = std::make_unique<LifecycleDeal>();
    EXPECT_EQ(false, abilityRecord_->IsReady());
    abilityRecord_->SetScheduler(scheduler);
    EXPECT_EQ(true, abilityRecord_->IsReady());
}

/*
 * Feature: Token
 * Function: GetAbilityRecordByToken
 * SubFunction: GetAbilityRecordByToken
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify AbilityRecord token GetAbilityRecordByToken success
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_GetAbilityRecordByToken, TestSize.Level1)
{
    EXPECT_EQ(Token::GetAbilityRecordByToken(abilityRecord_->GetToken()).get(), abilityRecord_.get());
    EXPECT_EQ(abilityRecord_->GetToken()->GetAbilityRecord(), abilityRecord_);
}

/*
 * Feature: AbilityRecord
 * Function: Dump
 * SubFunction: Dump
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify Dump success
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_Dump, TestSize.Level1)
{
    std::vector<std::string> info;
    info.push_back(std::string("0"));
    abilityRecord_->Dump(info);
    EXPECT_EQ(std::find_if(info.begin(), info.end(), IsTestAbilityExist) != info.end(), true);
    Want wantPre;
    std::string entity = Want::ENTITY_HOME;
    wantPre.AddEntity(entity);

    std::string testAppName = "test_pre_app";
    OHOS::AppExecFwk::AbilityInfo abilityInfoPre;
    abilityInfoPre.applicationName = testAppName;
    OHOS::AppExecFwk::ApplicationInfo appinfoPre;
    appinfoPre.name = testAppName;

    auto preAbilityRecord = std::make_shared<AbilityRecord>(wantPre, abilityInfoPre, appinfoPre);
    abilityRecord_->SetPreAbilityRecord(nullptr);
    abilityRecord_->Dump(info);
    abilityRecord_->SetPreAbilityRecord(preAbilityRecord);
    abilityRecord_->Dump(info);

    Want wantNext;
    std::string entityNext = Want::ENTITY_HOME;
    wantNext.AddEntity(entityNext);
    std::string testAppNameNext = "test_next_app";
    OHOS::AppExecFwk::AbilityInfo abilityInfoNext;
    abilityInfoNext.applicationName = testAppNameNext;
    OHOS::AppExecFwk::ApplicationInfo appinfoNext;
    appinfoNext.name = testAppNameNext;
    auto nextAbilityRecord = std::make_shared<AbilityRecord>(wantNext, abilityInfoNext, appinfoNext);
    abilityRecord_->SetNextAbilityRecord(nullptr);
    abilityRecord_->Dump(info);
    abilityRecord_->SetNextAbilityRecord(nextAbilityRecord);
    abilityRecord_->Dump(info);
}

/*
 * Feature: AbilityRecord
 * Function: SetWant GetWant
 * SubFunction: SetWant GetWant
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify SetWant GetWant can get,set success
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_Want, TestSize.Level1)
{
    Want want;
    want.SetFlags(100);
    abilityRecord_->SetWant(want);
    EXPECT_EQ(want.GetFlags(), abilityRecord_->GetWant().GetFlags());
}

/*
 * Feature: AbilityRecord
 * Function: GetRequestCode
 * SubFunction: GetRequestCode
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify GetRequestCode success
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_GetRequestCode, TestSize.Level1)
{
    EXPECT_EQ(abilityRecord_->GetRequestCode(), -1);
}

/*
 * Feature: AbilityRecord
 * Function: GetAbilityTypeString
 * SubFunction: GetAbilityTypeString
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify GetAbilityTypeString can get success
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_GetAbilityTypeString, TestSize.Level1)
{
    std::string typeStr;
    std::shared_ptr<AbilityRecord> recordUn;
    OHOS::AppExecFwk::AbilityInfo ability;
    OHOS::AppExecFwk::ApplicationInfo appInfo;
    Want wantUn;
    recordUn = std::make_shared<AbilityRecord>(wantUn, ability, appInfo);
    recordUn->GetAbilityTypeString(typeStr);
    EXPECT_EQ(typeStr, "UNKNOWN");

    std::shared_ptr<AbilityRecord> recordService;
    OHOS::AppExecFwk::AbilityInfo abilityService;
    abilityService.type = OHOS::AppExecFwk::AbilityType::SERVICE;
    OHOS::AppExecFwk::ApplicationInfo appInfoService;
    Want wantService;
    recordService = std::make_shared<AbilityRecord>(wantService, abilityService, appInfoService);
    recordService->GetAbilityTypeString(typeStr);
    EXPECT_EQ(typeStr, "SERVICE");

    std::shared_ptr<AbilityRecord> recordPage;
    OHOS::AppExecFwk::AbilityInfo abilityPage;
    abilityPage.type = OHOS::AppExecFwk::AbilityType::PAGE;
    OHOS::AppExecFwk::ApplicationInfo appInfoPage;
    Want wantPage;
    recordPage = std::make_shared<AbilityRecord>(wantPage, abilityPage, appInfoPage);
    recordPage->GetAbilityTypeString(typeStr);
    EXPECT_EQ(typeStr, "PAGE");

    std::shared_ptr<AbilityRecord> recordData;
    OHOS::AppExecFwk::AbilityInfo abilityData;
    abilityData.type = OHOS::AppExecFwk::AbilityType::DATA;
    OHOS::AppExecFwk::ApplicationInfo appInfoData;
    Want wantData;
    recordData = std::make_shared<AbilityRecord>(wantData, abilityData, appInfoData);
    recordData->GetAbilityTypeString(typeStr);
    EXPECT_EQ(typeStr, "DATA");
}

/*
 * Feature: AbilityRecord
 * Function: SetResult GetResult
 * SubFunction: SetResult GetResult
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify SetResult GetResult can get,set success
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_Result, TestSize.Level1)
{
    abilityResult_->requestCode_ = 10;
    abilityRecord_->SetResult(abilityResult_);
    EXPECT_EQ(10, abilityRecord_->GetResult()->requestCode_);
}

/*
 * Feature: AbilityRecord
 * Function: SendResult
 * SubFunction: SendResult
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify SendResult scheduler is nullptr
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_SendResult, TestSize.Level1)
{
    OHOS::sptr<IAbilityScheduler> scheduler = new AbilityScheduler();
    abilityRecord_->SetScheduler(scheduler);
    abilityRecord_->SetResult(abilityResult_);
    abilityRecord_->SendResult();
    EXPECT_EQ(nullptr, abilityRecord_->GetResult());
}

/*
 * Feature: AbilityRecord
 * Function: SetConnRemoteObject GetConnRemoteObject
 * SubFunction: SetConnRemoteObject GetConnRemoteObject
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify SetConnRemoteObject GetConnRemoteObject UT
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_ConnRemoteObject, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> remote;
    abilityRecord_->SetConnRemoteObject(remote);
    EXPECT_EQ(remote, abilityRecord_->GetConnRemoteObject());
}

/*
 * Feature: AbilityRecord
 * Function: IsCreateByConnect SetCreateByConnectMode
 * SubFunction: IsCreateByConnect SetCreateByConnectMode
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify IsCreateByConnect SetCreateByConnectMode UT
 */
HWTEST_F(AbilityRecordTest, AaFwk_AbilityMS_CreateByConnect, TestSize.Level1)
{
    abilityRecord_->SetCreateByConnectMode();
    EXPECT_EQ(true, abilityRecord_->IsCreateByConnect());
}
}  // namespace AAFwk
}  // namespace OHOS