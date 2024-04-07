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
#include "applypatch/transfer_manager.h"
#include <fcntl.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include "applypatch/command_function.h"
#include "log/log.h"
#include "utils.h"

namespace updater {
using namespace updater::utils;
static TransferManagerPtr g_transferManagerInstance = nullptr;
TransferManagerPtr TransferManager::GetTransferManagerInstance()
{
    if (g_transferManagerInstance == nullptr) {
        g_transferManagerInstance = new TransferManager();
        g_transferManagerInstance->Init();
    }
    return g_transferManagerInstance;
}

void TransferManager::ReleaseTransferManagerInstance(TransferManagerPtr transferManager)
{
    delete transferManager;
    g_transferManagerInstance = nullptr;
    transferManager = nullptr;
}

TransferManager::~TransferManager()
{
    if (globalParams != nullptr) {
        if (globalParams->writerThreadInfo != nullptr) {
            globalParams->writerThreadInfo.reset();
        }
        globalParams.reset();
    }
}

bool TransferManager::CommandsParser(int fd, const std::vector<std::string> &context)
{
    UPDATER_ERROR_CHECK(context.size() >= 1, "too small context in transfer file", return false);
    std::vector<std::string>::const_iterator ct = context.begin();
    globalParams->version = utils::String2Int<size_t>(*ct++, utils::N_DEC);
    globalParams->blockCount = utils::String2Int<size_t>(*ct++, utils::N_DEC);
    globalParams->maxEntries = utils::String2Int<size_t>(*ct++, utils::N_DEC);
    globalParams->maxBlocks = utils::String2Int<size_t>(*ct++, utils::N_DEC);
    std::string retryCmd = "";
    if (globalParams != nullptr && globalParams->env != nullptr && globalParams->env->IsRetry()) {
        retryCmd = ReloadForRetry();
    }
    std::unique_ptr<Command> cmd;
    while (ct != context.end()) {
        cmd = std::make_unique<Command>();
        // null pointer, return false
        UPDATER_ERROR_CHECK(cmd != nullptr, "Failed to parse command line.", return false);
        if (cmd->Init(*ct) && cmd->GetCommandType() != CommandType::LAST) {
            if (!retryCmd.empty() && globalParams->env->IsRetry()) {
                if (*ct == retryCmd) {
                    retryCmd.clear();
                }
                if (cmd->GetCommandType() != CommandType::NEW) {
                    LOG(INFO) << "Retry: Command " << *ct << " passed";
                    cmd.reset();
                    ct++;
                    continue;
                }
            }
            cmd->SetFileDescriptor(fd);
            std::unique_ptr<CommandFunction> cf = CommandFunctionFactory::GetCommandFunction(cmd->GetCommandType());
            if (cf == nullptr) {
                LOG(ERROR) << "Failed to get cmd exec";
                return false;
            }
            CommandResult ret = cf->Execute(const_cast<Command &>(*cmd.get()));
            CommandFunctionFactory::ReleaseCommandFunction(cf);
            if (CheckResult(ret, cmd->GetCommandLine()) == false) {
                return false;
            }
            LOG(INFO) << "Running command : " << cmd->GetArgumentByPos(0) << " success";
        }
        cmd.reset();
        ct++;
    }
    return true;
}

void TransferManager::Init()
{
    globalParams = std::make_unique<GlobalParams>();
    globalParams->writerThreadInfo = std::make_unique<WriterThreadInfo>();
    blocksetMap.clear();
}

bool TransferManager::RegisterForRetry(const std::string &cmd)
{
    std::string path = globalParams->storeBase + "/" + "retry_flag";
    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_OPEN_PERMISSION);
    UPDATER_ERROR_CHECK(fd != -1, "Failed to create", return false);
    UPDATER_ERROR_CHECK(fchown(fd, S_USER_PERMISSION, S_USER_PERMISSION) == 0,
        "Failed to chown", close(fd); return -1);
    bool ret = utils::WriteStringToFile(fd, cmd);
    UPDATER_ERROR_CHECK_NOT_RETURN(ret, "Write retry flag error");
    fsync(fd);
    close(fd);

    return ret;
}

std::string TransferManager::ReloadForRetry() const
{
    std::string path = globalParams->storeBase + "/" + "retry_flag";
    int fd = open(path.c_str(), O_RDONLY);
    UPDATER_ERROR_CHECK(fd >= 0, "Failed to open", return "");
    (void)lseek(fd, 0, SEEK_SET);
    std::string cmd = "";
    UPDATER_ERROR_CHECK_NOT_RETURN(utils::ReadFileToString(fd, cmd), "Error to read retry flag");
    close(fd);
    return cmd;
}

bool TransferManager::CheckResult(const CommandResult result, const std::string &cmd)
{
    switch (result) {
        case SUCCESS:
            RegisterForRetry(cmd);
            break;
        case NEED_RETRY:
            LOG(INFO) << "Running command need retry!";
            UPDATER_CHECK_ONLY_RETURN(!globalParams->env, globalParams->env->PostMessage("retry_update", cmd));
            return false;
            break;
        case FAILED:
        default:
            LOG(ERROR) << "Running command failed";
            return false;
            break;
    }
    return true;
}
} // namespace updater
