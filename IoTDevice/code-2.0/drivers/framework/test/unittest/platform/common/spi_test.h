/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SPI_TEST_H
#define SPI_TEST_H

#include "hdf_device_desc.h"
#include "hdf_platform.h"

enum SpiTestCmd {
    SPI_SET_CFG_TEST = 0,
    SPI_TRANSFER_TEST,
    SPI_WRITE_TEST,
    SPI_READ_TEST,
    SPI_RELIABILITY_TEST,
    SPI_PERFORMANCE_TEST,
    SPI_TEST_ALL,
};

struct SpiTest {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    int32_t (*TestEntry)(struct SpiTest *test, int32_t cmd);
    uint32_t bus;
    uint32_t cs;
    uint32_t len;
    uint8_t *wbuf;
    uint8_t *rbuf;
    DevHandle handle;
};

static inline struct SpiTest *GetSpiTest(void)
{
    return (struct SpiTest *)DevSvcManagerClntGetService("SPI_TEST");
}

#endif /* SPI_TEST_H */
