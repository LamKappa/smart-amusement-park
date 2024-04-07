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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_STATE_CALLBACK_HOST_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_STATE_CALLBACK_HOST_H

#include <map>

#include "iremote_stub.h"
#include "nocopyable.h"
#include "string_ex.h"
#include "app_mgr_constants.h"
#include "iapp_state_callback.h"

namespace OHOS {
namespace AppExecFwk {

class AppStateCallbackHost : public IRemoteStub<IAppStateCallback> {
public:
    AppStateCallbackHost();
    virtual ~AppStateCallbackHost();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    /**
     * AbilityMgr's request is done.
     *
     * @param token Ability token.
     * @param state Application state.
     */
    virtual void OnAbilityRequestDone(const sptr<IRemoteObject> &, const AbilityState) override;

    /**
     * Application state changed callback.
     *
     * @param appProcessData Process data
     */
    virtual void OnAppStateChanged(const AppProcessData &) override;

private:
    int32_t HandleOnAppStateChanged(MessageParcel &data, MessageParcel &reply);
    int32_t HandleOnAbilityRequestDone(MessageParcel &data, MessageParcel &reply);

    using AppStateCallbackFunc = int32_t (AppStateCallbackHost::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, AppStateCallbackFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(AppStateCallbackHost);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_STATE_CALLBACK_HOST_H
