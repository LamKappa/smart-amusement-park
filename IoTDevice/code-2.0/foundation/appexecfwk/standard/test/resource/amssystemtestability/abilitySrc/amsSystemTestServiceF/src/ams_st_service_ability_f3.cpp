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

#include "app_log_wrapper.h"
#include "ams_st_service_ability_f3.h"
#include "common_event.h"
#include "common_event_manager.h"
using namespace OHOS::EventFwk;

namespace OHOS {
namespace AppExecFwk {
const std::string APP_F3_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_f3";

std::vector<std::string> AmsStServiceAbilityF3::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    APP_LOGI("AmsStServiceAbilityF3 <<<<GetFileTypes>>>>");
    PublishEvent(APP_F3_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "GetFileTypes");
    std::vector<std::string> fileType = {"filetypes"};
    return fileType;
}

void AmsStServiceAbilityF3::OnStart(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityF3::onStart");

    GetWantInfo(want);
    Ability::OnStart(want);
    PublishEvent(APP_F3_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnStart");
}
void AmsStServiceAbilityF3::OnNewWant(const Want &want)
{
    APP_LOGI("AmsStServiceAbilityF3::OnNewWant");

    GetWantInfo(want);
    Ability::OnNewWant(want);
}
void AmsStServiceAbilityF3::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    APP_LOGI("AmsStServiceAbilityF3::OnCommand");

    GetWantInfo(want);
    Ability::OnCommand(want, restart, startId);
    PublishEvent(APP_F3_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnCommand");
}
void AmsStServiceAbilityF3::OnStop()
{
    APP_LOGI("AmsStServiceAbilityF3::onStop");

    Ability::OnStop();
    PublishEvent(APP_F3_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INITIAL, "OnStop");
}
void AmsStServiceAbilityF3::OnActive()
{
    APP_LOGI("AmsStServiceAbilityF3::OnActive");

    Ability::OnActive();
    PublishEvent(APP_F3_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnActive");
}
void AmsStServiceAbilityF3::OnInactive()
{
    APP_LOGI("AmsStServiceAbilityF3::OnInactive");

    Ability::OnInactive();
    PublishEvent(APP_F3_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnInactive");
}
void AmsStServiceAbilityF3::OnBackground()
{
    APP_LOGI("AmsStServiceAbilityF3::OnBackground");

    Ability::OnBackground();
    PublishEvent(APP_F3_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnBackground");
}

void AmsStServiceAbilityF3::Clear()
{
    shouldReturn_ = "";
    targetBundle_ = "";
    targetAbility_ = "";
}
void AmsStServiceAbilityF3::GetWantInfo(const Want &want)
{
    Want mWant(want);
    shouldReturn_ = mWant.GetStringParam("shouldReturn");
    targetBundle_ = mWant.GetStringParam("targetBundle");
    targetAbility_ = mWant.GetStringParam("targetAbility");
}
bool AmsStServiceAbilityF3::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    APP_LOGI("AmsStServiceAbilityF3::PublishEvent eventName = %s, code = %d, data = %s",
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
REGISTER_AA(AmsStServiceAbilityF3);
}  // namespace AppExecFwk
}  // namespace OHOS