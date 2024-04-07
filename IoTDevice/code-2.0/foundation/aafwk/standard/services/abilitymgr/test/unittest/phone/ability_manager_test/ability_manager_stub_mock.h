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

#ifndef ABILITY_UNITTEST_ABILITY_MANAGER_STUB_MOCK_H
#define ABILITY_UNITTEST_ABILITY_MANAGER_STUB_MOCK_H
#include <gmock/gmock.h>
#include <iremote_object.h>
#include <iremote_stub.h>
#include "hilog_wrapper.h"
#include "ability_manager_interface.h"

namespace OHOS {
namespace AAFwk {
class AbilityManagerStubMock : public IRemoteStub<IAbilityManager> {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"IAbilityManagerMock");

    AbilityManagerStubMock()
    {}
    virtual ~AbilityManagerStubMock()
    {}

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));
    MOCK_METHOD2(StartAbility, int(const Want &, int));
    MOCK_METHOD3(TerminateAbility, int(const sptr<IRemoteObject> &, int, const Want *));
    MOCK_METHOD3(ConnectAbility, int(const Want &, const sptr<IAbilityConnection> &, const sptr<IRemoteObject> &));

    MOCK_METHOD1(DisconnectAbility, int(const sptr<IAbilityConnection> &));
    MOCK_METHOD3(AcquireDataAbility, sptr<IAbilityScheduler>(const Uri &, bool, const sptr<IRemoteObject> &));
    MOCK_METHOD2(ReleaseDataAbility, int(sptr<IAbilityScheduler>, const sptr<IRemoteObject> &));
    MOCK_METHOD2(AddWindowInfo, void(const sptr<IRemoteObject> &, int32_t));
    MOCK_METHOD2(AttachAbilityThread, int(const sptr<IAbilityScheduler> &, const sptr<IRemoteObject> &));
    MOCK_METHOD2(AbilityTransitionDone, int(const sptr<IRemoteObject> &, int));
    MOCK_METHOD2(ScheduleConnectAbilityDone, int(const sptr<IRemoteObject> &, const sptr<IRemoteObject> &));
    MOCK_METHOD1(ScheduleDisconnectAbilityDone, int(const sptr<IRemoteObject> &));
    MOCK_METHOD1(ScheduleCommandAbilityDone, int(const sptr<IRemoteObject> &));
    MOCK_METHOD2(DumpState, void(const std::string &, std::vector<std::string> &));
    MOCK_METHOD2(TerminateAbilityResult, int(const sptr<IRemoteObject> &, int));
    MOCK_METHOD1(StopServiceAbility, int(const Want &));
    MOCK_METHOD1(GetAllStackInfo, int(StackInfo &));
    MOCK_METHOD3(GetRecentMissions, int(const int32_t, const int32_t, std::vector<RecentMissionInfo> &));
    MOCK_METHOD2(GetMissionSnapshot, int(const int32_t, MissionSnapshotInfo &));
    MOCK_METHOD1(RemoveMission, int(int));
    MOCK_METHOD1(RemoveStack, int(int));
    MOCK_METHOD1(MoveMissionToTop, int(int32_t));
    MOCK_METHOD1(KillProcess, int(const std::string &));
    MOCK_METHOD1(UninstallApp, int(const std::string &));
    MOCK_METHOD2(TerminateAbilityByCaller, int(const sptr<IRemoteObject> &callerToken, int requestCode));
    MOCK_METHOD3(StartAbility, int(const Want &want, const sptr<IRemoteObject> &callerToken, int requestCode));
};

}  // namespace AAFwk
}  // namespace OHOS

#endif