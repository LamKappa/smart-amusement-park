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
#include <cstring>

#include "ohos/aafwk/content/intent.h"

using namespace testing::ext;
using namespace OHOS::AAFwk;

using testBoolType = std::tuple<std::string, std::string, bool, bool, bool>;
class IntentParametersBoolTest : public testing::TestWithParam<testBoolType> {
public:
    IntentParametersBoolTest() : intent_(nullptr)
    {}
    ~IntentParametersBoolTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersBoolTest::SetUpTestCase(void)
{}

void IntentParametersBoolTest::TearDownTestCase(void)
{}

void IntentParametersBoolTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersBoolTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetBoolParam/GetBoolParam
 * SubFunction: NA
 * FunctionPoints: SetBoolParam/GetBoolParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersBoolTest, AaFwk_Intent_Parameters_Bool, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    bool setValue = std::get<2>(GetParam());
    bool defaultValue = std::get<3>(GetParam());
    bool result = std::get<4>(GetParam());
    intent_->SetBoolParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetBoolParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(IntentParametersBoolTestCaseP, IntentParametersBoolTest,
    testing::Values(testBoolType("", "aa", true, true, true), testBoolType("", "aa", true, false, false),
        testBoolType("", "", true, true, true), testBoolType("", "", true, false, true),
        testBoolType("123", "123", true, false, true), testBoolType("123", "aa", true, false, false),
        testBoolType("-~*&%￥", "-~*&%￥", true, false, true), testBoolType("-~*&%￥", "aa", true, false, false),
        testBoolType("中文", "中文", true, false, true), testBoolType("中文", "aa", true, false, false),
        testBoolType("_中文ddPEJKJ#(&*~#^%", "_中文ddPEJKJ#(&*~#^%", true, false, true),
        testBoolType("_中文ddPEJKJ#(&*~#^%", "123", true, false, false)));

/*
 * Feature: Intent
 * Function: SetBoolParam/GetBoolParam
 * SubFunction: NA
 * FunctionPoints: SetBoolParam/GetBoolParam
 * EnvConditions: NA
 * CaseDescription: Verify when set twice and get twice
 */
HWTEST_F(IntentParametersBoolTest, AaFwk_Intent_Parameters_Bool_001, TestSize.Level1)
{
    std::string firstKey("_中文ddPEJKJ#(&*~#^%");
    std::string secondKey("key33");
    intent_->SetBoolParam(firstKey, true);
    intent_->SetBoolParam(secondKey, true);
    EXPECT_EQ(true, intent_->GetBoolParam(firstKey, false));
    EXPECT_EQ(true, intent_->GetBoolParam(secondKey, false));
}

/*
 * Feature: Intent
 * Function: SetBoolParam/GetBoolParam
 * SubFunction: NA
 * FunctionPoints: SetBoolParam/GetBoolParam
 * EnvConditions: NA
 * CaseDescription: Verify when set 20 times, and get once
 */
HWTEST_F(IntentParametersBoolTest, AaFwk_Intent_Parameters_Bool_002, TestSize.Level1)
{
    std::string keyStr("_中文ddPEJKJ#(&*~#^%");
    for (int i = 0; i < 20; i++) {
        intent_->SetBoolParam(keyStr, true);
    }
    EXPECT_EQ(true, intent_->GetBoolParam(keyStr, false));
}

using testBoolArrayType = std::tuple<std::string, std::string, std::vector<bool>, std::vector<bool>, std::vector<bool>>;
class IntentParametersBoolArrayTest : public testing::TestWithParam<testBoolArrayType> {
public:
    IntentParametersBoolArrayTest() : intent_(nullptr)
    {}
    ~IntentParametersBoolArrayTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersBoolArrayTest::SetUpTestCase(void)
{}

void IntentParametersBoolArrayTest::TearDownTestCase(void)
{}

void IntentParametersBoolArrayTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersBoolArrayTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetBoolArrayParam/GetBoolArrayParam
 * SubFunction: NA
 * FunctionPoints: SetBoolArrayParam/GetBoolArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersBoolArrayTest, AaFwk_Intent_Parameters_BoolArray, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<bool> setValue = std::get<2>(GetParam());
    std::vector<bool> result = std::get<4>(GetParam());
    intent_->SetBoolArrayParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetBoolArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(IntentParametersBoolArrayTestCaseP, IntentParametersBoolArrayTest,
    testing::Values(testBoolArrayType("", "aa", {true, false}, {}, {}),
        testBoolArrayType("", "", {true, false}, {}, {true, false}),
        testBoolArrayType("1*中_aR", "aa", {true, false}, {}, {}),
        testBoolArrayType("1*中_aR", "1*中_aR", {false, true}, {}, {false, true})));

/*
 * Feature: Intent
 * Function: SetBoolArrayParam/GetBoolArrayParam
 * SubFunction: NA
 * FunctionPoints: SetBoolArrayParam/GetBoolArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is bool array
 */
HWTEST_F(IntentParametersBoolArrayTest, AaFwk_Intent_Parameters_Bool_Array_001, TestSize.Level1)
{
    std::vector<bool> defaultValue;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, intent_->GetBoolArrayParam(getKey));
}

/*
 * Feature: Intent
 * Function: SetBoolArrayParam/GetBoolArrayParam
 * SubFunction: NA
 * FunctionPoints: SetBoolArrayParam/GetBoolArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is bool array
 */
HWTEST_F(IntentParametersBoolArrayTest, AaFwk_Intent_Parameters_Bool_Array_002, TestSize.Level1)
{
    std::string emptyStr("");
    std::vector<bool> firstValue({true, false});
    std::vector<bool> secondValue({true, true});
    std::vector<bool> firstDefaultValue({false, true});
    std::string keyStr("aa");
    intent_->SetBoolArrayParam(emptyStr, firstValue);
    intent_->SetBoolArrayParam(emptyStr, firstValue);
    intent_->SetBoolArrayParam(emptyStr, secondValue);
    std::vector<bool> defaultValue;
    EXPECT_EQ(defaultValue, intent_->GetBoolArrayParam(keyStr));
    intent_->SetBoolArrayParam(emptyStr, firstDefaultValue);
    EXPECT_EQ(firstDefaultValue, intent_->GetBoolArrayParam(emptyStr));
}

/*
 * Feature: Intent
 * Function: SetBoolArrayParam/GetBoolArrayParam
 * SubFunction: NA
 * FunctionPoints: SetBoolArrayParam/GetBoolArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is bool array
 */
HWTEST_F(IntentParametersBoolArrayTest, AaFwk_Intent_Parameters_Bool_Array_003, TestSize.Level1)
{
    std::string firstKey("%1uH3");
    std::vector<bool> firstValue({true, false});
    std::vector<bool> secondValue({true, true});
    std::string secondKey("aa");
    intent_->SetBoolArrayParam(firstKey, firstValue);
    intent_->SetBoolArrayParam(firstKey, firstValue);
    intent_->SetBoolArrayParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, intent_->GetBoolArrayParam(firstKey));
    intent_->SetBoolArrayParam(firstKey, firstValue);
    std::vector<bool> defaultValue;
    EXPECT_EQ(defaultValue, intent_->GetBoolArrayParam(secondKey));
}

using testByteType = std::tuple<std::string, std::string, byte, byte, byte>;
class IntentParametersByteTest : public testing::TestWithParam<testByteType> {
public:
    IntentParametersByteTest() : intent_(nullptr)
    {}
    ~IntentParametersByteTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersByteTest::SetUpTestCase(void)
{}

void IntentParametersByteTest::TearDownTestCase(void)
{}

void IntentParametersByteTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersByteTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetByteParam/GetByteParam
 * SubFunction: NA
 * FunctionPoints: SetByteParam/GetByteParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersByteTest, AaFwk_Intent_Parameters_Byte, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    byte setValue = std::get<2>(GetParam());
    byte defaultValue = std::get<3>(GetParam());
    byte result = std::get<4>(GetParam());
    intent_->SetByteParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetByteParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(IntentParametersByteTestCaseP, IntentParametersByteTest,
    testing::Values(testByteType("", "aa", '#', 'U', 'U'), testByteType("", "", 'N', 'K', 'N'),
        testByteType("1*中_aR", "aa", 'a', '%', '%'), testByteType("1*中_aR", "1*中_aR", 'a', 'z', 'a')));

/*
 * Feature: Intent
 * Function: SetByteParam/GetByteParam
 * SubFunction: NA
 * FunctionPoints: SetByteParam/GetByteParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is byte
 */
HWTEST_F(IntentParametersByteTest, AaFwk_Intent_Parameters_Byte_001, TestSize.Level1)
{
    byte defaultValue = '%';
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, intent_->GetByteParam(getKey, defaultValue));
}

/*
 * Feature: Intent
 * Function: SetByteParam/GetByteParam
 * SubFunction: NA
 * FunctionPoints: SetByteParam/GetByteParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is byte
 */
HWTEST_F(IntentParametersByteTest, AaFwk_Intent_Parameters_Byte_002, TestSize.Level1)
{
    std::string emptyStr("");
    byte firstValue = 'a';
    byte secondValue = '1';
    byte thirdValue = '*';
    byte firstDefaultValue = '2';
    byte secondDefaultValue = '!';
    std::string keyStr("aa");
    intent_->SetByteParam(emptyStr, firstValue);
    intent_->SetByteParam(emptyStr, firstValue);
    intent_->SetByteParam(emptyStr, secondValue);
    EXPECT_EQ(firstDefaultValue, intent_->GetByteParam(keyStr, firstDefaultValue));
    intent_->SetByteParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, intent_->GetByteParam(emptyStr, secondDefaultValue));
}

/*
 * Feature: Intent
 * Function: SetByteParam/GetByteParam
 * SubFunction: NA
 * FunctionPoints: SetByteParam/GetByteParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is byte
 */
HWTEST_F(IntentParametersByteTest, AaFwk_Intent_Parameters_Byte_003, TestSize.Level1)
{
    std::string firstKey("%1uH3");
    byte firstValue = 'a';
    byte secondValue = '_';
    byte firstDefaultValue = 'W';
    byte secondDefaultValue = '%';
    std::string secondKey("aa");
    intent_->SetByteParam(firstKey, firstValue);
    intent_->SetByteParam(firstKey, firstValue);
    intent_->SetByteParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, intent_->GetByteParam(firstKey, firstDefaultValue));
    intent_->SetByteParam(firstKey, firstValue);
    EXPECT_EQ(secondDefaultValue, intent_->GetByteParam(secondKey, secondDefaultValue));
}

using testByteArrayType = std::tuple<std::string, std::string, std::vector<byte>, std::vector<byte>, std::vector<byte>>;
class IntentParametersByteArrayTest : public testing::TestWithParam<testByteArrayType> {
public:
    IntentParametersByteArrayTest() : intent_(nullptr)
    {}
    ~IntentParametersByteArrayTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersByteArrayTest::SetUpTestCase(void)
{}

void IntentParametersByteArrayTest::TearDownTestCase(void)
{}

void IntentParametersByteArrayTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersByteArrayTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetByteArrayParam/GetByteArrayParam
 * SubFunction: NA
 * FunctionPoints: SetByteArrayParam/GetByteArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersByteArrayTest, AaFwk_Intent_Parameters_ByteArray, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<byte> setValue = std::get<2>(GetParam());
    std::vector<byte> result = std::get<4>(GetParam());
    intent_->SetByteArrayParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetByteArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(IntentParametersByteArrayTestCaseP, IntentParametersByteArrayTest,
    testing::Values(testByteArrayType("", "aa", {'*', 'D'}, {}, {}),
        testByteArrayType("", "", {'%', ')'}, {}, {'%', ')'}), testByteArrayType("1*中_aR", "aa", {'R', '.'}, {}, {}),
        testByteArrayType("1*中_aR", "1*中_aR", {'R', 'b'}, {}, {'R', 'b'})));

/*
 * Feature: Intent
 * Function: GetByteArrayParam
 * SubFunction: NA
 * FunctionPoints: GetByteArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is byte array
 */
HWTEST_F(IntentParametersByteArrayTest, AaFwk_Intent_Parameters_Byte_Array_001, TestSize.Level1)
{
    std::vector<byte> defaultValue;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, intent_->GetByteArrayParam(getKey));
}

/*
 * Feature: Intent
 * Function: SetByteArrayParam/GetByteArrayParam
 * SubFunction: NA
 * FunctionPoints: SetByteArrayParam/GetByteArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is byte array
 */
HWTEST_F(IntentParametersByteArrayTest, AaFwk_Intent_Parameters_Byte_Array_002, TestSize.Level1)
{
    std::string emptyStr("");
    std::vector<byte> firstValue({'a', '2'});
    std::vector<byte> secondValue({'1', 'd'});
    std::vector<byte> thirdValue({'t', '3'});
    std::string keyStr("aa");
    intent_->SetByteArrayParam(emptyStr, firstValue);
    intent_->SetByteArrayParam(emptyStr, firstValue);
    intent_->SetByteArrayParam(emptyStr, secondValue);
    std::vector<byte> defaultValue;
    EXPECT_EQ(defaultValue, intent_->GetByteArrayParam(keyStr));
    intent_->SetByteArrayParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, intent_->GetByteArrayParam(emptyStr));
}

/*
 * Feature: Intent
 * Function: SetByteArrayParam/GetByteArrayParam
 * SubFunction: NA
 * FunctionPoints: SetByteArrayParam/GetByteArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is byte array
 */
HWTEST_F(IntentParametersByteArrayTest, AaFwk_Intent_Parameters_Byte_Array_003, TestSize.Level1)
{
    std::string firstKey("%1uH3");
    std::vector<byte> firstValue({'a', '2'});
    std::vector<byte> secondValue({'w', '$'});
    std::vector<byte> defaultValue;
    std::string secondKey("aa");
    intent_->SetByteArrayParam(firstKey, firstValue);
    intent_->SetByteArrayParam(firstKey, firstValue);
    intent_->SetByteArrayParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, intent_->GetByteArrayParam(firstKey));
    intent_->SetByteArrayParam(firstKey, firstValue);
    EXPECT_EQ(defaultValue, intent_->GetByteArrayParam(secondKey));
}

using testCharType = std::tuple<std::string, std::string, zchar, zchar, zchar>;
class IntentParametersCharTest : public testing::TestWithParam<testCharType> {
public:
    IntentParametersCharTest() : intent_(nullptr)
    {}
    ~IntentParametersCharTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersCharTest::SetUpTestCase(void)
{}

void IntentParametersCharTest::TearDownTestCase(void)
{}

void IntentParametersCharTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersCharTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetCharParam/GetCharParam
 * SubFunction: NA
 * FunctionPoints: SetCharParam/GetCharParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersCharTest, AaFwk_Intent_Parameters_Char, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    zchar setValue = std::get<2>(GetParam());
    zchar defaultValue = std::get<3>(GetParam());
    zchar result = std::get<4>(GetParam());
    intent_->SetCharParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetCharParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(IntentParametersCharTestCaseP, IntentParametersCharTest,
    testing::Values(testCharType("", "aa", U'#', U'中', U'中'), testCharType("", "", U'中', U'K', U'中'),
        testCharType("1*中_aR", "aa", U'a', U'中', U'中'), testCharType("1*中_aR", "1*中_aR", U'中', U'z', U'中')));

/*
 * Feature: Intent
 * Function: SetCharParam/GetCharParam
 * SubFunction: NA
 * FunctionPoints: SetCharParam/GetCharParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is char
 */
HWTEST_F(IntentParametersCharTest, AaFwk_Intent_Parameters_Char_001, TestSize.Level1)
{
    zchar defaultValue = U'文';
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, intent_->GetCharParam(getKey, defaultValue));
}

/*
 * Feature: Intent
 * Function: SetCharParam/GetCharParam
 * SubFunction: NA
 * FunctionPoints: SetCharParam/GetCharParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is char
 */
HWTEST_F(IntentParametersCharTest, AaFwk_Intent_Parameters_Char_002, TestSize.Level1)
{
    std::string emptyStr("");
    zchar firstValue = U'中';
    zchar secondValue = U'文';
    zchar thirdValue = U'字';
    zchar firstDefaultValue = U'符';
    zchar secondDefaultValue = U'集';
    std::string keyStr("aa");
    intent_->SetCharParam(emptyStr, firstValue);
    intent_->SetCharParam(emptyStr, firstValue);
    intent_->SetCharParam(emptyStr, secondValue);
    EXPECT_EQ(firstDefaultValue, intent_->GetCharParam(keyStr, firstDefaultValue));
    intent_->SetCharParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, intent_->GetCharParam(emptyStr, secondDefaultValue));
}

/*
 * Feature: Intent
 * Function: SetCharParam/GetCharParam
 * SubFunction: NA
 * FunctionPoints: SetCharParam/GetCharParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is char
 */
HWTEST_F(IntentParametersCharTest, AaFwk_Intent_Parameters_Char_003, TestSize.Level1)
{
    std::string firstKey("%1uH3");
    zchar firstValue = U'中';
    zchar secondValue = U'文';
    zchar firstDefaultValue = U'字';
    zchar secondDefaultValue = U'符';
    std::string secondKey("aa");
    intent_->SetCharParam(firstKey, firstValue);
    intent_->SetCharParam(firstKey, firstValue);
    intent_->SetCharParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, intent_->GetCharParam(firstKey, firstDefaultValue));
    intent_->SetCharParam(firstKey, firstValue);
    EXPECT_EQ(secondDefaultValue, intent_->GetCharParam(secondKey, secondDefaultValue));
}

using testCharArrayType =
    std::tuple<std::string, std::string, std::vector<zchar>, std::vector<zchar>, std::vector<zchar>>;
class IntentParametersCharArrayTest : public testing::TestWithParam<testCharArrayType> {
public:
    IntentParametersCharArrayTest() : intent_(nullptr)
    {}
    ~IntentParametersCharArrayTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersCharArrayTest::SetUpTestCase(void)
{}

void IntentParametersCharArrayTest::TearDownTestCase(void)
{}

void IntentParametersCharArrayTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersCharArrayTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetCharArrayParam/GetCharArrayParam
 * SubFunction: NA
 * FunctionPoints: SetCharArrayParam/GetCharArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersCharArrayTest, AaFwk_Intent_Parameters_CharArray, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<zchar> setValue = std::get<2>(GetParam());
    std::vector<zchar> result = std::get<4>(GetParam());
    intent_->SetCharArrayParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetCharArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(IntentParametersCharArrayTestCaseP, IntentParametersCharArrayTest,
    testing::Values(testCharArrayType("", "aa", {U'中', U'文'}, {}, {}),
        testCharArrayType("", "", {U'中', U'文'}, {}, {U'中', U'文'}),
        testCharArrayType("1*中_aR", "aa", {U'中', U'文'}, {}, {}),
        testCharArrayType("1*中_aR", "1*中_aR", {U'中', U'文'}, {}, {U'中', U'文'})));

/*
 * Feature: Intent
 * Function: GetCharArrayParam
 * SubFunction: NA
 * FunctionPoints: GetCharArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is char array
 */
HWTEST_F(IntentParametersCharArrayTest, AaFwk_Intent_Parameters_Char_Array_001, TestSize.Level1)
{
    std::vector<zchar> defaultValue;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, intent_->GetCharArrayParam(getKey));
}

/*
 * Feature: Intent
 * Function: SetCharArrayParam/GetCharArrayParam
 * SubFunction: NA
 * FunctionPoints: SetCharArrayParam/GetCharArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is char array
 */
HWTEST_F(IntentParametersCharArrayTest, AaFwk_Intent_Parameters_Char_Array_002, TestSize.Level1)
{
    std::string emptyStr("");
    std::vector<zchar> firstValue({U'中', U'文'});
    std::vector<zchar> secondValue({U'字', U'符'});
    std::vector<zchar> thirdValue({U'集', U'英'});
    std::string keyStr("aa");
    intent_->SetCharArrayParam(emptyStr, firstValue);
    intent_->SetCharArrayParam(emptyStr, firstValue);
    intent_->SetCharArrayParam(emptyStr, secondValue);
    std::vector<zchar> defaultValue;
    EXPECT_EQ(defaultValue, intent_->GetCharArrayParam(keyStr));
    intent_->SetCharArrayParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, intent_->GetCharArrayParam(emptyStr));
}

/*
 * Feature: Intent
 * Function: SetCharArrayParam/GetCharArrayParam
 * SubFunction: NA
 * FunctionPoints: SetCharArrayParam/GetCharArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is char array
 */
HWTEST_F(IntentParametersCharArrayTest, AaFwk_Intent_Parameters_Char_Array_003, TestSize.Level1)
{
    std::string firstKey("%1uH3");
    std::vector<zchar> firstValue({U'中', U'文'});
    std::vector<zchar> secondValue({U'字', U'符'});
    std::vector<zchar> defaultValue;
    std::string secondKey("aa");
    intent_->SetCharArrayParam(firstKey, firstValue);
    intent_->SetCharArrayParam(firstKey, firstValue);
    intent_->SetCharArrayParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, intent_->GetCharArrayParam(firstKey));
    intent_->SetCharArrayParam(firstKey, firstValue);
    EXPECT_EQ(defaultValue, intent_->GetCharArrayParam(secondKey));
}

using testShortType = std::tuple<std::string, std::string, short, short, short>;
class IntentParametersShortTest : public testing::TestWithParam<testShortType> {
public:
    IntentParametersShortTest() : intent_(nullptr)
    {}
    ~IntentParametersShortTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersShortTest::SetUpTestCase(void)
{}

void IntentParametersShortTest::TearDownTestCase(void)
{}

void IntentParametersShortTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersShortTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetShortParam/GetShortParam
 * SubFunction: NA
 * FunctionPoints: SetShortParam/GetShortParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersShortTest, AaFwk_Intent_Parameters_Short, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    short setValue = std::get<2>(GetParam());
    short defaultValue = std::get<3>(GetParam());
    short result = std::get<4>(GetParam());
    intent_->SetShortParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetShortParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(IntentParametersShortTestCaseP, IntentParametersShortTest,
    testing::Values(testShortType("", "aa", -1, 100, 100), testShortType("", "", -9, -41, -9),
        testShortType("1*中_aR", "aa", 50, 5, 5), testShortType("1*中_aR", "1*中_aR", -5000, 5000, -5000)));

/*
 * Feature: Intent
 * Function: GetShortParam
 * SubFunction: NA
 * FunctionPoints: GetShortParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is short
 */
HWTEST_F(IntentParametersShortTest, AaFwk_Intent_Parameters_Short_001, TestSize.Level1)
{
    short defaultValue = 200;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, intent_->GetShortParam(getKey, defaultValue));
}

/*
 * Feature: Intent
 * Function: SetShortParam/GetShortParam
 * SubFunction: NA
 * FunctionPoints: SetShortParam/GetShortParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is short
 */
HWTEST_F(IntentParametersShortTest, AaFwk_Intent_Parameters_Short_002, TestSize.Level1)
{
    std::string emptyStr("");
    short firstValue = 1;
    short secondValue = 2;
    short thirdValue = 4;
    short firstDefaultValue = 3;
    short secondDefaultValue = 5;
    std::string keyStr("aa");
    intent_->SetShortParam(emptyStr, firstValue);
    intent_->SetShortParam(emptyStr, firstValue);
    intent_->SetShortParam(emptyStr, secondValue);
    EXPECT_EQ(firstDefaultValue, intent_->GetShortParam(keyStr, firstDefaultValue));
    intent_->SetShortParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, intent_->GetShortParam(emptyStr, secondDefaultValue));
}

/*
 * Feature: Intent
 * Function: SetShortParam/GetShortParam
 * SubFunction: NA
 * FunctionPoints: SetShortParam/GetShortParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is short
 */
HWTEST_F(IntentParametersShortTest, AaFwk_Intent_Parameters_Short_003, TestSize.Level1)
{
    std::string firstKey("%1uH3");
    short firstValue = -1;
    short secondValue = 0;
    short thirdValue = 4;
    short firstDefaultValue = 9;
    short secondDefaultValue = -10;
    std::string secondKey("aa");
    intent_->SetShortParam(firstKey, firstValue);
    intent_->SetShortParam(firstKey, firstValue);
    intent_->SetShortParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, intent_->GetShortParam(firstKey, firstDefaultValue));
    intent_->SetShortParam(firstKey, thirdValue);
    EXPECT_EQ(secondDefaultValue, intent_->GetShortParam(secondKey, secondDefaultValue));
}

using testShortArrayType =
    std::tuple<std::string, std::string, std::vector<short>, std::vector<short>, std::vector<short>>;
class IntentParametersShortArrayTest : public testing::TestWithParam<testShortArrayType> {
public:
    IntentParametersShortArrayTest() : intent_(nullptr)
    {}
    ~IntentParametersShortArrayTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersShortArrayTest::SetUpTestCase(void)
{}

void IntentParametersShortArrayTest::TearDownTestCase(void)
{}

void IntentParametersShortArrayTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersShortArrayTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetShortArrayParam/GetShortArrayParam
 * SubFunction: NA
 * FunctionPoints: SetShortArrayParam/GetShortArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersShortArrayTest, AaFwk_Intent_Parameters_ShortArray, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<short> setValue = std::get<2>(GetParam());
    std::vector<short> defaultValue = std::get<3>(GetParam());
    std::vector<short> result = std::get<4>(GetParam());
    intent_->SetShortArrayParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetShortArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(IntentParametersShortArrayTestCaseP, IntentParametersShortArrayTest,
    testing::Values(testShortArrayType("", "aa", {-1, 3, 25, -9}, {}, {}),
        testShortArrayType("", "", {-41, 0, 0, 9}, {}, {-41, 0, 0, 9}),
        testShortArrayType("1*中_aR", "aa", {50, 2, -9}, {}, {}),
        testShortArrayType("1*中_aR", "1*中_aR", {-5000}, {}, {-5000})));

/*
 * Feature: Intent
 * Function: GetShortArrayParam
 * SubFunction: NA
 * FunctionPoints: GetShortArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is short array
 */
HWTEST_F(IntentParametersShortArrayTest, AaFwk_Intent_Parameters_Short_Array_001, TestSize.Level1)
{
    std::vector<short> defaultValue;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, intent_->GetShortArrayParam(getKey));
}

/*
 * Feature: Intent
 * Function: SetShortArrayParam/GetShortArrayParam
 * SubFunction: NA
 * FunctionPoints: SetShortArrayParam/GetShortArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is short array
 */
HWTEST_F(IntentParametersShortArrayTest, AaFwk_Intent_Parameters_Short_Array_002, TestSize.Level1)
{
    std::string emptyStr("");
    std::vector<short> firstValue({1, 4, -9});
    std::vector<short> secondValue({1, 8, -9});
    std::vector<short> thirdValue({1, 4, 9});
    std::string keyStr("aa");
    intent_->SetShortArrayParam(emptyStr, firstValue);
    intent_->SetShortArrayParam(emptyStr, firstValue);
    intent_->SetShortArrayParam(emptyStr, secondValue);
    std::vector<short> defaultValue;
    EXPECT_EQ(defaultValue, intent_->GetShortArrayParam(keyStr));
    intent_->SetShortArrayParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, intent_->GetShortArrayParam(emptyStr));
}

/*
 * Feature: Intent
 * Function: SetShortArrayParam/GetShortArrayParam
 * SubFunction: NA
 * FunctionPoints: SetShortArrayParam/GetShortArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is short array
 */
HWTEST_F(IntentParametersShortArrayTest, AaFwk_Intent_Parameters_Short_Array_003, TestSize.Level1)
{
    std::string firstKey("%1uH3");
    std::vector<short> firstValue({-1, -2});
    std::vector<short> secondValue({-1, -2, -1, -2, 0});
    std::vector<short> thirdValue({-1, -2, 100});
    std::string secondKey("aa");
    intent_->SetShortArrayParam(firstKey, firstValue);
    intent_->SetShortArrayParam(firstKey, firstValue);
    intent_->SetShortArrayParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, intent_->GetShortArrayParam(firstKey));
    intent_->SetShortArrayParam(firstKey, thirdValue);
    std::vector<short> defaultValue;
    EXPECT_EQ(defaultValue, intent_->GetShortArrayParam(secondKey));
}

using testIntType = std::tuple<std::string, std::string, int, int, int>;
class IntentParametersIntTest : public testing::TestWithParam<testIntType> {
public:
    IntentParametersIntTest() : intent_(nullptr)
    {}
    ~IntentParametersIntTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersIntTest::SetUpTestCase(void)
{}

void IntentParametersIntTest::TearDownTestCase(void)
{}

void IntentParametersIntTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersIntTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetIntParam/GetIntParam
 * SubFunction: NA
 * FunctionPoints: SetIntParam/GetIntParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersIntTest, AaFwk_Intent_Parameters_Int, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    int setValue = std::get<2>(GetParam());
    int defaultValue = std::get<3>(GetParam());
    int result = std::get<4>(GetParam());
    intent_->SetIntParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetIntParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(IntentParametersIntTestCaseP, IntentParametersIntTest,
    testing::Values(testIntType("", "aa", -1, 100, 100), testIntType("", "", -9, -41, -9),
        testIntType("1*中_aR", "aa", 50, 5, 5), testIntType("1*中_aR", "1*中_aR", -5000, 5000, -5000)));

/*
 * Feature: Intent
 * Function: GetIntParam
 * SubFunction: NA
 * FunctionPoints: GetIntParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is integer
 */
HWTEST_F(IntentParametersIntTest, AaFwk_Intent_Parameters_Int_001, TestSize.Level1)
{
    int defaultValue = 200;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, intent_->GetIntParam(getKey, defaultValue));
}

/*
 * Feature: Intent
 * Function: SetIntParam/GetIntParam
 * SubFunction: NA
 * FunctionPoints: SetIntParam/GetIntParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is integer
 */
HWTEST_F(IntentParametersIntTest, AaFwk_Intent_Parameters_Int_002, TestSize.Level1)
{
    std::string emptyStr("");
    int firstValue = 1;
    int secondValue = 2;
    int thirdValue = 4;
    int firstDefaultValue = 3;
    int secondDefaultValue = 5;
    std::string keyStr("aa");
    intent_->SetIntParam(emptyStr, firstValue);
    intent_->SetIntParam(emptyStr, firstValue);
    intent_->SetIntParam(emptyStr, secondValue);
    EXPECT_EQ(firstDefaultValue, intent_->GetIntParam(keyStr, firstDefaultValue));
    intent_->SetIntParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, intent_->GetIntParam(emptyStr, secondDefaultValue));
}

/*
 * Feature: Intent
 * Function: SetIntParam/GetIntParam
 * SubFunction: NA
 * FunctionPoints: SetIntParam/GetIntParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is integer
 */
HWTEST_F(IntentParametersIntTest, AaFwk_Intent_Parameters_Int_003, TestSize.Level1)
{
    std::string firstKey("%1uH3");
    int firstValue = -1;
    int secondValue = 0;
    int thirdValue = 4;
    int firstDefaultValue = 9;
    int secondDefaultValue = -10;
    std::string secondKey("aa");
    intent_->SetIntParam(firstKey, firstValue);
    intent_->SetIntParam(firstKey, firstValue);
    intent_->SetIntParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, intent_->GetIntParam(firstKey, firstDefaultValue));
    intent_->SetIntParam(firstKey, thirdValue);
    EXPECT_EQ(secondDefaultValue, intent_->GetIntParam(secondKey, secondDefaultValue));
}

using testIntArrayType = std::tuple<std::string, std::string, std::vector<int>, std::vector<int>, std::vector<int>>;
class IntentParametersIntArrayTest : public testing::TestWithParam<testIntArrayType> {
public:
    IntentParametersIntArrayTest() : intent_(nullptr)
    {}
    ~IntentParametersIntArrayTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersIntArrayTest::SetUpTestCase(void)
{}

void IntentParametersIntArrayTest::TearDownTestCase(void)
{}

void IntentParametersIntArrayTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersIntArrayTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetIntArrayParam/GetIntArrayParam
 * SubFunction: NA
 * FunctionPoints: SetIntArrayParam/GetIntArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersIntArrayTest, AaFwk_Intent_Parameters_IntArray, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<int> setValue = std::get<2>(GetParam());
    std::vector<int> defaultValue = std::get<3>(GetParam());
    std::vector<int> result = std::get<4>(GetParam());
    intent_->SetIntArrayParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetIntArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(IntentParametersIntArrayTestCaseP, IntentParametersIntArrayTest,
    testing::Values(testIntArrayType("", "aa", {-1, 3, 25, -9}, {}, {}),
        testIntArrayType("", "", {-41, 0, 0, 9}, {}, {-41, 0, 0, 9}),
        testIntArrayType("1*中_aR", "aa", {50, 2, -9}, {}, {}),
        testIntArrayType("1*中_aR", "1*中_aR", {-5000}, {}, {-5000})));

/*
 * Feature: Intent
 * Function: GetIntArrayParam
 * SubFunction: NA
 * FunctionPoints: GetIntArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is integer array
 */
HWTEST_F(IntentParametersIntArrayTest, AaFwk_Intent_Parameters_Int_Array_001, TestSize.Level1)
{
    std::vector<int> defaultValue;
    std::string getKey("aa");
    EXPECT_EQ(defaultValue, intent_->GetIntArrayParam(getKey));
}

/*
 * Feature: Intent
 * Function: SetIntArrayParam/GetIntArrayParam
 * SubFunction: NA
 * FunctionPoints: SetIntArrayParam/GetIntArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is integer array
 */
HWTEST_F(IntentParametersIntArrayTest, AaFwk_Intent_Parameters_Int_Array_002, TestSize.Level1)
{
    std::string emptyStr("");
    std::vector<int> firstValue({1, 4, -9});
    std::vector<int> secondValue({1, 8, -9});
    std::vector<int> thirdValue({1, 4, 9});
    std::string keyStr("aa");
    intent_->SetIntArrayParam(emptyStr, firstValue);
    intent_->SetIntArrayParam(emptyStr, firstValue);
    intent_->SetIntArrayParam(emptyStr, secondValue);
    std::vector<int> defaultValue;
    EXPECT_EQ(defaultValue, intent_->GetIntArrayParam(keyStr));
    intent_->SetIntArrayParam(emptyStr, thirdValue);
    EXPECT_EQ(thirdValue, intent_->GetIntArrayParam(emptyStr));
}

/*
 * Feature: Intent
 * Function: SetIntArrayParam/GetIntArrayParam
 * SubFunction: NA
 * FunctionPoints: SetIntArrayParam/GetIntArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when the value is integer array
 */
HWTEST_F(IntentParametersIntArrayTest, AaFwk_Intent_Parameters_Int_Array_003, TestSize.Level1)
{
    std::string firstKey("%1uH3");
    std::vector<int> firstValue({-1, -2});
    std::vector<int> secondValue({-1, -2, -1, -2, 0});
    std::vector<int> thirdValue({-1, -2, 100});
    std::string secondKey("aa");
    intent_->SetIntArrayParam(firstKey, firstValue);
    intent_->SetIntArrayParam(firstKey, firstValue);
    intent_->SetIntArrayParam(firstKey, secondValue);
    EXPECT_EQ(secondValue, intent_->GetIntArrayParam(firstKey));
    intent_->SetIntArrayParam(firstKey, thirdValue);
    std::vector<int> defaultValue;
    EXPECT_EQ(defaultValue, intent_->GetIntArrayParam(secondKey));
}

using testLongType = std::tuple<std::string, std::string, long, long, long>;
class IntentParametersLongTest : public testing::TestWithParam<testLongType> {
public:
    IntentParametersLongTest() : intent_(nullptr)
    {}
    ~IntentParametersLongTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersLongTest::SetUpTestCase(void)
{}

void IntentParametersLongTest::TearDownTestCase(void)
{}

void IntentParametersLongTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersLongTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetLongParam/GetLongParam
 * SubFunction: NA
 * FunctionPoints: SetLongParam/GetLongParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersLongTest, AaFwk_Intent_Parameters_Long, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    long setValue = std::get<2>(GetParam());
    long defaultValue = std::get<3>(GetParam());
    long result = std::get<4>(GetParam());
    intent_->SetLongParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetLongParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(IntentParametersLongTestCaseP, IntentParametersLongTest,
    testing::Values(testLongType("", "aa", -1, 100, 100), testLongType("", "", -9, -41, -9),
        testLongType("1*中_aR", "aa", 50, 5, 5), testLongType("1*中_aR", "1*中_aR", -5000, 5000, -5000)));

/*
 * Feature: Intent
 * Function: SetLongParam & GetLongParam
 * SubFunction: NA
 * FunctionPoints: SetLongParam & GetLongParam
 * EnvConditions: NA
 * CaseDescription: get param when IntentParam is empty
 */
HWTEST_F(IntentParametersLongTest, AaFwk_Intent_Parameters_Long_001, TestSize.Level1)
{
    long defaultValue = 100;
    std::string key = "aa";
    EXPECT_EQ(defaultValue, intent_->GetLongParam(key, defaultValue));
}

/*
 * Feature: Intent
 * Function: SetLongParam & GetLongParam
 * SubFunction: NA
 * FunctionPoints: SetLongParam & GetLongParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, but get param of another nonexistent key
 */
HWTEST_F(IntentParametersLongTest, AaFwk_Intent_Parameters_Long_002, TestSize.Level1)
{
    std::string setKey1 = "";
    std::string setKey2 = "aa";
    long setValue1 = 1;
    long setValue2 = 5;
    intent_->SetLongParam(setKey1, setValue1);
    intent_->SetLongParam(setKey1, setValue1);
    setValue1 = 2;
    intent_->SetLongParam(setKey1, setValue1);
    setValue1 = 3;
    EXPECT_EQ(setValue1, intent_->GetLongParam(setKey2, setValue1));
    setValue1 = 4;
    intent_->SetLongParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetLongParam(setKey1, setValue2));
}

/*
 * Feature: Intent
 * Function: SetLongParam & GetLongParam
 * SubFunction: NA
 * FunctionPoints: SetLongParam & GetLongParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, then get param of the key
 */
HWTEST_F(IntentParametersLongTest, AaFwk_Intent_Parameters_Long_003, TestSize.Level1)
{
    std::string setKey1 = "%1uH3";
    std::string setKey2 = "aa";
    long setValue1 = -1;
    long setValue2 = 9;
    intent_->SetLongParam(setKey1, setValue1);
    intent_->SetLongParam(setKey1, setValue1);
    setValue1 = 0;
    intent_->SetLongParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetLongParam(setKey1, setValue2));
    setValue1 = 4;
    intent_->SetLongParam(setKey1, setValue1);
    setValue1 = -10;
    EXPECT_EQ(setValue1, intent_->GetLongParam(setKey2, setValue1));
}

using testLongArrayType = std::tuple<std::string, std::string, std::vector<long>, std::vector<long>, std::vector<long>>;
class IntentParametersLongArrayTest : public testing::TestWithParam<testLongArrayType> {
public:
    IntentParametersLongArrayTest() : intent_(nullptr)
    {}
    ~IntentParametersLongArrayTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersLongArrayTest::SetUpTestCase(void)
{}

void IntentParametersLongArrayTest::TearDownTestCase(void)
{}

void IntentParametersLongArrayTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersLongArrayTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetLongArrayParam/GetLongArrayParam
 * SubFunction: NA
 * FunctionPoints: SetLongArrayParam/GetLongArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersLongArrayTest, AaFwk_Intent_Parameters_LongArray, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<long> setValue = std::get<2>(GetParam());
    std::vector<long> defaultValue = std::get<3>(GetParam());
    std::vector<long> result = std::get<4>(GetParam());
    intent_->SetLongArrayParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetLongArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(IntentParametersLongArrayTestCaseP, IntentParametersLongArrayTest,
    testing::Values(testLongArrayType("", "aa", {-1, 3, 25, -9}, {}, {}),
        testLongArrayType("", "", {-41, 0, 0, 9}, {}, {-41, 0, 0, 9}),
        testLongArrayType("1*中_aR", "aa", {50, 2, -9}, {}, {}),
        testLongArrayType("1*中_aR", "1*中_aR", {-5000}, {}, {-5000})));

/*
 * Feature: Intent
 * Function: SetLongArrayParam & GetLongArrayParam
 * SubFunction: NA
 * FunctionPoints: SetLongArrayParam & GetLongArrayParam
 * EnvConditions: NA
 * CaseDescription: get param when IntentParam is empty
 */
HWTEST_F(IntentParametersLongArrayTest, AaFwk_Intent_Parameters_Long_Array_001, TestSize.Level1)
{
    std::vector<long> defaultValue;
    std::string key = "aa";
    EXPECT_EQ(defaultValue, intent_->GetLongArrayParam(key));
}

/*
 * Feature: Intent
 * Function: SetLongArrayParam & GetLongArrayParam
 * SubFunction: NA
 * FunctionPoints: SetLongArrayParam & GetLongArrayParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, but get param of another nonexistent key
 */
HWTEST_F(IntentParametersLongArrayTest, AaFwk_Intent_Parameters_Long_Array_002, TestSize.Level1)
{
    std::vector<long> defaultValue;
    std::string setKey1 = "";
    std::string setKey2 = "aa";
    std::vector<long> setValue1 = {1, 2};
    intent_->SetLongArrayParam(setKey1, setValue1);
    intent_->SetLongArrayParam(setKey1, setValue1);
    setValue1 = {2, 3};
    intent_->SetLongArrayParam(setKey1, setValue1);
    EXPECT_EQ(defaultValue, intent_->GetLongArrayParam(setKey2));
    setValue1 = {4, 5};
    intent_->SetLongArrayParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetLongArrayParam(setKey1));
}

/*
 * Feature: Intent
 * Function: SetLongArrayParam & GetLongArrayParam
 * SubFunction: NA
 * FunctionPoints: SetLongArrayParam & GetLongArrayParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, then get param of the key
 */
HWTEST_F(IntentParametersLongArrayTest, AaFwk_Intent_Parameters_Long_Array_003, TestSize.Level1)
{
    std::vector<long> defaultValue;
    std::string setKey1 = "%1uH3";
    std::string setKey2 = "aa";
    std::vector<long> setValue1 = {-1, -2};
    intent_->SetLongArrayParam(setKey1, setValue1);
    intent_->SetLongArrayParam(setKey1, setValue1);
    setValue1 = {0, 1};
    intent_->SetLongArrayParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetLongArrayParam(setKey1));
    setValue1 = {4, 5};
    intent_->SetLongArrayParam(setKey1, setValue1);
    EXPECT_EQ(defaultValue, intent_->GetLongArrayParam(setKey2));
}

using testFloatType = std::tuple<std::string, std::string, float, float, float>;
class IntentParametersFloatTest : public testing::TestWithParam<testFloatType> {
public:
    IntentParametersFloatTest() : intent_(nullptr)
    {}
    ~IntentParametersFloatTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersFloatTest::SetUpTestCase(void)
{}

void IntentParametersFloatTest::TearDownTestCase(void)
{}

void IntentParametersFloatTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersFloatTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetFloatParam/GetFloatParam
 * SubFunction: NA
 * FunctionPoints: SetFloatParam/GetFloatParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersFloatTest, AaFwk_Intent_Parameters_Float, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    float setValue = std::get<2>(GetParam());
    float defaultValue = std::get<3>(GetParam());
    float result = std::get<4>(GetParam());
    intent_->SetFloatParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetFloatParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(IntentParametersFloatTestCaseP, IntentParametersFloatTest,
    testing::Values(testFloatType("", "aa", -1.1, 100.1, 100.1), testFloatType("", "", -9.1, -41.1, -9.1),
        testFloatType("1*中_aR", "aa", 50.1, 5.1, 5.1), testFloatType("1*中_aR", "1*中_aR", -5000.1, 5000.1, -5000.1)));

/*
 * Feature: Intent
 * Function: SetFloatParam & GetFloatParam
 * SubFunction: NA
 * FunctionPoints: SetFloatParam & GetFloatParam
 * EnvConditions: NA
 * CaseDescription: get param when IntentParam is empty
 */
HWTEST_F(IntentParametersFloatTest, AaFwk_Intent_Parameters_Float_001, TestSize.Level1)
{
    float defaultValue = 100.1;
    std::string key = "aa";
    EXPECT_EQ(defaultValue, intent_->GetFloatParam(key, defaultValue));
}

/*
 * Feature: Intent
 * Function: SetFloatParam & GetFloatParam
 * SubFunction: NA
 * FunctionPoints: SetFloatParam & GetFloatParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, but get param of another nonexistent key
 */
HWTEST_F(IntentParametersFloatTest, AaFwk_Intent_Parameters_Float_002, TestSize.Level1)
{
    std::string setKey1 = "";
    std::string setKey2 = "aa";
    float setValue1 = 1.1;
    float setValue2 = 5.1;
    intent_->SetFloatParam(setKey1, setValue1);
    intent_->SetFloatParam(setKey1, setValue1);
    setValue1 = 2.1;
    intent_->SetFloatParam(setKey1, setValue1);
    setValue1 = 3.1;
    EXPECT_EQ(setValue1, intent_->GetFloatParam(setKey2, setValue1));
    setValue1 = 4.1;
    intent_->SetFloatParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetFloatParam(setKey1, setValue2));
}

/*
 * Feature: Intent
 * Function: SetFloatParam & GetFloatParam
 * SubFunction: NA
 * FunctionPoints: SetFloatParam & GetFloatParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, then get param of the key
 */
HWTEST_F(IntentParametersFloatTest, AaFwk_Intent_Parameters_Float_003, TestSize.Level1)
{
    std::string setKey1 = "%1uH3";
    std::string setKey2 = "aa";
    float setValue1 = -1.1;
    float setValue2 = 9.1;
    intent_->SetFloatParam(setKey1, setValue1);
    intent_->SetFloatParam(setKey1, setValue1);
    setValue1 = 0.1;
    intent_->SetFloatParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetFloatParam(setKey1, setValue2));
    setValue1 = 4.1;
    intent_->SetFloatParam(setKey1, setValue1);
    setValue1 = -10.1;
    EXPECT_EQ(setValue1, intent_->GetFloatParam(setKey2, setValue1));
}

using testFloatArrayType =
    std::tuple<std::string, std::string, std::vector<float>, std::vector<float>, std::vector<float>>;
class IntentParametersFloatArrayTest : public testing::TestWithParam<testFloatArrayType> {
public:
    IntentParametersFloatArrayTest() : intent_(nullptr)
    {}
    ~IntentParametersFloatArrayTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersFloatArrayTest::SetUpTestCase(void)
{}

void IntentParametersFloatArrayTest::TearDownTestCase(void)
{}

void IntentParametersFloatArrayTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersFloatArrayTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetFloatArrayParam/GetFloatArrayParam
 * SubFunction: NA
 * FunctionPoints: SetFloatArrayParam/GetFloatArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersFloatArrayTest, AaFwk_Intent_Parameters_FloatArray, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<float> setValue = std::get<2>(GetParam());
    std::vector<float> defaultValue = std::get<3>(GetParam());
    std::vector<float> result = std::get<4>(GetParam());
    intent_->SetFloatArrayParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetFloatArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(IntentParametersFloatArrayTestCaseP, IntentParametersFloatArrayTest,
    testing::Values(testFloatArrayType("", "aa", {-1.1, -2.1}, {}, {}),
        testFloatArrayType("", "", {-41.1, -42.1}, {}, {-41.1, -42.1}),
        testFloatArrayType("1*中_aR", "aa", {50.1, 51.1}, {}, {}),
        testFloatArrayType("1*中_aR", "1*中_aR", {5000.1, 5001.1}, {}, {5000.1, 5001.1})));

/*
 * Feature: Intent
 * Function: SetFloatArrayParam & GetFloatArrayParam
 * SubFunction: NA
 * FunctionPoints: SetFloatArrayParam & GetFloatArrayParam
 * EnvConditions: NA
 * CaseDescription: get param when IntentParam is empty
 */
HWTEST_F(IntentParametersFloatArrayTest, AaFwk_Intent_Parameters_Float_Array_001, TestSize.Level1)
{
    std::vector<float> defaultValue;
    std::string key = "aa";
    EXPECT_EQ(defaultValue, intent_->GetFloatArrayParam(key));
}

/*
 * Feature: Intent
 * Function: SetFloatArrayParam & GetFloatArrayParam
 * SubFunction: NA
 * FunctionPoints: SetFloatArrayParam & GetFloatArrayParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, but get param of another nonexistent key
 */
HWTEST_F(IntentParametersFloatArrayTest, AaFwk_Intent_Parameters_Float_Array_002, TestSize.Level1)
{
    std::vector<float> defaultValue;
    std::string setKey1 = "";
    std::string setKey2 = "aa";
    std::vector<float> setValue1 = {1.1, 2.1};
    intent_->SetFloatArrayParam(setKey1, setValue1);
    intent_->SetFloatArrayParam(setKey1, setValue1);
    setValue1 = {2.1, 3.1};
    intent_->SetFloatArrayParam(setKey1, setValue1);
    EXPECT_EQ(defaultValue, intent_->GetFloatArrayParam(setKey2));
    setValue1 = {4.1, 5.1};
    intent_->SetFloatArrayParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetFloatArrayParam(setKey1));
}

/*
 * Feature: Intent
 * Function: SetFloatArrayParam & GetFloatArrayParam
 * SubFunction: NA
 * FunctionPoints: SetFloatArrayParam & GetFloatArrayParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, then get param of the key
 */
HWTEST_F(IntentParametersFloatArrayTest, AaFwk_Intent_Parameters_Float_Array_003, TestSize.Level1)
{
    std::vector<float> defaultValue;
    std::string setKey1 = "%1uH3";
    std::string setKey2 = "aa";
    std::vector<float> setValue1 = {-1.1, -2.1};
    intent_->SetFloatArrayParam(setKey1, setValue1);
    intent_->SetFloatArrayParam(setKey1, setValue1);
    setValue1 = {0.1, 1.1};
    intent_->SetFloatArrayParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetFloatArrayParam(setKey1));
    setValue1 = {4.1, 5.1};
    intent_->SetFloatArrayParam(setKey1, setValue1);
    EXPECT_EQ(defaultValue, intent_->GetFloatArrayParam(setKey2));
}

using testDoubleType = std::tuple<std::string, std::string, double, double, double>;
class IntentParametersDoubleTest : public testing::TestWithParam<testDoubleType> {
public:
    IntentParametersDoubleTest() : intent_(nullptr)
    {}
    ~IntentParametersDoubleTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersDoubleTest::SetUpTestCase(void)
{}

void IntentParametersDoubleTest::TearDownTestCase(void)
{}

void IntentParametersDoubleTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersDoubleTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetDoubleParam/GetDoubleParam
 * SubFunction: NA
 * FunctionPoints: SetDoubleParam/GetDoubleParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersDoubleTest, AaFwk_Intent_Parameters_Double, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    double setValue = std::get<2>(GetParam());
    double defaultValue = std::get<3>(GetParam());
    double result = std::get<4>(GetParam());
    intent_->SetDoubleParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetDoubleParam(getKey, defaultValue));
}

INSTANTIATE_TEST_CASE_P(IntentParametersDoubleTestCaseP, IntentParametersDoubleTest,
    testing::Values(testDoubleType("", "aa", -1.1, 100.1, 100.1), testDoubleType("", "", -9.1, -41.1, -9.1),
        testDoubleType("1*中_aR", "aa", 50.1, 5.1, 5.1),
        testDoubleType("1*中_aR", "1*中_aR", -5000.1, 5000.1, -5000.1)));

/*
 * Feature: Intent
 * Function: SetDoubleParam & GetDoubleParam
 * SubFunction: NA
 * FunctionPoints: SetDoubleParam & GetDoubleParam
 * EnvConditions: NA
 * CaseDescription: get param when IntentParam is empty
 */
HWTEST_F(IntentParametersDoubleTest, AaFwk_Intent_Parameters_Double_001, TestSize.Level1)
{
    double defaultValue = 100.1;
    std::string key = "aa";
    EXPECT_EQ(defaultValue, intent_->GetDoubleParam(key, defaultValue));
}

/*
 * Feature: Intent
 * Function: SetDoubleParam & GetDoubleParam
 * SubFunction: NA
 * FunctionPoints: SetDoubleParam & GetDoubleParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, but get param of another nonexistent key
 */
HWTEST_F(IntentParametersDoubleTest, AaFwk_Intent_Parameters_Double_002, TestSize.Level1)
{
    std::string setKey1 = "";
    std::string setKey2 = "aa";
    double setValue1 = 1.1;
    double setValue2 = 5.1;
    intent_->SetDoubleParam(setKey1, setValue1);
    intent_->SetDoubleParam(setKey1, setValue1);
    setValue1 = 2.1;
    intent_->SetDoubleParam(setKey1, setValue1);
    setValue1 = 3.1;
    EXPECT_EQ(setValue1, intent_->GetDoubleParam(setKey2, setValue1));
    setValue1 = 4.1;
    intent_->SetDoubleParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetDoubleParam(setKey1, setValue2));
}

/*
 * Feature: Intent
 * Function: SetDoubleParam & GetDoubleParam
 * SubFunction: NA
 * FunctionPoints: SetDoubleParam & GetDoubleParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, then get param of the key
 */
HWTEST_F(IntentParametersDoubleTest, AaFwk_Intent_Parameters_Double_003, TestSize.Level1)
{
    std::string setKey1 = "%1uH3";
    std::string setKey2 = "aa";
    double setValue1 = -1.1;
    double setValue2 = 9.1;
    intent_->SetDoubleParam(setKey1, setValue1);
    intent_->SetDoubleParam(setKey1, setValue1);
    setValue1 = 0.1;
    intent_->SetDoubleParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetDoubleParam(setKey1, setValue2));
    setValue1 = 4.1;
    intent_->SetDoubleParam(setKey1, setValue1);
    setValue1 = -10.1;
    EXPECT_EQ(setValue1, intent_->GetDoubleParam(setKey2, setValue1));
}

using testDoubleArrayType =
    std::tuple<std::string, std::string, std::vector<double>, std::vector<double>, std::vector<double>>;
class IntentParametersDoubleArrayTest : public testing::TestWithParam<testDoubleArrayType> {
public:
    IntentParametersDoubleArrayTest() : intent_(nullptr)
    {}
    ~IntentParametersDoubleArrayTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersDoubleArrayTest::SetUpTestCase(void)
{}

void IntentParametersDoubleArrayTest::TearDownTestCase(void)
{}

void IntentParametersDoubleArrayTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersDoubleArrayTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetDoubleArrayParam/GetDoubleArrayParam
 * SubFunction: NA
 * FunctionPoints: SetDoubleArrayParam/GetDoubleArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersDoubleArrayTest, AaFwk_Intent_Parameters_DoubleArray, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<double> setValue = std::get<2>(GetParam());
    std::vector<double> defaultValue = std::get<3>(GetParam());
    std::vector<double> result = std::get<4>(GetParam());
    intent_->SetDoubleArrayParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetDoubleArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(IntentParametersDoubleArrayTestCaseP, IntentParametersDoubleArrayTest,
    testing::Values(testDoubleArrayType("", "aa", {-1.1, -2.1}, {}, {}),
        testDoubleArrayType("", "", {-41.1, -42.1}, {}, {-41.1, -42.1}),
        testDoubleArrayType("1*中_aR", "aa", {50.1, 51.1}, {}, {}),
        testDoubleArrayType("1*中_aR", "1*中_aR", {5000.1, 5001.1}, {}, {5000.1, 5001.1})));

/*
 * Feature: Intent
 * Function: SetDoubleArrayParam & GetDoubleArrayParam
 * SubFunction: NA
 * FunctionPoints: SetDoubleArrayParam & GetDoubleArrayParam
 * EnvConditions: NA
 * CaseDescription: get param when IntentParam is empty
 */
HWTEST_F(IntentParametersDoubleArrayTest, AaFwk_Intent_Parameters_Double_Array_001, TestSize.Level1)
{
    std::vector<double> defaultValue;
    std::string key = "aa";
    EXPECT_EQ(defaultValue, intent_->GetDoubleArrayParam(key));
}

/*
 * Feature: Intent
 * Function: SetDoubleArrayParam & GetDoubleArrayParam
 * SubFunction: NA
 * FunctionPoints: SetDoubleArrayParam & GetDoubleArrayParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, but get param of another nonexistent key
 */
HWTEST_F(IntentParametersDoubleArrayTest, AaFwk_Intent_Parameters_Double_Array_002, TestSize.Level1)
{
    std::vector<double> defaultValue;
    std::string setKey1 = "";
    std::string setKey2 = "aa";
    std::vector<double> setValue1 = {1.1, 2.1};
    std::vector<double> setValue2 = {5.1, 6.1};
    intent_->SetDoubleArrayParam(setKey1, setValue1);
    intent_->SetDoubleArrayParam(setKey1, setValue1);
    setValue1 = {2.1, 3.1};
    intent_->SetDoubleArrayParam(setKey1, setValue1);
    EXPECT_EQ(defaultValue, intent_->GetDoubleArrayParam(setKey2));
    setValue1 = {4.1, 5.1};
    intent_->SetDoubleArrayParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetDoubleArrayParam(setKey1));
}

/*
 * Feature: Intent
 * Function: SetDoubleArrayParam & GetDoubleArrayParam
 * SubFunction: NA
 * FunctionPoints: SetDoubleArrayParam & GetDoubleArrayParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, then get param of the key
 */
HWTEST_F(IntentParametersDoubleArrayTest, AaFwk_Intent_Parameters_Double_Array_003, TestSize.Level1)
{
    std::vector<double> defaultValue;
    std::string setKey1 = "%1uH3";
    std::string setKey2 = "aa";
    std::vector<double> setValue1 = {-1.1, -2.1};
    intent_->SetDoubleArrayParam(setKey1, setValue1);
    intent_->SetDoubleArrayParam(setKey1, setValue1);
    setValue1 = {0.1, 1.1};
    intent_->SetDoubleArrayParam(setKey1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetDoubleArrayParam(setKey1));
    setValue1 = {4.1, 5.1};
    intent_->SetDoubleArrayParam(setKey1, setValue1);
    setValue1 = {-10.1, -11.1};
    EXPECT_EQ(defaultValue, intent_->GetDoubleArrayParam(setKey2));
}

using testStrType = std::tuple<std::string, std::string, std::string, std::string, std::string>;
class IntentParametersStringTest : public testing::TestWithParam<testStrType> {
public:
    IntentParametersStringTest() : intent_(nullptr)
    {}
    ~IntentParametersStringTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersStringTest::SetUpTestCase(void)
{}

void IntentParametersStringTest::TearDownTestCase(void)
{}

void IntentParametersStringTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersStringTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetStringParam/GetStringParam
 * SubFunction: NA
 * FunctionPoints: SetStringParam/GetStringParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersStringTest, AaFwk_Intent_Parameters_String, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::string setValue = std::get<2>(GetParam());
    std::string defaultValue = std::get<3>(GetParam());
    std::string result = std::get<4>(GetParam());
    intent_->SetStringParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetStringParam(getKey));
}

INSTANTIATE_TEST_CASE_P(IntentParametersStringTestCaseP, IntentParametersStringTest,
    testing::Values(testStrType("", "aa", "1*中_aR", "", ""), testStrType("", "", "1*中_aR", "", "1*中_aR"),
        testStrType("1*中_aR", "aa", "aaa", "", ""), testStrType("1*中_aR", "1*中_aR", "aaa", "", "aaa")));

/*
 * Feature: Intent
 * Function: SetStringParam & GetStringParam
 * SubFunction: NA
 * FunctionPoints: SetStringParam & GetStringParam
 * EnvConditions: NA
 * CaseDescription: get param when IntentParam is empty
 */
HWTEST_F(IntentParametersStringTest, AaFwk_Intent_Parameters_String_001, TestSize.Level1)
{
    std::string defaultStrValue;
    std::string key = "aa";
    EXPECT_EQ(defaultStrValue, intent_->GetStringParam(key));
}

/*
 * Feature: Intent
 * Function: SetStringParam & GetStringParam
 * SubFunction: NA
 * FunctionPoints: SetStringParam & GetStringParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, but get param of another nonexistent key
 */
HWTEST_F(IntentParametersStringTest, AaFwk_Intent_Parameters_String_002, TestSize.Level1)
{
    std::string defaultStrValue;
    std::string setValue1 = "aaa";
    std::string setValue2 = "1*中_aR";
    std::string key1 = "";
    std::string key2 = "aa";
    intent_->SetStringParam(key1, setValue1);
    intent_->SetStringParam(key1, setValue1);
    intent_->SetStringParam(key1, setValue2);
    EXPECT_EQ(defaultStrValue, intent_->GetStringParam(key2));
    intent_->SetStringParam(key1, setValue1);
    EXPECT_EQ(setValue1, intent_->GetStringParam(key1));
}

/*
 * Feature: Intent
 * Function: SetStringParam & GetStringParam
 * SubFunction: NA
 * FunctionPoints: SetStringParam & GetStringParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, then get param of the key
 */
HWTEST_F(IntentParametersStringTest, AaFwk_Intent_Parameters_String_003, TestSize.Level1)
{
    std::string key1 = "%1uH3";
    std::string defaultStrValue;
    std::string setValue1 = "aaa";
    std::string setValue2 = "1*中_aR";
    std::string key2 = "aa";
    intent_->SetStringParam(key1, setValue1);
    intent_->SetStringParam(key1, setValue1);
    intent_->SetStringParam(key1, setValue2);
    EXPECT_EQ("1*中_aR", intent_->GetStringParam(key1));
    intent_->SetStringParam(key1, setValue1);
    EXPECT_EQ(defaultStrValue, intent_->GetStringParam(key2));
}

using testStrArrayType =
    std::tuple<std::string, std::string, std::vector<std::string>, std::vector<std::string>, std::vector<std::string>>;
