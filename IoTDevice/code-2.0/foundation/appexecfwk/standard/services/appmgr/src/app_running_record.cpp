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

#include "app_running_record.h"

#include "ability_running_record.h"
#include "app_log_wrapper.h"
#include "app_mgr_service_inner.h"

namespace OHOS {
namespace AppExecFwk {

AppRunningRecord::AppRunningRecord(
    const std::shared_ptr<ApplicationInfo> &info, const int32_t recordId, const std::string &processName)
    : appInfo_(info), appRecordId_(recordId), processName_(processName)
{}

void AppRunningRecord::SetApplicationClient(const sptr<IAppScheduler> &thread)
{
    if (!appLifeCycleDeal_) {
        appLifeCycleDeal_ = std::make_shared<AppLifeCycleDeal>();
    }
    appLifeCycleDeal_->SetApplicationClient(thread);
}

std::string AppRunningRecord::GetBundleName() const
{
    return appInfo_->bundleName;
}

int32_t AppRunningRecord::GetRecordId() const
{
    return appRecordId_;
}

const std::string &AppRunningRecord::GetName() const
{
    return appInfo_->name;
}

const std::string &AppRunningRecord::GetProcessName() const
{
    return processName_;
}

int32_t AppRunningRecord::GetUid() const
{
    return uid_;
}

void AppRunningRecord::SetUid(const int32_t uid)
{
    uid_ = uid;
}

ApplicationState AppRunningRecord::GetState() const
{
    return curState_;
}

void AppRunningRecord::SetState(const ApplicationState state)
{
    if (state >= ApplicationState::APP_STATE_END) {
        APP_LOGE("Invalid application state");
        return;
    }
    curState_ = state;
}

const std::map<const sptr<IRemoteObject>, std::shared_ptr<AbilityRunningRecord>> &AppRunningRecord::GetAbilities() const
{
    return abilities_;
}

sptr<IAppScheduler> AppRunningRecord::GetApplicationClient() const
{
    return (appLifeCycleDeal_ ? appLifeCycleDeal_->GetApplicationClient() : nullptr);
}

std::shared_ptr<AbilityRunningRecord> AppRunningRecord::AddAbility(
    const sptr<IRemoteObject> &token, const std::shared_ptr<AbilityInfo> &abilityInfo)
{
    if (!token || !abilityInfo) {
        APP_LOGE("Param abilityInfo or token is null");
        return nullptr;
    }
    if (GetAbilityRunningRecordByToken(token)) {
        APP_LOGE("AbilityRecord already exists and no need to add");
        return nullptr;
    }
    auto abilityRecord = std::make_shared<AbilityRunningRecord>(abilityInfo, token);
    abilities_.emplace(token, abilityRecord);
    return abilityRecord;
}

std::shared_ptr<AbilityRunningRecord> AppRunningRecord::GetAbilityRunningRecord(const std::string &abilityName) const
{
    const auto &iter = std::find_if(abilities_.begin(), abilities_.end(), [&abilityName](const auto &pair) {
        return pair.second->GetName() == abilityName;
    });
    return ((iter == abilities_.end()) ? nullptr : iter->second);
}

void AppRunningRecord::ClearAbility(const std::shared_ptr<AbilityRunningRecord> &record)
{
    if (!record) {
        APP_LOGE("Param record is null");
        return;
    }
    if (!GetAbilityRunningRecordByToken(record->GetToken())) {
        APP_LOGE("Param record is not exist");
        return;
    }
    abilities_.erase(record->GetToken());
}

void AppRunningRecord::ForceKillApp([[maybe_unused]] const std::string &reason) const
{}

void AppRunningRecord::ScheduleAppCrash([[maybe_unused]] const std::string &description) const
{}

void AppRunningRecord::LaunchApplication()
{
    if (!appInfo_ || !appLifeCycleDeal_->GetApplicationClient()) {
        APP_LOGE("appInfo or appThread is null");
        return;
    }
    AppLaunchData launchData;
    launchData.SetApplicationInfo(*appInfo_);
    ProcessInfo processInfo(processName_, GetPriorityObject()->GetPid());
    launchData.SetProcessInfo(processInfo);
    launchData.SetRecordId(appRecordId_);
    launchData.SetUId(uid_);
    APP_LOGI("ScheduleLaunchApplication app:%{public}s", GetName().c_str());
    appLifeCycleDeal_->LaunchApplication(launchData);
}

void AppRunningRecord::LaunchAbility(const std::shared_ptr<AbilityRunningRecord> &ability)
{
    if (!ability || !ability->GetToken()) {
        APP_LOGE("null abilityRecord or abilityToken");
        return;
    }
    const auto &iter = abilities_.find(ability->GetToken());
    if (iter != abilities_.end() && appLifeCycleDeal_->GetApplicationClient()) {
        APP_LOGI("ScheduleLaunchAbility ability:%{public}s", ability->GetName().c_str());
        appLifeCycleDeal_->LaunchAbility(ability);
        ability->SetState(AbilityState::ABILITY_STATE_READY);
        OptimizerAbilityStateChanged(ability, AbilityState::ABILITY_STATE_CREATE);
    }
}

void AppRunningRecord::LaunchPendingAbilities()
{
    for (auto item : abilities_) {
        if (item.second->GetState() == AbilityState::ABILITY_STATE_CREATE) {
            LaunchAbility(item.second);
        }
    }
}

void AppRunningRecord::ScheduleTerminate()
{
    appLifeCycleDeal_->ScheduleTerminate();
}

void AppRunningRecord::ScheduleForegroundRunning()
{
    appLifeCycleDeal_->ScheduleForegroundRunning();
}

void AppRunningRecord::ScheduleBackgroundRunning()
{
    appLifeCycleDeal_->ScheduleBackgroundRunning();
}

void AppRunningRecord::ScheduleProcessSecurityExit()
{
    appLifeCycleDeal_->ScheduleProcessSecurityExit();
}

void AppRunningRecord::ScheduleTrimMemory()
{
    appLifeCycleDeal_->ScheduleTrimMemory(priorityObject_->GetTimeLevel());
}

void AppRunningRecord::LowMemoryWarning()
{
    appLifeCycleDeal_->LowMemoryWarning();
}

void AppRunningRecord::OnAbilityStateChanged(
    const std::shared_ptr<AbilityRunningRecord> &ability, const AbilityState state)
{
    if (!ability) {
        APP_LOGE("ability is null");
        return;
    }
    AbilityState oldState = ability->GetState();
    ability->SetState(state);
    OptimizerAbilityStateChanged(ability, oldState);
    auto serviceInner = appMgrServiceInner_.lock();
    if (serviceInner) {
        serviceInner->OnAbilityStateChanged(ability, state);
    }
}

std::shared_ptr<AbilityRunningRecord> AppRunningRecord::GetAbilityRunningRecordByToken(
    const sptr<IRemoteObject> &token) const
{
    if (!token) {
        APP_LOGE("token is null");
        return nullptr;
    }
    const auto &iter = abilities_.find(token);
    if (iter != abilities_.end()) {
        return iter->second;
    }
    return nullptr;
}

void AppRunningRecord::UpdateAbilityState(const sptr<IRemoteObject> &token, const AbilityState state)
{
    APP_LOGD("state is :%{public}d", static_cast<int32_t>(state));
    auto abilityRecord = GetAbilityRunningRecordByToken(token);
    if (!abilityRecord) {
        APP_LOGE("can not find ability record");
        return;
    }
    if (state == abilityRecord->GetState()) {
        APP_LOGE("current state is already, no need update");
        return;
    }

    if (state == AbilityState::ABILITY_STATE_FOREGROUND) {
        AbilityForeground(abilityRecord);
    } else if (state == AbilityState::ABILITY_STATE_BACKGROUND) {
        AbilityBackground(abilityRecord);
    } else {
        APP_LOGW("wrong state");
    }
}

void AppRunningRecord::AbilityForeground(const std::shared_ptr<AbilityRunningRecord> &ability)
{
    if (!ability) {
        APP_LOGE("ability is null");
        return;
    }
    AbilityState curAbilityState = ability->GetState();
    if (curAbilityState != AbilityState::ABILITY_STATE_READY &&
        curAbilityState != AbilityState::ABILITY_STATE_BACKGROUND) {
        APP_LOGE("ability state(%{public}d) error", static_cast<int32_t>(curAbilityState));
        return;
    }

    // We need schedule application to foregrounded when current application state is ready or background running.
    if (curState_ == ApplicationState::APP_STATE_READY || curState_ == ApplicationState::APP_STATE_BACKGROUND) {
        if (foregroundingAbilityTokens_.empty()) {
            ScheduleForegroundRunning();
        }
        foregroundingAbilityTokens_.push_back(ability->GetToken());
        return;
    } else if (curState_ == ApplicationState::APP_STATE_FOREGROUND) {
        // Just change ability to foreground if current application state is foreground.
        OnAbilityStateChanged(ability, AbilityState::ABILITY_STATE_FOREGROUND);
    } else {
        APP_LOGW("wrong application state");
    }
}

void AppRunningRecord::AbilityBackground(const std::shared_ptr<AbilityRunningRecord> &ability)
{
    if (!ability) {
        APP_LOGE("ability is null");
        return;
    }
    if (ability->GetState() != AbilityState::ABILITY_STATE_FOREGROUND) {
        APP_LOGE("ability state is not foreground");
        return;
    }

    // First change ability to backgrounded.
    OnAbilityStateChanged(ability, AbilityState::ABILITY_STATE_BACKGROUND);

    if (curState_ == ApplicationState::APP_STATE_FOREGROUND) {
        int32_t foregroundSize = 0;
        for (const auto &item : abilities_) {
            const auto &abilityRecord = item.second;
            if (abilityRecord && abilityRecord->GetState() == AbilityState::ABILITY_STATE_FOREGROUND) {
                foregroundSize++;
                break;
            }
        }

        // Then schedule application background when all ability is not foreground.
        if (foregroundSize == 0) {
            ScheduleBackgroundRunning();
        }
    } else {
        APP_LOGW("wrong application state");
    }
}

void AppRunningRecord::OptimizerAbilityStateChanged(
    const std::shared_ptr<AbilityRunningRecord> &ability, const AbilityState state)
{
    auto serviceInner = appMgrServiceInner_.lock();
    if (serviceInner) {
        serviceInner->OptimizerAbilityStateChanged(ability, state);
    }
}

void AppRunningRecord::PopForegroundingAbilityTokens()
{
    APP_LOGD("size:%{public}d", static_cast<int32_t>(foregroundingAbilityTokens_.size()));
    while (!foregroundingAbilityTokens_.empty()) {
        const auto &token = foregroundingAbilityTokens_.front();
        auto ability = GetAbilityRunningRecordByToken(token);
        OnAbilityStateChanged(ability, AbilityState::ABILITY_STATE_FOREGROUND);
        foregroundingAbilityTokens_.pop_front();
    }
}

void AppRunningRecord::TerminateAbility(const sptr<IRemoteObject> &token)
{
    APP_LOGD("AppRunningRecord::TerminateAbility begin");
    auto abilityRecord = GetAbilityRunningRecordByToken(token);
    if (!abilityRecord) {
        APP_LOGE("AppRunningRecord::TerminateAbility can not find ability record");
        return;
    }
    auto curAbilityState = abilityRecord->GetState();
    if (curAbilityState != AbilityState::ABILITY_STATE_BACKGROUND) {
        APP_LOGE("AppRunningRecord::TerminateAbility current state(%{public}d) error",
            static_cast<int32_t>(curAbilityState));
        return;
    }

    OptimizerAbilityStateChanged(abilityRecord, AbilityState::ABILITY_STATE_TERMINATED);
    appLifeCycleDeal_->ScheduleCleanAbility(token);

    APP_LOGD("AppRunningRecord::TerminateAbility end");
}

void AppRunningRecord::AbilityTerminated(const sptr<IRemoteObject> &token)
{
    if (!token) {
        APP_LOGE("token is null");
        return;
    }
    abilities_.erase(token);
    if (abilities_.empty()) {
        ScheduleTerminate();
    }
}

void AppRunningRecord::RegisterAppDeathRecipient() const
{
    if (!appLifeCycleDeal_->GetApplicationClient()) {
        APP_LOGE("appThread is null");
        return;
    }
    auto object = appLifeCycleDeal_->GetApplicationClient()->AsObject();
    if (object) {
        object->AddDeathRecipient(appDeathRecipient_);
    }
}

void AppRunningRecord::RemoveAppDeathRecipient() const
{
    if (!appLifeCycleDeal_->GetApplicationClient()) {
        APP_LOGE("appThread is null");
        return;
    }
    auto object = appLifeCycleDeal_->GetApplicationClient()->AsObject();
    if (object) {
        object->RemoveDeathRecipient(appDeathRecipient_);
    }
}

void AppRunningRecord::SetAppMgrServiceInner(const std::weak_ptr<AppMgrServiceInner> &inner)
{
    appMgrServiceInner_ = inner;
}

void AppRunningRecord::SetAppDeathRecipient(const sptr<AppDeathRecipient> &appDeathRecipient)
{
    appDeathRecipient_ = appDeathRecipient;
}

std::shared_ptr<PriorityObject> AppRunningRecord::GetPriorityObject()
{
    if (!priorityObject_) {
        priorityObject_ = std::make_shared<PriorityObject>();
    }

    return priorityObject_;
}

}  // namespace AppExecFwk
}  // namespace OHOS
