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

#include "mission_snapshot_info.h"

#include "hilog_wrapper.h"
#include "nlohmann/json.hpp"
#include "string_ex.h"

namespace OHOS {
namespace AAFwk {
bool MissionSnapshotInfo::ReadFromParcel(Parcel &parcel)
{
    std::unique_ptr<ImageInfo> image(parcel.ReadParcelable<ImageInfo>());
    if (image == nullptr) {
        return false;
    }
    snapshot = *image;
    return true;
}

MissionSnapshotInfo *MissionSnapshotInfo::Unmarshalling(Parcel &parcel)
{
    MissionSnapshotInfo *info = new (std::nothrow) MissionSnapshotInfo();
    if (info == nullptr) {
        return nullptr;
    }

    if (!info->ReadFromParcel(parcel)) {
        delete info;
        info = nullptr;
    }
    return info;
}

bool MissionSnapshotInfo::Marshalling(Parcel &parcel) const
{
    parcel.WriteParcelable(&snapshot);
    return true;
}
}  // namespace AAFwk
}  // namespace OHOS