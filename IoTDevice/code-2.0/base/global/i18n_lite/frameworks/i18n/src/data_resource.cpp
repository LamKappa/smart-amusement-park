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

#include "data_resource.h"
#include <cstring>
#include "securec.h"
#include "str_util.h"

using namespace OHOS::I18N;
using namespace std;

DataResource::DataResource(const LocaleInfo *localeInfo)
{
    uint32_t enMask = LocaleInfo("en", "US").GetMask();
    if (localeInfo == nullptr) {
        localeMask = enMask;
    } else {
        localeMask = localeInfo->GetMask();
        if (localeInfo->IsDefaultLocale()) {
            fallbackMask = 0;
        } else {
            fallbackMask = GetFallbackMask(*localeInfo);
        }
        if ((fallbackMask != 0) && (fallbackMask != enMask)) {
            defaultMask = enMask;
        }
    }
    for (int i = 0; i < RESOURCE_COUNT; ++i) {
        loaded[i] = RESOURCE_COUNT;
    }
}

DataResource::~DataResource()
{
    if (resourceIndex) {
        delete[] resourceIndex;
        resourceIndex = nullptr;
    }
    if (resource) {
        delete[] resource;
        resource = nullptr;
    }
    if (fallbackResourceIndex) {
        delete[] fallbackResourceIndex;
        fallbackResourceIndex = nullptr;
    }
    if (fallbackResource) {
        delete[] fallbackResource;
        fallbackResource = nullptr;
    }
    if (defaultResourceIndex) {
        delete[] defaultResourceIndex;
        defaultResourceIndex = nullptr;
    }
    if (defaultResource) {
        delete[] defaultResource;
        defaultResource = nullptr;
    }
}

string DataResource::GetString(DataResourceType type) const
{
    switch (type) {
        case DataResourceType::DEFAULT_HOUR: {
            return GetString(DefaultHourIndex);
        }
        case DataResourceType::GREGORIAN_AM_PMS: {
            return GetString(GregorianAmPmsIndex);
        }
        case DataResourceType::GREGORIAN_DATE_PATTERNS: {
            return GetString(GregorianDatePatternsIndex);
        }
        case DataResourceType::GREGORIAN_FORMAT_ABBR_MONTH: {
            return GetString(GregorianFormatAbbMonthNamesIndex);
        }
        case DataResourceType::GREGORIAN_FORMAT_ABBR_DAY: {
            return GetString(GregorianFormatAbbDayNamesIndex);
        }
        case DataResourceType::GREGORIAN_FORMAT_WIDE_MONTH: {
            return GetString(GregorianFormatWideMonthNamesIndex);
        }
        case DataResourceType::GREGORIAN_STANDALONE_ABBR_MONTH: {
            return GetString(GregorianStandaloneAbbMonthNamesIndex);
        }
        case DataResourceType::GREGORIAN_STANDALONE_ABBR_DAY: {
            return GetString(GregorianStandaloneAbbDayNamesIndex);
        }
        case DataResourceType::GREGORIAN_TIME_PATTERNS: {
            return GetString(GregorianTimePatternsIndex);
        }
        case DataResourceType::NUMBER_FORMAT: {
            return GetString(NumberFormatIndex);
        }
        case DataResourceType::NUMBER_DIGIT: {
            return GetString(NumberDigitIndex);
        }
        case DataResourceType::PLURAL: {
            return GetString(PluralIndex);
        }
        case DataResourceType::TIME_SEPARATOR: {
            return GetString(TimeSeparatorIndex);
        }
        default: {
            return GetString2(type);
        }
    }
}

string DataResource::GetString2(DataResourceType type) const
{
    switch (type) {
        case DataResourceType::GREGORIAN_FULL_MEDIUM_SHORT_PATTERN: {
            return GetString(GregorianFullMediumShortPatternsIndex);
        }
        case DataResourceType::GREGORIAN_FORMAT_WIDE_DAY: {
            return GetString(GregorianFormatWideDayNamesIndex);
        }
        case DataResourceType::GREGORIAN_HOUR_MINUTE_SECOND_PATTERN: {
            return GetString(GregorianHourMinuteSecondPatternsIndex);
        }
        case DataResourceType::GREGORIAN_STANDALONE_WIDE_DAY: {
            return GetString(GregorianStandaloneWideDayNamesIndex);
        }
        case DataResourceType::GREGORIAN_STANDALONE_WIDE_MONTH: {
            return GetString(GregorianStandaloneWideMonthNamesIndex);
        }
        default: {
            return "";
        }
    }
}

string DataResource::GetString(uint32_t index) const
{
    string temp("");
    if (index >= RESOURCE_COUNT) {
        return "";
    }
    uint32_t targetType = loaded[index];
    if (targetType == RESOURCE_COUNT) {
        return "";
    }
    switch (targetType) {
        case LocaleDataType::RESOURCE: {
            return BinarySearchString(resourceIndex, resourceCount, index, resource);
        }
        case LocaleDataType::FALLBACK_RESOURCE: {
            return BinarySearchString(fallbackResourceIndex, fallbackResourceCount, index, fallbackResource);
        }
        default: {
            return BinarySearchString(defaultResourceIndex, defaultResourceCount, index, defaultResource);
        }
    }
}

string DataResource::BinarySearchString(uint32_t *indexArray, uint32_t length, uint32_t target,
    string *stringArray) const
{
    if ((indexArray == nullptr) || (stringArray == nullptr)) {
        return "";
    }
    uint32_t low = 0;
    uint32_t high = length - 1;
    while (low <= high) {
        uint32_t mid = low + ((high - low) >> 1);
        uint32_t temp = indexArray[mid];
        if (temp == target) {
            return stringArray[mid];
        } else if (temp < target) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return "";
}

bool DataResource::Init(void)
{
    int32_t infile = open(gDataResourcePath, O_RDONLY);
    if (infile < 0) {
        return false;
    }
    bool ret = ReadHeader(infile);
    if (!ret) {
        close(infile);
        return false;
    }
    if ((localesCount < 1) || (localesCount > MAX_LOCALE_ITEM_SIZE)) {
        close(infile);
        return false;
    }
    PrepareData(infile);
    close(infile);
    return true;
}

bool DataResource::ReadHeader(int32_t infile)
{
    int32_t seekSize = lseek(infile, GLOBAL_RESOURCE_HEADER_SKIP, SEEK_SET);
    if (seekSize < 0) {
        return false;
    }
    char cache[GLOBAL_RESOURCE_HEADER_LEFT] = {0};
    int32_t readSize = read(infile, cache, GLOBAL_RESOURCE_HEADER_LEFT);
    if (readSize != GLOBAL_RESOURCE_HEADER_LEFT) {
        return false;
    }
    localesCount = ((static_cast<unsigned char>(cache[0]) << SHIFT_ONE_BYTE) | (static_cast<unsigned char>(cache[1])));
    stringPoolOffset = ((static_cast<unsigned char>(cache[GLOBAL_RESOURCE_INDEX_OFFSET]) << SHIFT_ONE_BYTE) |
        (static_cast<unsigned char>(cache[GLOBAL_RESOURCE_INDEX_OFFSET + 1])));
    return true;
}

bool DataResource::PrepareData(int32_t infile)
{
    uint32_t localeSize = localesCount * GLOBAL_LOCALE_MASK_ITEM_SIZE;
    char *locales = new(nothrow) char[localeSize];
    if (locales == nullptr) {
        return false;
    }
    int32_t readSize = read(infile, locales, localeSize);
    if (readSize < 0 || localeSize != static_cast<uint32_t>(readSize)) {
        delete[] locales;
        locales = nullptr;
        return false;
    }
    int32_t localeIndex = BinarySearchLocale(localeMask, reinterpret_cast<unsigned char*>(locales));
    int32_t fallbackLocaleIndex = -1;
    int32_t defaultLocaleIndex = -1;
    GetFallbackAndDefaultLocaleIndex(fallbackLocaleIndex, defaultLocaleIndex, locales);
    uint32_t configOffset = 0;
    if (localeIndex >= 0) {
        configOffset = ConvertUChar(reinterpret_cast<unsigned char*>(locales + GLOBAL_LOCALE_MASK_ITEM_SIZE *
            localeIndex + GLOBAL_RESOURCE_MASK_OFFSET));
        resourceCount = ConvertUChar(reinterpret_cast<unsigned char*>(locales + GLOBAL_LOCALE_MASK_ITEM_SIZE *
            localeIndex + GLOBAL_RESOURCE_MASK_OFFSET + GLOBAL_RESOURCE_INDEX_OFFSET));
    }
    uint32_t fallbackConfigOffset = 0;
    uint32_t defaultConfigOffset = 0;
    GetFallbackAndDefaultInfo(fallbackLocaleIndex, defaultLocaleIndex, fallbackConfigOffset, defaultConfigOffset,
        locales);
    delete[] locales;
    locales = nullptr;
    bool ret = true;
    if ((localeIndex >= 0) && (resourceCount > 0) &&
        (resourceCount <= DataResourceType::GREGORIAN_STANDALONE_WIDE_MONTH + 1)) {
        ret = PrepareLocaleData(infile, configOffset, resourceCount, LocaleDataType::RESOURCE);
    }
    if ((fallbackLocaleIndex >= 0) && !FullLoaded() && ret && (fallbackResourceCount > 0) &&
        (fallbackResourceCount <= DataResourceType::GREGORIAN_STANDALONE_WIDE_MONTH + 1)) {
        ret = PrepareLocaleData(infile, fallbackConfigOffset, fallbackResourceCount,
            LocaleDataType::FALLBACK_RESOURCE);
    }
    if ((defaultLocaleIndex >= 0) && !FullLoaded() && ret && (defaultResourceCount > 0) &&
        (defaultResourceCount <= DataResourceType::GREGORIAN_STANDALONE_WIDE_MONTH + 1)) {
        ret = PrepareLocaleData(infile, defaultConfigOffset, defaultResourceCount, LocaleDataType::DEFAULT_RESOURCE);
    }
    return ret;
}

void DataResource::GetFallbackAndDefaultLocaleIndex(int32_t &fallbackLocaleIndex, int32_t &defaultLocaleIndex,
    char* locales)
{
    if (fallbackMask != 0) {
        fallbackLocaleIndex = BinarySearchLocale(fallbackMask, reinterpret_cast<unsigned char*>(locales));
    }
    if (defaultMask != 0) {
        defaultLocaleIndex = BinarySearchLocale(defaultMask, reinterpret_cast<unsigned char*>(locales));
    }
}

void DataResource::GetFallbackAndDefaultInfo(const int32_t &fallbackLocaleIndex, const int32_t &defaultLocaleIndex,
    uint32_t &fallbackConfigOffset, uint32_t &defaultConfigOffset, char* locales)
{
    if (fallbackLocaleIndex != -1) {
        fallbackConfigOffset = ConvertUChar(reinterpret_cast<unsigned char*>(locales + GLOBAL_LOCALE_MASK_ITEM_SIZE *
            fallbackLocaleIndex + GLOBAL_RESOURCE_MASK_OFFSET));
        fallbackResourceCount = ConvertUChar(reinterpret_cast<unsigned char*>(locales + GLOBAL_LOCALE_MASK_ITEM_SIZE *
            fallbackLocaleIndex + GLOBAL_RESOURCE_MASK_OFFSET + GLOBAL_RESOURCE_INDEX_OFFSET));
    }
    if (defaultLocaleIndex != -1) {
        defaultConfigOffset = ConvertUChar(reinterpret_cast<unsigned char*>(locales + GLOBAL_LOCALE_MASK_ITEM_SIZE *
            defaultLocaleIndex + GLOBAL_RESOURCE_MASK_OFFSET));
        defaultResourceCount = ConvertUChar(reinterpret_cast<unsigned char*>(locales + GLOBAL_LOCALE_MASK_ITEM_SIZE *
            defaultLocaleIndex + GLOBAL_RESOURCE_MASK_OFFSET + GLOBAL_RESOURCE_INDEX_OFFSET));
    }
}

bool DataResource::PrepareLocaleData(int32_t infile, uint32_t configOffset, uint32_t count, LocaleDataType type)
{
    currentType = type;
    if (count < 1 || count > DataResourceType::GREGORIAN_STANDALONE_WIDE_MONTH + 1) {
        return false;
    }
    uint32_t resourceSize = count * GLOBAL_RESOURCE_CONFIG_SIZE;
    char *configs = new(nothrow) char[resourceSize];
    if (configs == nullptr) {
        return false;
    }
    int32_t seekSize = lseek(infile, configOffset, SEEK_SET);
    if (configOffset != static_cast<uint32_t>(seekSize)) {
        delete[] configs;
        configs = nullptr;
        return false;
    }
    int32_t readSize = read(infile, configs, resourceSize);
    if (readSize != resourceSize) {
        delete[] configs;
        return false;
    }
    bool ret = GetStringFromStringPool(configs, resourceSize, infile, type);
    delete[] configs;
    return ret;
}

uint32_t DataResource::GetFinalCount(char *configs, uint32_t configSize, LocaleDataType type)
{
    uint32_t count = 0;
    switch (type) {
        case LocaleDataType::RESOURCE: {
            count = resourceCount;
            break;
        }
        case LocaleDataType::FALLBACK_RESOURCE: {
            count = fallbackResourceCount;
            break;
        }
        default: {
            count = defaultResourceCount;
        }
    }
    uint32_t finalCount = 0;
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t index = ConvertUChar(reinterpret_cast<unsigned char*>(configs + i * GLOBAL_RESOURCE_CONFIG_SIZE));
        if (index >= RESOURCE_COUNT) {
            return 0;
        }
        if (loaded[index] != RESOURCE_COUNT) {
            continue;
        }
        ++finalCount;
    }
    return finalCount;
}

bool DataResource::GetStringFromStringPool(char *configs, const uint32_t configsSize, int32_t infile,
    LocaleDataType type)
{
    uint32_t finalCount = GetFinalCount(configs, configsSize, type);
    if (finalCount == 0) {
        return true;
    }
    uint32_t originalCount = 0;
    switch (type) {
        case LocaleDataType::RESOURCE: {
            originalCount = resourceCount;
            resourceCount = finalCount;
            resourceIndex = new(nothrow) uint32_t[finalCount];
            if (resourceIndex == nullptr) {
                return false;
            }
            resource = new(nothrow) string[finalCount];
            if (resource == nullptr) {
                return false;
            }
            break;
        }
        case LocaleDataType::FALLBACK_RESOURCE: {
            originalCount = fallbackResourceCount;
            fallbackResourceCount = finalCount;
            fallbackResourceIndex = new(nothrow) uint32_t[finalCount];
            if (fallbackResourceIndex == nullptr) {
                return false;
            }
            fallbackResource = new(nothrow) string[finalCount];
            if (fallbackResource == nullptr) {
                return false;
            }
            break;
        }
        default: {
            originalCount = defaultResourceCount;
            defaultResourceCount = finalCount;
            defaultResourceIndex = new(nothrow) uint32_t[finalCount];
            if (defaultResourceIndex == nullptr) {
                return false;
            }
            defaultResource = new(nothrow) string[finalCount];
            if (defaultResource == nullptr) {
                return false;
            }
            break;
        }
    }
    return Retrieve(configs, configsSize, infile, originalCount, type);
}

void DataResource::GetType(string* &adjustResource, uint32_t* &adjustResourceIndex, uint32_t &count,
    LocaleDataType type)
{
    switch (type) {
        case LocaleDataType::RESOURCE: {
            adjustResource = resource;
            adjustResourceIndex = resourceIndex;
            count = resourceCount;
            break;
        }
        case LocaleDataType::FALLBACK_RESOURCE: {
            adjustResource = fallbackResource;
            adjustResourceIndex = fallbackResourceIndex;
            count = fallbackResourceCount;
            break;
        }
        default: {
            adjustResource = defaultResource;
            adjustResourceIndex = defaultResourceIndex;
            count = defaultResourceCount;
            break;
        }
    }
}

bool DataResource::Retrieve(char *configs, uint32_t configsSize, int32_t infile, const uint32_t orginalCount,
    LocaleDataType type)
{
    uint32_t count = 0;
    string *adjustResource = nullptr;
    uint32_t *adjustResourceIndex = nullptr;
    GetType(adjustResource, adjustResourceIndex, count, type);
    uint32_t currentIndex = 0;
    for (uint32_t i = 0; i < orginalCount; ++i) {
        uint32_t index = ConvertUChar(reinterpret_cast<unsigned char*>(configs + i * GLOBAL_RESOURCE_CONFIG_SIZE));
        if (loaded[index] != RESOURCE_COUNT) {
            continue;
        }

        uint32_t offset = ConvertUChar(reinterpret_cast<unsigned char*>(configs + i *
            GLOBAL_RESOURCE_CONFIG_SIZE + GLOBAL_RESOURCE_INDEX_OFFSET));
        uint32_t length = ConvertUChar(reinterpret_cast<unsigned char*>(configs + i *
            GLOBAL_RESOURCE_CONFIG_SIZE + GLOBAL_RESOURCE_MASK_OFFSET));
        int32_t seekSize = lseek(infile, stringPoolOffset + offset, SEEK_SET);
        if ((length == 0) || (seekSize != stringPoolOffset + offset)) {
            continue;
        }
        char *temp = new(nothrow) char[length];
        if (temp == nullptr) {
            return false;
        }
        int32_t readSize = read(infile, temp, length);
        if ((readSize< 0) || (static_cast<uint32_t>(readSize) != length)) {
            delete[] temp;
            return false;
        }

        adjustResource[currentIndex] = string(temp, length);
        adjustResourceIndex[currentIndex] = index;
        delete[] temp;
        switch (currentType) {
            case LocaleDataType::RESOURCE: {
                loaded[index] = RESOURCE_INDEX;
                break;
            }
            case LocaleDataType::FALLBACK_RESOURCE: {
                loaded[index] = FALLBACK_RESOURCE_INDEX;
                break;
            }
            default: {
                loaded[index] = DEFAULT_RESOURCE_INDEX;
            }
        }
        ++currentIndex;
    }
    return true;
}

