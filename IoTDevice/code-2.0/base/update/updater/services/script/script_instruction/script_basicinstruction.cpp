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
#include "script_basicinstruction.h"
#include <unistd.h>
#include "script_loadscript.h"
#include "script_manager_impl.h"
#include "script_registercmd.h"
#include "script_utils.h"

using namespace uscript;
using namespace std;

namespace BasicInstruction {
int32_t UScriptInstructionAbort::Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context)
{
    int32_t result = 1;
    int32_t ret = context.GetParam(0, result);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
    return ((result == 0) ? USCRIPT_ABOART : USCRIPT_SUCCESS);
}

int32_t UScriptInstructionAssert::Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context)
{
    int32_t result = 1;
    int32_t ret = context.GetParam(0, result);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
    return ((result == 0) ? USCRIPT_ASSERT : USCRIPT_SUCCESS);
}

int32_t UScriptInstructionSleep::Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context)
{
    int32_t seconds = 1;
    int32_t ret = context.GetParam(0, seconds);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
    sleep(seconds);
    return USCRIPT_SUCCESS;
}

int32_t UScriptInstructionConcat::Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context)
{
    int32_t ret = 0;
    std::string str;
    ret = context.GetParam(0, str);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");

    for (int32_t i = 1; i < context.GetParamCount(); i++) {
        switch (context.GetParamType(i)) {
            case UScriptContext::PARAM_TYPE_INTEGER: {
                int32_t v;
                ret = context.GetParam(i, v);
                USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
                str.append(to_string(v));
                break;
            }
            case UScriptContext::PARAM_TYPE_FLOAT: {
                float v;
                ret = context.GetParam(i, v);
                USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
                str.append(to_string(v));
                break;
            }
            case UScriptContext::PARAM_TYPE_STRING: {
                std::string v;
                ret = context.GetParam(i, v);
                USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
                str.append(v);
                break;
            }
            default:
                break;
        }
    }
    context.PushParam(str);
    return USCRIPT_SUCCESS;
}

int32_t UScriptInstructionIsSubString::Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context)
{
    std::string str;
    std::string subStr;
    int32_t ret = context.GetParam(0, str);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
    ret = context.GetParam(1, subStr);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
    string::size_type last = str.find(subStr);
    if (last == string::npos) {
        context.PushParam(0);
    } else {
        context.PushParam(1);
    }
    return USCRIPT_SUCCESS;
}

int32_t UScriptInstructionStdout::Execute(uscript::UScriptEnv &env, uscript::UScriptContext &context)
{
    int32_t ret;
    for (int32_t i = 0; i < context.GetParamCount(); i++) {
        if (context.GetParamType(i) == UScriptContext::PARAM_TYPE_INTEGER) {
            int32_t v;
            ret = context.GetParam(i, v);
            USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
            std::cout << v << "  ";
        } else if (context.GetParamType(i) == UScriptContext::PARAM_TYPE_FLOAT) {
            float v;
            ret = context.GetParam(i, v);
            USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
            std::cout << v << "  ";
        } else if (context.GetParamType(i) == UScriptContext::PARAM_TYPE_STRING) {
            std::string v;
            ret = context.GetParam(i, v);
            USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
            std::cout << v << "  ";
        }
    }
    std::cout << std::endl;
    return USCRIPT_SUCCESS;
}
} // namespace BasicInstruction
