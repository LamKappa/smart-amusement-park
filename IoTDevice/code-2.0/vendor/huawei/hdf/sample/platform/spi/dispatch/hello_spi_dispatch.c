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

#include "spi_if.h"
#include "hdf_log.h"

#define HDF_LOG_TAG hello_spi_dispatch

int main()
{
    int32_t ret;
    struct SpiDevInfo spiDevInfo;       /* SPI device descriptor */
    struct DevHandle *spiHandle = NULL; /* SPI device handle */
    spiDevInfo.busNum = 3;              /* SPI device bus number */
    spiDevInfo.csNum = 0;               /* SPI device CS number */
    spiHandle = SpiOpen(&spiDevInfo);
    if (spiHandle == NULL) {
        HDF_LOGE("SpiOpen failed");
        return HDF_FAILURE;
    }

    uint8_t wbuff[1] = {0x12};
    uint8_t rbuff[1] = {0};
    struct SpiMsg msg;        /* Custom message to be transferred */
    msg.wbuf = wbuff;         /* Pointer to the data to write */
    msg.rbuf = rbuff;         /* Pointer to the data to read */
    msg.len = 1;              /* The length of the data to be read or written is 1 bits. */
    msg.csChange = 1;         /* Disable the CS before the next transfer. */
    msg.delayUs = 0;          /* No delay before the next transfer */
    msg.speed = 115200;       /* Speed of this transfer */
    ret = SpiTransfer(spiHandle, &msg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SpiTransfer failed, ret %d", ret);
        return ret;
    }
    SpiClose(spiHandle);
    return ret;
}