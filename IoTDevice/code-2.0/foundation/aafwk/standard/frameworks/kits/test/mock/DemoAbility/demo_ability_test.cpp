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

#include "demo_ability_test.h"
#include <future>
#include <gtest/gtest.h>
#include <iostream>
#include "mock_lifecycle_observer.h"

using namespace std;
const int openfileValue = 123;
const int deleteValue = 234;
const int insertValue = 345;
const int updateValue = 456;
const int openRawFileValue = 567;
const int batchInsertValue = 789;
const int resultCodeCompare = 1992;

namespace OHOS {
namespace AppExecFwk {
void DemoAbility::OnStart(const Want &want)
{
    GTEST_LOG_(INFO) << "DemoAbility::OnStart called";
    std::shared_ptr<MockLifecycleObserver> observer = std::make_shared<MockLifecycleObserver>();
    Ability::GetLifecycle()->AddObserver(observer);
    Ability::OnStart(want);
    EXPECT_STREQ(std::string("abilityName").c_str(), want.GetElement().GetAbilityName().c_str());
}
void DemoAbility::OnStop()
{
    GTEST_LOG_(INFO) << "DemoAbility::OnStop called";
    Ability::OnStop();
}
void DemoAbility::OnActive()
{
    GTEST_LOG_(INFO) << "DemoAbility::OnActive called";
    Ability::OnActive();
    std::shared_ptr<AbilityInfo> abilityInfo = GetAbilityInfo();
    if (AppExecFwk::AbilityType::PAGE == abilityInfo->type) {
        EXPECT_NE(nullptr, Ability::GetWindow());
    }
}
void DemoAbility::OnInactive()
{
    GTEST_LOG_(INFO) << "DemoAbility::OnInactive called";
    Ability::OnInactive();
}
void DemoAbility::OnBackground()
{
    GTEST_LOG_(INFO) << "DemoAbility::OnBackground called";
    Ability::OnBackground();
}

void DemoAbility::OnForeground(const Want &want)
{
    GTEST_LOG_(INFO) << "DemoAbility::OnForeground called";
    Ability::OnForeground(want);
}

void DemoAbility::OnNewWant(const Want &want)
{
    GTEST_LOG_(INFO) << "DemoAbility::OnNewWant called";
}

void DemoAbility::OnRestoreAbilityState(const PacMap &inState)
{
    GTEST_LOG_(INFO) << "DemoAbility::OnRestoreAbilityState called";
}

void DemoAbility::OnSaveAbilityState(PacMap &outState)
{
    std::shared_ptr<ApplicationInfo> appInfo = GetApplicationInfo();
    std::shared_ptr<AbilityInfo> abilityInfo = GetAbilityInfo();

    EXPECT_STREQ(abilityInfo->resourcePath.c_str(), std::string("resourcePath").c_str());
    EXPECT_STREQ(appInfo->dataDir.c_str(), std::string("dataDir").c_str());
    EXPECT_STREQ(GetCodeCacheDir().c_str(), std::string("dataDir/code_cache").c_str());
    EXPECT_STREQ(GetCacheDir().c_str(), std::string("cacheDir").c_str());
    EXPECT_STREQ(GetDatabaseDir().c_str(), std::string("dataBaseDir").c_str());
    EXPECT_STREQ(GetDataDir().c_str(), std::string("dataDir").c_str());
    EXPECT_STREQ(GetBundleCodePath().c_str(), std::string("codePath").c_str());
    EXPECT_STREQ(GetBundleName().c_str(), std::string("bundleName").c_str());
    EXPECT_STREQ(GetBundleResourcePath().c_str(), std::string("resourcePath").c_str());
    EXPECT_STREQ(GetAppType().c_str(), std::string("system").c_str());

    EXPECT_NE(GetBundleManager(), nullptr);
    EXPECT_NE(GetApplicationContext(), nullptr);
    EXPECT_NE(GetContext(), nullptr);
    EXPECT_NE(GetAbilityManager(), nullptr);
    EXPECT_NE(GetProcessInfo(), nullptr);

    GTEST_LOG_(INFO) << "DemoAbility::OnSaveAbilityState called";
}

void DemoAbility::OnAbilityResult(int requestCode, int resultCode, const Want &resultData)
{
    GTEST_LOG_(INFO) << "DemoAbility::OnAbilityResult called";
    EXPECT_EQ(resultCodeCompare, resultCode);
}

sptr<IRemoteObject> DemoAbility::OnConnect(const Want &want)
{
    GTEST_LOG_(INFO) << "DemoAbility::OnConnect called";
    Ability::OnConnect(want);
    EXPECT_STREQ(std::string("abilityName").c_str(), want.GetElement().GetAbilityName().c_str());
    return nullptr;
}

void DemoAbility::OnDisconnect(const Want &want)
{
    GTEST_LOG_(INFO) << "DemoAbility::OnDisconnect called";
    Ability::OnDisconnect(want);
    EXPECT_STREQ(std::string("abilityName").c_str(), want.GetElement().GetAbilityName().c_str());
}

void DemoAbility::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    GTEST_LOG_(INFO) << "DemoAbility::OnCommand called";
    EXPECT_STREQ(std::string("abilityName").c_str(), want.GetElement().GetAbilityName().c_str());
    Ability::OnCommand(want, restart, startId);
}

std::vector<std::string> DemoAbility::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    GTEST_LOG_(INFO) << "DemoAbility::GetFileTypes called";
    std::vector<std::string> types;
    types.push_back("Type1");
    types.push_back("Type2");
    types.push_back("Type3");
    return types;
}

int DemoAbility::OpenFile(const Uri &uri, const std::string &mode)
{
    GTEST_LOG_(INFO) << "DemoAbility::OpenFile called";
    return openfileValue;
}

int DemoAbility::Delete(const Uri &uri, const DataAbilityPredicates &predicates)
{
    GTEST_LOG_(INFO) << "DemoAbility::Delete called";
    return deleteValue;
}

int DemoAbility::Insert(const Uri &uri, const ValuesBucket &value)
{
    GTEST_LOG_(INFO) << "DemoAbility::Insert called";
    return insertValue;
}

int DemoAbility::Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
{
    GTEST_LOG_(INFO) << "DemoAbility::Update called";
    return updateValue;
}

int DemoAbility::OpenRawFile(const Uri &uri, const std::string &mode)
{
    GTEST_LOG_(INFO) << "DemoAbility::OpenRawFile called";
    return openRawFileValue;
}

bool DemoAbility::Reload(const Uri &uri, const PacMap &extras)
{
    GTEST_LOG_(INFO) << "DemoAbility::Reload called";
    return true;
}

int DemoAbility::BatchInsert(const Uri &uri, const std::vector<ValuesBucket> &values)
{
    GTEST_LOG_(INFO) << "DemoAbility::BatchInsert called";
    return batchInsertValue;
}

std::string DemoAbility::GetType(const Uri &uri)
{
    GTEST_LOG_(INFO) << "DemoAbility::GetType called";
    std::string type("Type1");
    return type;
}

std::shared_ptr<ResultSet> DemoAbility::Query(
    const Uri &uri, const std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
{
    GTEST_LOG_(INFO) << "DemoAbility::Query called";
    std::shared_ptr<ResultSet> resultset = std::make_shared<ResultSet>("resultset");
    return resultset;
}

REGISTER_AA(DemoAbility)
}  // namespace AppExecFwk
}  // namespace OHOS
