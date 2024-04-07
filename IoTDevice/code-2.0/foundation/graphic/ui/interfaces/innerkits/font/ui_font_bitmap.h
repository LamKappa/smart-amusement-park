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

#ifndef UI_FONT_BITMAP_H
#define UI_FONT_BITMAP_H

#include "font/base_font.h"
#include "font/glyphs_manager.h"
#include "font/ui_font_cache.h"

namespace OHOS {
class UIFontBitmap : public BaseFont {
public:
    UIFontBitmap();
    virtual ~UIFontBitmap();
    UIFontBitmap(const UIFontBitmap&) = delete;
    UIFontBitmap& operator=(const UIFontBitmap&) noexcept = delete;
    bool IsVectorFont() const override;
    uint8_t GetShapingFontId(char* text, uint8_t& ttfId, uint32_t& script, uint8_t fontId, uint8_t size) const override;
    int8_t SetFontPath(const char* dpath, const char* spath) override;
    int8_t SetCurrentFontId(uint8_t fontId, uint8_t size = 0) override;
    uint16_t GetHeight() override;
    uint8_t GetFontId(const char* ttfName, uint8_t size = 0) const override;
    int16_t GetWidth(uint32_t unicode, uint8_t fontId) override;
    uint8_t* GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId) override;
    int8_t GetCurrentFontHeader(FontHeader& fontHeader) override;
    int8_t GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode) override;
    uint8_t GetFontWeight(uint8_t fontId) override;
    int8_t GetFontVersion(char* dVersion, uint8_t dLen, char* sVersion, uint8_t sLen) const override;
    int8_t SetCurrentLangId(uint8_t langId) override;
    UITextLanguageFontParam* GetFontInfo(uint8_t fontId) const override;

protected:
    uint32_t GetBitmapRamUsed();
    uint32_t GetDynamicFontRamUsed();
    uint32_t GetRamUsedLen(uint32_t textManagerRamUsed, uint32_t langFontRamUsed);
    int8_t GetDynamicFontBitmap(uint32_t unicode, uint8_t* bitmap);
    uint8_t* GetCacheBitmap(uint8_t fontId, uint32_t unicode);
    uint8_t* GetCacheSpace(uint8_t fontId, uint32_t unicode, uint32_t size);
    void PutCacheSpace(uint8_t* addr);
    int8_t SetDynamicFontId(uint8_t fontId);
    int16_t GetDynamicFontWidth(uint32_t unicode, uint8_t fontId);

private:
    uint8_t* SearchInFont(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId);
    int16_t GetWidthInFontId(uint32_t unicode, uint8_t fontId);

    static constexpr uint32_t FONT_BITMAP_CACHE_SIZE = 0x64000;
    static constexpr uint8_t FONT_ID_MAX = 0xFF;
    static constexpr uint8_t TTF_NAME_LEN_MAX = 128;
    GlyphsManager dynamicFont_;
    uint32_t dynamicFontRamUsed_;
    uint32_t bitmapRamUsed_;
    UIFontCache* bitmapCache_;
    int32_t dynamicFontFd_;
};
} // namespce OHOS
#endif // UI_FONT_BITMAP_H