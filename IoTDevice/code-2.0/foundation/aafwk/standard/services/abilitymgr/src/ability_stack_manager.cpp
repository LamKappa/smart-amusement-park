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

#include "ability_stack_manager.h"

#include <singleton.h>

#include "ability_manager_errors.h"
#include "ability_manager_service.h"
#include "app_scheduler.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
AbilityStackManager::AbilityStackManager(int userId) : userId_(userId)
{}

void AbilityStackManager::Init()
{
    launcherMissionStack_ = std::make_shared<MissionStack>(LAUNCHER_MISSION_STACK_ID, userId_);
    missionStackList_.push_back(launcherMissionStack_);
    defaultMissionStack_ = std::make_shared<MissionStack>(DEFAULT_MISSION_STACK_ID, userId_);
    missionStackList_.push_back(defaultMissionStack_);
    currentMissionStack_ = launcherMissionStack_;
}

AbilityStackManager::~AbilityStackManager()
{}

int AbilityStackManager::StartAbility(const AbilityRequest &abilityRequest)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    if (!waittingAbilityQueue_.empty()) {
        HILOG_INFO("waiting queue is not empty, so enqueue ability for waiting.");
        EnqueueWaittingAbility(abilityRequest);
        return START_ABILITY_WAITING;
    }
    std::shared_ptr<AbilityRecord> currentTopAbilityRecord = GetCurrentTopAbility();
    if (currentTopAbilityRecord != nullptr) {
        std::string element = currentTopAbilityRecord->GetWant().GetElement().GetURI();
        HILOG_DEBUG("%s, current top %s", __func__, element.c_str());
        if (currentTopAbilityRecord->GetAbilityState() != ACTIVE) {
            HILOG_INFO("Top ability is not active, so enqueue ability for waiting.");
            EnqueueWaittingAbility(abilityRequest);
            return START_ABILITY_WAITING;
        }
    }
    return StartAbilityLocked(currentTopAbilityRecord, abilityRequest);
}

int AbilityStackManager::StartAbilityLocked(
    const std::shared_ptr<AbilityRecord> &currentTopAbility, const AbilityRequest &abilityRequest)
{
    if (!currentMissionStack_) {
        HILOG_ERROR("currentMissionStack_ is nullptr");
        return INNER_ERR;
    }
    lastMissionStack_ = currentMissionStack_;
    // 1. choose target mission stack
    std::shared_ptr<MissionStack> stack = GetTargetMissionStack(abilityRequest);
    if (stack == nullptr) {
        HILOG_ERROR("stack is nullptr");
        return CREATE_MISSION_STACK_FAILED;
    }
    // 2. move target mission stack to top
    MoveMissionStackToTop(stack);
    // 3. get mission record and ability recode
    std::shared_ptr<AbilityRecord> targetAbilityRecord;
    std::shared_ptr<MissionRecord> targetMissionRecord;
    GetMissionRecordAndAbilityRecord(abilityRequest, currentTopAbility, targetAbilityRecord, targetMissionRecord);
    if (targetAbilityRecord == nullptr || targetMissionRecord == nullptr) {
        HILOG_ERROR("failed to get ability record or mission record");
        MoveMissionStackToTop(lastMissionStack_);
        return ERR_INVALID_VALUE;
    }
    // 4. set relationship of mission record and ability record
    if (currentTopAbility != nullptr) {
        targetAbilityRecord->SetPreAbilityRecord(currentTopAbility);
        currentTopAbility->SetNextAbilityRecord(targetAbilityRecord);
        targetMissionRecord->SetPreMissionRecord(currentTopAbility->GetMissionRecord());
        if (currentTopAbility->IsLauncherAbility()) {
            targetMissionRecord->SetIsLauncherCreate();
        }
    }

    // add caller record
    targetAbilityRecord->AddCallerRecord(abilityRequest.callerToken, abilityRequest.requestCode);

    targetAbilityRecord->SetMissionRecord(targetMissionRecord);
    // add ability record to mission record.
    // if this ability record exist this mission record, do not add.
    targetMissionRecord->AddAbilityRecordToTop(targetAbilityRecord);
    // add mission record to mission stack.
    // if this mission record exist this mission stack, do not add.
    currentMissionStack_->AddMissionRecordToTop(targetMissionRecord);
    // move mission record to top
    // if this mission exist at top, do not move.
    currentMissionStack_->MoveMissionRecordToTop(targetMissionRecord);

    // 5. load ability or inactive top ability
    // If top ability is null, then launch the first Ability.
    // If top ability is not null,. then inactive current top ability
    int result = ERR_OK;
    if (currentTopAbility == nullptr) {
        targetAbilityRecord->SetLauncherRoot();
        result = targetAbilityRecord->LoadAbility();
    } else {
        currentTopAbility->Inactivate();
    }
    return result;
}

int AbilityStackManager::TerminateAbility(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::shared_ptr<AbilityRecord> abilityRecord = Token::GetAbilityRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("token is invalid");
        return ERR_INVALID_VALUE;
    }
    // if ability was already in terminate list, don't do anything but wait.
    if (abilityRecord->IsTerminating()) {
        HILOG_INFO("ability is on terminating");
        return ERR_OK;
    }
    // check if ability is in stack to avoid user create fake token.
    if (GetAbilityRecordByToken(token) == nullptr) {
        HILOG_ERROR("ability is not in stack, nor in terminating list.");
        return INNER_ERR;
    }
    std::shared_ptr<MissionRecord> missionRecord = abilityRecord->GetMissionRecord();
    if (missionRecord == nullptr) {
        HILOG_ERROR("missionRecord is nullptr");
        return INNER_ERR;
    }
    if (abilityRecord->IsLauncherAbility() && abilityRecord->IsLauncherRoot()) {
        HILOG_INFO("Don't allow terminate root launcher");
        return TERMINATE_LAUNCHER_DENIED;
    }
    abilityRecord->SetTerminatingState();
    return TerminateAbilityLocked(abilityRecord, resultCode, resultWant);
}

int AbilityStackManager::TerminateAbility(const std::shared_ptr<AbilityRecord> &caller, int requestCode)
{
    HILOG_INFO("%{public}s, called", __func__);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);

    std::shared_ptr<AbilityRecord> targetAbility = nullptr;
    for (auto &stack : missionStackList_) {
        targetAbility = stack->GetAbilityRecordByCaller(caller, requestCode);
        if (targetAbility) {
            break;
        }
    }

    if (!targetAbility) {
        HILOG_ERROR("%{public}s, Can't find target ability", __func__);
        return INNER_ERR;
    }

    return TerminateAbilityLocked(targetAbility, -1, nullptr);
}

int AbilityStackManager::TerminateAbilityLocked(
    const std::shared_ptr<AbilityRecord> &abilityRecord, int resultCode, const Want *resultWant)
{
    HILOG_INFO("%{public}s, called", __func__);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("abilityRecord is invalid");
        return ERR_INVALID_VALUE;
    }
    // save result to caller AbilityRecord
    if (resultWant != nullptr) {
        abilityRecord->SaveResultToCallers(resultCode, resultWant);
    }
    // common case, ability terminate at active and at top position
    int abilityState = abilityRecord->GetAbilityState();
    if (abilityRecord == GetCurrentTopAbility()) {
        HILOG_DEBUG("terminate top ability %d", abilityState);
        // if ability is onActive or on activating state, we must activate next top ability.
        if (abilityState == AbilityState::ACTIVE || abilityState == AbilityState::ACTIVATING ||
            abilityState == AbilityState::INITIAL) {
            RemoveTerminatingAbility(abilityRecord);
            abilityRecord->Inactivate();
            return ERR_OK;
        }
    }
    // it's not common case when ability terminate at non-active state and non-top position.
    if (abilityState == AbilityState::INACTIVE) {
        // ability on inactive, remove AbilityRecord out of stack and then schedule to background.
        RemoveTerminatingAbility(abilityRecord);
        abilityRecord->SendResultToCallers();
        MoveToBackgroundTask(abilityRecord);
    } else if (abilityState == AbilityState::BACKGROUND) {
        // ability on background, remove AbilityRecord out of stack and then schedule to terminate.
        RemoveTerminatingAbility(abilityRecord);
        abilityRecord->SendResultToCallers();
        auto task = [abilityRecord, stackManager = shared_from_this()]() {
            HILOG_WARN("disconnect ability terminate timeout.");
            stackManager->CompleteTerminate(abilityRecord);
        };
        abilityRecord->Terminate(task);
    } else if (abilityState == AbilityState::INACTIVATING || abilityState == AbilityState::MOVING_BACKGROUND) {
        // ability on inactivating or moving to background.
        // remove AbilityRecord out of stack and waiting for ability(kit) AbilityTransitionDone.
        RemoveTerminatingAbility(abilityRecord);
        abilityRecord->SendResultToCallers();
    } else {
        HILOG_WARN("abilityState is invalid");
    }
    return ERR_OK;
}

int AbilityStackManager::RemoveMissionById(int missionId)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    if (missionId < 0) {
        HILOG_ERROR("missionId is invalid");
        return ERR_INVALID_VALUE;
    }
    return RemoveMissionByIdLocked(missionId);
}

int AbilityStackManager::RemoveMissionByIdLocked(int missionId)
{
    if (defaultMissionStack_ == nullptr) {
        HILOG_ERROR("defaultMissionStack_ is invalid");
        return ERR_NO_INIT;
    }
    auto missionRecord = GetMissionRecordFromAllStacks(missionId);
    if (missionRecord == nullptr) {
        HILOG_ERROR("missionId is invalid");
        return REMOVE_MISSION_ID_NOT_EXIST;
    }
    if (IsLauncherMission(missionId)) {
        HILOG_ERROR("don't allow to terminate mission which has launcher ability");
        return REMOVE_MISSION_LAUNCHER_DENIED;
    }

    std::vector<AbilityRecordInfo> abilityInfos;
    missionRecord->GetAllAbilityInfo(abilityInfos);
    // If the mission has the ability to be in active, activing, or inactive-visible state,
    // don't allow to remove this mission.
    for (auto &ability : abilityInfos) {
        auto abilityRecord = missionRecord->GetAbilityRecordById(ability.id);
        if (abilityRecord != nullptr) {
            if (abilityRecord == GetCurrentTopAbility() || abilityRecord->GetAbilityState() == AbilityState::ACTIVE ||
                abilityRecord->GetAbilityState() == AbilityState::ACTIVATING) {
                return REMOVE_MISSION_ACTIVE_DENIED;
            }
            auto windowInfo = abilityRecord->GetWindowInfo();
            if (windowInfo != nullptr && windowInfo->isVisible_ &&
                abilityRecord->GetAbilityState() == AbilityState::INACTIVE) {
                return REMOVE_MISSION_ACTIVE_DENIED;
            }
        }
    }

    for (auto &ability : abilityInfos) {
        auto abilityRecord = missionRecord->GetAbilityRecordById(ability.id);
        if (abilityRecord == nullptr || abilityRecord->IsTerminating()) {
            HILOG_WARN("ability record is not exist or is on terminating");
            continue;
        }

        if (abilityRecord->GetAbilityState() == AbilityState::INITIAL) {
            HILOG_INFO("ability record state is INITIAL, remove ability, continue");
            missionRecord->RemoveAbilityRecord(abilityRecord);
            continue;
        }

        abilityRecord->SetTerminatingState();
        int ret = TerminateAbilityLocked(abilityRecord, -1, nullptr);
        if (ret != ERR_OK) {
            HILOG_ERROR("remove mission error: %{public}d.", ret);
            return REMOVE_MISSION_FAILED;
        }
    }
    return ERR_OK;
}

int AbilityStackManager::RemoveStack(int stackId)
{
    HILOG_DEBUG("AbilityStackManager::RemoveStack, stackId : %{public}d", stackId);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    if (stackId < 0) {
        HILOG_ERROR("stackId is invalid");
        return ERR_INVALID_VALUE;
    }

    return RemoveStackLocked(stackId);
}

int AbilityStackManager::RemoveStackLocked(int stackId)
{
    HILOG_DEBUG("AbilityStackManager::RemoveStackLocked, stackId : %{public}d", stackId);

    if (missionStackList_.empty()) {
        HILOG_ERROR("mission stack list is empty");
        return MISSION_STACK_LIST_IS_EMPTY;
    }

    auto isExist = [stackId](
                       const std::shared_ptr<MissionStack> &stack) { return stackId == stack->GetMissionStackId(); };
    auto iter = std::find_if(missionStackList_.begin(), missionStackList_.end(), isExist);
    if (iter == missionStackList_.end()) {
        HILOG_ERROR("remove stack id is not exist");
        return REMOVE_STACK_ID_NOT_EXIST;
    }

    if (*iter == launcherMissionStack_) {
        HILOG_ERROR("don't allow remove luncher mission stack");
        return REMOVE_STACK_LAUNCHER_DENIED;
    }

    if (*iter == defaultMissionStack_) {
        std::vector<MissionRecordInfo> missionInfos;
        defaultMissionStack_->GetAllMissionInfo(missionInfos);
        for (auto &mission : missionInfos) {
            int result = RemoveMissionByIdLocked(mission.id);
            if (result != ERR_OK) {
                HILOG_ERROR("remove mission failed, mission id : %{public}d", mission.id);
                return result;
            }
        }
    }

    return ERR_OK;
}

/**
 * remove AbilityRecord from stack to terminate list.
 * update MissionStack to prepare next top ability.
 */
void AbilityStackManager::RemoveTerminatingAbility(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    if (!abilityRecord) {
        HILOG_ERROR("%{public}s, abilityRecord is nullptr", __func__);
        return;
    }
    std::shared_ptr<MissionRecord> missionRecord = abilityRecord->GetMissionRecord();
    if (!missionRecord) {
        HILOG_ERROR("%{public}s, missionRecord is nullptr", __func__);
        return;
    }

    auto abilityState = abilityRecord->GetAbilityState();
    auto isActive = abilityState == AbilityState::ACTIVE || abilityState == AbilityState::ACTIVATING ||
                    abilityState == AbilityState::INITIAL;

    missionRecord->RemoveAbilityRecord(abilityRecord);
    terminateAbilityRecordList_.push_back(abilityRecord);
    // if MissionRecord is empty, remove it and update missionStack by caller.
    if (missionRecord->GetAbilityRecordCount() == 0) {
        auto isExist = (!missionRecord->IsLauncherCreate() && missionRecord->GetPreMissionRecord() != nullptr &&
                        launcherMissionStack_->IsExistMissionRecord(
                            missionRecord->GetPreMissionRecord()->GetMissionRecordId()) &&
                        isActive);
        if ((missionRecord->IsLauncherCreate() && isActive) ||
            (missionRecord == missionStackList_.back()->GetTopMissionRecord()) || isExist) {
            RemoveMissionRecordById(missionRecord->GetMissionRecordId());
            MoveMissionStackToTop(launcherMissionStack_);
        } else {
            RemoveMissionRecordById(missionRecord->GetMissionRecordId());
        }
    }
}

int AbilityStackManager::GetAbilityStackManagerUserId() const
{
    return userId_;
}

std::shared_ptr<AbilityRecord> AbilityStackManager::GetCurrentTopAbility() const
{
    std::shared_ptr<MissionStack> topMissionStack = missionStackList_.front();
    HILOG_DEBUG("Top Mission id is %{public}d", topMissionStack->GetMissionStackId());
    return topMissionStack->GetTopAbilityRecord();
}

sptr<Token> AbilityStackManager::GetCurrentTopAbilityToken()
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    sptr<Token> token = nullptr;
    std::shared_ptr<MissionStack> topMissionStack = missionStackList_.front();
    std::shared_ptr<AbilityRecord> abilityRecord = topMissionStack->GetTopAbilityRecord();
    if (abilityRecord != nullptr) {
        token = abilityRecord->GetToken();
    }
    return token;
}

std::shared_ptr<AbilityRecord> AbilityStackManager::GetAbilityRecordById(const int64_t recordId)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    for (auto missionStack : missionStackList_) {
        std::shared_ptr<AbilityRecord> abilityRecord = missionStack->GetAbilityRecordById(recordId);
        if (abilityRecord != nullptr) {
            return abilityRecord;
        }
    }
    return nullptr;
}

std::shared_ptr<MissionRecord> AbilityStackManager::GetTopMissionRecord() const
{
    if (currentMissionStack_ == nullptr) {
        HILOG_ERROR("currentMissionStack_ is nullptr");
        return nullptr;
    }
    return currentMissionStack_->GetTopMissionRecord();
}

std::shared_ptr<MissionRecord> AbilityStackManager::GetMissionRecordById(int id) const
{
    if (currentMissionStack_ == nullptr) {
        HILOG_ERROR("currentMissionStack_ is nullptr");
        return nullptr;
    }
    return currentMissionStack_->GetMissionRecordById(id);
}

std::shared_ptr<MissionRecord> AbilityStackManager::GetMissionRecordFromAllStacks(int id) const
{
    for (auto missionStack : missionStackList_) {
        std::shared_ptr<MissionRecord> missionRecord = missionStack->GetMissionRecordById(id);
        if (missionRecord != nullptr) {
            return missionRecord;
        }
    }
    return nullptr;
}

std::shared_ptr<AbilityRecord> AbilityStackManager::GetAbilityRecordByToken(const sptr<IRemoteObject> &token)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    for (auto missionStack : missionStackList_) {
        std::shared_ptr<AbilityRecord> abilityRecord = missionStack->GetAbilityRecordByToken(token);
        if (abilityRecord != nullptr) {
            return abilityRecord;
        }
    }
    return nullptr;
}

std::shared_ptr<AbilityRecord> AbilityStackManager::GetAbilityFromTerminateList(const sptr<IRemoteObject> &token)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    for (auto abilityRecord : terminateAbilityRecordList_) {
        // token is type of IRemoteObject, abilityRecord->GetToken() is type of Token extending from IRemoteObject.
        if (abilityRecord && token == abilityRecord->GetToken()->AsObject()) {
            return abilityRecord;
        }
    }
    return nullptr;
}

bool AbilityStackManager::RemoveMissionRecordById(int id)
{
    for (auto missionstack : missionStackList_) {
        if (missionstack->RemoveMissionRecord(id)) {
            return true;
        }
    }
    return false;
}

void AbilityStackManager::MoveMissionStackToTop(const std::shared_ptr<MissionStack> &stack)
{
    if (stack == nullptr) {
        HILOG_ERROR("stack is nullptr");
        return;
    }
    if (missionStackList_.front() == stack) {
        HILOG_DEBUG("stack is at the top of list, mission id: %d", stack->GetMissionStackId());
        return;
    }
    missionStackList_.remove(stack);
    missionStackList_.push_front(stack);
    currentMissionStack_ = stack;
}

std::shared_ptr<MissionStack> AbilityStackManager::GetTargetMissionStack(const AbilityRequest &abilityRequest)
{
    if (abilityRequest.abilityInfo.applicationInfo.isLauncherApp) {
        return launcherMissionStack_;
    }
    return defaultMissionStack_;
}

int AbilityStackManager::AttachAbilityThread(const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::shared_ptr<AbilityRecord> abilityRecord = GetAbilityRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("abilityRecord is null");
        return ERR_INVALID_VALUE;
    }
    std::string element = abilityRecord->GetWant().GetElement().GetURI();
    HILOG_DEBUG("%s, ability: %s", __func__, element.c_str());

    std::shared_ptr<AbilityEventHandler> handler =
        DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
        return ERR_INVALID_VALUE;
    }
    handler->RemoveEvent(AbilityManagerService::LOAD_TIMEOUT_MSG, abilityRecord->GetEventId());

    abilityRecord->SetScheduler(scheduler);
    DelayedSingleton<AppScheduler>::GetInstance()->MoveToForground(token);

    return ERR_OK;
}

int AbilityStackManager::AbilityTransitionDone(const sptr<IRemoteObject> &token, int state)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::shared_ptr<AbilityRecord> abilityRecord = GetAbilityRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_INFO("abilityRecord may in terminate list");
        abilityRecord = GetAbilityFromTerminateList(token);
        if (abilityRecord == nullptr) {
            HILOG_ERROR("abilityRecord is null");
            return ERR_INVALID_VALUE;
        }
    }

    std::string element = abilityRecord->GetWant().GetElement().GetURI();
    int targetState = AbilityRecord::ConvertLifeCycleToAbilityState(static_cast<AbilityLifeCycleState>(state));
    std::string abilityState = AbilityRecord::ConvertAbilityState(static_cast<AbilityState>(targetState));
    HILOG_INFO("%{public}s, ability: %{public}s, state: %{public}s", __func__, element.c_str(), abilityState.c_str());

    return DispatchState(abilityRecord, targetState);
}

int AbilityStackManager::DispatchState(const std::shared_ptr<AbilityRecord> &abilityRecord, int state)
{
    HILOG_DEBUG("%{public}s", __func__);
    switch (state) {
        case AbilityState::ACTIVE: {
            return DispatchActive(abilityRecord, state);
        }
        case AbilityState::INACTIVE: {
            return DispatchInactive(abilityRecord, state);
        }
        case AbilityState::BACKGROUND: {
            return DispatchBackground(abilityRecord, state);
        }
        case AbilityState::INITIAL: {
            return DispatchTerminate(abilityRecord, state);
        }
        default: {
            HILOG_WARN("don't support transiting state: %d", state);
            return ERR_INVALID_VALUE;
        }
    }
}

int AbilityStackManager::DispatchActive(const std::shared_ptr<AbilityRecord> &abilityRecord, int state)
{
    std::shared_ptr<AbilityEventHandler> handler =
        DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
        return ERR_INVALID_VALUE;
    }
    if (abilityRecord == nullptr) {
        return ERR_INVALID_VALUE;
    }
    if (abilityRecord->GetAbilityState() != AbilityState::ACTIVATING) {
        HILOG_ERROR("ability transition life state error. expect %{public}d, actual %{public}d callback %{public}d",
            AbilityState::ACTIVATING,
            abilityRecord->GetAbilityState(),
            state);
        return ERR_INVALID_VALUE;
    }
    handler->RemoveEvent(AbilityManagerService::ACTIVE_TIMEOUT_MSG, abilityRecord->GetEventId());
    auto task = [stackManager = shared_from_this(), abilityRecord]() { stackManager->CompleteActive(abilityRecord); };
    handler->PostTask(task);
    return ERR_OK;
}

int AbilityStackManager::DispatchInactive(const std::shared_ptr<AbilityRecord> &abilityRecord, int state)
{
    std::shared_ptr<AbilityEventHandler> handler =
        DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
        return ERR_INVALID_VALUE;
    }
    if (abilityRecord == nullptr) {
        return ERR_INVALID_VALUE;
    }
    if (abilityRecord->GetAbilityState() != AbilityState::INACTIVATING) {
        HILOG_ERROR("ability transition life state error. expect %{public}d, actual %{public}d callback %{public}d",
            AbilityState::INACTIVATING,
            abilityRecord->GetAbilityState(),
            state);
        return ERR_INVALID_VALUE;
    }
    handler->RemoveEvent(AbilityManagerService::INACTIVE_TIMEOUT_MSG, abilityRecord->GetEventId());
    auto task = [stackManager = shared_from_this(), abilityRecord]() { stackManager->CompleteInactive(abilityRecord); };
    handler->PostTask(task);
    return ERR_OK;
}

