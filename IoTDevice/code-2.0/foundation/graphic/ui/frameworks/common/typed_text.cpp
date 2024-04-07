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

#include "common/typed_text.h"
#include "font/ui_font.h"
#include "font/ui_font_adaptor.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/mem_api.h"
#include "gfx_utils/transform.h"

namespace OHOS {
#ifndef _FONT_TOOL
Point TypedText::GetTextSize(const char* text, int16_t letterSpace, int16_t lineSpace, int16_t maxWidth)
{
    Point size{0, 0};

    if (text == nullptr) {
        GRAPHIC_LOGE("TypedText::GetTextSize invalid parameter");
        return size;
    }

    uint32_t lineBegin = 0;
    uint32_t newLineBegin = 0;
    uint16_t letterHeight = UIFont::GetInstance()->GetHeight();

    while (text[lineBegin] != '\0') {
        int16_t lineWidth = maxWidth;
        newLineBegin += UIFontAdaptor::GetNextLineAndWidth(&text[lineBegin], letterSpace, lineWidth);
        if (newLineBegin == lineBegin) {
            break;
        }
        size.y += letterHeight + lineSpace;
        size.x = MATH_MAX(lineWidth, size.x);
        lineBegin = newLineBegin;
    }

    if ((lineBegin != 0) && ((text[lineBegin - 1] == '\n') || (text[lineBegin - 1] == '\r'))) {
        size.y += letterHeight + lineSpace;
    }

    if (size.y == 0) {
        size.y = letterHeight;
    } else {
        size.y -= lineSpace;
    }
    return size;
}

Rect TypedText::GetArcTextRect(const char* text,
                               const Point& arcCenter,
                               int16_t letterSpace,
                               UIArcLabel::TextOrientation orientation,
                               const UIArcLabel::ArcTextInfo& arcTextInfo)
{
    if ((text == nullptr) || (arcTextInfo.lineStart == arcTextInfo.lineEnd) || (arcTextInfo.radius == 0)) {
        GRAPHIC_LOGE("TypedText::GetArcTextRect invalid parameter\n");
        return Rect();
    }

    uint16_t letterHeight = UIFont::GetInstance()->GetHeight();
    bool xorFlag = (orientation == UIArcLabel::TextOrientation::INSIDE) ^ (arcTextInfo.direct == TEXT_DIRECT_LTR);
    float posX = 0;
    float posY = 0;
    uint32_t i = arcTextInfo.lineStart;
    float angle = arcTextInfo.startAngle;
    Rect rect;
    Rect rectLetter;
    TransformMap transform;
    while (i < arcTextInfo.lineEnd) {
        uint32_t tmp = i;
        uint32_t letter = GetUTF8Next(text, tmp, i);
        if (letter == 0) {
            continue;
        }
        if ((letter == '\r') || (letter == '\n')) {
            break;
        }
        uint16_t letterWidth = UIFont::GetInstance()->GetWidth(letter, 0);
        if (tmp == arcTextInfo.lineStart) {
            angle += xorFlag ? GetAngleForArcLen(static_cast<float>(letterWidth), letterHeight, arcTextInfo.radius,
                                                 arcTextInfo.direct, orientation)
                             : 0;
            GetArcLetterPos(arcCenter, arcTextInfo.radius, angle, posX, posY);
            rect.SetPosition(MATH_ROUND(posX), MATH_ROUND(posY));
        }
        rectLetter.SetPosition(MATH_ROUND(posX), MATH_ROUND(posY));
        rectLetter.Resize(letterWidth, letterHeight);
        transform.SetTransMapRect(rectLetter);

        uint16_t arcLen = letterWidth + letterSpace;
        if (arcLen == 0) {
            continue;
        }
        float incrementAngle = GetAngleForArcLen(static_cast<float>(arcLen), letterHeight, arcTextInfo.radius,
                                                 arcTextInfo.direct, orientation);
        float rotateAngle =
            (orientation == UIArcLabel::TextOrientation::INSIDE) ? angle : (angle - SEMICIRCLE_IN_DEGREE);
        // 2: letterWidth's half
        float fineTuningAngle = incrementAngle * (static_cast<float>(letterWidth) / (2 * arcLen));
        rotateAngle += (xorFlag ? -fineTuningAngle : fineTuningAngle);

        transform.Rotate(MATH_ROUND(rotateAngle), Vector2<float>(0, 0));
        rect.Join(rect, transform.GetBoxRect());

        angle += incrementAngle;
        GetArcLetterPos(arcCenter, arcTextInfo.radius, angle, posX, posY);
    }
    return rect;
}

float TypedText::GetAngleForArcLen(float len,
                                   uint16_t height,
                                   uint16_t radius,
                                   UITextLanguageDirect direct,
                                   UIArcLabel::TextOrientation orientation)
{
    if (radius == 0) {
        return 0;
    }
    float realRadius =
        static_cast<float>((orientation == UIArcLabel::TextOrientation::OUTSIDE) ? (radius + height) : radius);
    float angle = static_cast<float>(len * SEMICIRCLE_IN_DEGREE) / (UI_PI * realRadius);
    return (direct == TEXT_DIRECT_LTR) ? angle : -angle;
}

void TypedText::GetArcLetterPos(const Point& arcCenter, uint16_t radius, float angle, float& posX, float& posY)
{
    posX = arcCenter.x + (static_cast<float>(radius) * Sin(MATH_ROUND(angle)));
    posY = arcCenter.y - (static_cast<float>(radius) * Sin(MATH_ROUND(angle + QUARTER_IN_DEGREE)));
}

uint32_t TypedText::GetNextLine(const char* text, int16_t letterSpace, int16_t maxWidth)
{
    uint32_t index = 0;
    if ((text == nullptr) || (GetWrapPoint(text, index) &&
        (TypedText::GetTextWidth(text, index, letterSpace) <= maxWidth))) {
        return index;
    }
    uint32_t lastBreakPos = 0;
    int16_t curW;
    uint32_t tmp = 0;
    while (true) {
        curW = TypedText::GetTextWidth(text, index, letterSpace);
        if (curW > maxWidth) {
            index = lastBreakPos;
            if (lastBreakPos == 0) {
                curW = 0;
                uint32_t i = 0;
                uint32_t letter;
                uint16_t letterWidth;
                while (text[i] != '\0') {
                    tmp = i;
                    letter = TypedText::GetUTF8Next(text, tmp, i);
                    letterWidth = UIFont::GetInstance()->GetWidth(letter, 0);
                    curW += letterWidth;
                    if (letterWidth > 0) {
                        curW += letterSpace;
                    }
                    if (curW > maxWidth) {
                        index = lastBreakPos;
                        return index;
                    }
                    lastBreakPos = i;
                }
            }
            break;
        }
        if ((index > 0) && (index < strlen(text)) && ((text[index - 1] == '\r') || (text[index - 1] == '\n'))) {
            break;
        }
        lastBreakPos = index;
        if (text[index] == '\0') {
            break;
        }
        if (GetWrapPoint(text + index, tmp) && (TypedText::GetTextWidth(text, index + tmp, letterSpace) <= maxWidth)) {
            return index + tmp;
        }
        index += tmp;
        if (lastBreakPos == index) {
            break;
        }
    }
    return index;
}

bool TypedText::GetWrapPoint(const char* text, uint32_t& breakPoint)
{
    breakPoint = 0;
    uint32_t j = 0;
    uint32_t letter = 0;
    if (text == nullptr) {
        return true;
    }

    while (text[breakPoint] != '\0') {
        letter = GetUTF8Next(text, breakPoint, j);
        breakPoint = j;
        if ((letter == ' ') || (letter == '.') || (letter == ',') || (letter == '!') || (letter == '=')
            || (letter == '?')) {
            return false;
        }
        if (letter == '\n') {
            return true;
        }
        if ((letter == '\r') && (GetUTF8Next(text, breakPoint, j) == '\n')) {
            breakPoint = j;
            return true;
        }
    }
    return false;
}

int16_t TypedText::GetTextWidth(const char* text, uint16_t length, int16_t letterSpace)
{
    if ((text == nullptr) || (length == 0) || (length > strlen(text))) {
        GRAPHIC_LOGE("TypedText::GetTextWidth invalid parameter\n");
        return 0;
    }

    uint32_t i = 0;
    uint16_t width = 0;
    uint32_t letter;

    while (i < length) {
        letter = GetUTF8Next(text, i, i);
        if ((letter == 0) || (letter == '\n') || (letter == '\r')) {
            continue;
        }
        uint16_t charWidth = UIFont::GetInstance()->GetWidth(letter, 0);
        width += charWidth + letterSpace;
    }
    if (width > 0) {
        width -= letterSpace;
    }
    return width;
}
#endif // _FONT_TOOL

uint8_t TypedText::GetUTF8OneCharacterSize(const char* str)
{
    if (str == nullptr) {
        return 0;
    }
    if ((str[0] & 0x80) == 0) {
        return 1;
    } else if ((str[0] & 0xE0) == 0xC0) {
        return 2; // 2: 2 bytes
    } else if ((str[0] & 0xF0) == 0xE0) {
        return 3; // 3: 3 bytes
    } else if ((str[0] & 0xF8) == 0xF0) {
        return 4; // 4: 4 bytes
    }
    return 0;
}

uint32_t TypedText::GetUTF8Next(const char* text, uint32_t i, uint32_t& j)
{
    uint32_t unicode = 0;
    if (text == nullptr) {
        GRAPHIC_LOGE("text invalid parameter");
        return 0;
    }

    j = i;
    uint8_t lettetSize = GetUTF8OneCharacterSize(text + i);
    switch (lettetSize) {
        case 1:
            unicode = text[j];
            break;
        case 2: // 2: letter size
            unicode = static_cast<uint32_t>(text[j] & 0x1F) << UTF8_TO_UNICODE_SHIFT1;
            j++;
            if ((text[j] & 0xC0) != 0x80) {
                return 0;
            }
            unicode += (text[j] & 0x3F);
            break;
        case 3: // 3: letter size
            unicode = static_cast<uint32_t>(text[j] & 0x0F) << UTF8_TO_UNICODE_SHIFT2;
            unicode += static_cast<uint32_t>(text[++j] & 0x3F) << UTF8_TO_UNICODE_SHIFT1;
            unicode += (text[++j] & 0x3F);
            break;
        case 4: // 4: letter size
            unicode = static_cast<uint32_t>(text[j] & 0x07) << UTF8_TO_UNICODE_SHIFT3;
            unicode += static_cast<uint32_t>(text[++j] & 0x3F) << UTF8_TO_UNICODE_SHIFT2;
            unicode += static_cast<uint32_t>(text[++j] & 0x3F) << UTF8_TO_UNICODE_SHIFT1;
            unicode += text[++j] & 0x3F;
            break;
        default:
            break;
    }
    j++;
    return unicode;
}

uint32_t TypedText::GetByteIndexFromUTF8Id(const char* text, uint32_t utf8Id)
{
    if (text == nullptr) {
        GRAPHIC_LOGE("TypedText::GetByteIndexFromUTF8Id text invalid parameter\n");
        return 0;
    }
    uint32_t byteIndex = 0;
    for (uint32_t i = 0; i < utf8Id; i++) {
        byteIndex += GetUTF8OneCharacterSize(&text[byteIndex]);
    }

    return byteIndex;
}

uint32_t TypedText::GetUTF8CharacterSize(const char* text, uint32_t byteIndex)
{
    uint32_t i = 0;
    uint32_t size = 0;

    if (text == nullptr) {
        GRAPHIC_LOGE("TypedText::GetUTF8CharacterSize text invalid parameter\n");
        return 0;
    }
    while ((text[i] != '\0') && (i < byteIndex)) {
        GetUTF8Next(text, i, i);
        size++;
    }

    return size;
}

void TypedText::Utf8ToUtf16(const char* utf8Str, uint16_t* utf16Str, uint32_t len)
{
    if ((utf8Str == nullptr) || (utf16Str == nullptr)) {
        GRAPHIC_LOGE("utf8Str or u16Str is null");
        return;
    }

    uint32_t i = 0;
    uint32_t cnt = 0;
    while (utf8Str[i] != '\0') {
        uint32_t unicode = GetUTF8Next(utf8Str, i, i);
        if (cnt < len) {
            if (unicode <= MAX_UINT16_LOW_SCOPE) {
                utf16Str[cnt] = (unicode & MAX_UINT16_LOW_SCOPE);
            } else if (unicode <= MAX_UINT16_HIGH_SCOPE) {
                if (cnt + 1 < len) {
                    utf16Str[cnt] = static_cast<uint16_t>(UTF16_LOW_PARAM + (unicode & UTF16_LOW_MASK)); // low
                    cnt++;
                    utf16Str[cnt] = static_cast<uint16_t>(UTF16_HIGH_PARAM1 + (unicode >> UTF16_HIGH_SHIFT) -
                                                          UTF16_HIGH_PARAM2); // high
                }
            } else {
                GRAPHIC_LOGE("Invalid unicode");
                return;
            }
            cnt++;
        }
    }
}

uint32_t TypedText::GetUtf16Cnt(const char* utf8Str)
{
    if (utf8Str == nullptr) {
        GRAPHIC_LOGE("text invalid parameter");
        return 0;
    }
    uint32_t len = 0;
    uint32_t i = 0;

    while (utf8Str[i] != '\0') {
        uint32_t unicode = GetUTF8Next(utf8Str, i, i);
        if (unicode <= MAX_UINT16_LOW_SCOPE) {
            len++;
        } else if (unicode <= MAX_UINT16_HIGH_SCOPE) {
            len += 2; // 2: low and high, two uint16_t numbers
        } else {
            GRAPHIC_LOGE("Invalid unicode");
            return 0;
        }
    }
    return len;
}
} // namespace OHOS
