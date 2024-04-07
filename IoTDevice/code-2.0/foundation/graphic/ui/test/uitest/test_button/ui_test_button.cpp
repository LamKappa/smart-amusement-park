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

#include "ui_test_button.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_digital_clock.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_radio_button.h"
#include "components/ui_toggle_button.h"
#include "font/ui_font.h"
#include "test_resource_config.h"

namespace OHOS {
void UITestBUTTON::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
    }
}

void UITestBUTTON::TearDown()
{
    if (clickEnableVisiableListener_ != nullptr) {
        delete clickEnableVisiableListener_;
        clickEnableVisiableListener_ = nullptr;
    }
    if (clickDisableVisiableListener_ != nullptr) {
        delete clickDisableVisiableListener_;
        clickDisableVisiableListener_ = nullptr;
    }
    if (clickEnableTouchableListener_ != nullptr) {
        delete clickEnableTouchableListener_;
        clickEnableTouchableListener_ = nullptr;
    }
    if (clickDisableTouchableListener_ != nullptr) {
        delete clickDisableTouchableListener_;
        clickDisableTouchableListener_ = nullptr;
    }
    if (clickColorToWhiteListener_ != nullptr) {
        delete clickColorToWhiteListener_;
        clickColorToWhiteListener_ = nullptr;
    }
    if (clickColorToRedListener_ != nullptr) {
        delete clickColorToRedListener_;
        clickColorToRedListener_ = nullptr;
    }
    if (clickRevetColorListener_ != nullptr) {
        delete clickRevetColorListener_;
        clickRevetColorListener_ = nullptr;
    }
    if (clickRevetToOriginListener_ != nullptr) {
        delete clickRevetToOriginListener_;
        clickRevetToOriginListener_ = nullptr;
    }
    if (clickBigListener_ != nullptr) {
        delete clickBigListener_;
        clickBigListener_ = nullptr;
    }
    if (clickLeftListener_ != nullptr) {
        delete clickLeftListener_;
        clickLeftListener_ = nullptr;
    }
    if (clickRightListener_ != nullptr) {
        delete clickRightListener_;
        clickRightListener_ = nullptr;
    }
    if (clickUpListener_ != nullptr) {
        delete clickUpListener_;
        clickUpListener_ = nullptr;
    }
    if (clickDownListener_ != nullptr) {
        delete clickDownListener_;
        clickDownListener_ = nullptr;
    }
    if (clickSmallListener_ != nullptr) {
        delete clickSmallListener_;
        clickSmallListener_ = nullptr;
    }
    if (toggleChangeListener_ != nullptr) {
        delete toggleChangeListener_;
        toggleChangeListener_ = nullptr;
    }
    if (toggleChangeListener1_ != nullptr) {
        delete toggleChangeListener1_;
        toggleChangeListener1_ = nullptr;
    }
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestBUTTON::GetTestView()
{
    UIKit_Check_Box_Test_001();
    UIKit_Radio_Button_Test_001();
    UIKit_Toggle_Button_Test_001();
    UIKit_Check_Box_Test_002();
    UIKit_Radio_Button_Test_002();
    UIKit_Toggle_Button_Test_002();
    UIKit_Button_Test_001();
    return container_;
}

void UITestBUTTON::UIKit_Check_Box_Test_001() const
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 5, // 5: y-coordinate
            Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("checkbox功能");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UICheckBox* checkbox = new UICheckBox();
        checkbox->SetPosition(30, 30); // 30: x-coordinate, 30: y-coordinate

        UICheckBox* checkbox2 = new UICheckBox();
        checkbox2->SetPosition(100, 30); // 100: x-coordinate, 30: y-coordinate

        UICheckBox* checkbox3 = new UICheckBox();
        checkbox3->SetImages("", "");
        checkbox3->SetPosition(170, 30, 100, 100); // 170: x-coordinate, 30: y-coordinate, 100 : width, 100 : height

        container_->Add(checkbox);
        container_->Add(checkbox2);
        container_->Add(checkbox3);
    }
}

void UITestBUTTON::UIKit_Radio_Button_Test_001() const
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        // 0:x-coordinate, 130:y-coordinate, 20:new height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 130, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("radiobutton功能");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIRadioButton* radioButton = new UIRadioButton("aaa");
        radioButton->SetPosition(0, 10); // 0: x-coordinate, 10: y-coordinate

        UIRadioButton* radioButton2 = new UIRadioButton("aaa");
        radioButton2->SetPosition(50, 10); // 50: x-coordinate, 10: y-coordinate
        radioButton2->SetWidth(40); // 40: width
        radioButton2->SetHeight(30); // 30: height

        UIRadioButton* radioButton3 = new UIRadioButton("aaa");
        radioButton3->SetPosition(110, 10); // 110: x-coordinate, 10: y-coordinate
        radioButton3->SetWidth(80); // 80: width
        radioButton3->SetHeight(60); // 60: height

        UIRadioButton* radioButton4 = new UIRadioButton("aaa");
        radioButton4->SetPosition(10, 10); // 10: x-coordinate, 10: y-coordinate
        radioButton4->SetWidth(70); // 70: width
        radioButton4->SetHeight(90); // 90: width

        UIRadioButton* radioButton5 = new UIRadioButton("aaa");
        radioButton5->SetPosition(110, 10); // 110: x-coordinate, 10: y-coordinate
        radioButton5->SetWidth(-1); // -1: width
        radioButton5->SetHeight(90); // 90: width

        UIRadioButton* radioButton6 = new UIRadioButton("aaa");
        radioButton6->SetPosition(130, 10); // 130: x-coordinate, 10: y-coordinate
        radioButton6->SetWidth(0); // 0: width
        radioButton6->SetHeight(0); // 0: width

        UIRadioButton* radioButton7 = new UIRadioButton("aaa");
        radioButton7->SetPosition(140, 10); // 140: x-coordinate, 10: y-coordinate
        radioButton7->SetWidth(1000); // 1000: width
        radioButton7->SetHeight(50); // 50: width

        OHOS::UIViewGroup* viewGroup = new UIViewGroup();
        viewGroup->SetPosition(30, 150); // 30: x-coordinate, 150: y-coordinate
        viewGroup->SetWidth(Screen::GetInstance().GetWidth() / 2); // 2: half width
        viewGroup->SetHeight(Screen::GetInstance().GetHeight() / 2); // 2: half height

        OHOS::UIViewGroup* viewGroup2 = new UIViewGroup();
        // 40, increase width, 2: half width, 150: y-coordinate
        viewGroup2->SetPosition(40 + Screen::GetInstance().GetWidth() / 2, 150);
        viewGroup2->SetWidth(Screen::GetInstance().GetWidth() / 2); // 2: half width
        viewGroup2->SetHeight(Screen::GetInstance().GetHeight() / 2); // 2: half height

        viewGroup2->Add(radioButton4);
        viewGroup2->Add(radioButton5);
        viewGroup2->Add(radioButton6);
        viewGroup2->Add(radioButton7);

        viewGroup->Add(radioButton);
        viewGroup->Add(radioButton2);
        viewGroup->Add(radioButton3);
        viewGroup->SetStyle(STYLE_BACKGROUND_OPA, 0);

        container_->Add(viewGroup);
        container_->Add(viewGroup2);
    }
}

class TestBtnOnStateChangeListener : public OHOS::UICheckBox::OnChangeListener {
public:
    explicit TestBtnOnStateChangeListener(UIView* uiView)
    {
        uiView_ = uiView;
    }

    ~TestBtnOnStateChangeListener() {}

    bool OnChange(UICheckBox::UICheckBoxState state) override
    {
        int16_t xPos = uiView_->GetX();
        int16_t yPos = uiView_->GetY();
        uiView_->Invalidate();
        xPos += 10; // 10: increase xPos
        yPos += 10; // 10: increase yPos

        uiView_->SetX(xPos);
        uiView_->SetY(yPos);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView* uiView_;
};

void UITestBUTTON::UIKit_Toggle_Button_Test_001()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        // 0:x-coordinate, 270:y-coordinate, 20:new height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 270, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("togglebutton功能");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIToggleButton* togglebutton = new UIToggleButton();
        togglebutton->SetPosition(20, 300); // 20: x-coordinate, 300: y-coordinate
        togglebutton->SetStyle(STYLE_BACKGROUND_OPA, 0); // 0: opacity
        togglebutton->SetState(false);

        UIToggleButton* togglebutton2 = new UIToggleButton();
        togglebutton2->SetPosition(100, 300); // 100: x-coordinate, 300: y-coordinate
        togglebutton2->SetStyle(STYLE_BACKGROUND_OPA, 0); // 0: opacity
        togglebutton2->SetState(false);
        togglebutton2->SetWidth(60); // 60: width
        togglebutton2->SetHeight(40); // 40: height

        UIToggleButton* togglebutton3 = new UIToggleButton();
        togglebutton3->SetPosition(190, 300); // 190: x-coordinate, 300: y-coordinate
        togglebutton3->SetStyle(STYLE_BACKGROUND_OPA, 0); // 0: opacity
        togglebutton3->SetState(false);
        togglebutton3->SetWidth(50); // 50: width
        togglebutton3->SetHeight(70); // 70: height

        UIToggleButton* togglebutton4 = new UIToggleButton();

        if (toggleChangeListener_ == nullptr) {
            toggleChangeListener_ = static_cast<UICheckBox::OnChangeListener*>(
                new TestBtnOnStateChangeListener(reinterpret_cast<UIView*>(togglebutton4)));
        }
        togglebutton4->SetOnChangeListener(toggleChangeListener_);
        togglebutton4->SetPosition(250, 300); // 250: x-coordinate, 300: y-coordinate
        togglebutton4->SetStyle(STYLE_BACKGROUND_OPA, 0); // 0: opacity
        togglebutton4->SetState(false);
        togglebutton4->SetWidth(80); // 80: width
        togglebutton4->SetHeight(80); // 80: height
        togglebutton4->SetState(true);

        UIToggleButton* togglebutton5 = new UIToggleButton();
        togglebutton5->SetPosition(340, 300); // 340: x-coordinate, 300: y-coordinate
        togglebutton5->SetWidth(-1); // -1: width
        togglebutton5->SetHeight(90); // 90: width

        UIToggleButton* togglebutton6 = new UIToggleButton();
        togglebutton6->SetPosition(350, 300); // 350: x-coordinate, 300: y-coordinate
        togglebutton6->SetWidth(0); // 0: width
        togglebutton6->SetHeight(0); // 0: width

        UIToggleButton* togglebutton7 = new UIToggleButton();
        togglebutton7->SetPosition(360, 300); // 360: x-coordinate, 300: y-coordinate
        togglebutton7->SetWidth(1000); // 1000: width
        togglebutton7->SetHeight(50); // 50: width

        container_->Add(togglebutton);
        container_->Add(togglebutton2);
        container_->Add(togglebutton3);
        container_->Add(togglebutton4);
        container_->Add(togglebutton5);
        container_->Add(togglebutton6);
        container_->Add(togglebutton7);
    }
}
void UITestBUTTON::UIKit_Check_Box_Test_002() const
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        // 0:x-coordinate, 400:y-coordinate, 20:new height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 400, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("checkbox SetImage功能");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UICheckBox* checkbox = new UICheckBox();
        checkbox->SetPosition(30, 430); // 30: x-coordinate, 430: y-coordinate
        checkbox->SetWidth(130); // 130: width
        checkbox->SetHeight(130); // 130: height
        checkbox->SetImages(BLUE_IMAGE_PATH, YELLOW_IMAGE_PATH);

        UICheckBox* checkbox2 = new UICheckBox();
        checkbox2->SetPosition(200, 430); // 200: x-coordinate, 430: y-coordinate
        checkbox2->SetWidth(130); // 130: width
        checkbox2->SetHeight(130); // 130: height
        checkbox2->SetImages(GREEN_IMAGE_PATH, RED_IMAGE_PATH);

        container_->Add(checkbox);
        container_->Add(checkbox2);
    }
}

