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

#include "ams_st_service_ability_d1.h"
#include "app_log_wrapper.h"
using namespace OHOS::EventFwk;

namespace OHOS {
namespace AppExecFwk {
const std::string APP_D1_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_d1";
const std::string APP_D1_REQ_EVENTNAME = "req_com_ohos_amsst_service_app_d1";
int AmsStServiceAbilityD1::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
std::map<std::string, AmsStServiceAbilityD1::func> AmsStServiceAbilityD1::funcMap_ = {
    {"StartOtherAbility", &AmsStServiceAbilityD1::StartOtherAbility},
    {"ConnectOtherAbility", &AmsStServiceAbilityD1::ConnectOtherAbility},
    {"DisConnectOtherAbility", &AmsStServiceAbilityD1::DisConnectOtherAbility},
    {"StopSelfAbility", &AmsStServiceAbilityD1::StopSelfAbility},
    {"GetDataByDataAbility", &AmsStServiceAbilityD1::GetDataByDataAbility},
};

AmsStServiceAbilityD1::~AmsStServiceAbilityD1()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

std::vector<std::string> AmsStServiceAbilityD1::Split(std::string str, const std::string &token)
{
    APP_LOGI("AmsStServiceAbilityD1::Split");

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
void AmsStServiceAbilityD1::StartOtherAbility()
{
    APP_LOGI("AmsStServiceAbilityD1::StartOtherAbility begin targetBundle=%{public}s, targetAbility=%{public}s",
        targetBundle_.c_str(),
        targetAbility_.c_str());
    APP_LOGI("AmsStServiceAbilityD1::StartOtherAbility begin nextTargetBundleConn=%{public}s, "
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
            want.SetParam("nextTargetBundleConn", nextTargetBundleConn_);
            want.SetParam("nextTargetAbilityConn", nextTargetAbilityConn_);
            StartAbility(want);
        }
    }
}
void AmsStServiceAbilityD1::ConnectOtherAbility()
{
    APP_LOGI(
        "AmsStServiceAbilityD1::ConnectOtherAbility begin targetBundleConn=%{public}s, targetAbilityConn=%{public}s",
        targetBundleConn_.c_str(),
        targetAbilityConn_.c_str());
    APP_LOGI("AmsStServiceAbilityD1::ConnectOtherAbility begin nextTargetBundleConn=%{public}s, "
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
            want.SetParam("nextTargetBundleConn", nextTargetBundleConn_);
            want.SetParam("nextTargetAbilityConn", nextTargetAbilityConn_);
            stub_.push_back(new (std::nothrow) AbilityConnectCallback());
            connCallback_.push_back(new (std::nothrow) AbilityConnectionProxy(stub_[i]));
            APP_LOGI("AmsStServiceAbilityD1::ConnectOtherAbility->ConnectAbility");
            bool ret = ConnectAbility(want, connCallback_[i]);
            if (!ret) {
                APP_LOGE("AmsStServiceAbilityD1::ConnectAbility failed!");
            }
        }
    }
    APP_LOGI("AmsStServiceAbilityD1::ConnectOtherAbility end");
}

void AmsStServiceAbilityD1::DisConnectOtherAbility()
{
    APP_LOGI("AmsStServiceAbilityD1::DisConnectOtherAbility begin");
    for (auto callBack : connCallback_) {
        DisconnectAbility(callBack);
    }
    APP_LOGI("AmsStServiceAbilityD1::DisConnectOtherAbility end");
}

void AmsStServiceAbilityD1::StopSelfAbility()
{
    APP_LOGI("AmsStServiceAbilityD1::StopSelfAbility");

    TerminateAbility();
}

void AmsStServiceAbilityD1::OnStart(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityD1::OnStart");

    GetWantInfo(want);
    Ability::OnStart(want);
    PublishEvent(APP_D1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnStart");
    SubscribeEvent();
}
void AmsStServiceAbilityD1::OnNewWant(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityD1::OnNewWant");

    GetWantInfo(want);
    Ability::OnNewWant(want);
}
void AmsStServiceAbilityD1::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    APP_LOGI("AmsStServiceAbilityD1::OnCommand");

    GetWantInfo(want);
    Ability::OnCommand(want, restart, startId);
    PublishEvent(APP_D1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnCommand");
}
void AmsStServiceAbilityD1::OnStop()
{
    APP_LOGI("AmsStServiceAbilityD1::OnStop");

    Ability::OnStop();
    PublishEvent(APP_D1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INITIAL, "OnStop");
}
void AmsStServiceAbilityD1::GetDataByDataAbility()
{
    APP_LOGI("AmsStServiceAbilityD1::GetDataByDataAbility");

    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(GetContext());
    if (helper == nullptr) {
        APP_LOGE("AmsStServiceAbilityD1::GetDataByDataAbility:helper == nullptr");
        return;
    }

    Uri uri2("dataability:///com.ohos.amsst.service.appF.dataability");
    std::string mimeTypeFilter("mimeTypeFiltertest");
    std::vector<std::string> result = helper->GetFileTypes(uri2, mimeTypeFilter);

    int count = result.size();
    if (count > 0) {
        APP_LOGI("AmsStServiceAbilityD1::GetDataByDataAbility get data ability data info result > 0!");
        PublishEvent(APP_D1_RESP_EVENT_NAME, 1, "GetDataByDataAbility");
    } else {
        APP_LOGI("AmsStServiceAbilityD1::GetDataByDataAbility get data ability data info result = 0!");
        PublishEvent(APP_D1_RESP_EVENT_NAME, 0, "GetDataByDataAbility");
    }
}
void AmsStServiceAbilityD1::OnActive()
{
    APP_LOGI("AmsStServiceAbilityD1::OnActive");

    Ability::OnActive();
    PublishEvent(APP_D1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnActive");
}
void AmsStServiceAbilityD1::OnInactive()
{
    APP_LOGI("AmsStServiceAbilityD1::OnInactive");

    Ability::OnInactive();
    PublishEvent(APP_D1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnInactive");
}
void AmsStServiceAbilityD1::OnBackground()
{
    APP_LOGI("AmsStServiceAbilityD1::OnBackground");

    Ability::OnBackground();
    PublishEvent(APP_D1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnBackground");
}

void AmsStServiceAbilityD1::Clear()
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
    AmsStServiceAbilityD1::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
}
void AmsStServiceAbilityD1::GetWantInfo(const Want &want)
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
    AmsStServiceAbilityD1::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
}
bool AmsStServiceAbilityD1::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    APP_LOGI("AmsStServiceAbilityD1::PublishEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
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
bool AmsStServiceAbilityD1::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(APP_D1_REQ_EVENTNAME);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility_ = this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}
void AmsStServiceAbilityD1::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    auto eventName = data.GetWant().GetAction();
    auto dataContent = data.GetData();
    APP_LOGI("AmsStServiceAbilityD1::OnReceiveEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
        eventName.c_str(),
        data.GetCode(),
        dataContent.c_str());
    if (APP_D1_REQ_EVENTNAME.compare(eventName) == 0) {
        if (funcMap_.find(dataContent) == funcMap_.end()) {
            APP_LOGI(
                "AmsStServiceAbilityD1::OnReceiveEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
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
REGISTER_AA(AmsStServiceAbilityD1);
}  // namespace AppExecFwk
}  // namespace OHOS