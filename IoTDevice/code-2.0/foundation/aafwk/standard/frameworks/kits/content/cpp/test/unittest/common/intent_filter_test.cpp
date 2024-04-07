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

#include "ohos/aafwk/content/intent.h"
#include "ohos/aafwk/content/intent_filter.h"

using namespace testing::ext;
using namespace OHOS::AAFwk;
using OHOS::Parcel;

static const int LARGE_STR_LEN = 65534;
static const int SET_COUNT = 20;
static const int LOOP_TEST = 1000;

class IntentFilterBaseTest : public testing::Test {
public:
    IntentFilterBaseTest() : filter_(nullptr)
    {}
    ~IntentFilterBaseTest()
    {
        filter_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    IntentFilter *filter_;
    void CompareFilter(IntentFilter &filter1, IntentFilter &filter2);
    void SendParcelTest(IntentFilter &filter);
};

void IntentFilterBaseTest::SetUpTestCase(void)
{}

void IntentFilterBaseTest::TearDownTestCase(void)
{}

void IntentFilterBaseTest::SetUp(void)
{
    filter_ = new (std::nothrow) IntentFilter();
}

void IntentFilterBaseTest::TearDown(void)
{
    delete filter_;
    filter_ = nullptr;
}

/*
 * Feature: IntentFilter
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string is empty
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_Entity_001, TestSize.Level1)
{
    std::string setValue;
    filter_->SetEntity(setValue);
    EXPECT_EQ(setValue, filter_->GetEntity());
}

/*
 * Feature: IntentFilter
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string contains special characters
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_Entity_002, TestSize.Level1)
{
    std::string setValue("@#￥#3243adsafdf_中文");
    filter_->SetEntity(setValue);
    EXPECT_EQ(setValue, filter_->GetEntity());
}

/*
 * Feature: IntentFilter
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string has a long size
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_Entity_003, TestSize.Level1)
{
    std::string setValue(LARGE_STR_LEN, 's');
    filter_->SetEntity(setValue);
    EXPECT_EQ(setValue, filter_->GetEntity());
}

/*
 * Feature: IntentFilter
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string is overrode
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_Entity_004, TestSize.Level1)
{
    std::string setValue1("1234");
    filter_->SetEntity(setValue1);

    std::string setValue2("abcd");
    filter_->SetEntity(setValue2);

    EXPECT_EQ(setValue2, filter_->GetEntity());
}

/*
 * Feature: IntentFilter
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string is set 20 times
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_Entity_005, TestSize.Level1)
{
    std::string setValue("1234");
    for (int i = 0; i < SET_COUNT; i++) {
        filter_->SetEntity(setValue);
    }
    EXPECT_EQ(setValue, filter_->GetEntity());
}

/*
 * Feature: IntentFilter
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string is default
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_Entity_006, TestSize.Level1)
{
    std::string setValue;
    EXPECT_EQ(setValue, filter_->GetEntity());
}

/*
 * Feature: IntentFilter
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string contains special characters
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_Entity_007, TestSize.Level1)
{
    std::string setValue("@#￥#3243adsafdf_中文");
    for (int i = 0; i < LOOP_TEST; i++) {
        filter_->SetEntity(setValue);
        EXPECT_EQ(setValue, filter_->GetEntity());
    }
}

/*
 * Feature: IntentFilter
 * Function: action
 * SubFunction: NA
 * FunctionPoints: action
 * EnvConditions: NA
 * CaseDescription: Verify the function when action is not exist
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_Action_001, TestSize.Level1)
{
    std::string empty;
    std::string action = "action.system.test";
    EXPECT_EQ(0, filter_->CountAction());
    EXPECT_EQ(false, filter_->HasAction(action));
    EXPECT_EQ(empty, filter_->GetAction(0));

    filter_->RemoveAction(action);
    EXPECT_EQ(0, filter_->CountAction());
    EXPECT_EQ(false, filter_->HasAction(action));
}

/*
 * Feature: IntentFilter
 * Function: action
 * SubFunction: NA
 * FunctionPoints: action
 * EnvConditions: NA
 * CaseDescription: Verify the function when actions are same
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_Action_002, TestSize.Level1)
{
    std::string action = "action.system.test";
    int actionCount = 1;

    for (int i = 0; i < SET_COUNT; i++) {
        filter_->AddAction(action);
    }

    EXPECT_EQ(actionCount, filter_->CountAction());
    EXPECT_EQ(true, filter_->HasAction(action));
    EXPECT_EQ(action, filter_->GetAction(0));

    filter_->RemoveAction(action);
    EXPECT_EQ(0, filter_->CountAction());
    EXPECT_EQ(false, filter_->HasAction(action));
}

/*
 * Feature: IntentFilter
 * Function: action
 * SubFunction: NA
 * FunctionPoints: action
 * EnvConditions: NA
 * CaseDescription: Verify the function when actions are different
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_Action_003, TestSize.Level1)
{
    std::string actionPrefix = "action.system.test";

    for (int i = 0; i < SET_COUNT; i++) {
        std::string action = actionPrefix + std::to_string(i);
        filter_->AddAction(action);
    }

    EXPECT_EQ(SET_COUNT, filter_->CountAction());
    for (int i = 0; i < SET_COUNT; i++) {
        std::string action = actionPrefix + std::to_string(i);
        EXPECT_EQ(true, filter_->HasAction(action));
        EXPECT_EQ(action, filter_->GetAction(i));
    }

    int remove = SET_COUNT / 2;
    for (int i = 0; i < remove; i++) {
        std::string action = actionPrefix + std::to_string(i);
        filter_->RemoveAction(action);
    }

    EXPECT_EQ(remove, filter_->CountAction());
    for (int i = 0; i < remove; i++) {
        std::string action = actionPrefix + std::to_string(i);
        EXPECT_EQ(false, filter_->HasAction(action));
    }

    for (int i = remove; i < SET_COUNT; i++) {
        std::string action = actionPrefix + std::to_string(i);
        EXPECT_EQ(true, filter_->HasAction(action));
        EXPECT_EQ(action, filter_->GetAction(i - remove));
    }
}

/*
 * Feature: IntentFilter
 * Function: action
 * SubFunction: NA
 * FunctionPoints: action
 * EnvConditions: NA
 * CaseDescription: Verify the function when actions are same
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_Action_004, TestSize.Level1)
{
    std::string action = "action.system.test";
    int actionCount = 1;

    for (int i = 0; i < LOOP_TEST; i++) {
        filter_->AddAction(action);
        EXPECT_EQ(actionCount, filter_->CountAction());
        EXPECT_EQ(true, filter_->HasAction(action));
        EXPECT_EQ(action, filter_->GetAction(0));

        filter_->RemoveAction(action);
        EXPECT_EQ(0, filter_->CountAction());
        EXPECT_EQ(false, filter_->HasAction(action));
    }
}

void IntentFilterBaseTest::CompareFilter(IntentFilter &filter1, IntentFilter &filter2)
{
    EXPECT_EQ(filter1.GetEntity(), filter2.GetEntity());
    EXPECT_EQ(filter1.CountAction(), filter2.CountAction());

    int count = filter1.CountAction();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(filter1.GetAction(i), filter2.GetAction(i));
    }
}

void IntentFilterBaseTest::SendParcelTest(IntentFilter &filter)
{
    Parcel data;
    bool result;

    result = data.WriteParcelable(&filter);
    EXPECT_EQ(result, true);

    IntentFilter *filterNew = nullptr;
    filterNew = data.ReadParcelable<IntentFilter>();
    EXPECT_NE(filterNew, nullptr);

    if (filterNew) {
        CompareFilter(filter, *filterNew);
        delete filterNew;
    }
}

/*
 * Feature: IntentFilter
 * Function: marshall and unmarshall
 * SubFunction: NA
 * FunctionPoints: marshall and unmarshall
 * EnvConditions: NA
 * CaseDescription: Verify marshall and unmarshall when filter is empty
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_parcelable_001, TestSize.Level0)
{
    SendParcelTest(*filter_);
}

/*
 * Feature: IntentFilter
 * Function: marshall and unmarshall
 * SubFunction: NA
 * FunctionPoints: marshall and unmarshall
 * EnvConditions: NA
 * CaseDescription: Verify marshall and unmarshall when filter has action and entity
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_parcelable_002, TestSize.Level0)
{
    filter_->SetEntity("entity.system.test");
    filter_->AddAction("action.system.test1");
    filter_->AddAction("action.system.test2");

    SendParcelTest(*filter_);
}

/*
 * Feature: IntentFilter
 * Function: marshall and unmarshall
 * SubFunction: NA
 * FunctionPoints: marshall and unmarshall
 * EnvConditions: NA
 * CaseDescription: Verify marshall and unmarshall. Pressure test.
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_parcelable_003, TestSize.Level0)
{
    filter_->SetEntity("entity.system.test");
    filter_->AddAction("action.system.test1");
    filter_->AddAction("action.system.test2");

    for (int i = 0; i < LOOP_TEST; i++) {
        SendParcelTest(*filter_);
    }
}

/*
 * Feature: IntentFilter
 * Function: Match
 * SubFunction: NA
 * FunctionPoints: Match
 * EnvConditions: NA
 * CaseDescription: Verify the function. Pressure test.
 */
HWTEST_F(IntentFilterBaseTest, AaFwk_IntentFilter_match_005, TestSize.Level1)
{
    for (int i = 0; i < LOOP_TEST; i++) {
        Intent intent;
        intent.SetAction("action.system.action1");
        intent.SetEntity("entity.system.entity1");

        filter_->SetEntity("entity.system.entity1");
        filter_->AddAction("action.system.action1");
        filter_->AddAction("action.system.action2");

        EXPECT_EQ(true, filter_->Match(intent));
    }
}

using testFilterMatchType = std::tuple<std::string, std::string, bool>;
class IntentFilterMatchTest : public testing::TestWithParam<testFilterMatchType> {
public:
    IntentFilterMatchTest() : filter_(nullptr)
    {}
    ~IntentFilterMatchTest()
    {
        filter_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    IntentFilter *filter_;
};

void IntentFilterMatchTest::SetUpTestCase(void)
{}

void IntentFilterMatchTest::TearDownTestCase(void)
{}

void IntentFilterMatchTest::SetUp(void)
{
    filter_ = new (std::nothrow) IntentFilter();
}

void IntentFilterMatchTest::TearDown(void)
{
    delete filter_;
    filter_ = nullptr;
}

/*
 * Feature: IntentFilter
 * Function: Match
 * SubFunction: NA
 * FunctionPoints: Match
 * EnvConditions: NA
 * CaseDescription: Verify whether parameter change.
 *                         AaFwk_IntentFilter_match_001
 *                         AaFwk_IntentFilter_match_002
 *                         AaFwk_IntentFilter_match_003
 *                         AaFwk_IntentFilter_match_004
 */

HWTEST_P(IntentFilterMatchTest, AaFwk_IntentFilter_match, TestSize.Level0)
{
    std::string filterEntity = "entity.system.entity1";
    std::string filterAction1 = "action.system.action1";
    std::string filterAction2 = "action.system.action2";
    std::string intentEntity = std::get<0>(GetParam());
    std::string intentAction = std::get<1>(GetParam());
    bool result = std::get<2>(GetParam());

    filter_->SetEntity(filterEntity);
    filter_->AddAction(filterAction1);
    filter_->AddAction(filterAction2);

    Intent intent;
    intent.SetEntity(intentEntity);
    intent.SetAction(intentAction);

    EXPECT_EQ(result, filter_->Match(intent));
}

INSTANTIATE_TEST_CASE_P(IntentFilterMatchTestP, IntentFilterMatchTest,
    testing::Values(testFilterMatchType("entity.system.entityA", "action.system.actionA", false),
        testFilterMatchType("entity.system.entity1", "action.system.actionA", false),
        testFilterMatchType("entity.system.entityA", "action.system.action2", false),
        testFilterMatchType("entity.system.entity1", "action.system.action1", true)));
