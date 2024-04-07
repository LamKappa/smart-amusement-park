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

#include "app_spawn_socket.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

// arg "AppSpawn" cannot be defined as string object since REGISTER_SYSTEM_ABILITY will
// firstly start without init this string object, which leads to error.

AppSpawnSocket::AppSpawnSocket() : clientSocket_(std::make_unique<AppSpawn::ClientSocket>("AppSpawn"))
{}

AppSpawnSocket::~AppSpawnSocket()
{}

ErrCode AppSpawnSocket::OpenAppSpawnConnection()
{
    APP_LOGD("ready to open connection");
    if (clientSocket_) {
        if (clientSocket_->CreateClient() != ERR_OK) {
            APP_LOGE("failed to create socketClient");
            return ERR_APPEXECFWK_BAD_APPSPAWN_CLIENT;
        }
        if (clientSocket_->ConnectSocket() != ERR_OK) {
            APP_LOGE("failed to connect socket");
            return ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED;
        }
        APP_LOGD("connection has been opened");
        return ERR_OK;
    }
    APP_LOGE("failed to open connection without socket");
    return ERR_APPEXECFWK_BAD_APPSPAWN_SOCKET;
}

void AppSpawnSocket::CloseAppSpawnConnection()
{
    if (clientSocket_) {
        clientSocket_->CloseClient();
    }
}

ErrCode AppSpawnSocket::WriteMessage(const void *buf, const int32_t len)
{
    APP_LOGD("ready to write message");
    if (len <= 0) {
        APP_LOGE("failed to write message due to invalid length of message");
        return ERR_INVALID_VALUE;
    }
    if (buf == nullptr) {
        APP_LOGE("failed to write message due to null buf");
        return ERR_INVALID_VALUE;
    }
    if (clientSocket_) {
        if (clientSocket_->WriteSocketMessage(buf, len) != len) {
            APP_LOGE("failed to write message due to invalid write length");
            return ERR_APPEXECFWK_SOCKET_WRITE_FAILED;
        }
        APP_LOGD("write message success");
        return ERR_OK;
    }

    APP_LOGE("failed to write message without socket");
    return ERR_APPEXECFWK_BAD_APPSPAWN_SOCKET;
}

ErrCode AppSpawnSocket::ReadMessage(void *buf, const int32_t len)
{
    APP_LOGD("ready to read message");
    if (len <= 0) {
        APP_LOGE("failed to read message due to invalid length of cache");
        return ERR_INVALID_VALUE;
    }
    if (buf == nullptr) {
        APP_LOGE("failed to read message due to null buf");
        return ERR_INVALID_VALUE;
    }
    if (clientSocket_) {
        if (clientSocket_->ReadSocketMessage(buf, len) != len) {
            APP_LOGE("failed to read message due to invalid read length");
            return ERR_APPEXECFWK_SOCKET_READ_FAILED;
        }
        APP_LOGD("read message success");
        return ERR_OK;
    }
    APP_LOGE("failed to read message without socket");
    return ERR_APPEXECFWK_BAD_APPSPAWN_CLIENT;
}

void AppSpawnSocket::SetClientSocket(const std::shared_ptr<OHOS::AppSpawn::ClientSocket> clientSocket)
{
    clientSocket_ = clientSocket;
}

}  // namespace AppExecFwk
}  // namespace OHOS
