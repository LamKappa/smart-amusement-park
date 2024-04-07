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
 * @tc.number: AaFwk_DataAbilityImplTest_BatchInsert_0100
 * @tc.name: BatchInsert
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_BatchInsert_001, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_BatchInsert_001 start";
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

    int ret;
    Uri uri("nullptr");
    std::vector<ValuesBucket> values;
    ret = dataabilityimpl->BatchInsert(uri, values);

    EXPECT_EQ(1, ret);
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_BatchInsert_001 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_BatchInsert_0200
 * @tc.name: BatchInsert
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_BatchInsert_002, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_BatchInsert_002 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();
    int ret;
    Uri uri("nullptr");
    std::vector<ValuesBucket> values;
    ret = dataabilityimpl->BatchInsert(uri, values);

    EXPECT_EQ(-1, ret);
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_BatchInsert_002 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_HandleAbilityTransaction_0100
 * @tc.name: GetFileTypes
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_HandleAbilityTransaction_001, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_HandleAbilityTransaction_001 start";
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

    Want want;
    LifeCycleStateInfo targetState;

    dataabilityimpl->HandleAbilityTransaction(want, targetState);
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_HandleAbilityTransaction_001 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_HandleAbilityTransaction_0200
 * @tc.name: GetFileTypes
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_HandleAbilityTransaction_002, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_HandleAbilityTransaction_002 start";
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

    Want want;
    LifeCycleStateInfo targetState;

    dataabilityimpl->HandleAbilityTransaction(want, targetState);
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_HandleAbilityTransaction_002 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS