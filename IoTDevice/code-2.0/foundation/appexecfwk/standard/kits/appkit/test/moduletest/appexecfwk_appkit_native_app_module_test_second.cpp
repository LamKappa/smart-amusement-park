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
#include "main_thread.h"
#include "sys_mgr_client.h"
#include "system_ability_definition.h"
#include "mock_app_mgr_service.h"
#include "mock_app_thread.h"
#include "mock_ability_token.h"
#include "mock_ability_mgr_service.h"
#include "mock_bundle_mgr_service.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace OHOS;
using namespace OHOS::AAFwk;

class AppkitNativeModuleTestSecond : public testing::Test {
public:
    AppkitNativeModuleTestSecond()
        : AppMgrObject_(nullptr), mockAppMgr(nullptr), mockHandler_(nullptr), runner_(nullptr)
    {}
    ~AppkitNativeModuleTestSecond()
    {}
    OHOS::sptr<OHOS::IRemoteObject> AppMgrObject_ = nullptr;
    MockAppMgrService *mockAppMgr = nullptr;
    std::shared_ptr<MockHandler> mockHandler_ = nullptr;
    std::shared_ptr<EventRunner> runner_ = nullptr;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AppkitNativeModuleTestSecond::SetUpTestCase(void)
{}

void AppkitNativeModuleTestSecond::TearDownTestCase(void)
{}

void AppkitNativeModuleTestSecond::SetUp(void)
{
    GTEST_LOG_(INFO) << "AppkitNativeModuleTestSecond SetUp";
    AppMgrObject_ = new (std::nothrow) MockAppMgrService();
    mockAppMgr = iface_cast<MockAppMgrService>(AppMgrObject_);

    runner_ = EventRunner::Create("AppkitNativeModuleTestMockHandlerSecond");
    mockHandler_ = std::make_shared<MockHandler>(runner_);

    auto task = [abilityThread = this]() { MainThread::Start(); };
    mockHandler_->PostTask(task);
}

void AppkitNativeModuleTestSecond::TearDown(void)
{
    GTEST_LOG_(INFO) << "AppkitNativeModuleTestSecond TearDown";
    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(50);

    mockAppMgr->ScheduleTerminateApplication();
}

/**
 * @tc.number: App_ApplicationLifeCycle_1300
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when the wrong App lifecycle flows, whether the corresponding callback function will not
 *           be called. (StartAppfwk->LaunchApp->Foreground->Background->Background)
 */
HWTEST_F(AppkitNativeModuleTestSecond, App_ApplicationLifeCycle_1300, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_1300 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(50);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(50);

    mockAppMgr->ScheduleForegroundApplication();

    usleep(50);

    mockAppMgr->ScheduleBackgroundApplication();

    usleep(50);

    mockAppMgr->ScheduleBackgroundApplication();

    usleep(50);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_1300 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_1400
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when launch the application with a null appInfo.name, whether the corresponding callback
 *           function will not be called.
 */
HWTEST_F(AppkitNativeModuleTestSecond, App_ApplicationLifeCycle_1400, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_1400 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(50);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(50);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_1400 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_1500
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when launch the application with a null processInfo.ProcessName, whether the corresponding
 *           callback function will not be called.
 */
HWTEST_F(AppkitNativeModuleTestSecond, App_ApplicationLifeCycle_1500, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_1500 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(50);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(50);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_1500 end";
}

/**
 * @tc.number: App_LaunchAblity_0100
 * @tc.name: App LaunchAblity
 * @tc.desc: Mock appmgr and register it into the systemmanager.
 *           The Appfwk has started successfully.
 *           The application has been launched successfully.
 *           Launch the ability for the application.
 */
HWTEST_F(AppkitNativeModuleTestSecond, App_LaunchAblity_0100, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_LaunchAblity_0100 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(50);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(50);

    AbilityInfo abilityinf;
    sptr<IRemoteObject> token = new (std::nothrow) MockAbilityToken();
    mockAppMgr->ScheduleLaunchAbility(abilityinf, token);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_LaunchAblity_0100 end";
}

/**
 * @tc.number: App_LaunchAblity_0200
 * @tc.name: App LaunchAblity
 * @tc.desc: Mock appmgr and register it into the systemmanager.
 *           The Appfwk has started successfully.
 *           The application has not been launched successfully.
 *           Launch the ability for the application, before the application has not been launched.
 */
HWTEST_F(AppkitNativeModuleTestSecond, App_LaunchAblity_0200, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_LaunchAblity_0200 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(50);

    AbilityInfo abilityinf;
    sptr<IRemoteObject> token = new (std::nothrow) MockAbilityToken();
    mockAppMgr->ScheduleLaunchAbility(abilityinf, token);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_LaunchAblity_0200 end";
}

/**
 * @tc.number: App_LaunchAblity_0300
 * @tc.name: App LaunchAblity
 * @tc.desc: Mock appmgr and register it into the systemmanager.
 *           The Appfwk has started successfully.
 *           The application has been launched successfully.
 *           The tocken should be null.
 *           Launch the ability for the application with a null token.
 */
HWTEST_F(AppkitNativeModuleTestSecond, App_LaunchAblity_0300, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_LaunchAblity_0300 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(50);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(50);

    AbilityInfo abilityinf;
    mockAppMgr->ScheduleLaunchAbility(abilityinf, nullptr);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_LaunchAblity_0300 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS