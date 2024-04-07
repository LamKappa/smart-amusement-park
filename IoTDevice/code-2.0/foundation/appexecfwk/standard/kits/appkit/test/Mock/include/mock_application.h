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

#ifndef FOUNDATION_APPEXECFWK_KITS_APPKIT_TEST_MOCK_APPLICATION_H
#define FOUNDATION_APPEXECFWK_KITS_APPKIT_TEST_MOCK_APPLICATION_H

#include <gtest/gtest.h>
#include "ohos_application.h"

namespace OHOS {
namespace AppExecFwk {

class MockModuleLifecycleCallbacks : public AbilityLifecycleCallbacks {
public:
    MockModuleLifecycleCallbacks() = default;
    virtual ~MockModuleLifecycleCallbacks() = default;

    void OnAbilityStart(const std::shared_ptr<Ability> &ability);
    void OnAbilityInactive(const std::shared_ptr<Ability> &ability);
    void OnAbilityBackground(const std::shared_ptr<Ability> &ability);
    void OnAbilityForeground(const std::shared_ptr<Ability> &ability);
    void OnAbilityActive(const std::shared_ptr<Ability> &ability);
    void OnAbilityStop(const std::shared_ptr<Ability> &ability);
    void OnAbilitySaveState(const PacMap &outState);
};

class MockModuleElementsCallback : public ElementsCallback {
public:
    MockModuleElementsCallback() = default;
    virtual ~MockModuleElementsCallback() = default;

    virtual void OnConfigurationUpdated(const std::shared_ptr<Ability> &ability, const Configuration &config);

    virtual void OnMemoryLevel(int level);
};

class MockApplication : public OHOSApplication {
public:
    MockApplication();
    virtual ~MockApplication() = default;

    virtual void OnConfigurationUpdated(const Configuration &config);
    virtual void OnMemoryLevel(int level);
    virtual void OnForeground();
    virtual void OnBackground();
    virtual void OnStart();
    virtual void OnTerminate();

private:
    std::shared_ptr<ElementsCallback> elementCallBack_ = nullptr;
    std::shared_ptr<AbilityLifecycleCallbacks> lifecycleCallBack_ = nullptr;
    int level_ = 0;
    std::shared_ptr<Configuration> config_ = nullptr;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_KITS_APPKIT_TEST_MOCK_APPLICATION_H