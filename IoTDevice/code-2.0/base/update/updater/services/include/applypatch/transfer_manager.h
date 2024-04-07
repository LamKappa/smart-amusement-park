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
#ifndef USCRIPT_TRANSFERLIST_H
#define USCRIPT_TRANSFERLIST_H

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "applypatch/command.h"
#include "command.h"
#include "script_instruction.h"
#include "script_manager.h"


namespace updater {
static std::unordered_map<std::string, BlockSet> blocksetMap;

struct WriterThreadInfo {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    BlockSet bs;
    std::unique_ptr<BlockWriter> writer;
    bool readyToWrite;
    uscript::UScriptEnv *env;
    hpackage::PkgManager::FileInfoPtr fileInfo;
    std::string newPatch;
};

struct TransferParams {
    size_t version;
    size_t blockCount;
    size_t maxEntries;
    size_t maxBlocks;
    size_t written;
    pthread_t thread;
    uscript::UScriptEnv *env;
    std::unique_ptr<WriterThreadInfo> writerThreadInfo;
    int storeCreated;
    std::string storeBase;
    uint8_t *patchDataBuffer;
    size_t patchDataSize;
};
using GlobalParams = TransferParams;
class TransferManager;
using TransferManagerPtr = TransferManager *;
class TransferManager {
public:
    TransferManager() {}
    static TransferManagerPtr GetTransferManagerInstance();
    static void ReleaseTransferManagerInstance(TransferManagerPtr transferManager);
    virtual ~TransferManager();

    void Init();
    bool CommandsParser(int fd, const std::vector<std::string> &context);

    GlobalParams* GetGlobalParams()
    {
        return globalParams.get();
    }
    std::string ReloadForRetry() const;
    bool CheckResult(const CommandResult result, const std::string &cmd);

private:
    bool RegisterForRetry(const std::string &cmd);
    std::unique_ptr<GlobalParams> globalParams;
};
} // namespace updater
#endif
