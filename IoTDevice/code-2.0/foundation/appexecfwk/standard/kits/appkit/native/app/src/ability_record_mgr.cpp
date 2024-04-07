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

#include "ability_record_mgr.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

/**
 * @brief Get the token witch is set to the AbilityRecordMgr.
 *
 * @return Returns the token which is set to the AbilityRecordMgr.
 */
sptr<IRemoteObject> AbilityRecordMgr::GetToken() const
{
    return tokens_;
}

/**
 * @brief Set the token witch the app launched.
 *
 * @param token The token which the is launched by app.
 */
void AbilityRecordMgr::SetToken(const sptr<IRemoteObject> &token)
{
    if (token == nullptr) {
        APP_LOGE("AbilityRecordMgr::SetToken failed, application is nullptr");
        return;
    }
    tokens_ = token;
}

/**
 * @brief Set the eventRunner of abilitythread to the AbilityRecordMgr.
 *
 * @param eventRunner The runner of the abilitythread.
 *
 */
void AbilityRecordMgr::SetEventRunner(const std::shared_ptr<EventRunner> &eventRunner)
{
    if (eventRunner == nullptr) {
        APP_LOGE("AbilityRecordMgr::SetEventRunner failed, eventRunner is nullptr");
        return;
    }
    sptr<IRemoteObject> token = GetToken();
    if (token == nullptr) {
        APP_LOGE("AbilityRecordMgr::SetEventRunner failed, token is nullptr");
        return;
    }

    std::shared_ptr<AbilityLocalRecord> abilityInstance = GetAbilityItem(token);
    if (abilityInstance != nullptr) {
        abilityInstance->SetEventRunner(eventRunner);
    } else {
        APP_LOGW("AbilityRecordMgr::setEventRunner failed, ability record is not exists");
    }
}

/**
 * @brief Save the token and abilityRecord to the AbilityRecordMgr.
 *
 * @param token The token which the abilityRecord belongs to.
 * @param abilityRecord the abilityRecord witch contains the context info belong the the ability.
 *
 */
void AbilityRecordMgr::AddAbilityRecord(
    const sptr<IRemoteObject> &token, const std::shared_ptr<AbilityLocalRecord> &abilityRecord)
{
    if (token == nullptr) {
        APP_LOGE("AbilityRecordMgr::AddAbilityRecord failed, token is nullptr");
        return;
    }

    if (abilityRecord == nullptr) {
        APP_LOGE("AbilityRecordMgr::AddAbilityRecord failed, abilityRecord is nullptr");
        return;
    }

    abilityRecords_[token] = abilityRecord;
}

/**
 * @brief Remove the abilityRecord by token.
 *
 * @param token The token which the abilityRecord belongs to.
 *
 */
void AbilityRecordMgr::RemoveAbilityRecord(const sptr<IRemoteObject> &token)
{
    if (token == nullptr) {
        APP_LOGE("AbilityRecordMgr::RemoveAbilityRecord failed, token is nullptr");
        return;
    }
    abilityRecords_.erase(token);
}

/**
 * @brief Get the number of abilityRecords which the AbilityRecordMgr saved.
 *
 * @return Return the number of abilityRecords which the AbilityRecordMgr saved.
 *
 */
int AbilityRecordMgr::GetRecordCount() const
{
    return abilityRecords_.size();
}

/**
 * @brief Get the abilityRecord by token.
 *
 * @param token The token which the abilityRecord belongs to.
 *
 */
std::shared_ptr<AbilityLocalRecord> AbilityRecordMgr::GetAbilityItem(const sptr<IRemoteObject> &token) const
{
    if (token == nullptr) {
        APP_LOGE("AbilityRecordMgr::GetAbilityItem failed, token is nullptr");
        return nullptr;
    }

    const auto &iter = abilityRecords_.find(token);
    if (iter != abilityRecords_.end()) {
        APP_LOGI("AbilityRecordMgr::GetAbilityItem : the ability found");
        return iter->second;
    } 
    APP_LOGI("AbilityRecordMgr::GetAbilityItem : the ability not found");
    return nullptr;
}

/**
 * @brief Get the all tokens in the abilityRecordMgr.
 *
 * @return all tokens in the abilityRecordMgr.
 *
 */
std::vector<sptr<IRemoteObject>> AbilityRecordMgr::GetAllTokens()
{
    std::vector<sptr<IRemoteObject>> tokens;
    for (std::map<sptr<IRemoteObject>, std::shared_ptr<AbilityLocalRecord>>::iterator it = abilityRecords_.begin();
         it != abilityRecords_.end();
         ++it) {
        sptr<IRemoteObject> token = it->first;
        tokens.emplace_back(token);
    }
    return tokens;
}

}  // namespace AppExecFwk
}  // namespace OHOS
