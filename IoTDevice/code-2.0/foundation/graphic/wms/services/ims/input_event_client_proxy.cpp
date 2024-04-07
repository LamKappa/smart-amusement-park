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

#include "input_event_client_proxy.h"
#include "gfx_utils/graphic_log.h"
#include "samgr_lite.h"

namespace OHOS {
pthread_mutex_t InputEventClientProxy::lock_;
InputEventClientProxy* InputEventClientProxy::GetInstance()
{
    static InputEventClientProxy ims;
    return &ims;
}

void InputEventClientProxy::ClientRequestHandle(int funcId, void* origin, IpcIo* req, IpcIo* reply)
{
    switch (funcId) {
        case LITEIMS_CLIENT_REGISTER: {
            InputEventClientProxy::GetInstance()->AddListener(origin, req, reply);
            break;
        }
        case LITEIMS_CLIENT_UNREGISTER: {
            InputEventClientProxy::GetInstance()->RemoveListener(origin, req, reply);
            break;
        }
        default: {
            break;
        }
    }
}

void InputEventClientProxy::AddListener(const void* origin, IpcIo* req, IpcIo* reply)
{
    if (clientInfoMap_.size() >= MAX_CLIENT_SIZE) {
        GRAPHIC_LOGE("Exceeded the maximum number!");
        return;
    }
    pid_t pid = GetCallingPid(origin);
    SvcIdentity* sid = IpcIoPopSvc(req);
    bool alwaysInvoke = IpcIoPopBool(req);
    if (sid == nullptr) {
        GRAPHIC_LOGE("Pop Svc failed.");
        return;
    }
    SvcIdentity svc = *sid;
#ifdef __LINUX__
    BinderAcquire(svc.ipcContext, svc.handle);
    free(sid);
    sid = nullptr;
#endif
    uint32_t cbId = 0;
    if (RegisterDeathCallback(NULL, svc, DeathCallback, const_cast<void*>(origin), &cbId) != LITEIPC_OK) {
        GRAPHIC_LOGE("Register death callback failed!");
        return;
    }
    struct ClientInfo clientInfo = { svc, cbId, alwaysInvoke };
    pthread_mutex_lock(&lock_);
    clientInfoMap_.insert(std::make_pair(pid, clientInfo));
    pthread_mutex_unlock(&lock_);
}

int32_t InputEventClientProxy::DeathCallback(const IpcContext* context, void* ipcMsg, IpcIo* data, void* origin)
{
    if (origin != nullptr) {
        InputEventClientProxy::GetInstance()->RemoveListener(origin, nullptr, nullptr);
        return 0;
    }
    return -1;
}

void InputEventClientProxy::RemoveListener(const void* origin, IpcIo* req, IpcIo* reply)
{
    pid_t pid = GetCallingPid(origin);
    if (clientInfoMap_.count(pid) > 0) {
#ifdef __LINUX__
        BinderRelease(clientInfoMap_[pid].svc.ipcContext, clientInfoMap_[pid].svc.handle);
#endif
        UnregisterDeathCallback(clientInfoMap_[pid].svc, clientInfoMap_[pid].cdId);
        pthread_mutex_lock(&lock_);
        clientInfoMap_.erase(pid);
        pthread_mutex_unlock(&lock_);
    }
}

void InputEventClientProxy::OnRawEvent(const RawEvent& event)
{
    IpcIo io;
    uint8_t tmpData[IMS_DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, IMS_DEFAULT_IPC_SIZE, 1);
    IpcIoPushFlatObj(&io, static_cast<const void*>(&event), sizeof(RawEvent));
    pthread_mutex_lock(&lock_);
    std::map<pid_t, ClientInfo>::iterator it;
    for (it = clientInfoMap_.begin(); it != clientInfoMap_.end(); it++) {
        if (it->second.alwaysInvoke || (event.state != lastState_)) {
            SendRequest(nullptr, it->second.svc, 0, &io, nullptr, LITEIPC_FLAG_ONEWAY, nullptr);
        }
    }
    lastState_ = event.state;
    pthread_mutex_unlock(&lock_);
}
} // namespace OHOS
