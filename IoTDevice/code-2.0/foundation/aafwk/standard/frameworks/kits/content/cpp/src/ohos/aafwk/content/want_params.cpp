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

#include "want_params.h"
#include "parcel.h"
#include "string_ex.h"
#include "ohos/aafwk/base/base_interfaces.h"
#include "ohos/aafwk/base/array_wrapper.h"
#include "ohos/aafwk/base/base_object.h"
#include "ohos/aafwk/base/bool_wrapper.h"
#include "ohos/aafwk/base/byte_wrapper.h"
#include "ohos/aafwk/base/short_wrapper.h"
#include "ohos/aafwk/base/int_wrapper.h"
#include "ohos/aafwk/base/long_wrapper.h"
#include "ohos/aafwk/base/float_wrapper.h"
#include "ohos/aafwk/base/double_wrapper.h"
#include "ohos/aafwk/base/string_wrapper.h"
#include "ohos/aafwk/base/zchar_wrapper.h"

namespace OHOS {
namespace AAFwk {
/**
 * @description: A constructor used to create an IntentParams instance by using the parameters of an existing
 * IntentParams object.
 * @param intentParams  Indicates the existing IntentParams object.
 */
WantParams::WantParams(const WantParams &wantParams)
{
    params_.clear();

    for (auto it : wantParams.params_) {
        params_.insert(make_pair(it.first, it.second));
    }
}
/**
 * @description: A WantParams used to
 *
 * @param intentParams  Indicates the existing IntentParams object.
 */
WantParams &WantParams::operator=(const WantParams &other)
{
    params_ = other.params_;
    return *this;
}
/**
 * @description: Sets a parameter in key-value pair format.
 * @param key Indicates the key matching the parameter.
 */
void WantParams::SetParam(const std::string &key, IInterface *value)
{
    params_[key] = value;
}

/**
 * @description: Obtains the parameter value based on a given key.
 * @param key Indicates the key matching the parameter.
 * @return Returns the value matching the given key.
 */
sptr<IInterface> WantParams::GetParam(const std::string &key) const
{
    auto it = params_.find(key);
    if (it == params_.cend()) {
        return nullptr;
    }
    return it->second;
}

/**
 * @description: Obtains the parameter value based on a given key.
 * @param key Indicates the key matching the parameter.
 * @return Returns the value matching the given key.
 */

const std::map<std::string, sptr<IInterface>> &WantParams::GetParams() const
{
    return params_;
}

/**
 * @description: Obtains a set of the keys of all parameters.
 * @param
 * @return Returns a set of keys.
 */
const std::set<std::string> WantParams::KeySet() const
{
    std::set<std::string> keySet;
    keySet.clear();

    for (auto it : params_) {
        keySet.emplace(it.first);
    }

    return keySet;
}

/**
 * @description: Removes the parameter matching the given key.
 * @param key Indicates the key matching the parameter to be removed.
 */
void WantParams::Remove(const std::string &key)
{
    params_.erase(key);
}

/**
 * @description: Checks whether the Intent contains the given key.
 * @param key Indicates the key to check.
 * @return Returns true if the Intent contains the key; returns false otherwise.
 */
bool WantParams::HasParam(const std::string &key) const
{
    return (params_.count(key) > 0);
}

/**
 * @description: Obtains the number of parameters contained in this IntentParams object.
 * @return Returns the number of parameters.
 */
int WantParams::Size() const
{
    return params_.size();
}

/**
 * @description: Checks whether this IntentParams object contains no parameters.
 * @return Returns true if this object does not contain any parameters; returns false otherwise.
 */
bool WantParams::IsEmpty() const
{
    return (params_.size() == 0);
}

bool WantParams::WriteToParcelString(Parcel &parcel, sptr<IInterface> &o) const
{
    std::string value = String::Unbox(IString::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_STRING)) {
        return false;
    }
    return parcel.WriteString16(Str8ToStr16(value));
}

bool WantParams::WriteToParcelBool(Parcel &parcel, sptr<IInterface> &o) const
{
    bool value = Boolean::Unbox(IBoolean::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_BOOLEAN)) {
        return false;
    }
    return parcel.WriteInt8(value);
}

bool WantParams::WriteToParcelByte(Parcel &parcel, sptr<IInterface> &o) const
{
    byte value = Byte::Unbox(IByte::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_BYTE)) {
        return false;
    }
    return parcel.WriteInt8(value);
}

bool WantParams::WriteToParcelChar(Parcel &parcel, sptr<IInterface> &o) const
{
    zchar value = Char::Unbox(IChar::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_CHAR)) {
        return false;
    }
    return parcel.WriteInt32(value);
}

bool WantParams::WriteToParcelShort(Parcel &parcel, sptr<IInterface> &o) const
{
    short value = Short::Unbox(IShort::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_SHORT)) {
        return false;
    }
    return parcel.WriteInt16(value);
}

bool WantParams::WriteToParcelInt(Parcel &parcel, sptr<IInterface> &o) const
{
    int value = Integer::Unbox(IInteger::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_INT)) {
        return false;
    }
    return parcel.WriteInt32(value);
}

bool WantParams::WriteToParcelLong(Parcel &parcel, sptr<IInterface> &o) const
{
    long value = Long::Unbox(ILong::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_LONG)) {
        return false;
    }
    return parcel.WriteInt64(value);
}

bool WantParams::WriteToParcelFloat(Parcel &parcel, sptr<IInterface> &o) const
{
    float value = Float::Unbox(IFloat::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_FLOAT)) {
        return false;
    }
    return parcel.WriteFloat(value);
}

bool WantParams::WriteToParcelDouble(Parcel &parcel, sptr<IInterface> &o) const
{
    double value = Double::Unbox(IDouble::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_DOUBLE)) {
        return false;
    }
    return parcel.WriteDouble(value);
}

bool WantParams::WriteMarshalling(Parcel &parcel, sptr<IInterface> &o) const
{
    if (IString::Query(o) != nullptr) {
        return WriteToParcelString(parcel, o);
    } else if (IBoolean::Query(o) != nullptr) {
        return WriteToParcelBool(parcel, o);
    } else if (IByte::Query(o) != nullptr) {
        return WriteToParcelByte(parcel, o);
    } else if (IChar::Query(o) != nullptr) {
        return WriteToParcelChar(parcel, o);
    } else if (IShort::Query(o) != nullptr) {
        return WriteToParcelShort(parcel, o);
    } else if (IInteger::Query(o) != nullptr) {
        return WriteToParcelInt(parcel, o);
    } else if (ILong::Query(o) != nullptr) {
        return WriteToParcelLong(parcel, o);
    } else if (IFloat::Query(o) != nullptr) {
        return WriteToParcelFloat(parcel, o);
    } else if (IDouble::Query(o) != nullptr) {
        return WriteToParcelDouble(parcel, o);
    } else {
        IArray *ao = IArray::Query(o);
        if (ao != nullptr) {
            sptr<IArray> array(ao);
            return WriteArrayToParcel(parcel, array);
        } else {
            return true;
        }
    }
}

/**
 * @description: Marshals an IntentParams object into a Parcel.
 * @param Key-value pairs in the IntentParams are marshalled separately.
 * @return If any key-value pair fails to be marshalled, false is returned.
 */
bool WantParams::Marshalling(Parcel &parcel) const
{
    size_t size = params_.size();
    if (!parcel.WriteInt32(size)) {
        return false;
    }

    auto iter = params_.cbegin();
    while (iter != params_.cend()) {
        std::string key = iter->first;
        sptr<IInterface> o = iter->second;
        if (!parcel.WriteString16(Str8ToStr16(key))) {
            return false;
        }

        if (!WriteMarshalling(parcel, o)) {
            return false;
        }

        iter++;
    }

    return true;
}

template <typename T1, typename T2>
static bool SetArray(const InterfaceID &id, const std::vector<T1> &value, sptr<IArray> &ao)
{
    typename std::vector<T1>::size_type size = value.size();
    ao = new (std::nothrow) Array(size, id);
    if (ao != nullptr) {
        for (typename std::vector<T1>::size_type i = 0; i < size; i++) {
            ao->Set(i, T2::Box(value[i]));
        }
        return true;
    }
    return false;
}

