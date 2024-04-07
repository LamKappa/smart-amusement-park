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
#include "gmock/gmock.h"

#include <thread>
#include <chrono>

#define private public
#define protected public
#include "system_ability_definition.h"
#include "lifecycle_test_base.h"
#include "mock_bundle_manager.h"
#include "sa_mgr_client.h"
#undef private
#undef protected

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
namespace {
const std::string NAME_BUNDLE_MGR_SERVICE = "BundleMgrService";
}

static void WaitUntilTaskFinished()
{
    const uint32_t maxRetryCount = 1000;
    const uint32_t sleepTime = 1000;
    uint32_t count = 0;
    auto handler = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        GTEST_LOG_(ERROR) << "handler is nullptr";
        return;
    }
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

class LifecycleTest : public testing::Test, public LifecycleTestBase {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp() override;

    void TearDown() override;

    bool StartLauncherAbility() override;

    bool StartNextAbility() override;

    int AttachAbility(const OHOS::sptr<OHOS::AAFwk::AbilityScheduler> &scheduler,
        const OHOS::sptr<OHOS::IRemoteObject> &token) override;

public:
    int startLancherFlag_ = false;

    std::shared_ptr<OHOS::AAFwk::AbilityManagerService> aams_;
    std::shared_ptr<OHOS::AAFwk::AbilityRecord> launcherAbilityRecord_;  // launcher ability
    OHOS::sptr<OHOS::IRemoteObject> launcherToken_;                      // token of launcher ability
    std::shared_ptr<OHOS::AAFwk::AbilityRecord> nextAbilityRecord_;      // ability being launched
    OHOS::sptr<OHOS::IRemoteObject> nextToken_;                          // token of ability being launched
    OHOS::sptr<OHOS::AAFwk::AbilityScheduler> launcherScheduler_;        // launcher ability thread interface
    OHOS::sptr<OHOS::AAFwk::AbilityScheduler> nextScheduler_;            // next ability thread interface
    std::unique_ptr<LifeTestCommand> command_;                           // test command_ interact with ams_
};

void LifecycleTest::SetUpTestCase(void)
{}

void LifecycleTest::TearDownTestCase(void)
{}

void LifecycleTest::SetUp(void)
{
    OHOS::sptr<OHOS::IRemoteObject> bundleObject = new BundleMgrService();
    OHOS::DelayedSingleton<SaMgrClient>::GetInstance()->RegisterSystemAbility(
        OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);

    aams_ = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance();
    aams_->OnStart();
    WaitUntilTaskFinished();
    StartLauncherAbility();
    command_ = std::make_unique<LifeTestCommand>();
}

void LifecycleTest::TearDown(void)
{
    aams_->OnStop();
    OHOS::DelayedSingleton<AbilityManagerService>::DestroyInstance();
    launcherAbilityRecord_.reset();
    launcherToken_ = nullptr;
    nextAbilityRecord_.reset();
    nextToken_ = nullptr;
    launcherScheduler_ = nullptr;
    nextScheduler_ = nullptr;
    command_.reset();
    aams_.reset();
    startLancherFlag_ = false;
}

bool LifecycleTest::StartLauncherAbility()
{
    ElementName element("device", "com.ix.hiWord", "LauncherAbility");
    Want want;
    want.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    want.SetElement(element);
    int ref = aams_->StartAbility(want, -1);
    WaitUntilTaskFinished();
    EXPECT_EQ(ref, 0);
    if (ref != 0) {
        GTEST_LOG_(ERROR) << "fail to start Launcher ability";
        return false;
    }
    auto stackManager = aams_->GetStackManager();
    EXPECT_TRUE(stackManager);
    launcherAbilityRecord_ = (stackManager->GetCurrentTopAbility());
    EXPECT_TRUE(launcherAbilityRecord_);
    if (launcherAbilityRecord_) {
        GTEST_LOG_(ERROR) << "launcherAbilityRecord_ is not null";
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
        launcherToken_ = launcherAbilityRecord_->GetToken();
        launcherScheduler_ = new AbilityScheduler();
        startLancherFlag_ = true;
        return true;
    }
    return false;
}

bool LifecycleTest::StartNextAbility()
{
    Want want;
    ElementName element("device", "com.ix.hiMusic", "MusicAbility");
    want.SetElement(element);

    auto stackManager = aams_->GetStackManager();
    EXPECT_TRUE(stackManager);
    if (stackManager) {
        GTEST_LOG_(ERROR) << "top BundleName ："
                          << stackManager->GetCurrentTopAbility()->GetWant().GetElement().GetBundleName();
        GTEST_LOG_(ERROR) << "top AbilityName ："
                          << stackManager->GetCurrentTopAbility()->GetWant().GetElement().GetAbilityName();
        stackManager->GetCurrentTopAbility()->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }
    int ref = aams_->StartAbility(want, -1);
    WaitUntilTaskFinished();
    EXPECT_EQ(ref, 0);
    if (ref != 0) {
        GTEST_LOG_(ERROR) << "fail to start next ability";
        return false;
    }

    nextAbilityRecord_ = stackManager->GetCurrentTopAbility();
    if (nextAbilityRecord_ != nullptr) {
        nextToken_ = nextAbilityRecord_->GetToken();
        nextScheduler_ = new AbilityScheduler();
        nextAbilityRecord_->SetScheduler(nextScheduler_);
        GTEST_LOG_(INFO) << "nextAbilityRecord_ is not null";
    } else {
        GTEST_LOG_(ERROR) << "next ability is nullptr";
        return false;
    }
    nextScheduler_ = new AbilityScheduler();
    return true;
}

int LifecycleTest::AttachAbility(
    const OHOS::sptr<OHOS::AAFwk::AbilityScheduler> &scheduler, const OHOS::sptr<OHOS::IRemoteObject> &token)
{
    int ret = aams_->AttachAbilityThread(scheduler, token);
    static int32_t windowToken = 0;
    aams_->AddWindowInfo(token, ++windowToken);
    return ret;
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: verify AttachAbilityThread parameters.
 * AttachAbilityThread fail if IAbilityScheduler or token is nullptr.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_StartLauncherAbilityLifeCycle_001, TestSize.Level1)
{
    if (startLancherFlag_) {
        EXPECT_TRUE(aams_);
        EXPECT_TRUE(launcherAbilityRecord_);
        EXPECT_NE(aams_->AttachAbilityThread(nullptr, launcherToken_), 0);
        EXPECT_NE(aams_->AttachAbilityThread(launcherScheduler_, nullptr), 0);
        EXPECT_EQ(launcherAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVE);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: verify launcher AbilityRecord state_ when AttachAbilityThread success.
 * 1. AbilityState transferred from INITIAL to ACTIVATING.
 * 2. AbilityRecord is attached.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_StartLauncherAbilityLifeCycle_002, TestSize.Level1)
{
    if (startLancherFlag_) {
        EXPECT_TRUE(aams_);
        EXPECT_TRUE(launcherAbilityRecord_);
        EXPECT_EQ(launcherAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVE);
        EXPECT_TRUE(launcherScheduler_);
        EXPECT_TRUE(launcherToken_);
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_EQ(launcherAbilityRecord_->IsReady(), true);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: verify AbilityRecord transition timeout handler.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_StartLauncherAbilityLifeCycle_003, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = false;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret =
            LifecycleTest::SemTimedWaitMillis(AbilityManagerService::LOAD_TIMEOUT + DELAY_TEST_TIME, command_->sem_);
        EXPECT_NE(ret, 0);
        // check timeout handler
        EXPECT_EQ(launcherAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVATING);
        pthread_join(tid, nullptr);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: verify AbilityTransitionDone parameters.
 * AbilityTransitionDone fail if launcher schedules incorrect Life state_.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_StartLauncherAbilityLifeCycle_004, TestSize.Level1)
{
    if (startLancherFlag_) {
        // AttachAbilityThread done and success
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);

        command_->callback_ = true;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->abnormalState_ = OHOS::AAFwk::AbilityState::INACTIVE;
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret =
            LifecycleTest::SemTimedWaitMillis(AbilityManagerService::LOAD_TIMEOUT + DELAY_TEST_TIME, command_->sem_);
        if (ret != 0) {
            // check timeout handler
            GTEST_LOG_(INFO) << "timeout. It shouldn't happen.";
            pthread_join(tid, nullptr);
            return;
        }
        pthread_join(tid, nullptr);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: AttachAbilityThread done, verify AbilityRecord state_ when AbilityStartThread success.
 * 1. Life transition from UNDEFINED to ACTIVATING to ACTIVE.
 * 2. AbilityRecord is attached.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_StartLauncherAbilityLifeCycle_005, TestSize.Level1)
{
    if (startLancherFlag_) {
        // AttachAbilityThread done and success
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        command_->callback_ = true;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        pthread_t tid = 0;

        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret =
            LifecycleTest::SemTimedWaitMillis(AbilityManagerService::LOAD_TIMEOUT + DELAY_TEST_TIME, command_->sem_);
        if (ret != 0) {
            // check timeout handler. It won't happen normally.
            GTEST_LOG_(INFO) << "timeout. It shouldn't happen.";
            pthread_join(tid, nullptr);
            return;
        }
        EXPECT_EQ(aams_->AbilityTransitionDone(launcherToken_, command_->state_), OHOS::ERR_OK);
        if (launcherAbilityRecord_->GetAbilityState() != OHOS::AAFwk::AbilityState::ACTIVE) {
            WaitUntilTaskFinished();
            EXPECT_EQ(launcherAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVE);
        }
        EXPECT_EQ(launcherAbilityRecord_->IsReady(), true);
        pthread_join(tid, nullptr);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription:  hnadeler is timeout
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_StartLauncherAbilityLifeCycle_006, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = false;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret = LifecycleTest::SemTimedWaitMillis(AbilityManagerService::ACTIVE_TIMEOUT, command_->sem_);
        EXPECT_NE(ret, 0);
        // check AttachAbilityThread timeout handler
        EXPECT_EQ(launcherAbilityRecord_->IsReady(), false);
        pthread_join(tid, nullptr);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: launcher OnInactive timeout, verify launcher AbilityTransitionDone timeout handler.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_001, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = false;
        command_->expectState_ = OHOS::AAFwk::AbilityState::INACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        // launcher is in inactivating process.
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_TRUE(StartNextAbility());
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret = LifecycleTest::SemTimedWaitMillis(AbilityManagerService::INACTIVE_TIMEOUT, command_->sem_);
        EXPECT_NE(ret, 0);
        // check AbilityTransitionDone timeout handler
        EXPECT_EQ(nextAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::INITIAL);
        pthread_join(tid, nullptr);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: verify AbilityTransitionDone parameters.
 * AbilityTransitionDone fail if life state_ is incompatible with
 * OnInactive process. Or launcher schedules incorrect life state_.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_002, TestSize.Level1)
{
    if (startLancherFlag_) {
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_TRUE(StartNextAbility());
        // launcher is in inactivating process.
        EXPECT_NE(aams_->AbilityTransitionDone(launcherToken_, OHOS::AAFwk::AbilityState::ACTIVE), 0);
        WaitUntilTaskFinished();
        EXPECT_EQ(launcherAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::INACTIVATING);
        EXPECT_EQ(nextAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::INITIAL);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread
 * EnvConditions:NA
 * CaseDescription: launcher OnInactive done, verify new ability AttachAbilityThread timeout handler.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_003, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = false;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        WaitUntilTaskFinished();
        EXPECT_TRUE(StartNextAbility());
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
        EXPECT_EQ(aams_->AbilityTransitionDone(launcherToken_, OHOS::AAFwk::AbilityState::INACTIVE), 0);
        // launcher oninactive done.
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret = LifecycleTest::SemTimedWaitMillis(
            AbilityManagerService::INACTIVE_TIMEOUT + DELAY_TEST_TIME, command_->sem_);
        EXPECT_NE(ret, 0);
        // check timeout handler
        EXPECT_EQ(nextAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVATING);
        pthread_join(tid, nullptr);

        WaitUntilTaskFinished();
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: launcher OnInactive done, verify AbilityTransitionDone parameter.
 * AbilityTransitionDone fail if new ability
 * IAbilityScheduler is nullptr.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_004, TestSize.Level1)
{
    if (startLancherFlag_) {
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_TRUE(StartNextAbility());
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
        EXPECT_EQ(aams_->AbilityTransitionDone(launcherToken_, OHOS::AAFwk::AbilityState::INACTIVE), 0);
        // launcher oninactive done.
        nextAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INITIAL);
        EXPECT_EQ(AttachAbility(nextScheduler_, nextToken_), 0);
        EXPECT_NE(aams_->AbilityTransitionDone(nullptr, OHOS::AAFwk::AbilityState::ACTIVE), 0);
        if (nextAbilityRecord_->GetAbilityState() != OHOS::AAFwk::AbilityState::ACTIVATING) {
            WaitUntilTaskFinished();
            EXPECT_EQ(nextAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVATING);
        }
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: launcher OnInactive done. verify AbilityTransitionDone parameter.
 * AbilityTransitionDone fail if new ability
 * schedules incorrect state_.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_005, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = true;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->abnormalState_ = OHOS::AAFwk::AbilityState::INACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_TRUE(StartNextAbility());
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
        EXPECT_EQ(aams_->AbilityTransitionDone(launcherToken_, OHOS::AAFwk::AbilityState::INACTIVE), 0);
        WaitUntilTaskFinished();
        // launcher oninactive done.
        nextAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INITIAL);
        EXPECT_EQ(AttachAbility(nextScheduler_, nextToken_), 0);
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret =
            LifecycleTest::SemTimedWaitMillis(AbilityManagerService::LOAD_TIMEOUT + DELAY_TEST_TIME, command_->sem_);
        if (ret != 0) {
            // check timeout handler
            pthread_join(tid, nullptr);
            return;
        }
        pthread_join(tid, nullptr);
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: launcher OnInactive done. verify new ability AbilityTransitionDone timeout handler.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_006, TestSize.Level1)
{

    if (startLancherFlag_) {
        command_->callback_ = false;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_TRUE(StartNextAbility());
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
        // launcher oninactive done.
        EXPECT_EQ(AttachAbility(nextScheduler_, nextToken_), 0);
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret =
            LifecycleTest::SemTimedWaitMillis(AbilityManagerService::ACTIVE_TIMEOUT + DELAY_TEST_TIME, command_->sem_);
        EXPECT_NE(ret, 0);
        // check timeout handler
        EXPECT_EQ(nextAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVATING);
        pthread_join(tid, nullptr);
        return;
    }
}

/*
 * Feature: Lifecycle schedule
 * Function: Lifecycle schedule
 * SubFunction: NA
 * FunctionPoints: AttachAbilityThread AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: launcher OnInactive done and starts new ability success. verify new AbilityRecord.
 * 1. Launcher oninactive done and is INACTIVE.
 * 2. new ability is ACTIVE.
 * 3. Launcher is transferred from INACTIVE to MOVING_BACKGROUND when new ability started.
 */
HWTEST_F(LifecycleTest, AAFWK_AbilityMS_startAbilityLifeCycle_007, TestSize.Level1)
{
    if (startLancherFlag_) {
        command_->callback_ = true;
        command_->expectState_ = OHOS::AAFwk::AbilityState::ACTIVE;
        command_->state_ = OHOS::AAFwk::AbilityState::INITIAL;
        EXPECT_EQ(AttachAbility(launcherScheduler_, launcherToken_), 0);
        EXPECT_TRUE(StartNextAbility());
        launcherAbilityRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
        EXPECT_EQ(aams_->AbilityTransitionDone(launcherToken_, OHOS::AAFwk::AbilityState::INACTIVE), OHOS::ERR_OK);
        // launcher oninactive done.
        WaitUntilTaskFinished();
        EXPECT_EQ(launcherAbilityRecord_->GetAbilityState(), OHOS::AAFwk::AbilityState::INACTIVE);
        EXPECT_EQ(AttachAbility(nextScheduler_, nextToken_), 0);
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, LifecycleTest::AbilityStartThread, command_.get());
        int ret = LifecycleTest::SemTimedWaitMillis(AbilityManagerService::ACTIVE_TIMEOUT * 2, command_->sem_);
        if (ret != 0) {
            // check timeout handler
            pthread_join(tid, nullptr);
            return;
        }
        pthread_join(tid, nullptr);
    }
}
}  // namespace AAFwk
}  // namespace OHOS