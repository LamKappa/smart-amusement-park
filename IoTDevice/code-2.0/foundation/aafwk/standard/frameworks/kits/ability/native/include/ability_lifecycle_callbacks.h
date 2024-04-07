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

#ifndef FOUNDATION_APPEXECFWK_OHOS_ABILITY_LIFECYCLE_CALLBACK_H
#define FOUNDATION_APPEXECFWK_OHOS_ABILITY_LIFECYCLE_CALLBACK_H

#include "pac_map.h"

namespace OHOS {
namespace AppExecFwk {
class Ability;
class AbilityLifecycleCallbacks {
public:
    AbilityLifecycleCallbacks() = default;
    virtual ~AbilityLifecycleCallbacks() = default;

    /**
     *
     * Will be called when the given ability calls Ability->onStart
     *
     * @param Ability Indicates the ability object that calls the onStart() method.
     */
    virtual void OnAbilityStart(const std::shared_ptr<Ability> &ability) = 0;

    /**
     *
     * Will be called when the given ability calls Ability->onInactive
     *
     * @param Ability Indicates the Ability object that calls the onInactive() method.
     */
    virtual void OnAbilityInactive(const std::shared_ptr<Ability> &ability) = 0;

    /**
     *
     * Will be called when the given ability calls Ability->onBackground
     *
     * @param Ability Indicates the Ability object that calls the onBackground() method.
     */
    virtual void OnAbilityBackground(const std::shared_ptr<Ability> &ability) = 0;

    /**
     *
     * Will be called when the given ability calls Ability->onForeground
     *
     * @param Ability Indicates the Ability object that calls the onForeground() method.
     */
    virtual void OnAbilityForeground(const std::shared_ptr<Ability> &ability) = 0;

    /**
     *
     * Will be called when the given ability calls Ability->onActive
     *
     * @param Ability Indicates the Ability object that calls the onActive() method.
     */
    virtual void OnAbilityActive(const std::shared_ptr<Ability> &ability) = 0;

    /**
     *
     * Will be called when the given ability calls Ability->onStop
     *
     * @param Ability Indicates the Ability object that calls the onStop() method.
     */
    virtual void OnAbilityStop(const std::shared_ptr<Ability> &ability) = 0;

    /**
     *
     * Will be Called when an ability calls Ability#onSaveAbilityState(PacMap).
     *
     * @param outState Indicates the PacMap object passed to the onSaveAbilityState() callback.
     */
    virtual void OnAbilitySaveState(const PacMap &outState) = 0;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_ABILITY_LIFECYCLE_CALLBACK_H