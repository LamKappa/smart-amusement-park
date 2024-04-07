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

#ifndef _KIT_TEST_ABILITY_MANAGER_H_
#define _KIT_TEST_ABILITY_MANAGER_H_
#include <unordered_map>
#include "ability_loader.h"
#include "running_process_info.h"
#include "ability_manager.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "kit_test_common_info.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using vector_str = std::vector<std::string>;
using vector_conststr = std::vector<const std::string>;
using vector_func = std::vector<std::function<void(int)>>;
using MAP_STR_STR = std::map<std::string, std::string>;
class KitTestManagerEventSubscriber;
class KitTestAbilityManager : public Ability {
public:
    void SubscribeEvent(const vector_conststr &eventList);
    void AbilityManagerStByCode(int apiIndex, int caseIndex, int code);
    void CompareProcessName(RunningProcessInfo &info, const std::string &expectedName, int code);
    void CompareProcessState(
        RunningProcessInfo &info, const std::string &processName, AppProcessState expectedState, int code);
    void StartAbilitySelf(
        const std::string &bundleName, const std::string &abilityNmae, AbilityManagerApi api, int codeIndex, int code);
    void GetAllStackInfo(AAFwk::MissionStackInfo &missionStackInfo, int stackID);

    // GetAllRunningProcesses ST kit case
    void AbilityManagerGetAllRunningProcessesCase1(int code);
    void AbilityManagerGetAllRunningProcessesCase2(int code);
    void AbilityManagerGetAllRunningProcessesCase3(int code);
    void AbilityManagerGetAllRunningProcessesCase4(int code);
    void AbilityManagerGetAllRunningProcessesCase5(int code);
    void AbilityManagerGetAllRunningProcessesCase6(int code);
    void AbilityManagerGetAllRunningProcessesCase7(int code);
    void AbilityManagerGetAllRunningProcessesCase8(int code);
    void AbilityManagerGetAllRunningProcessesCase9(int code);
    void AbilityManagerGetAllRunningProcessesCase10(int code);

    // GetAllStackInfo ST kit case
    void AbilityManagerGetAllStackInfoCase1(int code);
    void AbilityManagerGetAllStackInfoCase2(int code);
    void AbilityManagerGetAllStackInfoCase3(int code);
    void AbilityManagerGetAllStackInfoCase4(int code);
    void AbilityManagerGetAllStackInfoCase5(int code);
    void AbilityManagerGetAllStackInfoCase6(int code);
    void AbilityManagerGetAllStackInfoCase7(int code);
    void AbilityManagerGetAllStackInfoCase8(int code);
    void AbilityManagerGetAllStackInfoCase9(int code);

    // QueryRecentAbilityMissionInfo ST kit case
    void QueryRecentAbilityMissionInfoParam(int numMax, int code, std::size_t size, int flags);
    void AbilityManagerQueryRecentAbilityMissionInfoCase1(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase2(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase3(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase4(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase5(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase6(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase7(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase8(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase9(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase10(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase11(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase12(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase13(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase14(int code);

    // QueryRunningAbilityMissionInfo ST kit case
    void QueryRunningAbilityMissionInfoParam(int numMax, int code, std::size_t size);
    void AbilityManagerQueryRunningAbilityMissionInfoCase1(int code);
    void AbilityManagerQueryRunningAbilityMissionInfoCase2(int code);
    void AbilityManagerQueryRunningAbilityMissionInfoCase3(int code);
    void AbilityManagerQueryRunningAbilityMissionInfoCase4(int code);
    void AbilityManagerQueryRunningAbilityMissionInfoCase5(int code);
    void AbilityManagerQueryRunningAbilityMissionInfoCase6(int code);
    void AbilityManagerQueryRunningAbilityMissionInfoCase7(int code);
    void AbilityManagerQueryRunningAbilityMissionInfoCase8(int code);
    void AbilityManagerQueryRunningAbilityMissionInfoCase9(int code);

    // MoveMissionToTop ST kit case
    void AbilityManagerMoveMissionToTopCase1(int code);
    void AbilityManagerMoveMissionToTopCase2(int code);

    // ClearUpApplicationData ST kit case
    void AbilityManagerClearUpApplicationDataCase1(int code);
    std::shared_ptr<KitTestManagerEventSubscriber> subscriber;

protected:
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnForeground(const Want &want) override;
    virtual void OnNewWant(const Want &want) override;

private:
    std::unordered_map<int, vector_func> mapStKitFunc_{
        {static_cast<int>(AbilityManagerApi::GetAllRunningProcesses),
            {{[this](int code) { AbilityManagerGetAllRunningProcessesCase1(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase2(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase3(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase4(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase5(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase6(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase7(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase8(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase9(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase10(code); }}}},
        {static_cast<int>(AbilityManagerApi::GetAllStackInfo),
            {{[this](int code) { AbilityManagerGetAllStackInfoCase1(code); }},
                {[this](int code) { AbilityManagerGetAllStackInfoCase2(code); }},
                {[this](int code) { AbilityManagerGetAllStackInfoCase3(code); }},
                {[this](int code) { AbilityManagerGetAllStackInfoCase4(code); }},
                {[this](int code) { AbilityManagerGetAllStackInfoCase5(code); }},
                {[this](int code) { AbilityManagerGetAllStackInfoCase6(code); }},
                {[this](int code) { AbilityManagerGetAllStackInfoCase7(code); }},
                {[this](int code) { AbilityManagerGetAllStackInfoCase8(code); }},
                {[this](int code) { AbilityManagerGetAllStackInfoCase9(code); }}}},
        {static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo),
            {{[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase1(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase2(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase3(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase4(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase5(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase6(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase7(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase8(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase9(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase10(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase11(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase12(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase13(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase14(code); }}}},
        {static_cast<int>(AbilityManagerApi::QueryRunningAbilityMissionInfo),
            {{[this](int code) { AbilityManagerQueryRunningAbilityMissionInfoCase1(code); }},
                {[this](int code) { AbilityManagerQueryRunningAbilityMissionInfoCase2(code); }},
                {[this](int code) { AbilityManagerQueryRunningAbilityMissionInfoCase3(code); }},
                {[this](int code) { AbilityManagerQueryRunningAbilityMissionInfoCase4(code); }},
                {[this](int code) { AbilityManagerQueryRunningAbilityMissionInfoCase5(code); }},
                {[this](int code) { AbilityManagerQueryRunningAbilityMissionInfoCase6(code); }},
                {[this](int code) { AbilityManagerQueryRunningAbilityMissionInfoCase7(code); }},
                {[this](int code) { AbilityManagerQueryRunningAbilityMissionInfoCase8(code); }},
                {[this](int code) { AbilityManagerQueryRunningAbilityMissionInfoCase9(code); }}}},
        {static_cast<int>(AbilityManagerApi::MoveMissionToTop),
            {{[this](int code) { AbilityManagerMoveMissionToTopCase1(code); }},
                {[this](int code) { AbilityManagerMoveMissionToTopCase2(code); }}}},
        {static_cast<int>(AbilityManagerApi::ClearUpApplicationData),
            {{[this](int code) { AbilityManagerClearUpApplicationDataCase1(code); }}}}};
};

// KitTestManagerEventSubscriber Class
class KitTestManagerEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    KitTestManagerEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp, KitTestAbilityManager *ability)
        : CommonEventSubscriber(sp)
    {
        mapTestFunc_ = {{"AbilityManagerApi",
            [this](int apiIndex, int caseIndex, int code) { AbilityManagerStByCode(apiIndex, caseIndex, code); }}};
        kitTestAbility_ = ability;
    }
    ~KitTestManagerEventSubscriber()
    {
        kitTestAbility_ = nullptr;
    }

    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data) override;
    void AbilityManagerStByCode(int apiIndex, int caseIndex, int code);
    void KitTerminateAbility();

private:
    std::unordered_map<std::string, std::function<void(int, int, int)>> mapTestFunc_;
    KitTestAbilityManager *kitTestAbility_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  //_KIT_TEST_ABILITY_MANAGER_H_