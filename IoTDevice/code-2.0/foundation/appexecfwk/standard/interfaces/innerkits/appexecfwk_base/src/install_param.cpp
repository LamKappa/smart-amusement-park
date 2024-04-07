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

#include "install_param.h"

#include "nlohmann/json.hpp"
#include "string_ex.h"

#include "app_log_wrapper.h"
#include "parcel_macro.h"

namespace OHOS {
namespace AppExecFwk {

bool InstallParam::ReadFromParcel(Parcel &parcel)
{
    int32_t flagData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, flagData);
    installFlag = static_cast<InstallFlag>(flagData);

    int32_t locationData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, locationData);
    installLocation = static_cast<InstallLocation>(locationData);

    userId = parcel.ReadInt32();
    isKeepData = parcel.ReadBool();
    return true;
}

InstallParam *InstallParam::Unmarshalling(Parcel &parcel)
{
    InstallParam *info = new (std::nothrow) InstallParam();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool InstallParam::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(installFlag));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(installLocation));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, userId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isKeepData);
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS
