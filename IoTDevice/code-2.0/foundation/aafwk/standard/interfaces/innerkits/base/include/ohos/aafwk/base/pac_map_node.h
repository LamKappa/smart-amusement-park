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

#ifndef OHOS_AppExecFwk_PAC_MAP_NODE_H
#define OHOS_AppExecFwk_PAC_MAP_NODE_H

#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {
// base:   0x00000001 ~ 0x000000FF
// array:  0x00000100 ~ 0x0000FF00
// boject: 0x00010000 ~ 0x00FF0000
#define PACMAP_DATA_SHORT 0x00000001
#define PACMAP_DATA_INTEGER 0x00000002
#define PACMAP_DATA_LONG 0x00000003
#define PACMAP_DATA_CHAR 0x00000004
#define PACMAP_DATA_BYTE 0x00000005
#define PACMAP_DATA_BOOLEAN 0x00000007
#define PACMAP_DATA_FLOAT 0x00000008
#define PACMAP_DATA_DOUBLE 0x00000009
#define PACMAP_DATA_STRING 0x0000000A
#define PACMAP_DATA_ARRAY_SHORT 0x00000100
#define PACMAP_DATA_ARRAY_INTEGER 0x00000200
#define PACMAP_DATA_ARRAY_LONG 0x00000300
#define PACMAP_DATA_ARRAY_CHAR 0x00000400
#define PACMAP_DATA_ARRAY_BYTE 0x00000500
#define PACMAP_DATA_ARRAY_BOOLEAN 0x00000600
#define PACMAP_DATA_ARRAY_FLOAT 0x00000700
#define PACMAP_DATA_ARRAY_DOUBLE 0x00000800
#define PACMAP_DATA_ARRAY_STRING 0x00000900
#define PACMAP_DATA_OBJECT 0x00010000

typedef enum {
    DT_PACMAP_BASE = 0, /* Basic data types */
    DT_PACMAP_ARRAY,    /* Basic aray of data types */
    DT_PACMAP_OBJECT    /* custum object */
} PacMapNodeType;

class PacMapNode {
public:
    /**
     * @brief A constructor data.
     * @param data_type Rough data types, it is an enumerated type, see {@PacMapNodeType}.
     */
    PacMapNode(PacMapNodeType data_type) : data_type_(data_type)
    {}
    virtual ~PacMapNode() = default;
    PacMapNode() = default;

    /**
     * @brief A  constructor data.
     */
    PacMapNode(const PacMapNode &other);

    PacMapNode &operator=(const PacMapNode &other);

    inline bool IsBase(void) const
    {
        return data_type_ == DT_PACMAP_BASE;
    }
    inline bool IsArray(void) const
    {
        return data_type_ == DT_PACMAP_ARRAY;
    }
    inline bool IsObject(void) const
    {
        return data_type_ == DT_PACMAP_OBJECT;
    }
    inline PacMapNodeType GetMapNodeTypType(void)
    {
        return data_type_;
    }

    /**
     * @brief Indicates whether some other object is "equal to" this one.
     * @param other The object with which to compare.
     * @return Resturns true if this object is the same as the obj argument; false otherwise.
     */
    virtual bool Equals(const PacMapNode *other) = 0;

    /**
     * @brief Copy the data of the specified object to the current object with deepcopy
     * @param other The original object that stores the data.
     */
    virtual void DeepCopy(const PacMapNode *other) = 0;

    /**
     * @brief Marshals this Sequenceable object to a Parcel.
     * @param key Indicates the key in String format.
     * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     * @return Marshals success returns true, otherwise returns false.
     */
    virtual bool Marshalling(const std::string &key, Parcel &parcel) const = 0;

    /**
     * @brief Unmarshals this Sequenceable object from a Parcel.
     * @param dataType Indicates the type of data stored.
     * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     * @return Unmarshals success returns true, otherwise returns false.
     */
    virtual bool Unmarshalling(int32_t dataType, Parcel &parcel) = 0;

protected:
    PacMapNodeType data_type_;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif
