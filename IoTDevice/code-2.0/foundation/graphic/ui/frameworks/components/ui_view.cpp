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

#include "components/ui_view.h"
#include "components/root_view.h"
#include "core/render_manager.h"
#include "dock/focus_manager.h"
#include "draw/draw_utils.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/mem_api.h"
#include "securec.h"
#include "themes/theme_manager.h"

namespace OHOS {
UIView::UIView()
    : touchable_(false),
      visible_(true),
      draggable_(false),
      dragParentInstead_(true),
      isViewGroup_(false),
      needRedraw_(false),
      styleAllocFlag_(false),
      isIntercept_(false),
#if ENABLE_FOCUS_MANAGER
      focusable_(false),
#endif
      opaScale_(OPA_OPAQUE),
      index_(0),
      id_(nullptr),
      parent_(nullptr),
      nextSibling_(nullptr),
      style_(nullptr),
      transMap_(nullptr),
      onClickListener_(nullptr),
      onLongPressListener_(nullptr),
      onDragListener_(nullptr),
      onTouchListener_(nullptr),
#if ENABLE_FOCUS_MANAGER
      onFocusListener_(nullptr),
#endif
#if ENABLE_ROTATE_INPUT
      onRotateListener_(nullptr),
#endif
      viewExtraMsg_(nullptr),
      rect_(0, 0, 0, 0),
      visibleRect_(nullptr)
{
    SetupThemeStyles();
}

UIView::~UIView()
{
    if (transMap_ != nullptr) {
        delete transMap_;
        transMap_ = nullptr;
    }
    if (visibleRect_ != nullptr) {
        delete visibleRect_;
        visibleRect_ = nullptr;
    }
    if (styleAllocFlag_) {
        delete style_;
        style_ = nullptr;
        styleAllocFlag_ = false;
    }
}

bool UIView::OnPreDraw(Rect& invalidatedArea) const
{
    Rect rect(GetRect());
    int16_t r = style_->borderRadius_;
    if (r == COORD_MAX) {
        return true;
    }
    if (r != 0) {
        r = ((r & 0x1) == 0) ? (r >> 1) : ((r + 1) >> 1);
        rect.SetLeft(rect.GetX() + r);
        rect.SetWidth(rect.GetWidth() - r);
        rect.SetTop(rect.GetY() + r);
        rect.SetHeight(rect.GetHeight() - r);
    }
    if (rect.IsContains(invalidatedArea)) {
        return true;
    }
    invalidatedArea.Intersect(invalidatedArea, rect);
    return false;
}

void UIView::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    uint8_t opa = GetMixOpaScale();
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetOrigRect(), invalidatedArea, *style_, opa);
}

void UIView::SetupThemeStyles()
{
    Theme* theme = ThemeManager::GetInstance().GetCurrent();
    if (theme != nullptr) {
        style_ = &(theme->GetMainStyle());
    } else {
        style_ = &(StyleDefault::GetDefaultStyle());
    }
}

void UIView::SetStyle(Style& style)
{
    if (styleAllocFlag_) {
        delete style_;
        styleAllocFlag_ = false;
    }
    style_ = &style;
}

void UIView::SetStyle(uint8_t key, int64_t value)
{
    if (!styleAllocFlag_) {
        style_ = new Style(*style_);
        if (style_ == nullptr) {
            GRAPHIC_LOGE("new Style fail");
            return;
        }
        styleAllocFlag_ = true;
    }
    int16_t width = GetWidth();
    int16_t height = GetHeight();
    style_->SetStyle(key, value);
    switch (key) {
        case STYLE_BORDER_WIDTH: {
            SetWidth(width);
            SetHeight(height);
            break;
        }
        case STYLE_PADDING_LEFT:
        case STYLE_PADDING_RIGHT: {
            SetWidth(width);
            break;
        }
        case STYLE_PADDING_TOP:
        case STYLE_PADDING_BOTTOM: {
            SetHeight(height);
            break;
        }
        default:
            break;
    }
}

void UIView::Rotate(int16_t angle, const Vector2<float>& pivot)
{
    if (transMap_ == nullptr) {
        ReMeasure();
        transMap_ = new TransformMap();
        if (transMap_ == nullptr) {
            GRAPHIC_LOGE("new TransformMap fail");
            return;
        }
    }
    Rect joinRect = transMap_->GetBoxRect();
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->Rotate(angle, pivot);
    joinRect.Join(joinRect, transMap_->GetBoxRect());
    joinRect.Join(joinRect, GetOrigRect());
    InvalidateRect(joinRect);
}

