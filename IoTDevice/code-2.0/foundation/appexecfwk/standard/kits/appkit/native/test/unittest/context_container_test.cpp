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
#include <singleton.h>
#include "ohos_application.h"
#include "system_ability_definition.h"
#include "sys_mgr_client.h"
#include "ability_context.h"
#include "ability.h"
#include "context_container.h"
#include "context_deal.h"
#include "mock_bundle_manager.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

class ContextContainerTest : public testing::Test {
public:
    ContextContainerTest() : context_(nullptr), contextDeal_(nullptr)
    {}
    ~ContextContainerTest()
    {}
    std::shared_ptr<AbilityContext> context_ = nullptr;
    std::shared_ptr<ContextDeal> contextDeal_ = nullptr;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ContextContainerTest::SetUpTestCase(void)
{}

void ContextContainerTest::TearDownTestCase(void)
{}

void ContextContainerTest::SetUp(void)
{
    OHOS::sptr<OHOS::IRemoteObject> bundleObject = new (std::nothrow) BundleMgrService();

    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(
        OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(
        OHOS::ABILITY_MGR_SERVICE_ID, bundleObject);
    context_ = std::make_shared<AbilityContext>();
    contextDeal_ = std::make_shared<ContextDeal>();
}

void ContextContainerTest::TearDown(void)
{}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetBundleName_0100
 * @tc.name: GetBundleName
 * @tc.desc: Test whether attachbasecontext is called normally,
 *           and verify whether the return value of getbundlename is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetBundleName_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string bundleName = "BundleName";
    info->bundleName = bundleName;
    contextDeal_->SetApplicationInfo(info);
    context_->AttachBaseContext(contextDeal_);

    EXPECT_STREQ(context_->GetBundleName().c_str(), bundleName.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetBundleName_0200
 * @tc.name: GetBundleName
 * @tc.desc: Test getbundlename exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetBundleName_0200, Function | MediumTest | Level3)
{
    std::string bundleName = "";

    EXPECT_STREQ(context_->GetBundleName().c_str(), bundleName.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetBundleManager_0100
 * @tc.name: GetBundleManager
 * @tc.desc: Test whether attachbasecontext is called normally,
 *           and verify whether the return value of getbundlemanager is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetBundleManager_0100, Function | MediumTest | Level1)
{
    context_->AttachBaseContext(contextDeal_);

    sptr<IBundleMgr> ptr = context_->GetBundleManager();

    EXPECT_NE(ptr, nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetBundleManager_0200
 * @tc.name: GetBundleManager
 * @tc.desc: Test getbundlemanager exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetBundleManager_0200, Function | MediumTest | Level3)
{
    sptr<IBundleMgr> ptr = context_->GetBundleManager();
    EXPECT_EQ(ptr, nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetBundleCodePath_0100
 * @tc.name: GetBundleCodePath
 * @tc.desc: Test whether attachbasecontext is called normally,
 *           and verify whether the return value of getbundlecodepath is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetBundleCodePath_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string codePath = "CodePath";
    info->codePath = codePath;
    contextDeal_->SetApplicationInfo(info);
    context_->AttachBaseContext(contextDeal_);

    EXPECT_STREQ(context_->GetBundleCodePath().c_str(), codePath.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetBundleCodePath_0200
 * @tc.name: GetBundleCodePath
 * @tc.desc: Test getbundlecodepath exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetBundleCodePath_0200, Function | MediumTest | Level3)
{
    std::string codePath = "";
    std::string path = context_->GetBundleCodePath();

    EXPECT_STREQ(path.c_str(), codePath.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetApplicationInfo_0100
 * @tc.name: GetApplicationInfo
 * @tc.desc: Test whether attachbasecontext is called normally,
 *           and verify whether the return value of getapplicationinfo is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetApplicationInfo_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string bundleName = "BundleName";
    info->bundleName = bundleName;
    contextDeal_->SetApplicationInfo(info);
    context_->AttachBaseContext(contextDeal_);

    EXPECT_STREQ(context_->GetApplicationInfo()->bundleName.c_str(), bundleName.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetApplicationInfo_0200
 * @tc.name: GetApplicationInfo
 * @tc.desc: Test getapplicationinfo exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetApplicationInfo_0200, Function | MediumTest | Level3)
{
    std::shared_ptr<ApplicationInfo> info = context_->GetApplicationInfo();
    EXPECT_EQ(info, nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetBundleResourcePath_0100
 * @tc.name: GetBundleResourcePath
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetBundleResourcePath is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetBundleResourcePath_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<AbilityInfo> info = std::make_shared<AbilityInfo>();
    std::string resourcePath = "ResourcePath";
    info->resourcePath = resourcePath;
    contextDeal_->SetAbilityInfo(info);
    context_->AttachBaseContext(contextDeal_);

    EXPECT_STREQ(context_->GetBundleResourcePath().c_str(), resourcePath.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetBundleResourcePath_0200
 * @tc.name: GetBundleResourcePath
 * @tc.desc: Test GetBundleResourcePath exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetBundleResourcePath_0200, Function | MediumTest | Level3)
{
    std::string path = context_->GetBundleResourcePath();
    std::string empty = "";
    EXPECT_STREQ(context_->GetBundleResourcePath().c_str(), empty.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetAppType_0100
 * @tc.name: GetAppType
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetAppType is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetAppType_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    info->bundleName = "hello";
    contextDeal_->SetApplicationInfo(info);
    context_->AttachBaseContext(contextDeal_);
    std::string path = context_->GetAppType();
    std::string appType = "system";

    EXPECT_STREQ(context_->GetAppType().c_str(), appType.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetAppType_0200
 * @tc.name: GetAppType
 * @tc.desc: Test GetAppType exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetAppType_0200, Function | MediumTest | Level3)
{
    std::string path = context_->GetAppType();
    std::string empty = "";
    EXPECT_STREQ(context_->GetAppType().c_str(), empty.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetAbilityManager_0100
 * @tc.name: GetAbilityManager
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetAbilityManager is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetAbilityManager_0100, Function | MediumTest | Level1)
{
    context_->AttachBaseContext(contextDeal_);

    sptr<AAFwk::IAbilityManager> ptr = context_->GetAbilityManager();
    EXPECT_NE(ptr, nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetAbilityManager_0200
 * @tc.name: GetAbilityManager
 * @tc.desc: Test GetAbilityManager exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetAbilityManager_0200, Function | MediumTest | Level3)
{
    sptr<AAFwk::IAbilityManager> ptr = context_->GetAbilityManager();
    EXPECT_EQ(ptr, nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetCodeCacheDir_0100
 * @tc.name: GetCodeCacheDir
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetCodeCacheDir is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetCodeCacheDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "CodeCacheDir";
    info->dataDir = dir;
    contextDeal_->SetApplicationInfo(info);
    context_->AttachBaseContext(contextDeal_);
    std::string dirCompare = "CodeCacheDir/code_cache";
    EXPECT_STREQ(context_->GetCodeCacheDir().c_str(), dirCompare.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetCodeCacheDir_0200
 * @tc.name: GetCodeCacheDir
 * @tc.desc: Test GetCodeCacheDir exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetCodeCacheDir_0200, Function | MediumTest | Level3)
{
    std::string empty = "";
    EXPECT_STREQ(context_->GetCodeCacheDir().c_str(), empty.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetCacheDir_0100
 * @tc.name: GetCacheDir
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetCacheDir is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetCacheDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "CacheDir";
    info->cacheDir = dir;
    contextDeal_->SetApplicationInfo(info);
    context_->AttachBaseContext(contextDeal_);

    EXPECT_STREQ(context_->GetCacheDir().c_str(), dir.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetCacheDir_0200
 * @tc.name: GetCacheDir
 * @tc.desc: Test GetCacheDir exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetCacheDir_0200, Function | MediumTest | Level3)
{
    std::string empty = "";
    EXPECT_STREQ(context_->GetCacheDir().c_str(), empty.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetDatabaseDir_0100
 * @tc.name: GetDatabaseDir
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetDatabaseDir is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetDatabaseDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "dataBaseDir";
    info->dataBaseDir = dir;
    contextDeal_->SetApplicationInfo(info);
    context_->AttachBaseContext(contextDeal_);

    EXPECT_STREQ(context_->GetDatabaseDir().c_str(), dir.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetCacheDir_0200
 * @tc.name: GetDatabaseDir
 * @tc.desc: Test GetDatabaseDir exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetDatabaseDir_0200, Function | MediumTest | Level3)
{
    std::string empty = "";
    EXPECT_STREQ(context_->GetDatabaseDir().c_str(), empty.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetDataDir_0100
 * @tc.name: GetDataDir
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetDataDir is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetDataDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "dataDir";
    info->dataDir = dir;
    contextDeal_->SetApplicationInfo(info);
    context_->AttachBaseContext(contextDeal_);

    EXPECT_STREQ(context_->GetDataDir().c_str(), dir.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetDataDir_0200
 * @tc.name: GetDataDir
 * @tc.desc: Test GetDataDir exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetDataDir_0200, Function | MediumTest | Level3)
{
    std::string empty = "";
    EXPECT_STREQ(context_->GetDataDir().c_str(), empty.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetDir_0100
 * @tc.name: GetDir
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetDir is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "dataDir";
    info->dataDir = dir;
    contextDeal_->SetApplicationInfo(info);
    context_->AttachBaseContext(contextDeal_);

    std::string name = "name";
    std::string dirCompare = "dataDir/name";
    int mode = 0;
    EXPECT_STREQ(context_->GetDir(name, mode).c_str(), dirCompare.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetDir_0200
 * @tc.name: GetDir
 * @tc.desc: Test GetDir exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetDir_0200, Function | MediumTest | Level3)
{
    std::string empty = "";
    std::string name = "name";
    int mode = 0;
    EXPECT_STREQ(context_->GetDir(name, mode).c_str(), empty.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetFilesDir_0100
 * @tc.name: GetFilesDir
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetFilesDir is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetFilesDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "codePath";
    info->dataDir = dir;
    contextDeal_->SetApplicationInfo(info);
    context_->AttachBaseContext(contextDeal_);
    std::string dirCompare = "codePath/files";
    EXPECT_STREQ(context_->GetFilesDir().c_str(), dirCompare.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetFilesDir_0200
 * @tc.name: GetFilesDir
 * @tc.desc: Test GetFilesDir exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetFilesDir_0200, Function | MediumTest | Level3)
{
    std::string empty = "";
    EXPECT_STREQ(context_->GetFilesDir().c_str(), empty.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetAbilityInfo_0100
 * @tc.name: GetAbilityInfo
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetAbilityInfo is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetAbilityInfo_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<AbilityInfo> info = std::make_shared<AbilityInfo>();
    std::string resourcePath = "ResourcePath";
    info->resourcePath = resourcePath;
    contextDeal_->SetAbilityInfo(info);
    context_->AttachBaseContext(contextDeal_);

    EXPECT_STREQ(context_->GetAbilityInfo()->resourcePath.c_str(), resourcePath.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetAbilityInfo_0200
 * @tc.name: GetAbilityInfo
 * @tc.desc: Test GetAbilityInfo exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetAbilityInfo_0200, Function | MediumTest | Level3)
{
    std::shared_ptr<AbilityInfo> ptr = context_->GetAbilityInfo();
    EXPECT_EQ(ptr, nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetContext_0100
 * @tc.name: GetContext
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetContext is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetContext_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<Ability> ability = std::make_shared<Ability>();
    std::shared_ptr<Context> context(ability);
    contextDeal_->SetContext(context);
    context_->AttachBaseContext(contextDeal_);

    EXPECT_NE(context_->GetContext(), nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetContext_0200
 * @tc.name: GetContext
 * @tc.desc: Test GetContext exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetContext_0200, Function | MediumTest | Level3)
{
    std::shared_ptr<Context> ptr = context_->GetContext();
    EXPECT_EQ(ptr, nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetApplicationContext_0100
 * @tc.name: GetApplicationContext
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetApplicationContext is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetApplicationContext_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    contextDeal_->SetApplicationContext(application);
    context_->AttachBaseContext(contextDeal_);

    EXPECT_NE(nullptr, context_->GetApplicationContext());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetApplicationContext_0200
 * @tc.name: GetApplicationContext
 * @tc.desc: Test GetApplicationContext exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetApplicationContext_0200, Function | MediumTest | Level3)
{
    std::shared_ptr<Context> ptr = context_->GetApplicationContext();
    EXPECT_EQ(ptr, nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetProcessInfo_0100
 * @tc.name: GetProcessInfo
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetProcessInfo is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetProcessInfo_0100, Function | MediumTest | Level1)
{
    std::string name = "OHOS";
    pid_t id = 0;
    ProcessInfo info(name, id);
    std::shared_ptr<ProcessInfo> processinfo = std::make_shared<ProcessInfo>(info);
    contextDeal_->SetProcessInfo(processinfo);
    context_->AttachBaseContext(contextDeal_);

    EXPECT_STREQ(name.c_str(), context_->GetProcessInfo()->GetProcessName().c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetProcessInfo_0200
 * @tc.name: GetProcessInfo
 * @tc.desc: Test GetProcessInfo exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetProcessInfo_0200, Function | MediumTest | Level3)
{
    std::shared_ptr<ProcessInfo> ptr = context_->GetProcessInfo();
    EXPECT_EQ(ptr, nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetProcessName_0100
 * @tc.name: GetProcessName
 * @tc.desc: Test whether AttachBaseContext is called normally,
 *           and verify whether the return value of GetProcessName is correct.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextDeal_GetProcessName_0100, Function | MediumTest | Level1)
{
    std::string name = "OHOS";
    pid_t id = 0;
    ProcessInfo info(name, id);
    std::shared_ptr<ProcessInfo> processinfo = std::make_shared<ProcessInfo>(info);
    contextDeal_->SetProcessInfo(processinfo);
    context_->AttachBaseContext(contextDeal_);

    EXPECT_STREQ(name.c_str(), context_->GetProcessName().c_str());
}

/**
 * @tc.number: AppExecFwk_ContextContainer_GetProcessName_0200
 * @tc.name: GetProcessName
 * @tc.desc: Test GetProcessName exception status.
 */
HWTEST_F(ContextContainerTest, AppExecFwk_ContextContainer_GetProcessName_0200, Function | MediumTest | Level3)
{
    std::string empty = "";
    std::string name = context_->GetProcessName();
    EXPECT_STREQ(empty.c_str(), name.c_str());
}
}  // namespace AppExecFwk
}  // namespace OHOS