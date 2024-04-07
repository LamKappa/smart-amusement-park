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

#include "locale_info.h"
#include "securec.h"
#include "str_util.h"
#include "types.h"

using namespace OHOS::I18N;

void LocaleInfo::Init(const char *newLang, const char *newScript, const char *newRegion, int &status)
{
    id = nullptr;
    status = IERROR;
    if (newLang == nullptr) {
        return;
    }
    int langLength = LenCharArray(newLang);
    if ((langLength > LANG_MAX_LENGTH) || (langLength < LANG_MIN_LENGTH)) { // language consists of two or three letters
        return;
    }
    int idLength = langLength;
    language = NewArrayAndCopy(newLang, langLength);
    int scriptLength = 0;
    std::string idStr(newLang);

    // script consists of four letters
    if (newScript != nullptr) {
        if ((scriptLength = LenCharArray(newScript)) == SCRIPT_LENGTH) {
            script = NewArrayAndCopy(newScript, scriptLength);
            idLength = idLength + scriptLength + 1;
        }
        if (scriptLength != 0) {
            idStr = idStr + "-" + newScript;
        }
    }
    int regionLength = 0;
    if (newRegion != nullptr) {
        if ((regionLength = LenCharArray(newRegion)) == REGION_LENGTH) {
            region = NewArrayAndCopy(newRegion, regionLength);
            idLength = idLength + regionLength + 1;
        }
        if (regionLength != 0) {
            idStr = idStr + "-" + newRegion;
        }
    }

    id = NewArrayAndCopy(idStr.data(), idLength);
    status = ISUCCESS;
}

LocaleInfo::LocaleInfo(const char *newLang, const char *newScript, const char *newRegion)
{
    int status = ISUCCESS;
    Init(newLang, newScript, newRegion, status);
    if (status != ISUCCESS) {
        SetFail();
    }
}

bool LocaleInfo::IsDefaultLocale() const
{
    if ((GetLanguage() == nullptr) || (GetRegion() == nullptr)) {
        return false;
    }
    return ((strcmp(GetLanguage(), "en") == 0) && (strcmp(GetRegion(), "US") == 0));
}

LocaleInfo::LocaleInfo(const char *newLang, const char *newRegion)
{
    int status = ISUCCESS;
    Init(newLang, nullptr, newRegion, status);
    if (status != ISUCCESS) {
        SetFail();
    }
}

LocaleInfo::LocaleInfo()
{
    id = nullptr;
    SetFail();
}

LocaleInfo::LocaleInfo(const LocaleInfo &o)
{
    int status = ISUCCESS;
    Init(o.language, o.script, o.region, status);
    if (status != ISUCCESS) {
        SetFail();
    }
}

LocaleInfo::~LocaleInfo()
{
    if (language != nullptr) {
        delete[] language;
        language = nullptr;
    }
    if (script != nullptr) {
        delete[] script;
        script = nullptr;
    }
    if (region != nullptr) {
        delete[] region;
        region = nullptr;
    }
    if (id != nullptr) {
        delete[] id;
        id = nullptr;
    }
}

bool LocaleInfo::operator == (const LocaleInfo &other) const
{
    bool ret = CompareLocaleItem(language, other.language);
    if (!ret) {
        return false;
    }
    ret = CompareLocaleItem(script, other.script);
    if (!ret) {
        return false;
    }
    ret = CompareLocaleItem(region, other.region);
    return ret;
}

LocaleInfo &LocaleInfo::operator = (const LocaleInfo &o)
{
    if (&o == this) {
        return *this;
    }
    if ((language != nullptr) && (LenCharArray(language) > 0)) {
        delete[] language;
        language = nullptr;
    }
    if ((script != nullptr) && (LenCharArray(script) > 0)) {
        delete[] script;
        script = nullptr;
    }
    if ((region != nullptr) && (LenCharArray(region) > 0)) {
        delete[] region;
        region = nullptr;
    }
    if ((id != nullptr) && (LenCharArray(id) > 0)) {
        delete[] id;
        id = nullptr;
    }
    if (o.language != nullptr) {
        language = NewArrayAndCopy(o.language, strlen(o.language));
    }
    if (o.script != nullptr) {
        script = NewArrayAndCopy(o.script, strlen(o.script));
    }
    if (o.region != nullptr) {
        region = NewArrayAndCopy(o.region, strlen(o.region));
    }
    if (o.id != nullptr) {
        id = NewArrayAndCopy(o.id, LenCharArray(o.id));
    }
    return *this;
}

