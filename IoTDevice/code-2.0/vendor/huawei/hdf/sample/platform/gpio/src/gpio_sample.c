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
#include "gpio_pl061_sample.h"
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "gpio_if.h"
#include "osal.h"
#include "osal_io.h"
#include "gpio_core.h"

#define HDF_LOG_TAG gpio_sample

/* HdfDriverEntry hook function prototypes */
static int32_t SampleGpioDriverBind(struct HdfDeviceObject *device);
static int32_t SampleGpioDriverInit(struct HdfDeviceObject *device);
static void SampleGpioDriverRelease(struct HdfDeviceObject *device);

/* HdfDriverEntry definition */
struct HdfDriverEntry g_sampleGpioDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "GPIO_SAMPLE",
    .Bind = SampleGpioDriverBind,
    .Init = SampleGpioDriverInit,
    .Release = SampleGpioDriverRelease,
};

/* Init HdfDriverEntry */
HDF_INIT(g_sampleGpioDriverEntry);

/* GPIO function prototypes */
static int32_t SampleGpioWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val);
static int32_t SampleGpioRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val);
static int32_t SampleGpioSetDirection(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir);
static int32_t SampleGpioGetDirection(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir);

/* GpioMethod definition */
struct GpioMethod g_sampleGpioMethod = {
    .request = NULL,
    .release = NULL,
    .write = SampleGpioWrite,
    .read = SampleGpioRead,
    .setDir = SampleGpioSetDirection,
    .getDir = SampleGpioGetDirection,
    .toIrq = NULL,
    .setIrq = NULL,
    .unsetIrq = NULL,
    .enableIrq = NULL,
    .disableIrq = NULL,
};

/* Private function prototypes */
/* Read GPIO device resource */
static int32_t GetGpioDeviceResource(struct Pl061GpioCntlr *cntlr, const struct DeviceResourceNode *node);
/* Init GPIO controller memory */
static int32_t InitGpioCntlrMem(struct Pl061GpioCntlr *cntlr);
/* Release GPIO controller memory */
static void ReleaseGpioCntlrMem(struct Pl061GpioCntlr *cntlr);

/* HdfDriverEntry hook function implementations */
static int32_t SampleGpioDriverBind(struct HdfDeviceObject *device)
{
    HDF_LOGD("%s: Enter", __func__);
    struct Pl061GpioCntlr *pl061Cntlr = &g_samplePl061GpioCntlr;
    pl061Cntlr->cntlr.device = device;
    device->service = &(pl061Cntlr->cntlr.service);
    pl061Cntlr->cntlr.device->service->Dispatch = SampleGpioDispatch;
    return HDF_SUCCESS;
}

static int32_t SampleGpioDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct Pl061GpioCntlr *pl061Cntlr = &g_samplePl061GpioCntlr;

    HDF_LOGD("%s: Enter", __func__);
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = GetGpioDeviceResource(pl061Cntlr, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get gpio device resource fail:%d", __func__, ret);
        return ret;
    }

    if (pl061Cntlr->groupNum > GROUP_MAX || pl061Cntlr->groupNum <= 0 || pl061Cntlr->bitNum > BIT_MAX ||
        pl061Cntlr->bitNum <= 0) {
        HDF_LOGE("%s: invalid groupNum:%u or bitNum:%u", __func__, pl061Cntlr->groupNum,
                 pl061Cntlr->bitNum);
        return HDF_ERR_INVALID_PARAM;
    }

    pl061Cntlr->regBase = OsalIoRemap(pl061Cntlr->phyBase, pl061Cntlr->groupNum * pl061Cntlr->regStep);
    if (pl061Cntlr->regBase == NULL) {
        HDF_LOGE("%s: err remap phy:0x%x", __func__, pl061Cntlr->phyBase);
        return HDF_ERR_IO;
    }

    ret = InitGpioCntlrMem(pl061Cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: err init cntlr mem:%d", __func__, ret);
        OsalIoUnmap((void *)pl061Cntlr->regBase);
        pl061Cntlr->regBase = NULL;
        return ret;
    }
    pl061Cntlr->cntlr.count = pl061Cntlr->groupNum * pl061Cntlr->bitNum;
    pl061Cntlr->cntlr.priv = (void *)device->property;
    pl061Cntlr->cntlr.ops = &g_sampleGpioMethod;
    ret = GpioCntlrAdd(&pl061Cntlr->cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: err add controller: %d", __func__, ret);
        return ret;
    }
    HDF_LOGI("%s: dev service:%s init success!", __func__, HdfDeviceGetServiceName(device));
    return ret;
}

static void SampleGpioDriverRelease(struct HdfDeviceObject *device)
{
    struct GpioCntlr *gpioCntlr = NULL;
    struct Pl061GpioCntlr *pl061GpioCntlr = NULL;

    HDF_LOGD("%s: Enter", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: device is null!", __func__);
        return;
    }

    gpioCntlr = GpioCntlrFromDevice(device);
    if (gpioCntlr == NULL) {
        HDF_LOGE("%s: no service bound!", __func__);
        return;
    }
    GpioCntlrRemove(gpioCntlr);

    pl061GpioCntlr = (struct Pl061GpioCntlr *)gpioCntlr;
    ReleaseGpioCntlrMem(pl061GpioCntlr);
    OsalIoUnmap((void *)pl061GpioCntlr->regBase);
    pl061GpioCntlr->regBase = NULL;
}

