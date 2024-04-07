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

#ifndef BASE_FONT
#define BASE_FONT
#include "font/ui_font_header.h"
#include "font/ui_font_builder.h"
#include "graphic_config.h"

namespace OHOS {
class BaseFont : public HeapBase {
public:
    BaseFont() : fontId_(0), ramAddr_(0), ramLen_(0) {}
    virtual ~BaseFont() {}

    /**
     * @brief Indicates whether the current font library is a vector font library.
     * @return uint8_t: 0 BitmapFont  1 VectorFont
     */
    virtual bool IsVectorFont() const = 0;

    /**
     * @brief Set font by id
     *
     * @param fontid [in] the font id
     * @param size [in] the font size
     * @return int8_t: -1 failed, 0 success
     */
    virtual int8_t SetCurrentFontId(uint8_t fontId, uint8_t size) = 0;

    /**
     * @brief Get height for specific font
     *
     * @return uint16_t
     */
    virtual uint16_t GetHeight() = 0;

    /**
     * @brief Get font id
     *
     * @param ttfName
     * @param size   0: invaild size
     * @return uint8_t
     */
    virtual uint8_t GetFontId(const char* ttfName, uint8_t size) const = 0;

    /**
     * @brief Get width
     *
     * @param unicode
     * @return int16_t
     */
    virtual int16_t GetWidth(uint32_t unicode, uint8_t fontId) = 0;

    virtual int32_t OpenVectorFont(uint8_t ttfId)
    {
        return -1;
    }
    /**
     * @brief Get bitmap for specific unicode
     *
     * @param unicode
     * @return uint8_t*
     */
    virtual uint8_t* GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId) = 0;

    /**
     * @brief Get font header
     *
     * @param fontHeader
     * @return int8_t
     */
    virtual int8_t GetCurrentFontHeader(FontHeader& fontHeader) = 0;

    /**
     * @brief Get the glyph node
     *
     * @param unicode
     * @param glyphNode
     * @param isGlyph
     * @return int8_t
     */
    virtual int8_t GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode) = 0;
    virtual uint8_t GetFontWeight(uint8_t fontId) = 0;

    virtual int8_t SetCurrentLangId(uint8_t langId)
    {
        return 0;
    }

    virtual uint8_t GetCurrentLangId() const
    {
        return UIFontBuilder::GetInstance()->GetTotalLangId();
    }

    int8_t GetDefaultParamByLangId(uint8_t langId, LangTextParam** pParam) const;

    /**
     * @brief Get the Font Shaping Property
     * @param text [in] the content
     * @param ttfId [out] the ttf id
     * @param fontId [in] the font id
     * @param size [in] the font size
     * @return uint8_t: needShaping property
     */
    virtual uint8_t GetShapingFontId(char* text, uint8_t& ttfId, uint32_t& script, uint8_t fontId, uint8_t size) const
    {
        return 0;
    }

    /**
     * @brief Set the Font Path
     *
     * @param dpath
     * @param spath
     * @return int8_t
     */
    virtual int8_t SetFontPath(const char* dpath, const char* spath) = 0;

    virtual int8_t GetFontVersion(char* dVersion, uint8_t dLen, char* sVersion, uint8_t sLen) const
    {
        return INVALID_RET_VALUE;
    }

    /**
     * @brief Get the text in utf-8 format
     *
     * @param textId
     * @param utf8Addr
     * @param utf8Len
     * @return int8_t
     */
    virtual int8_t GetTextUtf8(uint16_t textId, uint8_t** utf8Addr, uint16_t& utf8Len) const
    {
        return 0;
    }

    /**
     * @brief Get the ttfId
     * @param fontId [in] the font id
     * @param size [in] the font size
     * @return uint8_t: ttfId property
     */
    virtual uint8_t GetFontTtfId(uint8_t fontId, uint8_t size) const
    {
        return 0;
    }

    virtual const UITextLanguageFontParam* GetFontInfo(uint8_t fontId) const
    {
        return nullptr;
    }

    virtual uint8_t RegisterFontInfo(const char* ttfName, uint8_t shaping)
    {
        return 0;
    }

    virtual uint8_t RegisterFontInfo(const UITextLanguageFontParam* fontsTable, uint8_t num)
    {
        return 0;
    }

    virtual uint8_t UnregisterFontInfo(const char* ttfName)
    {
        return 0;
    }

    virtual uint8_t UnregisterFontInfo(const UITextLanguageFontParam* fontsTable, uint8_t num)
    {
        return 0;
    }

    virtual int8_t GetWildCardStaticStr(uint16_t textId,
                                        UITextWildcardStaticType type,
                                        uint8_t** strAddr,
                                        uint16_t& strLen) const
    {
        return 0;
    }

    virtual int8_t GetCodePoints(uint16_t textId, uint32_t** codePoints, uint16_t& codePointsNum) const
    {
        return 0;
    }

    virtual int8_t GetTextParam(uint16_t textId, UITextLanguageTextParam& param) const
    {
        return 0;
    }

    /**
     * @brief Get current font id
     *
     * @return uint8_t
     */
    void SetBaseFontId(uint8_t fontId);
    uint8_t GetBaseFontId();
    void SetRamAddr(uintptr_t ramAddr);
    uintptr_t GetRamAddr();
    uint32_t GetRamLen();
    void SetRamLen(uint32_t ramLen);
    void SetPsramMemory(uintptr_t psramAddr, uint32_t psramLen);

private:
    uint8_t fontId_;
    uintptr_t ramAddr_;
    uint32_t ramLen_;
};
} // namespace OHOS
#endif /* UI_BASE_FONT_H */
