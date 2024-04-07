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

using namespace testing::ext;
using namespace OHOS;
using namespace AAFwk;
using OHOS::AppExecFwk::ElementName;

static const int LARGE_STR_LEN = 65534;
static const int SET_COUNT = 20;
static const int LOOP_TEST = 1000;

class IntentBaseTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

public:
    Intent *intentInst = nullptr;
};

void IntentBaseTest::SetUpTestCase(void)
{}

void IntentBaseTest::TearDownTestCase(void)
{}

void IntentBaseTest::SetUp(void)
{
    intentInst = new (std::nothrow) Intent();
}

void IntentBaseTest::TearDown(void)
{
    delete (intentInst);
    intentInst = nullptr;
}

/*
 * Feature: Intent
 * Function: SetElement & GetElement
 * SubFunction: NA
 * FunctionPoints: SetElement & GetElement
 * EnvConditions:NA
 * CaseDescription: Verify SetElement & GetElement
 */
HWTEST_F(IntentBaseTest, AaFwk_Intent_Element_013, TestSize.Level1)
{
    ElementName emptyElement;
    std::string bundleName = "bundle";
    std::string abilityName = "ability";
    std::string sliceName = "slice";

    EXPECT_EQ(emptyElement, intentInst->GetElement());

    ElementName element(bundleName, abilityName, sliceName);
    intentInst->SetElement(element);

    ElementName newElement = intentInst->GetElement();
    EXPECT_EQ(element, newElement);
}

/*
 * Feature: Intent
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string is empty
 */
HWTEST_F(IntentBaseTest, AaFwk_Intent_Entity_001, TestSize.Level1)
{
    std::string setValue;
    intentInst->SetEntity(setValue);
    EXPECT_EQ(setValue, intentInst->GetEntity());
}

/*
 * Feature: Intent
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string contains special characters
 */
HWTEST_F(IntentBaseTest, AaFwk_Intent_Entity_002, TestSize.Level1)
{
    std::string setValue("@#￥#3243adsafdf_中文");
    intentInst->SetEntity(setValue);
    EXPECT_EQ(setValue, intentInst->GetEntity());
}

/*
 * Feature: Intent
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string has a long size
 */
HWTEST_F(IntentBaseTest, AaFwk_Intent_Entity_003, TestSize.Level1)
{
    std::string setValue(LARGE_STR_LEN, 's');
    intentInst->SetEntity(setValue);
    EXPECT_EQ(setValue, intentInst->GetEntity());
}

/*
 * Feature: Intent
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string is overrode
 */
HWTEST_F(IntentBaseTest, AaFwk_Intent_Entity_004, TestSize.Level1)
{
    std::string setValue1("1234");
    intentInst->SetEntity(setValue1);

    std::string setValue2("abcd");
    intentInst->SetEntity(setValue2);

    EXPECT_EQ(setValue2, intentInst->GetEntity());
}

/*
 * Feature: Intent
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string is set 20 times
 */
HWTEST_F(IntentBaseTest, AaFwk_Intent_Entity_005, TestSize.Level1)
{
    std::string setValue("1234");
    for (int i = 0; i < SET_COUNT; i++) {
        intentInst->SetEntity(setValue);
    }
    EXPECT_EQ(setValue, intentInst->GetEntity());
}

/*
 * Feature: Intent
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string is default
 */
HWTEST_F(IntentBaseTest, AaFwk_Intent_Entity_006, TestSize.Level1)
{
    std::string setValue;
    EXPECT_EQ(setValue, intentInst->GetEntity());
}

/*
 * Feature: Intent
 * Function: SetEntity/GetEntity
 * SubFunction: NA
 * FunctionPoints: SetEntity/GetEntity
 * EnvConditions: NA
 * CaseDescription: Verify the function when the input string contains special characters
 */
HWTEST_F(IntentBaseTest, AaFwk_Intent_Entity_007, TestSize.Level1)
{
    std::string setValue("@#￥#3243adsafdf_中文");
    for (int i = 0; i < LOOP_TEST; i++) {
        intentInst->SetEntity(setValue);
        EXPECT_EQ(setValue, intentInst->GetEntity());
    }
}
