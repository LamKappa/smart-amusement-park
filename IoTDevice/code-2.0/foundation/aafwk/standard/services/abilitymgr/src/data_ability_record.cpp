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

#include "data_ability_record.h"

#include <algorithm>

#include "app_scheduler.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
DataAbilityRecord::DataAbilityRecord(const AbilityRequest &req) : request_(req)
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    if (request_.abilityInfo.type != AppExecFwk::AbilityType::DATA) {
        HILOG_ERROR("BUG: Construct a data ability with wrong ability type.");
    }
}

DataAbilityRecord::~DataAbilityRecord()
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);
}

int DataAbilityRecord::StartLoading()
{
    HILOG_INFO("Start data ability loading...");

    if (ability_ || scheduler_) {
        HILOG_ERROR("Data ability already started.");
        return ERR_ALREADY_EXISTS;
    }

    if (request_.abilityInfo.type != AppExecFwk::AbilityType::DATA) {
        HILOG_ERROR("Start a data ability with wrong ability type.");
        return ERR_INVALID_VALUE;
    }

    auto ability = AbilityRecord::CreateAbilityRecord(request_);
    if (!ability) {
        HILOG_ERROR("Failed to allocate ability for DataAbilityRecord.");
        return ERR_NO_MEMORY;
    }

    int ret = ability->LoadAbility();
    if (ret != ERR_OK) {
        HILOG_ERROR("Failed to start data ability loading.");
        return ret;
    }

    ability_ = ability;

    // Ability state is 'INITIAL' now.

    return ERR_OK;
}

int DataAbilityRecord::WaitForLoaded(std::mutex &mutex, const std::chrono::system_clock::duration &timeout)
{
    if (!ability_) {
        return ERR_INVALID_STATE;
    }

    // Data ability uses 'ACTIVATE' as loaded state.
    if (ability_->GetAbilityState() == ACTIVE) {
        return ERR_OK;
    }

    auto ret = loadedCond_.wait_for(mutex, timeout, [this] { return ability_->GetAbilityState() == ACTIVE; });

    if (!ret) {
        return ERR_TIMED_OUT;
    }

    if (!scheduler_ || ability_->GetAbilityState() != ACTIVE) {
        return ERR_INVALID_STATE;
    }

    return ERR_OK;
}

sptr<IAbilityScheduler> DataAbilityRecord::GetScheduler()
{
    // Check if data ability is attached.
    if (!ability_ || !scheduler_) {
        return nullptr;
    }

    // Check if data ability is loaded.
    if (ability_->GetAbilityState() != ACTIVE) {
        return nullptr;
    }

    return scheduler_;
}

int DataAbilityRecord::Attach(const sptr<IAbilityScheduler> &scheduler)
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    if (!scheduler) {
        HILOG_ERROR("Attach data ability: invalid scheduler.");
        return ERR_INVALID_DATA;
    }

    if (!ability_) {
        HILOG_ERROR("Data ability attach: not startloading.");
        return ERR_INVALID_STATE;
    }

    if (scheduler_) {
        HILOG_ERROR("Attach data ability: already attached.");
        return ERR_INVALID_STATE;
    }

    // INITIAL => ACTIVATING

    if (ability_->GetAbilityState() != INITIAL) {
        HILOG_ERROR("Attaching data ability: not in 'INITIAL' state.");
        return ERR_INVALID_STATE;
    }

    HILOG_DEBUG("Attaching data ability...");
    ability_->SetScheduler(scheduler);
    scheduler_ = scheduler;

    HILOG_INFO("Scheduling 'OnStart' for data ability '%{public}s|%{public}s'...",
        ability_->GetApplicationInfo().bundleName.c_str(),
        ability_->GetAbilityInfo().name.c_str());

    ability_->SetAbilityState(ACTIVATING);

    LifeCycleStateInfo state;
    state.state = AbilityLifeCycleState::ABILITY_STATE_ACTIVE;

    scheduler->ScheduleAbilityTransaction(ability_->GetWant(), state);

    return ERR_OK;
}

