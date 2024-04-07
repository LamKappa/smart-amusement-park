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

#include "third_ability.h"
#include <iostream>
#include <numeric>
#include <sstream>
#include "app_log_wrapper.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

namespace {
const std::string bundleName = "com.ohos.amsst.AppKit";
const std::string specialProcessName = "//\\@#$%^&*!~;:,.";
const std::string normalProcessName = "com.ohos.amsst.AppKit";
}  // namespace

void ThirdAbility::SubscribeEvent(const vector_conststr &eventList)
{
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber = std::make_shared<KitTestThirdEventSubscriber>(subscribeInfo, this);
    CommonEventManager::SubscribeCommonEvent(subscriber);
}

void ThirdAbility::ProcessInfoStByCode(int apiIndex, int caseIndex, int code)
{
    APP_LOGI("ThirdAbility::ProcessInfoStByCode");
    if (mapStKitFunc_.find(apiIndex) != mapStKitFunc_.end() &&
        static_cast<int>(mapStKitFunc_[apiIndex].size()) > caseIndex) {
        mapStKitFunc_[apiIndex][caseIndex](code);
    } else {
        APP_LOGI("ProcessInfoStByCode error");
    }
}

bool ThirdAbility::CompareProcessInfo(const ProcessInfo &processInfo1, const ProcessInfo &processInfo2)
{
    bool equalProcessName = (processInfo1.GetProcessName() == processInfo2.GetProcessName());
    bool equalPid = (processInfo1.GetPid() == processInfo2.GetPid());
    if (equalProcessName && equalPid) {
        return true;
    } else {
        return false;
    }
}

