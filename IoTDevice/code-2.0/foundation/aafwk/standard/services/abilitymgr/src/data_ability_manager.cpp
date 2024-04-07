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

#include "data_ability_manager.h"

#include <chrono>
#include <thread>

#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
using namespace std::chrono;
using namespace std::placeholders;

namespace {
constexpr bool DEBUG_ENABLED = false;
constexpr system_clock::duration DATA_ABILITY_LOAD_TIMEOUT = 11000ms;
}  // namespace

DataAbilityManager::DataAbilityManager()
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);
}

DataAbilityManager::~DataAbilityManager()
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);
}

sptr<IAbilityScheduler> DataAbilityManager::Acquire(
    const AbilityRequest &abilityRequest, bool tryBind, const sptr<IRemoteObject> &client)
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    if (abilityRequest.abilityInfo.type != AppExecFwk::AbilityType::DATA) {
        HILOG_ERROR("Data ability manager acquire: not a data ability.");
        return nullptr;
    }

    if (abilityRequest.abilityInfo.bundleName.empty() || abilityRequest.abilityInfo.name.empty()) {
        HILOG_ERROR("Data ability manager acquire: invalid name.");
        return nullptr;
    }

    std::shared_ptr<AbilityRecord> clientAbilityRecord;
    const std::string dataAbilityName(abilityRequest.abilityInfo.bundleName + '.' + abilityRequest.abilityInfo.name);

    if (client) {
        clientAbilityRecord = Token::GetAbilityRecordByToken(client);
        if (!clientAbilityRecord) {
            HILOG_ERROR("Data ability manager acquire: invalid client token.");
            return nullptr;
        }
        if (abilityRequest.abilityInfo.bundleName == clientAbilityRecord->GetAbilityInfo().bundleName &&
            abilityRequest.abilityInfo.name == clientAbilityRecord->GetAbilityInfo().name) {
            HILOG_ERROR("Data ability '%{public}s' cannot acquires itself.", dataAbilityName.c_str());
            return nullptr;
        }
        HILOG_INFO("Ability '%{public}s' acquiring data ability '%{public}s'...",
            clientAbilityRecord->GetAbilityInfo().name.c_str(),
            dataAbilityName.c_str());
    } else {
        HILOG_INFO("Loading data ability '%{public}s'...", dataAbilityName.c_str());
    }

    std::lock_guard<std::mutex> locker(mutex_);

    if (DEBUG_ENABLED) {
        DumpLocked(__func__, __LINE__);
    }

    DataAbilityRecordPtr dataAbilityRecord;

    auto it = dataAbilityRecordsLoaded_.find(dataAbilityName);
    if (it == dataAbilityRecordsLoaded_.end()) {
        HILOG_DEBUG("Acquiring data ability is not existed, loading...");
        dataAbilityRecord = LoadLocked(dataAbilityName, abilityRequest);
        if (!dataAbilityRecord) {
            HILOG_ERROR("Failed to load data ability '%{public}s'.", dataAbilityName.c_str());
            return nullptr;
        }
    } else {
        HILOG_DEBUG("Acquiring data ability is existed .");
        dataAbilityRecord = it->second;
    }

    auto scheduler = dataAbilityRecord->GetScheduler();
    if (!scheduler) {
        if (DEBUG_ENABLED) {
            HILOG_ERROR("BUG: data ability '%{public}s' is not loaded, removing it...", dataAbilityName.c_str());
        }
        auto it = dataAbilityRecordsLoaded_.find(dataAbilityName);
        if (it != dataAbilityRecordsLoaded_.end()) {
            dataAbilityRecordsLoaded_.erase(it);
        }
        return nullptr;
    }

    if (clientAbilityRecord) {
        dataAbilityRecord->AddClient(clientAbilityRecord, tryBind);
    }

    if (DEBUG_ENABLED) {
        DumpLocked(__func__, __LINE__);
    }

    return scheduler;
}

