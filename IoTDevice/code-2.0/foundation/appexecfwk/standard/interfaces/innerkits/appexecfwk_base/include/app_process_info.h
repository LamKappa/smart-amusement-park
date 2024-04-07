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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_APP_PROCESS_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_APP_PROCESS_INFO_H

#include <string>

#include "parcel.h"

namespace OHOS {

namespace AppExecFwk {

enum class AppProcessState {
    APP_STATE_BEGIN = 0,
    APP_STATE_CREATE = APP_STATE_BEGIN,
    APP_STATE_READY,
    APP_STATE_FOREGROUND,
    APP_STATE_BACKGROUND,
    APP_STATE_SUSPENDED,
    APP_STATE_TERMINATED,
    APP_STATE_END,
};

enum class WeightReasonCode {
    REASON_UNKNOWN = 0,
    WEIGHT_FOREGROUND = 100,
    WEIGHT_FOREGROUND_SERVICE = 125,
    WEIGHT_VISIBLE = 200,
    WEIGHT_PERCEPTIBLE = 230,
    WEIGHT_SERVICE = 300,
    WEIGHT_TOP_SLEEPING = 325,
    WEIGHT_CANT_SAVE_STATE = 350,
    WEIGHT_CACHED = 400,
    WEIGHT_GONE = 1000,
};

struct AppProcessInfo : public Parcelable {
    std::string processName_;
    std::int32_t pid_;
    std::int32_t uid_;
    AppProcessState state_;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static AppProcessInfo *Unmarshalling(Parcel &parcel);
};

}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_APP_PROCESS_INFO_H