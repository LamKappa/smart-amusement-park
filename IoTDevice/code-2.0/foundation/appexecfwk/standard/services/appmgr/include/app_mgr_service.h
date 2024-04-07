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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_MGR_SERVICE_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_MGR_SERVICE_H

#include <list>
#include <string>
#include <vector>

#include "if_system_ability_manager.h"
#include "nocopyable.h"
#include "system_ability.h"

#include "ability_info.h"
#include "ability_running_record.h"
#include "appexecfwk_errors.h"
#include "application_info.h"
#include "app_mgr_constants.h"
#include "app_mgr_stub.h"
#include "app_mgr_service_event_handler.h"
#include "app_mgr_service_inner.h"
#include "app_record_id.h"
#include "app_running_record.h"
#include "app_scheduler_proxy.h"
#include "ams_mgr_scheduler.h"

namespace OHOS {
namespace AppExecFwk {

enum class ServiceRunningState { STATE_NOT_START, STATE_RUNNING };

struct AppMgrServiceState {
    ServiceRunningState serviceRunningState = ServiceRunningState::STATE_NOT_START;
    SpawnConnectionState connectionState = SpawnConnectionState::STATE_NOT_CONNECT;
};

class AMSEventHandler;

class AppMgrService : public SystemAbility, public AppMgrStub {
public:
    DECLEAR_SYSTEM_ABILITY(AppMgrService);

    AppMgrService();
    AppMgrService(const int32_t serviceId, bool runOnCreate = false);
    virtual ~AppMgrService() override;

    // the function about application
    // attach the application to ams, then ams can control it.
    /**
     * AttachApplication, call AttachApplication() through proxy object,
     * get all the information needed to start the Application (data related to the Application ).
     *
     * @param app, information needed to start the Application.
     * @return
     */
    virtual void AttachApplication(const sptr<IRemoteObject> &app) override;

    // notify the ams update the state of an app, when it entered foreground.

    /**
     * ApplicationForegrounded, call ApplicationForegrounded() through proxy object,
     * set the application to Foreground State.
     *
     * @param recordId, a unique record that identifies this Application from others.
     * @return
     */
    virtual void ApplicationForegrounded(const int32_t recordId) override;

    /**
     * ApplicationBackgrounded, call ApplicationBackgrounded() through proxy object,
     * set the application to Backgrounded State.
     *
     * @param recordId, a unique record that identifies this Application from others.
     * @return
     */
    virtual void ApplicationBackgrounded(const int32_t recordId) override;

    /**
     * ApplicationTerminated, call ApplicationTerminated() through proxy object,
     * terminate the application.
     *
     * @param recordId, a unique record that identifies this Application from others.
     * @return
     */
    virtual void ApplicationTerminated(const int32_t recordId) override;

    /**
     * AbilityCleaned,call through AbilityCleaned() proxy project, clean Ability record.
     *
     * @param token, a unique record that identifies AbilityCleaned from others.
     * @return
     */
    virtual void AbilityCleaned(const sptr<IRemoteObject> &token) override;

    /**
     * ClearUpApplicationData, call ClearUpApplicationData() through proxy project,
     * clear the application data.
     *
     * @param bundleName, bundle name in Application record.
     * @return
     */
    virtual void ClearUpApplicationData(const std::string &bundleName) override;

    /**
     * IsBackgroundRunningRestricted, call IsBackgroundRunningRestricted() through proxy project,
     * Checks whether the process of this application is forbidden to run in the background.
     *
     * @param bundleName, bundle name in Application record.
     * @return ERR_OK, return back success, others fail.
     */
    virtual int32_t IsBackgroundRunningRestricted(const std::string &bundleName) override;

    /**
     * GetAllRunningProcesses, call GetAllRunningProcesses() through proxy project.
     * Obtains information about application processes that are running on the device.
     *
     * @param runningProcessInfo, app name in Application record.
     * @return ERR_OK ,return back success，others fail.
     */
    virtual int32_t GetAllRunningProcesses(std::shared_ptr<RunningProcessInfo> &runningProcessInfo) override;

    // the function about system
    /**
     * CheckPermission, call CheckPermission() through proxy object, check the permission.
     *
     * @param recordId, a unique record that identifies this Application from others.
     * @param permission, check the permissions.
     * @return ERR_OK, return back success, others fail.
     */
    virtual int32_t CheckPermission(const int32_t recordId, const std::string &permission) override;

    // the function about service running info
    /**
     * QueryServiceState, Query application service status.
     *
     * @return the application service status.
     */
    AppMgrServiceState QueryServiceState();

    /**
     * GetAmsMgr, call GetAmsMgr() through proxy object, get AMS interface instance.
     *
     * @return sptr<IAmsMgr>, return to AMS interface instance.
     */
    virtual sptr<IAmsMgr> GetAmsMgr() override;

private:
    /**
     * Init, Initialize application services.
     *
     * @return ERR_OK, return back success, others fail.
     */
    ErrCode Init();

    // the function that overrode from SystemAbility
    /**
     * OnStart, Start application service.
     *
     * @return
     */
    virtual void OnStart() override;

    /**
     * OnStop, Stop application service.
     *
     * @return
     */
    virtual void OnStop() override;

    /**
     * @brief Judge whether the application service is ready.
     *
     * @return Returns true means service is ready, otherwise service is not ready.
     */
    bool IsReady() const;

    /**
     * AddAppDeathRecipient, Add monitoring death application record.
     *
     * @param pid, the application pid.
     * @param appDeathRecipient, Application death recipient list.
     *
     * @return
     */
    void AddAppDeathRecipient(const pid_t pid) const;

    /**
     * SetInnerService, Setting application service Inner instance.
     *
     * @return
     */
    void SetInnerService(const std::shared_ptr<AppMgrServiceInner> &innerService);

private:
    std::shared_ptr<AppMgrServiceInner> appMgrServiceInner_;
    AppMgrServiceState appMgrServiceState_;
    std::shared_ptr<EventRunner> runner_;
    std::shared_ptr<AMSEventHandler> handler_;
    sptr<ISystemAbilityManager> systemAbilityMgr_;
    sptr<IAmsMgr> amsMgrScheduler_;

    DISALLOW_COPY_AND_MOVE(AppMgrService);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_MGR_SERVICE_H
