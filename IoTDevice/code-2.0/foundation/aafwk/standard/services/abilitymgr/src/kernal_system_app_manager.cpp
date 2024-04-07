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

#include "kernal_system_app_manager.h"

#include "ability_manager_errors.h"
#include "ability_manager_service.h"
#include "app_scheduler.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {

KernalSystemAppManager::KernalSystemAppManager(int userId) : userId_(userId)
{}

KernalSystemAppManager::~KernalSystemAppManager()
{}

int KernalSystemAppManager::StartAbility(const AbilityRequest &abilityRequest)
{
    HILOG_INFO("start kernal systerm ability.");
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    if (!waittingAbilityQueue_.empty()) {
        HILOG_INFO("waiting queue is not empty, so enqueue systerm ui ability for waiting.");
        EnqueueWaittingAbility(abilityRequest);
        return START_ABILITY_WAITING;
    }

    std::shared_ptr<AbilityRecord> topAbilityRecord = GetCurrentTopAbility();
    auto requestFlag = GetFlagOfAbility(abilityRequest.abilityInfo.bundleName, abilityRequest.abilityInfo.name);
    if (topAbilityRecord != nullptr) {
        auto topFlag =
            GetFlagOfAbility(topAbilityRecord->GetAbilityInfo().bundleName, topAbilityRecord->GetAbilityInfo().name);
        if (topFlag == requestFlag && topAbilityRecord->GetAbilityState() == INITIAL) {
            HILOG_INFO("top systerm ui ability need to restart.");
        }
        if (topAbilityRecord->GetAbilityState() == ACTIVATING) {
            HILOG_INFO("top systerm ui ability is not active, so enqueue ability for waiting.");
            EnqueueWaittingAbility(abilityRequest);
            return START_ABILITY_WAITING;
        }
    }

    return StartAbilityLocked(abilityRequest);
}

int KernalSystemAppManager::StartAbilityLocked(const AbilityRequest &abilityRequest)
{
    std::shared_ptr<AbilityRecord> targetAbility;
    GetOrCreateAbilityRecord(abilityRequest, targetAbility);
    if (targetAbility == nullptr) {
        HILOG_ERROR("failed to get ability record , is nullptr");
        return ERR_INVALID_VALUE;
    }
    targetAbility->SetKernalSystemAbility();

    HILOG_INFO("%{public}s Load kernal system ability, bundleName:%{public}s , abilityName:%{public}s",
        __func__,
        abilityRequest.abilityInfo.bundleName.c_str(),
        abilityRequest.abilityInfo.name.c_str());

    if (targetAbility->GetAbilityState() == AbilityState::ACTIVE ||
        targetAbility->GetAbilityState() == AbilityState::ACTIVATING) {
        HILOG_INFO("kernal system ability is already activing or activated.");
        targetAbility->Activate();
        return ERR_OK;
    }
    return targetAbility->LoadAbility();
}

int KernalSystemAppManager::AttachAbilityThread(
    const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token)
{
    HILOG_INFO("%{public}s.", __func__);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::shared_ptr<AbilityRecord> abilityRecord = GetAbilityRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("abilityRecord is null");
        return ERR_INVALID_VALUE;
    }

    std::string flag = KernalSystemAppManager::GetFlagOfAbility(
        abilityRecord->GetAbilityInfo().bundleName, abilityRecord->GetAbilityInfo().name);
    HILOG_INFO("%{public}s, ability: %{public}s", __func__, flag.c_str());

    auto handler = DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
        return ERR_INVALID_VALUE;
    }
    handler->RemoveEvent(AbilityManagerService::LOAD_TIMEOUT_MSG, abilityRecord->GetEventId());

    abilityRecord->SetScheduler(scheduler);
    DelayedSingleton<AppScheduler>::GetInstance()->MoveToForground(token);

    return ERR_OK;
}

void KernalSystemAppManager::OnAbilityRequestDone(const sptr<IRemoteObject> &token, const int32_t state)
{
    HILOG_INFO("%{public}s", __func__);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    AppAbilityState abilitState = DelayedSingleton<AppScheduler>::GetInstance()->ConvertToAppAbilityState(state);
    if (abilitState == AppAbilityState::ABILITY_STATE_FOREGROUND) {
        std::shared_ptr<AbilityRecord> abilityRecord = GetAbilityRecordByToken(token);
        if (abilityRecord == nullptr) {
            HILOG_ERROR("abilityRecord is null");
            return;
        }
        abilityRecord->Activate();
    }
}

int KernalSystemAppManager::AbilityTransitionDone(const sptr<IRemoteObject> &token, int state)
{
    HILOG_INFO("%{public}s", __func__);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::shared_ptr<AbilityRecord> abilityRecord = GetAbilityRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("abilityRecord is null");
        return ERR_INVALID_VALUE;
    }

    std::string flag = KernalSystemAppManager::GetFlagOfAbility(
        abilityRecord->GetAbilityInfo().bundleName, abilityRecord->GetAbilityInfo().name);
    int targetState = AbilityRecord::ConvertLifeCycleToAbilityState(static_cast<AbilityLifeCycleState>(state));
    std::string abilityState = AbilityRecord::ConvertAbilityState(static_cast<AbilityState>(targetState));
    HILOG_INFO("%{public}s, ability: %{public}s, state: %{public}s", __func__, flag.c_str(), abilityState.c_str());

    switch (targetState) {
        case AbilityState::ACTIVE: {
            return DispatchActive(abilityRecord, targetState);
        }
        default: {
            HILOG_WARN("don't support transiting state: %d", targetState);
            return ERR_INVALID_VALUE;
        }
    }
}

int KernalSystemAppManager::DispatchActive(const std::shared_ptr<AbilityRecord> &abilityRecord, int state)
{
    auto handler = DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr || abilityRecord == nullptr) {
        HILOG_ERROR("event handler or ability record is nullptr.");
        return ERR_INVALID_VALUE;
    }
    if (abilityRecord->GetAbilityState() != AbilityState::ACTIVATING) {
        HILOG_ERROR("kernal ability transition life state error. start:%{public}d", state);
        return ERR_INVALID_VALUE;
    }
    handler->RemoveEvent(AbilityManagerService::ACTIVE_TIMEOUT_MSG, abilityRecord->GetEventId());

    auto task = [kernalManager = shared_from_this(), abilityRecord]() { kernalManager->CompleteActive(abilityRecord); };
    handler->PostTask(task);
    return ERR_OK;
}

void KernalSystemAppManager::CompleteActive(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    HILOG_INFO("%{public}s", __func__);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    abilityRecord->SetAbilityState(AbilityState::ACTIVE);

    auto handler = DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
        return;
    }
    auto task = [kernalManager = shared_from_this()]() { kernalManager->DequeueWaittingAbility(); };
    handler->PostTask(task, "DequeueWaittingAbility");
}

void KernalSystemAppManager::GetOrCreateAbilityRecord(
    const AbilityRequest &abilityRequest, std::shared_ptr<AbilityRecord> &targetAbility)
{
    std::string abilityFlag = KernalSystemAppManager::GetFlagOfAbility(
        abilityRequest.abilityInfo.bundleName, abilityRequest.abilityInfo.name);
    auto isExist = [targetFlag = abilityFlag](const std::shared_ptr<AbilityRecord> &ability) {
        if (ability == nullptr) {
            return false;
        }
        return KernalSystemAppManager::GetFlagOfAbility(
                   ability->GetAbilityInfo().bundleName, ability->GetAbilityInfo().name) == targetFlag;
    };
    auto iter = std::find_if(abilities_.begin(), abilities_.end(), isExist);
    if (iter != abilities_.end()) {
        targetAbility = *iter;
        targetAbility->SetWant(abilityRequest.want);
        targetAbility->SetIsNewWant(true);
        return;
    }
    targetAbility = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilities_.push_front(targetAbility);
}

std::string KernalSystemAppManager::GetFlagOfAbility(const std::string &bundleName, const std::string &abilityName)
{
    return bundleName + ":" + abilityName;
}

int KernalSystemAppManager::GetManagerUserId() const
{
    return userId_;
}

std::shared_ptr<AbilityRecord> KernalSystemAppManager::GetCurrentTopAbility() const
{
    if (abilities_.empty()) {
        return nullptr;
    }
    return abilities_.front();
}

std::shared_ptr<AbilityRecord> KernalSystemAppManager::GetAbilityRecordByToken(const sptr<IRemoteObject> &token)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
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

std::shared_ptr<AbilityRecord> KernalSystemAppManager::GetAbilityRecordByEventId(const int64_t eventId) const
{
    auto isExist = [targetEventId = eventId](const std::shared_ptr<AbilityRecord> &ability) {
        if (ability == nullptr) {
            return false;
        }
        return (ability->GetEventId() == targetEventId);
    };
    auto iter = std::find_if(abilities_.begin(), abilities_.end(), isExist);
    if (iter != abilities_.end()) {
        return *iter;
    }
    return nullptr;
}