int DataAbilityRecord::OnTransitionDone(int state)
{
    if (!ability_ || !scheduler_) {
        HILOG_ERROR("Data ability on transition done: not attached.");
        return ERR_INVALID_STATE;
    }

    if (ability_->GetAbilityState() != ACTIVATING) {
        HILOG_ERROR("Data ability on transition done: not in 'ACTIVATING' state.");
        return ERR_INVALID_STATE;
    }

    if (state != AbilityLifeCycleState::ABILITY_STATE_ACTIVE) {
        HILOG_ERROR("Data ability on transition done: not ACTIVE.");
        ability_->SetAbilityState(INITIAL);
        loadedCond_.notify_all();
        return ERR_INVALID_STATE;
    }

    // ACTIVATING => ACTIVE(loaded):
    // Set loaded state, data ability uses 'ACTIVE' as loaded state.

    ability_->SetAbilityState(ACTIVE);
    loadedCond_.notify_all();

    HILOG_INFO("Data ability '%{public}s|%{public}s' is loaded.",
        ability_->GetApplicationInfo().bundleName.c_str(),
        ability_->GetAbilityInfo().name.c_str());

    return ERR_OK;
}

int DataAbilityRecord::AddClient(const std::shared_ptr<AbilityRecord> &client, bool tryBind)
{
    HILOG_INFO("Adding data ability client...");

    if (!client) {
        HILOG_ERROR("Data ability add client: invalid param.");
        return ERR_INVALID_STATE;
    }

    if (!ability_ || !scheduler_) {
        HILOG_ERROR("Data ability add client: not attached.");
        return ERR_INVALID_STATE;
    }

    if (ability_->GetAbilityState() != ACTIVE) {
        HILOG_ERROR("Data ability add client: not loaded.");
        return ERR_INVALID_STATE;
    }

    auto appScheduler = DelayedSingleton<AppScheduler>::GetInstance();
    if (!appScheduler) {
        HILOG_ERROR("Data ability add client: failed to get app scheduler.");
        return ERR_NULL_OBJECT;
    }

    // One client can be added multi-times, so 'RemoveClient()' must be called in corresponding times.
    auto &clientInfo = clients_.emplace_back();
    clientInfo.ability = client;
    clientInfo.tryBind = tryBind;

    appScheduler->AbilityBehaviorAnalysis(ability_->GetToken(), client->GetToken(), 0, 0, 1);

    if (clients_.size() == 1) {
        HILOG_INFO("Moving data ability app to foreground...");
        appScheduler->MoveToForground(ability_->GetToken());
    }

    HILOG_INFO("Ability '%{public}s|%{public}s' ----> Data ability '%{public}s|%{public}s'.",
        client->GetApplicationInfo().bundleName.c_str(),
        client->GetAbilityInfo().name.c_str(),
        ability_->GetApplicationInfo().bundleName.c_str(),
        ability_->GetAbilityInfo().name.c_str());

    return ERR_OK;
}

