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

#include "app_scheduler.h"

#include "ability_manager_errors.h"
#include "ability_record.h"
#include "appmgr/app_mgr_constants.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
AppScheduler::AppScheduler() : appMgrClient_(std::make_unique<AppExecFwk::AppMgrClient>())
{}

AppScheduler::~AppScheduler()
{}

bool AppScheduler::Init(const std::weak_ptr<AppStateCallback> &callback)
{
    if (callback.lock() == nullptr) {
        HILOG_ERROR("AppScheduler Init:callback is nullptr");
        return false;
    }
    if (appMgrClient_ == nullptr) {
        HILOG_ERROR("AppScheduler Init:appMgrClient_ is nullptr");
        return false;
    }
    callback_ = callback;
    /* because the errcode type of AppMgr Client API will be changed to int,
     * so must to covert the return result  */
    int result = static_cast<int>(appMgrClient_->ConnectAppMgrService());
    if (result != ERR_OK) {
        HILOG_ERROR("failed to ConnectAppMgrService");
        return false;
    }
    this->IncStrongRef(this);
    result = static_cast<int>(appMgrClient_->RegisterAppStateCallback(sptr<AppScheduler>(this)));
    if (result != ERR_OK) {
        HILOG_ERROR("failed to RegisterAppStateCallback");
        return false;
    }
    HILOG_INFO("success to ConnectAppMgrService");
    return true;
}

int AppScheduler::LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const AppExecFwk::AbilityInfo &abilityInfo, const AppExecFwk::ApplicationInfo &applicationInfo)
{
    HILOG_DEBUG("AppScheduler::LoadAbility");
    if (appMgrClient_ == nullptr) {
        HILOG_ERROR("appMgrClient_ is nullptr");
        return INNER_ERR;
    }
    /* because the errcode type of AppMgr Client API will be changed to int,
     * so must to covert the return result  */
    int ret = static_cast<int>(appMgrClient_->LoadAbility(token, preToken, abilityInfo, applicationInfo));
    if (ret != ERR_OK) {
        HILOG_ERROR("AppScheduler fail to LoadAbility. ret %d", ret);
        return INNER_ERR;
    }
    return ERR_OK;
}

int AppScheduler::TerminateAbility(const sptr<IRemoteObject> &token)
{
    HILOG_DEBUG("AppScheduler::TerminateAbility");
    if (appMgrClient_ == nullptr) {
        HILOG_ERROR("appMgrClient_ is nullptr");
        return INNER_ERR;
    }
    /* because the errcode type of AppMgr Client API will be changed to int,
     * so must to covert the return result  */
    int ret = static_cast<int>(appMgrClient_->TerminateAbility(token));
    if (ret != ERR_OK) {
        HILOG_ERROR("AppScheduler fail to TerminateAbility. ret %d", ret);
        return INNER_ERR;
    }
    return ERR_OK;
}

void AppScheduler::MoveToForground(const sptr<IRemoteObject> &token)
{
    HILOG_DEBUG("AppScheduler::MoveToForground");
    if (appMgrClient_ == nullptr) {
        HILOG_ERROR("appMgrClient_ is nullptr");
        return;
    }
    appMgrClient_->UpdateAbilityState(token, AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND);
}

void AppScheduler::MoveToBackground(const sptr<IRemoteObject> &token)
{
    HILOG_DEBUG("AppScheduler::MoveToBackground");
    if (appMgrClient_ == nullptr) {
        HILOG_ERROR("appMgrClient_ is nullptr");
        return;
    }
    appMgrClient_->UpdateAbilityState(token, AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
}

void AppScheduler::AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const int32_t visibility, const int32_t perceptibility, const int32_t connectionState)
{
    HILOG_DEBUG("AppScheduler::AbilityBehaviorAnalysis");
    if (appMgrClient_ == nullptr) {
        HILOG_ERROR("appMgrClient_ is nullptr");
        return;
    }
    appMgrClient_->AbilityBehaviorAnalysis(token, preToken, visibility, perceptibility, connectionState);
}

void AppScheduler::KillProcessByAbilityToken(const sptr<IRemoteObject> &token)
{
    HILOG_DEBUG("AppScheduler::KillProcessByAbilityToken");
    if (appMgrClient_ == nullptr) {
        HILOG_ERROR("appMgrClient_ is nullptr");
        return;
    }
    appMgrClient_->KillProcessByAbilityToken(token);
}

AppAbilityState AppScheduler::ConvertToAppAbilityState(const int32_t state)
{
    AppExecFwk::AbilityState abilityState = static_cast<AppExecFwk::AbilityState>(state);
    switch (abilityState) {
        case AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND: {
            return AppAbilityState::ABILITY_STATE_FOREGROUND;
        }
        case AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND: {
            return AppAbilityState::ABILITY_STATE_BACKGROUND;
        }
        default:
            return AppAbilityState::ABILITY_STATE_UNDEFINED;
    }
}

AppAbilityState AppScheduler::GetAbilityState() const
{
    return appAbilityState_;
}

void AppScheduler::OnAbilityRequestDone(const sptr<IRemoteObject> &token, const AppExecFwk::AbilityState state)
{
    HILOG_INFO("AppScheduler::OnAbilityRequestDone:%d", static_cast<int32_t>(state));
    auto callback = callback_.lock();
    if (callback == nullptr) {
        HILOG_ERROR("callback_ is nullptr");
        return;
    }
    appAbilityState_ = ConvertToAppAbilityState(static_cast<int32_t>(state));
    callback->OnAbilityRequestDone(token, static_cast<int32_t>(state));
}

int AppScheduler::KillApplication(const std::string &bundleName)
{
    HILOG_DEBUG("%{public}s, %{public}d", __func__, __LINE__);
    if (appMgrClient_ == nullptr) {
        HILOG_ERROR("appMgrClient_ is nullptr");
        return INNER_ERR;
    }
    int ret = (int)appMgrClient_->KillApplication(bundleName);
    if (ret != ERR_OK) {
        HILOG_ERROR("Fail to kill application.");
        return INNER_ERR;
    }

    return ERR_OK;
}
}  // namespace AAFwk
}  // namespace OHOS
