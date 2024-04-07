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

#ifndef _KIT_TEST_ABILITY_MANAGER_SECOND_H_
#define _KIT_TEST_ABILITY_MANAGER_SECOND_H_
#include <unordered_map>
#include "ability_loader.h"
#include "running_process_info.h"
#include "ability_manager.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "kit_test_common_info.h"

namespace OHOS {
namespace AppExecFwk {
using vector_str = std::vector<std::string>;
using vector_conststr = std::vector<const std::string>;
using vector_func = std::vector<std::function<void(int)>>;
class KitTestManagerSecondEventSubscriber;

class KitTestAbilityManagerSecond : public Ability {
public:
    bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);
    void SubscribeEvent(const vector_conststr &eventList);
    void AbilityManagerStByCode(int apiIndex, int caseIndex, int code);
    void CompareProcessName(RunningProcessInfo &info, const std::string &expectedName, int code);
    void CompareProcessState(
        RunningProcessInfo &info, const std::string &processName, AppProcessState expectedState, int code);
    void ProcessStateNotEqual(
        RunningProcessInfo &info, const std::string &processName, AppProcessState expectedState, int code);
    void GetAllStackInfo(AAFwk::MissionStackInfo &missionStackInfo, int stackID);

    // GetAllRunningProcesses ST kit case
    void AbilityManagerGetAllRunningProcessesCase1(int code);
    void AbilityManagerGetAllRunningProcessesCase2(int code);
    void AbilityManagerGetAllRunningProcessesCase3(int code);
    void AbilityManagerGetAllRunningProcessesCase4(int code);
    void AbilityManagerGetAllRunningProcessesCase5(int code);
    void AbilityManagerGetAllRunningProcessesCase6(int code);

    // GetAllStackInfo ST kit case
    void AbilityManagerGetAllStackInfoCase1(int code);
    void AbilityManagerGetAllStackInfoCase2(int code);
    void AbilityManagerGetAllStackInfoCase3(int code);
    void AbilityManagerGetAllStackInfoCase4(int code);
    void AbilityManagerGetAllStackInfoCase5(int code);

    // QueryRecentAbilityMissionInfo ST kit case
    void AbilityManagerQueryRecentAbilityMissionInfoCase1(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase2(int code);
    void AbilityManagerQueryRecentAbilityMissionInfoCase3(int code);

    // QueryRunningAbilityMissionInfo ST kit case
    void AbilityManagerQueryRunningAbilityMissionInfoCase1(int code);
    void AbilityManagerQueryRunningAbilityMissionInfoCase2(int code);
    void AbilityManagerQueryRunningAbilityMissionInfoCase3(int code);

    // MoveMissionToTop ST kit case
    void AbilityManagerMoveMissionToTopCase1(int code);

    // ClearUpApplicationData ST kit case
    void AbilityManagerClearUpApplicationDataCase1(int code);

    std::shared_ptr<KitTestManagerSecondEventSubscriber> subscriber;

protected:
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnForeground(const Want &want) override;
    virtual void OnNewWant(const Want &want) override;

private:
    std::unordered_map<int, vector_func> mapStKitFunc_ = {
        {static_cast<int>(AbilityManagerApi::GetAllRunningProcesses),
            {{[this](int code) { AbilityManagerGetAllRunningProcessesCase1(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase2(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase3(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase4(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase5(code); }},
                {[this](int code) { AbilityManagerGetAllRunningProcessesCase6(code); }}}},
        {static_cast<int>(AbilityManagerApi::GetAllStackInfo),
            {{[this](int code) { AbilityManagerGetAllStackInfoCase1(code); }},
                {[this](int code) { AbilityManagerGetAllStackInfoCase2(code); }},
                {[this](int code) { AbilityManagerGetAllStackInfoCase3(code); }},
                {[this](int code) { AbilityManagerGetAllStackInfoCase4(code); }},
                {[this](int code) { AbilityManagerGetAllStackInfoCase5(code); }}}},
        {static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo),
            {{[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase1(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase2(code); }},
                {[this](int code) { AbilityManagerQueryRecentAbilityMissionInfoCase3(code); }}}},
        {static_cast<int>(AbilityManagerApi::QueryRunningAbilityMissionInfo),
            {{[this](int code) { AbilityManagerQueryRunningAbilityMissionInfoCase1(code); }},
                {[this](int code) { AbilityManagerQueryRunningAbilityMissionInfoCase2(code); }},
                {[this](int code) { AbilityManagerQueryRunningAbilityMissionInfoCase3(code); }}}},
        {static_cast<int>(AbilityManagerApi::MoveMissionToTop),
            {{[this](int code) { AbilityManagerMoveMissionToTopCase1(code); }}}},
        {static_cast<int>(AbilityManagerApi::ClearUpApplicationData),
            {{[this](int code) { AbilityManagerClearUpApplicationDataCase1(code); }}}},
    };

    void Clear();
    void GetWantInfo(const Want &want);
    std::string strapiIndex_;
    std::string strcaseIndex_;
    std::string strcode_;
};

// KitTestManagerSecondEventSubscriber Class
class KitTestManagerSecondEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    KitTestManagerSecondEventSubscriber(
        const EventFwk::CommonEventSubscribeInfo &sp, KitTestAbilityManagerSecond *ability)
        : CommonEventSubscriber(sp)
    {
        kitTestAbility_ = ability;
    }

    ~KitTestManagerSecondEventSubscriber()
    {
        kitTestAbility_ = nullptr;
    }

    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data) override;
    void KitTerminateAbility();
    KitTestAbilityManagerSecond *kitTestAbility_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // _KIT_TEST_ABILITY_MANAGER_SECOND_H_