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

#ifndef NATIVE_RDB_STEP_RESULT_SET_H
#define NATIVE_RDB_STEP_RESULT_SET_H

#include <memory>
#include <thread>
#include <vector>

#include "rdb_store_impl.h"
#include "result_set.h"
#include "sqlite_statement.h"

namespace OHOS {
namespace NativeRdb {

class StepResultSet : public ResultSet {
public:
    StepResultSet(
        std::shared_ptr<RdbStoreImpl> rdb, const std::string &sql, const std::vector<std::string> &selectionArgs);
    ~StepResultSet() override;

    int GetAllColumnNames(std::vector<std::string> &columnNames) override;
    int GetColumnCount(int &count) override;
    int GetColumnTypeForIndex(int columnIndex, ColumnType &columnType) const override;
    int GetColumnIndexForName(const std::string &columnName, int &columnIndex) override;
    int GetColumnNameForIndex(int columnIndex, std::string &columnName) override;
    int GetRowCount(int &count) const override;
    int GetRowIndex(int &position) const override;
    int GoTo(int offset) override;
    int GoToRow(int position) override;
    int GoToFirstRow() override;
    int GoToLastRow() override;
    int GoToNextRow() override;
    int GoToPreviousRow() override;
    int IsEnded(bool &result) const override;
    int IsStarted(bool &result) const override;
    int IsAtFirstRow(bool &result) const override;
    int IsAtLastRow(bool &result) const override;
    int GetBlob(int columnIndex, std::vector<uint8_t> &blob) const override;
    int GetString(int columnIndex, std::string &value) const override;
    int GetInt(int columnIndex, int &value) const override;
    int GetLong(int columnIndex, int64_t &value) const override;
    int GetDouble(int columnIndex, double &value) const override;
    int IsColumnNull(int columnIndex, bool &isNull) const override;
    bool IsClosed() const override;
    int Close() override;

private:
    int CheckSession();
    int PrepareStep();
    int FinishStep();
    std::shared_ptr<RdbStoreImpl> rdb;
    std::string sql;
    std::vector<std::string> selectionArgs;
    int pos;
    bool isAfterLast;
    bool isClosed;
    std::thread::id tid;
    std::shared_ptr<SqliteStatement> sqliteStatement;
    static const int INIT_POS = -1;
    // Max times of retrying step query
    static const int STEP_QUERY_RETRY_MAX_TIMES = 50;
    // Interval of retrying step query in millisecond
    static const int STEP_QUERY_RETRY_INTERVAL = 1000;
};

} // namespace NativeRdb
} // namespace OHOS
#endif
