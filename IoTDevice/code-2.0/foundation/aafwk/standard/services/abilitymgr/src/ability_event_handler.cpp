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

#include "ability_event_handler.h"

#include "ability_manager_service.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
AbilityEventHandler::AbilityEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, const std::weak_ptr<AbilityManagerService> &server)
    : AppExecFwk::EventHandler(runner), server_(server)
{
    HILOG_INFO("AbilityEventHandler::AbilityEventHandler::instance created.");
}

void AbilityEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        HILOG_ERROR("AMSEventHandler::ProcessEvent::parameter error");
        return;
    }
    HILOG_DEBUG("AMSEventHandler::ProcessEvent::inner event id obtained: %u.", event->GetInnerEventId());
    switch (event->GetInnerEventId()) {
        case AbilityManagerService::LOAD_TIMEOUT_MSG: {
            ProcessLoadTimeOut(event->GetParam());
            break;
        }
        case AbilityManagerService::ACTIVE_TIMEOUT_MSG: {
            ProcessActiveTimeOut(event->GetParam());
            break;
        }
        case AbilityManagerService::INACTIVE_TIMEOUT_MSG: {
            HILOG_INFO("inactive timeout.");
            // inactivate pre ability immediately in case blocking next ability start
            ProcessInactiveTimeOut(event->GetParam());
            break;
        }
        default: {
            HILOG_WARN("unsupported timeout message.");
            break;
        }
    }
}

void AbilityEventHandler::ProcessLoadTimeOut(int64_t eventId)
{
    HILOG_INFO("attach timeout.");
    auto server = server_.lock();
    if (!server) {
        HILOG_WARN("process event, the ams service is null.");
        return;
    }
    server->HandleLoadTimeOut(eventId);
}

void AbilityEventHandler::ProcessActiveTimeOut(int64_t eventId)
{
    HILOG_INFO("active timeout.");
    auto server = server_.lock();
    if (!server) {
        HILOG_WARN("process event, the ams service is null.");
        return;
    }
    server->HandleActiveTimeOut(eventId);
}
void AbilityEventHandler::ProcessInactiveTimeOut(int64_t eventId)
{
    HILOG_INFO("inactive timeout.");
    auto server = server_.lock();
    if (!server) {
        HILOG_WARN("process event, the ams service is null.");
        return;
    }
    server->HandleInactiveTimeOut(eventId);
}
}  // namespace AAFwk
}  // namespace OHOS