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

#include "app_task_info.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

const std::string &AppTaskInfo::GetName() const
{
    return appName_;
}

const std::string &AppTaskInfo::GetProcessName() const
{
    return processName_;
}

pid_t AppTaskInfo::GetPid() const
{
    return pid_;
}

int32_t AppTaskInfo::GetRecordId() const
{
    return appRecordId_;
}

void AppTaskInfo::SetName(const std::string &appName)
{
    appName_ = appName;
}

void AppTaskInfo::SetProcessName(const std::string &processName)
{
    processName_ = processName;
}

void AppTaskInfo::SetPid(const pid_t pid)
{
    pid_ = pid;
}

void AppTaskInfo::SetRecordId(const int32_t appRecordId)
{
    appRecordId_ = appRecordId;
}

bool AppTaskInfo::Marshalling(Parcel &parcel) const
{
    return (parcel.WriteString(appName_) && parcel.WriteString(processName_) && parcel.WriteInt32(pid_) &&
            parcel.WriteInt32(appRecordId_));
}

bool AppTaskInfo::ReadFromParcel(Parcel &parcel)
{
    return (parcel.ReadString(appName_) && parcel.ReadString(processName_) && parcel.ReadInt32(pid_) &&
            parcel.ReadInt32(appRecordId_));
}

AppTaskInfo *AppTaskInfo::Unmarshalling(Parcel &parcel)
{
    AppTaskInfo *appTaskInfo = new (std::nothrow) AppTaskInfo();
    if (appTaskInfo && !appTaskInfo->ReadFromParcel(parcel)) {
        APP_LOGW("failed, because ReadFromParcel failed");
        delete appTaskInfo;
        appTaskInfo = nullptr;
    }
    return appTaskInfo;
}

}  // namespace AppExecFwk
}  // namespace OHOS
