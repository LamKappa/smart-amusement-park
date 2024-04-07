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

#include <string_view>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "want.h"

#define private public
#include "app_scheduler.h"
#include "ability_record.h"
#include "mission_record.h"
#include "connection_record.h"
#include "mock_app_mgr_client.h"
#include "mock_ability_scheduler_stub.h"
#undef private

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using OHOS::iface_cast;
using OHOS::sptr;
using testing::_;
using testing::Invoke;
using testing::Return;

namespace {
constexpr int COUNT = 1000;
}  // namespace

namespace OHOS::AppExecFwk {
bool operator==(const AbilityInfo &a, const AbilityInfo &b)
{
    if (&a != &b) {
        return a.package == b.package && a.name == b.name && a.label == b.label && a.description == b.description &&
               a.iconPath == b.iconPath && a.visible == b.visible && a.kind == b.kind &&
               a.permissions == b.permissions && a.bundleName == b.bundleName &&
               a.applicationName == b.applicationName && a.deviceId == b.deviceId && a.codePath == b.codePath &&
               a.resourcePath == b.resourcePath && a.libPath == b.libPath;
    }

    return true;
}

bool operator!=(const AbilityInfo &a, const AbilityInfo &b)
{
    return !(a == b);
}

bool operator==(const ApplicationInfo &a, const ApplicationInfo &b)
{
    if (&a != &b) {
        return a.name == b.name && a.bundleName == b.bundleName && a.deviceId == b.deviceId &&
               a.signatureKey == b.signatureKey;
    }

    return true;
}

bool operator!=(const ApplicationInfo &a, const ApplicationInfo &b)
{
    return !(a == b);
}

}  // namespace OHOS::AppExecFwk

namespace OHOS::AAFwk {

bool operator==(const Want &a, const Want &b)
{
    if (&a != &b) {
        return a.GetAction() == b.GetAction() && a.GetEntities() == b.GetEntities();
    }

    return true;
}

bool operator!=(const Want &a, const Want &b)
{
    return !(a == b);
}

}  // namespace OHOS::AAFwk

namespace OHOS {
namespace AAFwk {
class AbilityRecordModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    static const AbilityRequest &MakeDefaultAbilityRequest();
    static const AbilityRequest &MakeHomeAbilityRequest();

private:
    inline static AbilityRequest testAbilityRequest_;
};

void AbilityRecordModuleTest::SetUpTestCase()
{
    testAbilityRequest_.requestCode = 123;

    testAbilityRequest_.abilityInfo.package = "test";
    testAbilityRequest_.abilityInfo.name = "test";
    testAbilityRequest_.abilityInfo.label = "test";
    testAbilityRequest_.abilityInfo.description = "test";
    testAbilityRequest_.abilityInfo.iconPath = "/test";
    testAbilityRequest_.abilityInfo.visible = false;
    testAbilityRequest_.abilityInfo.kind = "page";
    testAbilityRequest_.abilityInfo.permissions = {};
    testAbilityRequest_.abilityInfo.bundleName = "test";
    testAbilityRequest_.abilityInfo.applicationName = "test";
    testAbilityRequest_.abilityInfo.deviceId = "test";
    testAbilityRequest_.abilityInfo.codePath = "/test";
    testAbilityRequest_.abilityInfo.resourcePath = "/test";
    testAbilityRequest_.abilityInfo.libPath = "/test";

    testAbilityRequest_.appInfo.name = "test";
    testAbilityRequest_.appInfo.bundleName = "test";
    testAbilityRequest_.appInfo.deviceId = "test";
    testAbilityRequest_.appInfo.signatureKey = "test";
}

void AbilityRecordModuleTest::TearDownTestCase()
{}

void AbilityRecordModuleTest::SetUp()
{}

void AbilityRecordModuleTest::TearDown()
{}

const AbilityRequest &AbilityRecordModuleTest::MakeDefaultAbilityRequest()
{
    Want::ClearWant(&testAbilityRequest_.want);
    testAbilityRequest_.want.SetAction("test");
    testAbilityRequest_.want.AddEntity("test");

    return testAbilityRequest_;
}

const AbilityRequest &AbilityRecordModuleTest::MakeHomeAbilityRequest()
{
    Want::ClearWant(&testAbilityRequest_.want);
    testAbilityRequest_.want.SetAction(Want::ACTION_HOME);
    testAbilityRequest_.want.AddEntity(Want::ENTITY_HOME);
    testAbilityRequest_.appInfo.isLauncherApp = true;

    return testAbilityRequest_;
}

/*
 * Feature: AbilityRecord
 * Function: Init
 * SubFunction: Init/CreateAbilityRecord
 * FunctionPoints: Normal AbilityRecord
 * CaseDescription: Create a normal 'AbilityRecord' instance, and check its state.
 */
HWTEST_F(AbilityRecordModuleTest, Init_001, TestSize.Level2)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    for (int i = 0; i < COUNT; ++i) {
        auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
        ASSERT_TRUE(abilityRecord);
        EXPECT_TRUE(abilityRecord->GetToken());
        EXPECT_TRUE(abilityRecord->GetRecordId() >= 0);
        EXPECT_FALSE(abilityRecord->IsLauncherAbility());
        EXPECT_EQ(abilityRecord->GetAbilityState(), INITIAL);
        EXPECT_EQ(abilityRecord->GetRequestCode(), abilityRequest.requestCode);
        EXPECT_EQ(abilityRecord->GetAbilityInfo(), abilityRequest.abilityInfo);
        EXPECT_EQ(abilityRecord->GetApplicationInfo(), abilityRequest.appInfo);
    }
}

/*
 * Feature: AbilityRecord
 * Function: Init
 * SubFunction: Init/CreateAbilityRecord
 * FunctionPoints: Launcher AbilityRecord
 * CaseDescription: Create a launcher 'AbilityRecord' instance, and check its state (includes 'IsLauncherAbility()).
 */
HWTEST_F(AbilityRecordModuleTest, Init_002, TestSize.Level2)
{
    auto &abilityRequest = MakeHomeAbilityRequest();

    for (int i = 0; i < COUNT; ++i) {
        auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
        ASSERT_TRUE(abilityRecord);
        EXPECT_TRUE(abilityRecord->GetToken());
        EXPECT_TRUE(abilityRecord->IsLauncherAbility());
        EXPECT_EQ(abilityRecord->GetAbilityState(), INITIAL);
        EXPECT_EQ(abilityRecord->GetRequestCode(), abilityRequest.requestCode);
        EXPECT_EQ(abilityRecord->GetAbilityInfo(), abilityRequest.abilityInfo);
        EXPECT_EQ(abilityRecord->GetApplicationInfo(), abilityRequest.appInfo);
    }
}

/*
 * Feature: AbilityRecord
 * Function: LoadAbility
 * SubFunction: N/A
 * FunctionPoints: Load a test ability
 * CaseDescription: Load a test ability and check state in 'AppMgrClient' mocker.
 */
HWTEST_F(AbilityRecordModuleTest, LoadAbility_001, TestSize.Level3)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    for (int i = 0; i < COUNT; ++i) {
        auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
        ASSERT_TRUE(abilityRecord);

        std::unique_ptr<MockAppMgrClient> mockAppMgrClient(new MockAppMgrClient);
        ASSERT_TRUE(mockAppMgrClient);

        auto mockHandler = [&](const sptr<IRemoteObject> &token,
                               const sptr<IRemoteObject> &preToken,
                               const AbilityInfo &abilityInfo,
                               const ApplicationInfo &appInfo) {
            if (abilityInfo != abilityRequest.abilityInfo) {
                return static_cast<AppMgrResultCode>(ERR_INVALID_VALUE);
            }
            if (appInfo != abilityRequest.appInfo) {
                return static_cast<AppMgrResultCode>(ERR_INVALID_VALUE);
            }
            return AppExecFwk::RESULT_OK;
        };

        EXPECT_CALL(*mockAppMgrClient, LoadAbility(_, _, _, _)).Times(1).WillOnce(Invoke(mockHandler));

        auto appScheduler = DelayedSingleton<AppScheduler>::GetInstance();
        auto backupAppMgrClient = std::move(appScheduler->appMgrClient_);
        appScheduler->appMgrClient_ = std::move(mockAppMgrClient);

        auto result = abilityRecord->LoadAbility();
        EXPECT_EQ(result, ERR_OK);

        EXPECT_EQ(abilityRecord->GetAbilityInfo(), abilityRequest.abilityInfo);
        EXPECT_EQ(abilityRecord->GetApplicationInfo(), abilityRequest.appInfo);

        appScheduler->appMgrClient_ = std::move(backupAppMgrClient);
    }
}

/*
 * Feature: AbilityRecord
 * Function: TerminateAbility
 * SubFunction: N/A
 * FunctionPoints: Terminate a ability
 * CaseDescription: Terminate a test ability and check state in 'AppMgrClient' mocker.
 */
HWTEST_F(AbilityRecordModuleTest, TerminateAbility_001, TestSize.Level3)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    for (int i = 0; i < COUNT; ++i) {
        auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
        ASSERT_TRUE(abilityRecord);

        auto mockAppMgrClient = std::make_unique<MockAppMgrClient>();
        ASSERT_TRUE(mockAppMgrClient);

        EXPECT_CALL(*mockAppMgrClient, TerminateAbility(_)).Times(1).WillOnce(Return(RESULT_OK));

        auto appScheduler = DelayedSingleton<AppScheduler>::GetInstance();
        auto backupAppMgrClient = std::move(appScheduler->appMgrClient_);
        appScheduler->appMgrClient_ = std::move(mockAppMgrClient);

        auto result = abilityRecord->TerminateAbility();
        EXPECT_EQ(result, ERR_OK);

        appScheduler->appMgrClient_ = std::move(backupAppMgrClient);
    }
}

/*
 * Feature: AbilityRecord
 * Function: Scheduler
 * SubFunction: Activate/Inactivate/MoveToBackground/Terminate/ConnectAbility/DisconnectAbility/SendResult
 * FunctionPoints: Check scheduler work flow.
 * CaseDescription: Change ability state and check if the work flow reachs the 'AbilityScheduler' mocker.
 */
HWTEST_F(AbilityRecordModuleTest, AbilityScheduler_001, TestSize.Level3)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();
    auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);

    sptr<MockAbilitySchedulerStub> mockAbilityScheduerStub(new MockAbilitySchedulerStub);
    ASSERT_TRUE(mockAbilityScheduerStub);

    bool testResult = false;
    abilityRecord->SetScheduler(mockAbilityScheduerStub);
    EXPECT_TRUE(abilityRecord->IsReady());

    for (int i = 0; i < COUNT; ++i) {
        // Activate
        auto mockActivateHandler = [&](const Want &want, const LifeCycleStateInfo &lifeCycleStateInfo) {
            testResult = (lifeCycleStateInfo.state == AbilityLifeCycleState::ABILITY_STATE_ACTIVE);
        };
        testResult = false;
        EXPECT_CALL(*mockAbilityScheduerStub, ScheduleAbilityTransaction(_, _))
            .Times(1)
            .WillOnce(Invoke(mockActivateHandler));

        abilityRecord->Activate();
        ASSERT_TRUE(testResult);
        EXPECT_EQ(abilityRecord->GetAbilityState(), ACTIVATING);

        // Inactivate
        testResult = false;
        auto mockInactivateHandler = [&](const Want &want, const LifeCycleStateInfo &lifeCycleStateInfo) {
            testResult = (lifeCycleStateInfo.state == AbilityLifeCycleState::ABILITY_STATE_INACTIVE);
        };
        EXPECT_CALL(*mockAbilityScheduerStub, ScheduleAbilityTransaction(_, _))
            .Times(1)
            .WillOnce(Invoke(mockInactivateHandler));

        abilityRecord->Inactivate();
        EXPECT_TRUE(testResult);
        EXPECT_EQ(abilityRecord->GetAbilityState(), INACTIVATING);

        // MoveToBackground
        testResult = false;
        auto mockBackgroundHandler = [&](const Want &want, const LifeCycleStateInfo &lifeCycleStateInfo) {
            testResult = (lifeCycleStateInfo.state == AbilityLifeCycleState::ABILITY_STATE_BACKGROUND);
        };
        EXPECT_CALL(*mockAbilityScheduerStub, ScheduleAbilityTransaction(_, _))
            .Times(1)
            .WillOnce(Invoke(mockBackgroundHandler));

        abilityRecord->MoveToBackground([] {});
        EXPECT_TRUE(testResult);
        EXPECT_EQ(abilityRecord->GetAbilityState(), MOVING_BACKGROUND);

        // Terminate
        EXPECT_CALL(*mockAbilityScheduerStub, ScheduleAbilityTransaction(_, _)).Times(1);
        abilityRecord->Terminate([] {});
        EXPECT_EQ(abilityRecord->GetAbilityState(), TERMINATING);

        // Connect
        testResult = false;
        auto mockConnectHandler = [&](const Want &want) { testResult = want == abilityRequest.want; };
        EXPECT_CALL(*mockAbilityScheduerStub, ScheduleConnectAbility(_)).Times(1).WillOnce(Invoke(mockConnectHandler));
        abilityRecord->ConnectAbility();
        EXPECT_TRUE(testResult);

        // Disconnect
        testResult = false;
        auto mockDisconnectHandler = [&](const Want &want) { testResult = want == abilityRequest.want; };
        EXPECT_CALL(*mockAbilityScheduerStub, ScheduleDisconnectAbility(_))
            .Times(1)
            .WillOnce(Invoke(mockDisconnectHandler));
        abilityRecord->DisconnectAbility();
        EXPECT_TRUE(testResult);

        // SendResult
        testResult = false;
        int testResultCode = 123;
        auto mockSendResultHandler = [&](int requestCode, int resultCode, const Want &want) {
            testResult = requestCode == abilityRequest.requestCode && resultCode == testResultCode &&
                         want == abilityRequest.want;
        };
        EXPECT_CALL(*mockAbilityScheduerStub, SendResult(_, _, _)).Times(1).WillOnce(Invoke(mockSendResultHandler));
        auto abilityResult =
            std::make_shared<AbilityResult>(abilityRequest.requestCode, testResultCode, abilityRequest.want);
        ASSERT_TRUE(abilityResult);
        abilityRecord->SetResult(abilityResult);
        abilityRecord->SendResult();
        EXPECT_TRUE(testResult);
    }

    abilityRecord->SetScheduler(nullptr);
}

