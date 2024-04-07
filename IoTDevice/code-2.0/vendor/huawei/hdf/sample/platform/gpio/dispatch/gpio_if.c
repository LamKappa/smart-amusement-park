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

#include "gpio_if.h"
#include "hdf_log.h"
#include "hdf_io_service_if.h"

#define HDF_LOG_TAG gpio_if

static struct HdfIoService *GetIoService()
{
    static struct HdfIoService *ioService = NULL;
    if (ioService != NULL) {
        return ioService;
    }

    ioService = HdfIoServiceBind(GPIO_SERVICE_NAME);
    if (ioService == NULL) {
        HDF_LOGE("Failed to get service %s", GPIO_SERVICE_NAME);
    }
    return ioService;
}

static int32_t GpioOperate(enum GpioOps ops, uint16_t gpio, uint16_t val)
{
    int ret = HDF_FAILURE;
    struct HdfIoService *service = GetIoService();
    if (service == NULL) {
        return ret;
    }
    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("Failed to obtain sBuf");
        return ret;
    }

    if (!HdfSbufWriteUint16(data, gpio) || !HdfSbufWriteUint16(data, val)) {
        HDF_LOGE("Failed to write sBuf");
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, ops, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("Failed to send service call, ret: %d", ret);
    }
    HdfSBufRecycle(data);
    return ret;
}

static int32_t GpioQuery(enum GpioOps ops, uint16_t gpio, uint16_t *val)
{
    int ret = HDF_FAILURE;
    struct HdfIoService *service = GetIoService();
    if (service == NULL) {
        return ret;
    }
    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (data == NULL || reply == NULL) {
        HDF_LOGE("Failed to obtain sBuf");
        return ret;
    }

    if (!HdfSbufWriteUint16(data, gpio)) {
        HDF_LOGE("Failed to write data sBuf");
        goto __ERR__;
    }
    ret = service->dispatcher->Dispatch(&service->object, ops, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("Failed to send service call");
        goto __ERR__;

    }
    if (!HdfSbufReadUint16(reply, val)) {
        HDF_LOGE("Failed to read reply sBuf");
        goto __ERR__;
    }
    goto __ERR__;

    __ERR__:
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

int32_t GpioOpen()
{
    struct HdfIoService *ioService = GetIoService();
    if (ioService == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t GpioSetDir(uint16_t gpio, uint16_t dir)
{
    return GpioOperate(GPIO_OPS_SET_DIR, gpio, dir);
}

int32_t GpioGetDir(uint16_t gpio, uint16_t *dir)
{
    return GpioQuery(GPIO_OPS_GET_DIR, gpio, dir);
}

int32_t GpioWrite(uint16_t gpio, uint16_t val)
{
    return GpioOperate(GPIO_OPS_WRITE, gpio, val);
}

int32_t GpioRead(uint16_t gpio, uint16_t *val)
{
    return GpioQuery(GPIO_OPS_READ, gpio, val);
}

int32_t GpioClose()
{
    struct HdfIoService *ioService = GetIoService();
    if (ioService == NULL) {
        return HDF_FAILURE;
    }
    HdfIoServiceRecycle(ioService);
    return HDF_SUCCESS;
}