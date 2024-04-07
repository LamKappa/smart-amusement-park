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

#include "ohos/aafwk/content/intent_params.h"

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

#include "parcel.h"
#include "string_ex.h"

namespace OHOS {
namespace AAFwk {

void IntentParams::SetParam(const std::string &key, IInterface *value)
{
    params_[key] = value;
}

sptr<IInterface> IntentParams::GetParam(const std::string &key) const
{
    auto it = params_.find(key);
    if (it == params_.cend()) {
        return nullptr;
    }
    return it->second;
}

const std::map<std::string, sptr<IInterface>> &IntentParams::GetParams() const
{
    return params_;
}

bool IntentParams::HasParam(const std::string &key) const
{
    return params_.count(key) > 0;
}

template <typename T1, typename T2, typename T3>
static void FillArray(IArray *ao, std::vector<T1> &array)
{
    auto func = [&](IInterface *object) { array.push_back(T2::Unbox(T3::Query(object))); };
    Array::ForEach(ao, func);
}

bool IntentParams::WriteArrayToParcel(Parcel &parcel, IArray *ao) const
{
    if (Array::IsStringArray(ao)) {
        std::vector<std::u16string> array;
        auto func = [&](IInterface *object) {
            std::string s = String::Unbox(IString::Query(object));
            array.push_back(Str8ToStr16(s));
        };
        Array::ForEach(ao, func);
        if (!parcel.WriteInt32(VALUE_TYPE_STRINGARRAY)) {
            return false;
        }
        if (!parcel.WriteString16Vector(array)) {
            return false;
        }
    } else if (Array::IsBooleanArray(ao)) {
        std::vector<int8_t> array;
        FillArray<int8_t, Boolean, IBoolean>(ao, array);
        if (!parcel.WriteInt32(VALUE_TYPE_BOOLEANARRAY)) {
            return false;
        }
        if (!parcel.WriteInt8Vector(array)) {
            return false;
        }
    } else if (Array::IsByteArray(ao)) {
        std::vector<int8_t> array;
        FillArray<int8_t, Byte, IByte>(ao, array);
        if (!parcel.WriteInt32(VALUE_TYPE_BYTEARRAY)) {
            return false;
        }
        if (!parcel.WriteInt8Vector(array)) {
            return false;
        }
    } else if (Array::IsCharArray(ao)) {
        std::vector<int32_t> array;
        FillArray<int32_t, Char, IChar>(ao, array);
        if (!parcel.WriteInt32(VALUE_TYPE_CHARARRAY)) {
            return false;
        }
        if (!parcel.WriteInt32Vector(array)) {
            return false;
        }
    } else if (Array::IsShortArray(ao)) {
        std::vector<short> array;
        FillArray<short, Short, IShort>(ao, array);
        if (!parcel.WriteInt32(VALUE_TYPE_SHORTARRAY)) {
            return false;
        }
        if (!parcel.WriteInt16Vector(array)) {
            return false;
        }
    } else if (Array::IsIntegerArray(ao)) {
        std::vector<int> array;
        FillArray<int, Integer, IInteger>(ao, array);
        if (!parcel.WriteInt32(VALUE_TYPE_INTARRAY)) {
            return false;
        }
        if (!parcel.WriteInt32Vector(array)) {
            return false;
        }
    } else if (Array::IsLongArray(ao)) {
        std::vector<int64_t> array;
        FillArray<int64_t, Long, ILong>(ao, array);
        if (!parcel.WriteInt32(VALUE_TYPE_LONGARRAY)) {
            return false;
        }
        if (!parcel.WriteInt64Vector(array)) {
            return false;
        }
    } else if (Array::IsFloatArray(ao)) {
        std::vector<float> array;
        FillArray<float, Float, IFloat>(ao, array);
        if (!parcel.WriteInt32(VALUE_TYPE_FLOATARRAY)) {
            return false;
        }
        if (!parcel.WriteFloatVector(array)) {
            return false;
        }
    } else if (Array::IsDoubleArray(ao)) {
        std::vector<double> array;
        FillArray<double, Double, IDouble>(ao, array);
        if (!parcel.WriteInt32(VALUE_TYPE_DOUBLEARRAY)) {
            return false;
        }
        if (!parcel.WriteDoubleVector(array)) {
            return false;
        }
    }

    return true;
}

bool IntentParams::Marshalling(Parcel &parcel) const
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
        if (IString::Query(o) != nullptr) {
            std::string value = String::Unbox(IString::Query(o));
            if (!parcel.WriteInt32(VALUE_TYPE_STRING)) {
                return false;
            }
            if (!parcel.WriteString16(Str8ToStr16(value))) {
                return false;
            }
        } else if (IBoolean::Query(o) != nullptr) {
            bool value = Boolean::Unbox(IBoolean::Query(o));
            if (!parcel.WriteInt32(VALUE_TYPE_BOOLEAN)) {
                return false;
            }
            if (!parcel.WriteInt8(value)) {
                return false;
            }
        } else if (IByte::Query(o) != nullptr) {
            byte value = Byte::Unbox(IByte::Query(o));
            if (!parcel.WriteInt32(VALUE_TYPE_BYTE)) {
                return false;
            }
            if (!parcel.WriteInt8(value)) {
                return false;
            }
        } else if (IChar::Query(o) != nullptr) {
            zchar value = Char::Unbox(IChar::Query(o));
            if (!parcel.WriteInt32(VALUE_TYPE_CHAR)) {
                return false;
            }
            if (!parcel.WriteInt32(value)) {
                return false;
            }
        } else if (IShort::Query(o) != nullptr) {
            short value = Short::Unbox(IShort::Query(o));
            if (!parcel.WriteInt32(VALUE_TYPE_SHORT)) {
                return false;
            }
            if (!parcel.WriteInt16(value)) {
                return false;
            }
        } else if (IInteger::Query(o) != nullptr) {
            int value = Integer::Unbox(IInteger::Query(o));
            if (!parcel.WriteInt32(VALUE_TYPE_INT)) {
                return false;
            }
            if (!parcel.WriteInt32(value)) {
                return false;
            }
        } else if (ILong::Query(o) != nullptr) {
            long value = Long::Unbox(ILong::Query(o));
            if (!parcel.WriteInt32(VALUE_TYPE_LONG)) {
                return false;
            }
            if (!parcel.WriteInt64(value)) {
                return false;
            }
        } else if (IFloat::Query(o) != nullptr) {
            float value = Float::Unbox(IFloat::Query(o));
            if (!parcel.WriteInt32(VALUE_TYPE_FLOAT)) {
                return false;
            }
            if (!parcel.WriteFloat(value)) {
                return false;
            }
        } else if (IDouble::Query(o) != nullptr) {
            double value = Double::Unbox(IDouble::Query(o));
            if (!parcel.WriteInt32(VALUE_TYPE_DOUBLE)) {
                return false;
            }
            if (!parcel.WriteDouble(value)) {
                return false;
            }
        } else {
            IArray *ao = IArray::Query(o);
            if (ao != nullptr && !WriteArrayToParcel(parcel, ao)) {
                return false;
            }
        }
        iter++;
    }

    return true;
}

template <typename T1, typename T2>
static void SetArray(const InterfaceID &id, const std::vector<T1> &value, sptr<IArray> &ao)
{
    typename std::vector<T1>::size_type size = value.size();
    ao = new (std::nothrow) Array(size, id);
    for (typename std::vector<T1>::size_type i = 0; i < size; i++) {
        ao->Set(i, T2::Box(value[i]));
    }
}

bool IntentParams::ReadArrayToParcel(Parcel &parcel, int type, sptr<IArray> &ao)
{
    switch (type) {
        case VALUE_TYPE_STRINGARRAY: {
            std::vector<std::u16string> value;
            if (!parcel.ReadString16Vector(&value)) {
                return false;
            }
            std::vector<std::u16string>::size_type size = value.size();
            ao = new (std::nothrow) Array(size, g_IID_IString);
            for (std::vector<std::u16string>::size_type i = 0; i < size; i++) {
                ao->Set(i, String::Box(Str16ToStr8(value[i])));
            }
            break;
        }
        case VALUE_TYPE_BOOLEANARRAY: {
            std::vector<int8_t> value;
            if (!parcel.ReadInt8Vector(&value)) {
                return false;
            }
            SetArray<int8_t, Boolean>(g_IID_IBoolean, value, ao);
            break;
        }
        case VALUE_TYPE_BYTEARRAY: {
            std::vector<int8_t> value;
            if (!parcel.ReadInt8Vector(&value)) {
                return false;
            }
            SetArray<int8_t, Byte>(g_IID_IByte, value, ao);
            break;
        }
        case VALUE_TYPE_CHARARRAY: {
            std::vector<int32_t> value;
            if (!parcel.ReadInt32Vector(&value)) {
                return false;
            }
            SetArray<int32_t, Char>(g_IID_IChar, value, ao);
            break;
        }
        case VALUE_TYPE_SHORTARRAY: {
            std::vector<short> value;
            if (!parcel.ReadInt16Vector(&value)) {
                return false;
            }
            SetArray<short, Short>(g_IID_IShort, value, ao);
            break;
        }
        case VALUE_TYPE_INTARRAY: {
            std::vector<int> value;
            if (!parcel.ReadInt32Vector(&value)) {
                return false;
            }
            SetArray<int, Integer>(g_IID_IInteger, value, ao);
            break;
        }
        case VALUE_TYPE_LONGARRAY: {
            std::vector<int64_t> value;
            if (!parcel.ReadInt64Vector(&value)) {
                return false;
            }
            SetArray<int64_t, Long>(g_IID_ILong, value, ao);
            break;
        }
        case VALUE_TYPE_FLOATARRAY: {
            std::vector<float> value;
            if (!parcel.ReadFloatVector(&value)) {
                return false;
            }
            SetArray<float, Float>(g_IID_IFloat, value, ao);
            break;
        }
        case VALUE_TYPE_DOUBLEARRAY: {
            std::vector<double> value;
            if (!parcel.ReadDoubleVector(&value)) {
                return false;
            }
            SetArray<double, Double>(g_IID_IDouble, value, ao);
            break;
        }
        default:
            // ignore
            ;
    }

    return true;
}

bool IntentParams::ReadFromParcel(Parcel &parcel)
{
    int32_t size;

    if (!parcel.ReadInt32(size)) {
        return false;
    }

    for (int32_t i = 0; i < size; i++) {
        std::u16string key;
        key = parcel.ReadString16();

        sptr<IInterface> intf = nullptr;
        int type;
        if (!parcel.ReadInt32(type)) {
            return false;
        }

        switch (type) {
            case VALUE_TYPE_STRING: {
                std::u16string value = parcel.ReadString16();
                intf = String::Box(Str16ToStr8(value));
                break;
            }
            case VALUE_TYPE_BOOLEAN: {
                int8_t value;
                if (!parcel.ReadInt8(value)) {
                    return false;
                }
                intf = Boolean::Box(value);
                break;
            }
            case VALUE_TYPE_BYTE: {
                int8_t value;
                if (!parcel.ReadInt8(value)) {
                    return false;
                }
                intf = Byte::Box(value);
                break;
            }
            case VALUE_TYPE_CHAR: {
                int32_t value;
                if (!parcel.ReadInt32(value)) {
                    return false;
                }
                intf = Char::Box(value);
                break;
            }
            case VALUE_TYPE_SHORT: {
                short value;
                if (!parcel.ReadInt16(value)) {
                    return false;
                }
                intf = Short::Box(value);
                break;
            }
            case VALUE_TYPE_INT: {
                int value;
                if (!parcel.ReadInt32(value)) {
                    return false;
                }
                intf = Integer::Box(value);
                break;
            }
            case VALUE_TYPE_LONG: {
                int64_t value;
                if (!parcel.ReadInt64(value)) {
                    return false;
                }
                intf = Long::Box(value);
                break;
            }
            case VALUE_TYPE_FLOAT: {
                float value;
                if (!parcel.ReadFloat(value)) {
                    return false;
                }
                intf = Float::Box(value);
                break;
            }
            case VALUE_TYPE_DOUBLE: {
                double value;
                if (!parcel.ReadDouble(value)) {
                    return false;
                }
                intf = Double::Box(value);
                break;
            }
            case VALUE_TYPE_NULL:
                break;
            default: {
                // handle array
                sptr<IArray> ao = nullptr;
                if (!ReadArrayToParcel(parcel, type, ao)) {
                    return false;
                }
                intf = ao;
                break;
            }
        }

        if (intf) {
            SetParam(Str16ToStr8(key), intf);
        }
    }

    return true;
}

IntentParams *IntentParams::Unmarshalling(Parcel &parcel)
{
    IntentParams *intentParams = new (std::nothrow) IntentParams();
    if (intentParams && !intentParams->ReadFromParcel(parcel)) {
        delete intentParams;
        intentParams = nullptr;
    }
    return intentParams;
}

}  // namespace AAFwk
}  // namespace OHOS
