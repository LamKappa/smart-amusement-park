
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

#include "ability_connect_callback_stub.h"

#include "ability_connect_callback_proxy.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "message_parcel.h"
#include "want.h"

namespace OHOS {
namespace AAFwk {
bool AbilityConnectionProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(AbilityConnectionProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AbilityConnectionProxy::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    HILOG_DEBUG("%s, abilityName:%s,resultCode:%d", __func__, element.GetAbilityName().c_str(), resultCode);

    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteParcelable(&element)) {
        HILOG_ERROR("connect done element error");
        return;
    }

    if (!data.WriteParcelable(remoteObject)) {
        HILOG_ERROR("connect done remote object error");
        return;
    }

    if (!data.WriteInt32(resultCode)) {
        HILOG_ERROR("connect done result code error");
        return;
    }

    error = Remote()->SendRequest(IAbilityConnection::ON_ABILITY_CONNECT_DONE, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("connect done fail, error: %d", error);
        return;
    }
}

void AbilityConnectionProxy::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    HILOG_DEBUG("%s, element:%s, resultCode:%d", __func__, element.GetURI().c_str(), resultCode);
    int error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteParcelable(&element) || !data.WriteInt32(resultCode)) {
        HILOG_ERROR("disconnect done data write error");
        return;
    }

    error = Remote()->SendRequest(IAbilityConnection::ON_ABILITY_DISCONNECT_DONE, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("disconnect done fail, error: %d", error);
        return;
    }
}

AbilityConnectionStub::AbilityConnectionStub()
{}

AbilityConnectionStub::~AbilityConnectionStub()
{}

int AbilityConnectionStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string descriptor = AbilityConnectionStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto element = data.ReadParcelable<AppExecFwk::ElementName>();
    switch (code) {
        case IAbilityConnection::ON_ABILITY_CONNECT_DONE: {
            if (element == nullptr) {
                HILOG_ERROR("callback stub receive element is nullptr");
                return ERR_INVALID_VALUE;
            }
            auto remoteObject = data.ReadParcelable<IRemoteObject>();
            auto resultCode = data.ReadInt32();
            OnAbilityConnectDone(*element, remoteObject, resultCode);
            delete element;
            return NO_ERROR;
        }
        case IAbilityConnection::ON_ABILITY_DISCONNECT_DONE: {
            if (element == nullptr) {
                HILOG_ERROR("callback stub receive element is nullptr");
                return ERR_INVALID_VALUE;
            }
            auto resultCode = data.ReadInt32();
            OnAbilityDisconnectDone(*element, resultCode);
            delete element;
            return NO_ERROR;
        }
        default: {
            if (element != nullptr) {
                delete element;
            }
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

void AbilityConnectCallbackRecipient::OnRemoteDied(const wptr<IRemoteObject> &__attribute__((unused)) remote)
{
    HILOG_ERROR("recv AbilityConnectCallbackRecipient death notice");
    if (handler_) {
        handler_(remote);
    }
}

AbilityConnectCallbackRecipient::AbilityConnectCallbackRecipient(RemoteDiedHandler handler) : handler_(handler)
{}

AbilityConnectCallbackRecipient::~AbilityConnectCallbackRecipient()
{}
}  // namespace AAFwk
}  // namespace OHOS
