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
#ifndef OHOS_SYSTEM_TEST_STEVENT_H
#define OHOS_SYSTEM_TEST_STEVENT_H

#include <string>
#include <vector>
#include <condition_variable>
#include <cstdio>
#include <iostream>
#include <unordered_map>
#include "hilog_wrapper.h"
namespace OHOS {
namespace STtools {

class Event {
public:
    Event();
    ~Event();
    bool Compare();
    int WaitingMessage(const std::string &message, int timeout_ms, bool locked);
    void CompleteMessage(const std::string &message);
    void Clean();
    std::string GetData(const std::string &message);
    void CompleteMessage(const std::string &message, const std::string &data);

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::string waiting_message_;
    std::vector<std::string> complete_message_;
    std::unordered_map<std::string, std::string> message_data_;
};

}  // namespace STtools
}  // namespace OHOS
#endif  // OHOS_SYSTEM_TEST_STEVENT_H