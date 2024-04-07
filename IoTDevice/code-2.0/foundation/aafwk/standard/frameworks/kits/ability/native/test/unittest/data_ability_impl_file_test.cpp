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
 * @tc.number: AaFwk_DataAbilityImplTest_GetType_0100
 * @tc.name: Query
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_GetType_001, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_GetType_001 start";
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

    EXPECT_STREQ("Type1", dataabilityimpl->GetType(uri).c_str());
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_GetType_001 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_GetType_0200
 * @tc.name: Query
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_GetType_002, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_GetType_002 start";
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
    std::shared_ptr<Ability> ability = nullptr;
    std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
    dataabilityimpl->Init(application, record, ability, handler, token, contextDeal);

    Uri uri("\nullptr");
    EXPECT_STREQ("", dataabilityimpl->GetType(uri).c_str());
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_GetType_002 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_GetFileTypes_0100
 * @tc.name: GetFileTypes
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_GetFileTypes_001, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_GetFileTypes_001 start";
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
    std::string mimeTypeFilter("nullptr");
    std::vector<std::string> types;

    types = dataabilityimpl->GetFileTypes(uri, mimeTypeFilter);

    if (types.size() != 0) {
        EXPECT_STREQ(mimeTypeFilter.c_str(), types.at(0).c_str());
    }
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_GetFileTypes_001 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_GetFileTypes_0200
 * @tc.name: GetFileTypes
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_GetFileTypes_002, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_GetFileTypes_002 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();
    Uri uri("\nullptr");
    std::string mimeTypeFilter("nullptr");
    std::vector<std::string> types;
    int number = 0;
    int typesSize;

    types = dataabilityimpl->GetFileTypes(uri, mimeTypeFilter);
    typesSize = types.size();
    EXPECT_EQ(number, typesSize);
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_GetFileTypes_002 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_OpenFile_0100
 * @tc.name: OpenFile
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_OpenFile_001, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_OpenFile_001 start";
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

    int fd = 1;
    int value;
    Uri uri("nullptr");
    std::string mode;
    value = dataabilityimpl->OpenFile(uri, mode);

    EXPECT_EQ(fd, value);
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_OpenFile_001 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_OpenFile_0200
 * @tc.name: OpenFile
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_OpenFile_002, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_OpenFile_002 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();
    int fd = -1;
    int value;
    Uri uri("nullptr");
    std::string mode;
    value = dataabilityimpl->OpenFile(uri, mode);

    EXPECT_EQ(fd, value);
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_OpenFile_002 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_OpenRawFile_0100
 * @tc.name: OpenRawFile
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_OpenRawFile_001, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_OpenRawFile_001 start";
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

    int fd = 1;
    int value;
    Uri uri("nullptr");
    std::string mode;
    value = dataabilityimpl->OpenRawFile(uri, mode);

    EXPECT_EQ(fd, value);
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_OpenRawFile_001 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_OpenRawFile_0200
 * @tc.name: OpenRawFile
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_OpenRawFile_002, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_OpenRawFile_002 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();
    int fd = -1;
    int value;
    Uri uri("nullptr");
    std::string mode;
    value = dataabilityimpl->OpenRawFile(uri, mode);

    EXPECT_EQ(fd, value);
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_OpenRawFile_002 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_Reload_0100
 * @tc.name: Reload
 * @tc.desc: Simulate successful test cases.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_Reload_001, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Reload_001 start";
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

    Uri uri("nullptr");
    PacMap extras;
    bool ret = dataabilityimpl->Reload(uri, extras);

    EXPECT_EQ(true, ret);
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Reload_001 end";
}

/**
 * @tc.number: AaFwk_DataAbilityImplTest_Reload_0200
 * @tc.name: Reload
 * @tc.desc: Validate when normally entering a string.
 */
HWTEST_F(DataAbilityImplTest, AaFwk_DataAbilityImplTest_Reload_002, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Reload_002 start";
    std::shared_ptr<DataAbilityImpl> dataabilityimpl = std::make_shared<DataAbilityImpl>();

    Uri uri("nullptr");
    PacMap extras;
    bool ret = dataabilityimpl->Reload(uri, extras);

    EXPECT_EQ(false, ret);
    sleep(1);
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityImplTest_Reload_002 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS