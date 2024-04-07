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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_SPAWN_CLIENT_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_SPAWN_CLIENT_H

#include "nocopyable.h"
#include "app_spawn_msg_wrapper.h"
#include "app_spawn_socket.h"

namespace OHOS {
namespace AppExecFwk {

enum class SpawnConnectionState { STATE_NOT_CONNECT, STATE_CONNECTED, STATE_CONNECT_FAILED };

class AppSpawnClient {
public:
    /**
     * Constructor.
     */
    AppSpawnClient();

    /**
     * Destructor
     */
    virtual ~AppSpawnClient() = default;

    /**
     * Disable copy.
     */
    DISALLOW_COPY_AND_MOVE(AppSpawnClient);

    /**
     * Try connect to appspawn.
     */
    ErrCode OpenConnection();

    /**
     * Close the connect of appspawn.
     */
    void CloseConnection();

    /**
     * AppSpawnClient core function, Start request to appspawn.
     *
     * @param startMsg, request message.
     * @param pid, pid of app process, get from appspawn.
     */
    virtual ErrCode StartProcess(const AppSpawnStartMsg &startMsg, pid_t &pid);

    /**
     * Return the connect state.
     */
    SpawnConnectionState QueryConnectionState() const;

    /**
     * Set function, unit test also use it.
     */
    void SetSocket(const std::shared_ptr<AppSpawnSocket> socket);

private:
    /**
     * AppSpawnClient core function,
     *
     * @param startMsg, request message.
     * @param pid, pid of app process, get it from appspawn.
     */
    ErrCode StartProcessImpl(const AppSpawnStartMsg &startMsg, pid_t &pid);

private:
    std::shared_ptr<AppSpawnSocket> socket_;
    SpawnConnectionState state_ = SpawnConnectionState::STATE_NOT_CONNECT;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_SPAWN_CLIENT_H
