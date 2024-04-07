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

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
class MissionRecordTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static AbilityRequest GenerateAbilityRequest(const std::string &deviceName, const std::string &abilityName,
        const std::string &appName, const std::string &bundleName);

    Want want_;
    AbilityInfo abilityInfo_;
    ApplicationInfo appInfo_;
};

void MissionRecordTest::SetUpTestCase(void)
{}
void MissionRecordTest::TearDownTestCase(void)
{}
void MissionRecordTest::SetUp(void)
{}
void MissionRecordTest::TearDown(void)
{}

AbilityRequest MissionRecordTest::GenerateAbilityRequest(const std::string &deviceName, const std::string &abilityName,
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
 * Feature: MissionRecord
 * Function: GetAbilityRecordCount and GetMissionRecordId
 * SubFunction: NA
 * FunctionPoints: MissionRecord GetAbilityRecordCount and GetMissionRecordId
 * EnvConditions:NA
 * CaseDescription: Verify GetAbilityRecordCount and GetMissionRecordId value
 */
HWTEST_F(MissionRecordTest, stack_operating_001, TestSize.Level1)
{
    auto missionRecord = std::make_shared<MissionRecord>("");
    EXPECT_EQ(0, missionRecord->GetAbilityRecordCount());
    EXPECT_EQ(0, missionRecord->GetMissionRecordId());
}

/*
 * Feature: MissionRecord
 * Function: AddAbilityRecordToTop and GetTopAbilityRecord
 * SubFunction: NA
 * FunctionPoints: MissionRecord AddAbilityRecordToTop and GetTopAbilityRecord
 * EnvConditions:NA
 * CaseDescription: add ability record, Verify get top ability record equality
 */
HWTEST_F(MissionRecordTest, stack_operating_002, TestSize.Level1)
{
    auto ability = std::make_shared<AbilityRecord>(want_, abilityInfo_, appInfo_);
    auto missionRecord = std::make_shared<MissionRecord>(abilityInfo_.bundleName);
    missionRecord->AddAbilityRecordToTop(ability);
    EXPECT_EQ(ability, missionRecord->GetTopAbilityRecord());
    EXPECT_EQ(1, missionRecord->GetMissionRecordId());
}

/*
 * Feature: MissionRecord
 * Function: AddAbilityRecordToTop and GetTopAbilityRecord
 * SubFunction: NA
 * FunctionPoints: MissionRecord AddAbilityRecordToTop and GetTopAbilityRecord
 * EnvConditions:NA
 * CaseDescription: add null ability record, Verify that get top ability record is empty
 */
HWTEST_F(MissionRecordTest, stack_operating_003, TestSize.Level1)
{
    auto ability = std::make_shared<AbilityRecord>(want_, abilityInfo_, appInfo_);
    auto missionRecord = std::make_shared<MissionRecord>(abilityInfo_.bundleName);
    missionRecord->AddAbilityRecordToTop(nullptr);
    EXPECT_EQ(0, missionRecord->GetAbilityRecordCount());
    EXPECT_EQ(nullptr, missionRecord->GetTopAbilityRecord());
}

/*
 * Feature: MissionRecord
 * Function: GetAbilityRecordCount and GetTopAbilityRecord
 * SubFunction: NA
 * FunctionPoints: MissionRecord GetAbilityRecordCount and GetTopAbilityRecord
 * EnvConditions:NA
 * CaseDescription: Create a mission record to view the value
 */
HWTEST_F(MissionRecordTest, stack_operating_005, TestSize.Level1)
{
    auto missionRecord = std::make_shared<MissionRecord>("");
    EXPECT_EQ(0, missionRecord->GetAbilityRecordCount());
    EXPECT_EQ(nullptr, missionRecord->GetTopAbilityRecord());
}

/*
 * Feature: MissionRecord
 * Function: RemoveAbilityRecord
 * SubFunction: NA
 * FunctionPoints: MissionRecord RemoveAbilityRecord
 * EnvConditions:NA
 * CaseDescription: Create a mission record, add ability, Verify the removeabilityrecord operation
 */
HWTEST_F(MissionRecordTest, stack_operating_006, TestSize.Level1)
{
    auto ability = std::make_shared<AbilityRecord>(want_, abilityInfo_, appInfo_);
    auto missionRecord = std::make_shared<MissionRecord>(abilityInfo_.bundleName);
    missionRecord->AddAbilityRecordToTop(ability);
    EXPECT_EQ(1, missionRecord->GetAbilityRecordCount());
    EXPECT_EQ(true, missionRecord->RemoveAbilityRecord(ability));
    EXPECT_EQ(0, missionRecord->GetAbilityRecordCount());
}

/*
 * Feature: MissionRecord
 * Function: RemoveAbilityRecord
 * SubFunction: NA
 * FunctionPoints: MissionRecord RemoveAbilityRecord
 * EnvConditions:NA
 * CaseDescription: Create a mission record, add ability, remove ability2 fail
 */
HWTEST_F(MissionRecordTest, stack_operating_007, TestSize.Level1)
{
    auto ability = std::make_shared<AbilityRecord>(want_, abilityInfo_, appInfo_);
    auto ability2 = std::make_shared<AbilityRecord>(want_, abilityInfo_, appInfo_);
    auto missionRecord = std::make_shared<MissionRecord>(abilityInfo_.bundleName);
    missionRecord->AddAbilityRecordToTop(ability);
    EXPECT_EQ(1, missionRecord->GetAbilityRecordCount());
    EXPECT_EQ(false, missionRecord->RemoveAbilityRecord(ability2));
}

/*
 * Feature: MissionRecord
 * Function: RemoveAll
 * SubFunction: NA
 * FunctionPoints: MissionRecord RemoveAll
 * EnvConditions:NA
 * CaseDescription: mission record reomve all, Verify count
 */
HWTEST_F(MissionRecordTest, stack_operating_008, TestSize.Level1)
{
    auto ability = std::make_shared<AbilityRecord>(want_, abilityInfo_, appInfo_);
    auto ability2 = std::make_shared<AbilityRecord>(want_, abilityInfo_, appInfo_);
    auto missionRecord = std::make_shared<MissionRecord>(abilityInfo_.bundleName);
    missionRecord->AddAbilityRecordToTop(ability);
    missionRecord->AddAbilityRecordToTop(ability2);
    EXPECT_EQ(2, missionRecord->GetAbilityRecordCount());
    missionRecord->RemoveAll();
    EXPECT_EQ(0, missionRecord->GetAbilityRecordCount());
}

/*
 * Feature: MissionRecord
 * Function: GetBottomAbilityRecord
 * SubFunction: NA
 * FunctionPoints: MissionRecord GetBottomAbilityRecord
 * EnvConditions:NA
 * CaseDescription: Verify GetBottomAbilityRecord value
 */
HWTEST_F(MissionRecordTest, stack_operating_009, TestSize.Level1)
{
    auto ability = std::make_shared<AbilityRecord>(want_, abilityInfo_, appInfo_);
    auto missionRecord = std::make_shared<MissionRecord>();
    EXPECT_EQ(nullptr, missionRecord->GetBottomAbilityRecord());

    missionRecord->AddAbilityRecordToTop(ability);
    EXPECT_EQ(ability, missionRecord->GetBottomAbilityRecord());
    EXPECT_EQ(ability, missionRecord->GetTopAbilityRecord());
}

/*
 * Feature: MissionRecord
 * Function: GetAbilityRecordByToken
 * SubFunction: NA
 * FunctionPoints: MissionRecord GetAbilityRecordByToken
 * EnvConditions:NA
 * CaseDescription: Verify GetAbilityRecordByToken value
 */
HWTEST_F(MissionRecordTest, stack_operating_010, TestSize.Level1)
{
    std::string deviceName = "device";
    std::string abilityName = "ServiceAbility";
    std::string appName = "hiservcie";
    std::string bundleName = "com.ix.hiservcie";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = record->GetToken();

    auto missionRecord = std::make_shared<MissionRecord>();
    EXPECT_EQ(nullptr, missionRecord->GetAbilityRecordByToken(token));
    missionRecord->AddAbilityRecordToTop(record);
    EXPECT_EQ(record, missionRecord->GetAbilityRecordByToken(token));
}

/*
 * Feature: MissionRecord
 * Function: RemoveTopAbilityRecord
 * SubFunction: NA
 * FunctionPoints: MissionRecord RemoveTopAbilityRecord
 * EnvConditions:NA
 * CaseDescription: Verify RemoveTopAbilityRecord operation
 */
HWTEST_F(MissionRecordTest, stack_operating_011, TestSize.Level1)
{
    auto missionRecord = std::make_shared<MissionRecord>();
    EXPECT_EQ(false, missionRecord->RemoveTopAbilityRecord());

    auto ability = std::make_shared<AbilityRecord>(want_, abilityInfo_, appInfo_);
    missionRecord->AddAbilityRecordToTop(ability);
    EXPECT_EQ(true, missionRecord->RemoveTopAbilityRecord());
}

/*
 * Feature: MissionRecord
 * Function: RemoveTopAbilityRecord
 * SubFunction: NA
 * FunctionPoints: MissionRecord RemoveTopAbilityRecord
 * EnvConditions:NA
 * CaseDescription: Verify RemoveTopAbilityRecord operation
 */
HWTEST_F(MissionRecordTest, stack_operating_012, TestSize.Level1)
{
    std::string deviceName = "device";
    std::string abilityName = "ServiceAbility";
    std::string appName = "hiservcie";
    std::string bundleName = "com.ix.hiservcie";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto missionRecord = std::make_shared<MissionRecord>();
    missionRecord->AddAbilityRecordToTop(record);
    std::vector<std::string> info;
    info.push_back(std::string("0"));
    missionRecord->Dump(info);

    EXPECT_NE(info.end(), std::find_if(info.begin(), info.end(), [](std::string &it) {
        return std::string::npos != it.find("hiservcie");
    }));
}

/*
 * Feature: MissionRecord
 * Function: NA
 * SubFunction: NA
 * FunctionPoints: SetPreMissionRecord GetPreMissionRecord
 * EnvConditions:NA
 * CaseDescription: SetPreMissionRecord GetPreMissionRecord UT.
 */
HWTEST_F(MissionRecordTest, stack_operating_013, TestSize.Level1)
{
    auto missionRecordPre = std::make_shared<MissionRecord>();
    auto missionRecordTarget = std::make_shared<MissionRecord>();
    missionRecordTarget->SetPreMissionRecord(missionRecordPre);
    EXPECT_EQ(missionRecordTarget->GetPreMissionRecord().get(), missionRecordPre.get());
}

/*
 * Feature: MissionRecord
 * Function: NA
 * SubFunction: NA
 * FunctionPoints: SetIsLauncherCreate IsLauncherCreate
 * EnvConditions:NA
 * CaseDescription: SetIsLauncherCreate IsLauncherCreate UT.
 */
HWTEST_F(MissionRecordTest, stack_operating_014, TestSize.Level1)
{
    auto missionRecord = std::make_shared<MissionRecord>();
    missionRecord->SetIsLauncherCreate();
    EXPECT_EQ(true, missionRecord->IsLauncherCreate());
}
}  // namespace AAFwk
}  // namespace OHOS