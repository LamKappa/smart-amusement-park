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

#ifndef OHOS_AAFWK_ABILITY_STACK_MANAGER_H
#define OHOS_AAFWK_ABILITY_STACK_MANAGER_H

#include <mutex>
#include <list>
#include <queue>
#include <unordered_map>

#include "ability_info.h"
#include "ability_record.h"
#include "application_info.h"
#include "mission_record.h"
#include "mission_stack.h"
#include "recent_mission_info.h"
#include "stack_info.h"
#include "want.h"

namespace OHOS {
namespace AAFwk {
/**
 * @class AbilityStackManager
 * AbilityStackManager provides a facility for managing page ability life cycle.
 */
class AbilityStackManager : public std::enable_shared_from_this<AbilityStackManager> {
public:
    explicit AbilityStackManager(int userId);

    ~AbilityStackManager();

    /**
     * init ability stack manager.
     *
     */
    void Init();

    /**
     * StartAbility with request.
     *
     * @param abilityRequest, the request of the service ability to start.
     * @return Returns ERR_OK on success, others on failure.
     */
    int StartAbility(const AbilityRequest &abilityRequest);

    /**
     * TerminateAbility with token and result want.
     *
     * @param token, the token of service type's ability to terminate.
     * @param resultCode, the result code of service type's ability to terminate.
     * @param resultWant, the result want for service type's ability to terminate.
     * @return Returns ERR_OK on success, others on failure.
     */
    int TerminateAbility(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant);

    /**
     * TerminateAbility, terminate the special ability.
     *
     * @param caller, caller ability record.
     * @param requestCode, abililty request code
     * @return Returns ERR_OK on success, others on failure.
     */
    int TerminateAbility(const std::shared_ptr<AbilityRecord> &caller, int requestCode);

    /**
     * get ability stack manager's user id.
     *
     * @return Returns userId.
     */
    int GetAbilityStackManagerUserId() const;

    /**
     * get current working mission stack.
     *
     * @return current mission stack.
     */
    std::shared_ptr<MissionStack> GetCurrentMissionStack() const
    {
        return currentMissionStack_;
    }

    /**
     * get current top ability of stack.
     *
     * @return top ability record.
     */
    std::shared_ptr<AbilityRecord> GetCurrentTopAbility() const;

    /**
     * get current top ability's token of stack.
     *
     * @return top ability record's token.
     */
    sptr<Token> GetCurrentTopAbilityToken();

    /**
     * get the ability record by token.
     *
     * @param recordId, ability record id.
     * @return ability record.
     */
    std::shared_ptr<AbilityRecord> GetAbilityRecordById(const int64_t recordId);

    /**
     * get current top mission of stack.
     *
     * @return top mission record.
     */
    std::shared_ptr<MissionRecord> GetTopMissionRecord() const;

    /**
     * get the ability record by token.
     *
     * @param token, the token of ability.
     * @return ability record.
     */
    std::shared_ptr<AbilityRecord> GetAbilityRecordByToken(const sptr<IRemoteObject> &token);

    /**
     * get terminating ability from terminate list.
     *
     * @param token, the token of ability.
     */
    std::shared_ptr<AbilityRecord> GetAbilityFromTerminateList(const sptr<IRemoteObject> &token);

    /**
     * get the mission record by record id.
     *
     * @param id, the record id of mission.
     * @return mission record.
     */
    std::shared_ptr<MissionRecord> GetMissionRecordById(int id) const;

    /**
     * get the mission record by record id from all stacks.
     *
     * @param id, the record id of mission.
     * @return mission record.
     */
    std::shared_ptr<MissionRecord> GetMissionRecordFromAllStacks(int id) const;

    /**
     * remove the mission record by record id.
     *
     * @param id, the record id of mission.
     * @return Returns true on success, false on failure.
     */
    bool RemoveMissionRecordById(int id);

    /**
     * attach ability thread ipc object.
     *
     * @param scheduler, ability thread ipc object.
     * @param token, the token of ability.
     * @return Returns ERR_OK on success, others on failure.
     */
    int AttachAbilityThread(const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token);

    /**
     * AbilityTransitionDone, ability call this interface after lift cycle was changed.
     *
     * @param token,.ability's token.
     * @param state,.the state of ability lift cycle.
     * @return Returns ERR_OK on success, others on failure.
     */
    int AbilityTransitionDone(const sptr<IRemoteObject> &token, int state);

    /**
     * AddWindowInfo, add windowToken to AbilityRecord.
     *
     * @param token, the token of the ability.
     * @param windowToken, window id of the ability.
     */
    void AddWindowInfo(const sptr<IRemoteObject> &token, int32_t windowToken);

    /**
     * OnAbilityRequestDone, app manager service call this interface after ability request done.
     *
     * @param token,ability's token.
     * @param state,the state of ability lift cycle.
     */
    void OnAbilityRequestDone(const sptr<IRemoteObject> &token, const int32_t state);

    /**
     * Remove the specified mission from the stack by mission id.
     *
     * @param missionId, target mission id.
     * @return Returns ERR_OK on success, others on failure.
     */
    int RemoveMissionById(int missionId);

    /**
     * Remove the specified mission stack by stack id
     *
     * @param id.
     * @return Returns ERR_OK on success, others on failure.
     */
    int RemoveStack(int stackId);

    /**
     * move the mission stack to the top.
     *
     * @param stack, target mission stack.
     */
    void MoveMissionStackToTop(const std::shared_ptr<MissionStack> &stack);

    /**
     * complete ability life cycle .
     *
     * @param abilityRecord.
     */
    void CompleteActive(const std::shared_ptr<AbilityRecord> &abilityRecord);
    void CompleteInactive(const std::shared_ptr<AbilityRecord> &abilityRecord);
    void CompleteBackground(const std::shared_ptr<AbilityRecord> &abilityRecord);
    void CompleteTerminate(const std::shared_ptr<AbilityRecord> &abilityRecord);

    void MoveToBackgroundTask(const std::shared_ptr<AbilityRecord> &abilityRecord);

    /**
     * dump ability stack info, about userID, mission stack info,
     * mission record info and ability info.
     *
     * @param info Ability stack info.
     * @return Returns ERR_OK on success, others on failure.
     */
    void Dump(std::vector<std::string> &info);
    void DumpWaittingAbilityQueue(std::string &result);
    void DumpTopAbility(std::vector<std::string> &info);
    void DumpMission(int missionId, std::vector<std::string> &info);
    void DumpStack(int missionStackId, std::vector<std::string> &info);
    void DumpStackList(std::vector<std::string> &info);

    /**
     * get the target mission stack by want info.
     *
     * @param want , the want for starting ability.
     */
    std::shared_ptr<MissionStack> GetTargetMissionStack(const AbilityRequest &abilityRequest);

    /**
     * Obtains information about ability stack that are running on the device.
     *
     * @param stackInfo Ability stack info.
     * @return Returns ERR_OK on success, others on failure.
     */
    void GetAllStackInfo(StackInfo &stackInfo);

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
    int GetRecentMissions(const int32_t numMax, const int32_t flags, std::vector<RecentMissionInfo> &recentList);

    /**
     * Ask that the mission associated with a given mission ID be moved to the
     * front of the stack, so it is now visible to the user.
     *
     * @param missionId.
     * @return Returns ERR_OK on success, others on failure.
     */
    int MoveMissionToTop(int32_t missionId);

    /**
     * Ability detects death
     *
     * @param abilityRecord
     */
    void OnAbilityDied(std::shared_ptr<AbilityRecord> abilityRecord);

    /**
     * Kill the process immediately.
     *
     * @param bundleName.
     * @return Returns ERR_OK on success, others on failure.
     */
    int KillProcess(const std::string &bundleName);

    /**
     * Uninstall app
     *
     * @param bundleName.
     * @return Returns ERR_OK on success, others on failure.
     */
    int UninstallApp(const std::string &bundleName);

    void OnTimeOut(uint32_t msgId, int64_t eventId);

private:
    /**
     * dispatch ability life cycle .
     *
     * @param abilityRecord.
     * @param state.
     */
    int DispatchState(const std::shared_ptr<AbilityRecord> &abilityRecord, int state);
    int DispatchActive(const std::shared_ptr<AbilityRecord> &abilityRecord, int state);
    int DispatchInactive(const std::shared_ptr<AbilityRecord> &abilityRecord, int state);
    int DispatchBackground(const std::shared_ptr<AbilityRecord> &abilityRecord, int state);
    int DispatchTerminate(const std::shared_ptr<AbilityRecord> &abilityRecord, int state);

    /**
     * StartAbilityLocked.
     *
     * @param currentTopAbilityRecord, current top ability.
     * @param abilityRequest the request of the ability to start.
     * @return Returns ERR_OK on success, others on failure.
     */
    int StartAbilityLocked(
        const std::shared_ptr<AbilityRecord> &currentTopAbility, const AbilityRequest &abilityRequest);

    /**
     * TerminateAbilityLocked.
     *
     * @param abilityRecord, target ability.
     * @param resultCode the result code of the ability to terminate.
     * @param resultWant the result Want of the ability to terminate.
     * @return Returns ERR_OK on success, others on failure.
     */
    int TerminateAbilityLocked(
        const std::shared_ptr<AbilityRecord> &abilityRecord, int resultCode, const Want *resultWant);

    /**
     * Remove the specified mission from the stack by mission id.
     *
     * @param missionId, target mission id.
     * @return Returns ERR_OK on success, others on failure.
     */
    int RemoveMissionByIdLocked(int missionId);

    /**
     * remove terminating ability from stack.
     *
     * @param abilityRecord, target ability.
     */
    void RemoveTerminatingAbility(const std::shared_ptr<AbilityRecord> &abilityRecord);

    /**
     * push waitting ability to queue.
     *
     * @param abilityRequest, the request of ability.
     */
    void EnqueueWaittingAbility(const AbilityRequest &abilityRequest);

    /**
     * start waitting ability.
     *
     */
    void StartWaittingAbility();

    /**
     * get tartget ability and mission by request and top ability.
     *
     * @param abilityRequest, the request of ability.
     * @param currentTopAbility, top ability.
     * @param tragetAbilityRecord, out param.
     * @param targetMissionRecord, out param.
     */
    void GetMissionRecordAndAbilityRecord(const AbilityRequest &abilityRequest,
        const std::shared_ptr<AbilityRecord> &currentTopAbility, std::shared_ptr<AbilityRecord> &tragetAbilityRecord,
        std::shared_ptr<MissionRecord> &targetMissionRecord);

    /**
     * check wheather the ability is launcher.
     *
     * @param abilityRequest, the abilityRequest fot starting ability.
     * @return Returns true on success, false on failure.
     */
    bool IsLauncherAbility(const AbilityRequest &abilityRequest);

    /**
     * check wheather the mission has launcher ability.
     *
     * @param id, mission id.
     * @return Returns true on success, false on failure.
     */
    bool IsLauncherMission(int id);

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
    int GetRecentMissionsLocked(const int32_t numMax, const int32_t flags, std::vector<RecentMissionInfo> &recentList);

    void CreateRecentMissionInfo(const MissionRecordInfo &mission, RecentMissionInfo &recentMissionInfo);

    /**
     * Ask that the mission associated with a given mission ID be moved to the
     * front of the stack, so it is now visible to the user.
     *
     * @param missionId.
     * @return Returns ERR_OK on success, others on failure.
     */
    int MoveMissionToTopLocked(int32_t missionId);

    /**
     * Remove the specified mission stack by stack id
     *
     * @param id.
     * @return Returns ERR_OK on success, others on failure.
     */
    int RemoveStackLocked(int stackId);

    /**
     * Force return to launcher
     */
    void BackToLauncher();
    void DelayedStartLauncher();

    /**
     * Ability from launcher stack detects death
     *
     * @param abilityRecord
     */
    void OnAbilityDiedByLauncher(std::shared_ptr<AbilityRecord> abilityRecord);

    /**
     * Ability from default stack detects death
     *
     * @param abilityRecord
     */
    void OnAbilityDiedByDefault(std::shared_ptr<AbilityRecord> abilityRecord);

    /**
     * Add uninstall tags to ability
     *
     * @param bundleName
     */
    void AddUninstallTags(const std::string &bundleName);

    void GetRecordBySingleton(const AbilityRequest &abilityRequest,
        const std::shared_ptr<AbilityRecord> &currentTopAbility, std::shared_ptr<AbilityRecord> &targetAbilityRecord,
        std::shared_ptr<MissionRecord> &targetMissionRecord);

    void GetRecordByStandard(const AbilityRequest &abilityRequest,
        const std::shared_ptr<AbilityRecord> &currentTopAbility, std::shared_ptr<AbilityRecord> &targetAbilityRecord,
        std::shared_ptr<MissionRecord> &targetMissionRecord);

    std::shared_ptr<AbilityRecord> GetLauncherRootAbility() const;
    std::shared_ptr<AbilityRecord> GetAbilityRecordByEventId(int64_t eventId) const;

private:
    const std::string MISSION_NAME_MARK_HEAD = "#";
    const std::string MISSION_NAME_SEPARATOR = ":";
    static constexpr int LAUNCHER_MISSION_STACK_ID = 0;
    static constexpr int DEFAULT_MISSION_STACK_ID = 1;
    int userId_;
    std::recursive_mutex stackLock_;
    std::shared_ptr<MissionStack> launcherMissionStack_;
    std::shared_ptr<MissionStack> defaultMissionStack_;
    std::shared_ptr<MissionStack> currentMissionStack_;
    std::shared_ptr<MissionStack> lastMissionStack_;
    std::list<std::shared_ptr<MissionStack>> missionStackList_;
    std::list<std::shared_ptr<AbilityRecord>> terminateAbilityRecordList_;  // abilities on terminating put in this
                                                                            // list.
    std::queue<AbilityRequest> waittingAbilityQueue_;
    // find AbilityRecord by windowToken. one windowToken has one and only one AbilityRecord.
    std::unordered_map<int, std::shared_ptr<AbilityRecord>> windowTokenToAbilityMap_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_ABILITY_STACK_MANAGER_H
