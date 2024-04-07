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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_TEST_UNITEST_AMS_APP_LIFE_CYCLE_TEST_MOCK_APP_SCHEDULER_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_TEST_UNITEST_AMS_APP_LIFE_CYCLE_TEST_MOCK_APP_SCHEDULER_H

#include "gmock/gmock.h"
#include "refbase.h"
#include "iremote_object.h"
#include "app_scheduler_host.h"
#include "app_launch_data.h"

namespace OHOS {
namespace AppExecFwk {

class MockAppScheduler : public AppSchedulerHost {
public:
    MockAppScheduler(){};
    virtual ~MockAppScheduler(){};

    MOCK_METHOD0(ScheduleForegroundApplication, void());
    MOCK_METHOD0(ScheduleBackgroundApplication, void());
    MOCK_METHOD0(ScheduleTerminateApplication, void());
    MOCK_METHOD1(ScheduleLaunchApplication, void(const AppLaunchData &));
    MOCK_METHOD2(ScheduleLaunchAbility, void(const AbilityInfo &, const sptr<IRemoteObject> &));
    MOCK_METHOD1(ScheduleCleanAbility, void(const sptr<IRemoteObject> &));
    MOCK_METHOD1(ScheduleProfileChanged, void(const Profile &));
    MOCK_METHOD1(ScheduleConfigurationUpdated, void(const Configuration &config));
    MOCK_METHOD1(ScheduleShrinkMemory, void(const int));
    MOCK_METHOD0(ScheduleLowMemory, void());
    MOCK_METHOD0(ScheduleProcessSecurityExit, void());
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_TEST_UNITEST_AMS_APP_LIFE_CYCLE_TEST_MOCK_APP_SCHEDULER_CLIENT_H
