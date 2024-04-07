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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_LMK_MANAGER_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_LMK_MANAGER_H

#include <list>
#include <string>

#include "app_running_record.h"
#include "cgroup_manager.h"
#include "nocopyable.h"

namespace OHOS {
namespace AppExecFwk {

class LmkUtil : public NoCopyable {
public:
    using AppPtr = std::shared_ptr<AppRunningRecord>;

public:
    LmkUtil();
    virtual ~LmkUtil();
    DISALLOW_COPY_AND_MOVE(LmkUtil);

    virtual int32_t KillProcess(std::list<AppPtr> &listApp, const CgroupManager::LowMemoryLevel level, int64_t &rss);

private:
    std::string GetProcName(pid_t pid);
    int32_t GetProcSize(pid_t pid);
    ssize_t ReadAll(int fd, char *buf, size_t maxLen);
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_LMK_MANAGER_H
