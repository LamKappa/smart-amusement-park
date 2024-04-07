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
#include "ability.h"
#include "context_deal.h"
#include "process_info.h"
#include "system_ability_definition.h"
#include "sys_mgr_client.h"
#include "mock_bundle_manager.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

class ContextDealTest : public testing::Test {
public:
    ContextDealTest() : context_(nullptr)
    {}
    ~ContextDealTest()
    {}
    std::shared_ptr<ContextDeal> context_ = nullptr;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ContextDealTest::SetUpTestCase(void)
{}

void ContextDealTest::TearDownTestCase(void)
{}

void ContextDealTest::SetUp(void)
{

    OHOS::sptr<OHOS::IRemoteObject> bundleObject = new (std::nothrow) BundleMgrService();
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(
        OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);
    OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->RegisterSystemAbility(
        OHOS::ABILITY_MGR_SERVICE_ID, bundleObject);
    context_ = std::make_shared<ContextDeal>();
}

void ContextDealTest::TearDown(void)
{}

/**
 * @tc.number: AppExecFwk_ContextDeal_StartAbility_0100
 * @tc.name: StartAbility
 * @tc.desc: Test whether startability is called normally.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_StartAbility_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_ContextDeal_StartAbility_0100 start";
    AAFwk::Want want;
    int requestCode = 0;
    context_->StartAbility(want, requestCode);
    GTEST_LOG_(INFO) << "AppExecFwk_ContextDeal_StartAbility_0100 end, StartAbility is empty";
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetBundleName_0100
 * @tc.name: GetBundleName
 * @tc.desc: Verify that the GetBundleName return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetBundleName_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string bundleName = "BundleName";
    info->bundleName = bundleName;
    context_->SetApplicationInfo(info);

    EXPECT_STREQ(context_->GetBundleName().c_str(), bundleName.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetBundleManager_0100
 * @tc.name: GetBundleManager
 * @tc.desc: Verify that the GetBundleManager return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetBundleManager_0100, Function | MediumTest | Level3)
{
    sptr<IBundleMgr> ptr = context_->GetBundleManager();
    EXPECT_NE(ptr, nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetBundleCodePath_0100
 * @tc.name: GetBundleCodePath
 * @tc.desc: Verify that the GetBundleCodePath return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetBundleCodePath_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string codePath = "CodePath";
    info->codePath = codePath;
    context_->SetApplicationInfo(info);

    EXPECT_STREQ(context_->GetBundleCodePath().c_str(), codePath.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetApplicationInfo_0100
 * @tc.name: GetApplicationInfo
 * @tc.desc: Verify that the GetApplicationInfo return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetApplicationInfo_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string bundleName = "BundleName";
    info->bundleName = bundleName;
    context_->SetApplicationInfo(info);

    EXPECT_STREQ(context_->GetApplicationInfo()->bundleName.c_str(), bundleName.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetBundleResourcePath_0100
 * @tc.name: GetBundleResourcePath
 * @tc.desc: Verify that the GetBundleResourcePath return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetBundleResourcePath_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<AbilityInfo> info = std::make_shared<AbilityInfo>();
    std::string resourcePath = "ResourcePath";
    info->resourcePath = resourcePath;
    context_->SetAbilityInfo(info);

    EXPECT_STREQ(context_->GetBundleResourcePath().c_str(), resourcePath.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetAbilityManager_0100
 * @tc.name: GetAbilityManager
 * @tc.desc: Verify that the GetAbilityManager return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetAbilityManager_0100, Function | MediumTest | Level3)
{
    sptr<AAFwk::IAbilityManager> ptr = context_->GetAbilityManager();
    EXPECT_NE(ptr, nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetCodeCacheDir_0100
 * @tc.name: GetCodeCacheDir
 * @tc.desc: Verify that the GetCodeCacheDir return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetCodeCacheDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "CodeCacheDir";
    info->dataDir = dir;
    context_->SetApplicationInfo(info);
    dir = dir + "/" + "code_cache";

    EXPECT_STREQ(context_->GetCodeCacheDir().c_str(), dir.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetCacheDir_0100
 * @tc.name: GetCacheDir
 * @tc.desc: Verify that the GetCacheDir return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetCacheDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "CacheDir";
    info->cacheDir = dir;
    context_->SetApplicationInfo(info);

    EXPECT_STREQ(context_->GetCacheDir().c_str(), dir.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetDatabaseDir_0100
 * @tc.name: GetDatabaseDir
 * @tc.desc: Verify that the GetDatabaseDir return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetDatabaseDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "dataBaseDir";
    info->dataBaseDir = dir;
    context_->SetApplicationInfo(info);

    EXPECT_STREQ(context_->GetDatabaseDir().c_str(), dir.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetFilesDir_0100
 * @tc.name: GetFilesDir
 * @tc.desc: Verify that the GetFilesDir return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetFilesDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "codePath";
    info->dataDir = dir;
    context_->SetApplicationInfo(info);
    dir = dir + "/" + "files";

    EXPECT_STREQ(context_->GetFilesDir().c_str(), dir.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetDataDir_0100
 * @tc.name: GetDataDir
 * @tc.desc: Verify that the GetDataDir return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetDataDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "dataDir";
    info->dataDir = dir;
    context_->SetApplicationInfo(info);

    EXPECT_STREQ(context_->GetDataDir().c_str(), dir.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetAppType_0100
 * @tc.name: GetAppType
 * @tc.desc: Verify that the GetAppType return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetAppType_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    info->bundleName = "hello";
    context_->SetApplicationInfo(info);

    std::string path = context_->GetAppType();
    std::string AppType = "system";

    EXPECT_NE(path.c_str(), AppType.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetAbilityInfo_0100
 * @tc.name: GetAbilityInfo
 * @tc.desc: Verify that the GetAbilityInfo return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetAbilityInfo_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<AbilityInfo> info = std::make_shared<AbilityInfo>();
    std::string codePath = "CodePath";
    info->codePath = codePath;
    context_->SetAbilityInfo(info);

    EXPECT_STREQ(context_->GetAbilityInfo()->codePath.c_str(), codePath.c_str());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetContext_0100
 * @tc.name: GetContext
 * @tc.desc: Verify that the GetContext return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetContext_0100, Function | MediumTest | Level3)
{
    std::shared_ptr<Ability> ability = std::make_shared<Ability>();
    std::shared_ptr<Context> context(ability);
    context_->SetContext(context);

    EXPECT_NE(context_->GetContext(), nullptr);
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetApplicationContext_0100
 * @tc.name: GetApplicationContext
 * @tc.desc: Verify that the GetApplicationContext return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetApplicationContext_0100, Function | MediumTest | Level3)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    context_->SetApplicationContext(application);
    EXPECT_NE(nullptr, context_->GetApplicationContext());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetProcessInfo_0100
 * @tc.name: GetProcessInfo
 * @tc.desc: Verify that the GetProcessInfo return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetProcessInfo_0100, Function | MediumTest | Level1)
{
    std::string name = "OHOS";
    pid_t id = 0;
    ProcessInfo info(name, id);
    std::shared_ptr<ProcessInfo> processinfo = std::make_shared<ProcessInfo>(info);
    context_->SetProcessInfo(processinfo);
    EXPECT_STREQ(name.c_str(), context_->GetProcessInfo()->GetProcessName().c_str());
}

/**
 * @tc.number: AppExecFwk_ContextDeal_GetProcessName_0100
 * @tc.name: GetProcessName
 * @tc.desc: Verify that the GetProcessName return value is correct.
 */
HWTEST_F(ContextDealTest, AppExecFwk_ContextDeal_GetProcessName_0100, Function | MediumTest | Level1)
{
    std::string name = "OHOS";
    pid_t id = 0;
    ProcessInfo info(name, id);
    std::shared_ptr<ProcessInfo> processinfo = std::make_shared<ProcessInfo>(info);
    context_->SetProcessInfo(processinfo);
    EXPECT_STREQ(name.c_str(), context_->GetProcessName().c_str());
}
}  // namespace AppExecFwk
}  