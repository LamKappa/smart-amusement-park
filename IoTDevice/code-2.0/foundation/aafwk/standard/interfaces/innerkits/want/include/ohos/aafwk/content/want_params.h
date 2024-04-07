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
#ifndef OHOS_AAFWK_WANT_PARAMS_H
#define OHOS_AAFWK_WANT_PARAMS_H

#include <iostream>
#include <map>
#include <set>

#include "ohos/aafwk/base/base_interfaces.h"
#include "refbase.h"
#include "parcel.h"

namespace OHOS {
namespace AAFwk {

class WantParams final : public Parcelable {
public:
    WantParams() = default;
    WantParams(const WantParams &wantParams);
    inline ~WantParams()
    {}
    WantParams &operator=(const WantParams &other);
    void SetParam(const std::string &key, IInterface *value);

    sptr<IInterface> GetParam(const std::string &key) const;

    const std::map<std::string, sptr<IInterface>> &GetParams() const;

    const std::set<std::string> KeySet() const;

    void Remove(const std::string &key);

    bool HasParam(const std::string &key) const;

    int Size() const;

    bool IsEmpty() const;

    virtual bool Marshalling(Parcel &parcel) const;

    static WantParams *Unmarshalling(Parcel &parcel);

private:
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
    bool ReadFromParcelParam(Parcel &parcel, const std::string &key, int type);
    bool ReadFromParcelString(Parcel &parcel, const std::string &key);
    bool ReadFromParcelBool(Parcel &parcel, const std::string &key);
    bool ReadFromParcelInt8(Parcel &parcel, const std::string &key);
    bool ReadFromParcelChar(Parcel &parcel, const std::string &key);
    bool ReadFromParcelShort(Parcel &parcel, const std::string &key);
    bool ReadFromParcelInt(Parcel &parcel, const std::string &key);
    bool ReadFromParcelLong(Parcel &parcel, const std::string &key);
    bool ReadFromParcelFloat(Parcel &parcel, const std::string &key);
    bool ReadFromParcelDouble(Parcel &parcel, const std::string &key);

    bool ReadFromParcelArrayString(Parcel &parcel, sptr<IArray> &ao);
    bool ReadFromParcelArrayBool(Parcel &parcel, sptr<IArray> &ao);
    bool ReadFromParcelArrayByte(Parcel &parcel, sptr<IArray> &ao);
    bool ReadFromParcelArrayChar(Parcel &parcel, sptr<IArray> &ao);
    bool ReadFromParcelArrayShort(Parcel &parcel, sptr<IArray> &ao);
    bool ReadFromParcelArrayInt(Parcel &parcel, sptr<IArray> &ao);
    bool ReadFromParcelArrayLong(Parcel &parcel, sptr<IArray> &ao);
    bool ReadFromParcelArrayFloat(Parcel &parcel, sptr<IArray> &ao);
    bool ReadFromParcelArrayDouble(Parcel &parcel, sptr<IArray> &ao);

    bool WriteArrayToParcelString(Parcel &parcel, IArray *ao) const;
    bool WriteArrayToParcelBool(Parcel &parcel, IArray *ao) const;
    bool WriteArrayToParcelByte(Parcel &parcel, IArray *ao) const;
    bool WriteArrayToParcelChar(Parcel &parcel, IArray *ao) const;
    bool WriteArrayToParcelShort(Parcel &parcel, IArray *ao) const;
    bool WriteArrayToParcelInt(Parcel &parcel, IArray *ao) const;
    bool WriteArrayToParcelLong(Parcel &parcel, IArray *ao) const;
    bool WriteArrayToParcelFloat(Parcel &parcel, IArray *ao) const;
    bool WriteArrayToParcelDouble(Parcel &parcel, IArray *ao) const;

    bool WriteMarshalling(Parcel &parcel, sptr<IInterface> &o) const;
    bool WriteToParcelString(Parcel &parcel, sptr<IInterface> &o) const;
    bool WriteToParcelBool(Parcel &parcel, sptr<IInterface> &o) const;
    bool WriteToParcelByte(Parcel &parcel, sptr<IInterface> &o) const;
    bool WriteToParcelChar(Parcel &parcel, sptr<IInterface> &o) const;
    bool WriteToParcelShort(Parcel &parcel, sptr<IInterface> &o) const;
    bool WriteToParcelInt(Parcel &parcel, sptr<IInterface> &o) const;
    bool WriteToParcelLong(Parcel &parcel, sptr<IInterface> &o) const;
    bool WriteToParcelFloat(Parcel &parcel, sptr<IInterface> &o) const;
    bool WriteToParcelDouble(Parcel &parcel, sptr<IInterface> &o) const;

    std::map<std::string, sptr<IInterface>> params_;
};
}  // namespace AAFwk
}  // namespace OHOS

#endif  // OHOS_AAFWK_WANT_PARAMS_H