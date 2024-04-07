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
#include "event_loop.h"

#include <climits>
#include <functional>
#include <thread>

#include <pthread.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/prctl.h>

#include "audit.h"
#include "file_util.h"
#include "logger.h"
#include "thread_util.h"
namespace OHOS {
namespace HiviewDFX {
namespace {
std::future<bool> GetFalseFuture()
{
    std::promise<bool> tmpPromise;
    tmpPromise.set_value(false);
    return tmpPromise.get_future();
}
}

DEFINE_LOG_TAG("HiView-EventLoop");

EventLoop::EventLoop(const std::string &name) : name_(name), nextWakeupTime_(0), currentProcessingEvent_(nullptr)
{}

EventLoop::~EventLoop()
{
    StopLoop();
}

bool EventLoop::InitEventQueueNotifier()
{
#if defined EPOLL_CLOEXEC
    sharedPollingFd_ = UniqueFd(epoll_create1(EPOLL_CLOEXEC));
#else
    sharedPollingFd_ = UniqueFd(epoll_create(1024)); // listen 1024 sockets
#endif
    pendingEventQueueFd_ = UniqueFd(eventfd(0, EFD_NONBLOCK));
    struct epoll_event eventItem;
    eventItem.events = EPOLLIN | EPOLLET;
    eventItem.data.fd = pendingEventQueueFd_.Get();
    int result = epoll_ctl(sharedPollingFd_.Get(), EPOLL_CTL_ADD, pendingEventQueueFd_.Get(), &eventItem);
    if (result < 0) {
        HIVIEW_LOGE("Fail to Create event poll queue.");
        return false;
    }
    return true;
}

void EventLoop::WakeUp()
{
    if (pendingEventQueueFd_.Get() > 0) {
        eventfd_t count = 1;
        write(pendingEventQueueFd_.Get(), &count, sizeof(count));
    }
}

void EventLoop::StartLoop(bool createNewThread)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    if (!IsRunning()) {
        if (!InitEventQueueNotifier()) {
            return;
        }

        isRunning_ = true;
        if (createNewThread) {
            thread_ = std::make_unique<std::thread>(&EventLoop::Run, this);
            return;
        }
    } else {
        return;
    }
    // handle loop in current thread cases
    Run();
}

void EventLoop::StopLoop()
{
    needQuit_ = true;
    if (!IsRunning()) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        while (!pendingEvents_.empty()) {
            pendingEvents_.pop();
        }
        isRunning_ = false;
    }

    WakeUp();
    if (thread_ != nullptr && thread_->joinable()) {
        thread_->join();
    }
}

uint64_t EventLoop::AddEvent(std::shared_ptr<EventHandler> handler, std::shared_ptr<Event> event, const Task task)
{
    if (needQuit_) {
        return 0;
    }

    uint64_t now = NanoSecondSinceSystemStart();
    if (Audit::IsEnabled() && (event != nullptr) && (handler != nullptr) && (!(event->isPipeline_))) {
        auto digest = event->sender_ + Audit::DOMAIN_DELIMITER + handler->GetHandlerInfo() + Audit::DOMAIN_DELIMITER +
                      GetName() + Audit::DOMAIN_DELIMITER + event->GetEventInfo();
        Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_IN, event->createTime_, digest);
    }

    LoopEvent loopEvent = LoopEvent::CreateLoopEvent(now);
    loopEvent.event = std::move(event);
    loopEvent.handler = handler;
    loopEvent.task = task;
    std::lock_guard<std::mutex> lock(queueMutex_);
    pendingEvents_.push(std::move(loopEvent));
    WakeUp();
    return now;
}

std::future<bool> EventLoop::AddEventForResult(std::shared_ptr<EventHandler> handler, std::shared_ptr<Event> event)
{
    if (needQuit_) {
        return GetFalseFuture();
    }

    if (handler == nullptr || event == nullptr) {
        return GetFalseFuture();
    }

    if (Audit::IsEnabled() && (event != nullptr) && (handler != nullptr) && (!(event->isPipeline_))) {
        auto digest = event->sender_ + Audit::DOMAIN_DELIMITER + handler->GetHandlerInfo() + Audit::DOMAIN_DELIMITER +
                      GetName() + Audit::DOMAIN_DELIMITER + event->GetEventInfo();
        Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_IN, event->createTime_, digest);
    }

    auto bind = std::bind(&EventHandler::OnEventProxy, handler.get(), event);
    auto task = std::make_shared<std::packaged_task<bool()>>(bind);
    auto result = task->get_future();
    uint64_t now = NanoSecondSinceSystemStart();
    LoopEvent loopEvent = LoopEvent::CreateLoopEvent(now);
    loopEvent.taskType = LOOP_PACKAGED_TASK;
    loopEvent.event = std::move(event);
    loopEvent.handler = handler;
    loopEvent.packagedTask = std::move(task);
    std::lock_guard<std::mutex> lock(queueMutex_);
    pendingEvents_.push(std::move(loopEvent));
    WakeUp();
    return result;
}

