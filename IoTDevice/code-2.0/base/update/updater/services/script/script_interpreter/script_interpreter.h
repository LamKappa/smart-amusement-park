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
#ifndef USCRIPT_INTERPRETER_H
#define USCRIPT_INTERPRETER_H

#include "script_context.h"
#include "script_expression.h"
#include "script_function.h"
#include "script_manager_impl.h"
#include "script_param.h"
#include "script_statement.h"

#define INTERPRETER_LOGE(inter, context, format, ...) \
    Logger(updater::INFO, (__FILE_NAME__), (__LINE__), \
    "[INTERPRETER %d-%d]"#format, (inter).GetInstanceId(), (context)->GetContextId(), ##__VA_ARGS__)
#define INTERPRETER_LOGI(inter, context, format, ...) \
    Logger(updater::INFO, (__FILE_NAME__), (__LINE__), \
    "[INTERPRETER %d-%d]"#format, (inter).GetInstanceId(), (context)->GetContextId(), ##__VA_ARGS__)

#define INTERPRETER_CHECK(inter, context, ret, statement, ...) \
    if (!(ret)) {                                              \
        INTERPRETER_LOGE(inter, context, __VA_ARGS__);         \
        statement;                                             \
    }

namespace uscript {
class Parser;
class Scanner;
class ScriptManagerImpl;

class ScriptInterpreter {
public:
    static int32_t ExecuteScript(ScriptManagerImpl *manager, hpackage::PkgManager::StreamPtr pkgStream);

    explicit ScriptInterpreter(ScriptManagerImpl *manager);
    ~ScriptInterpreter();

    void AddStatement(UScriptStatement *statement);
    int32_t AddFunction(ScriptFunction *function);
    ScriptFunction* FindFunction(const std::string &name);
    bool IsNativeFunction(std::string name);
    UScriptValuePtr ExecuteNativeFunc(UScriptContextPtr upContext, const std::string &name,
        ScriptParams *params);
    UScriptValuePtr FindVariable(UScriptContextPtr local, std::string id);
    UScriptValuePtr UpdateVariable(UScriptContextPtr local, std::string id, UScriptValuePtr var);
    int32_t GetInstanceId() const
    {
        return instanceId_;
    }

    void ContextPush(UScriptContextPtr context)
    {
        contextStack_.push_back(context);
    }
    void ContextPop()
    {
        contextStack_.pop_back();
    }

private:
    int32_t LoadScript(hpackage::PkgManager::StreamPtr pkgStream);
    int32_t Execute();

private:
    UScriptStatementList* statements_ = nullptr;
    std::map<std::string, ScriptFunction*> functions_;
    std::vector<UScriptContextPtr> contextStack_;
    ScriptManagerImpl* scriptManager_ = nullptr;
    Parser* parser_ = nullptr;
    Scanner* scanner_ = nullptr;
    int32_t instanceId_ = 0;
};
} // namespace uscript
#endif