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

#include "ability_context.h"
#include "context_deal.h"
#include "ability_info.h"
#include "ability.h"
#include "mock_bundle_manager.h"
#include "sys_mgr_client.h"
#include "sa_mgr_client.h"
#include "mock_ability_manager_service.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

class AbilityPermissionTest : public testing::Test {
public:
    AbilityPermissionTest() : context_(nullptr)
    {}
    ~AbilityPermissionTest()
    {}
    std::shared_ptr<AbilityContext> context_;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AbilityPermissionTest::SetUpTestCase(void)
{
    OHOS::sptr<OHOS::IRemoteObject> bundleObject = new BundleMgrService();
    OHOS::sptr<OHOS::IRemoteObject> abilityObject = new OHOS::AAFwk::MockAbilityManagerService();

    auto sysMgr = OHOS::DelayedSingleton<SysMrgClient>::GetInstance();

    if (sysMgr == NULL) {
        GTEST_LOG_(ERROR) << "fail to get ISystemAbilityManager";
        return;
    }

    sysMgr->RegisterSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);
    sysMgr->RegisterSystemAbility(OHOS::ABILITY_MGR_SERVICE_ID, abilityObject);
}

void AbilityPermissionTest::TearDownTestCase(void)
{}

void AbilityPermissionTest::SetUp(void)
{
    context_ = std::make_shared<AbilityContext>();
}

void AbilityPermissionTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_AbilityPermissionTest_VerifySelfPermission_0100
 * @tc.name: VerifySelfPermission
 * @tc.desc: Verify that the verifyselfpermission return value is correct.
 */
HWTEST_F(AbilityPermissionTest, AaFwk_AbilityPermissionTest_VerifySelfPermission_0100, Function | MediumTest | Level1)
{
    if (context_ == nullptr) {
        EXPECT_EQ(true, context_ != nullptr);
        return;
    }

    std::string permission_name("permission_VerifySelfPermission");
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    std::string name = "hello";
    appInfo->bundleName = name;

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetApplicationInfo(appInfo);
    context_->AttachBaseContext(deal);

    EXPECT_EQ(0, context_->VerifySelfPermission(permission_name));
}

/**
 * @tc.number: AaFwk_AbilityPermissionTest_VerifyCallingOrSelfPermission_0100
 * @tc.name: VerifyCallingOrSelfPermission
 * @tc.desc: Verify that the VerifyCallingOrSelfPermission return value is correct.
 */
HWTEST_F(AbilityPermissionTest, AaFwk_AbilityPermissionTest_VerifyCallingOrSelfPermission_0100, Function | MediumTest | Level1)
{
    if (context_ == nullptr) {
        EXPECT_EQ(true, context_ != nullptr);
        return;
    }

    std::string permission_name("permission_VerifyCallingOrSelfPermission");
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    std::string name = "hello";
    appInfo->bundleName = name;

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetApplicationInfo(appInfo);

    context_->AttachBaseContext(deal);

    EXPECT_EQ(0, context_->VerifyCallingOrSelfPermission(permission_name));
}

/**
 * @tc.number: AaFwk_AbilityPermissionTest_VerifyPermission_0100
 * @tc.name: VerifyPermission
 * @tc.desc: Verify that the VerifyPermission return value is correct.
 */
HWTEST_F(AbilityPermissionTest, AaFwk_AbilityPermissionTest_VerifyPermission_0100, Function | MediumTest | Level1)
{
    if (context_ == nullptr) {
        EXPECT_EQ(true, context_ != nullptr);
        return;
    }

    std::string permission_name("permission_VerifyPermission");
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    std::string name = "hello";
    appInfo->bundleName = name;

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetApplicationInfo(appInfo);

    context_->AttachBaseContext(deal);

    EXPECT_EQ(0, context_->VerifyPermission(permission_name, 0, 10));
}

/**
 * @tc.number: AaFwk_AbilityPermissionTest_CanRequestPermission_0100
 * @tc.name: CanRequestPermission
 * @tc.desc: Verify that the CanRequestPermission return value is correct.
 */
HWTEST_F(AbilityPermissionTest, AaFwk_AbilityPermissionTest_CanRequestPermission_0100, Function | MediumTest | Level1)
{
    if (context_ == nullptr) {
        EXPECT_EQ(true, context_ != nullptr);
        return;
    }

    std::string permission_name("permission_CanRequestPermission");
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    std::string name = "hello";
    appInfo->bundleName = name;

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetApplicationInfo(appInfo);

    context_->AttachBaseContext(deal);
    EXPECT_EQ(true, context_->CanRequestPermission(permission_name));
}


/**
 * @tc.number: AaFwk_AbilityPermissionTest_RequestPermissionsFromUser_0100
 * @tc.name: RequestPermissionsFromUser
 * @tc.desc: Verify that requestpermissionsfromuser is called.
 */
HWTEST_F(AbilityPermissionTest, AaFwk_AbilityPermissionTest_RequestPermissionsFromUser_0100, Function | MediumTest | Level1)
{
    if (context_ == nullptr) {
        EXPECT_EQ(true, context_ != nullptr);
        return;
    }

    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    std::string name = "hello";
    appInfo->bundleName = name;
    std::vector<std::string> permissions;
    permissions.emplace_back("permission_1");
    permissions.emplace_back("permission_2");
    permissions.emplace_back("permission_3");

    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>();
    deal->SetApplicationInfo(appInfo);
    context_->AttachBaseContext(deal);
    context_->RequestPermissionsFromUser(permissions, 1004);
}
}  // namespace AppExecFwk
}  // namespace OHOS