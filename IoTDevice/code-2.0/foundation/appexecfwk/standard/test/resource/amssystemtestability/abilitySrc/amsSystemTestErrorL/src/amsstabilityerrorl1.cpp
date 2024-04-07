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

#include "amsstabilityerrorl1.h"

namespace OHOS {
namespace AppExecFwk {
void AmsStAbilityErrorL1::OnStart(const Want &want)
{
    Want mWant(want);
    shouldReturn = mWant.GetStringParam("shouldReturn");
    targetBundle = mWant.GetStringParam("targetBundle");
    targetAbility = mWant.GetStringParam("targetAbility");

    APP_LOGI("AmsStAbilityErrorL1::onStart");
    Ability::OnStart(want);
}

void AmsStAbilityErrorL1::OnStop()
{
    APP_LOGI("AmsStAbilityErrorL1::onStop");
    Ability::OnStop();
}

void AmsStAbilityErrorL1::OnActive()
{
    APP_LOGI("AmsStAbilityErrorL1::OnActive");
    Ability::OnActive();
    if (!targetBundle.empty() && !targetAbility.empty()) {
        Want want;
        want.SetElementName(targetBundle, targetAbility);
        want.SetParam("shouldReturn", shouldReturn);
        StartAbility(want);
    }
    if (std::string::npos != shouldReturn.find(GetAbilityName())) {
        TerminateAbility();
    }
}

void AmsStAbilityErrorL1::OnInactive()
{
    APP_LOGI("AmsStAbilityErrorL1::OnInactive");
    Ability::OnInactive();
}

void AmsStAbilityErrorL1::OnBackground()
{
    APP_LOGI("AmsStAbilityErrorL1::OnBackground");
    Ability::OnBackground();
}

REGISTER_AA(AmsStAbilityErrorL1);
}  // namespace AppExecFwk
}  // namespace OHOS