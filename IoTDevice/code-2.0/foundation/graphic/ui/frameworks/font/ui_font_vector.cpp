/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "font/ui_font_vector.h"
#include "gfx_utils/file.h"
#include "graphic_config.h"
#include "securec.h"
#if ENABLE_MULTI_FONT
#include "font/ui_multi_font_manager.h"
#endif

namespace OHOS {
UIFontVector::UIFontVector()
{
#ifdef _WIN32
    ttfDir_ = _pgmptr;
    size_t len = ttfDir_.size();
    size_t pos = ttfDir_.find_last_of('\\');
    if (pos != std::string::npos) {
        ttfDir_.replace((pos + 1), (len - pos), VECTOR_FONT_DIR);
    }
#else
    ttfDir_ = VECTOR_FONT_DIR;
#endif // _WIN32
    ftLibrary_ = nullptr;
    freeTypeInited_ = ((FT_Init_FreeType(&ftLibrary_) == 0) ? true : false);
    SetBaseFontId(FONT_ID_MAX);
    bitmapCache_ = nullptr;
}

UIFontVector::~UIFontVector()
{
    if (freeTypeInited_) {
        FT_Done_FreeType(ftLibrary_);
        freeTypeInited_ = false;
        UnregisterFontInfo(DEFAULT_VECTOR_FONT_FILENAME);
    }
    delete bitmapCache_;
}

uint8_t UIFontVector::RegisterFontInfo(const char* ttfName, uint8_t shaping)
{
    if ((ttfName == nullptr) || !freeTypeInited_) {
        return FONT_INVALID_TTF_ID;
    }
    int32_t j = 0;
    while (j < FONT_ID_MAX) {
        if ((fontInfo_[j].ttfName != nullptr) && !strncmp(fontInfo_[j].ttfName, ttfName, TTF_NAME_LEN_MAX)) {
            return j;
        } else if (fontInfo_[j].ttfName == nullptr) {
            std::string ttfPath = ttfDir_;
            ttfPath.append(ttfName);
            int32_t error = FT_New_Face(ftLibrary_, ttfPath.c_str(), 0, &ftFaces_[j]);
            if (error != 0) {
                return FONT_INVALID_TTF_ID;
            }
            fontInfo_[j].ttfName = ttfName;
            fontInfo_[j].shaping = shaping;
            fontInfo_[j].ttfId = j;
#if ENABLE_MULTI_FONT
            UIMultiFontManager::GetInstance()->UpdateScript(fontInfo_[j]);
#endif
            return j;
        }
        j++;
    }
    return FONT_INVALID_TTF_ID;
}

uint8_t UIFontVector::RegisterFontInfo(const UITextLanguageFontParam* fontsTable, uint8_t num)
{
    if (fontsTable == nullptr) {
        return FONT_INVALID_TTF_ID;
    }
    uint8_t count = 0;
    for (int i = 0; i < num; i++) {
        uint8_t result = RegisterFontInfo(fontsTable[i].ttfName, fontsTable[i].shaping);
        if (result == FONT_INVALID_TTF_ID) {
            continue;
        }
        count++;
    }
    return count;
}

uint8_t UIFontVector::UnregisterFontInfo(const UITextLanguageFontParam* fontsTable, uint8_t num)
{
    if (fontsTable == nullptr) {
        return 0;
    }
    uint8_t count = 0;
    for (int i = 0; i < num; i++) {
        uint8_t result = UnregisterFontInfo(fontsTable[i].ttfName);
        if (result == FONT_INVALID_TTF_ID) {
            return FONT_INVALID_TTF_ID;
        }
        count++;
    }
    return count;
}
uint8_t UIFontVector::UnregisterFontInfo(const char* ttfName)
{
    if (ttfName != nullptr) {
        int32_t i = 0;
        while (i < FONT_ID_MAX) {
            if ((fontInfo_[i].ttfName != nullptr) && !strncmp(fontInfo_[i].ttfName, ttfName, TTF_NAME_LEN_MAX)) {
                fontInfo_[i].ttfName = nullptr;
                FT_Done_Face(ftFaces_[i]);
                ftFaces_[i] = nullptr;
                return static_cast<uint8_t>(i);
            }
            i++;
        }
    }
    return FONT_INVALID_TTF_ID;
}

const UITextLanguageFontParam* UIFontVector::GetFontInfo(uint8_t fontId) const
{
    if (fontId < FONT_ID_MAX) {
        return static_cast<const UITextLanguageFontParam*>(&fontInfo_[fontId]);
    }
    return nullptr;
}

int32_t UIFontVector::OpenVectorFont(uint8_t ttfId)
{
    int32_t i = 0;
    int32_t fp = 0;
    while (i < FONT_ID_MAX) {
        if (fontInfo_[i].ttfName == nullptr) {
            i++;
            continue;
        }
        if (fontInfo_[i].ttfId == ttfId) {
            std::string ttfPath = ttfDir_;
            ttfPath.append(fontInfo_[i].ttfName);
#ifdef _WIN32
            fp = open(ttfPath.c_str(), O_RDONLY | O_BINARY);
#else
            fp = open(ttfPath.c_str(), O_RDONLY);
#endif
            return fp;
        }
        i++;
    }
    return -1;
}

bool UIFontVector::IsVectorFont() const
{
    return true;
}

uint8_t UIFontVector::GetFontWeight(uint8_t fontId)
{
    return FONT_BPP_8;
}

int8_t UIFontVector::SetFontPath(const char* dpath, const char* spath)
{
    if (dpath == nullptr) {
        return INVALID_RET_VALUE;
    }
    ttfDir_ = dpath;
    return RET_VALUE_OK;
}

int8_t UIFontVector::SetCurrentFontId(uint8_t fontId, uint8_t size)
{
    if (size == 0) {
        size = key_ & 0xFF; // the last 8bit is size
    }
    if ((fontId >= FONT_ID_MAX) || (size == 0)) {
        return INVALID_RET_VALUE;
    }
    const UITextLanguageFontParam* fontInfo = GetFontInfo(fontId);
    if ((fontInfo == nullptr) || (fontInfo->ttfName == nullptr)) {
        return INVALID_RET_VALUE;
    }
    uint32_t key = GetKey(fontId, size);
    if (key_ == key) {
        return RET_VALUE_OK;
    }

    if (!freeTypeInited_) {
        return INVALID_RET_VALUE;
    }

    // Set the size
    int error = FT_Set_Char_Size(ftFaces_[fontId], size * FONT_PIXEL_IN_POINT, 0, 0, 0);
    if (error != 0) {
        return INVALID_RET_VALUE;
    }
    key_ = key;
    SetBaseFontId(fontId);
    uintptr_t ramAddr_ = GetRamAddr();
    uint32_t ramLen_ = GetRamLen();
    if (bitmapCache_ == nullptr) {
        bitmapCache_ = new(std::nothrow) UIFontCache(reinterpret_cast<uint8_t*>(ramAddr_), ramLen_);
        if (bitmapCache_ == nullptr) {
            return INVALID_RET_VALUE;
        }
    }
    return RET_VALUE_OK;
}

uint16_t UIFontVector::GetHeight()
{
    uint8_t fontId_ = GetBaseFontId();
    if (!freeTypeInited_ || (ftFaces_[fontId_] == nullptr) || (bitmapCache_ == nullptr)) {
        return 0;
    }
    return static_cast<uint16_t>(ftFaces_[fontId_]->size->metrics.height / FONT_PIXEL_IN_POINT);
}

uint8_t UIFontVector::GetShapingFontId(char* text, uint8_t& ttfId, uint32_t& script, uint8_t fontId, uint8_t size) const
{
#if ENABLE_MULTI_FONT
    const UITextLanguageFontParam* fontParam1 = GetFontInfo(fontId);
    if (fontParam1 == nullptr) {
        return 0;
    }
    if (fontParam1->shaping == 0) {
        if (!UIMultiFontManager::GetInstance()->IsNeedShaping(text, ttfId, script)) {
            return 0; // 0 means  no need to shape
        }
        uint8_t* searchLists = nullptr;
        int8_t length = UIMultiFontManager::GetInstance()->GetSearchFontList(fontId, &searchLists);
        const UITextLanguageFontParam* fontParam2 = nullptr;
        for (uint8_t i = 0; i < length; i++) {
            fontParam2 = GetFontInfo(searchLists[i]);
            if (fontParam2 == nullptr) {
                continue;
            }
            if (fontParam2->ttfId == ttfId) {
                return fontParam2->shaping;
            }
        }
        return 0;
    }
    ttfId = fontParam1->ttfId;

#if ENABLE_SHAPING
    script = UIMultiFontManager::GetInstance()->GetScriptByTtfId(ttfId);
#endif
    return fontParam1->shaping;
#else
    const UITextLanguageFontParam* fontInfo = GetFontInfo(fontId);
    if (fontInfo == nullptr) {
        return 0;
    }
    ttfId = fontInfo->ttfId;
    return fontInfo->shaping;
#endif
}
uint8_t UIFontVector::GetFontId(const char* ttfName, uint8_t size) const
{
    if (ttfName != nullptr) {
        int32_t i = 0;
        while (i < FONT_ID_MAX) {
            if ((fontInfo_[i].ttfName != nullptr) && (strstr(fontInfo_[i].ttfName, ttfName) != nullptr)) {
                return static_cast<uint8_t>(i);
            }
            i++;
        }
    }

    return FONT_ID_MAX;
}

uint8_t UIFontVector::GetFontId(uint32_t unicode) const
{
    int32_t i = 0;
    uint8_t ttfId = ((unicode >> 24) & 0x1F); // 24: Whether 25 ~29 bit storage is ttfId 0x1F:5bit
    while (i < FONT_ID_MAX) {
        if (fontInfo_[i].ttfName == nullptr) {
            i++;
            continue;
        }
        if (fontInfo_[i].ttfId == ttfId) {
            return i;
        }
        i++;
    }
    return FONT_INVALID_TTF_ID;
}

int16_t UIFontVector::GetWidth(uint32_t unicode, uint8_t fontId)
{
    if (!freeTypeInited_ || (ftFaces_[fontId] == nullptr) || (bitmapCache_ == nullptr)) {
        return INVALID_RET_VALUE;
    }
    uint8_t* bitmap = bitmapCache_->GetBitmap(key_, unicode);
    if (bitmap != nullptr) {
        return reinterpret_cast<Metric*>(bitmap)->advance;
    }

    int8_t error = LoadGlyphIntoFace(fontId, unicode);
    if (error != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }
    SetFace(ftFaces_[fontId], unicode);
    return static_cast<uint16_t>(ftFaces_[fontId]->glyph->advance.x / FONT_PIXEL_IN_POINT);
}

int8_t UIFontVector::GetCurrentFontHeader(FontHeader& fontHeader)
{
    uint8_t fontId_ = GetBaseFontId();
    if (!freeTypeInited_ || (ftFaces_[fontId_] == nullptr) || (bitmapCache_ == nullptr)) {
        return INVALID_RET_VALUE;
    }

    fontHeader.ascender = static_cast<int16_t>(ftFaces_[fontId_]->size->metrics.ascender / FONT_PIXEL_IN_POINT);
    fontHeader.descender = static_cast<int16_t>(ftFaces_[fontId_]->size->metrics.descender / FONT_PIXEL_IN_POINT);
    fontHeader.fontHeight = static_cast<uint16_t>(ftFaces_[fontId_]->size->metrics.height / FONT_PIXEL_IN_POINT);
    return RET_VALUE_OK;
}

int8_t UIFontVector::GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode)
{
    uint8_t fontId_ = GetBaseFontId();
    if (!freeTypeInited_ || (ftFaces_[fontId_] == nullptr) || (bitmapCache_ == nullptr)) {
        return INVALID_RET_VALUE;
    }
    uint8_t* bitmap = bitmapCache_->GetBitmap(key_, unicode);
    if (bitmap != nullptr) {
        Metric* f = reinterpret_cast<Metric*>(bitmap);
        glyphNode.left = f->left;
        glyphNode.top = f->top;
        glyphNode.cols = f->cols;
        glyphNode.rows = f->rows;
        glyphNode.advance = f->advance;
        return RET_VALUE_OK;
    }
    int8_t error = LoadGlyphIntoFace(fontId_, unicode);
    if (error != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    glyphNode.left = ftFaces_[fontId_]->glyph->bitmap_left;
    glyphNode.top = ftFaces_[fontId_]->glyph->bitmap_top;
    glyphNode.cols = ftFaces_[fontId_]->glyph->bitmap.width;
    glyphNode.rows = ftFaces_[fontId_]->glyph->bitmap.rows;
    glyphNode.advance = static_cast<uint16_t>(ftFaces_[fontId_]->glyph->advance.x / FONT_PIXEL_IN_POINT);
    SetFace(ftFaces_[fontId_], unicode);
    return RET_VALUE_OK;
}

uint8_t* UIFontVector::GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId)
{
    if (GetGlyphNode(unicode, glyphNode) != RET_VALUE_OK) {
        return nullptr;
    }
    uint8_t* bitmap = bitmapCache_->GetBitmap(key_, unicode);
    if (bitmap != nullptr) {
        return bitmap + sizeof(Metric);
    }
    SetFace(ftFaces_[fontId], unicode);
    return static_cast<uint8_t*>(ftFaces_[fontId]->glyph->bitmap.buffer);
}

