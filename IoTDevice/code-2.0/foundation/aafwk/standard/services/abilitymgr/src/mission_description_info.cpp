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

#include "mission_description_info.h"

#include "hilog_wrapper.h"
#include "nlohmann/json.hpp"
#include "string_ex.h"

namespace OHOS {
namespace AAFwk {
bool MissionDescriptionInfo::ReadFromParcel(Parcel &parcel)
{
    label = Str16ToStr8(parcel.ReadString16());
    iconPath = Str16ToStr8(parcel.ReadString16());

    return true;
}

MissionDescriptionInfo *MissionDescriptionInfo::Unmarshalling(Parcel &parcel)
{
    MissionDescriptionInfo *info = new (std::nothrow) MissionDescriptionInfo();
    if (info == nullptr) {
        return nullptr;
    }

    if (!info->ReadFromParcel(parcel)) {
        delete info;
        info = nullptr;
    }
    return info;
}

bool MissionDescriptionInfo::Marshalling(Parcel &parcel) const
{
    parcel.WriteString16(Str8ToStr16(label));
    parcel.WriteString16(Str8ToStr16(iconPath));

    return true;
}
}  // namespace AAFwk
}  // namespace OHOS