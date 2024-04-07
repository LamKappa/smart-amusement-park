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

#ifndef FOUNDATION_APPEXECFWK_OHOS_ABILITY_IMPL_FACTORY_H
#define FOUNDATION_APPEXECFWK_OHOS_ABILITY_IMPL_FACTORY_H

#include "ability_impl.h"
#include "singleton.h"

namespace OHOS {
namespace AppExecFwk {
class AbilityImplFactory {
    DECLARE_DELAYED_SINGLETON(AbilityImplFactory)
public:
    /**
     * @brief Create impl object based on abilitytype
     *
     * @param type AbilityType:PAGE/SERVICE/PROVIDER
     *
     * @return AbilityImpl object
     */
    std::shared_ptr<AbilityImpl> MakeAbilityImplObject(const std::shared_ptr<AbilityInfo> &info);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_ABILITY_IMPL_FACTORY_H