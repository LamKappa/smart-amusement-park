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

#include "store_session.h"

#include "logger.h"
#include "rdb_errno.h"
#include "sqlite_utils.h"

namespace OHOS {
namespace NativeRdb {
StoreSession::StoreSession(SqliteConnectionPool &connectionPool)
    : connectionPool(connectionPool), connection(nullptr), connectionUseCount(0), isInStepQuery(false),
      transactionStack()
{
}

StoreSession::~StoreSession()
{
}

void StoreSession::AcquireConnection(bool isReadOnly)
{
    if (connection == nullptr) {
        connection = connectionPool.AcquireConnection(isReadOnly);
    }

    connectionUseCount += 1;
}

void StoreSession::ReleaseConnection()
{
    if ((connection == nullptr) || (connectionUseCount <= 0)) {
        LOG_ERROR("SQLiteSession ReleaseConnection repeated release");
        return;
    }

    if (--connectionUseCount == 0) {
        connectionPool.ReleaseConnection(connection);
        connection = nullptr;
    }
}

int StoreSession::BeginExecuteSql(const std::string &sql)
{
    int type = SqliteUtils::GetSqlStatementType(sql);
    if (SqliteUtils::IsSpecial(type)) {
        return E_TRANSACTION_IN_EXECUTE;
    }

    bool assumeReadOnly = SqliteUtils::IsSqlReadOnly(type);
    bool isReadOnly = false;
    AcquireConnection(assumeReadOnly);
    int errCode = connection->Prepare(sql, isReadOnly);
    if (errCode != 0) {
        ReleaseConnection();
        return errCode;
    }

    if (isReadOnly == connection->IsWriteConnection()) {
        ReleaseConnection();
        AcquireConnection(isReadOnly);
        if (!isReadOnly && !connection->IsWriteConnection()) {
            LOG_ERROR("StoreSession BeginExecutea : read connection can not execute write operation");
            ReleaseConnection();
            return E_EXECUTE_WRITE_IN_READ_CONNECTION;
        }
    }

    return E_OK;
}
int StoreSession::ExecuteSql(const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = BeginExecuteSql(sql);
    if (errCode != 0) {
        return errCode;
    }

    errCode = connection->ExecuteSql(sql, bindArgs);
    ReleaseConnection();
    return errCode;
}

int StoreSession::ExecuteForChangedRowCount(
    int &changedRows, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = BeginExecuteSql(sql);
    if (errCode != 0) {
        return errCode;
    }

    errCode = connection->ExecuteForChangedRowCount(changedRows, sql, bindArgs);
    ReleaseConnection();
    return errCode;
}

int StoreSession::ExecuteForLastInsertedRowId(
    int64_t &outRowId, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = BeginExecuteSql(sql);
    if (errCode != 0) {
        return errCode;
    }

    errCode = connection->ExecuteForLastInsertedRowId(outRowId, sql, bindArgs);
    ReleaseConnection();
    return errCode;
}

int StoreSession::ExecuteGetLong(int64_t &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = BeginExecuteSql(sql);
    if (errCode != 0) {
        return errCode;
    }

    errCode = connection->ExecuteGetLong(outValue, sql, bindArgs);
    ReleaseConnection();
    return errCode;
}

int StoreSession::ExecuteGetString(
    std::string &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = BeginExecuteSql(sql);
    if (errCode != 0) {
        return errCode;
    }

    errCode = connection->ExecuteGetString(outValue, sql, bindArgs);
    ReleaseConnection();
    return errCode;
}

int StoreSession::BeginTransaction()
{
    if (transactionStack.empty()) {
        AcquireConnection(false);
        if (!connection->IsWriteConnection()) {
            LOG_ERROR("StoreSession BeginExecutea : read connection can not begin transaction");
            ReleaseConnection();
            return E_BEGIN_TRANSACTION_IN_READ_CONNECTION;
        }

        int errCode = connection->ExecuteSql("BEGIN EXCLUSIVE;");
        if (errCode != E_OK) {
            ReleaseConnection();
            return errCode;
        }
    }

    Transaction transaction;
    transactionStack.push(transaction);
    return E_OK;
}
int StoreSession::MarkAsCommit()
{
    if (transactionStack.empty()) {
        return E_NO_TRANSACTION_IN_SESSION;
    }
    transactionStack.top().SetMarkedSuccessful(true);
    return E_OK;
}
int StoreSession::EndTransaction()
{
    if (transactionStack.empty()) {
        return E_NO_TRANSACTION_IN_SESSION;
    }

    Transaction transaction = transactionStack.top();
    bool isSucceed = transaction.IsAllBeforeSuccessful() && transaction.IsMarkedSuccessful();
    transactionStack.pop();
    if (!transactionStack.empty()) {
        if (!isSucceed) {
            transactionStack.top().SetAllBeforeSuccessful(false);
        }
    } else {
        int errCode = connection->ExecuteSql(isSucceed ? "COMMIT;" : "ROLLBACK;");
        ReleaseConnection();
        return errCode;
    }

    return E_OK;
}
bool StoreSession::IsInTransaction() const
{
    return !transactionStack.empty();
}

std::shared_ptr<SqliteStatement> StoreSession::BeginStepQuery(
    int &errCode, const std::string &sql, const std::vector<std::string> &selectionArgs)
{
    if (isInStepQuery == true) {
        LOG_ERROR("StoreSession BeginStepQuery fail : begin more step query in one session !");
        errCode = E_MORE_STEP_QUERY_IN_ONE_SESSION;
        return nullptr; // fail,already in
    }

    if (SqliteUtils::GetSqlStatementType(sql) != SqliteUtils::STATEMENT_SELECT) {
        LOG_ERROR("StoreSession BeginStepQuery fail : not select sql !");
        errCode = E_EXECUTE_IN_STEP_QUERY;
        return nullptr;
    }

    AcquireConnection(true);
    std::shared_ptr<SqliteStatement> statement = connection->BeginStepQuery(errCode, sql, selectionArgs);
    if (statement == nullptr) {
        ReleaseConnection();
        return nullptr;
    }
    isInStepQuery = true;
    return statement;
}

int StoreSession::EndStepQuery()
{
    if (isInStepQuery == false) {
        return E_OK;
    }

    int errCode = connection->EndStepQuery();
    isInStepQuery = false;
    ReleaseConnection();
    return errCode;
}

StoreSession::Transaction::Transaction() : isAllBeforeSuccessful(true), isMarkedSuccessful(false)
{
}

StoreSession::Transaction::~Transaction()
{
}

bool StoreSession::Transaction::IsAllBeforeSuccessful() const
{
    return isAllBeforeSuccessful;
}

void StoreSession::Transaction::SetAllBeforeSuccessful(bool isAllBeforeSuccessful)
{
    this->isAllBeforeSuccessful = isAllBeforeSuccessful;
}

bool StoreSession::Transaction::IsMarkedSuccessful() const
{
    return isMarkedSuccessful;
}

void StoreSession::Transaction::SetMarkedSuccessful(bool isMarkedSuccessful)
{
    this->isMarkedSuccessful = isMarkedSuccessful;
}
} // namespace NativeRdb
} // namespace OHOS
