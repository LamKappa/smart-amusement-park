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

#include "mission_record.h"

#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
int MissionRecord::nextMissionId_ = 0;

MissionRecord::MissionRecord(const std::string &bundleName) : bundleName_(bundleName)
{
    missionId_ = GetNextMissionId();
}

MissionRecord::~MissionRecord()
{}

int MissionRecord::GetNextMissionId()
{
    return nextMissionId_++;
}

int MissionRecord::GetMissionRecordId() const
{
    return missionId_;
}

int MissionRecord::GetAbilityRecordCount() const
{
    return abilities_.size();
}

std::shared_ptr<AbilityRecord> MissionRecord::GetBottomAbilityRecord() const
{
    if (abilities_.empty()) {
        HILOG_ERROR("abilities is empty");
        return nullptr;
    }
    return abilities_.back();
}

std::shared_ptr<AbilityRecord> MissionRecord::GetTopAbilityRecord() const
{
    if (abilities_.empty()) {
        HILOG_ERROR("abilities is empty");
        return nullptr;
    }
    return abilities_.front();
}

std::shared_ptr<AbilityRecord> MissionRecord::GetAbilityRecordByToken(const sptr<IRemoteObject> &token) const
{
    std::shared_ptr<AbilityRecord> abilityToFind = Token::GetAbilityRecordByToken(token);
    if (abilityToFind == nullptr) {
        HILOG_ERROR("ability in token is null");
        return nullptr;
    }

    auto isExist = [targetAbility = abilityToFind](const std::shared_ptr<AbilityRecord> &ability) {
        if (ability == nullptr) {
            return false;
        }
        return targetAbility == ability;
    };

    auto iter = std::find_if(abilities_.begin(), abilities_.end(), isExist);
    if (iter != abilities_.end()) {
        return *iter;
    }

    return nullptr;
}

std::shared_ptr<AbilityRecord> MissionRecord::GetAbilityRecordById(const int64_t recordId) const
{
    for (std::shared_ptr<AbilityRecord> ability : abilities_) {
        if (ability != nullptr && ability->GetRecordId() == recordId) {
            return ability;
        }
    }
    return nullptr;
}

std::shared_ptr<AbilityRecord> MissionRecord::GetAbilityRecordByCaller(
    const std::shared_ptr<AbilityRecord> &caller, int requestCode)
{
    for (auto &ability : abilities_) {
        auto callerList = ability->GetCallerRecordList();
        for (auto callerAbility : callerList) {
            if (callerAbility->GetCaller() == caller && callerAbility->GetRequestCode() == requestCode) {
                return ability;
            }
        }
    }
    return nullptr;
}

void MissionRecord::AddAbilityRecordToTop(std::shared_ptr<AbilityRecord> ability)
{
    if (ability == nullptr) {
        HILOG_ERROR("ability is null");
        return;
    }
    auto isExist = [targetAbility = ability](const std::shared_ptr<AbilityRecord> &ability) {
        if (ability == nullptr) {
            return false;
        }
        return targetAbility == ability;
    };
    auto iter = std::find_if(abilities_.begin(), abilities_.end(), isExist);
    if (iter == abilities_.end()) {
        abilities_.push_front(ability);
    }
}

bool MissionRecord::RemoveAbilityRecord(std::shared_ptr<AbilityRecord> ability)
{
    if (ability == nullptr) {
        HILOG_ERROR("ability is null");
        return false;
    }
    for (auto iter = abilities_.begin(); iter != abilities_.end(); iter++) {
        if ((*iter) == ability) {
            abilities_.erase(iter);
            return true;
        }
    }
    HILOG_ERROR("can not find ability");
    return false;
}

bool MissionRecord::RemoveTopAbilityRecord()
{
    if (abilities_.empty()) {
        HILOG_ERROR("abilities is empty");
        return false;
    }
    abilities_.pop_front();
    return true;
}

void MissionRecord::RemoveAll()
{
    abilities_.clear();
}

void MissionRecord::Dump(std::vector<std::string> &info)
{
    std::string dumpInfo = "    MissionRecord ID #" + std::to_string(missionId_);
    std::shared_ptr<AbilityRecord> bottomAbility = GetBottomAbilityRecord();
    if (bottomAbility) {
        dumpInfo += "  bottom app [" + bottomAbility->GetAbilityInfo().name + "]";
        info.push_back(dumpInfo);
        for (auto abilityRecord : abilities_) {
            abilityRecord->Dump(info);
        }
    }
}

bool MissionRecord::IsSameMissionRecord(const std::string &bundleName) const
{
    if (bundleName.empty() || bundleName_.empty()) {
        return false;
    }
    return (bundleName == bundleName_);
}

void MissionRecord::GetAllAbilityInfo(std::vector<AbilityRecordInfo> &abilityInfos)
{
    for (auto ability : abilities_) {
        AbilityRecordInfo abilityInfo;
        ability->GetAbilityRecordInfo(abilityInfo);
        abilityInfos.emplace_back(abilityInfo);
    }
}

bool MissionRecord::IsTopAbilityRecordByName(const std::string &abilityName)
{
    std::shared_ptr<AbilityRecord> topAbility = GetTopAbilityRecord();
    if (topAbility == nullptr) {
        return false;
    }

    return (topAbility->GetAbilityInfo().name.compare(abilityName) == 0);
}

void MissionRecord::SetIsLauncherCreate()
{
    isLuncherCreate_ = true;
}

bool MissionRecord::IsLauncherCreate() const
{
    return isLuncherCreate_;
}

void MissionRecord::SetPreMissionRecord(const std::shared_ptr<MissionRecord> &record)
{
    preMissionRecord_ = record;
}

std::shared_ptr<MissionRecord> MissionRecord::GetPreMissionRecord() const
{
    return preMissionRecord_.lock();
}

bool MissionRecord::IsExistAbilityRecord(int32_t id)
{
    for (auto &it : abilities_) {
        if (it->GetRecordId() == id) {
            return true;
        }
    }
    return false;
}
}  // namespace AAFwk
}  // namespace OHOS
