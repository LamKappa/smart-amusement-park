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
#if 1
#include <gtest/gtest.h>

#define private public
#include "kernal_system_app_manager.h"
#include "app_scheduler.h"
#undef private

#include "ability_config.h"
#include "ability_manager_service.h"
#include "ability_manager_errors.h"
#include "ability_scheduler_mock.h"
#include "mock_bundle_manager.h"
#include "mock_app_manager_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace AAFwk {

static void WaitUntilTaskFinished()
{
    const uint32_t maxRetryCount = 1000;
    const uint32_t sleepTime = 1000;
    uint32_t count = 0;
    auto handler = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };
    if (handler->PostTask(f)) {
        while (!taskCalled.load()) {
            ++count;
            if (count >= maxRetryCount) {
                break;
            }
            usleep(sleepTime);
        }
    }
}

class KernalSystemAppManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void StartSystemUI();

public:
    std::shared_ptr<KernalSystemAppManager> kernalSystemMgr_;
    int usrId_ = 10;
};

void KernalSystemAppManagerTest::SetUpTestCase()
{

    DelayedSingleton<AbilityManagerService>::GetInstance();
    DelayedSingleton<AbilityManagerService>::GetInstance()->OnStart();
}

void KernalSystemAppManagerTest::TearDownTestCase()
{
    OHOS::DelayedSingleton<AbilityManagerService>::GetInstance()->OnStop();
    OHOS::DelayedSingleton<AbilityManagerService>::DestroyInstance();
}

void KernalSystemAppManagerTest::SetUp()
{
    kernalSystemMgr_ = std::make_shared<KernalSystemAppManager>(usrId_);
    kernalSystemMgr_->abilities_.clear();
    std::queue<AbilityRequest>().swap(kernalSystemMgr_->waittingAbilityQueue_);
}

void KernalSystemAppManagerTest::TearDown()
{
    kernalSystemMgr_.reset();
}

void KernalSystemAppManagerTest::StartSystemUI()
{
    Want want;
    want.SetElementName(AbilityConfig::SYSTEM_UI_BUNDLE_NAME, AbilityConfig::SYSTEM_UI_STATUS_BAR);

    AbilityRequest request;
    request.want = want;
    request.requestCode = -1;
    request.callerToken = nullptr;
    request.abilityInfo.name = AbilityConfig::SYSTEM_UI_STATUS_BAR;
    request.abilityInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;
    request.appInfo.isLauncherApp = true;
    request.appInfo.name = AbilityConfig::SYSTEM_UI_STATUS_BAR;
    request.appInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;

    EXPECT_TRUE(kernalSystemMgr_);

    auto ref = kernalSystemMgr_->StartAbility(request);
    WaitUntilTaskFinished();
    EXPECT_EQ(ERR_OK, ref);
}

/*
 * Feature: KernalSystemAppManager
 * Function: StartAbility，StartAbilityLocked
 * SubFunction: Processing preconditions
 * FunctionPoints: KernalSystemAppManager StartAbility
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Start a system level aa
 */
HWTEST_F(KernalSystemAppManagerTest, StartAbility_001, TestSize.Level0)
{
    // start a system ui
    Want want;
    want.SetElementName(AbilityConfig::SYSTEM_UI_BUNDLE_NAME, AbilityConfig::SYSTEM_UI_STATUS_BAR);

    AbilityRequest request;
    request.want = want;
    request.requestCode = -1;
    request.callerToken = nullptr;
    request.abilityInfo.name = AbilityConfig::SYSTEM_UI_STATUS_BAR;
    request.abilityInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;
    request.appInfo.isLauncherApp = true;
    request.appInfo.name = "syetemUi";
    request.appInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;
    EXPECT_TRUE(kernalSystemMgr_);

    auto ref = kernalSystemMgr_->StartAbility(request);
    WaitUntilTaskFinished();
    EXPECT_EQ(ERR_OK, ref);
    EXPECT_EQ(static_cast<int>(kernalSystemMgr_->abilities_.size()), 1);

    auto topAbilityRecord = kernalSystemMgr_->GetCurrentTopAbility();
    EXPECT_TRUE(topAbilityRecord);
    EXPECT_TRUE(topAbilityRecord->IsKernalSystemAbility());
    topAbilityRecord->SetAbilityState(AbilityState::INITIAL);

    // start agin
    auto refBegin = kernalSystemMgr_->StartAbility(request);
    WaitUntilTaskFinished();
    EXPECT_EQ(ERR_OK, refBegin);
    // same aa, so size is 1. Just update not push
    EXPECT_EQ(static_cast<int>(kernalSystemMgr_->abilities_.size()), 1);
    EXPECT_TRUE(topAbilityRecord->IsNewWant());
}

/*
 * Feature: KernalSystemAppManager
 * Function: StartAbility，StartAbilityLocked
 * SubFunction: Processing preconditions
 * FunctionPoints: KernalSystemAppManager StartAbility
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Start a system level aa
 */
HWTEST_F(KernalSystemAppManagerTest, StartAbility_002, TestSize.Level0)
{
    // start a system ui
    Want want;
    want.SetElementName(AbilityConfig::SYSTEM_UI_BUNDLE_NAME, AbilityConfig::SYSTEM_UI_STATUS_BAR);
    AbilityRequest request;
    request.want = want;
    request.requestCode = -1;
    request.callerToken = nullptr;
    request.abilityInfo.name = AbilityConfig::SYSTEM_UI_STATUS_BAR;
    request.abilityInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;
    request.appInfo.isLauncherApp = true;
    request.appInfo.name = "syetemUi";
    request.appInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;

    Want wantBar;
    wantBar.SetElementName(AbilityConfig::SYSTEM_UI_BUNDLE_NAME, AbilityConfig::SYSTEM_UI_NAVIGATION_BAR);
    AbilityRequest requestBar;
    requestBar.want = wantBar;
    requestBar.requestCode = -1;
    requestBar.callerToken = nullptr;
    requestBar.abilityInfo.name = AbilityConfig::SYSTEM_UI_STATUS_BAR;
    requestBar.abilityInfo.bundleName = AbilityConfig::SYSTEM_UI_NAVIGATION_BAR;
    requestBar.appInfo.isLauncherApp = true;
    requestBar.appInfo.name = "syetemUi";
    requestBar.appInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;

    // nullptr name and bundlename
    Want wantPage;
    AbilityRequest requestPage;
    requestPage.want = wantPage;
    requestPage.requestCode = -1;
    requestPage.callerToken = nullptr;
    EXPECT_TRUE(kernalSystemMgr_);

    int firstStartRef = kernalSystemMgr_->StartAbility(request);
    WaitUntilTaskFinished();
    auto topAbilityRecord = kernalSystemMgr_->GetCurrentTopAbility();
    EXPECT_TRUE(topAbilityRecord);
    topAbilityRecord->SetAbilityState(AbilityState::ACTIVATING);

    int secondStartRef = kernalSystemMgr_->StartAbility(requestBar);
    int thirdStartRef = kernalSystemMgr_->StartAbility(requestPage);

    EXPECT_EQ(static_cast<int>(kernalSystemMgr_->abilities_.size()), 1);
    EXPECT_EQ(static_cast<int>(kernalSystemMgr_->waittingAbilityQueue_.size()), 2);
    EXPECT_EQ(ERR_OK, firstStartRef);
    EXPECT_EQ(START_ABILITY_WAITING, secondStartRef);
    EXPECT_EQ(START_ABILITY_WAITING, thirdStartRef);
}

/*
 * Feature: KernalSystemAppManager
 * Function: AttachAbilityThread
 * SubFunction: binding app
 * FunctionPoints: bingding app and move to forground
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: have a ability to attach
 */
HWTEST_F(KernalSystemAppManagerTest, AttachAbilityThread_001, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);
    sptr<IAbilityScheduler> scheduler = nullptr;
    sptr<IRemoteObject> token = nullptr;

    int ref = kernalSystemMgr_->AttachAbilityThread(scheduler, token);
    EXPECT_EQ(ERR_INVALID_VALUE, ref);

    StartSystemUI();
    auto topAbilityRecord = kernalSystemMgr_->GetCurrentTopAbility();
    EXPECT_TRUE(topAbilityRecord);
    // scheduler judge null is on the upper level, in ams, so is return ok
    ref = kernalSystemMgr_->AttachAbilityThread(scheduler, topAbilityRecord->GetToken());
    EXPECT_EQ(ERR_OK, ref);

    sptr<IAbilityScheduler> mokecheduler = new AbilitySchedulerMock();
    ref = kernalSystemMgr_->AttachAbilityThread(mokecheduler, topAbilityRecord->GetToken());
    EXPECT_EQ(ERR_OK, ref);
}

/*
 * Feature: KernalSystemAppManager
 * Function: AbilityTransitionDone
 * SubFunction: Processing preconditions
 * FunctionPoints: KernalSystemAppManager StartAbility
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification status
 */
HWTEST_F(KernalSystemAppManagerTest, AbilityTransitionDone_001, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);
    sptr<IRemoteObject> token = nullptr;

    int ref = kernalSystemMgr_->AbilityTransitionDone(token, -1);
    EXPECT_EQ(ERR_INVALID_VALUE, ref);

    StartSystemUI();
    auto topAbilityRecord = kernalSystemMgr_->GetCurrentTopAbility();
    EXPECT_TRUE(topAbilityRecord);
    ref = kernalSystemMgr_->AbilityTransitionDone(topAbilityRecord->GetToken(), -1);
    EXPECT_EQ(ERR_INVALID_VALUE, ref);

    // The right example
    topAbilityRecord->SetAbilityState(AbilityState::ACTIVATING);
    ref = kernalSystemMgr_->AbilityTransitionDone(topAbilityRecord->GetToken(), AbilityState::ACTIVE);
    EXPECT_EQ(ERR_OK, ref);
    WaitUntilTaskFinished();
    ref = kernalSystemMgr_->AbilityTransitionDone(topAbilityRecord->GetToken(), AbilityState::INITIAL);
    EXPECT_EQ(ERR_INVALID_VALUE, ref);
}

/*
 * Feature: KernalSystemAppManagerssssssssssssssss
 * Function: OnAbilityRequestDone
 * SubFunction: Processing preconditions
 * FunctionPoints: KernalSystemAppManager OnAbilityRequestDone
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification status
 */
HWTEST_F(KernalSystemAppManagerTest, OnAbilityRequestDone_001, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);
    StartSystemUI();
    auto topAbilityRecord = kernalSystemMgr_->GetCurrentTopAbility();
    EXPECT_TRUE(topAbilityRecord);
    kernalSystemMgr_->OnAbilityRequestDone(topAbilityRecord->GetToken(), -1);
    EXPECT_NE(topAbilityRecord->GetAbilityState(), AbilityState::ACTIVATING);

    kernalSystemMgr_->OnAbilityRequestDone(
        topAbilityRecord->GetToken(), static_cast<int>(AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND));
    EXPECT_EQ(topAbilityRecord->GetAbilityState(), AbilityState::ACTIVATING);
}

/*
 * Feature: KernalSystemAppManager
 * Function: OnAbilityRequestDone
 * SubFunction: Processing preconditions
 * FunctionPoints: KernalSystemAppManager OnAbilityRequestDone
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification status
 */
HWTEST_F(KernalSystemAppManagerTest, GetManagerUserId_001, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);
    StartSystemUI();
    int testUserID = kernalSystemMgr_->GetManagerUserId();
    EXPECT_EQ(10, testUserID);
}

/*
 * Feature: KernalSystemAppManager
 * Function: DumpState
 * SubFunction: Processing preconditions
 * FunctionPoints:
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification status
 */
HWTEST_F(KernalSystemAppManagerTest, DumpState_001, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);
    StartSystemUI();
    std::vector<std::string> info;
    kernalSystemMgr_->DumpState(info);

    auto isFindAbilityInfo = [](std::string &abilityInfo) {
        return std::string::npos != abilityInfo.find(AbilityConfig::SYSTEM_UI_BUNDLE_NAME);
    };
    // find the SYSTEM_UI_BUNDLE_NAME
    EXPECT_NE(info.end(), std::find_if(info.begin(), info.end(), isFindAbilityInfo));
}

/*
 * Feature: KernalSystemAppManager
 * Function: OnAbilityDied
 * SubFunction: Processing preconditions
 * FunctionPoints:
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification ability died
 */
HWTEST_F(KernalSystemAppManagerTest, OnAbilityDied_001, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);
    StartSystemUI();
    auto topAbilityRecord = kernalSystemMgr_->GetCurrentTopAbility();
    EXPECT_TRUE(topAbilityRecord);
    kernalSystemMgr_->OnAbilityDied(topAbilityRecord);
    EXPECT_EQ(topAbilityRecord->GetAbilityState(), AbilityState::INITIAL);
}

/*
 * Feature: KernalSystemAppManager
 * Function: OnAbilityDied
 * SubFunction: Processing preconditions
 * FunctionPoints:
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification status
 */
HWTEST_F(KernalSystemAppManagerTest, OnAbilityDied_002, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);
    StartSystemUI();
    auto topAbilityRecord = kernalSystemMgr_->GetCurrentTopAbility();
    EXPECT_TRUE(topAbilityRecord);
    kernalSystemMgr_->OnAbilityDied(topAbilityRecord);
    EXPECT_EQ(topAbilityRecord->GetAbilityState(), AbilityState::INITIAL);
}

/*
 * Feature: KernalSystemAppManager
 * Function: OnAbilityDied
 * SubFunction: Processing preconditions
 * FunctionPoints:
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification status
 */
HWTEST_F(KernalSystemAppManagerTest, OnTimeOut_001, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);
    StartSystemUI();
    auto topAbilityRecord = kernalSystemMgr_->GetCurrentTopAbility();
    EXPECT_TRUE(topAbilityRecord);
    std::vector<std::string> info;
    kernalSystemMgr_->DumpState(info);
    auto isFindAbilityInfo = [](std::string &abilityInfo) {
        return std::string::npos != abilityInfo.find(AbilityConfig::SYSTEM_UI_BUNDLE_NAME);
    };
    EXPECT_NE(info.end(), std::find_if(info.begin(), info.end(), isFindAbilityInfo));

    // remove form vector;
    kernalSystemMgr_->OnTimeOut(AbilityManagerService::LOAD_TIMEOUT_MSG, topAbilityRecord->GetEventId());

    info.clear();
    kernalSystemMgr_->DumpState(info);
    // topAbilityRecord must be delete
    EXPECT_EQ(info.end(), std::find_if(info.begin(), info.end(), isFindAbilityInfo));
}
/*
 * Feature: KernalSystemAppManager
 * Function: DequeueWaittingAbility
 * SubFunction: Processing preconditions
 * FunctionPoints:
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification status
 */
HWTEST_F(KernalSystemAppManagerTest, DequeueWaittingAbility_001, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);
    StartSystemUI();
    auto topAbilityRecord = kernalSystemMgr_->GetCurrentTopAbility();
    EXPECT_TRUE(topAbilityRecord);
    topAbilityRecord->SetAbilityState(AbilityState::ACTIVATING);

    Want want;
    AbilityRequest request;
    request.want = want;
    request.requestCode = -1;
    request.callerToken = nullptr;
    request.abilityInfo.name = AbilityConfig::SYSTEM_UI_STATUS_BAR;
    request.abilityInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;
    request.appInfo.isLauncherApp = true;
    request.appInfo.name = "syetemUi";
    request.appInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;

    // push in waiting queue;
    kernalSystemMgr_->StartAbility(request);
    EXPECT_EQ(static_cast<int>(kernalSystemMgr_->waittingAbilityQueue_.size()), 1);

    topAbilityRecord->SetAbilityState(AbilityState::ACTIVE);
    kernalSystemMgr_->DequeueWaittingAbility();
    EXPECT_EQ(static_cast<int>(kernalSystemMgr_->waittingAbilityQueue_.size()), 0);
}

/*
 * Feature: KernalSystemAppManager
 * Function: DequeueWaittingAbility
 * SubFunction: Processing preconditions
 * FunctionPoints:
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification status
 */
HWTEST_F(KernalSystemAppManagerTest, GetOrCreateAbilityRecord_001, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);

    Want want;
    AbilityRequest request;
    request.want = want;
    request.requestCode = -1;
    request.callerToken = nullptr;
    request.abilityInfo.name = AbilityConfig::SYSTEM_UI_STATUS_BAR;
    request.abilityInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;
    request.appInfo.isLauncherApp = true;
    request.appInfo.name = "syetemUi";
    request.appInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;

    std::shared_ptr<AbilityRecord> targetAbility;
    EXPECT_FALSE(targetAbility);
    kernalSystemMgr_->GetOrCreateAbilityRecord(request, targetAbility);

    EXPECT_EQ(static_cast<int>(kernalSystemMgr_->abilities_.size()), 1);
    EXPECT_TRUE(targetAbility);
    EXPECT_FALSE(targetAbility->IsNewWant());

    // again
    kernalSystemMgr_->GetOrCreateAbilityRecord(request, targetAbility);
    EXPECT_TRUE(targetAbility->IsNewWant());
}

/*
 * Feature: KernalSystemAppManager
 * Function: DequeueWaittingAbility
 * SubFunction: Processing preconditions
 * FunctionPoints:
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification status
 */
HWTEST_F(KernalSystemAppManagerTest, GetOrCreateAbilityRecord_002, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);

    Want want;
    AbilityRequest request;
    request.want = want;
    request.requestCode = -1;
    request.callerToken = nullptr;
    request.abilityInfo.name = AbilityConfig::SYSTEM_UI_STATUS_BAR;
    request.abilityInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;
    request.appInfo.isLauncherApp = true;
    request.appInfo.name = "syetemUi";
    request.appInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;

    std::shared_ptr<AbilityRecord> targetAbility;
    EXPECT_FALSE(targetAbility);
    kernalSystemMgr_->GetOrCreateAbilityRecord(request, targetAbility);

    EXPECT_EQ(static_cast<int>(kernalSystemMgr_->abilities_.size()), 1);
    EXPECT_TRUE(targetAbility);
    EXPECT_FALSE(targetAbility->IsNewWant());

    // again
    kernalSystemMgr_->GetOrCreateAbilityRecord(request, targetAbility);
    EXPECT_TRUE(targetAbility->IsNewWant());
    EXPECT_EQ(static_cast<int>(kernalSystemMgr_->abilities_.size()), 1);

    std::string abilityNameInfo = AbilityConfig::SYSTEM_UI_BUNDLE_NAME + ":" + AbilityConfig::SYSTEM_UI_STATUS_BAR;
    auto abilityFlag =
        kernalSystemMgr_->GetFlagOfAbility(AbilityConfig::SYSTEM_UI_BUNDLE_NAME, AbilityConfig::SYSTEM_UI_STATUS_BAR);
    EXPECT_TRUE(abilityNameInfo == abilityFlag);
}

/*
 * Feature: KernalSystemAppManager
 * Function: DequeueWaittingAbility
 * SubFunction: Processing preconditions
 * FunctionPoints:
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification status
 */
HWTEST_F(KernalSystemAppManagerTest, GetAbilityRecordByToken_001, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);

    StartSystemUI();
    auto topAbilityRecord = kernalSystemMgr_->GetCurrentTopAbility();
    EXPECT_TRUE(topAbilityRecord);
    auto testRecord = kernalSystemMgr_->GetAbilityRecordByToken(topAbilityRecord->GetToken());
    EXPECT_TRUE(testRecord == topAbilityRecord);

    sptr<IRemoteObject> token;
    auto testRecord1 = kernalSystemMgr_->GetAbilityRecordByToken(token);
    EXPECT_TRUE(testRecord1 == nullptr);

    auto testRecord2 = kernalSystemMgr_->GetAbilityRecordByEventId(topAbilityRecord->GetEventId());
    EXPECT_TRUE(testRecord2 == topAbilityRecord);

    auto testRecord3 = kernalSystemMgr_->GetAbilityRecordByEventId(-1);
    EXPECT_TRUE(testRecord3 == nullptr);
}

/*
 * Feature: KernalSystemAppManager
 * Function: DispatchActive
 * SubFunction: Processing preconditions
 * FunctionPoints:
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification status
 */
HWTEST_F(KernalSystemAppManagerTest, DispatchActive_001, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);

    StartSystemUI();
    auto topAbilityRecord = kernalSystemMgr_->GetCurrentTopAbility();
    EXPECT_TRUE(topAbilityRecord);
    topAbilityRecord->SetAbilityState(AbilityState::ACTIVATING);

    Want want;
    AbilityRequest request;
    request.want = want;
    request.requestCode = -1;
    request.callerToken = nullptr;
    request.abilityInfo.name = AbilityConfig::SYSTEM_UI_STATUS_BAR;
    request.abilityInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;
    request.appInfo.isLauncherApp = true;
    request.appInfo.name = "syetemUi";
    request.appInfo.bundleName = AbilityConfig::SYSTEM_UI_BUNDLE_NAME;

    kernalSystemMgr_->EnqueueWaittingAbility(request);
    EXPECT_EQ(static_cast<int>(kernalSystemMgr_->waittingAbilityQueue_.size()), 1);

    int ref = kernalSystemMgr_->DispatchActive(topAbilityRecord, AbilityState::ACTIVE);
    WaitUntilTaskFinished();
    EXPECT_EQ(ERR_OK, ref);
}

/*
 * Feature: KernalSystemAppManager
 * Function: RemoveAbilityRecord
 * SubFunction: Processing preconditions
 * FunctionPoints:
 * EnvConditions:Is Kernal System Ability
 * CaseDescription: Notification status
 */
HWTEST_F(KernalSystemAppManagerTest, DispatchActive_002, TestSize.Level0)
{
    EXPECT_TRUE(kernalSystemMgr_);
    StartSystemUI();
    auto topAbilityRecord = kernalSystemMgr_->GetCurrentTopAbility();
    EXPECT_TRUE(topAbilityRecord);

    EXPECT_EQ(static_cast<int>(kernalSystemMgr_->abilities_.size()), 1);
    bool isRemove = kernalSystemMgr_->RemoveAbilityRecord(topAbilityRecord);
    EXPECT_TRUE(isRemove);

    isRemove = kernalSystemMgr_->RemoveAbilityRecord(nullptr);
    EXPECT_FALSE(isRemove);
}
}  // namespace AAFwk
}  // namespace OHOS

#endif