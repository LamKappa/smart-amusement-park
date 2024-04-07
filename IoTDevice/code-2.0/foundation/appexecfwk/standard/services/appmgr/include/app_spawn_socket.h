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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_SPAWN_SOCKET_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_SPAWN_SOCKET_H

#include <string>

#include "nocopyable.h"
#include "client_socket.h"
#include "appexecfwk_errors.h"

namespace OHOS {
namespace AppExecFwk {

class AppSpawnSocket {
public:
    /**
     * Constructor.
     */
    AppSpawnSocket();

    /**
     * Destructor
     */
    virtual ~AppSpawnSocket();

    /**
     * Disable copy.
     */
    DISALLOW_COPY_AND_MOVE(AppSpawnSocket);

    /**
     * Create client socket and connect the socket.
     */
    virtual ErrCode OpenAppSpawnConnection();

    /**
     * Close the client socket.
     */
    virtual void CloseAppSpawnConnection();

    /**
     * Write message to the socket.
     *
     * @param buf, message pointer.
     * @param len, message size.
     */
    virtual ErrCode WriteMessage(const void *buf, const int32_t len);

    /**
     * Read message from the socket.
     *
     * @param buf, message pointer.
     * @param len, message size.
     */
    virtual ErrCode ReadMessage(void *buf, const int32_t len);

    /**
     * Set function, unit test also use it.
     *
     * @param clientSocket, client socket.
     */
    void SetClientSocket(const std::shared_ptr<OHOS::AppSpawn::ClientSocket> clientSocket);

private:
    std::shared_ptr<AppSpawn::ClientSocket> clientSocket_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_SPAWN_SOCKET_H