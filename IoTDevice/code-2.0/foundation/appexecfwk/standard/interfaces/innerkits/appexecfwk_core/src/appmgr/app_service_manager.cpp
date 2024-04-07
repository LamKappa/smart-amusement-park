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

#include "app_service_manager.h"

#include "ipc_skeleton.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"

#include "iservice_registry.h"
#include "app_mgr_constants.h"

namespace OHOS {
namespace AppExecFwk {

sptr<IRemoteObject> AppServiceManager::GetAppMgrService() const
{
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityMgr) {
        return nullptr;
    }
    return systemAbilityMgr->GetSystemAbility(APP_MGR_SERVICE_ID);
}

}  // namespace AppExecFwk
}  // namespace OHOS