void UITestBUTTON::UIKit_Radio_Button_Test_002() const
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        // 0:x-coordinate, 570:y-coordinate, 20:new height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 570, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("radiobutton SetImage功能");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIRadioButton* radioButton = new UIRadioButton("aaa");
        radioButton->SetPosition(30, 10); // 30: x-coordinate, 10: y-coordinate
        radioButton->SetWidth(130); // 130: width
        radioButton->SetHeight(130); // 130: height
        radioButton->SetImages(BLUE_IMAGE_PATH, YELLOW_IMAGE_PATH);

        UIRadioButton* radioButton2 = new UIRadioButton("aaa");
        radioButton2->SetPosition(200, 10); // 200: x-coordinate, 10: y-coordinate
        radioButton2->SetWidth(130); // 130: width
        radioButton2->SetHeight(130); // 130: height
        radioButton2->SetImages(GREEN_IMAGE_PATH, RED_IMAGE_PATH);

        OHOS::UIViewGroup* viewGroup = new UIViewGroup();
        viewGroup->SetPosition(0, 600); // 0: x-coordinate, 600: y-coordinate
        viewGroup->SetWidth(Screen::GetInstance().GetWidth()); // 2: half width
        viewGroup->SetHeight(Screen::GetInstance().GetHeight() / 2); // 2: half height

        viewGroup->Add(radioButton);
        viewGroup->Add(radioButton2);
        viewGroup->SetStyle(STYLE_BACKGROUND_OPA, 0);

        container_->Add(viewGroup);
    }
}

void UITestBUTTON::UIKit_Toggle_Button_Test_002()
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        // 0:x-coordinate, 740:y-coordinate, 20:new height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 740, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText("togglebutton SetImage功能");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIToggleButton* togglebutton = new UIToggleButton();
        togglebutton->SetPosition(30, 770); // 30: x-coordinate, 770: y-coordinate
        togglebutton->SetStyle(STYLE_BACKGROUND_OPA, 0); // 0: opacity
        togglebutton->SetState(true);
        togglebutton->SetWidth(130); // 130: width
        togglebutton->SetHeight(130); // 130: height
        togglebutton->SetImages(BLUE_IMAGE_PATH, YELLOW_IMAGE_PATH);

        UIToggleButton* togglebutton2 = new UIToggleButton();
        if (toggleChangeListener1_ == nullptr) {
            toggleChangeListener1_ = static_cast<UICheckBox::OnChangeListener*>(
                new TestBtnOnStateChangeListener(reinterpret_cast<UIView*>(togglebutton2)));
        }
        togglebutton2->SetOnChangeListener(toggleChangeListener1_);
        togglebutton2->SetPosition(200, 770); // 200: x-coordinate, 770: y-coordinate
        togglebutton2->SetStyle(STYLE_BACKGROUND_OPA, 0); // 0: opacity
        togglebutton2->SetState(false);
        togglebutton2->SetWidth(130); // 130: width
        togglebutton2->SetHeight(130); // 130: height
        togglebutton2->SetImages(GREEN_IMAGE_PATH, RED_IMAGE_PATH);

        container_->Add(togglebutton);
        container_->Add(togglebutton2);
    }
}

class TestBtnOnClickShapeChangeListener : public UIView::OnClickListener {
public:
    TestBtnOnClickShapeChangeListener(UIView* uiView, int16_t grid) : uiView_(uiView), changeGrid_(grid) {}

    ~TestBtnOnClickShapeChangeListener() {}

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        int16_t width = uiView_->GetWidth() + changeGrid_;
        int16_t height = uiView_->GetHeight() + changeGrid_;
        if (changeGrid_ < 0) {
            uiView_->Invalidate();
        }
        uiView_->Resize(width, height);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView* uiView_;
    int16_t changeGrid_;
};

class TestBtnOnClickPositionChangeListener : public OHOS::UIView::OnClickListener {
public:
    enum class MoveType {
        MOVE_LEFT,
        MOVE_RIGHT,
        MOVE_TOP,
        MOVE_BOTTOM,
    };
    TestBtnOnClickPositionChangeListener(UIView* uiView, MoveType moveType, uint16_t grid)
        : uiView_(uiView), moveType_(moveType), grid_(grid)
    {
    }

