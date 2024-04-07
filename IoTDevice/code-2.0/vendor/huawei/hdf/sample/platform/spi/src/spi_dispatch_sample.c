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

#include "spi_dispatch_sample.h"
#include "spi_sample.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"

#define HDF_LOG_TAG spi_dispatch_sample

static int32_t SampleSpiTransfer(struct SpiCntlr *cntlr, struct HdfSBuf *txBuf)
{
    HDF_LOGD("%s: Enter", __func__);
    uint32_t readSize = sizeof(struct SpiMsg);
    struct SpiMsg *msg = NULL;

    if (cntlr == NULL || cntlr->priv == NULL || txBuf == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadBuffer(txBuf, (const void **)&msg, &readSize)) {
        HDF_LOGE("%s: Failed to read sbuf", __func__);
        return HDF_DEV_ERR_NO_MEMORY;
    }

    if (SampleSpiCntlrTransfer(cntlr, msg, msg->len) != HDF_SUCCESS) {
        HDF_LOGE("%s: SampleSpiCntlrTransfer error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t SampleSpiDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (client == NULL || client->device == NULL) {
        HDF_LOGE("%s: client or client->device is NULL", __func__);
        return HDF_FAILURE;
    }

    struct SpiCntlr *cntlr = (struct SpiCntlr *)client->device->service;
    if (cntlr == NULL || cntlr->method == NULL) {
        HDF_LOGE("%s: cntlr or cntlr->method is NULL", __func__);
        return HDF_FAILURE;
    }

    switch (cmdId) {
        case SPI_TRANSFER:
            return SampleSpiTransfer(cntlr, data);
        default:
            HDF_LOGE("%s: invalid cmdId %d", __func__, cmdId);
            return HDF_FAILURE;
    }
}