class IntentParametersStringArrayTest : public testing::TestWithParam<testStrArrayType> {
public:
    IntentParametersStringArrayTest() : intent_(nullptr)
    {}
    ~IntentParametersStringArrayTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersStringArrayTest::SetUpTestCase(void)
{}

void IntentParametersStringArrayTest::TearDownTestCase(void)
{}

void IntentParametersStringArrayTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersStringArrayTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: SetStringArrayParam/GetStringArrayParam
 * SubFunction: NA
 * FunctionPoints: SetStringArrayParam/GetStringArrayParam
 * EnvConditions: NA
 * CaseDescription: Verify when parameter change.
 */
HWTEST_P(IntentParametersStringArrayTest, AaFwk_Intent_Parameters_StringArray, TestSize.Level1)
{
    std::string setKey = std::get<0>(GetParam());
    std::string getKey = std::get<1>(GetParam());
    std::vector<std::string> setValue = std::get<2>(GetParam());
    std::vector<std::string> defaultValue = std::get<3>(GetParam());
    std::vector<std::string> result = std::get<4>(GetParam());
    intent_->SetStringArrayParam(setKey, setValue);
    EXPECT_EQ(result, intent_->GetStringArrayParam(getKey));
}

INSTANTIATE_TEST_CASE_P(IntentParametersStringArrayTestCaseP, IntentParametersStringArrayTest,
    testing::Values(testStrArrayType("", "aa", {"1*中_aR", "dadb"}, {}, {}),
        testStrArrayType("", "", {"1*中_aR", "dadb"}, {}, {"1*中_aR", "dadb"}),
        testStrArrayType("1*中_aR", "aa", {"1*中_aR", "dadb"}, {}, {}),
        testStrArrayType("1*中_aR", "1*中_aR", {"1*中_aR", "dadb"}, {}, {"1*中_aR", "dadb"})));

/*
 * Feature: Intent
 * Function: SetStringArrayParam & GetStringArrayParam
 * SubFunction: NA
 * FunctionPoints: SetStringArrayParam & GetStringArrayParam
 * EnvConditions: NA
 * CaseDescription: get param when IntentParam is empty
 */
HWTEST_F(IntentParametersStringArrayTest, AaFwk_Intent_Parameters_String_Array_001, TestSize.Level1)
{
    std::vector<std::string> defaultValue;
    std::string key = "aa";
    std::vector<std::string> resultValue = intent_->GetStringArrayParam(key);
    EXPECT_EQ(defaultValue, resultValue);
}

/*
 * Feature: Intent
 * Function: SetStringArrayParam & GetStringArrayParam
 * SubFunction: NA
 * FunctionPoints: SetStringArrayParam & GetStringArrayParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, but get param of another nonexistent key
 */
HWTEST_F(IntentParametersStringArrayTest, AaFwk_Intent_Parameters_String_Array_002, TestSize.Level1)
{
    std::vector<std::string> defaultValue;
    std::vector<std::string> setValue1 = {"aaa", "2132"};
    std::vector<std::string> setValue2 = {"1*中_aR", "dadb"};
    std::string key1 = "";
    std::string key2 = "aa";
    intent_->SetStringArrayParam(key1, setValue1);
    intent_->SetStringArrayParam(key1, setValue1);
    intent_->SetStringArrayParam(key1, setValue2);
    std::vector<std::string> resultValue = intent_->GetStringArrayParam(key2);
    EXPECT_EQ(defaultValue, resultValue);

    intent_->SetStringArrayParam(key1, setValue1);
    resultValue = intent_->GetStringArrayParam(key1);
    EXPECT_EQ(setValue1, resultValue);
}

/*
 * Feature: Intent
 * Function: SetStringArrayParam & GetStringArrayParam
 * SubFunction: NA
 * FunctionPoints: SetStringArrayParam & GetStringArrayParam
 * EnvConditions: NA
 * CaseDescription: set empty-string key repeatedly, then get param of the key
 */
HWTEST_F(IntentParametersStringArrayTest, AaFwk_Intent_Parameters_String_Array_003, TestSize.Level1)
{
    std::vector<std::string> defaultValue;
    std::vector<std::string> setValue = {"aaa", "2132"};
    std::string key1 = "%1uH3";
    std::string key2 = "aa";
    intent_->SetStringArrayParam(key1, setValue);
    intent_->SetStringArrayParam(key1, setValue);
    setValue = {"1*中_aR", "3#$%"};
    intent_->SetStringArrayParam(key1, setValue);
    std::vector<std::string> resultValue = intent_->GetStringArrayParam(key1);
    EXPECT_EQ(setValue, resultValue);

    setValue = {"aaa", "2132"};
    intent_->SetStringArrayParam(key1, setValue);
    resultValue = intent_->GetStringArrayParam(key2);
    EXPECT_EQ(defaultValue, resultValue);
}

class IntentParametersHasParamTest : public testing::Test {
public:
    IntentParametersHasParamTest() : intent_(nullptr)
    {}
    ~IntentParametersHasParamTest()
    {
        intent_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Intent *intent_;
};

void IntentParametersHasParamTest::SetUpTestCase(void)
{}

void IntentParametersHasParamTest::TearDownTestCase(void)
{}

void IntentParametersHasParamTest::SetUp(void)
{
    intent_ = new (std::nothrow) Intent();
}

void IntentParametersHasParamTest::TearDown(void)
{
    delete intent_;
    intent_ = nullptr;
}

/*
 * Feature: Intent
 * Function: HasParameter
 * SubFunction: NA
 * FunctionPoints: HasParameter
 * EnvConditions: NA
 * CaseDescription: verify if parameter exists
 */
HWTEST_F(IntentParametersHasParamTest, AaFwk_Intent_Parameters_Has_Parameter_001, TestSize.Level1)
{
    std::string keyBool = "keyBool";
    std::string keyChar = "keyChar";
    std::string keyShort = "keyShort";
    std::string keyInt = "keyInt";
    std::string keyLong = "keyLong";
    std::string keyFloat = "keyFloat";
    std::string keyDouble = "keyDouble";
    std::string keyString = "keyString";
    std::string keyBoolArray = "keyBoolArray";
    std::string keyCharArray = "keyCharArray";
    std::string keyShortArray = "keyShortArray";
    std::string keyIntArray = "keyIntArray";
    std::string keyLongArray = "keyLongArray";
    std::string keyFloatArray = "keyFloatArray";
    std::string keyDoubleArray = "keyDoubleArray";
    std::string keyStringArray = "keyStringArray";

    std::string keyBoolNotExist = "keyBoolNotExist";
    std::string keyCharNotExist = "keyCharNotExist";
    std::string keyShortNotExist = "keyShortNotExist";
    std::string keyIntNotExist = "keyIntNotExist";
    std::string keyLongNotExist = "keyLongNotExist";
    std::string keyFloatNotExist = "keyFloatNotExist";
    std::string keyDoubleNotExist = "keyDoubleNotExist";
    std::string keyStringNotExist = "keyStringNotExist";
    std::string keyBoolArrayNotExist = "keyBoolArrayNotExist";
    std::string keyCharArrayNotExist = "keyCharArrayNotExist";
    std::string keyShortArrayNotExist = "keyShortArrayNotExist";
    std::string keyIntArrayNotExist = "keyIntArrayNotExist";
    std::string keyLongArrayNotExist = "keyLongArrayNotExist";
    std::string keyFloatArrayNotExist = "keyFloatArrayNotExist";
    std::string keyDoubleArrayNotExist = "keyDoubleArrayNotExist";
    std::string keyStringArrayNotExist = "keyStringArrayNotExist";

    bool valueBool = false;
    zchar valueChar = 1;
    short valueShort = 2;
    int valueInt = 3;
    long valueLong = 4;
    float valueFloat = 5.1;
    double valueDouble = 6.2;
    std::string valueString = "1*中_aRabc";
    std::vector<bool> valueBoolArray = {true, false};
    std::vector<zchar> valueCharArray = {1, 2};
    std::vector<short> valueShortArray = {3, 4};
    std::vector<int> valueIntArray = {5, 6};
    std::vector<long> valueLongArray = {7, 8};
    std::vector<float> valueFloatArray = {9.1, 10.2};
    std::vector<double> valueDoubleArray = {11.3, 12.4};
    std::vector<std::string> valueStringArray = {"1*中_aRabc", "1*中_aRpinyin"};

    intent_->SetBoolParam(keyBool, valueBool);
    intent_->SetCharParam(keyChar, valueChar);
    intent_->SetShortParam(keyShort, valueShort);
    intent_->SetIntParam(keyInt, valueInt);
    intent_->SetLongParam(keyLong, valueLong);
    intent_->SetFloatParam(keyFloat, valueFloat);
    intent_->SetDoubleParam(keyDouble, valueDouble);
    intent_->SetStringParam(keyString, valueString);

    intent_->SetBoolArrayParam(keyBoolArray, valueBoolArray);
    intent_->SetCharArrayParam(keyCharArray, valueCharArray);
    intent_->SetShortArrayParam(keyShortArray, valueShortArray);
    intent_->SetIntArrayParam(keyIntArray, valueIntArray);
    intent_->SetLongArrayParam(keyLongArray, valueLongArray);
    intent_->SetFloatArrayParam(keyFloatArray, valueFloatArray);
    intent_->SetDoubleArrayParam(keyDoubleArray, valueDoubleArray);
    intent_->SetStringArrayParam(keyStringArray, valueStringArray);

    EXPECT_EQ(true, intent_->HasParameter(keyBool));
    EXPECT_EQ(true, intent_->HasParameter(keyChar));
    EXPECT_EQ(true, intent_->HasParameter(keyShort));
    EXPECT_EQ(true, intent_->HasParameter(keyInt));
    EXPECT_EQ(true, intent_->HasParameter(keyLong));
    EXPECT_EQ(true, intent_->HasParameter(keyFloat));
    EXPECT_EQ(true, intent_->HasParameter(keyDouble));
    EXPECT_EQ(true, intent_->HasParameter(keyString));
    EXPECT_EQ(true, intent_->HasParameter(keyBoolArray));
    EXPECT_EQ(true, intent_->HasParameter(keyCharArray));
    EXPECT_EQ(true, intent_->HasParameter(keyShortArray));
    EXPECT_EQ(true, intent_->HasParameter(keyIntArray));
    EXPECT_EQ(true, intent_->HasParameter(keyLongArray));
    EXPECT_EQ(true, intent_->HasParameter(keyFloatArray));
    EXPECT_EQ(true, intent_->HasParameter(keyDoubleArray));
    EXPECT_EQ(true, intent_->HasParameter(keyStringArray));

    EXPECT_EQ(false, intent_->HasParameter(keyBoolNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyCharNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyShortNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyIntNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyLongNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyFloatNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyDoubleNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyStringNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyBoolArrayNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyCharArrayNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyShortArrayNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyIntArrayNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyLongArrayNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyFloatArrayNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyDoubleArrayNotExist));
    EXPECT_EQ(false, intent_->HasParameter(keyStringArrayNotExist));
}
