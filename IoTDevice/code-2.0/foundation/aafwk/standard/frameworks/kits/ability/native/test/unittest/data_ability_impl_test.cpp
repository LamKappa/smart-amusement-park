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
#include "ability_loader.h"
#include "app_log_wrapper.h"
#include "data_ability_impl.h"
#include "mock_ability_token.h"
#include "mock_data_ability.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

REGISTER_AA(MockDataAbility)

class DataAbilityImplTest : public testing::Test {
public:
    DataAbilityImplTest() : dataabilityimpl(nullptr)
    {}
    ~DataAbilityImplTest()
    {
        dataabilityimpl = nullptr;
    }
    DataAbilityImpl *dataabilityimpl;

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DataAbilityImplTest::SetUpTestCase(void)
{}

void DataAbilityImplTest::TearDownTestCase(void)
{}

void DataAbilityImplTest::SetUp(void)
{}

void DataAbilityImplTest::TearDown(void)
{}


/**
 * @tc.number: AaFwk_DataAbilityImplTest_Insert_0100
 * @tc.name: Insert
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_Insert_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Insert_0100 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "MockDataAbility";
    abilityInfo->type = AbilityType::DATA;
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    std::shared_ptr<MockDataAbility> dataAbility = std::make_shared<MockDataAbility>();
    std::shared_ptr<Ability> ability;
    ability.reset(dataAbility.get());
    std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
    dataabilityimpl->Init(application, record, ability, handler, token, contextDeal);

    Uri uri("\nullptr");
    int number = 1;
    ValuesBucket value;

    EXPECT_EQ(number, dataabilityimpl->Insert(uri, value));
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Insert_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_Insert_0200
 * @tc.name: Insert
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_Insert_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Insert_0200 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();
    Uri uri("\nullptr");
    int number = -1;
    ValuesBucket value;

    EXPECT_EQ(number, dataabilityimpl->Insert(uri, value));
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Insert_0200 end";
}


/**
 * @tc.number: AaFwk_DataAbilityImplTest_Update_0100
 * @tc.name: Update
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_Update_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Update_0100 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "MockDataAbility";
    abilityInfo->type = AbilityType::DATA;
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    std::shared_ptr<MockDataAbility> dataAbility = std::make_shared<MockDataAbility>();
    std::shared_ptr<Ability> ability;
    ability.reset(dataAbility.get());
    std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
    dataabilityimpl->Init(application, record, ability, handler, token, contextDeal);

    Uri uri("\nullptr");
    int number = 1;
    ValuesBucket value;
    DataAbilityPredicates predicates;

    EXPECT_EQ(number, dataabilityimpl->Update(uri, value, predicates));
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Update_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_Update_0200
 * @tc.name: Update
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_Update_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Update_0200 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();
    Uri uri("\nullptr");
    int number = -1;
    ValuesBucket value;
    DataAbilityPredicates predicates;

    EXPECT_EQ(number, dataabilityimpl->Update(uri, value, predicates));
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Update_0200 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_Delete_0100
 * @tc.name: Delete
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_Delete_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Delete_0100 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "MockDataAbility";
    abilityInfo->type = AbilityType::DATA;
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    std::shared_ptr<MockDataAbility> dataAbility = std::make_shared<MockDataAbility>();
    std::shared_ptr<Ability> ability;
    ability.reset(dataAbility.get());
    std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
    dataabilityimpl->Init(application, record, ability, handler, token, contextDeal);

    Uri uri("\nullptr");
    int number = 1;

    DataAbilityPredicates predicates;

    EXPECT_EQ(number, dataabilityimpl->Delete(uri, predicates));
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Delete_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_Delete_0200
 * @tc.name: Delete
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_Delete_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Delete_0200 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();
    Uri uri("\nullptr");
    int number = -1;
    DataAbilityPredicates predicates;

    EXPECT_EQ(number, dataabilityimpl->Delete(uri, predicates));
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Delete_0200 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_Query_0100
 * @tc.name: Query
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_Query_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Query_0100 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "MockDataAbility";
    abilityInfo->type = AbilityType::DATA;
    sptr<IRemoteObject> token = sptr<IRemoteObject>(new (std::nothrow) MockAbilityToken());
    std::shared_ptr<AbilityLocalRecord> record = std::make_shared<AbilityLocalRecord>(abilityInfo, token);
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    std::shared_ptr<MockDataAbility> dataAbility = std::make_shared<MockDataAbility>();
    std::shared_ptr<Ability> ability;
    ability.reset(dataAbility.get());
    std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
    dataabilityimpl->Init(application, record, ability, handler, token, contextDeal);

    Uri uri("\nullptr");
    std::vector<std::string> columns;
    columns.push_back("string1");

    DataAbilityPredicates predicates;
    std::shared_ptr<ResultSet> set = dataabilityimpl->Query(uri, columns, predicates);

    if (set != nullptr) {
        EXPECT_STREQ("QueryTest", set->testInf_.c_str());
    }
    dataabilityimpl.reset();
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Query_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_Query_0200
 * @tc.name: Query
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_Query_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Query_0200 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();
    Uri uri("\nullptr");
    std::vector<std::string> columns;
    columns.push_back("string1");
    DataAbilityPredicates predicates;

    std::shared_ptr<ResultSet> set = dataabilityimpl->Query(uri, columns, predicates);
    EXPECT_EQ(nullptr, set);
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Query_0200 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS