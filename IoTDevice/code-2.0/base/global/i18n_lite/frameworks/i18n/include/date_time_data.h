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


#ifndef DATE_TIME_DATA_H
#define DATE_TIME_DATA_H

#include <string>
#include "locale_info.h"
#include "types.h"

#define MONTH_SIZE 12
#define DAY_SIZE 7
#define AM_SIZE 2
#define TIME_PATTERN_SIZE 2
#define REGULAR_PATTERN_SIZE 12
#define SEP_HOUR_SIZE 2
#define HOUR_MINUTE_SECOND_PATTERN_SIZE 2
#define FULL_MEDIUM_SHORT_PATTERN_SIZE 3
#define FULL_DATE_INDEX 0
#define MEDIUM_DATE_INDEX 1
#define SHORT_DATE_INDEX 2

namespace OHOS {
namespace I18N {
enum PatternType {
    REGULAR_PATTERN,
    HOUR_MINUTE_SECOND_PATTERN,
    FULL_MEDIUM_SHORT_PATTERN
};

class DateTimeData {
public:
    DateTimeData(const std::string &amPmMarkers, const char *sepAndHour, const int size);
    ~DateTimeData() = default;
    std::string GetMonthName(int32_t index, DateTimeDataType type = DateTimeDataType::FORMAT_ABBR);
    std::string GetDayName(int32_t index, DateTimeDataType type = DateTimeDataType::FORMAT_ABBR);
    std::string GetAmPmMarker(int32_t index, DateTimeDataType type);
    std::string GetPattern(int32_t index, PatternType type = PatternType::REGULAR_PATTERN);
    char GetTimeSeparator(void) const;
    char GetDefaultHour(void) const;
    void SetMonthNamesData(const std::string &formatAbbreviatedMonthNames, const std::string &formatWideMonthNames,
        const std::string &standaloneAbbreviatedMonthNames, const std::string &standaloneWideMonthNames)
    {
        this->formatAbbreviatedMonthNames = formatAbbreviatedMonthNames;
        this->formatWideMonthNames = formatWideMonthNames;
        this->standaloneAbbreviatedMonthNames = standaloneAbbreviatedMonthNames;
        this->standaloneWideMonthNames = standaloneWideMonthNames;
    }
    void SetDayNamesData(const std::string &formatAbbreviatedDayNames, const std::string &formatWideDayNames,
        const std::string &standaloneAbbreviatedDayNames, const std::string &standaloneWideDayNames)
    {
        this->formatAbbreviatedDayNames = formatAbbreviatedDayNames;
        this->formatWideDayNames = formatWideDayNames;
        this->standaloneAbbreviatedDayNames = standaloneAbbreviatedDayNames;
        this->standaloneWideDayNames = standaloneWideDayNames;
    }
    void SetPatternsData(const std::string &patterns, const std::string &hourMinuteSecondPatterns,
        const std::string &fullMediumShortPatterns)
    {
        this->patterns = patterns;
        this->hourMinuteSecondPatterns = hourMinuteSecondPatterns;
        this->fullMediumShortPatterns = fullMediumShortPatterns;
    }
private:
    std::string Parse(const std::string &str, int32_t count);
    std::string formatAbbreviatedMonthNames;
    std::string formatWideMonthNames;
    std::string standaloneAbbreviatedMonthNames;
    std::string standaloneWideMonthNames;
    std::string formatAbbreviatedDayNames;
    std::string formatWideDayNames;
    std::string standaloneAbbreviatedDayNames;
    std::string standaloneWideDayNames;
    std::string patterns;
    std::string amPmMarkers;
    std::string hourMinuteSecondPatterns;
    std::string fullMediumShortPatterns;
    char timeSeparator = ':';
    char defaultHour = 'H';
};
}
}
#endif