int32_t DataResource::BinarySearchLocale(const uint32_t mask, unsigned char *locales)
{
    if (locales == nullptr) {
        return -1;
    }
    uint32_t low = 0;
    uint32_t high = localesCount - 1;
    while (low <= high) {
        uint32_t mid = low + ((high - low) >> 1);
        uint32_t midMask = ConvertUint(locales + mid * GLOBAL_LOCALE_MASK_ITEM_SIZE);
        if (midMask == mask) {
            return static_cast<int32_t>(mid);
        } else if (midMask < mask) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

uint32_t DataResource::ConvertUint(unsigned char *src)
{
    if (src == nullptr) {
        return 0;
    }
    uint32_t ret = 0;
    ret |= (src[0] << SHIFT_THREE_BYTE); // first byte
    ret |= (src[1] << SHIFT_TWO_BYTE); // second byte
    ret |= (src[2] << SHIFT_ONE_BYTE); // third byte
    ret |= src[3]; // forth byte
    return ret;
}

uint32_t DataResource::ConvertUChar(unsigned char *src)
{
    if (src == nullptr) {
        return 0;
    }
    uint32_t ret = 0;
    ret |= (src[0] << SHIFT_ONE_BYTE);
    ret |= src[1];
    return ret;
}

bool DataResource::FullLoaded()
{
    for (uint32_t i = 0; i < RESOURCE_COUNT; ++i) {
        if (loaded[i] == RESOURCE_COUNT) {
            return false;
        }
    }
    return true;
}

uint32_t DataResource::GetFallbackMask(const LocaleInfo &src)
{
    const char *language = src.GetLanguage();
    const char *script = src.GetScript();
    const char *region = src.GetRegion();
    if ((language == "en") && (script == nullptr)) {
        return LocaleInfo("en", "", "US").GetMask();
    }
    if (region == nullptr) {
        return LocaleInfo("en", "US").GetMask();
    }
    if (script == nullptr) {
        return LocaleInfo(language, "", "").GetMask();
    }
    return LocaleInfo(language, script, "").GetMask();
}
