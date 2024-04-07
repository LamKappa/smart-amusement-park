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

#include "ability_manager.h"
#include "app_log_wrapper.h"
#include "singleton.h"
#include "system_ability_definition.h"
#include "sys_mgr_client.h"

namespace OHOS {
namespace AppExecFwk {

AbilityManager &AbilityManager::GetInstance()
{
    static AbilityManager abilityManager;
    return abilityManager;
}

void AbilityManager::StartAbility(const Want &want, int requestCode = -1)
{
    APP_LOGD("%s, %d", __func__, __LINE__);
    ErrCode error = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want, requestCode);
    if (error != ERR_OK) {
        APP_LOGE("%s failed, error : %d", __func__, error);
    }
}

void AbilityManager::MoveMissionToTop(int missionId)
{
    APP_LOGD("%s, %d", __func__, __LINE__);
    ErrCode error = AAFwk::AbilityManagerClient::GetInstance()->MoveMissionToTop(missionId);
    if (error != ERR_OK) {
        APP_LOGE("%s failed, error : %d", __func__, error);
    }
}

StackInfo AbilityManager::GetAllStackInfo() const
{
    APP_LOGD("%s, %d", __func__, __LINE__);
    StackInfo info;
    ErrCode error = AAFwk::AbilityManagerClient::GetInstance()->GetAllStackInfo(info);
    if (error != ERR_OK) {
        APP_LOGE("%s failed, error : %d", __func__, error);
    }

    return info;
}

std::vector<RecentMissionInfo> AbilityManager::QueryRecentAbilityMissionInfo(int numMax, int flags) const
{
    APP_LOGD("%s, %d", __func__, __LINE__);
    std::vector<RecentMissionInfo> info;
    ErrCode error = AAFwk::AbilityManagerClient::GetInstance()->GetRecentMissions(numMax, flags, info);
    if (error != ERR_OK) {
        APP_LOGE("%s failed, error : %d", __func__, error);
    }

    return info;
}

std::vector<RecentMissionInfo> AbilityManager::QueryRunningAbilityMissionInfo(int numMax) const
{
    APP_LOGD("%s, %d", __func__, __LINE__);
    std::vector<RecentMissionInfo> info;
    ErrCode error =
        AAFwk::AbilityManagerClient::GetInstance()->GetRecentMissions(numMax, RECENT_IGNORE_UNAVAILABLE, info);
    if (error != ERR_OK) {
        APP_LOGE("%s failed, error : %d", __func__, error);
    }

    return info;
}

void AbilityManager::RemoveMissions(const std::vector<int> &missionId)
{
    APP_LOGD("%s, %d", __func__, __LINE__);
    ErrCode error = AAFwk::AbilityManagerClient::GetInstance()->RemoveMissions(missionId);
    if (error != ERR_OK) {
        APP_LOGE("%s failed, error : %d", __func__, error);
    }
}

void AbilityManager::ClearUpApplicationData(const std::string &bundleName)
{
    APP_LOGD("%s, %d", __func__, __LINE__);
    auto object = OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->GetSystemAbility(APP_MGR_SERVICE_ID);
    sptr<IAppMgr> appMgr_ = iface_cast<IAppMgr>(object);
    if (appMgr_ == nullptr) {
        APP_LOGE("%s, appMgr_ is nullptr", __func__);
        return;
    }

    appMgr_->ClearUpApplicationData(bundleName);
}

RunningProcessInfo AbilityManager::GetAllRunningProcesses()
{
    APP_LOGD("%s, %d", __func__, __LINE__);
    auto object = OHOS::DelayedSingleton<SysMrgClient>::GetInstance()->GetSystemAbility(APP_MGR_SERVICE_ID);
    sptr<IAppMgr> appMgr_ = iface_cast<IAppMgr>(object);
    RunningProcessInfo info;
    std::shared_ptr<RunningProcessInfo> prpcessInfo = std::make_shared<RunningProcessInfo>();

    if (appMgr_ == nullptr || prpcessInfo == nullptr) {
        APP_LOGE("%s, appMgr_ is nullptr", __func__);
        return info;
    }

    appMgr_->GetAllRunningProcesses(prpcessInfo);
    info.appProcessInfos = prpcessInfo->appProcessInfos;
    return info;
}

}  // namespace AppExecFwk
}  // namespace OHOS