int DataAbilityRecord::RemoveClient(const std::shared_ptr<AbilityRecord> &client)
{
    HILOG_INFO("Removing data ability client...");

    if (!client) {
        HILOG_ERROR("Data ability remove client: invalid client.");
        return ERR_INVALID_STATE;
    }

    if (!ability_ || !scheduler_) {
        HILOG_ERROR("Data ability remove clients: not attached.");
        return ERR_INVALID_STATE;
    }

    if (ability_->GetAbilityState() != ACTIVE) {
        HILOG_ERROR("Data ability remove client: not loaded.");
        return ERR_INVALID_STATE;
    }

    if (clients_.empty()) {
        HILOG_DEBUG("BUG: Data ability record has no clients.");
        return ERR_OK;
    }

    auto appScheduler = DelayedSingleton<AppScheduler>::GetInstance();
    if (!appScheduler) {
        HILOG_ERROR("Data ability record remove client: invalid app scheduler.");
        return ERR_NULL_OBJECT;
    }

    for (auto it(clients_.begin()); it != clients_.end(); ++it) {
        if (it->ability == client) {
            appScheduler->AbilityBehaviorAnalysis(ability_->GetToken(), client->GetToken(), 0, 0, 0);
            clients_.erase(it);
            HILOG_INFO("Ability '%{public}s|%{public}s' --X-> Data ability '%{public}s|%{public}s'.",
                client->GetApplicationInfo().bundleName.c_str(),
                client->GetAbilityInfo().name.c_str(),
                ability_->GetApplicationInfo().bundleName.c_str(),
                ability_->GetAbilityInfo().name.c_str());
            break;
        }
    }

    if (clients_.empty()) {
        HILOG_INFO("Moving data ability to background...");
        appScheduler->MoveToBackground(ability_->GetToken());
    }

    return ERR_OK;
}

int DataAbilityRecord::RemoveClients(const std::shared_ptr<AbilityRecord> &client)
{
    HILOG_DEBUG("%{public}s(%{public}d)", __PRETTY_FUNCTION__, __LINE__);

    if (!ability_ || !scheduler_) {
        HILOG_ERROR("Data ability remove clients: not attached.");
        return ERR_INVALID_STATE;
    }

    if (ability_->GetAbilityState() != ACTIVE) {
        HILOG_ERROR("Data ability remove clients: not loaded.");
        return ERR_INVALID_STATE;
    }

    if (clients_.empty()) {
        HILOG_DEBUG("Data ability remove clients: no clients.");
        return ERR_OK;
    }

    auto appScheduler = DelayedSingleton<AppScheduler>::GetInstance();
    if (!appScheduler) {
        HILOG_ERROR("Data ability remove clients: invalid app scheduler.");
        return ERR_NULL_OBJECT;
    }

    if (client) {
        HILOG_DEBUG("Removing data ability clients with filter...");
        auto it = clients_.begin();
        while (it != clients_.end()) {
            if (it->ability == client) {
                appScheduler->AbilityBehaviorAnalysis(ability_->GetToken(), client->GetToken(), 0, 0, 0);
                it = clients_.erase(it);
                HILOG_INFO("Ability '%{public}s|%{public}s' --X-> Data ability '%{public}s|%{public}s'.",
                    client->GetApplicationInfo().bundleName.c_str(),
                    client->GetAbilityInfo().name.c_str(),
                    ability_->GetApplicationInfo().bundleName.c_str(),
                    ability_->GetAbilityInfo().name.c_str());
            } else {
                ++it;
            }
        }
    } else {
        HILOG_DEBUG("Removing data ability clients...");
        while (!clients_.empty()) {
            auto tmpClient = clients_.front().ability;
            appScheduler->AbilityBehaviorAnalysis(ability_->GetToken(), tmpClient->GetToken(), 0, 0, 0);
            clients_.pop_front();
            HILOG_INFO("Ability '%{public}s|%{public}s' --X-> Data ability '%{public}s|%{public}s'.",
                tmpClient->GetApplicationInfo().bundleName.c_str(),
                tmpClient->GetAbilityInfo().name.c_str(),
                ability_->GetApplicationInfo().bundleName.c_str(),
                ability_->GetAbilityInfo().name.c_str());
        }
    }

    if (clients_.empty()) {
        HILOG_INFO("Moving data ability to background...");
        appScheduler->MoveToBackground(ability_->GetToken());
    }

    return ERR_OK;
}

size_t DataAbilityRecord::GetClientCount(const std::shared_ptr<AbilityRecord> &client) const
{
    if (!ability_ || !scheduler_) {
        HILOG_ERROR("Data ability get client count: not attached.");
        return 0;
    }

    if (ability_->GetAbilityState() != ACTIVE) {
        HILOG_ERROR("Data ability get client count: not loaded.");
        return 0;
    }

    if (client) {
        return std::count_if(
            clients_.begin(), clients_.end(), [client](const ClientInfo &ci) { return ci.ability == client; });
    }

    return clients_.size();
}

int DataAbilityRecord::KillBoundClientProcesses()
{
    if (!ability_ || !scheduler_) {
        HILOG_ERROR("Data ability kill bound clients: not attached.");
        return ERR_INVALID_STATE;
    }

    if (ability_->GetAbilityState() != ACTIVE) {
        HILOG_ERROR("Data ability kill bound clients: not loaded.");
        return ERR_INVALID_STATE;
    }

    auto appScheduler = DelayedSingleton<AppScheduler>::GetInstance();
    if (!appScheduler) {
        HILOG_ERROR("Data ability kill bound clients: invalid app scheduler.");
        return ERR_INVALID_STATE;
    }

    for (auto it = clients_.begin(); it != clients_.end(); ++it) {
        if (it->tryBind) {
            HILOG_INFO("Killing bound client '%{public}s|%{public}s' of data ability '%{public}s|%{public}s'...",
                it->ability->GetApplicationInfo().bundleName.c_str(),
                it->ability->GetAbilityInfo().name.c_str(),
                ability_->GetApplicationInfo().bundleName.c_str(),
                ability_->GetAbilityInfo().name.c_str());
            appScheduler->KillProcessByAbilityToken(it->ability->GetToken());
        }
    }

    return ERR_OK;
}

const AbilityRequest &DataAbilityRecord::GetRequest() const
{
    return request_;
}

std::shared_ptr<AbilityRecord> DataAbilityRecord::GetAbilityRecord()
{
    return ability_;
}

sptr<IRemoteObject> DataAbilityRecord::GetToken()
{
    if (!ability_) {
        return nullptr;
    }

    return ability_->GetToken();
}

void DataAbilityRecord::Dump() const
{
    if (!ability_) {
        HILOG_INFO("DataAbility <null>");
        return;
    }

    HILOG_INFO("attached: %{public}s, clients: %{public}u, refcnt: %{public}d, state: %{public}s",
        scheduler_ ? "true" : "false",
        clients_.size(),
        scheduler_ ? scheduler_->GetSptrRefCount() : 0,
        AbilityRecord::ConvertAbilityState(ability_->GetAbilityState()).c_str());

    int i = 0;

    for (auto it = clients_.begin(); it != clients_.end(); ++it) {
        auto record = it->ability;
        HILOG_INFO("  %{public}2d '%{public}s|%{public}s' - tryBind: %{public}s",
            i++,
            record->GetApplicationInfo().bundleName.c_str(),
            record->GetAbilityInfo().name.c_str(),
            it->tryBind ? "true" : "false");
    }
}

void DataAbilityRecord::Dump(std::vector<std::string> &info) const
{
    if (!ability_) {
        return;
    }
    info.emplace_back("    AbilityRecord ID #" + std::to_string(ability_->GetRecordId()) + "   state #" +
                      AbilityRecord::ConvertAbilityState(ability_->GetAbilityState()) + "   start time [" +
                      std::to_string(ability_->GetStartTime()) + "]");
    info.emplace_back("    main name [" + ability_->GetAbilityInfo().name + "]");
    info.emplace_back("    bundle name [" + ability_->GetAbilityInfo().bundleName + "]");
    info.emplace_back("    ability type [DATA]");
    info.emplace_back("    Clients: " + std::to_string(clients_.size()));

    for (auto &&client : clients_) {
        info.emplace_back("     > " + client.ability->GetAbilityInfo().bundleName + "/" +
                          client.ability->GetAbilityInfo().name + "  tryBind #" + (client.tryBind ? "true" : "false"));
    }
}
}  // namespace AAFwk
}  // namespace OHOS
