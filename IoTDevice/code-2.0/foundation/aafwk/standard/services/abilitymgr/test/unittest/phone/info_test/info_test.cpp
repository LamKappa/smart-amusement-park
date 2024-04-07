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
#include "stack_info.h"
#include "lifecycle_state_info.h"
#include "image_info.h"
#include "mission_snapshot_info.h"
#include "mission_description_info.h"
#include "recent_mission_info.h"

using namespace testing::ext;

namespace OHOS {
namespace AAFwk {
class InfoTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    StackInfo Stackinfo_;
    MissionStackInfo missionStackInfo_;
    MissionRecordInfo missionRecordInfo_;
    AbilityRecordInfo abilityRecordInfo_;
    LifeCycleStateInfo lifeCycleStateInfo_;
    ImageInfo imageInfo_;
    ImageHeader imageHeader_;
    MissionSnapshotInfo missionSnapshotInfo_;
    MissionDescriptionInfo missionDescriptionInfo_;
    RecentMissionInfo recentMissionInfo_;
};

void InfoTest::SetUpTestCase(void)
{}
void InfoTest::TearDownTestCase(void)
{}
void InfoTest::SetUp()
{}
void InfoTest::TearDown()
{}

/*
 * Feature: StackInfo
 * Function: ReadFromParcel and Marshalling and Unmarshalling
 * SubFunction: NA
 * FunctionPoints: StackInfo ReadFromParcel and Marshalling and Unmarshalling
 * EnvConditions:NA
 * CaseDescription: The process of verifying stackenfo parcel
 */
HWTEST_F(InfoTest, stack_info_oprator_001, TestSize.Level0)
{
    MissionStackInfo missionInfo;
    missionInfo.id = 10;
    Stackinfo_.missionStackInfos.emplace_back(missionInfo);
    Parcel parcel;
    Stackinfo_.Marshalling(parcel);
    StackInfo info;
    StackInfo *obj = info.Unmarshalling(parcel);
    EXPECT_TRUE(obj != nullptr);

    if (!obj->missionStackInfos.empty()) {
        EXPECT_EQ(obj->missionStackInfos[0].id, missionInfo.id);
    }
}

/*
 * Feature: MissionStackInfo
 * Function: ReadFromParcel and Marshalling and Unmarshalling
 * SubFunction: NA
 * FunctionPoints: MissionStackInfo ReadFromParcel and Marshalling and Unmarshalling
 * EnvConditions:NA
 * CaseDescription: The process of verifying MissionStackInfo parcel
 */
HWTEST_F(InfoTest, stack_info_oprator_002, TestSize.Level0)
{
    MissionRecordInfo info;
    info.id = 1;
    missionStackInfo_.id = 10;
    missionStackInfo_.missionRecords.emplace_back(info);
    Parcel parcel;
    missionStackInfo_.Marshalling(parcel);
    MissionStackInfo *obj = missionStackInfo_.Unmarshalling(parcel);
    ASSERT_TRUE(obj);
    EXPECT_EQ(obj->id, missionStackInfo_.id);
    EXPECT_EQ(obj->missionRecords[0].id, info.id);
}

/*
 * Feature: MissionRecordInfo
 * Function: ReadFromParcel and Marshalling and Unmarshalling
 * SubFunction: NA
 * FunctionPoints: MissionRecordInfo ReadFromParcel and Marshalling and Unmarshalling
 * EnvConditions:NA
 * CaseDescription: The process of verifying MissionRecordInfo parcel
 */
HWTEST_F(InfoTest, stack_info_oprator_003, TestSize.Level0)
{
    AbilityRecordInfo info;
    info.id = 10;
    info.elementName = "test";
    info.appName = "app";
    info.mainName = "main";
    info.abilityType = 3;
    info.previousAppName = "preApp";
    info.previousMainName = "preMain";
    info.nextAppName = "nextApp";
    info.nextMainName = "nextMain";
    info.state = AbilityState::BACKGROUND;
    info.startTime = "1:00";
    info.ready = true;
    info.windowAttached = true;
    info.lanucher = true;
    missionRecordInfo_.id = 10;
    missionRecordInfo_.abilityRecordInfos.emplace_back(info);
    Parcel parcel;
    missionRecordInfo_.Marshalling(parcel);
    MissionRecordInfo *obj = missionRecordInfo_.Unmarshalling(parcel);
    ASSERT_TRUE(obj);
    EXPECT_EQ(obj->id, missionRecordInfo_.id);
    EXPECT_EQ(obj->abilityRecordInfos[0].id, info.id);
    EXPECT_EQ(obj->abilityRecordInfos[0].elementName, info.elementName);
    EXPECT_EQ(obj->abilityRecordInfos[0].appName, info.appName);
    EXPECT_EQ(obj->abilityRecordInfos[0].mainName, info.mainName);
    EXPECT_EQ(obj->abilityRecordInfos[0].abilityType, info.abilityType);
    EXPECT_EQ(obj->abilityRecordInfos[0].previousAppName, info.previousAppName);
    EXPECT_EQ(obj->abilityRecordInfos[0].previousMainName, info.previousMainName);
    EXPECT_EQ(obj->abilityRecordInfos[0].nextAppName, info.nextAppName);
    EXPECT_EQ(obj->abilityRecordInfos[0].nextMainName, info.nextMainName);
    EXPECT_EQ(obj->abilityRecordInfos[0].state, info.state);
    EXPECT_EQ(obj->abilityRecordInfos[0].startTime, info.startTime);
    EXPECT_EQ(obj->abilityRecordInfos[0].ready, info.ready);
    EXPECT_EQ(obj->abilityRecordInfos[0].windowAttached, info.windowAttached);
    EXPECT_EQ(obj->abilityRecordInfos[0].lanucher, info.lanucher);
}

/*
 * Feature: AbilityRecordInfo
 * Function: ReadFromParcel and Marshalling and Unmarshalling
 * SubFunction: NA
 * FunctionPoints: AbilityRecordInfo ReadFromParcel and Marshalling and Unmarshalling
 * EnvConditions:NA
 * CaseDescription: The process of verifying AbilityRecordInfo parcel
 */
HWTEST_F(InfoTest, stack_info_oprator_004, TestSize.Level0)
{
    abilityRecordInfo_.id = 10;
    abilityRecordInfo_.elementName = "test";
    abilityRecordInfo_.appName = "app";
    abilityRecordInfo_.mainName = "main";
    abilityRecordInfo_.abilityType = 3;
    abilityRecordInfo_.previousAppName = "preApp";
    abilityRecordInfo_.previousMainName = "preMain";
    abilityRecordInfo_.nextAppName = "nextApp";
    abilityRecordInfo_.nextMainName = "nextMain";
    abilityRecordInfo_.state = AbilityState::BACKGROUND;
    abilityRecordInfo_.startTime = "1:00";
    abilityRecordInfo_.ready = true;
    abilityRecordInfo_.windowAttached = true;
    abilityRecordInfo_.lanucher = true;
    Parcel parcel;
    abilityRecordInfo_.Marshalling(parcel);
    AbilityRecordInfo *obj = abilityRecordInfo_.Unmarshalling(parcel);
    ASSERT_TRUE(obj);
   
    EXPECT_EQ(obj->id, abilityRecordInfo_.id);
    EXPECT_EQ(obj->elementName, abilityRecordInfo_.elementName);
    EXPECT_EQ(obj->appName, abilityRecordInfo_.appName);
    EXPECT_EQ(obj->mainName, abilityRecordInfo_.mainName);
    EXPECT_EQ(obj->abilityType, abilityRecordInfo_.abilityType);
    EXPECT_EQ(obj->previousAppName, abilityRecordInfo_.previousAppName);
    EXPECT_EQ(obj->previousMainName, abilityRecordInfo_.previousMainName);
    EXPECT_EQ(obj->nextAppName, abilityRecordInfo_.nextAppName);
    EXPECT_EQ(obj->nextMainName, abilityRecordInfo_.nextMainName);
    EXPECT_EQ(obj->state, abilityRecordInfo_.state);
    EXPECT_EQ(obj->startTime, abilityRecordInfo_.startTime);
    EXPECT_EQ(obj->ready, abilityRecordInfo_.ready);
    EXPECT_EQ(obj->windowAttached, abilityRecordInfo_.windowAttached);
    EXPECT_EQ(obj->lanucher, abilityRecordInfo_.lanucher);
}

/*
 * Feature: LifeCycleStateInfo
 * Function: ReadFromParcel and Marshalling and Unmarshalling
 * SubFunction: NA
 * FunctionPoints: LifeCycleStateInfo ReadFromParcel and Marshalling and Unmarshalling
 * EnvConditions:NA
 * CaseDescription: The process of verifying LifeCycleStateInfo parcel
 */
HWTEST_F(InfoTest, stack_info_oprator_005, TestSize.Level0)
{
    lifeCycleStateInfo_.isNewWant = 10;
    lifeCycleStateInfo_.state = AbilityLifeCycleState::ABILITY_STATE_BACKGROUND;
    Parcel parcel;
    lifeCycleStateInfo_.Marshalling(parcel);
    LifeCycleStateInfo *obj = lifeCycleStateInfo_.Unmarshalling(parcel);
    ASSERT_TRUE(obj);

    EXPECT_EQ(obj->isNewWant, lifeCycleStateInfo_.isNewWant);
    EXPECT_EQ(obj->state, lifeCycleStateInfo_.state);
}

/*
 * Feature: ImageHeader
 * Function: ReadFromParcel and Marshalling and Unmarshalling
 * SubFunction: NA
 * FunctionPoints: ImageHeader ReadFromParcel and Marshalling and Unmarshalling
 * EnvConditions:NA
 * CaseDescription: The process of verifying ImageHeader parcel
 */
HWTEST_F(InfoTest, stack_info_oprator_007, TestSize.Level0)
{
    imageHeader_.colorMode = 8;
    imageHeader_.reserved = 24;
    imageHeader_.width = 10;
    imageHeader_.height = 10;
    Parcel parcel;
    imageHeader_.Marshalling(parcel);
    ImageHeader *obj = imageHeader_.Unmarshalling(parcel);
    ASSERT_TRUE(obj);
    EXPECT_EQ(obj->colorMode, imageHeader_.colorMode);
    EXPECT_EQ(obj->reserved, imageHeader_.reserved);
    EXPECT_EQ(obj->width, imageHeader_.width);
    EXPECT_EQ(obj->height, imageHeader_.height);
}

/*
 * Feature: MissionDescriptionInfo
 * Function: ReadFromParcel and Marshalling and Unmarshalling
 * SubFunction: NA
 * FunctionPoints: MissionDescriptionInfo ReadFromParcel and Marshalling and Unmarshalling
 * EnvConditions:NA
 * CaseDescription: The process of verifying MissionDescriptionInfo parcel
 */
HWTEST_F(InfoTest, stack_info_oprator_008, TestSize.Level0)
{
    missionDescriptionInfo_.label = "label";
    missionDescriptionInfo_.iconPath = "iconpath";
    Parcel parcel;
    missionDescriptionInfo_.Marshalling(parcel);
    MissionDescriptionInfo *obj = missionDescriptionInfo_.Unmarshalling(parcel);
    EXPECT_EQ(obj->label, missionDescriptionInfo_.label);
    EXPECT_EQ(obj->iconPath, missionDescriptionInfo_.iconPath);
}

/*
 * Feature: RecentMissionInfo
 * Function: ReadFromParcel and Marshalling and Unmarshalling
 * SubFunction: NA
 * FunctionPoints: RecentMissionInfo ReadFromParcel and Marshalling and Unmarshalling
 * EnvConditions:NA
 * CaseDescription: The process of verifying RecentMissionInfo parcel
 */
HWTEST_F(InfoTest, stack_info_oprator_009, TestSize.Level0)
{
    recentMissionInfo_.id = 10;
    recentMissionInfo_.runingState = -1;
    recentMissionInfo_.size = -1;
    MissionDescriptionInfo missionDescriptionInfo;
    missionDescriptionInfo.label = "label";
    missionDescriptionInfo.iconPath = "iconpath";
    recentMissionInfo_.missionDescription = missionDescriptionInfo;
    Parcel parcel;
    recentMissionInfo_.Marshalling(parcel);
    RecentMissionInfo *obj = recentMissionInfo_.Unmarshalling(parcel);
    ASSERT_TRUE(obj);
    EXPECT_EQ(obj->id, recentMissionInfo_.id);
    EXPECT_EQ(obj->runingState, recentMissionInfo_.runingState);
    EXPECT_EQ(obj->size, recentMissionInfo_.size);
    EXPECT_EQ(obj->missionDescription.label, missionDescriptionInfo.label);
}
}  // namespace AAFwk
}  // namespace OHOS