/*
 * Feature: AbilityRecord
 * Function: MissionRecord
 * SubFunction: SetMissionRecord/GetMissionRecord
 * FunctionPoints: Mission record getter and setter
 * CaseDescription: Check the mission record getter and setter.
 */
HWTEST_F(AbilityRecordModuleTest, MissionRecord_001, TestSize.Level1)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);

    auto missionRecord = std::make_shared<MissionRecord>("test");
    ASSERT_TRUE(missionRecord);

    abilityRecord->SetMissionRecord(missionRecord);
    EXPECT_EQ(missionRecord, abilityRecord->GetMissionRecord());
}

/*
 * Feature: AbilityRecord
 * Function: Pre and Next Abilities
 * SubFunction: SetPreAbilityRecord/GetPreAbility/SetNextAbilityRecord/GetNextAbilityRecord
 * FunctionPoints: Pre and Next ability record getter and setter
 * CaseDescription: Check the pre and next ability record getter and setter.
 */
HWTEST_F(AbilityRecordModuleTest, PreNextAbilities_001, TestSize.Level1)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);

    auto preAbilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(preAbilityRecord);

    auto nextAbilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(nextAbilityRecord);

    for (int i = 0; i < COUNT; ++i) {
        abilityRecord->SetPreAbilityRecord(preAbilityRecord);
        abilityRecord->SetNextAbilityRecord(nextAbilityRecord);
        EXPECT_EQ(abilityRecord->GetPreAbilityRecord(), preAbilityRecord);
        EXPECT_EQ(abilityRecord->GetNextAbilityRecord(), nextAbilityRecord);
    }
}

/*
 * Feature: AbilityRecord
 * Function: EventId
 * SubFunction: SetEventId/GetEventId
 * FunctionPoints: Event id getter and setter
 * CaseDescription: Check ability record event id getter and setter.
 */
HWTEST_F(AbilityRecordModuleTest, EventId_001, TestSize.Level1)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);

    for (int i = 0; i < COUNT; ++i) {
        abilityRecord->SetEventId(i);
        EXPECT_EQ(abilityRecord->GetEventId(), i);
    }
}

/*
 * Feature: AbilityRecord
 * Function: AbilityState
 * SubFunction: SetAbilityState/GetAbilityState
 * FunctionPoints: Ability state getter and setter
 * CaseDescription: Check ability state getter and setter.
 */
