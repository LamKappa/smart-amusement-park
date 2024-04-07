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

#include "lmkd_client.h"

#include <errno.h>
#include <memory>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "event_handler.h"
#include "securec.h"
#include "unique_fd.h"

namespace OHOS {
namespace AppExecFwk {

namespace {
constexpr std::string_view LMKD_SOCKET_PATH("/dev/socket/lmkd");
static_assert(LMKD_SOCKET_PATH.size() < UNIX_PATH_MAX);

constexpr int LMKD_SOCKET_TIMEOUT = 3;

constexpr size_t LMKD_MAX_TARGETS = 6;

constexpr int LMKD_CMD_TARGET = 0;
constexpr int LMKD_CMD_PROCPRIO = 1;
constexpr int LMKD_CMD_PROCREMOVE = 2;
constexpr int LMKD_CMD_PROCPURGE = 3;

constexpr int OOM_SCORE_ADJ_MIN = -1000;
constexpr int OOM_SCORE_ADJ_MAX = 1000;
}  // namespace

LmkdClient::LmkdClient() : socket_(-1)
{}

LmkdClient::~LmkdClient()
{
    if (IsOpen()) {
        Close();
    }
}

int32_t LmkdClient::Open()
{
    APP_LOGI("%{public}s(%{public}d) connecting lmkd.", __func__, __LINE__);

    if (socket_ >= 0) {
        APP_LOGE("%{public}s(%{public}d) already connected.", __func__, __LINE__);
        return -1;
    }

    UniqueFd sk(socket(PF_LOCAL, SOCK_SEQPACKET, 0));
    if (!sk) {
        APP_LOGE("%{public}s(%{public}d) failed to create local socket %{public}d.", __func__, __LINE__, errno);
        return (-errno);
    }

    struct timeval timeOut = {.tv_sec = LMKD_SOCKET_TIMEOUT, .tv_usec = 0};
    int fd = sk.Get();
    if (fd < 0) {
        APP_LOGE("%{public}s(%{public}d) fd is negative.", __func__, __LINE__);
        return -1;
    }
    if ((setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeOut, sizeof(timeOut)) != 0) ||
        (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeOut, sizeof(timeOut)) != 0)) {
        APP_LOGE("%{public}s(%{public}d) failed to set local socket timeout %{public}s.",
            __func__,
            __LINE__,
            strerror(errno));
        return (-errno);
    }

    struct sockaddr_un addr;
    int32_t ret;
    ret = memset_s(&addr, sizeof(addr), 0, sizeof(addr));
    if (ret != EOK) {
        APP_LOGE("%{public}s(%{public}d) failed to clear local socket addr.", __func__, __LINE__);
        return ret;
    }

    ret = memcpy_s(addr.sun_path, UNIX_PATH_MAX, LMKD_SOCKET_PATH.data(), LMKD_SOCKET_PATH.size());
    if (ret != EOK) {
        APP_LOGE("%{public}s(%{public}d) failed to make local socket path.", __func__, __LINE__);
        return ret;
    }

    addr.sun_family = AF_LOCAL;
    socklen_t addrLen = offsetof(struct sockaddr_un, sun_path) + LMKD_SOCKET_PATH.size() + 1;
    if (connect(sk, reinterpret_cast<struct sockaddr *>(&addr), addrLen) < 0) {
        APP_LOGE("%{public}s(%{public}d) failed to connect to lmkd %{public}s.", __func__, __LINE__, strerror(errno));
        return (-errno);
    }

    socket_ = sk.Release();

    return ERR_OK;
}

void LmkdClient::Close()
{
    APP_LOGI("%{public}s(%{public}d) closing lmkd.", __func__, __LINE__);

    if (socket_ < 0) {
        APP_LOGE("%{public}s(%{public}d) not connected.", __func__, __LINE__);
        return;
    }

    close(socket_);
    socket_ = -1;
}

bool LmkdClient::IsOpen() const
{
    return socket_ >= 0;
}

int32_t LmkdClient::Target(const Targets &targets)
{
    if (targets.empty() || targets.size() > LMKD_MAX_TARGETS) {
        APP_LOGE(
            "%{public}s(%{public}d) empty target or too many targets. %{public}zu", __func__, __LINE__, targets.size());
        return (-EINVAL);
    }

    int i = 0;
    int32_t buf[1 + 2 * LMKD_MAX_TARGETS];
    buf[i++] = LMKD_CMD_TARGET;

    for (auto target : targets) {
        if (target.first < 0 || !CheckOomAdj(target.second)) {
            APP_LOGE("%{public}s(%{public}d) invalid target: %{public}d %{public}d",
                __func__,
                __LINE__,
                target.first,
                target.second);
            return (-EINVAL);
        }
        buf[i++] = target.first;
        buf[i++] = target.second;
    }

    return Write(buf, i * sizeof(int32_t)) ? ERR_OK : -1;
}

int32_t LmkdClient::ProcPrio(pid_t pid, uid_t uid, int oomAdj)
{
    if (pid < 0 || uid < 0 || !CheckOomAdj(oomAdj)) {
        APP_LOGE("%{public}s(%{public}d) invalid parameter: %{public}d %{public}d %{public}d.",
            __func__,
            __LINE__,
            pid,
            uid,
            oomAdj);
        return (-EINVAL);
    }

    int32_t buf[4] = {LMKD_CMD_PROCPRIO, pid, uid, oomAdj};

    return Write(buf, sizeof(buf)) ? ERR_OK : -1;
}

int32_t LmkdClient::ProcRemove(pid_t pid)
{
    if (pid < 1) {
        APP_LOGE("%{public}s(%{public}d) invalid pid %{public}d.", __func__, __LINE__, pid);
        return (-EINVAL);
    }

    int32_t buf[2] = {LMKD_CMD_PROCREMOVE, pid};

    return Write(buf, sizeof(buf)) ? ERR_OK : -1;
}

bool LmkdClient::ProcPurge()
{
    int32_t cmd = LMKD_CMD_PROCPURGE;

    return Write(reinterpret_cast<void *>(&cmd), sizeof(cmd));
}

bool LmkdClient::CheckOomAdj(int v)
{
    return (OOM_SCORE_ADJ_MIN <= v && v <= OOM_SCORE_ADJ_MAX);
}

bool LmkdClient::Write(const void *buf, size_t len)
{
    if (buf == nullptr || len < 1) {
        APP_LOGE("%{public}s(%{public}d) invalid parameter.", __func__, __LINE__);
        return false;
    }

    constexpr int retryTimes = 5;
    for (int i = 0; i < retryTimes; ++i) {
        if (socket_ < 0 && !Open()) {
            std::this_thread::yield();
            continue;
        }
        int ret = TEMP_FAILURE_RETRY(send(socket_, buf, len, 0));
        if (ret <= 0) {
            APP_LOGE("%{public}s(%{public}d) failed to send data to lmkd %{public}d.", __func__, __LINE__, errno);
            Close();
            std::this_thread::yield();
        } else {
            return true;
        }
    }

    return false;
}
}  // namespace AppExecFwk
}  // namespace OHOS
