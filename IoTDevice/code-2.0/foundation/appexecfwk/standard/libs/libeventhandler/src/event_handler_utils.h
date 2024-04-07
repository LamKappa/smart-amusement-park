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

#ifndef FOUNDATION_APPEXECFWK_LIBS_LIBEVENTHANDLER_SRC_EVENT_HANDLER_UTILS_H
#define FOUNDATION_APPEXECFWK_LIBS_LIBEVENTHANDLER_SRC_EVENT_HANDLER_UTILS_H

#include <cerrno>
#include <chrono>
#include <cstring>
#include <string>

#include "hilog/log.h"
#include "inner_event.h"

#define DEFINE_HILOG_LABEL(name) static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_DOMAIN, name}

#define HILOGE(...) OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, ##__VA_ARGS__)
#define HILOGW(...) OHOS::HiviewDFX::HiLog::Warn(LOG_LABEL, ##__VA_ARGS__)
#define HILOGI(...) OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, ##__VA_ARGS__)
#define HILOGD(...) OHOS::HiviewDFX::HiLog::Debug(LOG_LABEL, ##__VA_ARGS__)

namespace OHOS {
namespace AppExecFwk {
inline const int64_t NANOSECONDS_PER_ONE_MILLISECOND = 1000000;
inline const int64_t NANOSECONDS_PER_ONE_SECOND = 1000000000;
inline const int32_t INFINITE_TIMEOUT = -1;

// Help to convert time point into delay time from now.
static inline int64_t TimePointToTimeOut(const InnerEvent::TimePoint &when)
{
    if (when <= InnerEvent::Clock::now()) {
        return 0;
    }

    auto duration = when - InnerEvent::Clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

static inline int32_t NanosecondsToTimeout(int64_t nanoseconds)
{
    if (nanoseconds < 0) {
        return INFINITE_TIMEOUT;
    }

    int64_t milliseconds = nanoseconds / NANOSECONDS_PER_ONE_MILLISECOND;
    if ((nanoseconds % NANOSECONDS_PER_ONE_MILLISECOND) > 0) {
        milliseconds += 1;
    }

    return (milliseconds > INT32_MAX) ? INT32_MAX : static_cast<int32_t>(milliseconds);
}

static inline char *GetLastErr()
{
    return strerror(errno);
}
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // #ifndef FOUNDATION_APPEXECFWK_LIBS_LIBEVENTHANDLER_SRC_EVENT_HANDLER_UTILS_H
