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

#ifndef OHOS_AAFWK_INTERFACES_INNERKITS_ABILITY_RECORD_INFO_H
#define OHOS_AAFWK_INTERFACES_INNERKITS_ABILITY_RECORD_INFO_H

#include <string>

#include "parcel.h"

namespace OHOS {
namespace AAFwk {
/**
 * @enum AbilityState
 * AbilityState defines the life cycle state of ability.
 */
enum AbilityState {
    INITIAL = 0,
    INACTIVE,
    ACTIVE,
    BACKGROUND,
    SUSPENDED,
    INACTIVATING,
    ACTIVATING,
    MOVING_BACKGROUND,
    TERMINATING,
};

/**
 * @struct AbilityRecordInfo
 * AbilityRecordInfo is used to save informations about ability record.
 */
struct AbilityRecordInfo : public Parcelable {
    int32_t id = -1;
    std::string elementName;
    std::string appName;
    std::string mainName;
    int32_t abilityType = 1;  // Page=1,Service=2,Data=3
    std::string previousAppName;
    std::string previousMainName;
    std::string nextAppName;
    std::string nextMainName;
    AbilityState state = AbilityState::INITIAL;
    std::string startTime;
    bool ready = false;
    bool windowAttached = false;
    bool lanucher = false;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static AbilityRecordInfo *Unmarshalling(Parcel &parcel);
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_INTERFACES_INNERKITS_ABILITY_RECORD_INFO_H