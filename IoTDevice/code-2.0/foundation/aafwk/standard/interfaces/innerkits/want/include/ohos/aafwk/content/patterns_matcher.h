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
#ifndef OHOS_AAFWK_PATTERN_MATCHER_H
#define OHOS_AAFWK_PATTERN_MATCHER_H

#include "match_type.h"

#include <string>
#include <memory>
#include <regex>
#include "parcel.h"
#include "string_ex.h"

namespace OHOS {
namespace AAFwk {
class PatternsMatcher : public Parcelable, public std::enable_shared_from_this<PatternsMatcher> {

public:
    /**
     * @brief A parameterized constructor used to create a PatternsMatcher instance.
     *
     */
    PatternsMatcher();

    /**
     * @brief A parameterized constructor used to create a PatternsMatcher instance.
     *
     * @param patternsMatcher Indicates patternsMatcher used to create a patternsMatcher instance.
     */
    PatternsMatcher(const PatternsMatcher &patternsMatcher);

    /**
     * @brief A parameterized constructor used to create a PatternsMatcher instance.
     *
     * @param pattern Indicates pattern used to create a patternsMatcher instance.
     * @param type Indicates type used to create a patternsMatcher instance.
     */
    PatternsMatcher(std::string pattern, MatchType type);
    ~PatternsMatcher();

    /**
     * @brief Obtains the pattern.
     *
     * @return the specified pattern.
     */
    std::string GetPattern() const;

    /**
     * @brief Obtains the specified type.
     *
     * @return the specified type.
     */
    MatchType GetType() const;

    /**
     * @brief Match this PatternsMatcher against a string data.
     *
     * @param str The desired string to look for.
     * @return Returns either a valid match constant.
     */
    bool match(std::string str);

    /**
     * @brief Match this PatternsMatcher against an Pattern's data.
     *
     * @param pattern The desired data to look for.
     * @param match The full data string to match against.
     * @param type The desired tyoe to look for.
     *
     * @return Returns either a valid match constant.
     */
    static bool MatchPattern(std::string pattern, std::string match, MatchType type);

    /**
     * @brief Marshals this Sequenceable object into a Parcel.
     *
     * @param outParcel Indicates the Parcel object to which the Sequenceable object will be marshaled.
     */
    bool Marshalling(Parcel &parcel) const;

    /**
     * @brief Unmarshals this Sequenceable object from a Parcel.
     *
     * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     */
    static PatternsMatcher *Unmarshalling(Parcel &parcel);

private:
    std::string pattern_;
    MatchType type_;

private:
    /**
     * @brief Match this PatternsMatcher against an Pattern's data.
     *
     * @param pattern The desired data to look for.
     * @param match The full data string to match against.
     *
     * @return Returns either a valid match constant.
     */
    static bool GlobPattern(std::string pattern, std::string match);

    bool ReadFromParcel(Parcel &parcel);
};

}  // namespace AAFwk
}  // namespace OHOS

#endif  // OHOS_AAFWK_PATTERN_MATCHER_H