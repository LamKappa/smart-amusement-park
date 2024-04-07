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
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "ipc_skeleton.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {

sptr<IRemoteObject> SysMrgClient::GetSystemAbility(const int32_t systemAbilityId)
{
    if (servicesMap_[systemAbilityId] == nullptr) {
        OHOS::sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityManager == nullptr) {
            HILOG_ERROR("%s:fail to get Registry", __func__);
            return nullptr;
        }
        OHOS::sptr<OHOS::IRemoteObject> object = systemAbilityManager->GetSystemAbility(systemAbilityId);
        servicesMap_[systemAbilityId] = object;
    }
    return servicesMap_[systemAbilityId];
}

void SysMrgClient::RegisterSystemAbility(const int32_t systemAbilityId, sptr<IRemoteObject> broker)
{
    servicesMap_[systemAbilityId] = broker;
}

}  // namespace AppExecFwk
}  // namespace OHOS
