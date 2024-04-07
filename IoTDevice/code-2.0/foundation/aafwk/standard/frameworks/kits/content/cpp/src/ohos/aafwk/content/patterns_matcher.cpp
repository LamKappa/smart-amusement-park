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

#include "ohos/aafwk/content/patterns_matcher.h"
#include "parcel_macro.h"
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {

/**
 * @brief A parameterized constructor used to create a PatternsMatcher instance.
 *
 */
PatternsMatcher::PatternsMatcher()
{
    pattern_ = "";
    type_ = MatchType::PATTERN_LITERAL;
}

/**
 * @brief A parameterized constructor used to create a PatternsMatcher instance.
 *
 * @param patternsMatcher Indicates patternsMatcher used to create a patternsMatcher instance.
 */
PatternsMatcher::PatternsMatcher(const PatternsMatcher &patternsMatcher)
{
    pattern_ = patternsMatcher.GetPattern();
    type_ = patternsMatcher.GetType();
}

/**
 * @brief A parameterized constructor used to create a PatternsMatcher instance.
 *
 * @param pattern Indicates pattern used to create a patternsMatcher instance.
 * @param type Indicates type used to create a patternsMatcher instance.
 */
PatternsMatcher::PatternsMatcher(std::string pattern, MatchType type)
{
    pattern_ = pattern;
    type_ = type;
}

PatternsMatcher::~PatternsMatcher()
{}

/**
 * @brief Obtains the pattern.
 *
 * @return the specified pattern.
 */
std::string PatternsMatcher::GetPattern() const
{
    return pattern_;
}

/**
 * @brief Obtains the specified type.
 *
 * @return the specified type.
 */
MatchType PatternsMatcher::GetType() const
{
    return type_;
}

/**
 * @brief Match this PatternsMatcher against a string data.
 *
 * @param str The desired string to look for.
 * @return Returns either a valid match constant.
 */
bool PatternsMatcher::match(std::string match)
{
    return MatchPattern(pattern_, match, type_);
}

/**
 * @brief Match this PatternsMatcher against an Pattern's data.
 *
 * @param pattern The desired data to look for.
 * @param match The full data string to match against.
 * @param type The desired tyoe to look for.
 *
 * @return Returns either a valid match constant.
 */
bool PatternsMatcher::MatchPattern(std::string pattern, std::string match, MatchType type)
{
    if (match.empty()) {
        return false;
    }
    switch (type) {
        case MatchType::PATTERN_LITERAL: {
            return pattern == match;
        }
        case MatchType::PATTERN_PREFIX: {
            return match.find(pattern) == 0;
        }
        case MatchType::PATTERN_REGEX: {
            std::regex regex_(pattern);
            return regex_match(match, regex_);
        }
        case MatchType::PATTERN_SIMPLE_GLOB: {
            return GlobPattern(pattern, match);
        }
        default: {
            APP_LOGE("MatchPattern::The other type");
            return false;
        }
    }
}

/**
 * @brief Match this PatternsMatcher against an Pattern's data.
 *
 * @param pattern The desired data to look for.
 * @param match The full data string to match against.
 *
 * @return Returns either a valid match constant.
 */
bool PatternsMatcher::GlobPattern(std::string pattern, std::string match)
{
    size_t indexP = 0;
    size_t find_pos = 0;
    size_t indexM = 0;
    while (indexP < pattern.length() && find_pos != std::string::npos) {
        find_pos = pattern.find("*", indexP);
        std::string p;
        if (find_pos == std::string::npos) {
            p = pattern.substr(indexP, pattern.length());
        } else {
            p = pattern.substr(indexP, find_pos - indexP);
        }
        if (p.length() == 0) {
            indexP = indexP + 1;
            continue;
        }
        size_t find_pos_m = match.find(p, indexM);
        if (find_pos_m == std::string::npos) {
            return false;
        }
        indexP = find_pos;
        indexM = find_pos_m + p.length();
    }
    if (indexM < match.length() && !(pattern.rfind("*") == pattern.length() - 1)) {
        return false;
    }
    return true;
}

/**
 * @brief Marshals this Sequenceable object into a Parcel.
 *
 * @param outParcel Indicates the Parcel object to which the Sequenceable object will be marshaled.
 */
bool PatternsMatcher::Marshalling(Parcel &parcel) const
{
    // write pattern_
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(pattern_));
    // type_
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(type_));

    return true;
}

/**
 * @brief Unmarshals this Sequenceable object from a Parcel.
 *
 * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 */
PatternsMatcher *PatternsMatcher::Unmarshalling(Parcel &parcel)
{
    PatternsMatcher *patternsMatcher = new (std::nothrow) PatternsMatcher();
    if (patternsMatcher != nullptr && !patternsMatcher->ReadFromParcel(parcel)) {
        delete patternsMatcher;
        patternsMatcher = nullptr;
    }
    return patternsMatcher;
}

bool PatternsMatcher::ReadFromParcel(Parcel &parcel)
{
    // pattern_
    std::u16string readString16;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, readString16);
    pattern_ = Str16ToStr8(readString16);

    // flags_
    int32_t type;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, type);
    type_ = static_cast<MatchType>(type);

    return true;
}
}  // namespace AAFwk
}  // namespace OHOS