uint64_t EventLoop::AddTimerEvent(std::shared_ptr<EventHandler> handler, std::shared_ptr<Event> event, const Task &task,
    uint64_t interval, bool repeat)
{
    if (needQuit_) {
        return 0;
    }

    uint64_t now = NanoSecondSinceSystemStart();
    uint64_t intervalMicro = interval * SECOND_TO_NANOSECOND;
    if (now + intervalMicro < now) {
        HIVIEW_LOGW("Add Timer Event fail. The interval is too large. please check.");
        return -1;
    }

    if (Audit::IsEnabled() && (event != nullptr) && (handler != nullptr) && (!(event->isPipeline_))) {
        auto digest = event->sender_ + Audit::DOMAIN_DELIMITER + handler->GetHandlerInfo() + Audit::DOMAIN_DELIMITER +
                      GetName() + Audit::DOMAIN_DELIMITER + event->GetEventInfo();
        Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_IN, event->createTime_, digest);
    }

    LoopEvent loopEvent = LoopEvent::CreateLoopEvent(now);
    loopEvent.isRepeat = repeat;
    loopEvent.taskType = LOOP_EVENT_TASK;
    loopEvent.interval = intervalMicro;
    loopEvent.targetTime = now + intervalMicro;
    loopEvent.event = std::move(event);
    loopEvent.handler = handler;
    loopEvent.task = task;
    std::lock_guard<std::mutex> lock(queueMutex_);
    pendingEvents_.push(std::move(loopEvent));
    ResetTimerIfNeedLocked();
    return now;
}

bool EventLoop::RemoveEvent(uint64_t seq)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    auto curEvent = currentProcessingEvent_.load(std::memory_order_relaxed);
    if ((curEvent != nullptr) && (curEvent->seq == seq)) {
        curEvent->seq = 0;
        HIVIEW_LOGI("removing the current processing event.");
        return false;
    }
    return pendingEvents_.remove(seq);
}

void EventLoop::ResetTimerIfNeedLocked()
{
    const LoopEvent &event = pendingEvents_.top();
    if (nextWakeupTime_ == event.targetTime) {
        return;
    }
    WakeUp();
}

bool EventLoop::AddFileDescriptorEventCallback(
    const std::string &name, std::shared_ptr<FileDescriptorEventCallback> source)
{
    if (needQuit_) {
        return false;
    }

    std::lock_guard<std::mutex> lock(queueMutex_);
    if (eventSourceNameMap_.find(name) != eventSourceNameMap_.end()) {
        HIVIEW_LOGW("Exist fd callback with same name.");
        return false;
    }

    int fd = source->GetPollFd();
    if (fd <= 0) {
        HIVIEW_LOGW("Invalid poll fd.");
        return false;
    }

    struct epoll_event eventItem;
    eventItem.events = source->GetPollType();
    eventItem.data.fd = fd;
    int result = epoll_ctl(sharedPollingFd_.Get(), EPOLL_CTL_ADD, fd, &eventItem);
    if (result < 0) {
        HIVIEW_LOGW("Fail to Add Fd callback.");
        return false;
    }

    eventSourceNameMap_[name] = fd;
    eventSourceMap_[fd] = source;

    return true;
}

bool EventLoop::RemoveFileDescriptorEventCallback(const std::string &name)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    if (eventSourceNameMap_.find(name) == eventSourceNameMap_.end()) {
        HIVIEW_LOGW("fd callback name is not existed.");
        return false;
    }

    int fd = eventSourceNameMap_[name];
    eventSourceNameMap_.erase(name);
    if (epoll_ctl(sharedPollingFd_.Get(), EPOLL_CTL_DEL, fd, nullptr) == -1) {
        HIVIEW_LOGW("fail to remove watched fd.");
    }
    eventSourceMap_.erase(fd);

    return true;
}

void EventLoop::Run()
{
    // set thread name
    const int maxLength = 16;
    std::string restrictedName = name_;
    if (name_.length() >= maxLength) {
        HIVIEW_LOGW("%s is too long for thread, please change to a shorter one.", name_.c_str());
        restrictedName = name_.substr(0, maxLength - 1);
    }
    Thread::SetThreadDescription(restrictedName);

    name_ = name_ + "@" + std::to_string(Thread::GetTid());

    while (true) {
        uint64_t leftTimeNanosecond = ProcessQueuedEvent();
        WaitNextEvent(leftTimeNanosecond);
        if (needQuit_) {
            break;
        }
    }
}

uint64_t EventLoop::ProcessQueuedEvent()
{
    if (pendingEvents_.empty()) {
        return INT_MAX;
    }

    uint64_t leftTimeNanosecond = 0;
    while (!pendingEvents_.empty()) {
        uint64_t now = NanoSecondSinceSystemStart();
        LoopEvent event;
        if (!FetchNextEvent(now, leftTimeNanosecond, event)) {
            break;
        }

        ProcessEvent(event);

        if (event.isRepeat && (event.interval > 0)) {
            // force update time
            now = NanoSecondSinceSystemStart();
            ReInsertPeriodicEvent(now, event);
        }

        std::lock_guard<std::mutex> lock(queueMutex_);
        currentProcessingEvent_.store(nullptr, std::memory_order_relaxed);
    }
    isWaken_ = false;
    return leftTimeNanosecond;
}

bool EventLoop::FetchNextEvent(uint64_t now, uint64_t& leftTimeNanosecond, LoopEvent& out)
{
    if (needQuit_) {
        return false;
    }

    std::lock_guard<std::mutex> lock(queueMutex_);
    if (pendingEvents_.empty()) {
        return false;
    }

    const LoopEvent &event = pendingEvents_.top();
    if (event.targetTime > now) {
        leftTimeNanosecond = event.targetTime - now;
        nextWakeupTime_ = event.targetTime;
        return false;
    }

    out = event;
    pendingEvents_.pop();
    currentProcessingEvent_.store(&out, std::memory_order_relaxed);
    return true;
}

void EventLoop::ProcessEvent(LoopEvent &event)
{
    if (event.taskType == LOOP_EVENT_TASK) {
        if (event.task != nullptr) {
            event.task();
        } else if ((event.handler != nullptr) && (event.event != nullptr)) {
            event.handler->OnEventProxy(event.event);
        } else {
            HIVIEW_LOGW("Loop event task with null tasks.");
        }
    } else if (event.taskType == LOOP_PACKAGED_TASK) {
        if (event.packagedTask != nullptr) {
            event.packagedTask->operator()();
        } else {
            HIVIEW_LOGW("Loop packaged task with null tasks.");
        }
    } else {
        HIVIEW_LOGW("unrecognized task type.");
    }
}

void EventLoop::ReInsertPeriodicEvent(uint64_t now, LoopEvent &event)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    currentProcessingEvent_.store(nullptr, std::memory_order_relaxed);
    if (event.seq == 0) {
        return;
    }

    event.enqueueTime = now;
    event.targetTime = now + event.interval;

    if (Audit::IsEnabled() && (event.event != nullptr) && (event.handler != nullptr)) {
        event.event->ResetTimestamp();
        auto digest = event.event->sender_ + Audit::DOMAIN_DELIMITER + event.handler->GetHandlerInfo() +
                      Audit::DOMAIN_DELIMITER + GetName() + Audit::DOMAIN_DELIMITER + event.event->GetEventInfo();
        Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_IN, event.event->createTime_, digest);
    }

    pendingEvents_.push(std::move(event));
    ResetTimerIfNeedLocked();
}

void EventLoop::WaitNextEvent(uint64_t leftTimeNanosecond)
{
    struct epoll_event eventItems[MAX_EVENT_SIZE];
    int eventCount = epoll_wait(sharedPollingFd_.Get(), eventItems, MAX_EVENT_SIZE,
        (leftTimeNanosecond / NANOSECOND_TO_MILLSECOND));
    isWaken_ = true;
    if (eventCount <= 0) {
        // no event read from watched fd, process queued events
        return;
    }

    for (int i = 0; i < eventCount; i++) {
        int fd = eventItems[i].data.fd;
        uint32_t events = eventItems[i].events;
        if (fd == pendingEventQueueFd_.Get()) {
            // new queued event arrived
            eventfd_t val = 0;
            read(fd, &val, sizeof(val));
            return;
        } else {
            // process data source callbacks
            auto it = eventSourceMap_.find(fd);
            if (it != eventSourceMap_.end()) {
                it->second->OnFileDescriptorEvent(fd, events);
            }
        }
    }
}

uint64_t EventLoop::NanoSecondSinceSystemStart()
{
    auto nanoNow = std::chrono::steady_clock::now().time_since_epoch();
    return nanoNow.count();
}
}  // namespace HiviewDFX
}  // namespace OHOS
