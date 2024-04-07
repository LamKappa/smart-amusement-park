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
#include "component.h"
#include "ace_log.h"
#include "ace_mem_base.h"
#include "directive/descriptor_utils.h"
#include "fatal_handler.h"
#include "handler.h"
#include "js_ability_impl.h"
#include "js_app_context.h"
#include "js_app_environment.h"
#include "js_profiler.h"
#include "js_router.h"
#include "key_parser.h"
#include "keys.h"
#include "lazy_load_manager.h"
#include "securec.h"
#include "stylemgr/app_style.h"
#include "stylemgr/app_style_manager.h"
#include "time_util.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
static Component::AnimationsNode *g_animationListHead = nullptr;
static bool g_isAnimatorStarted = false;
void Component::HandlerAnimations()
{
    Component::AnimationsNode *point = g_animationListHead;
    while (point != nullptr && point->transitionImpl != nullptr) {
        point->transitionImpl->Start();
        point = point->next;
    }
    g_isAnimatorStarted = true;
}

void Component::ReleaseAnimations()
{
    Component::AnimationsNode *point = g_animationListHead;
    while (point != nullptr) {
        Component::AnimationsNode *temp = point->next;
        if (point->transitionImpl) {
            delete (point->transitionImpl);
            point->transitionImpl = nullptr;
        }
        delete (point);
        point = temp;
    }
    g_animationListHead = nullptr;
    g_isAnimatorStarted = false;
}

Component::Component(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager)
    : childHead_(nullptr),
      nextSibling_(nullptr),
      parent_(nullptr),
      styleManager_(styleManager),
      options_(options),
      children_(UNDEFINED),
      onClickListener_(nullptr),
      onLongPressListener_(nullptr),
      onSwipeListener_(nullptr),
#ifdef JS_TOUCH_EVENT_SUPPORT
      onTouchStartListener_(nullptr),
      onTouchMoveListener_(nullptr),
      onTouchCancelListener_(nullptr),
      onTouchEndListener_(nullptr),
      keyBoardEventListener_(nullptr),
#endif
      viewId_(nullptr),
      componentName_(K_UNKNOWN),
      rendered_(false),
      isAnimationKeyFramesSet_(false),
      curTransitionImpl_(nullptr),
      trans_(nullptr),
      descriptors_(jerry_acquire_value(children)),
      watchersHead_(nullptr),
      height_(-1, DimensionType::TYPE_UNKNOWN),
      width_(-1, DimensionType::TYPE_UNKNOWN),
      top_(-1, DimensionType::TYPE_UNKNOWN),
      left_(-1, DimensionType::TYPE_UNKNOWN),
      marginTop_(-1, DimensionType::TYPE_UNKNOWN),
      marginLeft_(-1, DimensionType::TYPE_UNKNOWN),
      marginRight_(-1, DimensionType::TYPE_UNKNOWN),
      marginBottom_(-1, DimensionType::TYPE_UNKNOWN)
{
    // create native element object before combining styles, as style data binding need it
    nativeElement_ = jerry_create_object();
    jerry_value_t global = jerry_get_global_object();
    viewModel_ = jerryx_get_property_str(global, ATTR_ROOT);
    jerry_release_value(global);
}

/**
 * @brief After construct a specific component, call this function to setup this component's native view
 * and process attribute/events/style/children properly before binding it on an JS object.
 * It generally calls a series of functions to complete the render work, some of which are needed to be
 * implemented by child class. See step1~step6 function notes.
 *
 * NOTE: Caller should check the return result to decide if it's needed to recycle the component if the
 * rendering failed.
 *
 * @return true if success, false if any error occurs
 */
bool Component::Render()
{
    if (rendered_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Render one component twice is not allowed.");
        return false;
    }

    PreRender();
    // step1: create native view/views
    START_TRACING_WITH_COMPONENT_NAME(RENDER_CREATE_COMPONENT, componentName_);
    bool renderResult = CreateNativeViews();
    if (!renderResult) {
        return false;
    }
    STOP_TRACING();

    SetViewExtraMsg();

    // step2: binding js object with this component
    jerry_set_object_native_pointer(nativeElement_, this, nullptr);

    // step3: parse options for attributes and events, will call child class's according methods
    ParseOptions();
    // step4: apply styles
    START_TRACING_WITH_COMPONENT_NAME(RENDER_APPLY_STYLE, componentName_);
    ApplyStyles(options_, *this);
    STOP_TRACING();

    // step5:process this component's children
    START_TRACING_WITH_COMPONENT_NAME(RENDER_PROCESS_CHILDREN, componentName_);
    renderResult = ProcessChildren();
    STOP_TRACING();
    if (!renderResult) {
        return false;
    }
    RecordAnimation();
    PostRender();
    rendered_ = true;

    return renderResult;
}

void Component::SetViewExtraMsg()
{
    UIView *view = GetComponentRootView();
    if (view == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "failed to set the extra message for view.");
        return;
    }
    UIView::ViewExtraMsg *extraMsg = new UIView::ViewExtraMsg();
    if (extraMsg == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to mapping native view with DOM element.");
        return;
    }
    extraMsg->elementPtr = (void *)&nativeElement_;
    view->SetExtraMsg(extraMsg);
}

void Component::ReleaseViewExtraMsg()
{
    UIView *view = GetComponentRootView();
    if (view == nullptr) {
        return;
    }
    UIView::ViewExtraMsg *extraMsg = view->GetExtraMsg();
    ACE_DELETE(extraMsg);
    view->SetExtraMsg(nullptr);
}

void Component::Release()
{
    // detach self from fatal handler monitoring
    FatalHandler::GetInstance().DetachComponentNode(this);
    if (parent_ != nullptr) {
        parent_->RemoveChild(this);
    }

    // stop view animation
    if (curTransitionImpl_) {
        curTransitionImpl_->Stop();
    }
    ReleaseViewExtraMsg();
    jerry_delete_object_native_pointer(nativeElement_, nullptr);
    // release all native views
    ReleaseNativeViews();
    // release transition param
    ReleaseTransitionParam();
    // release the common event listeners if any
    ReleaseCommonEventListeners();
    // release children = jerry_create_array() in Init()
    ClearWatchersCommon(watchersHead_);
    // free viewId string if it's set
    ACE_FREE(viewId_);
    // release js object
    jerry_release_value(nativeElement_);
    jerry_release_value(descriptors_);
    jerry_release_value(children_);
    jerry_release_value(viewModel_);
}

bool Component::UpdateView(uint16_t attrKeyId, jerry_value_t attrValue)
{
    if (!KeyParser::IsKeyValid(attrKeyId)) {
        return false;
    }

    START_TRACING_WITH_EXTRA_INFO(SET_ATTR_SET_TO_NATIVE, componentName_, attrKeyId);
    PreUpdate();

    // check if need to invalided self before changing in case component's area will be changed
    InvalidateIfNeeded(attrKeyId, true);
    bool updateResult = SetAttribute(attrKeyId, attrValue);
    if (!updateResult) {
        AppStyleItem *styleItem = AppStyleItem::CreateStyleItem(attrKeyId, attrValue);
        if (styleItem != nullptr) {
            updateResult = ApplyStyle(styleItem);
            delete styleItem;
            styleItem = nullptr;
        }
    }

    if (parent_ != nullptr) {
        ConstrainedParameter parentParam;
        parent_->GetConstrainedParam(parentParam);
        AlignDimensions(parentParam);
    }
    AdaptBoxSizing();
    // force parent to relayout the children in case component's area is changed
    InvalidateIfNeeded(attrKeyId, false);
    if (updateResult) {
        PostUpdate(attrKeyId);
    }
    StartAnimation();
    STOP_TRACING();

    return updateResult;
}
void Component::RegisterNamedFunction(const char * const name, jerry_external_handler_t handler) const
{
    JerrySetFuncProperty(nativeElement_, name, handler);
}
// default implementation
bool Component::SetAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    UIView *uiView = GetComponentRootView();
    if ((uiView == nullptr) || !KeyParser::IsKeyValid(attrKeyId) || IS_UNDEFINED(attrValue)) {
        return false;
    }

    // try private first
    bool setResult = SetPrivateAttribute(attrKeyId, attrValue);
    if (!setResult) {
        // this means no private attributes matches, so need to try private ones
        setResult = SetCommonAttribute(*uiView, attrKeyId, attrValue);
    }

    return setResult;
}

bool Component::SetCommonAttribute(UIView &view, const uint16_t attrKeyId, const jerry_value_t attrValue)
{
    switch (attrKeyId) {
        case K_ID: {
            // this string in component itself.
            ACE_FREE(viewId_);
            viewId_ = MallocStringOf(attrValue);
            if (viewId_ == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "failed to set `id` attribute.");
                return false;
            }
            view.SetViewId(viewId_);
            break;
        }
        case K_SHOW: {
            view.SetVisible(BoolOf(attrValue));
            break;
        }
        case K_REF: {
            uint16_t length = 0;
            char *refName = MallocStringOf(attrValue, &length);
            if (refName == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "failed to set `ref` attribute.");
                return false;
            }
            if (length != 0) {
                jerry_value_t refs = jerryx_get_property_str(viewModel_, ATTR_REFS);
                if (jerry_value_is_undefined(refs)) {
                    jerry_release_value(refs);
                    refs = jerry_create_object();
                    jerryx_set_property_str(viewModel_, ATTR_REFS, refs);
                }
                jerryx_set_property_str(refs, refName, nativeElement_);
                jerry_release_value(refs);
            }
            ace_free(refName);
            refName = nullptr;
            break;
        }
        default: {
            // this is not error case, just no one get matched
            return false;
        }
    }

    return true;
}

void Component::ApplyStyles(const jerry_value_t options, Component &curr) const
{
    if (jerry_value_is_undefined(options)) {
        return;
    }
    styleManager_->ApplyComponentStyles(options, curr);
}

void Component::GetDimensionFromStyle(Dimension &dimension, const AppStyleItem &styleItem) const
{
    if (styleItem.GetValueType() == STYLE_PROP_VALUE_TYPE_PERCENT) {
        // percent format
        dimension.value.percentage = styleItem.GetPercentValue();
        dimension.type = DimensionType::TYPE_PERCENT;
        return;
    }
    // number or string format
    // use INT32_MIN as the impossible default value
    int32_t pixelValue = GetStylePixelValue(&styleItem, INT32_MIN);
    if (pixelValue == INT32_MIN) {
        // get pixel failed, reset to unknown type
        dimension.type = DimensionType::TYPE_UNKNOWN;
        return;
    }
    dimension.value.pixel = (int16_t)(pixelValue);
    dimension.type = DimensionType::TYPE_PIXEL;
}

void Component::CalculateDimensionPixel(Dimension &dimension, int16_t base) const
{
    const uint8_t hundred = 100;
    if (dimension.type == DimensionType::TYPE_PERCENT) {
        dimension.value.pixel = (int16_t)((dimension.value.percentage * base) / hundred);
        dimension.type = DimensionType::TYPE_PIXEL;
    }
}

const Dimension &Component::GetDimension(uint16_t keyNameId) const
{
    const static Dimension unknownDimension = {0, DimensionType::TYPE_UNKNOWN};
    switch (keyNameId) {
        case K_WIDTH:
            return width_;
        case K_HEIGHT:
            return height_;
        case K_TOP:
            return top_;
        case K_LEFT:
            return left_;
        case K_MARGIN_TOP:
            return marginTop_;
        case K_MARGIN_BOTTOM:
            return marginBottom_;
        case K_MARGIN_RIGHT:
            return marginRight_;
        case K_MARGIN_LEFT:
            return marginLeft_;
        default: {
            return unknownDimension;
        }
    }
}

void Component::AlignDimensions(const ConstrainedParameter &param)
{
    // width
    CalculateDimensionPixel(width_, param.maxWidth);
    CalculateDimensionPixel(height_, param.maxHeight);
    // top & left
    CalculateDimensionPixel(top_, param.maxHeight);
    CalculateDimensionPixel(left_, param.maxWidth);
    // margin
    CalculateDimensionPixel(marginTop_, param.maxHeight);
    CalculateDimensionPixel(marginLeft_, param.maxHeight);
    CalculateDimensionPixel(marginRight_, param.maxHeight);
    CalculateDimensionPixel(marginBottom_, param.maxHeight);
    // notify
    OnDimensionsAligned();
}

void Component::GetConstrainedParam(ConstrainedParameter &param) const
{
    param.maxWidth = width_.value.pixel;
    param.maxHeight = height_.value.pixel;
}

void Component::ApplyAlignedMargin(UIView &uiView) const
{
    if (marginTop_.type == DimensionType::TYPE_PIXEL) {
        uiView.SetStyle(STYLE_MARGIN_TOP, marginTop_.value.pixel);
    }
    if (marginBottom_.type == DimensionType::TYPE_PIXEL) {
        uiView.SetStyle(STYLE_MARGIN_BOTTOM, marginBottom_.value.pixel);
    }
    if (marginLeft_.type == DimensionType::TYPE_PIXEL) {
        uiView.SetStyle(STYLE_MARGIN_LEFT, marginLeft_.value.pixel);
    }
    if (marginRight_.type == DimensionType::TYPE_PIXEL) {
        uiView.SetStyle(STYLE_MARGIN_RIGHT, marginRight_.value.pixel);
    }
}

void Component::ApplyAlignedPosition(UIView &uiView) const
{
    if (top_.type == DimensionType::TYPE_PIXEL) {
        uiView.SetY(top_.value.pixel);
    }
    if (left_.type == DimensionType::TYPE_PIXEL) {
        uiView.SetX(left_.value.pixel);
    }
}

void Component::AdapteBoxRectArea(UIView &uiView) const
{
    // set view height and width
    uint8_t borderNum = 2;
    int16_t height = (height_.type == DimensionType::TYPE_PIXEL) ? height_.value.pixel : -1;
    int16_t width = (width_.type == DimensionType::TYPE_PIXEL) ? width_.value.pixel : -1;
    if (height >= 0) {
        // as uiView->GetStyle(STYLE_PADDING_TOP) and uiView->GetStyle(STYLE_PADDING_BOTTOM) is defined
        // as uint16_t, so do not need to judge whether less than 0
        if (uiView.GetStyle(STYLE_BORDER_WIDTH) < 0) {
            HILOG_WARN(HILOG_MODULE_ACE, "border and padding size should not less than 0");
        }
        int16_t contentHeight = height - (uiView.GetStyle(STYLE_BORDER_WIDTH) * borderNum) -
                                uiView.GetStyle(STYLE_PADDING_TOP) - uiView.GetStyle(STYLE_PADDING_BOTTOM);
        if (contentHeight <= 0) {
            HILOG_WARN(HILOG_MODULE_ACE,
                "component height can not include padding and border width, padding and border will be set 0");
            uiView.SetStyle(STYLE_BORDER_WIDTH, 0);
            uiView.SetStyle(STYLE_PADDING_TOP, 0);
            uiView.SetStyle(STYLE_PADDING_BOTTOM, 0);
            uiView.SetHeight(height);
        } else {
            uiView.SetHeight(contentHeight);
        }
    }
    if (width >= 0) {
        if (uiView.GetStyle(STYLE_BORDER_WIDTH) < 0) {
            HILOG_WARN(HILOG_MODULE_ACE, "border and padding size should not less than 0");
        }
        int16_t contentWidth = width - (uiView.GetStyle(STYLE_BORDER_WIDTH) * borderNum) -
                               uiView.GetStyle(STYLE_PADDING_LEFT) - uiView.GetStyle(STYLE_PADDING_RIGHT);
        if (contentWidth <= 0) {
            HILOG_WARN(HILOG_MODULE_ACE,
                "component width can not include padding and border width, padding and border will be set 0");
            uiView.SetStyle(STYLE_BORDER_WIDTH, 0);
            uiView.SetStyle(STYLE_PADDING_LEFT, 0);
            uiView.SetStyle(STYLE_PADDING_RIGHT, 0);
            uiView.SetWidth(width);
        } else {
            uiView.SetWidth(contentWidth);
        }
    }
}

bool Component::AdaptBoxSizing() const
{
    UIView *uiView = GetComponentRootView();
    if (uiView == nullptr) {
        return false;
    }
    // apply aligned top and left
    ApplyAlignedPosition(*uiView);
    // apply aligned magin
    ApplyAlignedMargin(*uiView);
    // adjust the box sizing
    AdapteBoxRectArea(*uiView);
    return true;
}

bool Component::ApplyStyle(const AppStyleItem *style)
{
    UIView *uiView = GetComponentRootView();
    if (uiView == nullptr) {
        return false;
    }

    // Try private styles first
    bool applyResult = ApplyPrivateStyle(style);
    if (applyResult) {
        // one private style get matched, no need to try private style ones
        return true;
    }
    return ApplyCommonStyle(*uiView, style);
}

/*
 * support common style items:
 *  [left|top]: number                       # flex layout not work
 * 'width': number,
 * 'height': number,
 * 'margin': number,                         # flex layout work
 * 'border-width': number,
 * 'border-color':number,
 * 'border-radius': number,
 * 'background-color': number,
 * 'opacity': number,
 * 'visibility': bool,
 *
 * not suport common style item:
 * border-[left|top|right|bottom]-width,
 * border-[left|top|right|bottom]-color,
 * border-[top|bottom]-[left|right]-radius,
 * border-style,
 * padding:number,
 * right|bottom
 */
bool Component::ApplyCommonStyle(UIView &view, const AppStyleItem *style)
{
    uint16_t styleNameId = GetStylePropNameId(style);
    if (!KeyParser::IsKeyValid(styleNameId)) {
        return false;
    }

    // we do not support pseudo class for all styles, child must handle itself
    if (style->IsPseudoClassItem()) {
        return false;
    }

    switch (styleNameId) {
        case K_HEIGHT: {
            GetDimensionFromStyle(height_, *style);
            break;
        }
        case K_WIDTH: {
            GetDimensionFromStyle(width_, *style);
            break;
        }
        case K_DISPLAY: {
            SetVisible(view, style);
            break;
        }
        case K_MARGIN: {
            GetDimensionFromStyle(marginBottom_, *style);
            GetDimensionFromStyle(marginLeft_, *style);
            GetDimensionFromStyle(marginRight_, *style);
            GetDimensionFromStyle(marginTop_, *style);
            break;
        }
        case K_MARGIN_BOTTOM: {
            GetDimensionFromStyle(marginBottom_, *style);
            break;
        }
        case K_MARGIN_LEFT: {
            GetDimensionFromStyle(marginLeft_, *style);
            break;
        }
        case K_MARGIN_RIGHT: {
            GetDimensionFromStyle(marginRight_, *style);
            break;
        }
        case K_MARGIN_TOP: {
            GetDimensionFromStyle(marginTop_, *style);
            break;
        }
        case K_PADDING:
            SetPadding(view, *style);
            break;
        case K_PADDING_BOTTOM:
            SetBottomPadding(view, *style);
            break;
        case K_PADDING_LEFT:
            SetLeftPadding(view, *style);
            break;
        case K_PADDING_RIGHT:
            SetRightPadding(view, *style);
            break;
        case K_PADDING_TOP: {
            SetTopPadding(view, *style);
            break;
        }
        case K_BORDER_BOTTOM_WIDTH:
        case K_BORDER_LEFT_WIDTH:
        case K_BORDER_RIGHT_WIDTH:
        case K_BORDER_TOP_WIDTH:
        case K_BORDER_WIDTH: {
            SetBorderWidth(view, *style);
            break;
        }
        case K_BORDER_BOTTOM_COLOR:
        case K_BORDER_LEFT_COLOR:
        case K_BORDER_RIGHT_COLOR:
        case K_BORDER_TOP_COLOR:
        case K_BORDER_COLOR: {
            SetBorderColor(view, *style);
            break;
        }
        case K_BORDER_RADIUS: {
            SetBorderRadius(view, *style);
            break;
        }
        case K_BACKGROUND_COLOR: {
            SetBackgroundColor(view, *style);
            break;
        }
        case K_LEFT: {
            GetDimensionFromStyle(left_, *style);
            break;
        }
        case K_TOP: {
            GetDimensionFromStyle(top_, *style);
            break;
        }
        case K_ANIMATION_DURATION: {
            SetAnimationStyle(view, style, K_ANIMATION_DURATION);
            break;
        }
        case K_ANIMATION_TIMING_FUNCTION: {
            SetAnimationStyle(view, style, K_ANIMATION_TIMING_FUNCTION);
            break;
        }
        case K_ANIMATION_FILL_MODE: {
            SetAnimationStyle(view, style, K_ANIMATION_FILL_MODE);
            break;
        }
        case K_ANIMATION_DELAY: {
            SetAnimationStyle(view, style, K_ANIMATION_DELAY);
            break;
        }
        case K_ANIMATION_ITERATION_COUNT: {
            SetAnimationStyle(view, style, K_ANIMATION_ITERATION_COUNT);
            break;
        }
        case K_ANIMATION_NAME: {
            SetAnimationKeyFrames(view, style);
            break;
        }
        case K_OPACITY: {
            SetOpacity(view, *style);
            break;
        }
        default: {
            return false;
        }
    }
    return true;
}

void Component::Invalidate()
{
    UIView *view = GetComponentRootView();
    if (view == nullptr) {
        return;
    }
    view->Invalidate();
}

void Component::ParseOptions()
{
    if (JSUndefined::Is(options_)) {
        HILOG_WARN(HILOG_MODULE_ACE, "options is null");
        return;
    }

    if (!JSObject::Is(options_)) {
        HILOG_WARN(HILOG_MODULE_ACE, "options is not a object type.");
        return;
    }

    START_TRACING(RENDER_PARSE_ATTR);
    ParseAttrs();
    STOP_TRACING();
    START_TRACING(RENDER_PARSE_EVENT);
    ParseEvents();
    STOP_TRACING();
}

void Component::SetAnimationKeyFrames(const UIView &view, const AppStyleItem *styleItem)
{
    if (trans_ == nullptr) {
        trans_ = new TransitionParams();
        if (trans_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create TransitionParams object error");
            return;
        }
    }

    const char * const value = GetStyleStrValue(styleItem);
    if (value == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "animation name is not string value");
        return;
    }

    const AppStyleSheet *styleSheet = GetStyleManager()->GetStyleSheet();
    if (styleSheet == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "styleSheet must set when you set animation attribute");
        return;
    }
    AppStyle *style = styleSheet->GetStyleFromKeyFramesSelectors(value);

    if (style) {
        const AppStyleItem *item = style->GetFirst();
        if (item == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "keyFrame style is not set!");
            return;
        }
        SetAnimationKeyFrames(item);
    }
}

void Component::SetAnimationKeyFrames(const AppStyleItem *item)
{
    if (item == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "keyFrame style is not set!");
        return;
    }

    const int8_t animatorFrom = 1;
    const int8_t animatorTo = 2;

    isAnimationKeyFramesSet_ = false;
    while (item) {
        const char * const itemValue = item->GetStrValue();

        if ((itemValue == nullptr) || (strlen(itemValue) == 0)) {
            return;
        }
        size_t valLength = strlen(itemValue);
        if ((valLength == 0) || (valLength >= UINT8_MAX)) {
            item = item->GetNext();
            continue;
        }
        char *animationValue = reinterpret_cast<char *>(ace_malloc(sizeof(char) * (valLength + 1)));
        if (animationValue == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "malloc animationValue memory heap failed.");
            return;
        }
        if (memcpy_s(animationValue, valLength, itemValue, valLength) != 0) {
            ace_free(animationValue);
            animationValue = nullptr;
            return;
        }
        animationValue[valLength] = '\0';
        int32_t valueTo;
        int32_t valueFrom;
        int16_t keyId = item->GetPropNameId();
        if (keyId == K_OPACITY) {
            valueTo = GetAnimatorValue(animationValue, animatorTo, true);
            valueFrom = GetAnimatorValue(animationValue, animatorFrom, true);
        } else {
            valueTo = GetAnimatorValue(animationValue, animatorTo);
            valueFrom = GetAnimatorValue(animationValue, animatorFrom);
        }
        ace_free(animationValue);
        animationValue = nullptr;
        SetAnimationKeyFrames(keyId, valueFrom, valueTo);
        item = item->GetNext();
    }
}

void Component::SetAnimationKeyFrames(int16_t keyId, int32_t valueFrom, int32_t valueTo)
{
    switch (keyId) {
        case K_ROTATE:
            trans_->transformType = const_cast<char *>(TRANSITION_ROTATE);
            trans_->transform_from = valueFrom;
            trans_->transform_to = valueTo;
            isAnimationKeyFramesSet_ = true;
            break;
        case K_TRANSLATE_X:
            trans_->transformType = const_cast<char *>(TRANSITION_TRANSFORM_X);
            trans_->transform_from = valueFrom;
            trans_->transform_to = valueTo;
            isAnimationKeyFramesSet_ = true;
            break;
        case K_TRANSLATE_Y:
            trans_->transformType = const_cast<char *>(TRANSITION_TRANSFORM_Y);
            trans_->transform_from = valueFrom;
            trans_->transform_to = valueTo;
            isAnimationKeyFramesSet_ = true;
            break;
        case K_HEIGHT:
            trans_->height_from = valueFrom;
            trans_->height_to = valueTo;
            isAnimationKeyFramesSet_ = true;
            break;
        case K_WIDTH:
            trans_->width_from = valueFrom;
            trans_->width_to = valueTo;
            isAnimationKeyFramesSet_ = true;
            break;
        case K_BACKGROUND_COLOR:
            trans_->background_color_from = valueFrom;
            trans_->background_color_to = valueTo;
            isAnimationKeyFramesSet_ = true;
            break;
        case K_OPACITY:
            trans_->opacity_from = valueFrom;
            trans_->opacity_to = valueTo;
            isAnimationKeyFramesSet_ = true;
            break;
        default:
            break;
    }
}

void Component::SetAnimationStyle(const UIView &view, const AppStyleItem *styleItem, const int16_t keyId)
{
    // special for "animation-iteration-count" which value could be a number or string "infinite"
    if ((styleItem == nullptr) || (!const_cast<AppStyleItem *>(styleItem)->UpdateNumValToStr())) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SetAnimationStyle fail");
        return;
    }
    if (trans_ == nullptr) {
        trans_ = new TransitionParams();
        if (trans_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create TransitionParams object error");
            return;
        }
    }

    const char * const strValue = GetStyleStrValue(styleItem);
    const size_t strLen = GetStyleStrValueLen(styleItem);
    if (strValue == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "animation style item is null");
        return;
    }
    switch (keyId) {
        case K_ANIMATION_DURATION: {
            if (!IsStyleValueTypeString(styleItem)) {
                HILOG_ERROR(HILOG_MODULE_ACE, "style animation during value is invalid!");
                return;
            }
            trans_->during = ParseToMilliseconds(strValue);
            break;
        }
        case K_ANIMATION_TIMING_FUNCTION: {
            uint16_t animationTimingKeyId = KeyParser::ParseKeyId(strValue, strLen);
            switch (animationTimingKeyId) {
                case K_EASE_IN:
                    trans_->easing = EasingType::EASE_IN;
                    break;
                case K_EASE_OUT:
                    trans_->easing = EasingType::EASE_OUT;
                    break;
                case K_EASE_IN_OUT:
                    trans_->easing = EasingType::EASE_IN_OUT;
                    break;
                default:
                    trans_->easing = EasingType::LINEAR;
                    break;
            }
            break;
        }
        case K_ANIMATION_FILL_MODE: {
            uint16_t animationFillKeyId = KeyParser::ParseKeyId(strValue, strLen);
            switch (animationFillKeyId) {
                case K_FORWARDS:
                    trans_->fill = OptionsFill::FORWARDS;
                    break;
                default:
                    trans_->fill = OptionsFill::FNONE;
                    break;
            }
            break;
        }
        case K_ANIMATION_DELAY: {
            if (!IsStyleValueTypeString(styleItem)) {
                HILOG_ERROR(HILOG_MODULE_ACE, "style animation delay value is invalid!");
                return;
            }
            trans_->delay = ParseToMilliseconds(strValue);
            break;
        }
        case K_ANIMATION_ITERATION_COUNT: {
            if (!IsStyleValueTypeString(styleItem)) {
                HILOG_ERROR(HILOG_MODULE_ACE, "style iteration count value is invalid!");
                return;
            }
            trans_->iterations = TransitionImpl::GetNumIterations(strValue);
            break;
        }
        default:
            break;
    }
}

void Component::AddAnimationToList(const TransitionImpl *transitionImpl) const
{
    AnimationsNode *animation = new AnimationsNode();
    if (animation == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create animation node error in startAnimation");
        return;
    }
    animation->transitionImpl = const_cast<TransitionImpl *>(transitionImpl);
    animation->next = g_animationListHead;
    g_animationListHead = animation;
}

void Component::RecordAnimation()
{
    if (trans_ == nullptr) {
        return;
    }

    if (trans_->during > 0 && isAnimationKeyFramesSet_) {
        UIView *uiView = GetComponentRootView();
        if (uiView) {
            curTransitionImpl_ = new TransitionImpl(*trans_, uiView);
            if (curTransitionImpl_ == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "create transitionImpl error");
                isAnimationKeyFramesSet_ = false;
                return;
            }
            curTransitionImpl_->Init();
            AddAnimationToList(curTransitionImpl_);
            isAnimationKeyFramesSet_ = false;
            // special for "if" situation, if g_isAnimatorStarted is started means the page has started all the
            // animators, and the current component is created by "if" situation, so the animator can start immediately
            if (g_isAnimatorStarted) {
                curTransitionImpl_->Start();
            }
        }
    }
}

void Component::StartAnimation()
{
    if (trans_ == nullptr) {
        return;
    }

    if (trans_->during > 0 && isAnimationKeyFramesSet_) {
        UIView *uiView = GetComponentRootView();
        if (uiView) {
            if (curTransitionImpl_ != nullptr) {
                curTransitionImpl_->Stop();
            }
            curTransitionImpl_ = new TransitionImpl(*trans_, uiView);
            if (curTransitionImpl_ == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "create transitionImpl error!");
                isAnimationKeyFramesSet_ = false;
                return;
            }
            curTransitionImpl_->Init();
            AddAnimationToList(curTransitionImpl_);
            curTransitionImpl_->Start();
            isAnimationKeyFramesSet_ = false;
        }
    }
}

void Component::ReleaseTransitionParam()
{
    if (trans_) {
        delete trans_;
        trans_ = nullptr;
    }
}

int32_t Component::GetAnimatorValue(char *animatorValue, const int8_t index, bool isOpacity) const
{
    if ((animatorValue == nullptr) || (strlen(animatorValue) == 0) || (strlen(animatorValue) >= UINT8_MAX)) {
        return 0;
    }
    const int8_t animatorfrom = 1;
    const int8_t animatorTo = 2;
    if ((index != animatorfrom) && (index != animatorTo)) {
        return 0;
    }

    char *next = nullptr;
    // try to get from part
    char *value = strtok_s(animatorValue, ANIMATION_VALUE_SEP, &next);
    if (index == animatorTo) {
        // get to part then if needed
        if (value != nullptr) {
            value = strtok_s(nullptr, ANIMATION_VALUE_SEP, &next);
        }
    }
    if (value == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetAnimatorValue strtok_s failed.");
        return 0;
    }

    long convertedValue = isOpacity ? (long)((strtod(value, nullptr) * ALPHA_MAX)) : strtol(value, nullptr, DEC);
    if (TransitionImpl::IsEndWith(value, "rad")) {
        uint8_t degConversionRate = 57;
        convertedValue = convertedValue * degConversionRate;
    }
    return convertedValue;
}

jerry_value_t Component::AddWatcherItem(const jerry_value_t attrKey, const jerry_value_t attrValue, bool isLazyLoading)
{
#ifdef FEATURE_LAZY_LOADING_MODULE
    isLazyLoading = true;
#endif
    jerry_value_t options = jerry_create_object();
    JerrySetNamedProperty(options, ARG_WATCH_EL, nativeElement_);
    JerrySetNamedProperty(options, ARG_WATCH_ATTR, attrKey);
    jerry_value_t watcher = CallJSWatcher(attrValue, WatcherCallbackFunc, options);
    jerry_value_t propValue = UNDEFINED;
    if (IS_UNDEFINED(watcher) || jerry_value_is_error(watcher)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to create Watcher instance.");
    } else {
        InsertWatcherCommon(watchersHead_, watcher);
        if (!isLazyLoading) {
            propValue = jerryx_get_property_str(watcher, "_lastValue");
        }
    }
    jerry_release_value(options);
    return propValue;
}

