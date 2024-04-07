/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "spi_if.h"
#include "spi_test.h"

#define HDF_LOG_TAG spi_test_c
struct SpiTestFunc {
    enum SpiTestCmd type;
    int32_t (*Func)(struct SpiTest *test);
};

static DevHandle SpiTestGetHandle(struct SpiTest *test)
{
    struct SpiDevInfo info;

    info.busNum = test->bus;
    info.csNum = test->cs;
    return SpiOpen(&info);
}

static void SpiTestReleaseHandle(DevHandle handle)
{
    if (handle == NULL) {
        HDF_LOGE("%s: spi handle is null", __func__);
        return;
    }
    SpiClose(handle);
}

#define BITS_PER_WORD 10
#define MAX_SPEED_HZ 10000000
static int32_t SpiSetCfgTest(struct SpiTest *test)
{
    int32_t ret;
    struct SpiCfg cfg;

    cfg.mode = SPI_CLK_PHASE | SPI_MODE_LOOP;
    cfg.bitsPerWord = BITS_PER_WORD;
    cfg.maxSpeedHz = MAX_SPEED_HZ;
    cfg.transferMode = SPI_INTERRUPT_TRANSFER;
    ret = SpiSetCfg(test->handle, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return ret;
}

static int32_t SpiTransferTest(struct SpiTest *test)
{
    int32_t i;
    struct SpiMsg msg;

    msg.rbuf = test->rbuf;
    msg.wbuf = test->wbuf;
    msg.len = test->len;
    msg.csChange = 1; // switch off the CS after transfer
    msg.delayUs = 0;
    msg.speed = 0;    // use default speed
    if (SpiTransfer(test->handle, &msg, 1) != HDF_SUCCESS) {
        HDF_LOGE("%s: spi transfer err", __func__);
        return HDF_FAILURE;
    }
    for (i = 0; i < test->len; i++) {
        HDF_LOGE("%s: wbuf[%d] = 0x%x rbuff[%d] = 0x%x", __func__, i, test->wbuf[i], i, test->rbuf[i]);
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t SpiWriteTest(struct SpiTest *test)
{
    if (SpiWrite(test->handle, test->wbuf, test->len) != HDF_SUCCESS) {
        HDF_LOGE("%s: spi write err\n", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t SpiReadTest(struct SpiTest *test)
{
    if (SpiRead(test->handle, test->rbuf, test->len) != HDF_SUCCESS) {
        HDF_LOGE("%s: spi read err\n", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t SpiReliabilityTest(struct SpiTest *test)
{
    struct SpiCfg cfg = {0};
    struct SpiMsg msg = {0};

    (void)SpiSetCfg(test->handle, &cfg);
    (void)SpiSetCfg(test->handle, NULL);
    (void)SpiTransfer(test->handle, &msg, 1);
    (void)SpiTransfer(test->handle, NULL, -1);
    (void)SpiWrite(test->handle, test->wbuf, test->len);
    (void)SpiWrite(test->handle, NULL, -1);
    (void)SpiRead(test->handle, test->rbuf, test->len);
    (void)SpiRead(test->handle, NULL, -1);
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t SpiTestAll(struct SpiTest *test)
{
    int32_t total = 0;
    int32_t error = 0;

    if (SpiSetCfgTest(test) != HDF_SUCCESS) {
        error++;
    }
    total++;
    if (SpiTransferTest(test) != HDF_SUCCESS) {
        error++;
    }
    total++;
    if (SpiWriteTest(test) != HDF_SUCCESS) {
        error++;
    }
    total++;
    if (SpiReadTest(test) != HDF_SUCCESS) {
        error++;
    }
    total++;
    if (SpiReliabilityTest(test) != HDF_SUCCESS) {
        error++;
    }
    total++;
    HDF_LOGE("%s: Spi Test Total %d Error %d", __func__, total, error);
    return HDF_SUCCESS;
}

struct SpiTestFunc g_spiTestFunc[] = {
    {SPI_SET_CFG_TEST, SpiSetCfgTest},
    {SPI_TRANSFER_TEST, SpiTransferTest},
    {SPI_WRITE_TEST, SpiWriteTest},
    {SPI_READ_TEST, SpiReadTest},
    {SPI_RELIABILITY_TEST, SpiReliabilityTest},
    {SPI_PERFORMANCE_TEST, NULL},
    {SPI_TEST_ALL, SpiTestAll},
};

static int32_t SpiTestEntry(struct SpiTest *test, int32_t cmd)
{
    int32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;

    if (test == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    test->handle = SpiTestGetHandle(test);
    if (test->handle == NULL) {
        HDF_LOGE("%s: spi test get handle fail", __func__);
        return HDF_FAILURE;
    }
    for (i = 0; i < sizeof(g_spiTestFunc) / sizeof(g_spiTestFunc[0]); i++) {
        if (cmd == g_spiTestFunc[i].type && g_spiTestFunc[i].Func != NULL) {
            ret = g_spiTestFunc[i].Func(test);
            HDF_LOGE("%s: cmd %d ret %d", __func__, cmd, ret);
            break;
        }
    }
    SpiTestReleaseHandle(test->handle);
    return ret;
}

static int32_t SpiTestBind(struct HdfDeviceObject *device)
{
    static struct SpiTest test;

    if (device != NULL) {
        device->service = &test.service;
    } else {
        HDF_LOGE("%s: device is NULL", __func__);
    }
    return HDF_SUCCESS;
}

static int32_t SpiTestInitFromHcs(struct SpiTest *test, const struct DeviceResourceNode *node)
{
    int32_t ret;
    int32_t i;
    uint32_t *tmp = NULL;
    struct DeviceResourceIface *face = NULL;

    face = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (face == NULL) {
        HDF_LOGE("%s: face is null", __func__);
        return HDF_FAILURE;
    }
    if (face->GetUint32 == NULL || face->GetUint32Array == NULL) {
        HDF_LOGE("%s: GetUint32 or GetUint32Array not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    ret = face->GetUint32(node, "bus", &test->bus, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read bus fail", __func__);
        return HDF_FAILURE;
    }
    ret = face->GetUint32(node, "cs", &test->cs, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read cs fail", __func__);
        return HDF_FAILURE;
    }
    ret = face->GetUint32(node, "len", &test->len, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read len fail", __func__);
        return HDF_FAILURE;
    }
    test->wbuf = (uint8_t *)OsalMemCalloc(test->len);
    if (test->wbuf == NULL) {
        HDF_LOGE("%s: wbuf OsalMemCalloc error\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    tmp = (uint32_t *)OsalMemCalloc(test->len * sizeof(uint32_t));
    if (tmp == NULL) {
        HDF_LOGE("%s: tmp OsalMemCalloc error\n", __func__);
        OsalMemFree(test->wbuf);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = face->GetUint32Array(node, "wbuf", tmp, test->len, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read wbuf fail", __func__);
        OsalMemFree(test->wbuf);
        OsalMemFree(tmp);
        return HDF_FAILURE;
    }
    for (i = 0; i < test->len; i++) {
        test->wbuf[i] = tmp[i];
    }
    OsalMemFree(tmp);
    test->rbuf = (uint8_t *)OsalMemCalloc(test->len);
    if (test->rbuf == NULL) {
        HDF_LOGE("%s: rbuf OsalMemCalloc error\n", __func__);
        OsalMemFree(test->wbuf);
        return HDF_ERR_MALLOC_FAIL;
    }
    return HDF_SUCCESS;
}

static int32_t SpiTestInit(struct HdfDeviceObject *device)
{
    struct SpiTest *test = NULL;

    if (device == NULL || device->service == NULL || device->property == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    test = (struct SpiTest *)device->service;
    SpiTestInitFromHcs(test, device->property);
    HDF_LOGE("%s: success", __func__);
    test->TestEntry = SpiTestEntry;
    return HDF_SUCCESS;
}

static void SpiTestRelease(struct HdfDeviceObject *device)
{
    struct SpiTest *test = NULL;

    if (device == NULL) {
        return;
    }
    test = (struct SpiTest *)device->service;
    if (test == NULL) {
        return;
    }
    if (test->wbuf != NULL) {
        OsalMemFree(test->wbuf);
    }
    if (test->rbuf != NULL) {
        OsalMemFree(test->rbuf);
    }
}

struct HdfDriverEntry g_spiTestEntry = {
    .moduleVersion = 1,
    .Bind = SpiTestBind,
    .Init = SpiTestInit,
    .Release = SpiTestRelease,
    .moduleName = "PLATFORM_SPI_TEST",
};
HDF_INIT(g_spiTestEntry);
