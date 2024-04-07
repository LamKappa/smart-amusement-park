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

#include "components/ui_toggle_button.h"
#include "common/image.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "imgdecode/cache_manager.h"

namespace OHOS {
UIToggleButton::UIToggleButton() : corner_(DEFAULT_CORNER_RADIUS), radius_(DEFAULT_CORNER_RADIUS - DEAFULT_RADIUS_DIFF),
                                   rectWidth_(DEFAULT_WIDTH)
{
    image_[UNSELECTED].SetSrc("");
    image_[SELECTED].SetSrc("");
    Resize(width_, height_);
}

void UIToggleButton::SetState(bool state)
{
    if (state) {
        UICheckBox::SetState(SELECTED);
    } else {
        UICheckBox::SetState(UNSELECTED);
    }
    Invalidate();
}

void UIToggleButton::CalculateSize()
{
    int16_t width = GetWidth();
    int16_t height = GetHeight();
    if ((width_ == width) && (height_ == height)) {
        return;
    }
    width_ = width;
    height_ = height;
    int16_t minValue = (width_ > height_) ? height_ : width_;
    corner_ = DEFAULT_CORNER_RADIUS * minValue / DEFAULT_HOT_HEIGHT;
    int16_t radiusDiff = DEAFULT_RADIUS_DIFF * minValue / DEFAULT_HOT_WIDTH;
    radius_ = corner_ - radiusDiff;
    rectWidth_ = DEFAULT_WIDTH * minValue / DEFAULT_HOT_WIDTH;
}

void UIToggleButton::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if ((image_[SELECTED].GetSrcType() != IMG_SRC_UNKNOWN) && (image_[UNSELECTED].GetSrcType() != IMG_SRC_UNKNOWN)) {
        UICheckBox::OnDraw(gfxDstBuffer, invalidatedArea);
    } else {
        CalculateSize();
        BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetRect(), invalidatedArea, *style_, opaScale_);
        Rect contentRect = GetContentRect();
        int16_t dx = (width_ - rectWidth_) >> 1;
        int16_t dy = (height_ >> 1) - corner_;
        int16_t x = contentRect.GetX() + dx;
        int16_t y = contentRect.GetY() + dy;
        Rect rectMid;
        rectMid.SetRect(x, y, x + rectWidth_, y + (corner_ << 1) + 1);
        Rect trunc = invalidatedArea;
        bool isIntersect = trunc.Intersect(trunc, contentRect);
        if (isIntersect == false) {
            return;
        }
        switch (state_) {
            case SELECTED: {
                Style styleSelect = StyleDefault::GetBackgroundTransparentStyle();
                styleSelect.borderRadius_ = corner_;
                styleSelect.bgColor_ = Color::GetColorFromRGB(DEFAULT_BG_RED, DEFAULT_BG_GREEN, DEFAULT_BG_BLUE);
                styleSelect.bgOpa_ = OPA_OPAQUE;
                BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rectMid, trunc, styleSelect, opaScale_);
                ArcInfo arcInfoRight = {
                    { static_cast<int16_t>(x + rectWidth_ - corner_), static_cast<int16_t>(y + corner_) }, { 0 },
                    radius_, 0, CIRCLE_IN_DEGREE, nullptr
                };
                styleSelect.bgColor_ = Color::White();
                styleSelect.lineWidth_ = radius_;
                styleSelect.lineColor_ = Color::White();
                BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfoRight, trunc,
                                                      styleSelect, OPA_OPAQUE, CapType::CAP_NONE);
                break;
            }
            case UNSELECTED: {
                Style styleUnSelect = StyleDefault::GetBackgroundTransparentStyle();
                styleUnSelect.bgColor_ = Color::White();
                styleUnSelect.bgOpa_ = DEFAULT_UNSELECTED_OPA;
                styleUnSelect.borderRadius_ = corner_;
                BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rectMid, trunc, styleUnSelect, opaScale_);
                ArcInfo arcInfoLeft = {
                    { static_cast<int16_t>(x + corner_), static_cast<int16_t>(y + corner_) }, { 0 }, radius_, 0,
                    CIRCLE_IN_DEGREE, nullptr
                };
                styleUnSelect.lineColor_ = Color::White();
                styleUnSelect.lineWidth_ = radius_;
                BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfoLeft, trunc,
                                                      styleUnSelect, OPA_OPAQUE, CapType::CAP_NONE);
                break;
            }
            default:
                break;
        }
    }
}
}   // namespace OHOS