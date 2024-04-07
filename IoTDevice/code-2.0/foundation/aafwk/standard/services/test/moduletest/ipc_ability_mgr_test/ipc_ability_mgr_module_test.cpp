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

#include "gtest/gtest.h"
#include <unistd.h>

#include "mock_ability_mgr_service.h"
#include "ability_manager_interface.h"
#include "ohos/aafwk/content/want.h"
#include "iremote_object.h"
#include "mock_ability_scheduler.h"
#include "ability_record.h"
#include "ability_info.h"
#include "application_info.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using OHOS::iface_cast;
using OHOS::sptr;
using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;
class IpcAbilityMgrModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    AbilityRequest CreateAbilityRequest(const std::string &abilityName, const std::string &bundleName);

    static constexpr int COUNT = 5;
};

void IpcAbilityMgrModuleTest::SetUpTestCase()
{}

void IpcAbilityMgrModuleTest::TearDownTestCase()
{}

void IpcAbilityMgrModuleTest::SetUp()
{}

void IpcAbilityMgrModuleTest::TearDown()
{}

AbilityRequest IpcAbilityMgrModuleTest::CreateAbilityRequest(
    const std::string &abilityName, const std::string &bundleName)
{
    AbilityRequest abilityRequest;
    Want want;
    AbilityInfo abilityInfo;
    abilityInfo.name = abilityName;
    abilityInfo.applicationName = bundleName;
    abilityInfo.bundleName = bundleName;
    ApplicationInfo appInfo;
    appInfo.name = bundleName;
    appInfo.bundleName = bundleName;

    abilityRequest.want = want;
    abilityRequest.abilityInfo = abilityInfo;
    abilityRequest.appInfo = appInfo;
    return abilityRequest;
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: start ability
 * EnvConditions: NA
 * CaseDescription: verify StartAbility IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_001 start";
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);
        const Want want;

        EXPECT_CALL(*mockAbilityMgr, StartAbility(_, _))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->StartAbility(want, 0);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_001 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: AbilityTransitionDone
 * EnvConditions: NA
 * CaseDescription: verify AbilityTransitionDone IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_002 start";
    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest = CreateAbilityRequest(abilityName, bundleName);
    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        EXPECT_CALL(*mockAbilityMgr, AbilityTransitionDone(_, _))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->AbilityTransitionDone(abilityRecord->GetToken(), 0);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_002 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: ScheduleConnectAbilityDone
 * EnvConditions: NA
 * CaseDescription: verify ScheduleConnectAbilityDone IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_003 start";
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        EXPECT_CALL(*mockAbilityMgr, ScheduleConnectAbilityDone(_, _))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->ScheduleConnectAbilityDone(nullptr, nullptr);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_003 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: ScheduleDisconnectAbilityDone
 * EnvConditions: NA
 * CaseDescription: verify ScheduleDisconnectAbilityDone IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_004 start";
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        EXPECT_CALL(*mockAbilityMgr, ScheduleDisconnectAbilityDone(_))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->ScheduleDisconnectAbilityDone(nullptr);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_004 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: AddWindowInfo
 * EnvConditions: NA
 * CaseDescription: verify AddWindowInfo IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_005 start";
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        EXPECT_CALL(*mockAbilityMgr, AddWindowInfo(_, _))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::PostVoid));
        abilityMgrClient->AddWindowInfo(nullptr, 0);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_005 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: ConnectAbility
 * EnvConditions: NA
 * CaseDescription: verify ConnectAbility IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_006 start";
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);
        Want want;

        EXPECT_CALL(*mockAbilityMgr, ConnectAbility(_, _, _))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->ConnectAbility(want, nullptr, nullptr);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_006 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: DisconnectAbility
 * EnvConditions: NA
 * CaseDescription: verify DisconnectAbility IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_007 start";
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);
        Want want;

        EXPECT_CALL(*mockAbilityMgr, DisconnectAbility(_))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->DisconnectAbility(nullptr);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_007 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: DumpState
 * EnvConditions: NA
 * CaseDescription: verify DumpState IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_008 start";
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);
        std::vector<std::string> state;
        std::string args = "key";

        EXPECT_CALL(*mockAbilityMgr, DumpState(_, _))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::PostVoid));
        abilityMgrClient->DumpState(args, state);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_008 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: AttachAbilityThread
 * EnvConditions: NA
 * CaseDescription: verify AttachAbilityThread IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_009 start";
    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest = CreateAbilityRequest(abilityName, bundleName);
    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        EXPECT_CALL(*mockAbilityMgr, AttachAbilityThread(_, _))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->AttachAbilityThread(scheduler, abilityRecord->GetToken());
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_009 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: TerminateAbility
 * EnvConditions: NA
 * CaseDescription: verify TerminateAbility IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_010 start";
    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest = CreateAbilityRequest(abilityName, bundleName);
    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        EXPECT_CALL(*mockAbilityMgr, TerminateAbility(_, _, _))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->TerminateAbility(abilityRecord->GetToken(), -1, nullptr);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_010 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: GetAllStackInfo
 * EnvConditions: NA
 * CaseDescription: verify GetAllStackInfo IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_011 start";

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        StackInfo stackInfo;
        EXPECT_CALL(*mockAbilityMgr, GetAllStackInfo(_))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->GetAllStackInfo(stackInfo);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_011 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: ScheduleCommandAbilityDone
 * EnvConditions: NA
 * CaseDescription: verify ScheduleCommandAbilityDone IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_012 start";
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        EXPECT_CALL(*mockAbilityMgr, ScheduleCommandAbilityDone(_))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->ScheduleCommandAbilityDone(nullptr);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_012 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: StopServiceAbility
 * EnvConditions: NA
 * CaseDescription: verify StopServiceAbility IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_013 start";
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);
        const Want want;

        EXPECT_CALL(*mockAbilityMgr, StopServiceAbility(_))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->StopServiceAbility(want);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_013 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: GetAllStackInfo
 * EnvConditions: NA
 * CaseDescription: verify GetRecentMissions IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_014 start";

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        std::vector<RecentMissionInfo> renCentList;
        EXPECT_CALL(*mockAbilityMgr, GetRecentMissions(_, _, _))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->GetRecentMissions(10, 0, renCentList);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_014 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: GetMissionSnapshot
 * EnvConditions: NA
 * CaseDescription: verify GetMissionSnapshot IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_015 start";

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        MissionSnapshotInfo snapshot;
        EXPECT_CALL(*mockAbilityMgr, GetMissionSnapshot(_, _))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->GetMissionSnapshot(1, snapshot);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_015 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: RemoveMission
 * EnvConditions: NA
 * CaseDescription: verify RemoveMission IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_016 start";

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        EXPECT_CALL(*mockAbilityMgr, RemoveMission(_))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->RemoveMission(1);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_016 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: RemoveStack
 * EnvConditions: NA
 * CaseDescription: verify RemoveStack IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_017 start";

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        EXPECT_CALL(*mockAbilityMgr, RemoveStack(_))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->RemoveStack(1);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_017 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: MoveMissionToTop
 * EnvConditions: NA
 * CaseDescription: verify MoveMissionToTop IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_018 start";

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        EXPECT_CALL(*mockAbilityMgr, MoveMissionToTop(_))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityMgr.GetRefPtr(), &MockAbilityMgrService::Post));
        abilityMgrClient->MoveMissionToTop(1);
        mockAbilityMgr->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_018 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityManagerService
 * SubFunction: IPC of client and server
 * FunctionPoints: AcquireDataAbility
 * EnvConditions: NA
 * CaseDescription: verify data ability acquire IPC between client and server.
 */
