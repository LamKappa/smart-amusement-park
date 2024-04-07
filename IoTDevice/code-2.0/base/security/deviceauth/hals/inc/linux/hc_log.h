/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef HC_LOG_H
#define HC_LOG_H

#ifdef HILOG_ENABLE

#include "hilog/log.h"

#define LOGD(fmt, ...) \
    ((void)HiLogPrint(LOG_CORE, LOG_DEBUG, LOG_DOMAIN, "[DEVAUTH]", "%s: " fmt, __FUNCTION__, ##__VA_ARGS__))
#define LOGE(fmt, ...) \
    ((void)HiLogPrint(LOG_CORE, LOG_ERROR, LOG_DOMAIN, "[DEVAUTH]", "%s: " fmt, __FUNCTION__, ##__VA_ARGS__))
#define LOGI(fmt, ...) \
    ((void)HiLogPrint(LOG_CORE, LOG_INFO, LOG_DOMAIN, "[DEVAUTH]", "%s: " fmt, __FUNCTION__, ##__VA_ARGS__))
#define LOGW(fmt, ...) \
    ((void)HiLogPrint(LOG_CORE, LOG_WARN, LOG_DOMAIN, "[DEVAUTH]", "%s: " fmt, __FUNCTION__, ##__VA_ARGS__))

#else

#include <stdio.h>
#include <stdlib.h>

#define LOGD(fmt, ...) printf("[D][%s]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define LOGE(fmt, ...) printf("[E][%s]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define LOGI(fmt, ...) printf("[I][%s]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define LOGW(fmt, ...) printf("[W][%s]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#endif

#endif
