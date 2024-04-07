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

#ifndef NATIVE_RDB_RDB_STORE_IMPL_H
#define NATIVE_RDB_RDB_STORE_IMPL_H

#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

#include "rdb_store.h"
#include "rdb_store_config.h"
#include "sqlite_connection_pool.h"
#include "sqlite_statement.h"
#include "store_session.h"

namespace OHOS {
namespace NativeRdb {

class RdbStoreImpl : public RdbStore, public std::enable_shared_from_this<RdbStoreImpl> {
public:
    static std::shared_ptr<RdbStore> Open(const RdbStoreConfig &config, int &errCode);
    RdbStoreImpl();
    ~RdbStoreImpl() override;

    int Insert(int64_t &outRowId, const std::string &table, const ValuesBucket &initialValues) override;
    int Replace(int64_t &outRowId, const std::string &table, const ValuesBucket &initialValues) override;
    int InsertWithConflictResolution(int64_t &outRowId, const std::string &table, const ValuesBucket &initialValues,
        ConflictResolution conflictResolution) override;
    int Update(int &changedRows, const std::string &table, const ValuesBucket &values, const std::string &whereClause,
        const std::vector<std::string> &whereArgs) override;
    int UpdateWithConflictResolution(int &changedRows, const std::string &table, const ValuesBucket &values,
        const std::string &whereClause, const std::vector<std::string> &whereArgs,
        ConflictResolution conflictResolution) override;
    int Delete(int &deletedRows, const std::string &table, const std::string &whereClause,
        const std::vector<std::string> &whereArgs) override;
    std::unique_ptr<ResultSet> Query(int &errCode, bool distinct, const std::string &table,
        const std::vector<std::string> &columns, const std::string &selection,
        const std::vector<std::string> &selectionArgs, const std::string &groupBy, const std::string &having,
        const std::string &orderBy, const std::string &limit) override;
    std::unique_ptr<ResultSet> QuerySql(const std::string &sql, const std::vector<std::string> &selectionArgs) override;
    int ExecuteSql(const std::string &sql, const std::vector<ValueObject> &bindArgs) override;
    int ExecuteAndGetLong(int64_t &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs) override;
    int ExecuteAndGetString(
        std::string &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs) override;
    int GetVersion(int &version) override;
    int SetVersion(int version) override;
    int BeginTransaction() override;
    int MarkAsCommit() override;
    int EndTransaction() override;
    bool IsInTransaction() override;
    int ChangeEncryptKey(const std::vector<uint8_t> &newKey) override;
    std::shared_ptr<SqliteStatement> BeginStepQuery(
        int &errCode, const std::string sql, const std::vector<std::string> &bindArgs);
    int EndStepQuery();

private:
    int InnerOpen(const RdbStoreConfig &config);
    std::shared_ptr<StoreSession> GetThreadSession();
    void ReleaseThreadSession();
    int CheckAttach(const std::string &sql);

    SqliteConnectionPool *connectionPool;
    static const int MAX_IDLE_SESSION_SIZE = 5;
    std::mutex sessionMutex;
    std::map<std::thread::id, std::pair<std::shared_ptr<StoreSession>, int>> threadMap;
    std::list<std::shared_ptr<StoreSession>> idleSessions;
};

} // namespace NativeRdb
} // namespace OHOS
#endif