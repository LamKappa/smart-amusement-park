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

#include "input_event_listener_proxy.h"
#include "gfx_utils/graphic_log.h"
#include "samgr_lite.h"

namespace OHOS {
InputEventListenerProxy::RawEventListener* InputEventListenerProxy::listener_ = nullptr;
InputEventListenerProxy::~InputEventListenerProxy()
{
    if (proxy_ != nullptr) {
        UnregisterInputEventListener();
        proxy_ = nullptr;
    }
}

InputEventListenerProxy* InputEventListenerProxy::GetInstance()
{
    static InputEventListenerProxy client;
    return &client;
}

bool InputEventListenerProxy::GetIClientProxy()
{
    if (proxy_ == nullptr) {
        IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(IMS_SERVICE_NAME);
        if (iUnknown == nullptr) {
            GRAPHIC_LOGE("iUnknown is NULL");
            return false;
        }
        (void)iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&proxy_);
        if (proxy_ == nullptr) {
            GRAPHIC_LOGE("QueryInterface failed, IClientProxy is empty!");
            return false;
        }
    }
    return true;
}

int32_t InputEventListenerProxy::ReceiveMsgHandler(const IpcContext* context, void* ipcMsg, IpcIo* io, void* arg)
{
    if (listener_ == nullptr) {
        return -1;
    }
    uint32_t size;
    RawEvent* eventTemp = static_cast<RawEvent*>(IpcIoPopFlatObj(io, &size));
    if (eventTemp == nullptr) {
        GRAPHIC_LOGE("pop raw event failed.");
        return -1;
    }
    RawEvent event = *eventTemp;
    listener_->OnRawEvent(event);
    FreeBuffer(nullptr, ipcMsg);
    return 0;
}

bool InputEventListenerProxy::RegisterInputEventListener(RawEventListener* listener)
{
    if (listener == nullptr) {
        GRAPHIC_LOGE("Input event listener is empty.");
        return false;
    }

    if (!GetIClientProxy()) {
        GRAPHIC_LOGE("Get input event client proxy failed.");
        return false;
    }
    IpcIo io;
    uint8_t tmpData[IMS_DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, IMS_DEFAULT_IPC_SIZE, 1);
    SvcIdentity svc;
    if (RegisterIpcCallback(ReceiveMsgHandler, 0, IPC_WAIT_FOREVER, &svc, NULL) != LITEIPC_OK) {
        GRAPHIC_LOGE("RegisterIpcCallback failed.");
        return false;
    }
    IpcIoPushSvc(&io, &svc);
    IpcIoPushBool(&io, listener->IsAlwaysInvoke());
    int32_t ret = proxy_->Invoke(proxy_, LITEIMS_CLIENT_REGISTER, &io, NULL, NULL);
    if (ret != 0) {
        GRAPHIC_LOGE("Client register failed, ret=%d", ret);
        return false;
    }
    listener_ = listener;
    return true;
}

bool InputEventListenerProxy::UnregisterInputEventListener()
{
    if (proxy_ != nullptr) {
        IpcIo io;
        uint8_t tmpData[IMS_DEFAULT_IPC_SIZE];
        IpcIoInit(&io, tmpData, IMS_DEFAULT_IPC_SIZE, 1);
        int32_t ret = proxy_->Invoke(proxy_, LITEIMS_CLIENT_UNREGISTER, &io, NULL, NULL);
        if (ret == 0) {
            if (listener_ != nullptr) {
                listener_ = nullptr;
            }
            return true;
        }
    }
    return false;
}
} // namespace OHOS
