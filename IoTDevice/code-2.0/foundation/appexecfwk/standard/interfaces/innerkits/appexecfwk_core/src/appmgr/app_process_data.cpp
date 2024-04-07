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

#include "app_process_data.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

bool AppProcessData::Marshalling(Parcel &parcel) const
{
    return (parcel.WriteString(appName) && parcel.WriteString(processName) &&
            parcel.WriteInt32(static_cast<int32_t>(appState)) && parcel.WriteInt32(pid));
}

bool AppProcessData::ReadFromParcel(Parcel &parcel)
{
    appName = parcel.ReadString();

    processName = parcel.ReadString();

    appState = static_cast<ApplicationState>(parcel.ReadInt32());

    pid = parcel.ReadInt32();

    return true;
}

AppProcessData *AppProcessData::Unmarshalling(Parcel &parcel)
{
    AppProcessData *appProcessData = new (std::nothrow) AppProcessData();
    if (appProcessData && !appProcessData->ReadFromParcel(parcel)) {
        APP_LOGW("failed, because ReadFromParcel failed");
        delete appProcessData;
        appProcessData = nullptr;
    }
    return appProcessData;
}

}  // namespace AppExecFwk
}  // namespace OHOS
