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

#include "start_via_asan.h"

#include "properties.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

StartViaAsan::StartViaAsan()
{}

StartViaAsan::~StartViaAsan()
{}

bool StartViaAsan::IsAsanVersion(const std::string &name) const
{
    if (name.empty()) {
        return false;
    }

    std::string defaultWrapValue = "";
    std::string wrapAppName = "wrap." + name;
    std::string propValue = GetProperty(wrapAppName, defaultWrapValue);
    if (propValue != defaultWrapValue) {
        APP_LOGI("%{public}s system prop set, value is %{public}s", wrapAppName.c_str(), propValue.c_str());
        return true;
    }
    APP_LOGI("%{public}s system prop not set", wrapAppName.c_str());
    return false;
}

void StartViaAsan::GetAsanStartMsg(AppSpawnStartMsg &startMsg) const
{
    if (startMsg.arg.empty()) {
        startMsg.arg = "wrap." + startMsg.procName;
    } else {
        startMsg.arg += " wrap." + startMsg.procName;
    }
    startMsg.argsNum++;
}

}  // namespace AppExecFwk
}  // namespace OHOS