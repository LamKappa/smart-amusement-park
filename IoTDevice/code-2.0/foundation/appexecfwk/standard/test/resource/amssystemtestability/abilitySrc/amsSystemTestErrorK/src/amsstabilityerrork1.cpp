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

#include "amsstabilityerrork1.h"

namespace OHOS {
namespace AppExecFwk {
void AmsStAbilityErrorK1::OnStart(const Want &want)
{
    Want mWant(want);
    shouldReturn = mWant.GetStringParam("shouldReturn");
    targetBundle = mWant.GetStringParam("targetBundle");
    targetAbility = mWant.GetStringParam("targetAbility");
    int errorCode1 = 0;
    int errorCode2 = 0;
    int errorCode3 = 0;
    errorCode1 = errorCode2 / errorCode3;

    APP_LOGI("AmsStAbilityErrorK1::onStart");
    Ability::OnStart(want);
}

void AmsStAbilityErrorK1::OnStop()
{
    APP_LOGI("AmsStAbilityErrorK1::onStop");
    Ability::OnStop();
}

void AmsStAbilityErrorK1::OnActive()
{
    APP_LOGI("AmsStAbilityErrorK1::OnActive");
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

void AmsStAbilityErrorK1::OnInactive()
{
    APP_LOGI("AmsStAbilityErrorK1::OnInactive");
    Ability::OnInactive();
}

void AmsStAbilityErrorK1::OnBackground()
{
    APP_LOGI("AmsStAbilityErrorK1::OnBackground");
    Ability::OnBackground();
}

REGISTER_AA(AmsStAbilityErrorK1);
}  // namespace AppExecFwk
}  // namespace OHOS