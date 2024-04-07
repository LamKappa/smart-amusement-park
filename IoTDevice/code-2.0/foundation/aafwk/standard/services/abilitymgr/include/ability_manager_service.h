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

#ifndef OHOS_AAFWK_ABILITY_MANAGER_SERVICE_H
#define OHOS_AAFWK_ABILITY_MANAGER_SERVICE_H

#include <memory>
#include <singleton.h>
#include <thread_ex.h>
#include <unordered_map>

#include "ability_connect_manager.h"
#include "ability_event_handler.h"
#include "ability_manager_stub.h"
#include "ability_stack_manager.h"
#include "app_scheduler.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "data_ability_manager.h"
#include "hilog_wrapper.h"
#include "iremote_object.h"
#include "kernal_system_app_manager.h"
#include "system_ability.h"
#include "uri.h"
#include "ability_config.h"

namespace OHOS {
namespace AAFwk {
enum class ServiceRunningState { STATE_NOT_START, STATE_RUNNING };
/**
 * @class AbilityManagerService
 * AbilityManagerService provides a facility for managing ability life cycle.
 */
class AbilityManagerService : public SystemAbility,
                              public AbilityManagerStub,
                              public AppStateCallback,
                              public std::enable_shared_from_this<AbilityManagerService> {
    DECLARE_DELAYED_SINGLETON(AbilityManagerService)
    DECLEAR_SYSTEM_ABILITY(AbilityManagerService)
public:
    void OnStart() override;
    void OnStop() override;
    ServiceRunningState QueryServiceState() const;

    /**
     * StartAbility with want, send want to ability manager service.
     *
     * @param want, the want of the ability to start.
     * @param requestCode, Ability request code.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int StartAbility(const Want &want, int requestCode = -1) override;

    /**
     * StartAbility with want, send want to ability manager service.
     *
     * @param want, the want of the ability to start.
     * @param callerToken, caller ability token.
     * @param requestCode the resultCode of the ability to start.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int StartAbility(const Want &want, const sptr<IRemoteObject> &callerToken, int requestCode = -1) override;

    /**
     * TerminateAbility, terminate the special ability.
     *
     * @param token, the token of the ability to terminate.
     * @param resultCode, the resultCode of the ability to terminate.
     * @param resultWant, the Want of the ability to return.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int TerminateAbility(
        const sptr<IRemoteObject> &token, int resultCode = -1, const Want *resultWant = nullptr) override;

    /**
     * TerminateAbility, terminate the special ability.
     *
     * @param callerToken, caller ability token.
     * @param requestCode, Ability request code.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int TerminateAbilityByCaller(const sptr<IRemoteObject> &callerToken, int requestCode) override;

    /**
     * ConnectAbility, connect session with service ability.
     *
     * @param want, Special want for service type's ability.
     * @param connect, Callback used to notify caller the result of connecting or disconnecting.
     * @param callerToken, caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int ConnectAbility(
        const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken) override;

    virtual int DisconnectAbility(const sptr<IAbilityConnection> &connect) override;

    /**
     * AcquireDataAbility, acquire a data ability by its authority, if it not existed,
     * AMS loads it synchronously.
     *
     * @param uri, data ability uri.
     * @param tryBind, true: when a data ability is died, ams will kill this client, or do nothing.
     * @param callerToken, specifies the caller ability token.
     * @return returns the data ability ipc object, or nullptr for failed.
     */
    virtual sptr<IAbilityScheduler> AcquireDataAbility(
        const Uri &uri, bool tryBind, const sptr<IRemoteObject> &callerToken) override;

    /**
     * ReleaseDataAbility, release the data ability that referenced by 'dataAbilityToken'.
     *
     * @param dataAbilityToken, specifies the data ability that will be released.
     * @param callerToken, specifies the caller ability token.
     * @return returns ERR_OK if succeeded, or error codes for failed.
     */
    virtual int ReleaseDataAbility(
        sptr<IAbilityScheduler> dataAbilityScheduler, const sptr<IRemoteObject> &callerToken) override;

    /**
     * AddWindowInfo, add windowToken to AbilityRecord.
     *
     * @param token, the token of the ability.
     * @param windowToken, window id of the ability.
     */
    virtual void AddWindowInfo(const sptr<IRemoteObject> &token, int32_t windowToken) override;

    /**
     * AttachAbilityThread, ability call this interface after loaded.
     *
     * @param scheduler,.the interface handler of kit ability.
     * @param token,.ability's token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int AttachAbilityThread(
        const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token) override;

    /**
     * AbilityTransitionDone, ability call this interface after lift cycle was changed.
     *
     * @param token,.ability's token.
     * @param state,.the state of ability lift cycle.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int AbilityTransitionDone(const sptr<IRemoteObject> &token, int state) override;

    /**
     * ScheduleConnectAbilityDone, service ability call this interface while session was connected.
     *
     * @param token,.service ability's token.
     * @param remoteObject,.the session proxy of service ability.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int ScheduleConnectAbilityDone(
        const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject) override;

    /**
     * ScheduleDisconnectAbilityDone, service ability call this interface while session was disconnected.
     *
     * @param token,.service ability's token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int ScheduleDisconnectAbilityDone(const sptr<IRemoteObject> &token) override;

    /**
     * ScheduleCommandAbilityDone, service ability call this interface while session was commanded.
     *
     * @param token,.service ability's token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int ScheduleCommandAbilityDone(const sptr<IRemoteObject> &token) override;

    /**
     * GetEventHandler, get the ability manager service's handler.
     *
     * @return Returns AbilityEventHandler ptr.
     */
    std::shared_ptr<AbilityEventHandler> GetEventHandler();

    /**
     * SetStackManager, set the user id of stack manager.
     *
     * @param userId, user id.
     */
    void SetStackManager(int userId);

    /**
     * GetStackManager, get the current stack manager.
     *
     * @return Returns AbilityStackManager ptr.
     */
    std::shared_ptr<AbilityStackManager> GetStackManager();

    /**
     * DumpWaittingAbilityQueue.
     *
     * @param result, result.
     */
    void DumpWaittingAbilityQueue(std::string &result);

    /**
     * dump ability stack info, about userID, mission stack info,
     * mission record info and ability info.
     *
     * @param state Ability stack info.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual void DumpState(const std::string &args, std::vector<std::string> &info) override;

    /**
     * Obtains information about ability stack that are running on the device.
     *
     * @param stackInfo Ability stack info.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int GetAllStackInfo(StackInfo &stackInfo) override;

    /**
     * Destroys this Service ability if the number of times it
     * has been started equals the number represented by
     * the given startId.
     *
     * @param token ability's token.
     * @param startId is incremented by 1 every time this ability is started.
     * @return Returns true if the startId matches the number of startup times
     * and this Service ability will be destroyed; returns false otherwise.
     */
    virtual int TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId) override;

    /**
     * Destroys this Service ability by Want.
     *
     * @param want, Special want for service type's ability.
     * @return Returns true if this Service ability will be destroyed; returns false otherwise.
     */
    virtual int StopServiceAbility(const Want &want) override;

    /**
     * Get the list of the missions that the user has recently launched,
     * with the most recent being first and older ones after in order.
     *
     * @param recentList recent mission info
     * @param numMax The maximum number of entries to return in the list. The
     * actual number returned may be smaller, depending on how many tasks the
     * user has started and the maximum number the system can remember.
     * @param falgs Information about what to return.  May be any combination
     * of {@link #RECENT_WITH_EXCLUDED} and {@link #RECENT_IGNORE_UNAVAILABLE}.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int GetRecentMissions(
        const int32_t numMax, const int32_t flags, std::vector<RecentMissionInfo> &recentList) override;

    /**
     * Get mission snapshot by mission id
     *
     * @param missionId the id of the mission to retrieve the sAutoapshots
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int GetMissionSnapshot(const int32_t missionId, MissionSnapshotInfo &snapshot) override;

    /**
     * Ask that the mission associated with a given mission ID be moved to the
     * front of the stack, so it is now visible to the user.
     *
     * @param missionId.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int MoveMissionToTop(int32_t missionId) override;

    /**
     * Remove the specified mission from the stack by mission id
     *
     * @param missionId.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int RemoveMission(int id) override;

    /**
     * Remove the specified mission stack by stack id
     *
     * @param id.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int RemoveStack(int id) override;

    /**
     * Kill the process immediately.
     *
     * @param bundleName.
     */
    virtual int KillProcess(const std::string &bundleName) override;

    /**
     * Uninstall app
     *
     * @param bundleName.
     */
    virtual int UninstallApp(const std::string &bundleName) override;

    /**
     * remove all service record.
     *
     */
    void RemoveAllServiceRecord();

    /**
     * get service record by element name.
     *
     */
    std::shared_ptr<AbilityRecord> GetServiceRecordByElementName(const std::string &element);
    std::list<std::shared_ptr<ConnectionRecord>> GetConnectRecordListByCallback(sptr<IAbilityConnection> callback);

    void OnAbilityDied(std::shared_ptr<AbilityRecord> abilityRecord);

    /**
     * wait for starting system ui.
     *
     */
    void StartSystemUi(const std::string name);

    void HandleLoadTimeOut(int64_t eventId);
    void HandleActiveTimeOut(int64_t eventId);
    void HandleInactiveTimeOut(int64_t eventId);

    // MSG 0 - 20 represents timeout message
    static constexpr uint32_t LOAD_TIMEOUT_MSG = 0;
    static constexpr uint32_t ACTIVE_TIMEOUT_MSG = 1;
    static constexpr uint32_t INACTIVE_TIMEOUT_MSG = 2;
    static constexpr uint32_t BACKGROUND_TIMEOUT_MSG = 3;
    static constexpr uint32_t TERMINATE_TIMEOUT_MSG = 4;

    static constexpr uint32_t LOAD_TIMEOUT = 500;          // ms
    static constexpr uint32_t ACTIVE_TIMEOUT = 5000;       // ms
    static constexpr uint32_t INACTIVE_TIMEOUT = 500;      // ms
    static constexpr uint32_t BACKGROUND_TIMEOUT = 10000;  // ms
    static constexpr uint32_t TERMINATE_TIMEOUT = 10000;   // ms
    static constexpr uint32_t CONNECT_TIMEOUT = 500;       // ms
    static constexpr uint32_t DISCONNECT_TIMEOUT = 500;    // ms
    static constexpr uint32_t COMMAND_TIMEOUT = 5000;      // ms
    static constexpr uint32_t SYSTEM_UI_TIMEOUT = 5000;    // ms
    static constexpr uint32_t RESTART_TIMEOUT = 5000;      // ms

    static constexpr uint32_t MIN_DUMP_ARGUMENT_NUM = 2;
    static constexpr uint32_t MAX_WAIT_SYSTEM_UI_NUM = 600;

    enum DumpKey {
        KEY_DUMP_ALL = 0,
        KEY_DUMP_STACK_LIST,
        KEY_DUMP_STACK,
        KEY_DUMP_MISSION,
        KEY_DUMP_TOP_ABILITY,
        KEY_DUMP_WAIT_QUEUE,
        KEY_DUMP_SERVICE,
        KEY_DUMP_DATA,
        KEY_DUMP_SYSTEM_UI
    };

protected:
    void OnAbilityRequestDone(const sptr<IRemoteObject> &token, const int32_t state) override;

private:
    /**
     * initialization of ability manager service.
     *
     */
    bool Init();
    /**
     * wait for starting lanucher ability.
     *
     */
    void WaitForStartingLauncherAbility();
    /**
     * get the user id.
     *
     */
    int GetUserId();
    bool IsSystemUiApp(const AppExecFwk::AbilityInfo &info) const;

    /**
     * generate ability request.
     *
     */
    int GenerateAbilityRequest(
        const Want &want, int requestCode, AbilityRequest &request, const sptr<IRemoteObject> &callerToken);

    sptr<AppExecFwk::IBundleMgr> GetBundleManager();
    int PreLoadAppDataAbilities(const std::string &bundleName);

    bool VerificationToken(const sptr<IRemoteObject> &token);

    void DumpInner(const std::string &args, std::vector<std::string> &info);
    void DumpStackListInner(const std::string &args, std::vector<std::string> &info);
    void DumpStackInner(const std::string &args, std::vector<std::string> &info);
    void DumpMissionInner(const std::string &args, std::vector<std::string> &info);
    void DumpTopAbilityInner(const std::string &args, std::vector<std::string> &info);
    void DumpWaittingAbilityQueueInner(const std::string &args, std::vector<std::string> &info);
    void DumpStateInner(const std::string &args, std::vector<std::string> &info);
    void DataDumpStateInner(const std::string &args, std::vector<std::string> &info);
    void SystemDumpStateInner(const std::string &args, std::vector<std::string> &info);
    void DumpFuncInit();
    using DumpFuncType = void (AbilityManagerService::*)(const std::string &args, std::vector<std::string> &info);
    std::map<uint32_t, DumpFuncType> dumpFuncMap_;

    const static int REPOLL_TIME_MICRO_SECONDS = 1000000;

    std::shared_ptr<AppExecFwk::EventRunner> eventLoop_;
    std::shared_ptr<AbilityEventHandler> handler_;
    ServiceRunningState state_;
    std::unordered_map<int, std::shared_ptr<AbilityStackManager>> stackManagers_;
    std::shared_ptr<AbilityStackManager> currentStackManager_;
    std::shared_ptr<AbilityConnectManager> connectManager_;
    sptr<AppExecFwk::IBundleMgr> iBundleManager_;
    std::shared_ptr<AppScheduler> appScheduler_;
    std::shared_ptr<DataAbilityManager> dataAbilityManager_;
    std::shared_ptr<KernalSystemAppManager> systemAppManager_;
    const static std::map<std::string, AbilityManagerService::DumpKey> dumpMap;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_ABILITY_MANAGER_SERVICE_H
