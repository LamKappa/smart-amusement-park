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

#include "common/text.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class TextTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static Text* text_;
};

Text* TextTest::text_ = nullptr;

void TextTest::SetUpTestCase(void)
{
    if (text_ == nullptr) {
        text_ = new Text();
    }
}

void TextTest::TearDownTestCase(void)
{
    if (text_ != nullptr) {
        delete text_;
        text_ = nullptr;
    }
}

/**
 * @tc.name: TextSetText_001
 * @tc.desc: Verify SetText function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(TextTest, TextSetText_001, TestSize.Level0)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const char* text = "unit test text";
    text_->SetText(text);
    EXPECT_EQ(strcmp(text_->GetText(), text), 0);
}

/**
 * @tc.name: TextSetDirect_001
 * @tc.desc: Verify SetDirect function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(TextTest, TextSetDirect_001, TestSize.Level0)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UITextLanguageDirect direct = UITextLanguageDirect::TEXT_DIRECT_LTR;
    text_->SetDirect(direct);
    EXPECT_EQ(text_->GetDirect(), direct);
    direct = UITextLanguageDirect::TEXT_DIRECT_RTL;
    text_->SetDirect(direct);
    EXPECT_EQ(text_->GetDirect(), direct);
}

/**
 * @tc.name: TextSetAlign_001
 * @tc.desc: Verify SetAlign function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(TextTest, TextSetAlign_001, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    text_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
    EXPECT_EQ(text_->IsNeedRefresh(), true);
    EXPECT_EQ(text_->GetHorAlign(), TEXT_ALIGNMENT_LEFT);
    EXPECT_EQ(text_->GetVerAlign(), TEXT_ALIGNMENT_TOP);
}

/**
 * @tc.name: TextSetExpand_001
 * @tc.desc: Verify SetExpand function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(TextTest, TextSetExpand_001, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(text_->IsExpandWidth(), false);
    text_->SetExpandWidth(true);
    EXPECT_EQ(text_->IsExpandWidth(), true);

    EXPECT_EQ(text_->IsExpandHeight(), false);
    text_->SetExpandHeight(true);
    EXPECT_EQ(text_->IsExpandHeight(), true);
}
} // namespace OHOS