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

#include "step_result_set.h"

#include <unistd.h>

#include "logger.h"
#include "rdb_errno.h"
#include "sqlite3sym.h"
#include "sqlite_errno.h"

namespace OHOS {
namespace NativeRdb {
StepResultSet::StepResultSet(
    std::shared_ptr<RdbStoreImpl> rdb, const std::string &sql, const std::vector<std::string> &selectionArgs)
    : rdb(rdb), sql(sql), selectionArgs(selectionArgs), pos(INIT_POS), isAfterLast(false), isClosed(false),
      sqliteStatement(nullptr)
{
}

StepResultSet::~StepResultSet()
{
    Close();
}

int StepResultSet::GetAllColumnNames(std::vector<std::string> &columnNames)
{
    int errCode = PrepareStep();
    if (errCode) {
        return errCode;
    }

    int columnCount = 0;
    errCode = sqliteStatement->GetColumnCount(columnCount);
    if (errCode) {
        return errCode;
    }

    columnNames.clear();
    for (int i = 0; i < columnCount; i++) {
        std::string columnName;
        errCode = sqliteStatement->GetColumnName(i, columnName);
        if (errCode) {
            columnNames.clear();
            return errCode;
        }
        columnNames.push_back(columnName);
    }

    return E_OK;
}

int StepResultSet::GetColumnCount(int &count)
{
    int errCode = PrepareStep();
    if (errCode) {
        return errCode;
    }

    errCode = sqliteStatement->GetColumnCount(count);
    return errCode;
}

int StepResultSet::GetColumnTypeForIndex(int columnIndex, ColumnType &columnType) const
{
    if (pos == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }

    int sqliteType;
    int errCode = sqliteStatement->GetColumnType(columnIndex, sqliteType);
    if (errCode) {
        return errCode;
    }

    switch (sqliteType) {
        case SQLITE_INTEGER:
            columnType = ColumnType::TYPE_INTEGER;
            break;
        case SQLITE_FLOAT:
            columnType = ColumnType::TYPE_FLOAT;
            break;
        case SQLITE_BLOB:
            columnType = ColumnType::TYPE_BLOB;
            break;
        case SQLITE_NULL:
            columnType = ColumnType::TYPE_NULL;
            break;
        default:
            columnType = ColumnType::TYPE_STRING;
    }

    return E_OK;
}

int StepResultSet::GetColumnIndexForName(const std::string &columnName, int &columnIndex)
{
    int errCode = PrepareStep();
    if (errCode) {
        return errCode;
    }

    int columnCount = 0;
    errCode = sqliteStatement->GetColumnCount(columnCount);
    if (errCode) {
        return errCode;
    }

    for (int i = 0; i < columnCount; i++) {
        std::string name;
        errCode = sqliteStatement->GetColumnName(i, name);
        if (errCode) {
            return errCode;
        }
        if (columnName.compare(name) == 0) {
            columnIndex = i;
            return E_OK;
        }
    }

    return E_INVALID_COLUMN_NAME;
}

int StepResultSet::GetColumnNameForIndex(int columnIndex, std::string &columnName)
{
    int errCode = PrepareStep();
    if (errCode) {
        return errCode;
    }

    errCode = sqliteStatement->GetColumnName(columnIndex, columnName);
    return errCode;
}

int StepResultSet::GetRowCount(int &count) const
{
    LOG_ERROR("GetRowCount is not supported by StepResultSet");
    return E_NOT_SUPPORTED_BY_STEP_RESULT_SET;
}

int StepResultSet::GetRowIndex(int &position) const
{
    position = pos;
    return E_OK;
}

int StepResultSet::GoTo(int offset)
{
    LOG_ERROR("Move is not supported by StepResultSet");
    return E_NOT_SUPPORTED_BY_STEP_RESULT_SET;
}

int StepResultSet::GoToRow(int position)
{
    LOG_ERROR("GoToRow is not supported by StepResultSet");
    return E_NOT_SUPPORTED_BY_STEP_RESULT_SET;
}

int StepResultSet::GoToFirstRow()
{
    if (pos == INIT_POS) {
        return GoToNextRow();
    }

    LOG_ERROR("GoToFirstRow is not supported by StepResultSet");
    return E_NOT_SUPPORTED_BY_STEP_RESULT_SET;
}

int StepResultSet::GoToLastRow()
{
    LOG_ERROR("GoToLastRow is not supported by StepResultSet");
    return E_NOT_SUPPORTED_BY_STEP_RESULT_SET;
}

int StepResultSet::GoToNextRow()
{
    if (isAfterLast) {
        return E_STEP_RESULT_IS_AFTER_LASET;
    }

    int errCode = PrepareStep();
    if (errCode) {
        return errCode;
    }

    int retryCount = 0;
    errCode = sqliteStatement->Step();
    while (errCode == SQLITE_LOCKED || errCode == SQLITE_BUSY) {
        // The table is locked, retry
        if (retryCount > STEP_QUERY_RETRY_MAX_TIMES) {
            LOG_ERROR("StepResultSet::GoToNextRow retrycount exceeded");
            return E_STEP_RESULT_QUERY_EXCEEDED;
        } else {
            // Sleep to give the thread holding the lock a chance to finish
            usleep(STEP_QUERY_RETRY_INTERVAL);
            errCode = sqliteStatement->Step();
            retryCount++;
        }
    }

    if (errCode == SQLITE_ROW) {
        pos++;
        return E_OK;
    } else if (errCode == SQLITE_DONE) {
        isAfterLast = true;
        FinishStep();
        return E_STEP_RESULT_IS_AFTER_LASET;
    } else {
        LOG_ERROR("StepResultSet::GoToNextRow step err = %{public}d", errCode);
        FinishStep();
        return SQLiteError::ErrNo(errCode);
    }
}

int StepResultSet::GoToPreviousRow()
{
    LOG_ERROR("GoToPreviousRow is not supported by StepResultSet");
    return E_NOT_SUPPORTED_BY_STEP_RESULT_SET;
}

int StepResultSet::IsEnded(bool &result) const
{
    result = isAfterLast;
    return E_OK;
}

int StepResultSet::IsStarted(bool &result) const
{
    result = (pos == INIT_POS);
    return E_OK;
}

int StepResultSet::IsAtFirstRow(bool &result) const
{
    result = (pos == 0);
    return E_OK;
}

int StepResultSet::IsAtLastRow(bool &result) const
{
    LOG_ERROR("IsAtLastRow is not supported by StepResultSet");
    return E_NOT_SUPPORTED_BY_STEP_RESULT_SET;
}

int StepResultSet::GetBlob(int columnIndex, std::vector<uint8_t> &blob) const
{
    if (pos == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }

    return sqliteStatement->GetColumnBlob(columnIndex, blob);
}

int StepResultSet::GetString(int columnIndex, std::string &value) const
{
    if (pos == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }
    return sqliteStatement->GetColumnString(columnIndex, value);
}

int StepResultSet::GetInt(int columnIndex, int &value) const
{
    if (pos == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }

    int64_t columnValue;
    int errCode = sqliteStatement->GetColumnLong(columnIndex, columnValue);
    if (errCode != E_OK) {
        return errCode;
    }
    value = static_cast<int>(columnValue);
    return E_OK;
}

int StepResultSet::GetLong(int columnIndex, int64_t &value) const
{
    if (pos == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }
    return sqliteStatement->GetColumnLong(columnIndex, value);
}

int StepResultSet::GetDouble(int columnIndex, double &value) const
{
    if (pos == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }
    return sqliteStatement->GetColumnDouble(columnIndex, value);
}

int StepResultSet::IsColumnNull(int columnIndex, bool &isNull) const
{
    ColumnType columnType;
    int errCode = GetColumnTypeForIndex(columnIndex, columnType);
    if (errCode != E_OK) {
        return errCode;
    }
    isNull = (columnType == ColumnType::TYPE_NULL);
    return E_OK;
}

bool StepResultSet::IsClosed() const
{
    return isClosed;
}

int StepResultSet::Close()
{
    if (isClosed) {
        return E_OK;
    }
    isClosed = true;
    int errCode = FinishStep();
    rdb = nullptr;
    return errCode;
}

int StepResultSet::CheckSession()
{
    if (std::this_thread::get_id() != tid) {
        LOG_ERROR("StepResultSet is passed cross threads!");
        return E_STEP_RESULT_SET_CROSS_THREADS;
    }
    return E_OK;
}

int StepResultSet::PrepareStep()
{
    if (isClosed) {
        return E_STEP_RESULT_CLOSED;
    }

    if (sqliteStatement != nullptr) {
        return CheckSession();
    }

    int errCode;
    sqliteStatement = rdb->BeginStepQuery(errCode, sql, selectionArgs);
    if (sqliteStatement == nullptr) {
        rdb->EndStepQuery();
        return errCode;
    }

    tid = std::this_thread::get_id();
    return E_OK;
}

int StepResultSet::FinishStep()
{
    int errCode = CheckSession();
    if (errCode != E_OK) {
        return errCode;
    }

    if (sqliteStatement == nullptr) {
        return E_OK;
    }

    sqliteStatement = nullptr;
    pos = INIT_POS;

    errCode = rdb->EndStepQuery();
    if (errCode != E_OK) {
        LOG_ERROR("StepResultSet::FinishStep err = %{public}d", errCode);
    }
    return errCode;
}
} // namespace NativeRdb
} // namespace OHOS