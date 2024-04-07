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

#ifndef UI_TEST_TRANSFORM_H
#define UI_TEST_TRANSFORM_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "layout/grid_layout.h"
#include "ui_test.h"

namespace OHOS {
class UITestTransform : public UITest, public UIView::OnClickListener {
public:
    UITestTransform() {}
    ~UITestTransform() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void SetUpButton(UILabelButton* btn, const char* title);

    bool OnClick(UIView& view, const ClickEvent& event) override;

    void UIKit_Transform_Test_Rotate_001();
    void UIKit_Transform_Test_Scale_002();
    void UIKit_Transform_Test_Translate_003();

private:
    UIScrollView* container_ = nullptr;
    GridLayout* layout_ = nullptr;
    UIImageView* imageView_ = nullptr;
    UIViewGroup* uiViewGroupFrame_ = nullptr;

    UILabelButton* rotateBtn_ = nullptr;
    UILabelButton* scaleBtn_ = nullptr;
    UILabelButton* translateBtn_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_TRANSFORM_H