const char *LocaleInfo::GetLanguage() const
{
    return language;
}

const char *LocaleInfo::GetScript() const
{
    return script;
}

const char *LocaleInfo::GetRegion() const
{
    return region;
}

const char *LocaleInfo::GetId() const
{
    const char *rid = id;
    return rid;
}

bool LocaleInfo::IsSuccess()
{
    bool r = isSucc;
    isSucc = true;
    return r;
}

void LocaleInfo::SetFail()
{
    isSucc = false;
}

bool LocaleInfo::ChangeLanguageCode(char *lang, const int32_t dstSize, const char *src, const int32_t srcSize) const
{
    if (srcSize > LANG_MIN_LENGTH) { // three letter language only support fil and mai
        if ((language[0] == 'f') && (language[1] == 'i') && (language[LANG_MIN_LENGTH] == 'l')) {
            lang[0] = 't';
            lang[1] = 'l';
        } else if ((language[0] == 'm') && (language[1] == 'a') && (language[LANG_MIN_LENGTH] == 'i')) {
            lang[0] = 'm';
            lang[1] = 'd';
        }
    } else {
        errno_t rc = strcpy_s(lang, dstSize, language);
        if (rc != EOK) {
            return false;
        }
    }
    if ((srcSize == LANG_MIN_LENGTH) && (language[0] == 'h') && (language[1] == 'e')) {
        lang[0] = 'i';
        lang[1] = 'w';
    } else if ((srcSize == LANG_MIN_LENGTH) && (language[0] == 'i') && (language[1] == 'd')) {
        lang[0] = 'i';
        lang[1] = 'n';
    }
    return true;
}

uint32_t LocaleInfo::GetMask() const
{
    if (language == nullptr) {
        return 0;
    }
    char lang[LANG_MAX_LENGTH];
    bool isRight = ChangeLanguageCode(lang, LANG_MAX_LENGTH, language, LenCharArray(language));
    if (!isRight) {
        return 0;
    }
    // use 7bit to represent an English letter,
    // 32--- language ---18--- script ---14--- region ---0
    uint32_t tempLangFirst = (lang[0] - CHAR_OFF);
    uint32_t tempLangSecond = (lang[1] - CHAR_OFF);
    uint32_t mask = (tempLangFirst << LANG_FIRST_BEGIN) | (tempLangSecond << LANG_SECOND_BEGIN);
    if ((script != nullptr) && (LenCharArray(script) > 0)) {
        if (strcmp(script, "Hans") == 0) {
            mask = mask | (HANS << SCRIPT_BEGIN);
        } else if (strcmp(script, "Hant") == 0) {
            mask = mask | (HANT << SCRIPT_BEGIN);
        } else if (strcmp(script, "Latn") == 0) {
            mask = mask | (LATN << SCRIPT_BEGIN);
        } else if (strcmp(script, "Qaag") == 0) {
            mask = mask | (QAAG << SCRIPT_BEGIN);
        } else if (strcmp(script, "Cyrl") == 0) {
            mask = mask | (CYRL << SCRIPT_BEGIN);
        } else if (strcmp(script, "Deva") == 0) {
            mask = mask | (DEVA << SCRIPT_BEGIN);
        } else if (strcmp(script, "Guru") == 0) {
            mask = mask | (GURU << SCRIPT_BEGIN);
        }
    }
    if ((region != nullptr) && (LenCharArray(region) > 1)) {
        uint32_t tempRegion = (region[0] - CHAR_OFF);
        uint32_t tempRegionSecond = (region[1] - CHAR_OFF);
        mask = mask | (tempRegion << REGION_FIRST_LETTER) | (tempRegionSecond);
    }
    return mask;
}