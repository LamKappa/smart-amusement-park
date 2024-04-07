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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_PROCESS_OPTIMIZER_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_PROCESS_OPTIMIZER_H

#include <functional>
#include <list>
#include <memory>
#include <set>
#include <string>

#include "ability_running_record.h"
#include "app_running_record.h"
#include "cgroup_manager.h"
#include "lmkd_client.h"
#include "lmk_util.h"
#include "event_handler.h"
#include "nocopyable.h"

namespace OHOS {
namespace AppExecFwk {

class ProcessOptimizer : public NoCopyable {
public:
    using EventHandlerPtr = std::shared_ptr<EventHandler>;
    using AppPtr = std::shared_ptr<AppRunningRecord>;
    using AbilityPtr = std::shared_ptr<AbilityRunningRecord>;
    using CgroupManagerPtr = std::shared_ptr<CgroupManager>;
    using LmkdClientPtr = std::shared_ptr<LmkdClient>;

    static constexpr int APP_SUSPEND_TIMEOUT_DEFAULT = 5000;  // in milliseconds

public:
    ProcessOptimizer(const LmkdClientPtr &lmkdClient = nullptr, int suspendTimeout = APP_SUSPEND_TIMEOUT_DEFAULT);

    virtual ~ProcessOptimizer();

public:
    // callbacks
    std::function<void(AppPtr)> AppSuspended;
    std::function<void(AppPtr)> AppResumed;
    std::function<void(AppPtr, CgroupManager::LowMemoryLevel)> AppLowMemoryAlert;
    std::function<AppPtr(AbilityPtr)> GetAbilityOwnerApp;

public:
    virtual bool Init();
    virtual void OnAppAdded(const AppPtr &app);
    virtual void OnAppRemoved(const AppPtr &app);
    virtual void OnAppStateChanged(const AppPtr &app, const ApplicationState oldState);
    virtual void OnAbilityStarted(const AbilityPtr &ability);
    virtual void OnAbilityConnected(const AbilityPtr &ability, const AbilityPtr &targetAbility);
    virtual void OnAbilityDisconnected(const AbilityPtr &ability, const AbilityPtr &targetAbility);
    virtual void OnAbilityStateChanged(const AbilityPtr &ability, const AbilityState oldState);
    virtual void OnAbilityVisibleChanged(const AbilityPtr &ability);
    virtual void OnAbilityPerceptibleChanged(const AbilityPtr &ability);
    virtual void OnAbilityRemoved(const AbilityPtr &ability);

protected:
    bool SetAppOomAdj(const AppPtr &app, int oomAdj);
    bool SetAppSchedPolicy(const AppPtr &app, const CgroupManager::SchedPolicy schedPolicy);
    virtual void OnLowMemoryAlert(const CgroupManager::LowMemoryLevel level);

private:
    bool UpdateAppOomAdj(const AppPtr &app);
    bool UpdateAppSchedPolicy(const AppPtr &app);
    void StartAppSuspendTimer(const AppPtr &app);
    void StopAppSuspendTimer(const AppPtr &app);
    void SuspendApp(const AppPtr &app);
    std::string GetAppSuspendTimerName(const AppPtr &app);

private:
    using AppLru = std::list<AppPtr>;
    using SuspendTimers = std::set<std::string>;

    LmkdClientPtr lmkdClient_;
    AppLru appLru_;
    EventHandlerPtr eventHandler_;
    SuspendTimers suspendTimers_;
    std::unique_ptr<LmkUtil> lmkUtil_;
    int suspendTimeout_;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_PROCESS_OPTIMIZER_H
