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

#include "recent_mission_info.h"

#include "hilog_wrapper.h"
#include "nlohmann/json.hpp"
#include "string_ex.h"

namespace OHOS {
namespace AAFwk {
bool RecentMissionInfo::ReadFromParcel(Parcel &parcel)
{
    id = parcel.ReadInt32();
    runingState = parcel.ReadInt32();

    auto want = parcel.ReadParcelable<Want>();
    if (want == nullptr) {
        return false;
    }
    baseWant = *want;

    auto bAbility = parcel.ReadParcelable<AppExecFwk::ElementName>();
    if (bAbility == nullptr) {
        return false;
    }
    baseAbility = *bAbility;

    auto tAbility = parcel.ReadParcelable<AppExecFwk::ElementName>();
    if (tAbility == nullptr) {
        return false;
    }
    topAbility = *tAbility;

    size = parcel.ReadInt32();

    auto mDescription = parcel.ReadParcelable<MissionDescriptionInfo>();
    if (mDescription == nullptr) {
        return false;
    }
    missionDescription = *mDescription;

    return true;
}

RecentMissionInfo *RecentMissionInfo::Unmarshalling(Parcel &parcel)
{
    RecentMissionInfo *info = new (std::nothrow) RecentMissionInfo();
    if (info == nullptr) {
        return nullptr;
    }

    if (!info->ReadFromParcel(parcel)) {
        delete info;
        info = nullptr;
    }
    return info;
}

bool RecentMissionInfo::Marshalling(Parcel &parcel) const
{
    parcel.WriteInt32(id);
    parcel.WriteInt32(runingState);
    parcel.WriteParcelable(&baseWant);
    parcel.WriteParcelable(&baseAbility);
    parcel.WriteParcelable(&topAbility);
    parcel.WriteInt32(size);
    parcel.WriteParcelable(&missionDescription);

    return true;
}
}  // namespace AAFwk
}  // namespace OHOS