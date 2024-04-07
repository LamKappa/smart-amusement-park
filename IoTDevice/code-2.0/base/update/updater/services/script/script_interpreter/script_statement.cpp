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
#include "script_statement.h"
#include "script_context.h"
#include "script_expression.h"
#include "script_interpreter.h"
#include "script_utils.h"

using namespace std;

namespace uscript {
void UScriptStatementResult::UpdateStatementResult(UScriptValuePtr value)
{
    USCRIPT_CHECK(value != nullptr, SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_ERROR),
        "Invalid value");
    switch (value->GetValueType()) {
        case UScriptValue::VALUE_TYPE_INTEGER:
            /* fallthrough */
        case UScriptValue::VALUE_TYPE_FLOAT:
            /* fallthrough */
        case UScriptValue::VALUE_TYPE_STRING:
            SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_NORMAL);
            SetResultValue(value);
            break;
        case UScriptValue::VALUE_TYPE_ERROR:
            SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_ERROR);
            SetError(USCRIPT_ERROR_INTERPRET);
            if (value->GetValueType() == UScriptValue::VALUE_TYPE_ERROR) {
                SetError(((ErrorValue*)value.get())->GetValue());
            }
            break;
        case UScriptValue::VALUE_TYPE_LIST:
            SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_NORMAL);
            SetResultValue(value);
            break;
        case UScriptValue::VALUE_TYPE_RETURN:
            SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_RTN);
            SetResultValue(value);
            break;
        default:
            SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_ERROR);
            break;
    }
    return;
}

std::string UScriptStatementResult::ScriptToString(UScriptStatementResult *result)
{
    std::string str;
    USCRIPT_CHECK(result != nullptr, return str, "null value");

    str.append("type: " + to_string(result->GetResultType()));
    if (result->GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_ERROR) {
        str.append("  errorCode : " + std::to_string(result->GetError()));
    } else {
        str.append("  value : " + UScriptValue::ScriptToString(result->GetResultValue()));
    }
    return str;
}

UScriptStatement* UScriptStatement::CreateStatement(UScriptStatement::StatementType type)
{
    return new UScriptStatementCtrl(type);
}

// ExpressionStatement
UScriptStatement* UScriptStatement::CreateExpressionStatement(UScriptExpression *expression)
{
    return new UScriptExpressionStatement(expression);
}

// IFStatement
UScriptStatement* UScriptStatement::CreateIfStatement(UScriptExpression *condition,
    UScriptStatementList *list1,
    UScriptStatementList *list2,
    UScriptStatement *nextIfState)
{
    auto ifStatement = new UScriptIfStatement(condition, list1);
    USCRIPT_CHECK(ifStatement != nullptr, return nullptr, "Create if statement failed ");
    ifStatement->AddFalseStatementList(list2);
    ifStatement->AddNextStatement(reinterpret_cast<UScriptIfStatement*>(nextIfState));
    return ifStatement;
}

// FORStatement
UScriptStatement* UScriptStatement::CreateForStatement(UScriptExpression *before,
    UScriptExpression *condition,
    UScriptExpression *after,
    UScriptStatementList *list)
{
    return new UScriptForStatement(before, condition, after, list);
}

UScriptStatement* UScriptStatement::CreateWhileStatement(UScriptExpression *condition,
    UScriptStatementList *list)
{
    return new UScriptWhileStatement(condition, list);
}

UScriptStatementList* UScriptStatementList::CreateInstance(UScriptStatement *statement)
{
    auto list = new UScriptStatementList();
    USCRIPT_CHECK(list != nullptr, return nullptr, "Failed to create statement list ");
    list->AddScriptStatement(statement);
    return list;
}

UScriptStatementList::~UScriptStatementList()
{
    for (auto iter = statements_.begin(); iter != statements_.end();) {
        delete *iter;
        iter = statements_.erase(iter);
    }
    statements_.clear();
}

void UScriptStatementList::AddScriptStatement(UScriptStatement *statement)
{
    statements_.push_back(statement);
}

UScriptStatementResult UScriptStatementCtrl::Execute(ScriptInterpreter &interpreter, UScriptContextPtr context)
{
    UScriptStatementResult result;
    switch (GetType()) {
        case STATEMENT_TYPE_BREAK:
            result.SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_BREAK);
            break;
        case STATEMENT_TYPE_RTN:
            result.SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_RTN);
            break;
        case STATEMENT_TYPE_CONTINUE:
            result.SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_CONTINUE);
            break;
        default:
            result.SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_ERROR);
            result.SetError(USCRIPT_INVALID_STATEMENT);
            break;
    }
    INTERPRETER_LOGI(interpreter, context, "UScriptStatementList::statement result :%s",
                     UScriptStatementResult::ScriptToString(&result).c_str());
    return result;
}

UScriptStatementResult UScriptExpressionStatement::Execute(ScriptInterpreter &interpreter,
    UScriptContextPtr context)
{
    UScriptStatementResult result(UScriptStatementResult::STATEMENT_RESULT_TYPE_NORMAL, nullptr);
    INTERPRETER_LOGI(interpreter, context, "UScriptExpressionStatement::statement ");
    UScriptValuePtr value = expression_->Execute(interpreter, context);
    result.UpdateStatementResult(value);
    INTERPRETER_LOGI(interpreter, context, "UScriptExpressionStatement::Execute result: %s",
        UScriptStatementResult::ScriptToString(&result).c_str());
    return result;
}

UScriptStatementResult UScriptForStatement::Execute(ScriptInterpreter &interpreter, UScriptContextPtr context)
{
    INTERPRETER_LOGI(interpreter, context, "UScriptForStatement::statement ");
    UScriptStatementResult result(UScriptStatementResult::STATEMENT_RESULT_TYPE_NORMAL, nullptr);
    if (before_ != nullptr) {
        INTERPRETER_LOGE(interpreter, context, "Execute before");
        before_->Execute(interpreter, context);
    }

    while (1) {
        if (condition_ != nullptr) {
            UScriptValuePtr v = condition_->Execute(interpreter, context);
            INTERPRETER_CHECK(interpreter, context, !(v == nullptr || v->GetValueType() ==
                UScriptValue::VALUE_TYPE_ERROR),
                result.SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_ERROR); return result,
                "Execute for condition failed: %s", UScriptValue::ScriptToString(v).c_str());
            if (!v->IsTrue()) {
                break;
            }
        }
        UScriptStatementResult centerResult = statements_->Execute(interpreter, context);
        INTERPRETER_LOGI(interpreter, context, "Execute statements result %s ",
            UScriptStatementResult::ScriptToString(&centerResult).c_str());
        if (centerResult.GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_BREAK) {
            break;
        }
        if (centerResult.GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_RTN ||
            centerResult.GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_ERROR) {
            return centerResult;
        }

        if (after_ != nullptr) {
            INTERPRETER_LOGI(interpreter, context, "Execute after");
            after_->Execute(interpreter, context);
        }
    }
    return result;
}

UScriptStatementResult UScriptWhileStatement::Execute(ScriptInterpreter &interpreter, UScriptContextPtr local)
{
    INTERPRETER_LOGI(interpreter, local, "UScriptStatementResult::statement ");
    UScriptStatementResult result(UScriptStatementResult::STATEMENT_RESULT_TYPE_NORMAL, nullptr);
    while (1) {
        if (condition_ != nullptr) {
            UScriptValuePtr v = condition_->Execute(interpreter, local);
            INTERPRETER_CHECK(interpreter, local, !(v == nullptr || v->GetValueType() ==
                UScriptValue::VALUE_TYPE_ERROR),
                result.SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_ERROR); return result,
                "Execute while condition failed: %s", UScriptValue::ScriptToString(v).c_str());
            if (!v->IsTrue()) {
                break;
            }
        }
        UScriptStatementResult centerResult = statements_->Execute(interpreter, local);
        INTERPRETER_LOGI(interpreter, local, "Execute statements result %s ",
            UScriptStatementResult::ScriptToString(&centerResult).c_str());
        if (centerResult.GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_BREAK) {
            break;
        }
        if (centerResult.GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_CONTINUE) {
            continue;
        }
        if (centerResult.GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_RTN ||
            centerResult.GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_ERROR) {
            return centerResult;
        }
    }
    return result;
}

