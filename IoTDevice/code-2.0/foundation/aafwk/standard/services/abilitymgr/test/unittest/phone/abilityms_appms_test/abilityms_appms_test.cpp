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
#include "system_ability_definition.h"

#define private public
#define protected public
#include "ability_scheduler.h"
#include "app_scheduler.h"
#undef private
#undef protected

#include "ability_manager_service.h"
#include "mock_bundle_manager.h"
#include "mock_app_manager_client.h"
#include "sa_mgr_client.h"
#include "app_state_callback_host.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace testing::ext;
using testing::_;

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

class AppStateCallbackS : public AppStateCallback {
public:
    AppStateCallbackS()
    {}
    ~AppStateCallbackS()
    {}
    MOCK_METHOD2(OnAbilityRequestDone, void(const sptr<IRemoteObject> &token, const int32_t state));
};

class AbilityMsAppmsTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<AbilityRecord> GetAbilityRecord() const;
    void ResetAbilityRecord();
    void startAbility();

public:
    std::shared_ptr<AbilityRecord> abilityRecord_;
    std::shared_ptr<AppStateCallbackS> callback_;
};

void AbilityMsAppmsTest::SetUpTestCase(void)
{}
void AbilityMsAppmsTest::TearDownTestCase(void)
{}

void AbilityMsAppmsTest::SetUp(void)
{
    sptr<OHOS::IRemoteObject> bundleObject = new BundleMgrService();
    DelayedSingleton<SaMgrClient>::GetInstance()->RegisterSystemAbility(
        OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);
    DelayedSingleton<AppScheduler>::GetInstance();

    callback_ = std::make_shared<AppStateCallbackS>();

    DelayedSingleton<AppScheduler>::GetInstance()->Init(callback_);

    DelayedSingleton<AbilityManagerService>::GetInstance()->OnStart();
    auto ams = DelayedSingleton<AbilityManagerService>::GetInstance();
    WaitUntilTaskFinished();
    startAbility();
}

void AbilityMsAppmsTest::TearDown(void)
{
    DelayedSingleton<AbilityManagerService>::GetInstance()->OnStop();
    DelayedSingleton<AbilityManagerService>::DestroyInstance();
}

std::shared_ptr<AbilityRecord> AbilityMsAppmsTest::GetAbilityRecord() const
{
    return abilityRecord_;
}

void AbilityMsAppmsTest::ResetAbilityRecord()
{
    abilityRecord_.reset();
    const Want want;
    AbilityInfo abilityInfo;
    abilityInfo.name = "HelloWorld";
    abilityInfo.applicationName = "HelloWorld";
    abilityInfo.package = "com.ix.hiworld";
    ApplicationInfo applicationInfo;
    applicationInfo.name = "HelloWorld";
    applicationInfo.bundleName = "HelloWorld";

    abilityRecord_ = std::make_shared<AbilityRecord>(want, abilityInfo, applicationInfo);
    abilityRecord_->Init();
}

void AbilityMsAppmsTest::startAbility()
{
    Want want;
    AbilityInfo abilityInfo;
    ApplicationInfo applicationInfo;

    auto abilityMs_ = DelayedSingleton<AbilityManagerService>::GetInstance();
    ElementName element("device", "com.ix.hiworld", "luncherAbility");
    want.SetElement(element);
    abilityMs_->StartAbility(want);
    auto topAbility = abilityMs_->GetStackManager()->GetCurrentTopAbility();
    if (topAbility) {
        topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    }
    abilityRecord_ = std::make_shared<AbilityRecord>(want, abilityInfo, applicationInfo);
}

/*
 * Feature:  Interaction of abilityms and appms
 * Function: Interaction of abilityms and appms
 * SubFunction: NA
 * FunctionPoints: LoadAbility
 * EnvConditions:NA
 * CaseDescription: verify LoadAbility parameters. LoadAbility fail if token is nullptr.
 */
HWTEST_F(AbilityMsAppmsTest, AaFwk_AbilityMS_AppMS_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityMsAppMsTest_AaFwk_AbilityMS_AppMS_001 start";
    auto result = GetAbilityRecord()->LoadAbility();
    EXPECT_EQ(ERR_INVALID_VALUE, result);
    GTEST_LOG_(INFO) << "AbilityMsAppMsTest_AaFwk_AbilityMS_AppMS_001 end";
}

/*
 * Feature: Interaction of abilityms and appms
 * Function: Interaction of abilityms and appms
 * SubFunction: NA
 * FunctionPoints: LoadAbility
 * EnvConditions:NA
 * CaseDescription: verify LoadAbility parameters. LoadAbility fail if abilityinfo or appinfo is empty.
 */
HWTEST_F(AbilityMsAppmsTest, AaFwk_AbilityMS_AppMS_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityMsAppMsTest_AaFwk_AbilityMS_AppMS_002 start";
    GetAbilityRecord()->Init();
    auto result = GetAbilityRecord()->LoadAbility();
    EXPECT_EQ(ERR_INVALID_VALUE, result);
    GTEST_LOG_(INFO) << "AbilityMsAppMsTest_AaFwk_AbilityMS_AppMS_002 end";
}

