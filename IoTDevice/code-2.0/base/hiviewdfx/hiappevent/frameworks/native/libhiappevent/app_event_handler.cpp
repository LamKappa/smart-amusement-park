/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "app_event_handler.h"
#include "hiappevent_base.h"
#include "hilog/log.h"

namespace OHOS {
namespace HiviewDFX {
static constexpr HiLogLabel LABEL = { LOG_CORE, HIAPPEVENT_DOMAIN, "HIAPPEVENT_HDL" };

void AppEventHandler::handler(std::shared_ptr<AppEventPack> appEventPack)
{
    HiLog::Info(LABEL, "AppEventHandler WriteEvent eventName=%{public}s, eventType=%{public}d\n",
        appEventPack->GetEventName().c_str(), appEventPack->GetType());
    HiLog::Info(LABEL, "AppEventHandler json=%{public}s\n", appEventPack->GetJsonString().c_str());
}
} // HiviewDFX
} // OHOS