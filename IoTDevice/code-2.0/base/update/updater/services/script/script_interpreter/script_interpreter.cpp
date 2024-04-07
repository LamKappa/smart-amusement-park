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
#include "script_interpreter.h"
#include <algorithm>
#include <fstream>
#include "script_context.h"
#include "script_manager_impl.h"
#include "scanner.h"
#include "script_utils.h"

using namespace std;

namespace uscript {
static int32_t g_instanceId = 0;

int32_t ScriptInterpreter::ExecuteScript(ScriptManagerImpl *manager, hpackage::PkgManager::StreamPtr pkgStream)
{
    USCRIPT_CHECK(pkgStream != nullptr, return USCRIPT_INVALID_PARAM, "Param error");
    USCRIPT_LOGI("ExecuteScript %s", pkgStream->GetFileName().c_str());
    auto inter = new ScriptInterpreter(manager);
    USCRIPT_CHECK(inter != nullptr, return USCRIPT_ERROR_CREATE_OBJ,
        "Fail to create ScriptInterpreter for script %s", pkgStream->GetFileName().c_str());
    int32_t ret = inter->LoadScript(pkgStream);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Fail to loadScript script %s", pkgStream->GetFileName().c_str());
    ret = inter->Execute();
    delete inter;
    USCRIPT_LOGI("ExecuteScript finish ret: %d  script: %s ",
        ret, pkgStream->GetFileName().c_str());
    return ret;
}

ScriptInterpreter::ScriptInterpreter(ScriptManagerImpl *manager) : scriptManager_(manager)
{
    instanceId_ = g_instanceId++;
}

ScriptInterpreter::~ScriptInterpreter()
{
    delete statements_;
    auto iter = functions_.begin();
    while (iter != functions_.end()) {
        auto entry = iter->second;
        if (entry) {
            delete entry;
        }
        iter = functions_.erase(iter);
    }
    functions_.clear();
    contextStack_.clear();
    delete scanner_;
    delete parser_;
}

int32_t ScriptInterpreter::LoadScript(hpackage::PkgManager::StreamPtr pkgStream)
{
    scanner_ = new Scanner(this);
    parser_ = new Parser(scanner_, this);
    if (scanner_ == nullptr || parser_ == nullptr) {
        USCRIPT_LOGE("Fail to open fstream %s", pkgStream->GetFileName().c_str());
        delete parser_;
        delete scanner_;
        return USCRIPT_ERROR_CREATE_OBJ;
    }
    scanner_->set_debug(0);
    scanner_->SetPkgStream(pkgStream);
    int32_t ret = parser_->parse();
    return ret;
}

int32_t ScriptInterpreter::Execute()
{
    UScriptContextPtr context = std::make_shared<UScriptInterpretContext>(true);
    UScriptStatementResult result = statements_->Execute(*this, context);
    INTERPRETER_LOGI(*this, context, "statements_ execute result %s ",
        UScriptStatementResult::ScriptToString(&result).c_str());
    if (result.GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_ERROR) {
        return result.GetError();
    }
    return USCRIPT_SUCCESS;
}

int32_t ScriptInterpreter::AddFunction(ScriptFunction *function)
{
    if (functions_.find(function->GetFunctionName()) != functions_.end()) {
        USCRIPT_LOGI("Fail to add function %s, function exist", function->GetFunctionName().c_str());
        return USCRIPT_SUCCESS;
    }
    functions_[function->GetFunctionName()] = function;
    return USCRIPT_SUCCESS;
}

ScriptFunction* ScriptInterpreter::FindFunction(const std::string &name)
{
    if (functions_.find(name) != functions_.end()) {
        return functions_[name];
    }
    return nullptr;
}

UScriptValuePtr ScriptInterpreter::FindVariable(UScriptContextPtr local, std::string id)
{
    for (auto context = contextStack_.rbegin(); context != contextStack_.rend(); context++) {
        UScriptValuePtr variable = (*context)->FindVariable(*this, id);
        if (variable != nullptr) {
            return variable;
        }
        if ((*context)->IsTop()) {
            break;
        }
    }
    return nullptr;
}

UScriptValuePtr ScriptInterpreter::UpdateVariable(UScriptContextPtr local, std::string id, UScriptValuePtr var)
{
    for (auto context = contextStack_.rbegin(); context != contextStack_.rend(); context++) {
        UScriptValuePtr variable = (*context)->FindVariable(*this, id);
        if (variable != nullptr) {
            (*context)->UpdateVariable(*this, id, var);
        }
        if ((*context)->IsTop()) {
            break;
        }
    }
    return nullptr;
}

void ScriptInterpreter::AddStatement(UScriptStatement *statement)
{
    if (statements_ == nullptr) {
        statements_ = UScriptStatementList::CreateInstance(statement);
    } else {
        statements_->AddScriptStatement(statement);
    }
}

bool ScriptInterpreter::IsNativeFunction(std::string name)
{
    return scriptManager_->FindInstruction(name) != nullptr;
}

UScriptValuePtr ScriptInterpreter::ExecuteNativeFunc(UScriptContextPtr context,
    const std::string &name, ScriptParams *params)
{
    std::shared_ptr<ErrorValue> error = std::make_shared<ErrorValue>(USCRIPT_ERROR_INTERPRET);
    std::shared_ptr<ReturnValue> retValue = std::make_shared<ReturnValue>();
    INTERPRETER_LOGI(*this, context, "ExecuteNativeFunc::Execute %s ", name.c_str());
    UScriptInstruction* instruction = scriptManager_->FindInstruction(name);
    USCRIPT_CHECK(instruction != nullptr, return error, "Fail to find instruction %s", name.c_str());

    std::shared_ptr<UScriptInstructionContext> funcContext = std::make_shared<UScriptInstructionContext>();
    USCRIPT_CHECK(funcContext != nullptr, return error, "Fail to create context %s", name.c_str());
    if (params == nullptr) {
        int32_t ret = instruction->Execute(*scriptManager_->GetScriptEnv(name), *funcContext.get());
        retValue->AddValues(funcContext->GetOutVar());
        INTERPRETER_LOGI(*this, context, "ExecuteNativeFunc::Execute %s result: %d", name.c_str(), ret);
        return retValue;
    }

    for (auto id : params->GetParams()) {
        UScriptValuePtr result = id->Execute(*this, context);
        if (result == nullptr || result->GetValueType() == UScriptValue::VALUE_TYPE_ERROR) {
            INTERPRETER_LOGI(*this, context, "ExecuteNativeFunc::Execute %s ", name.c_str());
            return error;
        }

        if (result->GetValueType() != UScriptValue::VALUE_TYPE_LIST) {
            funcContext->AddInputParam(result);
        } else {
            ReturnValue* values = (ReturnValue*)(result.get());
            for (auto out : values->GetValues()) {
                funcContext->AddInputParam(out);
            }
        }
    }

    int32_t ret = instruction->Execute(*scriptManager_->GetScriptEnv(name), *funcContext.get());
    INTERPRETER_LOGI(*this, context, "ExecuteNativeFunc::Execute %s result: %d", name.c_str(), ret);
    if (ret != USCRIPT_SUCCESS) {
        error->SetValue(ret);
        return error;
    }
    retValue->AddValues(funcContext->GetOutVar());
    return retValue;
}
} // namespace uscript