void Component::ParseAttrs()
{
    jerry_value_t attrs = jerryx_get_property_str(options_, ATTR_ATTRS);
    if (jerry_value_is_undefined(attrs)) {
        return;
    }

    jerry_value_t attrKeys = jerry_get_object_keys(attrs);
    if (jerry_value_is_undefined(attrKeys)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "None attributes to parse.");
        jerry_release_value(attrs);
        return;
    }

    uint16_t length = jerry_get_array_length(attrKeys);
    uint16_t attrKeyStrLength = 0;
    for (uint32_t index = 0; index < length; ++index) {
        jerry_value_t attrKey = jerry_get_property_by_index(attrKeys, index);
        jerry_value_t attrValue = jerry_get_property(attrs, attrKey);
        jerry_value_t newAttrValue = attrValue;

        if (jerry_value_is_function(attrValue)) {
            START_TRACING_WITH_COMPONENT_NAME(SET_ATTR_PARSE_EXPRESSION, componentName_);
#ifdef FEATURE_LAZY_LOADING_MODULE
            newAttrValue = CallJSFunction(attrValue, viewModel_, nullptr, 0);
            JsAppContext *context = JsAppContext::GetInstance();
            LazyLoadManager *lazyLoadManager = const_cast<LazyLoadManager *>(context->GetLazyLoadManager());
            lazyLoadManager->AddLazyLoadWatcher(nativeElement_, attrKey, attrValue);
#else
            newAttrValue = AddWatcherItem(attrKey, attrValue);
#endif
            STOP_TRACING();
        }

        char *attrKeyStr = MallocStringOf(attrKey, &attrKeyStrLength);
        if (attrKeyStr != nullptr) {
            if (attrKeyStrLength != 0) {
                uint16_t attrKeyId = KeyParser::ParseKeyId(attrKeyStr, attrKeyStrLength);
                // ignore the return result for no need to invalided views here
                START_TRACING_WITH_EXTRA_INFO(SET_ATTR_SET_TO_NATIVE, componentName_, attrKeyId);
                SetAttribute(attrKeyId, newAttrValue);
                STOP_TRACING();
            }
            ace_free(attrKeyStr);
            attrKeyStr = nullptr;
        }
        if (newAttrValue != attrValue) {
            // the new value has been calculated out by ParseExpression, need to be released
            jerry_release_value(newAttrValue);
        }
        ReleaseJerryValue(attrKey, attrValue, VA_ARG_END_FLAG);
    }
    jerry_release_value(attrKeys);
    jerry_release_value(attrs);
}

void Component::SetClickEventListener(UIView &view, const jerry_value_t eventFunc, bool isStopPropagation)
{
    onClickListener_ = new ViewOnClickListener(eventFunc, isStopPropagation);
    if (onClickListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "click listener create failed");
        return;
    }

    view.SetOnClickListener(onClickListener_);
    view.SetTouchable(true);
}

#ifdef JS_TOUCH_EVENT_SUPPORT
void Component::SetTouchStartEventListener(UIView &view, jerry_value_t eventFunc, uint16_t eventTypeId)
{
    onTouchStartListener_ = new ViewOnTouchStartListener(eventFunc, eventTypeId);
    if (onTouchStartListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "touch move event listener create failed");
        return;
    }

    view.SetOnTouchListener(onTouchStartListener_);
    view.SetTouchable(true);
}

void Component::SetTouchMoveEventListener(UIView &view, jerry_value_t eventFunc, uint16_t eventTypeId)
{
    onTouchMoveListener_ = new ViewOnTouchMoveListener(eventFunc, eventTypeId);
    if (onTouchMoveListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "touch start event listener create failed");
        return;
    }

    view.SetOnDragListener(onTouchMoveListener_);
    view.SetTouchable(true);
    view.SetDraggable(true);
}

void Component::SetTouchCancelEventListener(UIView &view, jerry_value_t eventFunc, uint16_t eventTypeId)
{
    onTouchCancelListener_ = new ViewOnTouchCancelListener(eventFunc, eventTypeId);
    if (onTouchCancelListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "touch cancel event listener create failed");
        return;
    }

    view.SetOnTouchListener(onTouchCancelListener_);
    view.SetTouchable(true);
    view.SetDraggable(true);
}

void Component::SetTouchEndEventListener(UIView &view, jerry_value_t eventFunc, uint16_t eventTypeId)
{
    onTouchEndListener_ = new ViewOnTouchEndListener(eventFunc, eventTypeId);
    if (onTouchEndListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "touch end event listener create failed");
        return;
    }

    view.SetOnTouchListener(onTouchEndListener_);
    view.SetTouchable(true);
}

void Component::SetKeyBoardEventListener(jerry_value_t eventFunc, uint16_t eventTypeId)
{
    RootView *rootView = RootView::GetInstance();
    if (rootView == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "get rootView is nullptr");
        return;
    }
    keyBoardEventListener_ = new KeyBoardEventListener(eventFunc, eventTypeId);
    if (keyBoardEventListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "on key borard event listener create failed");
        return;
    }
    rootView->SetOnKeyActListener(keyBoardEventListener_);
}
#endif

void Component::SetLongPressEventListener(UIView &view, const jerry_value_t eventFunc, bool isStopPropagation)
{
    onLongPressListener_ = new ViewOnLongPressListener(eventFunc, isStopPropagation);
    if (onLongPressListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "long press listener create failed");
        return;
    }

    view.SetOnLongPressListener(onLongPressListener_);
    view.SetTouchable(true);
}

void Component::SetSwipeEventListener(UIView &view, jerry_value_t eventFunc, bool isStopPropagation)
{
    onSwipeListener_ = new ViewOnSwipeListener(eventFunc, isStopPropagation);
    if (onSwipeListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "swipe listener create failed");
        return;
    }

    view.SetOnDragListener(onSwipeListener_);
    view.SetDraggable(true);
    view.SetTouchable(true);
}

// default implementation
bool Component::RegisterEventListener(uint16_t eventTypeId, jerry_value_t funcValue, bool isStopPropagation)
{
    if (!KeyParser::IsKeyValid(eventTypeId) || IS_UNDEFINED(funcValue)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "register event listener failed cause by invalid attribute name or value.");
        return false;
    }

    UIView *uiView = GetComponentRootView();
    if (uiView == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "register event listener failed cause by empty view.");
        return false;
    }

    bool registerResult = RegisterPrivateEventListener(eventTypeId, funcValue, isStopPropagation);
    if (registerResult) {
        return true;
    }

    return RegisterCommonEventListener(*uiView, eventTypeId, funcValue, isStopPropagation);
}

bool Component::RegisterCommonEventListener(UIView &view,
                                            const uint16_t eventTypeId,
                                            const jerry_value_t funcValue,
                                            bool isStopPropagation)
{
    switch (eventTypeId) {
        case K_CLICK: {
            SetClickEventListener(view, funcValue, isStopPropagation);
            break;
        }
        case K_LONGPRESS: {
            SetLongPressEventListener(view, funcValue, isStopPropagation);
            break;
        }
        case K_SWIPE: {
            SetSwipeEventListener(view, funcValue, isStopPropagation);
            break;
        }
#ifdef JS_TOUCH_EVENT_SUPPORT
        case K_KEY: {
            SetKeyBoardEventListener(funcValue, eventTypeId);
            break;
        }
        case K_TOUCHSTART: {
            SetTouchStartEventListener(view, funcValue, eventTypeId);
            break;
        }
        case K_TOUCHCANCEL: {
            SetTouchCancelEventListener(view, funcValue, eventTypeId);
            break;
        }
        case K_TOUCHMOVE: {
            SetTouchMoveEventListener(view, funcValue, eventTypeId);
            break;
        }
        case K_TOUCHEND: {
            SetTouchEndEventListener(view, funcValue, eventTypeId);
            break;
        }
#endif
        default: {
            return false;
        }
    }
    return true;
}

void Component::ReleaseCommonEventListeners()
{
    ACE_DELETE(onClickListener_);
    ACE_DELETE(onLongPressListener_);
#ifdef JS_TOUCH_EVENT_SUPPORT
    ACE_DELETE(keyBoardEventListener_);
    ACE_DELETE(onTouchStartListener_);
    ACE_DELETE(onTouchCancelListener_);
    ACE_DELETE(onTouchMoveListener_);
    ACE_DELETE(onTouchEndListener_);
#endif
    ACE_DELETE(onSwipeListener_);
}

void Component::AppendDescriptorOrElements(Component *parent, const JSValue descriptorOrElements)
{
    if (!JSUndefined::Is(descriptorOrElements)) {
        uint16_t size = JSArray::Length(descriptorOrElements);
        for (uint16_t idx = 0; idx < size; ++idx) {
            JSValue descriptorOrElement = JSArray::Get(descriptorOrElements, idx);
            AppendDescriptorOrElement(parent, descriptorOrElement);
            JSRelease(descriptorOrElement);
        }
    }
}

void Component::InvalidateIfNeeded(uint16_t attrKeyId, bool invalidateSelf) const
{
    UIView *uiView = GetComponentRootView();
    if ((uiView == nullptr) || !KeyParser::IsKeyValid(attrKeyId)) {
        return;
    }

    if (attrKeyId == K_HEIGHT || attrKeyId == K_WIDTH || attrKeyId == K_MARGIN || attrKeyId == K_MARGIN_BOTTOM ||
        attrKeyId == K_MARGIN_LEFT || attrKeyId == K_MARGIN_RIGHT || attrKeyId == K_MARGIN_TOP ||
        attrKeyId == K_PADDING || attrKeyId == K_PADDING_BOTTOM || attrKeyId == K_PADDING_LEFT ||
        attrKeyId == K_PADDING_RIGHT || attrKeyId == K_PADDING_TOP || attrKeyId == K_BORDER_BOTTOM_WIDTH ||
        attrKeyId == K_BORDER_LEFT_WIDTH || attrKeyId == K_BORDER_RIGHT_WIDTH || attrKeyId == K_BORDER_TOP_WIDTH ||
        attrKeyId == K_BORDER_WIDTH || attrKeyId == K_BORDER_RADIUS || attrKeyId == K_LEFT || attrKeyId == K_TOP) {
        if (invalidateSelf) {
            uiView->Invalidate();
            return;
        }
        UIView *parent = uiView->GetParent();
        if (parent != nullptr) {
            parent->LayoutChildren(true);
        }
    }
}

void Component::ParseEvents()
{
    /*
     New JS bundle:
     _c('div', {
         catchBubbleEvents: {
             longpress: _vm.handleLongPress
         },
         onBubbleEvents: {
             swipe: _vm.handleSwipe
         }
     });

     The events bound to 'catchBubbleEvents' field should be stoped propagation,
     but the events bound to 'onBubbleEvents' field should not.

     Old JS bundle:
     _('div', {
         on: {
             click: _vm.handleClick
         },
     });
     The old framework does not support the event bubble mechanism.
     Therefore, the events bound to 'on' field are processed as bound to 'onBubbleEvents' field.
    */
    BindEvents("on", true);
    BindEvents("catchBubbleEvents", true);
    BindEvents("onBubbleEvents", false);
}

void Component::BindEvents(const char *type, bool isStopPropagation)
{
    JSValue events = JSObject::Get(options_, type);
    if (JSUndefined::Is(events)) {
        JSRelease(events);
        return;
    }
    JSValue keys = JSObject::Keys(events);
    if (JSUndefined::Is(keys)) {
        JSRelease(keys);
        JSRelease(events);
        return;
    }
    uint16_t length = JSArray::Length(keys);
    if (length == 0) {
        JSRelease(keys);
        JSRelease(events);
        return;
    }
    for (uint16_t idx = 0; idx < length; ++idx) {
        JSValue key = JSArray::Get(keys, idx);
        JSValue func = JSObject::Get(events, key);

        uint16_t keyLength = 0;
        char *keyName = MallocStringOf(key, &keyLength);
        if (keyLength != 0) {
            uint16_t keyId = KeyParser::ParseKeyId(keyName, keyLength);
            if (!RegisterEventListener(keyId, func, isStopPropagation)) {
                HILOG_ERROR(HILOG_MODULE_ACE, "Register event listener error.");
            }
        }
        ACE_FREE(keyName);
        JSRelease(func);
        JSRelease(key);
    }
    JSRelease(keys);
    JSRelease(events);
}

void Component::SetVisible(UIView &view, const AppStyleItem *styleItem) const
{
    if (!IsStyleValueTypeString(styleItem)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Visible style value is invalid!");
        return;
    }
    const char * const strValue = GetStyleStrValue(styleItem);
    if (strValue == nullptr) {
        return;
    }

    uint16_t valueId = KeyParser::ParseKeyId(strValue, GetStyleStrValueLen(styleItem));
    view.SetVisible(valueId != K_NONE);
}

void Component::SetBackgroundColor(UIView &view, const AppStyleItem &styleItem) const
{
    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;

    if (GetStyleColorValue(&styleItem, color, alpha)) {
        ColorType backgroundRGBColor = GetRGBColor(color);
        view.SetStyle(STYLE_BACKGROUND_COLOR, backgroundRGBColor.full);
        view.SetStyle(STYLE_BACKGROUND_OPA, alpha);
    }
}

void Component::SetOpacity(UIView &view, const AppStyleItem &styleItem) const
{
    if (styleItem.GetValueType() != STYLE_PROP_VALUE_TYPE_FLOATING) {
        return;
    }
    double opacity = styleItem.GetFloatingValue();
    const double opacityMin = 0.0;
    const double opacityMax = 1.0;
    if (opacity < opacityMin) {
        opacity = opacityMin;
    } else if (opacity > opacityMax) {
        opacity = opacityMax;
    }
    view.SetOpaScale(opacity * OPA_OPAQUE);
}

void Component::SetMargin(UIView &view) const
{
    SetLeftMargin(view);
    SetTopMargin(view);
    SetRightMargin(view);
    SetBottomMargin(view);
}

void Component::SetLeftMargin(UIView &view) const
{
    if (marginLeft_.type == DimensionType::TYPE_PIXEL) {
        view.SetStyle(STYLE_MARGIN_LEFT, marginLeft_.value.pixel);
    }
}

void Component::SetTopMargin(UIView &view) const
{
    if (marginTop_.type == DimensionType::TYPE_PIXEL) {
        view.SetStyle(STYLE_MARGIN_TOP, marginTop_.value.pixel);
    }
}

void Component::SetRightMargin(UIView &view) const
{
    if (marginRight_.type == DimensionType::TYPE_PIXEL) {
        view.SetStyle(STYLE_MARGIN_RIGHT, marginRight_.value.pixel);
    }
}

void Component::SetBottomMargin(UIView &view) const
{
    if (marginBottom_.type == DimensionType::TYPE_PIXEL) {
        view.SetStyle(STYLE_MARGIN_BOTTOM, marginBottom_.value.pixel);
    }
}

void Component::SetPadding(UIView &view, const AppStyleItem &styleItem) const
{
    SetLeftPadding(view, styleItem);
    SetTopPadding(view, styleItem);
    SetRightPadding(view, styleItem);
    SetBottomPadding(view, styleItem);
}

void Component::SetLeftPadding(UIView &view, const AppStyleItem &styleItem) const
{
    int32_t paddingLeft = GetStylePixelValue(&styleItem);
    if (paddingLeft >= 0) {
        view.SetStyle(STYLE_PADDING_LEFT, paddingLeft);
    }
}

void Component::SetTopPadding(UIView &view, const AppStyleItem &styleItem) const
{
    int32_t paddingTop = GetStylePixelValue(&styleItem);
    if (paddingTop >= 0) {
        view.SetStyle(STYLE_PADDING_TOP, paddingTop);
    }
}

