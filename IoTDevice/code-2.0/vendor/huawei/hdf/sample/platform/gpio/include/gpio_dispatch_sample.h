/* Copyright 2020 Huawei Device Co., Ltd.
 *
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

#ifndef GPIO_DISPATCH_SAMPLE_H
#define GPIO_DISPATCH_SAMPLE_H

#include "gpio_pl061_sample.h"

enum GpioOps {
    GPIO_OPS_SET_DIR = 1,
    GPIO_OPS_GET_DIR,
    GPIO_OPS_WRITE,
    GPIO_OPS_READ
};

int32_t SampleGpioDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply);

#endif // GPIO_DISPATCH_SAMPLE_H
