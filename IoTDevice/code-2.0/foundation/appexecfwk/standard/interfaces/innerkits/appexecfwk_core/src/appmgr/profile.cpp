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

#include "profile.h"

#include "string_ex.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

Profile::Profile(const std::string &name) : profileName_(name)
{}

bool Profile::ReadFromParcel(Parcel &parcel)
{
    profileName_ = Str16ToStr8(parcel.ReadString16());

    return true;
}

Profile *Profile::Unmarshalling(Parcel &parcel)
{
    Profile *profile = new (std::nothrow) Profile();
    if (profile && !profile->ReadFromParcel(parcel)) {
        APP_LOGW("failed, because ReadFromParcel failed");
        delete profile;
        profile = nullptr;
    }
    return profile;
}

bool Profile::Marshalling(Parcel &parcel) const
{
    return (parcel.WriteString16(Str8ToStr16(profileName_)));
}

}  // namespace AppExecFwk
}  // namespace OHOS
