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
#include "updater_ui.h"
#include <cstdio>
#include <termio.h>
#include "animation_label.h"
#include "frame.h"
#include "input_event.h"
#include "log/log.h"
#include "progress_bar.h"
#include "securec.h"
#include "surface_dev.h"
#include "text_label.h"
#include "updater_main.h"
#include "updater_ui_const.h"
#include "utils.h"
#include "view.h"

namespace updater {
using utils::String2Int;

constexpr int LABEL_HEIGHT = 15;
constexpr int LABEL0_OFFSET = 0;
constexpr int LABEL2_OFFSET = 1;
constexpr int LABEL3_OFFSET = 2;
constexpr int MAX_IMGS_NAME_SIZE = 255;
constexpr int MAX_IMGS = 62;

int g_updateFlag = 0;
int g_textLabelNum = 0;
int g_updateErrFlag = 0;

Frame *g_hosFrame;
Frame *g_updateFrame;
TextLable *g_textLabel0;
TextLable *g_textLabel2;
TextLable *g_textLabel3;
TextLable *g_logLabel;
TextLable *g_logResultLabel;
TextLable *g_updateStateLable;
TextLable *g_updateInfoLabel;
AnimationLable *g_anmimationLabel;
ProgressBar *g_progressBar;

void OnKeyEvent(int viewId)
{
    if (viewId == g_textLabel0->GetViewId()) {
        LOG(INFO) << "g_textLabel0 clicked!";
        PostUpdater();
        utils::DoReboot("");
    } else if (viewId == g_textLabel2->GetViewId()) {
        g_logLabel->SetText("Wipe data");
        g_updateFlag = 1;
        ShowUpdateFrame(true);
        DoProgress();
        int ret = FactoryReset(USER_WIPE_DATA, "/data");
        ShowUpdateFrame(false);
        if (ret != 0) {
            g_logResultLabel->SetText("Wipe data failed");
        } else {
            g_logResultLabel->SetText("Wipe data done");
        }
    } else if (viewId == g_textLabel3->GetViewId()) {
        g_logLabel->SetText("Update system!");
        auto status = UpdaterFromSdcard();
        if (status != UPDATE_SUCCESS) {
            g_logResultLabel->SetText("install failed.\n");
        } else {
            g_logResultLabel->SetText("install success\n");
        }
    }
}

void LoadImgs()
{
    char nameBuf[MAX_IMGS_NAME_SIZE];
    for (int i = 0; i < MAX_IMGS; i++) {
        UPDATER_ERROR_CHECK(!memset_s(nameBuf, MAX_IMGS_NAME_SIZE + 1, 0, MAX_IMGS_NAME_SIZE),
            "Memset_s failed", return);
        if (i < LOOP_TOP_PICTURES) {
            UPDATER_CHECK_ONLY_RETURN(snprintf_s(nameBuf, MAX_IMGS_NAME_SIZE, MAX_IMGS_NAME_SIZE - 1,
                "/resources/loop0000%d.png", i) != -1, return);
        } else {
            UPDATER_CHECK_ONLY_RETURN(snprintf_s(nameBuf, MAX_IMGS_NAME_SIZE, MAX_IMGS_NAME_SIZE - 1,
                "/resources/loop000%d.png", i) != -1, return);
        }
        g_anmimationLabel->AddImg(nameBuf);
    }
    g_anmimationLabel->AddStaticImg(nameBuf);
}

void ShowUpdateFrame(bool isShow)
{
    const int sleepMs = 300 * 100;
    g_updateErrFlag = 0;
    if (isShow) {
        g_hosFrame->Hide();
        g_updateInfoLabel->SetText("");
        g_updateStateLable->SetText("");
        g_updateFrame->Show();
        g_anmimationLabel->Start();
        return;
    }
    usleep(sleepMs);
    g_anmimationLabel->Stop();
    g_updateFrame->Hide();
    g_hosFrame->Show();
    g_updateFlag = 0;
}

void DoProgress()
{
    const int sleepMs = 300 * 1000;
    const int maxSleepMs = 1000 * 1000;
    const int progressValueStep = 10;
    const int maxProgressValue = 100;
    std::string progressValue;
    int progressvalueTmp = 0;
    if (!g_updateFlag) {
        return;
    }
    g_progressBar->SetProgressValue(0);
    while (progressvalueTmp <= maxProgressValue) {
        if (!(g_updateInfoLabel->IsVisiable()) || !(g_progressBar->IsVisiable()) ||
            !(g_updateStateLable->IsVisiable()) || !(g_updateFrame->IsVisiable())) {
            LOG(INFO) <<"is not visable in  updater_frame";
        }
        usleep(sleepMs);
        if (g_updateFlag == 1) {
            progressvalueTmp = progressvalueTmp + progressValueStep;
            g_progressBar->SetProgressValue(progressvalueTmp);
            if (progressvalueTmp >= maxProgressValue) {
                g_updateStateLable->SetText("100%");
                usleep(maxSleepMs);
                return;
            }
            progressValue = std::to_string(progressvalueTmp).append("%");
            g_updateStateLable->SetText(progressValue.c_str());
        }
    }
}

struct FocusInfo {
    bool focus;
    bool focusable;
};

struct Bold {
    bool top;
    bool bottom;
};

static void TextLableInit(TextLable *t, const std::string &text, struct Bold bold,
    struct FocusInfo focus, View::BRGA888Pixel color)
{
    if (t != nullptr) {
        t->SetText(text.c_str());
        t->SetOutLineBold(bold.top, bold.bottom);
        t->OnFocus(focus.focus);
        t->SetBackgroundColor(&color);
        t->SetFocusAble(focus.focusable);
    }
}

static void MenuItemInit(int height, int width, View::BRGA888Pixel bgColor)
{
    if (g_hosFrame == nullptr) {
        LOG(ERROR) << "Frame is null";
        return;
    }
    g_textLabel0 = new TextLable(0, height * LABEL0_OFFSET / LABEL_HEIGHT, width, height /
        LABEL_HEIGHT, g_hosFrame);
    struct FocusInfo info {true, true};
    struct Bold bold {true, false};
    TextLableInit(g_textLabel0, "Reboot to normal system", bold, info, bgColor);
    if (!g_textLabel0) {
        LOG(ERROR) << "g_textLabel0 is null";
        return;
    }
    g_textLabel0->SetOnClickCallback(OnKeyEvent);
    g_textLabelNum++;

    g_textLabel2 = new TextLable(0, height * LABEL2_OFFSET / LABEL_HEIGHT, width, height /
        LABEL_HEIGHT, g_hosFrame);
    info = {false, true};
    bold = {false, false};
    TextLableInit(g_textLabel2, "Userdata reset", bold, info, bgColor);
    if (!g_textLabel2) {
        LOG(ERROR) << "g_textLabel2 is null";
        return;
    }
    g_textLabel2->SetOnClickCallback(OnKeyEvent);
    g_textLabelNum++;

    g_textLabel3 = new TextLable(0, height * LABEL3_OFFSET / LABEL_HEIGHT, width, height /
        LABEL_HEIGHT, g_hosFrame);
    info = {false, true};
    bold = {false, true};
    TextLableInit(g_textLabel3, "Update from SD Card", bold, info, bgColor);
    if (!g_textLabel3) {
        LOG(ERROR) << "g_textLabel3 is null";
        return;
    }
    g_textLabel3->SetOnClickCallback(OnKeyEvent);
    g_textLabelNum++;
}

void HosInit()
{
    constexpr char alpha = 0xff;
    int screenH = 0;
    int screenW = 0;
    auto *sfDev = new SurfaceDev(SurfaceDev::DevType::DRM_DEVICE);
    sfDev->GetScreenSize(screenW, screenH);
    View::BRGA888Pixel bgColor {0x00, 0x00, 0x00, alpha};

    g_hosFrame = new Frame(screenW, screenH, View::PixelFormat::BGRA888, sfDev);
    g_hosFrame->SetBackgroundColor(&bgColor);
    g_hosFrame->Hide();

    MenuItemInit(screenH, screenW, bgColor);

    g_logLabel = new TextLable(START_X1, START_Y1, WIDTH1, HEIGHT1, g_hosFrame);
    struct FocusInfo info {false, false};
    struct Bold bold {false, false};
    TextLableInit(g_logLabel, "", bold, info, bgColor);

    g_logResultLabel = new TextLable(START_X2, START_Y2, WIDTH2, HEIGHT2, g_hosFrame);
    TextLableInit(g_logResultLabel, "", bold, info, bgColor);

    g_updateFrame = new Frame(screenW, screenH, View::PixelFormat::BGRA888, sfDev);
    g_updateFrame->SetBackgroundColor(&bgColor);
    g_updateFrame->Hide();

    g_anmimationLabel = new AnimationLable(screenW / START_X_SCALE, screenH / START_Y_SCALE,
            screenW * WIDTH_SCALE1 / WIDTH_SCALE2, screenH >> 1, g_updateFrame);
    g_anmimationLabel->SetBackgroundColor(&bgColor);
    LoadImgs();
    g_progressBar = new ProgressBar(START_X3, START_Y3, WIDTH3, HEIGHT3, g_updateFrame);
    g_updateStateLable = new TextLable(START_X4, START_Y4, screenH, HEIGHT4, g_updateFrame);
    g_updateStateLable->SetOutLineBold(false, false);
    g_updateStateLable->SetBackgroundColor(&bgColor);

    g_updateInfoLabel = new TextLable(START_X5, START_Y5, screenW, HEIGHT5, g_updateFrame);
    g_updateInfoLabel->SetOutLineBold(false, false);
    g_updateInfoLabel->SetBackgroundColor(&bgColor);
    HdfInit();
}
} // namespace updater
