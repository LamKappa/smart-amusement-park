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

#include "test_utils.h"
#include <iostream>
#include <numeric>
#include <sstream>
#include "common_event_data.h"
#include "common_event_manager.h"
namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;
bool TestUtils::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    Want want;
    want.SetAction(eventName);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}

Want TestUtils::MakeWant(
    std::string deviceId, std::string abilityName, std::string bundleName, std::map<std::string, std::string> params)
{
    ElementName element(deviceId, bundleName, abilityName);
    Want want;
    want.SetElement(element);
    for (const auto &param : params) {
        want.SetParam(param.first, param.second);
    }
    return want;
}
}  // namespace AppExecFwk
}  // namespace OHOS