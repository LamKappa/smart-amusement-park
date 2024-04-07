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

#ifndef SPI_PL022_SAMPLE_H
#define SPI_PL022_SAMPLE_H

#include "spi_if.h"
#include "osal.h"

/* ********** spi reg offset define start *************** */
#define REG_SPI_PL022_CR0              0x00
#define SPI_PL022_CR0_SCR_SHIFT        8
#define SPI_PL022_CR0_SPH_SHIFT        7
#define SPI_PL022_CR0_SPO_SHIFT        6
#define SPI_PL022_CR0_FRF_SHIFT        4
#define SPI_PL022_CR0_DSS_SHIFT        0
#define SPI_PL022_CR0_SCR              (0xff << 8) /* clkout=clk/(cpsdvsr*(scr+1)) */
#define SPI_PL022_CR0_SPH              (0x1 << 7)  /* spi phase */
#define SPI_PL022_CR0_SPO              (0x1 << 6)  /* spi clk polarity */
#define SPI_PL022_CR0_FRF              (0x3 << 4)  /* frame format set */
#define SPI_PL022_CR0_DSS              (0xf << 0)  /* data bits width */

#define REG_SPI_PL022_CR1              0x04
#define SPI_PL022_CR1_WAIT_EN_SHIFT    15
#define SPI_PL022_CR1_WAIT_VAL_SHIFT   8
#define SPI_PL022_CR1_ALT_SHIFT        6
#define SPI_PL022_CR1_BIG_END_SHIFT    4
#define SPI_PL022_CR1_MS_SHIFT         2
#define SPI_PL022_CR1_SSE_SHIFT        1
#define SPI_PL022_CR1_LBN_SHIFT        0
#define SPI_PL022_CR1_WAIT_EN          (0x1 << 15)
#define SPI_PL022_CR1_WAIT_VAL         (0x7f << 8)

/* alt mode:spi enable csn is select; spi disable csn is cancel */
#define SPI_PL022_CR1_ALT              (0x1 << 6)
#define SPI_PL022_CR1_BIG_END          (0x1 << 4) /* big end or little */
#define SPI_PL022_CR1_MS               (0x1 << 2) /* cntlr-device mode */
#define SPI_PL022_CR1_SSE              (0x1 << 1) /* spi enable set */
#define SPI_PL022_CR1_LBM              (0x1 << 0) /* loopback mode */

#define REG_SPI_PL022_DR               0x08

#define REG_SPI_PL022_SR               0x0c
#define SPI_PL022_SR_BSY_SHIFT         4
#define SPI_PL022_SR_RFF_SHIFT         3
#define SPI_PL022_SR_RNE_SHIFT         2
#define SPI_PL022_SR_TNF_SHIFT         1
#define SPI_PL022_SR_TFE_SHIFT         0
#define SPI_PL022_SR_BSY               (0x1 << 4) /* spi busy flag */
#define SPI_PL022_SR_RFF               (0x1 << 3) /* Whether to send fifo is full */
#define SPI_PL022_SR_RNE               (0x1 << 2) /* Whether to send fifo is no empty */
#define SPI_PL022_SR_TNF               (0x1 << 1) /* Whether to send fifo is no full */
#define SPI_PL022_SR_TFE               (0x1 << 0) /* Whether to send fifo is empty */

#define REG_SPI_PL022_CPSR             0x10
#define SPI_PL022_CPSR_CPSDVSR_SHIFT   0
#define SPI_PL022_CPSR_CPSDVSR         (0xff << 0)  /* even 2~254 */

#define REG_SPI_PL022_IMSC             0x14
#define REG_SPI_PL022_RIS              0x18
#define REG_SPI_PL022_MIS              0x1c
#define REG_SPI_PL022_ICR              0x20

#define REG_SPI_CRG                    0x120100e4 /* CRG_REG_BASE(0x12010000) + 0x0e4 */
#define SPI_CRG_CLK_EN                 0
#define SPI_CRG_CLK_RST                0

#define REG_SPI_MISC_CTRL              0x12030024 /* MISC_REG_BASE(0x12030000) + 0x24 */
#define SPI_MISC_CTRL_CS               0
#define SPI_MISC_CTRL_CS_SHIFT         0
/* ********** spi reg offset define end *************** */

#define MAX_WAIT                       5000
#define DEFAULT_SPEED                  2000000

#define SCR_MAX                        255
#define SCR_MIN                        0
#define CPSDVSR_MAX                    254
#define CPSDVSR_MIN                    2
#define SPI_CS_ACTIVE                  0
#define SPI_CS_INACTIVE                1
#define TWO_BYTES                      2
#define BITS_PER_WORD_MIN              4
#define BITS_PER_WORD_DEFAULT          8
#define BITS_PER_WORD_MAX              16

struct Pl022SpiCntlr {
    struct SpiCntlr *cntlr;
    struct DListHead deviceList;
    volatile unsigned char *regBase;
    uint32_t busNum;
    uint32_t numCs;
    uint32_t curCs;
    uint32_t speed;
    uint32_t fifoSize;
    uint32_t clkRate;
    uint32_t maxSpeedHz;
    uint32_t minSpeedHz;
    uint32_t regCrg;
    uint32_t clkEnBit;
    uint32_t clkRstBit;
    uint32_t regMiscCtrl;
    uint32_t miscCtrlCsShift;
    uint32_t miscCtrlCs;
    uint16_t mode;
    uint8_t bitsPerWord;
    uint8_t transferMode;
};

int ConfigPl022SpiCntlr(struct Pl022SpiCntlr *cntlr);
int32_t TransferOneMessage(struct Pl022SpiCntlr *cntlr, struct SpiMsg *msg);

#endif // SPI_PL022_SAMPLE_H
