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

#include "font/ui_font_bitmap.h"

#include "font/ui_font_adaptor.h"
#include "font/ui_font_builder.h"
#include "gfx_utils/file.h"
#include "graphic_config.h"
#if ENABLE_MULTI_FONT
#include "font/ui_multi_font_manager.h"
#endif
#if ENABLE_SHAPING
#include "font/ui_text_shaping.h"
#endif

namespace OHOS {
UIFontBitmap::UIFontBitmap() : dynamicFont_(), dynamicFontRamUsed_(0), dynamicFontFd_(-1)
{
    SetBaseFontId(UIFontBuilder::GetInstance()->GetBitmapFontIdMax());
    bitmapCache_ = nullptr;
    bitmapRamUsed_ = FONT_BITMAP_CACHE_SIZE;
}

UIFontBitmap:: ~UIFontBitmap()
{
    if (dynamicFontFd_ >= 0) {
        close(dynamicFontFd_);
    }
    if (bitmapCache_ != nullptr) {
        delete bitmapCache_;
        bitmapCache_ = nullptr;
    }
}

bool UIFontBitmap::IsVectorFont() const
{
    return false;
}

uint8_t UIFontBitmap::GetShapingFontId(char* text, uint8_t& ttfId, uint32_t& script, uint8_t fontId, uint8_t size) const
{
#if ENABLE_MULTI_FONT
    return UIMultiFontManager::GetInstance()->GetShapingFontId(text, fontId, ttfId, script);
#else
    UITextLanguageFontParam* fontParam = UIFontBuilder::GetInstance()->GetTextLangFontsTable(fontId);
    if (fontParam == nullptr) {
        return 0;
    }
    ttfId = fontParam->ttfId;
    return fontParam->shaping;
#endif
}

uint8_t UIFontBitmap::GetFontWeight(uint8_t fontId)
{
    UITextLanguageFontParam* fontParam = UIFontBuilder::GetInstance()->GetTextLangFontsTable(fontId);
    if (fontParam == nullptr) {
        return 0;
    }
    return fontParam->fontWeight;
}

int8_t UIFontBitmap::SetFontPath(const char* dpath, const char* spath)
{
    if (dpath == nullptr) {
        return INVALID_RET_VALUE;
    }
#ifdef _WIN32
    dynamicFontFd_ = open(dpath, O_RDONLY | O_BINARY);
#else 
    dynamicFontFd_ = open(dpath, O_RDONLY);
#endif
    if (dynamicFontFd_ < 0) {
        return INVALID_RET_VALUE;
    }
    dynamicFont_.SetRamBuffer(GetRamAddr());
    uint32_t start = 0;
    int32_t ret = dynamicFont_.SetFile(dynamicFontFd_, start);
    if (ret == INVALID_RET_VALUE) {
        close(dynamicFontFd_);
        dynamicFontFd_ = -1;
        return ret;
    }
    dynamicFontRamUsed_ = dynamicFont_.GetRamUsedLen();
    return RET_VALUE_OK;
}

int8_t UIFontBitmap::SetCurrentFontId(uint8_t fontId, uint8_t size)
{
    int8_t ret = SetDynamicFontId(fontId);
    if (ret == RET_VALUE_OK) {
        SetBaseFontId(fontId);
    }
    return ret;
}

uint16_t UIFontBitmap::GetHeight()
{
    int16_t ret = dynamicFont_.SetCurrentFontId(GetBaseFontId());
    if (ret == INVALID_RET_VALUE) {
        return ret;
    }
    return dynamicFont_.GetFontHeight();
}

uint8_t UIFontBitmap::GetFontId(const char* ttfName, uint8_t size) const
{
    if (ttfName == nullptr) {
        return UIFontBuilder::GetInstance()->GetBitmapFontIdMax();
    }
    uint8_t id;
    for (id = 0; id < UIFontBuilder::GetInstance()->GetBitmapFontIdMax(); ++id) {
        UITextLanguageFontParam* fontParam = UIFontBuilder::GetInstance()->GetTextLangFontsTable(id);
        if (fontParam != nullptr) {
            if ((fontParam->size == size) && (strncmp(fontParam->ttfName, ttfName, TTF_NAME_LEN_MAX) == 0)) {
                break;
            }
        }
    }
    return id;
}

int16_t UIFontBitmap::GetWidth(uint32_t unicode, uint8_t fontId)
{
    return GetWidthInFontId(unicode, fontId);
}

uint8_t* UIFontBitmap::GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId)
{
    return SearchInFont(unicode, glyphNode, fontId);
}

int8_t UIFontBitmap::GetCurrentFontHeader(FontHeader& fontHeader)
{
    int8_t ret = dynamicFont_.SetCurrentFontId(GetBaseFontId());
    if (ret == INVALID_RET_VALUE) {
        return ret;
    }
    const FontHeader* header = dynamicFont_.GetCurrentFontHeader();
    if (header != nullptr) {
        fontHeader = *header;
        return RET_VALUE_OK;
    }
    return INVALID_RET_VALUE;
}

int8_t UIFontBitmap::GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode)
{
    int8_t ret = dynamicFont_.SetCurrentFontId(GetBaseFontId());
    if (ret == INVALID_RET_VALUE) {
        return ret;
    }
    const GlyphNode* node = dynamicFont_.GetGlyphNode(unicode);
    if (node != nullptr) {
        glyphNode = *node;
        return RET_VALUE_OK;
    }
    return INVALID_RET_VALUE;
}

int8_t UIFontBitmap::GetFontVersion(char* dVersion, uint8_t dLen, char* sVersion, uint8_t sLen) const
{
    return dynamicFont_.GetFontVersion(dVersion, dLen);
}

int8_t UIFontBitmap::SetCurrentLangId(uint8_t langId)
{
    if (bitmapCache_ == nullptr) {
        uint8_t* bitmapCacheAddr = reinterpret_cast<uint8_t*>(GetRamAddr() + dynamicFontRamUsed_);
        bitmapCache_ = new UIFontCache(bitmapCacheAddr, bitmapRamUsed_);
    }
    uint32_t total = dynamicFontRamUsed_ + bitmapRamUsed_;
    return (total <= GetRamLen()) ? RET_VALUE_OK : INVALID_RET_VALUE;
}

UITextLanguageFontParam* UIFontBitmap::GetFontInfo(uint8_t fontId) const
{
    return UIFontBuilder::GetInstance()->GetTextLangFontsTable(fontId);
}


uint32_t UIFontBitmap::GetBitmapRamUsed()
{
    return bitmapRamUsed_;
}

uint32_t UIFontBitmap::GetDynamicFontRamUsed()
{
    return dynamicFontRamUsed_;
}

uint32_t UIFontBitmap::GetRamUsedLen(uint32_t textManagerRamUsed, uint32_t langFontRamUsed)
{
    if (bitmapCache_ == nullptr) {
        uint8_t* bitmapCacheAddr = reinterpret_cast<uint8_t*>(GetRamAddr() + dynamicFontRamUsed_ + textManagerRamUsed);
        bitmapCache_ = new UIFontCache(bitmapCacheAddr, bitmapRamUsed_);
    }
    return dynamicFontRamUsed_ + textManagerRamUsed + bitmapRamUsed_ + langFontRamUsed;
}

int8_t UIFontBitmap::GetDynamicFontBitmap(uint32_t unicode, uint8_t* bitmap)
{
    return dynamicFont_.GetBitmap(unicode, bitmap);
}

uint8_t* UIFontBitmap::GetCacheBitmap(uint8_t fontId, uint32_t unicode)
{
    if (bitmapCache_ != nullptr) {
        return bitmapCache_->GetBitmap(fontId, unicode);
    }
    return nullptr;
}

uint8_t* UIFontBitmap::GetCacheSpace(uint8_t fontId, uint32_t unicode, uint32_t size)
{
    if (bitmapCache_ != nullptr) {
        return bitmapCache_->GetSpace(fontId, unicode, size);
    }
    return nullptr;
}

void UIFontBitmap::PutCacheSpace(uint8_t* addr)
{
    if (bitmapCache_ != nullptr) {
        bitmapCache_->PutSpace(addr);
    }
}

int8_t UIFontBitmap::SetDynamicFontId(uint8_t fontId)
{
    return dynamicFont_.SetCurrentFontId(fontId);
}

int16_t UIFontBitmap::GetDynamicFontWidth(uint32_t unicode, uint8_t fontId)
{
    int16_t ret = dynamicFont_.SetCurrentFontId(fontId);
    if (ret == INVALID_RET_VALUE) {
        return ret;
    }
    return dynamicFont_.GetFontWidth(unicode);
}

uint8_t* UIFontBitmap::SearchInFont(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId)
{
    if (!UIFontAdaptor::IsSameTTFId(fontId, unicode)) {
        return nullptr;
    }
    if (fontId != GetBaseFontId()) {
        SetCurrentFontId(fontId);
    }
    if (bitmapCache_ == nullptr) {
        return nullptr;
    }
    uint8_t* bitmap = bitmapCache_->GetBitmap(GetBaseFontId(), unicode);
    if (bitmap != nullptr) {
        GetGlyphNode(unicode, glyphNode);
        return bitmap;
    }

    int8_t ret = GetGlyphNode(unicode, glyphNode);
    if (ret != RET_VALUE_OK) {
        return nullptr;
    }

    if (glyphNode.kernOff <= glyphNode.dataOff) {
        return nullptr;
    }
    uint32_t bitmapSize = glyphNode.kernOff - glyphNode.dataOff;
    bitmap = bitmapCache_->GetSpace(GetBaseFontId(), unicode, bitmapSize);
    ret = dynamicFont_.GetBitmap(unicode, bitmap);
    if (ret == RET_VALUE_OK) {
        return bitmap;
    }
    bitmapCache_->PutSpace(bitmap);
    return nullptr;
}

int16_t UIFontBitmap::GetWidthInFontId(uint32_t unicode, uint8_t fontId)
{
    if (!UIFontAdaptor::IsSameTTFId(fontId, unicode)) {
        return INVALID_RET_VALUE;
    }
    if (fontId != GetBaseFontId()) {
        SetCurrentFontId(fontId);
    }
    return GetDynamicFontWidth(unicode, GetBaseFontId());
}
} // namespace
