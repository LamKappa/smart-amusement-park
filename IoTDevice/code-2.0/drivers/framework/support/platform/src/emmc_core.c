/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "emmc_core.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG emmc_core_c

int32_t EmmcCntlrFindHost(struct EmmcCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->method == NULL || cntlr->method->findHost == NULL) {
        HDF_LOGE("EmmcCntlrFindHost: cntlr or method or findHost is null!");
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method->findHost(cntlr, &(cntlr->configData)) != HDF_SUCCESS) {
        HDF_LOGE("EmmcCntlrFindHost: findHost failed");
        return HDF_ERR_INVALID_OBJECT;
    }
    return HDF_SUCCESS;
}

int32_t EmmcCntlrGetCid(struct EmmcCntlr *cntlr, uint8_t *cid, uint32_t size)
{
    if (cntlr == NULL) {
        HDF_LOGE("EmmcCntlrGetCid: cntlr is null");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->method == NULL || cntlr->method->getCid == NULL) {
        HDF_LOGE("EmmcCntlrGetCid: ops or getCid is null");
        return HDF_ERR_NOT_SUPPORT;
    }
    return cntlr->method->getCid(cntlr, cid, size);
}

static int32_t EmmcGetCidWriteBackReply(struct HdfSBuf *reply, uint8_t *cid, uint32_t size)
{
    if (!HdfSbufWriteBuffer(reply, cid, size)) {
        HDF_LOGE("EmmcGetCidWriteBackReply: write to reply failed");
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t EmmcGetCidDispatch(struct EmmcCntlr *cntlr, struct HdfSBuf *reply)
{
    int32_t ret;
    uint8_t cid[EMMC_CID_LEN] = {0};

    ret = EmmcCntlrGetCid(cntlr, cid, EMMC_CID_LEN);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EmmcGetCidDispatch: EmmcCntlrGetCid failed");
        return ret;
    }
    return EmmcGetCidWriteBackReply(reply, cid, EMMC_CID_LEN);
}

static int32_t EmmcIoDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    struct EmmcCntlr *cntlr = NULL;
    (void)data;

    if (client == NULL || client->device == NULL) {
        HDF_LOGE("EmmcIoDispatch: client or client->device is null");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (reply == NULL) {
        HDF_LOGE("EmmcIoDispatch: reply is null");
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr = (struct EmmcCntlr *)client->device->service;
    if (cntlr == NULL) {
        HDF_LOGE("EmmcIoDispatch: service is null");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->priv == NULL) {
        if (EmmcCntlrFindHost(cntlr) != HDF_SUCCESS) {
            HDF_LOGE("EmmcIoDispatch: find host failed");
            return HDF_ERR_INVALID_OBJECT;
        }
    }

    switch (cmd) {
        case EMMC_IO_GET_CID:
            ret = EmmcGetCidDispatch(cntlr, reply);
            break;
        default:
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }

    return ret;
}

struct EmmcCntlr *EmmcCntlrCreateAndBind(struct HdfDeviceObject *device)
{
    struct EmmcCntlr *cntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("EmmcCntlrCreateAndBind: device is null!");
        return NULL;
    }

    cntlr = (struct EmmcCntlr *)OsalMemCalloc(sizeof(*cntlr));
    if (cntlr == NULL) {
        HDF_LOGE("EmmcCntlrCreateAndBind: malloc host failed");
        return NULL;
    }
    cntlr->device = device;
    device->service = &cntlr->service;
    cntlr->device->service->Dispatch = EmmcIoDispatch;
    return cntlr;
}

void EmmcCntlrDestroy(struct EmmcCntlr *cntlr)
{
    if (cntlr != NULL) {
        cntlr->device = NULL;
        cntlr->priv = NULL;
        cntlr->method = NULL;
        OsalMemFree(cntlr);
    }
}

int32_t EmmcFillConfigData(struct HdfDeviceObject *device, struct EmmcConfigData *configData)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (device == NULL || configData == NULL) {
        HDF_LOGE("EmmcFillConfigData: input para is null.");
        return HDF_FAILURE;
    }

    node = device->property;
    if (node == NULL) {
        HDF_LOGE("EmmcFillConfigData: drs node is null.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("EmmcFillConfigData: invalid drs ops failed");
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "hostId", &(configData->hostId), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EmmcFillConfigData: read funcNum failed");
        return ret;
    }

    HDF_LOGD("EmmcFillConfigData: Success! hostId = %u.", configData->hostId);
    return HDF_SUCCESS;
}
