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
#include "script_updateprocesser.h"
#include <sstream>
#include "script_instruction.h"
#include "script_manager.h"
#include "script_utils.h"

using namespace uscript;

namespace BasicInstruction {
int32_t UScriptInstructionSetProcess::Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context)
{
    float setProcess = 0.0f;
    int32_t ret = context.GetParam(0, setProcess);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
    std::string content;
    std::stringstream sstream;
    sstream << setProcess;
    sstream >> content;
    env.PostMessage("set_progress", content);
    return USCRIPT_SUCCESS;
}

int32_t UScriptInstructionShowProcess::Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context)
{
    float startProcess = 0.0f;
    float endProcess = 0.0f;
    int32_t ret = context.GetParam(0, startProcess);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
    ret = context.GetParam(1, endProcess);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
    std::string content;
    std::stringstream sstream;
    sstream << startProcess;
    sstream << ",";
    sstream << endProcess;
    sstream >> content;
    env.PostMessage("show_progress", content);
    return USCRIPT_SUCCESS;
}

int32_t UScriptInstructionUiPrint::Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context)
{
    std::string message;
    int32_t ret = context.GetParam(0, message);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
    env.PostMessage("ui_log", message);
    return USCRIPT_SUCCESS;
}
} // namespace BasicInstruction