UScriptStatementResult UScriptIfStatement::Execute(ScriptInterpreter &interpreter, UScriptContextPtr context)
{
    UScriptStatementResult result(UScriptStatementResult::STATEMENT_RESULT_TYPE_NORMAL, nullptr);
    UScriptValuePtr v = expression_->Execute(interpreter, context);
    INTERPRETER_CHECK(interpreter, context,
        !(v == nullptr || v->GetValueType() == UScriptValue::VALUE_TYPE_ERROR),
        result.SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_ERROR); return result,
        "Execute for condition failed: %s", UScriptValue::ScriptToString(v).c_str());

    if (v->IsTrue()) {
        if (trueStatements_ == nullptr) {
            return result;
        }
        UScriptContextPtr local = std::make_shared<UScriptInterpretContext>();
        return trueStatements_->Execute(interpreter, local);
    } else if (falseStatements_ != nullptr) {
        UScriptContextPtr local = std::make_shared<UScriptInterpretContext>();
        return falseStatements_->Execute(interpreter, local);
    } else if (nextStatement_ != nullptr) {
        return nextStatement_->Execute(interpreter, context);
    }
    return result;
}

UScriptStatementResult UScriptStatementList::Execute(ScriptInterpreter &inter, UScriptContextPtr context)
{
    INTERPRETER_LOGI(inter, context, "UScriptStatementList::Execute ");
    inter.ContextPush(context);
    UScriptStatementResult result(UScriptStatementResult::STATEMENT_RESULT_TYPE_NORMAL, nullptr);
    for (auto statement : statements_) {
        result = statement->Execute(inter, context);
        if (result.GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_BREAK) {
            break;
        } else if (result.GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_CONTINUE) {
            break;
        } else if (result.GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_RTN) {
            break;
        } else if (result.GetResultType() == UScriptStatementResult::STATEMENT_RESULT_TYPE_ERROR) {
            break;
        }
    }
    inter.ContextPop();
    INTERPRETER_LOGI(inter, context, "UScriptStatementList finish %s",
        UScriptStatementResult::ScriptToString(&result).c_str());
    return result;
}

UScriptExpressionStatement::~UScriptExpressionStatement()
{
    delete expression_;
}

UScriptIfStatement::~UScriptIfStatement()
{
    delete expression_;
    delete trueStatements_;
    delete falseStatements_;
    delete nextStatement_;
}

UScriptForStatement::~UScriptForStatement()
{
    delete before_;
    delete condition_;
    delete after_;
    delete statements_;
}

UScriptWhileStatement::~UScriptWhileStatement()
{
    delete condition_;
    delete statements_;
}


UScriptReturnStatement::~UScriptReturnStatement()
{
    delete params_;
}

UScriptReturnStatement* UScriptReturnStatement::CreateStatement(ScriptParams *params)
{
    auto statement = new UScriptReturnStatement();
    if (params != nullptr) {
        statement->AddParams(params);
    }
    return statement;
}

UScriptStatementResult UScriptReturnStatement::Execute(ScriptInterpreter &interpreter, UScriptContextPtr context)
{
    UScriptStatementResult result(UScriptStatementResult::STATEMENT_RESULT_TYPE_RTN, nullptr);
    USCRIPT_CHECK(params_ != nullptr, return result, "Invalid parm");

    std::shared_ptr<ReturnValue> retValue = std::make_shared<ReturnValue>();
    USCRIPT_CHECK(retValue != nullptr, return result, "Create ret value failed");
    for (auto id : params_->GetParams()) {
        UScriptValuePtr var = id->Execute(interpreter, context);
        INTERPRETER_LOGI(interpreter, context, "params result: %s", UScriptValue::ScriptToString(var).c_str());
        if (var->GetValueType() == UScriptValue::VALUE_TYPE_LIST) {
            retValue->AddValues(((ReturnValue*)var.get())->GetValues());
        } else {
            retValue->AddValue(var);
        }
    }
    result.SetResultType(UScriptStatementResult::STATEMENT_RESULT_TYPE_RTN);
    result.SetResultValue(retValue);
    return result;
}
} // namespace uscript
