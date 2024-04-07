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
 * @tc.number: App_Context_ApplicationContext_0100
 * @tc.name: Application Context
 * @tc.desc: Mock appmgr and register it into the systemmanager.
 *           The Appfwk has started successfully.
 *           The application has been launched successfully.
 *           Verifying the interface about getting context infos when the OnStart function of application is called.
 */
HWTEST_F(AppkitNativeModuleTestThird, App_Context_ApplicationContext_0100, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "App_Context_ApplicationContext_0100 start";
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(APP_MGR_SERVICE_ID, AppMgrObject_);
    runner_->Run();
    usleep(50);

    AppLaunchData lanchdate;
    ApplicationInfo appinf;
    ProcessInfo processinf("TestProcess", 9998);
    appinf.name = "MockTestApplication";
    appinf.cacheDir = "/hos/lib/cacheDir";
    appinf.dataBaseDir = "/hos/lib/dataBaseDir";
    appinf.dataDir = "/hos/lib/dataDir";
    appinf.bundleName = "MockBundleName";
    appinf.moduleSourceDirs.push_back("/hos/lib/libabilitydemo_native.z.so");
    lanchdate.SetApplicationInfo(appinf);
    lanchdate.SetProcessInfo(processinf);
    mockAppMgr->ScheduleLaunchApplication(lanchdate);

    usleep(50);

    runner_->Stop();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->UnregisterSystemAbility(APP_MGR_SERVICE_ID);

    usleep(50);
    GTEST_LOG_(INFO) << "App_Context_ApplicationContext_0100 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS