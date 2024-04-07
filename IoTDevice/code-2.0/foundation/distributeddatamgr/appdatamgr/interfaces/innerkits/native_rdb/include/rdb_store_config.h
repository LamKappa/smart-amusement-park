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

#ifndef NATIVE_RDB_RDB_STORE_CONFIG_H
#define NATIVE_RDB_RDB_STORE_CONFIG_H

#include <string>
#include <vector>

namespace OHOS {
namespace NativeRdb {

/* indicates the type of the storage */
enum class StorageMode {
    MODE_MEMORY = 101,
    MODE_DISK,
};

enum class JournalMode {
    MODE_DELETE,
    MODE_TRUNCATE,
    MODE_PERSIST,
    MODE_MEMORY,
    MODE_WAL,
    MODE_OFF,
};

class RdbStoreConfig {
public:
    RdbStoreConfig(const std::string &path, StorageMode storageMode = StorageMode::MODE_DISK, bool readOnly = false,
        const std::vector<uint8_t> &encryptKey = std::vector<uint8_t>());
    ~RdbStoreConfig();
    /* set the journal mode, if not set, the default mode is WAL */
    void SetJournalMode(JournalMode journalMode);
    std::string GetPath() const;
    StorageMode GetStorageMode() const;
    std::string GetJournalMode() const;
    bool IsReadOnly() const;
    std::vector<uint8_t> GetEncryptKey() const;
    void ClearEncryptKey();

private:
    std::string path;
    StorageMode storageMode;
    bool readOnly;
    std::vector<uint8_t> encryptKey;
    std::string journalMode;
};

} // namespace NativeRdb
} // namespace OHOS
#endif
