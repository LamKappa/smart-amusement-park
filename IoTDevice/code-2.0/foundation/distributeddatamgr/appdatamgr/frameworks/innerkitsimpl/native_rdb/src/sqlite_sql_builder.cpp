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

#include "sqlite_sql_builder.h"

#include "rdb_errno.h"

namespace OHOS {
namespace NativeRdb {
int SqliteSqlBuilder::BuildQueryString(bool distinct, const std::string &table,
    const std::vector<std::string> &columns, const std::string &where, const std::string &groupBy,
    const std::string &having, const std::string &orderBy, const std::string &limit, std::string &outSql)
{
    if (groupBy.empty() == true && having.empty() == false) {
        return E_HAVING_CLAUSE_NOT_IN_GROUP_BY;
    }

    if (table.empty()) {
        return E_EMPTY_TABLE_NAME;
    }

    std::stringstream sql;
    sql << "SELECT ";
    if (distinct) {
        sql << "DISTINCT ";
    }
    AppendColumns(sql, columns);
    sql << "FROM " << table;

    AppendClause(sql, " WHERE ", where);
    AppendClause(sql, " GROUP BY ", groupBy);
    AppendClause(sql, " HAVING ", having);
    AppendClause(sql, " ORDER BY ", orderBy);
    AppendClause(sql, " LIMIT ", limit);

    outSql = sql.str();
    return E_OK;
}

void SqliteSqlBuilder::AppendClause(std::stringstream &sql, const std::string &name, const std::string &clause)
{
    if (clause.empty()) {
        return;
    }
    sql << name << clause;
}

void SqliteSqlBuilder::AppendColumns(std::stringstream &sql, const std::vector<std::string> &columns)
{
    if (columns.empty()) {
        sql << "* ";
        return;
    }

    bool isFirst = true;
    for (auto &iter : columns) {
        if (iter.empty()) {
            continue;
        }

        if (isFirst) {
            isFirst = false;
        } else {
            sql << ", ";
        }
        sql << iter;
    }

    sql << ' ';
}
} // namespace NativeRdb
} // namespace OHOS
