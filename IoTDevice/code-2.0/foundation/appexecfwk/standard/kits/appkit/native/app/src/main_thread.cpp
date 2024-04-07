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

#include "main_thread.h"
#include <new>
#include "ohos_application.h"
#include "app_loader.h"
#include "application_env_impl.h"
#include "ability_thread.h"
#include "task_handler_client.h"
#include "context_deal.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "resource_manager.h"
#include "sys_mgr_client.h"
#include "system_ability_definition.h"
#include "app_log_wrapper.h"

#if defined(ABILITY_LIBRARY_LOADER) || defined(APPLICATION_LIBRARY_LOADER)
#include <dirent.h>
#include <dlfcn.h>
#endif

namespace OHOS {
namespace AppExecFwk {
#define ACEABILITY_LIBRARY_LOADER
#ifdef ABILITY_LIBRARY_LOADER
#endif

/**
 *
 * @brief Notify the AppMgrDeathRecipient that the remote is dead.
 *
 * @param remote The remote whitch is dead.
 */
void AppMgrDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    APP_LOGE("MainThread::AppMgrDeathRecipient remote died receive");
}

MainThread::MainThread()
{
#ifdef ABILITY_LIBRARY_LOADER
    fileEntries_.clear();
    handleAbilityLib_.clear();
#endif  // ABILITY_LIBRARY_LOADER
}

MainThread::~MainThread()
{
#ifdef ABILITY_LIBRARY_LOADER
    CloseAbilityLibrary();
#endif  // ABILITY_LIBRARY_LOADER
}

/**
 *
 * @brief Get the current MainThreadState.
 *
 * @return Returns the current MainThreadState.
 */
MainThreadState MainThread::GetMainThreadState() const
{
    return mainThreadState_;
}

/**
 *
 * @brief Set the runner state of mainthread.
 *
 * @param runnerStart whether the runner is started.
 */
void MainThread::SetRunnerStarted(bool runnerStart)
{
    isRunnerStarted_ = runnerStart;
}

/**
 *
 * @brief Get the runner state of mainthread.
 *
 * @return Returns the runner state of mainthread.
 */
bool MainThread::GetRunnerStarted() const
{
    return isRunnerStarted_;
}

/**
 *
 * @brief Get the newThreadId.
 *
 * @return Returns the newThreadId.
 */
int MainThread::GetNewThreadId()
{
    return newThreadId_++;
}

/**
 *
 * @brief Get the application.
 *
 * @return Returns the application.
 */
std::shared_ptr<OHOSApplication> MainThread::GetApplication() const
{
    return application_;
}

/**
 *
 * @brief Get the applicationInfo.
 *
 * @return Returns the applicationInfo.
 */
std::shared_ptr<ApplicationInfo> MainThread::GetApplicationInfo() const
{
    return applicationInfo_;
}

/**
 *
 * @brief Get the applicationImpl.
 *
 * @return Returns the applicationImpl.
 */
std::shared_ptr<ApplicationImpl> MainThread::GetApplicationImpl()
{
    return applicationImpl_;
}

/**
 *
 * @brief Connect the mainthread to the AppMgr.
 *
 */
bool MainThread::ConnectToAppMgr()
{
    APP_LOGI("MainThread::connectToAppMgr start");
    auto object = OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->GetSystemAbility(APP_MGR_SERVICE_ID);
    if (object == nullptr) {
        APP_LOGE("failed to get bundle manager service");
        return false;
    }
    deathRecipient_ = new (std::nothrow) AppMgrDeathRecipient();
    if (deathRecipient_ == nullptr) {
        APP_LOGE("failed to new AppMgrDeathRecipient");
        return false;
    }

    if (!object->AddDeathRecipient(deathRecipient_)) {
        APP_LOGE("failed to AddDeathRecipient");
        return false;
    }

    appMgr_ = iface_cast<IAppMgr>(object);
    if (appMgr_ == nullptr) {
        APP_LOGE("failed to iface_cast object to appMgr_");
        return false;
    }
    appMgr_->AttachApplication(this);
    APP_LOGI("MainThread::connectToAppMgr end");
    return true;
}

/**
 *
 * @brief Attach the mainthread to the AppMgr.
 *
 */
void MainThread::Attach()
{
    APP_LOGI("MainThread::attach called");
    if (!ConnectToAppMgr()) {
        APP_LOGE("attachApplication failed");
        return;
    }
    mainThreadState_ = MainThreadState::ATTACH;
    APP_LOGI("MainThread::attach mainThreadState: %{public}d", mainThreadState_);
}

/**
 *
 * @brief remove the deathRecipient from appMgr.
 *
 */
void MainThread::RemoveAppMgrDeathRecipient()
{
    APP_LOGD("MainThread::RemoveAppMgrDeathRecipient called");
    if (appMgr_ == nullptr) {
        APP_LOGE("MainThread::RemoveAppMgrDeathRecipient failed");
        return;
    }

    sptr<IRemoteObject> object = appMgr_->AsObject();
    if (object != nullptr) {
        object->RemoveDeathRecipient(deathRecipient_);
    } else {
        APP_LOGE("appMgr_->AsObject() failed");
    }
}

/**
 *
 * @brief Get the eventHandler of mainthread.
 *
 * @return Returns the eventHandler of mainthread.
 */
std::shared_ptr<EventHandler> MainThread::GetMainHandler() const
{
    return mainHandler_;
}

/**
 *
 * @brief Schedule the foreground lifecycle of application.
 *
 */
void MainThread::ScheduleForegroundApplication()
{
    APP_LOGI("MainThread::scheduleForegroundApplication called");
    auto task = [appThread = this]() { appThread->HandleForegroundApplication(); };
    if (!mainHandler_->PostTask(task)) {
        APP_LOGE("PostTask task failed");
    }
}

/**
 *
 * @brief Schedule the background lifecycle of application.
 *
 */
void MainThread::ScheduleBackgroundApplication()
{
    APP_LOGI("MainThread::scheduleBackgroundApplication called");

    auto task = [appThread = this]() { appThread->HandleBackgroundApplication(); };
    if (!mainHandler_->PostTask(task)) {
        APP_LOGE("MainThread::ScheduleBackgroundApplication PostTask task failed");
    }
}

/**
 *
 * @brief Schedule the terminate lifecycle of application.
 *
 */
void MainThread::ScheduleTerminateApplication()
{
    APP_LOGI("MainThread::scheduleTerminateApplication called");

    auto task = [appThread = this]() { appThread->HandleTerminateApplication(); };
    if (!mainHandler_->PostTask(task)) {
        APP_LOGE("MainThread::ScheduleTerminateApplication PostTask task failed");
    }
}

/**
 *
 * @brief Shrink the memory whitch used by application.
 *
 * @param level Indicates the memory trim level, which shows the current memory usage status.
 */
void MainThread::ScheduleShrinkMemory(const int level)
{
    APP_LOGI("MainThread::scheduleShrinkMemory level: %{public}d", level);

    auto task = [appThread = this, level]() { appThread->HandleShrinkMemory(level); };
    if (!mainHandler_->PostTask(task)) {
        APP_LOGE("MainThread::ScheduleShrinkMemory PostTask task failed");
    }
}

/**
 *
 * @brief Schedule the application process exit safely.
 *
 */
void MainThread::ScheduleProcessSecurityExit()
{
    APP_LOGI("MainThread::ScheduleProcessSecurityExit called");

    auto task = [appThread = this]() { appThread->HandleProcessSecurityExit(); };
    if (!mainHandler_->PostTask(task)) {
        APP_LOGE("MainThread::ScheduleProcessSecurityExit PostTask task failed");
    }
}

/**
 *
 * @brief Low the memory whitch used by application.
 *
 */
void MainThread::ScheduleLowMemory()
{
    APP_LOGI("MainThread::scheduleLowMemory called");
}

/**
 *
 * @brief Launch the application.
 *
 * @param data The launchdata of the application witch launced.
 *
 */
void MainThread::ScheduleLaunchApplication(const AppLaunchData &data)
{
    APP_LOGI("MainThread::scheduleLaunchApplication called");

    auto task = [appThread = this, data]() { appThread->HandleLaunchApplication(data); };
    if (!mainHandler_->PostTask(task)) {
        APP_LOGE("MainThread::ScheduleLaunchApplication PostTask task failed");
    }
}

/**
 *
 * @brief launch the application.
 *
 * @param info The launchdata of the application witch launced.
 * @param token The launchdata of the application witch launced.
 *
 */
void MainThread::ScheduleLaunchAbility(const AbilityInfo &info, const sptr<IRemoteObject> &token)
{
    APP_LOGI("MainThread::scheduleLaunchAbility called");
    APP_LOGI(
        "MainThread::scheduleLaunchAbility AbilityInfo name:%{public}s type:%{public}d", info.name.c_str(), info.type);

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>(info);
    if (abilityInfo == nullptr) {
        APP_LOGE("MainThread::ScheduleLaunchAbility abilityInfo is nullptr");
        return;
    }
    sptr<IRemoteObject> abilityToken = token;
    std::shared_ptr<AbilityLocalRecord> abilityRecord = std::make_shared<AbilityLocalRecord>(abilityInfo, abilityToken);

    auto task = [appThread = this, abilityRecord]() { appThread->HandleLaunchAbility(abilityRecord); };
    if (!mainHandler_->PostTask(task)) {
        APP_LOGE("MainThread::ScheduleLaunchAbility PostTask task failed");
    }
}

/**
 *
 * @brief clean the ability by token.
 *
 * @param token The token belong to the ability whitch want to be cleaned.
 *
 */
void MainThread::ScheduleCleanAbility(const sptr<IRemoteObject> &token)
{
    APP_LOGI("MainThread::scheduleCleanAbility called");
    auto task = [appThread = this, token]() { appThread->HandleCleanAbility(token); };
    if (!mainHandler_->PostTask(task)) {
        APP_LOGE("MainThread::ScheduleCleanAbility PostTask task failed");
    }
}

/**
 *
 * @brief send the new profile.
 *
 * @param profile The updated profile.
 *
 */
void MainThread::ScheduleProfileChanged(const Profile &profile)
{
    APP_LOGI("MainThread::scheduleProfileChanged profile name: %{public}s", profile.GetName().c_str());
}

/**
 *
 * @brief send the new config to the application.
 *
 * @param config The updated config.
 *
 */
void MainThread::ScheduleConfigurationUpdated(const Configuration &config)
{
    APP_LOGI("MainThread::ScheduleConfigurationUpdated called");
    auto task = [appThread = this, config]() { appThread->HandleConfigurationUpdated(config); };
    if (!mainHandler_->PostTask(task)) {
        APP_LOGE("MainThread::ScheduleConfigurationUpdated PostTask task failed");
    }
}

/**
 *
 * @brief Check whether the appLaunchData is legal.
 *
 * @param appLaunchData The appLaunchData should be checked.
 *
 * @return if the appLaunchData is legal, return true. else return false.
 */
bool MainThread::CheckLaunchApplicationParam(const AppLaunchData &appLaunchData) const
{
    ApplicationInfo appInfo = appLaunchData.GetApplicationInfo();
    ProcessInfo processInfo = appLaunchData.GetProcessInfo();

    if (appInfo.name.empty()) {
        APP_LOGE("MainThread::checkLaunchApplicationParam applicationName is empty");
        return false;
    }

    if (processInfo.GetProcessName().empty()) {
        APP_LOGE("MainThread::checkLaunchApplicationParam processName is empty");
        return false;
    }

    return true;
}

/**
 *
 * @brief Check whether the record is legal.
 *
 * @param record The record should be checked.
 *
 * @return if the record is legal, return true. else return false.
 */
bool MainThread::CheckAbilityItem(const std::shared_ptr<AbilityLocalRecord> &record) const
{
    if (record == nullptr) {
        APP_LOGE("MainThread::checkAbilityItem record is null");
        return false;
    }

    std::shared_ptr<AbilityInfo> abilityInfo = record->GetAbilityInfo();
    sptr<IRemoteObject> token = record->GetToken();

    if (abilityInfo == nullptr) {
        APP_LOGE("MainThread::checkAbilityItem abilityInfo is null");
        return false;
    }

    if (token == nullptr) {
        APP_LOGE("MainThread::checkAbilityItem token is null");
        return false;
    }

    return true;
}

/**
 *
 * @brief Terminate the application but don't notify ams.
 *
 */
void MainThread::HandleTerminateApplicationLocal()
{
    APP_LOGI("MainThread::HandleTerminateApplicationLocal called...");
    if (application_ == nullptr) {
        APP_LOGE("MainThread::HandleTerminateApplicationLocal error!");
        return;
    }
    applicationImpl_->PerformTerminateStrong();
    std::shared_ptr<EventRunner> runner = mainHandler_->GetEventRunner();
    if (runner == nullptr) {
        APP_LOGE("MainThread::HandleTerminateApplicationLocal get manHandler error");
        return;
    }
    int ret = runner->Stop();
    if (ret != ERR_OK) {
        APP_LOGE("MainThread::HandleTerminateApplicationLocal failed. runner->Run failed ret = %{public}d", ret);
    }
    APP_LOGI("runner is stopped");
    SetRunnerStarted(false);

#ifdef ABILITY_LIBRARY_LOADER
    CloseAbilityLibrary();
#endif  // ABILITY_LIBRARY_LOADER
#ifdef APPLICATION_LIBRARY_LOADER
    if (handleAppLib_ != nullptr) {
        dlclose(handleAppLib_);
        handleAppLib_ = nullptr;
    }
#endif  // APPLICATION_LIBRARY_LOADER
}

/**
 *
 * @brief Schedule the application process exit safely.
 *
 */
void MainThread::HandleProcessSecurityExit()
{
    APP_LOGI("MainThread::HandleProcessSecurityExit called");
    if (abilityRecordMgr_ == nullptr) {
        APP_LOGE("MainThread::HandleProcessSecurityExit abilityRecordMgr_ is null");
        return;
    }

    std::vector<sptr<IRemoteObject>> tokens = (abilityRecordMgr_->GetAllTokens());

    for (auto iter = tokens.begin(); iter != tokens.end(); ++iter) {
        HandleCleanAbilityLocal(*iter);
    }

    HandleTerminateApplicationLocal();
}

/**
 *
 * @brief Launch the application.
 *
 * @param appLaunchData The launchdata of the application witch launced.
 *
 */
void MainThread::HandleLaunchApplication(const AppLaunchData &appLaunchData)
{
    APP_LOGI("MainThread::handleLaunchApplication called");
    if (application_ != nullptr) {
        APP_LOGE("MainThread::handleLaunchApplication already create application");
        return;
    }

    if (!CheckLaunchApplicationParam(appLaunchData)) {
        APP_LOGE("MainThread::handleLaunchApplication appLaunchData invalid");
        return;
    }

#ifdef ABILITY_LIBRARY_LOADER
    LoadAbilityLibrary(appLaunchData.GetApplicationInfo().moduleSourceDirs);
#endif  // ABILITY_LIBRARY_LOADER
#ifdef APPLICATION_LIBRARY_LOADER
    std::string appPath = applicationLibraryPath;
    handleAppLib_ = dlopen(appPath.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (handleAppLib_ == nullptr) {
        APP_LOGE("Fail to dlopen %{public}s, [%{public}s]", appPath.c_str(), dlerror());
        exit(-1);
    }
#endif  // APPLICATION_LIBRARY_LOADER

    ApplicationInfo appInfo = appLaunchData.GetApplicationInfo();
    ProcessInfo processInfo = appLaunchData.GetProcessInfo();
    Profile appProfile = appLaunchData.GetProfile();

    applicationInfo_ = std::make_shared<ApplicationInfo>(appInfo);
    if (applicationInfo_ == nullptr) {
        APP_LOGE("MainThread::handleLaunchApplication create applicationInfo_ failed");
        return;
    }

    processInfo_ = std::make_shared<ProcessInfo>(processInfo);
    if (processInfo_ == nullptr) {
        APP_LOGE("MainThread::handleLaunchApplication create processInfo_ failed");
        return;
    }

    appProfile_ = std::make_shared<Profile>(appProfile);
    if (appProfile_ == nullptr) {
        APP_LOGE("MainThread::handleLaunchApplication create appProfile_ failed");
        return;
    }

    applicationImpl_ = std::make_shared<ApplicationImpl>();
    if (applicationImpl_ == nullptr) {
        APP_LOGE("MainThread::handleLaunchApplication create applicationImpl_ failed");
        return;
    }

    std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
    if (contextDeal == nullptr) {
        APP_LOGE("MainThread::handleLaunchApplication create contextDeal failed");
        return;
    }

    contextDeal->SetProcessInfo(processInfo_);
    contextDeal->SetApplicationInfo(applicationInfo_);
    contextDeal->SetProfile(appProfile_);
    contextDeal->SetBundleCodePath(applicationInfo_->codePath);  // BMS need to add cpath

    // BMS should set the type (native or ace) of application
    bool isNativeApp = true;
    std::string appName = isNativeApp ? appInfo.name : aceApplicationName_;
    application_ = std::shared_ptr<OHOSApplication>(ApplicationLoader::GetInstance().GetApplicationByName(appName));
    if (application_ == nullptr) {
        APP_LOGE("HandleLaunchApplication::application launch failed");
        return;
    }

    // init resourceManager.
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager(Global::Resource::CreateResourceManager());
    if (resourceManager == nullptr) {
        APP_LOGE("MainThread::handleLaunchApplication create resourceManager failed");
        return;
    }

    sptr<IBundleMgr> bundleMgr = contextDeal->GetBundleManager();
    if (bundleMgr == nullptr) {
        APP_LOGE("MainThread::handleLaunchApplication GetBundleManager is nullptr");
        return;
    }

    BundleInfo bundleInfo;
    APP_LOGI("MainThread::handleLaunchApplication length: %{public}d, bundleName: %{public}s",
        appInfo.bundleName.length(),
        appInfo.bundleName.c_str());
    bundleMgr->GetBundleInfo(appInfo.bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);

    APP_LOGI("MainThread::handleLaunchApplication moduleResPaths count: %{public}d", bundleInfo.moduleResPaths.size());
    for (auto moduleResPath : bundleInfo.moduleResPaths) {
        if (!moduleResPath.empty()) {
            APP_LOGI("MainThread::handleLaunchApplication length: %{public}d, moduleResPath: %{public}s",
                moduleResPath.length(),
                moduleResPath.c_str());
            if (!resourceManager->AddResource(moduleResPath.c_str())) {
                APP_LOGE("MainThread::handleLaunchApplication AddResource failed");
            }
        }
    }

    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    resConfig->SetLocaleInfo("zh", "Hans", "CN");
    const Global::Resource::LocaleInfo *localeInfo = resConfig->GetLocaleInfo();
    if (localeInfo != nullptr) {
        APP_LOGI("MainThread::handleLaunchApplication language: %{public}s, script: %{public}s, region: %{public}s,",
            localeInfo->GetLanguage(),
            localeInfo->GetScript(),
            localeInfo->GetRegion());
    } else {
        APP_LOGI("MainThread::handleLaunchApplication localeInfo is nullptr.");
    }

    resourceManager->UpdateResConfig(*resConfig);
    contextDeal->initResourceManager(resourceManager);
    contextDeal->SetApplicationContext(application_);
    application_->AttachBaseContext(contextDeal);

    abilityRecordMgr_ = std::make_shared<AbilityRecordMgr>();
    if (abilityRecordMgr_ == nullptr) {
        APP_LOGE("HandleLaunchApplication::application create AbilityRecordMgr failed");
        return;
    }

    application_->SetAbilityRecordMgr(abilityRecordMgr_);

    applicationImpl_->SetRecordId(appLaunchData.GetRecordId());
    applicationImpl_->SetApplication(application_);
    mainThreadState_ = MainThreadState::READY;
    if (!applicationImpl_->PerformAppReady()) {
        APP_LOGE("HandleLaunchApplication::application applicationImpl_->PerformAppReady failed");
        return;
    }

    // L1 needs to add corresponding interface
    ApplicationEnvImpl *pAppEvnIml = ApplicationEnvImpl::GetInstance();

    if (pAppEvnIml) {
        pAppEvnIml->SetAppInfo(*applicationInfo_.get());
    }
}

/**
 *
 * @brief launch the ability.
 *
 * @param abilityRecord The abilityRecord whitch belongs to the ability launched.
 *
 */
void MainThread::HandleLaunchAbility(const std::shared_ptr<AbilityLocalRecord> &abilityRecord)
{
    APP_LOGI("MainThread::handleLaunchAbility called");

    if (applicationImpl_ == nullptr) {
        APP_LOGE("MainThread::ScheduleLaunchAbility applicationImpl_ is null");
        return;
    }

    if (abilityRecordMgr_ == nullptr) {
        APP_LOGE("MainThread::ScheduleLaunchAbility abilityRecordMgr_ is null");
        return;
    }

    if (abilityRecord == nullptr) {
        APP_LOGE("MainThread::ScheduleLaunchAbility parameter(abilityRecord) is null");
        return;
    }

    auto abilityToken = abilityRecord->GetToken();
    if (abilityToken == nullptr) {
        APP_LOGE("MainThread::ScheduleLaunchAbility failed. abilityRecord->GetToken failed");
        return;
    }

    abilityRecordMgr_->SetToken(abilityToken);
    abilityRecordMgr_->AddAbilityRecord(abilityToken, abilityRecord);

    if (!IsApplicationReady()) {
        APP_LOGE("MainThread::handleLaunchAbility not init OHOSApplication, should launch application first");
        return;
    }

    if (!CheckAbilityItem(abilityRecord)) {
        APP_LOGE("MainThread::handleLaunchAbility record is invalid");
        return;
    }

    mainThreadState_ = MainThreadState::RUNNING;
#ifdef APP_ABILITY_USE_TWO_RUNNER
    AbilityThread::AbilityThreadMain(application_, abilityRecord);
#else
    AbilityThread::AbilityThreadMain(application_, abilityRecord, mainHandler_->GetEventRunner());
#endif
}

/**
 *
 * @brief Clean the ability but don't notify ams.
 *
 * @param token The token whitch belongs to the ability launched.
 *
 */
void MainThread::HandleCleanAbilityLocal(const sptr<IRemoteObject> &token)
{
    APP_LOGI("MainThread::HandleCleanAbilityLocal called");
    if (!IsApplicationReady()) {
        APP_LOGE("MainThread::HandleCleanAbilityLocal not init OHOSApplication, should launch application first");
        return;
    }

    if (token == nullptr) {
        APP_LOGE("MainThread::HandleCleanAbilityLocal token is null");
        return;
    }

    std::shared_ptr<AbilityLocalRecord> record = abilityRecordMgr_->GetAbilityItem(token);
    if (record == nullptr) {
        APP_LOGE("MainThread::HandleCleanAbilityLocal abilityRecord not found");
        return;
    }
    std::shared_ptr<AbilityInfo> abilityInfo = record->GetAbilityInfo();
    if (abilityInfo == nullptr) {
        APP_LOGE("MainThread::HandleCleanAbilityLocal record->GetAbilityInfo() failed");
        return;
    }
    APP_LOGI("MainThread::HandleCleanAbilityLocal ability name: %{public}s", abilityInfo->name.c_str());

    abilityRecordMgr_->RemoveAbilityRecord(token);

#ifdef APP_ABILITY_USE_TWO_RUNNER
    std::shared_ptr<EventRunner> runner = record->GetEventRunner();

    if (runner != nullptr) {
        int ret = runner->Stop();
        if (ret != ERR_OK) {
            APP_LOGE("MainThread::main failed. ability runner->Run failed ret = %{public}d", ret);
        }
        abilityRecordMgr_->RemoveAbilityRecord(token);
    } else {
        APP_LOGW("MainThread::HandleCleanAbilityLocal runner not found");
    }
#endif
}

/**
 *
 * @brief Clean the ability.
 *
 * @param token The token whitch belongs to the ability launched.
 *
 */
void MainThread::HandleCleanAbility(const sptr<IRemoteObject> &token)
{
    APP_LOGI("MainThread::handleCleanAbility called");
    if (!IsApplicationReady()) {
        APP_LOGE("MainThread::handleCleanAbility not init OHOSApplication, should launch application first");
        return;
    }

    if (token == nullptr) {
        APP_LOGE("MainThread::handleCleanAbility token is null");
        return;
    }

    std::shared_ptr<AbilityLocalRecord> record = abilityRecordMgr_->GetAbilityItem(token);
    if (record == nullptr) {
        APP_LOGE("MainThread::handleCleanAbility abilityRecord not found");
        return;
    }
    std::shared_ptr<AbilityInfo> abilityInfo = record->GetAbilityInfo();
    if (abilityInfo == nullptr) {
        APP_LOGE("MainThread::handleCleanAbility record->GetAbilityInfo() failed");
        return;
    }
    APP_LOGI("MainThread::handleCleanAbility ability name: %{public}s", abilityInfo->name.c_str());

    abilityRecordMgr_->RemoveAbilityRecord(token);

#ifdef APP_ABILITY_USE_TWO_RUNNER
    std::shared_ptr<EventRunner> runner = record->GetEventRunner();

    if (runner != nullptr) {
        int ret = runner->Stop();
        if (ret != ERR_OK) {
            APP_LOGE("MainThread::main failed. ability runner->Run failed ret = %{public}d", ret);
        }
        abilityRecordMgr_->RemoveAbilityRecord(token);
    } else {
        APP_LOGW("MainThread::handleCleanAbility runner not found");
    }
#endif
    appMgr_->AbilityCleaned(token);
}

/**
 *
 * @brief Foreground the application.
 *
 */
void MainThread::HandleForegroundApplication()
{
    APP_LOGI("MainThread::handleForegroundApplication called...");
    if ((application_ == nullptr) || (appMgr_ == nullptr)) {
        APP_LOGE("MainThread::handleForegroundApplication error!");
        return;
    }

    if (!applicationImpl_->PerformForeground()) {
        APP_LOGE("MainThread::handleForegroundApplication error!, applicationImpl_->PerformForeground() failed");
        return;
    }

    appMgr_->ApplicationForegrounded(applicationImpl_->GetRecordId());
}

/**
 *
 * @brief Background the application.
 *
 */
void MainThread::HandleBackgroundApplication()
{
    APP_LOGI("MainThread::handleBackgroundApplication called...");

    if ((application_ == nullptr) || (appMgr_ == nullptr)) {
        APP_LOGE("MainThread::handleBackgroundApplication error!");
        return;
    }

    if (!applicationImpl_->PerformBackground()) {
        APP_LOGE("MainThread::handleForegroundApplication error!, applicationImpl_->PerformBackground() failed");
        return;
    }

    appMgr_->ApplicationBackgrounded(applicationImpl_->GetRecordId());
}

/**
 *
 * @brief Terminate the application.
 *
 */
void MainThread::HandleTerminateApplication()
{
    APP_LOGI("MainThread::handleTerminateApplication called...");
    if ((application_ == nullptr) || (appMgr_ == nullptr)) {
        APP_LOGE("MainThread::handleTerminateApplication error!");
        return;
    }

    if (!applicationImpl_->PerformTerminate()) {
        APP_LOGE("MainThread::handleForegroundApplication error!, applicationImpl_->PerformTerminate() failed");
        return;
    }

    appMgr_->ApplicationTerminated(applicationImpl_->GetRecordId());
    std::shared_ptr<EventRunner> runner = mainHandler_->GetEventRunner();
    if (runner == nullptr) {
        APP_LOGE("MainThread::handleTerminateApplication get manHandler error");
        return;
    }
    int ret = runner->Stop();
    if (ret != ERR_OK) {
        APP_LOGE("MainThread::handleTerminateApplication failed. runner->Run failed ret = %{public}d", ret);
    }
    SetRunnerStarted(false);

#ifdef ABILITY_LIBRARY_LOADER
    CloseAbilityLibrary();
#endif  // ABILITY_LIBRARY_LOADER
#ifdef APPLICATION_LIBRARY_LOADER
    if (handleAppLib_ != nullptr) {
        dlclose(handleAppLib_);
        handleAppLib_ = nullptr;
    }
#endif  // APPLICATION_LIBRARY_LOADER
}

/**
 *
 * @brief Shrink the memory whitch used by application.
 *
 * @param level Indicates the memory trim level, which shows the current memory usage status.
 *
 */
void MainThread::HandleShrinkMemory(const int level)
{
    APP_LOGI("MainThread::HandleShrinkMemory called...");

    if (applicationImpl_ == nullptr) {
        APP_LOGE("MainThread::HandleShrinkMemory error! applicationImpl_ is null");
        return;
    }

    applicationImpl_->PerformMemoryLevel(level);
}

/**
 *
 * @brief send the new config to the application.
 *
 * @param config The updated config.
 *
 */
void MainThread::HandleConfigurationUpdated(const Configuration &config)
{
    APP_LOGI("MainThread::HandleConfigurationUpdated called...");

    if (applicationImpl_ == nullptr) {
        APP_LOGE("MainThread::HandleConfigurationUpdated error! applicationImpl_ is null");
        return;
    }

    applicationImpl_->PerformConfigurationUpdated(config);
}

void MainThread::Init(const std::shared_ptr<EventRunner> &runner)
{
    mainHandler_ = std::make_shared<MainHandler>(runner, this);
    auto task = [appThread = this]() {
        APP_LOGI("MainThread:MainHandler Start");
        appThread->SetRunnerStarted(true);
    };
    if (!mainHandler_->PostTask(task)) {
        APP_LOGE("MainThread::Init PostTask task failed");
    }

    TaskHandlerClient::GetInstance()->CreateRunner();
}

void MainThread::Start()
{
    APP_LOGI("MainThread::main called start");
    std::shared_ptr<EventRunner> runner = EventRunner::GetMainEventRunner();
    if (runner == nullptr) {
        APP_LOGE("MainThread::main called start");
        return;
    }
    sptr<MainThread> thread = sptr<MainThread>(new (std::nothrow) MainThread());
    if (thread == nullptr) {
        APP_LOGE("MainThread::static failed. new MainThread failed");
        return;
    }

    thread->Init(runner);
    thread->Attach();

    int ret = runner->Run();
    if (ret != ERR_OK) {
        APP_LOGE("MainThread::main failed. runner->Run failed ret = %{public}d", ret);
    }

    thread->RemoveAppMgrDeathRecipient();
    APP_LOGW("MainThread::main runner stopped");
}

MainThread::MainHandler::MainHandler(const std::shared_ptr<EventRunner> &runner, const sptr<MainThread> &thread)
    : AppExecFwk::EventHandler(runner), mainThreadObj_(thread)
{}

/**
 *
 * @brief Process the event.
 *
 * @param event the event want to be processed.
 *
 */
void MainThread::MainHandler::ProcessEvent(const OHOS::AppExecFwk::InnerEvent::Pointer &event)
{}

/**
 *
 * @brief Check whether the OHOSApplication is ready.
 *
 * @return if the record is legal, return true. else return false.
 *
 */
bool MainThread::IsApplicationReady() const
{
    if (application_ == nullptr || applicationImpl_ == nullptr) {
        return false;
    }

    return true;
}

#ifdef ABILITY_LIBRARY_LOADER
/**
 *
 * @brief Load the ability library.
 *
 * @param libraryPaths the library paths.
 *
 */
void MainThread::LoadAbilityLibrary(const std::vector<std::string> &libraryPaths)
{
#ifdef ACEABILITY_LIBRARY_LOADER
    std::string acelibdir("/system/lib/libace.z.so");
    void *AceAbilityLib = nullptr;
    AceAbilityLib = dlopen(acelibdir.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (AceAbilityLib == nullptr) {
        APP_LOGE("Fail to dlopen %{public}s, [%{public}s]", acelibdir.c_str(), dlerror());
    } else {
        APP_LOGI("Success to dlopen %{public}s", acelibdir.c_str());
        handleAbilityLib_.emplace_back(AceAbilityLib);
    }
#endif  // ACEABILITY_LIBRARY_LOADER
    int size = libraryPaths.size();
    for (int index = 0; index < size; index++) {
        std::string libraryPath = libraryPaths[index];
        if (!ScanDir(libraryPath)) {
            APP_LOGE("Fail to scanDir %{public}s", libraryPath.c_str());
            continue;
        }
    }

    if (fileEntries_.empty()) {
        APP_LOGE("No ability library");
        return;
    }

    void *handleAbilityLib = nullptr;
    for (auto fileEntry : fileEntries_) {
        if (!fileEntry.empty()) {
            handleAbilityLib = dlopen(fileEntry.c_str(), RTLD_NOW | RTLD_GLOBAL);
            if (handleAbilityLib == nullptr) {
                APP_LOGE("Fail to dlopen %{public}s, [%{public}s]", fileEntry.c_str(), dlerror());
                exit(-1);
            } else {
                APP_LOGI("Success to dlopen %{public}s", fileEntry.c_str());
            }
            handleAbilityLib_.emplace_back(handleAbilityLib);
        }
    }
}

/**
 *
 * @brief Close the ability library loaded.
 *
 */
void MainThread::CloseAbilityLibrary()
{
    for (auto iter : handleAbilityLib_) {
        if (iter != nullptr) {
            dlclose(iter);
            iter = nullptr;
        }
    }
    handleAbilityLib_.clear();
    fileEntries_.clear();
}

/**
 *
 * @brief Scan the dir ability library loaded.
 *
 * @param dirPath the the path should be scan.
 *
 */
bool MainThread::ScanDir(const std::string &dirPath)
{
    DIR *dirp = opendir(dirPath.c_str());
    if (dirp == nullptr) {
        APP_LOGE("MainThread::ScanDir open dir:%{private}s fail", dirPath.c_str());
        return false;
    }

    struct dirent *df = nullptr;
    for (;;) {
        df = readdir(dirp);
        if (df == nullptr) {
            break;
        }

        std::string currentName(df->d_name);
        APP_LOGD("folder found:'%{private}s'", df->d_name);
        if (currentName.compare(".") == 0 || currentName.compare("..") == 0) {
            continue;
        }

        if (CheckFileType(currentName, abilityLibraryType_)) {
            fileEntries_.emplace_back(dirPath + pathSeparator_ + currentName);
        }
    }

    if (closedir(dirp) == -1) {
        APP_LOGW("close dir fail");
    }
    return true;
}

/**
 *
 * @brief Check the fileType.
 *
 * @param fileName The fileName of the lib.
 * @param extensionName The extensionName of the lib.
 *
 * @return if the FileType is legal, return true. else return false.
 *
 */
bool MainThread::CheckFileType(const std::string &fileName, const std::string &extensionName)
{
    APP_LOGD("path is %{public}s, support suffix is %{public}s", fileName.c_str(), extensionName.c_str());
    if (fileName.empty()) {
        APP_LOGE("the file name is empty");
        return false;
    }

    auto position = fileName.rfind('.');
    if (position == std::string::npos) {
        APP_LOGE("filename no extension name");
        return false;
    }

    std::string suffixStr = fileName.substr(position);
    return LowerStr(suffixStr) == extensionName;
}
#endif  // ABILITY_LIBRARY_LOADER
}  // namespace AppExecFwk
}  // namespace OHOS
