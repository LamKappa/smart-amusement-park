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

#include "kit_test_ability_manager.h"
#include <iostream>
#include <numeric>
#include <sstream>
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

using namespace OHOS::EventFwk;
using namespace OHOS::AAFwk;

namespace {
const std::string bundleName = "com.ohos.amsst.AppKitAbilityManager";
const std::string abilityName = "KitTestAbilityManagerSecond";
const std::string currentAbilityName = "KitTestAbilityManager";
const std::string currentProcessInfo = "com.ohos.amsst.AppKit";
const std::string launchAbilityName = "LauncherAbility";
const std::string launchProcessInfo = "com.ix.launcher";
bool isMoveMissionToTop = false;
int moveMissionToTopCode = -1;
int isClearUpApplicationData = false;
}  // namespace

void KitTestAbilityManager::SubscribeEvent(const vector_conststr &eventList)
{
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber = std::make_shared<KitTestManagerEventSubscriber>(subscribeInfo, this);
    CommonEventManager::SubscribeCommonEvent(subscriber);
}

void KitTestAbilityManager::AbilityManagerStByCode(int apiIndex, int caseIndex, int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerStByCode");
    if (mapStKitFunc_.find(apiIndex) != mapStKitFunc_.end() &&
        static_cast<int>(mapStKitFunc_[apiIndex].size()) > caseIndex) {
        mapStKitFunc_[apiIndex][caseIndex](code);
    } else {
        APP_LOGI("AbilityManagerStByCode error");
    }
}

void KitTestAbilityManager::CompareProcessName(RunningProcessInfo &info, const std::string &expectedName, int code)
{
    bool result = false;
    for (auto processInfo : info.appProcessInfos) {
        if (processInfo.processName_ == expectedName) {
            result = true;
        }
    }
    TestUtils::PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManager::CompareProcessState(
    RunningProcessInfo &info, const std::string &processName, AppProcessState expectedState, int code)
{
    bool result = false;
    for (auto processInfo : info.appProcessInfos) {
        if (processInfo.processName_ == processName && processInfo.state_ == expectedState) {
            result = true;
        }
    }
    TestUtils::PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManager::StartAbilitySelf(
    const std::string &bundleName, const std::string &abilityNmae, AbilityManagerApi api, int codeIndex, int code)
{
    MAP_STR_STR params;
    params["apiIndex"] = std::to_string(static_cast<int>(api));
    params["caseIndex"] = std::to_string(codeIndex);
    params["code"] = std::to_string(code);
    Want want = TestUtils::MakeWant("device", abilityNmae, bundleName, params);
    StartAbility(want);
}

void KitTestAbilityManager::GetAllStackInfo(MissionStackInfo &missionStackInfo, int stackID)
{
    StackInfo stackInfo = AbilityManager::GetInstance().GetAllStackInfo();
    for (const auto &missionInfo : stackInfo.missionStackInfos) {
        if (missionInfo.id == stackID) {
            missionStackInfo = missionInfo;
            break;
        }
    }
}

// GetAllRunningProcesses ST kit case
void KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase1(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase1");
    RunningProcessInfo info = AbilityManager::GetInstance().GetAllRunningProcesses();
    CompareProcessName(info, currentProcessInfo, code);
}

void KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase2(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase2");
    RunningProcessInfo info = AbilityManager::GetInstance().GetAllRunningProcesses();
    CompareProcessName(info, launchProcessInfo, code);
}

void KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase3(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase3");
    RunningProcessInfo info = AbilityManager::GetInstance().GetAllRunningProcesses();
    std::shared_ptr<ProcessInfo> processInfo = AbilityContext::GetProcessInfo();
    bool result = false;
    for (auto runProcessInfo : info.appProcessInfos) {
        if (runProcessInfo.processName_ == currentProcessInfo && processInfo->GetPid() == runProcessInfo.pid_) {
            result = true;
        }
    }
    TestUtils::PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase4(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase4");
    RunningProcessInfo info = AbilityManager::GetInstance().GetAllRunningProcesses();
    CompareProcessState(info, currentProcessInfo, AppProcessState::APP_STATE_FOREGROUND, code);
}

void KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase5(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase5");
    auto index = 0;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::GetAllRunningProcesses, index, code);
}

void KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase6(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase6");
    auto index = 1;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::GetAllRunningProcesses, index, code);
}

void KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase7(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase7");
    auto index = 2;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::GetAllRunningProcesses, index, code);
}

void KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase8(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase8");
    auto index = 3;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::GetAllRunningProcesses, index, code);
}

void KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase9(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase9");
    auto index = 4;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::GetAllRunningProcesses, index, code);
}

void KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase10(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllRunningProcessesCase10");
    auto index = 5;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::GetAllRunningProcesses, index, code);
}

// GetAllStackInfo ST kit case
void KitTestAbilityManager::AbilityManagerGetAllStackInfoCase1(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllStackInfoCase1");
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo, 1);
    bool result = (missionStackInfo.missionRecords.size() == 1);
    TestUtils::PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManager::AbilityManagerGetAllStackInfoCase2(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllStackInfoCase2");
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo, 1);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    bool result = (abilityInfos[0].mainName.compare(currentAbilityName) == 0);
    TestUtils::PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManager::AbilityManagerGetAllStackInfoCase3(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllStackInfoCase3");
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo, 0);
    bool result = (missionStackInfo.missionRecords.size() == 1);
    TestUtils::PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManager::AbilityManagerGetAllStackInfoCase4(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllStackInfoCase4");
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo, 0);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    bool result = (abilityInfos[0].mainName.compare(launchAbilityName) == 0);
    TestUtils::PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManager::AbilityManagerGetAllStackInfoCase5(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllStackInfoCase5");
    auto index = 0;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::GetAllStackInfo, index, code);
}

void KitTestAbilityManager::AbilityManagerGetAllStackInfoCase6(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllStackInfoCase6");
    auto index = 1;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::GetAllStackInfo, index, code);
}

void KitTestAbilityManager::AbilityManagerGetAllStackInfoCase7(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllStackInfoCase7");
    auto index = 2;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::GetAllStackInfo, index, code);
}

void KitTestAbilityManager::AbilityManagerGetAllStackInfoCase8(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllStackInfoCase8");
    auto index = 3;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::GetAllStackInfo, index, code);
}

void KitTestAbilityManager::AbilityManagerGetAllStackInfoCase9(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerGetAllStackInfoCase9");
    auto index = 4;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::GetAllStackInfo, index, code);
}

