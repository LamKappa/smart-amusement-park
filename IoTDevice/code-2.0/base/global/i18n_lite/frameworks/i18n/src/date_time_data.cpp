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

#include "date_time_data.h"

using namespace OHOS::I18N;

using namespace std;

DateTimeData::DateTimeData(const string &amPmMarkers, const char *sepAndHour, const int size)
{
    this->amPmMarkers = amPmMarkers;
    // size must >= 2, The first 2 element of sepAndHour need to be extracted, the first element
    // is the time separator and the second is the default hour.
    if (sepAndHour && size >= 2) {
        timeSeparator = sepAndHour[0];
        defaultHour = sepAndHour[1];
    }
}

/**
 * split str with "_"
 */
string DateTimeData::Parse(const string &str, int32_t count)
{
    if (str.empty()) {
        return "";
    }
    int length = str.size();
    int tempCount = 0;
    int ind = 0;
    while ((ind < length) && (tempCount < count)) {
        if (str.at(ind) == '_') {
            ++tempCount;
        }
        ++ind;
    }
    int last = ind;
    --ind;
    while (last < length) {
        if (str.at(last) == '_') {
            break;
        }
        ++last;
    }
    if (last - ind - 1 <= 0) {
        return "";
    }
    return str.substr(ind + 1, last - ind - 1);
}

string DateTimeData::GetMonthName(int32_t index, DateTimeDataType type)
{
    if ((index < 0) || (index >= MONTH_SIZE)) {
        return "";
    }
    switch (type) {
        case DateTimeDataType::FORMAT_ABBR: {
            return Parse(formatAbbreviatedMonthNames, index);
        }
        case DateTimeDataType::FORMAT_WIDE: {
            return Parse(formatWideMonthNames, index);
        }
        case DateTimeDataType::STANDALONE_ABBR: {
            return Parse(standaloneAbbreviatedMonthNames, index);
        }
        default: {
            return Parse(standaloneWideMonthNames, index);
        }
    }
}

string DateTimeData::GetDayName(int32_t index, DateTimeDataType type)
{
    if ((index < 0) || (index >= DAY_SIZE)) {
        return "";
    }
    switch (type) {
        case DateTimeDataType::FORMAT_ABBR: {
            return Parse(formatAbbreviatedDayNames, index);
        }
        case DateTimeDataType::FORMAT_WIDE: {
            return Parse(formatWideDayNames, index);
        }
        case DateTimeDataType::STANDALONE_ABBR: {
            return Parse(standaloneAbbreviatedDayNames, index);
        }
        default: {
            return Parse(standaloneWideDayNames, index);
        }
    }
}

string DateTimeData::GetAmPmMarker(int32_t index, DateTimeDataType type)
{
    if ((index < 0) || (index >= AM_SIZE)) {
        return "";
    }
    return (amPmMarkers != "") ? Parse(amPmMarkers, index) : "";
}

char DateTimeData::GetTimeSeparator(void) const
{
    return timeSeparator;
}

char DateTimeData::GetDefaultHour(void) const
{
    return defaultHour;
}

std::string DateTimeData::GetPattern(int32_t index, PatternType type)
{
    switch (type) {
        case PatternType::HOUR_MINUTE_SECOND_PATTERN: {
            if ((index < 0) || (index >= HOUR_MINUTE_SECOND_PATTERN_SIZE)) {
                return "";
            }
            return Parse(hourMinuteSecondPatterns, index);
        }
        case PatternType::REGULAR_PATTERN: {
            if ((index < 0) || (index >= REGULAR_PATTERN_SIZE)) {
                return "";
            }
            return Parse(patterns, index);
        }
        default: {
            if ((index < 0) || (index >= FULL_MEDIUM_SHORT_PATTERN_SIZE)) {
                return "";
            }
            return Parse(fullMediumShortPatterns, index);
        }
    }
}