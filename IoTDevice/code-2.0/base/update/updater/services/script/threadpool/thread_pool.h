/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef USCRIPT_THREADPOOL_H
#define USCRIPT_THREADPOOL_H
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace uscript {
struct Task {
    std::function<void(int)> processor;
    int32_t workSize;
};
struct TaskNode {
    Task task;
    bool available;
    std::vector<std::atomic_bool*> subTaskFlag;
};

class ThreadPool {
public:
    static ThreadPool* CreateThreadPool(int32_t number);
    static void AddTask(Task &&task);
    static void Destroy();

    void Init(int32_t number);

    void AddNewTask(Task &&task);

    int32_t GetThreadNumber() const
    {
        return threadNumber_;
    }
private:
    void ThreadRun(int32_t threadIndex);
    void RunTask(Task &&task, int32_t index);
    int32_t AcquireWorkIndex();

    static void ThreadExecute(void* context, int32_t threadIndex)
    {
        ((ThreadPool*)context)->ThreadRun(threadIndex);
    }

    ThreadPool()
    {
    }
    ~ThreadPool();

private:
    static const int32_t THREAD_POOL_MAX_TASKS = 2;
    std::vector<std::thread> workers_;
    std::atomic<bool> stop_ = { false };

    std::vector<TaskNode> taskQueue_;
    std::mutex queueMutex_;
    int32_t threadNumber_ = 0;
};
} // namespace uscript
#endif
