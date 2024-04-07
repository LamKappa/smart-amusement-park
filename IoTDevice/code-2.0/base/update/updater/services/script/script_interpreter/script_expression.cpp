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
#include "script_expression.h"
#include "script_function.h"
#include "script_interpreter.h"
#include "script_utils.h"

using namespace std;

namespace uscript {
UScriptExpression::UScriptExpression(ExpressionType expressType) : expressType_(expressType) {}
UScriptExpression::~UScriptExpression() {}

UScriptExpression* AssignExpression::CreateExpression(std::string identifier, UScriptExpression *expression)
{
    return new AssignExpression(identifier, expression);
}
void AssignExpression::AddIdentifier(const std::string &identifier)
{
    multipleIdentifiers_.push_back(identifier);
}

UScriptExpression* AssignExpression::AddIdentifier(UScriptExpression *expression, std::string identifier)
{
    auto assign = reinterpret_cast<AssignExpression*>(expression);
    if (assign != nullptr) {
        assign->AddIdentifier(identifier);
    }
    return assign;
}

// binary operator
UScriptExpression* BinaryExpression::CreateExpression(ExpressionAction action,
    UScriptExpression *left, 
    UScriptExpression *right)
{
    return new BinaryExpression(action, left, right);
}
UScriptExpression* FunctionCallExpression::CreateExpression(std::string identifier, ScriptParams *params)
{
    return new FunctionCallExpression(identifier, params);
}
UScriptValuePtr UScriptExpression::Execute(ScriptInterpreter &inter, UScriptContextPtr local)
{
    return std::make_shared<UScriptValue>(UScriptValue::VALUE_TYPE_ERROR);
}
UScriptValuePtr IntegerExpression::Execute(ScriptInterpreter &inter, UScriptContextPtr local)
{
    return std::make_shared<IntegerValue>(this->value_);
}
UScriptValuePtr FloatExpression::Execute(ScriptInterpreter &inter, UScriptContextPtr local)
{
    return std::make_shared<FloatValue>(this->value_);
}
UScriptValuePtr StringExpression::Execute(ScriptInterpreter &inter, UScriptContextPtr local)
{
    return std::make_shared<StringValue>(this->value_);
}
UScriptValuePtr IdentifierExpression::Execute(ScriptInterpreter &inter, UScriptContextPtr local)
{
    INTERPRETER_LOGI(inter, local, "Execute statements identifier %s ", identifier_.c_str());
    UScriptValuePtr variable = inter.FindVariable(local, identifier_);
    INTERPRETER_LOGI(inter, local, "IdentifierExpression::Execute '%s = %s ' ", identifier_.c_str(),
        UScriptValue::ScriptToString(variable).c_str());
    if (variable != nullptr) {
        return variable;
    }
    return std::make_shared<UScriptValue>(UScriptValue::VALUE_TYPE_ERROR);
}

int32_t IdentifierExpression::GetIdentifierName(UScriptExpression *expression, std::string &name)
{
    if (expression->GetExpressType() != EXPRESSION_TYPE_IDENTIFIER) {
        return USCRIPT_INVALID_PARAM;
    } else {
        auto identifier = reinterpret_cast<IdentifierExpression*>(expression);
        if (identifier != nullptr) {
            name = identifier->GetIdentifier();
            return USCRIPT_SUCCESS;
        }
    }
    return USCRIPT_INVALID_PARAM;
}
UScriptValuePtr AssignExpression::Execute(ScriptInterpreter &inter, UScriptContextPtr local)
{
    UScriptValuePtr result = expression_->Execute(inter, local);
    INTERPRETER_LOGI(inter, local, "AssignExpression::Execute update local var '%s = %s ' ", identifier_.c_str(),
        UScriptValue::ScriptToString(result).c_str());
    if (result->GetValueType() == UScriptValue::VALUE_TYPE_ERROR) {
        return result;
    }
    UScriptValuePtr var = inter.FindVariable(local, identifier_);
    if (var != nullptr) {
        inter.UpdateVariable(local, identifier_, result);
        return result;
    }

    std::vector<std::string> identifiers;
    identifiers.push_back(identifier_);
    identifiers.insert(identifiers.begin() + 1, multipleIdentifiers_.begin(), multipleIdentifiers_.end());
    size_t index = 0;
    local->UpdateVariables(inter, result, identifiers, index);
    return result;
}

UScriptValuePtr BinaryExpression::Execute(ScriptInterpreter &inter, UScriptContextPtr local)
{
    static std::vector<std::string> opStr = {
        "", "add", "sub", "mul", "div", ">", ">=", "<", "<=", "==", "!=", "&&", "||"
    };
    UScriptValuePtr left;
    UScriptValuePtr right;

    INTERPRETER_LOGI(inter, local, "BinaryExpression::Execute ");
    if (left_ != nullptr) {
        left = left_->Execute(inter, local);
    }

    if (action_ == OR_OPERATOR && left->IsTrue()) {
        INTERPRETER_LOGE(inter, local, "BinaryExpression::Execute left:%s %s",
            UScriptValue::ScriptToString(left).c_str(), opStr[action_].c_str());
        return std::make_shared<IntegerValue>(1);
    }
    if (right_ != nullptr) {
        right = right_->Execute(inter, local);
    }
    if (left != nullptr && right != nullptr) {
        UScriptValuePtr value = left->Computer(action_, right);
        INTERPRETER_LOGI(inter, local, "BinaryExpression::Execute left:%s %s right:%s result:%s",
            UScriptValue::ScriptToString(left).c_str(), opStr[action_].c_str(),
            UScriptValue::ScriptToString(right).c_str(), UScriptValue::ScriptToString(value).c_str());
        return value;
    }
    return std::make_shared<ErrorValue>(USCRIPT_ERROR_INTERPRET);
}

UScriptValuePtr FunctionCallExpression::Execute(ScriptInterpreter &inter, UScriptContextPtr local)
{
    UScriptValuePtr v;
    INTERPRETER_LOGI(inter, local, "FunctionCallExpression::Execute %s ", functionName_.c_str());

    if (inter.IsNativeFunction(functionName_)) {
        return inter.ExecuteNativeFunc(local, functionName_, params_);
    }

    ScriptFunction* function = function_;
    if (function_ == nullptr) {
        function = inter.FindFunction(functionName_);
    }
    if (function == nullptr) {
        INTERPRETER_LOGI(inter, local, "Can not find function %s", functionName_.c_str());
        return std::make_shared<ErrorValue>(USCRIPT_NOTEXIST_INSTRUCTION);
    }
    return function->Execute(inter, local, params_);
}

BinaryExpression::~BinaryExpression()
{
    delete left_;
    delete right_;
}
AssignExpression::~AssignExpression()
{
    delete this->expression_;
}
FunctionCallExpression::~FunctionCallExpression()
{
    delete function_;
    delete params_;
}
} // namespace uscript