template <typename T1, typename T2, typename T3>
static void FillArray(IArray *ao, std::vector<T1> &array)
{
    auto func = [&](IInterface *object) { array.push_back(T2::Unbox(T3::Query(object))); };
    Array::ForEach(ao, func);
}

bool WantParams::WriteArrayToParcelString(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<std::u16string> array;
    auto func = [&](IInterface *object) {
        std::string s = String::Unbox(IString::Query(object));
        array.push_back(Str8ToStr16(s));
    };

    Array::ForEach(ao, func);

    if (!parcel.WriteInt32(VALUE_TYPE_STRINGARRAY)) {
        return false;
    }
    return parcel.WriteString16Vector(array);
}

bool WantParams::WriteArrayToParcelBool(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<int8_t> array;
    FillArray<int8_t, Boolean, IBoolean>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_BOOLEANARRAY)) {
        return false;
    }
    return parcel.WriteInt8Vector(array);
}

bool WantParams::WriteArrayToParcelByte(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<int8_t> array;
    FillArray<int8_t, Byte, IByte>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_BYTEARRAY)) {
        return false;
    }
    return parcel.WriteInt8Vector(array);
}

bool WantParams::WriteArrayToParcelChar(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<int32_t> array;
    FillArray<int32_t, Char, IChar>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_CHARARRAY)) {
        return false;
    }
    return parcel.WriteInt32Vector(array);
}

bool WantParams::WriteArrayToParcelShort(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<short> array;
    FillArray<short, Short, IShort>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_SHORTARRAY)) {
        return false;
    }
    return parcel.WriteInt16Vector(array);
}

bool WantParams::WriteArrayToParcelInt(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<int> array;
    FillArray<int, Integer, IInteger>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_INTARRAY)) {
        return false;
    }
    return parcel.WriteInt32Vector(array);
}

bool WantParams::WriteArrayToParcelLong(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<int64_t> array;
    FillArray<int64_t, Long, ILong>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_LONGARRAY)) {
        return false;
    }
    return parcel.WriteInt64Vector(array);
}

bool WantParams::WriteArrayToParcelFloat(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<float> array;
    FillArray<float, Float, IFloat>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_FLOATARRAY)) {
        return false;
    }
    return parcel.WriteFloatVector(array);
}

bool WantParams::WriteArrayToParcelDouble(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<double> array;
    FillArray<double, Double, IDouble>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_DOUBLEARRAY)) {
        return false;
    }
    return parcel.WriteDoubleVector(array);
}

bool WantParams::WriteArrayToParcel(Parcel &parcel, IArray *ao) const
{
    if (Array::IsStringArray(ao)) {
        return WriteArrayToParcelString(parcel, ao);
    } else if (Array::IsBooleanArray(ao)) {
        return WriteArrayToParcelBool(parcel, ao);
    } else if (Array::IsByteArray(ao)) {
        return WriteArrayToParcelByte(parcel, ao);
    } else if (Array::IsCharArray(ao)) {
        return WriteArrayToParcelChar(parcel, ao);
    } else if (Array::IsShortArray(ao)) {
        return WriteArrayToParcelShort(parcel, ao);
    } else if (Array::IsIntegerArray(ao)) {
        return WriteArrayToParcelInt(parcel, ao);
    } else if (Array::IsLongArray(ao)) {
        return WriteArrayToParcelLong(parcel, ao);
    } else if (Array::IsFloatArray(ao)) {
        return WriteArrayToParcelFloat(parcel, ao);
    } else if (Array::IsDoubleArray(ao)) {
        return WriteArrayToParcelDouble(parcel, ao);
    } else {
        return true;
    }
}

bool WantParams::ReadFromParcelArrayString(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<std::u16string> value;
    if (!parcel.ReadString16Vector(&value)) {
        return false;
    }

    std::vector<std::u16string>::size_type size = value.size();
    ao = new (std::nothrow) Array(size, g_IID_IString);
    if (ao != nullptr) {
        for (std::vector<std::u16string>::size_type i = 0; i < size; i++) {
            ao->Set(i, String::Box(Str16ToStr8(value[i])));
        }
        return true;
    }
    return false;
}

bool WantParams::ReadFromParcelArrayBool(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<int8_t> value;
    if (!parcel.ReadInt8Vector(&value)) {
        return false;
    }
    return SetArray<int8_t, Boolean>(g_IID_IBoolean, value, ao);
}

bool WantParams::ReadFromParcelArrayByte(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<int8_t> value;
    if (!parcel.ReadInt8Vector(&value)) {
        return false;
    }
    return SetArray<int8_t, Byte>(g_IID_IByte, value, ao);
}

bool WantParams::ReadFromParcelArrayChar(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<int32_t> value;
    if (!parcel.ReadInt32Vector(&value)) {
        return false;
    }
    return SetArray<int32_t, Char>(g_IID_IChar, value, ao);
}

bool WantParams::ReadFromParcelArrayShort(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<short> value;
    if (!parcel.ReadInt16Vector(&value)) {
        return false;
    }
    return SetArray<short, Short>(g_IID_IShort, value, ao);
}

bool WantParams::ReadFromParcelArrayInt(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<int> value;
    if (!parcel.ReadInt32Vector(&value)) {
        return false;
    }
    return SetArray<int, Integer>(g_IID_IInteger, value, ao);
}

bool WantParams::ReadFromParcelArrayLong(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<int64_t> value;
    if (!parcel.ReadInt64Vector(&value)) {
        return false;
    }
    return SetArray<int64_t, Long>(g_IID_ILong, value, ao);
}

bool WantParams::ReadFromParcelArrayFloat(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<float> value;
    if (!parcel.ReadFloatVector(&value)) {
        return false;
    }
    return SetArray<float, Float>(g_IID_IFloat, value, ao);
}

bool WantParams::ReadFromParcelArrayDouble(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<double> value;
    if (!parcel.ReadDoubleVector(&value)) {
        return false;
    }
    return SetArray<double, Double>(g_IID_IDouble, value, ao);
}

bool WantParams::ReadArrayToParcel(Parcel &parcel, int type, sptr<IArray> &ao)
{
    switch (type) {
        case VALUE_TYPE_STRINGARRAY:
            return ReadFromParcelArrayString(parcel, ao);
        case VALUE_TYPE_BOOLEANARRAY:
            return ReadFromParcelArrayBool(parcel, ao);
        case VALUE_TYPE_BYTEARRAY:
            return ReadFromParcelArrayByte(parcel, ao);
        case VALUE_TYPE_CHARARRAY:
            return ReadFromParcelArrayChar(parcel, ao);
        case VALUE_TYPE_SHORTARRAY:
            return ReadFromParcelArrayShort(parcel, ao);
        case VALUE_TYPE_INTARRAY:
            return ReadFromParcelArrayInt(parcel, ao);
        case VALUE_TYPE_LONGARRAY:
            return ReadFromParcelArrayLong(parcel, ao);
        case VALUE_TYPE_FLOATARRAY:
            return ReadFromParcelArrayFloat(parcel, ao);
        case VALUE_TYPE_DOUBLEARRAY:
            return ReadFromParcelArrayDouble(parcel, ao);
        default:
            // ignore
            ;
    }

    return true;
}

bool WantParams::ReadFromParcelString(Parcel &parcel, const std::string &key)
{
    std::u16string value = parcel.ReadString16();
    sptr<IInterface> intf = String::Box(Str16ToStr8(value));
    if (intf) {
        SetParam(key, intf);
    }
    return true;
}

bool WantParams::ReadFromParcelBool(Parcel &parcel, const std::string &key)
{
    int8_t value;
    if (parcel.ReadInt8(value)) {
        sptr<IInterface> intf = Boolean::Box(value);
        if (intf) {
            SetParam(key, intf);
        }
        return true;
    } else {
        return false;
    }
}

bool WantParams::ReadFromParcelInt8(Parcel &parcel, const std::string &key)
{
    int8_t value;
    if (parcel.ReadInt8(value)) {
        sptr<IInterface> intf = Byte::Box(value);
        if (intf) {
            SetParam(key, intf);
        }
        return true;
    } else {
        return false;
    }
}

bool WantParams::ReadFromParcelChar(Parcel &parcel, const std::string &key)
{
    int32_t value;
    if (parcel.ReadInt32(value)) {
        sptr<IInterface> intf = Char::Box(value);
        if (intf) {
            SetParam(key, intf);
        }
        return true;
    } else {
        return false;
    }
}

bool WantParams::ReadFromParcelShort(Parcel &parcel, const std::string &key)
{
    short value;
    if (parcel.ReadInt16(value)) {
        sptr<IInterface> intf = Short::Box(value);
        if (intf) {
            SetParam(key, intf);
        }
        return true;
    } else {
        return false;
    }
}

bool WantParams::ReadFromParcelInt(Parcel &parcel, const std::string &key)
{
    int value;
    if (parcel.ReadInt32(value)) {
        sptr<IInterface> intf = Integer::Box(value);
        if (intf) {
            SetParam(key, intf);
        }
        return true;
    } else {
        return false;
    }
}

bool WantParams::ReadFromParcelLong(Parcel &parcel, const std::string &key)
{
    int64_t value;
    if (parcel.ReadInt64(value)) {
        sptr<IInterface> intf = Long::Box(value);
        if (intf) {
            SetParam(key, intf);
        }
        return true;
    } else {
        return false;
    }
}

bool WantParams::ReadFromParcelFloat(Parcel &parcel, const std::string &key)
{
    float value;
    if (parcel.ReadFloat(value)) {
        sptr<IInterface> intf = Float::Box(value);
        if (intf) {
            SetParam(key, intf);
        }
        return true;
    } else {
        return false;
    }
}

bool WantParams::ReadFromParcelDouble(Parcel &parcel, const std::string &key)
{
    double value;
    if (parcel.ReadDouble(value)) {
        sptr<IInterface> intf = Double::Box(value);
        if (intf) {
            SetParam(key, intf);
        }
        return true;
    } else {
        return false;
    }
}

bool WantParams::ReadFromParcelParam(Parcel &parcel, const std::string &key, int type)
{
    switch (type) {
        case VALUE_TYPE_STRING:
            return ReadFromParcelString(parcel, key);
        case VALUE_TYPE_BOOLEAN:
            return ReadFromParcelBool(parcel, key);
        case VALUE_TYPE_BYTE:
            return ReadFromParcelInt8(parcel, key);
        case VALUE_TYPE_CHAR:
            return ReadFromParcelChar(parcel, key);
        case VALUE_TYPE_SHORT:
            return ReadFromParcelShort(parcel, key);
        case VALUE_TYPE_INT:
            return ReadFromParcelInt(parcel, key);
        case VALUE_TYPE_LONG:
            return ReadFromParcelLong(parcel, key);
        case VALUE_TYPE_FLOAT:
            return ReadFromParcelFloat(parcel, key);
        case VALUE_TYPE_DOUBLE:
            return ReadFromParcelDouble(parcel, key);
        case VALUE_TYPE_NULL:
            break;
        default: {
            // handle array
            sptr<IArray> ao = nullptr;
            if (!ReadArrayToParcel(parcel, type, ao)) {
                return false;
            }
            sptr<IInterface> intf = ao;
            if (intf) {
                SetParam(key, intf);
            }
            break;
        }
    }
    return true;
}

bool WantParams::ReadFromParcel(Parcel &parcel)
{
    int32_t size;

    if (!parcel.ReadInt32(size)) {
        return false;
    }

    for (int32_t i = 0; i < size; i++) {
        std::u16string key = parcel.ReadString16();
        int type;
        if (!parcel.ReadInt32(type)) {
            return false;
        }

        if (!ReadFromParcelParam(parcel, Str16ToStr8(key), type)) {
            return false;
        }
    }

    return true;
}

/**
 * @description: Unmarshals an IntentParams object from a Parcel.
 * @param Key-value pairs in the IntentParams are unmarshalled separately.
 * @return If any key-value pair fails to be unmarshalled, false is returned.
 */
WantParams *WantParams::Unmarshalling(Parcel &parcel)
{
    WantParams *wantParams = new (std::nothrow) WantParams();
    if (wantParams != nullptr && !wantParams->ReadFromParcel(parcel)) {
        delete wantParams;
        wantParams = nullptr;
    }
    return wantParams;
}
}  // namespace AAFwk
}  // namespace OHOS
