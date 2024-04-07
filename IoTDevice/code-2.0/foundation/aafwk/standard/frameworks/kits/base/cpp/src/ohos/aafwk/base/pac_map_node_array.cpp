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

#include <iostream>
#include "ohos/aafwk/base/pac_map_node_array.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
#define IS_STORED_ARRAY_DATA_TYPE(id, value)           \
    do {                                               \
        if (value.GetRefPtr() != nullptr) {            \
            return AAFwk::Array::Is##id##Array(value); \
        }                                              \
        return false;                                  \
    } while (0)

#define EQUALS_ARRAY_DATA(id, dataType, right)       \
    do {                                             \
        if (Is##id() && right->Is##id()) {           \
            std::vector<dataType> left_value;        \
            std::vector<dataType> right_value;       \
            Get##id##ValueArray(left_value);         \
            right->Get##id##ValueArray(right_value); \
            return (left_value == right_value);      \
        }                                            \
    } while (0)

#define READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(type, parcel, data) \
    do {                                                               \
        if (!(parcel).Read##type(data)) {                              \
            return false;                                              \
        }                                                              \
    } while (0)

#define WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(type, parcel, data) \
    do {                                                                \
        if (!(parcel).Write##type(data)) {                              \
            return false;                                               \
        }                                                               \
    } while (0)

#define INNER_DEEP_COPY(id, dataType, obj) \
    do {                                   \
        std::vector<dataType> array;       \
        obj->Get##id##ValueArray(array);   \
        Put##id##ValueArray(array);        \
    } while (0)

template <typename T1, typename T2>
static void PacMapSetArray(const AAFwk::InterfaceID &id, const std::vector<T1> &value, sptr<AAFwk::IArray> &ao)
{
    typename std::vector<T1>::size_type size = value.size();
    ao = new (std::nothrow) AAFwk::Array(size, id);
    if (ao != nullptr) {
        for (typename std::vector<T1>::size_type i = 0; i < size; i++) {
            ao->Set(i, T2::Box(value[i]));
        }
    }
}

template <typename T1, typename T2, typename T3>
static void PacMapGetArray(const sptr<AAFwk::IArray> &ao, std::vector<T1> &array)
{
    auto func = [&](AAFwk::IInterface *object) { array.emplace_back(T2::Unbox(T3::Query(object))); };
    AAFwk::Array::ForEach(ao.GetRefPtr(), func);
}

PacMapNodeTypeArray::PacMapNodeTypeArray(const PacMapNodeTypeArray &other) : PacMapNode(other)
{
    InnerDeepCopy(&other);
}

PacMapNodeTypeArray &PacMapNodeTypeArray::operator=(const PacMapNodeTypeArray &other)
{
    if (this != &other) {
        value_ = other.value_;
    }
    return *this;
}

/**
 * @brief Adds some short values to current object.
 * @param value Added list of data.
 */
void PacMapNodeTypeArray::PutShortValueArray(const std::vector<short> &value)
{
    PacMapSetArray<short, AAFwk::Short>(AAFwk::g_IID_IShort, value, value_);
}

/**
 * @brief Adds some int values to current object.
 * @param value Added list of data.
 */
void PacMapNodeTypeArray::PutIntegerValueArray(const std::vector<int> &value)
{
    PacMapSetArray<int, AAFwk::Integer>(AAFwk::g_IID_IInteger, value, value_);
}

/**
 * @brief Adds some long values to current object.
 * @param value Added list of data.
 */
void PacMapNodeTypeArray::PutLongValueArray(const std::vector<long> &value)
{
    PacMapSetArray<long, AAFwk::Long>(AAFwk::g_IID_ILong, value, value_);
}

/**
 * @brief Adds some boolean values to current object.
 * @param value Added list of data.
 */
void PacMapNodeTypeArray::PutBooleanValueArray(const std::vector<bool> &value)
{
    PacMapSetArray<bool, AAFwk::Boolean>(AAFwk::g_IID_IBoolean, value, value_);
}

/**
 * @brief Adds some char values to current object.
 * @param value Added list of data.
 */
void PacMapNodeTypeArray::PutCharValueArray(const std::vector<char> &value)
{
    PacMapSetArray<char, AAFwk::Byte>(AAFwk::g_IID_IByte, value, value_);
}

/**
 * @brief Adds some byte values to current object.
 * @param value Added list of data.
 */
void PacMapNodeTypeArray::PutByteValueArray(const std::vector<AAFwk::byte> &value)
{
    PacMapSetArray<AAFwk::byte, AAFwk::Byte>(AAFwk::g_IID_IByte, value, value_);
}

/**
 * @brief Adds some float values to current object.
 * @param value Added list of data.
 */
void PacMapNodeTypeArray::PutFloatValueArray(const std::vector<float> &value)
{
    PacMapSetArray<float, AAFwk::Float>(AAFwk::g_IID_IFloat, value, value_);
}

/**
 * @brief Adds some double values to current object.
 * @param value Added list of data.
 */
void PacMapNodeTypeArray::PutDoubleValueArray(const std::vector<double> &value)
{
    PacMapSetArray<double, AAFwk::Double>(AAFwk::g_IID_IDouble, value, value_);
}

/**
 * @brief Adds some string {std::string} values to current object.
 * @param value Added list of data.
 */
void PacMapNodeTypeArray::PutStringValueArray(const std::vector<std::string> &value)
{
    PacMapSetArray<std::string, AAFwk::String>(AAFwk::g_IID_IString, value, value_);
}

/**
 * @brief Obtains some short values.
 * @param value Save the returned short values.
 */
void PacMapNodeTypeArray::GetShortValueArray(std::vector<short> &value)
{
    PacMapGetArray<short, AAFwk::Short, AAFwk::IShort>(value_, value);
}

/**
 * @brief Obtains some int values.
 * @param value Save the returned int values.
 */
void PacMapNodeTypeArray::GetIntegerValueArray(std::vector<int> &value)
{
    PacMapGetArray<int, AAFwk::Integer, AAFwk::IInteger>(value_, value);
}

/**
 * @brief Obtains some long values.
 * @param value Save the returned long values.
 */
void PacMapNodeTypeArray::GetLongValueArray(std::vector<long> &value)
{
    PacMapGetArray<long, AAFwk::Long, AAFwk::ILong>(value_, value);
}

/**
 * @brief Obtains some boolean values.
 * @param value Save the returned boolean values.
 */
void PacMapNodeTypeArray::GetBooleanValueArray(std::vector<bool> &value)
{
    PacMapGetArray<bool, AAFwk::Boolean, AAFwk::IBoolean>(value_, value);
}

/**
 * @brief Obtains some char values.
 * @param value Save the returned char values.
 */
void PacMapNodeTypeArray::GetCharValueArray(std::vector<char> &value)
{
    PacMapGetArray<char, AAFwk::Byte, AAFwk::IByte>(value_, value);
}

/**
 * @brief Obtains some byte values.
 * @param value Save the returned byte values.
 */
void PacMapNodeTypeArray::GetByteValueArray(std::vector<AAFwk::byte> &value)
{
    PacMapGetArray<AAFwk::byte, AAFwk::Byte, AAFwk::IByte>(value_, value);
}

/**
 * @brief Obtains some float values.
 * @param value Save the returned float values.
 */
void PacMapNodeTypeArray::GetFloatValueArray(std::vector<float> &value)
{
    PacMapGetArray<float, AAFwk::Float, AAFwk::IFloat>(value_, value);
}

/**
 * @brief Obtains some double values.
 * @param value Save the returned double values.
 */
void PacMapNodeTypeArray::GetDoubleValueArray(std::vector<double> &value)
{
    PacMapGetArray<double, AAFwk::Double, AAFwk::IDouble>(value_, value);
}

/**
 * @brief Obtains some std::string values.
 * @param value Save the returned std::string values.
 */
void PacMapNodeTypeArray::GetStringValueArray(std::vector<std::string> &value)
{
    PacMapGetArray<std::string, AAFwk::String, AAFwk::IString>(value_, value);
}

/**
 * @brief Indicates whether some other object is "equal to" this one.
 * @param other The object with which to compare.
 * @return true if this object is the same as the obj argument; false otherwise.
 */
bool PacMapNodeTypeArray::Equals(const PacMapNode *other)
{
    if (other == nullptr) {
        return false;
    }

    PacMapNode *pnode = const_cast<PacMapNode *>(other);
    PacMapNodeTypeArray *other_ = static_cast<PacMapNodeTypeArray *>(pnode);
    if (other_ == nullptr) {
        return false;
    }

    if (value_.GetRefPtr() == other_->value_.GetRefPtr()) {
        return true;
    }

    if (value_.GetRefPtr() == nullptr || other_->value_.GetRefPtr() == nullptr) {
        return false;
    }

    EQUALS_ARRAY_DATA(Short, short, other_);
    EQUALS_ARRAY_DATA(Integer, int, other_);
    EQUALS_ARRAY_DATA(Long, long, other_);
    EQUALS_ARRAY_DATA(Char, char, other_);
    EQUALS_ARRAY_DATA(Byte, AAFwk::byte, other_);
    EQUALS_ARRAY_DATA(Boolean, bool, other_);
    EQUALS_ARRAY_DATA(Float, float, other_);
    EQUALS_ARRAY_DATA(Double, double, other_);
    EQUALS_ARRAY_DATA(String, std::string, other_);

    return false;
}

/**
 * @brief Copy the data of the specified object to the current object with deepcopy
 * @param other The original object that stores the data.
 */
void PacMapNodeTypeArray::DeepCopy(const PacMapNode *other)
{
    PacMapNodeTypeArray *array_object = (PacMapNodeTypeArray *)other;
    InnerDeepCopy(array_object);
}

void PacMapNodeTypeArray::InnerDeepCopy(const PacMapNodeTypeArray *other)
{
    if (other == nullptr) {
        return;
    }

    PacMapNodeTypeArray *pother = const_cast<PacMapNodeTypeArray *>(other);
    if (AAFwk::Array::IsStringArray(pother->value_)) {
        INNER_DEEP_COPY(String, std::string, pother);
    } else if (AAFwk::Array::IsBooleanArray(pother->value_)) {
        INNER_DEEP_COPY(Boolean, bool, pother);
    } else if (AAFwk::Array::IsByteArray(pother->value_)) {
        INNER_DEEP_COPY(Byte, AAFwk::byte, pother);
    } else if (AAFwk::Array::IsShortArray(pother->value_)) {
        INNER_DEEP_COPY(Short, short, pother);
    } else if (AAFwk::Array::IsIntegerArray(pother->value_)) {
        INNER_DEEP_COPY(Integer, int, pother);
    } else if (AAFwk::Array::IsLongArray(pother->value_)) {
        INNER_DEEP_COPY(Long, long, pother);
    } else if (AAFwk::Array::IsFloatArray(pother->value_)) {
        INNER_DEEP_COPY(Float, float, pother);
    } else if (AAFwk::Array::IsDoubleArray(pother->value_)) {
        INNER_DEEP_COPY(Double, double, pother);
    } else {
        return;
    }
}

/**
 * @brief Whether the stored data is of short type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeArray::IsShort(void)
{
    IS_STORED_ARRAY_DATA_TYPE(Short, value_);
}

/**
 * @brief Whether the stored data is of integer type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeArray::IsInteger(void)
{
    IS_STORED_ARRAY_DATA_TYPE(Integer, value_);
}

/**
 * @brief Whether the stored data is of long type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeArray::IsLong(void)
{
    IS_STORED_ARRAY_DATA_TYPE(Long, value_);
}

/**
 * @brief Whether the stored data is of char type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeArray::IsChar(void)
{
    return IsByte();
}

/**
 * @brief Whether the stored data is of byte type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeArray::IsByte(void)
{
    IS_STORED_ARRAY_DATA_TYPE(Byte, value_);
}

/**
 * @brief Whether the stored data is of boolean type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeArray::IsBoolean(void)
{
    IS_STORED_ARRAY_DATA_TYPE(Boolean, value_);
}

/**
 * @brief Whether the stored data is of float type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeArray::IsFloat(void)
{
    IS_STORED_ARRAY_DATA_TYPE(Float, value_);
}

/**
 * @brief Whether the stored data is of double type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeArray::IsDouble(void)
{
    IS_STORED_ARRAY_DATA_TYPE(Double, value_);
}

/**
 * @brief Whether the stored data is of string type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeArray::IsString(void)
{
    IS_STORED_ARRAY_DATA_TYPE(String, value_);
}

bool PacMapNodeTypeArray::MarshallingArrayString(Parcel &parcel) const
{
    std::vector<std::u16string> array;
    auto func = [&](AAFwk::IInterface *object) {
        std::string s = AAFwk::String::Unbox(AAFwk::IString::Query(object));
        array.push_back(Str8ToStr16(s));
    };
    AAFwk::Array::ForEach(value_, func);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int32, parcel, PACMAP_DATA_ARRAY_STRING);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(String16Vector, parcel, array);
    return true;
}

bool PacMapNodeTypeArray::MarshallingArrayBoolean(Parcel &parcel) const
{
    std::vector<int8_t> array;
    PacMapGetArray<int8_t, AAFwk::Boolean, AAFwk::IBoolean>(value_, array);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int32, parcel, PACMAP_DATA_ARRAY_BOOLEAN);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int8Vector, parcel, array);
    return true;
}

bool PacMapNodeTypeArray::MarshallingArrayByte(Parcel &parcel) const
{
    std::vector<int8_t> array;
    PacMapGetArray<int8_t, AAFwk::Byte, AAFwk::IByte>(value_, array);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int32, parcel, PACMAP_DATA_ARRAY_BYTE);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int8Vector, parcel, array);
    return true;
}

bool PacMapNodeTypeArray::MarshallingArrayShort(Parcel &parcel) const
{
    std::vector<short> array;
    PacMapGetArray<short, AAFwk::Short, AAFwk::IShort>(value_, array);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int32, parcel, PACMAP_DATA_ARRAY_SHORT);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int16Vector, parcel, array);
    return true;
}

bool PacMapNodeTypeArray::MarshallingArrayInteger(Parcel &parcel) const
{
    std::vector<int> array;
    PacMapGetArray<int, AAFwk::Integer, AAFwk::IInteger>(value_, array);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int32, parcel, PACMAP_DATA_ARRAY_INTEGER);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int32Vector, parcel, array);
    return true;
}

bool PacMapNodeTypeArray::MarshallingArrayLong(Parcel &parcel) const
{
    std::vector<long> array;
    PacMapGetArray<long, AAFwk::Long, AAFwk::ILong>(value_, array);
    return WriteLongVector(parcel, array);
}

bool PacMapNodeTypeArray::MarshallingArrayFloat(Parcel &parcel) const
{
    std::vector<float> array;
    PacMapGetArray<float, AAFwk::Float, AAFwk::IFloat>(value_, array);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int32, parcel, PACMAP_DATA_ARRAY_FLOAT);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(FloatVector, parcel, array);
    return true;
}

bool PacMapNodeTypeArray::MarshallingArrayDouble(Parcel &parcel) const
{
    std::vector<double> array;
    PacMapGetArray<double, AAFwk::Double, AAFwk::IDouble>(value_, array);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int32, parcel, PACMAP_DATA_ARRAY_DOUBLE);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(DoubleVector, parcel, array);
    return true;
}

/**
 * @brief Marshals this Sequenceable object to a Parcel.
 * @param key Indicates the key in String format.
 * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 * @return Marshals success returns true, otherwise returns false.
 */
bool PacMapNodeTypeArray::Marshalling(const std::string &key, Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(String16, parcel, Str8ToStr16(key));

    if (AAFwk::Array::IsStringArray(value_)) {
        return MarshallingArrayString(parcel);
    } else if (AAFwk::Array::IsBooleanArray(value_)) {
        return MarshallingArrayBoolean(parcel);
    } else if (AAFwk::Array::IsByteArray(value_)) {
        return MarshallingArrayByte(parcel);
    } else if (AAFwk::Array::IsShortArray(value_)) {
        return MarshallingArrayShort(parcel);
    } else if (AAFwk::Array::IsIntegerArray(value_)) {
        return MarshallingArrayInteger(parcel);
    } else if (AAFwk::Array::IsLongArray(value_)) {
        return MarshallingArrayLong(parcel);
    } else if (AAFwk::Array::IsFloatArray(value_)) {
        return MarshallingArrayFloat(parcel);
    } else if (AAFwk::Array::IsDoubleArray(value_)) {
        return MarshallingArrayDouble(parcel);
    } else {
        return false;
    }
}

bool PacMapNodeTypeArray::UnmarshallingArrayShort(Parcel &parcel)
{
    std::vector<short> value;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int16Vector, parcel, &value);
    PutShortValueArray(value);
    return true;
}

bool PacMapNodeTypeArray::UnmarshallingArrayInteger(Parcel &parcel)
{
    std::vector<int> value;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int32Vector, parcel, &value);
    PutIntegerValueArray(value);
    return true;
}

bool PacMapNodeTypeArray::UnmarshallingArrayLong(Parcel &parcel)
{
    std::vector<long> value;
    if (!ReadLongVector(parcel, value)) {
        return false;
    }
    PutLongValueArray(value);
    return true;
}

bool PacMapNodeTypeArray::UnmarshallingArrayByte(Parcel &parcel)
{
    std::vector<int8_t> value;
    std::vector<AAFwk::byte> byteValue;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int8Vector, parcel, &value);

    for (int i = 0; i < (int)value.size(); i++) {
        byteValue.emplace_back(value[i]);
    }
    PutByteValueArray(byteValue);
    return true;
}

bool PacMapNodeTypeArray::UnmarshallingArrayBoolean(Parcel &parcel)
{
    std::vector<int8_t> value;
    std::vector<bool> boolValue;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int8Vector, parcel, &value);

    for (int i = 0; i < (int)value.size(); i++) {
        boolValue.emplace_back((value[i] == 0) ? false : true);
    }
    PutBooleanValueArray(boolValue);
    return true;
}

bool PacMapNodeTypeArray::UnmarshallingArrayFloat(Parcel &parcel)
{
    std::vector<float> value;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(FloatVector, parcel, &value);
    PutFloatValueArray(value);
    return true;
}

bool PacMapNodeTypeArray::UnmarshallingArrayDouble(Parcel &parcel)
{
    std::vector<double> value;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(DoubleVector, parcel, &value);
    PutDoubleValueArray(value);
    return true;
}

bool PacMapNodeTypeArray::UnmarshallingArrayString(Parcel &parcel)
{
    std::vector<std::u16string> value;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(String16Vector, parcel, &value);

    std::vector<std::u16string>::size_type size = value.size();
    value_ = new (std::nothrow) AAFwk::Array(size, AAFwk::g_IID_IString);
    if (value_ != nullptr) {
        for (std::vector<std::u16string>::size_type i = 0; i < size; i++) {
            value_->Set(i, AAFwk::String::Box(Str16ToStr8(value[i])));
        }
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Unmarshals this Sequenceable object from a Parcel.
 * @param dataType Indicates the type of data stored.
 * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 * @return Unmarshals success returns true, otherwise returns false.
 */
bool PacMapNodeTypeArray::Unmarshalling(int32_t dataType, Parcel &parcel)
{
    switch (dataType) {
        case PACMAP_DATA_ARRAY_SHORT:
            return UnmarshallingArrayShort(parcel);
        case PACMAP_DATA_ARRAY_INTEGER:
            return UnmarshallingArrayInteger(parcel);
        case PACMAP_DATA_ARRAY_LONG:
            return UnmarshallingArrayLong(parcel);
        case PACMAP_DATA_ARRAY_CHAR:
            [[clang::fallthrough]];
        case PACMAP_DATA_ARRAY_BYTE:
            return UnmarshallingArrayByte(parcel);
        case PACMAP_DATA_ARRAY_BOOLEAN:
            return UnmarshallingArrayBoolean(parcel);
        case PACMAP_DATA_ARRAY_FLOAT:
            return UnmarshallingArrayFloat(parcel);
        case PACMAP_DATA_ARRAY_DOUBLE:
            return UnmarshallingArrayDouble(parcel);
        case PACMAP_DATA_ARRAY_STRING:
            return UnmarshallingArrayString(parcel);
        default:
            return false;
    }
    return true;
}

bool PacMapNodeTypeArray::WriteLongVector(Parcel &parcel, const std::vector<long> &value) const
{
    if (value.size() > INT_MAX) {
        return false;
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int32, parcel, PACMAP_DATA_ARRAY_LONG);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int32, parcel, value.size());
    for (int i = 0; i < (int)value.size(); i++) {
        long value_ = value[i];
        if (!parcel.WriteBuffer((void *)&value_, sizeof(long))) {
            return false;
        }
    }
    return true;
}

bool PacMapNodeTypeArray::ReadLongVector(Parcel &parcel, std::vector<long> &value)
{
    int count = 0;
    value.clear();
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_ARRAY(Int32, parcel, count);

    long *pvalue_ = nullptr;
    for (auto i = 0; i < count; i++) {
        const uint8_t *pdata = parcel.ReadBuffer(sizeof(long));
        if (pdata != nullptr) {
            pvalue_ = (long *)pdata;
            value.emplace_back(*pvalue_);
        } else {
            return false;
        }
    }
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
