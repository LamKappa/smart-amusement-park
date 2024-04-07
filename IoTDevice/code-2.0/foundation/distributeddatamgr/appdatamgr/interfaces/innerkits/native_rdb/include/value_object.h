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

#ifndef NATIVE_RDB_VALUE_OBJECT_H
#define NATIVE_RDB_VALUE_OBJECT_H

#include <string>
#include <variant>
#include <vector>

namespace OHOS {
namespace NativeRdb {

enum class ValueObjectType {
    TYPE_NULL = 0,
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_BLOB,
    TYPE_BOOL,
};

class ValueObject {
public:
    ValueObject();
    ~ValueObject();
    explicit ValueObject(int val);
    explicit ValueObject(int64_t val);
    explicit ValueObject(double val);
    explicit ValueObject(bool val);
    explicit ValueObject(const std::string &val);
    explicit ValueObject(const std::vector<uint8_t> &blob);

    ValueObjectType GetType() const;
    int GetInt(int &val) const;
    int GetLong(int64_t &val) const;
    int GetDouble(double &val) const;
    int GetBool(bool &val) const;
    int GetString(std::string &val) const;
    int GetBlob(std::vector<uint8_t> &val) const;

private:
    ValueObjectType type;
    std::variant<int64_t, double, std::string, bool, std::vector<uint8_t>> value;
};

} // namespace NativeRdb
} // namespace OHOS
#endif
