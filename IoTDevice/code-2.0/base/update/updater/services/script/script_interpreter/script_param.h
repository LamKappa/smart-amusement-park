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
#ifndef USCRIPT_PARAM_H
#define USCRIPT_PARAM_H

#include <cstdint>
#include <string>
#include <vector>
#include "script_context.h"

namespace uscript {
/**
 * 定义函数的参数，使用vector保存函数参数
 * 在定义函数时，实参，对应标识符表达式
 * 函数调用时，可能是标识符，也可能是字符串、整数、浮点数或者函数调用
 */
class UScriptExpression;

class ScriptParams {
public:
    friend class ScriptFunction;
    static const int32_t g_maxParamNumber = 5;

    ScriptParams() {}
    ~ScriptParams();
    void AddParams(UScriptExpression *expression);
    const std::vector<UScriptExpression*> GetParams() const
    {
        return expressionList_;
    }

    static ScriptParams* CreateParams(UScriptExpression *expression);
    static ScriptParams* AddParams(ScriptParams *params, UScriptExpression *expression);

private:
    std::vector<UScriptExpression*> expressionList_ {};
};
} // namespace uscript
#endif // HS_PARAM_H
