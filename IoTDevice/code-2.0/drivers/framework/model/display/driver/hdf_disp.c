/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <securec.h>
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_disp.h"

#define OFFSET_TWO_BYTE    16

static struct PlatformOps *g_platformOps = NULL;

int32_t PlatformRegister(struct PlatformOps *ops)
{
    if (g_platformOps == NULL) {
        g_platformOps = ops;
        HDF_LOGI("%s: disp ops register success", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGD("%s: success", __func__);
    return HDF_FAILURE;
}

static struct PlatformOps *GetPlatformOps(void)
{
    return g_platformOps;
}

static int32_t InitDisp(uint32_t devId);
static int32_t DispOn(uint32_t devId);
static int32_t DispOff(uint32_t devId);
static int32_t SetDispBacklight(uint32_t devId, uint32_t level);
static int32_t GetDispInfo(uint32_t devId, struct DispInfo *info);

struct DispOperations *GetDispOps(void)
{
    static struct DispOperations dispOps = {
        .init = InitDisp,
        .on = DispOn,
        .off = DispOff,
        .setBacklight = SetDispBacklight,
        .getDispInfo = GetDispInfo,
    };
    return &dispOps;
}

#ifdef __KERNEL__
EXPORT_SYMBOL(GetDispOps);
#endif

static int32_t InitDisp(uint32_t devId)
{
    return 0;
}

static int32_t DispOn(uint32_t devId)
{
    int32_t ret = HDF_FAILURE;
    struct PlatformOps *ops = NULL;

    ops = GetPlatformOps();
    if (ops && ops->on) {
        ret = ops->on(devId);
    }
    return ret;
}

static int32_t DispOff(uint32_t devId)
{
    int32_t ret = HDF_FAILURE;
    struct PlatformOps *ops = NULL;

    ops = GetPlatformOps();
    if (ops && ops->off) {
        ret = ops->off(devId);
    }
    return ret;
}

static int32_t SetDispBacklight(uint32_t devId, uint32_t level)
{
    int32_t ret = HDF_FAILURE;
    struct PanelStatus *panelStatus = NULL;

    struct PanelInfo *info = GetPanelInfo(devId);
    if (info == NULL) {
        HDF_LOGE("%s:GetPanelInfo failed", __func__);
        return HDF_FAILURE;
    }
    if (level > info->blk.maxLevel) {
        level = info->blk.maxLevel;
    } else if (level < info->blk.minLevel && level != 0) {
        level = info->blk.minLevel;
    }
    struct PlatformOps *ops = GetPlatformOps();
    if (ops == NULL) {
        HDF_LOGE("%s: ops is null", __func__);
        return HDF_FAILURE;
    }
    if (ops->setBacklight != NULL) {
        ret = ops->setBacklight(devId, level);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: setBacklight failed", __func__);
            return HDF_FAILURE;
        }
    }
    if (ret == HDF_SUCCESS) {
        panelStatus = GetPanelStatus(devId);
        if (!panelStatus) {
            HDF_LOGE("%s: panelStatus is null", __func__);
            return HDF_FAILURE;
        }
        panelStatus->currLevel = level;
    }
    HDF_LOGI("%s:level = %u", __func__, level);
    return HDF_SUCCESS;
}

static int32_t GetDispInfo(uint32_t devId, struct DispInfo *info)
{
    struct PlatformOps *ops = NULL;

    if (info == NULL) {
        HDF_LOGE("%s:info is null", __func__);
        return HDF_FAILURE;
    }
    ops = GetPlatformOps();
    if (ops == NULL) {
        HDF_LOGE("%s: ops is null", __func__);
        return HDF_FAILURE;
    }
    if (ops->getDispInfo != NULL) {
        if (ops->getDispInfo(devId, info)) {
            HDF_LOGE("%s: getDispInfo failed", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

static int32_t SetPowerStatus(struct HdfDeviceObject *device, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    uint32_t para = 0;
    int32_t ret = HDF_FAILURE;
    struct PanelStatus *panelStatus = NULL;

    (void)device;
    (void)rspData;
    if (reqData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(reqData, &para)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed", __func__);
        return HDF_FAILURE;
    }
    uint32_t devId = (para >> OFFSET_TWO_BYTE) & 0xffff;
    uint32_t powerStatus = para & 0xffff;
    switch (powerStatus) {
        case POWER_STATUS_ON:
            ret = DispOn(devId);
            break;
        case POWER_STATUS_OFF:
            ret = DispOff(devId);
            break;
        default:
            HDF_LOGE("%s: not support powerStatus: %d", __func__, powerStatus);
            break;
    }
    if (ret == HDF_SUCCESS) {
        panelStatus = GetPanelStatus(devId);
        if (panelStatus == NULL) {
            HDF_LOGE("%s: panelStatus is null", __func__);
            return HDF_FAILURE;
        }
        panelStatus->powerStatus = powerStatus;
    }
    return ret;
}

static int32_t GetPowerStatus(struct HdfDeviceObject *device, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    uint32_t devId = 0;
    struct PanelStatus *panelStatus = NULL;

    (void)device;
    if (reqData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(reqData, &devId)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed", __func__);
        return HDF_FAILURE;
    }
    panelStatus = GetPanelStatus(devId);
    if (!panelStatus) {
        HDF_LOGE("%s: panelStatus is null", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(rspData, panelStatus->powerStatus)) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t SetBacklight(struct HdfDeviceObject *device, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;

    (void)device;
    (void)rspData;
    if (reqData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t para = 0;
    if (!HdfSbufReadUint32(reqData, &para)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed", __func__);
        return HDF_FAILURE;
    }
    uint32_t devId = (para >> OFFSET_TWO_BYTE) & 0xffff;
    uint32_t level = para & 0xffff;
    ret = SetDispBacklight(devId, level);
    return ret;
}

static int32_t GetBacklight(struct HdfDeviceObject *device, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    uint32_t devId = 0;
    struct PanelStatus *panelStatus = NULL;

    (void)device;
    if (reqData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(reqData, &devId)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed", __func__);
        return HDF_FAILURE;
    }
    panelStatus = GetPanelStatus(devId);
    if (!panelStatus) {
        HDF_LOGE("%s: panelStatus is null", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(rspData, panelStatus->currLevel)) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t GetInfo(struct HdfDeviceObject *device, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    (void)device;
    (void)rspData;
    uint32_t devId = 0;
    struct DispInfo info;

    if (reqData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(reqData, &devId)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed", __func__);
        return HDF_FAILURE;
    }
    (void)memset_s(&info, sizeof(struct DispInfo), 0, sizeof(struct DispInfo));
    if (GetDispInfo(devId, &info) != HDF_SUCCESS) {
        HDF_LOGE("%s: GetDispInfo failed", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteBuffer(rspData, &info, sizeof(struct DispInfo)) != 0) {
        HDF_LOGE("%s: copy info failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

DispCmdHandle g_dispCmdHandle[] = {
    GetPowerStatus,
    GetInfo,
    SetPowerStatus,
    SetBacklight,
    GetBacklight,
};

static int32_t DispCmdProcess(struct HdfDeviceObject *device, int32_t cmd, struct HdfSBuf *reqData,
    struct HdfSBuf *rspData)
{
    int32_t cmdNum = sizeof(g_dispCmdHandle) / sizeof(g_dispCmdHandle[0]);
    if (device == NULL || reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (cmd >= cmdNum || cmd < 0) {
        HDF_LOGE("%s: invalid cmd = %d", __func__, cmd);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: cmd = %d", __func__, cmd);
    if (g_dispCmdHandle[cmd] == NULL) {
        return HDF_FAILURE;
    }

    return g_dispCmdHandle[cmd](device, reqData, rspData);
}

static int32_t HdfDispDispatch(struct HdfDeviceIoClient *client, int id, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    if (client == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    return DispCmdProcess(client->device, id, data, reply);
}

static int HdfDispBind(struct HdfDeviceObject *dev)
{
    if (dev == NULL) {
        return HDF_FAILURE;
    }
    static struct IDeviceIoService dispService = {
        .object.objectId = 1,
        .Dispatch = HdfDispDispatch,
    };
    dev->service = &dispService;
    return HDF_SUCCESS;
}

static int32_t HdfDispEntryInit(struct HdfDeviceObject *object)
{
    int32_t ret;
    int32_t i;
    struct PlatformOps *ops = GetPlatformOps();

    if (object == NULL) {
        HDF_LOGE("%s: object is null!", __func__);
        return HDF_FAILURE;
    }
    if (ops == NULL) {
        HDF_LOGE("%s: ops is null", __func__);
        return HDF_FAILURE;
    }
    if (ops->init != NULL) {
        for (i = 0; i < g_numRegisteredPanel; i++) {
            ret = ops->init(i);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s: init failed", __func__);
                return HDF_FAILURE;
            }
        }
    }
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_dispDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_DISP",
    .Init = HdfDispEntryInit,
    .Bind = HdfDispBind,
};

HDF_INIT(g_dispDevEntry);