/*
 * Feature: Interaction of abilityms and appms
 * Function: Interaction of abilityms and appms
 * SubFunction: NA
 * FunctionPoints: LoadAbility
 * EnvConditions:NA
 *  CaseDescription: 1. abilityinfo or appinfo is empty.
 *                   2. Load ability
 *                   3. the result of load ability is successfully,
 *                      the LoadAbility function of Appscheduler is called.
 *                   4. Ability state is still INITIAL, since called is asynchronous.
 */
HWTEST_F(AbilityMsAppmsTest, AaFwk_AbilityMS_AppMS_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityMsAppMsTest_AaFwk_AbilityMS_AppMS_003 start";
    ResetAbilityRecord();
    int result = GetAbilityRecord()->LoadAbility();
    EXPECT_EQ(ERR_OK, result);
    auto state = GetAbilityRecord()->GetAbilityState();
    EXPECT_EQ(AAFwk::AbilityState::INITIAL, state);
    GTEST_LOG_(INFO) << "AbilityMsAppMsTest_AaFwk_AbilityMS_AppMS_003 end";
}

/*
 * Feature: Interaction of abilityms and appms
 * Function: Interaction of abilityms and appms
 * SubFunction: NA
 * FunctionPoints: MoveForeground
 * EnvConditions:NA
 * CaseDescription: 1. launcher ability is started.
 *                  2. the LoadAbility and MoveToForeground function of AppSchedule are called
 *                  3. the Active  function of AbilityRecord is called.
 *                  4. the state of launcher ability is ACTIVATING.
 */
HWTEST_F(AbilityMsAppmsTest, AaFwk_AbilityMS_AppMS_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityMsAppMsTest_AaFwk_AbilityMS_AppMS_004 start";
    auto handler = DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    sptr<AbilityScheduler> scheduler = new AbilityScheduler();
    std::shared_ptr<AbilityRecord> abilityRecord = nullptr;
    sptr<Token> token = nullptr;

    auto appScheduler = OHOS::DelayedSingleton<AppScheduler>::GetInstance();
    auto mockAppMgrClient = std::make_unique<MockAppMgrClient>();
    appScheduler->appMgrClient_.reset(mockAppMgrClient.get());
    EXPECT_CALL(*mockAppMgrClient, UpdateAbilityState(_, _)).Times(1);

    auto checkStateFun = [&scheduler, &token, &abilityRecord]() {
        auto stackManager = DelayedSingleton<AbilityManagerService>::GetInstance()->GetStackManager();
        abilityRecord = stackManager->GetCurrentTopAbility();
        token = abilityRecord->GetToken();
        DelayedSingleton<AbilityManagerService>::GetInstance()->AttachAbilityThread(scheduler, token);
    };

    handler->PostTask(checkStateFun);
    WaitUntilTaskFinished();
    GTEST_LOG_(INFO) << "AbilityMsAppMsTest_AaFwk_AbilityMS_AppMS_004 end";
}

/*
 * Feature: Interaction of abilityms and appms
 * Function: Interaction of abilityms and appms
 * SubFunction: NA
 * FunctionPoints: MoveBackground
 * EnvConditions:NA
 * CaseDescription: 1. launcher ability is started.
 *                  2. the LoadAbility and MoveToForeground function of AppSchedule are called
 *                  3. the Active  function of AbilityRecord is called.
 *                  4. the state of launcher ability is ACTIVATING.
 *                  5. perform ability to background
 *                  6. the MoveBackground fun of appSchedule is called.
 */
HWTEST_F(AbilityMsAppmsTest, AaFwk_AbilityMS_AppMS_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityMsAppMsTest_AaFwk_AbilityMS_AppMS_005 start";
    auto handler = DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();

    sptr<AbilityScheduler> scheduler = new AbilityScheduler();
    std::shared_ptr<AbilityRecord> sourceAbilityRecord = nullptr;
    std::shared_ptr<AbilityStackManager> stackManager = nullptr;
    sptr<Token> sourcetoken = nullptr;

    auto appScheduler = OHOS::DelayedSingleton<AppScheduler>::GetInstance();
    auto mockAppMgrClient = std::make_unique<MockAppMgrClient>();
    appScheduler->appMgrClient_.reset(mockAppMgrClient.get());
    EXPECT_CALL(*mockAppMgrClient, UpdateAbilityState(_, _)).Times(1);

    auto checkSourceActivtingState = [&stackManager, &sourceAbilityRecord, &scheduler, &sourcetoken]() {
        stackManager = DelayedSingleton<AbilityManagerService>::GetInstance()->GetStackManager();
        sourceAbilityRecord = stackManager->GetCurrentTopAbility();
        sourcetoken = sourceAbilityRecord->GetToken();
        DelayedSingleton<AbilityManagerService>::GetInstance()->AttachAbilityThread(scheduler, sourcetoken);
        auto sourceAbilityInfo = sourceAbilityRecord->GetAbilityInfo();
        EXPECT_EQ(sourceAbilityInfo.bundleName, "com.ix.hiworld");
    };
    handler->PostTask(checkSourceActivtingState);
    WaitUntilTaskFinished();
    GTEST_LOG_(INFO) << "AbilityMsAppMsTest_AaFwk_AbilityMS_AppMS_005 end";
}
}  // namespace AAFwk
}  // namespace OHOS