int8_t UIFontVector::LoadGlyphIntoFace(uint8_t fontId, uint32_t unicode)
{
    int32_t error;
    if (IsGlyphFont(unicode) != 0) {
        if (fontId != GetFontId(unicode)) {
            return INVALID_RET_VALUE;
        }
        unicode = unicode & (0xFFFFFF); // Whether 0 ~24 bit storage is unicode
        error = FT_Load_Glyph(ftFaces_[fontId], unicode, FT_LOAD_RENDER);
    } else {
        error = FT_Load_Char(ftFaces_[fontId], unicode, FT_LOAD_RENDER);
    }
    if ((error != 0) || (ftFaces_[fontId]->glyph->glyph_index == 0)) {
        return INVALID_RET_VALUE;
    }
    return RET_VALUE_OK;
}

uint8_t UIFontVector::IsGlyphFont(uint32_t unicode)
{
    uint8_t unicodeFontId = GetFontId(unicode);
    if (unicodeFontId == FONT_INVALID_TTF_ID) {
        return 0;
    } else {
        return fontInfo_[unicodeFontId].shaping;
    }
}

void UIFontVector::SetFace(FT_Face ftFace, uint32_t unicode) const
{
    Metric f;
    f.advance = static_cast<uint16_t>(ftFace->glyph->advance.x / FONT_PIXEL_IN_POINT);
    f.left = ftFace->glyph->bitmap_left;
    f.top = ftFace->glyph->bitmap_top;
    f.cols = ftFace->glyph->bitmap.width;
    f.rows = ftFace->glyph->bitmap.rows;

    uint32_t bitmapSize = ftFace->glyph->bitmap.width * ftFace->glyph->bitmap.rows;
    uint8_t* bitmap = bitmapCache_->GetSpace(key_, unicode, bitmapSize + sizeof(Metric));
    if (bitmap != nullptr) {
        if (memcpy_s(bitmap, sizeof(Metric), &f, sizeof(Metric)) != EOK) {
            return;
        }
        if (memcpy_s(bitmap + sizeof(Metric), bitmapSize, ftFace->glyph->bitmap.buffer, bitmapSize) != EOK) {
            return;
        }
    }
}

inline uint32_t UIFontVector::GetKey(uint8_t fontId, uint32_t size)
{
    return ((static_cast<uint32_t>(fontId)) << 24) + size; // fontId store at the (24+1)th bit
}
} // namespace OHOS
