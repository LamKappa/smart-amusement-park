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

#include "lite_wm_requestor.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/input_event_info.h"
#include "gfx_utils/pixel_format_utils.h"
#include "lite_wms_client.h"
#include "surface_impl.h"

namespace OHOS {
LiteWMRequestor* LiteWMRequestor::GetInstance()
{
    static LiteWMRequestor requestor;
    return &requestor;
}

LiteWMRequestor::LiteWMRequestor() : proxy_(nullptr), listener_(nullptr), surface_(nullptr), sid_({}), layerInfo_({})
{
    proxy_ = LiteWMSClient::GetInstance()->GetClientProxy();
}

int LiteWMRequestor::Callback(void* owner, int code, IpcIo* reply)
{
    if ((code != 0) || (owner == nullptr)) {
        return -1;
    }

    CallBackPara* para = (CallBackPara*)owner;
    switch (para->funcId) {
        case LiteWMS_CreateWindow: {
            LiteWinRequestor** requestor = (LiteWinRequestor**)(para->data);
            if (requestor == nullptr) {
                break;
            }
            int32_t id = IpcIoPopInt32(reply);
            GRAPHIC_LOGI("CreateWindow, id=%d", id);
            if (id == INVALID_WINDOW_ID) {
                *requestor = nullptr;
            } else {
                *requestor = new LiteWinRequestor(id);
            }
            break;
        }
        case LiteWMS_GetEventData: {
            uint32_t size;
            DeviceData* data = static_cast<DeviceData*>(IpcIoPopFlatObj(reply, &size));
            DeviceData* retData = (DeviceData*)(para->data);
            if (data != nullptr && retData != nullptr) {
                *retData = *data;
            }
            break;
        }
        case LiteWMS_Screenshot: {
            int32_t ret = IpcIoPopInt32(reply);
            if (ret != LiteWMS_EOK) {
                GRAPHIC_LOGW("Screenshot busy!");
                LiteWMRequestor::GetInstance()->ScreenShotClearup();
            }
            break;
        }
        case LiteWMS_GetLayerInfo: {
            uint32_t size;
            LiteLayerInfo* data = static_cast<LiteLayerInfo*>(IpcIoPopFlatObj(reply, &size));
            LiteLayerInfo* retData = (LiteLayerInfo*)(para->data);
            if (data != nullptr && retData != nullptr) {
                *retData = *data;
            }
            break;
        }
        default:
            break;
    }
    return 0;
}

int32_t LiteWMRequestor::WmsMsgHandler(const IpcContext* context, void* ipcMsg, IpcIo* io, void* arg)
{
    // It's not used yet
    return 0;
}

void LiteWMRequestor::ClientRegister()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 1);

    SvcIdentity svc;
    if (RegisterIpcCallback(WmsMsgHandler, 0, IPC_WAIT_FOREVER, &svc, NULL) != LITEIPC_OK) {
        GRAPHIC_LOGE("RegisterIpcCallback failed.");
        return;
    }
    IpcIoPushSvc(&io, &svc);
    int32_t ret = proxy_->Invoke(proxy_, LiteWMS_ClientRegister, &io, NULL, Callback);
    if (ret != 0) {
        GRAPHIC_LOGE("ClientRegister failed, ret=%d", ret);
    }
}

void LiteWMRequestor::GetLayerInfo()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);

    CallBackPara para = {};
    para.funcId = LiteWMS_GetLayerInfo;
    para.data = &layerInfo_;
    int32_t ret = proxy_->Invoke(proxy_, LiteWMS_GetLayerInfo, &io, &para, Callback);
    if (ret != 0) {
        GRAPHIC_LOGE("GetLayerInfo failed, ret=%d", ret);
    }
}

LiteWinRequestor* LiteWMRequestor::CreateWindow(const LiteWinConfig& config)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    IpcIoPushFlatObj(&io, &config, sizeof(LiteWinConfig));

    LiteWinRequestor* requestor = nullptr;
    CallBackPara para = {};
    para.funcId = LiteWMS_CreateWindow;
    para.data = &requestor;
    int32_t ret = proxy_->Invoke(proxy_, LiteWMS_CreateWindow, &io, &para, Callback);
    if (ret != 0) {
        GRAPHIC_LOGE("CreateWindow failed, ret=%d", ret);
    }

    return requestor;
}

void LiteWMRequestor::RemoveWindow(int32_t id)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    IpcIoPushInt32(&io, id);

    int32_t ret = proxy_->Invoke(proxy_, LiteWMS_RemoveWindow, &io, NULL, Callback);
    if (ret != 0) {
        GRAPHIC_LOGE("RemoveWindow failed, ret=%d", ret);
    }
}

void LiteWMRequestor::GetEventData(DeviceData* data)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);

    CallBackPara para = {};
    para.funcId = LiteWMS_GetEventData;
    para.data = data;
    (void)proxy_->Invoke(proxy_, LiteWMS_GetEventData, &io, &para, Callback);
}

int LiteWMRequestor::SurfaceRequestHandler(const IpcContext* context, void* ipcMsg, IpcIo* io, void* arg)
{
    SurfaceImpl* surface = (SurfaceImpl*)arg;
    if (surface == nullptr) {
        return 0;
    }
    surface->DoIpcMsg(ipcMsg, io);
    return 0;
}

void LiteWMRequestor::ScreenShotClearup()
{
    UnregisterIpcCallback(sid_);
    if (surface_ != nullptr) {
        delete surface_;
        surface_ = nullptr;
    }
}

void LiteWMRequestor::OnBufferAvailable()
{
    GRAPHIC_LOGD("OnBufferAvailable");
    if (surface_ != nullptr) {
        SurfaceBuffer* buffer = surface_->AcquireBuffer();
        if (buffer != nullptr) {
            if (listener_ != nullptr) {
                uint8_t* virAddr = static_cast<uint8_t*>(buffer->GetVirAddr());
                uint32_t width = surface_->GetWidth();
                uint32_t height = surface_->GetHeight();
                ImagePixelFormat format = static_cast<ImagePixelFormat>(surface_->GetFormat());
                uint32_t stride = surface_->GetStride();
                listener_->OnScreenshotEnd(virAddr, width, height, format, stride);
            }
            surface_->ReleaseBuffer(buffer);
        }
        ScreenShotClearup();
    }
}

void LiteWMRequestor::Screenshot()
{
    if (surface_ != nullptr || listener_ == nullptr) {
        return;
    }

    surface_ = Surface::CreateSurface();
    if (surface_ == nullptr) {
        return;
    }

    surface_->SetWidthAndHeight(layerInfo_.width, layerInfo_.height);
    surface_->SetFormat(layerInfo_.pixelFormat);
    surface_->SetUsage(1);
    surface_->RegisterConsumerListener(*this);

    int32_t ret = RegisterIpcCallback(SurfaceRequestHandler, 0, IPC_WAIT_FOREVER, &sid_, surface_);
    if (ret != LITEIPC_OK) {
        GRAPHIC_LOGE("RegisterIpcCallback failed.");
        delete surface_;
        surface_ = nullptr;
        return;
    }

    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 1);
    IpcIoPushSvc(&io, &sid_);
    CallBackPara para = {};
    para.funcId = LiteWMS_Screenshot;
    ret = proxy_->Invoke(proxy_, LiteWMS_Screenshot, &io, &para, Callback);
    if (ret != 0) {
        GRAPHIC_LOGE("Screenshot failed, ret=%d", ret);
    }
}
} // namespace OHOS