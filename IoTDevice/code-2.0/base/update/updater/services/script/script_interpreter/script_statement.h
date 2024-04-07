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
#ifndef USCRIPT_STATEMENT_H
#define USCRIPT_STATEMENT_H

#include <vector>
#include "script_context.h"
#include "script_param.h"

namespace uscript {
class UScriptStatementList;
class UScriptExpression;

class UScriptStatementResult {
public:
    enum StatementResultType {
        STATEMENT_RESULT_TYPE_NORMAL = 1,
        STATEMENT_RESULT_TYPE_BREAK,
        STATEMENT_RESULT_TYPE_CONTINUE,
        STATEMENT_RESULT_TYPE_RTN,
        STATEMENT_RESULT_TYPE_ERROR
    };

public:
    UScriptStatementResult() {}
    ~UScriptStatementResult() {}

    UScriptStatementResult(StatementResultType type, UScriptValuePtr value) : type_(type), value_(value) {}

    StatementResultType GetResultType() const
    {
        return type_;
    }

    void SetResultType(const StatementResultType type)
    {
        type_ = type;
    }

    UScriptValuePtr GetResultValue() const
    {
        return value_;
    }

    void SetResultValue(const UScriptValuePtr value)
    {
        value_ = value;
    }

    void SetError(const int32_t error)
    {
        error_ = error;
    }

    int32_t GetError() const
    {
        return error_;
    }

    void UpdateStatementResult(UScriptValuePtr value);
    static std::string ScriptToString(UScriptStatementResult *result);

private:
    StatementResultType type_ = STATEMENT_RESULT_TYPE_NORMAL;
    UScriptValuePtr value_ = nullptr;
    int32_t error_ = 0;
};

class UScriptStatement {
public:
    enum StatementType {
        STATEMENT_TYPE_EXPRESSION = 1,
        STATEMENT_TYPE_IF,
        STATEMENT_TYPE_FOR,
        STATEMENT_TYPE_WHILE,
        STATEMENT_TYPE_BREAK,
        STATEMENT_TYPE_CONTINUE,
        STATEMENT_TYPE_RTN
    };

    explicit UScriptStatement(UScriptStatement::StatementType type) : type_(type) {}

    virtual ~UScriptStatement() {}

    static UScriptStatement* CreateStatement(UScriptStatement::StatementType type);
    // ExpressionStatement
    static UScriptStatement* CreateExpressionStatement(UScriptExpression *expression);
    // IFStatement
    static UScriptStatement* CreateIfStatement(UScriptExpression *condition,
        UScriptStatementList *list1,
        UScriptStatementList *list2 = nullptr,
        UScriptStatement* nextIfState = nullptr);
    // FORStatement
    static UScriptStatement* CreateForStatement(UScriptExpression *before,
        UScriptExpression *condition,
        UScriptExpression *after,
        UScriptStatementList *list);
    // while
    static UScriptStatement* CreateWhileStatement(UScriptExpression *condition, UScriptStatementList *list);

    virtual UScriptStatementResult Execute(ScriptInterpreter &interpreter, UScriptContextPtr context) = 0;

    StatementType GetType() const
    {
        return type_;
    }

private:
    enum StatementType type_;
};

class UScriptStatementCtrl : public UScriptStatement {
public:
    explicit UScriptStatementCtrl(UScriptStatement::StatementType type) : UScriptStatement(type) {}
    ~UScriptStatementCtrl() override {}
    UScriptStatementResult Execute(ScriptInterpreter &interpreter, UScriptContextPtr context) override;
};

class UScriptExpressionStatement : public UScriptStatement {
public:
    explicit UScriptExpressionStatement(UScriptExpression *expression) :
        UScriptStatement(STATEMENT_TYPE_EXPRESSION), expression_(expression) {}
    ~UScriptExpressionStatement() override;
    UScriptStatementResult Execute(ScriptInterpreter &interpreter, UScriptContextPtr context) override;
private:
    UScriptExpression* expression_;
};

class UScriptIfStatement : public UScriptStatement {
public:
    UScriptIfStatement(UScriptExpression *expression, UScriptStatementList *statements)
        : UScriptStatement(STATEMENT_TYPE_IF), expression_(expression), trueStatements_(statements)  {}
    ~UScriptIfStatement() override;
    UScriptStatementResult Execute(ScriptInterpreter &interpreter, UScriptContextPtr context) override;

    void AddFalseStatementList(UScriptStatementList *statements)
    {
        falseStatements_ = statements;
    }

    void AddNextStatement(UScriptIfStatement *statement)
    {
        nextStatement_ = statement;
    }
private:
    UScriptExpression* expression_ = nullptr;
    UScriptStatementList* trueStatements_ = nullptr;
    UScriptStatementList* falseStatements_ = nullptr;
    UScriptIfStatement* nextStatement_ = nullptr;
};

class UScriptForStatement : public UScriptStatement {
public:
    UScriptForStatement(UScriptExpression *before, UScriptExpression *condition, UScriptExpression *after,
        UScriptStatementList *statements) : UScriptStatement(STATEMENT_TYPE_FOR), before_(before),
        condition_(condition), after_(after), statements_(statements) {}

    ~UScriptForStatement() override;
    UScriptStatementResult Execute(ScriptInterpreter &interpreter, UScriptContextPtr context) override;

private:
    UScriptExpression* before_ = nullptr;
    UScriptExpression* condition_ = nullptr;
    UScriptExpression* after_ = nullptr;
    UScriptStatementList* statements_ = nullptr;
};

class UScriptWhileStatement : public UScriptStatement {
public:
    UScriptWhileStatement(UScriptExpression *condition, UScriptStatementList *statements) :
        UScriptStatement(STATEMENT_TYPE_WHILE), condition_(condition), statements_(statements) {}

    ~UScriptWhileStatement() override;
    UScriptStatementResult Execute(ScriptInterpreter &interpreter, UScriptContextPtr context) override;

private:
    UScriptExpression *condition_ = nullptr;
    UScriptStatementList *statements_ = nullptr;
};

class UScriptReturnStatement : public UScriptStatement {
public:
    UScriptReturnStatement() : UScriptStatement(STATEMENT_TYPE_RTN) {}

    ~UScriptReturnStatement() override;
    UScriptStatementResult Execute(ScriptInterpreter &interpreter, UScriptContextPtr context) override;

    void AddParams(ScriptParams* params)
    {
        params_ = params;
    }

    static UScriptReturnStatement* CreateStatement(ScriptParams *params);

private:
    ScriptParams* params_ {};
};

class UScriptStatementList {
public:
    UScriptStatementList() {}

    ~UScriptStatementList();

    static UScriptStatementList* CreateInstance(UScriptStatement *statement);
    void AddScriptStatement(UScriptStatement *statement);

    UScriptStatementResult Execute(ScriptInterpreter &interpreter, UScriptContextPtr context);

    static UScriptStatementResult DoExecute(ScriptInterpreter &inter, UScriptContextPtr context,
        UScriptStatementList *statements);

private:
    std::vector<UScriptStatement*> statements_ = {};
};
} // namespace uscript
#endif // USCRIPT_STATEMENT_H