    ~TestBtnOnClickPositionChangeListener() {}

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        int16_t xPos = uiView_->GetX();
        int16_t yPos = uiView_->GetY();
        uiView_->Invalidate();
        if (moveType_ == MoveType::MOVE_LEFT) {
            xPos = xPos - grid_;
        } else if (moveType_ == MoveType::MOVE_RIGHT) {
            xPos = xPos + grid_;
        } else if (moveType_ == MoveType::MOVE_TOP) {
            yPos = yPos - grid_;
        } else if (moveType_ == MoveType::MOVE_BOTTOM) {
            yPos = yPos + grid_;
        }
        uiView_->SetX(xPos);
        uiView_->SetY(yPos);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView* uiView_;
    MoveType moveType_;
    uint16_t grid_;
};

class TestBtnOnClickVisableListener : public OHOS::UIView::OnClickListener {
public:
    TestBtnOnClickVisableListener(UIView* uiView, bool visible) : uiView_(uiView), visible_(visible) {}

    ~TestBtnOnClickVisableListener() {}

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        uiView_->SetVisible(visible_);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView* uiView_;
    bool visible_;
};

class TestBtnOnClickTouchableListener : public UIView::OnClickListener {
public:
    TestBtnOnClickTouchableListener(UIView* uiView, bool touchable) : uiView_(uiView), touchable_(touchable) {}

    ~TestBtnOnClickTouchableListener() {}

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        uiView_->SetTouchable(touchable_);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView* uiView_;
    bool touchable_;
};

class TestBtnOnClickChangeColorListener : public OHOS::UIView::OnClickListener {
public:
    TestBtnOnClickChangeColorListener(UIView* uiView, uint16_t red, uint16_t green, uint16_t blue)
        : uiView_(uiView), red_(red), green_(green), blue_(blue)
    {
    }

    ~TestBtnOnClickChangeColorListener() {}

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        ColorType color;
        color.full = uiView_->GetStyle(STYLE_BACKGROUND_COLOR);
        color.red = red_;
        color.green = green_;
        color.blue = blue_;
        uiView_->SetStyle(STYLE_BACKGROUND_COLOR, color.full);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView* uiView_;
    uint16_t red_;
    uint16_t green_;
    uint16_t blue_;
};

class TestBtnOnClickRevertColorListener : public UIView::OnClickListener {
public:
    explicit TestBtnOnClickRevertColorListener(UIView* uiView) : uiView_(uiView), originColor_(0)
    {
        if (uiView != nullptr) {
            originColor_ = uiView->GetStyle(STYLE_BACKGROUND_COLOR);
        }
    }

    ~TestBtnOnClickRevertColorListener() {}

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        uiView_->SetStyle(STYLE_BACKGROUND_COLOR, originColor_);
        uiView_->Invalidate();
        return true;
    }

private:
    UIView* uiView_;
    uint32_t originColor_;
};

class TestBtnOnClickRevertToOriginStateListener : public UIView::OnClickListener {
public:
    explicit TestBtnOnClickRevertToOriginStateListener(UIView* uiView) : uiView_(uiView), originColor_(0),
                                                                         width_(0), height_(0), visible_(false),
                                                                         touchable_(false)
    {
        if (uiView != nullptr) {
            originColor_ = uiView->GetStyle(STYLE_BACKGROUND_COLOR);
            width_ = uiView->GetWidth();
            height_ = uiView->GetHeight();
            visible_ = uiView->IsVisible();
            touchable_ = uiView->IsTouchable();
        }
    }

    ~TestBtnOnClickRevertToOriginStateListener() {}

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        uiView_->Invalidate();
        uiView_->SetStyle(STYLE_BACKGROUND_COLOR, originColor_);
        uiView_->SetWidth(width_);
        uiView_->SetHeight(height_);
        uiView_->SetVisible(visible_);
        uiView_->SetTouchable(touchable_);
        ((UIButton*)uiView_)->Invalidate();
        return true;
    }

private:
    UIView* uiView_;
    uint32_t originColor_;
    uint32_t width_;
    uint32_t height_;
    bool visible_;
    bool touchable_;
};

UILabel* GetTestUILabel(const char* titlename)
{
    if (titlename == nullptr) {
        return nullptr;
    }

    UILabel* label = new UILabel();
    // 900:y-coordinate
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 900, Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText(titlename);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    return label;
}

UILabelButton* GetTestUIButton(const char* buttonText, int16_t x, int16_t y, UIButton* button)
{
    if (buttonText == nullptr) {
        return nullptr;
    }

    UILabelButton* labelButton = new UILabelButton();
    // 150: x-coordinate, 440: y-coordinate
    labelButton->SetPosition(x, y);
    labelButton->Resize(BUTTON_WIDHT1, BUTTON_HEIGHT1);
    labelButton->SetText(buttonText);
    labelButton->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    labelButton->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    labelButton->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    labelButton->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    labelButton->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    labelButton->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    return labelButton;
}

void UITestBUTTON::UIKit_Button_Test_002(UIScrollView* container, UIButton* button)
{
    if ((container == nullptr) || (button == nullptr)) {
        return;
    }
    UILabelButton* button8 = GetTestUIButton("隐藏", 430, 940, button); // 430: x-coordinate, 940: y-coordinate
    if (clickEnableVisiableListener_ == nullptr) {
        clickEnableVisiableListener_ = static_cast<UIView::OnClickListener*>(
        new TestBtnOnClickVisableListener((UIView*)button, false));
    }
    button8->SetOnClickListener(clickEnableVisiableListener_);

    UILabelButton* button9 = GetTestUIButton("显示", 430, 990, button); // 430: x-coordinate, 990: y-coordinate
    if (clickDisableVisiableListener_ == nullptr) {
        clickDisableVisiableListener_ = static_cast<UIView::OnClickListener*>(
        new TestBtnOnClickVisableListener((UIView*)button, true));
    }
    button9->SetOnClickListener(clickDisableVisiableListener_);

    UILabelButton* button10 = GetTestUIButton("可触摸 ", 520, 940, button); // 520: x-coordinate, 940: y-coordinate
    if (clickEnableTouchableListener_ == nullptr) {
        clickEnableTouchableListener_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickTouchableListener((UIView*)button, true));
    }
    button10->SetOnClickListener(clickEnableTouchableListener_);

    UILabelButton* button11 = GetTestUIButton("不可触摸", 520, 990, button); // 520: x-coordinate, 990: y-coordinate
    if (clickDisableTouchableListener_ == nullptr) {
        clickDisableTouchableListener_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickTouchableListener((UIView*)button, false));
    }
    button11->SetOnClickListener(clickDisableTouchableListener_);

    UILabelButton* button12 = GetTestUIButton("变白", 160, 1040, button); // 160: x-coordinate, 1040: y-coordinate
    if (clickColorToWhiteListener_ == nullptr) {
        clickColorToWhiteListener_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickChangeColorListener((UIView*)button, 0xFF, 0xFF, 0xFF));
    }
    button12->SetOnClickListener(clickColorToWhiteListener_);

    UILabelButton* button13 = GetTestUIButton("变红", 160, 1090, button); // 160: x-coordinate, 1090: y-coordinate
    if (clickColorToRedListener_ == nullptr) {
        clickColorToRedListener_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickChangeColorListener((UIView*)button, 0xFF, 0, 0));
    }
    button13->SetOnClickListener(clickColorToRedListener_);

    UILabelButton* button14 = GetTestUIButton("色彩还原", 250, 1040, button); // 250: x-coordinate, 1040: y-coordinate
    if (clickRevetColorListener_ == nullptr) {
        clickRevetColorListener_ = static_cast<UIView::OnClickListener*>(
        new TestBtnOnClickRevertColorListener((UIView*)button));
    }
    button14->SetOnClickListener(clickRevetColorListener_);

    UILabelButton* button15 = GetTestUIButton("全部还原", 250, 1090, button); // 250: x-coordinate, 1090: y-coordinate
    if (clickRevetToOriginListener_ == nullptr) {
        clickRevetToOriginListener_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickRevertToOriginStateListener((UIView*)button));
    }
    button15->SetOnClickListener(clickRevetToOriginListener_);

    container->Add(button8);
    container->Add(button9);
    container->Add(button10);
    container->Add(button11);
    container->Add(button12);
    container->Add(button13);
    container->Add(button14);
    container->Add(button15);
}

void UITestBUTTON::UIKit_Button_Test_001()
{
    if (container_ != nullptr) {
        UILabel* label = GetTestUILabel("普通button功能效果");
        container_->Add(label);

        UIButton* button = new UIButton();
        button->SetPosition(10, 935); // 10: x-coordinate, 935: y-coordinate
        button->SetWidth(80); // 80: width
        button->SetHeight(60); // 60: height
        button->SetImageSrc(BLUE_IMAGE_PATH, YELLOW_IMAGE_PATH);

        UILabelButton* button2 = GetTestUIButton("放大", 160, 940, button); // 160: x-coordinate, 940: y-coordinate
        if (clickBigListener_ == nullptr) {
            clickBigListener_ = static_cast<UIView::OnClickListener*>(
                new TestBtnOnClickShapeChangeListener(button, CHANGE_SIZE));
        }
        button2->SetOnClickListener(clickBigListener_);

        UILabelButton* button3 = GetTestUIButton("缩小", 160, 990, button); // 160: x-coordinate, 990: y-coordinate
        if (clickSmallListener_ == nullptr) {
            clickSmallListener_ = static_cast<UIView::OnClickListener*>(
                new TestBtnOnClickShapeChangeListener(button, -CHANGE_SIZE));
        }
        button3->SetOnClickListener(clickSmallListener_);

        UILabelButton* button4 = GetTestUIButton("左移", 250, 940, button); // 250: x-coordinate, 940: y-coordinate
        if (clickLeftListener_ == nullptr) {
            clickLeftListener_ = static_cast<UIView::OnClickListener*>(new TestBtnOnClickPositionChangeListener(
                (UIView*)button, TestBtnOnClickPositionChangeListener::MoveType::MOVE_LEFT, CHANGE_SIZE));
        }
        button4->SetOnClickListener(clickLeftListener_);

        UILabelButton* button5 = GetTestUIButton("右移", 250, 990, button); // 250: x-coordinate, 990: y-coordinate
        button5->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        if (clickRightListener_ == nullptr) {
            clickRightListener_ = static_cast<UIView::OnClickListener*>(new TestBtnOnClickPositionChangeListener(
                (UIView*)button, TestBtnOnClickPositionChangeListener::MoveType::MOVE_RIGHT, CHANGE_SIZE));
        }
        button5->SetOnClickListener(clickRightListener_);

        UILabelButton* button6 = GetTestUIButton("上移", 340, 940, button); // 340: x-coordinate, 940: y-coordinate
        button6->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        if (clickUpListener_ == nullptr) {
            clickUpListener_ = static_cast<UIView::OnClickListener*>(new TestBtnOnClickPositionChangeListener(
                (UIView*)button, TestBtnOnClickPositionChangeListener::MoveType::MOVE_TOP, CHANGE_SIZE));
        }
        button6->SetOnClickListener(clickUpListener_);

        UILabelButton* button7 = GetTestUIButton("下移", 340, 990, button); // 340: x-coordinate, 990: y-coordinate
        if (clickDownListener_ == nullptr) {
            clickDownListener_ = static_cast<UIView::OnClickListener*>(new TestBtnOnClickPositionChangeListener(
                (UIView*)button, TestBtnOnClickPositionChangeListener::MoveType::MOVE_BOTTOM, CHANGE_SIZE));
        }
        button7->SetOnClickListener(clickDownListener_);

        container_->Add(button);
        container_->Add(button2);
        container_->Add(button3);
        container_->Add(button4);
        container_->Add(button5);
        container_->Add(button6);
        container_->Add(button7);

        UIKit_Button_Test_002(container_, button);
    }
}
} // namespace OHOS
