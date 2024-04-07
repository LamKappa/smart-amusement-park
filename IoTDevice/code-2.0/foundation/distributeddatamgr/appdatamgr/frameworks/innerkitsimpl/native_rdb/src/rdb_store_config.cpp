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

#include "rdb_store_config.h"

namespace OHOS {
namespace NativeRdb {
RdbStoreConfig::RdbStoreConfig(
    const std::string &path, StorageMode storageMode, bool isReadOnly, const std::vector<uint8_t> &encryptKey)
    : path(path), storageMode(storageMode), readOnly(isReadOnly), encryptKey(encryptKey), journalMode("")
{
}

RdbStoreConfig::~RdbStoreConfig()
{
    ClearEncryptKey();
}

void RdbStoreConfig::SetJournalMode(JournalMode journalMode)
{
    switch (journalMode) {
        case JournalMode::MODE_DELETE:
            this->journalMode = "DELETE";
            break;
        case JournalMode::MODE_TRUNCATE:
            this->journalMode = "TRUNCATE";
            break;
        case JournalMode::MODE_PERSIST:
            this->journalMode = "PERSIST";
            break;
        case JournalMode::MODE_MEMORY:
            this->journalMode = "MEMORY";
            break;
        case JournalMode::MODE_WAL:
            this->journalMode = "WAL";
            break;
        case JournalMode::MODE_OFF:
            this->journalMode = "OFF";
            break;
        default:
            break;
    }
}

std::string RdbStoreConfig::GetPath() const
{
    return path;
}

StorageMode RdbStoreConfig::GetStorageMode() const
{
    return storageMode;
}

std::string RdbStoreConfig::GetJournalMode() const
{
    return journalMode;
}

bool RdbStoreConfig::IsReadOnly() const
{
    return readOnly;
}

std::vector<uint8_t> RdbStoreConfig::GetEncryptKey() const
{
    return encryptKey;
}

void RdbStoreConfig::ClearEncryptKey()
{
    std::fill(encryptKey.begin(), encryptKey.end(), 0);
    encryptKey.clear();
}
} // namespace NativeRdb
} // namespace OHOS