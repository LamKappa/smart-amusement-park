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

#ifndef FOUNDATION_APPEXECFWK_OHOS_ABILITY_LIFECYCLE_H
#define FOUNDATION_APPEXECFWK_OHOS_ABILITY_LIFECYCLE_H

#include <vector>
#include <string>
#include <list>
#include <refbase.h>
#include <memory>
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
using Want = OHOS::AAFwk::Want;
class ILifecycleObserver;
class LifeCycle {
public:
    LifeCycle() = default;
    virtual ~LifeCycle() = default;

    enum Event { ON_ACTIVE, ON_BACKGROUND, ON_FOREGROUND, ON_INACTIVE, ON_START, ON_STOP, UNDEFINED };

    /**
     * @brief Obtains the current lifecycle event.
     * Lifecycle events drive lifecycle state changes. Therefore, you are able to know the lifecycle state
     * once you obtain the lifecycle event. For example, if the ON_ACTIVE event is received, the ability or
     * ability slice is in the ACTIVE state; if the ON_FOREGROUND event is received, the ability or ability
     * slice is changing from the BACKGROUND state to INACTIVE.
     *
     * @return Returns the current lifecycle event.
     */
    LifeCycle::Event GetLifecycleState();

    /**
     * @brief Adds a lifecycle observer.
     * The observer will be notified of lifecycle changes.
     *
     * @param observer Indicates the lifecycle observer, either LifecycleObserver or LifecycleStateObserver.
     * The value cannot be null.
     *
     */
    void AddObserver(const std::shared_ptr<ILifecycleObserver> &observer);

    /**
     * @brief While Ability's lifecycle changes, dispatch lifecycle event.
     *
     * @param event  Lifecycle state.
     * @param want  Indicates the Want containing information about the target ability to change lifecycle state.
     */
    void DispatchLifecycle(const LifeCycle::Event &event, const Want &want);

    /**
     * @brief While Ability's lifecycle changes, dispatch lifecycle event.
     *
     * @param event  Lifecycle state.
     * @param want  Indicates the Want containing information about the target ability to change lifecycle state.
     */
    void DispatchLifecycle(const LifeCycle::Event &event);

    /**
     * @brief Removes a lifecycle observer.
     * You are advised to call this method if you no longer need to listen to lifecycle changes. This reduces the
     * performance loss caused by observing lifecycle changes.
     *
     * @param observer  Indicates the lifecycle observer, either LifecycleObserver or LifecycleStateObserver.
     * The value cannot be null.
     */
    void RemoveObserver(const std::shared_ptr<ILifecycleObserver> &observer);

private:
    LifeCycle::Event state_ = UNDEFINED;

    std::list<std::shared_ptr<ILifecycleObserver>> callbacks_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_ABILITY_LIFECYCLE_H