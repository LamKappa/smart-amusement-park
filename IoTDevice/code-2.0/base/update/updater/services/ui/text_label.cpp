/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "text_label.h"
#include <cstdio>
#include <iostream>
#include <linux/input.h>
#include <string>
#include "log/log.h"
#include "png.h"
#include "securec.h"

namespace updater {
TextLable::TextLable(int mStartX, int mStartY, int w, int h, Frame *mparent)
{
    startX_ = mStartX;
    startY_ = mStartY;
    this->CreateBuffer(w, h, View::PixelFormat::BGRA888);
    parent_ = mparent;
    parent_->ViewRegister(this);

    SetFocusAble(true);
    outlineColor_.r = 0x00;
    outlineColor_.g = 0x00;
    outlineColor_.b = 0x00;
    outlineColor_.a = 0x00;
    boldTopLine_ = false;
    boldTopLine_ = false;

    const char midAlpha = 0xAA;
    actionBgColor_.r = 0x00;
    actionBgColor_.g = 0x00;
    actionBgColor_.b = 0x00;
    actionBgColor_.a = midAlpha;

    const char maxAlpha = 0xff;
    normalBgColor_.r = 0x00;
    normalBgColor_.g = 0x00;
    normalBgColor_.b = 0x00;
    normalBgColor_.a = maxAlpha;

    const char maxLevel = 0xff;
    textColor_.r = maxLevel;
    textColor_.g = maxLevel;
    textColor_.b = maxLevel;
    textColor_.a = maxLevel;

    (void)memset_s(textBuf_, MAX_TEXT_SIZE + 1, 0, MAX_TEXT_SIZE);
    InitFont();
}

void TextLable::SetFont(FontType fType)
{
    fontType_ = fType;
    InitFont();
    OnDraw();
}

static void PngInitSet(png_structp fontPngPtr, FILE *fp, int size, png_infop fontInfoPtr)
{
    png_init_io(fontPngPtr, fp);
    png_set_sig_bytes(fontPngPtr, size);
    png_read_info(fontPngPtr, fontInfoPtr);
    return;
}

static void PNGReadRow(png_uint_32 fontWidth, png_uint_32 fontHeight, png_structp fontPngPtr, char *fontBuf)
{
    if ((fontWidth > MAX_FONT_BUFFER_SIZE_HW) || (fontHeight > MAX_FONT_BUFFER_SIZE_HW)) {
        LOG(ERROR) << "font file size is too big!";
        return;
    }
    for (unsigned int y = 0; y < fontHeight; y++) {
        uint8_t* pRow = reinterpret_cast<uint8_t *>((fontBuf) + y * MAX_FONT_BUFFER_SIZE_HW);
        png_read_row(fontPngPtr, pRow, nullptr);
    }
    return;
}

void TextLable::InitFont()
{
    char resPath[MAX_TEXT_SIZE + 1];
    png_infop fontInfoPtr = nullptr;
    png_uint_32 fontWidth = 0;
    png_uint_32 fontHeight = 0;
    png_byte fontChannels = 0;
    png_structp fontPngPtr = nullptr;
    int fontBitDepth = 0;
    int fontColorType = 0;
    uint32_t offset = 2;
    UPDATER_CHECK_ONLY_RETURN(!memset_s(resPath, MAX_TEXT_SIZE + offset, 0, MAX_TEXT_SIZE + 1), return);
    switch (fontType_) {
        case DEFAULT_FONT:
            UPDATER_CHECK_ONLY_RETURN(snprintf_s(resPath, sizeof(resPath), sizeof(resPath) -1, "/resources/%s.png",
                DEFAULT_FONT_NAME.c_str()) != -1, return);
            break;
        default:
            UPDATER_CHECK_ONLY_RETURN(snprintf_s(resPath, sizeof(resPath), sizeof(resPath)-1, "/resources/%s.png",
                DEFAULT_FONT_NAME.c_str()) != -1, return);
            break;
    }
    FILE* fp = fopen(resPath, "rb");
    UPDATER_ERROR_CHECK(fp, "open font failed!", return);
    const int headerNumber = 8;
    uint8_t header[headerNumber];
    size_t bytesRead = fread(header, 1, sizeof(header), fp);
    UPDATER_ERROR_CHECK(bytesRead == sizeof(header), "read header failed!", return);
    if (png_sig_cmp(header, 0, sizeof(header))) {
        LOG(ERROR) << "cmp header failed!";
        return;
    }
    fontPngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    UPDATER_ERROR_CHECK(fontPngPtr, "creat font ptr_ failed!", return);
    fontInfoPtr = png_create_info_struct(fontPngPtr);
    if ((!fontInfoPtr) || (setjmp(png_jmpbuf(fontPngPtr)))) {
        return;
    }
    PngInitSet(fontPngPtr, fp, sizeof(header), fontInfoPtr);
    png_get_IHDR(fontPngPtr, fontInfoPtr, &fontWidth, &fontHeight, &fontBitDepth, &fontColorType,
        nullptr, nullptr, nullptr);
    fontChannels = png_get_channels(fontPngPtr, fontInfoPtr);
    const int defaultFontBitDepth = 8;
    if (fontBitDepth <= defaultFontBitDepth && fontChannels == 1 && fontColorType == PNG_COLOR_TYPE_GRAY) {
        png_set_expand_gray_1_2_4_to_8(fontPngPtr);
    }
    const int defaultFontWidth = 96;
    fontWidth_ = fontWidth / defaultFontWidth;
    fontHeight_ = fontHeight >> 1;
    PNGReadRow(fontWidth_, fontHeight_, fontPngPtr, fontBuf_);
}

void TextLable::SetText(const char *str)
{
    UPDATER_CHECK_ONLY_RETURN(!memset_s(textBuf_, MAX_TEXT_SIZE + 1, 0, MAX_TEXT_SIZE), return);
    OnDraw();
    UPDATER_CHECK_ONLY_RETURN(!memcpy_s(textBuf_, MAX_TEXT_SIZE + 1, str, strlen(const_cast<char*>(str))), return);
    OnDraw();
}

void TextLable::OnDraw()
{
    std::unique_lock<std::mutex> locker(mutex_);
    SyncBuffer();
    if (IsSelected()) {
        DrawFocus();
    }
    DrawOutline();
    DrawText();
    if (parent_ != nullptr) {
        parent_->OnDraw();
    }
}

void TextLable::SetOutLineBold(bool topBold, bool bottomBold)
{
    boldTopLine_ = topBold;
    boldBottomLine_ = bottomBold;
    OnDraw();
}

void TextLable::DrawOutline()
{
    void *tmpBuf = GetBuffer();
    auto *pixelBuf = static_cast<BRGA888Pixel*>(tmpBuf);
    for (int i = 0; i < viewWidth_; i++) {
        if (boldTopLine_) {
            pixelBuf[i].r = outlineColor_.r;
            pixelBuf[i].g = outlineColor_.g;
            pixelBuf[i].b = outlineColor_.b;
            pixelBuf[i].a = outlineColor_.a;

            pixelBuf[viewWidth_ + i].r = outlineColor_.r;
            pixelBuf[viewWidth_ + i].g = outlineColor_.g;
            pixelBuf[viewWidth_ + i].b = outlineColor_.b;
            pixelBuf[viewWidth_ + i].a = outlineColor_.a;
        }

        const int lines = 2;
        if (boldBottomLine_) {
            pixelBuf[(viewHeight_ - lines) * viewWidth_ + i].r = outlineColor_.r;
            pixelBuf[(viewHeight_ - lines) * viewWidth_ + i].g = outlineColor_.g;
            pixelBuf[(viewHeight_ - lines) * viewWidth_ + i].b = outlineColor_.b;
            pixelBuf[(viewHeight_ - lines) * viewWidth_ + i].a = outlineColor_.a;
            pixelBuf[(viewHeight_ - 1) * viewWidth_ + i].r = outlineColor_.r;
            pixelBuf[(viewHeight_ - 1) * viewWidth_ + i].g = outlineColor_.g;
            pixelBuf[(viewHeight_ - 1) * viewWidth_ + i].b = outlineColor_.b;
            pixelBuf[(viewHeight_ - 1) * viewWidth_ + i].a = outlineColor_.a;
        }
    }
}

void TextLable::SetTextColor(BRGA888Pixel color)
{
    textColor_.r = color.r;
    textColor_.g = color.g;
    textColor_.b = color.b;
    textColor_.a = color.a;
}

void TextLable::SetTextAlignmentMethod(AlignmentMethod methodH, AlignmentMethod methodV)
{
    fontAligMethodLevel_ = methodH;
    fontAligMethodUpright_ = methodV;
}

void TextLable::SetOnClickCallback(ClickCallback cb)
{
    callBack_ = cb;
}

void TextLable::DrawText()
{
    void *tmpBuf = GetBuffer();
    int textSx = 0;
    int textSy = 0;
    switch (fontAligMethodUpright_) {
        case ALIGN_CENTER:
            textSy = (viewHeight_ - fontHeight_) >> 1;
            break;
        case ALIGN_TO_TOP:
            textSy = 0;
            break;
        default:
            break;
    }

    const int minPosition = 10;
    const int average = 2;
    switch (fontAligMethodLevel_) {
        case ALIGN_CENTER:
            textSx = (viewWidth_ - (strlen(textBuf_) * fontWidth_)) / average;
            UPDATER_CHECK_ONLY_RETURN(textSx >= minPosition, textSx = minPosition);
            break;
        case ALIGN_TO_LEFT:
            textSx = minPosition;
            break;
        default:
            break;
    }
    unsigned char ch;
    char* s = textBuf_;
    while ((ch = *s++)) {
        UPDATER_CHECK_ONLY_RETURN(!(ch < ' ' || ch > '~'), ch = '?');
        auto *srcP = reinterpret_cast<uint8_t*>(static_cast<char*>(fontBuf_) + ((ch - ' ') * fontWidth_));
        auto *dstP = reinterpret_cast<BRGA888Pixel*>(static_cast<char*>(tmpBuf) + (textSy * viewWidth_ + textSx) *
            sizeof(BRGA888Pixel));
        for (unsigned int j = 0; j < fontHeight_; j++) {
            for (unsigned int i = 0; i < fontWidth_; i++) {
                uint8_t a = srcP[i];
                if (a > 0) {
                    dstP[i].r = textColor_.r;
                    dstP[i].g = textColor_.g;
                    dstP[i].b = textColor_.b;
                    dstP[i].a = textColor_.a;
                }
            }
            srcP += MAX_FONT_BUFFER_SIZE_HW;
            dstP = dstP + viewWidth_;
        }
        textSx += fontWidth_;
    }
}

void TextLable::DrawFocus()
{
    BRGA888Pixel pixBuf[viewWidth_];
    for (int a =0; a< viewWidth_; a++) {
        pixBuf[a].r = actionBgColor_.r;
        pixBuf[a].g = actionBgColor_.g;
        pixBuf[a].b = actionBgColor_.b;
        pixBuf[a].a = actionBgColor_.a;
    }
    void *viewBgBuf = GetBuffer();
    for (int i = 0; i < viewHeight_; i++) {
        UPDATER_CHECK_ONLY_RETURN(!memcpy_s(static_cast<char*>(static_cast<char*>(viewBgBuf) + i * viewWidth_ *
            sizeof(BRGA888Pixel)), viewWidth_ * sizeof(BRGA888Pixel) + 1, reinterpret_cast<char*>(pixBuf),
            viewWidth_ * sizeof(BRGA888Pixel)), return);
    }
}

void TextLable::OnKeyEvent(int key)
{
    LOG(INFO) << "OnKeyEvent !";
    switch (key) {
        case KEY_POWER:
            if (callBack_ != nullptr) {
                callBack_(GetViewId());
            }
            break;
        default:
            break;
    }
}
} // namespace updater
