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

#ifndef _THIRD_ABILITY_H_
#define _THIRD_ABILITY_H_
#include <unordered_map>
#include "ability_loader.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "kit_test_common_info.h"
#include "process_info.h"

namespace OHOS {
namespace AppExecFwk {
using vector_str = std::vector<std::string>;
using vector_conststr = std::vector<const std::string>;
using vector_func = std::vector<std::function<void(int)>>;
class KitTestThirdEventSubscriber;

class ThirdAbility : public Ability {
public:
    void SubscribeEvent(const vector_conststr &eventList);
    void ProcessInfoStByCode(int apiIndex, int caseIndex, int code);
    bool CompareProcessInfo(const ProcessInfo &processInfo1, const ProcessInfo &processInfo2);
    void GetParcelByProcessName(ProcessInfo &processInfo, const std::string &expectedString, int code);
    void GetParcelByProcessID(ProcessInfo &processInfo, const int expectedInt, int code);
    void GetProcessNameByParcel(const std::string &processInfo, int processID, int code);
    void GetProcessIDByParcel(const std::string &processName, int processID, int code);
    void ComparePidProcessName(ProcessInfo &processInfo, int expectedPid, const std::string &expectedName, int code);

    // GetPid ST kit Case
    void ProcessInfoGetPidCase1(int code);
    void ProcessInfoGetPidCase2(int code);
    void ProcessInfoGetPidCase3(int code);
    void ProcessInfoGetPidCase4(int code);

    // GetProcessName ST kit Case
    void ProcessInfoGetProcessNameCase1(int code);
    void ProcessInfoGetProcessNameCase2(int code);
    void ProcessInfoGetProcessNameCase3(int code);
    void ProcessInfoGetProcessNameCase4(int code);

    // Marshalling ST kit Case
    void ProcessInfoMarshallingCase1(int code);
    void ProcessInfoMarshallingCase2(int code);
    void ProcessInfoMarshallingCase3(int code);
    void ProcessInfoMarshallingCase4(int code);
    void ProcessInfoMarshallingCase5(int code);
    void ProcessInfoMarshallingCase6(int code);

    // Unmarshalling ST kit Case
    void ProcessInfoUnmarshallingCase1(int code);
    void ProcessInfoUnmarshallingCase2(int code);
    void ProcessInfoUnmarshallingCase3(int code);
    void ProcessInfoUnmarshallingCase4(int code);
    void ProcessInfoUnmarshallingCase5(int code);
    void ProcessInfoUnmarshallingCase6(int code);
    void ProcessInfoUnmarshallingCase7(int code);
    void ProcessInfoUnmarshallingCase8(int code);
    void ProcessInfoUnmarshallingCase9(int code);

    // ProcessInfo ST kit Case
    void ProcessInfoProcessInfoCase1(int code);
    void ProcessInfoProcessInfoCase2(int code);

    // ProcessInfo_String_int ST kit Case
    void ProcessInfoProcessInfoStringintCase1(int code);
    void ProcessInfoProcessInfoStringintCase2(int code);
    void ProcessInfoProcessInfoStringintCase3(int code);
    void ProcessInfoProcessInfoStringintCase4(int code);
    void ProcessInfoProcessInfoStringintCase5(int code);
    void ProcessInfoProcessInfoStringintCase6(int code);

    std::shared_ptr<KitTestThirdEventSubscriber> subscriber;

protected:
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnForeground(const Want &want) override;
    virtual void OnNewWant(const Want &want) override;

private:
    void Clear();
    void GetWantInfo(const Want &want);
    std::string Split(std::string &str, std::string delim);

    std::string shouldReturn_;
    std::string targetBundle_;
    std::string targetAbility_;
    std::unordered_map<int, vector_func> mapStKitFunc_ = {
        {static_cast<int>(ProcessInfoApi::GetPid),
            {{[this](int code) { ProcessInfoGetPidCase1(code); }},
                {[this](int code) { ProcessInfoGetPidCase2(code); }},
                {[this](int code) { ProcessInfoGetPidCase3(code); }},
                {[this](int code) { ProcessInfoGetPidCase4(code); }}}},
        {static_cast<int>(ProcessInfoApi::GetProcessName),
            {{[this](int code) { ProcessInfoGetProcessNameCase1(code); }},
                {[this](int code) { ProcessInfoGetProcessNameCase2(code); }},
                {[this](int code) { ProcessInfoGetProcessNameCase3(code); }},
                {[this](int code) { ProcessInfoGetProcessNameCase4(code); }}}},
        {static_cast<int>(ProcessInfoApi::Marshalling),
            {{[this](int code) { ProcessInfoMarshallingCase1(code); }},
                {[this](int code) { ProcessInfoMarshallingCase2(code); }},
                {[this](int code) { ProcessInfoMarshallingCase3(code); }},
                {[this](int code) { ProcessInfoMarshallingCase4(code); }},
                {[this](int code) { ProcessInfoMarshallingCase5(code); }},
                {[this](int code) { ProcessInfoMarshallingCase6(code); }}}},
        {static_cast<int>(ProcessInfoApi::Unmarshalling),
            {{[this](int code) { ProcessInfoUnmarshallingCase1(code); }},
                {[this](int code) { ProcessInfoUnmarshallingCase2(code); }},
                {[this](int code) { ProcessInfoUnmarshallingCase3(code); }},
                {[this](int code) { ProcessInfoUnmarshallingCase4(code); }},
                {[this](int code) { ProcessInfoUnmarshallingCase5(code); }},
                {[this](int code) { ProcessInfoUnmarshallingCase6(code); }},
                {[this](int code) { ProcessInfoUnmarshallingCase7(code); }},
                {[this](int code) { ProcessInfoUnmarshallingCase8(code); }},
                {[this](int code) { ProcessInfoUnmarshallingCase9(code); }}}},
        {static_cast<int>(ProcessInfoApi::ProcessInfo),
            {{[this](int code) { ProcessInfoProcessInfoCase1(code); }},
                {[this](int code) { ProcessInfoProcessInfoCase2(code); }}}},
        {static_cast<int>(ProcessInfoApi::ProcessInfo_String_int),
            {{[this](int code) { ProcessInfoProcessInfoStringintCase1(code); }},
                {[this](int code) { ProcessInfoProcessInfoStringintCase2(code); }},
                {[this](int code) { ProcessInfoProcessInfoStringintCase3(code); }},
                {[this](int code) { ProcessInfoProcessInfoStringintCase4(code); }},
                {[this](int code) { ProcessInfoProcessInfoStringintCase5(code); }},
                {[this](int code) { ProcessInfoProcessInfoStringintCase6(code); }}}}};
};

class KitTestThirdEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    KitTestThirdEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp, ThirdAbility *ability)
        : CommonEventSubscriber(sp)
    {
        mapTestFunc_ = {
            {"ProcessInfoApi",
                [this](int apiIndex, int caseIndex, int code) { ProcessInfoStByCode(apiIndex, caseIndex, code); }},
        };
        thirdAbility_ = ability;
    }
    ~KitTestThirdEventSubscriber() = default;
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data) override;
    void ProcessInfoStByCode(int apiIndex, int caseIndex, int code);
    void KitTerminateAbility();
    ThirdAbility *thirdAbility_;

private:
    std::unordered_map<std::string, std::function<void(int, int, int)>> mapTestFunc_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // _THIRD_ABILITY_H_