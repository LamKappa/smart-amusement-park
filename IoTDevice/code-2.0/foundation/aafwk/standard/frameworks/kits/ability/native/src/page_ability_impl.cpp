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

#include "page_ability_impl.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
using AbilityManagerClient = OHOS::AAFwk::AbilityManagerClient;
/**
 * @brief Handling the life cycle switching of PageAbility.
 *
 * @param want Indicates the structure containing information about the ability.
 * @param targetState The life cycle state to switch to.
 *
 */
void PageAbilityImpl::HandleAbilityTransaction(const Want &want, const AAFwk::LifeCycleStateInfo &targetState)
{
    APP_LOGI("PageAbilityImpl::sourceState:%{public}d; targetState: %{public}d; isNewWant: %{public}d",
        lifecycleState_,
        targetState.state,
        targetState.isNewWant);
    if ((lifecycleState_ == targetState.state) && !targetState.isNewWant) {
        APP_LOGE("Org lifeCycleState equals to Dst lifeCycleState.");
        return;
    }

    if (lifecycleState_ == AAFwk::ABILITY_STATE_INITIAL) {
        Start(want);
    }

    if (lifecycleState_ == AAFwk::ABILITY_STATE_ACTIVE) {
        Inactive();
    }

    bool ret = false;
    ret = AbilityTransaction(want, targetState);
    if (ret) {
        AbilityManagerClient::GetInstance()->AbilityTransitionDone(token_, targetState.state);
    }
}

/**
 * @brief Handling the life cycle switching of PageAbility in switch.
 *
 * @param want Indicates the structure containing information about the ability.
 * @param targetState The life cycle state to switch to.
 *
 * @return return true if the lifecycle transaction successfully, otherwise return false.
 *
 */
bool PageAbilityImpl::AbilityTransaction(const Want &want, const AAFwk::LifeCycleStateInfo &targetState)
{
    APP_LOGE("PageAbilityImpl::AbilityTransaction called.");
    bool ret = true;
    switch (targetState.state) {
        case AAFwk::ABILITY_STATE_INITIAL: {
            if (lifecycleState_ == AAFwk::ABILITY_STATE_INACTIVE) {
                Background();
            }
            Stop();
            break;
        }
        case AAFwk::ABILITY_STATE_INACTIVE: {
            if (lifecycleState_ == AAFwk::ABILITY_STATE_BACKGROUND) {
                Foreground(want);
            }
            break;
        }
        case AAFwk::ABILITY_STATE_ACTIVE: {
            if (lifecycleState_ == AAFwk::ABILITY_STATE_BACKGROUND) {
                Foreground(want);
            }
            if (targetState.isNewWant) {
                NewWant(want);
            }
            SerUriString(targetState.caller.deviceId + "/" + targetState.caller.bundleName + "/" +
                         targetState.caller.abilityName);
            Active();
            break;
        }
        case AAFwk::ABILITY_STATE_BACKGROUND: {
            if (lifecycleState_ == AAFwk::ABILITY_STATE_INACTIVE) {
                Background();
            }
            break;
        }
        default: {
            ret = false;
            APP_LOGE("PageAbilityImpl::HandleAbilityTransaction state error");
            break;
        }
    }
    return ret;
}

/**
 * @brief Execution the KeyDown callback of the ability
 * @param keyCode Indicates the code of the key pressed.
 * @param keyEvent Indicates the key-down event.
 *
 * @return Returns true if this event is handled and will not be passed further; returns false if this event is
 * not handled and should be passed to other handlers.
 *
 */
bool PageAbilityImpl::DoKeyDown(int keyCode, const KeyEvent &keyEvent)
{
    if (ability_ == nullptr) {
        APP_LOGE("PageAbilityImpl::DoKeyDown ability_ == nullptr");
        return false;
    }

    return ability_->OnKeyDown(keyCode, keyEvent);
}

/**
 * @brief Execution the KeyUp callback of the ability
 * @param keyCode Indicates the code of the key released.
 * @param keyEvent Indicates the key-up event.
 *
 * @return Returns true if this event is handled and will not be passed further; returns false if this event is
 * not handled and should be passed to other handlers.
 *
 */
bool PageAbilityImpl::DoKeyUp(int keyCode, const KeyEvent &keyEvent)
{
    if (ability_ == nullptr) {
        APP_LOGE("PageAbilityImpl::DoKeyUp ability_ == nullptr");
        return false;
    }

    return ability_->OnKeyUp(keyCode, keyEvent);
}

/**
 * @brief Called when a touch event is dispatched to this ability. The default implementation of this callback
 * does nothing and returns false.
 * @param touchEvent Indicates information about the touch event.
 *
 * @return Returns true if the event is handled; returns false otherwise.
 *
 */
bool PageAbilityImpl::DoTouchEvent(const TouchEvent &touchEvent)
{
    if (ability_ == nullptr) {
        APP_LOGE("PageAbilityImpl::DoTouchEvent ability_ == nullptr");
        return false;
    }

    return ability_->OnTouchEvent(touchEvent);
}
}  // namespace AppExecFwk
}  // namespace OHOS