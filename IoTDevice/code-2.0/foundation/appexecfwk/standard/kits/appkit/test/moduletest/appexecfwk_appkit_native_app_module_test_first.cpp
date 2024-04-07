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

class AppkitNativeModuleTestFirst : public testing::Test {
public:
    AppkitNativeModuleTestFirst() : AppMgrObject_(nullptr), mockAppMgr(nullptr), mockHandler_(nullptr), runner_(nullptr)
    {}
    ~AppkitNativeModuleTestFirst()
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

void AppkitNativeModuleTestFirst::SetUpTestCase(void)
{}

void AppkitNativeModuleTestFirst::TearDownTestCase(void)
{}

void AppkitNativeModuleTestFirst::SetUp(void)
{
    GTEST_LOG_(INFO) << "AppkitNativeModuleTestFirst SetUp";
    AppMgrObject_ = new (std::nothrow) MockAppMgrService();
    mockAppMgr = iface_cast<MockAppMgrService>(AppMgrObject_);

    runner_ = EventRunner::Create("AppkitNativeModuleTestMockHandlerFirst");
    mockHandler_ = std::make_shared<MockHandler>(runner_);

    auto task = [abilityThread = this]() { MainThread::Start(); };
    mockHandler_->PostTask(task);
}

void AppkitNativeModuleTestFirst::TearDown(void)
{
    GTEST_LOG_(INFO) << "AppkitNativeModuleTestFirst TearDown";
    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    mockAppMgr->ScheduleTerminateApplication();
}

/**
 * @tc.number: App_Start_0100
 * @tc.name: Start Appfwk
 * @tc.desc: Mock appmgr and register it into the systemmanager.
 *           Start the Appfwk and the Appfwk attach the AppMgr.
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_Start_0100, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_Start_0100 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);
    mockAppMgr->ScheduleTerminateApplication();
    usleep(200);
    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    usleep(200);
    GTEST_LOG_(INFO) << "App_Start_0100 end";
}

/**
 * @tc.number: App_Start_0200
 * @tc.name: Start Appfwk
 * @tc.desc: Didn't register Mock appmgr into the systemmanager.
 *           Start the Appfwk and the Appfwk attach the AppMgr failed.
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_Start_0200, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_Start_0200 start";
    runner_->Run();
    usleep(200);
    EXPECT_EQ(false, mockAppMgr->IsAttached());
    runner_->Stop();
    GTEST_LOG_(INFO) << "App_Start_0200 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_0100
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager.
 *           The Appfwk has started successfully.
 *           Verifying when the correct App lifecycle flows, whether the corresponding
 *           callback function will be called.
 *           (StartAppfwk->LaunchApp->Foreground->Background->Terminate)
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_ApplicationLifeCycle_0100, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0100 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    mockAppMgr->ScheduleForegroundApplication();

    usleep(200);

    mockAppMgr->ScheduleBackgroundApplication();

    usleep(200);

    mockAppMgr->ScheduleTerminateApplication();

    usleep(200);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0100 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_0200
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when the wrong App lifecycle flows, whether the corresponding callback function will not
 *           be called. (StartAppfwk->Foreground)
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_ApplicationLifeCycle_0200, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0200 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);

    mockAppMgr->ScheduleForegroundApplication();

    usleep(200);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0200 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_0300
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when the wrong App lifecycle flows, whether the corresponding callback function will not
 *           be called. (StartAppfwk->Background)
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_ApplicationLifeCycle_0300, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0300 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);

    mockAppMgr->ScheduleBackgroundApplication();

    usleep(200);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0300 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_0400
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when the wrong App lifecycle flows, whether the corresponding callback function will not
 *           be called. (StartAppfwk->Terminate)
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_ApplicationLifeCycle_0400, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0400 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);

    mockAppMgr->ScheduleTerminateApplication();

    usleep(200);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0400 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_0500
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when the wrong App lifecycle flows, whether the corresponding callback function will not
 *           be called. (StartAppfwk->LaunchApp->Foreground)
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_ApplicationLifeCycle_0500, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0500 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    mockAppMgr->ScheduleBackgroundApplication();

    usleep(200);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0500 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_0600
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when the wrong App lifecycle flows, whether the corresponding callback function will not
 *           be called. (StartAppfwk->LaunchApp->Terminate)
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_ApplicationLifeCycle_0600, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0600 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    mockAppMgr->ScheduleTerminateApplication();

    usleep(200);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0600 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_0700
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when the wrong App lifecycle flows, whether the corresponding callback function will not
 *           be called. (StartAppfwk->LaunchApp->LaunchApp)
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_ApplicationLifeCycle_0700, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0700 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0700 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_0800
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when the wrong App lifecycle flows, whether the corresponding callback function will not
 *           be called. (StartAppfwk->LaunchApp->Foreground->Terminate)
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_ApplicationLifeCycle_0800, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0800 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    mockAppMgr->ScheduleForegroundApplication();

    usleep(200);

    mockAppMgr->ScheduleTerminateApplication();

    usleep(200);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0800 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_0900
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when the wrong App lifecycle flows, whether the corresponding callback function will not
 *           be called. (StartAppfwk->LaunchApp->Foreground->LaunchApp)
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_ApplicationLifeCycle_0900, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0900 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    mockAppMgr->ScheduleForegroundApplication();

    usleep(200);

    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_0900 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_1000
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when the wrong App lifecycle flows, whether the corresponding callback function will not
 *           be called. (StartAppfwk->LaunchApp->Foreground->LaunchApp)
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_ApplicationLifeCycle_1000, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_1000 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    mockAppMgr->ScheduleForegroundApplication();

    usleep(200);

    mockAppMgr->ScheduleForegroundApplication();

    usleep(200);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_1000 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_1100
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when the wrong App lifecycle flows, whether the corresponding callback function will not
 *           be called. (StartAppfwk->LaunchApp->Foreground->Background->LaunchApp)
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_ApplicationLifeCycle_1100, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_1100 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    mockAppMgr->ScheduleForegroundApplication();

    usleep(200);

    mockAppMgr->ScheduleBackgroundApplication();

    usleep(200);

    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_1100 end";
}

/**
 * @tc.number: App_ApplicationLifeCycle_1200
 * @tc.name: Application lifecycle switch
 * @tc.desc: Mock appmgr and register it into the systemmanager. The Appfwk has started successfully.
 *           Verifying when the wrong App lifecycle flows, whether the corresponding callback function will not
 *           be called. (StartAppfwk->LaunchApp->Foreground->Background->Foreground)
 */
HWTEST_F(AppkitNativeModuleTestFirst, App_ApplicationLifeCycle_1200, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_1200 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(200);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9999);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(200);

    mockAppMgr->ScheduleForegroundApplication();

    usleep(200);

    mockAppMgr->ScheduleBackgroundApplication();

    usleep(200);

    mockAppMgr->ScheduleForegroundApplication();

    usleep(200);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ApplicationLifeCycle_1200 end";

    mockAppMgr->ScheduleBackgroundApplication();
}
}  // namespace AppExecFwk
}  // namespace OHOS