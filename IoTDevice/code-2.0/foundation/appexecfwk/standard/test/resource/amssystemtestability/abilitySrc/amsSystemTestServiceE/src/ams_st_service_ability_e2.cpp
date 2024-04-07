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

#include "ams_st_service_ability_e2.h"
#include "app_log_wrapper.h"
#include "ability_context.h"
#include "ability.h"

using namespace OHOS::EventFwk;

namespace OHOS {
namespace AppExecFwk {
int AmsStServiceAbilityE2::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
std::map<std::string, AmsStServiceAbilityE2::func> AmsStServiceAbilityE2::funcMap_ = {
    {"StartOtherAbility", &AmsStServiceAbilityE2::StartOtherAbility},
    {"ConnectOtherAbility", &AmsStServiceAbilityE2::ConnectOtherAbility},
    {"DisConnectOtherAbility", &AmsStServiceAbilityE2::DisConnectOtherAbility},
};

AmsStServiceAbilityE2::~AmsStServiceAbilityE2()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

std::vector<std::string> AmsStServiceAbilityE2::Split(std::string str, const std::string &token)
{
    APP_LOGI("AmsStServiceAbilityE2::Split");

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
void AmsStServiceAbilityE2::StartOtherAbility()
{
    APP_LOGI("AmsStServiceAbilityE2::StartOtherAbility begin targetBundle=%{public}s, targetAbility=%{public}s",
        targetBundle_.c_str(),
        targetAbility_.c_str());
    APP_LOGI("AmsStServiceAbilityE2::StartOtherAbility begin nextTargetBundleConn=%{public}s, "
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
void AmsStServiceAbilityE2::ConnectOtherAbility()
{
    APP_LOGI(
        "AmsStServiceAbilityE2::ConnectOtherAbility begin targetBundleConn=%{public}s, targetAbilityConn=%{public}s",
        targetBundleConn_.c_str(),
        targetAbilityConn_.c_str());
    APP_LOGI("AmsStServiceAbilityE2::ConnectOtherAbility begin nextTargetBundleConn=%{public}s, "
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
            APP_LOGI("AmsStServiceAbilityE2::ConnectOtherAbility->ConnectAbility");
            bool ret = ConnectAbility(want, connCallback_[i]);
            if (!ret) {
                APP_LOGE("AmsStServiceAbilityE2::ConnectAbility failed!");
            }
        }
    }
    APP_LOGI("AmsStServiceAbilityE2::ConnectOtherAbility end");
}
void AmsStServiceAbilityE2::OnStart(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityE2::OnStart");

    GetWantInfo(want);
    Ability::OnStart(want);
    PublishEvent(APP_E2_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnStart");
    SubscribeEvent();
}
void AmsStServiceAbilityE2::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    APP_LOGI("AmsStServiceAbilityE2::OnCommand");

    GetWantInfo(want);
    Ability::OnCommand(want, restart, startId);
    PublishEvent(APP_E2_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnCommand");
}
void AmsStServiceAbilityE2::OnNewWant(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityE2::OnNewWant");

    GetWantInfo(want);
    Ability::OnNewWant(want);
}
void AmsStServiceAbilityE2::DisConnectOtherAbility()
{
    APP_LOGI("AmsStServiceAbilityE2::DisConnectOtherAbility begin");
    if (connCallback_.size() > 0) {
        DisconnectAbility(connCallback_[0]);
    }
    APP_LOGI("AmsStServiceAbilityE2::DisConnectOtherAbility end");
}

void AmsStServiceAbilityE2::OnStop()
{
    APP_LOGI("AmsStServiceAbilityE2::onStop");

    Ability::OnStop();
    PublishEvent(APP_E2_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INITIAL, "OnStop");
}
void AmsStServiceAbilityE2::GetDataByDataAbility()
{
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(GetContext());
    if (helper == nullptr) {
        APP_LOGE("AmsStServiceAbilityE2::GetDataByDataAbility:helper == nullptr");
        return;
    }

    Uri uri2("dataability:///com.ohos.amsst.service.appF.dataability");
    std::string mimeTypeFilter("mimeTypeFiltertest");
    std::vector<std::string> result = helper->GetFileTypes(uri2, mimeTypeFilter);

    int count = result.size();
    if (count > 0) {
        APP_LOGI("AmsStServiceAbilityE2::OnBackground get data ability data info result > 0!");
        PublishEvent(APP_E2_RESP_EVENT_NAME, 1, "GetDataByDataAbility");
    } else {
        APP_LOGI("AmsStServiceAbilityE2::OnBackground get data ability data info result = 0!");
        PublishEvent(APP_E2_RESP_EVENT_NAME, 0, "GetDataByDataAbility");
    }
}
void AmsStServiceAbilityE2::OnActive()
{
    APP_LOGI("AmsStServiceAbilityE2::OnActive");

    Ability::OnActive();
    PublishEvent(APP_E2_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnActive");
}
void AmsStServiceAbilityE2::OnInactive()
{
    APP_LOGI("AmsStServiceAbilityE2::OnInactive");

    Ability::OnInactive();
    PublishEvent(APP_E2_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnInactive");
}
void AmsStServiceAbilityE2::OnBackground()
{
    APP_LOGI("AmsStServiceAbilityE2::OnBackground");

    Ability::OnBackground();
    PublishEvent(APP_E2_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnBackground");
}
sptr<IRemoteObject> AmsStServiceAbilityE2::OnConnect(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityE2::OnConnect");

    Ability::OnConnect(want);
    PublishEvent(APP_E2_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnConnect");
    return nullptr;
}
void AmsStServiceAbilityE2::OnDisconnect(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityE2::OnDisconnect");

    Ability::OnDisconnect(want);
    PublishEvent(APP_E2_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnDisconnect");
}
void AmsStServiceAbilityE2::Clear()
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
    AmsStServiceAbilityE2::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
}
void AmsStServiceAbilityE2::GetWantInfo(const Want &want)
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
    AmsStServiceAbilityE2::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
}
bool AmsStServiceAbilityE2::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    APP_LOGI("AmsStServiceAbilityE2::PublishEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
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
bool AmsStServiceAbilityE2::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(APP_E2_REQ_EVENT_NAME);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility_ = this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}
void AmsStServiceAbilityE2::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    auto eventName = data.GetWant().GetAction();
    auto dataContent = data.GetData();
    APP_LOGI("AmsStServiceAbilityE2::OnReceiveEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
        eventName.c_str(),
        data.GetCode(),
        dataContent.c_str());
    if (APP_E2_REQ_EVENT_NAME.compare(eventName) == 0) {
        if (funcMap_.find(dataContent) == funcMap_.end()) {
            APP_LOGI(
                "AmsStServiceAbilityE2::OnReceiveEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
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
REGISTER_AA(AmsStServiceAbilityE2);
}  // namespace AppExecFwk
}  // namespace OHOS