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

#include "lifecycle_state_info.h"

#include "nlohmann/json.hpp"
#include "string_ex.h"

namespace OHOS {
namespace AAFwk {
bool LifeCycleStateInfo::ReadFromParcel(Parcel &parcel)
{
    isNewWant = parcel.ReadBool();
    int32_t stateData = parcel.ReadInt32();
    state = static_cast<AbilityLifeCycleState>(stateData);
    missionId = parcel.ReadInt32();
    std::unique_ptr<CallerInfo> callerInfo(parcel.ReadParcelable<CallerInfo>());
    if (callerInfo == nullptr) {
        return false;
    }
    caller = *callerInfo;
    return true;
}

LifeCycleStateInfo *LifeCycleStateInfo::Unmarshalling(Parcel &parcel)
{
    LifeCycleStateInfo *info = new (std::nothrow) LifeCycleStateInfo();
    if (info == nullptr) {
        return nullptr;
    }

    if (!info->ReadFromParcel(parcel)) {
        delete info;
        info = nullptr;
    }
    return info;
}

bool LifeCycleStateInfo::Marshalling(Parcel &parcel) const
{
    // write isNewWant
    if (!parcel.WriteBool(isNewWant)) {
        return false;
    }
    // write state
    if (!parcel.WriteInt32(static_cast<int32_t>(state))) {
        return false;
    }
    // write missionId
    if (!parcel.WriteInt32(missionId)) {
        return false;
    }
    // write caller
    if (parcel.WriteParcelable(&caller)) {
        return false;
    }
    return true;
}
}  // namespace AAFwk
}  // namespace OHOS