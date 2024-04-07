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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_TEST_UT_MOCK_APP_SPAWN_SOCKET_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_TEST_UT_MOCK_APP_SPAWN_SOCKET_H

#include "gmock/gmock.h"
#include "app_spawn_socket.h"
#include "app_spawn_msg_wrapper.h"
#include "securec.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

class MockAppSpawnSocket : public AppSpawnSocket {
public:
    MockAppSpawnSocket() = default;
    virtual ~MockAppSpawnSocket() = default;

    MOCK_METHOD0(OpenAppSpawnConnection, ErrCode());
    MOCK_METHOD0(CloseAppSpawnConnection, void());
    MOCK_METHOD2(WriteMessage, ErrCode(const void *buf, const int32_t len));
    MOCK_METHOD2(ReadMessage, ErrCode(void *buf, int32_t len));

    ErrCode ReadImpl(void *buf, [[maybe_unused]] int32_t len)
    {
        if (buf == nullptr) {
            return ERR_NO_MEMORY;
        }
        AppSpawnPidMsg msg;
        msg.pid = expectPid_;
        if (memcpy_s(buf, sizeof(msg), msg.pidBuf, sizeof(msg)) != 0) {
            return ERR_APPEXECFWK_ASSEMBLE_START_MSG_FAILED;
        }
        return ERR_OK;
    }

    void SetExpectPid(int32_t expectPid)
    {
        expectPid_ = expectPid;
    }

private:
    int32_t expectPid_ = 0;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_TEST_UT_MOCK_APP_SPAWN_SOCKET_H
