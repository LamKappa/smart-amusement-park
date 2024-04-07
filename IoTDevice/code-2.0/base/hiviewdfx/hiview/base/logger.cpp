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
#include "logger.h"

#include <cstdarg>
#include <cstdio>

#include <securec.h>

#include "hilog/log.h"

#ifdef LOG_DOMAIN
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD002D10
#endif

constexpr int LOG_BUF_LEN = 1024;

int HiviewLogDebug(const char *tag, unsigned int domain, const char *format, ...)
{
    OHOS::HiviewDFX::HiLogLabel logLabel = {LOG_CORE, domain, tag};
    int ret;
    char buf[LOG_BUF_LEN] = {0};
    va_list args;
    va_start(args, format);
    ret = vsnprintf_s(buf, LOG_BUF_LEN, LOG_BUF_LEN - 1, format, args);
    va_end(args);
    OHOS::HiviewDFX::HiLog::Debug(logLabel, "%{public}s ", buf);
    return ret;
}

int HiviewLogInfo(const char *tag, unsigned int domain, const char *format, ...)
{
    OHOS::HiviewDFX::HiLogLabel logLabel = {LOG_CORE, domain, tag};
    int ret;
    char buf[LOG_BUF_LEN] = {0};
    va_list args;
    va_start(args, format);
    ret = vsnprintf_s(buf, LOG_BUF_LEN, LOG_BUF_LEN - 1, format, args);
    va_end(args);
    OHOS::HiviewDFX::HiLog::Info(logLabel, "%{public}s", buf);
    return ret;
}

int HiviewLogWarn(const char *tag, unsigned int domain, const char *format, ...)
{
    OHOS::HiviewDFX::HiLogLabel logLabel = {LOG_CORE, domain, tag};
    int ret;
    char buf[LOG_BUF_LEN] = {0};
    va_list args;
    va_start(args, format);
    ret = vsnprintf_s(buf, LOG_BUF_LEN, LOG_BUF_LEN - 1, format, args);
    va_end(args);
    OHOS::HiviewDFX::HiLog::Warn(logLabel, "%{public}s", buf);
    return ret;
}

int HiviewLogError(const char *tag, unsigned int domain, const char *format, ...)
{
    OHOS::HiviewDFX::HiLogLabel logLabel = {LOG_CORE, domain, tag};
    int ret;
    char buf[LOG_BUF_LEN] = {0};
    va_list args;
    va_start(args, format);
    ret = vsnprintf_s(buf, LOG_BUF_LEN, LOG_BUF_LEN - 1, format, args);
    va_end(args);
    OHOS::HiviewDFX::HiLog::Error(logLabel, "%{public}s", buf);
    return ret;
}

int HiviewLogFatal(const char *tag, unsigned int domain, const char *format, ...)
{
    OHOS::HiviewDFX::HiLogLabel logLabel = {LOG_CORE, domain, tag};
    int ret;
    char buf[LOG_BUF_LEN] = {0};
    va_list args;
    va_start(args, format);
    ret = vsnprintf_s(buf, LOG_BUF_LEN, LOG_BUF_LEN - 1, format, args);
    va_end(args);
    OHOS::HiviewDFX::HiLog::Fatal(logLabel, "%{public}s", buf);
    return ret;
}
