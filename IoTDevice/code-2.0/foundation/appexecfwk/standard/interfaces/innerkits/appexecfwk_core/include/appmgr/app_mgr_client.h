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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APP_MGR_CLIENT_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APP_MGR_CLIENT_H

#include "iremote_object.h"
#include "refbase.h"

#include "ability_info.h"
#include "application_info.h"
#include "app_mgr_constants.h"
#include "iapp_state_callback.h"
#include "running_process_info.h"

namespace OHOS {
namespace AppExecFwk {

class AppServiceManager;

class AppMgrClient {
public:
    AppMgrClient();
    virtual ~AppMgrClient();

    /**
     * Load ability.
     *
     * @param token, Ability identify.
     * @param abilityInfo, Ability information.
     * @param appInfo, Application information.
     * @return Returns RESULT_OK on success, others on failure.
     */
    virtual AppMgrResultCode LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
        const AbilityInfo &abilityInfo, const ApplicationInfo &appInfo);

    /**
     * Terminate ability.
     *
     * @param token Ability identify.
     *
     * @return Returns RESULT_OK on success, others on failure.
     */
    virtual AppMgrResultCode TerminateAbility(const sptr<IRemoteObject> &token);

    /**
     * Update ability state.
     *
     * @param token Ability identify.
     * @param state Ability running state.
     * @return Returns RESULT_OK on success, others on failure.
     */
    virtual AppMgrResultCode UpdateAbilityState(const sptr<IRemoteObject> &token, const AbilityState state);

    /**
     * Register Application state callback.
     *
     * @param callback IAppStateCallback
     * @return Returns RESULT_OK on success, others on failure.
     */
    virtual AppMgrResultCode RegisterAppStateCallback(const sptr<IAppStateCallback> &callback);

    /**
     * Connect service.
     *
     * @return Returns RESULT_OK on success, others on failure.
     */
    virtual AppMgrResultCode ConnectAppMgrService();

    /**
     * Ability manager resst.
     *
     * @return Returns RESULT_OK on success, others on failure.
     */
    virtual AppMgrResultCode Reset();

    /**
     * AbilityBehaviorAnalysis, ability behavior analysis assistant process optimization.
     *
     * @param token, the unique identification to start the ability.
     * @param preToken, the unique identification to call the ability.
     * @param visibility, the visibility information about windows info.
     * @param perceptibility, the Perceptibility information about windows info.
     * @param connectionState, the service ability connection state.
     * @return Returns RESULT_OK on success, others on failure.
     */
    virtual AppMgrResultCode AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token,
        const sptr<IRemoteObject> &preToken, const int32_t visibility, const int32_t perceptibility,
        const int32_t connectionState);

    /**
     * KillProcessByAbilityToken, call KillProcessByAbilityToken() through proxy object,
     * kill the process by ability token.
     *
     * @param token, the unique identification to the ability.
     * @return Returns RESULT_OK on success, others on failure.
     */
    virtual AppMgrResultCode KillProcessByAbilityToken(const sptr<IRemoteObject> &token);

    /**
     * KillApplication, call KillApplication() through proxy object, kill the application.
     *
     * @param  bundleName, bundle name in Application record.
     * @return ERR_OK, return back success, others fail.
     */
    virtual AppMgrResultCode KillApplication(const std::string &bundleName);

    /**
     * ClearUpApplicationData, call ClearUpApplicationData() through proxy project,
     * clear the application data.
     *
     * @param bundleName, bundle name in Application record.
     * @return
     */
    virtual AppMgrResultCode ClearUpApplicationData(const std::string &bundleName);

    /**
     * GetAllRunningProcesses, call GetAllRunningProcesses() through proxy project.
     * Obtains information about application processes that are running on the device.
     *
     * @param runningProcessInfo, app name in Application record.
     * @return ERR_OK ,return back success，others fail.
     */
    virtual AppMgrResultCode GetAllRunningProcesses(std::shared_ptr<RunningProcessInfo> &runningProcessInfo);

private:
    void SetServiceManager(std::unique_ptr<AppServiceManager> serviceMgr);

private:
    std::unique_ptr<AppServiceManager> serviceManager_;
    sptr<IRemoteObject> remote_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APP_MGR_CLIENT_H