/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "circle_progress_component.h"

namespace OHOS {
namespace ACELite {
CircleProgressComponent::CircleProgressComponent(jerry_value_t options,
                                                 jerry_value_t children, AppStyleManager *styleManager)
    : Component(options, children, styleManager),
    centerX_(INT16_MAX),
    centerY_(INT16_MAX),
    radius_(INT16_MAX),
    startAngle_(DEFAULT_START_ANGLE),
    totalAngle_(DEFAULT_TOTAL_ANGLE)
{
}

bool CircleProgressComponent::CreateNativeViews()
{
    // set default value
    progressView_.SetBackgroundStyle(StyleDefault::GetBrightStyle());
    const int8_t defaultLineWidth = 32;
    progressView_.SetBackgroundStyle(STYLE_LINE_WIDTH, defaultLineWidth); // Compatible with rich devices
    progressView_.SetBackgroundStyle(STYLE_LINE_CAP, CapType::CAP_ROUND);
    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    const char * const defaultBackgroundColor = "rgba(255,255,255,0.15)";
    if (ParseColor(defaultBackgroundColor, color, alpha)) {
        progressView_.SetBackgroundStyle(STYLE_LINE_COLOR, GetRGBColor(color).full);
    }
    progressView_.SetForegroundStyle(StyleDefault::GetBrightColorStyle());
    progressView_.SetForegroundStyle(STYLE_BACKGROUND_OPA, 0);
    progressView_.SetForegroundStyle(STYLE_BORDER_OPA, 0);
    progressView_.SetForegroundStyle(STYLE_LINE_WIDTH, defaultLineWidth);
    progressView_.SetForegroundStyle(STYLE_LINE_CAP, CapType::CAP_ROUND);
    const char * const defaultColor = "#5EA1FF";
    if (ParseColor(defaultColor, color, alpha)) {
        progressView_.SetForegroundStyle(STYLE_LINE_COLOR, GetRGBColor(color).full);
        progressView_.SetForegroundStyle(STYLE_LINE_OPA, OPA_OPAQUE);
    }
    return true;
}

bool CircleProgressComponent::ApplyPrivateStyle(const AppStyleItem *style)
{
    return SetArcProgressStyle(style);
}

UIView *CircleProgressComponent::GetComponentRootView() const
{
    return const_cast<UICircleProgress *>(&progressView_);
}

bool CircleProgressComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    switch (attrKeyId) {
        case K_PERCENT: {
            // set the specific attribute of this progress view
            int16_t rangeMax = 100;
            int16_t rangeMin = 0;
            progressView_.SetRange(rangeMax, rangeMin);
            progressView_.SetValue(IntegerOf(attrValue));
            return true;
        }
        default: {
            return false;
        }
    }
}

void CircleProgressComponent::SetAngles()
{
    if (centerX_ == INT16_MAX) {
        centerX_ = static_cast<uint16_t>(progressView_.GetWidth()) >> 1;
    }
    if (centerY_ == INT16_MAX) {
        centerY_ = static_cast<uint16_t>(progressView_.GetHeight()) >> 1;
    }
    if (radius_ == INT16_MAX) {
        if (progressView_.GetWidth() <= progressView_.GetHeight()) {
            radius_ = static_cast<uint16_t>(progressView_.GetWidth()) >> 1;
        } else {
            radius_ = static_cast<uint16_t>(progressView_.GetHeight()) >> 1;
        }
    }

    progressView_.SetCenterPosition(centerX_, centerY_);
    progressView_.SetRadius(radius_);
    progressView_.SetStartAngle(startAngle_);
    progressView_.SetEndAngle(startAngle_ + totalAngle_);
}

void CircleProgressComponent::SetStartAngle(const AppStyleItem *style)
{
    const int16_t minStartAngle = 0;
    const int16_t maxStartAngle = 360;
    startAngle_ = GetStyleDegValue(style); // Compatible with rich devices, value should between -180 and 0
    if (startAngle_ > maxStartAngle) {
        startAngle_ = maxStartAngle;
    } else if (startAngle_ < minStartAngle) {
        startAngle_ = minStartAngle;
    }
}

void CircleProgressComponent::SetTotalAngle(const AppStyleItem *style)
{
    const int16_t minStartAngle = -360;
    const int16_t maxStartAngle = 360;
    totalAngle_ = GetStyleDegValue(style); // Compatible with rich devices, value should between 0 and 180
    if (totalAngle_ < minStartAngle) {
        totalAngle_ = minStartAngle;
    } else if (totalAngle_ > maxStartAngle) {
        totalAngle_ = maxStartAngle;
    }
}

bool CircleProgressComponent::SetArcColor(const AppStyleItem *style)
{
    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    if (!GetStyleColorValue(style, color, alpha)) {
        return false;
    }
    progressView_.SetForegroundStyle(STYLE_LINE_COLOR, GetRGBColor(color).full);
    progressView_.SetForegroundStyle(STYLE_LINE_OPA, alpha);
    return true;
}

bool CircleProgressComponent::SetArcBackgroundColor(const AppStyleItem *style)
{
    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    if (!GetStyleColorValue(style, color, alpha)) {
        return false;
    }
    progressView_.SetBackgroundStyle(STYLE_LINE_COLOR, GetRGBColor(color).full);
    progressView_.SetBackgroundStyle(STYLE_LINE_OPA, alpha);
    return true;
}

bool CircleProgressComponent::SetArcProgressStyle(const AppStyleItem *style)
{
    uint16_t stylePropNameId = GetStylePropNameId(style);
    switch (stylePropNameId) {
        case K_CENTER_X: {
            centerX_ = GetStylePixelValue(style);
            return true;
        }
        case K_CENTER_Y: {
            centerY_ = GetStylePixelValue(style);
            return true;
        }
        case K_RADIUS: {
            radius_ = GetStylePixelValue(style);
            return true;
        }
        case K_START_ANGLE: {
            SetStartAngle(style);
            return true;
        }
        case K_TOTAL_ANGLE: {
            SetTotalAngle(style);
            return true;
        }
        case K_COLOR: {
            return SetArcColor(style);
        }
        case K_STROKE_WIDTH: {
            progressView_.SetBackgroundStyle(STYLE_LINE_WIDTH, GetStylePixelValue(style));
            progressView_.SetForegroundStyle(STYLE_LINE_WIDTH, GetStylePixelValue(style));
            return true;
        }
        case K_BACKGROUND_COLOR: {
            return SetArcBackgroundColor(style);
        }
        default: {
            return false;
        }
    }
}

void CircleProgressComponent::OnViewAttached()
{
    HandleExtraUpdate();
}

void CircleProgressComponent::PostUpdate(uint16_t attrKeyId)
{
    UNUSED(attrKeyId);
    HandleExtraUpdate();
}

void CircleProgressComponent::HandleExtraUpdate()
{
    SetAngles();
}
} // namespace ACELite
} // namespace OHOS
