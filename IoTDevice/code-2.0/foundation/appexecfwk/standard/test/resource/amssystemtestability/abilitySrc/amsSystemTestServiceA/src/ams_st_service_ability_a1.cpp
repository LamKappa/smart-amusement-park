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

#include "ams_st_service_ability_a1.h"
#include "app_log_wrapper.h"
#include "common_event.h"
#include "common_event_manager.h"
using namespace OHOS::EventFwk;

namespace OHOS {
namespace AppExecFwk {
using AbilityConnectionProxy = OHOS::AAFwk::AbilityConnectionProxy;

int AmsStServiceAbilityA1::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
std::map<std::string, AmsStServiceAbilityA1::func> AmsStServiceAbilityA1::funcMap_ = {
    {"StartOtherAbility", &AmsStServiceAbilityA1::StartOtherAbility},
    {"ConnectOtherAbility", &AmsStServiceAbilityA1::ConnectOtherAbility},
    {"DisConnectOtherAbility", &AmsStServiceAbilityA1::DisConnectOtherAbility},
    {"StopSelfAbility", &AmsStServiceAbilityA1::StopSelfAbility},
};

AmsStServiceAbilityA1::~AmsStServiceAbilityA1()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

std::vector<std::string> AmsStServiceAbilityA1::Split(std::string str, const std::string &token)
{
    APP_LOGI("AmsStServiceAbilityA1::Split");

    std::vector<std::string> splitString;
    while (str.size()) {
        size_t index = str.find(token);
        if (index != std::string::npos) {
            splitString.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0) {
                splitString.push_back(str);
            }
        } else {
            splitString.push_back(str);
            str = "";
        }
    }
    return splitString;
}
void AmsStServiceAbilityA1::StartOtherAbility()
{
    APP_LOGI("AmsStServiceAbilityA1::StartOtherAbility begin targetBundle=%{public}s, targetAbility=%{public}s",
        targetBundle_.c_str(),
        targetAbility_.c_str());
    APP_LOGI("AmsStServiceAbilityA1::StartOtherAbility begin nextTargetBundleConn=%{public}s, "
             "nextTargetAbilityConn=%{public}s",
        nextTargetBundleConn_.c_str(),
        nextTargetAbilityConn_.c_str());

    if (!targetBundle_.empty() && !targetAbility_.empty()) {
        std::vector<std::string> strtargetBundles = Split(targetBundle_, ",");
        std::vector<std::string> strTargetAbilitys = Split(targetAbility_, ",");
        for (size_t i = 0; i < strtargetBundles.size() && i < strTargetAbilitys.size(); i++) {
            Want want;
            want.SetElementName(strtargetBundles[i], strTargetAbilitys[i]);
            want.SetParam("shouldReturn", shouldReturn_);
            want.SetParam("targetBundle", nextTargetBundle_);
            want.SetParam("targetAbility", nextTargetAbility_);
            want.SetParam("targetBundleConn", nextTargetBundleConn_);
            want.SetParam("targetAbilityConn", nextTargetAbilityConn_);
            StartAbility(want);
        }
    }
}
void AmsStServiceAbilityA1::ConnectOtherAbility()
{
    APP_LOGI(
        "AmsStServiceAbilityA1::ConnectOtherAbility begin targetBundleConn=%{public}s, targetAbilityConn=%{public}s",
        targetBundleConn_.c_str(),
        targetAbilityConn_.c_str());
    APP_LOGI("AmsStServiceAbilityA1::ConnectOtherAbility begin nextTargetBundleConn=%{public}s, "
             "nextTargetAbilityConn=%{public}s",
        nextTargetBundleConn_.c_str(),
        nextTargetAbilityConn_.c_str());

    // connect service ability
    if (!targetBundleConn_.empty() && !targetAbilityConn_.empty()) {
        std::vector<std::string> strtargetBundles = Split(targetBundleConn_, ",");
        std::vector<std::string> strTargetAbilitys = Split(targetAbilityConn_, ",");
        for (size_t i = 0; i < strtargetBundles.size() && i < strTargetAbilitys.size(); i++) {
            Want want;
            want.SetElementName(strtargetBundles[i], strTargetAbilitys[i]);
            want.SetParam("shouldReturn", shouldReturn_);
            want.SetParam("targetBundle", nextTargetBundle_);
            want.SetParam("targetAbility", nextTargetAbility_);
            want.SetParam("targetBundleConn", nextTargetBundleConn_);
            want.SetParam("targetAbilityConn", nextTargetAbilityConn_);
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            APP_LOGI("AmsStServiceAbilityA1::ConnectOtherAbility->ConnectAbility");
            bool ret = ConnectAbility(want, connCallback_);
            if (!ret) {
                APP_LOGE("AmsStServiceAbilityA1::ConnectAbility failed!");
            }
        }
    }
}
void AmsStServiceAbilityA1::DisConnectOtherAbility()
{
    APP_LOGI("AmsStServiceAbilityA1::DisConnectOtherAbility begin");
    if (connCallback_ != nullptr) {
        DisconnectAbility(connCallback_);
    }
    APP_LOGI("AmsStServiceAbilityA1::DisConnectOtherAbility end");
}

void AmsStServiceAbilityA1::StopSelfAbility()
{
    APP_LOGI("AmsStServiceAbilityA1::StopSelfAbility");

    TerminateAbility();
}

void AmsStServiceAbilityA1::OnStart(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityA1::OnStart");

    GetWantInfo(want);
    Ability::OnStart(want);
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnStart");
    SubscribeEvent();

    // make exception for test
    if (!zombie_.empty()) {
        std::unique_ptr<Want> pWant = nullptr;
        pWant->GetScheme();
    }
}
void AmsStServiceAbilityA1::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    APP_LOGI("AmsStServiceAbilityA1::OnCommand");

