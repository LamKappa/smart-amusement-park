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

#ifndef FOUNDATION_APPEXECFWK_OHOS_ABILITY_RECORD_MGR_H
#define FOUNDATION_APPEXECFWK_OHOS_ABILITY_RECORD_MGR_H

#include <map>
#include "iremote_object.h"
#include "ability_local_record.h"

namespace OHOS {
namespace AppExecFwk {
class AbilityLocalRecord;
class AbilityRecordMgr {
public:
    AbilityRecordMgr() = default;
    ~AbilityRecordMgr() = default;

    /**
     * @brief Set the eventRunner of abilitythread to the AbilityRecordMgr.
     *
     * @param eventRunner The runner of the abilitythread.
     *
     */
    void SetEventRunner(const std::shared_ptr<EventRunner> &eventRunner);

    /**
     * @brief Get the token witch is set to the AbilityRecordMgr.
     *
     * @return Returns the token which is set to the AbilityRecordMgr.
     */
    sptr<IRemoteObject> GetToken() const;

    /**
     * @brief Set the token witch the app launched.
     *
     * @param token The token which the is launched by app.
     */
    void SetToken(const sptr<IRemoteObject> &token);

    /**
     * @brief Save the token and abilityRecord to the AbilityRecordMgr.
     *
     * @param token The token which the abilityRecord belongs to.
     * @param abilityRecord the abilityRecord witch contains the context info belong the the ability.
     *
     */
    void AddAbilityRecord(const sptr<IRemoteObject> &token, const std::shared_ptr<AbilityLocalRecord> &abilityRecord);

    /**
     * @brief Remove the abilityRecord by token.
     *
     * @param token The token which the abilityRecord belongs to.
     *
     */
    void RemoveAbilityRecord(const sptr<IRemoteObject> &token);

    /**
     * @brief Get the number of abilityRecords which the AbilityRecordMgr saved.
     *
     * @return Return the number of abilityRecords which the AbilityRecordMgr saved.
     *
     */
    int GetRecordCount() const;

    /**
     * @brief Get the abilityRecord by token.
     *
     * @param token The token which the abilityRecord belongs to.
     *
     */
    std::shared_ptr<AbilityLocalRecord> GetAbilityItem(const sptr<IRemoteObject> &token) const;

    /**
     * @brief Get the all tokens in the abilityRecordMgr.
     *
     * @return all tokens in the abilityRecordMgr.
     *
     */
    std::vector<sptr<IRemoteObject>> GetAllTokens();

private:
    std::map<sptr<IRemoteObject>, std::shared_ptr<AbilityLocalRecord>> abilityRecords_;
    sptr<IRemoteObject> tokens_;  // we use ThreadLocal
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_ABILITY_RECORD_MGR_H
