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
#include "mission_record.h"
#include "mission_stack.h"
#include "hilog_wrapper.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
constexpr int id = 1;
constexpr int userId = 2;

class MissionStackTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<MissionStack> missionStack_;
};

void MissionStackTest::SetUpTestCase(void)
{}
void MissionStackTest::TearDownTestCase(void)
{}
void MissionStackTest::TearDown()
{}

void MissionStackTest::SetUp()
{
    missionStack_ = std::make_shared<MissionStack>(id, userId);
}

AbilityRequest GenerateAbilityRequest(const std::string &deviceName, const std::string &abilityName,
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

std::shared_ptr<MissionRecord> getFirstMissionRecord()
{
    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.first";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto missionRecord = std::make_shared<MissionRecord>();
    missionRecord->AddAbilityRecordToTop(record);

    return missionRecord;
}

std::shared_ptr<MissionRecord> getSecondMissionRecord()
{
    std::string deviceName = "device";
    std::string abilityName = "SecondAbility";
    std::string appName = "SecondApp";
    std::string bundleName = "com.ix.second";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto missionRecord = std::make_shared<MissionRecord>();
    missionRecord->AddAbilityRecordToTop(record);

    return missionRecord;
}

/*
 * Feature: MissionStack
 * Function: RemoveAll
 * SubFunction: NA
 * FunctionPoints: MissionStack RemoveAll
 * EnvConditions:NA
 * CaseDescription: Results after verifying removeAll
 */
HWTEST_F(MissionStackTest, MS_oprator_01, TestSize.Level0)
{
    missionStack_->AddMissionRecordToTop(getFirstMissionRecord());
    missionStack_->RemoveAll();
    EXPECT_EQ(nullptr, missionStack_->GetTopMissionRecord());
}

/*
 * Feature: MissionStack
 * Function: AddMissionRecordToTop
 * SubFunction: NA
 * FunctionPoints: MissionStack AddMissionRecordToTop
 * EnvConditions:NA
 * CaseDescription: Verify that the top ability record is the same as the added record
 */
HWTEST_F(MissionStackTest, MS_oprator_002, TestSize.Level0)
{
    std::shared_ptr<MissionRecord> nullMissionRecord = nullptr;
    missionStack_->AddMissionRecordToTop(nullMissionRecord);

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.first";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto missionRecord = std::make_shared<MissionRecord>(bundleName);
    missionRecord->AddAbilityRecordToTop(record);

    missionStack_->AddMissionRecordToTop(missionRecord);

    auto ability = missionStack_->GetTopAbilityRecord();

    EXPECT_EQ(missionStack_->GetTopAbilityRecord(), record);
}

/*
 * Feature: MissionStack
 * Function: AddMissionRecordToTop
 * SubFunction: NA
 * FunctionPoints: MissionStack AddMissionRecordToTop
 * EnvConditions:NA
 * CaseDescription: Verify that the top mission record is not empty
 */
HWTEST_F(MissionStackTest, MS_oprator_003, TestSize.Level0)
{
    missionStack_->AddMissionRecordToTop(getFirstMissionRecord());
    missionStack_->AddMissionRecordToTop(getSecondMissionRecord());

    EXPECT_NE(missionStack_->GetTopAbilityRecord(), nullptr);
}

/*
 * Feature: MissionStack
 * Function: GetTopMissionRecord
 * SubFunction: NA
 * FunctionPoints: MissionStack GetTopMissionRecord
 * EnvConditions:NA
 * CaseDescription: Verify that the top mission record is the same as the added record
 */
HWTEST_F(MissionStackTest, MS_oprator_004, TestSize.Level0)
{
    EXPECT_EQ(missionStack_->GetTopMissionRecord(), nullptr);

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.first";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto missionRecord = std::make_shared<MissionRecord>();
    missionRecord->AddAbilityRecordToTop(record);

    missionStack_->AddMissionRecordToTop(missionRecord);
    EXPECT_EQ(missionStack_->GetTopMissionRecord(), missionRecord);
}

/*
 * Feature: MissionStack
 * Function: GetMissionStackId
 * SubFunction: NA
 * FunctionPoints: MissionStack GetMissionStackId
 * EnvConditions:NA
 * CaseDescription: Verify that the mission stack id is not 0
 */
HWTEST_F(MissionStackTest, MS_oprator_005, TestSize.Level0)
{
    missionStack_->AddMissionRecordToTop(getFirstMissionRecord());
    EXPECT_NE(missionStack_->GetMissionStackId(), 0);
}

/*
 * Feature: MissionStack
 * Function: GetTargetMissionRecord
 * SubFunction: NA
 * FunctionPoints: MissionStack GetTargetMissionRecord
 * EnvConditions:NA
 * CaseDescription: Verify that the get target mission record is equal to the one added
 */
HWTEST_F(MissionStackTest, MS_oprator_006, TestSize.Level0)
{
    EXPECT_EQ(nullptr, missionStack_->GetTargetMissionRecord("FirstApp"));

    std::string deviceName = "aaaa";
    std::string abilityName = "bbbb";
    std::string appName = "ccccc";
    std::string bundleName = "ddddd";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto missionRecord = std::make_shared<MissionRecord>(bundleName);
    missionRecord->AddAbilityRecordToTop(record);

    missionStack_->AddMissionRecordToTop(missionRecord);

    EXPECT_EQ(missionRecord, missionStack_->GetTargetMissionRecord("ddddd"));
}

/*
 * Feature: MissionStack
 * Function: GetMissionStackId GetMissionStackUserId
 * SubFunction: NA
 * FunctionPoints: MissionStack GetMissionStackId and GetMissionStackUserId
 * EnvConditions:NA
 * CaseDescription: Verify the value of get mission stack ID and get mission stack user ID
 */
HWTEST_F(MissionStackTest, MS_oprator_007, TestSize.Level0)
{
    EXPECT_EQ(missionStack_->GetMissionStackId(), 1);
    EXPECT_EQ(missionStack_->GetMissionStackUserId(), 2);
}

/*
 * Feature: MissionStack
 * Function: GetMissionRecordCount
 * SubFunction: NA
 * FunctionPoints: MissionStack GetMissionRecordCount
 * EnvConditions:NA
 * CaseDescription: Verify the value of get mission record count
 */
HWTEST_F(MissionStackTest, MS_oprator_008, TestSize.Level0)
{
    EXPECT_EQ(missionStack_->GetMissionRecordCount(), 0);
    missionStack_->AddMissionRecordToTop(getSecondMissionRecord());
    EXPECT_EQ(missionStack_->GetMissionRecordCount(), 1);
}

/*
 * Feature: MissionStack
 * Function: GetTopMissionRecord
 * SubFunction: NA
 * FunctionPoints: MissionStack GetTopMissionRecord
 * EnvConditions:NA
 * CaseDescription: Verify that the get top mission record is equal to the one added
 */
HWTEST_F(MissionStackTest, MS_oprator_009, TestSize.Level0)
{
    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.first";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto missionRecord = std::make_shared<MissionRecord>();
    missionRecord->AddAbilityRecordToTop(record);

    std::shared_ptr<MissionRecord> nullMR = nullptr;
    missionStack_->MoveMissionRecordToTop(nullMR);

    missionStack_->AddMissionRecordToTop(getSecondMissionRecord());

    missionStack_->MoveMissionRecordToTop(missionRecord);
    EXPECT_EQ(missionStack_->GetTopMissionRecord(), missionRecord);
}

/*
 * Feature: MissionStack
 * Function: Dump DumpStackList
 * SubFunction: NA
 * FunctionPoints: MissionStack Dump and DumpStackList
 * EnvConditions:NA
 * CaseDescription: Verify Dump and DumpStackList results
 */
HWTEST_F(MissionStackTest, MS_oprator_010, TestSize.Level0)
{
    std::vector<std::string> info;
    std::vector<std::string> listInfo;

    const Want want;
    AbilityInfo abilityInfo;
    abilityInfo.name = "FirstAbility";
    abilityInfo.bundleName = "com.ix.first";
    ApplicationInfo appInfo;
    appInfo.name = "FirstApp";
    appInfo.bundleName = "com.ix.first";

    auto record = std::make_shared<AbilityRecord>(want, abilityInfo, appInfo, -1);
    auto missionRecord = std::make_shared<MissionRecord>();
    missionRecord->AddAbilityRecordToTop(record);
    missionStack_->MoveMissionRecordToTop(missionRecord);

    missionStack_->Dump(info);
    missionStack_->DumpStackList(listInfo);

    EXPECT_NE(info.end(), std::find_if(info.begin(), info.end(), [](std::string &it) {
        return std::string::npos != it.find("FirstAbility");
    }));

    EXPECT_NE(listInfo.end(), std::find_if(listInfo.begin(), listInfo.end(), [](std::string &it) {
        return std::string::npos != it.find("MissionStack ID #1 [ #10 ]");
    }));
}

/*
 * Feature: MissionStack
 * Function: GetMissionRecordById
 * SubFunction: NA
 * FunctionPoints: MissionStack GetMissionRecordById
 * EnvConditions:NA
 * CaseDescription: Verify that the get mission record by ID value is empty
 */
HWTEST_F(MissionStackTest, MS_oprator_011, TestSize.Level0)
{
    EXPECT_EQ(missionStack_->GetMissionRecordById(0), nullptr);

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.first";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto missionRecord = std::make_shared<MissionRecord>();
    missionRecord->AddAbilityRecordToTop(record);

    missionStack_->AddMissionRecordToTop(missionRecord);

    EXPECT_NE(nullptr, missionStack_->GetMissionRecordById(11));
}

/*
 * Feature: MissionStack
 * Function: GetMissionRecordById
 * SubFunction: NA
 * FunctionPoints: MissionStack GetMissionRecordById
 * EnvConditions:NA
 * CaseDescription: Verify the get mission record by ID value
 */
HWTEST_F(MissionStackTest, MS_oprator_012, TestSize.Level0)
{
    EXPECT_EQ(missionStack_->GetMissionRecordById(0), nullptr);

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.first";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto missionRecord = std::make_shared<MissionRecord>();
    missionRecord->AddAbilityRecordToTop(record);

    missionStack_->AddMissionRecordToTop(missionRecord);

    EXPECT_NE(nullptr, missionStack_->GetMissionRecordById(12));
}

/*
 * Feature: MissionStack
 * Function: GetAbilityRecordByToken
 * SubFunction: NA
 * FunctionPoints: MissionStack GetAbilityRecordByToken
 * EnvConditions:NA
 * CaseDescription: Verify that the values of get ability record by token are equal
 */
HWTEST_F(MissionStackTest, MS_oprator_013, TestSize.Level0)
{
    OHOS::sptr<Token> token = nullptr;
    EXPECT_EQ(missionStack_->GetAbilityRecordByToken(token), nullptr);

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.first";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto missionRecord = std::make_shared<MissionRecord>();
    missionRecord->AddAbilityRecordToTop(record);
    auto firstToken = record->GetToken();

    missionStack_->AddMissionRecordToTop(missionRecord);

    EXPECT_EQ(record, missionStack_->GetAbilityRecordByToken(firstToken));
}

/*
 * Feature: MissionStack
 * Function: RemoveAbilityRecordByToken
 * SubFunction: NA
 * FunctionPoints: MissionStack RemoveAbilityRecordByToken
 * EnvConditions:NA
 * CaseDescription: Verify that remove ability record by token is successful
 */
HWTEST_F(MissionStackTest, MS_oprator_014, TestSize.Level0)
{
    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.first";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto missionRecord = std::make_shared<MissionRecord>();
    missionRecord->AddAbilityRecordToTop(record);
    auto firstToken = record->GetToken();

    EXPECT_EQ(missionStack_->RemoveAbilityRecordByToken(*(firstToken.GetRefPtr())), false);

    missionStack_->AddMissionRecordToTop(missionRecord);

    EXPECT_EQ(true, missionStack_->RemoveAbilityRecordByToken(*(firstToken.GetRefPtr())));
}

/*
 * Feature: MissionStack
 * Function: RemoveMissionRecord
 * SubFunction: NA
 * FunctionPoints: MissionStack RemoveMissionRecord
 * EnvConditions:NA
 * CaseDescription: Verify that remove mission record is successful
 */
HWTEST_F(MissionStackTest, MS_oprator_015, TestSize.Level0)
{
    EXPECT_EQ(missionStack_->RemoveMissionRecord(100), false);

    std::string deviceName = "device";
    std::string abilityName = "FirstAbility";
    std::string appName = "FirstApp";
    std::string bundleName = "com.ix.first";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto missionRecord = std::make_shared<MissionRecord>("com.ix.first");
    missionRecord->AddAbilityRecordToTop(record);

    missionStack_->AddMissionRecordToTop(missionRecord);

    EXPECT_EQ(true, missionStack_->RemoveMissionRecord(15));
}
}  // namespace AAFwk
}  // namespace OHOS