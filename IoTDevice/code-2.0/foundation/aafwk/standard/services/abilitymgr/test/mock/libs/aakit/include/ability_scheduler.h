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

#ifndef OHOS_AAFWK_ABILITY_SCHEDULER_H
#define OHOS_AAFWK_ABILITY_SCHEDULER_H

#include "ability_scheduler_stub.h"
#include "ability_record.h"
#include "dummy_values_bucket.h"
#include "dummy_data_ability_predicates.h"
#include "dummy_result_set.h"

namespace OHOS {
namespace AAFwk {

/**
 * @class AbilityScheduler
 * AbilityScheduler is used to schedule ability kit lifecycle.
 */
class AbilityScheduler : public AbilitySchedulerStub, virtual RefBase {
public:
    AbilityScheduler();
    virtual ~AbilityScheduler();

    void ScheduleAbilityTransaction(const Want &want, const LifeCycleStateInfo &targetState) override;

    void SendResult(int requestCode, int resultCode, const Want &resultWant) override;

    const AbilityResult &GetResult() const;

    void ScheduleConnectAbility(const Want &want) override;

    void ScheduleDisconnectAbility(const Want &want) override;

    void ScheduleCommandAbility(const Want &want, bool restart, int startId) override;

    void ScheduleSaveAbilityState(PacMap &outState) override;

    void ScheduleRestoreAbilityState(const PacMap &inState) override;

    std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter) override;

    int OpenFile(const Uri &uri, const std::string &mode) override;

    int Insert(const Uri &uri, const ValuesBucket &value) override;

    int Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates) override;

    int Delete(const Uri &uri, const DataAbilityPredicates &predicates) override;

    std::shared_ptr<ResultSet> Query(
        const Uri &uri, std::vector<std::string> &columns, const DataAbilityPredicates &predicates) override;

    std::string GetType(const Uri &uri) override;

    int OpenRawFile(const Uri &uri, const std::string &mode) override;

    bool Reload(const Uri &uri, const PacMap &extras) override;

    int BatchInsert(const Uri &uri, const std::vector<ValuesBucket> &values) override;

private:
    AbilityResult result_;
};

}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_ABILITY_SCHEDULER_H
