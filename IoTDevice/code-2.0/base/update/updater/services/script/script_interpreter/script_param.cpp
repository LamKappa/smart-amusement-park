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
#include "script_param.h"
#include "script_expression.h"
#include "script_utils.h"

using namespace std;

namespace uscript {
ScriptParams::~ScriptParams()
{
    for (auto iter = expressionList_.begin(); iter != expressionList_.end();) {
        delete *iter;
        iter = expressionList_.erase(iter);
    }
    this->expressionList_.clear();
}

ScriptParams* ScriptParams::CreateParams(UScriptExpression *expression)
{
    auto params = new ScriptParams();
    params->AddParams(expression);
    return params;
}

ScriptParams* ScriptParams::AddParams(ScriptParams *params, UScriptExpression *expression)
{
    if (params == nullptr) {
        params = new ScriptParams();
    }
    params->AddParams(expression);
    return params;
}

void ScriptParams::AddParams(UScriptExpression *expression)
{
    expressionList_.push_back(expression);
}
} // namespace uscript
