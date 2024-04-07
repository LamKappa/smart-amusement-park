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

#include "time_util.h"

#include <sys/time.h>
#include <unistd.h>
#include <chrono>
namespace OHOS {
namespace HiviewDFX {
namespace TimeUtil {
uint64_t GenerateTimestamp()
{
    struct timeval now;
    if (gettimeofday(&now, nullptr) == -1) {
        return 0;
    }
    return (now.tv_sec * SEC_TO_MICROSEC + now.tv_usec);
}

void Sleep(unsigned int seconds)
{
    sleep(seconds);
}

int GetMillSecOfSec()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count() % SEC_TO_MILLISEC;
}
} // namespace TimeUtil
} // namespace HiviewDFX
} // namespace OHOS
