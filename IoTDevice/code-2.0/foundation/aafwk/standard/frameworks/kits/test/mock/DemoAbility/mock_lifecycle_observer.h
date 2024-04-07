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

#ifndef FOUNDATION_APPEXECFWK_OHOS_MOCKLIFECYCLE_OBSERVER_INTERFACE_H
#define FOUNDATION_APPEXECFWK_OHOS_MOCKLIFECYCLE_OBSERVER_INTERFACE_H

#include "ability_lifecycle_observer_interface.h"
#include "ability_lifecycle.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
using Want = OHOS::AAFwk::Want;

class MockLifecycleObserver : public ILifecycleObserver {
public:
    MockLifecycleObserver() = default;
    virtual ~MockLifecycleObserver() = default;

    void OnActive() override;

    void OnBackground() override;

    void OnForeground(const Want &want) override;

    void OnInactive() override;

    void OnStart(const Want &want) override;

    void OnStop() override;

    void OnStateChanged(LifeCycle::Event event, const Want &want) override;

    void OnStateChanged(LifeCycle::Event event) override;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_MOCKLIFECYCLE_OBSERVER_INTERFACE_H