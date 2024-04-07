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

#include "number_data.h"
#include "securec.h"
#include "str_util.h"

using namespace OHOS::I18N;

StyleData::StyleData(const StyleData &data)
{
    decLen = data.decLen;
    decZeroLen = data.decZeroLen;
    suffixZero = data.suffixZero;
    intLen = data.intLen;
    preZero = data.preZero;
    if (data.numFormat != nullptr) {
        int len = LenCharArray(data.numFormat);
        numFormat = NewArrayAndCopy(data.numFormat, len);
    }
    if (data.entireFormat != nullptr) {
        int len = LenCharArray(data.entireFormat);
        entireFormat = NewArrayAndCopy(data.entireFormat, len);
    }
}

StyleData::~StyleData()
{
    if (numFormat != nullptr) {
        delete[] numFormat;
        numFormat = nullptr;
    }
    if (entireFormat != nullptr) {
        delete[] entireFormat;
        entireFormat = nullptr;
    }
}

StyleData &StyleData::operator=(const StyleData &data)
{
    decLen = data.decLen;
    decZeroLen = data.decZeroLen;
    suffixZero = data.suffixZero;
    intLen = data.intLen;
    preZero = data.preZero;
    if (data.numFormat != nullptr) {
        int len = LenCharArray(data.numFormat);
        numFormat = NewArrayAndCopy(data.numFormat, len);
    }
    if (data.entireFormat != nullptr) {
        int len = LenCharArray(data.entireFormat);
        entireFormat = NewArrayAndCopy(data.entireFormat, len);
    }
    return *this;
}

void NumberData::SetNumSystem(std::string *numSym, const int numSize)
{
    if (numSym == nullptr || numSize <= 0) {
        return;
    }
    ArrayCopy(nativeNums, NUM_SIZE, numSym, numSize);
    if (!(numSym[0]).empty() && ((numSym[0]).at(0) != '0')) {
        isNative = true;
    }
}

void NumberData::Init(const char *pat, int patLen, const char *percentPat, int perPatLen)
{
    if (pat == nullptr || patLen <= 0 || percentPat == nullptr || perPatLen <= 0) {
        return;
    }
    pattern = NewArrayAndCopy(pat, patLen);
    percentPattern = NewArrayAndCopy(percentPat, perPatLen);
    percentStyle.type = PERCENT;
    if (pattern != nullptr) {
        ParsePattern(pattern, patLen, style);
    }
    if (percentPattern != nullptr) {
        ParsePattern(percentPattern, perPatLen, percentStyle);
    }
}

void NumberData::InitSign(const std::string *signs, int size)
{
    if (signs == nullptr || size < PERCENT_SIGN_INDEX) {
        return;
    }
    std::string decSign = signs[0]; // use array to store num data, first is decimal sign
    std::string groupSign = signs[1]; // use array to store num data, second is group sign
    std::string perSign = signs[PERCENT_SIGN_INDEX]; // use array to store num data, third is percent sign
    const char *td = decSign.c_str();
    decimal = NewArrayAndCopy(td, LenCharArray(td));
    const char *tdg = groupSign.c_str();
    group = NewArrayAndCopy(tdg, LenCharArray(tdg));
    const char *tdp = perSign.c_str();
    percent = NewArrayAndCopy(tdp, LenCharArray(tdp));
}

