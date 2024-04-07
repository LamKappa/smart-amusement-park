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
#ifndef USCRIPT_UPDATE_PROCESSOR_H
#define USCRIPT_UPDATE_PROCESSOR_H

#include "script_instruction.h"
#include "script_manager.h"
#include "script_utils.h"

namespace BasicInstruction {
class UScriptInstructionSetProcess : public uscript::UScriptInstruction {
public:
    UScriptInstructionSetProcess() {}
    virtual ~UScriptInstructionSetProcess() {}
    int32_t Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context) override;
};

class UScriptInstructionShowProcess : public uscript::UScriptInstruction {
public:
    UScriptInstructionShowProcess() {}
    virtual ~UScriptInstructionShowProcess() {}
    int32_t Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context) override;
};

class UScriptInstructionUiPrint : public uscript::UScriptInstruction {
public:
    UScriptInstructionUiPrint() {}
    virtual ~UScriptInstructionUiPrint() {}
    int32_t Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context) override;
};
} // namespace BasicInstruction
#endif // USCRIPT_UPDATE_PROCESSOR_H
