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

#include "spi_sample.h"
#include "spi_pl022_sample.h"
#include "spi_dispatch_sample.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "osal_mem.h"
#include "hdf_log.h"
#include "spi_core.h"
#include "los_vm_zone.h"

#define HDF_LOG_TAG spi_sample

/* HdfDriverEntry hook function prototypes */
static int32_t SampleSpiDriverBind(struct HdfDeviceObject *device);
static int32_t SampleSpiDriverInit(struct HdfDeviceObject *device);
static void SampleSpiDriverRelease(struct HdfDeviceObject *device);

/* SpiCntlrMethod definition */
struct SpiCntlrMethod g_sampleSpiMethod = {
    .Transfer = SampleSpiCntlrTransfer,
    .SetCfg = SampleSpiCntlrSetCfg,
    .GetCfg = SampleSpiCntlrGetCfg,
};

/* HdfDriverEntry definition */
struct HdfDriverEntry g_sampleSpiDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "SPI_SAMPLE",
    .Bind = SampleSpiDriverBind,
    .Init = SampleSpiDriverInit,
    .Release = SampleSpiDriverRelease,
};

/* Init HdfDriverEntry */
HDF_INIT(g_sampleSpiDriverEntry);

/* Private function prototypes */
static int InitSpiDevice(struct SpiCntlr *cntlr, const struct DeviceResourceNode *property);
static int ConfigSpiDevice(struct Pl022SpiCntlr *pl022Cntlr);
static int32_t InitSpiDeviceResource(struct Pl022SpiCntlr *pl022Cntlr, const struct DeviceResourceNode *node);
static int32_t CreateSpiDev(struct Pl022SpiCntlr *pl022Cntlr);
static void ReleaseSpiDev(struct Pl022SpiCntlr *pl022Cntlr);
static struct SpiDev *FindDeviceByCsNum(const struct Pl022SpiCntlr *pl022Cntlr, uint32_t cs);

/* HdfDriverEntry hook function implementations */
static int32_t SampleSpiDriverBind(struct HdfDeviceObject *device)
{
    HDF_LOGD("%s: Enter", __func__);
    struct SpiCntlr *cntlr = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = SpiCntlrCreate(device);
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL", __func__);
        return HDF_FAILURE;
    }

    cntlr->service.Dispatch = SampleSpiDispatch;
    return HDF_SUCCESS;
}

static int32_t SampleSpiDriverInit(struct HdfDeviceObject *device)
{
    HDF_LOGD("%s: Enter", __func__);
    int ret;
    struct SpiCntlr *cntlr = NULL;
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or device->property is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    cntlr = SpiCntlrFromDevice(device);
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = InitSpiDevice(cntlr, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: InitSpiDevice failed", __func__);
        return ret;
    }
    ret = ConfigSpiDevice(cntlr->priv);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: ConfigSpiDevice failed", __func__);
        return ret;
    }
    return ret;
}

static void SampleSpiDriverRelease(struct HdfDeviceObject *device)
{
    HDF_LOGD("%s: Enter", __func__);
    struct SpiCntlr *cntlr = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }
    cntlr = SpiCntlrFromDevice(device);
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return;
    }
    if (cntlr->priv != NULL) {
        ReleaseSpiDev((struct Pl022SpiCntlr *)cntlr->priv);
    }
    SpiCntlrDestroy(cntlr);
}

/* SPI function implementations */
int32_t SampleSpiCntlrTransfer(struct SpiCntlr *cntlr, struct SpiMsg *msg, uint32_t count)
{
    HDF_LOGD("%s: Enter", __func__);
    int ret;
    struct Pl022SpiCntlr *pl022Cntlr = NULL;
    struct SpiDev *spiDev = NULL;

    if (cntlr == NULL || cntlr->priv == NULL || msg == NULL || count == 0) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    pl022Cntlr = (struct Pl022SpiCntlr *)cntlr->priv;
    spiDev = FindDeviceByCsNum(pl022Cntlr, cntlr->curCs);
    if (spiDev == NULL) {
        HDF_LOGE("%s: spiDev is null, curCs %u", __func__, cntlr->curCs);
        return HDF_FAILURE;
    }
    pl022Cntlr->mode = spiDev->mode;
    pl022Cntlr->transferMode = spiDev->transferMode;
    pl022Cntlr->bitsPerWord = spiDev->bitsPerWord;
    pl022Cntlr->maxSpeedHz = spiDev->maxSpeedHz;
    pl022Cntlr->curCs = spiDev->csNum;
    for (uint32_t i = 0; i < count; i++) {
        ret = TransferOneMessage(pl022Cntlr, &(msg[i]));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: transfer error", __func__);
            return ret;
        }
    }
    return ret;
}

int32_t SampleSpiCntlrSetCfg(struct SpiCntlr *cntlr, struct SpiCfg *cfg)
{
    HDF_LOGD("%s: Enter", __func__);
    struct Pl022SpiCntlr *pl022Cntlr = NULL;
    struct SpiDev *spiDev = NULL;

    if (cntlr == NULL || cntlr->priv == NULL || cfg == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    pl022Cntlr = (struct Pl022SpiCntlr *)cntlr->priv;
    spiDev = FindDeviceByCsNum(pl022Cntlr, cntlr->curCs);
    if (spiDev == NULL) {
        HDF_LOGE("%s: spiDev is null, curCs %u", __func__, cntlr->curCs);
        return HDF_FAILURE;
    }
    spiDev->mode = cfg->mode;
    spiDev->transferMode = cfg->transferMode;
    spiDev->bitsPerWord = cfg->bitsPerWord;
    if ((cfg->bitsPerWord < BITS_PER_WORD_MIN) || (cfg->bitsPerWord > BITS_PER_WORD_MAX)) {
        HDF_LOGE("%s: bitsPerWord %u not support, use default bitsPerWord %u",
                 __func__, cfg->bitsPerWord, BITS_PER_WORD_DEFAULT);
        spiDev->bitsPerWord = BITS_PER_WORD_DEFAULT;
    }
    if (cfg->maxSpeedHz != 0) {
        spiDev->maxSpeedHz = cfg->maxSpeedHz;
    }
    return HDF_SUCCESS;
}

int32_t SampleSpiCntlrGetCfg(struct SpiCntlr *cntlr, struct SpiCfg *cfg)
{
    HDF_LOGD("%s: Enter", __func__);
    struct Pl022SpiCntlr *pl022Cntlr = NULL;
    struct SpiDev *spiDev = NULL;

    if (cntlr == NULL || cntlr->priv == NULL || cfg == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    pl022Cntlr = (struct Pl022SpiCntlr *)cntlr->priv;
    spiDev = FindDeviceByCsNum(pl022Cntlr, cntlr->curCs);
    if (spiDev == NULL) {
        HDF_LOGE("%s: spiDev is null, curCs %u", __func__, cntlr->curCs);
        return HDF_FAILURE;
    }
    cfg->mode = spiDev->mode;
    cfg->transferMode = spiDev->transferMode;
    cfg->bitsPerWord = spiDev->bitsPerWord;
    cfg->maxSpeedHz = spiDev->maxSpeedHz;
    return HDF_SUCCESS;
}

/* Private function implementations */
static int InitSpiDevice(struct SpiCntlr *cntlr, const struct DeviceResourceNode *property)
{
    int ret;
    struct Pl022SpiCntlr *pl022Cntlr = NULL;

    pl022Cntlr = (struct Pl022SpiCntlr *)OsalMemCalloc(sizeof(*pl022Cntlr));
    if (pl022Cntlr == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        return HDF_FAILURE;
    }
    ret = InitSpiDeviceResource(pl022Cntlr, property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: InitSpiDeviceResource error", __func__);
        OsalMemFree(pl022Cntlr);
        return HDF_FAILURE;
    }
    pl022Cntlr->maxSpeedHz = (pl022Cntlr->clkRate) / ((SCR_MIN + 1) * CPSDVSR_MIN);
    pl022Cntlr->minSpeedHz = (pl022Cntlr->clkRate) / ((SCR_MAX + 1) * CPSDVSR_MAX);
    DListHeadInit(&pl022Cntlr->deviceList);
    pl022Cntlr->cntlr = cntlr;
    cntlr->priv = pl022Cntlr;
    cntlr->busNum = pl022Cntlr->busNum;
    cntlr->method = &g_sampleSpiMethod;
    ret = CreateSpiDev(pl022Cntlr);
    if (ret != HDF_SUCCESS) {
        ReleaseSpiDev(pl022Cntlr);
        return ret;
    }
    return HDF_SUCCESS;
}

static int ConfigSpiDevice(struct Pl022SpiCntlr *pl022Cntlr)
{
    int ret;

    ret = ConfigPl022SpiCntlr(pl022Cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HiPl022Config error", __func__);
    }
    return ret;
}

static int32_t InitSpiDeviceResource(struct Pl022SpiCntlr *pl022Cntlr, const struct DeviceResourceNode *node)
{
    uint32_t tmp;
    struct DeviceResourceIface *resIf = NULL;

    resIf = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (resIf == NULL || resIf->GetUint8 == NULL || resIf->GetUint16 == NULL || resIf->GetUint32 == NULL) {
        HDF_LOGE("%s: resource is invalid", __func__);
        return HDF_FAILURE;
    }
    if (resIf->GetUint32(node, "regBase", &tmp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read regBase fail", __func__);
        return HDF_FAILURE;
    }
    pl022Cntlr->regBase = (void *)(uintptr_t)(IO_DEVICE_ADDR(tmp));
    if (resIf->GetUint32(node, "busNum", &pl022Cntlr->busNum, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read busNum fail", __func__);
        return HDF_FAILURE;
    }
    if (resIf->GetUint32(node, "numCs", &pl022Cntlr->numCs, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read numCs fail", __func__);
        return HDF_FAILURE;
    }
    if (resIf->GetUint32(node, "speed", &pl022Cntlr->speed, DEFAULT_SPEED) != HDF_SUCCESS) {
        HDF_LOGE("%s: read speed fail", __func__);
        return HDF_FAILURE;
    }
    if (resIf->GetUint32(node, "fifoSize", &pl022Cntlr->fifoSize, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fifoSize fail", __func__);
        return HDF_FAILURE;
    }
    if (resIf->GetUint32(node, "clkRate", &pl022Cntlr->clkRate, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read clkRate fail", __func__);
        return HDF_FAILURE;
    }
    if (resIf->GetUint16(node, "mode", &pl022Cntlr->mode, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read mode fail", __func__);
        return HDF_FAILURE;
    }
    if (resIf->GetUint8(node, "bitsPerWord", &pl022Cntlr->bitsPerWord, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read bitsPerWord fail", __func__);
        return HDF_FAILURE;
    }
    if (resIf->GetUint8(node, "transferMode", &pl022Cntlr->transferMode, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read comMode fail", __func__);
        return HDF_FAILURE;
    }
    pl022Cntlr->regCrg = REG_SPI_CRG;
    pl022Cntlr->clkEnBit = SPI_CRG_CLK_EN;
    pl022Cntlr->clkRstBit = SPI_CRG_CLK_RST;
    pl022Cntlr->regMiscCtrl = REG_SPI_MISC_CTRL;
    pl022Cntlr->miscCtrlCs = SPI_MISC_CTRL_CS;
    pl022Cntlr->miscCtrlCsShift = SPI_MISC_CTRL_CS_SHIFT;
    return HDF_SUCCESS;
}

static int32_t CreateSpiDev(struct Pl022SpiCntlr *pl022Cntlr)
{
    uint32_t i;
    struct SpiDev *device = NULL;

    for (i = 0; i < pl022Cntlr->numCs; i++) {
        device = (struct SpiDev *)OsalMemCalloc(sizeof(*device));
        if (device == NULL) {
            HDF_LOGE("%s: OsalMemCalloc error", __func__);
            return HDF_FAILURE;
        }
        device->cntlr = pl022Cntlr->cntlr;
        device->csNum = i;
        device->bitsPerWord = pl022Cntlr->bitsPerWord;
        device->transferMode = pl022Cntlr->transferMode;
        device->maxSpeedHz = pl022Cntlr->maxSpeedHz;
        device->mode = pl022Cntlr->mode;
        DListHeadInit(&device->list);
        DListInsertTail(&device->list, &pl022Cntlr->deviceList);
    }
    return HDF_SUCCESS;
}

static void ReleaseSpiDev(struct Pl022SpiCntlr *pl022Cntlr)
{
    struct SpiDev *dev = NULL;
    struct SpiDev *tmpDev = NULL;

    DLIST_FOR_EACH_ENTRY_SAFE(dev, tmpDev, &(pl022Cntlr->deviceList), struct SpiDev, list) {
        if (dev != NULL) {
            DListRemove(&(dev->list));
            OsalMemFree(dev);
        }
    }
    OsalMemFree(pl022Cntlr);
}

static struct SpiDev *FindDeviceByCsNum(const struct Pl022SpiCntlr *pl022Cntlr, uint32_t cs)
{
    struct SpiDev *dev = NULL;
    struct SpiDev *tmpDev = NULL;

    if (pl022Cntlr == NULL || pl022Cntlr->numCs <= cs) {
        return NULL;
    }
    DLIST_FOR_EACH_ENTRY_SAFE(dev, tmpDev, &(pl022Cntlr->deviceList), struct SpiDev, list) {
        if (dev->csNum == cs) {
            break;
        }
    }
    return dev;
}