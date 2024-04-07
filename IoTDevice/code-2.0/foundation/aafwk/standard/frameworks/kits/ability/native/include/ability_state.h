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

/**
 * @addtogroup AbilityKit
 * @{
 *
 * @brief Provides ability-related functions, including ability lifecycle callbacks and functions for connecting to
 *        or disconnecting from AAs.
 *
 * Abilities are classified into Feature Abilities (FAs) and Atomic Abilities (AAs). FAs support the Page template,
 * and AAs support the Service template. An ability using the Page template is called Page ability for short and that
 * using the Service template is called Service ability.
 *
 * @since 1
 * @version 1
 */

/**
 * @file ability_state.h
 *
 * @brief Declares ability-related functions, including ability lifecycle callbacks and functions for connecting to
 *        or disconnecting from AAs.
 * As the fundamental unit of OpenHarmony, abilities are classified into Feature Abilities (FAs) and Atomic
 * Abilities (AAs). FAs support the Page template, and AAs support the Service template. An ability using the Page
 * template is called Page ability for short and that using the Service template is called Service ability.
 *
 * @since 1
 * @version 1
 */
#ifndef OHOS_ABILITY_STATE_H
#define OHOS_ABILITY_STATE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
/**
 * @brief Enumerates all lifecycle states that an ability will go through over the course of its lifetime.
 *
 * @since 1
 * @version 1
 */
typedef enum {
    /**
     * Initial state: An ability is in this state after it is initialized or stopped.
     */
    STATE_INITIAL,

    /**
     * Inactive state: An ability is in this state when it is switched to the foreground but is not interactive.
     */
    STATE_INACTIVE,

    /**
     * Active state: An ability is in this state when it is switched to the foreground and is interactive.
     */
    STATE_ACTIVE,

    /**
     * Background state: An ability is in this state after it returns to the background.
     */
    STATE_BACKGROUND,
    STATE_SUSPENDED,
    STATE_INACTIVATING,
    STATE_ACTIVATING,
    STATE_MOVING_BACKGROUND,
    STATE_TERMINATING,
} State;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // OHOS_ABILITY_STATE_H
