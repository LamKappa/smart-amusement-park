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

#ifndef FOUNDATION_APPEXECFWK_OHOS_ABILITY_NATIVE_MOCK_ABILITY_MANAGER_CLIENT_H
#define FOUNDATION_APPEXECFWK_OHOS_ABILITY_NATIVE_MOCK_ABILITY_MANAGER_CLIENT_H

#include <gtest/gtest.h>
#include <mutex>
#include "ability_connect_callback_interface.h"
#include "ability_manager_errors.h"
#include "ability_context.h"
#include "ability_manager_interface.h"
#include "ability_scheduler_interface.h"
#include "app_log_wrapper.h"
#include "iremote_object.h"
#include "iremote_stub.h"
#include "want.h"

#define OPENFILENUM (1246)
#define INSERTNUM (2345)
#define UPDATENUM (3456)
#define OPENRAWFILENUM (5678)
#define DELETENUM (6789)
#define BATCHINSERTNUM (789)

namespace OHOS {
namespace AppExecFwk {
class MockAbility : public AbilityContext {
public:
    MockAbility() = default;
    virtual ~MockAbility() = default;
};

class MockAbilityThread : public IRemoteStub<AAFwk::IAbilityScheduler> {
public:
    void ScheduleAbilityTransaction(const Want &want, const AAFwk::LifeCycleStateInfo &targetState)
    {}

    void SendResult(int requestCode, int resultCode, const Want &resultWant)
    {}

    void ScheduleConnectAbility(const Want &want)
    {}

    void ScheduleDisconnectAbility()
    {}

    void ScheduleSaveAbilityState(PacMap &outState)
    {}

    void ScheduleRestoreAbilityState(const PacMap &inState)
    {}

    void ScheduleDisconnectAbility(const Want &want)
    {}

    void ScheduleCommandAbility(const Want &want, bool restart, int startId)
    {}

    std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
    {
        GTEST_LOG_(INFO) << "MockAbilityThread::GetFileTypes called";
        std::vector<std::string> types;
        types.push_back("Types1");
        types.push_back("Types2");
        types.push_back("Types3");
        return types;
    }

    int OpenFile(const Uri &uri, const std::string &mode)
    {
        GTEST_LOG_(INFO) << "MockAbilityThread::OpenFile called";
        return OPENFILENUM;
    }

    int Insert(const Uri &uri, const ValuesBucket &value)
    {
        GTEST_LOG_(INFO) << "MockAbilityThread::Insert called";
        return INSERTNUM;
    }

    int Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
    {
        GTEST_LOG_(INFO) << "MockAbilityThread::Update called";
        return UPDATENUM;
    }

    int Delete(const Uri &uri, const DataAbilityPredicates &predicates)
    {
        GTEST_LOG_(INFO) << "MockAbilityThread::Delete called";
        return DELETENUM;
    }

    int OpenRawFile(const Uri &uri, const std::string &mode)
    {
        GTEST_LOG_(INFO) << "MockAbilityThread::OpenRawFile called";
        return OPENRAWFILENUM;
    }

    bool Reload(const Uri &uri, const PacMap &extras)
    {
        GTEST_LOG_(INFO) << "MockAbilityThread::Reload called";
        return true;
    }

    int BatchInsert(const Uri &uri, const std::vector<ValuesBucket> &values)
    {
        GTEST_LOG_(INFO) << "MockAbilityThread::BatchInsert called";
        return BATCHINSERTNUM;
    }

    std::shared_ptr<ResultSet> Query(
        const Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
    {
        GTEST_LOG_(INFO) << "MockAbilityThread::Query called";
        std::shared_ptr<ResultSet> set = std::make_shared<ResultSet>("resultset");
        return set;
    }

    std::string GetType(const Uri &uri)
    {
        return "Type1";
    }
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_ABILITY_NATIVE_MOCK_ABILITY_MANAGER_CLIENT_H