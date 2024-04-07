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

#include "mock_application.h"
#include "ability.h"
#include "app_loader.h"

const int INMOCKAPPLICATION_ONE = 9996;
const int INMOCKAPPLICATION_TWO = 9997;
const int INMOCKAPPLICATION_THREE = 9998;
#define NUMBER (10)

namespace OHOS {
namespace AppExecFwk {

REGISTER_APPLICATION("MockTestApplication", MockApplication)

MockApplication::MockApplication()
{
    elementCallBack_ = std::make_shared<MockModuleElementsCallback>();
    lifecycleCallBack_ = std::make_shared<MockModuleLifecycleCallbacks>();
}

void MockApplication::OnConfigurationUpdated(const Configuration &config)
{
    GTEST_LOG_(INFO) << "MockApplication::OnConfigurationUpdated called";
    bool iscalled = true;
    EXPECT_TRUE(iscalled);
    if (INMOCKAPPLICATION_TWO == GetProcessInfo()->GetPid()) {
        RegisterElementsCallbacks(elementCallBack_);
        OHOSApplication::OnConfigurationUpdated(config);
        UnregisterElementsCallbacks(elementCallBack_);
        OHOSApplication::OnConfigurationUpdated(config);
    }
}

void MockApplication::OnMemoryLevel(int level)
{
    GTEST_LOG_(INFO) << "MockApplication::OnMemoryLevel called";
    bool iscalled = true;
    EXPECT_TRUE(iscalled);
    level_ = level;
    EXPECT_EQ(level, level_);
    if (INMOCKAPPLICATION_TWO == GetProcessInfo()->GetPid()) {
        RegisterElementsCallbacks(elementCallBack_);
        OHOSApplication::OnMemoryLevel(level);
        UnregisterElementsCallbacks(elementCallBack_);
        OHOSApplication::OnMemoryLevel(level);
    }
}

void MockApplication::OnForeground()
{
    GTEST_LOG_(INFO) << "MockApplication::OnForeground called";
    bool iscalled = true;
    EXPECT_TRUE(iscalled);
}

void MockApplication::OnBackground()
{
    GTEST_LOG_(INFO) << "MockApplication::OnBackground called";
    bool iscalled = true;
    EXPECT_TRUE(iscalled);
}

void MockApplication::OnStart()
{
    GTEST_LOG_(INFO) << "MockApplication::OnStart called";
    bool iscalled = true;
    EXPECT_TRUE(iscalled);
    int a = 1;
    EXPECT_EQ(1, a);

    if (INMOCKAPPLICATION_THREE == GetProcessInfo()->GetPid()) {
        EXPECT_STREQ("TestProcess", GetProcessInfo()->GetProcessName().c_str());
        EXPECT_STREQ("/hos/lib/cacheDir", GetCacheDir().c_str());
        EXPECT_STREQ("/hos/lib/cacheDir", GetCodeCacheDir().c_str());
        EXPECT_STREQ("/hos/lib/dataBaseDir", GetDatabaseDir().c_str());
        EXPECT_STREQ("/hos/lib/dataDir", GetDataDir().c_str());
        EXPECT_STREQ("/hos/lib/dataDir", GetDir("test", 1).c_str());
        EXPECT_STREQ("MockBundleName", GetBundleName().c_str());
        EXPECT_STREQ("MockTestApplication", GetApplicationInfo()->name.c_str());

        EXPECT_STREQ("", GetBundleCodePath().c_str());
        EXPECT_STREQ("", GetBundleResourcePath().c_str());
        EXPECT_EQ(nullptr, GetContext());
        EXPECT_EQ(nullptr, GetAbilityInfo());
        EXPECT_STREQ("/hos/lib/dataDir", GetApplicationContext()->GetDir("test", 1).c_str());
        std::vector<std::string> state;
        EXPECT_NE(nullptr, GetAbilityManager());
        if (nullptr != GetAbilityManager()) {
            GetAbilityManager()->DumpState("test", state);
        }

        EXPECT_NE(nullptr, GetBundleManager());
        if (nullptr != GetBundleManager()) {
            EXPECT_STREQ("ModuleTestType", GetBundleManager()->GetAppType("test").c_str());
        }
    } else if (INMOCKAPPLICATION_ONE == GetProcessInfo()->GetPid()) {
        RegisterAbilityLifecycleCallbacks(lifecycleCallBack_);

        std::shared_ptr<Ability> ability = std::make_shared<Ability>();
        PacMap pacmap;

        OHOSApplication::OnAbilityStart(ability);
        OHOSApplication::OnAbilityInactive(ability);
        OHOSApplication::OnAbilityBackground(ability);
        OHOSApplication::OnAbilityForeground(ability);
        OHOSApplication::OnAbilityActive(ability);
        OHOSApplication::OnAbilityStop(ability);
        OHOSApplication::OnAbilitySaveState(pacmap);

        UnregisterAbilityLifecycleCallbacks(lifecycleCallBack_);

        OHOSApplication::OnAbilityStart(ability);
        OHOSApplication::OnAbilityInactive(ability);
        OHOSApplication::OnAbilityBackground(ability);
        OHOSApplication::OnAbilityForeground(ability);
        OHOSApplication::OnAbilityActive(ability);
        OHOSApplication::OnAbilityStop(ability);
        OHOSApplication::OnAbilitySaveState(pacmap);
    }
}

void MockApplication::OnTerminate()
{
    GTEST_LOG_(INFO) << "MockApplication::OnTerminate called";
    bool iscalled = true;
    EXPECT_TRUE(iscalled);
}

void MockModuleElementsCallback::OnConfigurationUpdated(
    const std::shared_ptr<Ability> &ability, const Configuration &config)
{
    GTEST_LOG_(INFO) << "MockModuleElementsCallback::OnConfigurationUpdated called";
    EXPECT_STREQ(config.GetName().c_str(), "testConfig");
}

void MockModuleElementsCallback::OnMemoryLevel(int level)
{
    GTEST_LOG_(INFO) << "MockModuleElementsCallback::OnMemoryLevel called";
    EXPECT_EQ(level, NUMBER);
}

void MockModuleLifecycleCallbacks::OnAbilityStart(const std::shared_ptr<Ability> &ability)
{
    GTEST_LOG_(INFO) << "MockModuleLifecycleCallbacks::OnAbilityStart called";
}

void MockModuleLifecycleCallbacks::OnAbilityInactive(const std::shared_ptr<Ability> &ability)
{
    GTEST_LOG_(INFO) << "MockModuleLifecycleCallbacks::OnAbilityInactive called";
}

void MockModuleLifecycleCallbacks::OnAbilityBackground(const std::shared_ptr<Ability> &ability)
{
    GTEST_LOG_(INFO) << "MockModuleLifecycleCallbacks::OnAbilityBackground called";
}

void MockModuleLifecycleCallbacks::OnAbilityForeground(const std::shared_ptr<Ability> &ability)
{
    GTEST_LOG_(INFO) << "MockModuleLifecycleCallbacks::OnAbilityForeground called";
}

void MockModuleLifecycleCallbacks::OnAbilityActive(const std::shared_ptr<Ability> &ability)
{
    GTEST_LOG_(INFO) << "MockModuleLifecycleCallbacks::OnAbilityActive called";
}

void MockModuleLifecycleCallbacks::OnAbilityStop(const std::shared_ptr<Ability> &ability)
{
    GTEST_LOG_(INFO) << "MockModuleLifecycleCallbacks::OnAbilityStop called";
}

void MockModuleLifecycleCallbacks::OnAbilitySaveState(const PacMap &outState)
{
    GTEST_LOG_(INFO) << "MockModuleLifecycleCallbacks::OnAbilitySaveState called";
}

}  // namespace AppExecFwk
}  // namespace OHOS