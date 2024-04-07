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
#include "log/log.h"
#include "securec.h"
#include "view.h"

namespace updater {
constexpr int RGBA_PIXEL_SIZE = 4;
void *View::CreateBuffer(int w, int h, int pixelFormat)
{
    int pixelSize = -1;
    switch (pixelFormat) {
        case View::PixelFormat::BGRA888:
            pixelSize = RGBA_PIXEL_SIZE;
            break;
        default:
            LOG(WARNING) << "Unsupported pixel_format: " << pixelFormat;
            LOG(WARNING) << "Use default BGRA888";
            pixelSize = RGBA_PIXEL_SIZE;
            break;
    }
    bufferSize_ = w * h * pixelSize;
    viewBuffer_ = static_cast<char *>(malloc(bufferSize_));
    if (viewBuffer_ == nullptr) {
        LOG(ERROR) << "Allocate memory for view failed: " << errno;
        return nullptr;
    }
    shadowBuffer_ = static_cast<char *>(malloc(bufferSize_));
    if (shadowBuffer_ == nullptr) {
        LOG(ERROR) << "Allocate memory for shadow failed: " << errno;
        free(viewBuffer_);
        viewBuffer_ = nullptr;
        return nullptr;
    }
    if (memset_s(viewBuffer_, bufferSize_, 0, bufferSize_) != EOK) {
        LOG(ERROR) << "Clean view buffer failed";
        free(viewBuffer_);
        viewBuffer_ = nullptr;
        return nullptr;
    }
    viewWidth_ = w;
    viewHeight_ = h;
    return viewBuffer_;
}

void View::SetBackgroundColor(BRGA888Pixel *color)
{
    BRGA888Pixel pixelBuffer[viewWidth_];
    for (int w = 0; w < viewWidth_; w++) {
        pixelBuffer[w].r = color->r;
        pixelBuffer[w].g = color->g;
        pixelBuffer[w].b = color->b;
        pixelBuffer[w].a = color->a;
    }
    for (int h = 0; h < viewHeight_; h++) {
        UPDATER_CHECK_ONLY_RETURN(!memcpy_s(viewBuffer_ + h * viewWidth_ * sizeof(BRGA888Pixel), viewWidth_ *
            sizeof(BRGA888Pixel) + 1, reinterpret_cast<char*>(pixelBuffer), viewWidth_ * sizeof(BRGA888Pixel)),
            return);
    }
    if (isVisiable_) {
        OnDraw();
        LOG(DEBUG) << "view---visable";
    }
}

void View::DrawSubView(int x, int y, int w, int h, void *buf)
{
    int minWidth = ((x + w) <= viewWidth_) ? w : (viewWidth_ - x);
    int minHeight = ((y + h) <= viewHeight_) ? h : (viewHeight_ - y);

    for (int i = 0; i < minHeight; i++) {
        char *src = static_cast<char *>(buf) + i * w * sizeof(BRGA888Pixel);
        char *dst = shadowBuffer_ + (i + y) * viewWidth_ * sizeof(BRGA888Pixel) + x * sizeof(BRGA888Pixel);
        UPDATER_CHECK_ONLY_RETURN(!memcpy_s(dst, minWidth * sizeof(BRGA888Pixel) + 1, src, minWidth *
            sizeof(BRGA888Pixel)), return);
    }
}

void View::OnDraw()
{
    std::unique_lock<std::mutex> locker(mutex_);
    SyncBuffer();
}

void View::Hide()
{
    if (isVisiable_) {
        isVisiable_ = false;
        OnDraw();
    }
}

void View::Show()
{
    if (!isVisiable_) {
        LOG(DEBUG) << "isVisiable:" << isVisiable_;
        isVisiable_ = true;
        OnDraw();
    }
}

void View::SyncBuffer()
{
    if (memcpy_s(shadowBuffer_, bufferSize_, viewBuffer_, bufferSize_) != EOK) {
        LOG(WARNING) << "Sync buffer failed";
    }
}

void *View::GetBuffer() const
{
    return shadowBuffer_;
}

void *View::GetRawBuffer() const
{
    return viewBuffer_;
}

void View::OnFocus(bool foucsed)
{
    isFocused_ = foucsed;
    OnDraw();
}

void View::SetViewId(int id)
{
    viewId_ = id;
}

int View::GetViewId() const
{
    return viewId_;
}

void View::FreeBuffer()
{
    free(viewBuffer_);
    free(shadowBuffer_);
    viewBuffer_ = nullptr;
    shadowBuffer_ = nullptr;
}

bool View::IsVisiable() const
{
    return isVisiable_;
}

bool View::IsSelected() const
{
    return isFocused_;
}

bool View::IsFocusAble() const
{
    return focusable_;
}

void View::SetFocusAble(bool focusable)
{
    focusable_ = focusable;
}

void View::OnKeyEvent(int key)
{
    (void)(key);
}
} // namespace updater
