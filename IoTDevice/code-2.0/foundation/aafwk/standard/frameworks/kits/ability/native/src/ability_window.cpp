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

#include "ability_window.h"
#include "ability.h"
#include "ability_handler.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
AbilityWindow::AbilityWindow()
{}

AbilityWindow::~AbilityWindow()
{}

#ifdef WMS_COMPILE
/**
 * @brief Init the AbilityWindow object.
 *
 * @param handler The EventHandler of the Ability the AbilityWindow belong.
 */
void AbilityWindow::Init(std::shared_ptr<AbilityHandler> &handler, std::shared_ptr<Ability> ability)
{
    APP_LOGI("AbilityWindow::Init called.");
    handler_ = handler;
    ability_ = std::weak_ptr<IAbilityEvent>(ability);
}

/**
 * @brief Sets the window config for the host ability to create window.
 *
 * @param config Indicates window config.
 */
bool AbilityWindow::SetWindowConfig(const WindowConfig &config)
{
    APP_LOGI("AbilityWindow::SetWindowConfig called.");

    APP_LOGI("config format = %{public}d, width = %{public}d, height = %{public}d.",
        config.format,
        config.width,
        config.height);
    APP_LOGI("config pos_x = %{public}d, pos_y = %{public}d, stride = %{public}d, type = %{public}d.",
        config.pos_x,
        config.pos_y,
        config.stride,
        config.type);
    window_ = (WindowManager::GetInstance()->CreateWindow(const_cast<WindowConfig *>(&config)));
    if (window_.get() == nullptr) {
        APP_LOGE("AbilityWindow::SetWindowConfig the window is nullptr.");
        return false;
    }

    auto callback = [abilityWindow = this](KeyEvent event) -> bool { return abilityWindow->OnKeyEvent(event); };
    window_->RegistOnKeyCb(callback);

    isWindowAttached = true;
    APP_LOGI("AbilityWindow::SetWindowConfig end.");

    return true;
}

/**
 * @brief Called when the KeyEvent sent.
 *
 * @param KeyEvent the key event.
 *
 * @return Returns true if the listener has processed the event; returns false otherwise.
 *
 */
bool AbilityWindow::OnKeyEvent(KeyEvent event)
{
    APP_LOGI("AbilityWindow::OnKeyEvent called.");
    bool ret = false;
    std::shared_ptr<IAbilityEvent> ability = nullptr;
    ability = ability_.lock();
    if (ability == nullptr) {
        APP_LOGE("AbilityWindow::OnKeyEvent ability is nullptr.");
        return ret;
    }
    switch (event.GetKeyCode()) {
        case KeyEvent::CODE_BACK:
            APP_LOGI("AbilityWindow::OnKeyEvent Back key pressed.");
            ret = OnBackPressed(ability);
            break;
        default:
            APP_LOGI("AbilityWindow::OnKeyEvent the key event is %{public}d.", event.GetKeyCode());
            break;
    }
    return ret;
}

/**
 * @brief Called back when the Back key is pressed.
 *
 * @param ability The ability receive the event.
 *
 * @return Returns true if the listener has processed the event; returns false otherwise.
 *
 */
bool AbilityWindow::OnBackPressed(std::shared_ptr<IAbilityEvent> &ability)
{
    APP_LOGI("AbilityWindow::OnBackPressed called.");
    if (handler_ == nullptr) {
        APP_LOGE("AbilityWindow::OnBackPressed handler_ is nullptr.");
        return false;
    }
    auto task = [abilityRun = ability]() { abilityRun->OnBackPressed(); };
    handler_->PostTask(task);
    return true;
}
#endif  // WMS_COMPILE

/**
 * @brief Called when this ability is started.
 *
 */
void AbilityWindow::OnPostAbilityStart()
{
    APP_LOGI("AbilityWindow::OnPostAbilityStart called.");
    if (!isWindowAttached) {
        APP_LOGE("AbilityWindow::OnPostAbilityStart window not attached.");
        return;
    }

#ifdef WMS_COMPILE
    if (window_ != nullptr) {
        APP_LOGI("AbilityWindow::widow::Hide called.");
        window_->Hide();
    }
#endif  // WMS_COMPILE
    APP_LOGI("AbilityWindow::OnPostAbilityStart end.");
}

