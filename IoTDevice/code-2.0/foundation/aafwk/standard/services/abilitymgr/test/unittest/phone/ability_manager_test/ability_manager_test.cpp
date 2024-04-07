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
#include "system_ability_definition.h"
#include "ability_manager_client.h"
#undef private
#undef protected

#include "ability_manager_stub_mock.h"
#include "ability_scheduler_mock.h"
#include "ability_manager_service.h"
#include "iservice_registry.h"
#include "ability_record.h"
#include "ability_scheduler.h"
#include "mock_ability_connect_callback.h"
#include "mock_bundle_manager.h"
#include "sa_mgr_client.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
namespace {
const std::string LAUNCHER_ABILITY = "MainAbility";
const std::string LAUNCHER_BUNDLE = "com.ohos.hiworld";
const std::string NAME_BUNDLE_MGR_SERVICE = "BundleMgrService";
const std::string NAME_ABILITY_MGR_SERVICE = "AbilityManagerService";
const std::string SERVICE_ABILITY = "ServiceAbility";
const std::string SERVICE_BUNDLE = "com.ohos.hiservcie";
}  // namespace

class AbilityManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

public:
    static constexpr int TEST_WAIT_TIME = 100000;
    std::shared_ptr<AbilityManagerClient> abilityClient_ = nullptr;
    sptr<AbilityManagerStubMock> mock_;
    Want want_;
};

void AbilityManagerTest::SetUpTestCase(void)
{}
void AbilityManagerTest::TearDownTestCase(void)
{}
void AbilityManagerTest::TearDown(void)
{}

void AbilityManagerTest::SetUp(void)
{
    abilityClient_ = std::make_shared<AbilityManagerClient>();
    mock_ = new AbilityManagerStubMock();
}

/*
 * Feature: AbilityManagerClient
 * Function: StartAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerClient StartAbility
 * EnvConditions: NA
 * CaseDescription: Verify the normal and abnormal cases of startability
 */
HWTEST_F(AbilityManagerTest, AAFWK_AbilityMS_AbilityManager_test_001, TestSize.Level2)
{
    abilityClient_->remoteObject_ = mock_;
    EXPECT_CALL(*mock_, StartAbility(::testing::_, ::testing::_)).Times(1);
    EXPECT_EQ(abilityClient_->StartAbility(want_), 0);

    abilityClient_->remoteObject_ = nullptr;
    EXPECT_EQ(abilityClient_->StartAbility(want_), ABILITY_SERVICE_NOT_CONNECTED);
}

/*
 * Feature: AbilityManagerClient
 * Function: AttachAbilityThread
 * SubFunction: NA
 * FunctionPoints: AbilityManagerClient AttachAbilityThread
 * EnvConditions: NA
 * CaseDescription: Verify the normal and abnormal cases of attachAbilityThread
 */
HWTEST_F(AbilityManagerTest, AAFWK_AbilityMS_AbilityManager_test_002, TestSize.Level2)
{
    abilityClient_->remoteObject_ = mock_;
    EXPECT_CALL(*mock_, AttachAbilityThread(::testing::_, ::testing::_)).Times(1);

    sptr<IAbilityScheduler> scheduler = new AbilitySchedulerMock();
    sptr<IRemoteObject> token = nullptr;
    EXPECT_EQ(abilityClient_->AttachAbilityThread(scheduler, token), 0);

    abilityClient_->remoteObject_ = nullptr;
    EXPECT_EQ(abilityClient_->AttachAbilityThread(scheduler, token), ABILITY_SERVICE_NOT_CONNECTED);
}

/*
 * Feature: AbilityManagerClient
 * Function: AbilityTransitionDone
 * SubFunction: NA
 * FunctionPoints: AbilityManagerClient AbilityTransitionDone
 * EnvConditions: NA
 * CaseDescription: Verify the normal and abnormal cases of abilityTransitionDone
 */
HWTEST_F(AbilityManagerTest, AAFWK_AbilityMS_AbilityManager_test_003, TestSize.Level2)
{
    abilityClient_->remoteObject_ = mock_;
    EXPECT_CALL(*mock_, AbilityTransitionDone(::testing::_, ::testing::_)).Times(1);

    sptr<IAbilityScheduler> scheduler = new AbilitySchedulerMock();
    sptr<IRemoteObject> token = nullptr;
    EXPECT_EQ(abilityClient_->AbilityTransitionDone(token, 1), 0);

    abilityClient_->remoteObject_ = nullptr;
    EXPECT_EQ(abilityClient_->AbilityTransitionDone(token, 1), ABILITY_SERVICE_NOT_CONNECTED);
}

/*
 * Feature: AbilityManagerClient
 * Function: ScheduleConnectAbilityDone
 * SubFunction: NA
 * FunctionPoints: AbilityManagerClient ScheduleConnectAbilityDone
 * EnvConditions: NA
 * CaseDescription: Verify the normal and abnormal cases of scheduleConnectAbilityDone
 */
HWTEST_F(AbilityManagerTest, AAFWK_AbilityMS_AbilityManager_test_004, TestSize.Level2)
{
    abilityClient_->remoteObject_ = mock_;
    EXPECT_CALL(*mock_, ScheduleConnectAbilityDone(::testing::_, ::testing::_)).Times(1);

    sptr<IAbilityScheduler> scheduler = new AbilitySchedulerMock();
    sptr<IRemoteObject> token = nullptr;
    sptr<IRemoteObject> remoteObject = nullptr;
    EXPECT_EQ(abilityClient_->ScheduleConnectAbilityDone(token, remoteObject), 0);

    abilityClient_->remoteObject_ = nullptr;
    EXPECT_EQ(abilityClient_->ScheduleConnectAbilityDone(token, remoteObject), ABILITY_SERVICE_NOT_CONNECTED);
}

/*
 * Feature: AbilityManagerClient
 * Function: ScheduleDisconnectAbilityDone
 * SubFunction: NA
 * FunctionPoints: AbilityManagerClient ScheduleDisconnectAbilityDone
 * EnvConditions: NA
 * CaseDescription: Verify the normal and abnormal cases of scheduleDisconnectAbilityDone
 */
HWTEST_F(AbilityManagerTest, AAFWK_AbilityMS_AbilityManager_test_005, TestSize.Level2)
{
    abilityClient_->remoteObject_ = mock_;
    EXPECT_CALL(*mock_, ScheduleDisconnectAbilityDone(::testing::_)).Times(1);

    sptr<IAbilityScheduler> scheduler = new AbilitySchedulerMock();
    sptr<IRemoteObject> token = nullptr;
    sptr<IRemoteObject> remoteObject = nullptr;
    EXPECT_EQ(abilityClient_->ScheduleDisconnectAbilityDone(token), 0);

    abilityClient_->remoteObject_ = nullptr;
    EXPECT_EQ(abilityClient_->ScheduleDisconnectAbilityDone(token), ABILITY_SERVICE_NOT_CONNECTED);
}

/*
 * Feature: AbilityManagerClient
 * Function: AddWindowInfo
 * SubFunction: NA
 * FunctionPoints: AbilityManagerClient AddWindowInfo
 * EnvConditions: NA
 * CaseDescription: Verify the normal and abnormal cases of addWindowInfo
 */
HWTEST_F(AbilityManagerTest, AAFWK_AbilityMS_AbilityManager_test_006, TestSize.Level2)
{
    std::string args;
    abilityClient_->remoteObject_ = mock_;
    EXPECT_CALL(*mock_, AddWindowInfo(::testing::_, ::testing::_)).Times(1);

    sptr<IAbilityScheduler> scheduler = new AbilitySchedulerMock();
    sptr<IRemoteObject> token = nullptr;
    int32_t t = 1;
    abilityClient_->AddWindowInfo(token, t);
    abilityClient_->remoteObject_ = nullptr;
    abilityClient_->AddWindowInfo(token, 1);
}

/*
 * Feature: AbilityManagerClient
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerClient TerminateAbility
 * EnvConditions: NA
 * CaseDescription: Verify the normal and abnormal cases of terminateAbility
 */
HWTEST_F(AbilityManagerTest, AAFWK_AbilityMS_AbilityManager_test_007, TestSize.Level2)
{
    abilityClient_->remoteObject_ = mock_;
    EXPECT_CALL(*mock_, TerminateAbility(::testing::_, ::testing::_, ::testing::_)).Times(1);

    sptr<IAbilityScheduler> scheduler = new AbilitySchedulerMock();
    sptr<IRemoteObject> token = nullptr;
    EXPECT_EQ(abilityClient_->TerminateAbility(token, 1, &want_), 0);

    abilityClient_->remoteObject_ = nullptr;
    EXPECT_EQ(abilityClient_->TerminateAbility(token, 1, &want_), ABILITY_SERVICE_NOT_CONNECTED);
}

/*
 * Feature: AbilityManagerClient
 * Function: ConnectAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerClient ConnectAbility
 * EnvConditions: NA
 * CaseDescription: Verify the normal and abnormal cases of ConnectAbility
 */
HWTEST_F(AbilityManagerTest, AAFWK_AbilityMS_AbilityManager_test_008, TestSize.Level2)
{
    abilityClient_->remoteObject_ = mock_;
    EXPECT_CALL(*mock_, ConnectAbility(::testing::_, ::testing::_, ::testing::_)).Times(1);

    sptr<IAbilityConnection> connect = nullptr;
    sptr<IRemoteObject> callerToken = nullptr;
    EXPECT_EQ(abilityClient_->ConnectAbility(want_, connect, callerToken), 0);

    abilityClient_->remoteObject_ = nullptr;
    EXPECT_EQ(abilityClient_->ConnectAbility(want_, connect, callerToken), ABILITY_SERVICE_NOT_CONNECTED);
}

/*
 * Feature: AbilityManagerClient
 * Function: DisconnectAbility
 * SubFunction: NA
 * FunctionPoints: AbilityManagerClient DisconnectAbility
 * EnvConditions: NA
 * CaseDescription: Verify the normal and abnormal cases of DisconnectAbility
 */
HWTEST_F(AbilityManagerTest, AAFWK_AbilityMS_AbilityManager_test_009, TestSize.Level2)
{
    abilityClient_->remoteObject_ = mock_;
    EXPECT_CALL(*mock_, DisconnectAbility(::testing::_)).Times(1);

    sptr<IAbilityConnection> connect = nullptr;
    EXPECT_EQ(abilityClient_->DisconnectAbility(connect), 0);

    abilityClient_->remoteObject_ = nullptr;
    EXPECT_EQ(abilityClient_->DisconnectAbility(connect), ABILITY_SERVICE_NOT_CONNECTED);
}

/*
 * Feature: AbilityManagerClient
 * Function: DumpState
 * SubFunction: NA
 * FunctionPoints: AbilityManagerClient DumpState
 * EnvConditions: NA
 * CaseDescription: Verify the normal and abnormal cases of DumpState
 */
HWTEST_F(AbilityManagerTest, AAFWK_AbilityMS_AbilityManager_test_010, TestSize.Level2)
{
    std::string args;
    std::vector<std::string> vec;
    abilityClient_->remoteObject_ = mock_;
    EXPECT_CALL(*mock_, DumpState(::testing::_, ::testing::_)).Times(1);

    sptr<IAbilityConnection> connect = nullptr;
    EXPECT_EQ(abilityClient_->DumpState(args, vec), 0);

    abilityClient_->remoteObject_ = nullptr;
    EXPECT_EQ(abilityClient_->DumpState(args, vec), ABILITY_SERVICE_NOT_CONNECTED);
}

/*
 * Feature: AbilityManagerClient
 * Function: GetAllStackInfo
 * SubFunction: NA
 * FunctionPoints: AbilityManagerClient GetAllStackInfo
 * EnvConditions: NA
 * CaseDescription: Verify the normal and abnormal cases of GetAllStackInfo
 */
HWTEST_F(AbilityManagerTest, AAFWK_AbilityMS_AbilityManager_test_011, TestSize.Level2)
{
    std::string args;
    std::vector<std::string> vec;
    abilityClient_->remoteObject_ = mock_;
    EXPECT_CALL(*mock_, GetAllStackInfo(::testing::_)).Times(1);

    StackInfo info;
    EXPECT_EQ(abilityClient_->GetAllStackInfo(info), 0);

    abilityClient_->remoteObject_ = nullptr;
    EXPECT_EQ(abilityClient_->GetAllStackInfo(info), ABILITY_SERVICE_NOT_CONNECTED);
}

/*
 * Feature: AbilityManagerClient
 * Function: Connect
 * SubFunction: NA
 * FunctionPoints: AbilityManagerClient Connect
 * EnvConditions: NA
 * CaseDescription: Verify Connect operation
 */
HWTEST_F(AbilityManagerTest, AAFWK_AbilityMS_AbilityManager_test_012, TestSize.Level2)
{
    abilityClient_->remoteObject_ = mock_;
    EXPECT_EQ(abilityClient_->Connect(), 0);

    sptr<ISystemAbilityManager> manager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ISystemAbilityManager::SAExtraProp prop;
    manager->AddSystemAbility(ABILITY_MGR_SERVICE_ID, mock_, prop);

    abilityClient_->remoteObject_ = nullptr;

    EXPECT_EQ(abilityClient_->Connect(), 0);
}
}  // namespace AAFwk
}  // namespace OHOS