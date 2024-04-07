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

#include "lite_win.h"

#include "gfx_utils/graphic_log.h"
#include "gfx_utils/pixel_format_utils.h"
#include "graphic_locker.h"
#include "graphic_performance.h"
#include "hals/gfx_engines.h"
#include "securec.h"

#include "lite_wm.h"
#ifdef ARM_NEON_OPT
#include "graphic_neon_utils.h"
#endif

namespace OHOS {
#define COLOR_BLEND_RGBA(r1, g1, b1, a1, r2, g2, b2, a2)  \
    const float A1 = static_cast<float>(a1) / OPA_OPAQUE; \
    const float A2 = static_cast<float>(a2) / OPA_OPAQUE; \
    const float a = 1 - (1 - A1) * (1 - A2);              \
    (r1) = (A2 * (r2) + (1 - A2) * A1 * (r1)) / a;        \
    (g1) = (A2 * (g2) + (1 - A2) * A1 * (g1)) / a;        \
    (b1) = (A2 * (b2) + (1 - A2) * A1 * (b1)) / a;        \
    (a1) = a * OPA_OPAQUE;

#define COLOR_BLEND_RGB(r1, g1, b1, r2, g2, b2, a2)                                    \
    (r1) = (((r2) * (a2)) / OPA_OPAQUE) + (((r1) * (OPA_OPAQUE - (a2))) / OPA_OPAQUE); \
    (g1) = (((g2) * (a2)) / OPA_OPAQUE) + (((g1) * (OPA_OPAQUE - (a2))) / OPA_OPAQUE); \
    (b1) = (((b2) * (a2)) / OPA_OPAQUE) + (((b1) * (OPA_OPAQUE - (a2))) / OPA_OPAQUE);

namespace {
    static const int16_t DEFAULT_QUEUE_SIZE = 2;
}

LiteWindow::LiteWindow(const LiteWinConfig& config)
    : id_(INVALID_WINDOW_ID), pid_(INVALID_PID), isShow_(false), config_(config), surface_(nullptr),
      backBuf_(nullptr), sid_({}), needUnregister_(false)
{
    pthread_mutex_init(&backBufMutex_, NULL);
    id_ = LiteWM::GetInstance()->GetUniqueWinId();
}

LiteWindow::~LiteWindow()
{
    if (needUnregister_) {
        GRAPHIC_LOGI("UnregisterIpcCallback");
        UnregisterIpcCallback(sid_);
    }

    if (surface_ != nullptr) {
        if (backBuf_ != nullptr) {
            surface_->CancelBuffer(backBuf_);
        }
        delete surface_;
        surface_ = nullptr;
    }

    LiteWM::GetInstance()->RecycleWinId(id_);
}

bool LiteWindow::CreateSurface()
{
    if (surface_ == nullptr) {
        surface_ = Surface::CreateSurface();
        if (surface_ == nullptr) {
            GRAPHIC_LOGE("CreateSurface failed!");
            return false;
        }
        surface_->SetWidthAndHeight(config_.rect.GetWidth(), config_.rect.GetHeight());
        surface_->SetQueueSize(DEFAULT_QUEUE_SIZE);
        surface_->SetFormat(config_.pixelFormat);
        surface_->SetUsage(BUFFER_CONSUMER_USAGE_HARDWARE);

        if (backBuf_ == nullptr) {
            backBuf_ = surface_->RequestBuffer();
        }
    }
    return true;
}

void LiteWindow::ReleaseSurface()
{
}

void LiteWindow::ResizeSurface(int16_t width, int16_t height)
{
    if (surface_ == nullptr) {
        return;
    }

    GraphicLocker lock(backBufMutex_);
    if (backBuf_ != nullptr) {
        surface_->CancelBuffer(backBuf_);
    }
    surface_->SetWidthAndHeight(width, height);
    backBuf_ = surface_->RequestBuffer();
}

void LiteWindow::Update(Rect rect)
{
    LiteWM::GetInstance()->UpdateWindowRegion(this, rect);
}

void LiteWindow::UpdateBackBuf()
{
    GraphicLocker lock(backBufMutex_);
    if (surface_ == nullptr || backBuf_ == nullptr) {
        return;
    }

    SurfaceBuffer* acquireBuffer = surface_->AcquireBuffer();
    if (acquireBuffer != nullptr) {
        void* acquireBufVirAddr = acquireBuffer->GetVirAddr();
        void* backBufVirAddr = backBuf_->GetVirAddr();
        if (acquireBufVirAddr != nullptr && backBufVirAddr != nullptr) {
            GRAPHIC_LOGI("memcpy, backBuf size=%d, acquireBuffer size=%d",
                backBuf_->GetSize(), acquireBuffer->GetSize());
#ifdef ARM_NEON_OPT
            {
                DEBUG_PERFORMANCE_TRACE("UpdateBackBuf_neon");
                NeonMemcpy(backBufVirAddr, backBuf_->GetSize(), acquireBufVirAddr, acquireBuffer->GetSize());
            }
#else
            {
                DEBUG_PERFORMANCE_TRACE("UpdateBackBuf");
                if (memcpy_s(backBufVirAddr, backBuf_->GetSize(),
                    acquireBufVirAddr, acquireBuffer->GetSize()) != EOK) {
                    GRAPHIC_LOGE("memcpy_s error!");
                }
            }
#endif
            GRAPHIC_LOGI("memcpy end");
        }
        surface_->ReleaseBuffer(acquireBuffer);
    }
}

void LiteWindow::FlushWithModeCopy(const Rect& srcRect, const LiteSurfaceData* layerData, int16_t dx, int16_t dy)
{
    int16_t x1 = srcRect.GetLeft();
    int16_t y1 = srcRect.GetTop();
    int16_t x2 = srcRect.GetRight();
    int16_t y2 = srcRect.GetBottom();

    uint32_t stride = surface_->GetStride();
    uint8_t* srcBuf = reinterpret_cast<uint8_t*>(backBuf_->GetVirAddr()) + y1 * stride + x1 * sizeof(ColorType);
    uint8_t* dstBuf = layerData->virAddr + dy * layerData->stride + dx * sizeof(LayerColorType);
    int32_t lineSize = static_cast<int32_t>(x2 - x1 + 1) * sizeof(LayerColorType);
    for (int16_t y = y1; y <= y2; ++y) {
#ifdef LAYER_PF_ARGB1555
        ColorType* tmpSrc = reinterpret_cast<ColorType*>(srcBuf);
        LayerColorType* tmpDst = reinterpret_cast<LayerColorType*>(dstBuf);
        for (int16_t x = x1; x <= x2; ++x) {
            *tmpDst++ = PixelFormatUtils::ARGB8888ToARGB1555((tmpSrc++)->full);
        }
#elif defined LAYER_PF_ARGB8888
        if (memcpy_s(dstBuf, lineSize, srcBuf, lineSize) != EOK) {
            GRAPHIC_LOGE("memcpy_s error!");
        }
#endif
        srcBuf += stride;
        dstBuf += layerData->stride;
    }
}

void LiteWindow::FlushWithModeBlend(const Rect& srcRect, const LiteSurfaceData* layerData, int16_t dx, int16_t dy)
{
    int16_t x1 = srcRect.GetLeft();
    int16_t y1 = srcRect.GetTop();
    int16_t x2 = srcRect.GetRight();
    int16_t y2 = srcRect.GetBottom();

    uint32_t stride = surface_->GetStride();
    uint8_t* srcBuf = reinterpret_cast<uint8_t*>(backBuf_->GetVirAddr()) + y1 * stride + x1 * sizeof(ColorType);
    uint8_t* dstBuf = layerData->virAddr + dy * layerData->stride + dx * sizeof(LayerColorType);
    for (int16_t y = y1; y <= y2; ++y) {
        ColorType* tmpSrc = reinterpret_cast<ColorType*>(srcBuf);
        LayerColorType* tmpDst = reinterpret_cast<LayerColorType*>(dstBuf);
        for (int16_t x = x1; x <= x2; ++x) {
            uint8_t alpha = tmpSrc->alpha * config_.opacity / OPA_OPAQUE;
#ifdef LAYER_PF_ARGB1555
            PF_ARGB1555* dst = reinterpret_cast<PF_ARGB1555*>(tmpDst);
            if (dst->alpha == 0) {
                if (alpha) {
                    // ARGB8888 to ARGB1555, R/G/B shoud right shift 3 bits
                    dst->red = (tmpSrc->red * alpha / OPA_OPAQUE) >> 3;
                    dst->green = (tmpSrc->green * alpha / OPA_OPAQUE) >> 3;
                    dst->blue = (tmpSrc->blue * alpha / OPA_OPAQUE) >> 3;
                    dst->alpha = 1;
                }
            } else {
                COLOR_BLEND_RGB(dst->red, dst->green, dst->blue,
                                (tmpSrc->red) >> 3, (tmpSrc->green) >> 3, (tmpSrc->blue) >> 3, alpha);
            }
#elif defined LAYER_PF_ARGB8888
            if (alpha == OPA_OPAQUE) {
                *tmpDst = tmpSrc->full;
            } else {
                Color32* dst = reinterpret_cast<Color32*>(tmpDst);
                COLOR_BLEND_RGBA(dst->red, dst->green, dst->blue, dst->alpha,
                                 tmpSrc->red, tmpSrc->green, tmpSrc->blue, alpha);
            }
#endif
            ++tmpSrc;
            ++tmpDst;
        }
        srcBuf += stride;
        dstBuf += layerData->stride;
    }
}

void LiteWindow::Flush(const Rect& srcRect, const LiteSurfaceData* layerData, int16_t dx, int16_t dy)
{
    if (layerData == nullptr) {
        return;
    }

    GraphicLocker lock(backBufMutex_);
#if ENABLE_GFX_ENGINES
    uintptr_t phyaddr = backBuf_->GetPhyAddr();
    if (IsCoverMode() && phyaddr) {
        LiteSurfaceData srcData;
        srcData.width = surface_->GetWidth();
        srcData.height = surface_->GetHeight();
        srcData.pixelFormat = (ImagePixelFormat)surface_->GetFormat();
        srcData.stride = surface_->GetStride();
        srcData.phyAddr = reinterpret_cast<uint8_t*>(phyaddr);
        GRAPHIC_LOGD("Hardware composite, width=%d, height=%d, pixelFormat=%d, stride=%d",
            srcData.width, srcData.height, srcData.pixelFormat, srcData.stride);
        if (GfxEngines::GetInstance()->GfxBlit(srcData, srcRect, *layerData, dx, dy)) {
            return;
        }
    }
#endif

    if (config_.compositeMode == LiteWinConfig::COPY) {
        FlushWithModeCopy(srcRect, layerData, dx, dy);
    } else if (config_.compositeMode == LiteWinConfig::BLEND) {
        FlushWithModeBlend(srcRect, layerData, dx, dy);
    }
}

Surface* LiteWindow::GetSurface()
{
    return surface_;
}

void LiteWindow::MoveTo(int16_t x, int16_t y)
{
    GRAPHIC_LOGI("{%d,%d}=>{%d,%d}", config_.rect.GetLeft(), config_.rect.GetTop(), x, y);
    LiteWM::GetInstance()->UpdateWindowRegion(this, config_.rect);
    config_.rect.SetPosition(x, y);
    LiteWM::GetInstance()->UpdateWindowRegion(this, config_.rect);
}

void LiteWindow::Resize(int16_t width, int16_t height)
{
    GRAPHIC_LOGI("{%d,%d}=>{%d,%d}", config_.rect.GetWidth(), config_.rect.GetHeight(), width, height);
    config_.rect.Resize(width, height);
    ResizeSurface(width, height);
}
}