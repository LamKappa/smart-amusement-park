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

#ifndef OHOS_AppExecFwk_PAC_MAP_NODE_BASE_H
#define OHOS_AppExecFwk_PAC_MAP_NODE_BASE_H

#include "parcel.h"
#include "base_types.h"
#include "bool_wrapper.h"
#include "zchar_wrapper.h"
#include "byte_wrapper.h"
#include "double_wrapper.h"
#include "float_wrapper.h"
#include "int_wrapper.h"
#include "long_wrapper.h"
#include "short_wrapper.h"
#include "string_wrapper.h"

#include "pac_map_node.h"

namespace OHOS {
namespace AppExecFwk {
class PacMapNodeTypeBase : public PacMapNode {
public:
    PacMapNodeTypeBase() : PacMapNode(DT_PACMAP_BASE), value_(nullptr)
    {}
    virtual ~PacMapNodeTypeBase() = default;

    PacMapNodeTypeBase(const PacMapNodeTypeBase &other);
    PacMapNodeTypeBase &operator=(const PacMapNodeTypeBase &other);

    /**
     * @brief Adds a short value to current object.
     * @param value Added data.
     */
    void PutShortValue(short value);

    /**
     * @brief Adds a int value to current object.
     * @param value Added data.
     */
    void PutIntValue(int value);

    /**
     * @brief Adds a long value to current object.
     * @param value Added data.
     */
    void PutLongValue(long value);

    /**
     * @brief Adds a boolean value to current object.
     * @param value Added data.
     */
    void PutBooleanValue(bool value);

    /**
     * @brief Adds a char value to current object.
     * @param value Added data.
     */
    void PutCharValue(char value);

    /**
     * @brief Adds a byte value to current object.
     * @param value Added data.
     */
    void PutByteValue(AAFwk::byte value);

    /**
     * @brief Adds a float value to current object.
     * @param value Added data.
     */
    void PutFloatValue(float value);

    /**
     * @brief Adds a double value to current object.
     * @param value Added data.
     */
    void PutDoubleValue(double value);

    /**
     * @brief Adds a string {std::string} value to current object.
     * @param value Added data.
     */
    void PutStringValue(const std::string &value);

    /**
     * @brief Obtains the short value.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the short value, otherwise return the @a defaultValue.
     */
    short GetShortValue(short defaultValue = 0);

    /**
     * @brief Obtains the int value.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the int value, otherwise return the @a defaultValue.
     */
    int GetIntegerValue(int defaultValue = 0);

    /**
     * @brief Obtains the long value.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the long value, otherwise return the @a defaultValue.
     */
    long GetLongValue(long defaultValue = 0);

    /**
     * @brief Obtains the boolean value.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the boolean value, otherwise return the @a defaultValue.
     */
    bool GetBooleanValue(bool defaultValue = false);

    /**
     * @brief Obtains the char value.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the char value, otherwise return the @a defaultValue.
     */
    char GetCharValue(char defaultValue = 0x00);

    /**
     * @brief Obtains the byte value.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the byte value, otherwise return the @a defaultValue.
     */
    AAFwk::byte GetByteValue(AAFwk::byte defaultValue = 0x00);

    /**
     * @brief Obtains the float value.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the float value, otherwise return the @a defaultValue.
     */
    float GetFloatValue(float defaultValue = 0.0f);

    /**
     * @brief Obtains the double value.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the double value, otherwise return the @a defaultValue.
     */
    double GetDoubleValue(double defaultValue = 0.0);

    /**
     * @brief Obtains the string {std::string} value.
     * @param defaultValue The return value when the function fails.
     * @return If the match is successful, return the std::string value, otherwise return the @a defaultValue.
     */
    std::string GetStringValue(const std::string &defaultValue = "");

    /**
     * @brief Whether the stored data is of short type.
     * @return If yes return true, otherwise return false.
     */
    bool IsShort(void);

    /**
     * @brief Whether the stored data is of integer type.
     * @return If yes return true, otherwise return false.
     */
    bool IsInteger(void);

    /**
     * @brief Whether the stored data is of long type.
     * @return If yes return true, otherwise return false.
     */
    bool IsLong(void);

    /**
     * @brief Whether the stored data is of char type.
     * @return If yes return true, otherwise return false.
     */
    bool IsChar(void);

    /**
     * @brief Whether the stored data is of byte type.
     * @return If yes return true, otherwise return false.
     */
    bool IsByte(void);

    /**
     * @brief Whether the stored data is of boolean type.
     * @return If yes return true, otherwise return false.
     */
    bool IsBoolean(void);

    /**
     * @brief Whether the stored data is of float type.
     * @return If yes return true, otherwise return false.
     */
    bool IsFloat(void);

    /**
     * @brief Whether the stored data is of double type.
     * @return If yes return true, otherwise return false.
     */
    bool IsDouble(void);

    /**
     * @brief Whether the stored data is of string type.
     * @return If yes return true, otherwise return false.
     */
    bool IsString(void);

    /**
     * @brief Indicates whether some other object is "equal to" this one.
     * @param other The object with which to compare.
     * @return true if this object is the same as the obj argument; false otherwise.
     */
    virtual bool Equals(const PacMapNode *other) override;

    /**
     * @brief Copy the data of the specified object to the current object with deepcopy
     * @param other The original object that stores the data.
     */
    virtual void DeepCopy(const PacMapNode *other) override;

    /**
     * @brief Marshals this Sequenceable object to a Parcel.
     * @param key Indicates the key in String format.
     * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     * @return Marshals success returns true, otherwise returns false.
     */
    virtual bool Marshalling(const std::string &key, Parcel &parcel) const override;

    /**
     * @brief Unmarshals this Sequenceable object from a Parcel.
     * @param dataType Indicates the type of data stored.
     * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     * @return Unmarshals success returns true, otherwise returns false.
     */
    virtual bool Unmarshalling(int32_t dataType, Parcel &parcel) override;

private:
    sptr<AAFwk::IInterface> value_;

    void InnerDeepCopy(const PacMapNodeTypeBase *other);
    bool WriteLong(Parcel &parcel, long value) const;
    bool ReadLong(Parcel &parcel, long &value);

    bool ReadFromParcelShort(Parcel &parcel);
    bool ReadFromParcelInt(Parcel &parcel);
    bool ReadFromParcelLong(Parcel &parcel);
    bool ReadFromParcelByte(Parcel &parcel);
    bool ReadFromParcelBool(Parcel &parcel);
    bool ReadFromParcelFloat(Parcel &parcel);
    bool ReadFromParcelString(Parcel &parcel);
    bool ReadFromParcelDouble(Parcel &parcel);
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif
