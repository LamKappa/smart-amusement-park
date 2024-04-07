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

#include "components/ui_radio_button.h"
#include "common/image.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "draw/draw_image.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/graphic_log.h"
#include "imgdecode/cache_manager.h"
#include "securec.h"

namespace OHOS {
UIRadioButton::UIRadioButton() : name_(nullptr), radiusBig_(DEFAULT_RADIUS_BIG), radiusSmall_(DEFAULT_RADIUS_SMALL),
                                 lineWidth_(DEFAULT_LINE_WIDTH)
{
    image_[UNSELECTED].SetSrc("");
    image_[SELECTED].SetSrc("");
    Resize(width_, height_);
}

UIRadioButton::UIRadioButton(const char* name) : name_(nullptr), radiusBig_(DEFAULT_RADIUS_BIG),
                                                 radiusSmall_(DEFAULT_RADIUS_SMALL), lineWidth_(DEFAULT_LINE_WIDTH)
{
    SetName(name);
    image_[UNSELECTED].SetSrc("");
    image_[SELECTED].SetSrc("");
    Resize(width_, height_);
}

bool UIRadioButton::OnClickEvent(const ClickEvent& event)
{
    SetState(SELECTED);
    Invalidate();
    UIView* view = this;
    while ((view != nullptr) && (view->GetParent() != nullptr)) {
        view = view->GetParent();
    }
    FindRadioButtonAndChangeState(view);
    return UIView::OnClickEvent(event);
}

void UIRadioButton::CalculateSize()
{
    int16_t width = GetWidth();
    int16_t height = GetHeight();
    if ((width_ == width) && (height_ == height)) {
        return;
    }
    width_ = width;
    height_ = height;
    int16_t minValue = (width_ > height_) ? height_ : width_;
    radiusBig_ = DEFAULT_RADIUS_BIG * minValue / DEFAULT_HOT_WIDTH;
    radiusSmall_ = DEFAULT_RADIUS_SMALL * minValue / DEFAULT_HOT_WIDTH;
    if (minValue >= DEFAULT_HOT_WIDTH) {
        lineWidth_ = DEFAULT_LINE_WIDTH * minValue / DEFAULT_HOT_WIDTH;
    }
}

void UIRadioButton::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if ((image_[SELECTED].GetSrcType() != IMG_SRC_UNKNOWN) && (image_[UNSELECTED].GetSrcType() != IMG_SRC_UNKNOWN)) {
        UICheckBox::OnDraw(gfxDstBuffer, invalidatedArea);
    } else {
        CalculateSize();
        BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetRect(), invalidatedArea, *style_, opaScale_);
        Rect contentRect = GetContentRect();
        int16_t dx = width_ >> 1;
        int16_t dy = height_ >> 1;
        int16_t x = contentRect.GetX() + dx;
        int16_t y = contentRect.GetY() + dy;
        ArcInfo arcInfoBig = { { x, y }, { 0 }, radiusBig_, 0, CIRCLE_IN_DEGREE, nullptr };
        ArcInfo arcInfoSmall = { { x, y }, { 0 }, radiusSmall_, 0, CIRCLE_IN_DEGREE, nullptr };
        Rect trunc = invalidatedArea;
        bool isIntersect = trunc.Intersect(trunc, contentRect);
        switch (state_) {
            case SELECTED: {
                Style styleSelect = StyleDefault::GetBackgroundTransparentStyle();
                styleSelect.lineWidth_ = arcInfoBig.radius;
                styleSelect.lineColor_ = Color::GetColorFromRGB(0x1F, 0x71, 0xFF);
                if (isIntersect) {
                    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfoBig, trunc, styleSelect,
                                                          OPA_OPAQUE, CapType::CAP_NONE);
                }
                styleSelect.lineWidth_ = arcInfoSmall.radius;
                styleSelect.lineColor_ = Color::White();
                if (isIntersect) {
                    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfoSmall, trunc, styleSelect,
                                                          OPA_OPAQUE, CapType::CAP_NONE);
                }
                break;
            }
            case UNSELECTED: {
                Style styleUnSelect = StyleDefault::GetBackgroundTransparentStyle();
                styleUnSelect.lineColor_ = Color::White();
                styleUnSelect.lineWidth_ = lineWidth_;
                if (isIntersect) {
                    // 0xa8 : opacity of drawing unselected button arc edge.
                    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfoBig, trunc, styleUnSelect,
                                                          0xa8, CapType::CAP_NONE);
                }
                break;
            }
            default:
                break;
        }
    }
}

void UIRadioButton::SetName(const char* name)
{
    if (name == nullptr) {
        return;
    }
    if (name_ != nullptr) {
        UIFree(name_);
        name_ = nullptr;
    }
    uint32_t nameLen = static_cast<uint32_t>(strlen(name) + 1);
    if (nameLen > MAX_TEXT_LENGTH) {
        return;
    }
    name_ = static_cast<char*>(UIMalloc(nameLen));
    if (name_ != nullptr) {
        if (memcpy_s(name_, nameLen, name, nameLen) != EOK) {
            UIFree(name_);
            name_ = nullptr;
            return;
        }
    }
}

void UIRadioButton::FindRadioButtonAndChangeState(UIView* view)
{
    if ((view == nullptr) || (name_ == nullptr)) {
        return;
    }
    if (view->IsViewGroup()) {
        UIView* childView = static_cast<UIViewGroup*>(view)->GetChildrenHead();
        while (childView != nullptr) {
            FindRadioButtonAndChangeState(childView);
            childView = childView->GetNextSibling();
        }
    }
    if ((view == this) || (view->GetViewType() != UI_RADIO_BUTTON)) {
        return;
    }
    UIRadioButton* uiRadioButtonInfo = static_cast<UIRadioButton*>(view);
    if ((uiRadioButtonInfo->GetName() != nullptr) && (strcmp(uiRadioButtonInfo->GetName(), name_) == 0)) {
        uiRadioButtonInfo->SetState(UNSELECTED);
    }
}
} // namespace OHOS
