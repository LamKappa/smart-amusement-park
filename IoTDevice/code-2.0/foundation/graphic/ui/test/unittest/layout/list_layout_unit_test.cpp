/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "layout/list_layout.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class ListLayoutTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static ListLayout* listLayout_;
};

ListLayout* ListLayoutTest::listLayout_ = nullptr;

void ListLayoutTest::SetUpTestCase(void)
{
    if (listLayout_ == nullptr) {
        listLayout_ = new ListLayout();
    }
}

void ListLayoutTest::TearDownTestCase(void)
{
    if (listLayout_ != nullptr) {
        delete listLayout_;
        listLayout_ = nullptr;
    }
}
/**
 * @tc.name: ListLayoutSetDirection_001
 * @tc.desc: Verify SetDirection function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(ListLayoutTest, ListLayoutSetDirection_001, TestSize.Level0)
{
    if (listLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(listLayout_->GetDirection(), 1);

    listLayout_->SetDirection(0);
    EXPECT_EQ(listLayout_->GetDirection(), 0);
}
} // namespace OHOS