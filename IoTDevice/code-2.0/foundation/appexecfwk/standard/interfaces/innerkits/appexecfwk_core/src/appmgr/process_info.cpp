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

#include "process_info.h"
#include "string_ex.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

ProcessInfo::ProcessInfo(const std::string &name, const pid_t &pid) : processName_(name), pid_(pid)
{}

/**
 * @brief read this Sequenceable object from a Parcel.
 *
 * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 * @return Returns true if read successed; returns false otherwise.
 */
bool ProcessInfo::ReadFromParcel(Parcel &parcel)
{
    processName_ = Str16ToStr8(parcel.ReadString16());
    pid_ = parcel.ReadInt32();
    return true;
}

/**
 * @brief Unmarshals this Sequenceable object from a Parcel.
 *
 * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 */
ProcessInfo *ProcessInfo::Unmarshalling(Parcel &parcel)
{
    ProcessInfo *processInfo = new (std::nothrow) ProcessInfo();
    if (processInfo && !processInfo->ReadFromParcel(parcel)) {
        APP_LOGE("ProcessInfo::Unmarshalling ReadFromParcel failed");
        delete processInfo;
        processInfo = nullptr;
    }
    return processInfo;
}

/**
 * @brief Marshals this Sequenceable object into a Parcel.
 *
 * @param outParcel Indicates the Parcel object to which the Sequenceable object will be marshaled.
 */
bool ProcessInfo::Marshalling(Parcel &parcel) const
{
    return (parcel.WriteString16(Str8ToStr16(processName_)) && parcel.WriteInt32(pid_));
}

}  // namespace AppExecFwk
}  // namespace OHOS
