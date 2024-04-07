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

#include "common/text.h"
#include "common/typed_text.h"
#include "draw/draw_label.h"
#include "font/ui_font.h"
#include "font/ui_font_adaptor.h"
#include "font/ui_font_builder.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"

namespace OHOS {
Text::TextLine Text::textLine_[MAX_LINE_COUNT] = {{0}};

Text::Text()
    : text_(nullptr),
      fontId_(0),
      fontSize_(0),
      textSize_({0, 0}),
      needRefresh_(false),
      expandWidth_(false),
      expandHeight_(false),
      direct_(TEXT_DIRECT_LTR),
      horizontalAlign_(TEXT_ALIGNMENT_LEFT),
      verticalAlign_(TEXT_ALIGNMENT_TOP)
{
    SetFont(DEFAULT_VECTOR_FONT_FILENAME, DEFAULT_VECTOR_FONT_SIZE);
}

Text::~Text()
{
    if (text_ != nullptr) {
        UIFree(text_);
        text_ = nullptr;
    }
}

void Text::SetText(const char* text)
{
    if (text == nullptr) {
        return;
    }
    uint32_t textLen = static_cast<uint32_t>(strlen(text));
    if (textLen > MAX_TEXT_LENGTH) {
        return;
    }
    if (text_ != nullptr) {
        if (strcmp(text, text_) == 0) {
            return;
        }
        UIFree(text_);
        text_ = nullptr;
    }
    text_ = static_cast<char*>(UIMalloc(++textLen));
    if (text_ == nullptr) {
        return;
    }
    if (memcpy_s(text_, textLen, text, textLen) != EOK) {
        UIFree(text_);
        text_ = nullptr;
        return;
    }
    needRefresh_ = true;
}

void Text::SetFont(const char* name, uint8_t size)
{
    if (name == nullptr) {
        return;
    }
    if (UIFont::GetInstance()->IsVectorFont()) {
        uint8_t fontId = UIFont::GetInstance()->GetFontId(name);
        if ((fontId != UIFontBuilder::GetInstance()->GetTotalFontId()) &&
            ((fontId_ != fontId) || (fontSize_ != size))) {
            fontId_ = fontId;
            fontSize_ = size;
            needRefresh_ = true;
        }
    } else {
        uint8_t fontId = UIFont::GetInstance()->GetFontId(name, size);
        SetFontId(fontId);
    }
}

void Text::SetFont(const char* name, uint8_t size, char*& destName, uint8_t& destSize)
{
    if (name == nullptr) {
        return;
    }
    uint32_t nameLen = static_cast<uint32_t>(strlen(name));
    if (nameLen > MAX_TEXT_LENGTH) {
        return;
    }
    if (destName != nullptr) {
        if (strcmp(destName, name) == 0) {
            destSize = size;
            return;
        }
        UIFree(destName);
        destName = nullptr;
    }
    if (nameLen != 0) {
        /* one more to store '\0' */
        destName = static_cast<char*>(UIMalloc(++nameLen));
        if (destName == nullptr) {
            return;
        }
        if (memcpy_s(destName, nameLen, name, nameLen) != EOK) {
            UIFree(destName);
            destName = nullptr;
            return;
        }
        destSize = size;
    }
}

void Text::SetFontId(uint8_t fontId)
{
    if ((fontId >= UIFontBuilder::GetInstance()->GetTotalFontId()) || (fontId_ == fontId)) {
        GRAPHIC_LOGE("Text::SetFontId invalid fontId(%d)", fontId);
        return;
    }
    UITextLanguageFontParam* fontParam = UIFontBuilder::GetInstance()->GetTextLangFontsTable(fontId);
    if (fontParam == nullptr) {
        return;
    }
    if (UIFont::GetInstance()->IsVectorFont()) {
        uint8_t fontId = UIFont::GetInstance()->GetFontId(fontParam->ttfName);
        if ((fontId != UIFontBuilder::GetInstance()->GetTotalFontId()) && ((fontId_ != fontId) ||
            (fontSize_ != fontParam->size))) {
            fontId_ = fontId;
            fontSize_ = fontParam->size;
            needRefresh_ = true;
        }
    } else {
        fontId_ = fontId;
        fontSize_ = fontParam->size;
        needRefresh_ = true;
    }
}

void Text::ReMeasureTextSize(const Rect& textRect, const Style& style)
{
    if (fontSize_ == 0) {
        return;
    }
    UIFont::GetInstance()->SetCurrentFontId(fontId_, fontSize_);
    int16_t maxWidth = (expandWidth_ ? COORD_MAX : textRect.GetWidth());
    if (maxWidth > 0) {
        textSize_ = TypedText::GetTextSize(text_, style.letterSpace_, style.lineSpace_, maxWidth);
        FontHeader head;
        if (UIFont::GetInstance()->GetCurrentFontHeader(head) != 0) {
            return;
        }
        textSize_.y += fontSize_ - head.ascender;
    }
}

void Text::ReMeasureTextWidthInEllipsisMode(const Rect& textRect, const Style& style, uint16_t ellipsisIndex)
{
    if (ellipsisIndex != TEXT_ELLIPSIS_END_INV) {
        int16_t lineMaxWidth  = expandWidth_ ? textSize_.x : textRect.GetWidth();
        uint32_t maxLineBytes = 0;
        uint16_t lineCount = GetLine(lineMaxWidth, style.letterSpace_, ellipsisIndex, maxLineBytes);
        if ((lineCount > 0) && (textSize_.x < textLine_[lineCount - 1].linePixelWidth)) {
            textSize_.x = textLine_[lineCount - 1].linePixelWidth;
        }
    }
}
void Text::OnDraw(BufferInfo& gfxDstBuffer,
                  const Rect& invalidatedArea,
                  const Rect& viewOrigRect,
                  const Rect& textRect,
                  int16_t offsetX,
                  const Style& style,
                  uint16_t ellipsisIndex,
                  OpacityType opaScale)
{
    if ((text_ == nullptr) || (strlen(text_) == 0) || (fontSize_ == 0)) {
        return;
    }
    UIFont::GetInstance()->SetCurrentFontId(fontId_, fontSize_);
    Rect mask = invalidatedArea;

    if (mask.Intersect(mask, textRect)) {
        Draw(gfxDstBuffer, mask, textRect, style, offsetX, ellipsisIndex, opaScale);
    }
}

void Text::Draw(BufferInfo& gfxDstBuffer,
                const Rect& mask,
                const Rect& coords,
                const Style& style,
                int16_t offsetX,
                uint16_t ellipsisIndex,
                OpacityType opaScale)
{
    Point offset = {offsetX, 0};
    int16_t lineMaxWidth = expandWidth_ ? textSize_.x : coords.GetWidth();
    int16_t lineHeight = UIFont::GetInstance()->GetHeight() + style.lineSpace_;
    uint16_t lineBegin = 0;
    uint32_t maxLineBytes = 0;
    uint16_t lineCount = GetLine(lineMaxWidth, style.letterSpace_, ellipsisIndex, maxLineBytes);
    Point pos;
    pos.y = TextPositionY(coords, (lineCount * lineHeight - style.lineSpace_));
    OpacityType opa = DrawUtils::GetMixOpacity(opaScale, style.textOpa_);
    for (int i = 0; i < lineCount; i++) {
        if (pos.y > mask.GetBottom()) {
            return;
        }
        int16_t nextLine = pos.y + lineHeight;
        if (nextLine >= mask.GetTop()) {
            pos.x = LineStartPos(coords, textLine_[i].linePixelWidth);
            LabelLineInfo labelLine{pos,
                                    offset,
                                    mask,
                                    lineHeight,
                                    textLine_[i].lineBytes,
                                    0,
                                    opa,
                                    style,
                                    &text_[lineBegin],
                                    textLine_[i].lineBytes,
                                    lineBegin,
                                    fontId_,
                                    fontSize_,
                                    0,
                                    static_cast<UITextLanguageDirect>(direct_),
                                    nullptr};
            DrawLabel::DrawTextOneLine(gfxDstBuffer, labelLine);
            if ((i == (lineCount - 1)) && (ellipsisIndex != TEXT_ELLIPSIS_END_INV)) {
                labelLine.offset.x = 0;
                labelLine.text = TEXT_ELLIPSIS;
                labelLine.lineLength = TEXT_ELLIPSIS_DOT_NUM;
                labelLine.length = TEXT_ELLIPSIS_DOT_NUM;
                DrawLabel::DrawTextOneLine(gfxDstBuffer, labelLine);
            }
        }
        lineBegin += textLine_[i].lineBytes;
        pos.y = nextLine;
    }
}

int16_t Text::TextPositionY(const Rect& textRect, int16_t textHeight)
{
    int16_t yOffset = 0;
    if (!expandHeight_ && (verticalAlign_ != TEXT_ALIGNMENT_TOP) && (textRect.GetHeight() > textHeight)) {
        if (verticalAlign_ == TEXT_ALIGNMENT_CENTER) {
            yOffset = (textRect.GetHeight() - textHeight) >> 1;
        } else if (verticalAlign_ == TEXT_ALIGNMENT_BOTTOM) {
            yOffset = textRect.GetHeight() - textHeight;
        }
    }
    return textRect.GetY() + yOffset;
}

int16_t Text::LineStartPos(const Rect& textRect, uint16_t lineWidth)
{
    int16_t xOffset = 0;
    int16_t rectWidth = textRect.GetWidth();
    if (horizontalAlign_ == TEXT_ALIGNMENT_CENTER) {
        xOffset = (direct_ == TEXT_DIRECT_RTL) ? ((rectWidth + lineWidth + 1) >> 1) : ((rectWidth - lineWidth) >> 1);
    } else if (horizontalAlign_ == TEXT_ALIGNMENT_RIGHT) {
        xOffset = (direct_ == TEXT_DIRECT_RTL) ? rectWidth : (rectWidth - lineWidth);
    } else {
        xOffset = (direct_ == TEXT_DIRECT_RTL) ? lineWidth : 0;
    }
    return textRect.GetX() + xOffset;
}

uint16_t Text::GetLine(int16_t width, uint8_t letterSpace, uint16_t ellipsisIndex, uint32_t& maxLineBytes)
{
    if (text_ == nullptr) {
        return 0;
    }
    uint16_t lineNum = 0;
    uint32_t textLen = GetTextStrLen();
    if ((ellipsisIndex != TEXT_ELLIPSIS_END_INV) && (ellipsisIndex < textLen)) {
        textLen = ellipsisIndex;
    }
    uint32_t begin = 0;
    while ((begin < textLen) && (text_[begin] != '\0') && (lineNum < MAX_LINE_COUNT)) {
        begin += GetTextLine(begin, textLen, width, lineNum, letterSpace);
        if (maxLineBytes < textLine_[lineNum].lineBytes) {
            maxLineBytes = textLine_[lineNum].lineBytes;
        }
        lineNum++;
    }
    if ((lineNum != 0) && (ellipsisIndex != TEXT_ELLIPSIS_END_INV)) {
        uint16_t ellipsisWidth = UIFont::GetInstance()->GetWidth('.', 0) + letterSpace;
        textLine_[lineNum - 1].linePixelWidth += ellipsisWidth * TEXT_ELLIPSIS_DOT_NUM;
    }
    return lineNum;
}

uint32_t Text::GetTextStrLen()
{
    return strlen(text_);
}

uint32_t Text::GetTextLine(uint32_t begin, uint32_t textLen, int16_t width, uint16_t lineNum, uint8_t letterSpace)
{
    int16_t lineWidth = width;
    uint16_t nextLineBytes = UIFontAdaptor::GetNextLineAndWidth(&text_[begin], letterSpace, lineWidth, false,
                                                                textLen - begin);
    if (nextLineBytes + begin > textLen) {
        nextLineBytes = textLen - begin;
    }
    textLine_[lineNum].lineBytes = nextLineBytes;
    textLine_[lineNum].linePixelWidth = lineWidth;
    return nextLineBytes;
}

uint16_t Text::GetEllipsisIndex(const Rect& textRect, const Style& style)
{
    if ((textSize_.y <= textRect.GetHeight()) || (TypedText::GetUTF8CharacterSize(text_) <= TEXT_ELLIPSIS_DOT_NUM)) {
        return TEXT_ELLIPSIS_END_INV;
    }
    UIFont* fontEngine = UIFont::GetInstance();
    fontEngine->SetCurrentFontId(fontId_, fontSize_);
    int16_t letterWidth = fontEngine->GetWidth('.', 0) + style.letterSpace_;
    Point p;
    p.x = textRect.GetWidth() - letterWidth * TEXT_ELLIPSIS_DOT_NUM;
    p.y = textRect.GetHeight();
    int16_t height = fontEngine->GetHeight() + style.lineSpace_;
    if (height) {
        p.y -= p.y % height;
    }

    p.y -= style.lineSpace_;
    return GetLetterIndexByPosition(textRect, style, p);
}

uint16_t Text::GetLetterIndexByPosition(const Rect& textRect, const Style& style, const Point& pos)
{
    if (text_ == nullptr) {
        return 0;
    }
    uint32_t lineStart = 0;
    uint32_t nextLineStart = 0;
    uint16_t letterHeight = UIFont::GetInstance()->GetHeight();
    int16_t y = 0;
    uint32_t textLen = static_cast<uint32_t>(strlen(text_));
    int16_t width = 0;
    while ((lineStart < textLen) && (text_[lineStart] != '\0')) {
        width = textRect.GetWidth();
        nextLineStart += UIFontAdaptor::GetNextLineAndWidth(&text_[lineStart], style.letterSpace_, width);
        if (nextLineStart == 0) {
            break;
        }
        if (pos.y <= y + letterHeight) {
            break;
        }
        y += letterHeight + style.lineSpace_;
        lineStart = nextLineStart;
    }
    /* Calculate the x coordinate */
    width = pos.x;
    lineStart += UIFontAdaptor::GetNextLineAndWidth(&text_[lineStart], style.letterSpace_, width, true);
    return (lineStart < textLen) ? lineStart : TEXT_ELLIPSIS_END_INV;
}
} // namespace OHOS
