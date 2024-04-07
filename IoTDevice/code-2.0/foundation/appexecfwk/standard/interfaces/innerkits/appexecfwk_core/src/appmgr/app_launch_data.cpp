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

#include "app_launch_data.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

void AppLaunchData::SetApplicationInfo(const ApplicationInfo &info)
{
    applicationInfo_ = info;
}

void AppLaunchData::SetProfile(const Profile &profile)
{
    profile_ = profile;
}

void AppLaunchData::SetProcessInfo(const ProcessInfo &info)
{
    processInfo_ = info;
}

void AppLaunchData::SetRecordId(const int32_t recordId)
{
    recordId_ = recordId;
}

void AppLaunchData::SetUId(const int32_t uId)
{
    uId_ = uId;
}

bool AppLaunchData::Marshalling(Parcel &parcel) const
{
    return (parcel.WriteParcelable(&applicationInfo_) && parcel.WriteParcelable(&profile_) &&
            parcel.WriteParcelable(&processInfo_) && parcel.WriteInt32(recordId_) && parcel.WriteInt32(uId_));
}

bool AppLaunchData::ReadFromParcel(Parcel &parcel)
{
    std::unique_ptr<ApplicationInfo> applicationInfoRead(parcel.ReadParcelable<ApplicationInfo>());
    if (!applicationInfoRead) {
        APP_LOGE("failed, applicationInfoRead is nullptr");
        return false;
    }
    applicationInfo_ = *applicationInfoRead;

    std::unique_ptr<Profile> profileRead(parcel.ReadParcelable<Profile>());
    if (!profileRead) {
        APP_LOGE("failed, profileRead is nullptr");
        return false;
    }
    profile_ = *profileRead;

    std::unique_ptr<ProcessInfo> processInfoRead(parcel.ReadParcelable<ProcessInfo>());
    if (!processInfoRead) {
        APP_LOGE("failed, processInfoRead is nullptr");
        return false;
    }
    processInfo_ = *processInfoRead;

    recordId_ = parcel.ReadInt32();
    uId_ = parcel.ReadInt32();
    return true;
}

AppLaunchData *AppLaunchData::Unmarshalling(Parcel &parcel)
{
    AppLaunchData *appLaunchData = new (std::nothrow) AppLaunchData();
    if (appLaunchData && !appLaunchData->ReadFromParcel(parcel)) {
        APP_LOGW("failed, because ReadFromParcel failed");
        delete appLaunchData;
        appLaunchData = nullptr;
    }
    return appLaunchData;
}

}  // namespace AppExecFwk
}  // namespace OHOS
