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

#include "animator/animator.h"
#include "common/graphic_startup.h"
#include "common/task_manager.h"
#include "components/root_view.h"
#include "components/ui_button.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "font/ui_font.h"
#if ENABLE_VECTOR_FONT
#include "font/ui_font_vector.h"
#else
#include "common/ui_text_language.h"
#include "font/ui_font_bitmap.h"
#endif
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "window/window.h"

#include <unistd.h>

namespace OHOS {
namespace {
const uint16_t MAX_LIST_NUM = 40;
}
static RootView* g_rootView1 = nullptr;
static RootView* g_rootView2 = nullptr;
static RootView* g_rootView3 = nullptr;
static bool g_flag = true;

void CreateDefaultWindow(RootView* rootView, int x, int y)
{
    if (rootView != nullptr) {
        WindowConfig config = {};
        config.rect = rootView->GetRect();
        config.rect.SetPosition(x, y);
        Window* window = Window::CreateWindow(config);
        if (window != nullptr) {
            window->BindRootView(rootView);
            window->Show();
        } else {
            GRAPHIC_LOGE("Create window false!");
        }
    }
}

class ImageAnimatorCallbackDemo : public AnimatorCallback {
public:
    ImageAnimatorCallbackDemo() : times_(0) {}
    virtual ~ImageAnimatorCallbackDemo() {}

    enum {
        CONDITION0,
        CONDITION1,
        CONDITION2,
        CONDITION3,
        CONDITION4,
        CONDITION5,
        CONDITION_COUNT,
    };

    virtual void Callback(UIView* view)
    {
        static int i = 0;
        if ((times_++ % 90) != 0) { // 90: animator callback is performed every 90 ticks
            return;
        }
        switch ((++i) % CONDITION_COUNT) {
            case CONDITION0: {
                if (g_flag) {
                    g_rootView3->GetBoundWindow()->LowerToBottom();
                } else {
                    g_rootView3->GetBoundWindow()->RaiseToTop();
                }
                g_flag = !g_flag;
                break;
            }
            case CONDITION1: {
                if (g_flag) {
                    g_rootView1->Resize(403, 201);                   // 403: width, 201: height
                    g_rootView1->GetBoundWindow()->Resize(403, 201); // 403: width, 201: height
                } else {
                    g_rootView1->Resize(600, 300);                   // 600: width, 300: height
                    g_rootView1->GetBoundWindow()->Resize(600, 300); // 600: width, 300: height
                }
                break;
            }
            case CONDITION2: {
                Window* window = g_rootView3->GetBoundWindow();
                int x = (window->GetRect().GetX() + 40) % 400; // 40: x offset, 400: maximum value of x
                window->MoveTo(x, window->GetRect().GetY());
                break;
            }
            case CONDITION3: {
                Window* window = g_rootView2->GetBoundWindow();
                if (window != nullptr) {
                    Window::DestoryWindow(window);
                } else {
                    g_rootView2->Invalidate();
                    CreateDefaultWindow(g_rootView2, 70, 75); // 70: x, 75: y
                }
                break;
            }
            case CONDITION4: {
                g_rootView3->GetBoundWindow()->Hide();
                break;
            }
            case CONDITION5: {
                g_rootView3->GetBoundWindow()->Show();
            }
            default:
                break;
        }
    }

protected:
    int16_t times_;
};

void AddButton()
{
    UIButton* button = new UIButton();
    button->SetPosition(40, 40); // 40: x, 40: y
    button->SetWidth(40);        // 40: width
    button->SetHeight(40);       // 40: height
    button->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);

    UIButton* button1 = new UIButton();
    button1->SetPosition(30, 10); // 30: x, 10: y
    button1->SetWidth(60);        // 60: width
    button1->SetHeight(60);       // 60: height
    button1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
    button1->SetStyle(STYLE_BACKGROUND_OPA, 200); // 200: background opacity
    button1->SetStyle(STYLE_BORDER_RADIUS, 3);    // 3: border radius
    g_rootView1->Add(button1);
    g_rootView2->Add(button);
}

void AddBlock()
{
    UIViewGroup* block = new UIViewGroup();
    block->SetPosition(100, 40); // 100: x, 40: y
    block->SetWidth(60);         // 60: width
    block->SetHeight(60);        // 60: height
    block->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    block->SetStyle(STYLE_BACKGROUND_OPA, 200); // 200: background opacity

    UIViewGroup* block2 = new UIViewGroup();
    block2->SetPosition(40, 40); // 40: x, 40: y
    block2->SetWidth(60);        // 60: width
    block2->SetHeight(60);       // 60: height
    block2->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    block2->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    UIViewGroup* block3 = new UIViewGroup();
    block3->SetPosition(100, 40); // 100: x, 40: y
    block3->SetWidth(60);         // 60: width
    block3->SetHeight(60);        // 60: height
    block3->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    block3->SetStyle(STYLE_BACKGROUND_OPA, 200); // 200: background opacity

    UIViewGroup* block4 = new UIViewGroup();
    block4->SetPosition(1, 1);
    block4->SetWidth(10);  // 10: width
    block4->SetHeight(10); // 10: height
    block4->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    block4->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    g_rootView1->Add(block4);
    g_rootView2->Add(block);
    g_rootView3->Add(block2);
    g_rootView3->Add(block3);
}

void TestWindow()
{
    g_rootView1 = RootView::GetWindowRootView();
    g_rootView1->SetWidth(600);  // 600: width
    g_rootView1->SetHeight(300); // 300: height
    g_rootView1->SetPosition(0, 0);
    g_rootView1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Olive().full);

    g_rootView2 = RootView::GetWindowRootView();
    g_rootView2->SetPosition(0, 0, 200, 200); // 200: width, 200: height
    g_rootView2->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);

    g_rootView3 = RootView::GetWindowRootView();
    g_rootView3->SetPosition(0, 0, 200, 200); // 200: width, 200: height
    g_rootView3->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    g_rootView3->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    UILabel* label = new UILabel();
    label->SetPosition(100, 0, 100, 100);  // 100: x, 100: width, 100: height
    label->SetFont("HYQiHei-65S.otf", 14); // 14: font size
    label->SetText("轻量鸿蒙GUI");
    label->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
    label->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    label->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    UIImageView* image = new UIImageView;
    image->SetPosition(220, 0, 80, 80); // 220: x, 80: width, 80: height
    image->SetSrc("/user/data/A021_028.bin");

    AddButton();
    g_rootView1->Add(label);
    g_rootView1->Add(image);
    AddBlock();

    g_rootView1->Invalidate();
    g_rootView2->Invalidate();
    g_rootView3->Invalidate();

    auto imageAnimCallback = new ImageAnimatorCallbackDemo();
    Animator* imageAnimator = new Animator(imageAnimCallback, g_rootView1, 0, true);
    imageAnimator->Start();

    CreateDefaultWindow(g_rootView1, 0, 50);    // 50: y
    CreateDefaultWindow(g_rootView2, 70, 75);   // 70: x, 75: y
    CreateDefaultWindow(g_rootView3, 120, 200); // 120: x, 200: y
}

RootView* g_rootViewList[MAX_LIST_NUM];
void TestWindowNumLimit()
{
    for (int i = 0; i < MAX_LIST_NUM; i++) {
        GRAPHIC_LOGI("CreateDefaultWindow, i = %d", i);
        if (i == 10) { // 10, 9: Delete the tenth window in the 11th loop.
            Window* window = g_rootViewList[9]->GetBoundWindow();
            Window::DestoryWindow(window);
        } else if (i == 15) { // 15, 5: Delete the sixth window in the 16th loop.
            Window* window = g_rootViewList[5]->GetBoundWindow();
            Window::DestoryWindow(window);
        }
        RootView* rootView = RootView::GetWindowRootView();
        g_rootViewList[i] = rootView;
        rootView->SetWidth(10);  // 10: width
        rootView->SetHeight(10); // 10: height
        rootView->SetPosition(0, 0);
        rootView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Olive().full);
        rootView->Invalidate();
        CreateDefaultWindow(rootView, 20 * i, 0); // 20: offset
    }
}

static uint32_t g_fontMemBaseAddr[MIN_FONT_PSRAM_LENGTH / 4];
#if ENABLE_ICU
static uint8_t g_icuMemBaseAddr[OHOS::SHAPING_WORD_DICT_LENGTH];
#endif
static void InitFontEngine()
{
#if ENABLE_VECTOR_FONT
    GraphicStartUp::InitFontEngine(reinterpret_cast<uintptr_t>(g_fontMemBaseAddr), MIN_FONT_PSRAM_LENGTH,
                                   VECTOR_FONT_DIR, DEFAULT_VECTOR_FONT_FILENAME);
#else
    BitmapFontInit();
    const char* dPath = "/user/data/font.bin";
    GraphicStartUp::InitFontEngine(reinterpret_cast<uintptr_t>(g_fontMemBaseAddr), MIN_FONT_PSRAM_LENGTH,
                                   dPath, nullptr);
#endif

#if ENABLE_ICU
    GraphicStartUp::InitLineBreakEngine(reinterpret_cast<uintptr_t>(g_icuMemBaseAddr), SHAPING_WORD_DICT_LENGTH,
                                        VECTOR_FONT_DIR, DEFAULT_LINE_BREAK_RULE_FILENAME);
#endif
}
} // namespace OHOS

int main(int argc, char* argv[])
{
    OHOS::GraphicStartUp::Init();
    OHOS::InitFontEngine();
    OHOS::TestWindow();
    while (1) {
        /* Periodically call TaskHandler(). It could be done in a timer interrupt or an OS task too. */
        OHOS::TaskManager::GetInstance()->TaskHandler();
        usleep(1000 * 10); /* 1000 * 10: Just to let the system breathe */
    }
    return 0;
}
