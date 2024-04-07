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

#include "app_state_callback_host.h"

#include "appexecfwk_errors.h"
#include "app_log_wrapper.h"

#include "ipc_types.h"
#include "iremote_object.h"

#include "app_state_callback_proxy.h"

namespace OHOS {
namespace AppExecFwk {

AppStateCallbackHost::AppStateCallbackHost()
{
    memberFuncMap_[static_cast<uint32_t>(IAppStateCallback::Message::TRANSACT_ON_APP_STATE_CHANGED)] =
        &AppStateCallbackHost::HandleOnAppStateChanged;
    memberFuncMap_[static_cast<uint32_t>(IAppStateCallback::Message::TRANSACT_ON_ABILITY_REQUEST_DONE)] =
        &AppStateCallbackHost::HandleOnAbilityRequestDone;
}

AppStateCallbackHost::~AppStateCallbackHost()
{
    memberFuncMap_.clear();
}

int AppStateCallbackHost::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_LOGI("AppStateCallbackHost::OnReceived, code = %{public}d, flags= %{public}d.", code, option.GetFlags());
    std::u16string descriptor = AppStateCallbackHost::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        APP_LOGE("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

void AppStateCallbackHost::OnAbilityRequestDone(const sptr<IRemoteObject> &, const AbilityState)
{
    APP_LOGD("called");
}

void AppStateCallbackHost::OnAppStateChanged(const AppProcessData &)
{
    APP_LOGD("called");
}

int32_t AppStateCallbackHost::HandleOnAppStateChanged(MessageParcel &data, MessageParcel &reply)
{
    std::unique_ptr<AppProcessData> processData(data.ReadParcelable<AppProcessData>());
    if (!processData) {
        APP_LOGE("ReadParcelable<AppProcessData> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    OnAppStateChanged(*processData);
    return NO_ERROR;
}

int32_t AppStateCallbackHost::HandleOnAbilityRequestDone(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> obj = data.ReadParcelable<IRemoteObject>();
    int32_t state = data.ReadInt32();
    OnAbilityRequestDone(obj, static_cast<AbilityState>(state));
    return NO_ERROR;
}

}  // namespace AppExecFwk
}  // namespace OHOS
