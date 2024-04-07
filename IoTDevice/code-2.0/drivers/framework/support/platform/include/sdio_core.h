/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SDIO_CORE_H
#define SDIO_CORE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "sdio_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct SdioCntlr;
struct SdioMethod;

struct SdioConfigData {
    uint32_t funcNum;
    uint32_t vendorId;
    uint32_t deviceId;
};

struct SdioCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct SdioConfigData configData;
    void *priv;
    struct SdioMethod *method;
};

/**
 * @brief sdio host device operations.
 * These methods need to be filled up by specific platform.
 */
struct SdioMethod {
    int32_t (*incrAddrReadBytes)(struct SdioCntlr *, uint8_t *, uint32_t, uint32_t, uint32_t);
    int32_t (*incrAddrWriteBytes)(struct SdioCntlr *, uint8_t *, uint32_t, uint32_t, uint32_t);
    int32_t (*fixedAddrReadBytes)(struct SdioCntlr *, uint8_t *, uint32_t, uint32_t, uint32_t);
    int32_t (*fixedAddrWriteBytes)(struct SdioCntlr *, uint8_t *, uint32_t, uint32_t, uint32_t);
    int32_t (*func0ReadBytes)(struct SdioCntlr *, uint8_t *, uint32_t, uint32_t, uint32_t);
    int32_t (*func0WriteBytes)(struct SdioCntlr *, uint8_t *, uint32_t, uint32_t, uint32_t);
    int32_t (*setBlockSize)(struct SdioCntlr *, uint32_t);
    int32_t (*getCommonInfo)(struct SdioCntlr *, SdioCommonInfo *, uint32_t);
    int32_t (*setCommonInfo)(struct SdioCntlr *, SdioCommonInfo *, uint32_t);
    int32_t (*flushData)(struct SdioCntlr *);
    void (*claimHost)(struct SdioCntlr *);
    void (*releaseHost)(struct SdioCntlr *);
    int32_t (*enableFunc)(struct SdioCntlr *);
    int32_t (*disableFunc)(struct SdioCntlr *);
    int32_t (*claimIrq)(struct SdioCntlr *, SdioIrqHandler *);
    int32_t (*releaseIrq)(struct SdioCntlr *);
    int32_t (*findFunc)(struct SdioCntlr *, struct SdioConfigData *);
};

/**
 * @brief Create a new SdioCntlr struct, and bind it to a HdfDeviceObject.
 *
 * @param device The HdfDeviceObject of this SdioCntlr.
 *
 * @return Retrns the pointer of the SdioCntlr struct on success; returns NULL otherwise.
 * @since 1.0
 */
struct SdioCntlr *SdioCntlrCreateAndBind(struct HdfDeviceObject *device);

/**
 * @brief Destroy a SdioCntlr struct, you should always destroy it with this function.
 *
 * @param cntlr Indicates the SDIO cntlr device.
 *
 * @since 1.0
 */
void SdioCntlrDestroy(struct SdioCntlr *cntlr);

/**
 * @brief Fill config data for an SdioCntlr.
 *
 * @param device The HdfDeviceObject of this SdioCntlr.
 * @param configData The config data of this SdioCntlr.
 *
 * @return Retrns zero on success; returns negative errno otherwise.
 * @since 1.0
 */
int32_t SdioFillConfigData(struct HdfDeviceObject *device, struct SdioConfigData *configData);

/**
 * @brief Turn SdioCntlr to a HdfDeviceObject.
 *
 * @param cntlr Indicates the SDIO cntlr device.
 *
 * @return Retrns the pointer of the HdfDeviceObject on success; returns NULL otherwise.
 * @since 1.0
 */
static inline struct HdfDeviceObject *SdioCntlrToDevice(struct SdioCntlr *cntlr)
{
    return (cntlr == NULL) ? NULL : cntlr->device;
}

/**
 * @brief Turn HdfDeviceObject to an SdioCntlr.
 *
 * @param device Indicates a HdfDeviceObject.
 *
 * @return Retrns the pointer of the SdioCntlr on success; returns NULL otherwise.
 * @since 1.0
 */
static inline struct SdioCntlr *SdioCntlrFromDevice(struct HdfDeviceObject *device)
{
    return (device == NULL) ? NULL : (struct SdioCntlr *)device->service;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* SDIO_CORE_H */
