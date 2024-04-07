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
#include <gtest/gtest.h>

#include "semaphore_ex.h"
#include "app_scheduler_proxy.h"
#include "app_scheduler_host.h"

#include "mock_ability_token.h"
#include "mock_application.h"
#include "mock_app_scheduler.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using OHOS::iface_cast;
using OHOS::sptr;
using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;

namespace {

const int32_t COUNT = 10000;

}  // namespace

class AmsIpcAppSchedulerModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<MockAbilityToken> GetMockToken() const
    {
        return mock_token_;
    }

private:
    sptr<MockAbilityToken> mock_token_;
};

void AmsIpcAppSchedulerModuleTest::SetUpTestCase()
{}

void AmsIpcAppSchedulerModuleTest::TearDownTestCase()
{}

void AmsIpcAppSchedulerModuleTest::SetUp()
{}

void AmsIpcAppSchedulerModuleTest::TearDown()
{}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: IApplicationScheduler
 * FunctionPoints: test ScheduleForegroundApplication API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute ScheduleForegroundApplication API 10000 times
 */
HWTEST_F(AmsIpcAppSchedulerModuleTest, ExcuteApplicationIPCInterface_001, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockApplication> mockApplication(new MockApplication());
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

        EXPECT_CALL(*mockApplication, ScheduleForegroundApplication())
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
        client->ScheduleForegroundApplication();
        mockApplication->Wait();
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: IApplicationScheduler
 * FunctionPoints: test ScheduleBackgroundApplication API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute ScheduleBackgroundApplication API 10000 times
 */
HWTEST_F(AmsIpcAppSchedulerModuleTest, ExcuteApplicationIPCInterface_002, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockApplication> mockApplication(new MockApplication());
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

        EXPECT_CALL(*mockApplication, ScheduleBackgroundApplication())
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
        client->ScheduleBackgroundApplication();
        mockApplication->Wait();
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: IApplicationScheduler
 * FunctionPoints: test ScheduleTerminateApplication API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute ScheduleTerminateApplication API 10000 times
 */
HWTEST_F(AmsIpcAppSchedulerModuleTest, ExcuteApplicationIPCInterface_003, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockApplication> mockApplication(new MockApplication());
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

        EXPECT_CALL(*mockApplication, ScheduleTerminateApplication())
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
        client->ScheduleTerminateApplication();
        mockApplication->Wait();
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: IApplicationScheduler
 * FunctionPoints: test ScheduleTrimMemory API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute ScheduleTrimMemory API 10000 times
 */
HWTEST_F(AmsIpcAppSchedulerModuleTest, ExcuteApplicationIPCInterface_004, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockApplication> mockApplication(new MockApplication());
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

        EXPECT_CALL(*mockApplication, ScheduleShrinkMemory(_))
            .Times(1)
            .WillOnce(Invoke(mockApplication.GetRefPtr(), &MockApplication::ShrinkMemory));
        int level = 1;
        client->ScheduleShrinkMemory(level);
        mockApplication->Wait();
        int getLevel = mockApplication->GetShrinkLevel();
        EXPECT_EQ(getLevel, level) << "excute fail, index is " << i;
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: IApplicationScheduler
 * FunctionPoints: test scheduleLowMemory API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute LowMemoryWarning API 10000 times
 */
HWTEST_F(AmsIpcAppSchedulerModuleTest, ExcuteApplicationIPCInterface_005, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockApplication> mockApplication(new MockApplication());
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

        EXPECT_CALL(*mockApplication, ScheduleLowMemory())
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
        client->ScheduleLowMemory();
        mockApplication->Wait();
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: IApplicationScheduler
 * FunctionPoints: test scheduleProfileChanged API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute scheduleProfileChanged API 10000 times
 */
HWTEST_F(AmsIpcAppSchedulerModuleTest, ExcuteApplicationIPCInterface_006, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockApplication> mockApplication(new MockApplication());
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

        std::string profileName("mockProfile");
        Profile profile(profileName);

        EXPECT_CALL(*mockApplication, ScheduleProfileChanged(_))
            .Times(1)
            .WillOnce(Invoke(mockApplication.GetRefPtr(), &MockApplication::ProfileChanged));
        client->ScheduleProfileChanged(profile);
        mockApplication->Wait();
        bool result = mockApplication->CompareProfile(profile);
        EXPECT_EQ(result, true) << "excute fail, index is " << i;
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: IApplicationScheduler
 * FunctionPoints: test ScheduleLaunchAbility API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute ScheduleLaunchAbility API 10000 times
 */
HWTEST_F(AmsIpcAppSchedulerModuleTest, ExcuteApplicationIPCInterface_007, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockApplication> mockApplication(new MockApplication());
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

        std::string abilityName("mockAbilityInfo");
        AbilityInfo info;
        info.name = abilityName;

        EXPECT_CALL(*mockApplication, ScheduleLaunchAbility(_, _))
            .Times(1)
            .WillOnce(Invoke(mockApplication.GetRefPtr(), &MockApplication::LaunchAbility));
        client->ScheduleLaunchAbility(info, GetMockToken());
        mockApplication->Wait();
        bool result = mockApplication->CompareAbilityInfo(info);
        EXPECT_EQ(result, true) << "excute fail, index is " << i;
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: IApplicationScheduler
 * FunctionPoints: test ScheduleLaunchApplication API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute ScheduleLaunchApplication API 10000 times
 */
HWTEST_F(AmsIpcAppSchedulerModuleTest, ExcuteApplicationIPCInterface_008, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockApplication> mockApplication(new MockApplication());
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

        std::string applicationName("mockApplicationInfo");
        ApplicationInfo applicationInfo;
        applicationInfo.name = applicationName;
        std::string profileName("mockProfile");
        Profile profile(profileName);
        std::string processName("mockProcessInfo");
        ProcessInfo processInfo(processName, 123);

        AppLaunchData launchData;
        launchData.SetApplicationInfo(applicationInfo);
        launchData.SetProfile(profile);
        launchData.SetProcessInfo(processInfo);

        EXPECT_CALL(*mockApplication, ScheduleLaunchApplication(_))
            .Times(1)
            .WillOnce(Invoke(mockApplication.GetRefPtr(), &MockApplication::LaunchApplication));
        client->ScheduleLaunchApplication(launchData);
        mockApplication->Wait();

        bool isEqual = mockApplication->CompareAppLaunchData(launchData);
        ASSERT_EQ(true, isEqual) << "excute fail, index is " << i;
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: IApplicationScheduler
 * FunctionPoints: test ScheduleCleanAbility API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute ScheduleCleanAbility API 10000 times
 */
HWTEST_F(AmsIpcAppSchedulerModuleTest, ExcuteApplicationIPCInterface_009, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockApplication> mockApplication(new MockApplication());
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockApplication);

        std::string abilityName("mockAbilityInfo");

        EXPECT_CALL(*mockApplication, ScheduleCleanAbility(_))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockApplication.GetRefPtr(), &MockApplication::Post));
        client->ScheduleCleanAbility(GetMockToken());
        mockApplication->Wait();
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: IApplicationScheduler
 * FunctionPoints: test ScheduleConfigurationUpdated API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute ScheduleConfigurationUpdated API 10000 times
 */
HWTEST_F(AmsIpcAppSchedulerModuleTest, ExcuteApplicationIPCInterface_010, TestSize.Level3)
{
    OHOS::Semaphore sem(0);
    Configuration testConfig("testConfig");

    for (int i = 0; i < COUNT; i++) {
        sptr<MockAppScheduler> mockAppScheduler(new MockAppScheduler());
        sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler);
        bool testResult = false;

        auto mockHandler = [&](const Configuration &config) {
            testResult = (config.GetName() == testConfig.GetName());
            sem.Post();
        };

        EXPECT_CALL(*mockAppScheduler, ScheduleConfigurationUpdated(_)).Times(1).WillOnce(Invoke(mockHandler));

        client->ScheduleConfigurationUpdated(testConfig);

        sem.Wait();

        EXPECT_TRUE(testResult);
    }
}