HWTEST_F(AbilityRecordModuleTest, AbilityState_001, TestSize.Level1)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);

    for (int i = 0; i < COUNT; ++i) {
        abilityRecord->SetAbilityState(INITIAL);
        EXPECT_EQ(abilityRecord->GetAbilityState(), INITIAL);
    }
}

/*
 * Feature: AbilityRecord
 * Function: Want
 * SubFunction: SetWant/GetWant
 * FunctionPoints: Ability want getter and setter
 * CaseDescription: Check ability want getter and setter.
 */
HWTEST_F(AbilityRecordModuleTest, Want_001, TestSize.Level1)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);

    auto initWant = abilityRecord->GetWant();
    EXPECT_EQ(initWant, abilityRequest.want);

    Want setWant;
    setWant.SetAction("newAction");
    setWant.AddEntity("newEntity");

    for (int i = 0; i < COUNT; ++i) {
        abilityRecord->SetWant(setWant);
        auto newWant = abilityRecord->GetWant();
        EXPECT_EQ(setWant, newWant);
    }
}

/*
 * Feature: AbilityRecord
 * Function: ConnectionRecord
 * SubFunction: AddConnectRecordToList/GetConnectRecordList/GetConnectingRecord/GetDisconnectingRecord
 * FunctionPoints: Ability connect record getter and setter
 * CaseDescription: Check ability connect record getter and setter.
 */
HWTEST_F(AbilityRecordModuleTest, ConnectionRecord_001, TestSize.Level2)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);
    EXPECT_TRUE(abilityRecord->IsConnectListEmpty());

    auto connectionRecord = ConnectionRecord::CreateConnectionRecord(nullptr, nullptr, nullptr);
    ASSERT_TRUE(connectionRecord);

    for (int i = 0; i < COUNT; ++i) {
        abilityRecord->AddConnectRecordToList(connectionRecord);
        EXPECT_FALSE(abilityRecord->IsConnectListEmpty());
        auto connectionRecordList = abilityRecord->GetConnectRecordList();
        auto it = std::find(connectionRecordList.begin(), connectionRecordList.end(), connectionRecord);
        EXPECT_TRUE(it != connectionRecordList.end());

        EXPECT_FALSE(abilityRecord->GetConnectingRecord());
        connectionRecord->SetConnectState(ConnectionState::CONNECTING);
        EXPECT_TRUE(abilityRecord->GetConnectingRecord());

        EXPECT_FALSE(abilityRecord->GetDisconnectingRecord());
        connectionRecord->SetConnectState(ConnectionState::DISCONNECTING);
        EXPECT_TRUE(abilityRecord->GetDisconnectingRecord());

        abilityRecord->RemoveConnectRecordFromList(connectionRecord);
        EXPECT_TRUE(abilityRecord->IsConnectListEmpty());
        connectionRecordList = abilityRecord->GetConnectRecordList();
        it = std::find(connectionRecordList.begin(), connectionRecordList.end(), connectionRecord);
        EXPECT_TRUE(it == connectionRecordList.end());
    }
}

/*
 * Feature: AbilityRecord
 * Function: ConnectionRecord
 * SubFunction: AddWindowInfo/RemoveWindowInfo/GetWindowInfo
 * FunctionPoints: Ability window info getter and setter
 * CaseDescription: Check ability window info getter and setter.
 */
HWTEST_F(AbilityRecordModuleTest, WindowInfo_001, TestSize.Level1)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);
    EXPECT_FALSE(abilityRecord->GetWindowInfo());

    for (int i = 0; i < COUNT; ++i) {
        abilityRecord->AddWindowInfo(123);
        EXPECT_TRUE(abilityRecord->GetWindowInfo());
        abilityRecord->RemoveWindowInfo();
        EXPECT_FALSE(abilityRecord->GetWindowInfo());
    }
}

/*
 * Feature: AbilityRecord
 * Function: Terminating
 * SubFunction: IsTerminating/SetTerminatingState
 * FunctionPoints: Ability terminating state
 * CaseDescription: Check ability terminating state.
 */
HWTEST_F(AbilityRecordModuleTest, Terminating_001, TestSize.Level1)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);

    EXPECT_FALSE(abilityRecord->IsTerminating());
    abilityRecord->SetTerminatingState();
    EXPECT_TRUE(abilityRecord->IsTerminating());
}

