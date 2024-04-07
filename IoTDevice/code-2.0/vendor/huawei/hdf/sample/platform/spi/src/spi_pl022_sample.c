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
#include "spi_pl022_sample.h"
#include "osal_io.h"

#define HDF_LOG_TAG spi_pl022_sample

/* Private function prototypes */
static int Pl022SampleSetCs(struct Pl022SpiCntlr *cntlr, uint32_t cs, uint32_t flag);
static int Pl022SampleFlushFifo(const struct Pl022SpiCntlr *cntlr);
static int Pl022SampleTxRx8(const struct Pl022SpiCntlr *cntlr, const struct SpiMsg *msg);
static int Pl022SampleTxRx16(const struct Pl022SpiCntlr *cntlr, const struct SpiMsg *msg);
static void Pl022SampleEnableCntlr(const struct Pl022SpiCntlr *cntlr);
static void Pl022SampleConfigCPSR(const struct Pl022SpiCntlr *cntlr, uint32_t cpsdvsr);
static void Pl022SampleConfigCR0(const struct Pl022SpiCntlr *cntlr, uint32_t scr);
static void Pl022SampleConfigCR1(const struct Pl022SpiCntlr *cntlr);
static int32_t Pl022SampleCfgCs(struct Pl022SpiCntlr *cntlr, uint32_t cs);
static void Pl022SampleDisableCntlr(const struct Pl022SpiCntlr *cntlr);
static int Pl022SampleCheckTimeout(const struct Pl022SpiCntlr *cntlr);

int ConfigPl022SpiCntlr(struct Pl022SpiCntlr *cntlr)
{
    uint32_t tmp;
    uint32_t scr;
    uint32_t cpsdvsr;

    Pl022SampleEnableCntlr(cntlr);
    /* Check if we can provide the requested rate */
    if (cntlr->speed > cntlr->maxSpeedHz) {
        cntlr->speed = cntlr->maxSpeedHz;
    }
    /* Min possible */
    if ((cntlr->speed < cntlr->minSpeedHz) || (cntlr->speed == 0)) {
        HDF_LOGE("%s: cntlr->speed is %u not support, max %u, min %u", __func__,
                 cntlr->speed, cntlr->maxSpeedHz, cntlr->minSpeedHz);
        return HDF_FAILURE;
    }
    /* Check if we can provide the requested bits_per_word */
    if ((cntlr->bitsPerWord < BITS_PER_WORD_MIN) || (cntlr->bitsPerWord > BITS_PER_WORD_MAX)) {
        HDF_LOGE("%s: cntlr->bitsPerWord is %u not support", __func__, cntlr->bitsPerWord);
        return HDF_FAILURE;
    }
    /* compute spi speed, speed=clk/(cpsdvsr*(scr+1)) */
    tmp = (cntlr->clkRate) / (cntlr->speed);
    if (tmp < CPSDVSR_MIN) {
        cpsdvsr = CPSDVSR_MIN;
        scr = 0;
    } else if (tmp <= CPSDVSR_MAX) {
        cpsdvsr = tmp & (~0x1);
        scr = (tmp / cpsdvsr) - 1;
    } else {
        cpsdvsr = CPSDVSR_MAX;
        scr = (tmp / cpsdvsr) - 1;
    }
    /* config SPICPSR register */
    Pl022SampleConfigCPSR(cntlr, cpsdvsr);
    /* config SPICR0 register */
    Pl022SampleConfigCR0(cntlr, scr);
    /* config SPICR1 register */
    Pl022SampleConfigCR1(cntlr);
    return HDF_SUCCESS;
}

int32_t TransferOneMessage(struct Pl022SpiCntlr *cntlr, struct SpiMsg *msg)
{
    int32_t ret;

    if (msg->speed != 0) {
        cntlr->speed = msg->speed;
    }
    ret = ConfigPl022SpiCntlr(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    ret = Pl022SampleSetCs(cntlr, cntlr->curCs, SPI_CS_ACTIVE);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    ret = Pl022SampleFlushFifo(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (cntlr->bitsPerWord <= BITS_PER_WORD_DEFAULT) {
        ret = Pl022SampleTxRx8(cntlr, msg);
    } else {
        ret = Pl022SampleTxRx16(cntlr, msg);
    }
    if (ret || msg->csChange) {
        Pl022SampleSetCs(cntlr, cntlr->curCs, SPI_CS_INACTIVE);
    }
    return ret;
}

/* Private function implementations */
static int Pl022SampleSetCs(struct Pl022SpiCntlr *cntlr, uint32_t cs, uint32_t flag)
{
    if (Pl022SampleCfgCs(cntlr, cs) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (flag == SPI_CS_ACTIVE) {
        Pl022SampleEnableCntlr(cntlr);
    } else {
        Pl022SampleDisableCntlr(cntlr);
    }
    return HDF_SUCCESS;
}

static int Pl022SampleFlushFifo(const struct Pl022SpiCntlr *cntlr)
{
    uint32_t value;
    uint32_t ret;
    uint32_t tmp = 0;

    ret = Pl022SampleCheckTimeout(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    while (true) {
        value = OSAL_READL((UINTPTR)(cntlr->regBase) + REG_SPI_PL022_SR);
        if (!(value & SPI_PL022_SR_RNE)) {
            break;
        }
        if (tmp++ > cntlr->fifoSize) {
            HDF_LOGE("%s: spi transfer check rx fifo wait timeout", __func__);
            return HDF_ERR_TIMEOUT;
        }
        OSAL_READL((UINTPTR)(cntlr->regBase) + REG_SPI_PL022_DR);
    }
    return HDF_SUCCESS;
}

static int Pl022SampleTxRx8(const struct Pl022SpiCntlr *cntlr, const struct SpiMsg *msg)
{
    uint32_t len = msg->len;
    uint32_t tmpLen;
    uint32_t count;
    const uint8_t *tx = (const uint8_t *)(msg->wbuf);
    uint8_t *rx = (uint8_t *)(msg->rbuf);
    uint8_t value;
    uint32_t ret;

    if (tx == NULL && rx == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    while (len > 0) {
        if (len > cntlr->fifoSize) {
            tmpLen = cntlr->fifoSize;
        } else {
            tmpLen = len;
        }
        len -= tmpLen;
        /* write fifo */
        count = tmpLen;
        value = 0;
        while (count > 0) {
            if (tx != NULL) {
                value = *tx++;
            }
            OSAL_WRITEL(value, (UINTPTR)(cntlr->regBase) + REG_SPI_PL022_DR);
            count -= 1;
            OSAL_READL((UINTPTR)(cntlr->regBase) + REG_SPI_PL022_SR);
        }
        ret = Pl022SampleCheckTimeout(cntlr);
        if (ret != HDF_SUCCESS) {
            return ret;
        }
        /* read fifo */
        count = tmpLen;
        while (count > 0) {
            value = OSAL_READL((UINTPTR)(cntlr->regBase) + REG_SPI_PL022_DR);
            if (rx != NULL) {
                *rx++ = value;
            }
            count -= 1;
            OSAL_READL((UINTPTR)(cntlr->regBase) + REG_SPI_PL022_SR);
        }
    }
    return HDF_SUCCESS;
}

static int Pl022SampleTxRx16(const struct Pl022SpiCntlr *cntlr, const struct SpiMsg *msg)
{
    uint32_t len = msg->len;
    uint32_t tmpLen;
    uint32_t count;
    const uint16_t *tx = (const uint16_t *)(msg->wbuf);
    uint16_t *rx = (uint16_t *)(msg->rbuf);
    uint16_t value;
    uint32_t ret;

    if (tx == NULL && rx == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    while (len > 0) {
        ret = cntlr->fifoSize * TWO_BYTES;
        if (len > ret) {
            tmpLen = ret;
        } else {
            tmpLen = len;
        }
        len -= tmpLen;
        /* write fifo */
        count = tmpLen;
        value = 0;
        while (count >= TWO_BYTES) {
            if (tx != NULL) {
                value = *tx++;
            }
            OSAL_WRITEL(value, (UINTPTR)(cntlr->regBase) + REG_SPI_PL022_DR);
            count -= TWO_BYTES;
        }
        ret = Pl022SampleCheckTimeout(cntlr);
        if (ret != 0) {
            return ret;
        }
        /* read fifo */
        count = tmpLen;
        while (count >= TWO_BYTES) {
            value = OSAL_READL((UINTPTR)(cntlr->regBase) + REG_SPI_PL022_DR);
            if (rx != NULL) {
                *rx++ = value;
            }
            count -= TWO_BYTES;
        }
    }
    return 0;
}

static void Pl022SampleEnableCntlr(const struct Pl022SpiCntlr *cntlr)
{
    uint32_t value;

    value = OSAL_READL((UINTPTR)(cntlr->regBase) + REG_SPI_PL022_CR1);
    value |= SPI_PL022_CR1_SSE;
    OSAL_WRITEL(value, (UINTPTR)(cntlr->regBase) + REG_SPI_PL022_CR1);
}

static void Pl022SampleConfigCPSR(const struct Pl022SpiCntlr *cntlr, uint32_t cpsdvsr)
{
    uint32_t value;

    value = OSAL_READL((UINTPTR)(cntlr->regBase) + REG_SPI_PL022_CPSR);
    value &= ~SPI_PL022_CPSR_CPSDVSR;
    value |= cpsdvsr << SPI_PL022_CPSR_CPSDVSR_SHIFT;
    OSAL_WRITEL(value, (UINTPTR)(cntlr->regBase) + REG_SPI_PL022_CPSR);
}

static void Pl022SampleConfigCR0(const struct Pl022SpiCntlr *cntlr, uint32_t scr)
{
    uint32_t tmp;
    uint32_t value;

    value = OSAL_READL((UINTPTR)(cntlr->regBase) + REG_SPI_PL022_CR0);
    value &= ~SPI_PL022_CR0_DSS;
    value |= (cntlr->bitsPerWord - 1) << SPI_PL022_CR0_DSS_SHIFT;
    value &= ~SPI_PL022_CR0_FRF;
    value &= ~SPI_PL022_CR0_SPO;
    tmp = (!!(cntlr->mode & SPI_CLK_POLARITY)) ? (1 << SPI_PL022_CR0_SPO_SHIFT) : 0;
    value |= tmp;
    value &= ~SPI_PL022_CR0_SPH;
    tmp = (!!(cntlr->mode & SPI_CLK_PHASE)) ? (1 << SPI_PL022_CR0_SPH_SHIFT) : 0;
    value |= tmp;
    value &= ~SPI_PL022_CR0_SCR;
    value |= (scr << SPI_PL022_CR0_SCR_SHIFT);
    OSAL_WRITEL(value, (UINTPTR)(cntlr->regBase) + REG_SPI_PL022_CR0);
}

static void Pl022SampleConfigCR1(const struct Pl022SpiCntlr *cntlr)
{
    uint32_t tmp;
    uint32_t value;

    value = OSAL_READL((UINTPTR)(cntlr->regBase) + REG_SPI_PL022_CR1);
    value &= ~SPI_PL022_CR1_LBM;
    tmp = (!!(cntlr->mode & SPI_MODE_LOOP)) ? (1 << SPI_PL022_CR1_LBN_SHIFT) : 0;
    value |= tmp;
    value &= ~SPI_PL022_CR1_MS;
    value &= ~SPI_PL022_CR1_BIG_END;
    tmp = (!!(cntlr->mode & SPI_MODE_LSBFE)) ? (1 << SPI_PL022_CR1_BIG_END_SHIFT) : 0;
    value |= tmp;
    value &= ~SPI_PL022_CR1_ALT;
    value |= 0x1 << SPI_PL022_CR1_ALT_SHIFT;
    OSAL_WRITEL(value, (UINTPTR)(cntlr->regBase) + REG_SPI_PL022_CR1);
}

static int32_t Pl022SampleCfgCs(struct Pl022SpiCntlr *cntlr, uint32_t cs)
{
    uint32_t value;
    uint32_t miscCtrlCs;

    if ((cs + 1) > cntlr->numCs) {
        HDF_LOGE("%s: cs %u is big than cntlr csNum %u", __func__, cs, cntlr->numCs);
        return HDF_FAILURE;
    }
    if (cntlr->numCs == 1) {
        return HDF_SUCCESS;
    }
    miscCtrlCs = (UINTPTR)(cntlr->regBase) + REG_SPI_MISC_CTRL;
    value = OSAL_READL(miscCtrlCs);
    value &= ~miscCtrlCs;
    value |= (cs << cntlr->miscCtrlCsShift);
    OSAL_WRITEL(value, miscCtrlCs);
    return HDF_SUCCESS;
}

static void Pl022SampleDisableCntlr(const struct Pl022SpiCntlr *cntlr)
{
    uint32_t value;

    value = OSAL_READL((UINTPTR)(cntlr->regBase) + REG_SPI_PL022_CR1);
    value &= ~SPI_PL022_CR1_SSE;
    OSAL_WRITEL(value, (UINTPTR)(cntlr->regBase) + REG_SPI_PL022_CR1);
}

static int Pl022SampleCheckTimeout(const struct Pl022SpiCntlr *cntlr)
{
    uint32_t value;
    uint32_t tmp = 0;

    while (true) {
        value = OSAL_READL((UINTPTR)(cntlr->regBase) + REG_SPI_PL022_SR);
        if ((value & SPI_PL022_SR_TFE) && (!(value & SPI_PL022_SR_BSY))) {
            break;
        }
        if (tmp++ > MAX_WAIT) {
            HDF_LOGE("%s: spi transfer wait timeout", __func__);
            return HDF_ERR_TIMEOUT;
        }
        OsalUDelay(1);
    }
    return HDF_SUCCESS;
}