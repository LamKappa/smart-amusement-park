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
#ifndef OHOS_AAFWK_INTENT_PARAMS_H
#define OHOS_AAFWK_INTENT_PARAMS_H

#include <iostream>
#include <map>

#include "ohos/aafwk/base/base_interfaces.h"
#include "refbase.h"
#include "parcel.h"

namespace OHOS {
namespace AAFwk {

class IntentParams final : public Parcelable {
public:
    void SetParam(const std::string &key, IInterface *value);

    sptr<IInterface> GetParam(const std::string &key) const;

    const std::map<std::string, sptr<IInterface>> &GetParams() const;

    bool HasParam(const std::string &key) const;

    virtual bool Marshalling(Parcel &parcel) const;

    static IntentParams *Unmarshalling(Parcel &parcel);

private:
    // Java side should keep consistent.
    enum {
        VALUE_TYPE_NULL = -1,
        VALUE_TYPE_BOOLEAN = 1,
        VALUE_TYPE_BYTE = 2,
        VALUE_TYPE_CHAR = 3,
        VALUE_TYPE_SHORT = 4,
        VALUE_TYPE_INT = 5,
        VALUE_TYPE_LONG = 6,
        VALUE_TYPE_FLOAT = 7,
        VALUE_TYPE_DOUBLE = 8,
        VALUE_TYPE_STRING = 9,
        VALUE_TYPE_CHARSEQUENCE = 10,
        VALUE_TYPE_BOOLEANARRAY = 11,
        VALUE_TYPE_BYTEARRAY = 12,
        VALUE_TYPE_CHARARRAY = 13,
        VALUE_TYPE_SHORTARRAY = 14,
        VALUE_TYPE_INTARRAY = 15,
        VALUE_TYPE_LONGARRAY = 16,
        VALUE_TYPE_FLOATARRAY = 17,
        VALUE_TYPE_DOUBLEARRAY = 18,
        VALUE_TYPE_STRINGARRAY = 19,
        VALUE_TYPE_CHARSEQUENCEARRAY = 20,
    };

    bool WriteArrayToParcel(Parcel &parcel, IArray *ao) const;
    bool ReadArrayToParcel(Parcel &parcel, int type, sptr<IArray> &ao);
    bool ReadFromParcel(Parcel &parcel);

    /* params_ has default construct function,
       no need explicit initialization in the construct function */
    std::map<std::string, sptr<IInterface>> params_;
};

}  // namespace AAFwk
}  // namespace OHOS

#endif  // OHOS_AAFWK_INTENT_PARAMS_H