    GetWantInfo(want);
    Ability::OnCommand(want, restart, startId);
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnCommand");
}
void AmsStServiceAbilityA1::OnNewWant(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityA1::OnNewWant");

    GetWantInfo(want);
    Ability::OnNewWant(want);
}
void AmsStServiceAbilityA1::OnStop()
{
    APP_LOGI("AmsStServiceAbilityA1::OnStop");

    Ability::OnStop();
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INITIAL, "OnStop");
}
void AmsStServiceAbilityA1::OnActive()
{
    APP_LOGI("AmsStServiceAbilityA1::OnActive");

    Ability::OnActive();
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnActive");
}
void AmsStServiceAbilityA1::OnInactive()
{
    APP_LOGI("AmsStServiceAbilityA1::OnInactive");

    Ability::OnInactive();
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnInactive");
}
void AmsStServiceAbilityA1::OnBackground()
{
    APP_LOGI("AmsStServiceAbilityA1::OnBackground");

    Ability::OnBackground();
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnBackground");
}

void AmsStServiceAbilityA1::Clear()
{
    shouldReturn_ = "";
    targetBundle_ = "";
    targetAbility_ = "";
    targetBundleConn_ = "";
    targetAbilityConn_ = "";
    nextTargetBundle_ = "";
    nextTargetAbility_ = "";
    nextTargetBundleConn_ = "";
    nextTargetAbilityConn_ = "";
}
void AmsStServiceAbilityA1::GetWantInfo(const Want &want)
{
    Want mWant(want);
    shouldReturn_ = mWant.GetStringParam("shouldReturn");
    targetBundle_ = mWant.GetStringParam("targetBundle");
    targetAbility_ = mWant.GetStringParam("targetAbility");
    targetBundleConn_ = mWant.GetStringParam("targetBundleConn");
    targetAbilityConn_ = mWant.GetStringParam("targetAbilityConn");
    nextTargetBundle_ = mWant.GetStringParam("nextTargetBundle");
    nextTargetAbility_ = mWant.GetStringParam("nextTargetAbility");
    nextTargetBundleConn_ = mWant.GetStringParam("nextTargetBundleConn");
    nextTargetAbilityConn_ = mWant.GetStringParam("nextTargetAbilityConn");
    zombie_ = mWant.GetStringParam("zombie");
    AmsStServiceAbilityA1::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
}
bool AmsStServiceAbilityA1::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    APP_LOGI("AmsStServiceAbilityA1::PublishEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
        eventName.c_str(),
        code,
        data.c_str());

    Want want;
    want.SetAction(eventName);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}
sptr<IRemoteObject> AmsStServiceAbilityA1::OnConnect(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityA1::OnConnect");

    sptr<IRemoteObject> ret = Ability::OnConnect(want);
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnConnect");
    return ret;
}
void AmsStServiceAbilityA1::OnDisconnect(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityA1::OnDisconnect");

    Ability::OnDisconnect(want);
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnDisconnect");
}
bool AmsStServiceAbilityA1::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(APP_A1_REQ_EVENT_NAME);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility_ = this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}
void AmsStServiceAbilityA1::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    auto eventName = data.GetWant().GetAction();
    auto dataContent = data.GetData();
    APP_LOGI("AmsStServiceAbilityA1::OnReceiveEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
        eventName.c_str(),
        data.GetCode(),
        dataContent.c_str());
    if (APP_A1_REQ_EVENT_NAME.compare(eventName) == 0) {
        if (funcMap_.find(dataContent) == funcMap_.end()) {
            APP_LOGI(
                "AmsStServiceAbilityA1::OnReceiveEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
                eventName.c_str(),
                data.GetCode(),
                dataContent.c_str());
        } else {
            if (mainAbility_ != nullptr) {
                (mainAbility_->*funcMap_[dataContent])();
            }
        }
    }
}
REGISTER_AA(AmsStServiceAbilityA1);
}  // namespace AppExecFwk
}  // namespace OHOS