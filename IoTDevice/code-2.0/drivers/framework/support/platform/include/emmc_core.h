/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef EMMC_CORE_H
#define EMMC_CORE_H

#include "emmc_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct EmmcMethod;

enum EmmcIoCmd {
    EMMC_IO_GET_CID = 0,
};

struct EmmcConfigData {
    uint32_t hostId;
};

struct EmmcCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct EmmcConfigData configData;
    void *priv;
    struct EmmcMethod *method;
};

/**
 * @brief emmc host device operations.
 * These methods need to be filled up by specific platform.
 */
struct EmmcMethod {
    int32_t (*getCid)(struct EmmcCntlr *, uint8_t *, uint32_t);
    int32_t (*findHost)(struct EmmcCntlr *, struct EmmcConfigData *);
};

int32_t EmmcCntlrFindHost(struct EmmcCntlr *cntlr);

/**
 * @brief Create a new EmmcCntlr struct, and bind it to a HdfDeviceObject.
 *
 * @param device The HdfDeviceObject of this EmmcCntlr.
 *
 * @return Retrns the pointer of the EmmcCntlr struct on success; returns NULL otherwise.
 * @since 1.0
 */
struct EmmcCntlr *EmmcCntlrCreateAndBind(struct HdfDeviceObject *device);

/**
 * @brief Destroy a EmmcCntlr struct, you should always destroy it with this function.
 *
 * @param cntlr Indicates the emmc cntlr device.
 *
 * @since 1.0
 */
void EmmcCntlrDestroy(struct EmmcCntlr *cntlr);

/**
 * @brief Fill config data for an EmmcCntlr.
 *
 * @param device The HdfDeviceObject of this EmmcCntlr.
 * @param configData The config data of this EmmcCntlr.
 *
 * @return Retrns zero on success; returns negative errno otherwise.
 * @since 1.0
 */
int32_t EmmcFillConfigData(struct HdfDeviceObject *device, struct EmmcConfigData *configData);

/**
 * @brief Turn EmmcCntlr to a HdfDeviceObject.
 *
 * @param cntlr Indicates the emmc cntlr device.
 *
 * @return Retrns the pointer of the HdfDeviceObject on success; returns NULL otherwise.
 * @since 1.0
 */
static inline struct HdfDeviceObject *EmmcCntlrToDevice(struct EmmcCntlr *cntlr)
{
    return (cntlr == NULL) ? NULL : cntlr->device;
}

/**
 * @brief Turn HdfDeviceObject to an EmmcCntlr.
 *
 * @param device Indicates a HdfDeviceObject.
 *
 * @return Retrns the pointer of the EmmcCntlr on success; returns NULL otherwise.
 * @since 1.0
 */
static inline struct EmmcCntlr *EmmcCntlrFromDevice(struct HdfDeviceObject *device)
{
    return (device == NULL) ? NULL : (struct EmmcCntlr *)device->service;
}

/**
 * @brief Get The CID of EMMC device.
 *
 * @param cntlr Indicates the emmc cntlr device.
 * @param cid Indicates the pointer to the CID to read.
 * @param size Indicates the length of the CID.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
int32_t EmmcCntlrGetCid(struct EmmcCntlr *cntlr, uint8_t *cid, uint32_t size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* EMMC_CORE_H */
