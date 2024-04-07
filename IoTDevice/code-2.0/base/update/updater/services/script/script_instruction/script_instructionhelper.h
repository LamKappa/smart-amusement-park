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

#ifndef USCRIPT_INSTRUCTION_HELPER_H
#define USCRIPT_INSTRUCTION_HELPER_H

#include "script_instruction.h"
#include "script_manager_impl.h"
#include "script_utils.h"

namespace uscript {
class ScriptInstructionHelper {
public:
    explicit ScriptInstructionHelper(ScriptManagerImpl *impl) : scriptManager_(impl) {}

    ~ScriptInstructionHelper();

    int32_t RegisterInstructions() const;

    bool IsReservedInstruction(const std::string &scriptName) const;

    int32_t AddScript(const std::string &scriptName, int32_t priority);

    int32_t AddInstruction(const std::string &instrName, const UScriptInstructionPtr instr);

    int32_t RegisterUserInstruction(const std::string &libName, const std::string &instrName);

    static ScriptInstructionHelper* GetBasicInstructionHelper(ScriptManagerImpl *impl = nullptr);

    static void ReleaseBasicInstructionHelper();
private:
    void* instrLib_ = nullptr;
    std::string userInstrLibName_ {};
    ScriptManagerImpl* scriptManager_ {};
};
} // namespace uscript
#endif // USCRIPT_INSTRUCTION_HELPER_H
