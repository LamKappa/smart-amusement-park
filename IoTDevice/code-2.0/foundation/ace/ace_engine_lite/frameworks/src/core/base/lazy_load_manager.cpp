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

#include "lazy_load_manager.h"
#include "ace_log.h"
#include "component.h"
#include "component_utils.h"
#include "lazy_load_watcher.h"

namespace OHOS {
namespace ACELite {
LazyLoadManager::LazyLoadManager() : firstWatcher_(nullptr), lastWatcher_(nullptr), state_(LazyLoadState::INIT)
{
}

LazyLoadManager::~LazyLoadManager()
{
    ResetWatchers();
}

void LazyLoadManager::ResetWatchers()
{
    LazyLoadWatcher *next = nullptr;
    while (firstWatcher_ != nullptr) {
        next = const_cast<LazyLoadWatcher *>(firstWatcher_->GetNext());
        delete firstWatcher_;
        firstWatcher_ = next;
    }

    lastWatcher_ = nullptr;
    state_ = LazyLoadState::INIT;
}

void LazyLoadManager::RenderLazyLoadWatcher()
{
    LazyLoadWatcher *next = nullptr;
    while (firstWatcher_ != nullptr) {
        Component *componet = ComponentUtils::GetComponentFromBindingObject(firstWatcher_->GetNativeElement());
        if (componet != nullptr) {
            componet->AddWatcherItem(firstWatcher_->GetAttrName(), firstWatcher_->GetAttrGetter());
        }
        next = const_cast<LazyLoadWatcher *>(firstWatcher_->GetNext());
        delete firstWatcher_;
        firstWatcher_ = next;
    }
    state_ = LazyLoadState::DONE;
}

void LazyLoadManager::AddLazyLoadWatcher(jerry_value_t nativeElement, jerry_value_t attrName, jerry_value_t getter)
{
    if (nativeElement == UNDEFINED || attrName == UNDEFINED || getter == UNDEFINED) {
        return;
    }

    LazyLoadWatcher *watcher =
            new LazyLoadWatcher(nativeElement, jerry_acquire_value(attrName), jerry_acquire_value(getter));
    if (watcher == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create watcher errpr");
        return;
    }
    if (firstWatcher_ == nullptr) {
        firstWatcher_ = watcher;
        lastWatcher_ = watcher;
    } else {
        lastWatcher_->SetNext(*watcher);
        lastWatcher_ = watcher;
    }
}
} // namespace ACELite
} // namespace OHOS
