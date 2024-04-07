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
#include "stoperator.h"

namespace OHOS {
namespace STtools {
using std::string;

int StOperator::countChild = 0;

StOperator::StOperator()
    : g_parentOperator(nullptr), g_abilityType("0"), g_bundleName(""), g_abilityName(""), g_operatorName(""), g_message("")
{
    g_childOperator.clear();
    StOperator::countChild++;
}

StOperator::StOperator(
    const string &type, const string &bundle, const string &ability, const string &operatorName, const string &message)
    : g_parentOperator(nullptr),
      g_abilityType(type),
      g_bundleName(bundle),
      g_abilityName(ability),
      g_operatorName(operatorName),
      g_message(message)
{
    g_childOperator.clear();
    StOperator::countChild++;
}

StOperator::~StOperator()
{
    g_childOperator.clear();
    StOperator::countChild--;
}

int StOperator::GetCountChild()
{
    return StOperator::countChild;
}

string StOperator::GetAbilityType()
{
    return g_abilityType;
}

StOperator &StOperator::SetAbilityType(const string &type)
{
    g_abilityType = type;
    return *this;
}

string StOperator::GetBundleName()
{
    return g_bundleName;
}

StOperator &StOperator::SetBundleName(const string &bundleName)
{
    g_bundleName = bundleName;
    return *this;
}

string StOperator::GetAbilityName()
{
    return g_abilityName;
}

StOperator &StOperator::SetAbilityName(const string &abilityName)
{
    g_abilityName = abilityName;
    return *this;
}

string StOperator::GetOperatorName()
{
    return g_operatorName;
}

StOperator &StOperator::SetOperatorName(const string &operatorName)
{
    g_operatorName = operatorName;
    return *this;
}

string StOperator::GetMessage()
{
    return g_message;
}

StOperator &StOperator::SetMessage(const string &message)
{
    g_message = message;
    return *this;
}

StOperator &StOperator::AddChildOperator(std::shared_ptr<StOperator> childOperator)
{
    if (childOperator == nullptr) {
        return *this;
    }
    childOperator->g_parentOperator = std::make_shared<StOperator>(*this);
    g_childOperator.emplace_back(childOperator);
    return *this;
}

std::vector<std::shared_ptr<StOperator>> StOperator::GetChildOperator()
{
    return g_childOperator;
}
}  // namespace STtools
}  // namespace OHOS