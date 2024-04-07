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

#include "ohos_application.h"
#include "ability.h"
#include "mock_ability_lifecycle_callbacks.h"
#include "mock_element_callback.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AppExecFwk {
class ApplicationTest : public testing::Test {
public:
    ApplicationTest() : ApplicationTest_(nullptr)
    {}
    ~ApplicationTest()
    {
        ApplicationTest_ = nullptr;
    }
    OHOSApplication *ApplicationTest_ = nullptr;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ApplicationTest::SetUpTestCase(void)
{}

void ApplicationTest::TearDownTestCase(void)
{}

void ApplicationTest::SetUp(void)
{
    ApplicationTest_ = new (std::nothrow) OHOSApplication();
}

void ApplicationTest::TearDown(void)
{
    delete ApplicationTest_;
    ApplicationTest_ = nullptr;
}

/**
 * @tc.number: AppExecFwk_Application_RegisterAbilityLifecycleCallbacks_0100
 * @tc.name: RegisterAbilityLifecycleCallbacks
 * @tc.desc: Test whether registerabilitylifecyclecallbacks and are called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_RegisterAbilityLifecycleCallbacks_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_RegisterAbilityLifecycleCallbacks_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockAbilityLifecycleCallbacks> callback = std::make_shared<MockAbilityLifecycleCallbacks>();
        ApplicationTest_->RegisterAbilityLifecycleCallbacks(callback);

        std::shared_ptr<Ability> ability = std::make_shared<Ability>();
        ApplicationTest_->OnAbilityActive(ability);

        ApplicationTest_->UnregisterAbilityLifecycleCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_RegisterAbilityLifecycleCallbacks_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_RegisterAbilityLifecycleCallbacks_0200
 * @tc.name: RegisterAbilityLifecycleCallbacks
 * @tc.desc: Test the abnormal state of registerabilitylifecyclecallbacks.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_RegisterAbilityLifecycleCallbacks_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_RegisterAbilityLifecycleCallbacks_0200 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        ApplicationTest_->RegisterAbilityLifecycleCallbacks(nullptr);

        std::shared_ptr<Ability> ability = std::make_shared<Ability>();
        ApplicationTest_->OnAbilityActive(ability);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_RegisterAbilityLifecycleCallbacks_0200 end";
}

/**
 * @tc.number: AppExecFwk_Application_UnregisterAbilityLifecycleCallbacks_0100
 * @tc.name: UnregisterAbilityLifecycleCallbacks
 * @tc.desc: Test whether unregisterabilitylife callbacks is successfully called.
 */
HWTEST_F(
    ApplicationTest, AppExecFwk_Application_UnregisterAbilityLifecycleCallbacks_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_UnregisterAbilityLifecycleCallbacks_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockAbilityLifecycleCallbacks> callback = std::make_shared<MockAbilityLifecycleCallbacks>();
        ApplicationTest_->RegisterAbilityLifecycleCallbacks(callback);
        ApplicationTest_->UnregisterAbilityLifecycleCallbacks(callback);

        std::shared_ptr<Ability> ability = std::make_shared<Ability>();
        ApplicationTest_->OnAbilityActive(ability);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_UnregisterAbilityLifecycleCallbacks_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_UnregisterAbilityLifecycleCallbacks_0200
 * @tc.name: UnregisterAbilityLifecycleCallbacks
 * @tc.desc: Test the abnormal state of unregisterability lifecycle callbacks.
 */
HWTEST_F(
    ApplicationTest, AppExecFwk_Application_UnregisterAbilityLifecycleCallbacks_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_UnregisterAbilityLifecycleCallbacks_0200 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockAbilityLifecycleCallbacks> callback = std::make_shared<MockAbilityLifecycleCallbacks>();
        ApplicationTest_->RegisterAbilityLifecycleCallbacks(callback);
        ApplicationTest_->UnregisterAbilityLifecycleCallbacks(nullptr);

        std::shared_ptr<Ability> ability = std::make_shared<Ability>();
        ApplicationTest_->OnAbilityActive(ability);

        ApplicationTest_->UnregisterAbilityLifecycleCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_UnregisterAbilityLifecycleCallbacks_0200 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnAbilityStart_0100
 * @tc.name: OnAbilityStart
 * @tc.desc: Test whether onabilitystart is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_OnAbilityStart_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilityStart_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockAbilityLifecycleCallbacks> callback = std::make_shared<MockAbilityLifecycleCallbacks>();
        ApplicationTest_->RegisterAbilityLifecycleCallbacks(callback);

        std::shared_ptr<Ability> ability = std::make_shared<Ability>();
        ApplicationTest_->OnAbilityStart(ability);

        ApplicationTest_->UnregisterAbilityLifecycleCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilityStart_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnAbilityInactive_0100
 * @tc.name: OnAbilityInactive
 * @tc.desc: Test whether onabilityinactive is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_OnAbilityInactive_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilityInactive_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockAbilityLifecycleCallbacks> callback = std::make_shared<MockAbilityLifecycleCallbacks>();
        ApplicationTest_->RegisterAbilityLifecycleCallbacks(callback);

        std::shared_ptr<Ability> ability = std::make_shared<Ability>();
        ApplicationTest_->OnAbilityInactive(ability);

        ApplicationTest_->UnregisterAbilityLifecycleCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilityInactive_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnAbilityBackground_0100
 * @tc.name: OnAbilityBackground
 * @tc.desc: Test whether onabilitybackground is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_OnAbilityBackground_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilityBackground_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockAbilityLifecycleCallbacks> callback = std::make_shared<MockAbilityLifecycleCallbacks>();
        ApplicationTest_->RegisterAbilityLifecycleCallbacks(callback);

        std::shared_ptr<Ability> ability = std::make_shared<Ability>();
        ApplicationTest_->OnAbilityBackground(ability);

        ApplicationTest_->UnregisterAbilityLifecycleCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilityBackground_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnAbilityForeground_0100
 * @tc.name: OnAbilityForeground
 * @tc.desc: Test whether onabilityforegroup is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_OnAbilityForeground_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilityForeground_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockAbilityLifecycleCallbacks> callback = std::make_shared<MockAbilityLifecycleCallbacks>();
        ApplicationTest_->RegisterAbilityLifecycleCallbacks(callback);

        std::shared_ptr<Ability> ability = std::make_shared<Ability>();
        ApplicationTest_->OnAbilityForeground(ability);

        ApplicationTest_->UnregisterAbilityLifecycleCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilityForeground_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnAbilityActive_0100
 * @tc.name: OnAbilityActive
 * @tc.desc: Test whether onabilityactive is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_OnAbilityActive_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilityActive_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockAbilityLifecycleCallbacks> callback = std::make_shared<MockAbilityLifecycleCallbacks>();
        ApplicationTest_->RegisterAbilityLifecycleCallbacks(callback);

        std::shared_ptr<Ability> ability = std::make_shared<Ability>();
        ApplicationTest_->OnAbilityActive(ability);

        ApplicationTest_->UnregisterAbilityLifecycleCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilityActive_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnAbilityStop_0100
 * @tc.name: OnAbilityStop
 * @tc.desc: Test whether onabilitystop is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_OnAbilityStop_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilityStop_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockAbilityLifecycleCallbacks> callback = std::make_shared<MockAbilityLifecycleCallbacks>();
        ApplicationTest_->RegisterAbilityLifecycleCallbacks(callback);

        std::shared_ptr<Ability> ability = std::make_shared<Ability>();
        ApplicationTest_->OnAbilityStop(ability);

        ApplicationTest_->UnregisterAbilityLifecycleCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilityStop_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnConfigurationUpdated_0100
 * @tc.name: OnConfigurationUpdated
 * @tc.desc: Test whether onconfigurationupdated is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_OnConfigurationUpdated_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnConfigurationUpdated_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockElementsCallback> callback = std::make_shared<MockElementsCallback>();
        ApplicationTest_->RegisterElementsCallbacks(callback);

        Configuration configuration;
        ApplicationTest_->OnConfigurationUpdated(configuration);

        ApplicationTest_->UnregisterElementsCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnConfigurationUpdated_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnMemoryLevel_0100
 * @tc.name: OnMemoryLevel
 * @tc.desc: Test whether onmemorylevel is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_OnMemoryLevel_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnMemoryLevel_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockElementsCallback> callback = std::make_shared<MockElementsCallback>();
        ApplicationTest_->RegisterElementsCallbacks(callback);

        ApplicationTest_->OnMemoryLevel(1);

        ApplicationTest_->UnregisterElementsCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnMemoryLevel_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnStart_0100
 * @tc.name: OnStart
 * @tc.desc: Test whether OnStart is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_OnStart_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnStart_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        ApplicationTest_->OnStart();
    }

    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnStart_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnTerminate_0100
 * @tc.name: OnTerminate
 * @tc.desc: Test whether OnTerminate is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_OnTerminate_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnTerminate_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        ApplicationTest_->OnTerminate();
    }

    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnTerminate_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_DispatchAbilitySavedState_0100
 * @tc.name: DispatchAbilitySavedState
 * @tc.desc: Test whether dispatchabilitysavedstate is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_DispatchAbilitySavedState_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_DispatchAbilitySavedState_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockAbilityLifecycleCallbacks> callback = std::make_shared<MockAbilityLifecycleCallbacks>();
        ApplicationTest_->RegisterAbilityLifecycleCallbacks(callback);

        PacMap outState;
        ApplicationTest_->DispatchAbilitySavedState(outState);

        ApplicationTest_->UnregisterAbilityLifecycleCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_DispatchAbilitySavedState_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnAbilitySaveState_0100
 * @tc.name: OnAbilitySaveState
 * @tc.desc: Test whether the onablitysavestate is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_OnAbilitySaveState_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilitySaveState_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockAbilityLifecycleCallbacks> callback = std::make_shared<MockAbilityLifecycleCallbacks>();
        ApplicationTest_->RegisterAbilityLifecycleCallbacks(callback);

        PacMap outState;
        ApplicationTest_->OnAbilitySaveState(outState);

        ApplicationTest_->UnregisterAbilityLifecycleCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_OnAbilitySaveState_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnMemoryLevel_0100
 * @tc.name: OnMemoryLevel
 * @tc.desc: Test whether onmemorylevel is called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_RegisterElementsCallbacks_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_RegisterElementsCallbacks_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockElementsCallback> callback = std::make_shared<MockElementsCallback>();
        ApplicationTest_->RegisterElementsCallbacks(callback);

        ApplicationTest_->OnMemoryLevel(1);

        ApplicationTest_->UnregisterElementsCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_RegisterElementsCallbacks_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_OnMemoryLevel_0200
 * @tc.name: OnMemoryLevel
 * @tc.desc: Test the abnormal state of onmemorylevel.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_RegisterElementsCallbacks_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_RegisterElementsCallbacks_0200 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockElementsCallback> callback = std::make_shared<MockElementsCallback>();
        ApplicationTest_->RegisterElementsCallbacks(nullptr);

        ApplicationTest_->OnMemoryLevel(1);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_RegisterElementsCallbacks_0200 end";
}

/**
 * @tc.number: AppExecFwk_Application_UnregisterElementsCallbacks_0100
 * @tc.name: UnregisterElementsCallbacks
 * @tc.desc: Test whether unregisterelementcallbacks are called normally.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_UnregisterElementsCallbacks_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_UnregisterElementsCallbacks_0100 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockElementsCallback> callback = std::make_shared<MockElementsCallback>();
        ApplicationTest_->RegisterElementsCallbacks(callback);
        ApplicationTest_->UnregisterElementsCallbacks(callback);
        ApplicationTest_->OnMemoryLevel(1);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_UnregisterElementsCallbacks_0100 end";
}

/**
 * @tc.number: AppExecFwk_Application_UnregisterElementsCallbacks_0200
 * @tc.name: UnregisterElementsCallbacks
 * @tc.desc: Test the unregisterelementcallbacks exception state.
 */
HWTEST_F(ApplicationTest, AppExecFwk_Application_UnregisterElementsCallbacks_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AppExecFwk_Application_UnregisterElementsCallbacks_0200 start";

    EXPECT_NE(ApplicationTest_, nullptr);
    if (ApplicationTest_ != nullptr) {
        std::shared_ptr<MockElementsCallback> callback = std::make_shared<MockElementsCallback>();
        ApplicationTest_->RegisterElementsCallbacks(callback);
        ApplicationTest_->UnregisterElementsCallbacks(nullptr);
        ApplicationTest_->OnMemoryLevel(1);

        ApplicationTest_->UnregisterElementsCallbacks(callback);
    }
    GTEST_LOG_(INFO) << "AppExecFwk_Application_UnregisterElementsCallbacks_0200 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS