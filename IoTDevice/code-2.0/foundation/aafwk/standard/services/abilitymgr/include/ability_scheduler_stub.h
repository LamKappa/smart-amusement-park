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

#ifndef OHOS_OS_AAFWK_ABILITY_SCHEDULE_STUB_H
#define OHOS_OS_AAFWK_ABILITY_SCHEDULE_STUB_H

#include <functional>

#include "ability_scheduler_interface.h"

#include <iremote_object.h>
#include <iremote_stub.h>

namespace OHOS {
namespace AAFwk {
/**
 * @class AbilitySchedulerStub
 * AbilityScheduler Stub.
 */
class AbilitySchedulerStub : public IRemoteStub<IAbilityScheduler> {
public:
    AbilitySchedulerStub();
    ~AbilitySchedulerStub();
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int AbilityTransactionInner(MessageParcel &data, MessageParcel &reply);
    int SendResultInner(MessageParcel &data, MessageParcel &reply);
    int ConnectAbilityInner(MessageParcel &data, MessageParcel &reply);
    int DisconnectAbilityInner(MessageParcel &data, MessageParcel &reply);
    int CommandAbilityInner(MessageParcel &data, MessageParcel &reply);
    int SaveAbilityStateInner(MessageParcel &data, MessageParcel &reply);
    int RestoreAbilityStateInner(MessageParcel &data, MessageParcel &reply);
    int GetFileTypesInner(MessageParcel &data, MessageParcel &reply);
    int OpenFileInner(MessageParcel &data, MessageParcel &reply);
    int OpenRawFileInner(MessageParcel &data, MessageParcel &reply);
    int InsertInner(MessageParcel &data, MessageParcel &reply);
    int UpdatetInner(MessageParcel &data, MessageParcel &reply);
    int DeleteInner(MessageParcel &data, MessageParcel &reply);
    int QueryInner(MessageParcel &data, MessageParcel &reply);
    int GetTypeInner(MessageParcel &data, MessageParcel &reply);
    int ReloadInner(MessageParcel &data, MessageParcel &reply);
    int BatchInsertInner(MessageParcel &data, MessageParcel &reply);
    using RequestFuncType = int (AbilitySchedulerStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, RequestFuncType> requestFuncMap_;
};

/**
 * @class AbilitySchedulerRecipient
 * AbilitySchedulerRecipient notices IRemoteBroker died.
 */
class AbilitySchedulerRecipient : public IRemoteObject::DeathRecipient {
public:
    using RemoteDiedHandler = std::function<void(const wptr<IRemoteObject> &)>;

    explicit AbilitySchedulerRecipient(RemoteDiedHandler handler);

    virtual ~AbilitySchedulerRecipient();

    virtual void OnRemoteDied(const wptr<IRemoteObject> &remote);

private:
    RemoteDiedHandler handler_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_ABILITY_SCHEDULE_STUB_H