void UIView::Scale(const Vector2<float>& scale, const Vector2<float>& pivot)
{
    if (transMap_ == nullptr) {
        ReMeasure();
        transMap_ = new TransformMap();
        if (transMap_ == nullptr) {
            GRAPHIC_LOGE("new TransformMap fail");
            return;
        }
    }
    bool firstTrans = transMap_->IsInvalid();
    Rect joinRect = transMap_->GetBoxRect();
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->Scale(scale, pivot);
    if (firstTrans) {
        joinRect = transMap_->GetBoxRect();
    } else {
        joinRect.Join(joinRect, transMap_->GetBoxRect());
    }
    joinRect.Join(joinRect, GetOrigRect());
    InvalidateRect(joinRect);
}

void UIView::Translate(const Vector2<int16_t>& trans)
{
    if (transMap_ == nullptr) {
        ReMeasure();
        transMap_ = new TransformMap(GetOrigRect());
        if (transMap_ == nullptr) {
            GRAPHIC_LOGE("new TransformMap fail");
            return;
        }
    }
    transMap_->Translate(trans);

    Rect prevRect = GetRect();
    Rect mapRect = transMap_->GetBoxRect();

    Rect joinRect;
    joinRect.Join(prevRect, mapRect);
    InvalidateRect(joinRect);
}

bool UIView::IsTransInvalid()
{
    if (transMap_ == nullptr) {
        return true;
    }
    return transMap_->IsInvalid();
}

void UIView::ResetTransParameter()
{
    if (transMap_ != nullptr) {
        delete transMap_;
        transMap_ = nullptr;
        Invalidate();
    }
}

#if ENABLE_ROTATE_INPUT
void UIView::RequestFocus()
{
    FocusManager::GetInstance()->RequestFocus(this);
}

void UIView::ClearFocus()
{
    FocusManager::GetInstance()->ClearFocus();
}
#endif

void UIView::Invalidate()
{
    InvalidateRect(GetOrigRect());
}

void UIView::InvalidateRect(const Rect& invalidatedArea)
{
    if (!visible_) {
        if (needRedraw_) {
            needRedraw_ = false;
        } else {
            return;
        }
    }

    Rect trunc(invalidatedArea);
    bool isIntersect = true;
    UIView* par = parent_;
    UIView* cur = this;

    while (par != nullptr) {
        if (!par->visible_) {
            return;
        }

        isIntersect = trunc.Intersect(par->GetContentRect(), trunc);
        if (!isIntersect) {
            break;
        }

        cur = par;
        par = par->parent_;
    }

    if (isIntersect && (cur->GetViewType() == UI_ROOT_VIEW)) {
        RootView* rootView = reinterpret_cast<RootView*>(cur);
        rootView->AddInvalidateRectWithLock(trunc, this);
    }
}

