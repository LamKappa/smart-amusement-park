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
#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_TEST_MOCK_APPLICATION_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_TEST_MOCK_APPLICATION_H

#include "gmock/gmock.h"
#include "semaphore_ex.h"

#include "app_scheduler_host.h"

namespace OHOS {
namespace AppExecFwk {

class MockApplication : public AppSchedulerHost {
public:
    MOCK_METHOD0(ScheduleForegroundApplication, void());
    MOCK_METHOD0(ScheduleBackgroundApplication, void());
    MOCK_METHOD0(ScheduleTerminateApplication, void());
    MOCK_METHOD1(ScheduleShrinkMemory, void(const int));
    MOCK_METHOD0(ScheduleLowMemory, void());
    MOCK_METHOD1(ScheduleLaunchApplication, void(const AppLaunchData &));
    MOCK_METHOD2(ScheduleLaunchAbility, void(const AbilityInfo &, const sptr<IRemoteObject> &));
    MOCK_METHOD1(ScheduleCleanAbility, void(const sptr<IRemoteObject> &));
    MOCK_METHOD1(ScheduleProfileChanged, void(const Profile &));
    MOCK_METHOD1(ScheduleConfigurationUpdated, void(const Configuration &));
    MOCK_METHOD0(ScheduleProcessSecurityExit, void());

    void Post()
    {
        lock_.Post();
    }

    void Wait()
    {
        lock_.Wait();
    }

    void ShrinkMemory(const int level)
    {
        shrinkLevel_ = level;
        lock_.Post();
    }

    int GetShrinkLevel() const
    {
        return shrinkLevel_;
    }

    void LaunchApplication(const AppLaunchData &launchData)
    {
        launchData_ = launchData;
        lock_.Post();
    }

    bool CompareAppLaunchData(const AppLaunchData &launchData) const
    {
        if (launchData_.GetApplicationInfo().name != launchData.GetApplicationInfo().name) {
            return false;
        }
        if (launchData_.GetProfile().GetName() != launchData.GetProfile().GetName()) {
            return false;
        }
        if (launchData_.GetProcessInfo().GetProcessName() != launchData.GetProcessInfo().GetProcessName()) {
            return false;
        }
        return true;
    }

    void LaunchAbility(const AbilityInfo &info, const sptr<IRemoteObject> &)
    {
        abilityInfo_ = info;
        lock_.Post();
    }

    bool CompareAbilityInfo(const AbilityInfo &info) const
    {
        return (info.name == abilityInfo_.name);
    }

    void ProfileChanged(const Profile &profile)
    {
        profile_ = profile;
        lock_.Post();
    }

    bool CompareProfile(const Profile &profile) const
    {
        return (profile.GetName() == profile_.GetName());
    }

private:
    Semaphore lock_;
    volatile int shrinkLevel_ = 0;
    AppLaunchData launchData_;
    AbilityInfo abilityInfo_;
    Profile profile_;
    //  Configuration configuration_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_TEST_MOCK_APPLICATION_H
