/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DISP_H
#define HDF_DISP_H
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_sbuf.h"
#include "lcd_abs_if.h"

#ifdef HDF_LOG_TAG
#undef HDF_LOG_TAG
#endif
#define HDF_LOG_TAG HDF_DISP

typedef int32_t (*DispCmdHandle)(struct HdfDeviceObject *device, struct HdfSBuf *reqData, struct HdfSBuf *rspData);

struct DispInfo {
    uint32_t width;
    uint32_t hbp;
    uint32_t hfp;
    uint32_t hsw;
    uint32_t height;
    uint32_t vbp;
    uint32_t vfp;
    uint32_t vsw;
    uint32_t frameRate;
    uint32_t intfType;
    enum IntfSync intfSync;
    uint32_t minLevel;
    uint32_t maxLevel;
    uint32_t defLevel;
};

struct DispOperations {
    int32_t (*init)(uint32_t devId);
    int32_t (*on)(uint32_t devId);
    int32_t (*off)(uint32_t devId);
    int32_t (*setBacklight)(uint32_t devId, uint32_t level);
    int32_t (*getDispInfo)(uint32_t devId, struct DispInfo *info);
};

struct PlatformOps {
    int32_t (*init)(uint32_t devId);
    int32_t (*on)(uint32_t devId);
    int32_t (*off)(uint32_t devId);
    int32_t (*setBacklight)(uint32_t devId, uint32_t level);
    int32_t (*getDispInfo)(uint32_t devId, struct DispInfo *info);
};

enum PowerMode {
    DISP_ON,
    DISP_OFF,
};

int32_t PlatformRegister(struct PlatformOps *ops);
struct PanelInfo *GetPanelInfo(int32_t index);
struct PanelStatus *GetPanelStatus(int32_t index);
#endif /* HDF_DISP_H */