/**
 * @brief Called when this ability is activated.
 *
 */
void AbilityWindow::OnPostAbilityActive()
{
    APP_LOGI("AbilityWindow::OnPostAbilityActive called.");
    if (!isWindowAttached) {
        APP_LOGE("AbilityWindow::OnPostAbilityActive window not attached.");
        return;
    }

#ifdef WMS_COMPILE
    if (window_ != nullptr) {
        APP_LOGI("AbilityWindow::widow::SwitchTop called.");
        window_->SwitchTop();
        APP_LOGI("AbilityWindow::widow::Show called.");
        window_->Show();
    }
#endif  // WMS_COMPILE
    APP_LOGI("AbilityWindow::OnPostAbilityActive end.");
}

/**
 * @brief Called when this ability is inactivated.
 *
 */
void AbilityWindow::OnPostAbilityInactive()
{
    APP_LOGI("AbilityWindow::OnPostAbilityInactive called.");
    if (!isWindowAttached) {
        APP_LOGE("AbilityWindow::OnPostAbilityInactive window not attached.");
        return;
    }

#ifdef WMS_COMPILE
    if (window_ != nullptr) {
        APP_LOGI("AbilityWindow::widow::Hide called.");
        window_->Hide();
    }
#endif  // WMS_COMPILE
    APP_LOGI("AbilityWindow::OnPostAbilityInactive end.");
}

/**
 * @brief Called when this ability is background.
 *
 */
void AbilityWindow::OnPostAbilityBackground()
{
    APP_LOGI("AbilityWindow::OnPostAbilityBackground called.");
    if (!isWindowAttached) {
        APP_LOGE("AbilityWindow::OnPostAbilityBackground window not attached.");
        return;
    }

#ifdef WMS_COMPILE
    if (window_ != nullptr) {
        APP_LOGI("AbilityWindow::widow::Hide called.");
        window_->Hide();
    }
#endif  // WMS_COMPILE
    APP_LOGI("AbilityWindow::OnPostAbilityBackground end.");
}

/**
 * @brief Called when this ability is foreground.
 *
 */
void AbilityWindow::OnPostAbilityForeground()
{
    APP_LOGI("AbilityWindow::OnPostAbilityForeground called.");
    if (!isWindowAttached) {
        APP_LOGE("AbilityWindow::OnPostAbilityForeground window not attached.");
        return;
    }

#ifdef WMS_COMPILE
    if (window_ != nullptr) {
        APP_LOGI("AbilityWindow::widow::Show called.");
        window_->Show();
    }
#endif  // WMS_COMPILE
    APP_LOGI("AbilityWindow::OnPostAbilityForeground end.");
}

/**
 * @brief Called when this ability is stopped.
 *
 */
void AbilityWindow::OnPostAbilityStop()
{
    APP_LOGI("AbilityWindow::OnPostAbilityStop called.");
    if (!isWindowAttached) {
        APP_LOGE("AbilityWindow::OnPostAbilityStop window not attached.");
        return;
    }

#ifdef WMS_COMPILE
    if (window_ != nullptr) {
        int32_t windowID = window_->GetWindowID();
        APP_LOGI("AbilityWindow::widow::DestroyWindow called windowID=%{public}d.", windowID);
        WindowManager::GetInstance()->DestroyWindow(windowID);
    }
#endif  // WMS_COMPILE
    isWindowAttached = false;
    APP_LOGI("AbilityWindow::OnPostAbilityStop end.");
}
#ifdef WMS_COMPILE
/**
 * @brief Get the window belong to the ability.
 *
 * @return Returns a Window object pointer.
 */
const std::unique_ptr<Window> &AbilityWindow::GetWindow()
{
    if (!isWindowAttached) {
        APP_LOGE("AbilityWindow::GetWindow window not attached.");
    }

    if (window_.get() == nullptr) {
        APP_LOGE("AbilityWindow::GetWindow the window is nullptr.");
    }
    return window_;
}
#endif  // WMS_COMPILE
}  // namespace AppExecFwk
}  // namespace OHOS