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

#include "ams_mgr_scheduler.h"
#include <sys/types.h>

#include "datetime_ex.h"
#include "ipc_skeleton.h"
#include "system_ability_definition.h"

#include "app_death_recipient.h"
#include "app_log_wrapper.h"
#include "app_mgr_constants.h"
#include "perf_profile.h"

namespace OHOS {

namespace AppExecFwk {

namespace {

const std::string TASK_LOAD_ABILITY = "LoadAbilityTask";
const std::string TASK_TERMINATE_ABILITY = "TerminateAbilityTask";
const std::string TASK_UPDATE_ABILITY_STATE = "UpdateAbilityStateTask";
const std::string TASK_REGISTER_APP_STATE_CALLBACK = "RegisterAppStateCallbackTask";
const std::string TASK_STOP_ALL_PROCESS = "StopAllProcessTask";
const std::string TASK_ABILITY_BEHAVIOR_ANALYSIS = "AbilityBehaviorAnalysisTask";
const std::string TASK_KILL_PROCESS_BY_ABILITYTOKEN = "KillProcessByAbilityTokenTask";
const std::string TASK_KILL_APPLICATION = "KillApplicationTask";
};  // namespace

AmsMgrScheduler::AmsMgrScheduler(
    const std::shared_ptr<AppMgrServiceInner> &mgrServiceInner_, const std::shared_ptr<AMSEventHandler> &handler_)
    : amsMgrServiceInner_(mgrServiceInner_), amsHandler_(handler_)
{}

AmsMgrScheduler::~AmsMgrScheduler()
{
    APP_LOGI("AmsMgrScheduler instance destroyed");
}

void AmsMgrScheduler::LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo)
{
    if (!abilityInfo || !appInfo) {
        APP_LOGE("param error");
        return;
    }

    if (!IsReady()) {
        return;
    }
    PerfProfile::GetInstance().SetAbilityLoadStartTime(GetTickCount());
    std::function<void()> loadAbilityFunc =
        std::bind(&AppMgrServiceInner::LoadAbility, amsMgrServiceInner_, token, preToken, abilityInfo, appInfo);

    amsHandler_->PostTask(loadAbilityFunc, TASK_LOAD_ABILITY);
}

void AmsMgrScheduler::UpdateAbilityState(const sptr<IRemoteObject> &token, const AbilityState state)
{
    if (!IsReady()) {
        return;
    }
    std::function<void()> updateAbilityStateFunc =
        std::bind(&AppMgrServiceInner::UpdateAbilityState, amsMgrServiceInner_, token, state);
    amsHandler_->PostTask(updateAbilityStateFunc, TASK_UPDATE_ABILITY_STATE);
}

void AmsMgrScheduler::TerminateAbility(const sptr<IRemoteObject> &token)
{
    if (!IsReady()) {
        return;
    }
    std::function<void()> terminateAbilityFunc =
        std::bind(&AppMgrServiceInner::TerminateAbility, amsMgrServiceInner_, token);
    amsHandler_->PostTask(terminateAbilityFunc, TASK_TERMINATE_ABILITY);
}

void AmsMgrScheduler::RegisterAppStateCallback(const sptr<IAppStateCallback> &callback)
{
    if (!IsReady()) {
        return;
    }
    std::function<void()> registerAppStateCallbackFunc =
        std::bind(&AppMgrServiceInner::RegisterAppStateCallback, amsMgrServiceInner_, callback);
    amsHandler_->PostTask(registerAppStateCallbackFunc, TASK_REGISTER_APP_STATE_CALLBACK);
}

void AmsMgrScheduler::Reset()
{
    if (!IsReady()) {
        return;
    }
    std::function<void()> resetFunc = std::bind(&AppMgrServiceInner::StopAllProcess, amsMgrServiceInner_);
    amsHandler_->PostTask(resetFunc, TASK_STOP_ALL_PROCESS);
}

void AmsMgrScheduler::AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const int32_t visibility, const int32_t perceptibility, const int32_t connectionState)
{
    if (!IsReady()) {
        return;
    }
    std::function<void()> abilityBehaviorAnalysisFunc = std::bind(&AppMgrServiceInner::AbilityBehaviorAnalysis,
        amsMgrServiceInner_,
        token,
        preToken,
        visibility,
        perceptibility,
        connectionState);
    amsHandler_->PostTask(abilityBehaviorAnalysisFunc, TASK_ABILITY_BEHAVIOR_ANALYSIS);
}

void AmsMgrScheduler::KillProcessByAbilityToken(const sptr<IRemoteObject> &token)
{
    if (!IsReady()) {
        return;
    }
    std::function<void()> killProcessByAbilityTokenFunc =
        std::bind(&AppMgrServiceInner::KillProcessByAbilityToken, amsMgrServiceInner_, token);
    amsHandler_->PostTask(killProcessByAbilityTokenFunc, TASK_KILL_PROCESS_BY_ABILITYTOKEN);
}

int32_t AmsMgrScheduler::KillApplication(const std::string &bundleName)
{
    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }
    return amsMgrServiceInner_->KillApplication(bundleName);
}

bool AmsMgrScheduler::IsReady() const
{
    if (!amsMgrServiceInner_) {
        APP_LOGE("amsMgrServiceInner_ is null");
        return false;
    }
    if (!amsHandler_) {
        APP_LOGE("amsHandler_ is null");
        return false;
    }
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS