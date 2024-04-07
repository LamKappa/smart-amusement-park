/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_COMMON_H
#define PLATFORM_COMMON_H

#include "hdf_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum PlatformModuleType {
    PLATFORM_MODULE_GPIO,
    PLATFORM_MODULE_I2C,
    PLATFORM_MODULE_SPI,
    PLATFORM_MODULE_PIN,
    PLATFORM_MODULE_CLOCK,
    PLATFORM_MODULE_REGULATOR,
    PLATFORM_MODULE_MIPI_DSI,
    PLATFORM_MODULE_UART,
    PLATFORM_MODULE_SDIO,
    PLATFORM_MODULE_MDIO,
    PLATFORM_MODULE_APB,
    PLATFORM_MODULE_PCIE,
    PLATFORM_MODULE_PCM,
    PLATFORM_MODULE_I2S,
    PLATFORM_MODULE_PWM,
    PLATFORM_MODULE_DMA,
    PLATFORM_MODULE_ADC,
    PLATFORM_MODULE_RTC,
    PLATFORM_MODULE_WDT,
    PLATFORM_MODULE_I3C,
    PLATFORM_MODULE_CAN,
    PLATFORM_MODULE_HDMI,
    PLATFORM_MODULE_MMC,
    PLATFORM_MODULE_MTD,
    PLATFORM_MODULE_DEFAULT,
    PLATFORM_MODULE_MAX,
};

struct PlatformModuleInfo {
    enum PlatformModuleType moduleType;
    const char *moduleName;
    void *priv;
};

struct PlatformModuleInfo *PlatformModuleInfoGet(enum PlatformModuleType moduleType);
int32_t PlatformModuleInfoCount(void);

enum PlatformErrno {
#define HDF_PLT_ERR_START HDF_BSP_ERR_START            /**< Error number start of platform. */
#define HDF_PLT_ERR_NUM(v) (HDF_PLT_ERR_START + (v))
    HDF_PLT_ERR_OS_API = HDF_ERR_BSP_PLT_API_ERR,
    HDF_PLT_ERR_OPEN_DEV = HDF_PAL_ERR_DEV_CREATE,     /**< Failed to open a device. */
    HDF_PLT_ERR_INNER = HDF_PAL_ERR_INNER,             /**< Internal error of platform framework. */

    HDF_PLT_ERR_NO_DEV = HDF_PLT_ERR_NUM(-5),          /**< There is no device present. */
    HDF_PLT_ERR_DEV_TYPE = HDF_PLT_ERR_NUM(-6),        /**< invalid device type */
    HDF_PLT_ERR_DEV_GET = HDF_PLT_ERR_NUM(-7),         /**< err on device get */
    HDF_PLT_ERR_DEV_ADD = HDF_PLT_ERR_NUM(-8),         /**< err on device add */
    HDF_PLT_ERR_DEV_FULL = HDF_PLT_ERR_NUM(-9),        /**< id number conflict */
    HDF_PLT_ERR_ID_REPEAT = HDF_PLT_ERR_NUM(-10),      /**< id number conflict */
#define HDF_MMC_ERR_START (HDF_PLT_ERR_START - 50)     /**< Error number start for mmc. */
#define HDF_MMC_ERR_NUM(v) (HDF_MMC_ERR_START + (v))
    HDF_MMC_ERR_SWITCH_FAIL = HDF_MMC_ERR_NUM(-1),
    HDF_MMC_ERR_OTHER_CMD_IS_RUNNING = HDF_MMC_ERR_NUM(-2),
    HDF_MMC_ERR_ILLEGAL_SEQ = HDF_MMC_ERR_NUM(-3),
};

void PlatformGlobalLock(void);
void PlatformGlobalUnlock(void);

/* Os adapt */
bool PlatInIrqContext(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PLATFORM_COMMON_H */
