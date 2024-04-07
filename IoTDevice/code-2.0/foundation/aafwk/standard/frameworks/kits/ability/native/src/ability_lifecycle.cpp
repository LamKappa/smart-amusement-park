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

#include "ability_lifecycle_observer_interface.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @brief Obtains the current lifecycle event.
 * Lifecycle events drive lifecycle state changes. Therefore, you are able to know the lifecycle state
 * once you obtain the lifecycle event. For example, if the ON_ACTIVE event is received, the ability or
 * ability slice is in the ACTIVE state; if the ON_FOREGROUND event is received, the ability or ability
 * slice is changing from the BACKGROUND state to INACTIVE.
 *
 * @return Returns the current lifecycle event.
 */
LifeCycle::Event LifeCycle::GetLifecycleState()
{
    APP_LOGI("LifeCycle::GetLifecycleState: called");
    return state_;
}

/**
 * @brief Adds a lifecycle observer.
 * The observer will be notified of lifecycle changes.
 *
 * @param observer Indicates the lifecycle observer, either LifecycleObserver or LifecycleStateObserver.
 * The value cannot be null.
 *
 */
void LifeCycle::AddObserver(const std::shared_ptr<ILifecycleObserver> &observer)
{
    APP_LOGI("LifeCycle::AddObserver: called");

    if (observer == nullptr) {
        APP_LOGI("LifeCycle::AddObserver: observer is null");
        return;
    }

    callbacks_.emplace_back(observer);
}

/**
 * @brief While Ability's lifecycle changes, dispatch lifecycle event.
 *
 * @param event  Lifecycle state.
 * @param want  Indicates the Want containing information about the target ability to change lifecycle state.
 */
void LifeCycle::DispatchLifecycle(const LifeCycle::Event &event, const Want &want)
{
    APP_LOGI("LifeCycle::DispatchLifecycle: called");

    if ((event != LifeCycle::Event::ON_FOREGROUND) && (event != LifeCycle::Event::ON_START)) {
        APP_LOGE("event value error: event is %{public}d", event);
        return;
    }

    state_ = event;
    if (callbacks_.size() != 0) {
        for (auto &callback : callbacks_) {
            switch (event) {
                case ON_FOREGROUND: {
                    if (callback != nullptr) {
                        callback->OnForeground(want);
                    }
                    break;
                }
                case ON_START: {
                    if (callback != nullptr) {
                        callback->OnStart(want);
                    }
                    break;
                }
                default:
                    break;
            }
            if (callback != nullptr) {
                callback->OnStateChanged(event, want);
            }
        }
    }
}

/**
 * @brief While Ability's lifecycle changes, dispatch lifecycle event.
 *
 * @param event  Lifecycle state.
 */
void LifeCycle::DispatchLifecycle(const LifeCycle::Event &event)
{
    APP_LOGI("LifeCycle::DispatchLifecycle: called");

    if ((event != LifeCycle::Event::ON_ACTIVE) && (event != LifeCycle::Event::ON_BACKGROUND) &&
        (event != LifeCycle::Event::ON_INACTIVE) && (event != LifeCycle::Event::ON_STOP)) {
        APP_LOGE("event value error: event is %{public}d", event);
        return;
    }

    state_ = event;
    if (callbacks_.size() != 0) {
        for (auto &callback : callbacks_) {
            switch (event) {
                case ON_ACTIVE: {
                    if (callback != nullptr) {
                        callback->OnActive();
                    }
                    break;
                }
                case ON_BACKGROUND: {
                    if (callback != nullptr) {
                        callback->OnBackground();
                    }
                    break;
                }
                case ON_INACTIVE: {
                    if (callback != nullptr) {
                        callback->OnInactive();
                    }
                    break;
                }
                case ON_STOP: {
                    if (callback != nullptr) {
                        callback->OnStop();
                    }
                    break;
                }
                default:
                    break;
            }
            if (callback != nullptr) {
                callback->OnStateChanged(event);
            }
        }
    }
}

/**
 * @brief Removes a lifecycle observer.
 * You are advised to call this method if you no longer need to listen to lifecycle changes. This reduces the
 * performance loss caused by observing lifecycle changes.
 *
 * @param observer  Indicates the lifecycle observer, either LifecycleObserver or LifecycleStateObserver.
 * The value cannot be null.
 */
void LifeCycle::RemoveObserver(const std::shared_ptr<ILifecycleObserver> &observer)
{
    APP_LOGI("LifeCycle::RemoveObserver: called");

    if (observer == nullptr) {
        APP_LOGI("LifeCycle::RemoveObserver: observer is null");
        return;
    }

    callbacks_.remove(observer);
}
}  // namespace AppExecFwk
}  // namespace OHOS