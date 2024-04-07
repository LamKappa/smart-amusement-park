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

#ifndef OHOS_RESOURCE_MANAGER_LOCALEINFO_IMPL_H
#define OHOS_RESOURCE_MANAGER_LOCALEINFO_IMPL_H
#include <cstdint>
#include <cstddef>
#include "rstate.h"
#include "lock.h"
#include "locale_info.h"

namespace OHOS {
namespace Global {
namespace Resource {
struct ParseResult {
    const char *tempLanguage = nullptr;
    const char *tempScript = nullptr;
    const char *tempRegion = nullptr;
    int16_t languageTagLen = 0;
    int16_t scriptTagLen = 0;
    int16_t regionTagLen = 0;
};
class LocaleInfoImpl : public LocaleInfo {
public:
    virtual const char* GetLanguage() const;

    virtual const char* GetRegion() const;

    virtual const char* GetScript() const;

    LocaleInfoImpl();

    RState CopyImpl(const LocaleInfoImpl* other);

    RState Copy(const LocaleInfo* other);

    static const LocaleInfoImpl* GetSysDefault();

    static bool UpdateSysDefault(const LocaleInfo& LocaleInfo, bool needNotify);

    static LocaleInfoImpl* BuildFromString(const char* bcp47String, char sep, RState& rState);

    static LocaleInfoImpl* BuildFromParts(const char* language, const char* script, const char* region, RState& rState);

    virtual ~LocaleInfoImpl();

    static constexpr uint16_t END_TYPE = 0x0000;
    // language parse
    static constexpr uint16_t LANG_TYPE = 0x0001;
    // script parse
    static constexpr uint16_t SCRIPT_TYPE = 0x0002;
    // region parse
    static constexpr uint16_t REGION_TYPE = 0x0004;

    static constexpr size_t MIN_BCP47_STR_LEN = 2;

private:
    RState SetLanguage(const char* language, size_t len);

    RState SetScript(const char* script, size_t len);

    RState SetRegion(const char* region, size_t len);

    static LocaleInfoImpl* DoParse(const char* bcp47String, char sep, RState& rState);

    static LocaleInfoImpl* CreateLocaleInfo(ParseResult& parseResult, RState& rState);

    RState Init(const char* language, size_t languageLen, const char* script, size_t scriptLen,
        const char* region, size_t regionLen);

    const char* language_;

    const char* region_;

    const char* script_;

    static LocaleInfoImpl* defaultLocale_;

    static Lock lock_;

    friend class LocaleMatcher;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif

