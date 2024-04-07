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

#ifndef FOUNDATION_APPEXECFWK_MAIN_THREAD_H
#define FOUNDATION_APPEXECFWK_MAIN_THREAD_H

#include <string>
#include <mutex>
#include "event_handler.h"
#include "inner_event.h"
#include "app_scheduler_host.h"
#include "app_mgr_interface.h"
#include "ability_record_mgr.h"
#include "application_impl.h"
#include "ohos/aafwk/base/ipc_singleton.h"
#define ABILITY_LIBRARY_LOADER

namespace OHOS {
namespace AppExecFwk {
enum class MainThreadState { INIT, ATTACH, READY, RUNNING };

class AppMgrDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    /**
     *
     * @brief Notify the AppMgrDeathRecipient that the remote is dead.
     *
     * @param remote The remote which is dead.
     */
    virtual void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    AppMgrDeathRecipient() = default;
    ~AppMgrDeathRecipient() override = default;
};

class MainThread : public AppSchedulerHost {
    DECLARE_DELAYED_IPCSINGLETON(MainThread);

public:
    /**
     *
     * @brief Get the current MainThreadState.
     *
     * @return Returns the current MainThreadState.
     */
    MainThreadState GetMainThreadState() const;

    /**
     *
     * @brief Get the runner state of mainthread.
     *
     * @return Returns the runner state of mainthread.
     */
    bool GetRunnerStarted() const;

    /**
     *
     * @brief Get the newThreadId.
     *
     * @return Returns the newThreadId.
     */
    int GetNewThreadId();

    /**
     *
     * @brief Get the application.
     *
     * @return Returns the application.
     */
    std::shared_ptr<OHOSApplication> GetApplication() const;

    /**
     *
     * @brief Get the applicationInfo.
     *
     * @return Returns the applicationInfo.
     */
    std::shared_ptr<ApplicationInfo> GetApplicationInfo() const;

    /**
     *
     * @brief Get the applicationImpl.
     *
     * @return Returns the applicationImpl.
     */
    std::shared_ptr<ApplicationImpl> GetApplicationImpl();

    /**
     *
     * @brief Get the eventHandler of mainthread.
     *
     * @return Returns the eventHandler of mainthread.
     */
    std::shared_ptr<EventHandler> GetMainHandler() const;

    /**
     *
     * @brief Schedule the foreground lifecycle of application.
     *
     */
    void ScheduleForegroundApplication() override;

    /**
     *
     * @brief Schedule the background lifecycle of application.
     *
     */
    void ScheduleBackgroundApplication() override;

    /**
     *
     * @brief Schedule the terminate lifecycle of application.
     *
     */
    void ScheduleTerminateApplication() override;

    /**
     *
     * @brief Shrink the memory which used by application.
     *
     * @param level Indicates the memory trim level, which shows the current memory usage status.
     */
    void ScheduleShrinkMemory(const int level) override;

    /**
     *
     * @brief Low the memory which used by application.
     *
     */
    void ScheduleLowMemory() override;

    /**
     *
     * @brief Launch the application.
     *
     * @param data The launchdata of the application witch launced.
     *
     */
    void ScheduleLaunchApplication(const AppLaunchData &data) override;

    /**
     *
     * @brief launch the application.
     *
     * @param info The launchdata of the application witch launced.
     * @param token The launchdata of the application witch launced.
     *
     */
    void ScheduleLaunchAbility(const AbilityInfo &info, const sptr<IRemoteObject> &token) override;

    /**
     *
     * @brief clean the ability by token.
     *
     * @param token The token belong to the ability which want to be cleaned.
     *
     */
    void ScheduleCleanAbility(const sptr<IRemoteObject> &token) override;

    /**
     *
     * @brief send the new profile.
     *
     * @param profile The updated profile.
     *
     */
    void ScheduleProfileChanged(const Profile &profile) override;

    /**
     *
     * @brief send the new config to the application.
     *
     * @param config The updated config.
     *
     */
    void ScheduleConfigurationUpdated(const Configuration &config) override;

    /**
     *
     * @brief Starts the mainthread.
     *
     */
    static void Start();

    /**
     *
     * @brief Schedule the application process exit safely.
     *
     */
    void ScheduleProcessSecurityExit() override;

private:
    /**
     *
     * @brief Terminate the application but don't notify ams.
     *
     */
    void HandleTerminateApplicationLocal();

    /**
     *
     * @brief Schedule the application process exit safely.
     *
     */
    void HandleProcessSecurityExit();

    /**
     *
     * @brief Clean the ability but don't notify ams.
     *
     * @param token The token which belongs to the ability launched.
     *
     */
    void HandleCleanAbilityLocal(const sptr<IRemoteObject> &token);

    /**
     *
     * @brief Launch the application.
     *
     * @param appLaunchData The launchdata of the application witch launced.
     *
     */
    void HandleLaunchApplication(const AppLaunchData &appLaunchData);

    /**
     *
     * @brief Launch the ability.
     *
     * @param abilityRecord The abilityRecord which belongs to the ability launched.
     *
     */
    void HandleLaunchAbility(const std::shared_ptr<AbilityLocalRecord> &abilityRecord);

    /**
     *
     * @brief Clean the ability.
     *
     * @param token The token which belongs to the ability launched.
     *
     */
    void HandleCleanAbility(const sptr<IRemoteObject> &token);

    /**
     *
     * @brief Foreground the application.
     *
     */
    void HandleForegroundApplication();

    /**
     *
     * @brief Background the application.
     *
     */
    void HandleBackgroundApplication();

    /**
     *
     * @brief Terminate the application.
     *
     */
    void HandleTerminateApplication();

    /**
     *
     * @brief Shrink the memory which used by application.
     *
     * @param level Indicates the memory trim level, which shows the current memory usage status.
     *
     */
    void HandleShrinkMemory(const int level);

    /**
     *
     * @brief send the new config to the application.
     *
     * @param config The updated config.
     *
     */
    void HandleConfigurationUpdated(const Configuration &config);

    /**
     *
     * @brief remove the deathRecipient from appMgr.
     *
     */
    void RemoveAppMgrDeathRecipient();

    /**
     *
     * @brief Attach the mainthread to the AppMgr.
     *
     */
    void Attach();

    /**
     *
     * @brief Set the runner state of mainthread.
     *
     * @param runnerStart whether the runner is started.
     */
    void SetRunnerStarted(bool runnerStart);

    /**
     *
     * @brief Connect the mainthread to the AppMgr.
     *
     */
    bool ConnectToAppMgr();

    /**
     *
     * @brief Check whether the appLaunchData is legal.
     *
     * @param appLaunchData The appLaunchData should be checked.
     *
     * @return if the appLaunchData is legal, return true. else return false.
     */
    bool CheckLaunchApplicationParam(const AppLaunchData &appLaunchData) const;

    /**
     *
     * @brief Check whether the record is legal.
     *
     * @param record The record should be checked.
     *
     * @return if the record is legal, return true. else return false.
     */
    bool CheckAbilityItem(const std::shared_ptr<AbilityLocalRecord> &record) const;

    /**
     *
     * @brief Init the mainthread.
     *
     * @param runner the runner belong to the mainthread.
     *
     */
    void Init(const std::shared_ptr<EventRunner> &runner);

    /**
     *
     * @brief Check whether the OHOSApplication is ready.
     *
     * @return if the OHOSApplication is ready, return true. else return false.
     *
     */
    bool IsApplicationReady() const;
    class MainHandler : public EventHandler {
    public:
        MainHandler(const std::shared_ptr<EventRunner> &runner, const sptr<MainThread> &thread);
        virtual ~MainHandler() = default;

        /**
         *
         * @brief Process the event.
         *
         * @param event the event want to be processed.
         *
         */
        void ProcessEvent(const OHOS::AppExecFwk::InnerEvent::Pointer &event) override;

    private:
        sptr<MainThread> mainThreadObj_ = nullptr;
    };

    bool isRunnerStarted_ = false;
    int newThreadId_ = -1;
    std::shared_ptr<Profile> appProfile_ = nullptr;
    std::shared_ptr<ApplicationInfo> applicationInfo_ = nullptr;
    std::shared_ptr<ProcessInfo> processInfo_ = nullptr;
    std::shared_ptr<OHOSApplication> application_ = nullptr;
    std::shared_ptr<ApplicationImpl> applicationImpl_ = nullptr;
    std::shared_ptr<MainHandler> mainHandler_ = nullptr;
    std::shared_ptr<AbilityRecordMgr> abilityRecordMgr_ = nullptr;
    MainThreadState mainThreadState_ = MainThreadState::INIT;
    sptr<IAppMgr> appMgr_ = nullptr;  // appMgrService Handler
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    std::string aceApplicationName_ = "AceApplication";
    std::string pathSeparator_ = "/";
    std::string abilityLibraryType_ = ".so";

#ifdef ABILITY_LIBRARY_LOADER
    /**
     *
     * @brief Load the ability library.
     *
     * @param libraryPaths the library paths.
     *
     */
    void LoadAbilityLibrary(const std::vector<std::string> &libraryPaths);

    /**
     *
     * @brief Close the ability library loaded.
     *
     */
    void CloseAbilityLibrary();

    /**
     *
     * @brief Scan the dir ability library loaded.
     *
     * @param dirPath the the path should be scan.
     *
     */
    bool ScanDir(const std::string &dirPath);

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
    bool CheckFileType(const std::string &fileName, const std::string &extensionName);
    std::vector<std::string> fileEntries_;
    std::vector<void *> handleAbilityLib_;  // the handler of ACE Library.
#endif                                      // ABILITY_LIBRARY_LOADER
#ifdef APPLICATION_LIBRARY_LOADER
    void *handleAppLib_ = nullptr;  // the handler of ACE Library.
    constexpr static std::string applicationLibraryPath = "/hos/lib/libapplication_native.z.so";
#endif  // APPLICATION_LIBRARY_LOADER
    DISALLOW_COPY_AND_MOVE(MainThread);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_MAIN_THREAD_H