HWTEST_F(IpcAbilityMgrModuleTest, AbilityMgrService_IPC_019, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_019 start";

    AbilityRequest abilityRequest;

    auto clientAbilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    Uri testUri("testuri");
    bool testTryBind = true;

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityMgrService> mockAbilityMgr(new MockAbilityMgrService());
        sptr<IAbilityManager> abilityMgrClient = iface_cast<IAbilityManager>(mockAbilityMgr);

        bool testResult = false;

        auto mockHandler = [&](const Uri &uri, bool tryBind, const sptr<IRemoteObject> &callerToken) {
            testResult = (uri.ToString() == testUri.ToString() && tryBind == testTryBind &&
                          Token::GetAbilityRecordByToken(callerToken) == clientAbilityRecord);
            mockAbilityMgr->Post();
            return nullptr;
        };

        EXPECT_CALL(*mockAbilityMgr, AcquireDataAbility(_, _, _)).Times(1).WillOnce(Invoke(mockHandler));

        abilityMgrClient->AcquireDataAbility(testUri, testTryBind, clientAbilityRecord->GetToken());

        mockAbilityMgr->Wait();

        EXPECT_TRUE(testResult);
    }

    GTEST_LOG_(INFO) << "IpcAbilityMgrModuleTest AbilityMgrService_IPC_019 end";
}