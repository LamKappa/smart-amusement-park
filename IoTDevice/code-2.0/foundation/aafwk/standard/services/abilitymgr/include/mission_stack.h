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

#ifndef OHOS_AAFWK_MISSION_STACK_H
#define OHOS_AAFWK_MISSION_STACK_H

#include <list>
#include <vector>

#include "ability_info.h"
#include "application_info.h"
#include "mission_record.h"
#include "mission_record_info.h"
#include "want.h"

namespace OHOS {
namespace AAFwk {
/**
 * @class MissionStack
 * MissionStack records mission info.
 */
class MissionStack {
public:
    MissionStack(int id, int userId);
    ~MissionStack();

    /**
     * get target mission record by app name
     *
     * @return mission record.
     */
    std::shared_ptr<MissionRecord> GetTargetMissionRecord(const std::string &appName);

    /**
     * get mission stack id
     *
     * @return id.
     */
    int GetMissionStackId() const;

    /**
     * get mission stack user id
     *
     * @return user id.
     */
    int GetMissionStackUserId() const;

    /**
     * get the number of missions owned by this mission stack
     *
     * @return count.
     */
    int GetMissionRecordCount() const;

    /**
     * get the top ability of this mission stack
     *
     * @return AbilityRecord.
     */
    std::shared_ptr<AbilityRecord> GetTopAbilityRecord() const;

    /**
     * get the top mission of this mission stack
     *
     * @return MissionRecord.
     */
    std::shared_ptr<MissionRecord> GetTopMissionRecord();

    /**
     * get the mission by id
     *
     * @return MissionRecord.
     */
    std::shared_ptr<MissionRecord> GetMissionRecordById(int id);

    /**
     * get the mission by token
     *
     * @return AbilityRecord.
     */
    std::shared_ptr<AbilityRecord> GetAbilityRecordByToken(const sptr<IRemoteObject> &token);

    /**
     * get the ability record by caller token and request code
     *
     * @return AbilityRecord.
     */
    std::shared_ptr<AbilityRecord> GetAbilityRecordByCaller(
        const std::shared_ptr<AbilityRecord> &caller, int requestCode);

    /**
     * get the mission by id
     *
     * @return AbilityRecord.
     */
    std::shared_ptr<AbilityRecord> GetAbilityRecordById(const int64_t recordId);

    /**
     * remove the ability record from stack by token
     */
    bool RemoveAbilityRecordByToken(const Token &token);

    /**
     * remove the mission record from stack by id
     */
    bool RemoveMissionRecord(int id);

    /**
     * remove all mission from stack
     */
    void RemoveAll();

    /**
     * add the mission at the top of the stack
     */
    void AddMissionRecordToTop(std::shared_ptr<MissionRecord> mission);

    /**
     * put the mission at the top of the stack
     */
    void MoveMissionRecordToTop(std::shared_ptr<MissionRecord> mission);

    /**
     * dump the mission stack information
     */
    void Dump(std::vector<std::string> &info);
    void DumpStackList(std::vector<std::string> &info);

    /**
     * get all mission info about this stack
     */
    void GetAllMissionInfo(std::vector<MissionRecordInfo> &missionInfos);

    /**
     * Check whether there is a mission record in the mission stack
     */
    bool IsExistMissionRecord(int missionId);

private:
    int missionStackId_;
    int userId_;
    std::list<std::shared_ptr<MissionRecord>> missions_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_MISSION_STACK_H
