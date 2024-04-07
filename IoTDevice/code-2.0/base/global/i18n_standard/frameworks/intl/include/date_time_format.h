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
#ifndef OHOS_GLOBAL_I18N_DATE_TIME_FORMAT_H
#define OHOS_GLOBAL_I18N_DATE_TIME_FORMAT_H

#include "unicode/locid.h"
#include "unicode/datefmt.h"
#include "unicode/localebuilder.h"

namespace OHOS {
namespace Global {
namespace I18n {
class DateTimeFormat {
public:
    DateTimeFormat(std::string locale);
    virtual ~DateTimeFormat();
    std::string Format(double timeStamp);
    std::string Format(int year, int month, int day, int hour, int minute, int second);
    static const int64_t CONVERSION_RATE = 1000;
    static const int64_t YEAR_START = 1900;
private:
    icu::DateFormat *dateFormat;
    icu::Calendar *calendar;
    static bool icuInitialized;
    static bool Init();
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif