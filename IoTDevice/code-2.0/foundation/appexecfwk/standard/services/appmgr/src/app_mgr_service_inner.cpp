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

#include "app_mgr_service_inner.h"

#include <sys/stat.h>
#include <securec.h>
#include <csignal>

#include "app_log_wrapper.h"
#include "perf_profile.h"
#include "datetime_ex.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "iremote_object.h"

#include "app_process_data.h"
namespace OHOS {
namespace AppExecFwk {
namespace {

// NANOSECONDS mean 10^9 nano second
constexpr int64_t NANOSECONDS = 1000000000;
// MICROSECONDS mean 10^6 millias second
constexpr int64_t MICROSECONDS = 1000000;
// Kill process timeout setting
constexpr int KILL_PROCESS_TIMEOUT_MICRO_SECONDS = 1000;
// Kill process delaytime setting
constexpr int KILL_PROCESS_DELAYTIME_MICRO_SECONDS = 200;
const std::string CLASS_NAME = "ohos.app.MainThread";
const std::string FUNC_NAME = "main";
const std::string SO_PATH = "system/lib64/libmapleappkit.z.so";
const int32_t SIGNAL_KILL = 9;
const std::string REQ_PERMISSION = "ohos.permission.LOCATION_IN_BACKGROUND";

}  // namespace

AppMgrServiceInner::AppMgrServiceInner()
    : appProcessManager_(std::make_shared<AppProcessManager>()),
      remoteClientManager_(std::make_shared<RemoteClientManager>()),
      appRunningManager_(std::make_shared<AppRunningManager>())
{}

AppMgrServiceInner::~AppMgrServiceInner()
{}

void AppMgrServiceInner::LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo)
{
    if (!token || !abilityInfo || !appInfo) {
        APP_LOGE("param error");
        return;
    }
    if (abilityInfo->name.empty() || appInfo->name.empty()) {
        APP_LOGE("error abilityInfo or appInfo");
        return;
    }
    if (abilityInfo->applicationName != appInfo->name) {
        APP_LOGE("abilityInfo and appInfo have different appName, don't load for it");
        return;
    }

    std::string processName;
    if (abilityInfo->process.empty()) {
        processName = appInfo->bundleName;
    } else {
        processName = abilityInfo->process;
    }
    auto appRecord = GetAppRunningRecordByProcessName(appInfo->name, processName);
    if (!appRecord) {
        RecordQueryResult result;
        int32_t defaultUid = 0;
        appRecord = GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, processName, defaultUid, result);
        if (FAILED(result.error)) {
            APP_LOGE("create appRunningRecord failed");
            return;
        }
        if (preToken != nullptr) {
            auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
            abilityRecord->SetPreToken(preToken);
        }
        StartProcess(abilityInfo->applicationName, processName, appRecord);
    } else {
        StartAbility(token, preToken, abilityInfo, appRecord);
    }
    PerfProfile::GetInstance().SetAbilityLoadEndTime(GetTickCount());
    PerfProfile::GetInstance().Dump();
    PerfProfile::GetInstance().Reset();
}

void AppMgrServiceInner::AttachApplication(const pid_t pid, const sptr<IAppScheduler> &app)
{
    if (pid <= 0) {
        APP_LOGE("invalid pid:%{public}d", pid);
        return;
    }
    if (!app) {
        APP_LOGE("app client is null");
        return;
    }
    APP_LOGI("attach application pid:%{public}d", pid);
    auto appRecord = GetAppRunningRecordByPid(pid);
    if (!appRecord) {
        APP_LOGE("no such appRecord");
        return;
    }
    appRecord->SetApplicationClient(app);
    if (appRecord->GetState() == ApplicationState::APP_STATE_CREATE) {
        LaunchApplication(appRecord);
    }
    appRecord->RegisterAppDeathRecipient();
}

void AppMgrServiceInner::LaunchApplication(const std::shared_ptr<AppRunningRecord> &appRecord)
{
    if (!appRecord) {
        APP_LOGE("appRecord is null");
        return;
    }
    if (appRecord->GetState() != ApplicationState::APP_STATE_CREATE) {
        APP_LOGE("wrong app state");
        return;
    }
    appRecord->LaunchApplication();
    appRecord->SetState(ApplicationState::APP_STATE_READY);
    OptimizerAppStateChanged(appRecord, ApplicationState::APP_STATE_CREATE);
    appRecord->LaunchPendingAbilities();
}

void AppMgrServiceInner::ApplicationForegrounded(const int32_t recordId)
{
    auto appRecord = GetAppRunningRecordByAppRecordId(recordId);
    if (!appRecord) {
        APP_LOGE("get app record failed");
        return;
    }
    appRecord->PopForegroundingAbilityTokens();
    ApplicationState appState = appRecord->GetState();
    if (appState == ApplicationState::APP_STATE_READY || appState == ApplicationState::APP_STATE_BACKGROUND) {
        appRecord->SetState(ApplicationState::APP_STATE_FOREGROUND);
        OptimizerAppStateChanged(appRecord, appState);
        OnAppStateChanged(appRecord, ApplicationState::APP_STATE_FOREGROUND);
    } else {
        APP_LOGW("app name(%{public}s), app state(%{public}d)!",
            appRecord->GetName().c_str(),
            static_cast<ApplicationState>(appState));
    }

    // push the foregrounded app front of RecentAppList.
    PushAppFront(recordId);
    APP_LOGI("application is foregrounded");
}

void AppMgrServiceInner::ApplicationBackgrounded(const int32_t recordId)
{
    auto appRecord = GetAppRunningRecordByAppRecordId(recordId);
    if (!appRecord) {
        APP_LOGE("get app record failed");
        return;
    }
    if (appRecord->GetState() == ApplicationState::APP_STATE_FOREGROUND) {
        appRecord->SetState(ApplicationState::APP_STATE_BACKGROUND);
        OptimizerAppStateChanged(appRecord, ApplicationState::APP_STATE_FOREGROUND);
        OnAppStateChanged(appRecord, ApplicationState::APP_STATE_BACKGROUND);
    } else {
        APP_LOGW("app name(%{public}s), app state(%{public}d)!",
            appRecord->GetName().c_str(),
            static_cast<ApplicationState>(appRecord->GetState()));
    }

    APP_LOGI("application is backgrounded");
}

void AppMgrServiceInner::ApplicationTerminated(const int32_t recordId)
{
    auto appRecord = GetAppRunningRecordByAppRecordId(recordId);
    if (!appRecord) {
        APP_LOGE("get app record failed");
        return;
    }
    if (appRecord->GetState() != ApplicationState::APP_STATE_BACKGROUND) {
        APP_LOGE("current state is not background");
        return;
    }
    appRecord->SetState(ApplicationState::APP_STATE_TERMINATED);
    OptimizerAppStateChanged(appRecord, ApplicationState::APP_STATE_BACKGROUND);
    appRecord->RemoveAppDeathRecipient();
    OnAppStateChanged(appRecord, ApplicationState::APP_STATE_TERMINATED);
    appRunningManager_->RemoveAppRunningRecordById(recordId);
    RemoveAppFromRecentListById(recordId);

    APP_LOGI("application is terminated");
}

int32_t AppMgrServiceInner::KillApplication(const std::string &bundleName)
{
    int result = ERR_OK;
    int64_t startTime = SystemTimeMillis();
    std::list<pid_t> pids;
    if (!GetPidsByBundleName(bundleName, pids)) {
        APP_LOGI("The process corresponding to the package name did not start");
        return result;
    }
    if (WaitForRemoteProcessExit(pids, startTime)) {
        APP_LOGI("The remote process exited successfully ");
        return result;
    }
    for (auto iter = pids.begin(); iter != pids.end(); ++iter) {
        result = KillProcessByPid(*iter);
        if (result < 0) {
            APP_LOGE("KillApplication is fail bundleName: %{public}s pid: %{public}d", bundleName.c_str(), *iter);
            return result;
        }
    }
    return result;
}

void AppMgrServiceInner::ClearUpApplicationData(const std::string &bundleName, int32_t uid, pid_t pid)
{
    if (pid <= 0) {
        APP_LOGE("invalid pid:%{public}d", pid);
        return;
    }
    if (uid <= 0) {
        APP_LOGE("invalid uid:%{public}d", uid);
        return;
    }
    auto bundleMgr_ = remoteClientManager_->GetBundleManager();
    if (bundleMgr_ == nullptr) {
        APP_LOGE("GetBundleManager fail");
        return;
    }
    int32_t result = 0;
    int32_t bmsUid = bundleMgr_->GetUidByBundleName(bundleName, 0);
    if (bmsUid != uid) {
        result = bundleMgr_->CheckPermission(bundleName, REQ_PERMISSION);
        if (result) {
            APP_LOGE("No permission to clear application data");
            return;
        }
    } else {
        result = bundleMgr_->CheckPermission(bundleName, REQ_PERMISSION);
        if (result) {
            // request to clear user information permission.
        }
    }
    // 2.delete bundle side user data
    if (!bundleMgr_->CleanBundleDataFiles(bundleName)) {
        APP_LOGE("Delete bundle side user data is fail");
        return;
    }
    // 3.kill application
    // 4.revoke user rights
    result = KillApplication(bundleName);
    if (result < 0) {
        APP_LOGE("Kill Application by bundle name is fail");
        return;
    }
}

int32_t AppMgrServiceInner::IsBackgroundRunningRestricted(const std::string &bundleName)
{
    auto bundleMgr_ = remoteClientManager_->GetBundleManager();
    if (bundleMgr_ == nullptr) {
        APP_LOGE("GetBundleManager fail");
        return ERR_DEAD_OBJECT;
    }
    return bundleMgr_->CheckPermission(bundleName, REQ_PERMISSION);
}

int32_t AppMgrServiceInner::GetAllRunningProcesses(std::shared_ptr<RunningProcessInfo> &runningProcessInfo)
{
    auto bundleMgr_ = remoteClientManager_->GetBundleManager();
    if (bundleMgr_ == nullptr) {
        APP_LOGE("GetBundleManager fail");
        return ERR_DEAD_OBJECT;
    }
    // check permission
    for (const auto &item : appRunningManager_->GetAppRunningRecordMap()) {
        const auto &appRecord = item.second;
        AppProcessInfo appProcessInfo_;
        appProcessInfo_.processName_ = appRecord->GetName();
        appProcessInfo_.pid_ = appRecord->GetPriorityObject()->GetPid();
        appProcessInfo_.uid_ = appRecord->GetUid();
        appProcessInfo_.state_ = static_cast<AppProcessState>(appRecord->GetState());
        runningProcessInfo->appProcessInfos.push_back(appProcessInfo_);
    }
    return ERR_OK;
}

int32_t AppMgrServiceInner::KillProcessByPid(const pid_t pid) const
{
    int32_t ret = -1;
    if (pid > 0) {
        APP_LOGI("kill pid %{public}d", pid);
        ret = kill(pid, SIGNAL_KILL);
    }
    return ret;
}

bool AppMgrServiceInner::WaitForRemoteProcessExit(std::list<pid_t> &pids, const int64_t startTime)
{
    int64_t delayTime = SystemTimeMillis() - startTime;
    while (delayTime < KILL_PROCESS_TIMEOUT_MICRO_SECONDS) {
        if (CheckALLProcessExist(pids)) {
            return true;
        }
        usleep(KILL_PROCESS_DELAYTIME_MICRO_SECONDS);
        delayTime = SystemTimeMillis() - startTime;
    }
    return false;
}

bool AppMgrServiceInner::GetPidsByBundleName(const std::string &bundleName, std::list<pid_t> &pids)
{
    for (const auto &item : appRunningManager_->GetAppRunningRecordMap()) {
        const auto &appRecord = item.second;
        if (appRecord->GetBundleName() == bundleName) {
            pid_t pid = appRecord->GetPriorityObject()->GetPid();
            if (pid > 0) {
                pids.push_back(pid);
                appRecord->ScheduleProcessSecurityExit();
            }
        }
    }

    return (pids.empty() ? false : true);
}

bool AppMgrServiceInner::GetAllPids(std::list<pid_t> &pids)
{
    for (const auto &appTaskInfo : appProcessManager_->GetRecentAppList()) {
        if (appTaskInfo) {
            auto appRecord = GetAppRunningRecordByPid(appTaskInfo->GetPid());
            if (appRecord) {
                pids.push_back(appTaskInfo->GetPid());
                appRecord->ScheduleProcessSecurityExit();
            }
        }
    }
    return (pids.empty() ? false : true);
}

bool AppMgrServiceInner::process_exist(pid_t &pid)
{
    char pid_path[128] = {0};
    struct stat stat_buf;
    if (!pid) {
        return false;
    }
    if (snprintf_s(pid_path, sizeof(pid_path), sizeof(pid_path) - 1, "/proc/%d/status", pid) < 0) {
        return false;
    }
    if (stat(pid_path, &stat_buf) == 0) {
        return true;
    }
    return false;
}

bool AppMgrServiceInner::CheckALLProcessExist(std::list<pid_t> &pids)
{
    for (auto iter = pids.begin(); iter != pids.end();) {
        if (!process_exist(*iter) && pids.size() != 0) {
            pids.erase(iter);
            if (pids.empty()) {
                return true;
            }
        } else {
            iter++;
        }
    }
    return false;
}

int64_t AppMgrServiceInner::SystemTimeMillis()
{
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (int64_t)((t.tv_sec) * NANOSECONDS + t.tv_nsec) / MICROSECONDS;
}

std::shared_ptr<AppRunningRecord> AppMgrServiceInner::GetAppRunningRecordByAppName(const std::string &appName) const
{
    return appRunningManager_->GetAppRunningRecordByAppName(appName);
}

std::shared_ptr<AppRunningRecord> AppMgrServiceInner::GetAppRunningRecordByProcessName(
    const std::string &appName, const std::string &processName) const
{
    return appRunningManager_->GetAppRunningRecordByProcessName(appName, processName);
}

std::shared_ptr<AppRunningRecord> AppMgrServiceInner::GetAppRunningRecordByPid(const pid_t pid) const
{
    return appRunningManager_->GetAppRunningRecordByPid(pid);
}

std::shared_ptr<AppRunningRecord> AppMgrServiceInner::GetOrCreateAppRunningRecord(const sptr<IRemoteObject> &token,
    const std::shared_ptr<ApplicationInfo> &appInfo, const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::string &processName, const int32_t uid, RecordQueryResult &result)
{
    return appRunningManager_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, processName, uid, result);
}

void AppMgrServiceInner::TerminateAbility(const sptr<IRemoteObject> &token)
{
    APP_LOGD("AppMgrServiceInner::TerminateAbility begin");
    if (!token) {
        APP_LOGE("AppMgrServiceInner::TerminateAbility token is null!");
        return;
    }
    auto appRecord = GetAppRunningRecordByAbilityToken(token);
    if (!appRecord) {
        APP_LOGE("AppMgrServiceInner::TerminateAbility app is not exist!");
        return;
    }
    if (appRecord->GetState() == ApplicationState::APP_STATE_SUSPENDED) {
        UnsuspendApplication(appRecord);
        OptimizerAppStateChanged(appRecord, ApplicationState::APP_STATE_SUSPENDED);
    }
    appRecord->TerminateAbility(token);
    APP_LOGD("AppMgrServiceInner::TerminateAbility end");
}

void AppMgrServiceInner::UpdateAbilityState(const sptr<IRemoteObject> &token, const AbilityState state)
{
    if (!token) {
        APP_LOGE("token is null!");
        return;
    }
    if (state > AbilityState::ABILITY_STATE_BACKGROUND || state < AbilityState::ABILITY_STATE_FOREGROUND) {
        APP_LOGE("state is not foreground or background!");
        return;
    }
    auto appRecord = GetAppRunningRecordByAbilityToken(token);
    if (!appRecord) {
        APP_LOGE("app is not exist!");
        return;
    }
    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
    if (!abilityRecord) {
        APP_LOGE("can not find ability record!");
        return;
    }
    if (state == abilityRecord->GetState()) {
        APP_LOGE("current state is already, no need update!");
        return;
    }
    if (appRecord->GetState() == ApplicationState::APP_STATE_SUSPENDED) {
        UnsuspendApplication(appRecord);
        OptimizerAppStateChanged(appRecord, ApplicationState::APP_STATE_SUSPENDED);
    }
    appRecord->UpdateAbilityState(token, state);
    APP_LOGD("end");
}

void AppMgrServiceInner::OnStop()
{
    appRunningManager_->ClearAppRunningRecordMap();
    CloseAppSpawnConnection();
}

ErrCode AppMgrServiceInner::OpenAppSpawnConnection()
{
    if (remoteClientManager_->GetSpawnClient()) {
        return remoteClientManager_->GetSpawnClient()->OpenConnection();
    }
    return ERR_APPEXECFWK_BAD_APPSPAWN_CLIENT;
}

void AppMgrServiceInner::CloseAppSpawnConnection() const
{
    if (remoteClientManager_->GetSpawnClient()) {
        remoteClientManager_->GetSpawnClient()->CloseConnection();
    }
}

SpawnConnectionState AppMgrServiceInner::QueryAppSpawnConnectionState() const
{
    if (remoteClientManager_->GetSpawnClient()) {
        return remoteClientManager_->GetSpawnClient()->QueryConnectionState();
    }
    return SpawnConnectionState::STATE_NOT_CONNECT;
}

const std::map<const int32_t, const std::shared_ptr<AppRunningRecord>> &AppMgrServiceInner::GetRecordMap() const
{
    return appRunningManager_->GetAppRunningRecordMap();
}

void AppMgrServiceInner::SetAppSpawnClient(std::shared_ptr<AppSpawnClient> spawnClient)
{
    remoteClientManager_->SetSpawnClient(std::move(spawnClient));
}

void AppMgrServiceInner::SetBundleManager(sptr<IBundleMgr> bundleManager)
{
    remoteClientManager_->SetBundleManager(bundleManager);
}

void AppMgrServiceInner::RegisterAppStateCallback(const sptr<IAppStateCallback> &callback)
{
    if (callback != nullptr) {
        appStateCallbacks_.push_back(callback);
    }
}

void AppMgrServiceInner::StopAllProcess()
{
    ClearRecentAppList();
    appRunningManager_->ClearAppRunningRecordMap();
}

void AppMgrServiceInner::AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const int32_t visibility,       // 0:false,1:true
    const int32_t perceptibility,   // 0:false,1:true
    const int32_t connectionState)  // 0:false,1:true
{
    if (!token) {
        APP_LOGE("token is null");
        return;
    }
    auto appRecord = GetAppRunningRecordByAbilityToken(token);
    if (!appRecord) {
        APP_LOGE("app record is not exist for ability token");
        return;
    }
    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
    if (!abilityRecord) {
        APP_LOGE("ability record is not exist for ability previous token");
        return;
    }
    if (preToken) {
        abilityRecord->SetPreToken(preToken);
    }
    if (abilityRecord->GetVisibility() != visibility) {
        if (processOptimizerUBA_) {
            processOptimizerUBA_->OnAbilityVisibleChanged(abilityRecord);
        }
    }
    if (abilityRecord->GetPerceptibility() != perceptibility) {
        if (processOptimizerUBA_) {
            processOptimizerUBA_->OnAbilityPerceptibleChanged(abilityRecord);
        }
    }
    abilityRecord->SetVisibility(visibility);
    abilityRecord->SetPerceptibility(perceptibility);
    abilityRecord->SetConnectionState(connectionState);
    OptimizerAbilityStateChanged(abilityRecord, abilityRecord->GetState());
}

void AppMgrServiceInner::KillProcessByAbilityToken(const sptr<IRemoteObject> &token)
{
    if (!token) {
        APP_LOGE("token is null");
        return;
    }
    auto appRecord = GetAppRunningRecordByAbilityToken(token);
    if (!appRecord) {
        APP_LOGE("app record is not exist for ability token");
        return;
    }
    std::list<pid_t> pids;
    pid_t pid = appRecord->GetPriorityObject()->GetPid();
    if (pid > 0) {
        pids.push_back(pid);
        appRecord->ScheduleProcessSecurityExit();
        if (!WaitForRemoteProcessExit(pids, SystemTimeMillis())) {
            int32_t result = KillProcessByPid(pid);
            if (result < 0) {
                APP_LOGE("KillProcessByAbilityToken kill process is fail");
                return;
            }
        }
    }
}

void AppMgrServiceInner::StartAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<AppRunningRecord> &appRecord)
{
    APP_LOGI("already create appRecord, just start ability");
    if (!appRecord) {
        APP_LOGE("appRecord is null");
        return;
    }

    if (abilityInfo->launchMode == LaunchMode::SINGLETON) {
        auto abilityRecord = appRecord->GetAbilityRunningRecord(abilityInfo->name);
        if (abilityRecord) {
            APP_LOGW("same ability info in singleton launch mode, will not add ability");
            return;
        }
    }

    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(token);
    if (abilityRecord && preToken) {
        APP_LOGE("Ability is already started");
        abilityRecord->SetPreToken(preToken);
        return;
    }

    ApplicationState appState = appRecord->GetState();
    if (appState == ApplicationState::APP_STATE_SUSPENDED) {
        UnsuspendApplication(appRecord);
        OptimizerAppStateChanged(appRecord, ApplicationState::APP_STATE_SUSPENDED);
    }

    abilityRecord = appRecord->AddAbility(token, abilityInfo);
    if (!abilityRecord) {
        APP_LOGE("add ability failed");
        return;
    }

    if (preToken != nullptr) {
        abilityRecord->SetPreToken(preToken);
    }

    if (appState == ApplicationState::APP_STATE_CREATE) {
        APP_LOGE("in create state, don't launch ability");
        return;
    }
    appRecord->LaunchAbility(abilityRecord);
}

std::shared_ptr<AppRunningRecord> AppMgrServiceInner::GetAppRunningRecordByAbilityToken(
    const sptr<IRemoteObject> &abilityToken) const
{
    return appRunningManager_->GetAppRunningRecordByAbilityToken(abilityToken);
}

void AppMgrServiceInner::UnsuspendApplication(const std::shared_ptr<AppRunningRecord> &appRecord)
{
    if (!appRecord) {
        APP_LOGE("app record is null");
        return;
    }

    APP_LOGD("app name is %{public}s", appRecord->GetName().c_str());
    // Resume subscription via UID
    appRecord->SetState(ApplicationState::APP_STATE_BACKGROUND);
    OptimizerAppStateChanged(appRecord, ApplicationState::APP_STATE_SUSPENDED);
}

void AppMgrServiceInner::SuspendApplication(const std::shared_ptr<AppRunningRecord> &appRecord)
{
    if (!appRecord) {
        APP_LOGE("app record is null");
        return;
    }
    APP_LOGD("app name is %{public}s", appRecord->GetName().c_str());
    // Temporary unsubscribe via UID
    appRecord->SetState(ApplicationState::APP_STATE_SUSPENDED);
    OptimizerAppStateChanged(appRecord, ApplicationState::APP_STATE_BACKGROUND);
}

void AppMgrServiceInner::LowMemoryApplicationAlert(
    const std::shared_ptr<AppRunningRecord> &appRecord, const CgroupManager::LowMemoryLevel level)
{
    if (!appRecord) {
        APP_LOGE("app record is null");
        return;
    }
}

std::shared_ptr<AppRunningRecord> AppMgrServiceInner::GetAbilityOwnerApp(
    const std::shared_ptr<AbilityRunningRecord> &abilityRecord) const
{
    if (!abilityRecord) {
        APP_LOGE("ability record is null");
        return nullptr;
    }
    if (!abilityRecord->GetToken()) {
        APP_LOGE("ability token is null");
        return nullptr;
    }
    auto appRecord = GetAppRunningRecordByAbilityToken(abilityRecord->GetToken());
    if (!appRecord) {
        APP_LOGE("The app information corresponding to token does not exist");
        return nullptr;
    }
    return appRecord;
}

std::shared_ptr<AbilityRunningRecord> AppMgrServiceInner::GetAbilityRunningRecordByAbilityToken(
    const sptr<IRemoteObject> &abilityToken) const
{
    if (!abilityToken) {
        APP_LOGE("ability token is null");
        return nullptr;
    }
    auto appRecord = GetAppRunningRecordByAbilityToken(abilityToken);
    if (!appRecord) {
        APP_LOGE("The app information corresponding to token does not exist");
        return nullptr;
    }
    auto abilityRecord = appRecord->GetAbilityRunningRecordByToken(abilityToken);
    if (!abilityRecord) {
        APP_LOGE("The ability information corresponding to token does not exist");
        return nullptr;
    }
    return abilityRecord;
}

void AppMgrServiceInner::AbilityTerminated(const sptr<IRemoteObject> &token)
{
    APP_LOGD("begin");
    if (!token) {
        APP_LOGE("token is null!");
        return;
    }
    auto appRecord = GetAppRunningRecordByAbilityToken(token);
    if (!appRecord) {
        APP_LOGE("app is not exist!");
        return;
    }
    appRecord->AbilityTerminated(token);
    APP_LOGD("end");
}

std::shared_ptr<AppRunningRecord> AppMgrServiceInner::GetAppRunningRecordByAppRecordId(const int32_t recordId) const
{
    const auto &iter = appRunningManager_->GetAppRunningRecordMap().find(recordId);
    if (iter != appRunningManager_->GetAppRunningRecordMap().end()) {
        return iter->second;
    }
    return nullptr;
}

void AppMgrServiceInner::OnAppStateChanged(
    const std::shared_ptr<AppRunningRecord> &appRecord, const ApplicationState state)
{
    APP_LOGD("begin, state:%{public}d", static_cast<int32_t>(state));
    if (!appRecord) {
        APP_LOGE("app record is null");
        return;
    }

    for (const auto &callback : appStateCallbacks_) {
        if (callback != nullptr) {
            AppProcessData processData;
            processData.appName = appRecord->GetName();
            processData.processName = appRecord->GetProcessName();
            processData.pid = appRecord->GetPriorityObject()->GetPid();
            processData.appState = state;
            callback->OnAppStateChanged(processData);
        }
    }
    APP_LOGD("end");
}

void AppMgrServiceInner::OnAbilityStateChanged(
    const std::shared_ptr<AbilityRunningRecord> &ability, const AbilityState state)
{
    APP_LOGD("begin, state:%{public}d", static_cast<int32_t>(state));
    if (!ability) {
        APP_LOGE("ability is null");
        return;
    }
    for (const auto &callback : appStateCallbacks_) {
        if (callback != nullptr) {
            callback->OnAbilityRequestDone(ability->GetToken(), state);
        }
    }
    APP_LOGD("end");
}