int AbilityStackManager::DispatchBackground(const std::shared_ptr<AbilityRecord> &abilityRecord, int state)
{
    std::shared_ptr<AbilityEventHandler> handler =
        DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
        return ERR_INVALID_VALUE;
    }
    if (abilityRecord->GetAbilityState() != AbilityState::MOVING_BACKGROUND) {
        HILOG_ERROR("ability transition life state error. expect %{public}d, actual %{public}d callback %{public}d",
            AbilityState::MOVING_BACKGROUND,
            abilityRecord->GetAbilityState(),
            state);
        return ERR_INVALID_VALUE;
    }
    // remove background timeout task.
    handler->RemoveTask(std::to_string(abilityRecord->GetEventId()));
    auto task = [stackManager = shared_from_this(), abilityRecord]() {
        stackManager->CompleteBackground(abilityRecord);
    };
    handler->PostTask(task);
    return ERR_OK;
}

int AbilityStackManager::DispatchTerminate(const std::shared_ptr<AbilityRecord> &abilityRecord, int state)
{
    std::shared_ptr<AbilityEventHandler> handler =
        DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
        return INNER_ERR;
    }
    if (abilityRecord->GetAbilityState() != AbilityState::TERMINATING) {
        HILOG_ERROR("ability transition life state error. expect %{public}d, actual %{public}d callback %{public}d",
            AbilityState::TERMINATING,
            abilityRecord->GetAbilityState(),
            state);
        return INNER_ERR;
    }
    // remove terminate timeout task.
    handler->RemoveTask(std::to_string(abilityRecord->GetEventId()));
    auto task = [stackManager = shared_from_this(), abilityRecord]() {
        stackManager->CompleteTerminate(abilityRecord);
    };
    handler->PostTask(task);
    return ERR_OK;
}

void AbilityStackManager::AddWindowInfo(const sptr<IRemoteObject> &token, int32_t windowToken)
{
    HILOG_DEBUG("add window id.");
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    // create WindowInfo and add to its AbilityRecord
    std::shared_ptr<AbilityRecord> abilityRecord = GetAbilityRecordByToken(token);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("abilityRecord is null. unable to add window info");
        return;
    }
    if (abilityRecord->GetWindowInfo() != nullptr) {
        HILOG_WARN("WindowInfo is already added. Can't add again.");
        return;
    }
    int abilityState = abilityRecord->GetAbilityState();
    if (abilityState != ACTIVATING) {
        HILOG_INFO("add windowInfo at state: %{public}d.", abilityState);
    }
    if (windowTokenToAbilityMap_[windowToken] != nullptr) {
        // It shouldn't happen. Possible reasons for this case:
        // 1. windowmanager generates same window token.
        // 2. abilityms doesn't destroy ability in terminate process.
        HILOG_ERROR("window token has been added to other AbilityRecord. ability name: %{private}s",
            abilityRecord->GetAbilityInfo().name.c_str());
    } else {
        abilityRecord->AddWindowInfo(windowToken);
        windowTokenToAbilityMap_[windowToken] = abilityRecord;
    }
}

void AbilityStackManager::OnAbilityRequestDone(const sptr<IRemoteObject> &token, const int32_t state)
{
    HILOG_DEBUG("ability request app state %{public}d done", state);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    AppAbilityState abilitState = DelayedSingleton<AppScheduler>::GetInstance()->ConvertToAppAbilityState(state);
    if (abilitState == AppAbilityState::ABILITY_STATE_FOREGROUND) {
        std::shared_ptr<AbilityRecord> abilityRecord = GetAbilityRecordByToken(token);
        if (abilityRecord == nullptr) {
            HILOG_ERROR("abilityRecord is null");
            return;
        }
        std::string element = abilityRecord->GetWant().GetElement().GetURI();
        HILOG_DEBUG("%{public}s, ability: %{public}s", __func__, element.c_str());
        abilityRecord->Activate();
    }
}

void AbilityStackManager::CompleteActive(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    if (!abilityRecord) {
        HILOG_ERROR("%{public}s, abilityRecord is nullptr", __func__);
        return;
    }
    std::string element = abilityRecord->GetWant().GetElement().GetURI();
    HILOG_INFO("%{public}s, ability: %{public}s", __func__, element.c_str());

    abilityRecord->SetAbilityState(AbilityState::ACTIVE);

    std::shared_ptr<AbilityEventHandler> handler =
        DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
        return;
    }
    /* PostTask to trigger start Ability from waiting queue */
    auto startWaittingAbilityTask = [stackManager = shared_from_this()]() { stackManager->StartWaittingAbility(); };
    handler->PostTask(startWaittingAbilityTask, "startWaittingAbility");

    // 1. preAbility must be inactive when start ability.
    // move preAbility to background only if it was inactive.
    std::shared_ptr<AbilityRecord> preAbilityRecord = abilityRecord->GetPreAbilityRecord();
    if (preAbilityRecord != nullptr && preAbilityRecord->GetAbilityState() == AbilityState::INACTIVE &&
        abilityRecord->GetAbilityInfo().name != AbilityConfig::SYSTEM_DIALOG_NAME &&
        abilityRecord->GetAbilityInfo().bundleName != AbilityConfig::SYSTEM_UI_BUNDLE_NAME) {
        std::string preElement = preAbilityRecord->GetWant().GetElement().GetURI();
        HILOG_INFO("%{public}s, pre ability record: %{public}s", __func__, preElement.c_str());
        // preAbility was inactive ,resume new want flag to false
        MoveToBackgroundTask(preAbilityRecord);
    }

    // 2. nextAbility was in terminate list when terminate ability.
    // should move to background and then terminate.
    std::shared_ptr<AbilityRecord> nextAbilityRecord = abilityRecord->GetNextAbilityRecord();
    if (nextAbilityRecord != nullptr && nextAbilityRecord->GetAbilityState() == AbilityState::INACTIVE &&
        nextAbilityRecord->IsTerminating() &&
        nextAbilityRecord->GetAbilityInfo().name != AbilityConfig::SYSTEM_DIALOG_NAME &&
        nextAbilityRecord->GetAbilityInfo().bundleName != AbilityConfig::SYSTEM_UI_BUNDLE_NAME) {
        std::string nextElement = nextAbilityRecord->GetWant().GetElement().GetURI();
        HILOG_INFO("%{public}s, next ability record : %{public}s", __func__, nextElement.c_str());
        MoveToBackgroundTask(nextAbilityRecord);
    }

    // 3. when the mission ends and returns to lanucher directly, the next and back are inconsistent.
    // shoukd move back ability to background and then terminate.
    std::shared_ptr<AbilityRecord> backAbilityRecord = abilityRecord->GetBackAbilityRecord();
    if (backAbilityRecord != nullptr && backAbilityRecord->GetAbilityState() == AbilityState::INACTIVE &&
        backAbilityRecord->IsTerminating() &&
        backAbilityRecord->GetAbilityInfo().name != AbilityConfig::SYSTEM_DIALOG_NAME &&
        backAbilityRecord->GetAbilityInfo().bundleName != AbilityConfig::SYSTEM_UI_BUNDLE_NAME &&
        (nextAbilityRecord == nullptr || nextAbilityRecord->GetRecordId() != backAbilityRecord->GetRecordId())) {
        std::string backElement = backAbilityRecord->GetWant().GetElement().GetURI();
        HILOG_INFO("%{public}s, back ability record: %{public}s", __func__, backElement.c_str());
        MoveToBackgroundTask(backAbilityRecord);
    }
}

void AbilityStackManager::MoveToBackgroundTask(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    if (!abilityRecord) {
        HILOG_ERROR("abilityRecord is nullptr");
        return;
    }
    abilityRecord->SetIsNewWant(false);
    std::string backElement = abilityRecord->GetWant().GetElement().GetURI();
    HILOG_INFO("ability record: %{public}s", backElement.c_str());
    auto task = [abilityRecord, stackManager = shared_from_this()]() {
        HILOG_WARN("stack manager move to background timeout.");
        stackManager->CompleteBackground(abilityRecord);
    };
    abilityRecord->MoveToBackground(task);
}

void AbilityStackManager::CompleteInactive(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::string element = abilityRecord->GetWant().GetElement().GetURI();
    HILOG_INFO("%{public}s, ability: %{public}s", __func__, element.c_str());
    abilityRecord->SetAbilityState(AbilityState::INACTIVE);
    // 1. it may be inactive callback of terminate ability.
    if (abilityRecord->IsTerminating()) {
        abilityRecord->SendResultToCallers();
        if (abilityRecord->IsForceTerminate()) {
            HILOG_WARN("ability is forced to terminate.");
            MoveToBackgroundTask(abilityRecord);
            return;
        }
        std::shared_ptr<AbilityRecord> topAbilityRecord = GetCurrentTopAbility();
        if (topAbilityRecord == nullptr) {
            HILOG_ERROR("no top ability! Jump to launcher.");
            return;
        }
        // need to specify the back ability as the current ability
        topAbilityRecord->SetBackAbilityRecord(abilityRecord);
        // top ability.has been pushed into stack, but haven't load.
        // so we need load it first
        topAbilityRecord->ProcessActivate();
        return;
    }
    // 2. it may be callback of start ability.
    // if next ability has been launched and is in bottom of mission, just resume other than loading ability.
    std::shared_ptr<AbilityRecord> nextAbilityRecord = abilityRecord->GetNextAbilityRecord();
    if (nextAbilityRecord == nullptr) {
        HILOG_ERROR("failed to get next ability record");
        return;
    }
    std::string nextElement = nextAbilityRecord->GetWant().GetElement().GetURI();
    HILOG_DEBUG("next ability record: %{public}s", nextElement.c_str());
    nextAbilityRecord->ProcessActivate();
}

void AbilityStackManager::CompleteBackground(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::string element = abilityRecord->GetWant().GetElement().GetURI();
    sptr<Token> token = abilityRecord->GetToken();
    HILOG_INFO("%{public}s, ability: %{public}s", __func__, element.c_str());

    if (abilityRecord->GetAbilityState() == ACTIVATING || abilityRecord->GetAbilityState() == ACTIVE) {
        HILOG_ERROR("%{public}s, ability may be activing or active, it can't complete background ", __func__);
        return;
    }

    abilityRecord->SetAbilityState(AbilityState::BACKGROUND);
    // send application state to AppMS.
    // notify AppMS to update application state.
    DelayedSingleton<AppScheduler>::GetInstance()->MoveToBackground(token);
    // Abilities ahead of the one started with SingleTask mode were put in terminate list, we need to terminate them.
    for (auto terminateAbility : terminateAbilityRecordList_) {
        if (terminateAbility->GetAbilityState() == AbilityState::BACKGROUND) {
            auto timeoutTask = [terminateAbility, stackManager = shared_from_this()]() {
                HILOG_WARN("disconnect ability terminate timeout.");
                stackManager->CompleteTerminate(terminateAbility);
            };
            terminateAbility->Terminate(timeoutTask);
        }
    }
}

void AbilityStackManager::CompleteTerminate(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    if (!abilityRecord) {
        HILOG_ERROR("%{public}s, abilityRecord is nullptr", __func__);
        return;
    }
    std::string element = abilityRecord->GetWant().GetElement().GetURI();
    HILOG_INFO("%{public}s, ability: %{public}s", __func__, element.c_str());

    // notify AppMS terminate
    if (abilityRecord->TerminateAbility() != ERR_OK) {
        // Don't return here
        HILOG_ERROR("AppMS fail to terminate ability");
    }
    // destroy abilityRecord
    auto windowInfo = abilityRecord->GetWindowInfo();
    if (windowInfo != nullptr) {
        windowTokenToAbilityMap_.erase(windowInfo->windowToken_);
    }
    for (auto it : terminateAbilityRecordList_) {
        if (it == abilityRecord) {
            terminateAbilityRecordList_.remove(it);
            HILOG_DEBUG("destroy ability record count %ld", abilityRecord.use_count());
            break;
        }
        HILOG_WARN("can't find ability in terminate list.");
    }
}

void AbilityStackManager::Dump(std::vector<std::string> &info)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::string dumpInfo = "User ID #" + std::to_string(userId_);
    info.push_back(dumpInfo);
    for (auto missionStack : missionStackList_) {
        missionStack->Dump(info);
    }
}

void AbilityStackManager::DumpStack(int missionStackId, std::vector<std::string> &info)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::string dumpInfo = "User ID #" + std::to_string(userId_);
    info.push_back(dumpInfo);
    for (auto missionStack : missionStackList_) {
        if (missionStackId == missionStack->GetMissionStackId()) {
            missionStack->Dump(info);
            return;
        }
    }
    info.push_back("Invalid stack number, please see ability dump stack-list.");
    return;
}

void AbilityStackManager::DumpStackList(std::vector<std::string> &info)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::string dumpInfo = "User ID #" + std::to_string(userId_);
    info.push_back(dumpInfo);
    for (auto missionStack : missionStackList_) {
        missionStack->DumpStackList(info);
    }
}

void AbilityStackManager::GetAllStackInfo(StackInfo &stackInfo)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    for (auto missionStack : missionStackList_) {
        MissionStackInfo missionStackInfo;
        missionStackInfo.id = missionStack->GetMissionStackId();
        missionStack->GetAllMissionInfo(missionStackInfo.missionRecords);
        stackInfo.missionStackInfos.emplace_back(missionStackInfo);
    }
}

void AbilityStackManager::DumpMission(int missionId, std::vector<std::string> &info)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::string dumpInfo = "User ID #" + std::to_string(userId_);
    info.push_back(dumpInfo);
    for (auto missionStack : missionStackList_) {
        auto mission = missionStack->GetMissionRecordById(missionId);
        if (mission != nullptr) {
            mission->Dump(info);
            return;
        }
    }
    info.push_back("error: invalid mission number, please see 'ability dump --stack-list'.");
    return;
}

void AbilityStackManager::DumpTopAbility(std::vector<std::string> &info)
{
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    std::string dumpInfo = "User ID #" + std::to_string(userId_);
    auto topAbility = GetCurrentTopAbility();
    if (topAbility) {
        topAbility->Dump(info);
    }
    return;
}

void AbilityStackManager::DumpWaittingAbilityQueue(std::string &result)
{
    std::queue<AbilityRequest> copyQueue;
    {
        std::lock_guard<std::recursive_mutex> guard(stackLock_);
        if (waittingAbilityQueue_.empty()) {
            result = "The waitting ability queue is empty.";
            return;
        }
        copyQueue = waittingAbilityQueue_;
    }

    result = "User ID #" + std::to_string(userId_) + LINE_SEPARATOR;
    while (!copyQueue.empty()) {
        auto ability = copyQueue.front();
        std::vector<std::string> state;
        ability.Dump(state);

        for (auto it : state) {
            result += it;
            result += LINE_SEPARATOR;
        }
        copyQueue.pop();
    }
    return;
}

void AbilityStackManager::EnqueueWaittingAbility(const AbilityRequest &abilityRequest)
{
    waittingAbilityQueue_.push(abilityRequest);
    return;
}

void AbilityStackManager::StartWaittingAbility()
{
    std::shared_ptr<AbilityRecord> topAbility;
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    topAbility = GetCurrentTopAbility();
    if (!topAbility) {
        HILOG_INFO("%{public}s, topAbility is nullptr", __func__);
        return;
    }
    if (topAbility->GetAbilityState() != ACTIVE) {
        HILOG_INFO("top ability is not active, must return for waiting again");
        return;
    }

    if (!waittingAbilityQueue_.empty()) {
        AbilityRequest abilityRequest = waittingAbilityQueue_.front();
        waittingAbilityQueue_.pop();
        StartAbilityLocked(topAbility, abilityRequest);
    }
}

void AbilityStackManager::GetMissionRecordAndAbilityRecord(const AbilityRequest &abilityRequest,
    const std::shared_ptr<AbilityRecord> &currentTopAbility, std::shared_ptr<AbilityRecord> &targetAbilityRecord,
    std::shared_ptr<MissionRecord> &targetMissionRecord)
{
    HILOG_DEBUG("%{public}s, %{public}d", __func__, __LINE__);
    if (currentMissionStack_ == nullptr) {
        HILOG_ERROR("currentMissionStack_ is nullptr");
        return;
    }
    // The singleInstance start mode, mission name is #bundleName:abilityName.
    if (abilityRequest.abilityInfo.launchMode == AppExecFwk::LaunchMode::SINGLETON) {
        GetRecordBySingleton(abilityRequest, currentTopAbility, targetAbilityRecord, targetMissionRecord);
        // The standard start mode, mission name is bundle name by default.
    } else {
        GetRecordByStandard(abilityRequest, currentTopAbility, targetAbilityRecord, targetMissionRecord);
    }
}

void AbilityStackManager::GetRecordBySingleton(const AbilityRequest &abilityRequest,
    const std::shared_ptr<AbilityRecord> &currentTopAbility, std::shared_ptr<AbilityRecord> &targetAbilityRecord,
    std::shared_ptr<MissionRecord> &targetMissionRecord)
{
    if (currentMissionStack_ == nullptr) {
        HILOG_ERROR("currentMissionStack_ is nullptr");
        return;
    }
    std::string bundleName = MISSION_NAME_MARK_HEAD + abilityRequest.abilityInfo.bundleName + MISSION_NAME_SEPARATOR +
                             abilityRequest.abilityInfo.name;
    std::shared_ptr<MissionRecord> missionRecord = currentMissionStack_->GetTargetMissionRecord(bundleName);
    if (missionRecord == nullptr) {
        targetAbilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
        targetMissionRecord = std::make_shared<MissionRecord>(bundleName);
    } else {
        targetAbilityRecord = missionRecord->GetTopAbilityRecord();
        if (targetAbilityRecord != nullptr) {
            targetAbilityRecord->SetWant(abilityRequest.want);
            targetAbilityRecord->SetIsNewWant(true);
        }
        targetMissionRecord = missionRecord;
    }
}

void AbilityStackManager::GetRecordByStandard(const AbilityRequest &abilityRequest,
    const std::shared_ptr<AbilityRecord> &currentTopAbility, std::shared_ptr<AbilityRecord> &targetAbilityRecord,
    std::shared_ptr<MissionRecord> &targetMissionRecord)
{
    if (currentMissionStack_ == nullptr) {
        HILOG_ERROR("currentMissionStack_ is nullptr");
        return;
    }
    if (currentTopAbility == nullptr ||
        (currentTopAbility && currentTopAbility->IsLauncherAbility() && !IsLauncherAbility(abilityRequest)) ||
        (currentTopAbility && !currentTopAbility->IsLauncherAbility() && IsLauncherAbility(abilityRequest)) ||
        (currentTopAbility && currentTopAbility->GetAbilityInfo().launchMode == AppExecFwk::LaunchMode::SINGLETON)) {
        // first get target mission record by bundleName
        auto missionRecord = currentMissionStack_->GetTargetMissionRecord(abilityRequest.abilityInfo.bundleName);
        if (missionRecord == nullptr) {
            targetAbilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
            targetMissionRecord = std::make_shared<MissionRecord>(abilityRequest.abilityInfo.bundleName);
        } else {
            /* If current top ability is singleton mode, target mission record will be changed.
             *  Check whether the requested ability is not at the top of the stack of the target mission,
             *  True: Need to create a new one . Other: Restart the top ability of this mission.
             */
            if (currentTopAbility && !missionRecord->IsTopAbilityRecordByName(abilityRequest.abilityInfo.name) &&
                (currentTopAbility->GetAbilityInfo().launchMode == AppExecFwk::LaunchMode::SINGLETON)) {
                targetAbilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
            } else {
                targetAbilityRecord = missionRecord->GetTopAbilityRecord();
                if (targetAbilityRecord != nullptr) {
                    // The third-party app is called back from the desktop,
                    // and the specified slice route does not take effect.
                    targetAbilityRecord->SetWant(abilityRequest.want);
                    targetAbilityRecord->SetIsNewWant(true);
                }
            }
            targetMissionRecord = missionRecord;
        }
    } else if ((currentTopAbility && currentTopAbility->IsLauncherAbility() && IsLauncherAbility(abilityRequest)) ||
               (currentTopAbility && !currentTopAbility->IsLauncherAbility() && !IsLauncherAbility(abilityRequest))) {
        // The requested ability is already top ability. Reuse top ability.
        if ((abilityRequest.abilityInfo.launchMode == AppExecFwk::LaunchMode::SINGLETOP &&
                currentTopAbility->GetMissionRecord()->IsTopAbilityRecordByName(abilityRequest.abilityInfo.name)) ||
            (abilityRequest.abilityInfo.launchMode == AppExecFwk::LaunchMode::STANDARD && abilityRequest.restart)) {
            targetAbilityRecord = currentTopAbility;
            targetAbilityRecord->SetWant(abilityRequest.want);
            targetAbilityRecord->SetIsNewWant(true);
            targetMissionRecord = currentTopAbility->GetMissionRecord();
        } else {
            targetAbilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
            targetMissionRecord = currentTopAbility->GetMissionRecord();
        }
    }
}

bool AbilityStackManager::IsLauncherAbility(const AbilityRequest &abilityRequest)
{
    return abilityRequest.abilityInfo.applicationInfo.isLauncherApp;
}

bool AbilityStackManager::IsLauncherMission(int id)
{
    if (launcherMissionStack_ == nullptr || launcherMissionStack_->GetMissionRecordById(id) == nullptr) {
        return false;
    }
    return true;
}

int AbilityStackManager::GetRecentMissions(
    const int32_t numMax, const int32_t flags, std::vector<RecentMissionInfo> &recentList)
{
    HILOG_INFO("%{public}s,%{public}d", __PRETTY_FUNCTION__, __LINE__);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    if (numMax < 0) {
        HILOG_ERROR("get recent missions, numMax is invalid");
        return ERR_INVALID_VALUE;
    }
    if (flags < RECENT_WITH_EXCLUDED || flags > RECENT_IGNORE_UNAVAILABLE) {
        HILOG_ERROR("get recent missions, flags is invalid");
        return ERR_INVALID_VALUE;
    }

    return GetRecentMissionsLocked(numMax, flags, recentList);
}

int AbilityStackManager::GetRecentMissionsLocked(
    const int32_t numMax, const int32_t flags, std::vector<RecentMissionInfo> &recentList)
{
    HILOG_INFO("%{public}s,%{public}d", __func__, __LINE__);
    if (defaultMissionStack_ == nullptr) {
        HILOG_ERROR("defaultMissionStack_ is invalid");
        return ERR_NO_INIT;
    }

    bool withExcluded = (static_cast<uint32_t>(flags) & RECENT_WITH_EXCLUDED) != 0;
    std::vector<MissionRecordInfo> missionInfos;
    defaultMissionStack_->GetAllMissionInfo(missionInfos);
    for (auto &mission : missionInfos) {
        if (static_cast<int>(recentList.size()) >= numMax) {
            break;
        }
        // flags is RECENT_IGNORE_UNAVAILABLE,
        // You need to determine the mission optimized by the process
        // Then continue
        if (!withExcluded) {
            auto missionRecord = defaultMissionStack_->GetMissionRecordById(mission.id);
            if (!missionRecord) {
                HILOG_ERROR("mission is nullptr, continue");
                continue;
            }
            auto ability = missionRecord->GetTopAbilityRecord();
            if (!ability) {
                HILOG_ERROR("ability is nullptr, continue");
                continue;
            }
            if (ability->GetAbilityState() == AbilityState::INITIAL) {
                HILOG_INFO("flag is RECENT_IGNORE_UNAVAILABLE, ability state: INITIAL, continue");
                continue;
            }
        }
        RecentMissionInfo recentMissionInfo;
        CreateRecentMissionInfo(mission, recentMissionInfo);
        recentList.emplace_back(recentMissionInfo);
    }

    return ERR_OK;
}

void AbilityStackManager::CreateRecentMissionInfo(
    const MissionRecordInfo &mission, RecentMissionInfo &recentMissionInfo)
{
    HILOG_INFO("%{public}s,%{public}d", __func__, __LINE__);
    recentMissionInfo.id = mission.id;
    recentMissionInfo.runingState = -1;

    auto missionRecord = defaultMissionStack_->GetMissionRecordById(mission.id);
    if (missionRecord == nullptr) {
        HILOG_INFO("%{public}s,mission record is not exist", __func__);
        return;
    }
    auto baseAbility = missionRecord->GetBottomAbilityRecord();
    if (baseAbility != nullptr) {
        recentMissionInfo.baseWant = baseAbility->GetWant();
        OHOS::AppExecFwk::ElementName baseElement(baseAbility->GetAbilityInfo().deviceId,
            baseAbility->GetAbilityInfo().bundleName,
            baseAbility->GetAbilityInfo().name);
        recentMissionInfo.baseAbility = baseElement;
    }

    auto topAbility = missionRecord->GetTopAbilityRecord();
    if (topAbility != nullptr) {
        OHOS::AppExecFwk::ElementName topElement(topAbility->GetAbilityInfo().deviceId,
            topAbility->GetAbilityInfo().bundleName,
            topAbility->GetAbilityInfo().name);
        recentMissionInfo.topAbility = topElement;

        MissionDescriptionInfo missionDescription;
        missionDescription.label = topAbility->GetApplicationInfo().label;
        missionDescription.iconPath = topAbility->GetApplicationInfo().iconPath;
        recentMissionInfo.missionDescription = missionDescription;
    }

    recentMissionInfo.size = missionRecord->GetAbilityRecordCount();
}

int AbilityStackManager::MoveMissionToTop(int32_t missionId)
{
    HILOG_INFO("%{public}s,%{public}d", __PRETTY_FUNCTION__, __LINE__);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    if (missionId < 0) {
        HILOG_ERROR("%{public}s, mission id is invalid", __func__);
        return ERR_INVALID_VALUE;
    }

    return MoveMissionToTopLocked(missionId);
}

int AbilityStackManager::MoveMissionToTopLocked(int32_t missionId)
{
    HILOG_INFO("%{public}s,%{public}d", __PRETTY_FUNCTION__, __LINE__);
    if (!defaultMissionStack_ || !launcherMissionStack_) {
        HILOG_ERROR("mission stack is invalid");
        return ERR_NO_INIT;
    }

    if (launcherMissionStack_->IsExistMissionRecord(missionId)) {
        HILOG_ERROR("Launcher stack mission cannot move to top");
        return MOVE_MISSION_FAILED;
    }

    auto missionRecord = defaultMissionStack_->GetMissionRecordById(missionId);
    if (!missionRecord) {
        HILOG_ERROR("Mission does not exist");
        return MOVE_MISSION_FAILED;
    }

    auto bottomAbilityRecord = missionRecord->GetBottomAbilityRecord();
    if (!bottomAbilityRecord) {
        HILOG_ERROR("the bottom ability does not exist");
        return MOVE_MISSION_FAILED;
    }

    AbilityRequest requestInfo;
    requestInfo.want = bottomAbilityRecord->GetWant();
    requestInfo.abilityInfo = bottomAbilityRecord->GetAbilityInfo();
    requestInfo.appInfo = bottomAbilityRecord->GetApplicationInfo();
    requestInfo.requestCode = bottomAbilityRecord->GetRequestCode();
    requestInfo.restart = (bottomAbilityRecord->GetAbilityState() == AbilityState::INITIAL);
    StartAbility(requestInfo);

    return ERR_OK;
}

void AbilityStackManager::OnAbilityDied(std::shared_ptr<AbilityRecord> abilityRecord)
{
    HILOG_INFO("%{public}s,%{public}d", __PRETTY_FUNCTION__, __LINE__);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    if (!abilityRecord) {
        HILOG_ERROR("OnAbilityDied record is nullptr");
        return;
    }

    if (!launcherMissionStack_ || !defaultMissionStack_) {
        HILOG_ERROR("mission stack is invalid");
        return;
    }

    if (abilityRecord->GetAbilityInfo().type != AbilityType::PAGE) {
        HILOG_ERROR("ability type is not page");
        return;
    }
    // If the launcher ability is dead, delete the record
    if (abilityRecord->IsLauncherAbility()) {
        OnAbilityDiedByLauncher(abilityRecord);
        return;
    }

    OnAbilityDiedByDefault(abilityRecord);
}

void AbilityStackManager::OnAbilityDiedByLauncher(std::shared_ptr<AbilityRecord> abilityRecord)
{
    HILOG_INFO("%{public}s,%{public}d", __func__, __LINE__);
    if (!abilityRecord || !launcherMissionStack_) {
        HILOG_ERROR("ability is nullptr.");
        return;
    }
    auto mission = abilityRecord->GetMissionRecord();
    if (!mission || !launcherMissionStack_->IsExistMissionRecord(mission->GetMissionRecordId()) ||
        !mission->IsExistAbilityRecord(abilityRecord->GetRecordId())) {
        HILOG_ERROR("mission or ability record is not in launcher stack.");
        return;
    }

    // Terminate launcher ability on the top of dead ability
    std::vector<AbilityRecordInfo> abilityInfos;
    mission->GetAllAbilityInfo(abilityInfos);
    for (auto &it : abilityInfos) {
        auto ability = mission->GetAbilityRecordById(it.id);
        if (!ability) {
            HILOG_WARN("ability is nullptr.");
            continue;
        }
        if (ability == abilityRecord) {
            break;
        }
        if (ability->IsTerminating()) {
            HILOG_ERROR("ability is terminating.");
            continue;
        }
        HILOG_DEBUG("terminate launcher ability.");
        ability->SetTerminatingState();
        TerminateAbilityLocked(ability, -1, nullptr);
    }

    // Process the dead ability record
    if (mission->GetBottomAbilityRecord() == abilityRecord && abilityRecord->IsLauncherRoot()) {
        HILOG_DEBUG("root launcher ability died, set state: INITIAL");
        abilityRecord->SetAbilityState(AbilityState::INITIAL);
    } else {
        mission->RemoveAbilityRecord(abilityRecord);
        if (mission->GetAbilityRecordCount() == 0) {
            launcherMissionStack_->RemoveMissionRecord(mission->GetMissionRecordId());
        }
    }

    DelayedStartLauncher();
}

void AbilityStackManager::DelayedStartLauncher()
{
    auto abilityManagerService = DelayedSingleton<AbilityManagerService>::GetInstance();
    auto handler = abilityManagerService->GetEventHandler();
    if (!handler) {
        HILOG_ERROR("handler is nullptr.");
        return;
    }
    auto timeoutTask = [stackManager = shared_from_this()]() {
        HILOG_DEBUG("the launcher needs to be restarted.");
        stackManager->BackToLauncher();
    };
    handler->PostTask(timeoutTask, "Launcher_Restart", AbilityManagerService::RESTART_TIMEOUT);
}

void AbilityStackManager::OnAbilityDiedByDefault(std::shared_ptr<AbilityRecord> abilityRecord)
{
    HILOG_INFO("%{public}s,%{public}d", __func__, __LINE__);
    if (!abilityRecord) {
        HILOG_ERROR("ability is nullptr,%{public}d", __LINE__);
        return;
    }
    auto mission = abilityRecord->GetMissionRecord();
    if (!mission || !mission->IsExistAbilityRecord(abilityRecord->GetRecordId())) {
        HILOG_ERROR("mission is nullptr or not exist");
        return;
    }
    std::vector<AbilityRecordInfo> abilityInfos;
    mission->GetAllAbilityInfo(abilityInfos);
    for (auto &it : abilityInfos) {
        auto ability = mission->GetAbilityRecordById(it.id);
        if (!ability) {
            HILOG_ERROR("ability is nullptr,%{public}d", __LINE__);
            continue;
        }
        if (ability == abilityRecord) {
            DelayedStartLauncher();
            if (abilityRecord->IsUninstallAbility()) {
                HILOG_INFO("ability uninstall,%{public}d", __LINE__);
                mission->RemoveAbilityRecord(abilityRecord);
                if (mission->GetAbilityRecordCount() == 0) {
                    defaultMissionStack_->RemoveMissionRecord(mission->GetMissionRecordId());
                }
                break;
            }
            if (mission->GetBottomAbilityRecord() == ability) {
                HILOG_INFO("ability died, state: INITIAL, %{public}d", __LINE__);
                abilityRecord->SetAbilityState(AbilityState::INITIAL);
            } else {
                HILOG_INFO("ability died, remove record, %{public}d", __LINE__);
                mission->RemoveAbilityRecord(abilityRecord);
            }
            break;
        }
        if (ability->IsTerminating()) {
            HILOG_ERROR("ability is terminating, %{public}d", __LINE__);
            continue;
        }
        HILOG_INFO("terminate ability, %{public}d", __LINE__);
        ability->SetForceTerminate(true);
        ability->SetTerminatingState();
        TerminateAbilityLocked(ability, -1, nullptr);
    }
}

void AbilityStackManager::BackToLauncher()
{
    HILOG_INFO("%{public}s,%{public}d", __PRETTY_FUNCTION__, __LINE__);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    if (!defaultMissionStack_ || !launcherMissionStack_) {
        HILOG_ERROR("mission stack is invalid");
        return;
    }

    auto currentTopAbility = GetCurrentTopAbility();
    if (currentTopAbility && currentTopAbility->GetAbilityState() == AbilityState::ACTIVE) {
        HILOG_WARN("current top ability is active, no need to start launcher.");
        return;
    }
    auto launcherAbility = GetLauncherRootAbility();
    if (!launcherAbility) {
        HILOG_ERROR("there is no root launcher ability record, back to launcher failed.");
        return;
    }

    MoveMissionStackToTop(launcherMissionStack_);

    auto missionRecord = launcherAbility->GetMissionRecord();
    if (!missionRecord) {
        HILOG_ERROR("can't get root launcher ability record's mission, back to launcher failed.");
        return;
    }

    if (currentTopAbility) {
        launcherAbility->SetPreAbilityRecord(currentTopAbility);
        currentTopAbility->SetNextAbilityRecord(launcherAbility);

        missionRecord->SetPreMissionRecord(currentTopAbility->GetMissionRecord());
        if (currentTopAbility->IsLauncherAbility()) {
            missionRecord->SetIsLauncherCreate();
        }
    }
    launcherMissionStack_->MoveMissionRecordToTop(missionRecord);
    launcherAbility->ProcessActivate();
}

std::shared_ptr<AbilityRecord> AbilityStackManager::GetLauncherRootAbility() const
{
    if (!launcherMissionStack_) {
        HILOG_ERROR("mission stack is invalid");
        return nullptr;
    }
    std::vector<MissionRecordInfo> missionInfos;
    launcherMissionStack_->GetAllMissionInfo(missionInfos);
    for (auto &mission : missionInfos) {
        auto missionRecord = launcherMissionStack_->GetMissionRecordById(mission.id);
        if (!missionRecord) {
            continue;
        }
        for (auto &it : mission.abilityRecordInfos) {
            auto abilityRecord = missionRecord->GetAbilityRecordById(it.id);
            if (abilityRecord && abilityRecord->IsLauncherRoot()) {
                return abilityRecord;
            }
        }
    }
    return nullptr;
}

int AbilityStackManager::KillProcess(const std::string &bundleName)
{
    HILOG_INFO("%{public}s, bundleName: %{public}s %{public}d", __func__, bundleName.c_str(), __LINE__);
    int ret = DelayedSingleton<AppScheduler>::GetInstance()->KillApplication(bundleName);
    if (ret != ERR_OK) {
        return KILL_PROCESS_FAILED;
    }
    return ERR_OK;
}

int AbilityStackManager::UninstallApp(const std::string &bundleName)
{
    HILOG_INFO("%{public}s, bundleName: %{public}s %{public}d", __func__, bundleName.c_str(), __LINE__);
    auto abilityManagerService = DelayedSingleton<AbilityManagerService>::GetInstance();
    if (!abilityManagerService) {
        HILOG_ERROR("Ability on scheduler died: failed to get ams.");
        return ERR_NO_MEMORY;
    }
    auto handler = abilityManagerService->GetEventHandler();
    if (!handler) {
        HILOG_ERROR("Ability on scheduler died: failed to get ams handler.");
        return ERR_NO_MEMORY;
    }
    auto task = [bundleName, this]() { AddUninstallTags(bundleName); };
    handler->PostTask(task);

    int ret = DelayedSingleton<AppScheduler>::GetInstance()->KillApplication(bundleName);
    if (ret != ERR_OK) {
        return UNINSTALL_APP_FAILED;
    }
    return ERR_OK;
}

void AbilityStackManager::AddUninstallTags(const std::string &bundleName)
{
    HILOG_INFO("%{public}s, bundleName: %{public}s %{public}d", __func__, bundleName.c_str(), __LINE__);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    for (auto &stack : missionStackList_) {
        std::vector<MissionRecordInfo> missions;
        stack->GetAllMissionInfo(missions);
        for (auto &missionInfo : missions) {
            auto mission = stack->GetMissionRecordById(missionInfo.id);
            if (!mission) {
                HILOG_ERROR("mission is nullptr");
                continue;
            }
            std::vector<AbilityRecordInfo> abilitys;
            mission->GetAllAbilityInfo(abilitys);
            for (auto &abilityInfo : abilitys) {
                auto ability = mission->GetAbilityRecordById(abilityInfo.id);
                if (!ability) {
                    HILOG_ERROR("ability is nullptr");
                    continue;
                }

                ability->SetIsUninstallAbility();
            }
        }
    }
}

std::shared_ptr<AbilityRecord> AbilityStackManager::GetAbilityRecordByEventId(int64_t eventId) const
{
    HILOG_DEBUG("%{public}s", __func__);
    for (auto &stack : missionStackList_) {
        std::vector<MissionRecordInfo> missionInfos;
        stack->GetAllMissionInfo(missionInfos);
        for (auto &mission : missionInfos) {
            auto missionRecord = stack->GetMissionRecordById(mission.id);
            if (!missionRecord) {
                continue;
            }
            for (auto &it : mission.abilityRecordInfos) {
                auto abilityRecord = missionRecord->GetAbilityRecordById(it.id);
                if (abilityRecord && abilityRecord->GetEventId() == eventId) {
                    HILOG_DEBUG("%{public}s get ability record", __func__);
                    return abilityRecord;
                }
            }
        }
    }
    return nullptr;
}

void AbilityStackManager::OnTimeOut(uint32_t msgId, int64_t eventId)
{
    HILOG_DEBUG("%{public}s", __func__);
    std::lock_guard<std::recursive_mutex> guard(stackLock_);
    auto abilityRecord = GetAbilityRecordByEventId(eventId);
    if (abilityRecord == nullptr) {
        HILOG_ERROR("stack manager on time out event: ability record is nullptr.");
        return;
    }
    HILOG_DEBUG("ability timeout ,msg:%{public}d,name:%{public}s", msgId, abilityRecord->GetAbilityInfo().name.c_str());
    switch (msgId) {
        case AbilityManagerService::LOAD_TIMEOUT_MSG:
        case AbilityManagerService::ACTIVE_TIMEOUT_MSG:
        case AbilityManagerService::INACTIVE_TIMEOUT_MSG:
            DelayedStartLauncher();
            break;
        default:
            break;
    }
}
}  // namespace AAFwk
}  // namespace OHOS
