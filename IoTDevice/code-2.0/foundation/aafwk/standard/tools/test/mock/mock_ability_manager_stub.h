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

#ifndef FOUNDATION_AAFWK_SERVICES_TOOLS_TEST_MOCK_MOCK_ABILITY_MANAGER_STUB_H
#define FOUNDATION_AAFWK_SERVICES_TOOLS_TEST_MOCK_MOCK_ABILITY_MANAGER_STUB_H

#include "gmock/gmock.h"

#include "string_ex.h"
#include "ability_manager_errors.h"
#include "ability_manager_stub.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
namespace {
const std::string STRING_DEVICE = "device";
const std::string STRING_ABILITY_NAME = "ability";
const std::string STRING_ABILITY_NAME_INVALID = "invalid_ability";
const std::string STRING_BUNDLE_NAME = "bundle";
const std::string STRING_BUNDLE_NAME_INVALID = "invalid_bundle";
const std::string STRING_RECORD_ID = "1024";
const std::string STRING_RECORD_ID_INVALID = "2048";
}  // namespace

class MockAbilityManagerStub : public AbilityManagerStub {
public:
    int StartAbility(const Want &want, int requestCode = -1);

    MOCK_METHOD3(StartAbility, int(const Want &want, const sptr<IRemoteObject> &callerToken, int requestCode));
    MOCK_METHOD3(TerminateAbility, int(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant));
    MOCK_METHOD3(ConnectAbility,
        int(const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken));
    MOCK_METHOD1(DisconnectAbility, int(const sptr<IAbilityConnection> &connect));
    MOCK_METHOD3(AcquireDataAbility,
        sptr<IAbilityScheduler>(const Uri &uri, bool tryBind, const sptr<IRemoteObject> &callerToken));
    MOCK_METHOD2(
        ReleaseDataAbility, int(sptr<IAbilityScheduler> dataAbilityScheduler, const sptr<IRemoteObject> &callerToken));
    MOCK_METHOD2(AddWindowInfo, void(const sptr<IRemoteObject> &token, int32_t windowToken));
    MOCK_METHOD2(AttachAbilityThread, int(const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token));
    MOCK_METHOD2(AbilityTransitionDone, int(const sptr<IRemoteObject> &token, int state));
    MOCK_METHOD2(
        ScheduleConnectAbilityDone, int(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject));
    MOCK_METHOD1(ScheduleDisconnectAbilityDone, int(const sptr<IRemoteObject> &token));
    MOCK_METHOD1(ScheduleCommandAbilityDone, int(const sptr<IRemoteObject> &token));

    void DumpState(const std::string &args, std::vector<std::string> &state);

    MOCK_METHOD2(TerminateAbilityResult, int(const sptr<IRemoteObject> &token, int startId));

    int StopServiceAbility(const Want &want);

    MOCK_METHOD2(TerminateAbilityByCaller, int(const sptr<IRemoteObject> &callerToken, int requestCode));
    MOCK_METHOD1(GetAllStackInfo, int(StackInfo &stackInfo));
    MOCK_METHOD3(
        GetRecentMissions, int(const int32_t numMax, const int32_t flags, std::vector<RecentMissionInfo> &recentList));
    MOCK_METHOD2(GetMissionSnapshot, int(const int32_t missionId, MissionSnapshotInfo &snapshot));
    MOCK_METHOD1(MoveMissionToTop, int(int32_t missionId));
    MOCK_METHOD1(RemoveMission, int(int id));
    MOCK_METHOD1(RemoveStack, int(int id));
    MOCK_METHOD1(KillProcess, int(const std::string &bundleName));
    MOCK_METHOD1(UninstallApp, int(const std::string &bundleName));
};

}  // namespace AAFwk
}  // namespace OHOS

#endif  // FOUNDATION_AAFWK_SERVICES_TOOLS_TEST_MOCK_MOCK_ABILITY_MANAGER_STUB_H