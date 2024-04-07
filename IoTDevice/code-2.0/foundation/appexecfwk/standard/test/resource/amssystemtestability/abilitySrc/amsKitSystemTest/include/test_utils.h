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
#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_
#include "ability_info.h"
#include "ability_lifecycle.h"
#include "application_info.h"
#include "process_info.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
const int OnStateChangedEventWant = LifeCycle::Event::UNDEFINED;
const int OnStateChangedEvent = (int)LifeCycle::Event::UNDEFINED + 1;
const int requestCodeForTerminate = 10;
const int requestCodeForResult = 20;

class TestUtils {
public:
    TestUtils() = default;
    virtual ~TestUtils() = default;
    static bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);
    static Want MakeWant(std::string deviceId, std::string abilityName, std::string bundleName,
        std::map<std::string, std::string> params);
    static std::string ApplicationInfoToString(std::shared_ptr<ApplicationInfo> applicationInfo);
    static std::string AbilityInfoToString(std::shared_ptr<AbilityInfo> abilityInfo);
    static std::string ProcessInfoToString(std::shared_ptr<ProcessInfo> processInfo);
    static std::vector<std::string> split(const std::string &in, const std::string &delim);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // _TEST_UTILS_H_