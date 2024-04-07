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
#include "date_time_format.h"
#include <time64.h>
#include "ohos/init_data.h"

namespace OHOS {
namespace Global {
namespace I18n {
using namespace icu;

DateTimeFormat::DateTimeFormat(std::string localeTag)
{
    UErrorCode status = U_ZERO_ERROR;
    auto builder = std::make_unique<LocaleBuilder>();
    Locale locale = builder->setLanguageTag(StringPiece(localeTag)).build(status);
    if (status != U_ZERO_ERROR) {
        locale = Locale::getDefault();
    }
    dateFormat = DateFormat::createDateInstance(DateFormat::DEFAULT, locale);
    if (dateFormat == nullptr) {
        dateFormat = DateFormat::createDateInstance();
    }
    calendar = Calendar::createInstance(locale, status);
    if (status != U_ZERO_ERROR) {
        calendar = Calendar::createInstance(status);
    }
    dateFormat->setCalendar(*calendar);
}

DateTimeFormat::~DateTimeFormat()
{
    if (calendar != nullptr) {
        delete calendar;
        calendar = nullptr;
    }
    if (dateFormat != nullptr) {
        delete dateFormat;
        dateFormat = nullptr;
    }
}

bool DateTimeFormat::icuInitialized = DateTimeFormat::Init();

std::string DateTimeFormat::Format(double timestamp)
{
    // The input time is millisecond needs to be converted to second.
    time64_t time = timestamp / CONVERSION_RATE;
    struct tm timeinfo = { 0 };
    gmtime64_r(&time, &timeinfo);
    // Year start from 1900.
    int year = YEAR_START + timeinfo.tm_year;
    int month = timeinfo.tm_mon;
    int day = timeinfo.tm_mday;
    int hour = timeinfo.tm_hour;
    int minute = timeinfo.tm_min;
    int second = timeinfo.tm_sec;
    UErrorCode status = U_ZERO_ERROR;
    std::string result;
    UnicodeString dateString;
    calendar->clear();
    calendar->set(year, month, day, hour, minute, second);
    dateString.remove();
    dateFormat->format(calendar->getTime(status), dateString, status);
    dateString.toUTF8String(result);
    return result;
}

std::string DateTimeFormat::Format(int year, int month, int day, int hour, int minute, int second)
{
    UErrorCode status = U_ZERO_ERROR;
    std::string result;
    UnicodeString dateString;
    calendar->clear();
    calendar->set(year, month, day, hour, minute, second);
    dateString.remove();
    dateFormat->format(calendar->getTime(status), dateString, status);
    dateString.toUTF8String(result);
    return result;
}

bool DateTimeFormat::Init()
{
    SetHwIcuDirectory();
    return true;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS