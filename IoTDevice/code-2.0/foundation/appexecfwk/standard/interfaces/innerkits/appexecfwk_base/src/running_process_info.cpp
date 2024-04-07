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

#include "running_process_info.h"

#include "nlohmann/json.hpp"
#include "string_ex.h"

#include "app_log_wrapper.h"
#include "parcel_macro.h"

namespace OHOS {
namespace AppExecFwk {

bool RunningProcessInfo::ReadFromParcel(Parcel &parcel)
{
    int32_t processInfoSize = parcel.ReadInt32();
    for (int32_t i = 0; i < processInfoSize; i++) {
        std::unique_ptr<AppProcessInfo> appProcessInfo(parcel.ReadParcelable<AppProcessInfo>());
        if (!appProcessInfo) {
            APP_LOGE("ReadParcelable<appProcessInfo> failed");
            return false;
        }
        appProcessInfos.emplace_back(*appProcessInfo);
    }
    return true;
}

bool RunningProcessInfo::Marshalling(Parcel &parcel) const
{
    size_t appProcessInfoSize = appProcessInfos.size();
    if (!parcel.WriteInt32(appProcessInfoSize)) {
        return false;
    }
    for (size_t i = 0; i < appProcessInfoSize; i++) {
        if (!parcel.WriteParcelable(&appProcessInfos[i])) {
            return false;
        }
    }
    return true;
}

RunningProcessInfo *RunningProcessInfo::Unmarshalling(Parcel &parcel)
{
    RunningProcessInfo *info = new (std::nothrow) RunningProcessInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

}  // namespace AppExecFwk
}  // namespace OHOS