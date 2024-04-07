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


#ifndef NUMBER_DATA_H
#define NUMBER_DATA_H

#define NUMBER_SIGN {                                                    \
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" \
    }
#define ARABIC_NOBREAK_ONE_MINUS 2
#define ARABIC_NOBREAK_TWO_MINUS 3
#define PERCENT_SIGN_INDEX 2
#define SIGNS_SIZE 3
#define PERCENT_INFO_SIZE 3

#include "types.h"
#include "data_resource.h"

namespace OHOS {
namespace I18N {
struct StyleData {
    NumberFormatType type = DECIMAL;
    int decLen = 0;
    int decZeroLen = 0;
    bool suffixZero = false;
    int intLen = 0;
    bool preZero = false;
    char *numFormat = nullptr; // number format style
    char *entireFormat = nullptr;
    bool isTwoGroup = false;
    StyleData() = default;
    ~StyleData();
    StyleData(const StyleData &data);
    StyleData &operator = (const StyleData &data);
};

class NumberData {
public:
    static const char NUMBER_DECIMAL = '.';
    static const char NUMBER_GROUPSIGN = ',';
    static const char NUMBER_PERCENT = '%';
    static const int NUMBER_GROUP = 3;
    static const int TWO_GROUP = 2;
    static constexpr int NUM_SIZE = 10;
    static constexpr int INFO_SIZE = 3;
    char *pattern = nullptr;
    char *percentPattern = nullptr;
    std::string nativeNums[NUM_SIZE] = {}; // used to store 0-9 letters in current language
    char *decimal = nullptr;
    char *group = nullptr;
    char *percent = nullptr;
    bool isNative = false;
    StyleData style;
    StyleData percentStyle;
    friend class NumberFormat;
    NumberData();
    NumberData(const char *pat, const char *percentPat, std::string decSign, std::string groupSign,
        std::string perSign);
    virtual ~NumberData();
    void SetNumSystem(std::string *numSym, const int numSize);
    bool SetMinDecimalLength(int length);
    bool SetMaxDecimalLength(int length);

private:
    void Init(const char *pat, int patLen, const char *percentPat, int perPatLen);
    void InitSign(const std::string *signs, const int signLength);
    void ParsePattern(const char *pattern, const int len, StyleData &styleData);
    void ParseStartPerPattern(const char *pattern, const int len, StyleData &styleData) const;
    void ParseOtherPerPattern(const char *pattern, const int len, StyleData &styleData,
        const int info[], const int infoSize) const;
    void CalculateIntLength(int &intEndPos, const char *pattern, const int len, StyleData &styleData, bool isDec);
    bool IsSuccess();
    void SetFail();
    bool isSucc = true;
    bool isPercent = false;
    int maxDecimalLength = -1;
    const char *NUMBER_FORMAT = "%%.%df";
    const int NUMBER_FORMAT_LENGTH = 5;
    const int ARABIC_NOBREAK_ONE = -96;
    const int ARABIC_NOBREAK_TWO = -62;
};

enum EPercentLocation {
    UNKOWN = 0,
    LEFT = 1,
    RIGHT = 2
};
}
}
#endif