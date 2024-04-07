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

#ifndef OHOS_ABILITY_WINDOW_H
#define OHOS_ABILITY_WINDOW_H

#define WMS_COMPILE

#ifdef WMS_COMPILE
#include "window_manager.h"
#endif  // WMS_COMPILE

#include <map>
#include "nocopyable.h"

namespace OHOS {
namespace AppExecFwk {
class IAbilityEvent;
class Ability;
class AbilityHandler;
class AbilityWindow : public NoCopyable {
public:
    AbilityWindow();
    virtual ~AbilityWindow();
#ifdef WMS_COMPILE
    /**
     * @brief Init the AbilityWindow object.
     *
     * @param handler The EventHandler of the Ability the AbilityWindow belong.
     */
    void Init(std::shared_ptr<AbilityHandler> &handler, std::shared_ptr<Ability> ability);

    /**
     * @brief Sets the window config for the host ability to create window.
     *
     * @param config Indicates window config.
     */
    bool SetWindowConfig(const WindowConfig &config);

    /**
     * @brief Called when the KeyEvent sent.
     *
     * @param KeyEvent the key event.
     *
     * @return Returns true if the listener has processed the event; returns false otherwise.
     *
     */
    bool OnKeyEvent(KeyEvent event);

    /**
     * @brief Called back when the Back key is pressed.
     *
     * @param ability The ability receive the event.
     *
     * @return Returns true if the listener has processed the event; returns false otherwise.
     *
     */
    bool OnBackPressed(std::shared_ptr<IAbilityEvent> &ability);
#endif  // WMS_COMPILE
    /**
     * @brief Called when this ability is started.
     *
     */
    void OnPostAbilityStart();

    /**
     * @brief Called when this ability is activated.
     *
     */
    void OnPostAbilityActive();

    /**
     * @brief Called when this ability is inactivated.
     *
     */
    void OnPostAbilityInactive();

    /**
     * @brief Called when this ability is background.
     *
     */
    void OnPostAbilityBackground();

    /**
     * @brief Called when this ability is foreground.
     *
     */
    void OnPostAbilityForeground();

    /**
     * @brief Called when this ability is stopped.
     *
     */
    void OnPostAbilityStop();

    /**
     * @brief Get the window belong to the ability.
     *
     * @return Returns a Window object pointer.
     */
#ifdef WMS_COMPILE
    const std::unique_ptr<Window> &GetWindow();
#endif  // WMS_COMPILE
private:
#ifdef WMS_COMPILE
    std::shared_ptr<AbilityHandler> handler_ = nullptr;
    std::weak_ptr<IAbilityEvent> ability_;
    std::unique_ptr<Window> window_;
#endif  // WMS_COMPILE
    bool isWindowAttached = false;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_WINDOW_H