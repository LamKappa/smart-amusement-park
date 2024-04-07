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

#ifndef UTILITY_STR_UTIL_H
#define UTILITY_STR_UTIL_H

#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <future>
#include <fstream>
#include <iostream>
#include <regex>
#include <codecvt>
#include <list>
namespace OHOS {
namespace HiviewDFX {
namespace StringUtil {
/**
 * The ReplaceStr function will replace src with dst int base.
 */
std::string ReplaceStr(const std::string& str, const std::string& src, const std::string& dst);

/**
 * The TrimStr function will trim str by cTrim front and end.
 */
std::string TrimStr(const std::string& str, const char cTrim = ' ');

/**
 * The SplitStr function will split str by strSep.
 */
void SplitStr(const std::string& str, const std::string& sep, std::vector<std::string>& strs,
              bool canEmpty = false, bool needTrim = true);

/**
 * The ToString function convert int and double and so on to str.
 */
template<class T>
inline std::string ToString(T iValue)
{
    return std::to_string(iValue);
}

/**
 * convert string to a specific type by sstream
 */
template<typename T>
static void ConvertStringTo(const std::string &in, T &out)
{
    std::stringstream ss;
    ss << in;
    ss >> out;
}

template<class T>
bool IsConvertable(const std::string &inValue)
{
    std::stringstream stream;
    stream << inValue;
    int64_t outValue;
    stream >> outValue;
    T value = (T)outValue;
    stream.clear();
    stream.str("");
    stream << (value + 0);
    std::string tempValue = stream.str();
    if (tempValue != inValue) {
        return false;
    }
    return true;
}

bool IsValidFloatNum(const std::string &value);

/**
 * The StrToInt function convert str to int.
 */
bool StrToInt(const std::string& str, int& value);

/**
 * Append the strings in list with specific delimiter
 */
std::string ConvertListToStr(const std::vector<std::string> &listStr, const std::string &split);

/**
 * The DexToHexString function convert dex to hex string.
 */
std::string DexToHexString(int value, bool upper = true);

/**
 * Get key-value pair separated by colon
 */
using KeyValuePair = std::pair<std::string, std::pair<std::string, char>>;
KeyValuePair GetKeyValueByString(int &start, const std::string &inputString);


template<typename T>
std::string ConvertToUTF8(const std::basic_string<T, std::char_traits<T>, std::allocator<T>> &source)
{
    std::wstring_convert<std::codecvt_utf8_utf16<T>, T> convertor;
    std::string result = convertor.to_bytes(source);
    return result;
}

std::list<std::string> SplitStr(const std::string& str, char delimiter = ' ');
} // namespace StrUtil
} // namespace HiviewDFX
} // namespace OHOS
#endif // UTILITY_STR_UTIL_H