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

#ifndef NATIVE_RDB_RDB_STORE_SESSION_H
#define NATIVE_RDB_RDB_STORE_SESSION_H

#include <memory>
#include <stack>

#include "sqlite_connection.h"
#include "sqlite_connection_pool.h"
#include "value_object.h"

namespace OHOS {
namespace NativeRdb {

class StoreSession {
public:
    explicit StoreSession(SqliteConnectionPool &connectionPool);
    ~StoreSession();
    int ExecuteSql(const std::string &sql, const std::vector<ValueObject> &bindArgs);
    int ExecuteForChangedRowCount(int &changedRows, const std::string &sql, const std::vector<ValueObject> &bindArgs);
    int ExecuteForLastInsertedRowId(
        int64_t &outRowId, const std::string &sql, const std::vector<ValueObject> &bindArgs);
    int ExecuteGetLong(int64_t &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs);
    int ExecuteGetString(std::string &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs);
    int BeginTransaction();
    int MarkAsCommit();
    int EndTransaction();
    bool IsInTransaction() const;
    std::shared_ptr<SqliteStatement> BeginStepQuery(
        int &errCode, const std::string &sql, const std::vector<std::string> &selectionArgs);
    int EndStepQuery();

private:
    class Transaction {
    public:
        Transaction();
        ~Transaction();
        bool IsAllBeforeSuccessful() const;
        void SetAllBeforeSuccessful(bool isAllBeforeSuccessful);
        bool IsMarkedSuccessful() const;
        void SetMarkedSuccessful(bool isMarkedSuccessful);

    private:
        bool isAllBeforeSuccessful;
        bool isMarkedSuccessful;
    };
    void AcquireConnection(bool isReadOnly);
    void ReleaseConnection();
    int BeginExecuteSql(const std::string &sql);
    SqliteConnectionPool &connectionPool;
    SqliteConnection *connection;
    int connectionUseCount;
    bool isInStepQuery;
    std::stack<Transaction> transactionStack;
};

} // namespace NativeRdb
} // namespace OHOS
#endif