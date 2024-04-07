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
#ifndef OHOS_GLOBAL_I18N_LOCALE_INFO_H
#define OHOS_GLOBAL_I18N_LOCALE_INFO_H

#include "unicode/locid.h"
#include "unicode/localebuilder.h"
#include "unicode/stringpiece.h"

namespace OHOS {
namespace Global {
namespace I18n {
class LocaleInfo {
public:
    LocaleInfo(std::string locale);
    virtual ~LocaleInfo();
    std::string GetLanguage() const;
    std::string GetScript() const;
    std::string GetRegion() const;
    std::string GetBaseName() const;
    static const uint32_t SCRIPT_LEN = 4;
    static const uint32_t REGION_LEN = 2;
private:
    std::string language;
    std::string region;
    std::string script;
    std::string baseName;
    static bool icuInitialized;
    static bool Init();
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif