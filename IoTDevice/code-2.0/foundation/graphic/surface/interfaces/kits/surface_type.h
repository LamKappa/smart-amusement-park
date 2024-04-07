/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

/**
 * @addtogroup Surface
 * @{
 *
 * @brief Provides shared memory for multimedia and graphics.
 *
 * This module is used to apply for and release shared memory and is used by the multimedia and graphics modules
 * across processes.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file surface_type.h
 *
 * @brief Provides the capabilities of setting shared memory parameters.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_SURFACE_TYPE_H
#define GRAPHIC_LITE_SURFACE_TYPE_H

#include "gfx_utils/pixel_format_utils.h"

namespace OHOS {
#define SURFACE_MAX_WIDTH 7680
#define SURFACE_MAX_HEIGHT 7680
#define SURFACE_MAX_QUEUE_SIZE 10
#define SURFACE_MIN_QUEUE_SIZE 1
#define SURFACE_DEFAULT_QUEUE_SIZE 1
#define SURFACE_MAX_STRIDE_ALIGNMENT 32
#define SURFACE_MIN_STRIDE_ALIGNMENT 4
#define SURFACE_DEFAULT_STRIDE_ALIGNMENT 4
#define SURFACE_MAX_SIZE 58982400 // 8K * 8K

/**
 * @brief Enumerates shared memory usage scenarios, including physically contiguous memory and virtual memory.
 *
 */
enum BufferConsumerUsage {
    /** Virtual memory */
    BUFFER_CONSUMER_USAGE_SORTWARE = 0,
    /** Physically contiguous memory. The cache is not used. */
    BUFFER_CONSUMER_USAGE_HARDWARE,
    /** Physically contiguous memory. Consumers use the cache. */
    BUFFER_CONSUMER_USAGE_HARDWARE_CONSUMER_CACHE,
    /** Physically contiguous memory. Producers use the cache. */
    BUFFER_CONSUMER_USAGE_HARDWARE_PRODUCER_CACHE,
    /** Valid maximum value, used to determine whether the current shared memory usage scenario is within a proper
     *  range. */
    BUFFER_CONSUMER_USAGE_MAX
};
} // end namespace OHOS
#endif