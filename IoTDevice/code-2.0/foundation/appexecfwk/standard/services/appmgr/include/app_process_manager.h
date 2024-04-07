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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_AMS_MGR_PROCESS_MANAGER_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_AMS_MGR_PROCESS_MANAGER_H

#include <list>

#include "iremote_object.h"
#include "refbase.h"

#include "app_task_info.h"

namespace OHOS {

namespace AppExecFwk {

class AppProcessManager {

public:
    AppProcessManager();
    virtual ~AppProcessManager();

    /**
     * RemoveAppFromRecentList, Remove the corresponding latest application list data.
     *
     * @param appTaskInfo, the application task information.
     *
     * @return
     */
    void RemoveAppFromRecentList(const std::shared_ptr<AppTaskInfo> &appTaskInfo);

    /**
     * GetRecentAppList, Clear recent application list.
     *
     * @return
     */
    void ClearRecentAppList();

    /**
     * AddAppToRecentList, Add application to recent list.
     *
     * @param appName, the app name.
     * @param processName, the process name.
     * @param pid, the app pid.
     * @param recordId, the app record id.
     *
     * @return
     */
    void AddAppToRecentList(
        const std::string &appName, const std::string &processName, const pid_t pid, const int32_t recordId);

    /**
     * GetRecentAppList, Get a list of recent applications.
     *
     * @return a list of recent applications.
     */
    const std::list<const std::shared_ptr<AppTaskInfo>> &GetRecentAppList() const;

    /**
     * PushAppFront, Adjust the latest application record to the top level.
     *
     * @param recordId, the app record id.
     *
     * @return
     */
    void PushAppFront(const int32_t recordId);

    /**
     * RemoveAppFromRecentListById, Remove the specified recent application record by application record id.
     *
     * @param recordId, the app record id.
     *
     * @return
     */
    void RemoveAppFromRecentListById(const int32_t recordId);

    /**
     * AddAppToRecentList, Get application task information through ID.
     *
     * @param recordId, the app record id.
     *
     * @return application task information.
     */
    const std::shared_ptr<AppTaskInfo> GetAppTaskInfoById(const int32_t recordId) const;

    /**
     * GetAppTaskInfoByProcessName, Get application task information through process name.
     *
     * @param appName, the application name.
     * @param processName, the process name.
     *
     * @return application task information.
     */
    std::shared_ptr<AppTaskInfo> GetAppTaskInfoByProcessName(
        const std::string &appName, const std::string &processName) const;

private:
    std::list<const std::shared_ptr<AppTaskInfo>> recentAppList_;
};

}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_AMS_MGR_PROCESS_MANAGER_H