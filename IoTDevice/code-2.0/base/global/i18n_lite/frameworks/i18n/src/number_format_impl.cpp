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

#include "number_format_impl.h"
#include "number_data.h"
#include "str_util.h"

using namespace OHOS::I18N;

std::string NumberFormatImpl::ConvertSignAndNum(const char *content, int len, NumberData *data, StyleData &style) const
{
    std::string strContent = content;
    int off = 0;
    for (int i = 0; i < len; i++) {
        switch (content[i]) {
            case NumberData::NUMBER_DECIMAL:
                off = ReplaceAndCountOff(strContent, i + off, data->decimal, off);
                break;
            case NumberData::NUMBER_GROUPSIGN:
                off = ReplaceAndCountOff(strContent, i + off, data->group, off);
                break;
            case NumberData::NUMBER_PERCENT:
                off = ReplaceAndCountOff(strContent, i + off, data->percent, off);
                break;
            default:
                break;
        }
        if (defaultData->isNative) {
            off = ConvertNum(strContent, content[i], data, i, off);
        }
    }
    return strContent;
}

int NumberFormatImpl::ConvertNum(std::string &strContent, char currentChar,
    const NumberData *data, int index, int off) const
{
    std::string numStr = "0123456789";
    int charPos = numStr.find(currentChar);
    return (charPos != std::string::npos) ?
        ReplaceAndCountOff(strContent, index + off, data->nativeNums[charPos].c_str(), off) : off;
}

NumberFormatImpl::NumberFormatImpl(LocaleInfo &locale, int &status)
{
    if (locale.GetId() == nullptr) {
        status = IERROR;
        return;
    }
    mLocale = locale;
}

bool NumberFormatImpl::Init(const DataResource &resource)
{
    std::string unprocessedNumberFormat = resource.GetString(DataResourceType::NUMBER_FORMAT);
    std::string split[NUM_PATTERN_SIZE];
    Split(unprocessedNumberFormat, split, NUM_PATTERN_SIZE, NUM_PATTERN_SEP);
    std::string decSign = split[NUM_DEC_SIGN_INDEX];
    std::string groupSign = split[NUM_GROUP_SIGN_INDEX];
    std::string perSign = split[NUM_PERCENT_SIGN_INDEX];
    std::string origin = split[NUM_PERCENT_PAT_INDEX];
    const char *pat = split[NUM_PAT_INDEX].c_str();
    int size = origin.size();
    std::string adjust = origin;
    // strip "0x80 0xe2 0x8f" these three bytes in pat
    if (size >= 3 && (static_cast<unsigned char>(origin.at(size - 1)) == 0x8f) &&
        (static_cast<unsigned char>(origin.at(size - 2)) == 0x80) &&
        (static_cast<unsigned char>(origin.at(size - 3)) == 0xe2)) {
        adjust = std::string(origin, 0, size - 3);
    }
    const char *percentPat = adjust.c_str();
    defaultData = new(std::nothrow) NumberData(pat, percentPat, decSign, groupSign, perSign);
    if (defaultData == nullptr) {
        return false;
    }
    std::string unprocessedNumberDigit = resource.GetString(DataResourceType::NUMBER_DIGIT);
    if (unprocessedNumberDigit != "") {
        std::string splitDigit[NUM_DIGIT_SIZE];
        Split(unprocessedNumberDigit, splitDigit, NUM_DIGIT_SIZE, NUM_DIGIT_SEP);
        defaultData->SetNumSystem(splitDigit, NUM_DIGIT_SIZE);
    }
    return true;
}


NumberFormatImpl::~NumberFormatImpl()
{
    if (defaultData != nullptr) {
        delete defaultData;
        defaultData = nullptr;
    }
}

