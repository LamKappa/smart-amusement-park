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

#ifndef APP_EVENT_DISPATCHER_H
#define APP_EVENT_DISPATCHER_H
#include <condition_variable>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

namespace OHOS {
namespace HiviewDFX {
class AppEventPack;
class AppEventDispatcher {
public:
    static AppEventDispatcher& GetInstance();
    void Start();
    void Stop();
    void AddEvent(std::shared_ptr<AppEventPack> appEventPack);
    void SetProperty(const std::string& prop, const std::string& value);
    void RegisterHandler(std::function<void(std::shared_ptr<AppEventPack>)> func);

private:
    AppEventDispatcher():isRunning_(false), stop_(false) {}
    AppEventDispatcher(const AppEventDispatcher&) = delete;
    AppEventDispatcher& operator=(const AppEventDispatcher&) = delete;
    ~AppEventDispatcher(){}
    void Run();

private:
    static AppEventDispatcher instance_;
    mutable std::mutex mutexLock_;
    std::condition_variable condition_;
    bool isRunning_;
    bool stop_;
    std::list<std::shared_ptr<AppEventPack>> appEventPacks_;
    std::unique_ptr<std::thread> thread_;
    std::map<std::string, std::string> properties_;
    std::function<void(std::shared_ptr<AppEventPack>)> callback_;
};
} // HiviewDFX
} // OHOS
#endif // APP_EVENT_DISPATCHER_H