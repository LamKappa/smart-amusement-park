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

#include "components/ui_list.h"

namespace OHOS {
UIList::Recycle::~Recycle()
{
    ListNode<UIView*>* node = scrapView_.Begin();
    while (node != scrapView_.End()) {
        if (node->data_) {
            UIView* deleteView = node->data_;
            if (deleteView != nullptr) {
                delete deleteView;
                deleteView = nullptr;
                node->data_ = nullptr;
            }
        }
        node = node->next_;
    }
    scrapView_.Clear();
}

void UIList::Recycle::InitRecycle()
{
    if ((adapter_ == nullptr) || (listView_ == nullptr)) {
        return;
    }
    FillActiveView();
    listView_->Invalidate();
}

UIView* UIList::Recycle::GetView(int16_t index)
{
    if (adapter_ == nullptr) {
        return nullptr;
    }
    UIView* inView = nullptr;
    UIView* retView = nullptr;

    if (scrapView_.Size() != 0) {
        inView = scrapView_.Back();
    }

    retView = adapter_->GetView(inView, index);
    if (retView != nullptr) {
        retView->SetViewIndex(index);
        scrapView_.PopBack();
    }
    return retView;
}

void UIList::Recycle::FillActiveView()
{
    if ((adapter_ == nullptr) || (listView_ == nullptr)) {
        return;
    }
    uint16_t index = listView_->GetStartIndex();
    if (listView_->GetDirection() == UIList::VERTICAL) {
        int16_t childBottom = 0;
        while ((index < adapter_->GetCount()) && (childBottom < listView_->GetHeight())) {
            UIView* view = GetView(index);
            if (view == nullptr) {
                break;
            }
            listView_->PushBack(view);
            if (listView_->childrenTail_) {
                childBottom =
                    listView_->childrenTail_->GetY() + listView_->childrenTail_->GetRelativeRect().GetHeight();
            } else {
                break;
            }
            index++;
        }
    } else {
        int16_t childRight = 0;
        while ((index < adapter_->GetCount()) && (childRight < listView_->GetWidth())) {
            UIView* view = GetView(index);
            listView_->PushBack(view);
            if (listView_->childrenTail_) {
                childRight = listView_->childrenTail_->GetX() + listView_->childrenTail_->GetRelativeRect().GetWidth();
            } else {
                break;
            }
            index++;
        }
    }
}

UIList::UIList() : UIList(VERTICAL) {}

UIList::UIList(uint8_t direction)
    : onSelectedView_(nullptr),
      isLoopList_(false),
      isReCalculateDragEnd_(true),
      autoAlign_(false),
      alignTime_(DEFAULT_ALINE_TIMES),
      startIndex_(0),
      topIndex_(0),
      bottomIndex_(0),
      selectPosition_(0),
      onSelectedIndex_(0),
      recycle_(this),
      scrollListener_(nullptr)
{
#if ENABLE_ROTATE_INPUT
    rotateFactor_ = DEFAULT_ROTATE_FACTOR;
    isRotating_ = false;
    lastRotateLen_ = 0;
#endif
#if ENABLE_VIBRATOR
    vibratorType_ = VibratorType::VIBRATOR_TYPE_ONE;
#endif
#if ENABLE_FOCUS_MANAGER
    focusable_ = true;
#endif
    direction_ = direction;
    touchable_ = true;
    draggable_ = true;
    dragParentInstead_ = false;
}

UIList::~UIList()
{
    UIView* view = GetChildrenHead();
    while (view != nullptr) {
        UIView* tmp = view->GetNextSibling();
        delete view;
        view = tmp;
    }
}

bool UIList::OnDragEvent(const DragEvent& event)
{
    if (scrollAnimator_.GetState() != Animator::STOP) {
        UIAbstractScroll::StopAnimator();
    }
    int16_t xDistance = event.GetDeltaX();
    int16_t yDistance = event.GetDeltaY();
    isReCalculateDragEnd_ = true;
    if (direction_ == VERTICAL) {
        RefreshDeltaY(yDistance);
        DragYInner(yDistance);
    } else {
        DragXInner(xDistance);
    }
    return UIView::OnDragEvent(event);
}

bool UIList::OnDragEndEvent(const DragEvent& event)
{
    Point last = event.GetPreLastPoint();
    Point current = event.GetLastPoint();
    if ((last.x == current.x) && (last.y == current.y)) {
        last = current;
        current = event.GetCurrentPos();
    }
    isReCalculateDragEnd_ = false;
    if (!DragThrowAnimator(current, last)) {
        if (scrollListener_ && (scrollListener_->GetScrollState() == ListScrollListener::SCROLL_STATE_MOVE)) {
            scrollListener_->SetScrollState(ListScrollListener::SCROLL_STATE_STOP);
            scrollListener_->OnScrollEnd(onSelectedIndex_, onSelectedView_);
        }
    }
    return UIView::OnDragEndEvent(event);
}

bool UIList::OnPressEvent(const PressEvent& event)
{
    StopAnimator();
    return UIView::OnPressEvent(event);
}

#if ENABLE_ROTATE_INPUT
bool UIList::OnRotateEvent(const RotateEvent& event)
{
    int16_t midPointX = static_cast<int16_t>(GetWidth() / 2);  // 2 : Get the middle point X coord of the view
    int16_t midPointY = static_cast<int16_t>(GetHeight() / 2); // 2 : Get the middle point Y coord of the view
    Point last, current;

    isRotating_ = true;
    if (throwDrag_ && event.GetRotate() == 0) {
        last = Point {midPointX, midPointY};
        (direction_ == VERTICAL) ? (current = Point {midPointX, static_cast<int16_t>(midPointY + lastRotateLen_)})
                                 : (current = Point {static_cast<int16_t>(midPointX + lastRotateLen_), midPointY});
        isReCalculateDragEnd_ = false;
        DragThrowAnimator(current, last);
        lastRotateLen_ = 0;
    } else {
        lastRotateLen_ = static_cast<int16_t>(event.GetRotate() * rotateFactor_);
        if (direction_ == VERTICAL) {
            DragYInner(lastRotateLen_);
        } else {
            DragXInner(lastRotateLen_);
        }
    }
    isRotating_ = false;
    return UIView::OnRotateEvent(event);
}
#endif

void UIList::ScrollBy(int16_t distance)
{
    if (direction_ == VERTICAL) {
        DragYInner(distance);
    } else {
        DragXInner(distance);
    }
    if (scrollListener_ && (scrollListener_->GetScrollState() == ListScrollListener::SCROLL_STATE_MOVE)) {
        scrollListener_->SetScrollState(ListScrollListener::SCROLL_STATE_STOP);
        scrollListener_->OnScrollEnd(onSelectedIndex_, onSelectedView_);
    }
}

bool UIList::DragXInner(int16_t distance)
{
    if (IsNeedReCalculateDragEnd()) {
        return false;
    }
    int16_t listWidth = GetWidth();
    if (distance == 0) {
        return true;
    }
    int16_t reboundSize = reboundSize_;
    if (isLoopList_ || (scrollAnimator_.GetState() != Animator::STOP)) {
        reboundSize = 0;
    }
    bool ret = 0;
    do {
        ret = MoveChildStep(distance);
    } while (ret);

    if (isLoopList_) {
        return MoveOffset(distance);
    }
    if (distance > 0) {
        if (childrenHead_ && ((childrenHead_->GetX() + distance) >
            (scrollBlankSize_ + reboundSize + childrenHead_->GetStyle(STYLE_MARGIN_LEFT)))) {
            distance =
                scrollBlankSize_ + reboundSize + childrenHead_->GetStyle(STYLE_MARGIN_LEFT) - childrenHead_->GetX();
        }
    } else {
        if (childrenTail_) {
            if (childrenTail_->GetRelativeRect().GetRight() <=
                (listWidth - scrollBlankSize_ - reboundSize - childrenTail_->GetStyle(STYLE_MARGIN_RIGHT))) {
                distance = 0;
            } else if ((listWidth - childrenTail_->GetX() - childrenTail_->GetRelativeRect().GetWidth() - distance) >
                       (scrollBlankSize_ + reboundSize + childrenTail_->GetStyle(STYLE_MARGIN_RIGHT))) {
                distance = listWidth - scrollBlankSize_ - reboundSize - childrenTail_->GetX() -
                           childrenTail_->GetRelativeRect().GetWidth() - childrenTail_->GetStyle(STYLE_MARGIN_RIGHT);
            }
        }
    }
    return MoveOffset(distance);
}

bool UIList::DragYInner(int16_t distance)
{
    if (IsNeedReCalculateDragEnd()) {
        return false;
    }
    int16_t listHeigh = GetHeight();
    if (distance == 0) {
        return true;
    }
    int16_t reboundSize = reboundSize_;
    if (isLoopList_ || (scrollAnimator_.GetState() != Animator::STOP)) {
        reboundSize = 0;
    }
    bool ret = 0;
    do {
        ret = MoveChildStep(distance);
    } while (ret);

    if (isLoopList_) {
        return MoveOffset(distance);
    }
    if (distance > 0) {
        if (childrenHead_ &&
            ((childrenHead_->GetY() + distance) >
            (scrollBlankSize_ + reboundSize + childrenHead_->GetStyle(STYLE_MARGIN_TOP)))) {
            distance =
                scrollBlankSize_ + reboundSize + childrenHead_->GetStyle(STYLE_MARGIN_TOP) - childrenHead_->GetY();
        }
    } else {
        if (childrenTail_) {
            if (childrenTail_->GetRelativeRect().GetBottom() <=
                (listHeigh - scrollBlankSize_ - reboundSize - childrenTail_->GetStyle(STYLE_MARGIN_BOTTOM))) {
                distance = 0;
            } else if ((listHeigh - childrenTail_->GetY() - childrenTail_->GetRelativeRect().GetHeight() - distance) >
                       (scrollBlankSize_ + reboundSize + childrenTail_->GetStyle(STYLE_MARGIN_BOTTOM))) {
                distance = listHeigh - scrollBlankSize_ - reboundSize - childrenTail_->GetY() -
                           childrenTail_->GetRelativeRect().GetHeight() - childrenTail_->GetStyle(STYLE_MARGIN_BOTTOM);
            }
        }
    }
    return MoveOffset(distance);
}

bool UIList::MoveOffset(int16_t offset)
{
    if (offset == 0) {
        return false;
    }
    if (direction_ == VERTICAL) {
        MoveChildByOffset(0, offset);
    } else {
        MoveChildByOffset(offset, 0);
    }
    Invalidate();
    if (scrollListener_ && (scrollListener_->GetScrollState() == ListScrollListener::SCROLL_STATE_STOP)) {
        scrollListener_->SetScrollState(ListScrollListener::SCROLL_STATE_MOVE);
        scrollListener_->OnScrollStart(onSelectedIndex_, onSelectedView_);
    }

    return true;
}

bool UIList::IsNeedReCalculateDragEnd()
{
    if (!autoAlign_ || isReCalculateDragEnd_ || (onSelectedView_ == nullptr)) {
        return false;
    }
    int16_t animationLess = 0;
    if (direction_ == VERTICAL) {
        animationLess = animatorCallback_.endValueY_ - animatorCallback_.previousValueY_;
    } else {
        animationLess = animatorCallback_.endValueX_ - animatorCallback_.previousValueX_;
    }
    if (!isDragging_ || (MATH_ABS(animationLess) > RECALCULATE_DRAG_DISTANCE)) {
        return false;
    }
    return true;
}
bool UIList::ReCalculateDragEnd()
{
    if ((onSelectedView_ == nullptr) || isReCalculateDragEnd_ || !autoAlign_) {
        return false;
    }

    int16_t offsetX = 0;
    int16_t offsetY = 0;
    if (direction_ == VERTICAL) {
        // 2: half
        offsetY = selectPosition_ - (onSelectedView_->GetY() + (onSelectedView_->GetRelativeRect().GetHeight() / 2));
    } else {
        // 2: half
        offsetX = selectPosition_ - (onSelectedView_->GetX() + (onSelectedView_->GetRelativeRect().GetWidth() / 2));
    }
    animatorCallback_.RsetCallback();
    animatorCallback_.SetDragStartValue(0, 0);
    animatorCallback_.SetDragEndValue(offsetX, offsetY);
    animatorCallback_.SetDragTimes(GetAutoAlignTime() / DEFAULT_TASK_PERIOD);
    scrollAnimator_.Start();
    isReCalculateDragEnd_ = true;
    return true;
}

bool UIList::MoveChildStepInner(int16_t distance,
                                int16_t (UIView::*getXOrY)() const,
                                int16_t (UIView::*getWidthOrHeight)())
{
    bool popRet = false;
    bool pushRet = false;
    if (distance > 0) {
        if ((childrenHead_ == nullptr) || ((childrenHead_->*getXOrY)() + distance > 0)) {
            uint16_t index = GetIndexDec(topIndex_);
            if (index == topIndex_) {
                return false;
            }
            UIView* newView = recycle_.GetView(index);
            if (newView == nullptr) {
                return false;
            }
            PushFront(newView);
            pushRet = true;
        }
        if (childrenTail_ != nullptr && ((childrenTail_->*getXOrY)() + distance > (this->*getWidthOrHeight)())) {
            PopItem(childrenTail_);
            popRet = true;
        }
    } else {
        if ((childrenTail_ == nullptr) ||
            ((childrenTail_->*getXOrY)() + (childrenTail_->*getWidthOrHeight)() + distance <
            (this->*getWidthOrHeight)())) {
            UIView* newView = recycle_.GetView(GetIndexInc(bottomIndex_));
            if (newView == nullptr) {
                return false;
            }
            PushBack(newView);
            pushRet = true;
        }
        if (childrenHead_ && (childrenHead_->*getXOrY)() + distance + (childrenHead_->*getWidthOrHeight)() < 0) {
            PopItem(childrenHead_);
            popRet = true;
        }
    }
    return (popRet || pushRet);
}

bool UIList::MoveChildStep(int16_t distance)
{
    if (direction_ == VERTICAL) {
        return MoveChildStepInner(distance, &UIView::GetY, &UIView::GetHeightWithMargin);
    } else {
        return MoveChildStepInner(distance, &UIView::GetX, &UIView::GetWidthWithMargin);
    }
}

void UIList::SetAdapter(AbstractAdapter* adapter)
{
    recycle_.SetAdapter(adapter);
    recycle_.InitRecycle();
}

UIView* UIList::GetSelectView()
{
    if (onSelectedView_ != nullptr) {
        return onSelectedView_;
    }
    if ((childrenHead_ == nullptr) || (selectPosition_ == 0)) {
        return nullptr;
    }
    UIView* child = childrenHead_;
    while (child != nullptr) {
        if (direction_ == VERTICAL) {
            if ((child->GetY() <= selectPosition_) &&
                (child->GetY() + child->GetRelativeRect().GetHeight() >= selectPosition_)) {
                if (scrollListener_ != nullptr) {
                    scrollListener_->OnItemSelected(child->GetViewIndex(), child);
                }
                return child;
            }
        } else {
            if ((child->GetX() <= selectPosition_) &&
                (child->GetX() + child->GetRelativeRect().GetWidth() >= selectPosition_)) {
                if (scrollListener_ != nullptr) {
                    scrollListener_->OnItemSelected(child->GetViewIndex(), child);
                }
                return child;
            }
        }
        child = child->GetNextSibling();
    }
    return nullptr;
}

void UIList::PushBack(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    if (childrenTail_ == nullptr) {
        SetHead(view);
    } else {
        if (direction_ == VERTICAL) {
            view->SetPosition(0, childrenTail_->GetY() + childrenTail_->GetHeightWithMargin());
        } else {
            view->SetPosition(childrenTail_->GetX() + childrenTail_->GetHeightWithMargin(), 0);
        }
        bottomIndex_ = GetIndexInc(bottomIndex_);
    }

    view->SetDragParentInstead(true);
    UIViewGroup::Add(view);
}

void UIList::PushFront(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    if (GetChildrenHead() == nullptr) {
        SetHead(view);
    } else {
        if (direction_ == VERTICAL) {
            view->SetPosition(0, GetChildrenHead()->GetY() - view->GetHeightWithMargin());
        } else {
            view->SetPosition(GetChildrenHead()->GetX() - view->GetWidthWithMargin(), 0);
        }
        topIndex_ = GetIndexDec(topIndex_);
    }
    view->SetDragParentInstead(true);
    UIViewGroup::Insert(nullptr, view);
}

void UIList::PopItem(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    recycle_.AddScrapView(view);
    if (view == GetChildrenHead()) {
        topIndex_ = GetIndexInc(topIndex_);
    }

    if (view == childrenTail_) {
        bottomIndex_ = GetIndexDec(bottomIndex_);
    }
    UIViewGroup::Remove(view);
}

void UIList::SetHead(UIView* view)
{
    if (view != nullptr) {
        view->SetPosition(0, 0);
        topIndex_ = startIndex_;
        bottomIndex_ = startIndex_;
    }
}

void UIList::MoveChildByOffset(int16_t xOffset, int16_t yOffset)
{
    UIView* view = GetChildrenHead();
    if (view == nullptr) {
        return;
    }
    int16_t x;
    int16_t y;
    int16_t height;
    int16_t width;

    if ((onSelectedIndex_ != NULL_SELECT_INDEX) && (selectPosition_ != 0)) {
        if (direction_ == VERTICAL) {
            height = view->GetRelativeRect().GetHeight();
            if ((GetChildrenHead()->GetY() + yOffset > selectPosition_) ||
                (childrenTail_->GetY() + height + childrenTail_->GetStyle(STYLE_MARGIN_BOTTOM) + yOffset <
                selectPosition_)) {
                onSelectedIndex_ = NULL_SELECT_INDEX;
                onSelectedView_ = nullptr;
                if (scrollListener_ != nullptr) {
                    scrollListener_->OnItemSelected(onSelectedIndex_, onSelectedView_);
                }
            }
        } else {
            width = view->GetRelativeRect().GetWidth();
            if ((GetChildrenHead()->GetX() + xOffset > selectPosition_) ||
                (childrenTail_->GetX() + width + childrenTail_->GetStyle(STYLE_MARGIN_RIGHT) < selectPosition_)) {
                onSelectedIndex_ = NULL_SELECT_INDEX;
                onSelectedView_ = nullptr;
                if (scrollListener_ != nullptr) {
                    scrollListener_->OnItemSelected(onSelectedIndex_, onSelectedView_);
                }
            }
        }
    }
    bool isSelectViewFind = false;
    while (view != nullptr) {
        x = view->GetX() + xOffset;
        y = view->GetY() + yOffset;
        view->SetPosition(x, y);
        if ((selectPosition_ != 0) && !isSelectViewFind) {
            if (direction_ == VERTICAL) {
                height = view->GetRelativeRect().GetHeight();
                /* Views may be the same but have different indexes because of view recycling. */
                if ((y - view->GetStyle(STYLE_PADDING_TOP) <= selectPosition_) &&
                    (y + view->GetStyle(STYLE_MARGIN_BOTTOM) + height >= selectPosition_) &&
                    ((onSelectedView_ != view) || (onSelectedIndex_ != view->GetViewIndex()))) {
                    onSelectedIndex_ = view->GetViewIndex();
                    onSelectedView_ = view;
                    if (scrollListener_ != nullptr) {
                        scrollListener_->OnItemSelected(onSelectedIndex_, onSelectedView_);
                    }
                    isSelectViewFind = true;
                }
            } else {
                width = view->GetRelativeRect().GetWidth();
                if ((x - view->GetStyle(STYLE_MARGIN_LEFT) <= selectPosition_) &&
                    (x + width + view->GetStyle(STYLE_MARGIN_RIGHT) >= selectPosition_) &&
                    ((onSelectedView_ != view) || (onSelectedIndex_ != view->GetViewIndex()))) {
                    onSelectedIndex_ = view->GetViewIndex();
                    onSelectedView_ = view;
                    if (scrollListener_ != nullptr) {
                        scrollListener_->OnItemSelected(onSelectedIndex_, onSelectedView_);
                    }
                    isSelectViewFind = true;
                }
            }
        }
        view = view->GetNextSibling();
    }

#if ENABLE_ROTATE_INPUT && ENABLE_VIBRATOR
    VibratorFunc vibratorFunc = VibratorManager::GetInstance()->GetVibratorFunc();
    if (isRotating_ && vibratorFunc != nullptr && isSelectViewFind) {
        vibratorFunc(VibratorType::VIBRATOR_TYPE_TWO);
    }
#endif
}

void UIList::StopAnimator()
{
    UIAbstractScroll::StopAnimator();
    if (!ReCalculateDragEnd()) {
        if ((scrollListener_ != nullptr) &&
            (scrollListener_->GetScrollState() == ListScrollListener::SCROLL_STATE_MOVE)) {
            scrollListener_->SetScrollState(ListScrollListener::SCROLL_STATE_STOP);
            scrollListener_->OnScrollEnd(onSelectedIndex_, onSelectedView_);
        }
    }
}

uint16_t UIList::GetIndexInc(uint16_t index)
{
    uint16_t ret = index + 1;
    if (isLoopList_ && (recycle_.GetAdapterItemCount() != 0)) {
        ret = ret % recycle_.GetAdapterItemCount();
    }
    return ret;
}

uint16_t UIList::GetIndexDec(uint16_t index)
{
    if (index == 0) {
        if (isLoopList_) {
            return recycle_.GetAdapterItemCount() - 1;
        } else {
            return 0;
        }
    } else {
        return index - 1;
    }
}

void UIList::ScrollTo(uint16_t index)
{
    UIView* child = GetChildrenHead();
    UIView* tmp = nullptr;
    while (child != nullptr) {
        tmp = child;
        child = child->GetNextSibling();
        PopItem(tmp);
    }
    onSelectedView_ = nullptr;
    SetStartIndex(index);
    recycle_.InitRecycle();
}

void UIList::RefreshList()
{
    int16_t topIndex = topIndex_;
    UIView* child = GetChildrenHead();
    UIView* tmp = nullptr;
    int16_t offset = 0;
    if (child != nullptr) {
        if (direction_ == VERTICAL) {
            offset = child->GetY();
        } else {
            offset = child->GetX();
        }
    }

    while (child != nullptr) {
        tmp = child;
        child = child->GetNextSibling();
        PopItem(tmp);
    }
    onSelectedView_ = nullptr;

    uint16_t tmpStartIndex = startIndex_;
    if (topIndex > recycle_.GetAdapterItemCount() - 1) {
        startIndex_ = 0;
        offset = 0;
    } else {
        startIndex_ = topIndex;
    }
    recycle_.InitRecycle();
    startIndex_ = tmpStartIndex;

    if (direction_ == VERTICAL) {
        DragYInner(offset);
    } else {
        DragXInner(offset);
    }
    Invalidate();
}

void UIList::RemoveAll()
{
    UIViewGroup::RemoveAll();
    recycle_.ClearScrapView();
}

void UIList::CalculateReboundDistance(int16_t& dragDistanceX, int16_t& dragDistanceY)
{
    if (isLoopList_) {
        return;
    }
    Rect rect = GetAllChildRelativeRect();
    int16_t top = rect.GetTop();
    int16_t bottom = rect.GetBottom();
    int16_t scrollHeight = GetHeight();
    int16_t left = rect.GetLeft();
    int16_t right = rect.GetRight();
    int16_t scrollWidth = GetWidth();
    if ((direction_ == VERTICAL) || (direction_ == HORIZONTAL_AND_VERTICAL)) {
        if (top > scrollBlankSize_) {
            if ((dragDistanceY + top) > (scrollBlankSize_ + reboundSize_)) {
                dragDistanceY = 0;
            }
            dragDistanceY += scrollBlankSize_ - (top + dragDistanceY);
        }
        if (bottom < (scrollHeight - scrollBlankSize_ - 1)) {
            if ((dragDistanceY + bottom) < (scrollHeight - scrollBlankSize_ - reboundSize_ - 1)) {
                dragDistanceY = 0;
            }
            dragDistanceY += scrollHeight - scrollBlankSize_ - 1 - (bottom + dragDistanceY);
        }
    } else {
        if (left > scrollBlankSize_) {
            if ((dragDistanceX + left) > (scrollBlankSize_ + reboundSize_)) {
                dragDistanceX = 0;
            }
            dragDistanceX += scrollBlankSize_ - (left + dragDistanceX);
        }
        if (right < (scrollWidth - scrollBlankSize_ - 1)) {
            if ((dragDistanceX + right) < (scrollWidth - scrollBlankSize_ - reboundSize_ - 1)) {
                dragDistanceX = 0;
            }
            dragDistanceX += scrollWidth - scrollBlankSize_ - 1 - (right + dragDistanceX);
        }
    }
}

#if ENABLE_VIBRATOR
void UIList::SetMotorType(VibratorType vibratorType)
{
    vibratorType_ = vibratorType;
}
#endif
} // namespace OHOS
