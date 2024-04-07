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

#ifndef ABILITY_UNITTEST_ABILITY_MANAGER_STUB_IMPL_MOCK_H
#define ABILITY_UNITTEST_ABILITY_MANAGER_STUB_IMPL_MOCK_H
#include <gmock/gmock.h>
#include <iremote_object.h>
#include <iremote_stub.h>
#include "ability_manager_interface.h"
#include "ability_manager_stub.h"

namespace OHOS {
namespace AAFwk {
class AbilityManagerStubImplMock : public AbilityManagerStub {
public:
    AbilityManagerStubImplMock()
    {}
    virtual ~AbilityManagerStubImplMock()
    {}

    MOCK_METHOD2(TerminateAbilityByCaller, int(const sptr<IRemoteObject> &callerToken, int requestCode));
    MOCK_METHOD3(StartAbility, int(const Want &want, const sptr<IRemoteObject> &callerToken, int requestCode));

    int InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        return 0;
    }

    int InvokeErrorSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        return UNKNOWN_ERROR;
    }

    int code_ = 0;

    virtual int StartAbility(const Want &want, int requestCode = -1)
    {
        return 0;
    }

    virtual int TerminateAbility(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant = nullptr)
    {
        return 0;
    }

    virtual int ConnectAbility(
        const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken)
    {
        return 0;
    }

    virtual int DisconnectAbility(const sptr<IAbilityConnection> &connect)
    {
        return 0;
    }

    virtual sptr<IAbilityScheduler> AcquireDataAbility(
        const Uri &uri, bool tryBind, const sptr<IRemoteObject> &callerToken) override
    {
        return nullptr;
    }

    virtual int ReleaseDataAbility(sptr<IAbilityScheduler> dataAbilityScheduler, const sptr<IRemoteObject> &callerToken)
    {
        return 0;
    }

    virtual void AddWindowInfo(const sptr<IRemoteObject> &token, int32_t windowToken)
    {}

    virtual int AttachAbilityThread(const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token)
    {
        return 0;
    }

    virtual int AbilityTransitionDone(const sptr<IRemoteObject> &token, int state)
    {
        return 0;
    }

    virtual int ScheduleConnectAbilityDone(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject)
    {
        return 0;
    }

    virtual int ScheduleDisconnectAbilityDone(const sptr<IRemoteObject> &token)
    {
        return 0;
    }

    virtual int ScheduleCommandAbilityDone(const sptr<IRemoteObject> &token)
    {
        return 0;
    }

    virtual void DumpState(const std::string &args, std::vector<std::string> &state)
    {}

    virtual int TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId)
    {
        return 0;
    }

    virtual int StopServiceAbility(const Want &want)
    {
        return 0;
    }

    virtual int GetAllStackInfo(StackInfo &stackInfo)
    {
        return 0;
    }

    virtual int GetRecentMissions(const int32_t numMax, const int32_t flags, std::vector<RecentMissionInfo> &recentList)
    {
        RecentMissionInfo info;
        info.id = 1;
        AppExecFwk::ElementName baseEle("baseDevice", "baseBundle", "baseAbility");
        info.baseAbility = baseEle;
        Want want;
        want.SetElement(baseEle);
        info.baseWant = want;
        AppExecFwk::ElementName topEle("topDevice", "topBundle", "topAbility");
        info.topAbility = topEle;
        info.size = 1;
        info.missionDescription.iconPath = "icon path";
        info.missionDescription.label = "label";
        recentList.emplace_back(info);
        return 0;
    }

    int GetMissionSnapshot(const int32_t missionId, MissionSnapshotInfo &snapshot)
    {
        return 0;
    }

    virtual int RemoveMission(int id)
    {
        return 0;
    }

    virtual int RemoveStack(int id)
    {
        return 0;
    }

    virtual int KillProcess(const std::string &bundleName)
    {
        return 0;
    }

    virtual int UninstallApp(const std::string &bundleName)
    {
        return 0;
    }

    virtual int MoveMissionToTop(int32_t missionId)
    {
        return 0;
    }
};

}  // namespace AAFwk
}  // namespace OHOS

#endif