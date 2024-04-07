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

#ifndef LOCINFO_H
#define LOCINFO_H

/**
* @addtogroup I18N
* @{
*
* @brief Provides functions related to internationalization (i18n), with which you can format date, time and numbers.
*
* @since 2.2
* @version 1.0
*/

/**
* @file locale_info.h
*
* @brief Declares functions for obtaining locale information, including language, script, and country/region.
*
* Example code: \n
* Creating a <b>LocaleInfo</b> instance: \n
*      {@code LocaleInfo locale("zh", "Hans", "CN");}
* Obtaining the language: \n
*      {@code const char *language = locale.GetLanguage();}
* Output: \n
*     zh
*
* @since 2.2
* @version 1.0
*/

#include "types.h"
#include <cstdint>

namespace OHOS {
namespace I18N {
class LocaleInfo {
public:
    /**
    * @brief A constructor used to create a <b>LocaleInfo</b> instance with specified language,
    *   script, and country/region.
    *
    * @param lang Indicates the pointer to the specified language.
    * @param script Indicates the pointer to the specified script.
    * @param region Indicates the pointer to the specified country/region.
    * @since 2.2
    * @version 1.0
    */
    LocaleInfo(const char *lang, const char *script, const char *region);

    /**
    * @brief A constructor used to create a <b>LocaleInfo</b> instance with specified language and country/region.
    *
    * @param lang Indicates the pointer to the specified language.
    * @param region Indicates the pointer to the specified country/region.
    * @since 2.2
    * @version 1.0
    */
    LocaleInfo(const char *lang, const char *region);

    /**
    * @brief A constructor used to create a <b>LocaleInfo</b> instance by copying a specified one.
    *
    * @param locale Indicates the specified <b>LocaleInfo</b> instance.
    * @since 2.2
    * @version 1.0
    */
    LocaleInfo(const LocaleInfo& locale);

    /**
    * @brief Default constructor used to create a <b>LocaleInfo</b> instance.
    *
    * @since 2.2
    * @version 1.0
    */
    LocaleInfo();

    /**
    * @brief A destructor used to delete the <b>LocaleInfo</b> instance.
    *
    * @since 2.2
    * @version 1.0
    */
    virtual ~LocaleInfo();

    /**
    * @brief Checks whether this <b>LocaleInfo</b> object equals a specified one.
    *
    * @param other Indicates the <b>LocaleInfo</b> object to compare.
    * @return Returns <b>true</b> if the two objects are equal; returns <b>false</b> otherwise.
    * @since 2.2
    * @version 1.0
    */
    virtual bool operator ==(const LocaleInfo &other) const;

    /**
    * @brief Creates a new <b>LocaleInfo</b> object based on a specified one.
    *
    * @param other Indicates the specified <b>LocaleInfo</b> object.
    * @return Returns the new <b>LocaleInfo</b> object.
    * @since 2.2
    * @version 1.0
    */
    virtual LocaleInfo &operator =(const LocaleInfo &other);

    /**
    * @brief Obtains the ID of this <b>LocaleInfo</b> object, which consists of the language,
    *   script, and country/region.
    *
    * @return Returns the ID.
    * @since 2.2
    * @version 1.0
    */
    const char *GetId() const;

    /**
    * @brief Obtains the language specified in this <b>LocaleInfo</b> object.
    *
    * @return Returns the language.
    * @since 2.2
    * @version 1.0
    */
    const char *GetLanguage() const;

    /**
    * @brief Obtains the script specified in this <b>LocaleInfo</b> object.
    *
    * @return Returns the script.
    * @since 2.2
    * @version 1.0
    */
    const char *GetScript() const;

    /**
    * @brief Obtains the country/region specified in this <b>LocaleInfo</b> object.
    *
    * @return Returns the country/region.
    * @since 2.2
    * @version 1.0
    */
    const char *GetRegion() const;

    /**
    * @brief Obtains the mask of this <b>LocaleInfo</b> object.
    *
    * @return Returns the mask.
    * @since 2.2
    * @version 1.0
    */
    uint32_t GetMask() const;

    /**
    * @brief Checks whether this <b>LocaleInfo</b> object represents the default locale (en-US).
    *
    * @return Returns <b>true</b> if the <b>LocaleInfo</b> object represents the default locale;
    *   returns <b>false</b> otherwise.
    * @since 2.2
    * @version 1.0
    */
    bool IsDefaultLocale() const;
private:
    bool ChangeLanguageCode(char *lang, const int32_t dstSize, const char* src, const int32_t srcSize) const;
    char *language = nullptr;
    char *script = nullptr;
    char *region = nullptr;
    char *id = nullptr;
    bool isSucc = true;
    bool IsSuccess();
    void SetFail();
    void Init(const char *lang, const char *script, const char *region, int &status);
    const int SCRIPT_LENGTH = 4;
    const int REGION_LENGTH = 2;
    static constexpr int LANG_MAX_LENGTH = 3;
    const int LANG_MIN_LENGTH = 2;
    const int CHAR_OFF = 48;
};

enum ESupportScript {
    NOKOWN = 0x0,
    LATN = 0x1,
    HANS = 0x2,
    HANT = 0x3,
    QAAG = 0x4,
    CYRL = 0x5,
    DEVA = 0x6,
    GURU = 0x7
};

enum EMask {
    REGION_FIRST_LETTER = 7,
    SCRIPT_BEGIN = 14,
    LANG_SECOND_BEGIN = 18,
    LANG_FIRST_BEGIN = 25
};
}
}
/** @} */
#endif
