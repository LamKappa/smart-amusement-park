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
#include "ability_stack_manager.h"
#include "ability_record.h"
#include "ability_manager_service.h"
#undef private
#undef protected

#include "mock_bundle_manager.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"
#include "ability_manager_errors.h"
#include "ability_scheduler.h"
#include "mock_ability_connect_callback.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
namespace OHOS {
namespace AAFwk {
class AbilityStackManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void init();

    AbilityRequest GenerateAbilityRequest(const std::string &deviceName, const std::string &abilityName,
        const std::string &appName, const std::string &bundleName);

    void makeScene(const std::string &abilityName, const std::string &bundleName, AbilityInfo &abilityInfo, Want &want);

    std::shared_ptr<AbilityStackManager> stackManager_;
    AbilityRequest launcherAbilityRequest_;
    AbilityRequest musicAbilityRequest_;
    AbilityRequest musicTopAbilityRequest_;
    AbilityRequest musicSAbilityRequest_;
    AbilityRequest radioAbilityRequest_;
    AbilityRequest radioTopAbilityRequest_;
};

void AbilityStackManagerTest::SetUpTestCase(void)
{}
void AbilityStackManagerTest::TearDownTestCase(void)
{}
void AbilityStackManagerTest::TearDown()
{}

void AbilityStackManagerTest::SetUp()
{
    stackManager_ = std::make_shared<AbilityStackManager>(0);
    init();
}

void AbilityStackManagerTest::init()
{
    launcherAbilityRequest_ = GenerateAbilityRequest("device", "LauncherAbility", "launcher", "com.ix.hiworld");

    musicAbilityRequest_ = GenerateAbilityRequest("device", "MusicAbility", "music", "com.ix.hiMusic");

    musicTopAbilityRequest_ = GenerateAbilityRequest("device", "MusicTopAbility", "music", "com.ix.hiMusic");

    musicSAbilityRequest_ = GenerateAbilityRequest("device", "MusicSAbility", "music", "com.ix.hiMusic");

    radioAbilityRequest_ = GenerateAbilityRequest("device", "RadioAbility", "radio", "com.ix.hiRadio");

    radioTopAbilityRequest_ = GenerateAbilityRequest("device", "RadioTopAbility", "radio", "com.ix.hiRadio");
}

void AbilityStackManagerTest::makeScene(
    const std::string &abilityName, const std::string &bundleName, AbilityInfo &abilityInfo, Want &want)
{
    if (bundleName == "com.ix.hiworld") {
        std::string entity = Want::ENTITY_HOME;
        want.AddEntity(entity);
        abilityInfo.type = AbilityType::PAGE;
        abilityInfo.applicationInfo.isLauncherApp = true;
        abilityInfo.process = "p";
    }

    if (bundleName == "com.ix.hiMusic") {
        abilityInfo.type = AbilityType::PAGE;
        abilityInfo.applicationInfo.isLauncherApp = false;

        if (abilityName == "MusicAbility") {
            abilityInfo.process = "p1";
            abilityInfo.launchMode = LaunchMode::STANDARD;
        }
        if (abilityName == "MusicTopAbility") {
            abilityInfo.process = "p1";
            abilityInfo.launchMode = LaunchMode::SINGLETOP;
        }
        if (abilityName == "MusicSAbility") {
            abilityInfo.process = "p2";
            abilityInfo.launchMode = LaunchMode::SINGLETON;
        }
    }

    if (bundleName == "com.ix.hiRadio") {
        abilityInfo.type = AbilityType::PAGE;
        abilityInfo.process = "p3";
        if (abilityName == "RadioAbility") {
            abilityInfo.launchMode = LaunchMode::STANDARD;
        }
        if (abilityName == "RadioTopAbility") {
            abilityInfo.launchMode = LaunchMode::SINGLETON;
        }
    }
}

AbilityRequest AbilityStackManagerTest::GenerateAbilityRequest(const std::string &deviceName,
    const std::string &abilityName, const std::string &appName, const std::string &bundleName)
{
    ElementName element(deviceName, abilityName, bundleName);
    Want want;
    want.SetElement(element);

    AbilityInfo abilityInfo;
    ApplicationInfo appinfo;

    abilityInfo.name = abilityName;
    abilityInfo.bundleName = bundleName;
    abilityInfo.applicationName = appName;
    abilityInfo.applicationInfo.bundleName = bundleName;
    abilityInfo.applicationInfo.name = appName;

    makeScene(abilityName, bundleName, abilityInfo, want);

    appinfo = abilityInfo.applicationInfo;
    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = abilityInfo;
    abilityRequest.appInfo = appinfo;

    return abilityRequest;
}
/*
 * Feature: AbilityStackManager
 * Function: stack operate
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify get ability by token success
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_001, TestSize.Level0)
{
    stackManager_->Init();
    stackManager_->StartAbility(launcherAbilityRequest_);
    auto topAbility = stackManager_->GetCurrentTopAbility();
    auto token = topAbility->GetToken();
    auto tokenAbility = stackManager_->GetAbilityRecordByToken(token);
    EXPECT_EQ(topAbility, tokenAbility);
}

/*
 * Feature: AbilityStackManager
 * Function: stack operate
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify get ability by token fail
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_002, TestSize.Level0)
{
    Want want;
    AbilityInfo abilityInfo;
    ApplicationInfo appinfo;

    stackManager_->Init();
    stackManager_->StartAbility(launcherAbilityRequest_);

    auto ability = std::make_shared<AbilityRecord>(want, abilityInfo, appinfo);
    ability->Init();
    auto token = ability->GetToken();
    auto tokenAbility = stackManager_->GetAbilityRecordByToken(token);
    EXPECT_EQ(nullptr, tokenAbility);
}

/*
 * Feature: AbilityStackManager
 * Function: stack operate
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify start launcher ability
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_003, TestSize.Level0)
{
    stackManager_->Init();
    auto topAbility = stackManager_->GetCurrentTopAbility();
    EXPECT_EQ(nullptr, topAbility);

    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(result, ERR_OK);
    topAbility = stackManager_->GetCurrentTopAbility();
    EXPECT_NE(nullptr, stackManager_->GetCurrentTopAbility());
    AbilityInfo topAbilityInfo = topAbility->GetAbilityInfo();
    EXPECT_EQ("launcher", topAbilityInfo.applicationName);
}

/*
 * Feature: AbilityStackManager
 * Function: stack operate
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify repeated start launcher ability
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_004, TestSize.Level0)
{
    stackManager_->Init();
    stackManager_->StartAbility(musicSAbilityRequest_);
    auto topAbility1 = stackManager_->GetCurrentTopAbility();
    topAbility1->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    stackManager_->StartAbility(musicSAbilityRequest_);
    auto topAbility2 = stackManager_->GetCurrentTopAbility();
    topAbility2->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(topAbility1, topAbility2);
}

/*
 * Feature: AbilityStackManager
 * Function: stack operate
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify no launcher ability GetTopMissionRecord
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_005, TestSize.Level0)
{
    stackManager_->Init();
    auto topMissionRecord = stackManager_->GetTopMissionRecord();
    EXPECT_EQ(nullptr, topMissionRecord);
}

/*
 * Feature: AbilityStackManager
 * Function: stack operate
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify no launcher ability GetCurrentTopAbility
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_006, TestSize.Level0)
{
    stackManager_->Init();
    auto topAbility = stackManager_->GetCurrentTopAbility();
    EXPECT_EQ(nullptr, topAbility);
}

/*
 * Feature: AbilityStackManager
 * Function: stack operate
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify repeated start launcher ability, GetTopMissionRecord
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_007, TestSize.Level0)
{
    stackManager_->Init();

    stackManager_->StartAbility(launcherAbilityRequest_);
    auto topMissionRecord1 = stackManager_->GetTopMissionRecord();
    auto topAbility1 = stackManager_->GetCurrentTopAbility();
    topAbility1->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_NE(nullptr, topMissionRecord1);

    stackManager_->StartAbility(musicAbilityRequest_);
    auto topMissionRecord2 = stackManager_->GetTopMissionRecord();
    auto topAbility2 = stackManager_->GetCurrentTopAbility();
    topAbility2->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_NE(nullptr, topMissionRecord2);
    EXPECT_NE(topMissionRecord1->GetMissionRecordId(), topMissionRecord2->GetMissionRecordId());
}

/*
 * Feature: AbilityStackManager
 * Function: stack operate
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify launcher ability RemoveMissionRecordById
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_008, TestSize.Level0)
{
    stackManager_->Init();
    stackManager_->StartAbility(launcherAbilityRequest_);
    auto topAbility1 = stackManager_->GetCurrentTopAbility();
    EXPECT_NE(nullptr, topAbility1);
    auto topMissionRecord1 = stackManager_->GetTopMissionRecord();
    EXPECT_NE(nullptr, topMissionRecord1);
    auto topId = topMissionRecord1->GetMissionRecordId();
    bool ret = stackManager_->RemoveMissionRecordById(topId);
    EXPECT_EQ(true, ret);
    auto topAbility2 = stackManager_->GetCurrentTopAbility();
    EXPECT_EQ(nullptr, topAbility2);
    auto topMissionRecord2 = stackManager_->GetTopMissionRecord();
    EXPECT_EQ(nullptr, topMissionRecord2);
}

/*
 * Feature: AbilityStackManager
 * Function: StartAbility
 * SubFunction: LoadFirstAbility
 * FunctionPoints: NA
 * EnvConditions: top ability is null
 * CaseDescription: start the first ability. verify:
 *                   1. the MissionStack is launcher mission stack
 *                   2. the mission record count is 1.
 *                   3. the ability record count is 1.
 *                   4. the result of StartAbility is ERRO_OK.
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_009, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    auto missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(0, missionStack->GetMissionStackId());
    EXPECT_EQ(1, missionStack->GetMissionRecordCount());
    auto missionRecord = missionStack->GetTopMissionRecord();
    ASSERT_TRUE(missionRecord != nullptr);
    EXPECT_EQ(1, missionRecord->GetAbilityRecordCount());
    auto topAbility = stackManager_->GetCurrentTopAbility();
    auto realAppinfo = topAbility->GetApplicationInfo();
    EXPECT_EQ("launcher", realAppinfo.name);
    EXPECT_EQ(0, result);
}

/*
 * Feature: AbilityStackManager
 * Function: StartAbility
 * SubFunction: LoadFirstAbility
 * FunctionPoints: NA
 * EnvConditions: top ability is null
 * CaseDescription: start the first ability not belong to launcher. verify:
 *                   1. the MissionStack is common app mission stack
 *                   2. the mission record count is 1.
 *                   3. the ability record count is 1.
 *                   4. the result of StartAbility is ERRO_OK.
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_010, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    auto missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(0, missionStack->GetMissionStackId());
    EXPECT_EQ(1, missionStack->GetMissionRecordCount());
    auto missionRecord = missionStack->GetTopMissionRecord();
    EXPECT_EQ(1, missionRecord->GetAbilityRecordCount());
    auto topAbility = stackManager_->GetCurrentTopAbility();
    auto realAppinfo = topAbility->GetApplicationInfo();
    EXPECT_EQ("launcher", realAppinfo.name);
    EXPECT_EQ(0, result);
}

/*
 * Feature: AbilityStackManager
 * Function: StartAbility
 * SubFunction: MoveMissionStackToTop, LoadFirstAbility, LoadAbility ChooseMissionRecord, Inactivate
 * FunctionPoints: NA
 * EnvConditions: top ability is not null, and top is launcher
 * CaseDescription: start the second ability belong to launcher. verify:
 *                   1. the MissionStack is launcher mission stack
 *                   2. the mission record count is 1.
 *                   3. the ability record count is 2.
 *                   4. the result of StartAbility is ERRO_OK.
 *                   5. the state of top ability is INACTIVATING.
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_011, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(0, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(0, result);

    // verify
    auto missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(1, missionStack->GetMissionStackId());
    EXPECT_EQ(1, missionStack->GetMissionRecordCount());
    auto missionRecord = missionStack->GetTopMissionRecord();
    EXPECT_EQ(1, missionRecord->GetAbilityRecordCount());
    auto realAbilityinfo = secondTopAbility->GetAbilityInfo();
    EXPECT_EQ("MusicAbility", realAbilityinfo.name);

    auto state = firstTopAbility->GetAbilityState();
    EXPECT_EQ(OHOS::AAFwk::INACTIVATING, state);
}

/*
 * Feature: AbilityStackManager
 * Function: StartAbility
 * SubFunction: MoveMissionStackToTop, LoadFirstAbility, LoadAbility ChooseMissionRecord, Inactivate
 * FunctionPoints: NA
 * EnvConditions: top ability is not null, and top is launcher
 * CaseDescription: start the second ability not belong to launcher. verify:
 *                   1. the MissionStack is common app mission stack
 *                   2. the mission record count is 1.
 *                   3. the ability record count is 1.
 *                   4. the result of StartAbility is ERRO_OK.
 *                   5. the state of top ability is INACTIVATING.
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_012, TestSize.Level0)
{
    stackManager_->Init();

    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(0, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(0, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(1, missionStack->GetMissionStackId());
    EXPECT_EQ(1, missionStack->GetMissionRecordCount());
    auto missionRecord = missionStack->GetTopMissionRecord();
    EXPECT_EQ(1, missionRecord->GetAbilityRecordCount());
    auto realAppinfo = secondTopAbility->GetApplicationInfo();
    EXPECT_EQ("music", realAppinfo.name);

    auto state = firstTopAbility->GetAbilityState();
    EXPECT_EQ(OHOS::AAFwk::INACTIVATING, state);
}

/*
 * Feature: AbilityStackManager
 * Function: StartAbility
 * SubFunction: MoveMissionStackToTop, LoadFirstAbility, LoadAbility ChooseMissionRecord, Inactivate
 * FunctionPoints: NA
 * EnvConditions: top ability is not null, and top is launcher
 * CaseDescription: resort the first ability. verify:
 *                   1. the MissionStack is common app mission stack
 *                   2. the mission record count is 1.
 *                   3. the ability record count is 2.
 *                   4. the result of StartAbility is ERRO_OK.
 *                   5. the name of top ability is "secondAbility".
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_013, TestSize.Level0)
{
    // start first ability not belong to launcher.
    stackManager_->Init();
    auto result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(0, result);
    auto firstAbility = stackManager_->GetCurrentTopAbility();
    firstAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // start second ability not belong to launcher.
    result = stackManager_->StartAbility(musicTopAbilityRequest_);
    EXPECT_EQ(0, result);
    auto secondAbility = stackManager_->GetCurrentTopAbility();
    secondAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // start launcher ability.
    result = stackManager_->StartAbility(launcherAbilityRequest_);
    auto missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(0, missionStack->GetMissionStackId());
    EXPECT_EQ(0, result);
    OHOS::AAFwk::AbilityState state = secondAbility->GetAbilityState();
    EXPECT_EQ(OHOS::AAFwk::INACTIVATING, state);

    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // restart first ability not belong to launcher.
    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(0, result);
    topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // verify
    missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(1, missionStack->GetMissionStackId());
    EXPECT_EQ(1, missionStack->GetMissionRecordCount());
    auto missionRecord = missionStack->GetTopMissionRecord();
    EXPECT_EQ(2, missionRecord->GetAbilityRecordCount());
    topAbility = stackManager_->GetCurrentTopAbility();
    auto realAbilityInfo = topAbility->GetAbilityInfo();
    EXPECT_EQ("MusicTopAbility", realAbilityInfo.name);
}

/*
 * Feature: AbilityStackManager
 * Function: StartAbility
 * SubFunction: MoveMissionStackToTop, LoadFirstAbility, LoadAbility ChooseMissionRecord, Inactivate
 * FunctionPoints: NA
 * EnvConditions: top ability is not null, and top is not launcher
 * CaseDescription: start a new ability from the common app first ability.verify:
 *                   1. the MissionStack is common app mission stack
 *                   2. the mission record count is 1.
 *                   3. the ability record count is 2.
 *                   4. the result of StartAbility is ERRO_OK.
 *                   5. the name of top ability is "secondAbility".
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_014, TestSize.Level0)
{
    // start launcher ability
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(0, result);
    auto launcherTopAbility = stackManager_->GetCurrentTopAbility();
    launcherTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // start common app the first ability from launcher
    result = stackManager_->StartAbility(musicAbilityRequest_);
    auto missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(1, missionStack->GetMissionStackId());
    EXPECT_EQ(0, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    auto state = launcherTopAbility->GetAbilityState();
    EXPECT_EQ(OHOS::AAFwk::INACTIVATING, state);

    // start the second ability from the common first ability
    result = stackManager_->StartAbility(musicTopAbilityRequest_);
    EXPECT_EQ(0, result);
    auto thirdTopAbility = stackManager_->GetCurrentTopAbility();
    thirdTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // verify
    missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(1, missionStack->GetMissionStackId());
    EXPECT_EQ(1, missionStack->GetMissionRecordCount());
    auto missionRecord = missionStack->GetTopMissionRecord();
    EXPECT_EQ(2, missionRecord->GetAbilityRecordCount());
    state = secondTopAbility->GetAbilityState();
    EXPECT_EQ(OHOS::AAFwk::INACTIVATING, state);
    auto realAbilityInfo = thirdTopAbility->GetAbilityInfo();
    EXPECT_EQ("MusicTopAbility", realAbilityInfo.name);
}

/*
 * Feature: AbilityStackManager
 * Function: StartAbility
 * SubFunction: MoveMissionStackToTop, LoadFirstAbility, LoadAbility ChooseMissionRecord, Inactivate
 * FunctionPoints: NA
 * EnvConditions: top ability is not null, and top is not launcher
 * CaseDescription: start launcher ability from the common app ability.verify:
 *                   1. the MissionStack is launcher mission stack
 *                   2. the mission record count is 1.
 *                   3. the ability record count is 1.
 *                   4. the result of StartAbility is ERRO_OK.
 *                   5. the name of top ability is "MainAbility".
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_015, TestSize.Level0)
{
    // start common app the first ability from launcher
    stackManager_->Init();

    int result = stackManager_->StartAbility(musicAbilityRequest_);
    auto missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(1, missionStack->GetMissionStackId());
    EXPECT_EQ(0, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // start the second ability from the common first ability
    result = stackManager_->StartAbility(musicTopAbilityRequest_);
    EXPECT_EQ(0, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // start launcher ability
    result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(0, result);
    auto thirdopAbility = stackManager_->GetCurrentTopAbility();
    thirdopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // verify
    missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(0, missionStack->GetMissionStackId());
    EXPECT_EQ(1, missionStack->GetMissionRecordCount());
    auto missionRecord = missionStack->GetTopMissionRecord();
    EXPECT_EQ(1, missionRecord->GetAbilityRecordCount());
    auto thirdTopAbility = stackManager_->GetCurrentTopAbility();
    auto realAbilityInfo = thirdTopAbility->GetAbilityInfo();
    EXPECT_EQ("LauncherAbility", realAbilityInfo.name);
    auto realAppInfo = thirdTopAbility->GetApplicationInfo();
    EXPECT_EQ("launcher", realAppInfo.name);
}

/*
 * Feature: AbilityStackManager
 * Function: StartAbility
 * SubFunction: MoveMissionStackToTop, LoadFirstAbility, LoadAbility ChooseMissionRecord, Inactivate
 * FunctionPoints: NA
 * EnvConditions: top ability is not null, and top is not launcher
 * CaseDescription: start two same ability.verify:
 *                   1. the MissionStack is common app mission stack
 *                   2. the mission record count is 1.
 *                   3. the ability record count is 2.
 *                   4. the result of StartAbility is ERRO_OK.
 *                   5. the name of top ability is "MainAbility".
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_016, TestSize.Level0)
{
    // start common app the first ability from launcher
    stackManager_->Init();
    auto result = stackManager_->StartAbility(musicAbilityRequest_);
    auto missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(1, missionStack->GetMissionStackId());
    EXPECT_EQ(0, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // start the same ability from the first ability
    result = stackManager_->StartAbility(radioAbilityRequest_);
    EXPECT_EQ(0, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // verify
    missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(1, missionStack->GetMissionStackId());
    EXPECT_EQ(1, missionStack->GetMissionRecordCount());
    auto missionRecord = missionStack->GetTopMissionRecord();
    EXPECT_EQ(2, missionRecord->GetAbilityRecordCount());
    auto topAbility = stackManager_->GetCurrentTopAbility();
    auto realAbilityInfo = topAbility->GetAbilityInfo();
    EXPECT_EQ("RadioAbility", realAbilityInfo.name);
    auto realAppInfo = topAbility->GetApplicationInfo();
    EXPECT_EQ("radio", realAppInfo.name);
}

/*
 * Feature: AbilityStackManager
 * Function: StartAbility
 * SubFunction: MoveMissionStackToTop, LoadFirstAbility, LoadAbility ChooseMissionRecord, Inactivate
 * FunctionPoints: NA
 * EnvConditions: top ability is not null, and top is not launcher
 * CaseDescription: start three ability. the bottom and the top are the same.
 *                   1. the MissionStack is common app mission stack
 *                   2. the mission record count is 1.
 *                   3. the ability record count is 3.
 *                   4. the result of StartAbility is ERRO_OK.
 *                   5. the name of top ability and bottom is "MainAbility".
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_017, TestSize.Level0)
{
    // start common app the first ability from launcher
    stackManager_->Init();
    auto result = stackManager_->StartAbility(musicAbilityRequest_);
    auto missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(1, missionStack->GetMissionStackId());
    EXPECT_EQ(0, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // start the second ability from the first ability
    result = stackManager_->StartAbility(musicTopAbilityRequest_);
    EXPECT_EQ(0, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // start the same ability with the first ability
    result = stackManager_->StartAbility(radioAbilityRequest_);
    EXPECT_EQ(0, result);
    auto thirdTopAbility = stackManager_->GetCurrentTopAbility();
    thirdTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // verify
    missionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(1, missionStack->GetMissionStackId());
    EXPECT_EQ(1, missionStack->GetMissionRecordCount());
    auto missionRecord = missionStack->GetTopMissionRecord();
    EXPECT_EQ(3, missionRecord->GetAbilityRecordCount());

    auto topAbility = stackManager_->GetCurrentTopAbility();
    auto realAbilityInfo = topAbility->GetAbilityInfo();
    EXPECT_EQ("RadioAbility", realAbilityInfo.name);

    auto realAppInfo = topAbility->GetApplicationInfo();
    EXPECT_EQ("radio", realAppInfo.name);

    auto bottomAbility = missionRecord->GetBottomAbilityRecord();
    EXPECT_EQ("RadioAbility", realAbilityInfo.name);
}

/*
 * Feature: AbilityStackManager
 * Function: GetAbilityStackManagerUserId
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify get user id value
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_018, TestSize.Level0)
{
    EXPECT_EQ(stackManager_->GetAbilityStackManagerUserId(), 0);
}

/*
 * Feature: AbilityStackManager
 * Function: StartAbility
 * SubFunction: NA
 * FunctionPoints: AbilityStackManager StartAbility
 * EnvConditions: NA
 * CaseDescription: launchMode is STANDARD, Startability and verify value
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_019, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(0, result);

    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto result1 = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(0, result1);

    auto topAbility1 = stackManager_->GetCurrentTopAbility();
    topAbility1->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto result2 = stackManager_->StartAbility(radioAbilityRequest_);
    EXPECT_EQ(0, result2);

    auto topAbility2 = stackManager_->GetCurrentTopAbility();
    topAbility2->SetAbilityState(OHOS::AAFwk::ACTIVE);

    std::vector<std::string> info;
    info.push_back("0");
    stackManager_->Dump(info);
    stackManager_->DumpStackList(info);
    info.push_back("1");
    stackManager_->DumpStack(stackManager_->GetTopMissionRecord()->GetMissionRecordId(), info);
    stackManager_->DumpMission(stackManager_->GetTopMissionRecord()->GetMissionRecordId(), info);
    stackManager_->DumpTopAbility(info);
    std::string s = "0";
    stackManager_->DumpWaittingAbilityQueue(s);

    EXPECT_NE(stackManager_->GetMissionRecordById(stackManager_->GetTopMissionRecord()->GetMissionRecordId()), nullptr);
    EXPECT_EQ(stackManager_->RemoveMissionRecordById(stackManager_->GetTopMissionRecord()->GetMissionRecordId()), true);
}

/*
 * Feature: AbilityStackManager
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: AbilityStackManager TerminateAbility
 * EnvConditions:NA
 * CaseDescription: 1. ability record is nullptr cause TerminateAbility failed
 *                  2. Verify TerminateAbility succeeded
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_020, TestSize.Level0)
{
    stackManager_->Init();

    auto result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(0, result);

    auto topAbility = stackManager_->GetCurrentTopAbility();
    auto want = topAbility->GetWant();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    std::shared_ptr<AbilityRecord> record = nullptr;
    auto nullToken = new Token(record);
    EXPECT_NE(0, stackManager_->TerminateAbility(nullToken, -1, &want));

    auto token = topAbility->GetToken();
    auto res = stackManager_->TerminateAbility(token, -1, &want);
    EXPECT_EQ(0, res);
}

/*
 * Feature: AbilityStackManager
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: AbilityStackManager TerminateAbility
 * EnvConditions:NA
 * CaseDescription: isTerminating_ is true cause TerminateAbility success
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_021, TestSize.Level0)
{
    stackManager_->Init();
    int result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(0, result);

    auto topAbility = stackManager_->GetCurrentTopAbility();
    auto want = topAbility->GetWant();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    topAbility->SetTerminatingState();
    auto token = topAbility->GetToken();
    auto res = stackManager_->TerminateAbility(token, -1, &want);
    EXPECT_EQ(0, res);
}

/*
 * Feature: AbilityStackManager
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Terminate other Ability cause fail
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_022, TestSize.Level0)
{
    stackManager_->Init();

    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(0, result);

    auto topAbility = stackManager_->GetCurrentTopAbility();
    auto want = topAbility->GetWant();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    std::string deviceName = "device";
    std::string abilityName = "otherAbility";
    std::string appName = "otherApp";
    std::string bundleName = "com.ix.other";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto nullToken = new Token(record);
    EXPECT_NE(0, stackManager_->TerminateAbility(nullToken, -1, &want));
}

/*
 * Feature: AbilityStackManager
 * Function: AbilityTransitionDone
 * SubFunction: NA
 * FunctionPoints: AbilityStackManager AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: handler is nullptr cause dispatchActive failed
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_023, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(0, result);

    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::INITIAL);
    auto token = topAbility->GetToken();
    EXPECT_NE(stackManager_->AbilityTransitionDone(token, OHOS::AAFwk::ACTIVE), 0);

    std::shared_ptr<AbilityRecord> record = nullptr;
    auto nullToken = new Token(record);
    EXPECT_NE(stackManager_->AbilityTransitionDone(nullToken, OHOS::AAFwk::INACTIVE), 0);

    auto token1 = topAbility->GetToken();
    EXPECT_NE(stackManager_->AbilityTransitionDone(token1, OHOS::AAFwk::BACKGROUND), 0);

    auto token2 = topAbility->GetToken();
    EXPECT_NE(stackManager_->AbilityTransitionDone(token2, OHOS::AAFwk::INITIAL), 0);

    auto token3 = topAbility->GetToken();
    EXPECT_NE(stackManager_->AbilityTransitionDone(token3, OHOS::AAFwk::TERMINATING), 0);

    auto token4 = topAbility->GetToken();
    EXPECT_NE(stackManager_->AbilityTransitionDone(token4, OHOS::AAFwk::INACTIVE), 0);
}

/*
 * Feature: AbilityStackManager
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: AbilityStackManager TerminateAbility
 * EnvConditions:NA
 * CaseDescription: start ability and remove ability record, verify terminal ability fail
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_024, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(0, result);

    auto topAbility = stackManager_->GetCurrentTopAbility();
    auto want = topAbility->GetWant();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    std::string deviceName = "device";
    std::string abilityName = "otherAbility";
    std::string appName = "otherApp";
    std::string bundleName = "com.ix.other";
    auto abilityReq = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    auto record = AbilityRecord::CreateAbilityRecord(abilityReq);
    auto token = topAbility->GetToken();
    stackManager_->GetTopMissionRecord()->RemoveTopAbilityRecord();
    auto res = stackManager_->TerminateAbility(token, -1, &want);
    EXPECT_NE(0, res);
}

/*
 * Feature: AbilityStackManager
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: AbilityStackManager TerminateAbility
 * EnvConditions:NA
 * CaseDescription: MissionRecord is nullptr cause TerminateAbility fail
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_025, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(0, result);

    auto topAbility = stackManager_->GetCurrentTopAbility();
    auto want = topAbility->GetWant();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto token = topAbility->GetToken();
    std::shared_ptr<MissionRecord> mission = nullptr;
    topAbility->SetMissionRecord(mission);
    auto res = stackManager_->TerminateAbility(token, -1, &want);
    EXPECT_NE(0, res);

    topAbility->isLauncherAbility_ = true;
}

/*
 * Feature: AbilityStackManager
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: AbilityStackManager TerminateAbility
 * EnvConditions:NA
 * CaseDescription: isLauncherAbility_ is true cause TerminateAbility failed
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_026, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(0, result);

    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto want = topAbility->GetWant();
    auto token = topAbility->GetToken();
    topAbility->isLauncherAbility_ = true;
    auto res = stackManager_->TerminateAbility(token, -1, &want);
    EXPECT_NE(0, res);
}

/*
 * Feature: AbilityStackManager
 * Function: GetTargetMissionStack
 * SubFunction: NA
 * FunctionPoints: AbilityStackManager GetTargetMissionStack
 * EnvConditions:NA
 * CaseDescription: Verify get target mission stack value
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_027, TestSize.Level0)
{
    Want want;
    want.AddEntity(Want::ENTITY_HOME);
    AbilityRequest request;
    EXPECT_EQ(stackManager_->launcherMissionStack_, stackManager_->GetTargetMissionStack(request));
    Want want1;
    AbilityRequest request1;
    want1.AddEntity(Want::ENTITY_VIDEO);
    EXPECT_EQ(stackManager_->defaultMissionStack_, stackManager_->GetTargetMissionStack(request1));
}

/*
 * Feature: AbilityStackManager
 * Function: AttachAbilityThread
 * SubFunction: NA
 * FunctionPoints: AbilityStackManager AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: handler is nullptr cause AttachAbilityThread fail
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_028, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(0, result);

    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::INITIAL);
    auto token = topAbility->GetToken();

    OHOS::sptr<IAbilityScheduler> scheduler = new AbilityScheduler();
    EXPECT_EQ(stackManager_->AttachAbilityThread(scheduler, token), ERR_INVALID_VALUE);

    std::shared_ptr<AbilityRecord> record = nullptr;
    auto nullToken = new Token(record);
    EXPECT_EQ(stackManager_->AttachAbilityThread(scheduler, nullToken), ERR_INVALID_VALUE);
}

/*
 * Feature: AbilityStackManager
 * Function: AddWindowInfo
 * SubFunction: NA
 * FunctionPoints: AbilityStackManager AddWindowInfo
 * EnvConditions:NA
 * CaseDescription: Verify AddWindowInfo operation
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_029, TestSize.Level0)
{
    stackManager_->Init();
    int result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(0, result);

    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::INITIAL);
    auto token = topAbility->GetToken();

    std::shared_ptr<AbilityRecord> nullAbility = nullptr;
    auto nullToken = new Token(nullAbility);
    stackManager_->AddWindowInfo(nullToken, 1);
    EXPECT_EQ(static_cast<int>(stackManager_->windowTokenToAbilityMap_.size()), 0);
    topAbility->AddWindowInfo(1);
    stackManager_->AddWindowInfo(token, 1);
    EXPECT_EQ(static_cast<int>(stackManager_->windowTokenToAbilityMap_.size()), 0);
    topAbility->RemoveWindowInfo();
    stackManager_->AddWindowInfo(token, 1);
    EXPECT_EQ(static_cast<int>(stackManager_->windowTokenToAbilityMap_.size()), 1);
    stackManager_->AddWindowInfo(token, 2);
    EXPECT_EQ(static_cast<int>(stackManager_->windowTokenToAbilityMap_.size()), 1);
}

/*
 * Feature: AbilityStackManager
 * Function: MoveMissionStackToTop
 * SubFunction: NA
 * FunctionPoints: AbilityStackManager MoveMissionStackToTop
 * EnvConditions:NA
 * CaseDescription: MoveMissionStackToTop UT Exception case
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_030, TestSize.Level0)
{
    stackManager_->Init();
    int result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(0, result);
    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    int result1 = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(0, result1);
    topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    std::shared_ptr<MissionStack> mission = nullptr;
    stackManager_->MoveMissionStackToTop(mission);

    EXPECT_EQ(topAbility->GetAbilityInfo().name, "MusicAbility");
    EXPECT_EQ(stackManager_->currentMissionStack_, stackManager_->defaultMissionStack_);

    stackManager_->MoveMissionStackToTop(stackManager_->launcherMissionStack_);
    topAbility = stackManager_->GetCurrentTopAbility();

    EXPECT_EQ(topAbility->GetAbilityInfo().name, "LauncherAbility");
    EXPECT_EQ(stackManager_->currentMissionStack_, stackManager_->launcherMissionStack_);
}

/*
 * Feature: AbilityStackManager
 * Function: IsLauncherAbility
 * SubFunction: NA
 * FunctionPoints: IsLauncherAbility
 * EnvConditions:NA
 * CaseDescription: Verify that ability is a launcher
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_031, TestSize.Level0)
{
    Want want;
    std::string entity = Want::ENTITY_HOME;
    want.AddEntity(entity);

    std::string testAppName = "ability_stack_manager_test_app";
    AbilityInfo abilityInfo;
    abilityInfo.applicationName = testAppName;
    ApplicationInfo appinfo;
    appinfo.name = testAppName;
    appinfo.isLauncherApp = true;
    abilityInfo.applicationInfo = appinfo;
    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = abilityInfo;
    abilityRequest.appInfo = appinfo;

    stackManager_->Init();
    auto topAbility = stackManager_->GetCurrentTopAbility();
    stackManager_->StartAbility(abilityRequest);
    EXPECT_EQ(stackManager_->IsLauncherAbility(abilityRequest), true);
}

/*
 * Feature: AbilityStackManager
 * Function:  GetRecentMissions
 * SubFunction: NA
 * FunctionPoints: GetRecentMissions
 * EnvConditions: NA
 * CaseDescription: Failed to verify getrecentmissions
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_033, TestSize.Level0)
{
    std::vector<RecentMissionInfo> info;
    auto result = stackManager_->GetRecentMissions(-1, 0, info);
    EXPECT_EQ(ERR_INVALID_VALUE, result);

    result = stackManager_->GetRecentMissions(10, 10, info);
    EXPECT_EQ(ERR_INVALID_VALUE, result);

    stackManager_->defaultMissionStack_ = nullptr;
    result = stackManager_->GetRecentMissions(10, 1, info);
    EXPECT_EQ(ERR_NO_INIT, result);
}

/*
 * Feature: AbilityStackManager
 * Function:  GetRecentMissions
 * SubFunction: NA
 * FunctionPoints: GetRecentMissions
 * EnvConditions: NA
 * CaseDescription: Get all recent missions list
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_034, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(radioAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto thirdTopAbility = stackManager_->GetCurrentTopAbility();
    thirdTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    std::vector<RecentMissionInfo> info;
    result = stackManager_->GetRecentMissions(10, 1, info);
    EXPECT_EQ(ERR_OK, result);

    EXPECT_EQ(static_cast<int>(info.size()), 1);
    EXPECT_EQ(info[0].id, thirdTopAbility->GetMissionRecord()->GetMissionRecordId());
    EXPECT_EQ("RadioAbility", info[0].topAbility.GetAbilityName());
}

/*
 * Feature: AbilityStackManager
 * Function:  GetRecentMissions
 * SubFunction: NA
 * FunctionPoints: GetRecentMissions
 * EnvConditions: NA
 * CaseDescription: Gets the list of recently active tasks
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_035, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(radioAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto thirdTopAbility = stackManager_->GetCurrentTopAbility();
    thirdTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicSAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::INITIAL);

    std::vector<RecentMissionInfo> info;
    result = stackManager_->GetRecentMissions(10, 2, info);
    EXPECT_EQ(ERR_OK, result);

    EXPECT_EQ(static_cast<int>(info.size()), 1);
    EXPECT_EQ(info[0].id, thirdTopAbility->GetMissionRecord()->GetMissionRecordId());
    EXPECT_EQ("RadioAbility", info[0].topAbility.GetAbilityName());
}

/*
 * Feature: AbilityStackManager
 * Function:  MoveMissionToTop
 * SubFunction: NA
 * FunctionPoints: MoveMissionToTop
 * EnvConditions: NA
 * CaseDescription: Failed to verify movemissiontotop
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_036, TestSize.Level0)
{
    stackManager_->Init();

    auto result = stackManager_->MoveMissionToTop(-1);
    EXPECT_EQ(ERR_INVALID_VALUE, result);

    stackManager_->launcherMissionStack_ = nullptr;
    auto result1 = stackManager_->MoveMissionToTop(10);
    EXPECT_EQ(ERR_NO_INIT, result1);
}

/*
 * Feature: AbilityStackManager
 * Function:  MoveMissionToTop
 * SubFunction: NA
 * FunctionPoints: MoveMissionToTop
 * EnvConditions: NA
 * CaseDescription: Failed to verify movemissiontotop
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_037, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    auto mission = secondTopAbility->GetMissionRecord();
    mission->RemoveAbilityRecord(secondTopAbility);

    auto result1 = stackManager_->MoveMissionToTop(mission->GetMissionRecordId());
    EXPECT_EQ(MOVE_MISSION_FAILED, result1);
}

/*
 * Feature: AbilityStackManager
 * Function:  MoveMissionToTop
 * SubFunction: NA
 * FunctionPoints: MoveMissionToTop
 * EnvConditions: NA
 * CaseDescription: Succeeded to verify movemissiontotop
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_038, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(radioAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto thirdTopAbility = stackManager_->GetCurrentTopAbility();
    thirdTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    auto mission = thirdTopAbility->GetMissionRecord();

    result = stackManager_->StartAbility(musicSAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto result1 = stackManager_->MoveMissionToTop(mission->GetMissionRecordId());
    EXPECT_EQ(ERR_OK, result1);

    topAbility = stackManager_->GetCurrentTopAbility();
    EXPECT_EQ("MusicAbility", topAbility->GetAbilityInfo().name);
}

/*
 * Feature: AbilityStackManager
 * Function:  OnAbilityDied
 * SubFunction: NA
 * FunctionPoints: OnAbilityDied
 * EnvConditions: NA
 * CaseDescription: If the current ability is active, if it is dead, it will return to the launcher and the state will
 * be init
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_039, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(radioAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto thirdTopAbility = stackManager_->GetCurrentTopAbility();
    thirdTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    auto mission = thirdTopAbility->GetMissionRecord();

    result = stackManager_->StartAbility(musicSAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    stackManager_->OnAbilityDied(topAbility);
    EXPECT_EQ(OHOS::AAFwk::INITIAL, topAbility->GetAbilityState());
    topAbility = stackManager_->GetCurrentTopAbility();
    EXPECT_EQ("MusicSAbility", topAbility->GetAbilityInfo().name);
}

/*
 * Feature: AbilityStackManager
 * Function:  OnAbilityDied
 * SubFunction: NA
 * FunctionPoints: OnAbilityDied
 * EnvConditions: NA
 * CaseDescription: If the current ability is uninstall, if it is dead, delete record, it will return to the launcher
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_040, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(radioAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto thirdTopAbility = stackManager_->GetCurrentTopAbility();
    thirdTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    auto mission = thirdTopAbility->GetMissionRecord();

    result = stackManager_->StartAbility(musicSAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    topAbility->SetIsUninstallAbility();

    stackManager_->OnAbilityDied(topAbility);
    EXPECT_EQ(topAbility->GetMissionRecord(), nullptr);
    topAbility = stackManager_->GetCurrentTopAbility();
    EXPECT_EQ("RadioAbility", topAbility->GetAbilityInfo().name);
}

/*
 * Feature: AbilityStackManager
 * Function:  OnAbilityDied
 * SubFunction: NA
 * FunctionPoints: OnAbilityDied
 * EnvConditions: NA
 * CaseDescription: kill music process, music ability state is init, back to launcher
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_041, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicTopAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto thirdTopAbility = stackManager_->GetCurrentTopAbility();
    thirdTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    auto mission = thirdTopAbility->GetMissionRecord();

    stackManager_->KillProcess("com.ix.hiMusic");
    // process died
    stackManager_->OnAbilityDied(secondTopAbility);
    stackManager_->OnAbilityDied(thirdTopAbility);
    auto topAbility = stackManager_->GetCurrentTopAbility();
    EXPECT_EQ("MusicAbility", topAbility->GetAbilityInfo().name);
    auto size = stackManager_->defaultMissionStack_->GetMissionRecordCount();
    EXPECT_EQ(size, 1);
}

/*
 * Feature: AbilityStackManager
 * Function:  OnAbilityDied
 * SubFunction: NA
 * FunctionPoints: OnAbilityDied
 * EnvConditions: NA
 * CaseDescription: Uninstall music process, delete music record, back to launcher
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_042, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicTopAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto thirdTopAbility = stackManager_->GetCurrentTopAbility();
    thirdTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    auto mission = thirdTopAbility->GetMissionRecord();

    stackManager_->UninstallApp("com.ix.hiMusic");
    // process died
    stackManager_->OnAbilityDied(secondTopAbility);
    stackManager_->OnAbilityDied(thirdTopAbility);
    auto topAbility = stackManager_->GetCurrentTopAbility();
    EXPECT_EQ("MusicAbility", topAbility->GetAbilityInfo().name);
    int size = stackManager_->defaultMissionStack_->GetMissionRecordCount();
    // handle is nullptr, not delete
    EXPECT_EQ(size, 1);
}

/*
 * Feature: AbilityStackManager
 * Function:  RemoveMissionById
 * SubFunction: NA
 * FunctionPoints: RemoveMissionById
 * EnvConditions: NA
 * CaseDescription: Failed to verify removemissionbyid
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_043, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    auto launcherMissionId = firstTopAbility->GetMissionRecord()->GetMissionRecordId();

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    auto missionId = secondTopAbility->GetMissionRecord()->GetMissionRecordId();

    result = stackManager_->RemoveMissionById(-1);
    EXPECT_EQ(ERR_INVALID_VALUE, result);

    result = stackManager_->RemoveMissionById(10);
    EXPECT_EQ(REMOVE_MISSION_ID_NOT_EXIST, result);

    result = stackManager_->RemoveMissionById(launcherMissionId);
    EXPECT_EQ(REMOVE_MISSION_LAUNCHER_DENIED, result);

    result = stackManager_->RemoveMissionById(missionId);
    EXPECT_EQ(REMOVE_MISSION_ACTIVE_DENIED, result);

    stackManager_->defaultMissionStack_ = nullptr;
    result = stackManager_->RemoveMissionById(missionId);
    EXPECT_EQ(ERR_NO_INIT, result);
}

/*
 * Feature: AbilityStackManager
 * Function:  RemoveMissionById
 * SubFunction: NA
 * FunctionPoints: RemoveMissionById
 * EnvConditions: NA
 * CaseDescription: Succeeded to verify removemissionbyid
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_044, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    auto missionId = secondTopAbility->GetMissionRecord()->GetMissionRecordId();

    result = stackManager_->StartAbility(musicSAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->RemoveMissionById(missionId);
    EXPECT_EQ(ERR_OK, result);
    auto size = stackManager_->defaultMissionStack_->GetMissionRecordCount();

    EXPECT_EQ(size, 1);
}

/*
 * Feature: AbilityStackManager
 * Function:  RemoveStack
 * SubFunction: NA
 * FunctionPoints: RemoveStack
 * EnvConditions: NA
 * CaseDescription: Failed to verify RemoveStack
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_045, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->RemoveStack(-1);
    EXPECT_EQ(ERR_INVALID_VALUE, result);

    result = stackManager_->RemoveStack(10);
    EXPECT_EQ(REMOVE_STACK_ID_NOT_EXIST, result);

    result = stackManager_->RemoveStack(0);
    EXPECT_EQ(REMOVE_STACK_LAUNCHER_DENIED, result);

    stackManager_->missionStackList_.clear();
    result = stackManager_->RemoveStack(1);
    EXPECT_EQ(MISSION_STACK_LIST_IS_EMPTY, result);
}

/*
 * Feature: AbilityStackManager
 * Function:  RemoveStack
 * SubFunction: NA
 * FunctionPoints: RemoveStack
 * EnvConditions: NA
 * CaseDescription: Succeeded to verify RemoveStack
 */
HWTEST_F(AbilityStackManagerTest, ability_stack_manager_operating_046, TestSize.Level0)
{
    stackManager_->Init();
    auto result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, result);
    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    result = stackManager_->RemoveStack(1);
    EXPECT_EQ(ERR_OK, result);
}
}  // namespace AAFwk
}  // namespace OHOS