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

class AppkitNativeModuleTestThird : public testing::Test {
public:
    AppkitNativeModuleTestThird() : AppMgrObject_(nullptr), mockAppMgr(nullptr), mockHandler_(nullptr), runner_(nullptr)
    {}
    ~AppkitNativeModuleTestThird()
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

void AppkitNativeModuleTestThird::SetUpTestCase(void)
{}

void AppkitNativeModuleTestThird::TearDownTestCase(void)
{}

void AppkitNativeModuleTestThird::SetUp(void)
{
    GTEST_LOG_(INFO) << "AppkitNativeModuleTestThird SetUp";
    AppMgrObject_ = new (std::nothrow) MockAppMgrService();
    mockAppMgr = iface_cast<MockAppMgrService>(AppMgrObject_);

    runner_ = EventRunner::Create("AppkitNativeModuleTestMockHandlerSecond");
    mockHandler_ = std::make_shared<MockHandler>(runner_);

    auto task = [abilityThread = this]() { MainThread::Start(); };
    mockHandler_->PostTask(task);
}

void AppkitNativeModuleTestThird::TearDown(void)
{
    GTEST_LOG_(INFO) << "AppkitNativeModuleTestThird TearDown";
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
 * @tc.number: App_CleanAbility_0100
 * @tc.name: App CleanAbility
 * @tc.desc: Mock appmgr and register it into the systemmanager.
 *           The Appfwk has started successfully.
 *           The application has been launched successfully.
 *           The ability has been launched successfully.
 *           Clean the ability which has been launched before.
 */
HWTEST_F(AppkitNativeModuleTestThird, App_CleanAbility_0100, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_CleanAbility_0100 start";
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

    usleep(50);

    mockAppMgr->ScheduleCleanAbility(token);

    usleep(50);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_CleanAbility_0100 end";
}

/**
 * @tc.number: App_CleanAbility_0200
 * @tc.name: App CleanAbility
 * @tc.desc: Mock appmgr and register it into the systemmanager.
 *           The Appfwk has started successfully.
 *           The application has been launched successfully.
 *           The application has not been launched successfully.
 *           Clean the ability before launch the application.
 */
HWTEST_F(AppkitNativeModuleTestThird, App_CleanAbility_0200, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_CleanAbility_0200 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(50);

    sptr<IRemoteObject> token = new (std::nothrow) MockAbilityToken();
    mockAppMgr->ScheduleCleanAbility(token);

    usleep(50);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_CleanAbility_0200 end";
}

/**
 * @tc.number: App_CleanAbility_0300
 * @tc.name: App CleanAbility
 * @tc.desc: Mock appmgr and register it into the systemmanager.
 *           The Appfwk has started successfully.
 *           The application has been launched successfully.
 *           The ability has not been launched successfully.
 *           Clean the ability before launch the ability.
 */
HWTEST_F(AppkitNativeModuleTestThird, App_CleanAbility_0300, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_CleanAbility_0300 start";
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

    sptr<IRemoteObject> token = new (std::nothrow) MockAbilityToken();
    mockAppMgr->ScheduleCleanAbility(token);

    usleep(50);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_CleanAbility_0300 end";
}

/**
 * @tc.number: App_CleanAbility_0400
 * @tc.name: App CleanAbility
 * @tc.desc: Mock appmgr and register it into the systemmanager.
 *           The Appfwk has started successfully.
 *           The application has been launched successfully.
 *           The ability has been launched successfully.
 *           Clean the ability with a null token.
 */
HWTEST_F(AppkitNativeModuleTestThird, App_CleanAbility_0400, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_CleanAbility_0400 start";
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

    usleep(50);

    mockAppMgr->ScheduleCleanAbility(nullptr);

    usleep(50);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_CleanAbility_0400 end";
}

/**
 * @tc.number: App_CleanAbility_0500
 * @tc.name: App CleanAbility
 * @tc.desc: Mock appmgr and register it into the systemmanager.
 *           The Appfwk has started successfully.
 *           The application has been launched successfully.
 *           The ability has been launched successfully.
 *           Clean the ability with a wrong token.
 */
HWTEST_F(AppkitNativeModuleTestThird, App_CleanAbility_0500, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_CleanAbility_0500 start";
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

    usleep(50);
    sptr<IRemoteObject> tokenOhter = new (std::nothrow) MockAbilityToken();
    mockAppMgr->ScheduleCleanAbility(tokenOhter);

    usleep(50);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_CleanAbility_0500 end";
}

/**
 * @tc.number: App_ElementsCallbacks_0100
 * @tc.name: Application ElementsCallbacks
 * @tc.desc: Mock application Mock appmgr and register it into the systemmanager.
 *           The Appfwk has started successfully.
 *           The Appfwk has started successfully.
 *           Verifying whether ElementsCallbacks registration, unregister, and its observer mechanism are valid.
 */
HWTEST_F(AppkitNativeModuleTestThird, App_ElementsCallbacks_0100, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_ElementsCallbacks_0100 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(50);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9997);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(50);

    mockAppMgr->ScheduleShrinkMemory(10);

    usleep(50);
    OHOS::AppExecFwk::Configuration config("testConfig");

    mockAppMgr->ScheduleConfigurationUpdated(config);

    usleep(50);

    mockAppMgr->ScheduleTerminateApplication();

    usleep(50);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_ElementsCallbacks_0100 end";
}

/**
 * @tc.number: App_AbilityLifecycleCallbacks_0100
 * @tc.name: Application AbilityLifecycleCallbacks
 * @tc.desc: Mock ability
 *           Mock application
 *           Mock appmgr and register it into the systemmanager.
 *           The Appfwk has started successfully.
 *           The application has been launched successfully.
 *           The ability has been launched successfully.
 *           Verifying whether AbilityLifecycleCallbacks registration, unregister, and its observer mechanism are
 valid.
 */
HWTEST_F(AppkitNativeModuleTestThird, App_AbilityLifecycleCallbacks_0100, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_AbilityLifecycleCallbacks_0100 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(50);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9996);
    appinf.name = "MockTestApplication";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(50);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);
    GTEST_LOG_(INFO) << "App_AbilityLifecycleCallbacks_0100 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS