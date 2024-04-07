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

#ifndef ABILITY_MANAGER_H
#define ABILITY_MANAGER_H

#include "ability_manager_client.h"
#include "app_mgr_interface.h"
#include "stack_info.h"

namespace OHOS {
namespace AppExecFwk {

using OHOS::AAFwk::AbilityManagerClient;
using OHOS::AAFwk::RECENT_IGNORE_UNAVAILABLE;
using OHOS::AAFwk::RecentMissionInfo;
using OHOS::AAFwk::StackInfo;
using OHOS::AAFwk::Want;
using OHOS::AppExecFwk::RunningProcessInfo;
using RuningMissionInfo = RecentMissionInfo;

class AbilityManager {
public:
    AbilityManager() = default;
    virtual ~AbilityManager() = default;

    static AbilityManager &GetInstance();

    /**
     * StartAbility with want, send want to ability manager service.
     *
     * @param want Ability want.
     * @param requestCode Ability request code.
     * @return Returns ERR_OK on success, others on failure.
     */
    void StartAbility(const Want &want, int requestCode);

    /**
     * Ask that the mission associated with a given mission ID be moved to the
     * front of the stack, so it is now visible to the user.
     *
     * @param missionId mission record id
     */
    void MoveMissionToTop(int missionId);

    /**
     * Get all stack info from ability manager service.
     *
     * @return Return stack information
     */
    StackInfo GetAllStackInfo() const;

    /**
     * Query recent Ability Mission info.
     * @param numMax The maximum number of entries to return in the list. The
     * actual number returned may be smaller, depending on how many tasks the
     * user has started and the maximum number the system can remember.
     * @param falgs Information about what to return.  May be any combination
     * of {@link #RECENT_WITH_EXCLUDED} and {@link #RECENT_IGNORE_UNAVAILABLE}.
     *
     * @returns Returns the RecentMissionInfo.
     */
    std::vector<RecentMissionInfo> QueryRecentAbilityMissionInfo(int numMax, int flags) const;

    /**
     * Query running Ability Mission info.
     * @param numMax The maximum number of entries to return in the list. The
     * actual number returned may be smaller, depending on how many tasks the
     * user has started and the maximum number the system can remember.
     *
     * @returns Returns the RecentMissionInfo.
     */
    std::vector<RuningMissionInfo> QueryRunningAbilityMissionInfo(int numMax) const;

    /**
     * Remove the specified mission from the stack by mission id
     */
    void RemoveMissions(const std::vector<int> &missionId);

    /**
     * Clears user data of the application, which is equivalent to initializing the application.
     *
     * @param bundleName.
     */
    void ClearUpApplicationData(const std::string &bundleName);

    /**
     * Obtains information about application processes that are running on the device.
     *
     * @returns Returns a list of running processes.
     */
    RunningProcessInfo GetAllRunningProcesses();
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // ABILITY_MANAGER_H
