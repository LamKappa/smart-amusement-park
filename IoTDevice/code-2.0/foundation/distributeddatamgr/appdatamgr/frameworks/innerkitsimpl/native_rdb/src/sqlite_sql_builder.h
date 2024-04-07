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

#ifndef NATIVE_RDB_SQLITE_SQL_BUILDER_H
#define NATIVE_RDB_SQLITE_SQL_BUILDER_H

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace OHOS {
namespace NativeRdb {

class SqliteSqlBuilder {
public:
    static int BuildQueryString(bool distinct, const std::string &table, const std::vector<std::string> &columns,
        const std::string &where, const std::string &groupBy, const std::string &having, const std::string &orderBy,
        const std::string &limit, std::string &outSql);

private:
    static void AppendClause(std::stringstream &sql, const std::string &name, const std::string &clause);
    static void AppendColumns(std::stringstream &sql, const std::vector<std::string> &columns);
};

} // namespace NativeRdb
} // namespace OHOS
#endif