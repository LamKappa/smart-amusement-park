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

#include "caller_info.h"

#include "string_ex.h"
#include "nlohmann/json.hpp"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {

bool CallerInfo::ReadFromParcel(Parcel &parcel)
{
    requestCode = parcel.ReadInt32();
    deviceId = Str16ToStr8(parcel.ReadString16());
    bundleName = Str16ToStr8(parcel.ReadString16());
    abilityName = Str16ToStr8(parcel.ReadString16());
    return true;
}

CallerInfo *CallerInfo::Unmarshalling(Parcel &parcel)
{
    CallerInfo *info = new (std::nothrow) CallerInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        delete info;
        info = nullptr;
    }
    return info;
}

bool CallerInfo::Marshalling(Parcel &parcel) const
{
    // write requestCode
    if (!parcel.WriteInt32(requestCode)) {
        return false;
    }
    // write deviceId
    if (!parcel.WriteString16(Str8ToStr16(deviceId))) {
        return false;
    }
    // write bundleName
    if (!parcel.WriteString16(Str8ToStr16(bundleName))) {
        return false;
    }
    // write abilityName
    if (!parcel.WriteString16(Str8ToStr16(abilityName))) {
        return false;
    }
    return true;
}

}  // namespace AAFwk
}  // namespace OHOS