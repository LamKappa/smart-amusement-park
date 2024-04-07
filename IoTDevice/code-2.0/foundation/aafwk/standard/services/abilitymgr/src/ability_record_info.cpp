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

#include "mission_record_info.h"

#include "hilog_wrapper.h"
#include "nlohmann/json.hpp"
#include "string_ex.h"

namespace OHOS {
namespace AAFwk {
bool AbilityRecordInfo::ReadFromParcel(Parcel &parcel)
{
    id = parcel.ReadInt32();
    elementName = Str16ToStr8(parcel.ReadString16());
    appName = Str16ToStr8(parcel.ReadString16());
    mainName = Str16ToStr8(parcel.ReadString16());
    abilityType = parcel.ReadInt32();
    previousAppName = Str16ToStr8(parcel.ReadString16());
    previousMainName = Str16ToStr8(parcel.ReadString16());
    nextAppName = Str16ToStr8(parcel.ReadString16());
    nextMainName = Str16ToStr8(parcel.ReadString16());
    int32_t abilityState = parcel.ReadInt32();
    state = static_cast<AbilityState>(abilityState);
    startTime = Str16ToStr8(parcel.ReadString16());
    ready = parcel.ReadBool();
    windowAttached = parcel.ReadBool();
    lanucher = parcel.ReadBool();

    return true;
}

AbilityRecordInfo *AbilityRecordInfo::Unmarshalling(Parcel &parcel)
{
    AbilityRecordInfo *info = new (std::nothrow) AbilityRecordInfo();
    if (info == nullptr) {
        return nullptr;
    }

    if (!info->ReadFromParcel(parcel)) {
        delete info;
        info = nullptr;
    }
    return info;
}

bool AbilityRecordInfo::Marshalling(Parcel &parcel) const
{
    parcel.WriteInt32(id);
    parcel.WriteString16(Str8ToStr16(elementName));
    parcel.WriteString16(Str8ToStr16(appName));
    parcel.WriteString16(Str8ToStr16(mainName));
    parcel.WriteInt32(abilityType);
    parcel.WriteString16(Str8ToStr16(previousAppName));
    parcel.WriteString16(Str8ToStr16(previousMainName));
    parcel.WriteString16(Str8ToStr16(nextAppName));
    parcel.WriteString16(Str8ToStr16(nextMainName));
    parcel.WriteInt32(static_cast<int32_t>(state));
    parcel.WriteString16(Str8ToStr16(startTime));
    parcel.WriteBool(ready);
    parcel.WriteBool(windowAttached);
    parcel.WriteBool(lanucher);

    return true;
}
}  // namespace AAFwk
}  // namespace OHOS