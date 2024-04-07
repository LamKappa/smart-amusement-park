/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DMAC_CORE_H
#define DMAC_CORE_H

#include "hdf_base.h"
#include "hdf_device.h"
#include "hdf_device_desc.h"
#include "hdf_object.h"
#include "osal_mutex.h"
#include "osal_spinlock.h"
#include "los_event.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef void DmacCallback(void *callbackData, int status);
typedef EVENT_CB_S DmacEvent;

#define DmaEventInit(event)   LOS_EventInit(event)
#define DmaEventSignal(event, bit)    LOS_EventWrite(event, bit)
#define DmaEventWait(event, bit, timeout) LOS_EventRead(event, bit, LOS_WAITMODE_OR + LOS_WAITMODE_CLR, timeout)

/* definition for the return value */
#define DMAC_ERROR_BASE                  0x100
#define DMAC_CHANNEL_INVALID             ((DMAC_ERROR_BASE) + 1)
#define DMAC_TRXFERSIZE_INVALID          ((DMAC_ERROR_BASE) + 2)
#define DMAC_SOURCE_ADDRESS_INVALID      ((DMAC_ERROR_BASE) + 3)
#define DMAC_DESTINATION_ADDRESS_INVALID ((DMAC_ERROR_BASE) + 4)
#define DMAC_MEMORY_ADDRESS_INVALID      ((DMAC_ERROR_BASE) + 5)
#define DMAC_PERIPHERAL_ID_INVALID       ((DMAC_ERROR_BASE) + 6)
#define DMAC_DIRECTION_ERROR             ((DMAC_ERROR_BASE) + 7)
#define DMAC_TRXFER_ERROR                ((DMAC_ERROR_BASE) + 8)
#define DMAC_LLIHEAD_ERROR               ((DMAC_ERROR_BASE) + 9)
#define DMAC_SWIDTH_ERROR                ((DMAC_ERROR_BASE) + 0xa)
#define DMAC_LLI_ADDRESS_INVALID         ((DMAC_ERROR_BASE) + 0xb)
#define DMAC_TRANS_CONTROL_INVALID       ((DMAC_ERROR_BASE) + 0xc)
#define DMAC_MEMORY_ALLOCATE_ERROR       ((DMAC_ERROR_BASE) + 0xd)
#define DMAC_NOT_FINISHED                ((DMAC_ERROR_BASE) + 0xe)
#define DMAC_TIMEOUT                     ((DMAC_ERROR_BASE) + 0xf)
#define DMAC_CHN_SUCCESS                 ((DMAC_ERROR_BASE) + 0x10)
#define DMAC_CHN_ERROR                   ((DMAC_ERROR_BASE) + 0x11)
#define DMAC_CHN_TIMEOUT                 ((DMAC_ERROR_BASE) + 0x12)
#define DMAC_CHN_ALLOCAT                 ((DMAC_ERROR_BASE) + 0x13)
#define DMAC_CHN_VACANCY                 ((DMAC_ERROR_BASE) + 0x14)

#define DMA_EVENT_WAIT_DEF_TIME ((LOSCFG_BASE_CORE_TICK_PER_SECOND) * 5)
#define DMAC_EVENT_DONE     0x1
#define DMAC_EVENT_ERROR    0x2

#define TRASFER_TYPE_M2M    0x0
#define TRASFER_TYPE_P2M    0x1
#define TRASFER_TYPE_M2P    0x2
#define PERIPHERALID_INVILD 0xfff

#define DMAC_CHAN_NUM_MAX   100

struct DmacMsg {
    UINTPTR srcAddr;
    UINTPTR destAddr;
    unsigned int transferSize;
    unsigned int direct;    /* 0: mem to mem; 1: periph to mem; 2:mem to periph */
    DmacCallback *cb;
    void *para;
};

/* structure for LLI */
struct DmacLli {
    /* must be 64Byte aligned */
    long long nextLli;
    unsigned int reserved[5];
    unsigned int count;
    long long srcAddr;
    long long destAddr;
    unsigned int config;
    unsigned int pad[51];
};

struct DmacChanInfo {
    unsigned int channel;
    unsigned int status;
    unsigned int useStatus;
    unsigned int transferType;
    unsigned int width;
    unsigned int config;
    long long lliEnFlag;
    DmacEvent waitEvent;
    DmacCallback *callback;
    void *callbackData;
    unsigned int lliCnt;
    struct DmacLli *lli;
};

struct DmaCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    unsigned int index;
    unsigned int irq;
    unsigned int phyBase;
    char *remapBase;
    unsigned int regSize;
    unsigned int maxTransSize;
    unsigned int channelNum;
    OsalSpinlock lock;
    struct DmacChanInfo *channelList;
    int (*getChanInfo)(struct DmaCntlr *cntlr, struct DmacChanInfo *chanInfo, unsigned int periphAddr);
    int (*dmaChanEnable)(struct DmaCntlr *cntlr, struct DmacChanInfo *chanInfo);
    int (*dmaM2mChanEnable)(struct DmaCntlr *cntlr, struct DmacChanInfo *chanInfo,
        UINTPTR src, UINTPTR dest, unsigned int length);
    void (*dmacChanDisable)(struct DmaCntlr *cntlr, unsigned int channel);
    void (*dmacCacheInv)(UINTPTR addr, UINTPTR end);
    void (*dmacCacheFlush)(UINTPTR addr, UINTPTR end);
    void *(*dmacPaddrToVaddr)(long long paddr);
    unsigned long (*dmacVaddrToPaddr)(void *vaddr);
    unsigned int (*dmacGetChanStatus)(struct DmaCntlr *cntlr, unsigned int chan);
    unsigned int (*dmacGetCurrDestAddr)(struct DmaCntlr *cntlr, unsigned int chan);
    void *private;
};

struct DmaCntlr *DmaCntlrCreate(struct HdfDeviceObject *dev);
void DmaCntlrDestroy(struct DmaCntlr *cntlr);
int32_t DmaCntlrTransfer(struct DmaCntlr *cntlr, struct DmacMsg *msg);
int DmacInit(struct DmaCntlr *cntlr);
unsigned int DmaGetCurrChanDestAddr(struct DmaCntlr *cntlr, unsigned int chan);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* DMAC_CORE_H */