void NumberData::ParsePattern(const char *pattern, const int len, StyleData &styleData)
{
    if (pattern == nullptr || len <= 0) {
        return;
    }
    bool isDec = false;
    int decLen = 0;
    int decZeroLen = 0;
    if (strcmp(pattern, "#,##,##0.###") == 0) {
        styleData.isTwoGroup = true;
    }
    for (int i = 0; i < len; i++) { // calculate the format after decimal sign
        char temp = pattern[i];
        if (temp == '.') {
            isDec = true;
            continue;
        }
        if (isDec) {
            if (temp == '#') {
                decLen++;
            } else if (temp == '0') {
                decLen++;
                decZeroLen++;
                styleData.suffixZero = true;
            }
        }
    }
    if (!isPercent && (maxDecimalLength != -1)) {
        styleData.decLen = maxDecimalLength;
        decLen = maxDecimalLength;
    } else {
        styleData.decLen = decLen;
    }
    styleData.decZeroLen = decZeroLen;
    int intEndPos = len - decLen; // cal how many must zero before decimal
    CalculateIntLength(intEndPos, pattern, len, styleData, isDec);
    char *format = new(std::nothrow) char[NUMBER_FORMAT_LENGTH];
    if (format == nullptr) {
        SetFail();
        return;
    }
    if (snprintf_s(format, NUMBER_FORMAT_LENGTH, NUMBER_FORMAT_LENGTH - 1, NUMBER_FORMAT, decLen) == -1) {
        SetFail();
        delete[] format;
        format = nullptr;
        return;
    }
    styleData.numFormat = format;
    if (styleData.type == PERCENT) { // parse percent
        ParseStartPerPattern(pattern, len, styleData);
    }
}

void NumberData::CalculateIntLength(int &intEndPos, const char *pattern, const int len,
    StyleData &styleData, bool isDec)
{
    if (pattern == nullptr || len <= 0) {
        return;
    }
    (void) len;
    if (isDec) {
        intEndPos--;
    }
    int intLen = 0;
    for (; intEndPos > 0; intEndPos--) {
        char temp = pattern[intEndPos - 1];
        if (temp == '0') {
            styleData.preZero = true;
            intLen++;
        }
    }
    styleData.intLen = intLen;
}

void NumberData::ParseStartPerPattern(const char *pattern, const int len, StyleData &styleData) const
{
    if (pattern == nullptr || len <= 0) {
        return;
    }

    // parse the percent sign and position
    int perSignPos = UNKOWN; // 0 : no percent 1:left 2:right;
    int hasSpace = 0;
    int space = 0; // 0 = 0020 1 = c2a0
    if (pattern[0] == '%') {
        perSignPos = LEFT;
        if ((len >= 2) && pattern[1] == ' ') { // length >= 2 guarantees that we can safely get second byte
            hasSpace = 1;
        }

        // length >= 3 guarantees that we can safely get third byte
        if ((len >= 3) && (static_cast<int>(pattern[2]) == ARABIC_NOBREAK_ONE) &&
            (static_cast<int>(pattern[1]) == ARABIC_NOBREAK_TWO)) {
            // the last two char is no break space (c2a0)
            hasSpace = 1;
            space = 1;
        }
    } else if (pattern[len - 1] == '%') {
        perSignPos = RIGHT; // percent in right
        if ((len >= 2) && (pattern[len - 2] == ' ')) { // the last but one is space
            hasSpace = 1;
        }

        // length >= 3 guarantees that we can safely get third byte
        if ((len >= 3) &&
            (static_cast<signed char>(pattern[len - 2]) == ARABIC_NOBREAK_ONE) && // the reciprocal second
            (static_cast<signed char>(pattern[len - 3]) == ARABIC_NOBREAK_TWO)) { // the reciprocal third
            // the last two chars is no break space (c2a0)
            hasSpace = 1;
            space = 1;
        }
    }
    int info[INFO_SIZE] = { perSignPos, hasSpace, space };
    ParseOtherPerPattern(pattern, len, styleData, info, PERCENT_INFO_SIZE);
}

void NumberData::ParseOtherPerPattern(const char *pattern, const int len,
    StyleData &styleData, const int *info, const int infoSize) const
{
    if (pattern == nullptr || len < 2 || infoSize < 3) {
        return;
    }
    int perSignPos = info[0]; // use array to store percent sign and space data, first is percent sign postion
    int hasSpace = info[1]; // use array to store percent sign and space data, second is hash space or not
    int space = info[2]; // use array to store percent sign and space data, second is space type
    if (perSignPos > 0) {
        std::string type;
        if (perSignPos == 1) {
            type = "%%%s";
            if ((hasSpace > 0) && (space == 0)) {
                type = "%% %s";
            } else if ((hasSpace > 0) && (space == 1)) {
                unsigned char typeChars[] = { 0x25, 0x25, 0xC2, 0xA0, 0x25, 0x73, 0x0 }; // %%\uc2a0%s
                type = reinterpret_cast<char const *>(typeChars);
            } else {
                // do nothing
            }
        } else {
            type = "%s%%";
            if ((hasSpace > 0) && (space == 0)) {
                type = "%s %%";
            } else if ((hasSpace > 0) && (space == 1)) {
                unsigned char typeChars[] = { 0x25, 0x73, 0xC2, 0xA0, 0x25, 0x25, 0x0 }; // %s\uc2a0%%
                type = reinterpret_cast<char const *>(typeChars);
            } else {
                // do nothing
            }
        }
        int typeLen = LenCharArray(type.data());
        styleData.entireFormat = NewArrayAndCopy(type.data(), typeLen);
    }
}

bool NumberData::SetMinDecimalLength(int length)
{
    if (length < style.decLen) {
        return true;
    }
    style.decLen = length;

    // calculate number format
    char *format = new(std::nothrow) char[NUMBER_FORMAT_LENGTH];
    if (format == nullptr) {
        SetFail();
        return false;
    }
    int re = sprintf_s(format, NUMBER_FORMAT_LENGTH, NUMBER_FORMAT, length);
    if (re == -1) {
        SetFail();
        delete[] format;
        format = nullptr;
        return false;
    }
    if (style.numFormat != nullptr) {
        delete[] style.numFormat;
    }
    style.numFormat = format;
    return true;
}

NumberData::NumberData(const char *pat, const char *percentPat, std::string decSign,
    std::string groupSign, std::string perSign)
{
    if (pat != nullptr || percentPat != nullptr) {
        std::string nums[NUM_SIZE] = NUMBER_SIGN;
        SetNumSystem(nums, NUM_SIZE);
        std::string signs[3] = { decSign, groupSign, perSign }; // use string array contain number data
        Init(pat, LenCharArray(pat), percentPat, LenCharArray(percentPat));
        InitSign(signs, SIGNS_SIZE);
    }
}

NumberData::NumberData()
{
    isNative = false;
    std::string ns[NUM_SIZE] = NUMBER_SIGN;
    std::string signs[3] = { ".", ",", "%" }; // use string array contain number data
    const char *enNumberPattern = "#,##0.###";
    const char *percentPattern = "#,##0%";
    Init(const_cast<char *>(enNumberPattern), LenCharArray(enNumberPattern), const_cast<char *>(percentPattern),
        LenCharArray(percentPattern));
    InitSign(signs, SIGNS_SIZE);
}

NumberData::~NumberData()
{
    if (pattern != nullptr) {
        delete[] pattern;
        pattern = nullptr;
    }
    if (percentPattern != nullptr) {
        delete[] percentPattern;
        percentPattern = nullptr;
    }
    if (style.numFormat != nullptr) {
        delete[] style.numFormat;
        style.numFormat = nullptr;
    }
    if (percentStyle.numFormat != nullptr) {
        delete[] percentStyle.numFormat;
        percentStyle.numFormat = nullptr;
    }
    if (style.entireFormat != nullptr) {
        delete[] style.entireFormat;
        style.entireFormat = nullptr;
    }
    if (percentStyle.entireFormat != nullptr) {
        delete[] percentStyle.entireFormat;
        percentStyle.entireFormat = nullptr;
    }
    if (group != nullptr) {
        delete[] group;
        group = nullptr;
    }
    if (percent != nullptr) {
        delete[] percent;
        percent = nullptr;
    }
    if (decimal != nullptr) {
        delete[] decimal;
        decimal = nullptr;
    }
}

bool NumberData::IsSuccess()
{
    bool r = isSucc;
    isSucc = true;
    return r;
}

void NumberData::SetFail()
{
    isSucc = false;
}

bool NumberData::SetMaxDecimalLength(int length)
{
    if (length < 0) {
        maxDecimalLength = -1;
        return true;
    }
    maxDecimalLength = length;
    if (pattern != nullptr) {
        isPercent = false;
        ParsePattern(pattern, strlen(pattern), style);
    }
    return true;
}
