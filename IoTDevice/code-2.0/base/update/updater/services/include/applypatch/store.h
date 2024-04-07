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
#ifndef UPDATER_STORE_H
#define UPDATER_STORE_H

#include <string>
#include <unordered_map>
#include <openssl/sha.h>
#include "applypatch/block_set.h"
#include "applypatch/command.h"

namespace updater {
class Store {
public:
    // Create new store space
    static int32_t CreateNewSpace(const std::string &path, bool needClear);
    static int32_t DoFreeSpace(const std::string &directoryPath);
    static int32_t FreeStore(const std::string &dirPath, const std::string &fileName);
    // Write data to store space by id
    static int32_t WriteDataToStore(const std::string &dirPath, const std::string &fileName,
        const std::vector<uint8_t> &buffer, int size);
    // Load data from store by id
    static int32_t LoadDataFromStore(const std::string &dirPath, const std::string &fileName,
        std::vector<uint8_t> &buffer);
};
} // namespace updater
#endif // UPDATER_STORE_H
