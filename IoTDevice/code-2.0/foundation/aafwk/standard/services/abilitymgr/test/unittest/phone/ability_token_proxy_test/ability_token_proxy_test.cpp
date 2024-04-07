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
#include "ability_token_proxy.h"

using namespace testing::ext;
namespace OHOS {
namespace AAFwk {
class AbilityTokenProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    OHOS::sptr<AbilityTokenProxy> abilityTokenProxy_;
    OHOS::sptr<OHOS::IRemoteObject> impl = nullptr;
};

void AbilityTokenProxyTest::SetUpTestCase(void)
{}
void AbilityTokenProxyTest::TearDownTestCase(void)
{}
void AbilityTokenProxyTest::TearDown(void)
{}

void AbilityTokenProxyTest::SetUp(void)
{
    abilityTokenProxy_ = new AbilityTokenProxy(impl);
}

/*
 * Feature: AbilityTokenProxy
 * Function: AbilityTokenProxy
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify AbilityTokenProxy is create success
 */
HWTEST_F(AbilityTokenProxyTest, ability_token_proxy_operating_001, TestSize.Level0)
{
    EXPECT_NE(abilityTokenProxy_, nullptr);
}
}  // namespace AAFwk
}  // namespace OHOS