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

#include "ohos/aafwk/base/pac_map_node_base.h"
#include <iostream>
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
#define EQUALS_BASE_DATA(id, right)                             \
    do {                                                        \
        if (Is##id() && right->Is##id()) {                      \
            return Get##id##Value() == right->Get##id##Value(); \
        }                                                       \
    } while (0)

#define READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(type, parcel, data) \
    do {                                                              \
        if (!(parcel).Read##type(data)) {                             \
            return false;                                             \
        }                                                             \
    } while (0)

#define WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(type, parcel, data) \
    do {                                                               \
        if (!(parcel).Write##type(data)) {                             \
            return false;                                              \
        }                                                              \
    } while (0)

template <typename T1, typename T2, typename T3>
static T1 PacMapGetBaseValue(sptr<AAFwk::IInterface> &object, T1 defaultValue)
{
    T2 *pv = T2::Query(object);
    if (pv != nullptr) {
        return T3::Unbox(pv);
    } else {
        return defaultValue;
    }
}

PacMapNodeTypeBase::PacMapNodeTypeBase(const PacMapNodeTypeBase &other) : PacMapNode(other)
{
    InnerDeepCopy(&other);
}

PacMapNodeTypeBase &PacMapNodeTypeBase::operator=(const PacMapNodeTypeBase &other)
{
    if (this != &other) {
        value_ = other.value_;
    }
    return *this;
}

/**
 * @brief Adds a short value to current object.
 * @param value Added data.
 */
void PacMapNodeTypeBase::PutShortValue(short value)
{
    value_ = AAFwk::Short::Box(value);
}

/**
 * @brief Adds a int value to current object.
 * @param value Added data.
 */
void PacMapNodeTypeBase::PutIntValue(int value)
{
    value_ = AAFwk::Integer::Box(value);
}

/**
 * @brief Adds a long value to current object.
 * @param value Added data.
 */
void PacMapNodeTypeBase::PutLongValue(long value)
{
    value_ = AAFwk::Long::Box(value);
}

/**
 * @brief Adds a boolean value to current object.
 * @param value Added data.
 */
void PacMapNodeTypeBase::PutBooleanValue(bool value)
{
    value_ = AAFwk::Boolean::Box(value);
}

/**
 * @brief Adds a char value to current object.
 * @param value Added data.
 */
void PacMapNodeTypeBase::PutCharValue(char value)
{
    value_ = AAFwk::Byte::Box(value);
}

/**
 * @brief Adds a byte value to current object.
 * @param value Added data.
 */
void PacMapNodeTypeBase::PutByteValue(AAFwk::byte value)
{
    value_ = AAFwk::Byte::Box(value);
}

/**
 * @brief Adds a float value to current object.
 * @param value Added data.
 */
void PacMapNodeTypeBase::PutFloatValue(float value)
{
    value_ = AAFwk::Float::Box(value);
}

/**
 * @brief Adds a double value to current object.
 * @param value Added data.
 */
void PacMapNodeTypeBase::PutDoubleValue(double value)
{
    value_ = AAFwk::Double::Box(value);
}

/**
 * @brief Adds a string {std::string} value to current object.
 * @param value Added data.
 */
void PacMapNodeTypeBase::PutStringValue(const std::string &value)
{
    value_ = AAFwk::String::Box(value);
}

/**
 * @brief Obtains the short value.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the short value, otherwise return the @a defaultValue.
 */
short PacMapNodeTypeBase::GetShortValue(short defaultValue)
{
    return PacMapGetBaseValue<short, AAFwk::IShort, AAFwk::Short>(value_, defaultValue);
}

/**
 * @brief Obtains the int value.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the int value, otherwise return the @a defaultValue.
 */
int PacMapNodeTypeBase::GetIntegerValue(int defaultValue)
{
    return PacMapGetBaseValue<int, AAFwk::IInteger, AAFwk::Integer>(value_, defaultValue);
}

/**
 * @brief Obtains the long value.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the long value, otherwise return the @a defaultValue.
 */
long PacMapNodeTypeBase::GetLongValue(long defaultValue)
{
    return PacMapGetBaseValue<long, AAFwk::ILong, AAFwk::Long>(value_, defaultValue);
}

/**
 * @brief Obtains the boolean value.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the boolean value, otherwise return the @a defaultValue.
 */
bool PacMapNodeTypeBase::GetBooleanValue(bool defaultValue)
{
    return PacMapGetBaseValue<bool, AAFwk::IBoolean, AAFwk::Boolean>(value_, defaultValue);
}

/**
 * @brief Obtains the char value.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the char value, otherwise return the @a defaultValue.
 */
char PacMapNodeTypeBase::GetCharValue(char defaultValue)
{
    return PacMapGetBaseValue<char, AAFwk::IByte, AAFwk::Byte>(value_, defaultValue);
}

/**
 * @brief Obtains the byte value.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the byte value, otherwise return the @a defaultValue.
 */
AAFwk::byte PacMapNodeTypeBase::GetByteValue(AAFwk::byte defaultValue)
{
    return PacMapGetBaseValue<AAFwk::byte, AAFwk::IByte, AAFwk::Byte>(value_, defaultValue);
}

/**
 * @brief Obtains the float value.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the float value, otherwise return the @a defaultValue.
 */
float PacMapNodeTypeBase::GetFloatValue(float defaultValue)
{
    return PacMapGetBaseValue<float, AAFwk::IFloat, AAFwk::Float>(value_, defaultValue);
}

/**
 * @brief Obtains the double value.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the double value, otherwise return the @a defaultValue.
 */
double PacMapNodeTypeBase::GetDoubleValue(double defaultValue)
{
    return PacMapGetBaseValue<double, AAFwk::IDouble, AAFwk::Double>(value_, defaultValue);
}

/**
 * @brief Obtains the string {std::string} value.
 * @param defaultValue The return value when the function fails.
 * @return If the match is successful, return the std::string value, otherwise return the @a defaultValue.
 */
std::string PacMapNodeTypeBase::GetStringValue(const std::string &defaultValue)
{
    return PacMapGetBaseValue<std::string, AAFwk::IString, AAFwk::String>(value_, defaultValue);
}

/**
 * @brief Indicates whether some other object is "equal to" this one.
 * @param other The object with which to compare.
 * @return true if this object is the same as the obj argument; false otherwise.
 */
bool PacMapNodeTypeBase::Equals(const PacMapNode *other)
{
    if (other == nullptr) {
        return false;
    }

    PacMapNode *pnode = const_cast<PacMapNode *>(other);
    PacMapNodeTypeBase *other_ = static_cast<PacMapNodeTypeBase *>(pnode);
    if (other_ == nullptr) {
        return false;
    }

    if (value_.GetRefPtr() == other_->value_.GetRefPtr()) {
        return true;
    }

    if (value_.GetRefPtr() == nullptr || other_->value_.GetRefPtr() == nullptr) {
        return false;
    }

    EQUALS_BASE_DATA(Short, other_);
    EQUALS_BASE_DATA(Integer, other_);
    EQUALS_BASE_DATA(Long, other_);
    EQUALS_BASE_DATA(Byte, other_);
    EQUALS_BASE_DATA(Char, other_);
    EQUALS_BASE_DATA(Boolean, other_);
    EQUALS_BASE_DATA(Float, other_);
    EQUALS_BASE_DATA(Double, other_);
    EQUALS_BASE_DATA(String, other_);

    return false;
}

/**
 * @brief Copy the data of the specified object to the current object with deepcopy
 * @param other The original object that stores the data.
 */
void PacMapNodeTypeBase::DeepCopy(const PacMapNode *other)
{
    PacMapNodeTypeBase *base_object = (PacMapNodeTypeBase *)other;
    InnerDeepCopy(base_object);
}

void PacMapNodeTypeBase::InnerDeepCopy(const PacMapNodeTypeBase *other)
{
    PacMapNodeTypeBase *other_ = const_cast<PacMapNodeTypeBase *>(other);

    if (AAFwk::IString::Query(other_->value_) != nullptr) {
        std::string value = AAFwk::String::Unbox(AAFwk::IString::Query(other_->value_));
        PutStringValue(value);
    } else if (AAFwk::IBoolean::Query(other_->value_) != nullptr) {
        bool value = AAFwk::Boolean::Unbox(AAFwk::IBoolean::Query(other_->value_));
        PutBooleanValue(value);
    } else if (AAFwk::IByte::Query(other_->value_) != nullptr) {
        AAFwk::byte value = AAFwk::Byte::Unbox(AAFwk::IByte::Query(other_->value_));
        PutByteValue(value);
    } else if (AAFwk::IShort::Query(other_->value_) != nullptr) {
        short value = AAFwk::Short::Unbox(AAFwk::IShort::Query(other_->value_));
        PutShortValue(value);
    } else if (AAFwk::IInteger::Query(other_->value_) != nullptr) {
        int value = AAFwk::Integer::Unbox(AAFwk::IInteger::Query(other_->value_));
        PutIntValue(value);
    } else if (AAFwk::ILong::Query(other_->value_) != nullptr) {
        long value = AAFwk::Long::Unbox(AAFwk::ILong::Query(other_->value_));
        PutLongValue(value);
    } else if (AAFwk::IFloat::Query(other_->value_) != nullptr) {
        float value = AAFwk::Float::Unbox(AAFwk::IFloat::Query(other_->value_));
        PutFloatValue(value);
    } else if (AAFwk::IDouble::Query(other_->value_) != nullptr) {
        double value = AAFwk::Double::Unbox(AAFwk::IDouble::Query(other_->value_));
        PutDoubleValue(value);
    }
}

/**
 * @brief Whether the stored data is of short type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeBase::IsShort(void)
{
    if (value_.GetRefPtr() != nullptr) {
        return AAFwk::IShort::Query(value_) != nullptr;
    }
    return false;
}

/**
 * @brief Whether the stored data is of integer type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeBase::IsInteger(void)
{
    if (value_.GetRefPtr() != nullptr) {
        return AAFwk::IInteger::Query(value_) != nullptr;
    }
    return false;
}

/**
 * @brief Whether the stored data is of long type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeBase::IsLong(void)
{
    if (value_.GetRefPtr() != nullptr) {
        return AAFwk::ILong::Query(value_) != nullptr;
    }
    return false;
}

/**
 * @brief Whether the stored data is of char type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeBase::IsChar(void)
{
    return IsByte();
}

/**
 * @brief Whether the stored data is of byte type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeBase::IsByte(void)
{
    if (value_.GetRefPtr() != nullptr) {
        return AAFwk::IByte::Query(value_) != nullptr;
    }
    return false;
}

/**
 * @brief Whether the stored data is of boolean type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeBase::IsBoolean(void)
{
    if (value_.GetRefPtr() != nullptr) {
        return AAFwk::IBoolean::Query(value_) != nullptr;
    }
    return false;
}

/**
 * @brief Whether the stored data is of float type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeBase::IsFloat(void)
{
    if (value_.GetRefPtr() != nullptr) {
        return AAFwk::IFloat::Query(value_) != nullptr;
    }
    return false;
}

/**
 * @brief Whether the stored data is of double type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeBase::IsDouble(void)
{
    if (value_.GetRefPtr() != nullptr) {
        return AAFwk::IDouble::Query(value_) != nullptr;
    }
    return false;
}

/**
 * @brief Whether the stored data is of string type.
 * @return If yes return true, otherwise return false.
 */
bool PacMapNodeTypeBase::IsString(void)
{
    if (value_.GetRefPtr() != nullptr) {
        return AAFwk::IString::Query(value_) != nullptr;
    }
    return false;
}

/**
 * @brief Marshals this Sequenceable object to a Parcel.
 * @param key Indicates the key in String format.
 * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 * @return Marshals success returns true, otherwise returns false.
 */
bool PacMapNodeTypeBase::Marshalling(const std::string &key, Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(String16, parcel, Str8ToStr16(key));

    if (AAFwk::IShort::Query(value_) != nullptr) {
        short value = AAFwk::Short::Unbox(AAFwk::IShort::Query(value_));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int32, parcel, PACMAP_DATA_SHORT);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Uint16, parcel, value);
    } else if (AAFwk::IInteger::Query(value_) != nullptr) {
        int value = AAFwk::Integer::Unbox(AAFwk::IInteger::Query(value_));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int32, parcel, PACMAP_DATA_INTEGER);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int32, parcel, value);
    } else if (AAFwk::ILong::Query(value_) != nullptr) {
        long value = AAFwk::Long::Unbox(AAFwk::ILong::Query(value_));
        if (!WriteLong(parcel, value)) {
            return false;
        }
    } else if (AAFwk::IFloat::Query(value_) != nullptr) {
        float value = AAFwk::Float::Unbox(AAFwk::IFloat::Query(value_));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int32, parcel, PACMAP_DATA_FLOAT);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Float, parcel, value);
    } else if (AAFwk::IDouble::Query(value_) != nullptr) {
        double value = AAFwk::Double::Unbox(AAFwk::IDouble::Query(value_));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int32, parcel, PACMAP_DATA_DOUBLE);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Double, parcel, value);
    } else if (AAFwk::IString::Query(value_) != nullptr) {
        std::string value = AAFwk::String::Unbox(AAFwk::IString::Query(value_));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int32, parcel, PACMAP_DATA_STRING);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(String16, parcel, Str8ToStr16(value));
    } else if (AAFwk::IBoolean::Query(value_) != nullptr) {
        bool value = AAFwk::Boolean::Unbox(AAFwk::IBoolean::Query(value_));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int32, parcel, PACMAP_DATA_BOOLEAN);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int8, parcel, value);
    } else if (AAFwk::IByte::Query(value_) != nullptr) {
        AAFwk::byte value = AAFwk::Byte::Unbox(AAFwk::IByte::Query(value_));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int32, parcel, PACMAP_DATA_BYTE);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int8, parcel, value);
    }
    return true;
}

