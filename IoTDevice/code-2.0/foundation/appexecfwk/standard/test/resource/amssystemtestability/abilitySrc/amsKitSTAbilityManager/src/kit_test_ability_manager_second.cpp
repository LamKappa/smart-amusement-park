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

#include "kit_test_ability_manager_second.h"
#include <iostream>
#include <numeric>
#include <sstream>
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;
using namespace OHOS::AAFwk;

namespace {
const std::string currentAbilityName = "KitTestAbilityManagerSecond";
const std::string topAbilityName = "KitTestAbilityManager";
const std::string launchAbilityName = "LauncherAbility";
const std::string bundleName = "com.ohos.amsst.AppKit";
const std::string topProcessInfo = "com.ohos.amsst.AppKit";
const std::string currentProcessInfo = "com.ohos.amsst.AppKitAbilityManager";
const std::string launchProcessInfo = "com.ix.launcher";
bool isMoveMissionToTop = false;
int moveMissionToTopCode = -1;
bool isClearUpApplicationData = false;
int clearUpApplicationDataCode = -1;
}  // namespace

bool KitTestAbilityManagerSecond::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    Want want;
    want.SetAction(eventName);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}

void KitTestAbilityManagerSecond::SubscribeEvent(const vector_conststr &eventList)
{
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber = std::make_shared<KitTestManagerSecondEventSubscriber>(subscribeInfo, this);
    CommonEventManager::SubscribeCommonEvent(subscriber);
}

void KitTestAbilityManagerSecond::AbilityManagerStByCode(int apiIndex, int caseIndex, int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerStByCode");
    if (mapStKitFunc_.find(apiIndex) != mapStKitFunc_.end() &&
        static_cast<int>(mapStKitFunc_[apiIndex].size()) > caseIndex) {
        mapStKitFunc_[apiIndex][caseIndex](code);
    } else {
        APP_LOGI("AbilityManagerStByCode error");
    }
}

void KitTestAbilityManagerSecond::CompareProcessName(
    RunningProcessInfo &info, const std::string &expectedName, int code)
{
    bool result = false;
    for (auto processInfo : info.appProcessInfos) {
        if (processInfo.processName_ == expectedName) {
            result = true;
        }
    }
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManagerSecond::CompareProcessState(
    RunningProcessInfo &info, const std::string &processName, AppProcessState expectedState, int code)
{
    bool result = false;
    for (auto processInfo : info.appProcessInfos) {
        if (processInfo.processName_ == processName && processInfo.state_ == expectedState) {
            result = true;
        }
    }
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManagerSecond::ProcessStateNotEqual(
    RunningProcessInfo &info, const std::string &processName, AppProcessState expectedState, int code)
{
    bool result = false;
    for (auto processInfo : info.appProcessInfos) {
        if (processInfo.processName_ == processName && processInfo.state_ != expectedState) {
            result = true;
        }
    }
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManagerSecond::GetAllStackInfo(MissionStackInfo &missionStackInfo, int stackID)
{
    StackInfo stackInfo;
    stackInfo = AbilityManager::GetInstance().GetAllStackInfo();
    for (const auto &stackInfo : stackInfo.missionStackInfos) {
        if (stackInfo.id == stackID) {
            missionStackInfo = stackInfo;
            break;
        }
    }
}

// GetAllRunningProcesses ST kit case
void KitTestAbilityManagerSecond::AbilityManagerGetAllRunningProcessesCase1(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerGetAllRunningProcessesCase1");
    RunningProcessInfo info = AbilityManager::GetInstance().GetAllRunningProcesses();
    CompareProcessName(info, currentProcessInfo, code);
}

void KitTestAbilityManagerSecond::AbilityManagerGetAllRunningProcessesCase2(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerGetAllRunningProcessesCase2");
    RunningProcessInfo info = AbilityManager::GetInstance().GetAllRunningProcesses();
    CompareProcessName(info, topProcessInfo, code);
}

void KitTestAbilityManagerSecond::AbilityManagerGetAllRunningProcessesCase3(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerGetAllRunningProcessesCase3");
    RunningProcessInfo info = AbilityManager::GetInstance().GetAllRunningProcesses();
    CompareProcessName(info, launchProcessInfo, code);
}

void KitTestAbilityManagerSecond::AbilityManagerGetAllRunningProcessesCase4(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerGetAllRunningProcessesCase4");
    RunningProcessInfo info = AbilityManager::GetInstance().GetAllRunningProcesses();
    CompareProcessState(info, launchProcessInfo, AppProcessState::APP_STATE_BACKGROUND, code);
}

void KitTestAbilityManagerSecond::AbilityManagerGetAllRunningProcessesCase5(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerGetAllRunningProcessesCase5");
    RunningProcessInfo info = AbilityManager::GetInstance().GetAllRunningProcesses();
    CompareProcessState(info, currentProcessInfo, AppProcessState::APP_STATE_FOREGROUND, code);
}

void KitTestAbilityManagerSecond::AbilityManagerGetAllRunningProcessesCase6(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerGetAllRunningProcessesCase6");
    RunningProcessInfo info = AbilityManager::GetInstance().GetAllRunningProcesses();
    std::shared_ptr<ProcessInfo> processInfo = AbilityContext::GetProcessInfo();
    bool result = false;
    for (auto runProcessInfo : info.appProcessInfos) {
        if (runProcessInfo.processName_ == currentProcessInfo && processInfo->GetPid() == runProcessInfo.pid_) {
            result = true;
        }
    }
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

// GetAllStackInfo ST kit case
void KitTestAbilityManagerSecond::AbilityManagerGetAllStackInfoCase1(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerGetAllStackInfoCase1");
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo, 0);
    bool result = (missionStackInfo.missionRecords.size() == 1);
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManagerSecond::AbilityManagerGetAllStackInfoCase2(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerGetAllStackInfoCase2");
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo, 0);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    bool result = (abilityInfos[0].mainName.compare(launchAbilityName) == 0);
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManagerSecond::AbilityManagerGetAllStackInfoCase3(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerGetAllStackInfoCase3");
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo, 1);
    bool result = (missionStackInfo.missionRecords.size() == 1);
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManagerSecond::AbilityManagerGetAllStackInfoCase4(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerGetAllStackInfoCase4");
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo, 1);
    bool result = false;
    if (missionStackInfo.missionRecords.size() == 1) {
        result = (missionStackInfo.missionRecords[0].abilityRecordInfos.size() == 2);
    }
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManagerSecond::AbilityManagerGetAllStackInfoCase5(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerGetAllStackInfoCase5");
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo, 1);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    bool currentResult = (abilityInfos[0].mainName.compare(currentAbilityName) == 0);
    bool topResult = (abilityInfos[1].mainName.compare(topAbilityName) == 0);
    bool result = (currentResult && topResult);
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

// QueryRecentAbilityMissionInfo ST kit case
void KitTestAbilityManagerSecond::AbilityManagerQueryRecentAbilityMissionInfoCase1(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerQueryRecentAbilityMissionInfoCase1");
    std::vector<RecentMissionInfo> info;
    info = AbilityManager::GetInstance().QueryRecentAbilityMissionInfo(3, RECENT_WITH_EXCLUDED);
    bool result = false;
    if (1 == info.size() && 2 == info[0].size) {
        if (info[0].baseAbility.GetBundleName() == bundleName) {
            result = true;
        }
    }
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManagerSecond::AbilityManagerQueryRecentAbilityMissionInfoCase2(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerQueryRecentAbilityMissionInfoCase2");
    std::vector<RecentMissionInfo> info;
    info = AbilityManager::GetInstance().QueryRecentAbilityMissionInfo(3, RECENT_WITH_EXCLUDED);
    bool result = false;
    if (1 == info.size() && 2 == info[0].size) {
        result = true;
    }
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManagerSecond::AbilityManagerQueryRecentAbilityMissionInfoCase3(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerQueryRecentAbilityMissionInfoCase3");
    std::vector<RecentMissionInfo> info;
    info = AbilityManager::GetInstance().QueryRecentAbilityMissionInfo(3, RECENT_WITH_EXCLUDED);
    bool result = false;
    if (1 == info.size() && 2 == info[0].size) {
        if (info[0].baseAbility.GetBundleName() == bundleName &&
            info[0].baseAbility.GetAbilityName() == topAbilityName) {
            result = true;
        }
    }
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

// QueryRunningAbilityMissionInfo ST kit case
void KitTestAbilityManagerSecond::AbilityManagerQueryRunningAbilityMissionInfoCase1(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerQueryRunningAbilityMissionInfoCase1");
    std::vector<RecentMissionInfo> info;
    info = AbilityManager::GetInstance().QueryRunningAbilityMissionInfo(3);
    bool result = false;
    if (1 == info.size() && 2 == info[0].size) {
        if (info[0].baseAbility.GetBundleName() == bundleName) {
            result = true;
        }
    }
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManagerSecond::AbilityManagerQueryRunningAbilityMissionInfoCase2(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerQueryRunningAbilityMissionInfoCase2");
    std::vector<RecentMissionInfo> info;
    info = AbilityManager::GetInstance().QueryRunningAbilityMissionInfo(3);
    bool result = false;
    if (1 == info.size() && 2 == info[0].size) {
        result = true;
    }
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManagerSecond::AbilityManagerQueryRunningAbilityMissionInfoCase3(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerQueryRunningAbilityMissionInfoCase3");
    std::vector<RecentMissionInfo> info;
    info = AbilityManager::GetInstance().QueryRunningAbilityMissionInfo(3);
    bool result = false;
    if (1 == info.size() && 2 == info[0].size) {
        if (info[0].baseAbility.GetBundleName() == bundleName &&
            info[0].baseAbility.GetAbilityName() == topAbilityName) {
            result = true;
        }
    }
    PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManagerSecond::AbilityManagerMoveMissionToTopCase1(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerMoveMissionToTopCase1");
    moveMissionToTopCode = code;
    isMoveMissionToTop = true;
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    StartAbility(wantEntity);
}

void KitTestAbilityManagerSecond::AbilityManagerClearUpApplicationDataCase1(int code)
{
    APP_LOGI("KitTestAbilityManagerSecond::AbilityManagerClearUpApplicationDataCase1");
    isClearUpApplicationData = true;
    clearUpApplicationDataCode = code;
}

void KitTestAbilityManagerSecond::OnStart(const Want &want)
{
    APP_LOGI("KitTestAbilityManagerSecond::onStart");
    Ability::OnStart(want);
    SubscribeEvent(g_requPageManagerSecondAbilitySTVector);
    GetWantInfo(want);
    std::string eventData = GetAbilityName() + g_abilityStateOnStart;
    PublishEvent(g_respPageManagerSecondAbilityST, 0, eventData);
}

void KitTestAbilityManagerSecond::OnStop()
{
    APP_LOGI("KitTestAbilityManagerSecond::onStop");
    Ability::OnStop();
    if (isClearUpApplicationData != false) {
        PublishEvent(g_respPageManagerAbilityST, clearUpApplicationDataCode, "1");
        isClearUpApplicationData = -1;
        clearUpApplicationDataCode = false;
    }
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    std::string eventData = GetAbilityName() + g_abilityStateOnStop;
    PublishEvent(g_respPageManagerSecondAbilityST, 0, eventData);
}

void KitTestAbilityManagerSecond::OnActive()
{
    APP_LOGI("KitTestAbilityManagerSecond::OnActive");
    Ability::OnActive();
    if (true == isMoveMissionToTop) {
        PublishEvent(g_respPageManagerAbilityST, moveMissionToTopCode, "1");
        isMoveMissionToTop = false;
        moveMissionToTopCode = -1;
    }
    int apiIndex_i = std::atoi(strapiIndex_.c_str());
    int codeIndex_i = std::atoi(strcaseIndex_.c_str());
    int code_i = std::atoi(strcode_.c_str());
    Clear();
    AbilityManagerStByCode(apiIndex_i, codeIndex_i, code_i);
    std::string eventData = GetAbilityName() + g_abilityStateOnActive;
    PublishEvent(g_respPageManagerSecondAbilityST, 0, eventData);
}

void KitTestAbilityManagerSecond::OnInactive()
{
    APP_LOGI("KitTestAbilityManagerSecond::OnInactive");
    Ability::OnInactive();
    std::string eventData = GetAbilityName() + g_abilityStateOnInactive;
    PublishEvent(g_respPageManagerSecondAbilityST, 0, eventData);
}

void KitTestAbilityManagerSecond::OnBackground()
{
    APP_LOGI("KitTestAbilityManagerSecond::OnBackground");
    Ability::OnBackground();
    if (true == isMoveMissionToTop) {
        std::vector<RecentMissionInfo> info;
        info = AbilityManager::GetInstance().QueryRecentAbilityMissionInfo(3, RECENT_WITH_EXCLUDED);
        if (1 == info.size() && 2 == info[0].size) {
            GetAbilityManager()->MoveMissionToTop(info[0].id);
            APP_LOGI("GetAbilityManager()->MoveMissionToTop(info[0].id);%{public}d", static_cast<int>(info[0].id));
        }
    }
    std::string eventData = GetAbilityName() + g_abilityStateOnBackground;
    PublishEvent(g_respPageManagerSecondAbilityST, 0, eventData);
}

void KitTestAbilityManagerSecond::OnForeground(const Want &want)
{
    APP_LOGI("KitTestAbilityManagerSecond::OnForeground");
    Ability::OnForeground(want);
    std::string eventData = GetAbilityName() + g_abilityStateOnForeground;
    PublishEvent(g_respPageManagerSecondAbilityST, 0, eventData);
}

void KitTestAbilityManagerSecond::OnNewWant(const Want &want)
{
    APP_LOGI("KitTestAbilityManagerSecond::OnNewWant");
    Ability::OnNewWant(want);
    std::string eventData = GetAbilityName() + g_abilityStateOnNewWant;
    PublishEvent(g_respPageManagerSecondAbilityST, 0, eventData);
}

void KitTestAbilityManagerSecond::Clear()
{
    strapiIndex_ = "";
    strcaseIndex_ = "";
    strcode_ = "";
}

void KitTestAbilityManagerSecond::GetWantInfo(const Want &want)
{
    Want mWant(want);
    strapiIndex_ = mWant.GetStringParam("apiIndex");
    strcaseIndex_ = mWant.GetStringParam("caseIndex");
    strcode_ = mWant.GetStringParam("code");
}

// KitTestManagerSecondEventSubscriber Class
void KitTestManagerSecondEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (g_requPageManagerSecondAbilityST == eventName) {
        auto target = data.GetData();
        if (target == "TerminateAbility") {
            KitTerminateAbility();
        } else {
            APP_LOGI("OnReceiveEvent: CommonEventData error(%{public}s)", target.c_str());
        }
    }
}

void KitTestManagerSecondEventSubscriber::KitTerminateAbility()
{
    if (kitTestAbility_ != nullptr) {
        kitTestAbility_->TerminateAbility();
    }
}

REGISTER_AA(KitTestAbilityManagerSecond)
}  // namespace AppExecFwk
}  // namespace OHOS