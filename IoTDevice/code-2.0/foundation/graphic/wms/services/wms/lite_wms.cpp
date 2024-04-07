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

#include "lite_wms.h"
#include "gfx_utils/graphic_log.h"
#include "lite_wm.h"
#include "lite_wm_type.h"
#include "surface.h"
#include "surface_impl.h"

namespace OHOS {
LiteWMS* LiteWMS::GetInstance()
{
    static LiteWMS wms;
    return &wms;
}

void LiteWMS::WMSRequestHandle(int funcId, void* origin, IpcIo* req, IpcIo* reply)
{
    switch (funcId) {
        case LiteWMS_GetSurface:
            OHOS::LiteWMS::GetInstance()->GetSurface(req, reply);
            break;
        case LiteWMS_Show:
            LiteWMS::GetInstance()->Show(req, reply);
            break;
        case LiteWMS_Hide:
            LiteWMS::GetInstance()->Hide(req, reply);
            break;
        case LiteWMS_RaiseToTop:
            LiteWMS::GetInstance()->RaiseToTop(req, reply);
            break;
        case LiteWMS_LowerToBottom:
            LiteWMS::GetInstance()->LowerToBottom(req, reply);
            break;
        case LiteWMS_MoveTo:
            LiteWMS::GetInstance()->MoveTo(req, reply);
            break;
        case LiteWMS_Resize:
            LiteWMS::GetInstance()->Resize(req, reply);
            break;
        case LiteWMS_Update:
            LiteWMS::GetInstance()->Update(req, reply);
            break;
        case LiteWMS_CreateWindow:
            LiteWMS::GetInstance()->CreateWindow(origin, req, reply);
            break;
        case LiteWMS_RemoveWindow:
            LiteWMS::GetInstance()->RemoveWindow(req, reply);
            break;
        case LiteWMS_GetEventData:
            LiteWMS::GetInstance()->GetEventData(req, reply);
            break;
        case LiteWMS_Screenshot:
            LiteWMS::GetInstance()->Screenshot(req, reply);
            break;
        case LiteWMS_ClientRegister:
            LiteWMS::GetInstance()->ClientRegister(origin, req, reply);
            break;
        case LiteWMS_GetLayerInfo:
            LiteWMS::GetInstance()->GetLayerInfo(req, reply);
            break;
        default:
            GRAPHIC_LOGW("code not support:%d!", funcId);
            break;
    }
}

int32_t LiteWMS::SurfaceRequestHandler(const IpcContext* context, void* ipcMsg, IpcIo* io, void* arg)
{
    uint32_t code;
    (void)GetCode(ipcMsg, &code);
    LiteWindow* window = (LiteWindow*)arg;
    if (code == 0) {
        GRAPHIC_LOGI("requestBuffer");
        window->UpdateBackBuf();
    }

    Surface* surface = window->GetSurface();
    SurfaceImpl* liteSurface = reinterpret_cast<SurfaceImpl*>(surface);
    liteSurface->DoIpcMsg(ipcMsg, io);
    return 0;
}

void LiteWMS::GetSurface(IpcIo* req, IpcIo* reply)
{
    int32_t id = IpcIoPopInt32(req);
    GRAPHIC_LOGI("GetSurface,id=%d", id);
    LiteWindow* window = LiteWM::GetInstance()->GetWindowById(id);
    if (window == nullptr) {
        GRAPHIC_LOGE("window not found, id = %d", id);
        return;
    }
    SvcIdentity svc;
    int32_t ret = RegisterIpcCallback(SurfaceRequestHandler, 0, IPC_WAIT_FOREVER, &svc, window);
    IpcIoPushInt32(reply, ret);
    if (ret != LITEIPC_OK) {
        GRAPHIC_LOGE("RegisterIpcCallback failed.");
        return;
    }
    window->SetSid(svc);
    IpcIoPushSvc(reply, &svc);
}

void LiteWMS::Show(IpcIo* req, IpcIo* reply)
{
    GRAPHIC_LOGI("Show");
    int32_t id = IpcIoPopInt32(req);
    LiteWM::GetInstance()->Show(id);
}

void LiteWMS::Hide(IpcIo* req, IpcIo* reply)
{
    GRAPHIC_LOGI("Hide");
    int32_t id = IpcIoPopInt32(req);
    LiteWM::GetInstance()->Hide(id);
}

void LiteWMS::RaiseToTop(IpcIo* req, IpcIo* reply)
{
    GRAPHIC_LOGI("RaiseToTop");
    int32_t id = IpcIoPopInt32(req);
    LiteWM::GetInstance()->RaiseToTop(id);
}

void LiteWMS::LowerToBottom(IpcIo* req, IpcIo* reply)
{
    GRAPHIC_LOGI("LowerToBottom");
    int32_t id = IpcIoPopInt32(req);
    LiteWM::GetInstance()->LowerToBottom(id);
}

void LiteWMS::MoveTo(IpcIo* req, IpcIo* reply)
{
    GRAPHIC_LOGI("MoveTo");
    int32_t id = IpcIoPopInt32(req);
    uint32_t x = IpcIoPopUint32(req);
    uint32_t y = IpcIoPopUint32(req);
    LiteWM::GetInstance()->MoveTo(id, x, y);
}

void LiteWMS::Resize(IpcIo* req, IpcIo* reply)
{
    GRAPHIC_LOGI("Resize");
    int32_t id = IpcIoPopInt32(req);
    uint32_t width = IpcIoPopUint32(req);
    uint32_t height = IpcIoPopUint32(req);
    LiteWM::GetInstance()->Resize(id, width, height);
}

void LiteWMS::Update(IpcIo* req, IpcIo* reply)
{
    GRAPHIC_LOGI("Update");
    int32_t id = IpcIoPopInt32(req);
    LiteWM::GetInstance()->UpdateWindow(id);
}

void LiteWMS::CreateWindow(const void* origin, IpcIo* req, IpcIo* reply)
{
    GRAPHIC_LOGI("CreateWindow");
    uint32_t size;
    LiteWinConfig* config = static_cast<LiteWinConfig*>(IpcIoPopFlatObj(req, &size));
    if (config != nullptr) {
        pid_t pid = GetCallingPid(origin);
        LiteWindow* window = LiteWM::GetInstance()->CreateWindow(*config, pid);
        if (window != nullptr) {
            IpcIoPushInt32(reply, window->GetWindowId());
            return;
        }
    }
    IpcIoPushInt32(reply, INVALID_WINDOW_ID);
}

void LiteWMS::RemoveWindow(IpcIo* req, IpcIo* reply)
{
    GRAPHIC_LOGI("RemoveWindow");
    int32_t id = IpcIoPopInt32(req);
    LiteWM::GetInstance()->RemoveWindow(id);
}

void LiteWMS::GetEventData(IpcIo* req, IpcIo* reply)
{
    DeviceData data;
    LiteWM::GetInstance()->GetEventData(&data);
    IpcIoPushFlatObj(reply, &data, sizeof(DeviceData));
}

void LiteWMS::Screenshot(IpcIo* req, IpcIo* reply)
{
    Surface* surface = SurfaceImpl::GenericSurfaceByIpcIo(*req);
    bool ret = LiteWM::GetInstance()->OnScreenshot(surface);
    IpcIoPushInt32(reply, ret ? LiteWMS_EOK : LiteWMS_EUNKONW);
}

void LiteWMS::ClientRegister(const void* origin, IpcIo* req, IpcIo* reply)
{
    pid_t pid = GetCallingPid(origin);
    SvcIdentity* sid = IpcIoPopSvc(req);
    if (sid == nullptr) {
        return;
    }

    DeathCallbackArg* arg = new DeathCallbackArg;
    arg->pid = pid;
    arg->sid = *sid;
    uint32_t cbId = -1;
#ifdef __LINUX__
    BinderAcquire(sid->ipcContext, sid->handle);
    free(sid);
    sid = nullptr;
#endif
    if (RegisterDeathCallback(NULL, arg->sid, DeathCallback, arg, &cbId) != LITEIPC_OK) {
        GRAPHIC_LOGE("RegisterDeathCallback failed!");
    }
}

int32_t LiteWMS::DeathCallback(const IpcContext* context, void* ipcMsg, IpcIo* data, void* arg)
{
    if (arg != nullptr) {
        DeathCallbackArg* cbArg = static_cast<DeathCallbackArg*>(arg);
        LiteWM::GetInstance()->OnClientDeathNotify(cbArg->pid);
#ifdef __LINUX__
        BinderRelease(cbArg->sid.ipcContext, cbArg->sid.handle);
#endif
        delete cbArg;
    }
    return 0;
}

void LiteWMS::GetLayerInfo(IpcIo* req, IpcIo* reply)
{
    LiteLayerInfo layerInfo = {};
    LiteWM::GetInstance()->GetLayerInfo(layerInfo);
    IpcIoPushFlatObj(reply, &layerInfo, sizeof(LiteLayerInfo));
}
} // namespace OHOS