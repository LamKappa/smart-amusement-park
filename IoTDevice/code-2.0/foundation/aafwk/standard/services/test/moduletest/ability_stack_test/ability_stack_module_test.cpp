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
#include "ability_manager_errors.h"
#include "ability_event_handler.h"
#include "mock_ability_scheduler.h"
#include "mock_app_mgr_client.h"
#include "mock_app_scheduler.h"
#include "ability_manager_service.h"
#undef private
#undef protected

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
class AbilityStackModuleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Want CreateWant(const std::string &entity);
    AbilityInfo CreateAbilityInfo(const std::string &name, const std::string &appName, const std::string &bundleName);
    ApplicationInfo CreateAppInfo(const std::string &appName, const std::string &bundleName);

    std::shared_ptr<AbilityStackManager> stackManager_;
};

void AbilityStackModuleTest::SetUpTestCase(void)
{
    // OHOS::DelayedSingleton<AbilityManagerService>::GetInstance()->OnStart();
}

void AbilityStackModuleTest::TearDownTestCase(void)
{
    // OHOS::DelayedSingleton<AbilityManagerService>::GetInstance()->OnStop();
    // OHOS::DelayedSingleton<AbilityManagerService>::DestroyInstance();
}

void AbilityStackModuleTest::SetUp(void)
{
    stackManager_ = std::make_shared<AbilityStackManager>(0);
}

void AbilityStackModuleTest::TearDown(void)
{}

Want AbilityStackModuleTest::CreateWant(const std::string &entity)
{
    Want want;
    if (!entity.empty()) {
        want.AddEntity(entity);
    }
    return want;
}

AbilityInfo AbilityStackModuleTest::CreateAbilityInfo(
    const std::string &name, const std::string &appName, const std::string &bundleName)
{
    AbilityInfo abilityInfo;
    abilityInfo.name = name;
    abilityInfo.applicationName = appName;
    abilityInfo.bundleName = bundleName;
    return abilityInfo;
}

ApplicationInfo AbilityStackModuleTest::CreateAppInfo(const std::string &appName, const std::string &bundleName)
{
    ApplicationInfo appInfo;
    appInfo.name = appName;
    appInfo.bundleName = bundleName;

    return appInfo;
}

