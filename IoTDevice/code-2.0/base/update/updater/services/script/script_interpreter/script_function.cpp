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
#include "script_function.h"
#include "script_context.h"
#include "script_interpreter.h"
#include "script_manager.h"
#include "script_param.h"
#include "script_utils.h"

using namespace std;

namespace uscript {
ScriptFunction::~ScriptFunction()
{
    delete params_;
    delete statements_;
}

UScriptValuePtr ScriptFunction::Execute(ScriptInterpreter &inter,
    UScriptContextPtr context, ScriptParams *inputParams)
{
    INTERPRETER_LOGI(inter, context, "ScriptFunction execute %s", functionName_.c_str());
    UScriptContextPtr funcContext = std::make_shared<UScriptInterpretContext>();
    USCRIPT_CHECK(funcContext != nullptr, return std::make_shared<ErrorValue>(USCRIPT_ERROR_CREATE_OBJ),
        "[interpreter-%d] Fail to create context for function %s", inter.GetInstanceId(), functionName_.c_str());

    if (inputParams == nullptr || params_ == nullptr) {
        USCRIPT_CHECK(!(params_ != nullptr || inputParams != nullptr),
            return std::make_shared<ErrorValue>(USCRIPT_ERROR_INTERPRET),
            "[interpreter-%d] ScriptFunction::Execute param not match %s",
            inter.GetInstanceId(), functionName_.c_str());
    } else {
        USCRIPT_CHECK(params_->GetParams().size() == params_->GetParams().size(),
            return std::make_shared<ErrorValue>(USCRIPT_ERROR_INTERPRET),
            "[interpreter-%d] ScriptFunction::Execute param not match %s",
            inter.GetInstanceId(), functionName_.c_str());

        size_t index = 0;
        std::vector<std::string> paramNames = GetParamNames(inter, context);
        for (auto expression : inputParams->GetParams()) {
            UScriptValuePtr var = expression->Execute(inter, context);
            USCRIPT_CHECK(!(var == nullptr || var->GetValueType() == UScriptValue::VALUE_TYPE_ERROR),
                return std::make_shared<ErrorValue>(USCRIPT_NOTEXIST_INSTRUCTION),
                "[interpreter-%d] ScriptFunction::Execute fail to computer param %s",
                inter.GetInstanceId(), functionName_.c_str());
            USCRIPT_CHECK(index < paramNames.size(), return std::make_shared<ErrorValue>(USCRIPT_NOTEXIST_INSTRUCTION),
                "[interpreter-%d] ScriptFunction::Execute invalid index %zu param %s",
                inter.GetInstanceId(), index, functionName_.c_str());
            funcContext->UpdateVariables(inter, var, paramNames, index);
        }
    }
    UScriptStatementResult result = statements_->Execute(inter, funcContext);
    INTERPRETER_LOGI(inter, context, "ScriptFunction execute %s result %s", functionName_.c_str(),
        UScriptStatementResult::ScriptToString(&result).c_str());
    return result.GetResultValue();
}

std::vector<std::string> ScriptFunction::GetParamNames(ScriptInterpreter &inter,
    UScriptContextPtr context) const
{
    int32_t ret;
    std::vector<std::string> names;
    for (auto expression : params_->GetParams()) {
        std::string varName;
        ret = IdentifierExpression::GetIdentifierName(expression, varName);
        INTERPRETER_CHECK(inter, context, ret == USCRIPT_SUCCESS, return names, "Fail to get param name %s",
            functionName_.c_str());
        names.push_back(varName);
    }
    return names;
}
} // namespace uscript
