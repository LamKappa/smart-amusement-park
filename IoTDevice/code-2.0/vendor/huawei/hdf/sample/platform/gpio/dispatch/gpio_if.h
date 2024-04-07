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

#ifndef GPIO_IF_H
#define GPIO_IF_H

#include <stdint.h>

#define GPIO_SERVICE_NAME "GPIO_SAMPLE"

enum GpioValue {
    GPIO_VAL_LOW = 0,
    GPIO_VAL_HIGH = 1,
    GPIO_VAL_ERR,
};

enum GpioDirType {
    GPIO_DIR_IN = 0,
    GPIO_DIR_OUT = 1,
    GPIO_DIR_ERR,
};

enum GpioOps {
    GPIO_OPS_SET_DIR = 1,
    GPIO_OPS_GET_DIR,
    GPIO_OPS_WRITE,
    GPIO_OPS_READ
};

int32_t GpioOpen();
int32_t GpioClose();
int32_t GpioSetDir(uint16_t gpio, uint16_t dir);
int32_t GpioGetDir(uint16_t gpio, uint16_t *dir);
int32_t GpioWrite(uint16_t gpio, uint16_t val);
int32_t GpioRead(uint16_t gpio, uint16_t *val);

#endif // GPIO_IF_H