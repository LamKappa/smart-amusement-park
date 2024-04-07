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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_AMS_MGR_SCHEDULER_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_AMS_MGR_SCHEDULER_H

#include "if_system_ability_manager.h"
#include "nocopyable.h"

#include "system_ability.h"
#include "ability_info.h"
#include "ability_running_record.h"
#include "appexecfwk_errors.h"
#include "application_info.h"
#include "app_mgr_constants.h"
#include "ams_mgr_stub.h"
#include "app_mgr_service_event_handler.h"
#include "app_mgr_service_inner.h"
#include "app_record_id.h"
#include "app_running_record.h"
#include "app_scheduler_proxy.h"

namespace OHOS {
namespace AppExecFwk {

class AmsMgrScheduler : public AmsMgrStub {
public:
    AmsMgrScheduler(
        const std::shared_ptr<AppMgrServiceInner> &MgrServiceInner_, const std::shared_ptr<AMSEventHandler> &Handler_);
    virtual ~AmsMgrScheduler() override;

    /**
     * LoadAbility, call LoadAbility() through proxy project, load the ability that needed to be started.
     *
     * @param token, the unique identification to start the ability.
     * @param preToken, the unique identification to call the ability.
     * @param abilityInfo, the ability information.
     * @param appInfo, the app information.
     * @return
     */
    virtual void LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
        const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo) override;

    /**
     * TerminateAbility, call TerminateAbility() through the proxy object, terminate the token ability.
     *
     * @param token, token, he unique identification to terminate the ability.
     * @return
     */
    virtual void TerminateAbility(const sptr<IRemoteObject> &token) override;

    /**
     * UpdateAbilityState, call UpdateAbilityState() through the proxy object, update the ability status.
     *
     * @param token, the unique identification to update the ability.
     * @param state, ability status that needs to be updated.
     * @return
     */
    virtual void UpdateAbilityState(const sptr<IRemoteObject> &token, const AbilityState state) override;

    /**
     * RegisterAppStateCallback, call RegisterAppStateCallback() through the proxy object, register the callback.
     *
     * @param callback, Ams register the callback.
     * @return
     */
    virtual void RegisterAppStateCallback(const sptr<IAppStateCallback> &callback) override;

    /**
     * Reset,call Reset() through the proxy object, reset DFX of AppMgr.
     *
     * @return
     */
    virtual void Reset() override;

    /**
     * AbilityBehaviorAnalysis, ability behavior analysis assistant process optimization.
     *
     * @param token, the unique identification to start the ability.
     * @param preToken, the unique identification to call the ability.
     * @param visibility, the visibility information about windows info.
     * @param perceptibility, the Perceptibility information about windows info.
     * @param connectionState, the service ability connection state.
     * @return
     */
    virtual void AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
        const int32_t visibility, const int32_t perceptibility, const int32_t connectionState) override;

    /**
     * KillProcessByAbilityToken, call KillProcessByAbilityToken() through proxy object,
     * kill the process by ability token.
     *
     * @param token, the unique identification to the ability.
     * @return
     */
    virtual void KillProcessByAbilityToken(const sptr<IRemoteObject> &token) override;

    /**
     * KillApplication, call KillApplication() through proxy object, kill the application.
     *
     * @param  bundleName, bundle name in Application record.
     * @return ERR_OK, return back success, others fail.
     */
    virtual int32_t KillApplication(const std::string &bundleName) override;

private:
    /**
     * @brief Judge whether the application service is ready.
     *
     * @return Returns true means service is ready, otherwise service is not ready.
     */
    bool IsReady() const;

private:
    std::shared_ptr<AppMgrServiceInner> amsMgrServiceInner_;
    std::shared_ptr<AMSEventHandler> amsHandler_;
    sptr<ISystemAbilityManager> systemAbilityMgr_;

    DISALLOW_COPY_AND_MOVE(AmsMgrScheduler);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_AMS_MGR_SCHEDULER_H
