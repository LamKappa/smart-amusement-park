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

#ifndef OHOS_AppExecFwk_PAC_MAP_NODE_ARRAY_H
#define OHOS_AppExecFwk_PAC_MAP_NODE_ARRAY_H

#include "parcel.h"
#include "base_types.h"
#include "array_wrapper.h"
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
class PacMapNodeTypeArray : public PacMapNode {
public:
    PacMapNodeTypeArray() : PacMapNode(DT_PACMAP_ARRAY), value_(nullptr)
    {}
    virtual ~PacMapNodeTypeArray() = default;

    PacMapNodeTypeArray(const PacMapNodeTypeArray &other);
    PacMapNodeTypeArray &operator=(const PacMapNodeTypeArray &other);

    /**
     * @brief Adds some short values to current object.
     * @param value Added list of data.
     */
    void PutShortValueArray(const std::vector<short> &value);

    /**
     * @brief Adds some int values to current object.
     * @param value Added list of data.
     */
    void PutIntegerValueArray(const std::vector<int> &value);

    /**
     * @brief Adds some long values to current object.
     * @param value Added list of data.
     */
    void PutLongValueArray(const std::vector<long> &value);

    /**
     * @brief Adds some boolean values to current object.
     * @param value Added list of data.
     */
    void PutBooleanValueArray(const std::vector<bool> &value);

    /**
     * @brief Adds some char values to current object.
     * @param value Added list of data.
     */
    void PutCharValueArray(const std::vector<char> &value);

    /**
     * @brief Adds some byte values to current object.
     * @param value Added list of data.
     */
    void PutByteValueArray(const std::vector<AAFwk::byte> &value);

    /**
     * @brief Adds some float values to current object.
     * @param value Added list of data.
     */
    void PutFloatValueArray(const std::vector<float> &value);

    /**
     * @brief Adds some double values to current object.
     * @param value Added list of data.
     */
    void PutDoubleValueArray(const std::vector<double> &value);

    /**
     * @brief Adds some string {std::string} values to current object.
     * @param value Added list of data.
     */
    void PutStringValueArray(const std::vector<std::string> &value);

    /**
     * @brief Obtains some short values.
     * @param value Save the returned short values.
     */
    void GetShortValueArray(std::vector<short> &value);

    /**
     * @brief Obtains some int values.
     * @param value Save the returned int values.
     */
    void GetIntegerValueArray(std::vector<int> &value);

    /**
     * @brief Obtains some long values.
     * @param value Save the returned long values.
     */
    void GetLongValueArray(std::vector<long> &value);

    /**
     * @brief Obtains some boolean values.
     * @param value Save the returned boolean values.
     */
    void GetBooleanValueArray(std::vector<bool> &value);

    /**
     * @brief Obtains some char values.
     * @param value Save the returned char values.
     */
    void GetCharValueArray(std::vector<char> &value);

    /**
     * @brief Obtains some byte values.
     * @param value Save the returned byte values.
     */
    void GetByteValueArray(std::vector<AAFwk::byte> &value);

    /**
     * @brief Obtains some float values.
     * @param value Save the returned float values.
     */
    void GetFloatValueArray(std::vector<float> &value);

    /**
     * @brief Obtains some double values.
     * @param value Save the returned double values.
     */
    void GetDoubleValueArray(std::vector<double> &value);

    /**
     * @brief Obtains some std::string values.
     * @param value Save the returned std::string values.
     */
    void GetStringValueArray(std::vector<std::string> &value);

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
    sptr<AAFwk::IArray> value_;

    void InnerDeepCopy(const PacMapNodeTypeArray *other);
    bool WriteLongVector(Parcel &parcel, const std::vector<long> &value) const;
    bool ReadLongVector(Parcel &parcel, std::vector<long> &value);

    bool MarshallingArrayString(Parcel &parcel) const;
    bool MarshallingArrayBoolean(Parcel &parcel) const;
    bool MarshallingArrayByte(Parcel &parcel) const;
    bool MarshallingArrayShort(Parcel &parcel) const;
    bool MarshallingArrayInteger(Parcel &parcel) const;
    bool MarshallingArrayLong(Parcel &parcel) const;
    bool MarshallingArrayFloat(Parcel &parcel) const;
    bool MarshallingArrayDouble(Parcel &parcel) const;

    bool UnmarshallingArrayShort(Parcel &parcel);
    bool UnmarshallingArrayInteger(Parcel &parcel);
    bool UnmarshallingArrayLong(Parcel &parcel);
    bool UnmarshallingArrayByte(Parcel &parcel);
    bool UnmarshallingArrayBoolean(Parcel &parcel);
    bool UnmarshallingArrayFloat(Parcel &parcel);
    bool UnmarshallingArrayDouble(Parcel &parcel);
    bool UnmarshallingArrayString(Parcel &parcel);
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif
