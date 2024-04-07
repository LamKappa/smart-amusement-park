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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_IAPP_STATE_CALLBACK_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_IAPP_STATE_CALLBACK_H

#include "iremote_broker.h"
#include "iremote_object.h"

#include "app_mgr_constants.h"
#include "app_process_data.h"

namespace OHOS {
namespace AppExecFwk {

class IAppStateCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.AppStateCallback");

    /**
     * Application state changed callback.
     *
     * @param appProcessData Process data
     */
    virtual void OnAppStateChanged(const AppProcessData &appProcessData) = 0;

    /**
     * AbilityMgr's request is done.
     *
     * @param token Ability token.
     * @param state Application state.
     */
    virtual void OnAbilityRequestDone(const sptr<IRemoteObject> &token, const AbilityState state) = 0;

    enum class Message {
        TRANSACT_ON_APP_STATE_CHANGED = 0,
        TRANSACT_ON_ABILITY_REQUEST_DONE,
    };
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_IAPP_STATE_CALLBACK_H
