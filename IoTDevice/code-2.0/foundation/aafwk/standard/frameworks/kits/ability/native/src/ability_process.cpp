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

#include "ability_process.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
std::shared_ptr<AbilityProcess> AbilityProcess::instance_ = nullptr;
std::map<Ability *, std::map<int, CallbackInfo>> AbilityProcess::abilityResultMap_;
std::map<Ability *, std::map<int, CallbackInfo>> AbilityProcess::abilityRequestPermissionsForUserMap_;
std::mutex AbilityProcess::mutex_;
std::shared_ptr<AbilityProcess> AbilityProcess::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<AbilityProcess>();
        }
    }
    return instance_;
}

AbilityProcess::AbilityProcess()
{}

AbilityProcess::~AbilityProcess()
{}

void AbilityProcess::StartAbility(Ability *ability, CallAbilityParam param, CallbackInfo callback)
{
    APP_LOGI("AbilityProcess::StartAbility called");
    if (ability == nullptr) {
        APP_LOGE("AbilityProcess::StartAbility ability is nullptr");
        return;
    }

    if (param.forResultOption == true) {
        if (param.setting == nullptr) {
            ability->StartAbilityForResult(param.want, param.requestCode);
        } else {
            ability->StartAbilityForResult(param.want, param.requestCode, *(param.setting.get()));
        }

        std::lock_guard<std::mutex> lock_l(mutex_);

        std::map<int, CallbackInfo> map;
        auto it = abilityResultMap_.find(ability);
        if (it == abilityResultMap_.end()) {
            APP_LOGI("AbilityProcess::StartAbility ability: %{public}p is not in the abilityResultMap_", ability);
        } else {
            APP_LOGI("AbilityProcess::StartAbility ability: %{public}p is in the abilityResultMap_", ability);
            map = it->second;
        }

        map[param.requestCode] = callback;
        abilityResultMap_[ability] = map;
    } else {
        if (param.setting == nullptr) {
            ability->StartAbility(param.want);
        } else {
            ability->StartAbility(param.want, *(param.setting.get()));
        }
    }
}

void AbilityProcess::OnAbilityResult(Ability *ability, int requestCode, int resultCode, const Want &resultData)
{
    APP_LOGI("AbilityProcess::OnAbilityResult called");

    std::lock_guard<std::mutex> lock_l(mutex_);

    auto it = abilityResultMap_.find(ability);
    if (it == abilityResultMap_.end()) {
        APP_LOGE("AbilityProcess::OnAbilityResult ability: %{public}p is not in the abilityResultMap", ability);
        return;
    }
    std::map<int, CallbackInfo> map = it->second;

    auto callback = map.find(requestCode);
    if (callback == map.end()) {
        APP_LOGE("AbilityProcess::OnAbilityResult requestCode: %{public}d is not in the map", requestCode);
        return;
    }
    CallbackInfo callbackInfo = callback->second;

    CallOnAbilityResult(requestCode, resultCode, resultData, callbackInfo);

    map.erase(requestCode);

    abilityResultMap_[ability] = map;
}

void AbilityProcess::RequestPermissionsFromUser(
    Ability *ability, CallAbilityPermissionParam &param, CallbackInfo callbackInfo)
{
    APP_LOGI("AbilityProcess::RequestPermissionsFromUser called");
    if (ability == nullptr) {
        APP_LOGE("AbilityProcess::RequestPermissionsFromUser ability is nullptr");
        return;
    }

    ability->RequestPermissionsFromUser(param.permission_list, param.requestCode);

    {
        std::lock_guard<std::mutex> lock_l(mutex_);
        std::map<int, CallbackInfo> map;
        auto it = abilityRequestPermissionsForUserMap_.find(ability);
        if (it == abilityRequestPermissionsForUserMap_.end()) {
            APP_LOGI("AbilityProcess::RequestPermissionsFromUser ability: %{public}p is not in the "
                     "abilityRequestPermissionsForUserMap_",
                ability);
        } else {
            APP_LOGI("AbilityProcess::RequestPermissionsFromUser ability: %{public}p is in the "
                     "abilityRequestPermissionsForUserMap_",
                ability);
            map = it->second;
        }

        map[param.requestCode] = callbackInfo;
        abilityRequestPermissionsForUserMap_[ability] = map;
    }
}

void AbilityProcess::OnRequestPermissionsFromUserResult(Ability *ability, int requestCode,
    const std::vector<std::string> &permissions, const std::vector<int> &grantResults)
{
    if (ability == nullptr) {
        APP_LOGE("AbilityProcess::OnRequestPermissionsFromUserResult ability is nullptr");
        return;
    }

    std::lock_guard<std::mutex> lock_l(mutex_);

    auto it = abilityRequestPermissionsForUserMap_.find(ability);
    if (it == abilityRequestPermissionsForUserMap_.end()) {
        APP_LOGE("AbilityProcess::OnRequestPermissionsFromUserResult ability: %{public}p is not in the "
                 "abilityRequestPermissionsForUserMap_",
            ability);
        return;
    }
    std::map<int, CallbackInfo> map = it->second;

    auto callback = map.find(requestCode);
    if (callback == map.end()) {
        APP_LOGE("AbilityProcess::OnRequestPermissionsFromUserResult requestCode: %{public}d is not in the map",
            requestCode);
        return;
    }
    CallbackInfo callbackInfo = callback->second;
    CallOnRequestPermissionsFromUserResult(requestCode, permissions, grantResults, callbackInfo);
    map.erase(requestCode);

    abilityRequestPermissionsForUserMap_[ability] = map;
}
}  // namespace AppExecFwk
}  // namespace OHOS