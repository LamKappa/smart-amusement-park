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

#include "draw/draw_label.h"
#include <cstdio>
#include "common/typed_text.h"
#include "draw/draw_utils.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "font/ui_font.h"
#include "font/ui_font_header.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
void DrawLabel::DrawTextOneLine(BufferInfo& gfxDstBuffer, const LabelLineInfo& labelLine)
{
    UIFont* fontEngine = UIFont::GetInstance();
    if (labelLine.direct == TEXT_DIRECT_RTL) {
        labelLine.pos.x -= labelLine.offset.x;
    } else {
        labelLine.pos.x += labelLine.offset.x;
    }

    uint32_t i = 0;
    uint32_t letter;
    uint16_t letterWidth;
    while (i < labelLine.lineLength) {
        letter = TypedText::GetUTF8Next(labelLine.text, i, i);

        LabelLetterInfo letterInfo{labelLine.pos,
                                   labelLine.mask,
                                   labelLine.style.textColor_,
                                   labelLine.opaScale,
                                   0,
                                   0,
                                   letter,
                                   labelLine.direct,
                                   labelLine.fontId,
                                   0,
                                   labelLine.fontSize};
        DrawUtils::GetInstance()->DrawLetter(gfxDstBuffer, letterInfo);
        letterWidth = fontEngine->GetWidth(letter, 0);
        if (labelLine.direct == TEXT_DIRECT_RTL) {
            labelLine.pos.x -= (letterWidth + labelLine.style.letterSpace_);
        } else {
            labelLine.pos.x += (letterWidth + labelLine.style.letterSpace_);
        }
    }
}

void DrawLabel::DrawArcText(BufferInfo& gfxDstBuffer,
                            const Rect& mask,
                            const char* text,
                            const Point& arcCenter,
                            uint8_t fontId,
                            const UIArcLabel::ArcTextInfo arcTextInfo,
                            UIArcLabel::TextOrientation orientation,
                            const Style& style,
                            OpacityType opaScale)
{
    if ((text == nullptr) || (arcTextInfo.lineStart == arcTextInfo.lineEnd) || (arcTextInfo.radius == 0)) {
        GRAPHIC_LOGE("DrawLabel::DrawArcText invalid parameter\n");
        return;
    }
    OpacityType opa = DrawUtils::GetMixOpacity(opaScale, style.textOpa_);
    if (opa == OPA_TRANSPARENT) {
        return;
    }
    uint16_t letterWidth;
    uint16_t letterHeight = UIFont::GetInstance()->GetHeight();
    uint32_t i = arcTextInfo.lineStart;
    float angle = arcTextInfo.startAngle;
    float posX;
    float posY;
    float rotateAngle;
    bool xorFlag = (orientation == UIArcLabel::TextOrientation::INSIDE) ^ (arcTextInfo.direct == TEXT_DIRECT_LTR);

    while (i < arcTextInfo.lineEnd) {
        uint32_t tmp = i;
        uint32_t letter = TypedText::GetUTF8Next(text, tmp, i);
        if (letter == 0) {
            continue;
        }
        if ((letter == '\r') || (letter == '\n')) {
            break;
        }
        letterWidth = UIFont::GetInstance()->GetWidth(letter, 0);
        if ((tmp == arcTextInfo.lineStart) && xorFlag) {
            angle += TypedText::GetAngleForArcLen(static_cast<float>(letterWidth), letterHeight, arcTextInfo.radius,
                                                  arcTextInfo.direct, orientation);
        }
        uint16_t arcLen = letterWidth + style.letterSpace_;
        if (arcLen == 0) {
            continue;
        }
        float incrementAngle = TypedText::GetAngleForArcLen(static_cast<float>(arcLen), letterHeight,
                                                            arcTextInfo.radius, arcTextInfo.direct, orientation);

        rotateAngle = (orientation == UIArcLabel::TextOrientation::INSIDE) ? angle : (angle - SEMICIRCLE_IN_DEGREE);

        // 2: half
        float fineTuningAngle = incrementAngle * (static_cast<float>(letterWidth) / (2 * arcLen));
        rotateAngle += (xorFlag ? -fineTuningAngle : fineTuningAngle);
        TypedText::GetArcLetterPos(arcCenter, arcTextInfo.radius, angle, posX, posY);
        angle += incrementAngle;

        DrawLetterWithRotate(gfxDstBuffer, mask, fontId, letter, Point { MATH_ROUND(posX), MATH_ROUND(posY) },
                             static_cast<int16_t>(rotateAngle), style.textColor_, opaScale);
    }
}

void DrawLabel::DrawLetterWithRotate(BufferInfo& gfxDstBuffer,
                                     const Rect& mask,
                                     uint8_t fontId,
                                     uint32_t letter,
                                     const Point& pos,
                                     int16_t rotateAngle,
                                     const ColorType& color,
                                     OpacityType opaScale)
{
    UIFont* fontEngine = UIFont::GetInstance();
    FontHeader head;
    GlyphNode node;
    if (fontEngine->GetCurrentFontHeader(head) != 0) {
        return;
    }

    const uint8_t* fontMap = fontEngine->GetBitmap(letter, node, 0);
    if (fontMap == nullptr) {
        return;
    }
    uint8_t fontWeight = fontEngine->GetFontWeight(fontId);
    ColorMode colorMode;
    switch (fontWeight) {
        case FONT_WEIGHT_1:
            colorMode = A1;
            break;
        case FONT_WEIGHT_2:
            colorMode = A2;
            break;
        case FONT_WEIGHT_4:
            colorMode = A4;
            break;
        case FONT_WEIGHT_8:
            colorMode = A8;
            break;
        default:
            return;
    }
    Rect rectLetter;
    rectLetter.SetPosition(pos.x + node.left, pos.y + head.ascender - node.top);
    rectLetter.Resize(node.cols, node.rows);
    TransformMap transMap(rectLetter);
    transMap.Rotate(rotateAngle, Vector2<float>(-node.left, node.top - head.ascender));
    TransformDataInfo letterTranDataInfo = {ImageHeader{colorMode, 0, 0, 0, node.cols, node.rows}, fontMap, fontWeight,
                                            BlurLevel::LEVEL0};
    BaseGfxEngine::GetInstance()->DrawTransform(gfxDstBuffer, mask, Point { 0, 0 }, color, opaScale, transMap,
                                                letterTranDataInfo);
}
} // namespace OHOS
