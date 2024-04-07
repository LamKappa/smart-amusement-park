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

#include "stack_info.h"

#include "hilog_wrapper.h"
#include "nlohmann/json.hpp"
#include "string_ex.h"

namespace OHOS {
namespace AAFwk {
bool StackInfo::ReadFromParcel(Parcel &parcel)
{
    int32_t missionStackInfosSize = parcel.ReadInt32();
    for (int32_t i = 0; i < missionStackInfosSize; i++) {
        std::unique_ptr<MissionStackInfo> missionStackInfo(parcel.ReadParcelable<MissionStackInfo>());
        if (!missionStackInfo) {
            HILOG_ERROR("ReadParcelable<MissionStackInfo> failed");
            return false;
        }
        missionStackInfos.emplace_back(*missionStackInfo);
    }
    return true;
}

StackInfo *StackInfo::Unmarshalling(Parcel &parcel)
{
    StackInfo *info = new (std::nothrow) StackInfo();
    if (info == nullptr) {
        return nullptr;
    }

    if (!info->ReadFromParcel(parcel)) {
        delete info;
        info = nullptr;
    }
    return info;
}

bool StackInfo::Marshalling(Parcel &parcel) const
{
    parcel.WriteInt32(missionStackInfos.size());
    for (auto &missionStackInfo : missionStackInfos) {
        parcel.WriteParcelable(&missionStackInfo);
    }

    return true;
}
}  // namespace AAFwk
}  // namespace OHOS