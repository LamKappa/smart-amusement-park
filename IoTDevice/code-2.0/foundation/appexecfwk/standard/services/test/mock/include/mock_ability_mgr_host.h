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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_MOCK_APP_MGR_HOST_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_MOCK_APP_MGR_HOST_H

#include "ability_manager_interface.h"
#include <iremote_object.h>
#include <iremote_stub.h>

namespace OHOS {
namespace AppExecFwk {

class MockAbilityMgrStub : public IRemoteStub<AAFwk::IAbilityManager> {
public:
    using Uri = OHOS::Uri;
    MockAbilityMgrStub() = default;
    virtual ~MockAbilityMgrStub() = default;

    virtual int StartAbility(const AAFwk::Want &want, int requestCode = -1) override
    {
        return 0;
    }
    virtual int StartAbility(
        const AAFwk::Want &want, const sptr<IRemoteObject> &callerToken, int requestCode = -1) override
    {
        return 0;
    }
    virtual int TerminateAbility(
        const sptr<IRemoteObject> &token, int resultCode, const AAFwk::Want *resultWant = nullptr) override
    {
        return 0;
    }
    virtual int ConnectAbility(const AAFwk::Want &want, const sptr<AAFwk::IAbilityConnection> &connect,
        const sptr<IRemoteObject> &callerToken) override
    {
        return 0;
    }
    virtual int DisconnectAbility(const sptr<AAFwk::IAbilityConnection> &connect) override
    {
        return 0;
    }
    virtual sptr<AAFwk::IAbilityScheduler> AcquireDataAbility(
        const Uri &uri, bool tryBind, const sptr<IRemoteObject> &callerToken) override
    {
        return nullptr;
    }
    virtual int ReleaseDataAbility(
        sptr<AAFwk::IAbilityScheduler> dataAbilityScheduler, const sptr<IRemoteObject> &callerToken) override
    {
        return 0;
    }
    virtual void AddWindowInfo(const sptr<IRemoteObject> &token, int32_t windowToken) override
    {
        return;
    }
    virtual int AttachAbilityThread(
        const sptr<AAFwk::IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token) override
    {
        return 0;
    }
    virtual int AbilityTransitionDone(const sptr<IRemoteObject> &token, int state) override
    {
        return 0;
    }
    virtual int ScheduleConnectAbilityDone(
        const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject) override
    {
        return 0;
    }
    virtual int ScheduleDisconnectAbilityDone(const sptr<IRemoteObject> &token) override
    {
        return 0;
    }
    virtual int ScheduleCommandAbilityDone(const sptr<IRemoteObject> &token) override
    {
        return 0;
    }
    virtual void DumpState(const std::string &args, std::vector<std::string> &state) override
    {
        return;
    }
    virtual int TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId) override
    {
        return 0;
    }
    virtual int StopServiceAbility(const AAFwk::Want &want) override
    {
        return 0;
    }
    virtual int GetAllStackInfo(AAFwk::StackInfo &stackInfo) override
    {
        return 0;
    }
    virtual int GetRecentMissions(
        const int32_t numMax, const int32_t flags, std::vector<AAFwk::RecentMissionInfo> &recentList) override
    {
        return 0;
    }
    virtual int GetMissionSnapshot(const int32_t missionId, AAFwk::MissionSnapshotInfo &snapshot) override
    {
        return 0;
    }
    virtual int MoveMissionToTop(int32_t missionId) override
    {
        return 0;
    }
    virtual int RemoveMission(int id) override
    {
        return 0;
    }
    virtual int RemoveStack(int id) override
    {
        return 0;
    }
    virtual int KillProcess(const std::string &bundleName) override
    {
        return 0;
    }
    virtual int UninstallApp(const std::string &bundleName) override
    {
        return 0;
    }
    virtual int TerminateAbilityByCaller(const sptr<IRemoteObject> &callerToken, int requestCode) override
    {
        return 0;
    }
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_MOCK_APP_MGR_HOST_H