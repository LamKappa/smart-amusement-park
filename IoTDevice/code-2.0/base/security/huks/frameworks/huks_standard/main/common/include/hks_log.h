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

#ifndef HKS_LOG_H
#define HKS_LOG_H

#include "hks_type.h"

enum HksLogLevel {
    HKS_LOG_LEVEL_I,
    HKS_LOG_LEVEL_E,
    HKS_LOG_LEVEL_W,
    HKS_LOG_LEVEL_D,
};

#ifdef __cplusplus
extern "C" {
#endif

void HksLog(uint32_t logLevel, const char *funcName, uint32_t lineNo, const char *format, ...);

#define HKS_LOG_I(...) HksLog(HKS_LOG_LEVEL_I, __func__, __LINE__, __VA_ARGS__)
#define HKS_LOG_W(...) HksLog(HKS_LOG_LEVEL_W, __func__, __LINE__, __VA_ARGS__)
#define HKS_LOG_E(...) HksLog(HKS_LOG_LEVEL_E, __func__, __LINE__, __VA_ARGS__)
#define HKS_LOG_D(...) HksLog(HKS_LOG_LEVEL_D, __func__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* HKS_LOG_H */