void Component::SetRightPadding(UIView &view, const AppStyleItem &styleItem) const
{
    int32_t paddingRight = GetStylePixelValue(&styleItem);
    if (paddingRight >= 0) {
        view.SetStyle(STYLE_PADDING_RIGHT, paddingRight);
    }
}

void Component::SetBottomPadding(UIView &view, const AppStyleItem &styleItem) const
{
    int32_t paddingBottom = GetStylePixelValue(&styleItem);
    if (paddingBottom >= 0) {
        view.SetStyle(STYLE_PADDING_BOTTOM, paddingBottom);
    }
}

void Component::SetBorderColor(UIView &view, const AppStyleItem &styleItem) const
{
    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    if (GetStyleColorValue(&styleItem, color, alpha)) {
        view.SetStyle(STYLE_BORDER_COLOR, GetRGBColor(color).full);
        view.SetStyle(STYLE_BORDER_OPA, alpha);
    }
}

void Component::SetBorderRadius(UIView &view, const AppStyleItem &styleItem) const
{
    view.SetStyle(STYLE_BORDER_RADIUS, GetStylePixelValue(&styleItem));
}

void Component::SetBorderWidth(UIView &view, const AppStyleItem &styleItem) const
{
    view.SetStyle(STYLE_BORDER_WIDTH, GetStylePixelValue(&styleItem));
}

jerry_value_t Component::SetListForWatcher(jerry_value_t getter, jerry_value_t children)
{
    jerry_value_t options = jerry_create_object();
    JerrySetNamedProperty(options, ARG_WATCH_EL, nativeElement_);

    jerry_value_t watcher = CallJSWatcher(getter, ListForWatcherCallbackFunc, options);
    if (IS_UNDEFINED(watcher) || jerry_value_is_error(watcher)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to create ListForWatcher instance.");
    } else {
        InsertWatcherCommon(watchersHead_, watcher);
    }
    jerry_release_value(options);
    return UNDEFINED;
}

void Component::HandleListForDireactive()
{
    uint16_t childrenLength = jerry_get_array_length(descriptors_);
    for (uint16_t index = 0; index < childrenLength; index++) {
        jerry_value_t child = jerry_get_property_by_index(descriptors_, index);
        jerry_value_t getterName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(DESCRIPTOR_ATTR_GETTER));
        // add watcher to the array which the getter function returned
        if (JerryHasProperty(child, getterName)) {
            jerry_value_t getter = jerry_get_property(child, getterName);
            SetListForWatcher(getter, descriptors_);
            jerry_release_value(getter);
        }
        ReleaseJerryValue(getterName, child, VA_ARG_END_FLAG);
    }
}

void Component::AppendChildren(Component *parent)
{
    if (JSUndefined::Is(descriptors_)) {
        return;
    }

    children_ = JSArray::Create(0);
    uint16_t size = JSArray::Length(descriptors_);
    for (uint16_t index = 0; index < size; ++index) {
        JSValue descriptorOrElement = JSArray::Get(descriptors_, index);
        if (!JSUndefined::Is(descriptorOrElement)) {
            bool isDescriptor = AppendDescriptorOrElement(parent, descriptorOrElement);
            if (isDescriptor) {
                CreateDirectiveWatcher(descriptorOrElement);
            }
        }
        JSRelease(descriptorOrElement);
    }
}

bool Component::AppendDescriptorOrElement(Component *parent, const jerry_value_t descriptorOrElement)
{
    if (DescriptorUtils::IsIfDescriptor(descriptorOrElement)) {
        AppendIfDescriptor(parent, descriptorOrElement);
        return true;
    }

    if (DescriptorUtils::IsForDescriptor(descriptorOrElement)) {
        AppendForDescriptor(parent, descriptorOrElement);
        return true;
    }
    AppendElement(parent, descriptorOrElement);
    return false;
}

void Component::AppendIfDescriptor(Component *parent, const jerry_value_t descriptor)
{
    bool isShown = DescriptorUtils::IsIfDescriptorShown(descriptor);
    if (isShown) {
        JSValue decriptorOrElement = DescriptorUtils::GetDescriptorRendered(descriptor);
        if (!JSUndefined::Is(decriptorOrElement)) {
            AppendDescriptorOrElement(parent, decriptorOrElement);
            JSRelease(decriptorOrElement);
        } else {
            // Don't release decriptorOrElement
            // because decriptorOrElement is the result of jerry_create_object but jerry_get_property
            decriptorOrElement = DescriptorUtils::RenderIfDescriptor(descriptor);
            AppendDescriptorOrElement(parent, decriptorOrElement);

            // does decriptorOrElement need to be release if decriptorOrElement is descriptor
        }
    } else {
        DescriptorUtils::DelIfDescriptorRendered(descriptor);
    }
}
void Component::AppendForDescriptor(Component *parent, const jerry_value_t descriptor)
{
    JSValue descriptorOrelements = DescriptorUtils::GetDescriptorRendered(descriptor);
    if (!JSUndefined::Is(descriptorOrelements)) {
        AppendDescriptorOrElements(parent, descriptorOrelements);
        JSRelease(descriptorOrelements);
    } else {
        // Don't release decriptorOrElements
        // because decriptorOrElements is the result of jerry_create_object but jerry_get_property
        descriptorOrelements = DescriptorUtils::RenderForDescriptor(descriptor);
        AppendDescriptorOrElements(parent, descriptorOrelements);
    }
}
void Component::AppendElement(Component *parent, const jerry_value_t element)
{
    if (parent == nullptr) {
        return;
    }
    Component *component = nullptr;
    if (!JSObject::GetNativePointer(element, reinterpret_cast<void **>(&component))) {
        // if get binding component native pointer failed from a child element, just release that element
        HILOG_ERROR(HILOG_MODULE_ACE, "fatal error, no component is binded to the child element, not allowed.");
        // try to release this element and its children, it means we drop them all
        DescriptorUtils::ReleaseDescriptorOrElement(element);
        return;
    }
    JSArray::Push(children_, element);
    parent->AddChild(component);
}

/*
 * NOTE: add one child will not attach the native view immediately, but
 * when removing one child, the child native view will be detached immediately
 */
void Component::AddChild(Component *childNode)
{
    if (childNode == nullptr) {
        return;
    }

    if (childHead_ == nullptr) {
        childNode->SetParent(this);
        childNode->SetNextSibling(nullptr);
        childHead_ = childNode;
        return;
    }

    // find the tail
    Component *temp = childHead_;
    while ((temp != nullptr) && (temp->GetNextSibling() != nullptr)) {
        if (temp == childNode) {
            // already added in the list, drop
            return;
        }
        temp = const_cast<Component *>(temp->GetNextSibling());
    }
    childNode->SetParent(this);
    temp->SetNextSibling(childNode);
    childNode->SetNextSibling(nullptr);
}

void Component::RemoveChild(Component *childNode)
{
    if ((childNode == nullptr) || (childHead_ == nullptr)) {
        return;
    }

    UIView *childNativeView = childNode->GetComponentRootView();
    UIViewGroup *parentView = reinterpret_cast<UIViewGroup *>(GetComponentRootView());
    if (childNativeView == nullptr || parentView == nullptr) {
        return;
    }

    if (childNode == childHead_) {
        // it is the head
        Component *next = const_cast<Component *>(childHead_->GetNextSibling());
        childNode->SetNextSibling(nullptr);
        childNode->SetParent(nullptr);
        childHead_ = next;
        parentView->Remove(childNativeView);
        return;
    }

    // find the target node's pre one
    Component *temp = childHead_;
    while (temp != nullptr) {
        if (temp->GetNextSibling() == childNode) {
            // found it
            break;
        }
        temp = const_cast<Component *>(temp->GetNextSibling());
    }
    if (temp == nullptr) {
        // not found
        return;
    }

    // the head itself is the last one
    if (temp == childHead_) {
        childHead_ = nullptr;
    }
    temp->SetNextSibling(nullptr);
    childNode->SetNextSibling(nullptr);
    childNode->SetParent(nullptr);
    parentView->Remove(childNativeView);
}

void Component::RemoveAllChildren()
{
    while (childHead_ != nullptr) {
        RemoveChild(childHead_);
    }
}