std::string NumberFormatImpl::InnerFormat(double num, StyleData &style, bool hasDec, bool isShowGroup,
    int &status) const
{
    errno_t rc = EOK;
    char buff[NUMBER_MAX] = { 0 };

    // convert decimal to char and format
    int len = static_cast<int>(sprintf_s(buff, NUMBER_MAX, style.numFormat, num));
    if (len < 0) {
        status = IERROR;
        return "";
    }

    char *content = buff;
    char *decimalNum = strchr(content, NumberData::NUMBER_DECIMAL);
    int decLen = (decimalNum == nullptr) ? 0 : LenCharArray(decimalNum);
    int lastLen = isShowGroup ? (len + CountGroupNum(len, decLen, style.isTwoGroup)) : len;
    char *result = new(std::nothrow) char[lastLen + 1];
    if (result == nullptr) {
        status = IERROR;
        return "";
    }
    result[lastLen] = '\0';
    if (isShowGroup) {
        char *resultAndContent[] = { result, content };
        int lengths[] = { lastLen, len, style.isTwoGroup};
        AddGroup(resultAndContent, lengths, decimalNum, hasDec, decLen);
    } else {
        rc = strcpy_s(result, lastLen + 1, content);
        CheckStatus(rc, status);
    }
    // del more zero
    lastLen = DelMoreZero(style, decLen, result, lastLen);
    // if percent
    if (!DealWithPercent(buff, result, status, style, lastLen)) {
        delete[] result;
        return "";
    }

    // if have native number to convert
    std::string outStr = ConvertSignAndNum(result, lastLen, defaultData, style);
    delete[] result;
    result = nullptr;
    return outStr;
}

bool NumberFormatImpl::DealWithPercent(char *buff, char *&result, int &status, StyleData &style, int &lastLen) const
{
    if (style.entireFormat != nullptr) {
        bool cleanRet = CleanCharArray(buff, NUMBER_MAX);
        if (!cleanRet) {
            return false;
        }
        int len = static_cast<int>(sprintf_s(buff, NUMBER_MAX, style.entireFormat, result));
        if (len < 0) {
            status = IERROR;
            delete[] result;
            result = nullptr;
            return false;
        }
        char *perResult = new char[len + 1];
        errno_t rc = strcpy_s(perResult, len + 1, buff);
        CheckStatus(rc, status);
        if (status == IERROR) {
            delete[] perResult;
            return false;
        }
        perResult[len] = '\0';
        lastLen = len;
        delete[] result;
        result = perResult;
        perResult = nullptr;
    }
    return true;
}


int NumberFormatImpl::DelMoreZero(const StyleData &style, int decLen, char *&result, int lastLen) const
{
    int num = 0;
    if (style.decZeroLen < decLen - 1) {
        int delNum = decLen - 1 - style.decZeroLen;
        num = DelZero(result, lastLen, delNum, true);
    }
    // delete more char
    if ((maxDecimalLength != NO_SET) && (maxDecimalLength < decLen - 1 - num)) {
        int delNum = decLen - 1 - num - maxDecimalLength;
        num = num + DelZero(result, lastLen - num, delNum, false);
    }
    // fill zero to min
    if ((minDecimalLength != NO_SET) && (minDecimalLength > decLen - 1 - num)) {
        if (decLen - 1 - num < 0) {
            int add = minDecimalLength + 1;
            result = FillMinDecimal(result, lastLen - num, add, false);
            num = num - add;
        } else {
            int add = minDecimalLength - decLen + num + 1;
            result = FillMinDecimal(result, lastLen - num, add, true);
            num = num - add;
        }
    }

    return lastLen - num;
}

void NumberFormatImpl::CheckStatus(const errno_t rc, int &status) const
{
    if (rc != EOK) {
        status = IERROR;
    }
}

int NumberFormatImpl::CountGroupNum(int len, int decLen, bool isTwoGrouped) const
{
    int intLen = len - decLen;
    int groupNum = 0;
    if (!isTwoGrouped) {
        groupNum = static_cast<int>(intLen / NumberData::NUMBER_GROUP);
        int mod = intLen % NumberData::NUMBER_GROUP;
        if (mod == 0) {
            --groupNum;
        }
        return groupNum;
    } else {
        if (intLen <= NumberData::NUMBER_GROUP) {
            return 0;
        }
        groupNum = 1;
        intLen -= NumberData::NUMBER_GROUP;
        groupNum += static_cast<int>(intLen / NumberData::TWO_GROUP);
        int mod = intLen % NumberData::TWO_GROUP;
        if (mod == 0) {
            --groupNum;
        }
        return groupNum;
    }
}

void NumberFormatImpl::AddGroup(char *targetAndSource[], const int len[], const char *decimal,
    bool hasDec, int decLen) const
{
    // The len array must have at least 3 elements and the targetAndSource array
    // must have at least 2 elements.
    if ((targetAndSource == nullptr) || (len == nullptr)) {
        return;
    }
    char *target = targetAndSource[0]; // use array to store target and source string, first is target string
    int targetLen = len[0]; // use array to store target length and source length, first is target legnth
    char *source = targetAndSource[1]; // use array to store target and source string, second is source string
    int sourceLen = len[1]; // use array to store target length and source length, second is source legnth
    int isTwoGroup = len[2];
    int intLen = sourceLen - decLen;
    int addIndex = 0;
    for (int i = 0; (i < intLen) && (addIndex < targetLen); i++, addIndex++) {
        int index = intLen - i;
        // ADD GROUP SIGN
        if (isTwoGroup == 0) {
            if ((index % NumberData::NUMBER_GROUP == 0) && (i != 0)) {
                target[addIndex] = ',';
                addIndex++;
            }
            target[addIndex] = source[i];
        } else {
            if ((index == NumberData::NUMBER_GROUP) && (i != 0)) {
                target[addIndex] = ',';
                addIndex++;
                target[addIndex] = source[i];
            } else if ((index > NumberData::NUMBER_GROUP) &&
                ((index - NumberData::NUMBER_GROUP) % NumberData::TWO_GROUP == 0) && (i != 0)) {
                target[addIndex] = ',';
                addIndex++;
                target[addIndex] = source[i];
            } else {
                target[addIndex] = source[i];
            }
        }
    }
    if (decLen > 0) {
        target[addIndex] = hasDec ? '.' : '\0';
        for (int i = 1; (i < decLen) && (addIndex < targetLen); i++) {
            target[addIndex + i] = hasDec ? decimal[i] : '\0';
        }
    }
}

int NumberFormatImpl::DelZero(char *target, int len, int delNum, bool onlyZero) const
{
    int num = 0;
    for (int i = len - 1; (i > len - delNum - 1) && (i >= 0); i--) {
        if ((target[i] != '0') && onlyZero) {
            break;
        }
        target[i] = '\0';
        num++;
        if ((i - 1 > 0) && (target[i - 1] == '.')) {
            target[i - 1] = '\0';
            num++;
            break;
        }
    }
    return num;
}

std::string NumberFormatImpl::Format(double num, NumberFormatType type, int &status) const
{
    if (defaultData == nullptr) {
        status = IERROR;
        return "";
    }
    if (type == PERCENT) { // percent,the decimal needs to be multiplied by 100.
        return InnerFormat(num * 100, defaultData->percentStyle, true, true, status);
    } else {
        return InnerFormat(num, defaultData->style, true, true, status);
    }
}

std::string NumberFormatImpl::Format(int num, int &status) const
{
    if (defaultData == nullptr) {
        status = IERROR;
        return "";
    }
    return InnerFormat(num, defaultData->style, false, true, status);
}

std::string NumberFormatImpl::FormatNoGroup(double num, NumberFormatType type, int &status) const
{
    if (defaultData == nullptr) {
        status = IERROR;
        return "";
    }
    if (type == PERCENT) { // percent,the decimal needs to be multiplied by 100.
        return InnerFormat(num * 100, defaultData->percentStyle, true, false, status);
    } else {
        return InnerFormat(num, defaultData->style, true, false, status);
    }
}

std::string NumberFormatImpl::FormatNoGroup(int num, int &status) const
{
    if (defaultData == nullptr) {
        status = IERROR;
        return "";
    }
    return InnerFormat(num, defaultData->style, false, false, status);
}

bool NumberFormatImpl::SetMaxDecimalLength(int length)
{
    if (length < 0) {
        maxDecimalLength = -1;
        return true;
    }
    maxDecimalLength = length;
    if (defaultData != nullptr) {
        defaultData->SetMaxDecimalLength(length);
    }
    return true;
}

bool NumberFormatImpl::SetMinDecimalLength(int length)
{
    if (length < 0) {
        minDecimalLength = -1;
    } else {
        minDecimalLength = length;
    }
    if (defaultData != nullptr) {
        return defaultData->SetMinDecimalLength(length);
    }
    return false;
}

char *NumberFormatImpl::FillMinDecimal(char *target, int len, int addSize, bool isDec) const
{
    char *content = NewArrayAndCopy(target, len + addSize);
    if (content == nullptr) {
        return nullptr;
    }
    for (int i = 0; i < addSize; i++) {
        if ((!isDec) && (i == 0)) {
            content[len + i] = '.';
            continue;
        }
        content[len + i] = '0';
    }
    if (target != nullptr) {
        delete [] target;
        target = nullptr;
    }
    return content;
}