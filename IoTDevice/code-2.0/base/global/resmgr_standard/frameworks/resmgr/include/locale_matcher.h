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
#ifndef OHOS_RESOURCE_MANAGER_LOCALEMATCHER_H
#define OHOS_RESOURCE_MANAGER_LOCALEMATCHER_H
#include "locale_info_impl.h"

namespace OHOS {
namespace Global {
namespace Resource {
class LocaleMatcher {
public:
    static int8_t IsMoreSuitable(const LocaleInfoImpl *current,
        const LocaleInfoImpl *other,
        const LocaleInfoImpl *request);

    static bool Match(const LocaleInfoImpl *current, const LocaleInfoImpl *other);

    static bool IsLanguageTag(const char *str, int32_t len);

    static bool IsScriptTag(const char *str, int32_t len);

    static bool IsRegionTag(const char *str, int32_t len);

    static bool Normalize(LocaleInfoImpl *localeInfo);

    static int8_t IsMoreSpecificThan(const LocaleInfoImpl *current, const LocaleInfoImpl *target);

public:
    static uint64_t EN_US_ENCODE;
    static uint64_t EN_GB_ENCODE;
    static uint64_t EN_QAAG_ENCODE;
    static uint64_t ZH_HANT_MO_ENCODE;
    static uint64_t ZH_HK_ENCODE;
    static uint32_t HANT_ENCODE;
    static constexpr uint64_t ROOT_LOCALE = 0x0;
    static constexpr uint16_t NULL_LANGUAGE = 0x0;
    static constexpr uint16_t NULL_REGION = 0x0;
    static constexpr uint16_t NULL_SCRIPT = 0x0;
    static constexpr uint64_t NULL_LOCALE = 0x0;
    static constexpr uint8_t TRACKPATH_ARRAY_SIZE = 5;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
