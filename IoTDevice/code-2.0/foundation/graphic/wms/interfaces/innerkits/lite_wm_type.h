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

#ifndef GRAPHIC_LITE_LITE_WM_TYPE_H
#define GRAPHIC_LITE_LITE_WM_TYPE_H

#include "gfx_utils/geometry2d.h"
#include "gfx_utils/pixel_format_utils.h"

namespace OHOS {
struct LiteWinConfig {
    enum CompositeMode {
        COPY,
        BLEND
    };
    Rect rect;
    uint8_t opacity;
    ImagePixelFormat pixelFormat;
    CompositeMode compositeMode;
    bool isModal;
};

struct LiteSurfaceData {
    ImagePixelFormat pixelFormat;
    uint16_t width;
    uint16_t height;
    uint8_t* virAddr;
    uint8_t* phyAddr;
    uint32_t stride;
    uint8_t bytePerPixel;
};

struct LiteLayerInfo {
    ImagePixelFormat pixelFormat;
    uint16_t width;
    uint16_t height;
};

typedef enum {
    LiteWMS_GetSurface,
    LiteWMS_Show,
    LiteWMS_Hide,
    LiteWMS_RaiseToTop,
    LiteWMS_LowerToBottom,
    LiteWMS_MoveTo,
    LiteWMS_Resize,
    LiteWMS_Update,
    LiteWMS_CreateWindow,
    LiteWMS_RemoveWindow,
    LiteWMS_GetEventData,
    LiteWMS_Screenshot,
    LiteWMS_ClientRegister,
    LiteWMS_GetLayerInfo
} LiteWMSCall;

typedef enum {
    LiteWMS_EOK = 0,
    LiteWMS_EUNKONW
} LiteWMSErrorCode;

struct CallBackPara {
    int funcId;
    void* data;
};

#ifdef LAYER_PF_ARGB1555
typedef uint16_t LayerColorType;
#elif defined LAYER_PF_ARGB8888
typedef uint32_t LayerColorType;
#endif

const char SERVICE_NAME[] = "WMS";
const int INVALID_WINDOW_ID = -1;
const int INVALID_PID = -1;
}
#endif
