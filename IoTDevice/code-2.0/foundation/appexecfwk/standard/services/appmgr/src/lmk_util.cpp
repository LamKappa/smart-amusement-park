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

#include "lmk_util.h"

#include <fcntl.h>
#include <string.h>
#include <signal.h>

#include "securec.h"

#include "cgroup_manager.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

namespace {
// pressure level low
constexpr int LMK_OOM_ADJ_LOW = 800;
// pressure level medium
constexpr int LMK_OOM_ADJ_MEDIUM = 600;
// pressure level critical
constexpr int LMK_OOM_ADJ_CRITICAL = 0;
constexpr int MemoryLevel[] = {LMK_OOM_ADJ_LOW, LMK_OOM_ADJ_MEDIUM, LMK_OOM_ADJ_CRITICAL};

constexpr int PROC_PATH_MAX = 256;
constexpr int PROC_LINE_MAX = 128;
}  // namespace

LmkUtil::LmkUtil()
{}

LmkUtil::~LmkUtil()
{}

std::string LmkUtil::GetProcName(pid_t pid)
{
    std::string name;

    if (pid < 1) {
        APP_LOGW("%{public}s(%{public}d) invalid pid %{public}d.", __func__, __LINE__, pid);
        return name;
    }

    char path[PROC_PATH_MAX];
    char line[PROC_LINE_MAX];
    int fd = -1;
    ssize_t ret = -1;

    if (memset_s(path, sizeof(path), 0x00, sizeof(path)) != EOK) {
        APP_LOGE("%{public}s(%{public}d) failed to memset path", __func__, __LINE__);
        return name;
    }

    if (memset_s(line, sizeof(line), 0x00, sizeof(line)) != EOK) {
        APP_LOGE("%{public}s(%{public}d) failed to memset line", __func__, __LINE__);
        return name;
    }

    if (snprintf_s(path, PROC_PATH_MAX, PROC_PATH_MAX - 1, "/proc/%d/cmdline", pid) < 0) {
        return name;
    }

    fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        return name;
    }

    ret = ReadAll(fd, line, sizeof(line) - 1);
    close(fd);
    if (ret < 0) {
        return name;
    }

    if (ret < PROC_LINE_MAX) {
        line[ret] = '\0';
    } else {
        line[PROC_LINE_MAX - 1] = '\0';
    }
    // assign name
    name = strchr(line, ' ');

    return name;
}

int32_t LmkUtil::GetProcSize(pid_t pid)
{
    int rss = 0;

    if (pid <= 0) {
        APP_LOGW("%{public}s(%{public}d) invalid pid %{public}d.", __func__, __LINE__, pid);
        return rss;
    }

    char path[PROC_PATH_MAX];
    char line[PROC_LINE_MAX];
    int fd = -1;
    int total = -1;
    ssize_t ret = -1;

    if (memset_s(path, sizeof(path), 0x00, sizeof(path)) != EOK) {
        APP_LOGE("%{public}s(%{public}d) failed to memset path", __func__, __LINE__);
        return -1;
    }

    if (memset_s(line, sizeof(line), 0x00, sizeof(line)) != EOK) {
        APP_LOGE("%{public}s(%{public}d) failed to memset line", __func__, __LINE__);
        return -1;
    }

    if (snprintf_s(path, PROC_PATH_MAX, PROC_PATH_MAX - 1, "/proc/%d/statm", pid) < 0) {
        return rss;
    }

    fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd == -1)
        return -1;

    ret = ReadAll(fd, line, sizeof(line) - 1);
    if (ret < 0) {
        close(fd);
        return -1;
    }

    if (ret < PROC_LINE_MAX) {
        line[ret] = '\0';
    } else {
        line[PROC_LINE_MAX - 1] = '\0';
    }

    if (sscanf_s(line, "%d %d ", &total, &rss) <= 0) {
        close(fd);
        return 0;
    }

    close(fd);
    return rss;
}

int32_t LmkUtil::KillProcess(std::list<AppPtr> &listApp, const CgroupManager::LowMemoryLevel level, int64_t &rss)
{
    int32_t killCnt = 0;
    int64_t taskSize = 0;
    std::list<AppPtr>::iterator iter = listApp.begin();
    int killThreshold = LMK_OOM_ADJ_LOW;

    if (level < CgroupManager::LOW_MEMORY_LEVEL_MAX) {
        killThreshold = MemoryLevel[level];
    }

    while (iter != listApp.end()) {
        auto priorityObject = (*iter)->GetPriorityObject();
        if (priorityObject != nullptr && priorityObject->GetCurAdj() >= killThreshold) {
            auto pid = priorityObject->GetPid();
            if (pid <= 0) {
                APP_LOGE("%{public}s(%{public}d) pid %{public}d invalid", __func__, __LINE__, pid);
                iter = listApp.erase(iter);
                continue;
            }

            auto procName = GetProcName(pid);
            if (procName.empty()) {
                APP_LOGW("%{public}s(%{public}d) pid %{public}d process name emptry", __func__, __LINE__, pid);
            }

            auto procSize = GetProcSize(pid);
            if (procSize <= 0) {
                APP_LOGW("%{public}s(%{public}d) pid %{public}d process size error", __func__, __LINE__, pid);
            }

            // kill process
            int ret = kill(pid, SIGKILL);
            if (ret) {
                APP_LOGW("%{public}s(%{public}d) kill pid %{public}d error", __func__, __LINE__, pid);
            } else {
                APP_LOGI("%{public}s(%{public}d) pid %{public}d", __func__, __LINE__, pid);

                // inc count and rss
                taskSize += procSize;
                killCnt++;

                // remove app which pid been killed
                iter = listApp.erase(iter);
                continue;
            }
        }
        iter++;
    }

    // out rss size
    rss = taskSize;

    return killCnt;
}

ssize_t LmkUtil::ReadAll(int fd, char *buf, size_t maxLen)
{
    ssize_t ret = 0;
    off_t offSet = 0;

    while (maxLen > 0) {
        ssize_t rc = TEMP_FAILURE_RETRY(pread(fd, buf, maxLen, offSet));
        if (rc == 0) {
            break;
        }
        if (rc == -1) {
            return -1;
        }
        ret += rc;
        buf += rc;
        offSet += rc;
        maxLen -= rc;
    }

    return ret;
}
}  // namespace AppExecFwk
}  // namespace OHOS
