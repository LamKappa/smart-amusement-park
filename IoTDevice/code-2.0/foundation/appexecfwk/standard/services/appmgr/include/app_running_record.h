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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_RUNNING_RECORD_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_RUNNING_RECORD_H

#include <list>
#include <map>
#include <memory>
#include <string>

#include "iremote_object.h"

#include "ability_running_record.h"
#include "application_info.h"
#include "app_death_recipient.h"
#include "app_launch_data.h"
#include "app_mgr_constants.h"
#include "app_scheduler_proxy.h"
#include "app_record_id.h"
#include "profile.h"
#include "priority_object.h"
#include "app_lifecycle_deal.h"

namespace OHOS {
namespace AppExecFwk {

class AbilityRunningRecord;
class AppMgrServiceInner;

class AppRunningRecord {
public:
    AppRunningRecord(
        const std::shared_ptr<ApplicationInfo> &info, const int32_t recordId, const std::string &processName);
    virtual ~AppRunningRecord() = default;

    /**
     * @brief Obtains the app record bundleName.
     *
     * @return Returns app record bundleName.
     */
    std::string GetBundleName() const;
    /**
     * @brief Obtains the app record id.
     *
     * @return Returns app record id.
     */
    int32_t GetRecordId() const;

    /**
     * @brief Obtains the app name.
     *
     * @return Returns the app name.
     */
    const std::string &GetName() const;

    /**
     * @brief Obtains the process name.
     *
     * @return Returns the process name.
     */
    const std::string &GetProcessName() const;

    /**
     * @brief Obtains the application uid.
     *
     * @return Returns the application uid.
     */
    int32_t GetUid() const;

    /**
     * @brief Setting the application uid.
     *
     * @param state, the application uid.
     */
    void SetUid(const int32_t uid);

    // Get current state for this process

    /**
     * @brief Obtains the application state.
     *
     * @return Returns the application state.
     */
    ApplicationState GetState() const;

    // Set current state for this process

    /**
     * @brief Setting the application state.
     *
     * @param state, the application state.
     */
    void SetState(const ApplicationState state);

    // Get abilities_ for this process
    /**
     * @brief Obtains the abilitys info for the application record.
     *
     * @return Returns the abilitys info for the application record.
     */
    const std::map<const sptr<IRemoteObject>, std::shared_ptr<AbilityRunningRecord>> &GetAbilities() const;
    // Update appThread with appThread

    /**
     * @brief Setting the application client.
     *
     * @param thread, the application client.
     */
    void SetApplicationClient(const sptr<IAppScheduler> &thread);

    /**
     * @brief Obtains the application client.
     *
     * @return Returns the application client.
     */
    sptr<IAppScheduler> GetApplicationClient() const;

    // Add new ability instance to current running abilities list managed by this process
    /**
     * AddAbility, Add new ability instance to current running abilities list managed by this process.
     *
     * @param token, the unique identification to the ability.
     * @param abilityInfo, the ability info.
     *
     * @return the ability record.
     */
    std::shared_ptr<AbilityRunningRecord> AddAbility(
        const sptr<IRemoteObject> &token, const std::shared_ptr<AbilityInfo> &abilityInfo);

    // It can only used in SINGLETON mode.
    /**
     * GetAbilityRunningRecord, Get ability record by the ability Name.
     *
     * @param abilityName, the ability name.
     *
     * @return the ability record.
     */
    std::shared_ptr<AbilityRunningRecord> GetAbilityRunningRecord(const std::string &abilityName) const;

    // Clear(remove) the specified ability record from the list

    /**
     * ClearAbility, Clear ability record by record info.
     *
     * @param record, the ability record.
     *
     * @return
     */
    void ClearAbility(const std::shared_ptr<AbilityRunningRecord> &record);

    // Update the trim memory level value of this process
    /**
     * @brief Setting the Trim Memory Level.
     *
     * @param level, the Memory Level.
     */
    void SetTrimMemoryLevel(int32_t level);

    // Kill this process with a given reason
    /**
     * ForceKillApp, Kill this process with a given reason.
     *
     * @param reason, The reason to kill the process.
     *
     * @return
     */
    void ForceKillApp(const std::string &reason) const;

    // Schedule to crash this app with a given description
    /**
     * ScheduleAppCrash, Schedule to crash this app with a given description.
     *
     * @param description, the given description.
     *
     * @return
     */
    void ScheduleAppCrash(const std::string &description) const;

    /**
     * LaunchApplication, Notify application to launch application.
     *
     * @return
     */
    void LaunchApplication();

    /**
     * LaunchAbility, Notify application to launch ability.
     *
     * @param ability, the ability record.
     *
     * @return
     */
    void LaunchAbility(const std::shared_ptr<AbilityRunningRecord> &ability);

    /**
     * LaunchPendingAbilities, Launch Pending Abilities.
     *
     * @return
     */
    void LaunchPendingAbilities();

