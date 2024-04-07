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

#include "ams_mgr_stub.h"

#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"

#include "ability_info.h"
#include "appexecfwk_errors.h"
#include "app_log_wrapper.h"
#include "app_mgr_proxy.h"
#include "app_scheduler_interface.h"
#include "iapp_state_callback.h"

namespace OHOS {
namespace AppExecFwk {

AmsMgrStub::AmsMgrStub()
{
    memberFuncMap_[static_cast<uint32_t>(IAmsMgr::Message::AMS_LOAD_ABILITY)] = &AmsMgrStub::HandleLoadAbility;
    memberFuncMap_[static_cast<uint32_t>(IAmsMgr::Message::AMS_TERMINATE_ABILITY)] =
        &AmsMgrStub::HandleTerminateAbility;
    memberFuncMap_[static_cast<uint32_t>(IAmsMgr::Message::AMS_UPDATE_ABILITY_STATE)] =
        &AmsMgrStub::HandleUpdateAbilityState;
    memberFuncMap_[static_cast<uint32_t>(IAmsMgr::Message::AMS_REGISTER_APP_STATE_CALLBACK)] =
        &AmsMgrStub::HandleRegisterAppStateCallback;
    memberFuncMap_[static_cast<uint32_t>(IAmsMgr::Message::AMS_RESET)] = &AmsMgrStub::HandleReset;
    memberFuncMap_[static_cast<uint32_t>(IAmsMgr::Message::AMS_ABILITY_BEHAVIOR_ANALYSIS)] =
        &AmsMgrStub::HandleAbilityBehaviorAnalysis;
    memberFuncMap_[static_cast<uint32_t>(IAmsMgr::Message::AMS_KILL_PEOCESS_BY_ABILITY_TOKEN)] =
        &AmsMgrStub::HandleKillProcessByAbilityToken;
    memberFuncMap_[static_cast<uint32_t>(IAmsMgr::Message::AMS_KILL_APPLICATION)] = &AmsMgrStub::HandleKillApplication;
}

AmsMgrStub::~AmsMgrStub()
{
    memberFuncMap_.clear();
}

int AmsMgrStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_LOGI("AmsMgrStub::OnReceived, code = %{public}d, flags= %{public}d.", code, option.GetFlags());
    std::u16string descriptor = AmsMgrStub::GetDescriptor();
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

ErrCode AmsMgrStub::HandleLoadAbility(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> token = data.ReadParcelable<IRemoteObject>();
    sptr<IRemoteObject> preToke = data.ReadParcelable<IRemoteObject>();
    std::shared_ptr<AbilityInfo> abilityInfo(data.ReadParcelable<AbilityInfo>());
    if (!abilityInfo) {
        APP_LOGE("ReadParcelable<AbilityInfo> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    std::shared_ptr<ApplicationInfo> appInfo(data.ReadParcelable<ApplicationInfo>());
    if (!appInfo) {
        APP_LOGE("ReadParcelable<ApplicationInfo> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    LoadAbility(token, preToke, abilityInfo, appInfo);
    return NO_ERROR;
}

ErrCode AmsMgrStub::HandleTerminateAbility(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> token = data.ReadParcelable<IRemoteObject>();
    TerminateAbility(token);
    return NO_ERROR;
}

ErrCode AmsMgrStub::HandleUpdateAbilityState(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> token = data.ReadParcelable<IRemoteObject>();
    int32_t state = data.ReadInt32();
    UpdateAbilityState(token, static_cast<AbilityState>(state));
    return NO_ERROR;
}

ErrCode AmsMgrStub::HandleRegisterAppStateCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> obj = data.ReadParcelable<IRemoteObject>();
    sptr<IAppStateCallback> callback = iface_cast<IAppStateCallback>(obj);
    RegisterAppStateCallback(callback);
    return NO_ERROR;
}

ErrCode AmsMgrStub::HandleReset(MessageParcel &data, MessageParcel &reply)
{
    Reset();
    return NO_ERROR;
}

ErrCode AmsMgrStub::HandleAbilityBehaviorAnalysis(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> token = data.ReadParcelable<IRemoteObject>();
    sptr<IRemoteObject> preToke = data.ReadParcelable<IRemoteObject>();
    int32_t visibility = data.ReadInt32();
    int32_t Perceptibility = data.ReadInt32();
    int32_t connectionState = data.ReadInt32();

    AbilityBehaviorAnalysis(token, preToke, visibility, Perceptibility, connectionState);
    return NO_ERROR;
}

ErrCode AmsMgrStub::HandleKillProcessByAbilityToken(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> token = data.ReadParcelable<IRemoteObject>();

    KillProcessByAbilityToken(token);
    return NO_ERROR;
}

ErrCode AmsMgrStub::HandleKillApplication(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleName = data.ReadString();
    int32_t result = KillApplication(bundleName);
    reply.WriteInt32(result);
    return NO_ERROR;
}

}  // namespace AppExecFwk
}  // namespace OHOS