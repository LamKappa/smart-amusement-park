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

#include "gpio_pl061_sample.h"
#include "osal_irq.h"

#define HDF_LOG_TAG gpio_pl061_sample

int32_t Pl061GetGroupByGpioNum(struct GpioCntlr *cntlr, uint16_t gpio, struct GpioGroup **group)
{
    struct Pl061GpioCntlr *pl061 = NULL;
    uint16_t groupIndex = Pl061ToGroupNum(gpio);

    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    pl061 = ToPl061GpioCntlr(cntlr);
    if (groupIndex >= pl061->groupNum) {
        HDF_LOGE("%s: err group index:%u", __func__, groupIndex);
        return HDF_ERR_INVALID_PARAM;
    }
    *group = &pl061->groups[groupIndex];
    return HDF_SUCCESS;
}