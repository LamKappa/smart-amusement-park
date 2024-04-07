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

#ifndef OHOS_RESOURCE_MANAGER_LOCALEINFO_H
#define OHOS_RESOURCE_MANAGER_LOCALEINFO_H
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include "rstate.h"

namespace OHOS {
namespace Global {
namespace Resource {
class LocaleInfo {
public:
    virtual const char* GetLanguage() const = 0;

    virtual const char* GetRegion() const = 0;

    virtual const char* GetScript() const = 0;

    virtual ~LocaleInfo() {};
};
const LocaleInfo* GetSysDefault();

void UpdateSysDefault(const LocaleInfo& localeInfo, bool needNotify);

LocaleInfo* BuildFromString(const char* str, char sep, RState& rState);

LocaleInfo* BuildFromParts(const char* language, const char* script, const char* region, RState& rState);

void FindAndSort(std::string localeStr, std::vector<std::string>& candidateLocale,  std::vector<std::string>& outValue);
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif

