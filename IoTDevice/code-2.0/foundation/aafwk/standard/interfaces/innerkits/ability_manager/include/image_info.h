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

#ifndef OHOS_AAFWK_INTERFACES_INNERKITS_IMAGE_INFO_H
#define OHOS_AAFWK_INTERFACES_INNERKITS_IMAGE_INFO_H

#include <string>

#include "ability_record_info.h"
#include "mission_description_info.h"
#include "parcel.h"
#include "want.h"

namespace OHOS {
namespace AAFwk {
/**
 * @struct ImageInfo
 * Defines image header information.
 */
struct ImageHeader : public Parcelable {
    /**
     * Color format, which is used to match image type. This variable is important.
     */
    uint32_t colorMode = 8;

    uint32_t reserved = 24;

    uint16_t width;

    uint16_t height;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static ImageHeader *Unmarshalling(Parcel &parcel);
};

/**
 * @struct ImageInfo
 * Defines image information.
 */
struct ImageInfo : public Parcelable {
    ImageHeader header;

    /**
     * Size of the image data (in bytes)
     */
    uint32_t dataSize;

    uint8_t *data;

    uint32_t userDataSize;
    /**
     * User-defined data
     */
    void *userData;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static ImageInfo *Unmarshalling(Parcel &parcel);
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_INTERFACES_INNERKITS_IMAGE_INFO_H