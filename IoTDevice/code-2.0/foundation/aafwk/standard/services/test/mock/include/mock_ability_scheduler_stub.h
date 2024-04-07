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

#ifndef FOUNDATION_AAFWK_SERVICES_TEST_MT_MOCK_ABILITY_SCHEDULER_STUB_H
#define FOUNDATION_AAFWK_SERVICES_TEST_MT_MOCK_ABILITY_SCHEDULER_STUB_H

#include "ability_scheduler_stub.h"

namespace OHOS {
namespace AAFwk {

class MockAbilitySchedulerStub : public AbilitySchedulerStub {
public:
    MOCK_METHOD2(ScheduleAbilityTransaction, void(const Want &, const LifeCycleStateInfo &));
    MOCK_METHOD3(SendResult, void(int, int, const Want &));
    MOCK_METHOD1(ScheduleConnectAbility, void(const Want &));
    MOCK_METHOD1(ScheduleDisconnectAbility, void(const Want &));
    MOCK_METHOD3(ScheduleCommandAbility, void(const Want &, bool, int));
    MOCK_METHOD1(ScheduleSaveAbilityState, void(PacMap &));
    MOCK_METHOD1(ScheduleRestoreAbilityState, void(const PacMap &));
    MOCK_METHOD2(GetFileTypes, std::vector<std::string>(const Uri &, const std::string &));
    MOCK_METHOD2(OpenFile, int(const Uri &, const std::string &));
    MOCK_METHOD2(Insert, int(const Uri &, const ValuesBucket &));
    MOCK_METHOD3(Update, int(const Uri &, const ValuesBucket &, const DataAbilityPredicates &));
    MOCK_METHOD2(Delete, int(const Uri &, const DataAbilityPredicates &));
    MOCK_METHOD3(
        Query, std::shared_ptr<ResultSet>(const Uri &, std::vector<std::string> &, const DataAbilityPredicates &));
    MOCK_METHOD1(GetType, std::string(const Uri &uri));
    MOCK_METHOD2(OpenRawFile, int(const Uri &uri, const std::string &mode));
    MOCK_METHOD2(Reload, bool(const Uri &uri, const PacMap &extras));
    MOCK_METHOD2(BatchInsert, int(const Uri &uri, const std::vector<ValuesBucket> &values));
};

}  // namespace AAFwk
}  // namespace OHOS

#endif  // FOUNDATION_AAFWK_SERVICES_TEST_MT_MOCK_ABILITY_SCHEDULER_STUB_H
