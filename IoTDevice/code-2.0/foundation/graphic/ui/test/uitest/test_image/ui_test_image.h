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

#ifndef UI_TEST_IMAGE_H
#define UI_TEST_IMAGE_H

#include "components/ui_scroll_view.h"
#include "imgdecode/cache_manager.h"
#include "ui_test.h"

#ifndef VERSION_LITE
#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "test_resource_config.h"
#endif

namespace OHOS {
#ifndef VERSION_LITE
class UITestImage : public UITest, public OHOS::UIView::OnClickListener {
#else
class UITestImage : public UITest {
#endif
public:
    UITestImage() {}
    ~UITestImage() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void UIKit_UIImage_Test_SetImage_001();
    void UIKit_UIImage_Test_SetImage_002();
    void UIKit_UIImage_Test_SetImage_004();
    void UIKit_UIImage_Test_SetImage_005();
    void UIKit_UIImage_Test_SetImage_006();
    void UIKit_UIImage_Test_SetImage_007();
    void UIKit_UIImage_Test_SetImage_008();
    void UIKit_UIImage_Test_SetImage_009();
    void UIKit_UIImage_Test_SetImage_010();
    void UIKit_UIImage_Test_SetImage_011();
    void UIKit_UIImage_Test_SetImage_012();
    void UIKit_UIImage_Test_SetImage_013();
    void UIKit_UIImage_Test_SetImage_014();
#ifndef VERSION_LITE
    void UIKit_UIImage_Test_SetImage_015();
    void UIKit_UIImage_Test_SetImage_016();
#endif
    void UIKit_UIImage_Test_Uncompress_001();

private:
    const char* GetCharByImageSrcType(ImageSrcType srcType) const;
#ifndef VERSION_LITE
    bool OnClick(UIView &view, const ClickEvent& event) override;
    void SetUpButton(UILabelButton* btn, const char* title);
#endif
    UIScrollView* container_ = nullptr;
#ifndef VERSION_LITE
    UIImageView* gifImageView_ = nullptr;
    UILabelButton* gifToGif_ = nullptr;
    UILabelButton* gifToJpeg_ = nullptr;
    UILabelButton* gifToPng_ = nullptr;
    UILabelButton* gifToBin01_ = nullptr;
    UILabelButton* gifToBin02_ = nullptr;
    UILabelButton* gifToBin03_ = nullptr;
    UILabelButton* gifToBin04_ = nullptr;
#endif
};
} // namespace OHOS
#endif // UI_TEST_IMAGE_H
