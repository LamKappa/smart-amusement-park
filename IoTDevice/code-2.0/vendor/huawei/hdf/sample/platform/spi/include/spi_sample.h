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

#ifndef SPI_SAMPLE_H
#define SPI_SAMPLE_H

#include "spi_core.h"

int32_t SampleSpiCntlrTransfer(struct SpiCntlr *cntlr, struct SpiMsg *msg, uint32_t count);
int32_t SampleSpiCntlrSetCfg(struct SpiCntlr *cntlr, struct SpiCfg *cfg);
int32_t SampleSpiCntlrGetCfg(struct SpiCntlr *cntlr, struct SpiCfg *cfg);

#endif // SPI_SAMPLE_H
