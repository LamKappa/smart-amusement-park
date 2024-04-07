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

#ifndef OHOS_OS_AAFWK_SYS_MRG_CLIENT_H
#define OHOS_OS_AAFWK_SYS_MRG_CLIENT_H

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "if_system_ability_manager.h"
#include "iremote_object.h"
#include "singleton.h"

namespace OHOS {
namespace AppExecFwk {

class SysMrgClient {
    DECLARE_DELAYED_SINGLETON(SysMrgClient)
public:
    /**
     *
     * Get the systemAbility by ID.
     *
     * @param systemAbilityId The ID of systemAbility which want to get.
     */
    sptr<IRemoteObject> GetSystemAbility(const int32_t systemAbilityId);

    /**
     *
     * Register the systemAbility by ID.
     *
     * @param systemAbilityId The ID of systemAbility which want to register.
     * @param broker The systemAbility which want to be registered.
     */
    void RegisterSystemAbility(const int32_t systemAbilityId, sptr<IRemoteObject> broker);

    /**
     *
     * Unregister the systemAbility by ID.
     *
     * @param systemAbilityId The ID of systemAbility which want to unregister.
     */
    void UnregisterSystemAbility(const int32_t systemAbilityId);

private:
    OHOS::sptr<ISystemAbilityManager> sm_;
    std::mutex saMutex_;
    std::unordered_map<int32_t, sptr<IRemoteObject>> servicesMap_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_OS_AAFWK_SYS_MRG_CLIENT_H
