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

#include "ability_handler.h"

namespace OHOS {
namespace AppExecFwk {
AbilityHandler::AbilityHandler(const std::shared_ptr<EventRunner> &runner, const sptr<AbilityThread> &server)
    : EventHandler(runner), server_(server)
{}

/**
 * Process the event. Developers should override this method.
 *
 * @param event The event should be processed.
 */
void AbilityHandler::ProcessEvent([[maybe_unused]] const InnerEvent::Pointer &event)
{}
}  // namespace AppExecFwk
}  // namespace OHOS