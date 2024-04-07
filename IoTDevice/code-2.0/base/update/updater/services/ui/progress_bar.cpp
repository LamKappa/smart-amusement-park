/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "progress_bar.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "log/log.h"
#include "securec.h"

namespace updater {
constexpr int DEFAULT_PROGRESS_COLOR_A = 0xCC;
constexpr int DEFAULT_NORMAL_COLOR = 0xFF;
constexpr int MAX_PROGRESS_VALUE = 100;

ProgressBar::ProgressBar(const int mStartX, const int mStartY, const int w, const int h, Frame *mParent)
{
    startX_ = mStartX;
    startY_ = mStartY;
    this->CreateBuffer(w, h, View::PixelFormat::BGRA888);
    parent_ = mParent;
    SetFocusAble(false);
    parent_->ViewRegister(this);
    progressColor_.r = 0x00;
    progressColor_.g = 0x00;
    progressColor_.b = 0x00;
    progressColor_.a = DEFAULT_PROGRESS_COLOR_A;
    normalColor_.r = DEFAULT_NORMAL_COLOR;
    normalColor_.g = DEFAULT_NORMAL_COLOR;
    normalColor_.b = DEFAULT_NORMAL_COLOR;
    normalColor_.a = DEFAULT_NORMAL_COLOR;
}

void ProgressBar::SetProgressValue(int value)
{
    if (value < 0) {
        return;
    }
    if (value > MAX_PROGRESS_VALUE) {
        value = MAX_PROGRESS_VALUE;
    }
    pValue_ = value;
    OnDraw();
    return;
}

void ProgressBar::DrawProgress()
{
    int ret = 0;
    int pixelLen = pValue_ * viewWidth_ / MAX_PROGRESS_VALUE;
    if (pixelLen > viewWidth_) {
        pixelLen = viewWidth_;
    }
    char *tmpBuf = static_cast<char*>(GetBuffer());
    BRGA888Pixel pixBuf[pixelLen];
    for (int a = 0; a < pixelLen; a++) {
        pixBuf[a].r = progressColor_.r;
        pixBuf[a].g = progressColor_.g;
        pixBuf[a].b = progressColor_.b;
        pixBuf[a].a = progressColor_.a;
    }
    for (int i = 0; i < viewHeight_; i++) {
        ret = memcpy_s(tmpBuf + i * viewWidth_ * sizeof(BRGA888Pixel), pixelLen * sizeof(BRGA888Pixel) + 1,
            reinterpret_cast<char*>(pixBuf), pixelLen * sizeof(BRGA888Pixel));
        UPDATER_ERROR_CHECK(ret == 0, "memcpy_s error", break);
    }
    return;
}

void ProgressBar::OnDraw()
{
    SyncBuffer();
    DrawProgress();
    if (parent_ != nullptr) {
        parent_->OnDraw();
    } else {
        LOG(INFO) << "no draw";
    }
    return;
}
} // namespace updater