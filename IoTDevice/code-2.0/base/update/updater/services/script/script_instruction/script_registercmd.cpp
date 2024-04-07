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
#include "script_registercmd.h"
#include <cstring>
#include <dlfcn.h>
#include "script_instruction.h"
#include "script_instructionhelper.h"
#include "script_manager.h"

using namespace uscript;

namespace BasicInstruction {
int32_t ScriptRegisterCmd::Execute(UScriptEnv &env, UScriptContext &context)
{
    ScriptInstructionHelper* helper = ScriptInstructionHelper::GetBasicInstructionHelper();
    USCRIPT_CHECK(helper != nullptr, return USCRIPT_INVALID_PARAM, "Fail to get instruction helper");

    std::string instrName;
    std::string libName;
    int32_t ret = context.GetParam(0, instrName);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Fail to get param");
    ret = context.GetParam(1, libName);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Fail to get param");

    // 动态加载对应的so，然后获取对应的指令
    return helper->RegisterUserInstruction(libName, instrName);
}
} // namespace BasicInstruction
