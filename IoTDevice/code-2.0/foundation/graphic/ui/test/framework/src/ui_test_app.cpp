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

#include "ui_test_app.h"
#include "common/screen.h"
#include "compare_tools.h"
#include "dfx/event_injector.h"
#include "test_resource_config.h"
#include "ui_auto_test.h"
#include "ui_auto_test_group.h"
#include "ui_test.h"
#include "ui_test_group.h"
#if ENABLE_WINDOW
#include "window/window.h"
#endif

namespace OHOS {
UITestApp* UITestApp::GetInstance()
{
    static UITestApp instance;
    return &instance;
}

void UITestApp::Start()
{
    if (rootView_ == nullptr) {
        rootView_ = RootView::GetInstance();
        rootView_->SetPosition(0, 0);
        rootView_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    }
    Init();
}

void UITestApp::Init()
{
    if (backBtn_ == nullptr) {
        backBtn_ = new UILabelButton();
        backBtn_->SetPosition(0, 0);
        backBtn_->Resize(163, 64); // 163: button width; 64: button height
        backBtn_->SetText("Back");
        backBtn_->SetViewId(UI_TEST_BACK_BUTTON_ID);
        backBtn_->SetLablePosition(72, 0);                   // 72: button label x-coordinate
        backBtn_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 24); // 24: means font size
        backBtn_->SetImageSrc(TEST_BACK_LEFT_ARROW, TEST_BACK_LEFT_ARROW);
        // 27: button Image x-coordinate; 18: half px of image height
        backBtn_->SetImagePosition(27, BACK_BUTTON_HEIGHT / 2 - 18);
        backBtn_->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::RELEASED);
        backBtn_->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::PRESSED);
        backBtn_->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::INACTIVE);
        backBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, 0, UIButton::RELEASED);
        backBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, 0, UIButton::PRESSED);
        backBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, 0, UIButton::INACTIVE);
    }
    if (testCaseLabel_ == nullptr) {
        testCaseLabel_ = new UILabel();
        testCaseLabel_->Resize(Screen::GetInstance().GetWidth(), BACK_BUTTON_HEIGHT);
        testCaseLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        testCaseLabel_->SetText("Test Case Name");
        testCaseLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 32); // 32: means font size
    }
    if (testLabel_ == nullptr) {
        testLabel_ = new UILabel();
        testLabel_->Resize(Screen::GetInstance().GetWidth(), BACK_BUTTON_HEIGHT);
        testLabel_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
        testLabel_->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 0);
        testLabel_->SetText("Test Demo");
        testLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 32); // 32: means font size
        rootView_->Add(testLabel_);
    }
    if ((mainList_ == nullptr) && (adapter_ == nullptr)) {
        uint8_t deltaHeight = 60; // 60: UIList height(64) - first button border width(4)
        mainList_ = new UIList(UIList::VERTICAL);
        mainList_->SetPosition(24, deltaHeight); // 24: x-coordinate
        mainList_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - deltaHeight);
        mainList_->SetThrowDrag(true);
        mainList_->SetViewId(UI_TEST_MAIN_LIST_ID);
        adapter_ = new TestCaseListAdapter(rootView_, mainList_, backBtn_, testCaseLabel_, testLabel_);
        UITestGroup::SetUpTestCase();
        mainList_->SetAdapter(adapter_);
        rootView_->Add(mainList_);
        rootView_->Invalidate();
    }
}

UITestApp::~UITestApp()
{
    if (mainList_ != nullptr) {
        delete adapter_;
        adapter_ = nullptr;
    }
    if (adapter_ != nullptr) {
        delete mainList_;
        mainList_ = nullptr;
    }
    if (backBtn_ != nullptr) {
        delete backBtn_;
        backBtn_ = nullptr;
    }
    if (rootView_ != nullptr) {
        rootView_ = nullptr;
    }
}

UIAutoTestApp* UIAutoTestApp::GetInstance()
{
    static UIAutoTestApp instance;
    return &instance;
}

void UIAutoTestApp::Start()
{
    EventInjector::GetInstance()->RegisterEventInjector(EventDataType::POINT_TYPE);
    EventInjector::GetInstance()->RegisterEventInjector(EventDataType::KEY_TYPE);
#ifdef _WIN32
    char logPath[] = ".\\auto_test_log.txt";
    CompareTools::SetLogPath(logPath, sizeof(logPath));
#else
    char logPath[] = "./auto_test_log.txt";
    CompareTools::SetLogPath(logPath, sizeof(logPath));
#endif

#if ENABLE_WINDOW
    Window* window = RootView::GetInstance()->GetBoundWindow();
    if (window != nullptr) {
        EventInjector::GetInstance()->SetWindowId(window->GetWindowId());
    }
#endif
    CompareTools::WaitSuspend();

    UIAutoTestGroup::SetUpTestCase();
    ListNode<UIAutoTest*>* node = UIAutoTestGroup::GetTestCase().Begin();
    while (node != UIAutoTestGroup::GetTestCase().End()) {
        node->data_->RunTestList();
        node->data_->ResetMainMenu();
        node = node->next_;
    }
}

UIAutoTestApp::~UIAutoTestApp()
{
    if (EventInjector::GetInstance()->IsEventInjectorRegistered(EventDataType::POINT_TYPE)) {
        EventInjector::GetInstance()->UnregisterEventInjector(EventDataType::POINT_TYPE);
    }
    if (EventInjector::GetInstance()->IsEventInjectorRegistered(EventDataType::KEY_TYPE)) {
        EventInjector::GetInstance()->UnregisterEventInjector(EventDataType::KEY_TYPE);
    }
    CompareTools::UnsetLogPath();
    UIAutoTestGroup::TearDownTestCase();
}
} // namespace OHOS
