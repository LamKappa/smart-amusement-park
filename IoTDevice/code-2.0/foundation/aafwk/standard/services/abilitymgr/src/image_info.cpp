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

#include "image_info.h"

#include "string_ex.h"
#include "nlohmann/json.hpp"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
bool ImageHeader::ReadFromParcel(Parcel &parcel)
{
    colorMode = parcel.ReadUint32();
    reserved = parcel.ReadUint32();
    width = parcel.ReadUint16();
    height = parcel.ReadUint16();
    return true;
}

ImageHeader *ImageHeader::Unmarshalling(Parcel &parcel)
{
    ImageHeader *info = new (std::nothrow) ImageHeader();
    if (info == nullptr) {
        return nullptr;
    }

    if (!info->ReadFromParcel(parcel)) {
        delete info;
        info = nullptr;
    }
    return info;
}

bool ImageHeader::Marshalling(Parcel &parcel) const
{
    parcel.WriteUint32(colorMode);
    parcel.WriteUint32(reserved);
    parcel.WriteUint16(width);
    parcel.WriteUint16(height);
    return true;
}

bool ImageInfo::ReadFromParcel(Parcel &parcel)
{
    return false;
}

ImageInfo *ImageInfo::Unmarshalling(Parcel &parcel)
{
    ImageInfo *info = new (std::nothrow) ImageInfo();
    if (info == nullptr) {
        return nullptr;
    }

    if (!info->ReadFromParcel(parcel)) {
        delete info;
        info = nullptr;
    }
    return info;
}

bool ImageInfo::Marshalling(Parcel &parcel) const
{
    return false;
}
}  // namespace AAFwk
}  // namespace OHOS