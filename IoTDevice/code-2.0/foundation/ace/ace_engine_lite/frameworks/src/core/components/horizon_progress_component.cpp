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

#include "horizon_progress_component.h"

namespace OHOS {
namespace ACELite {
HorizonProgressComponent::HorizonProgressComponent(jerry_value_t options,
                                                   jerry_value_t children, AppStyleManager *styleManager)
    : Component(options, children, styleManager),
    hStrokeWidth_(0)
{
}

bool HorizonProgressComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    if (attrKeyId == K_PERCENT) {
        int16_t rangeMax = 100;
        int16_t rangeMin = 0;
        progressView_.SetRange(rangeMax, rangeMin);
        progressView_.SetValue(IntegerOf(attrValue));
        return true;
    }

    return false;
}

bool HorizonProgressComponent::ApplyPrivateStyle(const AppStyleItem *style)
{
    return SetHorizonProgressStyle(style);
}

bool HorizonProgressComponent::CreateNativeViews()
{
    // set default style for progress & background
    Style progressStyle = StyleDefault::GetBrightColorStyle();
    Style backStyle = StyleDefault::GetBrightStyle();
    progressView_.SetForegroundStyle(progressStyle);
    progressView_.SetBackgroundStyle(backStyle);

    // set default progress self border width
    progressView_.SetForegroundStyle(STYLE_BORDER_WIDTH, 0);
    progressView_.SetBackgroundStyle(STYLE_BORDER_WIDTH, 0);
    const double alpha = 0.15;
    progressView_.SetBackgroundStyle(STYLE_BACKGROUND_COLOR,
                                     Color::GetColorFromRGBA(0xFF, 0xFF, 0xFF, alpha * 0xFF).full);
    // set defaut progress stroke width & canvas width & canvas height & border width
    const int16_t width = 4;
    hStrokeWidth_ = width;
    return true;
}


UIView *HorizonProgressComponent::GetComponentRootView() const
{
    return const_cast<UIBoxProgress *>(&progressView_);
}

bool HorizonProgressComponent::SetHorizonProgressStyle(const AppStyleItem *style)
{
    uint16_t stylePropNameId = GetStylePropNameId(style);

    switch (stylePropNameId) {
        // Get stroke width.
        case K_STROKE_WIDTH: {
            hStrokeWidth_ = GetStylePixelValue(style);
            break;
        }
        // Set horizon progress style: color.
        case K_COLOR: {
            uint32_t color = 0;
            uint8_t alpha = OPA_OPAQUE;
            if (!GetStyleColorValue(style, color, alpha)) {
                return false;
            }
            progressView_.SetForegroundStyle(STYLE_BACKGROUND_COLOR, GetRGBColor(color).full);
            progressView_.SetForegroundStyle(STYLE_BACKGROUND_OPA, alpha);
            break;
        }
        case K_BACKGROUND_COLOR: {
            uint32_t color = 0;
            uint8_t alpha = OPA_OPAQUE;
            if (!GetStyleColorValue(style, color, alpha)) {
                return false;
            }
            progressView_.SetBackgroundStyle(STYLE_BACKGROUND_COLOR, GetRGBColor(color).full);
            progressView_.SetBackgroundStyle(STYLE_BACKGROUND_OPA, alpha);
            break;
        }
        default: {
            return false;
        }
    }
    return true;
}

void HorizonProgressComponent::OnViewAttached()
{
    HandleExtraUpdate();
}

void HorizonProgressComponent::PostUpdate(uint16_t attrKeyId)
{
    UNUSED(attrKeyId);
    HandleExtraUpdate();
}

void HorizonProgressComponent::HandleExtraUpdate()
{
    // set width & height of progress
    if (progressView_.GetWidth() < 0) {
        progressView_.SetValidWidth(0);
    } else {
        progressView_.SetValidWidth(progressView_.GetWidth());
    }
    progressView_.SetValidHeight(hStrokeWidth_);
}
} // namespace ACELite
} // namespace OHOS
