/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MIPI_DSI_CORE_H
#define MIPI_DSI_CORE_H

#include "osal_mutex.h"
#include "hdf_base.h"
#include "hdf_device.h"
#include "hdf_device_desc.h"
#include "hdf_object.h"
#include "mipi_dsi_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define MAX_CNTLR_CNT 4

struct MipiDsiCntlr {
    unsigned int devNo;            /* device number */
    enum DsiLane lane;
    enum DsiMode mode;
    enum DsiBurstMode burstMode;
    enum DsiOutFormat format;
    struct DsiTimingInfo timing;
    unsigned int phyDataRate;      /* mbps */
    unsigned int pixelClk;         /* KHz */
    int32_t (*setCntlrCfg)(struct MipiDsiCntlr *cntlr);
    int32_t (*setCmd)(struct MipiDsiCntlr *cntlr, struct DsiCmdDesc *cmd);
    int32_t (*getCmd)(struct MipiDsiCntlr *cntlr, struct DsiCmdDesc *cmd, uint32_t readLen, uint8_t *out);
    void (*toHs)(struct MipiDsiCntlr *cntlr);
    void (*toLp)(struct MipiDsiCntlr *cntlr);
    void (*enterUlps)(struct MipiDsiCntlr *cntlr);
    void (*exitUlps)(struct MipiDsiCntlr *cntlr);
    int32_t (*powerControl)(struct MipiDsiCntlr *cntlr, uint8_t enable);
};

struct MipiDsiHandle {
    struct MipiDsiCntlr *cntlr;
    struct OsalMutex lock;
};

int32_t MipiDsiRegisterCntlr(struct MipiDsiCntlr *);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
