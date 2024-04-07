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

#include "value_object.h"

#include "rdb_errno.h"
#include "sqlite_utils.h"

namespace OHOS {
namespace NativeRdb {
ValueObject::ValueObject() : type(ValueObjectType::TYPE_NULL)
{
}

ValueObject::~ValueObject()
{
}

ValueObject::ValueObject(int val) : type(ValueObjectType::TYPE_INT)
{
    value = static_cast<int64_t>(val);
}

ValueObject::ValueObject(int64_t val) : type(ValueObjectType::TYPE_INT)
{
    value = val;
}
ValueObject::ValueObject(double val) : type(ValueObjectType::TYPE_DOUBLE)
{
    value = val;
}
ValueObject::ValueObject(bool val) : type(ValueObjectType::TYPE_BOOL)
{
    value = val;
}
ValueObject::ValueObject(const std::string &val) : type(ValueObjectType::TYPE_STRING)
{
    value = val;
}
ValueObject::ValueObject(const std::vector<uint8_t> &val) : type(ValueObjectType::TYPE_BLOB)
{
    std::vector<uint8_t> blob = val;
    value = blob;
}

ValueObjectType ValueObject::GetType() const
{
    return type;
}

int ValueObject::GetInt(int &val) const
{
    if (type != ValueObjectType::TYPE_INT) {
        return E_INVLAID_ONJECT_TYPE;
    }

    int64_t v = std::get<int64_t>(value);
    val = static_cast<int>(v);
    return E_OK;
}

int ValueObject::GetLong(int64_t &val) const
{
    if (type != ValueObjectType::TYPE_INT) {
        return E_INVLAID_ONJECT_TYPE;
    }

    val = std::get<int64_t>(value);
    return E_OK;
}

int ValueObject::GetDouble(double &val) const
{
    if (type != ValueObjectType::TYPE_DOUBLE) {
        return E_INVLAID_ONJECT_TYPE;
    }

    val = std::get<double>(value);
    return E_OK;
}

int ValueObject::GetBool(bool &val) const
{
    if (type != ValueObjectType::TYPE_BOOL) {
        return E_INVLAID_ONJECT_TYPE;
    }

    val = std::get<bool>(value);
    return E_OK;
}

int ValueObject::GetString(std::string &val) const
{
    if (type != ValueObjectType::TYPE_STRING) {
        return E_INVLAID_ONJECT_TYPE;
    }

    val = std::get<std::string>(value);
    return E_OK;
}

int ValueObject::GetBlob(std::vector<uint8_t> &val) const
{
    if (type != ValueObjectType::TYPE_BLOB) {
        return E_INVLAID_ONJECT_TYPE;
    }

    val = std::get<std::vector<uint8_t>>(value);
    return E_OK;
}
} // namespace NativeRdb
} // namespace OHOS