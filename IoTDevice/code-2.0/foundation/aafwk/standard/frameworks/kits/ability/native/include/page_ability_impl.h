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

#ifndef FOUNDATION_APPEXECFWK_OHOS_PAGE_ABILITY_IMPL_H
#define FOUNDATION_APPEXECFWK_OHOS_PAGE_ABILITY_IMPL_H

#include "ability_impl.h"

namespace OHOS {
namespace AppExecFwk {
class Ability;
class AbilityHandler;
class AbilityLocalRecord;
class AbilityImpl;
class PageAbilityImpl final : public AbilityImpl {
public:
    /**
     * @brief Constructor.
     *
     */
    PageAbilityImpl() = default;

    /**
     * @brief Destructor.
     *
     */
    ~PageAbilityImpl() = default;

    /**
     * @brief Handling the life cycle switching of PageAbility.
     *
     * @param want Indicates the structure containing information about the ability.
     * @param targetState The life cycle state to switch to.
     *
     */
    void HandleAbilityTransaction(const Want &want, const AAFwk::LifeCycleStateInfo &targetState);

    /**
     * @brief Handling the life cycle switching of PageAbility in switch.
     *
     * @param want Indicates the structure containing information about the ability.
     * @param targetState The life cycle state to switch to.
     *
     * @return return true if the lifecycle transaction successfully, otherwise return false.
     *
     */
    bool AbilityTransaction(const Want &want, const AAFwk::LifeCycleStateInfo &targetState);

    /**
     * @brief Execution the KeyDown callback of the ability
     * @param keyCode Indicates the code of the key pressed.
     * @param keyEvent Indicates the key-down event.
     *
     * @return Returns true if this event is handled and will not be passed further; returns false if this event is
     * not handled and should be passed to other handlers.
     *
     */
    bool DoKeyDown(int keyCode, const KeyEvent &keyEvent);

    /**
     * @brief Execution the KeyUp callback of the ability
     * @param keyCode Indicates the code of the key released.
     * @param keyEvent Indicates the key-up event.
     *
     * @return Returns true if this event is handled and will not be passed further; returns false if this event is
     * not handled and should be passed to other handlers.
     *
     */
    bool DoKeyUp(int keyCode, const KeyEvent &keyEvent);

    /**
     * @brief Called when a touch event is dispatched to this ability. The default implementation of this callback
     * does nothing and returns false.
     * @param touchEvent Indicates information about the touch event.
     *
     * @return Returns true if the event is handled; returns false otherwise.
     *
     */
    bool DoTouchEvent(const TouchEvent &touchEvent);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_PAGE_ABILITY_IMPL_H