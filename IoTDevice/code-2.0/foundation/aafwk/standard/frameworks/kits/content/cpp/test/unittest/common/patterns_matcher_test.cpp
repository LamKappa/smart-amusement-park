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

#include "ohos/aafwk/content/patterns_matcher.h"

using namespace testing::ext;
using namespace OHOS::AAFwk;
using OHOS::Parcel;

namespace OHOS {
namespace AAFwk {
class PatternsMatcherBaseTest : public testing::Test {
public:
    PatternsMatcherBaseTest() 
    {}
    ~PatternsMatcherBaseTest()
    {
    }
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<PatternsMatcher> PatternsMatcherIn_ = nullptr;
    std::shared_ptr<PatternsMatcher> PatternsMatcherOut_ = nullptr;
};

void PatternsMatcherBaseTest::SetUpTestCase(void)
{}

void PatternsMatcherBaseTest::TearDownTestCase(void)
{}

void PatternsMatcherBaseTest::SetUp(void)
{
    PatternsMatcherIn_ = std::make_shared<PatternsMatcher>();
    PatternsMatcherOut_ = std::make_shared<PatternsMatcher>();
}

void PatternsMatcherBaseTest::TearDown(void)
{
}

/**
 * @tc.number: AaFwk_PatternsMatcher_Parcelable_0100
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling PatternMatcher, and then check result.
 */
HWTEST_F(PatternsMatcherBaseTest, AaFwk_PatternsMatcher_Parcelable_0100, Function | MediumTest | Level1)
{
    PatternsMatcherIn_ = std::make_shared<PatternsMatcher>("1234", MatchType::PATTERN_LITERAL);
    if (PatternsMatcherIn_ != nullptr) {
        Parcel in;
        PatternsMatcherIn_->Marshalling(in);
        std::shared_ptr<PatternsMatcher> PatternsMatcherOut_(PatternsMatcher::Unmarshalling(in));
        if (PatternsMatcherOut_ != nullptr) {
            EXPECT_EQ(PatternsMatcherIn_->GetPattern(), (PatternsMatcherOut_->GetPattern()));
            EXPECT_EQ(PatternsMatcherIn_->GetType(), (PatternsMatcherOut_->GetType()));
        }
    }
}

/**
 * @tc.number: AaFwk_PatternsMatcher_Parcelable_0200
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling PatternMatcher, and then check result.
 */
HWTEST_F(PatternsMatcherBaseTest, AaFwk_PatternsMatcher_Parcelable_0200, Function | MediumTest | Level1)
{
    PatternsMatcherIn_ = std::make_shared<PatternsMatcher>("@#￥#3243adsafdf_中文", MatchType::PATTERN_PREFIX);
    if (PatternsMatcherIn_ != nullptr) {
        Parcel in;
        PatternsMatcherIn_->Marshalling(in);
        std::shared_ptr<PatternsMatcher> PatternsMatcherOut_(PatternsMatcher::Unmarshalling(in));
        if (PatternsMatcherOut_ != nullptr) {
            EXPECT_EQ(PatternsMatcherIn_->GetPattern(), (PatternsMatcherOut_->GetPattern()));
            EXPECT_EQ(PatternsMatcherIn_->GetType(), (PatternsMatcherOut_->GetType()));
        }
    }
}

/**
 * @tc.number: AaFwk_PatternsMatcher_Parcelable_0300
 * @tc.name: Marshalling/Unmarshalling
 * @tc.desc: marshalling PatternMatcher, and then check result.
 */
HWTEST_F(PatternsMatcherBaseTest, AaFwk_PatternsMatcher_Parcelable_0300, Function | MediumTest | Level1)
{
    PatternsMatcherIn_ = std::make_shared<PatternsMatcher>("", MatchType::PATTERN_SIMPLE_GLOB);
    if (PatternsMatcherIn_ != nullptr) {
        Parcel in;
        PatternsMatcherIn_->Marshalling(in);
        std::shared_ptr<PatternsMatcher> PatternsMatcherOut_(PatternsMatcher::Unmarshalling(in));
        if (PatternsMatcherOut_ != nullptr) {
            EXPECT_EQ(PatternsMatcherIn_->GetPattern(), (PatternsMatcherOut_->GetPattern()));
            EXPECT_EQ(PatternsMatcherIn_->GetType(), (PatternsMatcherOut_->GetType()));
        }
    }
}

/**
 * @tc.number: AaFwk_PatternsMatcher_Match_0100
 * @tc.name: Match
 * @tc.desc: Match this PatternsMatcher against an Pattern's data, and then check result.
 */
HWTEST_F(PatternsMatcherBaseTest, AaFwk_PatternsMatcher_Match_0100, Function | MediumTest | Level1)
{
    PatternsMatcherIn_ = std::make_shared<PatternsMatcher>("abcdefg", MatchType::PATTERN_LITERAL);
    if (PatternsMatcherIn_ != nullptr) {
        EXPECT_EQ(PatternsMatcherIn_->match("abcdefg"), true);
        EXPECT_EQ(PatternsMatcherIn_->match("Abcdefg"), false);
    }
}

/**
 * @tc.number: AaFwk_PatternsMatcher_Match_0200
 * @tc.name: Match
 * @tc.desc: Match this PatternsMatcher against an Pattern's data, and then check result.
 */
HWTEST_F(PatternsMatcherBaseTest, AaFwk_PatternsMatcher_Match_0200, Function | MediumTest | Level1)
{
    PatternsMatcherIn_ = std::make_shared<PatternsMatcher>("abcdefg", MatchType::PATTERN_PREFIX);
    if (PatternsMatcherIn_ != nullptr) {
        EXPECT_EQ(PatternsMatcherIn_->match("abcdefgABCDEFG"), true);
        EXPECT_EQ(PatternsMatcherIn_->match("AbcdefgABCDEFG"), false);
    }
}

/**
 * @tc.number: AaFwk_PatternsMatcher_Match_0300
 * @tc.name: Match
 * @tc.desc: Match this PatternsMatcher against an Pattern's data, and then check result.
 */
HWTEST_F(PatternsMatcherBaseTest, AaFwk_PatternsMatcher_Match_0300, Function | MediumTest | Level1)
{
    PatternsMatcherIn_ = std::make_shared<PatternsMatcher>("abc*defg.", MatchType::PATTERN_REGEX);
    if (PatternsMatcherIn_ != nullptr) {
        EXPECT_EQ(PatternsMatcherIn_->match("abcdefgG"), true);
        EXPECT_EQ(PatternsMatcherIn_->match("abcccccdefgG"), true);
        EXPECT_EQ(PatternsMatcherIn_->match("abcdefg"), false);
        EXPECT_EQ(PatternsMatcherIn_->match("ABCDEFG"), false);
    }
}

/**
 * @tc.number: AaFwk_PatternsMatcher_Match_0400
 * @tc.name: Match
 * @tc.desc: Match this PatternsMatcher against an Pattern's data, and then check result.
 */
HWTEST_F(PatternsMatcherBaseTest, AaFwk_PatternsMatcher_Match_0400, Function | MediumTest | Level1)
{
    PatternsMatcherIn_ = std::make_shared<PatternsMatcher>("abc*ABC*123", MatchType::PATTERN_SIMPLE_GLOB);
    if (PatternsMatcherIn_ != nullptr) {
        EXPECT_EQ(PatternsMatcherIn_->match("abcABC123"), true);
        EXPECT_EQ(PatternsMatcherIn_->match("abcdefgABCDEFG123"), true);
        EXPECT_EQ(PatternsMatcherIn_->match("000abcdefg000ABCDEFG000123"), true);
        EXPECT_EQ(PatternsMatcherIn_->match("aBc123"), false);
        EXPECT_EQ(PatternsMatcherIn_->match("AbC123"), false);
        EXPECT_EQ(PatternsMatcherIn_->match("abcABC12345"), false);
    }
}
}
}
