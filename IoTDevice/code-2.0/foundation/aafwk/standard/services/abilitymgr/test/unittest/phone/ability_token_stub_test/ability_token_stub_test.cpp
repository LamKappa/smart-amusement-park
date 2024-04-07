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
#include "ability_token_stub.h"

using namespace testing::ext;
namespace OHOS {
namespace AAFwk {
class AbilityTokenStubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    OHOS::sptr<AbilityTokenStub> abilityTokenStub_;
    OHOS::sptr<AbilityTokenRecipient> abilityTokenRecipient_;
};

void AbilityTokenStubTest::SetUpTestCase(void)
{}
void AbilityTokenStubTest::TearDownTestCase(void)
{}
void AbilityTokenStubTest::TearDown(void)
{}

void AbilityTokenStubTest::SetUp(void)
{
    abilityTokenStub_ = new AbilityTokenStub();
    abilityTokenRecipient_ = new AbilityTokenRecipient();
}

/*
 * Feature: AbilityTokenStub
 * Function: AbilityTokenStub
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify AbilityTokenStub is create success
 */
HWTEST_F(AbilityTokenStubTest, ability_token_stub_operating_001, TestSize.Level0)
{
    EXPECT_NE(abilityTokenStub_, nullptr);
}

/*
 * Feature: AbilityTokenStub
 * Function: AbilityTokenRecipient
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify AbilityTokenRecipient is create success
 */
HWTEST_F(AbilityTokenStubTest, ability_token_stub_operating_002, TestSize.Level0)
{
    EXPECT_NE(abilityTokenRecipient_, nullptr);
}
}  // namespace AAFwk
}  // namespace OHOS