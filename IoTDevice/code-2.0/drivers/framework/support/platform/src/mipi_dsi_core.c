/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mipi_dsi_core.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"

#define HDF_LOG_TAG mipi_dsi_core

static struct MipiDsiHandle g_mipiDsihandle[MAX_CNTLR_CNT];

int32_t MipiDsiRegisterCntlr(struct MipiDsiCntlr *cntlr)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->devNo >= MAX_CNTLR_CNT) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (g_mipiDsihandle[cntlr->devNo].cntlr == NULL) {
        g_mipiDsihandle[cntlr->devNo].cntlr = cntlr;
        if (OsalMutexInit(&g_mipiDsihandle[cntlr->devNo].lock) != HDF_SUCCESS) {
            HDF_LOGE("%s: init lock fail!", __func__);
            g_mipiDsihandle[cntlr->devNo].cntlr = NULL;
            return HDF_FAILURE;
        }
        return HDF_SUCCESS;
    }
    HDF_LOGE("cntlr is not NULL");
    return HDF_FAILURE;
}

DevHandle MipiDsiOpen(uint8_t number)
{
    if (number >= MAX_CNTLR_CNT) {
        HDF_LOGE("invalid number");
        return NULL;
    }
    if (g_mipiDsihandle[number].cntlr == NULL) {
        HDF_LOGE("no mipi_dsi %d cntlr", number);
        return NULL;
    }
    if (OsalMutexLock(&(g_mipiDsihandle[number].lock)) != HDF_SUCCESS) {
        HDF_LOGE("mutex lock fail");
        return NULL;
    }
    return (DevHandle)(&(g_mipiDsihandle[number]));
}

void MipiDsiClose(DevHandle handle)
{
    struct MipiDsiHandle *mipiHandle = (struct MipiDsiHandle *)handle;

    if (mipiHandle != NULL) {
        (void)OsalMutexUnlock(&(mipiHandle->lock));
    }
}

static int32_t MipiDsiSetDevCfg(struct MipiDsiCntlr *cntlr)
{
    /* set controller config */
    if (cntlr->setCntlrCfg == NULL) {
        HDF_LOGE("setCntlrCfg is NULL");
        return HDF_FAILURE;
    }
    return cntlr->setCntlrCfg(cntlr);
}

static struct MipiDsiCntlr *GetCntlr(DevHandle handle)
{
    struct MipiDsiHandle *mipiHandle = (struct MipiDsiHandle *)handle;

    return (mipiHandle == NULL) ? NULL : mipiHandle->cntlr;
}

int32_t MipiDsiSetCfg(DevHandle handle, struct MipiCfg *cfg)
{
    struct MipiDsiCntlr *cntlr = NULL;

    cntlr = GetCntlr(handle);
    if (cntlr == NULL || cfg == NULL) {
        return HDF_FAILURE;
    }
    cntlr->phyDataRate = cfg->phyDataRate;
    cntlr->pixelClk = cfg->pixelClk;
    cntlr->timing = cfg->timing;
    cntlr->mode = cfg->mode;
    cntlr->burstMode = cfg->burstMode;
    cntlr->format = cfg->format;
    cntlr->lane = cfg->lane;
    return MipiDsiSetDevCfg(cntlr);
}

int32_t MipiDsiGetCfg(DevHandle handle, struct MipiCfg *cfg)
{
    struct MipiDsiCntlr *cntlr = NULL;

    cntlr = GetCntlr(handle);
    if (cntlr == NULL || cfg == NULL) {
        return HDF_FAILURE;
    }
    cfg->phyDataRate = cntlr->phyDataRate;
    cfg->pixelClk = cntlr->pixelClk;
    cfg->timing.xPixels = cntlr->timing.xPixels;
    cfg->timing.hsaPixels = cntlr->timing.hsaPixels;
    cfg->timing.hbpPixels = cntlr->timing.hbpPixels;
    cfg->timing.hlinePixels = cntlr->timing.hlinePixels;
    cfg->timing.vsaLines = cntlr->timing.vsaLines;
    cfg->timing.vbpLines = cntlr->timing.vbpLines;
    cfg->timing.vfpLines = cntlr->timing.vfpLines;
    cfg->timing.ylines = cntlr->timing.ylines;
    cfg->timing.edpiCmdSize = cntlr->timing.edpiCmdSize;
    cfg->mode = cntlr->mode;
    cfg->burstMode = cntlr->burstMode;
    cfg->format = cntlr->format;
    cfg->lane = cntlr->lane;
    return HDF_SUCCESS;
}

void MipiDsiSetLpMode(DevHandle handle)
{
    struct MipiDsiCntlr *cntlr = NULL;

    cntlr = GetCntlr(handle);
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->toLp != NULL) {
        cntlr->toLp(cntlr);
    } else {
        HDF_LOGI("toLp not support!");
    }
}

void MipiDsiSetHsMode(DevHandle handle)
{
    struct MipiDsiCntlr *cntlr = NULL;

    cntlr = GetCntlr(handle);
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->toHs != NULL) {
        cntlr->toHs(cntlr);
    } else {
        HDF_LOGI("toHs not support!");
    }
}

void MipiDsiEnterUlps(DevHandle handle)
{
    struct MipiDsiCntlr *cntlr = NULL;

    cntlr = GetCntlr(handle);
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->enterUlps != NULL) {
        cntlr->enterUlps(cntlr);
    } else {
        HDF_LOGI("enterUlps not support!");
    }
}

void MipiDsiExitUlps(DevHandle handle)
{
    struct MipiDsiCntlr *cntlr = NULL;

    cntlr = GetCntlr(handle);
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->exitUlps != NULL) {
        cntlr->exitUlps(cntlr);
    } else {
        HDF_LOGI("exitUlps not support!");
    }
}

int32_t MipiDsiTx(DevHandle handle, struct DsiCmdDesc *cmd)
{
    struct MipiDsiCntlr *cntlr = NULL;
    int32_t ret;

    cntlr = GetCntlr(handle);
    if (cntlr == NULL || cmd == NULL) {
        return HDF_FAILURE;
    }
    if (cntlr->setCmd != NULL) {
        ret = cntlr->setCmd(cntlr, cmd);
        if (cmd->delay > 0) {
            OsalMSleep(cmd->delay);
        }
        return ret;
    }
    return HDF_ERR_NOT_SUPPORT;
}

int32_t MipiDsiRx(DevHandle handle, struct DsiCmdDesc *cmd, int32_t readLen, uint8_t *out)
{
    struct MipiDsiCntlr *cntlr = NULL;

    cntlr = GetCntlr(handle);
    if (cntlr == NULL || cmd == NULL) {
        return HDF_FAILURE;
    }
    if (cntlr->getCmd != NULL) {
        return cntlr->getCmd(cntlr, cmd, readLen, out);
    }
    return HDF_ERR_NOT_SUPPORT;
}

int32_t MipiDsiPowerControl(DevHandle handle, uint8_t enable)
{
    struct MipiDsiCntlr *cntlr = NULL;

    cntlr = GetCntlr(handle);
    if (cntlr == NULL) {
        return HDF_FAILURE;
    }
    if (cntlr->powerControl != NULL) {
        return cntlr->powerControl(cntlr, enable);
    }
    return HDF_ERR_NOT_SUPPORT;
}