/*
 * Feature: AbilityRecord
 * Function: NewWant
 * SubFunction: IsNewWant/SetIsNewWant
 * FunctionPoints: Ability 'IsNewWant' state
 * CaseDescription: Check ability 'IsNewWant' state.
 */
HWTEST_F(AbilityRecordModuleTest, NewWant_001, TestSize.Level1)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);
    EXPECT_FALSE(abilityRecord->IsNewWant());

    for (int i = 0; i < COUNT; ++i) {
        abilityRecord->SetIsNewWant(true);
        EXPECT_TRUE(abilityRecord->IsNewWant());
        abilityRecord->SetIsNewWant(false);
        EXPECT_FALSE(abilityRecord->IsNewWant());
    }
}

/*
 * Feature: AbilityRecord
 * Function: ConvertAbilityState
 * SubFunction: N/A
 * FunctionPoints: ConvertAbilityState
 * CaseDescription: Check function 'ConvertAbilityState'.
 */
HWTEST_F(AbilityRecordModuleTest, ConvertAbilityState_001, TestSize.Level1)
{
    static std::string result;

    result = AbilityRecord::ConvertAbilityState(INITIAL);
    EXPECT_EQ(result, "INITIAL");

    result = AbilityRecord::ConvertAbilityState(INACTIVE);
    EXPECT_EQ(result, "INACTIVE");

    result = AbilityRecord::ConvertAbilityState(ACTIVE);
    EXPECT_EQ(result, "ACTIVE");

    result = AbilityRecord::ConvertAbilityState(BACKGROUND);
    EXPECT_EQ(result, "BACKGROUND");

    result = AbilityRecord::ConvertAbilityState(SUSPENDED);
    EXPECT_EQ(result, "SUSPENDED");

    result = AbilityRecord::ConvertAbilityState(INACTIVATING);
    EXPECT_EQ(result, "INACTIVATING");

    result = AbilityRecord::ConvertAbilityState(ACTIVATING);
    EXPECT_EQ(result, "ACTIVATING");

    result = AbilityRecord::ConvertAbilityState(MOVING_BACKGROUND);
    EXPECT_EQ(result, "MOVING_BACKGROUND");

    result = AbilityRecord::ConvertAbilityState(TERMINATING);
    EXPECT_EQ(result, "TERMINATING");
}

/*
 * Feature: AbilityRecord
 * Function: ConvertLifeCycleToAbilityState
 * SubFunction: N/A
 * FunctionPoints: ConvertLifeCycleToAbilityState
 * CaseDescription: Check function 'ConvertLifeCycleToAbilityState'.
 */
HWTEST_F(AbilityRecordModuleTest, ConvertLifeCycle_001, TestSize.Level1)
{
    int result;

    result = AbilityRecord::ConvertLifeCycleToAbilityState(ABILITY_STATE_INITIAL);
    EXPECT_EQ(result, INITIAL);

    result = AbilityRecord::ConvertLifeCycleToAbilityState(ABILITY_STATE_INACTIVE);
    EXPECT_EQ(result, INACTIVE);

    result = AbilityRecord::ConvertLifeCycleToAbilityState(ABILITY_STATE_ACTIVE);
    EXPECT_EQ(result, ACTIVE);

    result = AbilityRecord::ConvertLifeCycleToAbilityState(ABILITY_STATE_BACKGROUND);
    EXPECT_EQ(result, BACKGROUND);

    result = AbilityRecord::ConvertLifeCycleToAbilityState(ABILITY_STATE_SUSPENDED);
    EXPECT_EQ(result, SUSPENDED);

    result = AbilityRecord::ConvertLifeCycleToAbilityState(static_cast<AbilityLifeCycleState>(-1));
    EXPECT_EQ(result, -1);
}

/*
 * Feature: AbilityRecord
 * Function: Dump
 * SubFunction: N/A
 * FunctionPoints: Dump
 * CaseDescription: Check 'Dump' result.
 */
HWTEST_F(AbilityRecordModuleTest, Dump_001, TestSize.Level2)
{
    auto &abilityRequest = MakeDefaultAbilityRequest();

    auto abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);

    std::vector<std::string> info;
    abilityRecord->Dump(info);
    EXPECT_FALSE(info.empty());
}
}  // namespace AAFwk
}  // namespace OHOS