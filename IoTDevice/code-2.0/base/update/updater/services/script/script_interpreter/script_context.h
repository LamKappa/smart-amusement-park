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
#ifndef USCRIPT_CONTEXT_H
#define USCRIPT_CONTEXT_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "script_instruction.h"
#include "script_manager.h"

namespace uscript {
class ScriptInterpreter;
class UScriptValue;
class UScriptInterpretContext;

using UScriptValuePtr = std::shared_ptr<UScriptValue>;
using UScriptContextPtr = std::shared_ptr<UScriptInterpretContext>;

class UScriptValue {
public:
    enum UScriptValueType {
        VALUE_TYPE_INTEGER = UScriptContext::PARAM_TYPE_INTEGER,
        VALUE_TYPE_FLOAT = UScriptContext::PARAM_TYPE_FLOAT,
        VALUE_TYPE_STRING = UScriptContext::PARAM_TYPE_STRING,
        VALUE_TYPE_ERROR,
        VALUE_TYPE_LIST,
        VALUE_TYPE_RETURN,
    };

    explicit UScriptValue(UScriptValueType type) : type_(type) {}
    virtual ~UScriptValue() {}

    UScriptValueType GetValueType() const
    {
        return type_;
    }

    virtual bool IsTrue() const
    {
        return false;
    }

    virtual UScriptValuePtr Computer(int32_t action, UScriptValuePtr rightValue);

    virtual std::string ToString();

    static std::string ScriptToString(UScriptValuePtr value);

    static UScriptValuePtr GetRightCompluteValue(UScriptValuePtr rightValue);
private:
    UScriptValueType type_;
};

class IntegerValue : public UScriptValue {
public:
    explicit IntegerValue(int32_t value) : UScriptValue(UScriptValue::VALUE_TYPE_INTEGER), value_(value) {}
    ~IntegerValue() override {}

    bool IsTrue() const override
    {
        return value_ != 0;
    }

    int32_t GetValue() const
    {
        return value_;
    }

    UScriptValuePtr Computer(int32_t action, UScriptValuePtr rightValue) override;
    std::string ToString() override;

private:
    int32_t value_;
};

class FloatValue : public UScriptValue {
public:
    explicit FloatValue(float value) : UScriptValue(UScriptValue::VALUE_TYPE_FLOAT), value_(value) {}
    ~FloatValue() override {}

    bool IsTrue() const override
    {
        return value_ != 0;
    }

    float GetValue() const
    {
        return value_;
    }

    UScriptValuePtr Computer(int32_t action, UScriptValuePtr rightValue) override;
    std::string ToString() override;

private:
    bool ComputerEqual(UScriptValuePtr rightValue);
    float value_;
};

class StringValue : public UScriptValue {
public:
    explicit StringValue(std::string value) : UScriptValue(UScriptValue::VALUE_TYPE_STRING),
        value_(std::move(value)) {}
    ~StringValue() override {}

    bool IsTrue() const override
    {
        return !value_.empty();
    }

    std::string GetValue() const
    {
        return value_;
    }

    UScriptValuePtr Computer(int32_t action, UScriptValuePtr rightValue) override;

    UScriptValuePtr ComputerReturn(int32_t action, UScriptValuePtr rightValue, UScriptValuePtr defReturn) const;

    std::string ToString() override;
private:
    int32_t ComputerLogic(UScriptValuePtr rightValue) const;
    std::string value_;
};

class ReturnValue : public UScriptValue {
public:
    ReturnValue() : UScriptValue(UScriptValue::VALUE_TYPE_LIST) {}
    ~ReturnValue() override
    {
        values_.clear();
    }

    bool IsTrue() const override
    {
        return false;
    }

    UScriptValuePtr Computer(int32_t action, UScriptValuePtr rightValue) override;
    std::string ToString() override;

    void AddValue(const UScriptValuePtr value);
    void AddValues(const std::vector<UScriptValuePtr> values);
    std::vector<UScriptValuePtr> GetValues() const;

private:
    std::vector<UScriptValuePtr> values_ {};
};

class ErrorValue : public UScriptValue {
public:
    explicit ErrorValue(int32_t retCode) : UScriptValue(UScriptValue::VALUE_TYPE_ERROR), retCode_(retCode) {}
    ~ErrorValue() override {}

    // 对于返回值，true表示返回ok
    bool IsTrue() const override
    {
        return retCode_ == USCRIPT_SUCCESS;
    }

    int32_t GetValue() const
    {
        return retCode_;
    }

    int32_t SetValue(const int32_t code)
    {
        return retCode_ = code;
    }
    UScriptValuePtr Computer(int32_t action, UScriptValuePtr rightValue) override
    {
        return nullptr;
    }

    std::string ToString() override;

private:
    int32_t retCode_ = 0;
};

/**
 * 脚本指令上下文，用来在执行脚本指令时，传递输入、输出参数
 */
class UScriptInstructionContext : public UScriptContext {
public:
    UScriptInstructionContext() {}

    virtual ~UScriptInstructionContext() {}

    virtual int32_t PushParam(int32_t value) override;
    virtual int32_t PushParam(float value) override;
    virtual int32_t PushParam(const std::string &value) override;
    virtual int32_t GetParamCount() override;
    virtual ParamType GetParamType(int32_t index) override;
    virtual int32_t GetParam(int32_t index, int32_t &value) override;
    virtual int32_t GetParam(int32_t index, float &value) override;
    virtual int32_t GetParam(int32_t index, std::string &value) override;

    int32_t AddInputParam(UScriptValuePtr value);

    std::vector<UScriptValuePtr> GetOutVar() const
    {
        return outParam_;
    }

private:
    template<class T, class TWapper> int32_t GetOutputValue(int32_t index, T &value);
    template<class T, class TWapper> int32_t GetParam(int32_t index, T &value);

    std::vector<UScriptValuePtr> innerParam_ {};
    std::vector<UScriptValuePtr> outParam_ {};
};

/**
 * 脚本解析中的上下文，保存每一层的变量
 */
class UScriptInterpretContext {
public:
    explicit UScriptInterpretContext(bool top = false);

    virtual ~UScriptInterpretContext()
    {
        localVariables_.clear();
    }

    UScriptValuePtr FindVariable(const ScriptInterpreter &inter, std::string id);
    void UpdateVariable(const ScriptInterpreter &inter, std::string id, UScriptValuePtr value);
    void UpdateVariables(const ScriptInterpreter &inter,
        UScriptValuePtr value,
        std::vector<std::string> ids,
        size_t& startIndex);

    uint32_t GetContextId() const
    {
        return contextId_;
    }

    bool IsTop() const
    {
        return top_;
    }

private:
    uint32_t contextId_ = 0;
    bool top_ = false;
    std::map<std::string, UScriptValuePtr> localVariables_ {};
};
} // namespace uscript
#endif // USCRIPT_CONTEXT_H
