/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hks_log.h"

#include <securec.h>
#include "hilog/log.h"

#undef LOG_DOMAIN
#undef LOG_TAG
static const unsigned int LOG_DOMAIN = 0xD002F01;
static const char* LOG_TAG = "HUKS";

#define MAX_LOG_BUFF_LEN 512

void HksLog(uint32_t logLevel, const char *funcName, uint32_t lineNo, const char *format, ...)
{
    char buf[MAX_LOG_BUFF_LEN] = {0};
    va_list ap = {0};
    va_start(ap, format);
    int32_t ret = vsnprintf_s(buf, MAX_LOG_BUFF_LEN, sizeof(buf) - 1, format, ap);
    va_end(ap);
    if (ret < 0) {
        HILOG_ERROR(LOG_CORE, "hks log concatenate error.");
        return;
    }

    switch (logLevel) {
        case HKS_LOG_LEVEL_I:
            HILOG_INFO(LOG_CORE, "%{public}s[%{public}u]: %{public}s\n", funcName, lineNo, buf);
            break;
        case HKS_LOG_LEVEL_E:
            HILOG_ERROR(LOG_CORE, "%{public}s[%{public}u]: %{public}s\n", funcName, lineNo, buf);
            break;
        case HKS_LOG_LEVEL_W:
            HILOG_WARN(LOG_CORE, "%{public}s[%{public}u]: %{public}s\n", funcName, lineNo, buf);
            break;
        case HKS_LOG_LEVEL_D:
            HILOG_DEBUG(LOG_CORE, "%{public}s[%{public}u]: %{private}s\n", funcName, lineNo, buf);
            break;
        default:
            return;
    }
}