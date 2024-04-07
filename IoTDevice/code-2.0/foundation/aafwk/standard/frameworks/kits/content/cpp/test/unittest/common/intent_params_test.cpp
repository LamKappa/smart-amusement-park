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

#include "ohos/aafwk/base/string_wrapper.h"
#include "ohos/aafwk/content/intent_params.h"

using namespace testing::ext;
using namespace OHOS::AAFwk;

static const int LARGE_STR_LEN = 65534;

using testParamsType = std::tuple<std::string, std::string>;
class IntentParamsTest : public testing::TestWithParam<testParamsType> {
public:
    IntentParamsTest() : intentParam_(nullptr)
    {}
    ~IntentParamsTest()
    {
        intentParam_ = nullptr;
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    IntentParams *intentParam_;
};

void IntentParamsTest::SetUpTestCase(void)
{}

void IntentParamsTest::TearDownTestCase(void)
{}

void IntentParamsTest::SetUp(void)
{
    intentParam_ = new (std::nothrow) IntentParams();
}

void IntentParamsTest::TearDown(void)
{
    delete intentParam_;
    intentParam_ = nullptr;
}

/*
 * Feature: IntentParams
 * Function: SetParam
 * SubFunction: NA
 * FunctionPoints: SetParam
 * EnvConditions: NA
 * CaseDescription: Verify whether parameter change.
 */

HWTEST_P(IntentParamsTest, AaFwk_IntentParams_Params, TestSize.Level1)
{
    std::string keyStr = std::get<0>(GetParam());
    std::string valueStr = std::get<1>(GetParam());
    intentParam_->SetParam(keyStr, String::Box(valueStr));
    EXPECT_EQ(valueStr, Object::ToString(intentParam_->GetParam(keyStr)));
}

INSTANTIATE_TEST_CASE_P(IntentParamsTestCaseP, IntentParamsTest,
    testing::Values(testParamsType("", "asdsdsdasa"), testParamsType(std::string(LARGE_STR_LEN + 1, 's'), "sadsdsdads"),
        testParamsType("#$%^&*(!@\":<>{}", "asdsdsdasa"), testParamsType("3456677", ""),
        testParamsType("1234", std::string(LARGE_STR_LEN + 1, 's')),
        testParamsType("2323sdasdZ", "#$%^&*(!@\":<>{}sadsdasdsaf"), testParamsType("12345667", "sdasdfdsffdgfdg"),
        testParamsType("", ""),
        testParamsType(std::string(LARGE_STR_LEN + 1, 'k'), std::string(LARGE_STR_LEN + 1, 'k')),
        testParamsType("#$%^&*(!@\":<>{},/", "#$%^&*(!@\":<>{},/")));

/*
 * Feature: IntentParams
 * Function: SetParam
 * SubFunction: NA
 * FunctionPoints: SetParam
 * EnvConditions: NA
 * CaseDescription: Verify whether the insertion can be overwritten.
 */
HWTEST_F(IntentParamsTest, AaFwk_IntentParams_Params_001, TestSize.Level1)
{
    std::string keyStr("123456");
    std::string firstValueStr("abcdse");
    intentParam_->SetParam(keyStr, String::Box(firstValueStr));
    std::string secondValueStr("dffghghhg");
    intentParam_->SetParam(keyStr, String::Box(secondValueStr));
    EXPECT_EQ(secondValueStr, Object::ToString(intentParam_->GetParam(keyStr)));
}

/*
 * Feature: IntentParams
 * Function: SetParam
 * SubFunction: NA
 * FunctionPoints: SetParam
 * EnvConditions: NA
 * CaseDescription: Verify whether the GetParams can get empty params_.
 */
HWTEST_F(IntentParamsTest, AaFwk_IntentParams_Params_002, TestSize.Level1)
{
    auto resultMap = intentParam_->GetParams();
    EXPECT_EQ(0UL, resultMap.size());
}

/*
 * Feature: IntentParams
 * Function: SetParam
 * SubFunction: NA
 * FunctionPoints: SetParam
 * EnvConditions: NA
 * CaseDescription: Verify whether the GetParams can get normal params_.
 */
HWTEST_F(IntentParamsTest, AaFwk_IntentParams_Params_003, TestSize.Level1)
{
    std::string firstKeyStr("123456");
    std::string firstValueStr("abcdse");
    intentParam_->SetParam(firstKeyStr, String::Box(firstValueStr));
    std::string secondKeyStr("@#￥#￥中文%");
    std::string secondValueStr("*%￥#_中文");
    intentParam_->SetParam(secondKeyStr, String::Box(secondValueStr));

    auto resultMap = intentParam_->GetParams();
    EXPECT_EQ(2UL, resultMap.size());

    auto firstIt = resultMap.find(firstKeyStr);
    EXPECT_NE(firstIt, resultMap.end());
    EXPECT_EQ(firstValueStr, Object::ToString(firstIt->second));

    auto secondIt = resultMap.find(secondKeyStr);
    EXPECT_NE(secondIt, resultMap.end());
    EXPECT_EQ(secondValueStr, Object::ToString(secondIt->second));
}

/*
 * Feature: IntentParams
 * Function: HasParam
 * SubFunction: NA
 * FunctionPoints: HasParam
 * EnvConditions: NA
 * CaseDescription: Verify if param exist.
 */
HWTEST_F(IntentParamsTest, AaFwk_IntentParams_Params_004, TestSize.Level1)
{
    std::string keyStr("abc");
    std::string keyStrNotExist("def");
    std::string valueStr("ghi");
    intentParam_->SetParam(keyStr, String::Box(valueStr));

    EXPECT_EQ(true, intentParam_->HasParam(keyStr));
    EXPECT_EQ(false, intentParam_->HasParam(keyStrNotExist));
}
