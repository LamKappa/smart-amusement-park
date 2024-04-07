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

#include "mission_stack.h"

#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
MissionStack::MissionStack(int id, int userId) : missionStackId_(id), userId_(userId)
{}

MissionStack::~MissionStack()
{}

std::shared_ptr<MissionRecord> MissionStack::GetTargetMissionRecord(const std::string &bundleName)
{
    /* We need choose which misson the target ability insert from misson list.
     * The first ability in the misson is the entrance of this misson.
     * Normally,
     * one applicaiton's abilites are in the same misson in misson stack.
     * So, the choose method is that the target ability's bundle name
     * whether matches to which misson's entrance.
     * Check the first ability record in mission record.
     * if the first ability bundle name in mission record
     * equal to the target ability name, then choose this mission record.
     */
    auto checkAbility = [&bundleName](const std::shared_ptr<MissionRecord> &missionRecord) {
        if (missionRecord == nullptr) {
            return false;
        }
        return missionRecord->IsSameMissionRecord(bundleName);
    };
    auto iter = std::find_if(missions_.begin(), missions_.end(), checkAbility);
    if (iter == missions_.end()) {
        return nullptr;
    }
    return *iter;
}

int MissionStack::GetMissionStackId() const
{
    return missionStackId_;
}

int MissionStack::GetMissionStackUserId() const
{
    return userId_;
}

int MissionStack::GetMissionRecordCount() const
{
    return missions_.size();
}

std::shared_ptr<AbilityRecord> MissionStack::GetTopAbilityRecord() const
{
    if (missions_.empty()) {
        HILOG_ERROR("missions_ is empty");
        return nullptr;
    }
    std::shared_ptr<MissionRecord> topMission = missions_.front();
    if (topMission) {
        return topMission->GetTopAbilityRecord();
    }
    return nullptr;
}

std::shared_ptr<AbilityRecord> MissionStack::GetAbilityRecordById(const int64_t recordId)
{
    std::shared_ptr<AbilityRecord> abilityRecord = nullptr;
    for (std::shared_ptr<MissionRecord> mission : missions_) {
        abilityRecord = mission->GetAbilityRecordById(recordId);
        if (abilityRecord != nullptr) {
            return abilityRecord;
        }
    }
    return nullptr;
}
std::shared_ptr<MissionRecord> MissionStack::GetTopMissionRecord()
{
    if (missions_.empty()) {
        HILOG_ERROR("missions_ is empty");
        return nullptr;
    }
    return missions_.front();
}

std::shared_ptr<MissionRecord> MissionStack::GetMissionRecordById(int id)
{
    if (missions_.empty()) {
        HILOG_ERROR("missions_ is empty");
        return nullptr;
    }
    for (auto iter = missions_.begin(); iter != missions_.end(); iter++) {
        if ((*iter) != nullptr && (*iter)->GetMissionRecordId() == id) {
            return *iter;
        }
    }
    return nullptr;
}

std::shared_ptr<AbilityRecord> MissionStack::GetAbilityRecordByToken(const sptr<IRemoteObject> &token)
{
    std::shared_ptr<AbilityRecord> abilityRecord = nullptr;
    for (std::shared_ptr<MissionRecord> mission : missions_) {
        abilityRecord = mission->GetAbilityRecordByToken(token);
        if (abilityRecord != nullptr) {
            return abilityRecord;
        }
    }
    return nullptr;
}

std::shared_ptr<AbilityRecord> MissionStack::GetAbilityRecordByCaller(
    const std::shared_ptr<AbilityRecord> &caller, int requestCode)
{
    for (auto mission : missions_) {
        auto abilityRecord = mission->GetAbilityRecordByCaller(caller, requestCode);
        if (abilityRecord) {
            return abilityRecord;
        }
    }
    return nullptr;
}

bool MissionStack::RemoveAbilityRecordByToken(const Token &token)
{
    std::shared_ptr<AbilityRecord> abilityRecord = token.GetAbilityRecord();
    for (std::shared_ptr<MissionRecord> mission : missions_) {
        if (mission->RemoveAbilityRecord(abilityRecord)) {
            return true;
        }
    }
    HILOG_INFO("RemoveAbilityRecordByToken can not find AbilityRecord");
    return false;
}

bool MissionStack::RemoveMissionRecord(int id)
{
    if (missions_.empty()) {
        return false;
    }
    for (auto iter = missions_.begin(); iter != missions_.end(); iter++) {
        if ((*iter)->GetMissionRecordId() == id) {
            missions_.erase(iter);
            return true;
        }
    }
    return false;
}

void MissionStack::RemoveAll()
{
    missions_.clear();
}

void MissionStack::AddMissionRecordToTop(std::shared_ptr<MissionRecord> mission)
{
    if (mission == nullptr) {
        HILOG_ERROR("mission is nullptr");
        return;
    }
    auto isExist = [targetMission = mission](const std::shared_ptr<MissionRecord> &mission) {
        if (mission == nullptr) {
            return false;
        }
        return targetMission == mission;
    };
    auto iter = std::find_if(missions_.begin(), missions_.end(), isExist);
    if (iter == missions_.end()) {
        missions_.push_front(mission);
    }
}

void MissionStack::MoveMissionRecordToTop(std::shared_ptr<MissionRecord> mission)
{
    if (mission == nullptr) {
        HILOG_ERROR("mission is nullptr");
        return;
    }
    if (missions_.front() == mission) {
        HILOG_ERROR("missions is at the top of list");
        return;
    }
    for (auto iter = missions_.begin(); iter != missions_.end(); iter++) {
        if ((*iter) == mission) {
            missions_.erase(iter);
            break;
        }
    }
    missions_.push_front(mission);
}

void MissionStack::Dump(std::vector<std::string> &info)
{
    std::string dumpInfo = "  MissionStack ID #" + std::to_string(missionStackId_);
    info.push_back(dumpInfo);
    for (auto missionRecord : missions_) {
        missionRecord->Dump(info);
    }
}

void MissionStack::DumpStackList(std::vector<std::string> &info)
{
    std::string dumpInfo = "  MissionStack ID #" + std::to_string(missionStackId_) + " [";
    for (auto missionRecord : missions_) {
        dumpInfo += " ";
        dumpInfo += "#";
        dumpInfo += std::to_string(missionRecord->GetMissionRecordId());
    }
    dumpInfo += " ]";
    info.push_back(dumpInfo);
}

void MissionStack::GetAllMissionInfo(std::vector<MissionRecordInfo> &missionInfos)
{
    for (auto mission : missions_) {
        MissionRecordInfo missionRecordInfo;
        missionRecordInfo.id = mission->GetMissionRecordId();
        mission->GetAllAbilityInfo(missionRecordInfo.abilityRecordInfos);
        missionInfos.emplace_back(missionRecordInfo);
    }
}

bool MissionStack::IsExistMissionRecord(int missionId)
{
    for (auto &iter : missions_) {
        if (iter->GetMissionRecordId() == missionId) {
            return true;
        }
    }
    return false;
}
}  // namespace AAFwk
}  // namespace OHOS
