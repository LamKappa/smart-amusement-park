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
#ifndef UPDATE_PROCESSOR_H
#define UPDATE_PROCESSOR_H

#include <cstdio>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include "applypatch/data_writer.h"
#include "pkg_manager.h"
#include "script_instruction.h"
#include "script_manager.h"

using uscript::UScriptEnv;
using uscript::UScriptInstructionFactory;
using uscript::UScriptInstructionFactoryPtr;
using uscript::UScriptInstructionPtr;

namespace updater {
class UpdaterEnv : public UScriptEnv {
public:
    UpdaterEnv(hpackage::PkgManager::PkgManagerPtr pkgManager, FILE* pipeWrite, bool retry) :
        UScriptEnv(pkgManager), pipeWrite_(pipeWrite), isRetry_(retry) {}
    virtual ~UpdaterEnv();

    virtual void PostMessage(const std::string &cmd, std::string content);
    virtual UScriptInstructionFactoryPtr GetInstructionFactory();
    virtual const std::vector<std::string> GetInstructionNames() const;
    virtual bool IsRetry() const
    {
        return isRetry_;
    }
private:
    UScriptInstructionFactoryPtr factory_ = nullptr;
    FILE* pipeWrite_ = nullptr;
    bool isRetry_ = false;
};

class UpdaterInstructionFactory : public UScriptInstructionFactory {
public:
    virtual int32_t CreateInstructionInstance(UScriptInstructionPtr& instr, const std::string& name);
    UpdaterInstructionFactory() {}
    virtual ~UpdaterInstructionFactory() {}
};

class UScriptInstructionSparseImageWrite : public uscript::UScriptInstruction {
public:
    UScriptInstructionSparseImageWrite() {}
    virtual ~UScriptInstructionSparseImageWrite() {}
    int32_t Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context) override;
};

class UScriptInstructionRawImageWrite : public uscript::UScriptInstruction {
public:
    UScriptInstructionRawImageWrite() {}
    virtual ~UScriptInstructionRawImageWrite() {}
    int32_t Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context) override;
private:
    static int RawImageWriteProcessor(const hpackage::PkgBuffer &buffer, size_t size, size_t start, bool isFinish,
        const void* context);
};
} // updater

enum EXIT_CODES {
    EXIT_INVALID_ARGS = EXIT_SUCCESS + 1,
    EXIT_READ_PACKAGE_ERROR = 2,
    EXIT_FOUND_SCRIPT_ERROR = 3,
    EXIT_PARSE_SCRIPT_ERROR = 4,
    EXIT_EXEC_SCRIPT_ERROR = 5,
};

int ProcessUpdater(bool retry, int pipeFd, const std::string &packagePath, const std::string &keyPath);

#endif /* UPDATE_PROCESSOR_H */
