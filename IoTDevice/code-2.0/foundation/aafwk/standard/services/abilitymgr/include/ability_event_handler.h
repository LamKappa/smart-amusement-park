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

#ifndef OHOS_AAFWK_ABILITY_EVENT_HANDLER_H
#define OHOS_AAFWK_ABILITY_EVENT_HANDLER_H

#include <memory>

#include "event_handler.h"
#include "event_runner.h"

namespace OHOS {
namespace AAFwk {
class AbilityManagerService;
/**
 * @class AbilityEventHandler
 * AbilityEventHandler handling the ability event.
 */
class AbilityEventHandler : public AppExecFwk::EventHandler {
public:
    AbilityEventHandler(
        const std::shared_ptr<AppExecFwk::EventRunner> &runner, const std::weak_ptr<AbilityManagerService> &server);
    virtual ~AbilityEventHandler() = default;

    /**
     * ProcessEvent with request.
     *
     * @param event, inner event loop.
     */
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;

private:
    void ProcessLoadTimeOut(int64_t eventId);
    void ProcessActiveTimeOut(int64_t eventId);
    void ProcessInactiveTimeOut(int64_t eventId);

private:
    std::weak_ptr<AbilityManagerService> server_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_ABILITY_EVENT_HANDLER_H