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

#include "appmgr/app_mgr_client.h"
#include "ability_info.h"
#include "ability_manager_service.h"
#include "ability_scheduler.h"
#include "application_info.h"
#include "hilog_wrapper.h"

using namespace OHOS::AAFwk;

namespace OHOS {
namespace AppExecFwk {

int AppMgrClient::loadAbilityCount = 0;
int AppMgrClient::terminateAbilityCount = 0;
int AppMgrClient::foregroundCount = 0;
int AppMgrClient::backgroundCount = 0;

AppMgrResultCode AppMgrClient::LoadAbility(const sptr<IRemoteObject> &__attribute__((unused)) token,
    const sptr<IRemoteObject> &__attribute__((unused)) preToken, const AbilityInfo &__attribute__((unused)) abilityInfo,
    const ApplicationInfo &__attribute__((unused)) appInfo)
{
    HILOG_DEBUG("AppMgrProxy LoadAbility");
    loadAbilityCount++;
    return AppMgrResultCode::RESULT_OK;
}

AppMgrResultCode AppMgrClient::TerminateAbility(const sptr<IRemoteObject> &__attribute__((unused)) token)
{
    HILOG_DEBUG("AppMgrProxy TerminateAbility");
    terminateAbilityCount++;
    return AppMgrResultCode::RESULT_OK;
}

AppMgrResultCode AppMgrClient::UpdateAbilityState(const sptr<IRemoteObject> &token, const AbilityState state)
{
    switch (state) {
        case AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND: {
            foregroundCount++;
            if (callback_ != nullptr) {
                callback_->OnAbilityRequestDone(token, state);
            }
            break;
        }
        case AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND: {
            HILOG_DEBUG("AppMgrProxy update ability state to background");
            backgroundCount++;
            if (callback_ != nullptr) {
                callback_->OnAbilityRequestDone(token, state);
            }
            break;
        }
        default: {
            HILOG_DEBUG("AppMgrProxy default case!");
            break;
        }
    }
    return AppMgrResultCode::RESULT_OK;
}

AppMgrResultCode AppMgrClient::KillProcessByAbilityToken(const sptr<IRemoteObject> &token)
{
    return AppMgrResultCode::RESULT_OK;
}

AppMgrResultCode AppMgrClient::RegisterAppStateCallback(const sptr<IAppStateCallback> &callback)
{
    callback_ = callback;
    return AppMgrResultCode::RESULT_OK;
}

AppMgrResultCode AppMgrClient::ConnectAppMgrService()
{
    return AppMgrResultCode::RESULT_OK;
}

AppMgrResultCode AppMgrClient::AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token,
    const sptr<IRemoteObject> &preToken, const int32_t visibility, const int32_t perceptibility,
    const int32_t connectionState)
{
    return AppMgrResultCode::RESULT_OK;
}

AppMgrResultCode AppMgrClient::KillApplication(const std::string &bundleName)
{
    return AppMgrResultCode::RESULT_OK;
}

}  // namespace AppExecFwk
}  // namespace OHOS
