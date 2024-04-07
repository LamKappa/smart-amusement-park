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

#ifndef UPDATER_UPDATE_IMAGE_BLOCK_H
#define UPDATER_UPDATE_IMAGE_BLOCK_H
#include "pkg_manager.h"
#include "script_instruction.h"
#include "script_manager.h"

namespace updater {
class UScriptInstructionBlockUpdate : public uscript::UScriptInstruction {
public:
    UScriptInstructionBlockUpdate() {}
    virtual ~UScriptInstructionBlockUpdate() {}
    int32_t Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context) override;
};

class UScriptInstructionBlockCheck : public uscript::UScriptInstruction {
public:
    UScriptInstructionBlockCheck() {}
    virtual ~UScriptInstructionBlockCheck() {}
    int32_t Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context) override;
};

class UScriptInstructionShaCheck : public uscript::UScriptInstruction {
public:
    UScriptInstructionShaCheck() {}
    virtual ~UScriptInstructionShaCheck() {}
    int32_t Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context) override;
};
}

#endif // UPDATER_UPDATE_IMAGE_BLOCK_H