// QueryRecentAbilityMissionInfo ST kit case
void KitTestAbilityManager::QueryRecentAbilityMissionInfoParam(int numMax, int code, std::size_t size, int flags)
{
    std::vector<RecentMissionInfo> info;
    info = AbilityManager::GetInstance().QueryRecentAbilityMissionInfo(numMax, flags);
    bool result = (info.size() == size);
    TestUtils::PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase1(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase1");
    auto num = -1;
    auto size = 0;
    QueryRecentAbilityMissionInfoParam(num, code, size, RECENT_IGNORE_UNAVAILABLE);
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase2(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase2");
    auto num = 3;
    auto size = 0;
    auto flag = -1;
    QueryRecentAbilityMissionInfoParam(num, code, size, flag);
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase3(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase3");
    auto num = 3;
    auto size = 0;
    auto flag = 0;
    QueryRecentAbilityMissionInfoParam(num, code, size, flag);
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase4(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase4");
    auto num = 3;
    auto size = 0;
    auto flag = 3;
    QueryRecentAbilityMissionInfoParam(num, code, size, flag);
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase5(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase5");
    auto num = 3;
    auto size = 1;
    QueryRecentAbilityMissionInfoParam(num, code, size, RECENT_WITH_EXCLUDED);
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase6(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase6");
    auto num = 3;
    auto size = 1;
    QueryRecentAbilityMissionInfoParam(num, code, size, RECENT_IGNORE_UNAVAILABLE);
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase7(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase7");
    std::vector<RecentMissionInfo> info;
    auto num = 3;
    info = AbilityManager::GetInstance().QueryRecentAbilityMissionInfo(num, RECENT_WITH_EXCLUDED);
    bool result = false;
    if (info.size() == 1 && info[0].size == 1) {
        result = true;
    }
    TestUtils::PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase8(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase8");
    auto index = 0;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::QueryRecentAbilityMissionInfo, index, code);
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase9(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase9");
    auto index = 1;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::QueryRecentAbilityMissionInfo, index, code);
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase10(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase10");
    auto index = 2;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::QueryRecentAbilityMissionInfo, index, code);
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase11(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase11");
    auto num = INT32_MIN;
    auto size = 0;
    QueryRecentAbilityMissionInfoParam(num, code, size, RECENT_WITH_EXCLUDED);
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase12(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase12");
    auto num = INT32_MAX;
    auto size = 1;
    QueryRecentAbilityMissionInfoParam(num, code, size, RECENT_WITH_EXCLUDED);
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase13(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase13");
    auto num = 3;
    auto size = 0;
    auto flag = INT32_MIN;
    QueryRecentAbilityMissionInfoParam(num, code, size, flag);
}

void KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase14(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRecentAbilityMissionInfoCase14");
    auto num = 3;
    auto size = 0;
    auto flag = INT32_MAX;
    QueryRecentAbilityMissionInfoParam(num, code, size, flag);
}

// QueryRunningAbilityMissionInfo ST kit case
void KitTestAbilityManager::QueryRunningAbilityMissionInfoParam(int numMax, int code, std::size_t size)
{
    std::vector<RecentMissionInfo> info;
    info = AbilityManager::GetInstance().QueryRunningAbilityMissionInfo(numMax);
    bool result = (info.size() == size);
    TestUtils::PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase1(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase1");
    auto num = -1;
    auto flag = 0;
    QueryRunningAbilityMissionInfoParam(num, code, flag);
}

void KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase2(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase2");
    auto num = 0;
    auto flag = 0;
    QueryRunningAbilityMissionInfoParam(num, code, flag);
}

void KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase3(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase3");
    auto num = 3;
    auto flag = 1;
    QueryRunningAbilityMissionInfoParam(num, code, flag);
}

void KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase4(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase4");
    std::vector<RecentMissionInfo> info;
    auto num = 3;
    info = AbilityManager::GetInstance().QueryRunningAbilityMissionInfo(num);
    bool result = false;
    if (info.size() == 1 && info[0].size == 1) {
        result = true;
    }
    TestUtils::PublishEvent(g_respPageManagerAbilityST, code, std::to_string(result));
}

void KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase5(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase5");
    auto index = 0;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::QueryRunningAbilityMissionInfo, index, code);
}

void KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase6(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase6");
    auto index = 1;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::QueryRunningAbilityMissionInfo, index, code);
}

void KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase7(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase7");
    auto index = 2;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::QueryRunningAbilityMissionInfo, index, code);
}

void KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase8(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase8");
    QueryRunningAbilityMissionInfoParam(INT32_MIN, code, 0);
}

void KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase9(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerQueryRunningAbilityMissionInfoCase9");
    QueryRunningAbilityMissionInfoParam(INT32_MAX, code, 1);
}

// MoveMissionToTop ST kit case
void KitTestAbilityManager::AbilityManagerMoveMissionToTopCase1(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerMoveMissionToTopCase1");
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::MoveMissionToTop, 0, code);
}

void KitTestAbilityManager::AbilityManagerMoveMissionToTopCase2(int code)
{
    APP_LOGI("KitTestAbilityManager::AbilityManagerMoveMissionToTopCase2");
    moveMissionToTopCode = code;
    isMoveMissionToTop = true;
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    StartAbility(wantEntity);
}

// ClearUpApplicationData ST kit case
void KitTestAbilityManager::AbilityManagerClearUpApplicationDataCase1(int code)
{
    isClearUpApplicationData = true;
    StartAbilitySelf(bundleName, abilityName, AbilityManagerApi::ClearUpApplicationData, 0, code);
}

void KitTestAbilityManager::OnStart(const Want &want)
{
    APP_LOGI("KitTestAbilityManager::onStart");
    Ability::OnStart(want);
    SubscribeEvent(g_requPageManagerAbilitySTVector);
    std::string eventData = GetAbilityName() + g_abilityStateOnStart;
    TestUtils::PublishEvent(g_respPageManagerAbilityST, 0, eventData);
}

void KitTestAbilityManager::OnStop()
{
    APP_LOGI("KitTestAbilityManager::onStop");
    Ability::OnStop();
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    std::string eventData = GetAbilityName() + g_abilityStateOnStop;
    TestUtils::PublishEvent(g_respPageManagerAbilityST, 0, eventData);
}

void KitTestAbilityManager::OnActive()
{
    APP_LOGI("KitTestAbilityManager::OnActive");
    Ability::OnActive();
    if (isMoveMissionToTop) {
        TestUtils::PublishEvent(g_respPageManagerAbilityST, moveMissionToTopCode, "1");
        isMoveMissionToTop = false;
        moveMissionToTopCode = -1;
    }
    std::string eventData = GetAbilityName() + g_abilityStateOnActive;
    TestUtils::PublishEvent(g_respPageManagerAbilityST, 0, eventData);
}

void KitTestAbilityManager::OnInactive()
{
    APP_LOGI("KitTestAbilityManager::OnInactive");
    Ability::OnInactive();
    std::string eventData = GetAbilityName() + g_abilityStateOnInactive;
    TestUtils::PublishEvent(g_respPageManagerAbilityST, 0, eventData);
}

void KitTestAbilityManager::OnBackground()
{
    APP_LOGI("KitTestAbilityManager::OnBackground");
    Ability::OnBackground();
    if (isMoveMissionToTop) {
        std::vector<RecentMissionInfo> info;
        info = AbilityManager::GetInstance().QueryRecentAbilityMissionInfo(3, RECENT_WITH_EXCLUDED);
        if (1 == info.size() && 1 == info[0].size) {
            GetAbilityManager()->MoveMissionToTop(info[0].id);
        }
    }
    if (isClearUpApplicationData != false) {
        AbilityManager::GetInstance().ClearUpApplicationData(currentProcessInfo);
        isClearUpApplicationData = false;
    }
    std::string eventData = GetAbilityName() + g_abilityStateOnBackground;
    TestUtils::PublishEvent(g_respPageManagerAbilityST, 0, eventData);
}

void KitTestAbilityManager::OnForeground(const Want &want)
{
    APP_LOGI("KitTestAbilityManager::OnForeground");
    Ability::OnForeground(want);
    std::string eventData = GetAbilityName() + g_abilityStateOnForeground;
    TestUtils::PublishEvent(g_respPageManagerAbilityST, 0, eventData);
}

void KitTestAbilityManager::OnNewWant(const Want &want)
{
    APP_LOGI("KitTestAbilityManager::OnNewWant");
    Ability::OnNewWant(want);
    std::string eventData = GetAbilityName() + g_abilityStateOnNewWant;
    TestUtils::PublishEvent(g_respPageManagerAbilityST, 0, eventData);
}

// KitTestManagerEventSubscriber Class
void KitTestManagerEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (g_requPageManagerAbilityST == eventName) {
        auto target = data.GetData();
        auto handle = 0;
        auto api = 1;
        auto code = 2;
        auto paramMinSize = 3;
        vector_str splitResult = TestUtils::split(target, "_");
        auto keyMap = splitResult.at(handle);
        if (mapTestFunc_.find(keyMap) != mapTestFunc_.end() &&
            splitResult.size() >= static_cast<unsigned int>(paramMinSize)) {
            auto apiIndex = atoi(splitResult.at(api).c_str());
            auto caseIndex = atoi(splitResult.at(code).c_str());
            mapTestFunc_[keyMap](apiIndex, caseIndex, data.GetCode());
        } else {
            if (keyMap == "TerminateAbility") {
                KitTerminateAbility();
            } else {
                APP_LOGI("OnReceiveEvent: CommonEventData error(%{public}s)", target.c_str());
            }
        }
    }
}

void KitTestManagerEventSubscriber::AbilityManagerStByCode(int apiIndex, int caseIndex, int code)
{
    if (kitTestAbility_ != nullptr) {
        kitTestAbility_->AbilityManagerStByCode(apiIndex, caseIndex, code);
    }
}

void KitTestManagerEventSubscriber::KitTerminateAbility()
{
    if (kitTestAbility_ != nullptr) {
        kitTestAbility_->TerminateAbility();
    }
}

REGISTER_AA(KitTestAbilityManager)
}  // namespace AppExecFwk
}  // namespace OHOS