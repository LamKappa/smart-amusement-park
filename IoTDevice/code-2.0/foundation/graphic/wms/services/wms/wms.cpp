/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "hals/gfx_engines.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_performance.h"
#include "hals/hi_fbdev.h"
#include "input_manager_service.h"
#include "samgr_lite.h"

#include "lite_wm.h"

extern "C" void __attribute__((weak)) HOS_SystemInit(void)
{
    SAMGR_Bootstrap();
}

namespace {
constexpr uint16_t US_PER_MILLISECOND = 1000;
constexpr uint16_t WMS_MAIN_TASK_PERIOD_IN_US = OHOS::WMS_MAIN_TASK_PERIOD * US_PER_MILLISECOND;
}

int main()
{
    DEBUG_PERFORMANCE_REGISTER_SIG();
    OHOS::HiFbdevInit();
    OHOS::GfxEngines::GetInstance()->InitDriver();
    HOS_SystemInit();
    OHOS::InputManagerService::GetInstance()->Run();
    while (1) {
        DEBUG_PERFORMANCE_PRINT_RESULT();
        OHOS::LiteWM::GetInstance()->MainTaskHandler();
        usleep(WMS_MAIN_TASK_PERIOD_IN_US);
    }
}