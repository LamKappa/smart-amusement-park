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
#include "window_info.h"

using namespace testing::ext;
namespace OHOS {
namespace AAFwk {
class WindowInfoTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<WindowInfo> get() const;

private:
    std::shared_ptr<WindowInfo> windowInfo_;
};

void WindowInfoTest::SetUpTestCase(void)
{}
void WindowInfoTest::TearDownTestCase(void)
{}
void WindowInfoTest::TearDown()
{}

void WindowInfoTest::SetUp()
{
    windowInfo_ = std::make_shared<WindowInfo>(1);
}

std::shared_ptr<WindowInfo> WindowInfoTest::get() const
{
    return windowInfo_;
}

/*
 * Feature: WindowInfo
 * Function: Constructors and Destructor
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: Verify member variable values
 */
HWTEST_F(WindowInfoTest, AAFWK_Window_Info, TestSize.Level0)
{
    EXPECT_EQ((int)get()->windowToken_, 1);
    EXPECT_FALSE(get()->isVisible_);
}
}  // namespace AAFwk
}  // namespace OHOS