void ThirdAbility::GetParcelByProcessName(ProcessInfo &processInfo, const std::string &expectedString, int code)
{
    Parcel parcel;
    processInfo.Marshalling(parcel);
    bool result = (expectedString == Str16ToStr8(parcel.ReadString16()));
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::GetParcelByProcessID(ProcessInfo &processInfo, const int expectedInt, int code)
{
    Parcel parcel;
    processInfo.Marshalling(parcel);
    bool result =
        (processInfo.GetProcessName() == Str16ToStr8(parcel.ReadString16())) && (expectedInt == parcel.ReadInt32());
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::GetProcessNameByParcel(const std::string &processName, int processID, int code)
{
    Parcel parcel;
    parcel.WriteString16(Str8ToStr16(processName));
    parcel.WriteInt32(processID);
    ProcessInfo *processInfo = ProcessInfo::Unmarshalling(parcel);
    bool result = (processName == processInfo->GetProcessName());
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
    delete processInfo;
    processInfo = nullptr;
}

void ThirdAbility::GetProcessIDByParcel(const std::string &processName, int processID, int code)
{
    Parcel parcel;
    parcel.WriteString16(Str8ToStr16(processName));
    parcel.WriteInt32(processID);
    ProcessInfo *processInfo = ProcessInfo::Unmarshalling(parcel);
    bool result = processInfo != nullptr;
    if (processInfo) {
        result = (processID == processInfo->GetPid());
        delete processInfo;
        processInfo = nullptr;
    }
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ComparePidProcessName(
    ProcessInfo &processInfo, int expectedPid, const std::string &expectedName, int code)
{
    bool resultPid = (expectedPid == processInfo.GetPid());
    bool resultName = (expectedName == processInfo.GetProcessName());
    bool result = (resultPid && resultName);
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoGetPidCase1(int code)
{
    std::shared_ptr<ProcessInfo> processInfo = AbilityContext::GetProcessInfo();
    bool result = processInfo != nullptr;
    if (processInfo) {
        result = (processInfo->GetPid() > 0);
    }
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoGetPidCase2(int code)
{
    ProcessInfo processInfo;
    bool result = (processInfo.GetPid() == 0);
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoGetPidCase3(int code)
{
    ProcessInfo processInfo(normalProcessName, INT32_MIN);
    bool result = (processInfo.GetPid() == INT32_MIN);
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoGetPidCase4(int code)
{
    ProcessInfo processInfo(normalProcessName, INT32_MAX);
    bool result = (processInfo.GetPid() == INT32_MAX);
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoGetProcessNameCase1(int code)
{
    std::shared_ptr<ProcessInfo> processInfo = AbilityContext::GetProcessInfo();
    bool result = (bundleName == processInfo->GetProcessName());
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoGetProcessNameCase2(int code)
{
    ProcessInfo processInfo;
    bool result = (std::string() == processInfo.GetProcessName());
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoGetProcessNameCase3(int code)
{
    ProcessInfo processInfo("", 0);
    bool result = (processInfo.GetProcessName() == "");
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoGetProcessNameCase4(int code)
{
    ProcessInfo processInfo(specialProcessName, 0);
    bool result = (specialProcessName == processInfo.GetProcessName());
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoMarshallingCase1(int code)
{
    std::shared_ptr<ProcessInfo> processInfo = AbilityContext::GetProcessInfo();
    Parcel parcel;
    processInfo->Marshalling(parcel);
    std::string processName = Str16ToStr8(parcel.ReadString16());
    bool result = (processName == bundleName);
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoMarshallingCase2(int code)
{
    std::shared_ptr<ProcessInfo> processInfo = AbilityContext::GetProcessInfo();
    Parcel parcel;
    processInfo->Marshalling(parcel);
    bool result = (parcel.ReadInt32() > 0);
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoMarshallingCase3(int code)
{
    ProcessInfo processInfo(specialProcessName, 0);
    GetParcelByProcessName(processInfo, specialProcessName, code);
}

void ThirdAbility::ProcessInfoMarshallingCase4(int code)
{
    ProcessInfo processInfo(normalProcessName, INT32_MIN);
    GetParcelByProcessID(processInfo, INT32_MIN, code);
}

void ThirdAbility::ProcessInfoMarshallingCase5(int code)
{
    ProcessInfo processInfo("", 0);
    GetParcelByProcessName(processInfo, "", code);
}

void ThirdAbility::ProcessInfoMarshallingCase6(int code)
{
    ProcessInfo processInfo(normalProcessName, INT32_MAX);
    GetParcelByProcessID(processInfo, INT32_MAX, code);
}

void ThirdAbility::ProcessInfoUnmarshallingCase1(int code)
{
    GetProcessNameByParcel(specialProcessName, 0, code);
}

void ThirdAbility::ProcessInfoUnmarshallingCase2(int code)
{
    GetProcessNameByParcel(normalProcessName, 0, code);
}

void ThirdAbility::ProcessInfoUnmarshallingCase3(int code)
{
    GetProcessNameByParcel("", 0, code);
}

void ThirdAbility::ProcessInfoUnmarshallingCase4(int code)
{
    GetProcessIDByParcel(normalProcessName, 0, code);
}

void ThirdAbility::ProcessInfoUnmarshallingCase5(int code)
{
    GetProcessIDByParcel(normalProcessName, INT32_MIN, code);
}

void ThirdAbility::ProcessInfoUnmarshallingCase6(int code)
{
    GetProcessIDByParcel(normalProcessName, INT32_MAX, code);
}

void ThirdAbility::ProcessInfoUnmarshallingCase7(int code)
{
    ProcessInfo processInfoIn(normalProcessName, INT32_MAX);
    ProcessInfo *processInfoOut = nullptr;
    Parcel in;
    processInfoIn.Marshalling(in);
    processInfoOut = ProcessInfo::Unmarshalling(in);
    if (processInfoOut == nullptr) {
        return;
    }
    TestUtils::PublishEvent(
        g_respPageThirdAbilityST, code, std::to_string(CompareProcessInfo(processInfoIn, *processInfoOut)));
    delete processInfoOut;
    processInfoOut = nullptr;
}

void ThirdAbility::ProcessInfoUnmarshallingCase8(int code)
{
    ProcessInfo processInfoIn(specialProcessName, INT32_MAX);
    ProcessInfo *processInfoOut = nullptr;
    Parcel in;
    processInfoIn.Marshalling(in);
    processInfoOut = ProcessInfo::Unmarshalling(in);
    if (processInfoOut == nullptr) {
        return;
    }
    TestUtils::PublishEvent(
        g_respPageThirdAbilityST, code, std::to_string(CompareProcessInfo(processInfoIn, *processInfoOut)));
    delete processInfoOut;
    processInfoOut = nullptr;
}

void ThirdAbility::ProcessInfoUnmarshallingCase9(int code)
{
    ProcessInfo processInfoIn(specialProcessName, INT32_MIN);
    ProcessInfo *processInfoOut = nullptr;
    Parcel in;
    processInfoIn.Marshalling(in);
    processInfoOut = ProcessInfo::Unmarshalling(in);
    if (processInfoOut == nullptr) {
        return;
    }
    TestUtils::PublishEvent(
        g_respPageThirdAbilityST, code, std::to_string(CompareProcessInfo(processInfoIn, *processInfoOut)));
    delete processInfoOut;
    processInfoOut = nullptr;
}

void ThirdAbility::ProcessInfoProcessInfoCase1(int code)
{
    ProcessInfo processInfo;
    bool result = (processInfo.GetPid() == 0);
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoProcessInfoCase2(int code)
{
    ProcessInfo processInfo;
    bool result = (processInfo.GetProcessName().empty());
    TestUtils::PublishEvent(g_respPageThirdAbilityST, code, std::to_string(result));
}

void ThirdAbility::ProcessInfoProcessInfoStringintCase1(int code)
{
    ProcessInfo processInfo(normalProcessName, 0);
    ComparePidProcessName(processInfo, 0, normalProcessName, code);
}

void ThirdAbility::ProcessInfoProcessInfoStringintCase2(int code)
{
    ProcessInfo processInfo(normalProcessName, INT32_MIN);
    ComparePidProcessName(processInfo, INT32_MIN, normalProcessName, code);
}

void ThirdAbility::ProcessInfoProcessInfoStringintCase3(int code)
{
    ProcessInfo processInfo(normalProcessName, INT32_MAX);
    ComparePidProcessName(processInfo, INT32_MAX, normalProcessName, code);
}

void ThirdAbility::ProcessInfoProcessInfoStringintCase4(int code)
{
    ProcessInfo processInfo(specialProcessName, 0);
    ComparePidProcessName(processInfo, 0, specialProcessName, code);
}

void ThirdAbility::ProcessInfoProcessInfoStringintCase5(int code)
{
    ProcessInfo processInfo(specialProcessName, INT32_MIN);
    ComparePidProcessName(processInfo, INT32_MIN, specialProcessName, code);
}

void ThirdAbility::ProcessInfoProcessInfoStringintCase6(int code)
{
    ProcessInfo processInfo(specialProcessName, INT32_MAX);
    ComparePidProcessName(processInfo, INT32_MAX, specialProcessName, code);
}

void ThirdAbility::OnStart(const Want &want)
{
    APP_LOGI("ThirdAbility::onStart");
    GetWantInfo(want);
    Ability::OnStart(want);
    SubscribeEvent(g_requPageThirdAbilitySTVector);
    std::string eventData = GetAbilityName() + g_abilityStateOnStart;
    TestUtils::PublishEvent(g_respPageThirdAbilityST, 0, eventData);
}

void ThirdAbility::OnStop()
{
    APP_LOGI("ThirdAbility::onStop");
    Ability::OnStop();
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    std::string eventData = GetAbilityName() + g_abilityStateOnStop;
    TestUtils::PublishEvent(g_respPageThirdAbilityST, 0, eventData);
}

void ThirdAbility::OnActive()
{
    APP_LOGI("ThirdAbility::OnActive");
    Ability::OnActive();
    std::string startBundleName = this->Split(targetBundle_, ",");
    std::string startAabilityName = this->Split(targetAbility_, ",");
    if (!startBundleName.empty() && !startAabilityName.empty()) {
        Want want;
        want.SetElementName(startBundleName, startAabilityName);
        want.SetParam("shouldReturn", shouldReturn_);
        if (!targetBundle_.empty() && !targetAbility_.empty()) {
            want.SetParam("targetBundle", targetBundle_);
            want.SetParam("targetAbility", targetAbility_);
        }
        StartAbility(want);
    }
    if (std::string::npos != shouldReturn_.find(GetAbilityName())) {
        TerminateAbility();
    }
    Clear();
    std::string eventData = GetAbilityName() + g_abilityStateOnActive;
    TestUtils::PublishEvent(g_respPageThirdAbilityST, 0, eventData);
}

void ThirdAbility::OnInactive()
{
    APP_LOGI("ThirdAbility::OnInactive");
    Ability::OnInactive();
    std::string eventData = GetAbilityName() + g_abilityStateOnInactive;
    TestUtils::PublishEvent(g_respPageThirdAbilityST, 0, eventData);
}

void ThirdAbility::OnBackground()
{
    APP_LOGI("ThirdAbility::OnBackground");
    Ability::OnBackground();
    std::string eventData = GetAbilityName() + g_abilityStateOnBackground;
    TestUtils::PublishEvent(g_respPageThirdAbilityST, 0, eventData);
}

void ThirdAbility::OnForeground(const Want &want)
{
    APP_LOGI("ThirdAbility::OnForeground");
    GetWantInfo(want);
    Ability::OnForeground(want);
    std::string eventData = GetAbilityName() + g_abilityStateOnForeground;
    TestUtils::PublishEvent(g_respPageThirdAbilityST, 0, eventData);
}

void ThirdAbility::OnNewWant(const Want &want)
{
    APP_LOGI("ThirdAbility::OnNewWant");
    GetWantInfo(want);
    Ability::OnNewWant(want);
    std::string eventData = GetAbilityName() + g_abilityStateOnNewWant;
    TestUtils::PublishEvent(g_respPageThirdAbilityST, 0, eventData);
}

void ThirdAbility::Clear()
{
    shouldReturn_ = "";
    targetBundle_ = "";
    targetAbility_ = "";
}

void ThirdAbility::GetWantInfo(const Want &want)
{
    Want mWant(want);
    shouldReturn_ = mWant.GetStringParam("shouldReturn");
    targetBundle_ = mWant.GetStringParam("targetBundle");
    targetAbility_ = mWant.GetStringParam("targetAbility");
}

std::string ThirdAbility::Split(std::string &str, std::string delim)
{
    std::string result;
    if (!str.empty()) {
        size_t index = str.find(delim);
        if (index != std::string::npos) {
            result = str.substr(0, index);
            str = str.substr(index + delim.size());
        } else {
            result = str;
            str = "";
        }
    }
    return result;
}

void KitTestThirdEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (g_requPageThirdAbilityST == eventName) {
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

void KitTestThirdEventSubscriber::ProcessInfoStByCode(int apiIndex, int caseIndex, int code)
{
    if (thirdAbility_ != nullptr) {
        thirdAbility_->ProcessInfoStByCode(apiIndex, caseIndex, code);
    }
}

void KitTestThirdEventSubscriber::KitTerminateAbility()
{
    if (thirdAbility_ != nullptr) {
        thirdAbility_->TerminateAbility();
    }
}

REGISTER_AA(ThirdAbility)
}  // namespace AppExecFwk
}  // namespace OHOS