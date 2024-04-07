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

#include "rdb_store_impl.h"

#include <sstream>

#include "logger.h"
#include "rdb_errno.h"
#include "sqlite_sql_builder.h"
#include "sqlite_utils.h"
#include "step_result_set.h"

namespace OHOS {
namespace NativeRdb {
std::shared_ptr<RdbStore> RdbStoreImpl::Open(const RdbStoreConfig &config, int &errCode)
{
    std::shared_ptr<RdbStoreImpl> rdbStore = std::make_shared<RdbStoreImpl>();
    errCode = rdbStore->InnerOpen(config);
    if (errCode != E_OK) {
        return nullptr;
    }

    return rdbStore;
}

int RdbStoreImpl::InnerOpen(const RdbStoreConfig &config)
{
    int errCode = E_OK;
    connectionPool = SqliteConnectionPool::Create(config, errCode);
    if (connectionPool == nullptr) {
        return errCode;
    }
    return E_OK;
}

RdbStoreImpl::RdbStoreImpl() : connectionPool(nullptr)
{
}

RdbStoreImpl::~RdbStoreImpl()
{
    if (connectionPool != nullptr) {
        delete connectionPool;
    }

    threadMap.clear();
    idleSessions.clear();
}

std::shared_ptr<StoreSession> RdbStoreImpl::GetThreadSession()
{
    std::thread::id tid = std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(sessionMutex);

    auto iter = threadMap.find(tid);
    if (iter != threadMap.end()) {
        iter->second.second++; // useCount++
        return iter->second.first;
    }

    // get from idle stack
    std::shared_ptr<StoreSession> session;
    if (idleSessions.empty()) {
        session = std::make_shared<StoreSession>(*connectionPool);
    } else {
        session = idleSessions.back();
        idleSessions.pop_back();
    }

    threadMap.insert(std::make_pair(tid, std::make_pair(session, 1))); // useCount is 1
    return session;
}

void RdbStoreImpl::ReleaseThreadSession()
{
    std::thread::id tid = std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(sessionMutex);

    auto iter = threadMap.find(tid);
    if (iter == threadMap.end()) {
        LOG_ERROR("RdbStoreImpl ReleaseThreadSession: no session found for the current thread");
        return;
    }
    int &useCount = iter->second.second;
    useCount--;
    if (useCount > 0) {
        return;
    }

    if (idleSessions.size() < MAX_IDLE_SESSION_SIZE) {
        idleSessions.push_back(iter->second.first);
    }
    threadMap.erase(iter);
}

int RdbStoreImpl::Insert(int64_t &outRowId, const std::string &table, const ValuesBucket &initialValues)
{
    return InsertWithConflictResolution(outRowId, table, initialValues, ConflictResolution::ON_CONFLICT_NONE);
}

int RdbStoreImpl::Replace(int64_t &outRowId, const std::string &table, const ValuesBucket &initialValues)
{
    return InsertWithConflictResolution(outRowId, table, initialValues, ConflictResolution::ON_CONFLICT_REPLACE);
}

int RdbStoreImpl::InsertWithConflictResolution(int64_t &outRowId, const std::string &table,
    const ValuesBucket &initialValues, ConflictResolution conflictResolution)
{
    if (table.empty()) {
        return E_EMPTY_TABLE_NAME;
    }

    if (initialValues.IsEmpty()) {
        return E_EMPTY_VALUES_BUCKET;
    }

    std::string conflictClause;
    int errCode = SqliteUtils::GetConflictClause(static_cast<int>(conflictResolution), conflictClause);
    if (errCode != E_OK) {
        return errCode;
    }

    std::stringstream sql;
    sql << "INSERT" << conflictClause << " INTO " << table << '(';

    std::map<std::string, ValueObject> valuesMap;
    initialValues.GetAll(valuesMap);
    std::vector<ValueObject> bindArgs;
    for (auto iter = valuesMap.begin(); iter != valuesMap.end(); iter++) {
        sql << ((iter == valuesMap.begin()) ? "" : ",");
        sql << iter->first;               // columnName
        bindArgs.push_back(iter->second); // columnValue
    }

    sql << ") VALUES (";
    for (size_t i = 0; i < valuesMap.size(); i++) {
        sql << ((i == 0) ? "?" : ",?");
    }
    sql << ')';

    std::shared_ptr<StoreSession> session = GetThreadSession();
    errCode = session->ExecuteForLastInsertedRowId(outRowId, sql.str(), bindArgs);
    ReleaseThreadSession();
    return errCode;
}

int RdbStoreImpl::Update(int &changedRows, const std::string &table, const ValuesBucket &values,
    const std::string &whereClause, const std::vector<std::string> &whereArgs)
{
    return UpdateWithConflictResolution(
        changedRows, table, values, whereClause, whereArgs, ConflictResolution::ON_CONFLICT_NONE);
}

int RdbStoreImpl::UpdateWithConflictResolution(int &changedRows, const std::string &table, const ValuesBucket &values,
    const std::string &whereClause, const std::vector<std::string> &whereArgs, ConflictResolution conflictResolution)
{
    if (table.empty()) {
        return E_EMPTY_TABLE_NAME;
    }

    if (values.IsEmpty()) {
        return E_EMPTY_VALUES_BUCKET;
    }

    std::string conflictClause;
    int errCode = SqliteUtils::GetConflictClause(static_cast<int>(conflictResolution), conflictClause);
    if (errCode != E_OK) {
        return errCode;
    }

    std::stringstream sql;
    sql << "UPDATE" << conflictClause << " " << table << " SET ";

    std::map<std::string, ValueObject> valuesMap;
    values.GetAll(valuesMap);
    std::vector<ValueObject> bindArgs;
    for (auto iter = valuesMap.begin(); iter != valuesMap.end(); iter++) {
        sql << ((iter == valuesMap.begin()) ? "" : ",");
        sql << iter->first << "=?";       // columnName
        bindArgs.push_back(iter->second); // columnValue
    }

    if (whereClause.empty() == false) {
        sql << " WHERE " << whereClause;
    }

    for (auto &iter : whereArgs) {
        bindArgs.push_back(ValueObject(iter));
    }

    std::shared_ptr<StoreSession> session = GetThreadSession();
    errCode = session->ExecuteForChangedRowCount(changedRows, sql.str(), bindArgs);
    ReleaseThreadSession();
    return errCode;
}

int RdbStoreImpl::Delete(int &deletedRows, const std::string &table, const std::string &whereClause,
    const std::vector<std::string> &whereArgs)
{
    if (table.empty()) {
        return E_EMPTY_TABLE_NAME;
    }

    std::stringstream sql;
    sql << "DELETE FROM " << table;
    if (whereClause.empty() == false) {
        sql << " WHERE " << whereClause;
    }

    std::vector<ValueObject> bindArgs;
    for (auto &iter : whereArgs) {
        bindArgs.push_back(ValueObject(iter));
    }

    std::shared_ptr<StoreSession> session = GetThreadSession();
    int errCode = session->ExecuteForChangedRowCount(deletedRows, sql.str(), bindArgs);
    ReleaseThreadSession();
    return errCode;
}

std::unique_ptr<ResultSet> RdbStoreImpl::Query(int &errCode, bool distinct, const std::string &table,
    const std::vector<std::string> &columns, const std::string &selection,
    const std::vector<std::string> &selectionArgs, const std::string &groupBy, const std::string &having,
    const std::string &orderBy, const std::string &limit)
{
    std::string sql;
    errCode =
        SqliteSqlBuilder::BuildQueryString(distinct, table, columns, selection, groupBy, having, orderBy, limit, sql);
    if (errCode != E_OK) {
        return nullptr;
    }

    return QuerySql(sql, selectionArgs);
}

std::unique_ptr<ResultSet> RdbStoreImpl::QuerySql(const std::string &sql, const std::vector<std::string> &selectionArgs)
{
    std::unique_ptr<ResultSet> resultSet = std::make_unique<StepResultSet>(shared_from_this(), sql, selectionArgs);
    return resultSet;
}

int RdbStoreImpl::ExecuteSql(const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = CheckAttach(sql);
    if (errCode != E_OK) {
        return errCode;
    }

    std::shared_ptr<StoreSession> session = GetThreadSession();
    errCode = session->ExecuteSql(sql, bindArgs);
    if (errCode != E_OK) {
        ReleaseThreadSession();
        return errCode;
    }
    int sqlType = SqliteUtils::GetSqlStatementType(sql);
    if (sqlType == SqliteUtils::STATEMENT_DDL) {
        errCode = connectionPool->ReOpenAvailableReadConnections();
    }
    ReleaseThreadSession();
    return errCode;
}

int RdbStoreImpl::ExecuteAndGetLong(int64_t &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    std::shared_ptr<StoreSession> session = GetThreadSession();
    int errCode = session->ExecuteGetLong(outValue, sql, bindArgs);
    ReleaseThreadSession();
    return errCode;
}

int RdbStoreImpl::ExecuteAndGetString(
    std::string &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    std::shared_ptr<StoreSession> session = GetThreadSession();
    int errCode = session->ExecuteGetString(outValue, sql, bindArgs);
    ReleaseThreadSession();
    return errCode;
}
int RdbStoreImpl::GetVersion(int &version)
{
    int64_t value;
    int errCode = ExecuteAndGetLong(value, "PRAGMA user_version;", std::vector<ValueObject>());
    version = static_cast<int>(value);
    return errCode;
}

int RdbStoreImpl::SetVersion(int version)
{
    std::string sql = "PRAGMA user_version = " + std::to_string(version);
    return ExecuteSql(sql, std::vector<ValueObject>());
}

int RdbStoreImpl::BeginTransaction()
{
    std::shared_ptr<StoreSession> session = GetThreadSession();
    int errCode = session->BeginTransaction();
    if (errCode != E_OK) {
        ReleaseThreadSession();
        return errCode;
    }
    return E_OK;
}

int RdbStoreImpl::MarkAsCommit()
{
    std::shared_ptr<StoreSession> session = GetThreadSession();
    int errCode = session->MarkAsCommit();
    ReleaseThreadSession();
    return errCode;
}

int RdbStoreImpl::EndTransaction()
{
    std::shared_ptr<StoreSession> session = GetThreadSession();
    int errCode = session->EndTransaction();
    // release the session got in EndTransaction()
    ReleaseThreadSession();
    // release the session got in BeginTransaction()
    ReleaseThreadSession();
    return errCode;
}

bool RdbStoreImpl::IsInTransaction()
{
    std::shared_ptr<StoreSession> session = GetThreadSession();
    bool inTransaction = session->IsInTransaction();
    ReleaseThreadSession();
    return inTransaction;
}

int RdbStoreImpl::ChangeEncryptKey(const std::vector<uint8_t> &newKey)
{
    return connectionPool->ChangeEncryptKey(newKey);
}

std::shared_ptr<SqliteStatement> RdbStoreImpl::BeginStepQuery(
    int &errCode, const std::string sql, const std::vector<std::string> &bindArgs)
{
    std::shared_ptr<StoreSession> session = GetThreadSession();
    return session->BeginStepQuery(errCode, sql, bindArgs);
}

int RdbStoreImpl::EndStepQuery()
{
    std::shared_ptr<StoreSession> session = GetThreadSession();
    int err = session->EndStepQuery();
    ReleaseThreadSession(); // release session got by EndStepQuery
    ReleaseThreadSession(); // release session got by BeginStepQuery
    return err;
}

int RdbStoreImpl::CheckAttach(const std::string &sql)
{
    size_t index = sql.find_first_not_of(' ');
    if (index == std::string::npos) {
        return E_OK;
    }

    /* The first 3 characters can determine the type */
    std::string sqlType = sql.substr(index, 3);
    sqlType = SqliteUtils::StrToUpper(sqlType);
    if (sqlType != "ATT") {
        return E_OK;
    }

    std::string journalMode;
    int errCode = ExecuteAndGetString(journalMode, "PRAGMA journal_mode", std::vector<ValueObject>());
    if (errCode != E_OK) {
        LOG_ERROR("RdbStoreImpl CheckAttach fail to get journal mode : %{public}d", errCode);
        return errCode;
    }

    journalMode = SqliteUtils::StrToUpper(journalMode);
    if (journalMode == "WAL") {
        LOG_ERROR("RdbStoreImpl attach is not supported in WAL mode");
        return E_NOT_SUPPROTED_ATTACH_IN_WAL_MODE;
    }

    return E_OK;
}
} // namespace NativeRdb
} // namespace OHOS