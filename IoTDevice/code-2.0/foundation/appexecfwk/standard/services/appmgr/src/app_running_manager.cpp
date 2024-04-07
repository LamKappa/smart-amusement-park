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

#include "app_running_manager.h"

#include "iremote_object.h"
#include "datetime_ex.h"

#include "app_log_wrapper.h"
#include "perf_profile.h"
#include "appexecfwk_errors.h"

namespace OHOS {

namespace AppExecFwk {

namespace {

bool CheckUid(const int32_t uid)
{
    return uid >= 0 && uid < std::numeric_limits<int32_t>::max();
}

}  // namespace

AppRunningManager::AppRunningManager()
{}
AppRunningManager::~AppRunningManager()
{}

std::shared_ptr<AppRunningRecord> AppRunningManager::GetOrCreateAppRunningRecord(const sptr<IRemoteObject> &token,
    const std::shared_ptr<ApplicationInfo> &appInfo, const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::string &processName, const int32_t uid, RecordQueryResult &result)
{
    result.Reset();
    if (!token || !appInfo || !abilityInfo) {
        APP_LOGE("param error");
        result.error = ERR_INVALID_VALUE;
        return nullptr;
    }
    if (!CheckUid(uid)) {
        APP_LOGE("uid invalid");
        result.error = ERR_APPEXECFWK_INVALID_UID;
        return nullptr;
    }
    if (processName.empty()) {
        APP_LOGE("processName error");
        result.error = ERR_INVALID_VALUE;
        return nullptr;
    }

    auto record = GetAppRunningRecordByProcessName(appInfo->name, processName);
    if (!record) {
        APP_LOGI("no app record, create");
        auto recordId = AppRecordId::Create();
        record = std::make_shared<AppRunningRecord>(appInfo, recordId, processName);
        appRunningRecordMap_.emplace(recordId, record);
    } else {
        result.appExists = true;
    }

    result.appRecordId = record->GetRecordId();
    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    result.abilityExists = !!abilityRecord;
    if (!abilityRecord) {
        APP_LOGI("no ability record, create");
        abilityRecord = record->AddAbility(token, abilityInfo);
    }
    return record;
}

std::shared_ptr<AppRunningRecord> AppRunningManager::GetAppRunningRecordByAppName(const std::string &appName) const
{
    auto iter = std::find_if(appRunningRecordMap_.begin(), appRunningRecordMap_.end(), [&appName](const auto &pair) {
        return pair.second->GetName() == appName;
    });
    return ((iter == appRunningRecordMap_.end()) ? nullptr : iter->second);
}

std::shared_ptr<AppRunningRecord> AppRunningManager::GetAppRunningRecordByProcessName(
    const std::string &appName, const std::string &processName) const
{
    auto iter = std::find_if(
        appRunningRecordMap_.begin(), appRunningRecordMap_.end(), [&appName, &processName](const auto &pair) {
            return ((pair.second->GetName() == appName) && (pair.second->GetProcessName() == processName));
        });
    return ((iter == appRunningRecordMap_.end()) ? nullptr : iter->second);
}

std::shared_ptr<AppRunningRecord> AppRunningManager::GetAppRunningRecordByPid(const pid_t pid) const
{
    auto iter = std::find_if(appRunningRecordMap_.begin(), appRunningRecordMap_.end(), [&pid](const auto &pair) {
        return pair.second->GetPriorityObject()->GetPid() == pid;
    });
    return ((iter == appRunningRecordMap_.end()) ? nullptr : iter->second);
}

std::shared_ptr<AppRunningRecord> AppRunningManager::GetAppRunningRecordByAbilityToken(
    const sptr<IRemoteObject> &abilityToken) const
{
    for (const auto &item : appRunningRecordMap_) {
        const auto &appRecord = item.second;
        if (appRecord && appRecord->GetAbilityRunningRecordByToken(abilityToken)) {
            return appRecord;
        }
    }
    return nullptr;
}

std::shared_ptr<AppRunningRecord> AppRunningManager::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    if (remote == nullptr) {
        APP_LOGE("remote is null");
        return nullptr;
    }
    sptr<IRemoteObject> object = remote.promote();
    if (!object) {
        APP_LOGE("object is null");
        return nullptr;
    }
    const auto &iter =
        std::find_if(appRunningRecordMap_.begin(), appRunningRecordMap_.end(), [&object](const auto &pair) {
            if (pair.second && pair.second->GetApplicationClient() != nullptr) {
                return pair.second->GetApplicationClient()->AsObject() == object;
            }
            return false;
        });
    if (iter != appRunningRecordMap_.end()) {
        auto appRecord = iter->second;
        appRunningRecordMap_.erase(iter);
        if (appRecord) {
            return appRecord;
        }
    }
    return nullptr;
}

const std::map<const int32_t, const std::shared_ptr<AppRunningRecord>> &AppRunningManager::GetAppRunningRecordMap()
    const
{
    return appRunningRecordMap_;
}

void AppRunningManager::RemoveAppRunningRecordById(const int32_t recordId)
{
    appRunningRecordMap_.erase(recordId);
}

void AppRunningManager::ClearAppRunningRecordMap()
{
    appRunningRecordMap_.clear();
}

}  // namespace AppExecFwk
}  // namespace OHOS