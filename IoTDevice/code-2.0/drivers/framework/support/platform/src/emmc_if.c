/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "emmc_if.h"
#include <securec.h>
#ifndef __USER__
#include "devsvc_manager_clnt.h"
#include "emmc_core.h"
#endif
#include "hdf_base.h"
#ifdef __USER__
#include "hdf_io_service_if.h"
#endif
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG emmc_if_c

#define EMMC_NAME_LEN 32

static void *EmmcCntlrObjGetByNumber(int16_t busNum)
{
    void *object = NULL;
    char *serviceName = NULL;

    if (busNum < 0) {
        HDF_LOGE("EmmcCntlrObjGetByNumber: invalid bus:%d", busNum);
        return NULL;
    }
    serviceName = OsalMemCalloc(EMMC_NAME_LEN + 1);
    if (serviceName == NULL) {
        HDF_LOGE("EmmcCntlrObjGetByNumber: OsalMemCalloc fail!");
        return NULL;
    }
    if (snprintf_s(serviceName, EMMC_NAME_LEN + 1, EMMC_NAME_LEN,
        "HDF_PLATFORM_EMMC_%d", busNum) < 0) {
        HDF_LOGE("EmmcCntlrObjGetByNumber: format service name fail!");
        goto __ERR;
    }

#ifdef __USER__
    object = (void *)HdfIoServiceBind(serviceName);
    HDF_LOGD("EmmcCntlrObjGetByNumber: user status");
#else
    object = (void *)DevSvcManagerClntGetService(serviceName);
    HDF_LOGD("EmmcCntlrObjGetByNumber: kernel status");
#endif
    if (object == NULL) {
        HDF_LOGE("EmmcCntlrObjGetByNumber: get service fail!");
        goto __ERR;
    }

#ifndef __USER__
    struct EmmcCntlr *cntlr = (struct EmmcCntlr *)object;
    if (cntlr->priv == NULL && EmmcCntlrFindHost(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("EmmcCntlrObjGetByNumber: EmmcCntlrFindHost fail!");
        goto __ERR;
    }
#endif

    HDF_LOGD("EmmcCntlrObjGetByNumber: success");
    OsalMemFree(serviceName);
    return object;
__ERR:
    OsalMemFree(serviceName);
    return NULL;
}

DevHandle EmmcOpen(int16_t busNum)
{
    return (DevHandle)EmmcCntlrObjGetByNumber(busNum);
}

void EmmcClose(DevHandle handle)
{
    if (handle != NULL) {
#ifdef __USER__
    HdfIoServiceRecycle((struct HdfIoService *)handle);
#endif
    }
}

#ifdef __USER__
enum EmmcIoCmd {
    EMMC_IO_GET_CID = 0,
};

static int32_t EmmcGetCidReadReplyData(struct HdfSBuf *reply, uint8_t *cid, uint32_t size)
{
    uint32_t rLen;
    const void *rBuf = NULL;

    if (!HdfSbufReadBuffer(reply, &rBuf, &rLen)) {
        HDF_LOGE("EmmcGetCidReadReplyData: read rBuf fail!");
        return HDF_ERR_IO;
    }

    if (memcpy_s(cid, size, rBuf, rLen) != EOK) {
        HDF_LOGE("EmmcGetCidReadReplyData: memcpy rBuf fail!");
        return HDF_ERR_IO;
    }
    HDF_LOGD("EmmcGetCidReadReplyData: success");
    return HDF_SUCCESS;
}

static int32_t EmmcServiceGetCid(struct HdfIoService *service, uint8_t *cid, uint32_t size)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;

    reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("EmmcServiceGetCid: failed to obtain reply!");
        ret = HDF_ERR_MALLOC_FAIL;
        goto __EXIT;
    }

    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("EmmcServiceGetCid: dispatcher or Dispatch is NULL!");
        ret = HDF_ERR_NOT_SUPPORT;
        goto __EXIT;
    }

    ret = service->dispatcher->Dispatch(&service->object, EMMC_IO_GET_CID, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EmmcServiceGetCid: failed to send service call:%d", ret);
        goto __EXIT;
    }

    ret = EmmcGetCidReadReplyData(reply, cid, size);
    if (ret != HDF_SUCCESS) {
        goto __EXIT;
    }
    HDF_LOGD("EmmcServiceGetCid: success");

__EXIT:
    if (reply != NULL) {
        HdfSBufRecycle(reply);
    }
    return ret;
}
#endif

int32_t EmmcGetCid(DevHandle handle, uint8_t *cid, uint32_t size)
{
    if (handle == NULL) {
        HDF_LOGE("EmmcGetCid: handle is NULL!");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cid == NULL || size < EMMC_CID_LEN) {
        HDF_LOGE("EmmcGetCid: error params!");
        return HDF_ERR_INVALID_PARAM;
    }
#ifdef __USER__
    return EmmcServiceGetCid((struct HdfIoService *)handle, cid, size);
#else
    return EmmcCntlrGetCid((struct EmmcCntlr *)handle, cid, size);
#endif
}

void EmmcGetHuid(uint8_t *cid, uint32_t size)
{
    DevHandle handle = NULL;

    if (cid == NULL || size == 0) {
        HDF_LOGE("EmmcGetUdid: error params!");
        return;
    }
    if (memset_s(cid, sizeof(uint8_t) * size, 0, sizeof(uint8_t) * size) != EOK) {
        HDF_LOGE("EmmcGetUdid: memset_s fail!");
        return;
    }

    handle = EmmcOpen(0);
    if (handle == NULL) {
        HDF_LOGW("EmmcGetUdid: open fail, use default value!");
        return;
    }
    if (EmmcGetCid(handle, cid, size) != HDF_SUCCESS) {
        HDF_LOGW("EmmcGetUdid: get fail, use default value!");
    }
    EmmcClose(handle);
}
