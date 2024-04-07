
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

#include "base_ability.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

void BaseAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("BaseAbility::Init called.");

    Ability::Init(abilityInfo, application, handler, token);
}

void BaseAbility::OnStart(const Want &want)
{
    APP_LOGI("BaseAbility::OnStart");

    Ability::OnStart(want);
}

void BaseAbility::OnStop()
{
    APP_LOGI("BaseAbility::OnStop");

    Ability::OnStop();
}

void BaseAbility::OnActive()
{
    APP_LOGI("BaseAbility::OnActive");

    Ability::OnActive();
}

void BaseAbility::OnInactive()
{
    APP_LOGI("BaseAbility::OnInactive");

    Ability::OnInactive();
}

void BaseAbility::OnBackground()
{
    APP_LOGI("BaseAbility::OnBackground");

    Ability::OnBackground();
}

void BaseAbility::OnForeground(const Want &want)
{
    APP_LOGI("BaseAbility::OnForeground");

    Ability::OnForeground(want);
}

void BaseAbility::OnCommand(const Want &want, bool restart, int startId)
{
    APP_LOGI("BaseAbility::OnCommand");

    Ability::OnCommand(want, restart, startId);
}

sptr<IRemoteObject> BaseAbility::OnConnect(const Want &want)
{
    APP_LOGI("BaseAbility::OnConnect");

    sptr<IRemoteObject> ret = Ability::OnConnect(want);
    return ret;
}

void BaseAbility::OnDisconnect(const Want &want)
{
    APP_LOGI("BaseAbility::OnDisconnect");

    Ability::OnDisconnect(want);
}

void BaseAbility::OnNewWant(const Want &want)
{
    APP_LOGI("BaseAbility::OnNewWant");

    Ability::OnNewWant(want);
}

void BaseAbility::OnAbilityResult(int requestCode, int resultCode, const Want &resultData)
{
    APP_LOGI("BaseAbility::OnAbilityResult");

    Ability::OnAbilityResult(requestCode, resultCode, resultData);
}

void BaseAbility::OnBackPressed()
{
    APP_LOGI("BaseAbility::OnBackPressed");

    Ability::OnBackPressed();
}

std::string BaseAbility::GetNoFromWantInfo(const Want &want)
{
    Want wantImpl(want);
    return wantImpl.GetStringParam("No.");
}
}  // namespace AppExecFwk
}  // namespace OHOS