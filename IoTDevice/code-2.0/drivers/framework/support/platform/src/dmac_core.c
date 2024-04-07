/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "dmac_core.h"
#include <string.h>
#include "hdf_log.h"
#include "osal_io.h"
#include "osal_irq.h"
#include "osal_mem.h"

#define HDF_LOG_TAG dmac_core

#define DMA_ALIGN_SIZE 256
#define DMA_MAX_TRANS_SIZE_DEFAULT 256

static int DmacCheck(struct DmaCntlr *cntlr)
{
    if (cntlr == NULL ||
        cntlr->channelNum == 0 ||
        cntlr->dmacGetChanStatus == NULL ||
        cntlr->dmacCacheFlush == NULL ||
        cntlr->dmacCacheInv == NULL ||
        cntlr->dmaM2mChanEnable == NULL ||
        cntlr->dmacPaddrToVaddr == NULL ||
        cntlr->dmaChanEnable == NULL ||
        cntlr->dmacVaddrToPaddr == NULL ||
        cntlr->getChanInfo == NULL ||
        cntlr->dmacChanDisable == NULL ||
        cntlr->dmacGetCurrDestAddr == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

struct DmaCntlr *DmaCntlrCreate(struct HdfDeviceObject *device)
{
    struct DmaCntlr *cntlr = NULL;

    if (device == NULL) {
        return NULL;
    }
    cntlr = (struct DmaCntlr *)OsalMemCalloc(sizeof(struct DmaCntlr));
    if (cntlr == NULL) {
        HDF_LOGE("service malloc fail!\n");
        return NULL;
    }
    cntlr->device = device;
    return cntlr;
}

static void DmacFreeLli(struct DmacChanInfo *chanInfo)
{
    if (chanInfo->lli != NULL) {
        OsalMemFree(chanInfo->lli);
        chanInfo->lli = NULL;
        chanInfo->lliCnt = 0;
    }
}

/*
 * If private is allocated, release private before calling this function
 */
void DmaCntlrDestroy(struct DmaCntlr *cntlr)
{
    unsigned int i;

    if (cntlr == NULL || cntlr->channelNum > DMAC_CHAN_NUM_MAX) {
        HDF_LOGE("%s: cntlr null or channel invalid!", __func__);
        return;
    }
    if (cntlr->channelList != NULL) {
        for (i = 0; i < cntlr->channelNum; i++) {
            DmacFreeLli(&(cntlr->channelList[i]));
        }
        OsalMemFree(cntlr->channelList);
        cntlr->channelList = NULL;
        cntlr->channelNum = 0;
    }
    /* Private is released by the caller */
    cntlr->private = NULL;
    OsalMemFree(cntlr);
}

static void DmacEventCallback(struct DmacChanInfo *chanInfo)
{
    if (chanInfo->status == DMAC_CHN_ERROR) {
        DmaEventSignal(&chanInfo->waitEvent, DMAC_EVENT_ERROR);
    } else {
        DmaEventSignal(&chanInfo->waitEvent, DMAC_EVENT_DONE);
    }
}

static void DmacCallbackHandle(struct DmacChanInfo *chanInfo)
{
    if (chanInfo->transferType == TRASFER_TYPE_M2M) {
        DmacEventCallback(chanInfo);
        return;
    }
    if (chanInfo->callback != NULL) {
        chanInfo->callback(chanInfo->callbackData, chanInfo->status);
    }
}

static int DmacWaitM2mSendComplete(struct DmaCntlr *cntlr, struct DmacChanInfo *chanInfo)
{
    uint32_t ret;

    if (DmacCheck(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("check fail");
        return HDF_FAILURE;
    }

    ret = DmaEventWait(&chanInfo->waitEvent, DMAC_EVENT_DONE | DMAC_EVENT_ERROR, DMA_EVENT_WAIT_DEF_TIME);
    if (ret == DMAC_EVENT_ERROR) {
        HDF_LOGE("wait event error!");
        return DMAC_CHN_ERROR;
    } else if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT) {
        HDF_LOGE("wait event timeout!");
        return DMAC_CHN_TIMEOUT;
    }

    cntlr->dmacChanDisable(cntlr, chanInfo->channel);
    HDF_LOGD("event finish!");
    return DMAC_CHN_SUCCESS;
}

static int DmacAllocateChannel(struct DmaCntlr *cntlr)
{
    uint32_t flags;
    int i;

    if (DmacCheck(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("check fail");
        return HDF_FAILURE;
    }

    OsalSpinLockIrqSave(&cntlr->lock, &flags);
    for (i = 0; i < cntlr->channelNum; i++) {
        if (cntlr->channelList[i].useStatus ==  DMAC_CHN_VACANCY) {
            cntlr->channelList[i].useStatus = DMAC_CHN_ALLOCAT;
            OsalSpinUnlockIrqRestore(&cntlr->lock, &flags);
            return i;
        }
    }
    OsalSpinUnlockIrqRestore(&cntlr->lock, &flags);
    return HDF_FAILURE;
}

static void DmacFreeChannel(struct DmaCntlr *cntlr, unsigned int channel)
{
    uint32_t flags;

    if (DmacCheck(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("check fail");
        return;
    }

    OsalSpinLockIrqSave(&cntlr->lock, &flags);
    cntlr->channelList[channel].useStatus = DMAC_CHN_VACANCY;
    OsalSpinUnlockIrqRestore(&cntlr->lock, &flags);
}

static struct DmacChanInfo *DmacRequestChannel(struct DmaCntlr *cntlr,
    int type, unsigned int periphAddr)
{
    int ret;
    struct DmacChanInfo *chanInfo = NULL;
    int chan;

    if (DmacCheck(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("check fail");
        return NULL;
    }

    chan = DmacAllocateChannel(cntlr);
    if (chan < 0) {
        HDF_LOGE("%s: getChannel is NULL", __func__);
        return NULL;
    }
    chanInfo = &(cntlr->channelList[chan]);
    chanInfo->channel = (unsigned int)chan;
    chanInfo->transferType = type;
    ret = cntlr->getChanInfo(cntlr, chanInfo, periphAddr);
    if (ret < 0) {
        DmacFreeChannel(cntlr, chan);
        HDF_LOGE("%s: get channel fail ret = %d", __func__, ret);
        return NULL;
    }
    HDF_LOGD("channel = %d, transfer type = %u width = %u, config = 0x%x, lliflag = 0x%x",
        ret, chanInfo->transferType, chanInfo->width, chanInfo->config, chanInfo->lliEnFlag);
    return chanInfo;
}

static int DmacFillLli(struct DmaCntlr *cntlr, struct DmacChanInfo *chanInfo,
    UINTPTR srcaddr, UINTPTR dstaddr, unsigned int length)
{
    unsigned int i;
    struct DmacLli  *plli = NULL;
    unsigned int lliNum;

    if (DmacCheck(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("check fail");
        return HDF_FAILURE;
    }

    plli = chanInfo->lli;
    if (plli == NULL) {
        HDF_LOGE("lli is NULL!\n");
        return HDF_FAILURE;
    }
    lliNum = chanInfo->lliCnt;
    for (i = 0; i < lliNum; i++) {
        plli->nextLli = (long long)cntlr->dmacVaddrToPaddr((void *)plli) + (long long)(i + 1) * sizeof(struct DmacLli);
        if (i < lliNum - 1) {
            plli->nextLli += chanInfo->lliEnFlag;
            plli->count = cntlr->maxTransSize;
        } else {
            plli->nextLli = 0;
            plli->count = cntlr->maxTransSize == 0 ? length : (length % cntlr->maxTransSize);
        }

        plli->srcAddr = (long long)srcaddr;
        plli->destAddr = (long long)dstaddr;
        plli->config = chanInfo->config;

        if (chanInfo->transferType == TRASFER_TYPE_P2M) {
            dstaddr += plli->count;
        } else if (chanInfo->transferType == TRASFER_TYPE_M2P) {
            srcaddr += plli->count;
        }
        plli++;
    }
    plli = chanInfo->lli;
    cntlr->dmacCacheFlush((UINTPTR)plli, (UINTPTR)plli + (UINTPTR)(sizeof(struct DmacLli) * lliNum));
    HDF_LOGD("alloc_addr = 0x%x, alloc_addr + (sizeof(DmacLli) * lli_num)= 0x%x\n",
        (UINTPTR)plli, (UINTPTR)plli + (UINTPTR)(sizeof(struct DmacLli) * lliNum));
    return HDF_SUCCESS;
}

int DmacAllocLli(struct DmacChanInfo *chanInfo, unsigned int length, unsigned int maxSize)
{
    unsigned int lliNum;
    unsigned int allocLength;
    unsigned long *allocAddr = NULL;

    if (maxSize == 0 || chanInfo == NULL) {
        return HDF_FAILURE;
    }
    lliNum = length / maxSize;
    if ((length % maxSize) > 0) {
        lliNum++;
    }
    if (lliNum > 2048) {  /* 2048: lliNum is not more than 2048 */
        HDF_LOGE("lliNum %u is bigger than 2048", lliNum);
        return HDF_FAILURE;
    }
    allocLength = lliNum * sizeof(struct DmacLli);
    allocLength = ALIGN(allocLength, CACHE_ALIGNED_SIZE);
    allocAddr = (unsigned long *)OsalMemAllocAlign(DMA_ALIGN_SIZE, allocLength);
    if (allocAddr == NULL) {
        HDF_LOGE("can't malloc llimem for dma!\n ");
        return HDF_FAILURE;
    }
    if (memset_s(allocAddr, allocLength, 0, allocLength) != EOK) {
        HDF_LOGE("memset_s fail");
        OsalMemFree(allocAddr);
        return HDF_FAILURE;
    }

    chanInfo->lliCnt = lliNum;
    chanInfo->lli = (struct DmacLli *)allocAddr;
    return HDF_SUCCESS;
}

static int32_t DmacPeriphTransfer(struct DmaCntlr *cntlr, struct DmacMsg *msg, unsigned int periphAddr)
{
    int ret;
    struct DmacChanInfo *chanInfo = NULL;

    if (DmacCheck(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("check fail");
        return HDF_FAILURE;
    }
    chanInfo = DmacRequestChannel(cntlr, msg->direct, periphAddr);
    if (chanInfo == NULL) {
        HDF_LOGE("allocate dma channel fail");
        return HDF_FAILURE;
    }
    chanInfo->callbackData = msg->para;
    chanInfo->callback = (DmacCallback *)msg->cb;
    ret = DmacAllocLli(chanInfo, msg->transferSize, cntlr->maxTransSize);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("malloc dmalli space failed");
        DmacFreeChannel(cntlr, chanInfo->channel);
        return HDF_FAILURE;
    }
    ret = DmacFillLli(cntlr, chanInfo, msg->srcAddr, msg->destAddr, msg->transferSize);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("build edmalli failed");
        DmacFreeLli(chanInfo);
        DmacFreeChannel(cntlr, chanInfo->channel);
        return HDF_FAILURE;
    }
    ret = cntlr->dmaChanEnable(cntlr, chanInfo);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("start edma failed!");
        DmacFreeLli(chanInfo);
        DmacFreeChannel(cntlr, chanInfo->channel);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int DmacM2mTransfer(struct DmaCntlr *cntlr, struct DmacMsg *msg)
{
    struct DmacChanInfo *chanInfo = NULL;
    unsigned int leftSize;
    unsigned int dmaCount = 0;
    unsigned int dmaSize;
    int ret;

    if (DmacCheck(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("check fail");
        return HDF_FAILURE;
    }

    chanInfo = DmacRequestChannel(cntlr, TRASFER_TYPE_M2M, PERIPHERALID_INVILD);
    if (chanInfo == NULL) {
        HDF_LOGE("allocate channel fail\n");
        return -1;
    }
    chanInfo->callback = msg->cb;
    chanInfo->callbackData = msg->para;
    cntlr->dmacCacheFlush((UINTPTR)msg->srcAddr, (UINTPTR)(msg->srcAddr + msg->transferSize));
    cntlr->dmacCacheInv((UINTPTR)msg->destAddr, (UINTPTR)(msg->destAddr + msg->transferSize));
    leftSize = msg->transferSize;
    while (leftSize > 0) {
        if (leftSize >= cntlr->maxTransSize) {
            dmaSize = cntlr->maxTransSize;
        } else {
            dmaSize = leftSize;
        }
        ret = cntlr->dmaM2mChanEnable(cntlr, chanInfo, msg->srcAddr + dmaCount * cntlr->maxTransSize,
            msg->destAddr + dmaCount * cntlr->maxTransSize, dmaSize);
        if (ret != 0) {
            HDF_LOGE("HiedmacStartM2m error");
            DmacFreeChannel(cntlr, chanInfo->channel);
            if (chanInfo->callback != NULL) {
                chanInfo->callback(chanInfo->callbackData, DMAC_CHN_ERROR);
            }
            return HDF_FAILURE;
        }
        ret = DmacWaitM2mSendComplete(cntlr, chanInfo);
        if (ret != DMAC_CHN_SUCCESS) {
            HDF_LOGE("dma transfer error");
            DmacFreeChannel(cntlr, chanInfo->channel);
            if (chanInfo->callback != NULL) {
                chanInfo->callback(chanInfo->callbackData, ret);
            }
            return HDF_FAILURE;
        }
        if (dmaSize == 0) {
            DmacFreeChannel(cntlr, chanInfo->channel);
            return HDF_FAILURE;
        }
        leftSize -= dmaSize;
        dmaCount++;
    }
    DmacFreeChannel(cntlr, chanInfo->channel);
    if (chanInfo->callback != NULL) {
        chanInfo->callback(chanInfo->callbackData, DMAC_CHN_SUCCESS);
    }
    return HDF_SUCCESS;
}

int32_t DmaCntlrTransfer(struct DmaCntlr *cntlr, struct DmacMsg *msg)
{
    unsigned int periphAddr;

    if (DmacCheck(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("check fail");
        return HDF_FAILURE;
    }
    if (msg == NULL) {
        return HDF_FAILURE;
    }
    if (msg->direct == TRASFER_TYPE_P2M) {
        periphAddr = msg->srcAddr;
        cntlr->dmacCacheInv((UINTPTR)cntlr->dmacPaddrToVaddr((paddr_t)msg->destAddr),
            (UINTPTR)cntlr->dmacPaddrToVaddr((paddr_t)msg->destAddr) + msg->transferSize);
    } else if (msg->direct == TRASFER_TYPE_M2P) {
        periphAddr = msg->destAddr;
        cntlr->dmacCacheFlush((UINTPTR)cntlr->dmacPaddrToVaddr((paddr_t)msg->srcAddr),
            (UINTPTR)cntlr->dmacPaddrToVaddr((paddr_t)msg->srcAddr) + msg->transferSize);
    } else if (msg->direct == TRASFER_TYPE_M2M) {
        return DmacM2mTransfer(cntlr, msg);
    } else {
        HDF_LOGE("%s: invalid direct %u", __func__, msg->direct);
        return HDF_FAILURE;
    }
    return DmacPeriphTransfer(cntlr, msg, periphAddr);
}

unsigned int DmaGetCurrChanDestAddr(struct DmaCntlr *cntlr, unsigned int chan)
{
    if (DmacCheck(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("check fail");
        return HDF_FAILURE;
    }

    return cntlr->dmacGetCurrDestAddr(cntlr, chan);
}

static uint32_t DmacIsr(uint32_t irq, void *dev)
{
    struct DmaCntlr *cntlr = (struct DmaCntlr *)dev;
    unsigned int channelStatus;
    unsigned int i;

    if (DmacCheck(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("check fail");
        return HDF_FAILURE;
    }

    if (irq != cntlr->irq || cntlr->channelNum > DMAC_CHAN_NUM_MAX) {
        HDF_LOGE("%s: cntlr param err! irq:%u, channel:%u",
            __func__, cntlr->irq, cntlr->channelNum);
        return HDF_ERR_INVALID_PARAM;
    }
    for (i = 0; i < cntlr->channelNum; i++) {
        channelStatus = cntlr->dmacGetChanStatus(cntlr, i);
        if (channelStatus == DMAC_CHN_SUCCESS || channelStatus == DMAC_CHN_ERROR) {
            cntlr->channelList[i].status = channelStatus;
            DmacCallbackHandle(&(cntlr->channelList[i]));
            DmacFreeChannel(cntlr, cntlr->channelList[i].channel);
        }
    }
    return HDF_SUCCESS;
}

int DmacInit(struct DmaCntlr *cntlr)
{
    int i;
    int ret;

    if (DmacCheck(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("check fail");
        return HDF_FAILURE;
    }
    if (cntlr->channelNum == 0 || cntlr->channelNum > DMAC_CHAN_NUM_MAX) {
        HDF_LOGE("%s: invalid channel:%u", __func__, cntlr->channelNum);
        return HDF_FAILURE;
    }
    if (cntlr->maxTransSize == 0) {
        cntlr->maxTransSize = DMA_MAX_TRANS_SIZE_DEFAULT;
    }
    cntlr->remapBase = (char *)OsalIoRemap((unsigned long)cntlr->phyBase, (unsigned long)cntlr->regSize);
    (void)OsalSpinInit(&cntlr->lock);
    cntlr->channelList = (struct DmacChanInfo *)OsalMemCalloc(sizeof(struct DmacChanInfo) * cntlr->channelNum);
    if (cntlr->channelList == NULL) {
        HDF_LOGE("channel list malloc fail");
        (void)OsalSpinDestroy(&cntlr->lock);
        OsalIoUnmap((void *)cntlr->remapBase);
        return HDF_FAILURE;
    }
    for (i = 0; i < cntlr->channelNum; i++) {
        cntlr->dmacChanDisable(cntlr, i);
        DmaEventInit(&(cntlr->channelList[i].waitEvent));
        cntlr->channelList[i].useStatus = DMAC_CHN_VACANCY;
    }
    ret = OsalRegisterIrq(cntlr->irq, 0, (OsalIRQHandle)DmacIsr, "PlatDmac", cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("DMA Irq %u request failed, ret = %d\n", cntlr->irq, ret);
        OsalMemFree(cntlr->channelList);
        cntlr->channelList = NULL;
        (void)OsalSpinDestroy(&cntlr->lock);
        OsalIoUnmap((void *)cntlr->remapBase);
    }
    return ret;
}

