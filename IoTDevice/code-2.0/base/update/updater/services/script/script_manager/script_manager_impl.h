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
#ifndef USCRIPT_MANAGER_IMPL_H
#define USCRIPT_MANAGER_IMPL_H

#include <map>
#include <memory>
#include <vector>
#include "pkg_manager.h"
#include "script_instruction.h"
#include "script_manager.h"
#include "thread_pool.h"

namespace uscript {
class ScriptInstructionHelper;
class ScriptManagerImpl : public ScriptManager {
public:
    friend class ScriptInterpreter;
    friend class ScriptInstructionHelper;

    explicit ScriptManagerImpl(UScriptEnv *env) : scriptEnv_(env) {}
    virtual ~ScriptManagerImpl();
    int32_t Init();
    virtual int32_t ExecuteScript(int32_t priority) override;

private:
    int32_t ExtractAndExecuteScript(hpackage::PkgManager::PkgManagerPtr manager,
        const std::string &scriptName);
    int32_t AddScript(const std::string &instrName, int32_t priority);
    int32_t AddInstruction(const std::string &instrName, const UScriptInstructionPtr instruction);
    UScriptInstruction* FindInstruction(const std::string &instrName);
    UScriptEnv* GetScriptEnv(const std::string &instrName) const;
    int32_t RegisterInstruction(ScriptInstructionHelper &helper);
private:
    static const int32_t MAX_THREAD_POOL = 4;
    std::map<std::string, UScriptInstructionPtr> scriptInstructions_;
    std::vector<std::string> scriptFiles_[MAX_PRIORITY] {};
    ThreadPool *threadPool_ = nullptr;
    UScriptEnv *scriptEnv_ = nullptr;
};
} // namespace uscript
#endif
