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

#include "ability_context.h"
#include "ohos_application.h"
#include "ability_info.h"
#include "ability.h"
#include "context_deal.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "mock_ability_manager_service.h"
#include "system_ability_definition.h"
#include "sys_mgr_client.h"
#include "sa_mgr_client.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

class AbilityContextTest : public testing::Test {
public:
    AbilityContextTest() : context_(nullptr)
    {}
    ~AbilityContextTest()
    {}
    std::unique_ptr<AbilityContext> context_ = nullptr;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AbilityContextTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "AppExecFwk_AbilityContext_SetUpTestCase start";
    OHOS::sptr<OHOS::IRemoteObject> bundleObject = new (std::nothrow) BundleMgrService();
    OHOS::sptr<OHOS::IRemoteObject> abilityObject = new (std::nothrow) OHOS::AAFwk::MockAbilityManagerService();

    auto sysMgr = OHOS::DelayedSingleton<SysMrgClient>::GetInstance();
    if (sysMgr == NULL) {
        GTEST_LOG_(ERROR) << "fail to get ISystemAbilityManager";
        return;
    }

    sysMgr->RegisterSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);
    sysMgr->RegisterSystemAbility(OHOS::ABILITY_MGR_SERVICE_ID, abilityObject);

    GTEST_LOG_(INFO) << "AppExecFwk_AbilityContext_SetUpTestCase end";
}

void AbilityContextTest::TearDownTestCase(void)
{}

void AbilityContextTest::SetUp(void)
{
    context_ = std::make_unique<AbilityContext>();
    GTEST_LOG_(INFO) << "AppExecFwk_AbilityContext_SetUp end";
}

void AbilityContextTest::TearDown(void)
{}


