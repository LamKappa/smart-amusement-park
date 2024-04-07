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

#ifndef FOUNDATION_APPEXECFWK_TASK_HANDLER_H
#define FOUNDATION_APPEXECFWK_TASK_HANDLER_H

#include "event_handler.h"
#include "task_handler_client.h"
#include "refbase.h"

namespace OHOS {
namespace AppExecFwk {
class TaskHandler : public EventHandler {
public:
    TaskHandler(const std::shared_ptr<EventRunner> &runner);
    ~TaskHandler()
    {}

    /**
     * Process the event. Developers should override this method.
     *
     * @param event The event should be processed.
     */
    void ProcessEvent(const InnerEvent::Pointer &event) override;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_TASK_HANDLER_H
