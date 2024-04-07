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

#include "app_process_manager.h"

#include <csignal>

#include "app_log_wrapper.h"

namespace OHOS {

namespace AppExecFwk {

AppProcessManager::AppProcessManager()
{}

AppProcessManager::~AppProcessManager()
{}

void AppProcessManager::RemoveAppFromRecentList(const std::shared_ptr<AppTaskInfo> &appTaskInfo)
{
    if (appTaskInfo) {
        recentAppList_.remove(appTaskInfo);
    }
}

void AppProcessManager::ClearRecentAppList()
{
    recentAppList_.clear();
}

void AppProcessManager::AddAppToRecentList(
    const std::string &appName, const std::string &processName, const pid_t pid, const int32_t recordId)
{
    auto appTaskInfo = std::make_shared<AppTaskInfo>();
    appTaskInfo->SetName(appName);
    appTaskInfo->SetProcessName(processName);
    appTaskInfo->SetPid(pid);
    appTaskInfo->SetRecordId(recordId);
    recentAppList_.push_front(appTaskInfo);
}

const std::list<const std::shared_ptr<AppTaskInfo>> &AppProcessManager::GetRecentAppList() const
{
    return recentAppList_;
}

void AppProcessManager::PushAppFront(const int32_t recordId)
{
    auto appTaskInfo = GetAppTaskInfoById(recordId);
    if (appTaskInfo) {
        recentAppList_.remove(appTaskInfo);
        recentAppList_.push_front(appTaskInfo);
    }
}

void AppProcessManager::RemoveAppFromRecentListById(const int32_t recordId)
{
    auto appTaskInfo = GetAppTaskInfoById(recordId);
    if (appTaskInfo) {
        recentAppList_.remove(appTaskInfo);
    }
}

const std::shared_ptr<AppTaskInfo> AppProcessManager::GetAppTaskInfoById(const int32_t recordId) const
{
    const auto &iter = std::find_if(recentAppList_.begin(), recentAppList_.end(), [&recordId](const auto &appTaskInfo) {
        return appTaskInfo ? (appTaskInfo->GetRecordId() == recordId) : false;
    });
    return (iter == recentAppList_.end()) ? nullptr : (*iter);
}

std::shared_ptr<AppTaskInfo> AppProcessManager::GetAppTaskInfoByProcessName(
    const std::string &appName, const std::string &processName) const
{
    const auto &iter =
        std::find_if(recentAppList_.begin(), recentAppList_.end(), [&appName, &processName](const auto &appTaskInfo) {
            return ((appTaskInfo->GetName() == appName) && (appTaskInfo->GetProcessName() == processName));
        });
    return ((iter == recentAppList_.end()) ? nullptr : *iter);
}

}  // namespace AppExecFwk
}  // namespace OHOS
