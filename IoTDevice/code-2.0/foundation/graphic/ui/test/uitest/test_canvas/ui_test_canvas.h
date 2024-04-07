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

#ifndef UI_TEST_CANVAS_H
#define UI_TEST_CANVAS_H

#include "components/ui_canvas.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "layout/grid_layout.h"
#include "ui_test.h"

namespace OHOS {
class UITestCanvas : public UITest {
public:
    UITestCanvas() {}
    ~UITestCanvas() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void UIKitCanvasTestDrawLine001();
    void UIKitCanvasTestDrawLine002();
    void UIKitCanvasTestDrawCurve001();
    void UIKitCanvasTestDrawCurve002();
    void UIKitCanvasTestDrawRect001();
    void UIKitCanvasTestDrawRect002();
    void UIKitCanvasTestDrawRect003();
    void UIKitCanvasTestDrawCircle001();
    void UIKitCanvasTestDrawCircle002();
    void UIKitCanvasTestDrawCircle003();
    void UIKitCanvasTestDrawArc001();
    void UIKitCanvasTestDrawImage001();
    void UIKitCanvasTestDrawLabel001();
    void UIKitCanvasTestDrawSector001();
    void UIKitCanvasTestClear001();
    void UIKitCanvasTestDrawPath001();
    void UIKitCanvasTestDrawPath002();
    void UIKitCanvasTestDrawPath003();
    void UIKitCanvasTestDrawPath004();
    void UIKitCanvasTestDrawPath005();
    void UIKitCanvasTestDrawPath006();
    void UIKitCanvasTestDrawPath007();
    void UIKitCanvasTestDrawPath008();
    void UIKitCanvasTestDrawPath009();
    void UIKitCanvasTestDrawPath010();
    void UIKitCanvasTestDrawPath011();
    void UIKitCanvasTestDrawPath012();
    void UIKitCanvasTestDrawPath013();
    void UIKitCanvasTestDrawPath014();
    void UIKitCanvasTestDrawPath015();
    void UIKitCanvasTestDrawPath016();
    void UIKitCanvasTestDrawPath017();
    void UIKitCanvasTestDrawPath018();
    void UIKitCanvasTestDrawPath019();
    void UIKitCanvasTestDrawPath020();
    void UIKitCanvasTestDrawPath021();
    void UIKitCanvasTestDrawPath022();
    void UIKitCanvasTestDrawPath023();
    void UIKitCanvasTestDrawPath024();
    void UIKitCanvasTestDrawPath025();
    void UIKitCanvasTestDrawPath026();
    void UIKitCanvasTestDrawPath027();
    void UIKitCanvasTestDrawPath028();
    void UIKitCanvasTestDrawPath029();
    void UIKitCanvasTestDrawPath030();
    void UIKitCanvasTestDrawPath031();
    void UIKitCanvasTestDrawPath032();
    void UIKitCanvasTestDrawPath033();
    void UIKitCanvasTestDrawPath034();

private:
    const static int16_t GAP = 10;
    const static int16_t TITLE_HEIGHT = 29;
    const static uint16_t CANVAS_WIDTH = 454;
    const static uint16_t CANVAS_HEIGHT = 200;

    static constexpr int16_t START1_X = 10;
    static constexpr int16_t START1_Y = 10;
    static constexpr int16_t START2_X = 80;
    static constexpr int16_t START2_Y = 10;
    static constexpr int16_t LINE1_X = 40;
    static constexpr int16_t LINE1_Y = 100;
    static constexpr int16_t LINE2_X = 100;
    static constexpr int16_t LINE2_Y = 120;
    static constexpr int16_t CENTER_X = 150;
    static constexpr int16_t CENTER_Y = 150;
    static constexpr int16_t RADIUS = 50;
    static constexpr int16_t START_ANGLE = 30;
    static constexpr int16_t END_ANGLE = 250;
    static constexpr int16_t RECT_X = 250;
    static constexpr int16_t RECT_Y = 50;
    static constexpr int16_t RECT_WIDTH = 100;
    static constexpr int16_t RECT_HEIGHT = 50;
    void CreateTitleLabel(const char* title);
    UICanvas* CreateCanvas();

    UIScrollView* container_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_CANVAS_H