bool UIView::OnLongPressEvent(const LongPressEvent& event)
{
    if (onLongPressListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onLongPressListener_->OnLongPress(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnDragStartEvent(const DragEvent& event)
{
    if (onDragListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onDragListener_->OnDragStart(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnDragEvent(const DragEvent& event)
{
    if (onDragListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onDragListener_->OnDrag(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnDragEndEvent(const DragEvent& event)
{
    if (onDragListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onDragListener_->OnDragEnd(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnClickEvent(const ClickEvent& event)
{
    if (onClickListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onClickListener_->OnClick(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnPressEvent(const PressEvent& event)
{
    if (onTouchListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onTouchListener_->OnPress(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnReleaseEvent(const ReleaseEvent& event)
{
    if (onTouchListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onTouchListener_->OnRelease(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnCancelEvent(const CancelEvent& event)
{
    if (onTouchListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onTouchListener_->OnCancel(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

#if ENABLE_ROTATE_INPUT
bool UIView::OnRotateEvent(const RotateEvent& event)
{
    if (onRotateListener_ != nullptr) {
        return onRotateListener_->OnRotate(*this, event);
    }
    return isIntercept_;
}
#endif

void UIView::GetTargetView(const Point& point, UIView** last)
{
    if (last == nullptr) {
        return;
    }
    UIView* par = parent_;
    Rect rect = GetRect();

    if (par != nullptr) {
        rect.Intersect(par->GetContentRect(), rect);
    }

    if (visible_ && touchable_ && rect.IsContains(point)) {
        *last = this;
    }
}

void UIView::GetTargetView(const Point& point, UIView** current, UIView** target)
{
    if (current == nullptr) {
        return;
    }
    UIView* par = parent_;
    Rect rect = GetRect();

    if (par != nullptr) {
        rect.Intersect(par->GetContentRect(), rect);
    }

    if (visible_ && rect.IsContains(point)) {
        if (touchable_) {
            *current = this;
        }
        *target = this;
    }
}

#if ENABLE_FOCUS_MANAGER
void UIView::Focus()
{
    if (focusable_ && onFocusListener_ != nullptr) {
        onFocusListener_->OnFocus(*this);
    }
}

void UIView::Blur()
{
    if (onFocusListener_ != nullptr) {
        onFocusListener_->OnBlur(*this);
    }
}
#endif

Rect UIView::GetRect() const
{
    if ((transMap_ != nullptr) && !transMap_->IsInvalid()) {
        Rect r = transMap_->GetBoxRect();
        Rect origRect = GetOrigRect();
        r.SetX(r.GetX() + origRect.GetX() - transMap_->GetTransMapRect().GetX());
        r.SetY(r.GetY() + origRect.GetY() - transMap_->GetTransMapRect().GetY());
        return r;
    }
    return GetOrigRect();
}

Rect UIView::GetContentRect()
{
    if ((transMap_ != nullptr) && !transMap_->IsInvalid()) {
        Rect r = transMap_->GetBoxRect();
        Rect origRect = GetOrigRect();
        r.SetX(r.GetX() + origRect.GetX() - transMap_->GetTransMapRect().GetX());
        r.SetY(r.GetY() + origRect.GetY() - transMap_->GetTransMapRect().GetY());
        return r;
    }

    Rect contentRect = GetRect();
    contentRect.SetX(contentRect.GetX() + style_->paddingLeft_ + style_->borderWidth_);
    contentRect.SetY(contentRect.GetY() + style_->paddingTop_ + style_->borderWidth_);
    contentRect.SetWidth(GetWidth());
    contentRect.SetHeight(GetHeight());
    return contentRect;
}

Rect UIView::GetOrigRect() const
{
    int16_t x = rect_.GetX();
    int16_t y = rect_.GetY();
    UIView* par = parent_;
    while (par != nullptr) {
        x += par->GetRelativeRect().GetX() + par->GetStyle(STYLE_PADDING_LEFT) + par->GetStyle(STYLE_BORDER_WIDTH);
        y += par->GetRelativeRect().GetY() + par->GetStyle(STYLE_PADDING_TOP) + par->GetStyle(STYLE_BORDER_WIDTH);
        par = par->parent_;
    }
    return Rect(x, y, x + rect_.GetWidth() - 1, y + rect_.GetHeight() - 1);
}

Rect UIView::GetMaskedRect() const
{
    Rect mask;
    if (visibleRect_ != nullptr) {
        mask.Intersect(GetRect(), GetVisibleRect());
    } else {
        mask = GetRect();
    }
    return mask;
}

Rect UIView::GetVisibleRect() const
{
    if (visibleRect_ == nullptr) {
        return GetRect();
    }
    Rect absoluteRect;
    int16_t x = visibleRect_->GetX();
    int16_t y = visibleRect_->GetY();
    UIView* par = parent_;
    while (par != nullptr) {
        x += par->GetX();
        y += par->GetY();
        par = par->parent_;
    }
    absoluteRect.SetX(x);
    absoluteRect.SetY(y);
    absoluteRect.SetWidth(visibleRect_->GetWidth());
    absoluteRect.SetHeight(visibleRect_->GetHeight());
    return absoluteRect;
}

void UIView::SetTransformMap(const TransformMap& transMap)
{
    if ((transMap_ != nullptr) && (*transMap_ == transMap)) {
        return;
    }

    Rect prevRect = GetRect();
    Rect mapRect = transMap.GetBoxRect();

    Rect joinRect;
    joinRect.Join(prevRect, mapRect);

    InvalidateRect(joinRect);

    if (transMap_ == nullptr) {
        transMap_ = new TransformMap();
    }

    *transMap_ = transMap;
}

void UIView::SetWidthPercent(float widthPercent)
{
    if (IsInvalid(widthPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1)) {
        int16_t newWidth = static_cast<int16_t>(GetParent()->GetWidth() * widthPercent);
        SetWidth(newWidth);
    }
}

void UIView::SetHeightPercent(float heightPercent)
{
    if (IsInvalid(heightPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetHeight() > 1)) {
        int16_t newHeight = static_cast<int16_t>(GetParent()->GetHeight() * heightPercent);
        SetHeight(newHeight);
    }
}

void UIView::ResizePercent(float widthPercent, float heightPercent)
{
    if (IsInvalid(widthPercent) || IsInvalid(heightPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1) && (GetParent()->GetHeight() > 1)) {
        int16_t newWidth = static_cast<int16_t>(GetParent()->GetWidth() * widthPercent);
        int16_t newHeight = static_cast<int16_t>(GetParent()->GetHeight() * heightPercent);
        Resize(newWidth, newHeight);
    }
}

void UIView::SetXPercent(float xPercent)
{
    if (IsInvalid(xPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1)) {
        int16_t newX = static_cast<int16_t>(GetParent()->GetWidth() * xPercent);
        SetX(newX);
    }
}

void UIView::SetYPercent(float yPercent)
{
    if (IsInvalid(yPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetHeight() > 1)) {
        int16_t newY = static_cast<int16_t>(GetParent()->GetHeight() * yPercent);
        SetY(newY);
    }
}

void UIView::SetPositionPercent(float xPercent, float yPercent)
{
    if (IsInvalid(xPercent) || IsInvalid(yPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1) && (GetParent()->GetHeight() > 1)) {
        int16_t newX = static_cast<int16_t>(GetParent()->GetWidth() * xPercent);
        int16_t newY = static_cast<int16_t>(GetParent()->GetHeight() * yPercent);
        SetPosition(newX, newY);
    }
}

void UIView::SetPositionPercent(float xPercent, float yPercent, float widthPercent, float heightPercent)
{
    if (IsInvalid(xPercent) || IsInvalid(yPercent) || IsInvalid(widthPercent) || IsInvalid(heightPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1) && (GetParent()->GetHeight() > 1)) {
        int16_t newX = static_cast<int16_t>(GetParent()->GetWidth() * xPercent);
        int16_t newY = static_cast<int16_t>(GetParent()->GetHeight() * yPercent);
        int16_t newWidth = static_cast<int16_t>(GetParent()->GetWidth() * widthPercent);
        int16_t newHeight = static_cast<int16_t>(GetParent()->GetHeight() * heightPercent);
        SetPosition(newX, newY, newWidth, newHeight);
    }
}

bool UIView::IsInvalid(float percent)
{
    if ((percent < 1) && (percent > 0)) {
        return false;
    }
    return true;
}

void UIView::LayoutCenterOfParent(int16_t xOffset, int16_t yOffset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t topMargin = style_->marginTop_;
    int16_t leftMargin = style_->marginLeft_;
    int16_t rightMargin = style_->marginRight_;
    int16_t bottomMargin = style_->marginBottom_;
    // 2: half
    int16_t posX = parent_->GetWidth() / 2 - (rect_.GetWidth() - leftMargin + rightMargin) / 2 + xOffset;
    int16_t posY = parent_->GetHeight() / 2 - (rect_.GetHeight() - topMargin + bottomMargin) / 2 + yOffset;
    SetPosition(posX, posY);
}

void UIView::LayoutLeftOfParent(int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t leftMargin = style_->marginLeft_;
    SetPosition(leftMargin + offset, GetY());
}

void UIView::LayoutRightOfParent(int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t rightMargin = style_->marginRight_;
    SetPosition(parent_->GetWidth() - offset - rect_.GetWidth() - rightMargin, GetY());
}

void UIView::LayoutTopOfParent(int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t topMargin = style_->marginTop_;
    SetPosition(GetX(), topMargin + offset);
}

void UIView::LayoutBottomOfParent(int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t bottomMargin = style_->marginBottom_;
    SetPosition(GetX(), parent_->GetHeight() - offset - rect_.GetHeight() - bottomMargin);
}

void UIView::AlignLeftToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginLeft_ - style_->marginLeft_;
        SetPosition(sib->GetX() - margin + offset, GetY());
    }
}

void UIView::AlignRightToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginRight_ - style_->marginRight_;
        SetPosition(sib->GetX() + sib->rect_.GetWidth() - rect_.GetWidth() - offset + margin, GetY());
    }
}

void UIView::AlignTopToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginTop_ - style_->marginTop_;
        SetPosition(GetX(), sib->GetY() + offset - margin);
    }
}

void UIView::AlignBottomToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginBottom_ - style_->marginBottom_;
        SetPosition(GetX(), sib->GetY() + sib->rect_.GetHeight() - rect_.GetHeight() - offset + margin);
    }
}

void UIView::AlignHorCenterToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin =
            (sib->style_->marginRight_ - sib->style_->marginLeft_ - style_->marginRight_ + style_->marginLeft_) /
            2; // 2 : half
        SetPosition(sib->GetX() + sib->rect_.GetWidth() / 2 - rect_.GetWidth() / 2 + margin + offset, GetY());
    }
}
void UIView::AlignVerCenterToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin =
            (sib->style_->marginBottom_ - sib->style_->marginTop_ - style_->marginBottom_ + style_->marginTop_) /
            2; // 2 : half
        SetPosition(GetX(), sib->GetY() + sib->rect_.GetHeight() / 2 - rect_.GetHeight() / 2 + margin + offset);
    }
}

void UIView::LayoutLeftToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginLeft_ + style_->marginRight_;
        SetPosition(sib->GetX() - offset - rect_.GetWidth() - margin, GetY());
    }
}

void UIView::LayoutRightToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginRight_ + style_->marginLeft_;
        SetPosition(sib->GetX() + sib->rect_.GetWidth() + offset + margin, GetY());
    }
}

void UIView::LayoutTopToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginTop_ + style_->marginBottom_;
        SetPosition(GetX(), sib->GetY() - offset - rect_.GetHeight() - margin);
    }
}

void UIView::LayoutBottomToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginBottom_ + style_->marginTop_;
        SetPosition(GetX(), sib->GetY() + sib->rect_.GetHeight() + offset + margin);
    }
}

uint8_t UIView::GetMixOpaScale() const
{
    uint8_t opaMix = opaScale_;
    UIView* parent = parent_;
    uint8_t opaParent;
    while (parent != nullptr) {
        opaParent = parent->GetOpaScale();
        // 8: Shift right 8 bits
        opaMix = (opaParent == OPA_OPAQUE) ? opaMix : ((static_cast<uint16_t>(opaParent) * opaMix) >> 8);
        parent = parent->GetParent();
    }
    return opaMix;
}

bool UIView::GetBitmap(ImageInfo& bitmap)
{
    UIView* tempSibling = nextSibling_;
    UIView* tempParent = parent_;
    int16_t tempX = rect_.GetX();
    int16_t tempY = rect_.GetY();
    nextSibling_ = nullptr;
    parent_ = nullptr;

    BufferInfo* bufferInfo = BaseGfxEngine::GetInstance()->GetFBBufferInfo();
    if (bufferInfo == nullptr) {
        return false;
    }
    int16_t screenWidth = bufferInfo->rect.GetWidth();
    int16_t screenHeight = bufferInfo->rect.GetHeight();
    Rect screenRect(0, 0, screenWidth, screenHeight);
    rect_.SetPosition(0, 0);
    Rect mask = GetRect();
    mask.Intersect(mask, screenRect);
    uint16_t bufferWidth = static_cast<uint16_t>(mask.GetWidth());
    uint16_t bufferHeight = static_cast<uint16_t>(mask.GetHeight());
    bitmap.header.colorMode = bufferInfo->mode;
    bitmap.dataSize = bufferWidth * bufferHeight * DrawUtils::GetByteSizeByColorMode(bitmap.header.colorMode);
    bitmap.header.width = bufferWidth;
    bitmap.header.height = bufferHeight;
    bitmap.header.reserved = 0;

    uint8_t* viewBitmapBuffer = reinterpret_cast<uint8_t*>(ImageCacheMalloc(bitmap));
    if (viewBitmapBuffer == nullptr) {
        nextSibling_ = tempSibling;
        parent_ = tempParent;
        rect_.SetPosition(tempX, tempY);
        return false;
    }

    BufferInfo newBufferInfo;
    newBufferInfo.virAddr = static_cast<void*>(viewBitmapBuffer);
    newBufferInfo.phyAddr = newBufferInfo.virAddr;
    newBufferInfo.rect = mask;
    newBufferInfo.width = bufferWidth;
    newBufferInfo.height = bufferHeight;
    newBufferInfo.mode = bufferInfo->mode;

    RootView::GetInstance()->SaveDrawContext();
    RootView::GetInstance()->UpdateBufferInfo(&newBufferInfo);
    RootView::GetInstance()->DrawTop(this, mask);
    bitmap.data = viewBitmapBuffer;
    RootView::GetInstance()->RestoreDrawContext();
    nextSibling_ = tempSibling;
    parent_ = tempParent;
    rect_.SetPosition(tempX, tempY);
    return true;
}
} // namespace OHOS
