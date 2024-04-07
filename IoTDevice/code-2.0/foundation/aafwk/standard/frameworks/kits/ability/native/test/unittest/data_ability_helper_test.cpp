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

#include "data_ability_helper.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-more-actions.h>
#include "mock_ability_manager_client.h"
#include "mock_ability_token.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using testing::_;
using testing::Invoke;
using testing::Return;

class DataAbilityHelperTest : public testing::Test {
public:
    DataAbilityHelperTest()
    {}
    ~DataAbilityHelperTest()
    {}

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DataAbilityHelperTest::SetUpTestCase(void)
{}

void DataAbilityHelperTest::TearDownTestCase(void)
{}

void DataAbilityHelperTest::SetUp(void)
{}

void DataAbilityHelperTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Create_0100
 * @tc.name: DataAbilityHelper
 * @tc.desc: Test the dataabilityhelper object.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Create_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Create_0100 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper1 = DataAbilityHelper::Creator(nullptr);
    EXPECT_EQ(helper1, nullptr);
    std::shared_ptr<DataAbilityHelper> helper2 = DataAbilityHelper::Creator(context);
    EXPECT_NE(helper2, nullptr);
    std::shared_ptr<DataAbilityHelper> helper3 = DataAbilityHelper::Creator(nullptr, uri);
    EXPECT_EQ(helper3, nullptr);
    std::shared_ptr<DataAbilityHelper> helper4 = DataAbilityHelper::Creator(nullptr, nullptr);
    EXPECT_EQ(helper4, nullptr);
    std::shared_ptr<DataAbilityHelper> helper5 = DataAbilityHelper::Creator(context, nullptr);
    EXPECT_EQ(helper5, nullptr);
    std::shared_ptr<DataAbilityHelper> helper6 = DataAbilityHelper::Creator(context, uri);
    EXPECT_NE(helper6, nullptr);
    std::shared_ptr<DataAbilityHelper> helper7 = DataAbilityHelper::Creator(nullptr, nullptr, false);
    EXPECT_EQ(helper7, nullptr);
    std::shared_ptr<DataAbilityHelper> helper8 = DataAbilityHelper::Creator(context, nullptr, false);
    EXPECT_EQ(helper8, nullptr);
    std::shared_ptr<DataAbilityHelper> helper9 = DataAbilityHelper::Creator(nullptr, uri, false);
    EXPECT_EQ(helper9, nullptr);
    std::shared_ptr<DataAbilityHelper> helper10 = DataAbilityHelper::Creator(context, uri, false);
    EXPECT_NE(helper10, nullptr);
    std::shared_ptr<DataAbilityHelper> helper11 = DataAbilityHelper::Creator(nullptr, nullptr, true);
    EXPECT_EQ(helper11, nullptr);
    std::shared_ptr<DataAbilityHelper> helper12 = DataAbilityHelper::Creator(context, nullptr, true);
    EXPECT_EQ(helper12, nullptr);
    std::shared_ptr<DataAbilityHelper> helper13 = DataAbilityHelper::Creator(nullptr, uri, true);
    EXPECT_EQ(helper13, nullptr);
    std::shared_ptr<DataAbilityHelper> helper14 = DataAbilityHelper::Creator(context, uri, true);
    EXPECT_NE(helper14, nullptr);

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Create_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Release_0100
 * @tc.name: Release
 * @tc.desc: Test whether the return value of release is true when the parameter passed by Creator is true.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Release_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Release_0100 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, true);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        EXPECT_EQ(true, helper->Release());
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Release_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Release_0200
 * @tc.name: Release
 * @tc.desc: Test whether the return value of release is false when the parameter passed by Creator is false.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Release_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Release_0200 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        EXPECT_EQ(false, helper->Release());
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Release_0200 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_GetFileTypes_0100
 * @tc.name: GetFileTypes
 * @tc.desc: When the parameter passed by Creator is true, test whether the return value of getfiletypes is correct.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_GetFileTypes_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_GetFileTypes_0100 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, true);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataability://com.example.myapplication5.DataAbilityTest");
        std::string mimeTypeFilter("mimeTypeFiltertest");
        std::vector<std::string> result = helper->GetFileTypes(uri2, mimeTypeFilter);

        int count = result.size();
        EXPECT_EQ(count, 3);

        std::vector<std::string> list;
        list.push_back("Types1");
        list.push_back("Types2");
        list.push_back("Types3");

        for (int i = 0; i < count; i++) {
            EXPECT_STREQ(result.at(i).c_str(), list.at(i).c_str());
        }
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_GetFileTypes_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_GetFileTypes_0200
 * @tc.name: GetFileTypes
 * @tc.desc: Test whether the return value of GetFileTypes is 0 when the parameter passed by Creator is false.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_GetFileTypes_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_GetFileTypes_0200 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, false);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataabilitytest://com.example.myapplication5.DataAbilityTest");
        std::string mimeTypeFilter("mimeTypeFiltertest");
        std::vector<std::string> result = helper->GetFileTypes(uri2, mimeTypeFilter);

        int count = result.size();
        EXPECT_EQ(count, 0);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_GetFileTypes_0200 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_OpenFile_0100
 * @tc.name: OpenFile
 * @tc.desc: Test whether the return value of OpenFile is 1246 when the parameter passed by Creator is true.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_OpenFile_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_OpenFile_0100 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, true);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataability://com.example.myapplication5.DataAbilityTest");
        std::string mode("modetest");
        int fd = helper->OpenFile(uri2, mode);

        EXPECT_EQ(fd, 1246);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_OpenFile_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_OpenFile_0200
 * @tc.name: OpenFile
 * @tc.desc: Test whether the return value of OpenFile is -1 when the parameter passed by Creator is false.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_OpenFile_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_OpenFile_0200 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, false);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataabilitytest://com.example.myapplication5.DataAbilityTest");
        std::string mode("modetest");
        int fd = helper->OpenFile(uri2, mode);

        EXPECT_EQ(fd, -1);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_OpenFile_0200 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Insert_0100
 * @tc.name: Insert
 * @tc.desc: Test whether the return value of Insert is 2345 when the parameter passed by Creator is true.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Insert_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Insert_0100 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, true);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataability://com.example.myapplication5.DataAbilityTest");
        ValuesBucket val("valtest");
        int index = helper->Insert(uri2, val);

        EXPECT_EQ(index, 2345);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Insert_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Insert_0200
 * @tc.name: Insert
 * @tc.desc: Test whether the return value of Insert is -1 when the parameter passed by Creator is false.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Insert_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Insert_0200 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, false);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataabilitytest://com.example.myapplication5.DataAbilityTest");
        ValuesBucket val("valtest");
        int index = helper->Insert(uri2, val);

        EXPECT_EQ(index, -1);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Insert_0200 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Update_0100
 * @tc.name: Update
 * @tc.desc: Test whether the return value of Update is 3456 when the parameter passed by Creator is true.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Update_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Update_0100 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, true);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataability://com.example.myapplication5.DataAbilityTest");
        ValuesBucket val("valtest");
        DataAbilityPredicates predicates("predicatestest");
        int index = helper->Update(uri2, val, predicates);

        EXPECT_EQ(index, 3456);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Update_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Update_0200
 * @tc.name: Update
 * @tc.desc: Test whether the return value of Update is -1 when the parameter passed by Creator is false.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Update_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Update_0200 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, false);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataabilitytest://com.example.myapplication5.DataAbilityTest");
        ValuesBucket val("valtest");
        DataAbilityPredicates predicates("predicatestest");
        int index = helper->Update(uri2, val, predicates);

        EXPECT_EQ(index, -1);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Update_0200 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Delete_0100
 * @tc.name: Delete
 * @tc.desc: Test whether the return value of Delete is 6789 when the parameter passed by Creator is true.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Delete_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Delete_0100 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, true);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataability://com.example.myapplication5.DataAbilityTest");
        DataAbilityPredicates predicates("predicatestest");
        int index = helper->Delete(uri2, predicates);

        EXPECT_EQ(index, 6789);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Delete_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Delete_0200
 * @tc.name: Delete
 * @tc.desc: Test whether the return value of Delete is -1 when the parameter passed by Creator is false.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Delete_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Delete_0200 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, false);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataabilitytest://com.example.myapplication5.DataAbilityTest");
        DataAbilityPredicates predicates("predicatestest");
        int index = helper->Delete(uri2, predicates);

        EXPECT_EQ(index, -1);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Delete_0200 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Query_0100
 * @tc.name: Query
 * @tc.desc: Test whether the return value of Query is null when the parameter passed by Creator is true.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Query_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Query_0100 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, true);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataability://com.example.myapplication5.DataAbilityTest");
        std::vector<std::string> columns;
        DataAbilityPredicates predicates("predicatestest");
        std::shared_ptr<ResultSet> set = helper->Query(uri2, columns, predicates);

        EXPECT_NE(set, nullptr);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Query_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Query_0200
 * @tc.name: Query
 * @tc.desc: Test whether the return value of Query is null when the parameter passed by Creator is false.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Query_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Query_0200 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, false);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataabilitytest://com.example.myapplication5.DataAbilityTest");
        std::vector<std::string> columns;
        DataAbilityPredicates predicates("predicatestest");
        std::shared_ptr<ResultSet> set = helper->Query(uri2, columns, predicates);

        EXPECT_EQ(set, nullptr);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Query_0200 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_GetType_0100
 * @tc.name: GetType
 * @tc.desc: Test whether the return value of GetType is Type1 when the parameter passed by Creator is true.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_GetType_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_GetType_0100 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, true);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataability://com.example.myapplication5.DataAbilityTest");
        std::string type = helper->GetType(uri2);

        EXPECT_STREQ(type.c_str(), "Type1");
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_GetType_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_GetType_0200
 * @tc.name: GetType
 * @tc.desc: Test whether the return value of GetType is Type1 when the parameter passed by Creator is false.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_GetType_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_GetType_0200 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, false);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataabilitytest://com.example.myapplication5.DataAbilityTest");
        std::string type = helper->GetType(uri2);

        EXPECT_STRNE(type.c_str(), "Type1");
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_GetType_0200 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_OpenRawFile_0100
 * @tc.name: OpenRawFile
 * @tc.desc: Test whether the return value of OpenRawFile is 5678 when the parameter passed by Creator is true.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_OpenRawFile_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_OpenRawFile_0100 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, true);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataability://com.example.myapplication5.DataAbilityTest");
        std::string mode("modetest");
        int fd = helper->OpenRawFile(uri2, mode);

        EXPECT_EQ(fd, 5678);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_OpenRawFile_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_OpenRawFile_0200
 * @tc.name: OpenRawFile
 * @tc.desc: Test whether the return value of OpenRawFile is -1 when the parameter passed by Creator is false.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_OpenRawFile_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_OpenRawFile_0200 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, false);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataabilitytest://com.example.myapplication5.DataAbilityTest");
        std::string mode("modetest");
        int fd = helper->OpenRawFile(uri2, mode);

        EXPECT_EQ(fd, -1);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_OpenRawFile_0200 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Reload_0100
 * @tc.name: Reload
 * @tc.desc: Test whether the return value of Reload is true when the parameter passed by Creator is true.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Reload_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Reload_0100 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, true);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataability://com.example.myapplication5.DataAbilityTest");
        PacMap extras;
        bool ret = helper->Reload(uri2, extras);

        EXPECT_EQ(ret, true);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Reload_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_Reload_0200
 * @tc.name: Reload
 * @tc.desc: Test whether the return value of Reload is false when the parameter passed by Creator is false.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_Reload_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Reload_0200 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, false);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataabilitytest://com.example.myapplication5.DataAbilityTest");
        PacMap extras;
        bool ret = helper->Reload(uri2, extras);

        EXPECT_EQ(ret, false);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_Reload_0200 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_BatchInsert_0100
 * @tc.name: BatchInsert
 * @tc.desc: Test whether the return value of BatchInsert is 789 when the parameter passed by Creator is true.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_BatchInsert_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_BatchInsert_0100 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, true);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataability://com.example.myapplication5.DataAbilityTest");
        std::vector<ValuesBucket> values;
        int ret = helper->BatchInsert(uri2, values);

        EXPECT_EQ(ret, 789);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_BatchInsert_0100 end";
}

/**
 * @tc.number: AaFwk_DataAbilityHelper_BatchInsert_0200
 * @tc.name: BatchInsert
 * @tc.desc: Test whether the return value of BatchInsert is -1 when the parameter passed by Creator is false.
 */
HWTEST_F(DataAbilityHelperTest, AaFwk_DataAbilityHelper_BatchInsert_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_BatchInsert_0200 start";

    std::shared_ptr<MockAbility> context = std::make_shared<MockAbility>();
    std::shared_ptr<Uri> uri = std::make_shared<Uri>("dataability://com.example.myapplication5.DataAbilityTest");
    std::shared_ptr<DataAbilityHelper> helper = DataAbilityHelper::Creator(context, uri, false);

    EXPECT_NE(helper, nullptr);
    if (helper != nullptr) {
        Uri uri2("dataabilitytest://com.example.myapplication5.DataAbilityTest");
        std::vector<ValuesBucket> values;
        int ret = helper->BatchInsert(uri2, values);

        EXPECT_EQ(ret, -1);
    }

    GTEST_LOG_(INFO) << "AaFwk_DataAbilityHelper_BatchInsert_0200 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS