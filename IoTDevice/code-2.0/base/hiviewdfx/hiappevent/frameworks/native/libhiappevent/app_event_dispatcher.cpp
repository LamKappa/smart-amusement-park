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

#include "app_event_dispatcher.h"
#include "app_event_handler.h"

namespace OHOS {
namespace HiviewDFX {
AppEventDispatcher AppEventDispatcher::instance_;

AppEventDispatcher& AppEventDispatcher::GetInstance()
{
    return instance_;
}

void AppEventDispatcher::Start()
{
    std::unique_lock<std::mutex> lock(mutexLock_);
    if (isRunning_) {
        return;
    }

    thread_ = std::make_unique<std::thread>(&AppEventDispatcher::Run, this);
}

void AppEventDispatcher::Stop()
{
    stop_ = true;
    condition_.notify_all();
    if (thread_ != nullptr && thread_->joinable()) {
        thread_->join();
    }
    isRunning_ = false;
}

void AppEventDispatcher::AddEvent(std::shared_ptr<AppEventPack> appEventPack)
{
    std::unique_lock<std::mutex> lock(mutexLock_);
    appEventPacks_.push_back(std::move(appEventPack));
    condition_.notify_one();
}

void AppEventDispatcher::SetProperty(const std::string& prop, const std::string& value)
{
    std::unique_lock<std::mutex> lock(mutexLock_);
    properties_[prop] = value;
}

void AppEventDispatcher::Run()
{
    isRunning_ = true;
    AppEventHandler appEventHandler;
    while (true) {
        std::shared_ptr<AppEventPack> appEventPack = nullptr;
        {
            std::unique_lock<std::mutex> lock(mutexLock_);
            while (appEventPacks_.empty()) {
                condition_.wait(lock);
                if (stop_) {
                    return;
                }
            }
            appEventPack = appEventPacks_.front();
            appEventPacks_.pop_front();
        }

        if (appEventPack == nullptr) {
            continue;
        }

        appEventHandler.handler(appEventPack);
        if (callback_ != nullptr) {
            callback_(appEventPack);
        }

        if (stop_) {
            break;
        }
    }
}
} // HiviewDFX
} // OHOS