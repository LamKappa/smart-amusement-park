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

#include "intl_test.h"
#include <gtest/gtest.h>
#include "date_time_format.h"
#include "locale_info.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace {
class IntlTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void IntlTest::SetUpTestCase(void)
{}

void IntlTest::TearDownTestCase(void)
{}

void IntlTest::SetUp(void)
{}

void IntlTest::TearDown(void)
{}

/**
 * @tc.name: IntlFuncTest001
 * @tc.desc: Test Intl DateTimeFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest,  IntlFuncTest001, TestSize.Level1)
{
    string locale = "en-GB";
    string expects = "14 Apr 2021";
    DateTimeFormat *dateFormat = new (std::nothrow) DateTimeFormat(locale);
    if (dateFormat == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    string out = dateFormat->Format(2021, 3, 14, 0, 0, 0);
    EXPECT_EQ(out, expects);
    delete dateFormat;
}

/**
 * @tc.name: IntlFuncTest002
 * @tc.desc: Test Intl LocaleInfo
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest,  IntlFuncTest002, TestSize.Level1)
{
    string locale = "en-GB";
    LocaleInfo *loc = new (std::nothrow) LocaleInfo(locale);
    if (loc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    string language = "en";
    string script = "";
    string region = "GB";
    string baseName = "en-GB";
    EXPECT_EQ(loc->GetLanguage(), language);
    EXPECT_EQ(loc->GetScript(), script);
    EXPECT_EQ(loc->GetRegion(), region);
    EXPECT_EQ(loc->GetBaseName(), baseName);
    delete loc;
}
}
