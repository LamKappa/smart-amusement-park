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

#include "mock_lifecycle_observer.h"
#include <gtest/gtest.h>

namespace OHOS {
namespace AppExecFwk {
void MockLifecycleObserver::OnActive()
{
    GTEST_LOG_(INFO) << "MockLifecycleObserver::OnActive called";
}

void MockLifecycleObserver::OnBackground()
{
    GTEST_LOG_(INFO) << "MockLifecycleObserver::OnBackground called";
}

void MockLifecycleObserver::OnForeground(const Want &want)
{
    GTEST_LOG_(INFO) << "MockLifecycleObserver::OnForeground called";
}

void MockLifecycleObserver::OnInactive()
{
    GTEST_LOG_(INFO) << "MockLifecycleObserver::OnInactive called";
}

void MockLifecycleObserver::OnStart(const Want &want)
{
    GTEST_LOG_(INFO) << "MockLifecycleObserver::OnStart called";
}

void MockLifecycleObserver::OnStop()
{
    GTEST_LOG_(INFO) << "MockLifecycleObserver::OnStop called";
}

void MockLifecycleObserver::OnStateChanged(LifeCycle::Event event, const Want &want)
{
    GTEST_LOG_(INFO) << "MockLifecycleObserver::OnStateChanged called";
}

void MockLifecycleObserver::OnStateChanged(LifeCycle::Event event)
{
    GTEST_LOG_(INFO) << "MockLifecycleObserver::OnStateChanged called";
}
}  // namespace AppExecFwk
}  // namespace OHOS