    /**
     * LowMemoryWarning, Low memory warning.
     *
     * @return
     */
    void LowMemoryWarning();

    /**
     * ScheduleTerminate, Notify application to terminate.
     *
     * @return
     */
    void ScheduleTerminate();

    /**
     * ScheduleForegroundRunning, Notify application to switch to foreground.
     *
     * @return
     */
    void ScheduleForegroundRunning();

    /**
     * ScheduleBackgroundRunning, Notify application to switch to background.
     *
     * @return
     */
    void ScheduleBackgroundRunning();

    /**
     * ScheduleTerminate, Notify application process exit safely.
     *
     * @return
     */
    void ScheduleProcessSecurityExit();

    /**
     * ScheduleTrimMemory, Notifies the application of the memory seen.
     *
     * @return
     */
    void ScheduleTrimMemory();

    /**
     * GetAbilityRunningRecordByToken, Obtaining the ability record through token.
     *
     * @param token, the unique identification to the ability.
     *
     * @return
     */
    std::shared_ptr<AbilityRunningRecord> GetAbilityRunningRecordByToken(const sptr<IRemoteObject> &token) const;

    /**
     * UpdateAbilityState, update the ability status.
     *
     * @param token, the unique identification to update the ability.
     * @param state, ability status that needs to be updated.
     *
     * @return
     */
    void UpdateAbilityState(const sptr<IRemoteObject> &token, const AbilityState state);

    /**
     * PopForegroundingAbilityTokens, Extract the token record from the foreground tokens list.
     *
     * @return
     */
    void PopForegroundingAbilityTokens();

    /**
     * TerminateAbility, terminate the token ability.
     *
     * @param token, he unique identification to terminate the ability.
     *
     * @return
     */
    void TerminateAbility(const sptr<IRemoteObject> &token);

    /**
     * AbilityTerminated, terminate the ability.
     *
     * @param token, the unique identification to terminated the ability.
     *
     * @return
     */
    void AbilityTerminated(const sptr<IRemoteObject> &token);

    /**
     * @brief Setting application service internal handler instance.
     *
     * @param serviceInner, application service internal handler instance.
     */
    void SetAppMgrServiceInner(const std::weak_ptr<AppMgrServiceInner> &inner);

    /**
     * @brief Setting application death recipient.
     *
     * @param appDeathRecipient, application death recipient instance.
     */
    void SetAppDeathRecipient(const sptr<AppDeathRecipient> &appDeathRecipient);

    /**
     * RegisterAppDeathRecipient, Register application death recipient.
     *
     * @return
     */
    void RegisterAppDeathRecipient() const;

    /**
     * @brief Obtains application priority info.
     *
     * @return Returns the application priority info.
     */
    std::shared_ptr<PriorityObject> GetPriorityObject();

    /**
     * RegisterAppDeathRecipient, Remove application death recipient record.
     *
     * @return
     */
    void RemoveAppDeathRecipient() const;

private:
    // drive application state changes when ability state changes.
    /**
     * OnAbilityStateChanged, Call ability state change.
     *
     * @param ability, the ability info.
     * @param state, the ability state.
     *
     * @return
     */
    void OnAbilityStateChanged(const std::shared_ptr<AbilityRunningRecord> &ability, const AbilityState state);

    /**
     * AbilityForeground, Handling the ability process when switching to the foreground.
     *
     * @param ability, the ability info.
     *
     * @return
     */
    void AbilityForeground(const std::shared_ptr<AbilityRunningRecord> &ability);

    /**
     * AbilityBackground, Handling the ability process when switching to the background.
     *
     * @param ability, the ability info.
     *
     * @return
     */
    void AbilityBackground(const std::shared_ptr<AbilityRunningRecord> &ability);

    /**
     * OptimizerAbilityStateChanged, Optimizer processing ability state changes.
     *
     * @param ability, the ability info.
     * @param state, the ability state.
     *
     * @return
     */
    void OptimizerAbilityStateChanged(const std::shared_ptr<AbilityRunningRecord> &ability, const AbilityState state);

private:
    ApplicationState curState_ = ApplicationState::APP_STATE_CREATE;  // current state of this process

    std::shared_ptr<ApplicationInfo> appInfo_;  // the application's info of this process
    int32_t appRecordId_ = 0;
    std::string processName_;  // the name of this process
    int32_t uid_ = 0;
    // List of abilities running in the process
    std::map<const sptr<IRemoteObject>, std::shared_ptr<AbilityRunningRecord>> abilities_;
    std::list<const sptr<IRemoteObject>> foregroundingAbilityTokens_;
    std::weak_ptr<AppMgrServiceInner> appMgrServiceInner_;
    sptr<AppDeathRecipient> appDeathRecipient_;
    std::shared_ptr<PriorityObject> priorityObject_;
    std::shared_ptr<AppLifeCycleDeal> appLifeCycleDeal_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_RUNNING_RECORD_H
