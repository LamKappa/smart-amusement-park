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

#ifndef OHOS_AAFWK_INTERFACES_INNERKITS_MISSION_STACK_INFO_H
#define OHOS_AAFWK_INTERFACES_INNERKITS_MISSION_STACK_INFO_H

#include <string>

#include "mission_record_info.h"
#include "parcel.h"

namespace OHOS {
namespace AAFwk {
/**
 * @struct MissionStackInfo
 * MissionStackInfo is used to save informations about mission stack.
 */
struct MissionStackInfo : public Parcelable {
    int32_t id = -1;
    std::vector<MissionRecordInfo> missionRecords;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static MissionStackInfo *Unmarshalling(Parcel &parcel);
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_INTERFACES_INNERKITS_MISSION_STACK_INFO_H