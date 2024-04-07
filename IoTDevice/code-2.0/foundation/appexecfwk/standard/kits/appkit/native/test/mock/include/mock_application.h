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

#ifndef FOUNDATION_APPEXECFWK_OHOS_MOCK_APPLICATION_H
#define FOUNDATION_APPEXECFWK_OHOS_MOCK_APPLICATION_H

#include "ohos_application.h"
#include <gtest/gtest.h>

namespace OHOS {
namespace AppExecFwk {

class OHOSApplication;
class MockApplication : public OHOSApplication {
public:
    MockApplication() = default;
    virtual ~MockApplication() = default;

    enum {
        APP_STATE_CREATE = 0,
        APP_STATE_READY = 1,
        APP_STATE_FOREGROUND = 2,
        APP_STATE_BACKGROUND = 3,
        APP_STATE_TERMINATED = 4
    };

    virtual void OnReady()
    {
        GTEST_LOG_(INFO) << "MockApplication::OnReady called";
        state_ = APP_STATE_READY;
    }

    virtual void OnForeground()
    {
        GTEST_LOG_(INFO) << "MockApplication::OnForeground called";
        state_ = APP_STATE_FOREGROUND;
    }

    virtual void OnBackground()
    {
        GTEST_LOG_(INFO) << "MockApplication::OnBackground called";
        state_ = APP_STATE_BACKGROUND;
    }

    virtual void OnConfigurationUpdated(const Configuration &config)
    {
        GTEST_LOG_(INFO) << "MockApplication::OnConfigurationUpdated called";
        onConfigurationUpdatedCalled_ = true;
    }

    virtual void OnMemoryLevel(int level)
    {
        GTEST_LOG_(INFO) << "MockApplication::OnMemoryLevel called";
        onMemoryLevelCalled_ = true;
    }

    virtual void OnStart()
    {
        GTEST_LOG_(INFO) << "MockApplication::OnStart called";
        state_ = APP_STATE_READY;
    }

    virtual void OnTerminate()
    {
        GTEST_LOG_(INFO) << "MockApplication::OnTerminate called";
        state_ = APP_STATE_TERMINATED;
    }

    int state_ = APP_STATE_CREATE;
    bool onMemoryLevelCalled_ = false;
    bool onConfigurationUpdatedCalled_ = false;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_MOCK_APPLICATION_H
