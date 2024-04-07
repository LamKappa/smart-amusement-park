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

#ifndef OHOS_AAFWK_IPCSINGLETON_H
#define OHOS_AAFWK_IPCSINGLETON_H

#include "nocopyable.h"
#include <mutex>
#include <memory>

namespace OHOS {
#define DECLARE_DELAYED_IPCSINGLETON(MyClass) \
public:                                       \
    ~MyClass();                               \
                                              \
private:                                      \
    friend DelayedIPCSingleton<MyClass>;      \
    MyClass();

template <typename T>
class DelayedIPCSingleton : public NoCopyable {
public:
    static sptr<T> GetInstance()
    {
        if (instance_ == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (instance_ == nullptr) {
                sptr<T> temp(new T);
                instance_ = temp;
            }
        }

        return instance_;
    };
    static void DestroyInstance();

private:
    static sptr<T> instance_;
    static std::mutex mutex_;
};

template <typename T>
sptr<T> DelayedIPCSingleton<T>::instance_ = nullptr;

template <typename T>
std::mutex DelayedIPCSingleton<T>::mutex_;

template <typename T>
void DelayedIPCSingleton<T>::DestroyInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ != nullptr) {
        instance_.reset();
        instance_ = nullptr;
    }
}
}  // namespace OHOS
#endif  // OHOS_AAFWK_IPCSINGLETON_H