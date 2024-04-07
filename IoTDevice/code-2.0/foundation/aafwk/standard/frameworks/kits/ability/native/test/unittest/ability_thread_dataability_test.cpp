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
#include <functional>
#include "ability_thread.h"
#include "ability_state.h"
#include "ability_loader.h"
#include "app_log_wrapper.h"
#include "ability_impl_factory.h"
#include "ability_impl.h"
#include "ability.h"
#include "context_deal.h"
#include "mock_page_ability.h"
#include "mock_ability_token.h"
#include "mock_ability_lifecycle_callbacks.h"
#include "mock_ability_impl.h"
#include "mock_ability_thread.h"
#include "mock_data_ability.h"
#include "ohos_application.h"
#include "page_ability_impl.h"
#include "uri.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

REGISTER_AA(MockDataAbility)
REGISTER_AA(MockPageAbility)

class AbilityThreadTest : public testing::Test {
public:
    AbilityThreadTest() : abilitythread_(nullptr)
    {}
    ~AbilityThreadTest()
    {
        abilitythread_ = nullptr;
    }
    AbilityThread *abilitythread_;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AbilityThreadTest::SetUpTestCase(void)
{}

void AbilityThreadTest::TearDownTestCase(void)
{}

void AbilityThreadTest::SetUp(void)
{
    GTEST_LOG_(INFO) << "AbilityThreadTest SetUp";
}

void AbilityThreadTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "AbilityThreadTest TearDown";
}

/**
 * @tc.number: AaFwk_AbilityThread_Query_0100
 * @tc.name: Query
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_Query_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Query_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockDataAbility";
        abilityInfo->type = AbilityType::DATA;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            abilitythread->Attach(application, abilityRecord, mainRunner);
            std::shared_ptr<MockDataAbility> mockdataability = std::make_shared<MockDataAbility>();

            Uri uri("dataabilitytest://com.example.myapplication5.DataAbilityTest");
            std::vector<std::string> columns;
            columns.push_back("string1");

            DataAbilityPredicates predicates("test");
            std::shared_ptr<ResultSet> resultSet = abilitythread->Query(uri, columns, predicates);
            EXPECT_STREQ(resultSet->testInf_.c_str(), "TestResultSet");
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Query_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_Query_0200
 * @tc.name: Query
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_Query_0200, Function | MediumTest | Level1)
{

    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Query_0200 start";
    AbilityThread *abilitythread = new AbilityThread();

    std::shared_ptr<AbilityImpl> abilityimpl = std::make_shared<AbilityImpl>();

    int valuetest = -1;
    int value = 0;
    Uri uri("\nullptr");
    std::vector<std::string> columns;
    columns.push_back("string1");
    DataAbilityPredicates predicates("test");

    abilitythread->Query(uri, columns, predicates);
    valuetest = abilityimpl->GetCurrentState();

    EXPECT_EQ(value, valuetest);

    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Query_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_GetFileTypes_0100
 * @tc.name: GetFileTypes
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_GetFileTypes_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_GetFileTypes_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockDataAbility";
        abilityInfo->type = AbilityType::DATA;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            abilitythread->Attach(application, abilityRecord, mainRunner);
            std::shared_ptr<MockDataAbility> mockdataability = std::make_shared<MockDataAbility>();

            Uri uri("dataabilitytest://com.example.myapplication5.DataAbilityTest");
            std::string mimeTypeFilter("nullptr");
            std::vector<std::string> types;

            types = abilitythread->GetFileTypes(uri, mimeTypeFilter);

            EXPECT_EQ((int)types.size(), 3);
            if (types.size() == 3) {
                EXPECT_STREQ("Type1", types.at(0).c_str());
                EXPECT_STREQ("Type2", types.at(1).c_str());
                EXPECT_STREQ("Type3", types.at(2).c_str());
            }
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_GetFileTypes_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_GetFileTypes_0200
 * @tc.name: GetFileTypes
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_GetFileTypes_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_GetFileTypes_0200 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        Uri uri("\nullptr");
        std::string mimeTypeFilter("nullptr");
        std::vector<std::string> types;
        int number = 0;
        int types_size;

        types = abilitythread->GetFileTypes(uri, mimeTypeFilter);

        types_size = types.size();
        EXPECT_EQ(number, types_size);
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_GetFileTypes_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_OpenFile_0100
 * @tc.name: OpenFile
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_OpenFile_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_OpenFile_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockDataAbility";
        abilityInfo->type = AbilityType::DATA;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            abilitythread->Attach(application, abilityRecord, mainRunner);
            std::shared_ptr<MockDataAbility> mockdataability = std::make_shared<MockDataAbility>();

            Uri uri("dataabilitytest://com.example.myapplication5.DataAbilityTest");
            std::string mode;
            int fd = abilitythread->OpenFile(uri, mode);

            EXPECT_EQ(fd, 11);
            sleep(1);
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_OpenFile_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_OpenFile_0200
 * @tc.name: OpenFile
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_OpenFile_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_OpenFile_0200 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        int fd = -1;
        int value;
        Uri uri("nullptr");
        std::string mode;
        value = abilitythread->OpenFile(uri, mode);

        EXPECT_EQ(fd, value);
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_OpenFile_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_Insert_0100
 * @tc.name: Insert
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_Insert_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Insert_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockDataAbility";
        abilityInfo->type = AbilityType::DATA;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            abilitythread->Attach(application, abilityRecord, mainRunner);
            std::shared_ptr<MockDataAbility> mockdataability = std::make_shared<MockDataAbility>();

            Uri uri("dataabilitytest://com.example.myapplication5.DataAbilityTest");
            ValuesBucket value;

            EXPECT_EQ(22, abilitythread->Insert(uri, value));
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Insert_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_Insert_0200
 * @tc.name: Insert
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_Insert_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Insert_0200 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        Uri uri("\nullptr");
        int number = -1;
        ValuesBucket value;

        EXPECT_EQ(number, abilitythread->Insert(uri, value));
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Insert_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_Update_0100
 * @tc.name: Update
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_Update_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Update_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        abilityInfo->name = "MockDataAbility";
        abilityInfo->type = AbilityType::DATA;
        sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
        EXPECT_NE(token, nullptr);
        if (token != nullptr) {
            std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
            std::shared_ptr<AbilityLocalRecord> abilityRecord =
                std::make_shared<AbilityLocalRecord>(abilityInfo, token);
            std::shared_ptr<EventRunner> mainRunner = EventRunner::Create(abilityInfo->name);
            abilitythread->Attach(application, abilityRecord, mainRunner);
            std::shared_ptr<MockDataAbility> mockdataability = std::make_shared<MockDataAbility>();

            Uri uri("dataabilitytest://com.example.myapplication5.DataAbilityTest");
            ValuesBucket value;
            DataAbilityPredicates predicates("test");

            EXPECT_EQ(33, abilitythread->Update(uri, value, predicates));
        }
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Update_0100 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_Update_0200
 * @tc.name: Update
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_Update_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Update_0200 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        Uri uri("\nullptr");
        int number = -1;
        ValuesBucket value;
        DataAbilityPredicates predicates("test");

        EXPECT_EQ(number, abilitythread->Update(uri, value, predicates));
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Update_0200 end";
}

/**
 * @tc.number: AaFwk_AbilityThread_Delete_0100
 * @tc.name: Delete
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(AbilityThreadTest, AaFwk_AbilityThread_Delete_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Delete_0100 start";

    AbilityThread *abilitythread = new (std::nothrow) AbilityThread();
    EXPECT_NE(abilitythread, nullptr);
    if (abilitythread != nullptr) {
        Uri uri("\nullptr");
        int number = -1;
        DataAbilityPredicates predicates("test");

        EXPECT_EQ(number, abilitythread->Delete(uri, predicates));
    }
    GTEST_LOG_(INFO) << "AaFwk_AbilityThread_Delete_0100 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS