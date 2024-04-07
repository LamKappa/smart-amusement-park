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

#include "amsstabilityn1.h"

namespace OHOS {
namespace AppExecFwk {
void AmsStAbilityN1::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("AmsStAbilityN1::Init");
    Ability::Init(abilityInfo, application, handler, token);
    pageAbilityEvent.SubscribeEvent(STEventName::g_eventList, shared_from_this());
    SendPath("Init");
}

void AmsStAbilityN1::OnStart(const Want &want)
{
    GetWantInfo(want);
    APP_LOGI("AmsStAbilityN1::onStart");
    Ability::OnStart(want);
    std::string eventData = GetAbilityName() + STEventName::g_abilityStateOnStart;
    pageAbilityEvent.PublishEvent(STEventName::g_eventName, pageAbilityEvent.GetOnStartCount(), eventData);
    SendPath("OnStart");
}

void AmsStAbilityN1::OnNewWant(const Want &want)
{
    APP_LOGI("AmsStAbilityN1::OnNewWant");
    Ability::OnNewWant(want);
    std::string eventData = GetAbilityName() + STEventName::g_abilityStateOnNewWant;
    pageAbilityEvent.PublishEvent(STEventName::g_eventName, pageAbilityEvent.GetOnNewWantCount(), eventData);
    SendPath("OnNewWant");
}

void AmsStAbilityN1::OnForeground(const Want &want)
{
    APP_LOGI("AmsStAbilityN1::OnForeground");
    Ability::OnForeground(want);
    std::string eventData = GetAbilityName() + STEventName::g_abilityStateOnForeground;
    pageAbilityEvent.PublishEvent(STEventName::g_eventName, pageAbilityEvent.GetOnForegroundCount(), eventData);
    SendPath("OnForeground");
}

void AmsStAbilityN1::OnStop()
{
    APP_LOGI("AmsStAbilityN1::onStop");
    Ability::OnStop();
    pageAbilityEvent.UnsubscribeEvent();
    std::string eventData = GetAbilityName() + STEventName::g_abilityStateOnStop;
    pageAbilityEvent.PublishEvent(STEventName::g_eventName, pageAbilityEvent.GetOnStopCount(), eventData);
    SendPath("OnStop");
}

void AmsStAbilityN1::OnActive()
{
    APP_LOGI("AmsStAbilityN1::OnActive");
    Ability::OnActive();
    std::string startBundleName = this->Split(targetBundle, ",");
    std::string startAbilityName = this->Split(targetAbility, ",");
    if (!startBundleName.empty() && !startAbilityName.empty()) {
        Want want;
        want.SetElementName(startBundleName, startAbilityName);
        want.SetParam("shouldReturn", shouldReturn);
        if (!targetBundle.empty() && !targetAbility.empty()) {
            want.SetParam("targetBundle", targetBundle);
            want.SetParam("targetAbility", targetAbility);
        }
        StartAbility(want);
    }
    if (std::string::npos != shouldReturn.find(GetAbilityName())) {
        TerminateAbility();
    }
    Clear();
    std::string eventData = GetAbilityName() + STEventName::g_abilityStateOnActive;
    pageAbilityEvent.PublishEvent(STEventName::g_eventName, pageAbilityEvent.GetOnActiveCount(), eventData);
    SendPath("OnActive");
}

void AmsStAbilityN1::OnInactive()
{
    APP_LOGI("AmsStAbilityN1::OnInactive");
    Ability::OnInactive();
    std::string eventData = GetAbilityName() + STEventName::g_abilityStateOnInactive;
    pageAbilityEvent.PublishEvent(STEventName::g_eventName, pageAbilityEvent.GetOnInactiveCount(), eventData);
    SendPath("OnInactive");
}

void AmsStAbilityN1::OnBackground()
{
    APP_LOGI("AmsStAbilityN1::OnBackground");
    Ability::OnBackground();
    std::string eventData = GetAbilityName() + STEventName::g_abilityStateOnBackground;
    pageAbilityEvent.PublishEvent(STEventName::g_eventName, pageAbilityEvent.GetOnBackgroundCount(), eventData);
    SendPath("OnBackground");
}

void AmsStAbilityN1::Clear()
{
    shouldReturn = "";
    targetBundle = "";
    targetAbility = "";
}

void AmsStAbilityN1::GetWantInfo(const Want &want)
{
    Want mWant(want);
    shouldReturn = mWant.GetStringParam("shouldReturn");
    targetBundle = mWant.GetStringParam("targetBundle");
    targetAbility = mWant.GetStringParam("targetAbility");
}

std::string AmsStAbilityN1::Split(std::string &str, std::string delim)
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

void AmsStAbilityN1::SendPath(const std::string &callBackPath)
{
    std::string callBack = pageAbilityEvent.GetCallBackPath(callBackPath);
    std::string abilityStatus = pageAbilityEvent.GetAbilityStatus(std::to_string(Ability::GetState()));
    std::string callBackPathEventData = Ability::GetAbilityName() + callBack;
    std::string abilityStatusEventData = Ability::GetAbilityName() + abilityStatus;
    pageAbilityEvent.PublishEvent(STEventName::g_eventName, STEventName::eventCode, callBackPathEventData);
    pageAbilityEvent.PublishEvent(STEventName::g_eventName, STEventName::eventCode, abilityStatusEventData);
}

REGISTER_AA(AmsStAbilityN1);
}  // namespace AppExecFwk
}  // namespace OHOS