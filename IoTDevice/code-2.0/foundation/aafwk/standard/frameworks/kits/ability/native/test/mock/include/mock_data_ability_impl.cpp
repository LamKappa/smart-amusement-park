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

#include "data_ability_impl.h"
#include <gtest/gtest.h>
#include "app_log_wrapper.h"

const int returnValueOpenfile = 11;
const int returnValueInsert = 22;
const int returnValueUpdate = 33;
const int returnValueDelete = 44;

namespace OHOS {
namespace AppExecFwk {
void DataAbilityImpl::HandleAbilityTransaction(const Want &want, const AAFwk::LifeCycleStateInfo &targetState)
{
    GTEST_LOG_(INFO) << "Mock DataAbilityImpl::HandleAbilityTransaction called";
}

std::vector<std::string> DataAbilityImpl::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    GTEST_LOG_(INFO) << "Mock DataAbilityImpl::GetFileTypes called";
    std::vector<std::string> types;
    types.push_back("Type1");
    types.push_back("Type2");
    types.push_back("Type3");
    return types;
}

int DataAbilityImpl::OpenFile(const Uri &uri, const std::string &mode)
{
    GTEST_LOG_(INFO) << "Mock DataAbilityImpl::OpenFile called";
    return returnValueOpenfile;
}

int DataAbilityImpl::Insert(const Uri &uri, const ValuesBucket &value)
{
    GTEST_LOG_(INFO) << "Mock DataAbilityImpl::Insert called";
    return returnValueInsert;
}

int DataAbilityImpl::Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
{
    GTEST_LOG_(INFO) << "Mock DataAbilityImpl::Update called";
    return returnValueUpdate;
}

int DataAbilityImpl::Delete(const Uri &uri, const DataAbilityPredicates &predicates)
{
    GTEST_LOG_(INFO) << "Mock DataAbilityImpl::Delete called";
    return returnValueDelete;
}

std::shared_ptr<ResultSet> DataAbilityImpl::Query(
    const Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
{
    GTEST_LOG_(INFO) << "Mock DataAbilityImpl::Query called";
    std::shared_ptr<ResultSet> resultSet = std::make_shared<ResultSet>("TestResultSet");
    return resultSet;
}

std::string DataAbilityImpl::GetType(const Uri &uri)
{
    GTEST_LOG_(INFO) << "Mock DataAbilityImpl::GetType called";
    std::string type("type");
    return type;
}
}  // namespace AppExecFwk
}  // namespace OHOS