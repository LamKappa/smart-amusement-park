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

#ifndef OHOS_AAFWK_TASK_HANDLER_CLIENT_H
#define OHOS_AAFWK_TASK_HANDLER_CLIENT_H

#include <mutex>
#include "task_handler.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @class TaskHandlerClient
 * TaskHandlerClient is used to access TaskHandler.
 */
class TaskHandler;
class TaskHandlerClient {
public:
    TaskHandlerClient();
    virtual ~TaskHandlerClient();
    static std::shared_ptr<TaskHandlerClient> GetInstance();
    bool PostTask(std::function<void()> task, long delayTime);
    bool CreateRunner();

private:
    static std::mutex mutex_;
    static std::shared_ptr<TaskHandlerClient> instance_;
    std::shared_ptr<TaskHandler> taskHandler_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_TASK_HANDLER_CLIENT_H