int DataAbilityManager::Release(const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &client)
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    if (!scheduler || !client) {
        HILOG_ERROR("Release data ability with invalid parameters.");
        return ERR_NULL_OBJECT;
    }

    auto clientAbilityRecord = Token::GetAbilityRecordByToken(client);
    if (!clientAbilityRecord) {
        HILOG_ERROR("Release data ability with unknown client.");
        return ERR_UNKNOWN_OBJECT;
    }

    std::lock_guard<std::mutex> locker(mutex_);

    if (DEBUG_ENABLED) {
        DumpLocked(__func__, __LINE__);
    }

    DataAbilityRecordPtrMap::iterator it;
    DataAbilityRecordPtr dataAbilityRecord;

    for (it = dataAbilityRecordsLoaded_.begin(); it != dataAbilityRecordsLoaded_.end(); ++it) {
        if (it->second->GetScheduler() != nullptr && it->second->GetScheduler()->AsObject() == scheduler->AsObject()) {
            dataAbilityRecord = it->second;
            break;
        }
    }

    if (!dataAbilityRecord) {
        HILOG_ERROR("Releasing not existed data ability.");
        return ERR_UNKNOWN_OBJECT;
    }

    if (dataAbilityRecord->GetClientCount(clientAbilityRecord) == 0) {
        HILOG_ERROR("Release data ability with wrong client.");
        return ERR_UNKNOWN_OBJECT;
    }

    HILOG_INFO("Releasing data ability '%{public}s'...", it->first.c_str());
    dataAbilityRecord->RemoveClient(clientAbilityRecord);

    if (DEBUG_ENABLED) {
        DumpLocked(__func__, __LINE__);
    }

    return ERR_OK;
}

int DataAbilityManager::AttachAbilityThread(const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token)
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    if (!scheduler || !token) {
        HILOG_ERROR("Attaching data ability with invalid parameters.");
        return ERR_NULL_OBJECT;
    }

    std::lock_guard<std::mutex> locker(mutex_);

    if (DEBUG_ENABLED) {
        DumpLocked(__func__, __LINE__);
    }

    HILOG_INFO("Attaching data ability...");

    DataAbilityRecordPtrMap::iterator it = dataAbilityRecordsLoading_.begin();
    DataAbilityRecordPtr dataAbilityRecord;
    auto record = Token::GetAbilityRecordByToken(token);
    std::string abilityName = "";
    if (record != nullptr) {
        abilityName = record->GetAbilityInfo().name;
    }
    for (it = dataAbilityRecordsLoading_.begin(); it != dataAbilityRecordsLoading_.end(); ++it) {
        if (it->second->GetToken() == token) {
            dataAbilityRecord = it->second;
            break;
        }
    }

    if (!dataAbilityRecord) {
        HILOG_ERROR("Attaching data ability '%{public}s' is not in loading state.", abilityName.c_str());
        return ERR_UNKNOWN_OBJECT;
    }

    if (DEBUG_ENABLED && dataAbilityRecord->GetClientCount() > 0) {
        HILOG_ERROR("BUG: Attaching data ability '%{public}s' has clients.", abilityName.c_str());
    }

    if (DEBUG_ENABLED && dataAbilityRecord->GetScheduler()) {
        HILOG_ERROR("BUG: Attaching data ability '%{public}s' has ready.", abilityName.c_str());
    }

    if (DEBUG_ENABLED && dataAbilityRecordsLoaded_.count(it->first) != 0) {
        HILOG_ERROR("BUG: The attaching data ability '%{public}s' has already existed.", abilityName.c_str());
    }

    return dataAbilityRecord->Attach(scheduler);
}

int DataAbilityManager::AbilityTransitionDone(const sptr<IRemoteObject> &token, int state)
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    if (!token) {
        HILOG_ERROR("Data ability transition done with invalid parameters.");
        return ERR_NULL_OBJECT;
    }

    std::lock_guard<std::mutex> locker(mutex_);

    if (DEBUG_ENABLED) {
        DumpLocked(__func__, __LINE__);
    }

    HILOG_INFO("Handling data ability transition done %{public}d...", state);

    DataAbilityRecordPtrMap::iterator it;
    DataAbilityRecordPtr dataAbilityRecord;
    auto record = Token::GetAbilityRecordByToken(token);
    std::string abilityName = "";
    if (record != nullptr) {
        abilityName = record->GetAbilityInfo().name;
    }
    for (it = dataAbilityRecordsLoading_.begin(); it != dataAbilityRecordsLoading_.end(); ++it) {
        if (it->second->GetToken() == token) {
            dataAbilityRecord = it->second;
            break;
        }
    }
    if (!dataAbilityRecord) {
        HILOG_ERROR("Attaching data ability '%{public}s' is not existed.", abilityName.c_str());
        return ERR_UNKNOWN_OBJECT;
    }

    int ret = dataAbilityRecord->OnTransitionDone(state);
    if (ret == ERR_OK) {
        dataAbilityRecordsLoaded_[it->first] = dataAbilityRecord;
        dataAbilityRecordsLoading_.erase(it);
    }

    return ret;
}

void DataAbilityManager::OnAbilityRequestDone(const sptr<IRemoteObject> &token, const int32_t state)
{
    /* Do nothing now. */
}

void DataAbilityManager::OnAbilityDied(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    if (!abilityRecord) {
        HILOG_ERROR("Data ability manager on ability died: null object.");
        return;
    }

    std::lock_guard<std::mutex> locker(mutex_);

    if (DEBUG_ENABLED) {
        DumpLocked(__func__, __LINE__);
    }

    if (abilityRecord->GetAbilityInfo().type == AppExecFwk::AbilityType::DATA) {
        // If 'abilityRecord' is a data ability server, trying to remove it from 'dataAbilityRecords_'.
        for (auto it = dataAbilityRecordsLoaded_.begin(); it != dataAbilityRecordsLoaded_.end(); ++it) {
            if (it->second->GetAbilityRecord() == abilityRecord) {
                it->second->KillBoundClientProcesses();
                HILOG_DEBUG("Removing died data ability record...");
                dataAbilityRecordsLoaded_.erase(it);
                break;
            }
        }
    }

    if (DEBUG_ENABLED) {
        DumpLocked(__func__, __LINE__);
    }

    // If 'abilityRecord' is a data ability client, tring to remove it from all servers.
    for (auto it = dataAbilityRecordsLoaded_.begin(); it != dataAbilityRecordsLoaded_.end(); ++it) {
        it->second->RemoveClients(abilityRecord);
    }

    if (DEBUG_ENABLED) {
        DumpLocked(__func__, __LINE__);
    }
}

std::shared_ptr<AbilityRecord> DataAbilityManager::GetAbilityRecordById(int64_t id)
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    std::lock_guard<std::mutex> locker(mutex_);

    for (auto it = dataAbilityRecordsLoaded_.begin(); it != dataAbilityRecordsLoaded_.end(); ++it) {
        auto abilityRecord = it->second->GetAbilityRecord();
        if (abilityRecord->GetRecordId() == id) {
            return abilityRecord;
        }
    }

    return nullptr;
}

std::shared_ptr<AbilityRecord> DataAbilityManager::GetAbilityRecordByToken(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    if (!token) {
        HILOG_ERROR("Get data ability by token with null object.");
        return nullptr;
    }

    std::lock_guard<std::mutex> locker(mutex_);
    for (auto it = dataAbilityRecordsLoaded_.begin(); it != dataAbilityRecordsLoaded_.end(); ++it) {
        auto abilityRecord = it->second->GetAbilityRecord();
        if (abilityRecord == Token::GetAbilityRecordByToken(token)) {
            return abilityRecord;
        }
    }
    for (auto it = dataAbilityRecordsLoading_.begin(); it != dataAbilityRecordsLoading_.end(); ++it) {
        auto abilityRecord = it->second->GetAbilityRecord();
        if (abilityRecord == Token::GetAbilityRecordByToken(token)) {
            return abilityRecord;
        }
    }
    return nullptr;
}

std::shared_ptr<AbilityRecord> DataAbilityManager::GetAbilityRecordByScheduler(const sptr<IAbilityScheduler> &scheduler)
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    if (!scheduler) {
        HILOG_ERROR("Get data ability by scheduler with null object.");
        return nullptr;
    }

    std::lock_guard<std::mutex> locker(mutex_);

    for (auto it = dataAbilityRecordsLoaded_.begin(); it != dataAbilityRecordsLoaded_.end(); ++it) {
        if (it->second->GetScheduler() != nullptr && it->second->GetScheduler()->AsObject() == scheduler->AsObject()) {
            return it->second->GetAbilityRecord();
        }
    }

    return nullptr;
}

void DataAbilityManager::Dump(const char *func, int line)
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    std::lock_guard<std::mutex> locker(mutex_);

    DumpLocked(func, line);
}

DataAbilityManager::DataAbilityRecordPtr DataAbilityManager::LoadLocked(
    const std::string &name, const AbilityRequest &req)
{
    HILOG_DEBUG("%{public}s(%{public}d) name '%{public}s'", __PRETTY_FUNCTION__, __LINE__, name.c_str());

    DataAbilityRecordPtr dataAbilityRecord;

    auto it = dataAbilityRecordsLoading_.find(name);
    if (it == dataAbilityRecordsLoading_.end()) {
        HILOG_INFO("Acquiring data ability is not in loading, trying to load it...");

        dataAbilityRecord = std::make_shared<DataAbilityRecord>(req);
        if (!dataAbilityRecord) {
            HILOG_ERROR("Failed to allocate data ability record.");
            return nullptr;
        }

        // Start data ability loading process asynchronously.
        int startResult = dataAbilityRecord->StartLoading();
        if (startResult != ERR_OK) {
            HILOG_ERROR("Failed to load data ability %{public}d", startResult);
            return nullptr;
        }

        auto insertResult = dataAbilityRecordsLoading_.insert({name, dataAbilityRecord});
        if (!insertResult.second) {
            HILOG_ERROR("Failed to insert data ability to loading map.");
            return nullptr;
        }
    } else {
        HILOG_INFO("Acquired data ability is loading...");
        dataAbilityRecord = it->second;
    }

    HILOG_INFO("Waiting for data ability loaded...");

    // Waiting for data ability loaded.
    int ret = dataAbilityRecord->WaitForLoaded(mutex_, DATA_ABILITY_LOAD_TIMEOUT);
    if (ret != ERR_OK) {
        HILOG_ERROR("Wait for data ability failed %{public}d.", ret);
        it = dataAbilityRecordsLoading_.find(name);
        if (it != dataAbilityRecordsLoading_.end()) {
            dataAbilityRecordsLoading_.erase(it);
        }
        return nullptr;
    }

    return dataAbilityRecord;
}

void DataAbilityManager::DumpLocked(const char *func, int line)
{
    if (func && line >= 0) {
        HILOG_INFO("Data ability manager dump at %{public}s(%{public}d)", func, line);
    } else {
        HILOG_INFO("Data ability manager dump");
    }

    HILOG_INFO("Available data ability count: %{public}u", dataAbilityRecordsLoaded_.size());

    for (auto it = dataAbilityRecordsLoaded_.begin(); it != dataAbilityRecordsLoaded_.end(); ++it) {
        HILOG_INFO("'%{public}s':", it->first.c_str());
        it->second->Dump();
    }

    HILOG_INFO("Loading data ability count: %{public}u", dataAbilityRecordsLoading_.size());

    for (auto it = dataAbilityRecordsLoading_.begin(); it != dataAbilityRecordsLoading_.end(); ++it) {
        HILOG_INFO("'%{public}s':", it->first.c_str());
        it->second->Dump();
    }
}

void DataAbilityManager::DumpState(std::vector<std::string> &info, const std::string &args) const
{
    if (!args.empty()) {
        auto it = std::find_if(dataAbilityRecordsLoaded_.begin(),
            dataAbilityRecordsLoaded_.end(),
            [&args](const auto &dataAbilityRecord) { return dataAbilityRecord.first.compare(args) == 0; });
        if (it != dataAbilityRecordsLoaded_.end()) {
            info.emplace_back("AbilityName [ " + it->first + " ]");
            it->second->Dump(info);
        } else {
            info.emplace_back(args + ": Nothing to dump.");
        }
    } else {
        info.emplace_back("dataAbilityRecords:");
        for (auto &&dataAbilityRecord : dataAbilityRecordsLoaded_) {
            info.emplace_back("  uri [" + dataAbilityRecord.first + "]");
            dataAbilityRecord.second->Dump(info);
        }
    }
    return;
}
}  // namespace AAFwk
}  // namespace OHOS
