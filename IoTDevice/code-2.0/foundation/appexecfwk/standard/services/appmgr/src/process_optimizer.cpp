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

#include "process_optimizer.h"

#include <algorithm>
#include <string>
#include <errno.h>

#include "app_log_wrapper.h"

#define LMK_UTIL
#define APP_SUSPEND_TIMER_NAME_PREFIX "AppSuspendTimer"

namespace OHOS {
namespace AppExecFwk {

using namespace std::placeholders;

namespace {
// constexpr int APP_OOM_ADJ_SYSTEM = -1;
// foreground process oom_adj
constexpr int APP_OOM_ADJ_FOREGROUND = 0;

// visible process oom_adj
constexpr int APP_OOM_ADJ_VISIBLE_MIN = 1;
constexpr int APP_OOM_ADJ_VISIBLE_MAX = 199;
constexpr int APP_OOM_ADJ_VISIBLE_MAX_VALUE = 1 * 1024;

// preceptible process oom_adj
constexpr int APP_OOM_ADJ_PERCEPTIBLE_MIN = 200;
constexpr int APP_OOM_ADJ_PERCEPTIBLE_MAX = 399;
constexpr int APP_OOM_ADJ_PERCEPTIBLE_MAX_VALUE = 4 * 1024;

// background process oom_adj
constexpr int APP_OOM_ADJ_BACKGROUND_MIN = 400;
constexpr int APP_OOM_ADJ_BACKGROUND_MAX = 599;
constexpr int APP_OOM_ADJ_BACKGROUND_MAX_VALUE = 16 * 1024;

// suspend process oom_adj
constexpr int APP_OOM_ADJ_SUSPEND_MIN = 600;
constexpr int APP_OOM_ADJ_SUSPEND_MAX = 799;
constexpr int APP_OOM_ADJ_SUSPEND_MAX_VALUE = 24 * 1024;

// empty process oom_adj
// constexpr int APP_OOM_ADJ_EMPTY_MIN = 800;
constexpr int APP_OOM_ADJ_EMPTY_MAX = 999;
constexpr int APP_OOM_ADJ_EMPTY_MAX_VALUE = 32 * 1024;

constexpr int APP_OOM_ADJ_UNKNOWN = 1000;
constexpr int APP_OOM_ADJ_UNKNOWN_VALUE = 64 * 1024;
}  // namespace

ProcessOptimizer::ProcessOptimizer(const LmkdClientPtr &lmkdClient, int suspendTimeout)
    : lmkdClient_(lmkdClient), suspendTimeout_(suspendTimeout)
{
    APP_LOGI("%{public}s(%{public}d) constructed.", __func__, __LINE__);
}

ProcessOptimizer::~ProcessOptimizer()
{
    if (lmkdClient_) {
        lmkdClient_->ProcPurge();
    }

    APP_LOGI("%{public}s(%{public}d) destructed.", __func__, __LINE__);
}

bool ProcessOptimizer::Init()
{
    APP_LOGI("%{public}s(%{public}d) initializing...", __func__, __LINE__);

    if (suspendTimeout_ < 0) {
        APP_LOGE("%{public}s(%{public}d) invalid suspend timeout.", __func__, __LINE__);
        return false;
    }

    if (eventHandler_) {
        APP_LOGE("%{public}s(%{public}d) already inited.", __func__, __LINE__);
        return false;
    }

#ifdef LMK_UTIL
    // create lmkManager
    if (!lmkUtil_) {
        lmkUtil_ = std::make_unique<LmkUtil>();
    }
#endif

    // Initializing cgroup manager.
    if (!DelayedSingleton<CgroupManager>::GetInstance()->IsInited()) {
        APP_LOGW("%{public}s(%{public}d) cgroup manager not inited.", __func__, __LINE__);
        if (DelayedSingleton<CgroupManager>::GetInstance()->Init()) {
            APP_LOGE("%{public}s(%{public}d) cannot init cgroup manager.", __func__, __LINE__);
            return false;
        }
    }

    if (DelayedSingleton<CgroupManager>::GetInstance()->LowMemoryAlert) {
        APP_LOGW("%{public}s(%{public}d) cgroup manager 'LowMemoryWarning' already registered.", __func__, __LINE__);
    }

    DelayedSingleton<CgroupManager>::GetInstance()->LowMemoryAlert =
        std::bind(&ProcessOptimizer::OnLowMemoryAlert, this, _1);

    // Initializing lmkd.
    LmkdClientPtr lmkdClientLocal;
    lmkdClientLocal.swap(lmkdClient_);
    if (!lmkdClientLocal) {
        lmkdClientLocal = std::make_shared<LmkdClient>();
        if (!lmkdClientLocal) {
            APP_LOGE("%{public}s(%{public}d) failed to create lmkd client.", __func__, __LINE__);
            return false;
        }
    }

    if (!lmkdClientLocal->IsOpen()) {
        if (!lmkdClientLocal->Open()) {
            APP_LOGE("%{public}s(%{public}d) cannot open lmkd connection.", __func__, __LINE__);
            return false;
        }
    }

    LmkdClient::Targets targets = {
        {APP_OOM_ADJ_VISIBLE_MAX_VALUE, APP_OOM_ADJ_VISIBLE_MAX},
        {APP_OOM_ADJ_PERCEPTIBLE_MAX_VALUE, APP_OOM_ADJ_PERCEPTIBLE_MAX},
        {APP_OOM_ADJ_BACKGROUND_MAX_VALUE, APP_OOM_ADJ_BACKGROUND_MAX},
        {APP_OOM_ADJ_SUSPEND_MAX_VALUE, APP_OOM_ADJ_SUSPEND_MAX},
        {APP_OOM_ADJ_EMPTY_MAX_VALUE, APP_OOM_ADJ_EMPTY_MAX},
        {APP_OOM_ADJ_UNKNOWN_VALUE, APP_OOM_ADJ_UNKNOWN},
    };

    if (!lmkdClientLocal->Target(targets)) {
        APP_LOGE("%{public}s(%{public}d) cannot init lmkd.", __func__, __LINE__);
        return false;
    }

    // Save initialized states.
    auto eventHandler = EventHandler::Current();
    if (!eventHandler) {
        APP_LOGE("%{public}s(%{public}d) no available event handler for current thread.", __func__, __LINE__);
        return false;
    }

    eventHandler_ = eventHandler;
    lmkdClient_ = lmkdClientLocal;

    return true;
}

void ProcessOptimizer::OnAppAdded(const AppPtr &app)
{
    if (!app) {
        APP_LOGE("%{public}s(%{public}d) invalid app.", __func__, __LINE__);
        return;
    }

    if (app != appLru_.front()) {
        auto it = std::find(appLru_.begin(), appLru_.end(), app);
        if (it != appLru_.end()) {
            APP_LOGE(
                "%{public}s(%{public}d) app '%{public}s' already existed.", __func__, __LINE__, app->GetName().c_str());
            appLru_.erase(it);
        }
        if (app->GetState() == ApplicationState::APP_STATE_FOREGROUND) {
            appLru_.push_front(app);
        } else {
            appLru_.push_back(app);
        }
    }
    // Initial sched policy has been already set by appspawn.
    UpdateAppOomAdj(app);
}

void ProcessOptimizer::OnAppRemoved(const AppPtr &app)
{
    if (!app) {
        APP_LOGE("%{public}s(%{public}d) invalid app.", __func__, __LINE__);
        return;
    }

    if (!lmkdClient_) {
        APP_LOGE("%{public}s(%{public}d) invalid lmkd client.", __func__, __LINE__);
        return;
    }

    // remove timer
    StopAppSuspendTimer(app);

    auto it = std::find(appLru_.begin(), appLru_.end(), app);
    if (it != appLru_.end()) {
        appLru_.erase(it);
    } else {
        APP_LOGE("%{pulbid}s(%{publid}d) app '%{public}s' is not existed.", __func__, __LINE__, app->GetName().c_str());
    }

    auto priorityObject = app->GetPriorityObject();
    if (!priorityObject) {
        APP_LOGE("%{public}s(%{public}d) invalid priority object.", __func__, __LINE__);
        return;
    }

    if (lmkdClient_->ProcRemove(priorityObject->GetPid()) != ERR_OK) {
        APP_LOGE("%{public}s(%{public}d) failed to remove app '%{public}s'(%{publid}d) from lmkd.",
            __func__,
            __LINE__,
            app->GetName().c_str(),
            priorityObject->GetPid());
    }
}

void ProcessOptimizer::OnAppStateChanged(const AppPtr &app, const ApplicationState oldState)
{
    if (!app) {
        APP_LOGE("%{public}s(%{public}d) invalid app.", __func__, __LINE__);
        return;
    }

    auto priorityObject = app->GetPriorityObject();
    if (!priorityObject) {
        APP_LOGE("%{public}s(%{public}d) invalid priority object.", __func__, __LINE__);
        return;
    }

    auto curState = app->GetState();
    APP_LOGD("%{public}s(%{pubic}d) ability state changed to %{public}d.", __func__, __LINE__, curState);
    if (curState == oldState) {
        APP_LOGW("%{public}s(%{public}d) no change.", __func__, __LINE__);
        return;
    }

    if (curState == ApplicationState::APP_STATE_FOREGROUND) {
        if (app != appLru_.front()) {
            auto it = std::find(appLru_.begin(), appLru_.end(), app);
            if (it != appLru_.end()) {
                appLru_.erase(it);
            } else {
                APP_LOGE("%{pulbid}s(%{publid}d) app '%{public}s' is not existed.",
                    __func__,
                    __LINE__,
                    app->GetName().c_str());
            }
            appLru_.push_front(app);
        }
    }

    UpdateAppSchedPolicy(app);
    UpdateAppOomAdj(app);

    // only background also no visible and no preceptible can freezer
    if (curState == ApplicationState::APP_STATE_BACKGROUND &&
        (!priorityObject->GetVisibleStatus() && !priorityObject->GetPerceptibleStatus())) {
        StartAppSuspendTimer(app);
    }

    if (oldState == ApplicationState::APP_STATE_BACKGROUND && curState != ApplicationState::APP_STATE_SUSPENDED) {
        StopAppSuspendTimer(app);
    }
}

void ProcessOptimizer::OnAbilityStarted(const AbilityPtr &ability)
{
    if (!ability) {
        APP_LOGE("%{public}s(%{public}d) invalid ability.", __func__, __LINE__);
        return;
    }

    APP_LOGI("%{public}s(%{public}d) OnAbilityStarted.", __func__, __LINE__);
}

void ProcessOptimizer::OnAbilityConnected(const AbilityPtr &ability, const AbilityPtr &targetAbility)
{
    if (!ability) {
        APP_LOGE("%{public}s(%{public}d) invalid ability.", __func__, __LINE__);
        return;
    }

    if (!targetAbility) {
        APP_LOGE("%{public}s(%{public}d) invalid targetAbility.", __func__, __LINE__);
        return;
    }

    APP_LOGI("%{public}s(%{public}d) OnAbilityConnected.", __func__, __LINE__);
}

void ProcessOptimizer::OnAbilityDisconnected(const AbilityPtr &ability, const AbilityPtr &targetAbility)
{
    if (!ability) {
        APP_LOGE("%{public}s(%{public}d) invalid ability.", __func__, __LINE__);
        return;
    }

    if (!targetAbility) {
        APP_LOGE("%{public}s(%{public}d) invalid targetAbility.", __func__, __LINE__);
        return;
    }

    APP_LOGI("%{public}s(%{public}d) OnAbilityDisconnected.", __func__, __LINE__);
}

void ProcessOptimizer::OnAbilityStateChanged(const AbilityPtr &ability, const AbilityState oldState)
{
    if (!ability) {
        APP_LOGE("%{public}s(%{public}d) invalid ability.", __func__, __LINE__);
        return;
    }

    APP_LOGI("%{public}s(%{public}d) OnAbilityStateChanged.", __func__, __LINE__);
}

void ProcessOptimizer::OnAbilityVisibleChanged(const AbilityPtr &ability)
{
    if (!ability) {
        APP_LOGE("%{public}s(%{public}d) invalid ability.", __func__, __LINE__);
        return;
    }

    APP_LOGI("%{public}s(%{public}d) OnAbilityVisibleChanged.", __func__, __LINE__);
}

void ProcessOptimizer::OnAbilityPerceptibleChanged(const AbilityPtr &ability)
{
    if (!ability) {
        APP_LOGE("%{public}s(%{public}d) invalid ability.", __func__, __LINE__);
    }

    APP_LOGI("%{public}s(%{public}d) OnAbilityPerceptibleChanged.", __func__, __LINE__);
}

void ProcessOptimizer::OnAbilityRemoved(const AbilityPtr &ability)
{
    if (!ability) {
        APP_LOGE("%{public}s(%{public}d) invalid ability.", __func__, __LINE__);
        return;
    }

    APP_LOGI("%{public}s(%{public}d) OnAbilityRemoved.", __func__, __LINE__);
}

bool ProcessOptimizer::SetAppOomAdj(const AppPtr &app, int oomAdj)
{
    if (!app) {
        APP_LOGE("%{public}s(%{public}d) invalid app.", __func__, __LINE__);
        return false;
    }

    if (!LmkdClient::CheckOomAdj(oomAdj)) {
        APP_LOGE("%{public}s(%{public}d) invalid oom adj %{public}d.", __func__, __LINE__, oomAdj);
        return false;
    }

    if (!lmkdClient_) {
        APP_LOGE("%{public}s(%{public}d) invalid lmkd client.", __func__, __LINE__);
        return false;
    }

    auto priorityObject = app->GetPriorityObject();
    if (!priorityObject) {
        APP_LOGE("%{public}s(%{public}d) invalid priority object.", __func__, __LINE__);
        return false;
    }

    if (priorityObject->GetCurAdj() == oomAdj) {
        APP_LOGW("%{public}s(%{public}d) oom adj has no change.", __func__, __LINE__);
        return true;
    }

    if (lmkdClient_->ProcPrio(priorityObject->GetPid(), app->GetUid(), oomAdj) != ERR_OK) {
        APP_LOGE("%{public}s(%{public}d) lmkd proc prio failed.", __func__, __LINE__);
        return false;
    }

    priorityObject->SetCurAdj(oomAdj);

    return true;
}

bool ProcessOptimizer::SetAppSchedPolicy(const AppPtr &app, const CgroupManager::SchedPolicy schedPolicy)
{
    if (!app) {
        APP_LOGE("%{public}s(%{public}d) invalid app.", __func__, __LINE__);
        return false;
    }

    auto priorityObject = app->GetPriorityObject();
    if (!priorityObject) {
        APP_LOGE("%{public}s(%{public}d) invalid priority object.", __func__, __LINE__);
        return false;
    }

    auto oldSchedPolicy = priorityObject->GetCurCgroup();
    if (oldSchedPolicy == schedPolicy) {
        APP_LOGW("%{public}s(%{public}d) no change.", __func__, __LINE__);
        return true;
    }

    bool result =
        DelayedSingleton<CgroupManager>::GetInstance()->SetProcessSchedPolicy(priorityObject->GetPid(), schedPolicy);
    if (result) {
        priorityObject->SetCurCgroup(schedPolicy);
        if (schedPolicy == CgroupManager::SCHED_POLICY_FREEZED) {
            if (AppSuspended) {
                AppSuspended(app);
            }
        } else if (oldSchedPolicy == CgroupManager::SCHED_POLICY_FREEZED) {
            if (AppResumed) {
                AppResumed(app);
            }
        }
    }

    return result;
}

void ProcessOptimizer::OnLowMemoryAlert(const CgroupManager::LowMemoryLevel level)
{
    // Find the oldest background app.
    for (auto it(appLru_.rbegin()); it != appLru_.rend(); ++it) {
        if ((*it)->GetState() == ApplicationState::APP_STATE_BACKGROUND) {
            AppLowMemoryAlert(*it, level);
            break;
        }
    }

    // kill process
    if (lmkUtil_) {
        int32_t proc_cnt = 0;
        int64_t free_rss = 0;

        // kill process and free memory by oom_adj. the function will remove app object in appLru
        proc_cnt = lmkUtil_->KillProcess(appLru_, level, free_rss);
        #if __WORDSIZE == 64
            APP_LOGI("%{public}s(%{public}d) free memory proc count %{public}d rss %{public}ld",
            __func__,
            __LINE__,
            proc_cnt,
            free_rss);
        #else
            APP_LOGI("%{public}s(%{public}d) free memory proc count %{public}d rss %{public}lld",
            __func__,
            __LINE__,
            proc_cnt,
            free_rss);
        #endif

    }
}

bool ProcessOptimizer::UpdateAppOomAdj(const AppPtr &app)
{
    if (!app) {
        APP_LOGE("%{public}s(%{public}d) invalid app.", __func__, __LINE__);
        return false;
    }

    auto priorityObject = app->GetPriorityObject();
    if (!priorityObject) {
        APP_LOGE("%{public}s(%{public}d) invalid priority object.", __func__, __LINE__);
        return false;
    }

    auto state = app->GetState();

    if (state == ApplicationState::APP_STATE_FOREGROUND || state == ApplicationState::APP_STATE_CREATE ||
        state == ApplicationState::APP_STATE_READY) {
        return SetAppOomAdj(app, APP_OOM_ADJ_FOREGROUND);
    }

    int oomAdj;
    int oomAdjMax;

    switch (state) {
        case ApplicationState::APP_STATE_BACKGROUND:
            oomAdj = APP_OOM_ADJ_BACKGROUND_MIN;
            oomAdjMax = APP_OOM_ADJ_BACKGROUND_MAX;

            // perceptible oom_adj
            if (priorityObject->GetPerceptibleStatus()) {
                oomAdj = APP_OOM_ADJ_PERCEPTIBLE_MIN;
                oomAdjMax = APP_OOM_ADJ_PERCEPTIBLE_MAX;
            }

            // visible oom_adj
            if (priorityObject->GetVisibleStatus()) {
                oomAdj = APP_OOM_ADJ_VISIBLE_MIN;
                oomAdjMax = APP_OOM_ADJ_VISIBLE_MAX;
            }
            break;
        case ApplicationState::APP_STATE_SUSPENDED:
            oomAdj = APP_OOM_ADJ_SUSPEND_MIN;
            oomAdjMax = APP_OOM_ADJ_SUSPEND_MAX;
            break;
        default:
            oomAdj = APP_OOM_ADJ_UNKNOWN;
            oomAdjMax = APP_OOM_ADJ_UNKNOWN;
            break;
    }

    for (auto curApp : appLru_) {
        if (curApp->GetState() != state) {
            continue;
        }
        SetAppOomAdj(curApp, oomAdj);
        if (oomAdj < oomAdjMax) {
            oomAdj += 1;
        }
    }

    return true;
}

bool ProcessOptimizer::UpdateAppSchedPolicy(const AppPtr &app)
{
    if (!app) {
        APP_LOGE("%{public}s(%{public}d) invalid app.", __func__, __LINE__);
        return false;
    }

    bool ret = false;

    switch (app->GetState()) {
        case ApplicationState::APP_STATE_CREATE:
        case ApplicationState::APP_STATE_READY:
        case ApplicationState::APP_STATE_FOREGROUND:
            ret = SetAppSchedPolicy(app, CgroupManager::SCHED_POLICY_DEFAULT);
            break;
        case ApplicationState::APP_STATE_BACKGROUND:
            ret = SetAppSchedPolicy(app, CgroupManager::SCHED_POLICY_BACKGROUND);
            break;
        case ApplicationState::APP_STATE_SUSPENDED:
            // SUSPEND state should be set in 'ProcessOptimizer::SuspendApp()' (to be specific,
            // in 'AppSuspended' callback), and the sched policy has been set in it, so do nothing here.
            ret = true;
            break;
        default:
            ret = true;
            break;
    }

    return ret;
}

void ProcessOptimizer::StartAppSuspendTimer(const AppPtr &app)
{
    if (!app) {
        APP_LOGE("%{public}s(%{public}d) invalid app.", __func__, __LINE__);
        return;
    }

    APP_LOGI("%{public}s(%{public}d) starting suspend timer for app '%{public}s'...",
        __func__,
        __LINE__,
        app->GetName().c_str());

    if (!eventHandler_) {
        APP_LOGE("%{public}s(%{public}d) invalid event handler.", __func__, __LINE__);
        return;
    }

    auto timerName = GetAppSuspendTimerName(app);
    if (timerName.empty()) {
        APP_LOGE("%{public}s(%{public}d) invalid suspend timer name.", __func__, __LINE__);
        return;
    }

    auto it = suspendTimers_.find(timerName);
    if (it != suspendTimers_.end()) {
        APP_LOGW("%{public}s(%{public}d) app '%{public}s' suspend timer already started.",
            __func__,
            __LINE__,
            app->GetName().c_str());
        return;
    }

    suspendTimers_.emplace(timerName);

    eventHandler_->PostTask(
        [=]() {
            auto it = suspendTimers_.find(timerName);
            if (it != suspendTimers_.end()) {
                APP_LOGD("%{public}s(%{public}d) removing app '%{public}s' suspend timer name...",
                    timerName.c_str(),
                    __LINE__,
                    app->GetName().c_str());
                suspendTimers_.erase(it);
            } else {
                APP_LOGE("%{public}s(%{public}d) invalid suspend timer for app '%{public}s'.",
                    timerName.c_str(),
                    __LINE__,
                    app->GetName().c_str());
            }
            SuspendApp(app);
        },
        timerName,
        suspendTimeout_);
}

void ProcessOptimizer::StopAppSuspendTimer(const AppPtr &app)
{
    if (!app) {
        APP_LOGE("%{public}s(%{public}d) invalid app.", __func__, __LINE__);
        return;
    }

    APP_LOGI("%{public}s(%{public}d) stopping suspend timer for app '%{public}s'...",
        __func__,
        __LINE__,
        app->GetName().c_str());

    if (!eventHandler_) {
        APP_LOGE("%{public}s(%{public}d) invalid event handler.", __func__, __LINE__);
        return;
    }

    auto timerName = GetAppSuspendTimerName(app);
    if (timerName.empty()) {
        APP_LOGE("%{public}s(%{public}d) invalid suspend timer name.", __func__, __LINE__);
        return;
    }

    auto it = suspendTimers_.find(timerName);
    if (it == suspendTimers_.end()) {
        APP_LOGW("%{public}s(%{public}d) app '%{public}s' suspend timer not started.",
            __func__,
            __LINE__,
            app->GetName().c_str());
        return;
    }

    suspendTimers_.erase(it);
    eventHandler_->RemoveTask(timerName);
}

void ProcessOptimizer::SuspendApp(const AppPtr &app)
{
    if (!app) {
        APP_LOGE("%{public}s(%{public}d) invalid app.", __func__, __LINE__);
        return;
    }

    APP_LOGI("%{public}s(%{public}d) suspending app '%{public}s'...", __func__, __LINE__, app->GetName().c_str());

    if (app->GetState() == ApplicationState::APP_STATE_SUSPENDED) {
        APP_LOGE(
            "%{public}s(%{public}d) app '%{public}s' already suspended.", __func__, __LINE__, app->GetName().c_str());
        return;
    }

    if (!SetAppSchedPolicy(app, CgroupManager::SCHED_POLICY_FREEZED)) {
        APP_LOGE("%{public}s(%{public}d) failed to suspend app '%s'.", __func__, __LINE__, app->GetName().c_str());
    }
}

std::string ProcessOptimizer::GetAppSuspendTimerName(const AppPtr &app)
{
    std::string ret;

    if (app) {
        auto priorityObject = app->GetPriorityObject();
        if (priorityObject) {
            ret = APP_SUSPEND_TIMER_NAME_PREFIX + std::to_string(priorityObject->GetPid());
        }
    }

    return ret;
}
}  // namespace AppExecFwk
}  // namespace OHOS
