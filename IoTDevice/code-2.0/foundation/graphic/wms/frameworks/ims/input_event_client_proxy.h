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

#ifndef GRAPHIC_LITE_IMS_LISTERNER_PROXY_H
#define GRAPHIC_LITE_IMS_LISTERNER_PROXY_H

#include "input_event_distributer.h"
#include "gfx_utils/input_event_info.h"
#include "input_manager_service.h"
#include "liteipc_adapter.h"
#include "serializer.h"
#include <pthread.h>
#include <map>

namespace OHOS {
#define MAX_CLIENT_SIZE 1
class InputEventClientProxy : public InputEventDistributer::RawEventListener {
public:
    static InputEventClientProxy* GetInstance();

    static void ClientRequestHandle(int funcId, void* origin, IpcIo* req, IpcIo* reply);

private:
    InputEventClientProxy()
    {
        InputManagerService::GetInstance()->GetDistributer()->AddRawEventListener(this);
        pthread_mutex_init(&lock_, nullptr);
    }

    ~InputEventClientProxy()
    {
        pthread_mutex_destroy(&lock_);
    }

    struct ClientInfo {
        SvcIdentity svc;
        uint32_t cdId;
        bool alwaysInvoke;
    };

    std::map<pid_t, ClientInfo> clientInfoMap_;
    static pthread_mutex_t lock_;
    int16_t lastState_ = 0;

    InputEventClientProxy(const InputEventClientProxy&) = delete;
    InputEventClientProxy& operator=(const InputEventClientProxy&) = delete;
    InputEventClientProxy(InputEventClientProxy&&) = delete;
    InputEventClientProxy& operator=(InputEventClientProxy&&) = delete;

    void OnRawEvent(const RawEvent& event) override;
    void AddListener(const void* origin, IpcIo* req, IpcIo* reply);
    void RemoveListener(const void* origin, IpcIo* req, IpcIo* reply);
    static int32_t DeathCallback(const IpcContext* context, void* ipcMsg, IpcIo* data, void* origin);
};
}
#endif