bool PacMapNodeTypeBase::ReadFromParcelShort(Parcel &parcel)
{
    short value;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int16, parcel, value);
    value_ = AAFwk::Short::Box(value);
    return true;
}

bool PacMapNodeTypeBase::ReadFromParcelInt(Parcel &parcel)
{
    int value;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int32, parcel, value);
    value_ = AAFwk::Integer::Box(value);
    return true;
}

bool PacMapNodeTypeBase::ReadFromParcelLong(Parcel &parcel)
{
    long value;
    if (ReadLong(parcel, value)) {
        value_ = AAFwk::Long::Box(value);
        return true;
    } else {
        return false;
    }
}

bool PacMapNodeTypeBase::ReadFromParcelByte(Parcel &parcel)
{
    int8_t value;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int8, parcel, value);
    value_ = AAFwk::Byte::Box(value);
    return true;
}

bool PacMapNodeTypeBase::ReadFromParcelBool(Parcel &parcel)
{
    int8_t value;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int8, parcel, value);
    value_ = AAFwk::Boolean::Box(value);
    return true;
}

bool PacMapNodeTypeBase::ReadFromParcelFloat(Parcel &parcel)
{
    float value;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Float, parcel, value);
    value_ = AAFwk::Float::Box(value);
    return true;
}

bool PacMapNodeTypeBase::ReadFromParcelDouble(Parcel &parcel)
{
    double value;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Double, parcel, value);
    value_ = AAFwk::Double::Box(value);
    return true;
}

bool PacMapNodeTypeBase::ReadFromParcelString(Parcel &parcel)
{
    std::u16string value;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(String16, parcel, value);
    value_ = AAFwk::String::Box(Str16ToStr8(value));
    return true;
}

/**
 * @brief Unmarshals this Sequenceable object from a Parcel.
 * @param dataType Indicates the type of data stored.
 * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 * @return Unmarshals success returns true, otherwise returns false.
 */
bool PacMapNodeTypeBase::Unmarshalling(int32_t dataType, Parcel &parcel)
{
    switch (dataType) {
        case PACMAP_DATA_SHORT:
            return ReadFromParcelShort(parcel);
        case PACMAP_DATA_INTEGER:
            return ReadFromParcelInt(parcel);
        case PACMAP_DATA_LONG:
            return ReadFromParcelLong(parcel);
        case PACMAP_DATA_CHAR:
            return ReadFromParcelByte(parcel);
        case PACMAP_DATA_BYTE:
            return ReadFromParcelByte(parcel);
        case PACMAP_DATA_BOOLEAN:
            return ReadFromParcelBool(parcel);
        case PACMAP_DATA_FLOAT:
            return ReadFromParcelFloat(parcel);
        case PACMAP_DATA_DOUBLE:
            return ReadFromParcelDouble(parcel);
        case PACMAP_DATA_STRING:
            return ReadFromParcelString(parcel);
        default:
            break;
    }
    return false;
}

bool PacMapNodeTypeBase::WriteLong(Parcel &parcel, long value) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL_BASE(Int32, parcel, PACMAP_DATA_LONG);
    return parcel.WriteBuffer((void *)&value, sizeof(long));
}

bool PacMapNodeTypeBase::ReadLong(Parcel &parcel, long &value)
{
    const uint8_t *pdata = parcel.ReadBuffer(sizeof(long));
    if (pdata == nullptr) {
        return false;
    }

    value = *((long *)pdata);
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
