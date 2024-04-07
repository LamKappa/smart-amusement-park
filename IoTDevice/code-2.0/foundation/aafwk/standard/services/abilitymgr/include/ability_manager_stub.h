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

#ifndef OHOS_AAFWK_ABILITY_MANAGER_STUB_H
#define OHOS_AAFWK_ABILITY_MANAGER_STUB_H

#include "ability_manager_interface.h"

#include <iremote_object.h>
#include <iremote_stub.h>

#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
/**
 * @class AbilityManagerStub
 * AbilityManagerStub.
 */
class AbilityManagerStub : public IRemoteStub<IAbilityManager> {
public:
    AbilityManagerStub();
    ~AbilityManagerStub();
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int TerminateAbilityInner(MessageParcel &data, MessageParcel &reply);
    int TerminateAbilityByCallerInner(MessageParcel &data, MessageParcel &reply);
    int AttachAbilityThreadInner(MessageParcel &data, MessageParcel &reply);
    int AbilityTransitionDoneInner(MessageParcel &data, MessageParcel &reply);
    int ScheduleConnectAbilityDoneInner(MessageParcel &data, MessageParcel &reply);
    int ScheduleDisconnectAbilityDoneInner(MessageParcel &data, MessageParcel &reply);
    int AddWindowInfoInner(MessageParcel &data, MessageParcel &reply);
    int TerminateAbilityResultInner(MessageParcel &data, MessageParcel &reply);
    int GetAllStackInfoInner(MessageParcel &data, MessageParcel &reply);
    int GetRecentMissionsInner(MessageParcel &data, MessageParcel &reply);
    int RemoveMissionInner(MessageParcel &data, MessageParcel &reply);
    int RemoveStackInner(MessageParcel &data, MessageParcel &reply);
    int ScheduleCommandAbilityDoneInner(MessageParcel &data, MessageParcel &reply);
    int GetMissionSnapshotInner(MessageParcel &data, MessageParcel &reply);
    int AcquireDataAbilityInner(MessageParcel &data, MessageParcel &reply);
    int ReleaseDataAbilityInner(MessageParcel &data, MessageParcel &reply);
    int MoveMissionToTopInner(MessageParcel &data, MessageParcel &reply);
    int KillProcessInner(MessageParcel &data, MessageParcel &reply);
    int UninstallAppInner(MessageParcel &data, MessageParcel &reply);
    int StartAbilityInner(MessageParcel &data, MessageParcel &reply);
    int StartAbilityAddCallerInner(MessageParcel &data, MessageParcel &reply);
    int ConnectAbilityInner(MessageParcel &data, MessageParcel &reply);
    int DisconnectAbilityInner(MessageParcel &data, MessageParcel &reply);
    int StopServiceAbilityInner(MessageParcel &data, MessageParcel &reply);
    int DumpStateInner(MessageParcel &data, MessageParcel &reply);

    using RequestFuncType = int (AbilityManagerStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, RequestFuncType> requestFuncMap_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_ABILITY_MANAGER_STUB_H
