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

#include "gpio_dispatch_sample.h"

#define HDF_LOG_TAG gpio_dispatch_sample

static int32_t SampleGpioSetDir(struct GpioCntlr *cntlr, struct HdfSBuf *data)
{
    uint16_t gpio;
    uint16_t dir;
    if (!HdfSbufReadUint16(data, &gpio) || !HdfSbufReadUint16(data, &dir)) {
        HDF_LOGE("%s: HdfSbufReadUint16 failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->ops->setDir == NULL) {
        HDF_LOGE("%s: cntlr->ops->setDir is NULL", __func__);
        return HDF_DEV_ERR_OP;
    }
    return cntlr->ops->setDir(cntlr, gpio, dir);
}

static int32_t SampleGpioGetDir(struct GpioCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint16_t gpio;
    uint16_t dir;
    if (!HdfSbufReadUint16(data, &gpio)) {
        HDF_LOGE("%s: HdfSbufReadUint16 failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->ops->getDir == NULL) {
        HDF_LOGE("%s: cntlr->ops->getDir is NULL", __func__);
        return HDF_DEV_ERR_OP;
    }
    ret = cntlr->ops->getDir(cntlr, gpio, &dir);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: cntlr->ops->getDir failed, ret: %d", __func__, ret);
        return ret;
    }
    if (!HdfSbufWriteUint16(reply, dir)) {
        HDF_LOGE("%s: HdfSbufWriteUint16 failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t SampleGpioWrite(struct GpioCntlr *cntlr, struct HdfSBuf *data)
{
    uint16_t gpio;
    uint16_t val;
    if (!HdfSbufReadUint16(data, &gpio) || !HdfSbufReadUint16(data, &val)) {
        HDF_LOGE("%s: HdfSbufReadUint16 failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->ops->write == NULL) {
        HDF_LOGE("%s: cntlr->ops->read is NULL", __func__);
        return HDF_DEV_ERR_OP;
    }
    return cntlr->ops->write(cntlr, gpio, val);
}

static int32_t SampleGpioRead(struct GpioCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint16_t gpio;
    uint16_t val;
    if (!HdfSbufReadUint16(data, &gpio)) {
        HDF_LOGE("%s: HdfSbufReadUint16 failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->ops->read == NULL) {
        HDF_LOGE("%s: cntlr->ops->read is NULL", __func__);
        return HDF_DEV_ERR_OP;
    }
    ret = cntlr->ops->read(cntlr, gpio, &val);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: cntlr->ops->read failed, ret: %d", __func__, ret);
        return ret;
    }
    if (!HdfSbufWriteUint16(reply, val)) {
        HDF_LOGE("%s: HdfSbufWriteUint16 failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t SampleGpioDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (client == NULL || client->device == NULL) {
        HDF_LOGE("%s: client or client->device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    struct GpioCntlr *cntlr = (struct GpioCntlr *)client->device->service;
    if (cntlr == NULL || cntlr->ops == NULL) {
        HDF_LOGE("%s: cntlr or cntlr->ops is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    switch (cmdId) {
        case GPIO_OPS_SET_DIR:
            return SampleGpioSetDir(cntlr, data);
        case GPIO_OPS_GET_DIR:
            return SampleGpioGetDir(cntlr, data, reply);
        case GPIO_OPS_WRITE:
            return SampleGpioWrite(cntlr, data);
        case GPIO_OPS_READ:
            return SampleGpioRead(cntlr, data, reply);
        default:
            HDF_LOGE("%s: invalid cmdId %d", __func__, cmdId);
            return HDF_FAILURE;
    }
}