bool KernalSystemAppManager::RemoveAbilityRecord(std::shared_ptr<AbilityRecord> ability)
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

void KernalSystemAppManager::EnqueueWaittingAbility(const AbilityRequest &abilityRequest)
{
    waittingAbilityQueue_.push(abilityRequest);
    return;
}

void KernalSystemAppManager::DequeueWaittingAbility()
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::shared_ptr<AbilityRecord> topAbility = GetCurrentTopAbility();
    if (topAbility != nullptr && topAbility->GetAbilityState() != ACTIVE) {
        HILOG_INFO("top ability is not active, must return for waiting again");
        return;
    }
    if (!waittingAbilityQueue_.empty()) {
        AbilityRequest abilityRequest = waittingAbilityQueue_.front();
        waittingAbilityQueue_.pop();
        HILOG_INFO("%{public}s ,bundleName:%{public}s , abilityName:%{public}s",
            __func__,
            abilityRequest.abilityInfo.bundleName.c_str(),
            abilityRequest.abilityInfo.name.c_str());

        StartAbilityLocked(abilityRequest);
    }
}
void KernalSystemAppManager::DumpState(std::vector<std::string> &info)
{
    info.emplace_back("SystemUIRecords:");
    for (auto &ability : abilities_) {
        ability->Dump(info);
    }
}

void KernalSystemAppManager::OnAbilityDied(std::shared_ptr<AbilityRecord> abilityRecord)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    if (!abilityRecord) {
        HILOG_ERROR("System UI on scheduler died, record is nullptr");
        return;
    }
    if (!abilityRecord->IsKernalSystemAbility()) {
        HILOG_ERROR("System UI on scheduler died, ability type is not system ui");
        return;
    }

    if (GetAbilityRecordByToken(abilityRecord->GetToken()) == nullptr) {
        HILOG_ERROR("System UI on scheduler died, record is not exist.");
        return;
    }
    auto ams = DelayedSingleton<AbilityManagerService>::GetInstance();
    if (!ams) {
        HILOG_ERROR("System UI on scheduler died: failed to get ams.");
        return;
    }
    auto handler = ams->GetEventHandler();
    if (!handler) {
        HILOG_ERROR("System UI on scheduler died: failed to get ams handler.");
        return;
    }
    HILOG_INFO("System UI on scheduler died: '%{public}s'", abilityRecord->GetAbilityInfo().name.c_str());
    std::string name = abilityRecord->GetAbilityInfo().name;
    abilityRecord->SetAbilityState(AbilityState::INITIAL);
    auto timeoutTask = [ams, abilityRecord]() {
        if (abilityRecord) {
            ams->StartSystemUi(abilityRecord->GetAbilityInfo().name);
        }
    };
    handler->PostTask(timeoutTask, "SystemUi_Die_" + name, AbilityManagerService::RESTART_TIMEOUT);
}
void KernalSystemAppManager::OnTimeOut(uint32_t msgId, int64_t eventId)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    if (abilities_.empty()) {
        HILOG_ERROR("System UI on time out event: ability stack is empty.");
        return;
    }

    auto abilityRecord = GetAbilityRecordByEventId(eventId);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("System UI on time out event: ability record is nullptr.");
        return;
    }

    auto ams = DelayedSingleton<AbilityManagerService>::GetInstance();
    if (!ams) {
        HILOG_ERROR("System UI on time out event: failed to get ams.");
        return;
    }
    auto handler = ams->GetEventHandler();
    if (!handler) {
        HILOG_ERROR("System UI on time out event: failed to get ams handler.");
        return;
    }

    switch (msgId) {
        case AbilityManagerService::LOAD_TIMEOUT_MSG:
        case AbilityManagerService::ACTIVE_TIMEOUT_MSG: {
            std::string bundleName = abilityRecord->GetAbilityInfo().bundleName;
            std::string name = abilityRecord->GetAbilityInfo().name;
            RemoveAbilityRecord(abilityRecord);
            auto task = [ams, bundleName]() {
                ams->KillProcess(bundleName);
                HILOG_ERROR("System UI on time out event: KillProcess:%{public}s", bundleName.c_str());
            };
            handler->PostTask(task);
            auto timeoutTask = [ams, name]() {
                ams->StartSystemUi(name);
                HILOG_ERROR("System UI on time out event: restart:%{public}s", name.c_str());
            };
            handler->PostTask(timeoutTask, "SystemUi_Timeout_" + name, AbilityManagerService::RESTART_TIMEOUT);
            break;
        }
        default:
            break;
    }
}
}  // namespace AAFwk
}  // namespace OHOS