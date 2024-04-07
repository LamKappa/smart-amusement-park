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

#ifndef OHOS_APPEXECFWK_APP_MGR_CLIENT_H
#define OHOS_APPEXECFWK_APP_MGR_CLIENT_H

#include <singleton.h>
#include "ability_info.h"
#include "application_info.h"
#include "app_mgr_constants.h"
#include "iremote_object.h"
#include "refbase.h"
#include "iapp_state_callback.h"

namespace OHOS {
namespace AppExecFwk {

class AppMgrClient {
public:
    AppMgrClient()
    {}
    virtual ~AppMgrClient()
    {}

    virtual AppMgrResultCode LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
        const AbilityInfo &abilityInfo, const ApplicationInfo &appInfo);

    virtual AppMgrResultCode TerminateAbility(const sptr<IRemoteObject> &token);

    virtual AppMgrResultCode UpdateAbilityState(const sptr<IRemoteObject> &token, const AbilityState state);

    virtual AppMgrResultCode RegisterAppStateCallback(const sptr<IAppStateCallback> &callback);

    virtual AppMgrResultCode ConnectAppMgrService();

    virtual AppMgrResultCode AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token,
        const sptr<IRemoteObject> &preToken, const int32_t visibility, const int32_t perceptibility,
        const int32_t connectionState);

    virtual AppMgrResultCode KillProcessByAbilityToken(const sptr<IRemoteObject> &token);

    virtual AppMgrResultCode KillApplication(const std::string &bundleName);

    static int loadAbilityCount;
    static int terminateAbilityCount;
    static int backgroundCount;
    static int foregroundCount;

private:
    sptr<IAppStateCallback> callback_;
    sptr<IRemoteObject> remote_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_APPEXECFWK_APP_MGR_CLIENT_H
