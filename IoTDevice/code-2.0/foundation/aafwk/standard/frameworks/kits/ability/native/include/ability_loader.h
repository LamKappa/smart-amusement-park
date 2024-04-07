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

#ifndef FOUNDATION_APPEXECFWK_OHOS_ABILITY_LOADER_H
#define FOUNDATION_APPEXECFWK_OHOS_ABILITY_LOADER_H

#include "ability.h"
#include "ohos_application.h"
#include <functional>
#include <string>
#include <unordered_map>

namespace OHOS {
namespace AppExecFwk {
using CreateAblity = std::function<Ability *(void)>;
#ifdef ABILITY_WINDOW_SUPPORT
using CreateSlice = std::function<AbilitySlice *(void)>;
#endif
/**
 * @brief Declares functions for registering the class names of {@link Ability} and {@link AbilitySlice} with the
 *        ability management framework.
 *
 * After creating your own {@link Ability} and {@link AbilitySlice} child classes, you should register their class
 * names with the ability management framework so that the application can start <b>Ability</b> instances created in
 * the background.
 *
 * @since 1
 * @version 1
 */
class AbilityLoader {
public:
    static AbilityLoader &GetInstance();

    ~AbilityLoader() = default;

    /**
     * @brief Register Ability Info
     *
     * @param abilityName ability classname
     * @param createFunc  Constructor address
     */
    void RegisterAbility(const std::string &abilityName, const CreateAblity &createFunc);

    /**
     * @brief Get Ability address
     *
     * @param abilityName ability classname
     *
     * @return return Ability address
     */
    Ability *GetAbilityByName(const std::string &abilityName);

#ifdef ABILITY_WINDOW_SUPPORT
    void RegisterAbilitySlice(const std::string &sliceName, const CreateSlice &createFunc);
    AbilitySlice *GetAbilitySliceByName(const std::string &sliceName);
#endif

private:
    AbilityLoader() = default;
    AbilityLoader(const AbilityLoader &) = delete;
    AbilityLoader &operator=(const AbilityLoader &) = delete;
    AbilityLoader(AbilityLoader &&) = delete;
    AbilityLoader &operator=(AbilityLoader &&) = delete;

    std::unordered_map<std::string, CreateAblity> abilities_;
};
/**
 * @brief Registers the class name of an {@link Ability} child class.
 *
 * After implementing your own {@link Ability} class, you should call this function so that the ability management
 * framework can create <b>Ability</b> instances when loading your <b>Ability</b> class.
 *
 * @param className Indicates the {@link Ability} class name to register.
 */
#define REGISTER_AA(className)                                                       \
    __attribute__((constructor)) void RegisterAA##className()                        \
    {                                                                                \
        AbilityLoader::GetInstance().RegisterAbility(                                \
            #className, []() -> Ability * { return new (std::nothrow) className; }); \
    }

/**
 * @brief Registers the class name of an {@link AbilitySlice} child class.
 *
 * After implementing your own {@link AbilitySlice} class, you should call this function so that the ability
 * management framework can create <b>AbilitySlice</b> instances when loading your <b>AbilitySlice</b> class.
 *
 * @param className Indicates the {@link AbilitySlice} class name to register.
 */
#ifdef ABILITY_WINDOW_SUPPORT
#define REGISTER_AS(className)                              \
    __attribute((constructor)) void RegisterAS##className() \
    {                                                       \
        AbilityLoader::GetInstance().RegisterAbilitySlice(  \
            #className, []() -> AbilitySlice * { return new (std::nothrow) className; });
}
#endif
}  // namespace OHOS
}  // OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_ABILITY_LOADER_H