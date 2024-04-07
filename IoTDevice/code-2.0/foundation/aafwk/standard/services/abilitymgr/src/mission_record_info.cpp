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
bool MissionRecordInfo::ReadFromParcel(Parcel &parcel)
{
    parcel.ReadInt32(id);
    int32_t abilityRecordInfosSize = parcel.ReadInt32();
    for (int32_t i = 0; i < abilityRecordInfosSize; i++) {
        std::unique_ptr<AbilityRecordInfo> abilityRecordInfo(parcel.ReadParcelable<AbilityRecordInfo>());
        if (!abilityRecordInfo) {
            HILOG_ERROR("ReadParcelable<AbilityRecordInfo> failed");
            return false;
        }
        abilityRecordInfos.emplace_back(*abilityRecordInfo);
    }
    return true;
}

MissionRecordInfo *MissionRecordInfo::Unmarshalling(Parcel &parcel)
{
    MissionRecordInfo *info = new (std::nothrow) MissionRecordInfo();
    if (info == nullptr) {
        return nullptr;
    }

    if (!info->ReadFromParcel(parcel)) {
        delete info;
        info = nullptr;
    }
    return info;
}

bool MissionRecordInfo::Marshalling(Parcel &parcel) const
{
    parcel.WriteInt32(id);
    parcel.WriteInt32(abilityRecordInfos.size());
    for (auto &abilityRecordInfo : abilityRecordInfos) {
        parcel.WriteParcelable(&abilityRecordInfo);
    }

    return true;
}

}  // namespace AAFwk
}  // namespace OHOS