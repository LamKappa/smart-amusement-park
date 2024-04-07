/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SCRIPT_INSTRUCTION_H
#define SCRIPT_INSTRUCTION_H

#include <string>
#include "pkg_manager.h"

namespace uscript {
class UScriptInstructionFactory;
class UScriptInstruction;
typedef UScriptInstructionFactory* UScriptInstructionFactoryPtr;
typedef UScriptInstruction* UScriptInstructionPtr;

/**
 * 定义环境变量，记录需要使用的全局对象
 */
class UScriptEnv {
public:
    UScriptEnv(hpackage::PkgManager::PkgManagerPtr pkgManager) : pkgManager_(pkgManager) {}

    virtual ~UScriptEnv() {}

    hpackage::PkgManager::PkgManagerPtr GetPkgManager()
    {
        return pkgManager_;
    }
    int32_t GetState()
    {
        return state_;
    }

    virtual void PostMessage(const std::string &cmd, std::string content) = 0;
    virtual UScriptInstructionFactoryPtr GetInstructionFactory() = 0;
    virtual const std::vector<std::string> GetInstructionNames() const = 0;
    virtual bool IsRetry() const = 0;
private:
    hpackage::PkgManager::PkgManagerPtr pkgManager_ = nullptr;
    int32_t state_ = 0;
};

/**
 * 脚本执行时的上下文描述，在调用脚本指令时，使用这个函数传参
 * 输入参数使用 GetParam 获取
 * 输出参数使用 PushParam 添加
 */
class UScriptContext {
public:
    enum ParamType {
        PARAM_TYPE_INTEGER = 1, // 整数类型
        PARAM_TYPE_FLOAT, // float类型
        PARAM_TYPE_STRING, // string类型
        PARAM_TYPE_INVALID = -1
    };

    virtual ~UScriptContext() = default;

    /**
     * 按不同的类型添加一个输出参数，可以添加任意输出
     */
    virtual int32_t PushParam(int value) = 0;
    virtual int32_t PushParam(float value) = 0;
    virtual int32_t PushParam(const std::string& value) = 0;

    /**
     * 获取输入参数的个数
     */
    virtual int32_t GetParamCount() = 0;

    /**
     * 获取对应索引的输入参数的类型
     */
    virtual ParamType GetParamType(int32_t index) = 0;

    /**
     * 获取对应索引的输入参数的值
     */
    virtual int32_t GetParam(int32_t index, int32_t& value) = 0;
    virtual int32_t GetParam(int32_t index, float& value) = 0;
    virtual int32_t GetParam(int32_t index, std::string& value) = 0;
};

/**
 * 脚本执行指令，实现对应指令的功能
 */
class UScriptInstruction {
public:
    virtual ~UScriptInstruction() = default;

    /**
     * 脚本调用，执行函数
     * context ： 函数执行的上下文信息
     * 入参：通过GetParam可以获取输入参数的类型和值
     * 出参：PushParam将输出结果压栈
     * 返回值：函数处理结果
     */
    virtual int32_t Execute(UScriptEnv &env, UScriptContext &context) = 0;
};

/**
 * 脚本执行指令的工厂类，根据指令名创建对应的实例
 */
class UScriptInstructionFactory {
public:
    // 创建时必须使用new，在程序结束后，会使用delete删除指令对象
    virtual int32_t CreateInstructionInstance(UScriptInstructionPtr &instr, const std::string &name) = 0;

    virtual ~UScriptInstructionFactory() = default;
};
} // namespace uscript

#ifdef __cplusplus
extern "C"{
#endif

/**
 * 接口，用来从用户自定义的共享库中获取factory
 */
uscript::UScriptInstructionFactoryPtr GetInstructionFactory();

#ifdef __cplusplus
}
#endif
#endif