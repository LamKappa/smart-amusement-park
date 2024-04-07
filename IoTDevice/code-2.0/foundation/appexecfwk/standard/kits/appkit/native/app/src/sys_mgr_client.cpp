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

#include "sys_mgr_client.h"

#include "app_log_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "ipc_skeleton.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {

SysMrgClient::SysMrgClient() : abilityManager_(nullptr)
{}

SysMrgClient::~SysMrgClient()
{}

/**
 *
 * Get the systemAbility by ID.
 *
 * @param systemAbilityId The ID of systemAbility which want to get.
 */
sptr<IRemoteObject> SysMrgClient::GetSystemAbility(const int32_t systemAbilityId)
{
    // use single instance of abilityManager_
    if (abilityManager_ == nullptr) {
        std::lock_guard<std::mutex> lock(saMutex_);
        if (abilityManager_ == nullptr) {
            abilityManager_ = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (abilityManager_ == nullptr) {
                APP_LOGE("fail to GetSystemAbility abilityManager_ == nullptr.");
                return nullptr;
            }
        }
    }
    return abilityManager_->GetSystemAbility(systemAbilityId);
}

/**
 *
 * Register the systemAbility by ID.
 *
 * @param systemAbilityId The ID of systemAbility which want to register.
 * @param broker The systemAbility which want to be registered.
 */
void SysMrgClient::RegisterSystemAbility(
    const int32_t __attribute__((unused)) systemAbilityId, sptr<IRemoteObject> __attribute__((unused)) broker)
{
    (void)servicesMap_;
}

/**
 *
 * Unregister the systemAbility by ID.
 *
 * @param systemAbilityId The ID of systemAbility which want to unregister.
 */
void SysMrgClient::UnregisterSystemAbility(const int32_t systemAbilityId)
{}

}  // namespace AppExecFwk
}  // namespace OHOS