/*
 * Feature: AaFwk
 * Function: ability stack management
 * SubFunction: start launcher ability
 * FunctionPoints: launcher mission stack
 * EnvConditions: NA
 * CaseDescription: start launcher ability when current launcher mission stack empty.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_001 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityInfo abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    ApplicationInfo appInfo = CreateAppInfo(bundleName, bundleName);
    Want want = CreateWant(Want::ENTITY_HOME);

    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = abilityInfo;
    abilityRequest.appInfo = appInfo;
    abilityRequest.appInfo.isLauncherApp = true;  // launcher ability
    abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    ASSERT_TRUE(curMissionStack);
    stackManager_->StartAbility(abilityRequest);
    EXPECT_EQ(AbilityStackManager::LAUNCHER_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    EXPECT_EQ(1, curMissionStack->GetMissionRecordCount());
    EXPECT_TRUE(curMissionStack->GetTopMissionRecord() != nullptr);
    EXPECT_EQ(1, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    EXPECT_STREQ(abilityName.c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().name.c_str());
    EXPECT_STREQ(bundleName.c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().bundleName.c_str());

    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_001 end";
}

/*
 * Feature: AaFwk
 * Function: ability stack management
 * SubFunction: start launcher ability
 * FunctionPoints: launcher mission stack
 * EnvConditions: NA
 * CaseDescription: not add new ability to current mission record when start the same launcher ability twice.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_002 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant(Want::ENTITY_HOME);
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);
    abilityRequest.appInfo.isLauncherApp = true;  // launcher ability
    abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

    stackManager_->missionStackList_.clear();
    EXPECT_EQ(true, stackManager_->waittingAbilityQueue_.empty());
    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();

    int result = stackManager_->StartAbility(abilityRequest);
    EXPECT_EQ(AbilityStackManager::LAUNCHER_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    EXPECT_EQ(1, curMissionStack->GetMissionRecordCount());
    EXPECT_EQ(1, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    usleep(1000);
    result = stackManager_->StartAbility(abilityRequest);  // same launcher ability
    // not change current mission stack
    EXPECT_EQ(AbilityStackManager::LAUNCHER_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    // not add new mission to current mission stack
    EXPECT_EQ(1, curMissionStack->GetMissionRecordCount());
    // not add new ability to current mission record
    EXPECT_EQ(1, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    EXPECT_STREQ(abilityName.c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().name.c_str());
    EXPECT_STREQ(bundleName.c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().bundleName.c_str());
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_002 end";
}

/*
 * Feature: AaFwk
 * Function: ability stack management
 * SubFunction: start launcher ability
 * FunctionPoints: launcher mission stack
 * EnvConditions: NA
 * CaseDescription: add new mission to current mission stack whene start the different launcher ability.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_003 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";
    int index = 1;

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant(Want::ENTITY_HOME);
    std::string strInd = std::to_string(index);
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName + strInd, bundleName + strInd, bundleName + strInd);
    abilityRequest.appInfo = CreateAppInfo(bundleName + strInd, bundleName + strInd);
    abilityRequest.appInfo.isLauncherApp = true;  // launcher ability
    abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

    AbilityRequest abilityRequest2;
    abilityRequest2.want = CreateWant(Want::ENTITY_HOME);
    index++;
    strInd = std::to_string(index);
    abilityRequest2.abilityInfo = CreateAbilityInfo(abilityName + strInd, bundleName + strInd, bundleName + strInd);
    abilityRequest2.appInfo = CreateAppInfo(bundleName + strInd, bundleName + strInd);
    abilityRequest2.appInfo.isLauncherApp = true;  // another launcher ability
    abilityRequest2.abilityInfo.applicationInfo = abilityRequest2.appInfo;

    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    int result = stackManager_->StartAbility(abilityRequest);
    usleep(1000);
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    curMissionStack->GetTopAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    result = stackManager_->StartAbility(abilityRequest2);
    // not change current mission stack
    EXPECT_EQ(AbilityStackManager::LAUNCHER_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    // add new mission to current mission stack
    EXPECT_EQ(1, curMissionStack->GetMissionRecordCount());
    EXPECT_EQ(2, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    EXPECT_STREQ((abilityName + strInd).c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().name.c_str());
    EXPECT_STREQ(
        (bundleName + strInd).c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().bundleName.c_str());
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_003 end";
}

/*
 * Feature: AaFwk
 * Function: ability stack management
 * SubFunction: start non-launcher ability
 * FunctionPoints: non-launcher mission stack
 * EnvConditions: NA
 * CaseDescription: change current mission stack when start non-launcher ability.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_004 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";
    int index = 1;

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant(Want::ENTITY_HOME);
    std::string strInd = std::to_string(index);
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName + strInd, bundleName + strInd, bundleName + strInd);
    abilityRequest.appInfo = CreateAppInfo(bundleName + strInd, bundleName + strInd);
    abilityRequest.appInfo.isLauncherApp = true;  // launcher ability
    abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

    AbilityRequest abilityRequest2;
    abilityRequest2.want = CreateWant("");
    strInd = std::to_string(++index);
    abilityRequest2.abilityInfo = CreateAbilityInfo(abilityName + strInd, bundleName + strInd, bundleName + strInd);
    abilityRequest2.appInfo = CreateAppInfo(bundleName + strInd, bundleName + strInd);
    abilityRequest2.appInfo.isLauncherApp = false;  // non-launcher ability
    abilityRequest2.abilityInfo.applicationInfo = abilityRequest2.appInfo;

    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    int result = stackManager_->StartAbility(abilityRequest);
    usleep(1000);
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    curMissionStack->GetTopAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(AbilityStackManager::LAUNCHER_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    result = stackManager_->StartAbility(abilityRequest2);
    curMissionStack = stackManager_->GetCurrentMissionStack();
    // change current mission stack
    EXPECT_EQ(AbilityStackManager::DEFAULT_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    // add new mission to current mission stack
    EXPECT_EQ(1, curMissionStack->GetMissionRecordCount());
    EXPECT_EQ(1, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    EXPECT_STREQ((abilityName + strInd).c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().name.c_str());
    EXPECT_STREQ(
        (bundleName + strInd).c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().bundleName.c_str());
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_004 end";
}

/*
 * Feature: AaFwk
 * Function: ability stack management
 * SubFunction: terminate launcher ability
 * FunctionPoints: launcher mission stack
 * EnvConditions: NA
 * CaseDescription: verify unique launcher ability can't be terminated.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_005 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant(Want::ENTITY_HOME);
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);
    abilityRequest.appInfo.isLauncherApp = true;  // launcher ability
    abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    int result = stackManager_->StartAbility(abilityRequest);
    usleep(1000);
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    curMissionStack->GetTopAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    result = stackManager_->TerminateAbility(curMissionStack->GetTopAbilityRecord()->GetToken(), -1, nullptr);
    EXPECT_EQ(TERMINATE_LAUNCHER_DENIED, result);
    // not change current mission stack
    EXPECT_EQ(AbilityStackManager::LAUNCHER_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    EXPECT_EQ(1, curMissionStack->GetMissionRecordCount());
    // not change current ability state
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::ACTIVE, curMissionStack->GetTopAbilityRecord()->GetAbilityState());
    EXPECT_STREQ(abilityName.c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().name.c_str());
    EXPECT_STREQ(bundleName.c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().bundleName.c_str());

    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_005 end";
}

/*
 * Feature: AaFwk
 * Function: ability stack management
 * SubFunction: start and terminate non-launcher ability
 * FunctionPoints: single top
 * EnvConditions: NA
 * CaseDescription: start and terminate multiple non-launcher ability(single application).
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_006 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    std::string appSuffix = "1";
    std::vector<AbilityRequest> abilityRequests;
    for (int i = 0; i < 3; i++) {
        AbilityRequest abilityRequest;
        abilityRequest.want = CreateWant("");
        std::string abilitySuffix = appSuffix + std::to_string(i + 1);
        abilityRequest.abilityInfo =
            CreateAbilityInfo(abilityName + abilitySuffix, bundleName + appSuffix, bundleName + appSuffix);
        abilityRequest.abilityInfo.launchMode = LaunchMode::STANDARD;
        abilityRequest.appInfo = CreateAppInfo(bundleName + appSuffix, bundleName + appSuffix);
        abilityRequest.appInfo.isLauncherApp = false;  // non-launcher ability
        abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

        abilityRequests.push_back(abilityRequest);
    }
    stackManager_->Init();
    // start "ability_name11"
    int result = stackManager_->StartAbility(abilityRequests[0]);
    usleep(1000);
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(AbilityStackManager::DEFAULT_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    std::shared_ptr<AbilityRecord> currentTopAbilityRecord = stackManager_->GetCurrentTopAbility();
    EXPECT_STREQ("ability_name11", currentTopAbilityRecord->GetAbilityInfo().name.c_str());
    currentTopAbilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    // start "ability_name12"
    result = stackManager_->StartAbility(abilityRequests[1]);
    usleep(1000);
    EXPECT_EQ(AbilityStackManager::DEFAULT_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    currentTopAbilityRecord = stackManager_->GetCurrentTopAbility();
    EXPECT_STREQ("ability_name12", currentTopAbilityRecord->GetAbilityInfo().name.c_str());
    currentTopAbilityRecord->GetPreAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);
    currentTopAbilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(2, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());

    // start "ability_name12"
    result = stackManager_->StartAbility(abilityRequests[1]);
    usleep(1000);
    currentTopAbilityRecord = stackManager_->GetCurrentTopAbility();
    EXPECT_STREQ("ability_name12", currentTopAbilityRecord->GetAbilityInfo().name.c_str());
    // EXPECT_STREQ("ability_name11",
    currentTopAbilityRecord->GetPreAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);
    currentTopAbilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(3, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());

    // start "ability_name13"
    result = stackManager_->StartAbility(abilityRequests[2]);
    usleep(1000);
    currentTopAbilityRecord = stackManager_->GetCurrentTopAbility();
    EXPECT_STREQ("ability_name13", currentTopAbilityRecord->GetAbilityInfo().name.c_str());
    currentTopAbilityRecord->GetPreAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);
    currentTopAbilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(4, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());

    // start "ability_name12"
    result = stackManager_->StartAbility(abilityRequests[1]);
    usleep(1000);
    currentTopAbilityRecord = stackManager_->GetCurrentTopAbility();
    EXPECT_STREQ("ability_name12", currentTopAbilityRecord->GetAbilityInfo().name.c_str());
    EXPECT_STREQ("ability_name13", currentTopAbilityRecord->GetPreAbilityRecord()->GetAbilityInfo().name.c_str());
    currentTopAbilityRecord->GetPreAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);
    currentTopAbilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(5, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());

    // terminate stack top ability "ability_name12"
    currentTopAbilityRecord->lifecycleDeal_ = nullptr;
    Want want;
    result = stackManager_->TerminateAbility(currentTopAbilityRecord->GetToken(), -1, &want);
    EXPECT_EQ(AbilityStackManager::DEFAULT_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    EXPECT_EQ(4, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    currentTopAbilityRecord = curMissionStack->GetTopAbilityRecord();
    ASSERT_TRUE(currentTopAbilityRecord != nullptr);
    EXPECT_STREQ("ability_name13", currentTopAbilityRecord->GetAbilityInfo().name.c_str());

    // terminate stack bottom ability
    auto bottomAbility = curMissionStack->GetTopMissionRecord()->abilities_.back();
    result = stackManager_->TerminateAbility(bottomAbility->GetToken(), -1, &want);
    EXPECT_EQ(3, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    currentTopAbilityRecord = curMissionStack->GetTopAbilityRecord();
    ASSERT_TRUE(currentTopAbilityRecord != nullptr);
    EXPECT_STREQ("ability_name13", currentTopAbilityRecord->GetAbilityInfo().name.c_str());

    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_006 end";
}

/*
 * Feature: AaFwk
 * Function: attach ability
 * SubFunction: attach ability thread
 * FunctionPoints: update ability state
 * EnvConditions: NA
 * CaseDescription: update ability state when attach ability.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_007 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant("");
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);
    abilityRequest.appInfo.isLauncherApp = false;  // non-launcher ability
    abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    mission->AddAbilityRecordToTop(abilityRecord);

    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);

    auto appScheduler = OHOS::DelayedSingleton<AppScheduler>::GetInstance();
    auto mockAppMgrClient = std::make_unique<MockAppMgrClient>();
    appScheduler->appMgrClient_.reset(mockAppMgrClient.get());

    OHOS::sptr<MockAbilityScheduler> abilityScheduler(new MockAbilityScheduler());
    // ams handler is non statrt so times is 0
    EXPECT_CALL(*mockAppMgrClient, UpdateAbilityState(testing::_, testing::_)).Times(0);
    EXPECT_TRUE(abilityRecord->GetToken());
    stackManager_->AttachAbilityThread(abilityScheduler, abilityRecord->GetToken());
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_007 end";
}

/*
 * Feature: AaFwk
 * Function: ability state transition
 * SubFunction: ability state transition done
 * FunctionPoints: ability state transition(TERMINATING->INITIAL)
 * EnvConditions: NA
 * CaseDescription: complete ability state transition(TERMINATING->INITIAL).
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_011 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant("");
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);

    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilityRecord->lifecycleDeal_ = nullptr;
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::TERMINATING);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    mission->AddAbilityRecordToTop(abilityRecord);
    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);

    std::shared_ptr<AbilityEventHandler> handler =
        std::make_shared<AbilityEventHandler>(nullptr, OHOS::DelayedSingleton<AbilityManagerService>::GetInstance());
    OHOS::DelayedSingleton<AbilityManagerService>::GetInstance()->handler_ = handler;

    int result = stackManager_->AbilityTransitionDone(abilityRecord->GetToken(), OHOS::AAFwk::AbilityState::INITIAL);
    EXPECT_EQ(OHOS::ERR_OK, result);

    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_011 end";
}

/*
 * Feature: AaFwk
 * Function: add Window
 * SubFunction: add window for ability
 * FunctionPoints: add window for ability
 * EnvConditions: NA
 * CaseDescription: add window for ability.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_012 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant("");
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);

    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    mission->AddAbilityRecordToTop(abilityRecord);
    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);

    stackManager_->AddWindowInfo(abilityRecord->GetToken(), 1);
    EXPECT_TRUE(abilityRecord->GetWindowInfo() != nullptr);

    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_012 end";
}

/*
 * Feature: AaFwk
 * Function: app state callback
 * SubFunction: OnAbilityRequestDone
 * FunctionPoints: OnAbilityRequestDone
 * EnvConditions: NA
 * CaseDescription: activate ability(ABILITY_STATE_FOREGROUND).
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_013 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant("");
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);

    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);

    OHOS::sptr<MockAbilityScheduler> scheduler(new MockAbilityScheduler());
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    abilityRecord->SetScheduler(scheduler);

    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    mission->AddAbilityRecordToTop(abilityRecord);
    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(testing::_, testing::_)).Times(1);
    stackManager_->OnAbilityRequestDone(
        abilityRecord->GetToken(), static_cast<int32_t>(OHOS::AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND));

    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_013 end";
}
}  // namespace AAFwk
}  // namespace OHOS