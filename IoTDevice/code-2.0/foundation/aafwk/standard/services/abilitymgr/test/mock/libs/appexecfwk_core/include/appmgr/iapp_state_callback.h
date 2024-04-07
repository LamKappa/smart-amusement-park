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

#ifndef OHOS_APPEXECFWK_IAPP_STATE_CALLBACK_H
#define OHOS_APPEXECFWK_IAPP_STATE_CALLBACK_H

#include <refbase.h>
#include "app_mgr_constants.h"
#include "iapp_state_callback.h"
#include "iremote_object.h"

namespace OHOS {
namespace AppExecFwk {

class IAppStateCallback : public virtual RefBase {
public:
    /**
     * MOCApplication state changed callback.
     */
    virtual void OnAppStateChanged(const std::string &appName, const ApplicationState state) = 0;

    /**
     * mock AbilityMgr's request is done.
     */
    virtual void OnAbilityRequestDone(const sptr<IRemoteObject> &token, const AbilityState state) = 0;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_APPEXECFWK_IAPP_STATE_CALLBACK_H