/**
 * @tc.number: AaFwk_AbilityContext_StartAbility_0100
 * @tc.name: StartAbility
 * @tc.desc: Test whether startability is called normally.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_StartAbility_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityContext_StartAbility_0100 start";
    AAFwk::Want want;
    int requestCode = 0;
    context_->StartAbility(want, requestCode);
    GTEST_LOG_(INFO) << "AaFwk_AbilityContext_StartAbility_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityContext_TerminateAbility_0100
 * @tc.name: TerminateAbility
 * @tc.desc: Test whether TerminateAbility is called normally.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_TerminateAbility_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_AbilityContext_TerminateAbility_0100 start";
    context_->TerminateAbility();
    GTEST_LOG_(INFO) << "AppExecFwk_AbilityContext_TerminateAbility_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityContext_GetCallingAbility_0100
 * @tc.name: GetCallingAbility
 * @tc.desc: Test whether the return value of getcallingability is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetCallingAbility_0100, Function | MediumTest | Level1)
{
    // init
    AAFwk::Want want;
    OHOS::AppExecFwk::ElementName elementName;
    elementName.SetBundleName(std::string("App.System.Test.bundleName_0127"));
    elementName.SetAbilityName(std::string("App.System.Test.AbilityName_0127"));
    want.SetElement(elementName);

    std::shared_ptr<Ability> ability = std::make_shared<Ability>();
    // start
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->type = AppExecFwk::AbilityType::PAGE;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    ability->Init(abilityInfo, nullptr, handler, nullptr);
    ability->StartAbilityForResult(want, -1);

    ability->SetCallingContext("", "", std::string("App.System.Test.AbilityName_0127"));
    std::shared_ptr<ElementName> elementNameTest = ability->GetCallingAbility();
    std::string abilityName;
    if (elementNameTest != nullptr) {
        abilityName = elementNameTest->GetAbilityName();
    }

    // Test
    EXPECT_EQ(abilityName, std::string("App.System.Test.AbilityName_0127"));
}

/**
 * @tc.number: AaFwk_AbilityContext_GetApplicationInfo_0100
 * @tc.name: GetApplicationInfo
 * @tc.desc: Test whether the getapplicationinfo return value is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetApplicationInfo_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string name = "WeChat";
    info->name = name;
    deal->SetApplicationInfo(info);

    context_->AttachBaseContext(deal);
    EXPECT_STREQ(name.c_str(), context_->GetApplicationInfo()->name.c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetCacheDir_0100
 * @tc.name: GetCacheDir
 * @tc.desc: Test the attachbasecontext call to verify whether the return value of getcachedir is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetCacheDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "CacheDir";
    info->cacheDir = dir;
    deal->SetApplicationInfo(info);
    context_->AttachBaseContext(deal);

    EXPECT_STREQ(dir.c_str(), context_->GetCacheDir().c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetCodeCacheDir_0100
 * @tc.name: GetCodeCacheDir
 * @tc.desc: Test the attachbasecontext call to verify whether the return value of getcodechedir is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetCodeCacheDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "CacheDir";
    info->dataDir = dir;
    deal->SetApplicationInfo(info);
    context_->AttachBaseContext(deal);
    std::string dirCompare = "CacheDir/code_cache";
    EXPECT_STREQ(dirCompare.c_str(), context_->GetCodeCacheDir().c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetDatabaseDir_0100
 * @tc.name: GetDatabaseDir
 * @tc.desc: Test the attachbasecontext call to verify whether the return value of getdatabasedir is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetDatabaseDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "dataBaseDir";
    info->dataBaseDir = dir;
    deal->SetApplicationInfo(info);
    context_->AttachBaseContext(deal);

    EXPECT_STREQ(dir.c_str(), context_->GetDatabaseDir().c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetDataDir_0100
 * @tc.name: GetDataDir
 * @tc.desc: Test the attachbasecontext call to verify whether the return value of getdatadir is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetDataDir_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    std::string dir = "dataDir";
    info->dataDir = dir;
    deal->SetApplicationInfo(info);
    context_->AttachBaseContext(deal);

    EXPECT_STREQ(dir.c_str(), context_->GetDataDir().c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetBundleManager_0100
 * @tc.name: GetBundleManager
 * @tc.desc: Test the attachbasecontext call to verify whether the return value of getbundlemanager is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetBundleManager_0100, Function | MediumTest | Level3)
{
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    context_->AttachBaseContext(deal);
    sptr<IBundleMgr> ptr = context_->GetBundleManager();

    EXPECT_NE(nullptr, ptr);
}

/**
 * @tc.number: AaFwk_AbilityContext_GetBundleCodePath_0100
 * @tc.name: GetBundleCodePath
 * @tc.desc: Test the attachbasecontext call to verify whether the return value of getbundlecodepath is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetBundleCodePath_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    std::string codePath = "hello";
    appInfo->codePath = codePath;

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetApplicationInfo(appInfo);

    context_->AttachBaseContext(deal);
    EXPECT_STREQ(codePath.c_str(), context_->GetBundleCodePath().c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetBundleName_0100
 * @tc.name: GetBundleName
 * @tc.desc: Test the attachbasecontext call to verify whether the return value of getbundlename is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetBundleName_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    std::string name = "hello";
    appInfo->bundleName = name;

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetApplicationInfo(appInfo);

    context_->AttachBaseContext(deal);
    EXPECT_STREQ(name.c_str(), context_->GetBundleName().c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetBundleResourcePath_0100
 * @tc.name: GetBundleResourcePath
 * @tc.desc: Test the attachbasecontext call to verify whether the return value of getbundleresourcepath is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetBundleResourcePath_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    std::string resourcePath = "hello";
    abilityInfo->resourcePath = resourcePath;

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetAbilityInfo(abilityInfo);

    context_->AttachBaseContext(deal);
    EXPECT_STREQ(resourcePath.c_str(), context_->GetBundleResourcePath().c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetApplicationContext_0100
 * @tc.name: GetApplicationContext
 * @tc.desc: Test the attachbasecontext call to verify whether the return value of getapplicationcontext is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetApplicationContext_0100, Function | MediumTest | Level3)
{
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetApplicationContext(application);
    context_->AttachBaseContext(deal);
    EXPECT_NE(nullptr, context_->GetApplicationContext());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetContext_0100
 * @tc.name: GetContext
 * @tc.desc: Test the attachbasecontext call to verify whether the getcontext return value is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetContext_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<Ability> ability = std::make_shared<Ability>();
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();

    std::string name = "hello";
    abilityInfo->name = name;

    deal->SetAbilityInfo(abilityInfo);
    std::shared_ptr<Context> context(ability);
    deal->SetContext(context);
    ability->AttachBaseContext(deal);

    EXPECT_STREQ(name.c_str(), ability->GetContext()->GetAbilityInfo()->name.c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetAbilityManager_0100
 * @tc.name: GetAbilityManager
 * @tc.desc: Test the attachbasecontext call to verify whether the return value of getabilitymanager is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetAbilityManager_0100, Function | MediumTest | Level3)
{
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    context_->AttachBaseContext(deal);
    sptr<AAFwk::IAbilityManager> ptr = context_->GetAbilityManager();

    EXPECT_NE(nullptr, ptr);
}

/**
 * @tc.number: AaFwk_AbilityContext_GetProcessInfo_0100
 * @tc.name: GetProcessInfo
 * @tc.desc: Test the attachbasecontext call to verify that the getprocessinfo return value is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetProcessInfo_0100, Function | MediumTest | Level1)
{
    std::string name = "OHOS";
    pid_t id = 0;
    ProcessInfo info(name, id);

    std::shared_ptr<ProcessInfo> processInfo = std::make_shared<ProcessInfo>(info);
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetProcessInfo(processInfo);
    context_->AttachBaseContext(deal);

    EXPECT_STREQ(name.c_str(), context_->GetProcessInfo()->GetProcessName().c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetAppType_0100
 * @tc.name: GetAppType
 * @tc.desc: Test the attachbasecontext call to verify that the getapptype return value is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetAppType_0100, Function | MediumTest | Level1)
{
    std::string empty = "system";
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    std::shared_ptr<ApplicationInfo> info = std::make_shared<ApplicationInfo>();
    info->bundleName = "hello";
    deal->SetApplicationInfo(info);
    context_->AttachBaseContext(deal);

    EXPECT_STREQ(empty.c_str(), context_->GetAppType().c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetAbilityInfo_0100
 * @tc.name: GetAbilityInfo
 * @tc.desc: Test the attachbasecontext call to verify that the getabilityinfo return value is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetAbilityInfo_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();

    std::string name = "hello";
    abilityInfo->name = name;
    deal->SetAbilityInfo(abilityInfo);
    context_->AttachBaseContext(deal);

    EXPECT_STREQ(name.c_str(), context_->GetAbilityInfo()->name.c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetElementName_0100
 * @tc.name: GetElementName
 * @tc.desc: Verify that the getelementname return value is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetElementName_0100, Function | MediumTest | Level3)
{
    EXPECT_EQ(nullptr, context_->GetElementName());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetElementName_0200
 * @tc.name: GetElementName
 * @tc.desc: Test getelementname exception status.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetElementName_0200, Function | MediumTest | Level3)
{
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();

    std::string name = "abilityName";
    abilityInfo->name = name;
    deal->SetAbilityInfo(abilityInfo);
    context_->AttachBaseContext(deal);

    EXPECT_STREQ(name.c_str(), context_->GetElementName()->GetAbilityName().c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetCallingBundle_0100
 * @tc.name: GetCallingBundle
 * @tc.desc: Test getcallingbundle exception state.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetCallingBundle_0100, Function | MediumTest | Level3)
{
    std::string empty = "";
    EXPECT_STREQ(empty.c_str(), context_->GetCallingBundle().c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetCallingBundle_0200
 * @tc.name: GetCallingBundle
 * @tc.desc: Test whether the return value of getcallingbundle is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetCallingBundle_0200, Function | MediumTest | Level1)
{
    Want want;
    std::string bundleName = "BundleName";
    std::string abilityName = "abilityName";
    want.SetElementName(bundleName, abilityName);

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->type = AppExecFwk::AbilityType::PAGE;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    std::shared_ptr<Ability> ability = std::make_shared<Ability>();
    ability->Init(abilityInfo, nullptr, handler, nullptr);
    ability->StartAbilityForResult(want, -1);
    ability->SetCallingContext("", "BundleName", "");

    EXPECT_STREQ(bundleName.c_str(), ability->GetCallingBundle().c_str());
}

/**
 * @tc.number: AaFwk_AbilityContext_GetCallingAbility_0200
 * @tc.name: GetCallingAbility
 * @tc.desc: Test getcallingability exception status.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_GetCallingAbility_0200, Function | MediumTest | Level3)
{
    EXPECT_NE(nullptr, context_->GetCallingAbility());
}

/**
 * @tc.number: AaFwk_AbilityContext_ConnectAbility_0100
 * @tc.name: ConnectAbility
 * @tc.desc: Test the attachbasecontext call to verify that the return value of connectability is true.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_ConnectAbility_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();

    abilityInfo->type = AppExecFwk::AbilityType::SERVICE;
    deal->SetAbilityInfo(abilityInfo);
    context_->AttachBaseContext(deal);

    Want want;
    bool ret = context_->ConnectAbility(want, nullptr);
    EXPECT_TRUE(ret);
}

/**
 * @tc.number: AaFwk_AbilityContext_ConnectAbility_0200
 * @tc.name: ConnectAbility
 * @tc.desc: Test the attachbasecontext call to verify that the return value of connectability is false.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_ConnectAbility_0200, Function | MediumTest | Level3)
{
    Want want;
    bool ret = context_->ConnectAbility(want, nullptr);
    EXPECT_FALSE(ret);
}

/**
 * @tc.number: AaFwk_AbilityContext_DisconnectAbility_0100
 * @tc.name: DisconnectAbility
 * @tc.desc: Test whether the disconnectability is called normally.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_DisconnectAbility_0100, Function | MediumTest | Level1)
{
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();

    abilityInfo->type = AppExecFwk::AbilityType::SERVICE;
    deal->SetAbilityInfo(abilityInfo);
    context_->AttachBaseContext(deal);
    context_->DisconnectAbility(nullptr);
}

/**
 * @tc.number: AaFwk_AbilityContext_StopAbility_0100
 * @tc.name: StopAbility
 * @tc.desc: Test the attachbasecontext call to verify that the return value of stopability is true.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_StopAbility_0100, Function | MediumTest | Level1)
{
    Want want;
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();

    abilityInfo->type = AppExecFwk::AbilityType::SERVICE;
    deal->SetAbilityInfo(abilityInfo);
    context_->AttachBaseContext(deal);
    bool ret = context_->StopAbility(want);
    EXPECT_TRUE(ret);
}

/**
 * @tc.number: AaFwk_AbilityContext_StopAbility_0200
 * @tc.name: StopAbility
 * @tc.desc: Test the attachbasecontext call to verify that the return value of stopability is false.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_StopAbility_0200, Function | MediumTest | Level3)
{
    Want want;
    bool ret = context_->StopAbility(want);
    EXPECT_FALSE(ret);
}

/**
 * @tc.number: AaFwk_AbilityContext_TerminateAbility_0200
 * @tc.name: TerminateAbility
 * @tc.desc: Test whether terminateability is called normally.
 */
HWTEST_F(AbilityContextTest, AaFwk_AbilityContext_TerminateAbility_0200, Function | MediumTest | Level1)
{
    int code = 1992;
    context_->TerminateAbility(code);
}

/**
 * @tc.number: AaFwk_Ability_GetHapModuleInfo_0100
 * @tc.name: GetHapModuleInfo
 * @tc.desc: Test the attachbasecontext call to verify that the return value of gethapmoduleinfo is correct.
 */
HWTEST_F(AbilityContextTest, AaFwk_Ability_GetHapModuleInfo_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_GetHapModuleInfo_0100 start";

    std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    std::string name = "Captain";
    abilityInfo->name = name;
    contextDeal->SetAbilityInfo(abilityInfo);
    context_->AttachBaseContext(contextDeal);
    Want want;
    std::shared_ptr<HapModuleInfo> info = context_->GetHapModuleInfo();
    EXPECT_STREQ(info->name.c_str(), name.c_str());
    GTEST_LOG_(INFO) << "AaFwk_Ability_GetHapModuleInfo_0100 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS
