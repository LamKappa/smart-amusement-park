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

#include "common_utils.h"

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cerrno>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "securec.h"

using namespace std;
namespace OHOS {
namespace HiviewDFX {
namespace CommonUtils {
std::string GetProcNameByPid(pid_t pid)
{
    std::string result;
    char buf[BUF_SIZE_256] = {0};
    // cmdline is empty ? use comm instead
    snprintf_s(buf, BUF_SIZE_256, BUF_SIZE_256 - 1, "/proc/%d/comm", pid);
    FileUtil::LoadStringFromFile(std::string(buf, strlen(buf)), result);
    auto pos = result.find_last_not_of(" \n\r\t");
    if (pos == std::string::npos) {
        return result;
    }
    result.erase(pos + 1);
    return result;
}

pid_t GetPidByName(const std::string& processName)
{
    pid_t pid = -1;
    DIR* dir = opendir("/proc");
    if (dir == nullptr) {
        return pid;
    }

    struct dirent* ptr = nullptr;
    while ((ptr = readdir(dir)) != nullptr) {
        if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
            continue;
        }

        if (DT_DIR != ptr->d_type) {
            continue;
        }

        std::string cmdlinePath = std::string("/proc/").append(ptr->d_name).append("/cmdline");
        std::string curTaskName;
        FileUtil::LoadStringFromFile(cmdlinePath, curTaskName);
        if (curTaskName.find(processName) != std::string::npos) {
            if (sscanf_s(ptr->d_name, "%d", &pid) <= 0) {
                break;
            }
        }
    }
    closedir(dir);
    return pid;
}
}
} // namespace HiviewDFX
} // namespace OHOS