void AppMgrServiceInner::StartProcess(
    const std::string &appName, const std::string &processName, const std::shared_ptr<AppRunningRecord> &appRecord)
{
    if (!remoteClientManager_->GetSpawnClient() || !appRecord) {
        APP_LOGE("appSpawnClient or apprecord is null");
        return;
    }

    auto bundleMgr_ = remoteClientManager_->GetBundleManager();
    if (bundleMgr_ == nullptr) {
        APP_LOGE("GetBundleManager fail");
        return;
    }

    AppSpawnStartMsg startMsg;
    BundleInfo bundleInfo;
    bool bundleMgrResult =
        bundleMgr_->GetBundleInfo(appRecord->GetBundleName(), BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    if (!bundleMgrResult) {
        APP_LOGE("GetBundleInfo is fail");
        return;
    }
    startMsg.uid = bundleInfo.uid;
    startMsg.gid = bundleInfo.gid;

    bundleMgrResult = bundleMgr_->GetBundleGids(appRecord->GetBundleName(), startMsg.gids);
    if (!bundleMgrResult) {
        APP_LOGE("GetBundleGids is fail");
        return;
    }
    startMsg.procName = processName;
    startMsg.soPath = SO_PATH;

    PerfProfile::GetInstance().SetAppForkStartTime(GetTickCount());
    pid_t pid = 0;
    ErrCode errCode = remoteClientManager_->GetSpawnClient()->StartProcess(startMsg, pid);
    if (FAILED(errCode)) {
        APP_LOGE("failed to spawn new app process, errCode %{public}08x", errCode);
        appRunningManager_->RemoveAppRunningRecordById(appRecord->GetRecordId());
        return;
    }
    APP_LOGI("newPid %{public}d", pid);
    appRecord->GetPriorityObject()->SetPid(pid);
    APP_LOGI("app uid %{public}d", startMsg.uid);
    appRecord->SetUid(startMsg.uid);
    OptimizerAppStateChanged(appRecord, ApplicationState::APP_STATE_CREATE);
    appRecord->SetAppMgrServiceInner(weak_from_this());
    OnAppStateChanged(appRecord, ApplicationState::APP_STATE_CREATE);
    AddAppToRecentList(appName, appRecord->GetProcessName(), pid, appRecord->GetRecordId());
    PerfProfile::GetInstance().SetAppForkEndTime(GetTickCount());
}

void AppMgrServiceInner::RemoveAppFromRecentList(const std::string &appName, const std::string &processName)
{
    int64_t startTime = 0;
    std::list<pid_t> pids;
    auto appTaskInfo = appProcessManager_->GetAppTaskInfoByProcessName(appName, processName);
    if (!appTaskInfo) {
        return;
    }
    auto appRecord = GetAppRunningRecordByPid(appTaskInfo->GetPid());
    if (!appRecord) {
        appProcessManager_->RemoveAppFromRecentList(appTaskInfo);
        return;
    }
    startTime = SystemTimeMillis();
    pids.push_back(appTaskInfo->GetPid());
    appRecord->ScheduleProcessSecurityExit();
    if (!WaitForRemoteProcessExit(pids, startTime)) {
        int32_t result = KillProcessByPid(appTaskInfo->GetPid());
        if (result < 0) {
            APP_LOGE("RemoveAppFromRecentList kill process is fail");
            return;
        }
    }
    appProcessManager_->RemoveAppFromRecentList(appTaskInfo);
}

const std::list<const std::shared_ptr<AppTaskInfo>> &AppMgrServiceInner::GetRecentAppList() const
{
    return appProcessManager_->GetRecentAppList();
}

void AppMgrServiceInner::ClearRecentAppList()
{
    int64_t startTime = 0;
    std::list<pid_t> pids;
    if (GetAllPids(pids)) {
        return;
    }
    startTime = SystemTimeMillis();
    if (WaitForRemoteProcessExit(pids, startTime)) {
        appProcessManager_->ClearRecentAppList();
        return;
    }
    for (auto iter = pids.begin(); iter != pids.end(); ++iter) {
        int32_t result = KillProcessByPid(*iter);
        if (result < 0) {
            APP_LOGE("ClearRecentAppList kill process is fail");
            return;
        }
    }
    appProcessManager_->ClearRecentAppList();
}

void AppMgrServiceInner::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    auto appRecord = appRunningManager_->OnRemoteDied(remote);
    if (appRecord) {
        for (const auto &item : appRecord->GetAbilities()) {
            const auto &abilityRecord = item.second;
            OptimizerAbilityStateChanged(abilityRecord, AbilityState::ABILITY_STATE_TERMINATED);
        }
        OptimizerAppStateChanged(appRecord, ApplicationState::APP_STATE_TERMINATED);
        RemoveAppFromRecentListById(appRecord->GetRecordId());
    }
}

void AppMgrServiceInner::PushAppFront(const int32_t recordId)
{
    appProcessManager_->PushAppFront(recordId);
}

void AppMgrServiceInner::RemoveAppFromRecentListById(const int32_t recordId)
{
    appProcessManager_->RemoveAppFromRecentListById(recordId);
}

void AppMgrServiceInner::AddAppToRecentList(
    const std::string &appName, const std::string &processName, const pid_t pid, const int32_t recordId)
{
    appProcessManager_->AddAppToRecentList(appName, processName, pid, recordId);
}

const std::shared_ptr<AppTaskInfo> AppMgrServiceInner::GetAppTaskInfoById(const int32_t recordId) const
{
    return appProcessManager_->GetAppTaskInfoById(recordId);
}

void AppMgrServiceInner::AddAppDeathRecipient(const pid_t pid, const sptr<AppDeathRecipient> &appDeathRecipient) const
{
    std::shared_ptr<AppRunningRecord> appRecord = GetAppRunningRecordByPid(pid);
    if (appRecord) {
        appRecord->SetAppDeathRecipient(appDeathRecipient);
    }
}

int32_t AppMgrServiceInner::ProcessOptimizerInit()
{
    processOptimizerUBA_ = std::make_shared<ProcessOptimizerUBA>(nullptr);
    bool isSuccess = processOptimizerUBA_->Init();
    if (!isSuccess) {
        processOptimizerUBA_.reset();
        processOptimizerUBA_ = nullptr;
        APP_LOGE("optimizer init is fail");
        return ERR_NO_INIT;
    }
    processOptimizerUBA_->AppSuspended =
        std::bind(&AppMgrServiceInner::SuspendApplication, this, std::placeholders::_1);
    // Register freeze callback function
    processOptimizerUBA_->AppResumed =
        std::bind(&AppMgrServiceInner::UnsuspendApplication, this, std::placeholders::_1);
    // Register freeze recovery callback function
    processOptimizerUBA_->AppLowMemoryAlert =
        std::bind(&AppMgrServiceInner::LowMemoryApplicationAlert, this, std::placeholders::_1, std::placeholders::_2);
    // Register low memory warning callback function
    processOptimizerUBA_->GetAbilityOwnerApp =
        std::bind(&AppMgrServiceInner::GetAbilityOwnerApp, this, std::placeholders::_1);
    // Register to get the application record callback of ability
    processOptimizerUBA_->GetAbilityByToken =
        std::bind(&AppMgrServiceInner::GetAbilityRunningRecordByAbilityToken, this, std::placeholders::_1);
    // Register to get the ability record through the token callback
    APP_LOGI("optimizer init is success");
    return ERR_OK;
}

void AppMgrServiceInner::OptimizerAbilityStateChanged(
    const std::shared_ptr<AbilityRunningRecord> &ability, const AbilityState state)
{
    if (!processOptimizerUBA_) {
        APP_LOGE("process optimizer is not init");
        return;
    }

    if ((ability->GetAbilityInfo()->type == AbilityType::PAGE) ||
        (ability->GetAbilityInfo()->type == AbilityType::DATA)) {
        if (ability->GetState() == AbilityState::ABILITY_STATE_CREATE) {
            processOptimizerUBA_->OnAbilityStarted(ability);
            APP_LOGI("optimizer OnAbilityStarted is called");
        } else if (ability->GetState() == AbilityState::ABILITY_STATE_TERMINATED) {
            processOptimizerUBA_->OnAbilityRemoved(ability);
            APP_LOGI("optimizer OnAbilityRemoved is called");
        } else {
            processOptimizerUBA_->OnAbilityStateChanged(ability, state);
            APP_LOGI("optimizer OnAbilityStateChanged is called");
        }
    } else if (ability->GetAbilityInfo()->type == AbilityType::SERVICE) {
        auto appRecord = GetAppRunningRecordByAbilityToken(ability->GetPreToken());
        if (!appRecord) {
            APP_LOGE("app record is not exist for ability token");
            return;
        }
        auto targetAbility = appRecord->GetAbilityRunningRecordByToken(ability->GetPreToken());
        if (!targetAbility) {
            APP_LOGE("ability record is not exist for ability previous token");
            return;
        }
        if (ability->GetConnectionState()) {
            // connect
            processOptimizerUBA_->OnAbilityConnected(ability, targetAbility);
            APP_LOGI("optimizer OnAbilityConnected is called");
        } else {
            // disconnect
            processOptimizerUBA_->OnAbilityDisconnected(ability, targetAbility);
            APP_LOGI("optimizer OnAbilityDisconnected is called");
        }
    } else {
        APP_LOGI("OptimizerAbilityStateChanged ability type is unknown");
    }

    if (ability->GetState() != state) {
        processOptimizerUBA_->OnAbilityStateChanged(ability, state);
        APP_LOGI("optimizer OnAbilityStateChanged is called");
    }
}

void AppMgrServiceInner::OptimizerAppStateChanged(
    const std::shared_ptr<AppRunningRecord> &appRecord, const ApplicationState state)
{
    if (!processOptimizerUBA_) {
        APP_LOGE("process optimizer is not init");
        return;
    }
    if (appRecord->GetState() == ApplicationState::APP_STATE_CREATE) {
        processOptimizerUBA_->OnAppAdded(appRecord);
        APP_LOGI("optimizer OnAppAdded is called");
    } else if (appRecord->GetState() == ApplicationState::APP_STATE_TERMINATED) {
        processOptimizerUBA_->OnAppRemoved(appRecord);
        APP_LOGI("optimizer OnAppRemoved is called");
    } else {
        processOptimizerUBA_->OnAppStateChanged(appRecord, state);
        APP_LOGI("optimizer OnAppStateChanged is called");
    }
}

}  // namespace AppExecFwk
}  // namespace OHOS
