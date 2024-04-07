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

#ifndef MOCK_OHOS_AAFWK_ABILITY_SCHEDULER_INTERFACE_H
#define MOCK_OHOS_AAFWK_ABILITY_SCHEDULER_INTERFACE_H

#include "want.h"
#include "lifecycle_state_info.h"
#include "gmock/gmock.h"
#include "ability_scheduler_interface.h"

namespace OHOS {
namespace AAFwk {

class MockAbilityScheduler : public IAbilityScheduler {
public:
    MOCK_METHOD2(ScheduleAbilityTransaction, void(const Want &want, const LifeCycleStateInfo &targetState));
    MOCK_METHOD3(SendResult, void(int requestCode, int resultCode, const Want &resultWant));
    MOCK_METHOD1(ScheduleConnectAbility, void(const Want &want));
    MOCK_METHOD1(ScheduleDisconnectAbility, void(const Want &want));
    MOCK_METHOD3(ScheduleCommandAbility, void(const Want &want, bool restart, int startid));
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
    MOCK_METHOD1(ScheduleSaveAbilityState, void(PacMap &outState));
    MOCK_METHOD1(ScheduleRestoreAbilityState, void(const PacMap &inState));
    MOCK_METHOD1(ScheduleNewWant, void(const Want &want));

    std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
    {
        std::vector<std::string> types;
        return types;
    }

    int OpenFile(const Uri &uri, const std::string &mode)
    {
        return -1;
    }

    int Insert(const Uri &uri, const ValuesBucket &value)
    {
        return -1;
    }

    int Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates)
    {
        return -1;
    }

    int Delete(const Uri &uri, const DataAbilityPredicates &predicates)
    {
        return -1;
    }

    std::shared_ptr<ResultSet> Query(
        const Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &predicates)
    {
        return nullptr;
    }

    virtual std::string GetType(const Uri &uri) override
    {
        return " ";
    }

    virtual int OpenRawFile(const Uri &uri, const std::string &mode) override
    {
        return -1;
    }

    virtual bool Reload(const Uri &uri, const PacMap &extras) override
    {
        return false;
    }

    virtual int BatchInsert(const Uri &uri, const std::vector<ValuesBucket> &values) override
    {
        return -1;
    }
};

}  // namespace AAFwk
}  // namespace OHOS

#endif  // MOCK_OHOS_AAFWK_ABILITY_SCHEDULER_INTERFACE_H
