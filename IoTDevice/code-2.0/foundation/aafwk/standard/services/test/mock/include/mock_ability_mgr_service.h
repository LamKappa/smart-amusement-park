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

#ifndef FOUNDATION_AAFWK_SERVICES_MOCK_ABILITY_MGR_SERVICE_H
#define FOUNDATION_AAFWK_SERVICES_MOCK_ABILITY_MGR_SERVICE_H

#include "gmock/gmock.h"
#include "semaphore_ex.h"
#include "ability_manager_stub.h"

namespace OHOS {
namespace AAFwk {
class MockAbilityMgrService : public AbilityManagerStub {
public:
    MOCK_METHOD2(StartAbility, int(const Want &want, int requestCode));
    MOCK_METHOD3(StartAbility, int(const Want &want, const sptr<IRemoteObject> &callerToken, int requestCode));
    MOCK_METHOD2(TerminateAbilityByCaller, int(const sptr<IRemoteObject> &callerToken, int requestCode));
    MOCK_METHOD3(TerminateAbility, int(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant));
    MOCK_METHOD3(ConnectAbility,
        int(const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken));
    MOCK_METHOD1(DisconnectAbility, int(const sptr<IAbilityConnection> &connect));
    MOCK_METHOD3(AcquireDataAbility, sptr<IAbilityScheduler>(const Uri &, bool, const sptr<IRemoteObject> &));
    MOCK_METHOD2(ReleaseDataAbility, int(sptr<IAbilityScheduler>, const sptr<IRemoteObject> &));
    MOCK_METHOD2(AddWindowInfo, void(const sptr<IRemoteObject> &token, int32_t windowToken));
    MOCK_METHOD2(AttachAbilityThread, int(const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token));
    MOCK_METHOD2(AbilityTransitionDone, int(const sptr<IRemoteObject> &token, int state));
    MOCK_METHOD2(
        ScheduleConnectAbilityDone, int(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject));
    MOCK_METHOD1(ScheduleDisconnectAbilityDone, int(const sptr<IRemoteObject> &token));
    MOCK_METHOD1(ScheduleCommandAbilityDone, int(const sptr<IRemoteObject> &));
    MOCK_METHOD2(DumpState, void(const std::string &args, std::vector<std::string> &state));
    MOCK_METHOD2(TerminateAbilityResult, int(const sptr<IRemoteObject> &, int startId));
    MOCK_METHOD1(StopServiceAbility, int(const Want &));
    MOCK_METHOD1(GetAllStackInfo, int(StackInfo &stackInfo));
    MOCK_METHOD3(GetRecentMissions, int(const int32_t, const int32_t, std::vector<RecentMissionInfo> &));
    MOCK_METHOD2(GetMissionSnapshot, int(const int32_t, MissionSnapshotInfo &));
    MOCK_METHOD1(RemoveMission, int(int));
    MOCK_METHOD1(RemoveStack, int(int));
    MOCK_METHOD1(MoveMissionToTop, int(int32_t));
    MOCK_METHOD1(KillProcess, int(const std::string &));
    MOCK_METHOD1(UninstallApp, int(const std::string &));
    MOCK_METHOD4(OnRemoteRequest, int(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option));

    void Wait()
    {
        sem_.Wait();
    }

    int Post()
    {
        sem_.Post();
        return 0;
    }

    void PostVoid()
    {
        sem_.Post();
    }

private:
    Semaphore sem_;
};

}  // namespace AAFwk
}  // namespace OHOS
#endif  // FOUNDATION_AAFWK_SERVICES_MOCK_ABILITY_MGR_SERVICE_H