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

#include "event_server.h"

#include <memory>
#include <string>
#include <vector>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <securec.h>

#include "logger.h"
#include "socket_util.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-EventServer");
namespace {
constexpr int BUFFER_SIZE = 128 * 1024;
}
void EventServer::InitSocket(int &socketId)
{
    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_UNIX;
    if (strcpy_s(serverAddr.sun_path, sizeof(serverAddr.sun_path), "/dev/socket/hisysevent") != EOK) {
        return;
    }
    serverAddr.sun_path[sizeof(serverAddr.sun_path) - 1] = '\0';
    socketId = TEMP_FAILURE_RETRY(socket(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0));
    if (socketId < 0) {
        HIVIEW_LOGE("create hisysevent socket fail %d", errno);
        return;
    }
    unlink(serverAddr.sun_path);
    if (TEMP_FAILURE_RETRY(bind(socketId, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr))) < 0) {
        HIVIEW_LOGE("bind hisysevent socket fail %d", errno);
        return;
    }
}

void EventServer::Start()
{
    HIVIEW_LOGE("start event server");
    socketId_ = SocketUtil::GetHiviewExistingSocketServer("hisysevent", SOCK_DGRAM);
    if (socketId_ < 0) {
        HIVIEW_LOGI("create hisysevent socket");
        InitSocket(socketId_);
    } else {
        HIVIEW_LOGI("use hisysevent exist socket");
    }

    if (socketId_ < 0) {
        HIVIEW_LOGE("hisysevent create socket fail");
        return;
    }

    isStart_ = true;
    while (isStart_) {
        struct sockaddr_un clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        char recvbuf[BUFFER_SIZE] = {0};
        int n = recvfrom(socketId_, recvbuf, sizeof(recvbuf), 0, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (n <= 0) {
            continue;
        }
        recvbuf[BUFFER_SIZE - 1] = 0;
        HIVIEW_LOGD("receive data from client %s", recvbuf);
        for (auto receiver = receivers_.begin(); receiver != receivers_.end(); receiver++) {
            (*receiver)->HandlerEvent(std::string(recvbuf));
        }
    }
}

void EventServer::Stop()
{
    isStart_ = false;
    if (socketId_ <= 0) {
        return;
    }
    close(socketId_);
    socketId_ = -1;
}

void EventServer::AddReceiver(std::shared_ptr<EventReceiver> receiver)
{
    receivers_.emplace_back(receiver);
}
} // namespace HiviewDFX
} // namespace OHOS
