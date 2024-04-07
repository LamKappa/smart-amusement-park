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

#include "ams_st_service_ability_c4.h"
#include "app_log_wrapper.h"
#include "common_event.h"
#include "common_event_manager.h"
using namespace OHOS::EventFwk;

namespace OHOS {
namespace AppExecFwk {
const std::string APP_C4_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_c4";
const std::string APP_C4_REQ_EVENT_NAME = "req_com_ohos_amsst_service_app_c4";

std::map<std::string, AmsStServiceAbilityC4::func> AmsStServiceAbilityC4::funcMap_ = {
    {"DisConnectOtherAbility", &AmsStServiceAbilityC4::DisConnectOtherAbility},
    {"StopSelfAbility", &AmsStServiceAbilityC4::StopSelfAbility},
};

AmsStServiceAbilityC4::~AmsStServiceAbilityC4()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void AmsStServiceAbilityC4::OnStart(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityC4::OnStart");

    GetWantInfo(want);
    Ability::OnStart(want);
    PublishEvent(APP_C4_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnStart");
    SubscribeEvent();
}
void AmsStServiceAbilityC4::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    APP_LOGI("AmsStServiceAbilityC4::OnCommand");

    GetWantInfo(want);
    Ability::OnCommand(want, restart, startId);
    PublishEvent(APP_C4_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnCommand");
}
void AmsStServiceAbilityC4::OnNewWant(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityC4::OnNewWant");

    GetWantInfo(want);
    Ability::OnNewWant(want);
}
void AmsStServiceAbilityC4::OnStop()
{
    APP_LOGI("AmsStServiceAbilityC4::OnStop");

    Ability::OnStop();
    PublishEvent(APP_C4_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INITIAL, "OnStop");
}
void AmsStServiceAbilityC4::OnActive()
{
    APP_LOGI("AmsStServiceAbilityC4::OnActive");

    Ability::OnActive();
    PublishEvent(APP_C4_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnActive");
}
void AmsStServiceAbilityC4::OnInactive()
{
    APP_LOGI("AmsStServiceAbilityC4::OnInactive");

    Ability::OnInactive();
    PublishEvent(APP_C4_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnInactive");
}
void AmsStServiceAbilityC4::OnBackground()
{
    APP_LOGI("AmsStServiceAbilityC4::OnBackground");

    Ability::OnBackground();
    PublishEvent(APP_C4_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnBackground");
}
sptr<IRemoteObject> AmsStServiceAbilityC4::OnConnect(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityC4::OnConnect");

    sptr<IRemoteObject> ret = Ability::OnConnect(want);
    PublishEvent(APP_C4_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnConnect");
    return ret;
}
void AmsStServiceAbilityC4::OnDisconnect(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityC4::OnDisconnect");

    Ability::OnDisconnect(want);
    PublishEvent(APP_C4_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnDisconnect");
}
void AmsStServiceAbilityC4::StopSelfAbility()
{
    APP_LOGI("AmsStServiceAbilityC4::StopSelfAbility");

    TerminateAbility();
}
void AmsStServiceAbilityC4::DisConnectOtherAbility()
{
    APP_LOGI("AmsStServiceAbilityC4::DisConnectOtherAbility begin");

    APP_LOGI("AmsStServiceAbilityB3::DisConnectOtherAbility end");
}
void AmsStServiceAbilityC4::Clear()
{
    shouldReturn_ = "";
    targetBundle_ = "";
    targetAbility_ = "";
}
void AmsStServiceAbilityC4::GetWantInfo(const Want &want)
{
    Want mWant(want);
    shouldReturn_ = mWant.GetStringParam("shouldReturn");
    targetBundle_ = mWant.GetStringParam("targetBundle");
    targetAbility_ = mWant.GetStringParam("targetAbility");
}
bool AmsStServiceAbilityC4::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    APP_LOGI("AmsStServiceAbilityC4::PublishEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
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

bool AmsStServiceAbilityC4::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(APP_C4_REQ_EVENT_NAME);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility_ = this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void AmsStServiceAbilityC4::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    auto eventName = data.GetWant().GetAction();
    auto dataContent = data.GetData();
    APP_LOGI("AmsStServiceAbilityC4::OnReceiveEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
        eventName.c_str(),
        data.GetCode(),
        dataContent.c_str());
    if (APP_C4_REQ_EVENT_NAME.compare(eventName) == 0) {
        if (funcMap_.find(dataContent) == funcMap_.end()) {
            APP_LOGI(
                "AmsStServiceAbilityC4::OnReceiveEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
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
REGISTER_AA(AmsStServiceAbilityC4);
}  // namespace AppExecFwk
}  // namespace OHOS