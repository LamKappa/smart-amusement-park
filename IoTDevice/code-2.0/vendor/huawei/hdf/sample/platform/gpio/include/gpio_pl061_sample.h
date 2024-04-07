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

#ifndef GPIO_PL061_SAMPLE_H
#define GPIO_PL061_SAMPLE_H

#include "gpio_core.h"
#include "gpio_if.h"
#include "osal.h"

#define GROUP_MAX 32
#define BIT_MAX   16

#define GPIO_DATA(base, bit)  ((base) + 0x000 + (1 << ((bit) + 2)))
#define GPIO_DIR(base)        ((base) + 0x400)
#define GPIO_IS(base)         ((base) + 0x404)
#define GPIO_IBE(base)        ((base) + 0x408)
#define GPIO_IEV(base)        ((base) + 0x40C)
#define GPIO_IE(base)         ((base) + 0x410)
#define GPIO_RIS(base)        ((base) + 0x414)
#define GPIO_MIS(base)        ((base) + 0x418)
#define GPIO_IC(base)         ((base) + 0x41C)

struct GpioGroup {
    volatile unsigned char *regBase;
    unsigned int index;
    OsalSpinlock lock;
};

struct Pl061GpioCntlr {
    struct GpioCntlr cntlr;
    volatile unsigned char *regBase;
    uint32_t phyBase;
    uint32_t regStep;
    uint32_t irqStart;
    uint16_t groupNum;
    uint16_t bitNum;
    uint8_t irqShare;
    struct GpioGroup *groups;
};

static struct Pl061GpioCntlr g_samplePl061GpioCntlr = {
    .groups = NULL,
    .groupNum = GROUP_MAX,
    .bitNum = BIT_MAX,
};

static inline struct Pl061GpioCntlr *ToPl061GpioCntlr(struct GpioCntlr *cntlr)
{
    return (struct Pl061GpioCntlr *)cntlr;
}

static inline uint16_t Pl061ToGroupNum(uint16_t gpio)
{
    return (uint16_t)(gpio / g_samplePl061GpioCntlr.bitNum);
}

static inline uint16_t Pl061ToBitNum(uint16_t gpio)
{
    return (uint16_t)(gpio % g_samplePl061GpioCntlr.bitNum);
}

static inline uint16_t Pl061ToGpioNum(uint16_t group, uint16_t bit)
{
    return (uint16_t)(group * g_samplePl061GpioCntlr.bitNum + bit);
}

int32_t Pl061GetGroupByGpioNum(struct GpioCntlr *cntlr, uint16_t gpio,
                               struct GpioGroup **group);
#endif // GPIO_PL061_SAMPLE_H