/* Private function implementations */
static int32_t GetGpioDeviceResource(struct Pl061GpioCntlr *cntlr, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *dri = NULL;

    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint8 == NULL || dri->GetUint16 == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid dri ops fail!", __func__);
        return HDF_FAILURE;
    }

    ret = dri->GetUint32(node, "regBase", &cntlr->phyBase, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read regBase fail!", __func__);
        return ret;
    }

    ret = dri->GetUint32(node, "regStep", &cntlr->regStep, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read regStep fail!", __func__);
        return ret;
    }

    ret = dri->GetUint16(node, "groupNum", &cntlr->groupNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read groupNum fail!", __func__);
        return ret;
    }

    ret = dri->GetUint16(node, "bitNum", &cntlr->bitNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read bitNum fail!", __func__);
        return ret;
    }

    ret = dri->GetUint32(node, "irqStart", &cntlr->irqStart, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read irqStart fail!", __func__);
        return ret;
    }

    ret = dri->GetUint8(node, "irqShare", &cntlr->irqShare, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read irqShare fail!", __func__);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t InitGpioCntlrMem(struct Pl061GpioCntlr *cntlr)
{
    size_t groupMemSize;
    struct GpioGroup *groups = NULL;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    groupMemSize = sizeof(struct GpioGroup) * cntlr->groupNum;
    groups = (struct GpioGroup *)OsalMemCalloc(groupMemSize);
    if (groups == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }
    cntlr->groups = groups;

    for (uint16_t i = 0; i < cntlr->groupNum; i++) {
        groups[i].index = i;
        groups[i].regBase = cntlr->regBase + (i * cntlr->regStep);
        if (OsalSpinInit(&groups[i].lock) != HDF_SUCCESS) {
            for (; i > 0; i--) {
                (void)OsalSpinDestroy(&groups[i - 1].lock);
            }
            OsalMemFree(groups);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

static void ReleaseGpioCntlrMem(struct Pl061GpioCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    if (cntlr->groups != NULL) {
        for (uint16_t i = 0; i < cntlr->groupNum; i++) {
            (void)OsalSpinDestroy(&cntlr->groups[i].lock);
        }
        OsalMemFree(cntlr->groups);
        cntlr->groups = NULL;
    }
}

/* GPIO function implementations */
static int32_t SampleGpioWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val)
{
    HDF_LOGD("%s: Enter", __func__);

    int32_t ret;
    uint32_t irqSave;
    unsigned int valCur;
    unsigned int bitNum = Pl061ToBitNum(gpio);
    volatile unsigned char *addr = NULL;
    struct GpioGroup *group = NULL;

    ret = Pl061GetGroupByGpioNum(cntlr, gpio, &group);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    addr = GPIO_DATA(group->regBase, bitNum);
    if (OsalSpinLockIrqSave(&group->lock, &irqSave) != HDF_SUCCESS) {
        return HDF_ERR_DEVICE_BUSY;
    }
    valCur = OSAL_READL(addr);
    if (val == GPIO_VAL_LOW) {
        valCur &= ~(1 << bitNum);
    } else {
        valCur |= (1 << bitNum);
    }
    OSAL_WRITEL(valCur, addr);
    (void)OsalSpinUnlockIrqRestore(&group->lock, &irqSave);
    HDF_LOGD("%s: gpio:%u, val:%u", __func__, gpio, val);
    return HDF_SUCCESS;
}

static int32_t SampleGpioRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val)
{
    HDF_LOGD("%s: Enter", __func__);

    int32_t ret;
    unsigned int valCur;
    volatile unsigned char *addr = NULL;
    unsigned int bitNum = Pl061ToBitNum(gpio);
    struct GpioGroup *group = NULL;

    ret = Pl061GetGroupByGpioNum(cntlr, gpio, &group);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    addr = GPIO_DATA(group->regBase, bitNum);
    valCur = OSAL_READL(addr);
    if (valCur & (1 << bitNum)) {
        *val = GPIO_VAL_HIGH;
    } else {
        *val = GPIO_VAL_LOW;
    }
    HDF_LOGD("%s: gpio:%u, val:%u", __func__, gpio, *val);
    return HDF_SUCCESS;
}

static int32_t SampleGpioSetDirection(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir)
{
    HDF_LOGD("%s: Enter", __func__);

    int32_t ret;
    uint32_t irqSave;
    unsigned int val;
    volatile unsigned char *addr = NULL;
    unsigned int bitNum = Pl061ToBitNum(gpio);
    struct GpioGroup *group = NULL;

    HDF_LOGD("%s: gpio:%u, dir:%d", __func__, gpio, dir);
    ret = Pl061GetGroupByGpioNum(cntlr, gpio, &group);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    addr = GPIO_DIR(group->regBase);
    if (OsalSpinLockIrqSave(&group->lock, &irqSave) != HDF_SUCCESS) {
        return HDF_ERR_DEVICE_BUSY;
    }
    val = OSAL_READL(addr);
    if (dir == GPIO_DIR_IN) {
        val &= ~(1 << bitNum);
    } else if (dir == GPIO_DIR_OUT) {
        val |= 1 << bitNum;
    }
    OSAL_WRITEL(val, addr);
    (void)OsalSpinUnlockIrqRestore(&group->lock, &irqSave);
    return HDF_SUCCESS;
}
static int32_t SampleGpioGetDirection(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir)
{
    HDF_LOGD("%s: Enter", __func__);

    int32_t ret;
    unsigned int val;
    volatile unsigned char *addr = NULL;
    unsigned int bitNum = Pl061ToBitNum(gpio);
    struct GpioGroup *group = NULL;

    HDF_LOGD("%s: gpio:%u, dir:%d", __func__, gpio, dir);
    ret = Pl061GetGroupByGpioNum(cntlr, gpio, &group);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    addr = GPIO_DIR(group->regBase);
    val = OSAL_READL(addr);
    if (val & (1 << bitNum)) {
        *dir = GPIO_DIR_OUT;
    } else {
        *dir = GPIO_DIR_IN;
    }
    return HDF_SUCCESS;
}