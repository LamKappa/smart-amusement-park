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

#include "app_process_info.h"

#include "nlohmann/json.hpp"
#include "string_ex.h"

#include "app_log_wrapper.h"
#include "parcel_macro.h"

namespace OHOS {

namespace AppExecFwk {

namespace {

const std::string JSON_KEY_PROCESSNAME = "processName";
const std::string JSON_KEY_PID = "pid";
const std::string JSON_KEY_STATE = "state";
}  // namespace

bool AppProcessInfo::ReadFromParcel(Parcel &parcel)
{
    processName_ = Str16ToStr8(parcel.ReadString16());
    int32_t typeData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, typeData);
    pid_ = static_cast<int32_t>(typeData);
    int32_t uidData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, uidData);
    uid_ = static_cast<int32_t>(uidData);
    int32_t stateData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, stateData);
    state_ = static_cast<AppProcessState>(stateData);
    return true;
}

AppProcessInfo *AppProcessInfo::Unmarshalling(Parcel &parcel)
{
    AppProcessInfo *info = new (std::nothrow) AppProcessInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool AppProcessInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(processName_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(pid_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(uid_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(state_));
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS