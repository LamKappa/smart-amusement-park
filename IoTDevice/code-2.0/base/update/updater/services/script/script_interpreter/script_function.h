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
#ifndef USCRIPT_FUNCTION_H
#define USCRIPT_FUNCTION_H

#include <vector>
#include "script_context.h"
#include "script_statement.h"

namespace uscript {
class UScriptStatementList;
class ScriptParams;

class ScriptFunction {
public:
    static ScriptFunction* CreateInstance(std::string &identifier, ScriptParams *params,
        UScriptStatementList *list)
    {
        return new ScriptFunction(identifier, params, list);
    }

public:
    ScriptFunction(std::string& functionName, ScriptParams *params, UScriptStatementList *list)
        : functionName_(functionName), params_(params), statements_(list)
    {
    }
    ~ScriptFunction();
    const std::string GetFunctionName()
    {
        return functionName_;
    }
    UScriptValuePtr Execute(ScriptInterpreter &inter, UScriptContextPtr local, ScriptParams *params);

private:
    std::vector<std::string> GetParamNames(ScriptInterpreter &inter, UScriptContextPtr context) const;
    std::string functionName_;
    ScriptParams* params_ = nullptr;
    UScriptStatementList* statements_ = nullptr;
};
} // namespace uscript
#endif // USCRIPT_FUNCTION_H
