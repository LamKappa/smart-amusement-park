/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sdio_core.h"
#include "device_resource_if.h"
#include "osal_mem.h"
#include "plat_log.h"

#define HDF_LOG_TAG sdio_core

struct SdioCntlr *SdioCntlrCreateAndBind(struct HdfDeviceObject *device)
{
    struct SdioCntlr *cntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("SdioCntlrCreateAndBind: device is NULL!");
        return NULL;
    }

    cntlr = (struct SdioCntlr *)OsalMemCalloc(sizeof(*cntlr));
    if (cntlr == NULL) {
        HDF_LOGE("SdioCntlrCreateAndBind: malloc host fail!");
        return NULL;
    }
    cntlr->device = device;
    device->service = &cntlr->service;
    return cntlr;
}

void SdioCntlrDestroy(struct SdioCntlr *cntlr)
{
    if (cntlr != NULL) {
        cntlr->device = NULL;
        cntlr->priv = NULL;
        cntlr->method = NULL;
        OsalMemFree(cntlr);
    }
}

int32_t SdioFillConfigData(struct HdfDeviceObject *device, struct SdioConfigData *configData)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (device == NULL || configData == NULL) {
        HDF_LOGE("SdioFillConfigData: input para is NULL.");
        return HDF_FAILURE;
    }

    node = device->property;
    if (node == NULL) {
        HDF_LOGE("SdioFillConfigData: drs node is NULL.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("SdioFillConfigData: invalid drs ops fail!");
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "funcNum", &(configData->funcNum), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioFillConfigData: read funcNum fail!");
        return ret;
    }
    ret = drsOps->GetUint32(node, "vendorId", &(configData->vendorId), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioFillConfigData: read vendorId fail!");
        return ret;
    }
    ret = drsOps->GetUint32(node, "deviceId", &(configData->deviceId), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioFillConfigData: read deviceId fail!");
        return ret;
    }

    PLAT_LOGV("SdioFillConfigData: Success! funcNum = %d", configData->funcNum);
    return HDF_SUCCESS;
}
