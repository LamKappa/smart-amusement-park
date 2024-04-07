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

#ifndef SPI_IF_H
#define SPI_IF_H

#include "hdf_platform.h"

#define SPI_DEV_SERVICE_NAME_PREFIX "HDF_PLATFORM_SPI_%u"
#define MAX_DEV_NAME_SIZE 32

struct SpiDevInfo {
    uint32_t busNum;
    uint32_t csNum;
};

struct SpiMsg {
    uint8_t *wbuf;
    uint8_t *rbuf;
    uint32_t len;
    uint32_t speed;
    uint16_t delayUs;
    uint8_t csChange;
};

enum {
    SPI_TRANSFER = 1
};

DevHandle SpiOpen(const struct SpiDevInfo *info);
void SpiClose(DevHandle handle);
int32_t SpiTransfer(DevHandle handle, struct SpiMsg *msgs);

#endif // SPI_IF_H
