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

#ifndef OHOS_AppExecFwk_APP_STATE_CALLBACK_HOST_H
#define OHOS_AppExecFwk_APP_STATE_CALLBACK_HOST_H

#include "app_mgr_constants.h"
#include "iapp_state_callback.h"
#include "iremote_object.h"

namespace OHOS {
namespace AppExecFwk {

class AppStateCallbackHost : public IAppStateCallback {
public:
    AppStateCallbackHost() = default;
    virtual ~AppStateCallbackHost() = default;

    void OnAppStateChanged(const std::string &__attribute__((unused)) appName,
        const ApplicationState __attribute__((unused)) state) override
    {}

    void OnAbilityRequestDone(const sptr<IRemoteObject> &__attribute__((unused)) token,
        const AbilityState __attribute__((unused)) state) override
    {}
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPMGR_INCLUDE_APP_STATE_CALLBACK_HOST_H