void Component::CreateDirectiveWatcher(jerry_value_t descriptor)
{
    JSValue watcher = DescriptorUtils::CreateDescriptorWatcher(nativeElement_, descriptor);
    if (!JSUndefined::Is(watcher)) {
        InsertWatcherCommon(watchersHead_, watcher);
    }
}

bool Component::IsAttached() const
{
    UIView *nativeView = GetComponentRootView();
    if (nativeView == nullptr) {
        return false;
    }
    return (nativeView->GetParent() != nullptr);
}

void Component::BuildViewTree(Component *currComponent, Component *parent, ConstrainedParameter &parentParameter)
{
    if (currComponent == nullptr) {
        return;
    }

    // align self dimension
    currComponent->AlignDimensions(parentParameter);
    // refresh rect (border box sizing -> content box sizing)
    currComponent->AdaptBoxSizing();
    // attach to parent, and avoid attaching repeatly
    if (parent != nullptr && !currComponent->IsAttached()) {
        parent->AttachView(currComponent);
        // notify view has been attached to tree, it means the parent's size is already calculated out
        currComponent->OnViewAttached();
    }

    Component *child = const_cast<Component *>(currComponent->GetChildHead());
    if (child == nullptr) {
        return;
    }
    // the child only can layout in the content area of parent
    ConstrainedParameter alignParameter;
    currComponent->GetConstrainedParam(alignParameter);
    while (child != nullptr) {
        BuildViewTree(child, currComponent, alignParameter);
        child = const_cast<Component *>(child->GetNextSibling());
    }
    // consider how to avoid layout repeatly, for example: div - div -- div
    currComponent->LayoutChildren();
}

void Component::HandleChildrenChange(jerry_value_t descriptor)
{
    RemoveAllChildren();
    if (!JSUndefined::Is(children_)) {
        JSRelease(children_);
        children_ = JSArray::Create(0);
    }

    uint16_t size = JSArray::Length(descriptors_);
    for (uint16_t idx = 0; idx < size; ++idx) {
        JSValue descriptorOrElement = JSArray::Get(descriptors_, idx);
        if (IS_UNDEFINED(descriptorOrElement)) {
            continue;
        }
        if (descriptorOrElement == descriptor) {
            UpdateDescriptor(this, descriptorOrElement);
        } else {
            ReappendDescriptorOrElement(this, descriptorOrElement);
        }
        JSRelease(descriptorOrElement);
    }

    Component *parent = const_cast<Component *>(GetParent());
    ConstrainedParameter parentParam;
    Component *target = (parent == nullptr) ? this : parent;
    target->GetConstrainedParam(parentParam);
    BuildViewTree(this, nullptr, parentParam);
    target->LayoutChildren();
    target->Invalidate();
}

void Component::UpdateDescriptor(Component *parent, const jerry_value_t descriptor)
{
    if (DescriptorUtils::IsIfDescriptor(descriptor)) {
        AppendIfDescriptor(parent, descriptor);
    } else if (DescriptorUtils::IsForDescriptor(descriptor)) {
        // Release descriptor last rendered
        DescriptorUtils::DelForDescriptorRendered(descriptor);

        // Re-render descriptor
        JSValue descriptorOrElements = DescriptorUtils::RenderForDescriptor(descriptor);
        AppendDescriptorOrElements(parent, descriptorOrElements);
    } else {
        // never
    }
}

void Component::ReappendDescriptorOrElement(Component *parent, const jerry_value_t descriptor)
{
    if (DescriptorUtils::IsIfDescriptor(descriptor)) {
        JSValue descriptorOrElement = DescriptorUtils::GetDescriptorRendered(descriptor);
        if (!JSUndefined::Is(descriptorOrElement)) {
            AppendDescriptorOrElement(parent, descriptor);
        }
        JSRelease(descriptorOrElement);
    } else if (DescriptorUtils::IsForDescriptor(descriptor)) {
        JSValue descriptorOrElements = DescriptorUtils::GetDescriptorRendered(descriptor);
        if (!JSUndefined::Is(descriptorOrElements)) {
            AppendDescriptorOrElements(parent, descriptorOrElements);
        }
        JSRelease(descriptorOrElements);
    } else {
        AppendElement(parent, descriptor);
    }
}

int32_t Component::GetStylePixelValue(const AppStyleItem *style, int32_t defaultValue) const
{
    if (style->GetValueType() == STYLE_PROP_VALUE_TYPE_NUMBER) {
        return style->GetNumValue();
    }
    if (style->GetValueType() == STYLE_PROP_VALUE_TYPE_STRING) {
        if (style->GetStrValue() == nullptr) {
            HILOG_WARN(HILOG_MODULE_ACE, "Get Style PixelValue failed, return default value!");
            return defaultValue;
        }
        return strtol(style->GetStrValue(), nullptr, DEC);
    }
    return defaultValue;
}

int16_t Component::GetStyleDegValue(const AppStyleItem *style, int16_t defaultValue) const
{
    if (style->GetValueType() == STYLE_PROP_VALUE_TYPE_NUMBER) {
        return style->GetNumValue();
    }
    if (style->GetValueType() == STYLE_PROP_VALUE_TYPE_STRING) {
        if (style->GetStrValue() == nullptr) {
            HILOG_WARN(HILOG_MODULE_ACE, "Get Style DegValue failed, return default value!");
            return defaultValue;
        }
        return strtol(style->GetStrValue(), nullptr, DEC);
    }
    return defaultValue;
}

bool Component::GetStyleColorValue(const AppStyleItem *style, uint32_t &color, uint8_t &alpha) const
{
    if (style->GetValueType() == STYLE_PROP_VALUE_TYPE_NUMBER) {
        color = style->GetNumValue();
        alpha = OPA_OPAQUE;
        return true;
    }
    if (style->GetValueType() == STYLE_PROP_VALUE_TYPE_STRING) {
        return ParseColor(style->GetStrValue(), color, alpha);
    }
    HILOG_ERROR(HILOG_MODULE_ACE, "invalid color format!");
    return false;
}

bool Component::HandleBackgroundImg(const AppStyleItem &styleItem, char *&pressedImage, char *&normalImage) const
{
    bool result = false;
    if (styleItem.GetValueType() == STYLE_PROP_VALUE_TYPE_STRING) {
        const char * const url = styleItem.GetStrValue();
        char *filePath = CreatePathStrFromUrl(url);
        if (filePath != nullptr) {
            char *imagePath = JsAppContext::GetInstance()->GetResourcePath(filePath);
            if (imagePath == nullptr) {
                ace_free(filePath);
                filePath = nullptr;
                return result;
            }
#ifdef OHOS_ACELITE_PRODUCT_WATCH
            // convert .png/jpeg/bmp to .bin subfix
            CureImagePath(imagePath);
#endif // OHOS_ACELITE_PRODUCT_WATCH
            if ((styleItem.GetPseudoClassType() == PSEUDO_CLASS_ACTIVE) ||
                (styleItem.GetPseudoClassType() == PSEUDO_CLASS_CHECKED)) {
                // in case we don't free the buffer after using
                ACE_FREE(pressedImage);
                pressedImage = imagePath;
            } else {
                // in case we don't free the buffer after using
                ACE_FREE(normalImage);
                normalImage = imagePath;
            }
            ace_free(filePath);
            filePath = nullptr;
            result = true;
        }
    }
    return result;
}

#ifdef FEATURE_ROTATION_API
jerry_value_t Component::HandleRotationRequest(const jerry_value_t func,
                                               const jerry_value_t dom,
                                               const jerry_value_t args[],
                                               const jerry_length_t size)
{
    UNUSED(func);
    UIView *bindedView = ComponentUtils::GetViewFromBindingObject(dom);
    if (bindedView == nullptr) {
        return UNDEFINED;
    }

    // default action is to request the focus even user do not pass the argument
    bool focusRequest = true;
    if (args != nullptr && size > 0) {
        if (!JerryGetBoolProperty(args[0], ATTR_NAME_FOCUS, focusRequest)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "not bool argument passed, will clear the focus!");
            focusRequest = false;
        }
    }
    if (focusRequest) {
        bindedView->RequestFocus();
    } else {
        bindedView->ClearFocus();
    }

    return UNDEFINED;
}
#endif // FEATURE_ROTATION_API
} // namespace ACELite
} // namespace OHOS
