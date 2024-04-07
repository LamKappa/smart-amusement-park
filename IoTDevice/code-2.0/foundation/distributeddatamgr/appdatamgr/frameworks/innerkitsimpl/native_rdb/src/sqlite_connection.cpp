/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sqlite_connection.h"

#include <sys/stat.h>

#include <cerrno>

#include "logger.h"
#include "rdb_errno.h"
#include "sqlite_errno.h"
#include "sqlite_global_config.h"
#include "sqlite_utils.h"

namespace OHOS {
namespace NativeRdb {
SqliteConnection *SqliteConnection::Open(const SqliteConfig &config, bool isWriteConnection, int &errCode)
{
    auto connection = new SqliteConnection(isWriteConnection);
    errCode = connection->InnerOpen(config);
    if (errCode != E_OK) {
        delete connection;
        return nullptr;
    }
    return connection;
}

SqliteConnection::SqliteConnection(bool isWriteConnection)
    : dbHandle(nullptr), isWriteConnection(isWriteConnection), isReadOnly(false), statement(), stepStatement(nullptr)
{
}

int SqliteConnection::InnerOpen(const SqliteConfig &config)
{
    std::string dbPath;
    if (config.GetStorageMode() == StorageMode::MODE_MEMORY) {
        dbPath = SqliteGlobalConfig::GetMemoryDbPath();
    } else if (config.GetPath().empty()) {
        LOG_ERROR("SqliteConnection InnerOpen input empty database path");
        return E_EMPTY_FILE_NAME;
    } else if (config.GetPath().front() != '/') { // change this to starts_with() after c++20
        LOG_ERROR("SqliteConnection InnerOpen input relative path");
        return E_RELATIVE_PATH;
    } else {
        dbPath = config.GetPath();
    }

    stepStatement = std::make_shared<SqliteStatement>();
    if (stepStatement == nullptr) {
        return E_STEP_STATEMENT_NOT_INIT;
    }

    isReadOnly = !isWriteConnection || config.IsReadOnly();
    int openFlags = config.IsReadOnly() ? SQLITE_OPEN_READONLY : (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
    int errCode = sqlite3_open_v2(dbPath.c_str(), &dbHandle, openFlags, nullptr);
    if (errCode != SQLITE_OK) {
        LOG_ERROR("SqliteConnection InnerOpen fail to open database err = %{public}d", errCode);
        return SQLiteError::ErrNo(errCode);
    }

    LimitPermission(dbPath);

    errCode = Config(config);
    if (errCode != E_OK) {
        return errCode;
    }

    return E_OK;
}

int SqliteConnection::Config(const SqliteConfig &config)
{
    if (config.GetStorageMode() == StorageMode::MODE_MEMORY) {
        return E_OK;
    }

    int errCode = SetPageSize();
    if (errCode != E_OK) {
        return errCode;
    }

    std::vector<uint8_t> encryptKey = config.GetEncryptKey();
    errCode = SetEncryptKey(encryptKey);
    std::fill(encryptKey.begin(), encryptKey.end(), 0);
    if (errCode != E_OK) {
        return errCode;
    }

    errCode = SetJournalMode(config.GetJournalMode());
    if (errCode != E_OK) {
        return errCode;
    }

    errCode = SetJournalSizeLimit();
    if (errCode != E_OK) {
        return errCode;
    }

    errCode = SetAutoCheckpoint();
    if (errCode != E_OK) {
        return errCode;
    }

    return E_OK;
}

SqliteConnection::~SqliteConnection()
{
    if (dbHandle != nullptr) {
        statement.Finalize();
        if (stepStatement != nullptr) {
            stepStatement->Finalize();
        }
        int errCode = sqlite3_close(dbHandle);
        if (errCode != SQLITE_OK) {
            LOG_ERROR("SqliteConnection ~SqliteConnection: could not close database err = %{public}d", errCode);
        }
    }
}

int SqliteConnection::SetPageSize()
{
    if (isReadOnly) {
        return E_OK;
    }

    int targetValue = SqliteGlobalConfig::GetPageSize();
    int64_t value;
    int errCode = ExecuteGetLong(value, "PRAGMA page_size");
    if (errCode != E_OK) {
        LOG_ERROR("SqliteConnection SetPageSize fail to get page size : %{public}d", errCode);
        return errCode;
    }

    if (value == targetValue) {
        return E_OK;
    }

    errCode = ExecuteSql("PRAGMA page_size=" + std::to_string(targetValue));
    if (errCode != E_OK) {
        LOG_ERROR("SqliteConnection SetPageSize fail to set page size : %{public}d", errCode);
    }
    return errCode;
}

int SqliteConnection::SetEncryptKey(const std::vector<uint8_t> &encryptKey)
{
    if (encryptKey.empty()) {
        return E_OK;
    }

    int errCode = sqlite3_key(dbHandle, static_cast<const void *>(encryptKey.data()), encryptKey.size());
    if (errCode != SQLITE_OK) {
        LOG_ERROR("SqliteConnection SetEncryptKey fail, err = %{public}d", errCode);
        return SQLiteError::ErrNo(errCode);
    }

    return E_OK;
}

int SqliteConnection::SetJournalMode(const std::string &journalMode)
{
    if (isReadOnly) {
        return E_OK;
    }

    std::string currentMode;
    int errCode = ExecuteGetString(currentMode, "PRAGMA journal_mode");
    if (errCode != E_OK) {
        LOG_ERROR("SqliteConnection SetJournalMode fail to get journal mode : %{public}d", errCode);
        return errCode;
    }

    currentMode = SqliteUtils::StrToUpper(currentMode);
    if (currentMode != journalMode) {
        std::string result;
        int errorCode = ExecuteGetString(result, "PRAGMA journal_mode=" + journalMode);
        if (errorCode != E_OK) {
            LOG_ERROR("SqliteConnection SetJournalMode: fail to set journal mode err=%{public}d", errorCode);
            return errorCode;
        }

        if (SqliteUtils::StrToUpper(result) != journalMode) {
            LOG_ERROR("SqliteConnection SetJournalMode: result incorrect");
            return E_EXECUTE_RESULT_INCORRECT;
        }
    }

    if (journalMode == "WAL") {
        errCode = SetWalSyncMode();
    }

    return errCode;
}

int SqliteConnection::SetJournalSizeLimit()
{
    if (isReadOnly) {
        return E_OK;
    }

    int targetValue = SqliteGlobalConfig::GetJournalFileSize();
    int64_t currentValue;
    int errCode = ExecuteGetLong(currentValue, "PRAGMA journal_size_limit");
    if (errCode != E_OK) {
        LOG_ERROR("SqliteConnection SetJournalSizeLimit fail to get journal_size_limit : %{public}d", errCode);
        return errCode;
    }

    if (currentValue == targetValue) {
        return E_OK;
    }

    int64_t result;
    errCode = ExecuteGetLong(result, "PRAGMA journal_size_limit=" + std::to_string(targetValue));
    if (errCode != E_OK) {
        LOG_ERROR("SqliteConnection SetJournalSizeLimit fail to set journal_size_limit : %{public}d", errCode);
    }
    return errCode;
}

int SqliteConnection::SetAutoCheckpoint()
{
    if (isReadOnly) {
        return E_OK;
    }

    int targetValue = SqliteGlobalConfig::GetWalAutoCheckpoint();
    int64_t value;
    int errCode = ExecuteGetLong(value, "PRAGMA wal_autocheckpoint");
    if (errCode != E_OK) {
        LOG_ERROR("SqliteConnection SetAutoCheckpoint fail to get wal_autocheckpoint : %{public}d", errCode);
        return errCode;
    }

    if (value == targetValue) {
        return E_OK;
    }

    int64_t result;
    errCode = ExecuteGetLong(result, "PRAGMA wal_autocheckpoint=" + std::to_string(targetValue));
    if (errCode != E_OK) {
        LOG_ERROR("SqliteConnection SetAutoCheckpoint fail to set wal_autocheckpoint : %{public}d", errCode);
    }
    return errCode;
}

int SqliteConnection::SetWalSyncMode()
{
    std::string targetValue = SqliteGlobalConfig::GetWalSyncMode();
    std::string value;
    int errCode = ExecuteGetString(value, "PRAGMA synchronous");
    if (errCode != E_OK) {
        LOG_ERROR("SqliteConnection setWalSyncMode fail to get synchronous mode : %{public}d", errCode);
        return errCode;
    }

    value = SqliteUtils::StrToUpper(value);
    if (value == targetValue) {
        return E_OK;
    }

    errCode = ExecuteSql("PRAGMA synchronous=" + targetValue);
    if (errCode != E_OK) {
        LOG_ERROR("SqliteConnection setWalSyncMode fail to set synchronous mode : %{public}d", errCode);
    }
    return errCode;
}

bool SqliteConnection::IsWriteConnection() const
{
    return isWriteConnection;
}

int SqliteConnection::Prepare(const std::string &sql, bool &outIsReadOnly)
{
    int errCode = statement.Prepare(dbHandle, sql);
    if (errCode != E_OK) {
        return errCode;
    }
    outIsReadOnly = statement.IsReadOnly();
    return E_OK;
}

int SqliteConnection::PrepareAndBind(const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = statement.Prepare(dbHandle, sql);
    if (errCode != E_OK) {
        return errCode;
    }

    if (!isWriteConnection && !statement.IsReadOnly()) {
        return E_EXECUTE_WRITE_IN_READ_CONNECTION;
    }

    errCode = statement.BindArguments(bindArgs);
    return errCode;
}

int SqliteConnection::ExecuteSql(const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = PrepareAndBind(sql, bindArgs);
    if (errCode != E_OK) {
        return errCode;
    }

    errCode = statement.Step();
    if (errCode == SQLITE_ROW) {
        LOG_ERROR("SqliteConnection Execute : Queries can be performed using query or QuerySql methods only");
        statement.ResetStatementAndClearBindings();
        return E_QUERY_IN_EXECUTE;
    } else if (errCode != SQLITE_DONE) {
        LOG_ERROR("SqliteConnection Execute : err %{public}d", errCode);
        statement.ResetStatementAndClearBindings();
        return SQLiteError::ErrNo(errCode);
    }

    errCode = statement.ResetStatementAndClearBindings();
    return errCode;
}

int SqliteConnection::ExecuteForChangedRowCount(
    int &changedRows, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = PrepareAndBind(sql, bindArgs);
    if (errCode != E_OK) {
        return errCode;
    }

    errCode = statement.Step();
    if (errCode == SQLITE_ROW) {
        LOG_ERROR("SqliteConnection ExecuteForChangedRowCount : Queries can be performed using query or QuerySql "
                  "methods only");
        statement.ResetStatementAndClearBindings();
        return E_QUERY_IN_EXECUTE;
    } else if (errCode != SQLITE_DONE) {
        LOG_ERROR("SqliteConnection ExecuteForChangedRowCount : failed %{public}d", errCode);
        statement.ResetStatementAndClearBindings();
        return SQLiteError::ErrNo(errCode);
    }

    changedRows = sqlite3_changes(dbHandle);
    errCode = statement.ResetStatementAndClearBindings();
    return errCode;
}

int SqliteConnection::ExecuteForLastInsertedRowId(
    int64_t &outRowId, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = PrepareAndBind(sql, bindArgs);
    if (errCode != E_OK) {
        return errCode;
    }

    errCode = statement.Step();
    if (errCode == SQLITE_ROW) {
        LOG_ERROR("SqliteConnection ExecuteForLastInsertedRowId : Queries can be performed using query or QuerySql "
                  "methods only");
        statement.ResetStatementAndClearBindings();
        return E_QUERY_IN_EXECUTE;
    } else if (errCode != SQLITE_DONE) {
        LOG_ERROR("SqliteConnection ExecuteForLastInsertedRowId : failed %{public}d", errCode);
        statement.ResetStatementAndClearBindings();
        return SQLiteError::ErrNo(errCode);
    }

    outRowId = (sqlite3_changes(dbHandle) > 0) ? sqlite3_last_insert_rowid(dbHandle) : -1;
    errCode = statement.ResetStatementAndClearBindings();
    return errCode;
}

int SqliteConnection::ExecuteGetLong(
    int64_t &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = PrepareAndBind(sql, bindArgs);
    if (errCode != E_OK) {
        return errCode;
    }

    errCode = statement.Step();
    if (errCode != SQLITE_ROW) {
        statement.ResetStatementAndClearBindings();
        return E_NO_ROW_IN_QUERY;
    }

    errCode = statement.GetColumnLong(0, outValue);
    if (errCode != E_OK) {
        statement.ResetStatementAndClearBindings();
        return errCode;
    }

    errCode = statement.ResetStatementAndClearBindings();
    return errCode;
}

int SqliteConnection::ExecuteGetString(
    std::string &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = PrepareAndBind(sql, bindArgs);
    if (errCode != E_OK) {
        return errCode;
    }

    errCode = statement.Step();
    if (errCode != SQLITE_ROW) {
        statement.ResetStatementAndClearBindings();
        return E_NO_ROW_IN_QUERY;
    }

    errCode = statement.GetColumnString(0, outValue);
    if (errCode != E_OK) {
        statement.ResetStatementAndClearBindings();
        return errCode;
    }

    errCode = statement.ResetStatementAndClearBindings();
    return errCode;
}

std::shared_ptr<SqliteStatement> SqliteConnection::BeginStepQuery(
    int &errCode, const std::string &sql, const std::vector<std::string> &selectionArgs)
{
    errCode = stepStatement->Prepare(dbHandle, sql);
    if (errCode != E_OK) {
        return nullptr;
    }

    std::vector<ValueObject> bindArgs;
    for (auto item : selectionArgs) {
        bindArgs.push_back(ValueObject(item));
    }

    errCode = stepStatement->BindArguments(bindArgs);
    if (errCode != E_OK) {
        return nullptr;
    }

    return stepStatement;
}

int SqliteConnection::EndStepQuery()
{
    return stepStatement->ResetStatementAndClearBindings();
}

int SqliteConnection::ChangeEncryptKey(const std::vector<uint8_t> &newKey)
{
    int errCode = sqlite3_rekey(dbHandle, static_cast<const void *>(newKey.data()), newKey.size());
    if (errCode != SQLITE_OK) {
        LOG_ERROR("SqliteConnection ChangeEncryptKey fail, err = %{public}d", errCode);
        return SQLiteError::ErrNo(errCode);
    }

    errCode = statement.Finalize();
    if (errCode != SQLITE_OK) {
        return errCode;
    }

    stepStatement->Finalize();
    if (errCode != SQLITE_OK) {
        return errCode;
    }

    return E_OK;
}

void SqliteConnection::LimitPermission(const std::string &dbPath) const
{
    struct stat st = { 0 };
    if (stat(dbPath.c_str(), &st) == 0) {
        if ((st.st_mode & (S_IXUSR | S_IXGRP | S_IRWXO)) != 0) {
            int ret = chmod(dbPath.c_str(), st.st_mode & (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP));
            if (ret != 0) {
                LOG_ERROR("SqliteConnection LimitPermission chmod fail, err = %{public}d", errno);
            }
        }
    } else {
        LOG_ERROR("SqliteConnection LimitPermission stat fail, err = %{public}d", errno);
    }
}
} // namespace NativeRdb
} // namespace OHOS
