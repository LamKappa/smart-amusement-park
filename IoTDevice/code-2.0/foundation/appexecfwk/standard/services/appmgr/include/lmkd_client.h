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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_LMKD_CLIENT_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_LMKD_CLIENT_H

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ability_running_record.h"
#include "app_running_record.h"
#include "nocopyable.h"

namespace OHOS {
namespace AppExecFwk {

class LmkdClient : public NoCopyable {
public:
    using Targets = std::vector<std::pair<int, int>>;

public:
    LmkdClient();
    virtual ~LmkdClient();
    virtual int32_t Open();
    virtual void Close();
    virtual bool IsOpen() const;
    virtual int32_t Target(const Targets &targets);
    virtual int32_t ProcPrio(pid_t pid, uid_t uid, int oomAdj);
    virtual int32_t ProcRemove(pid_t pid);
    virtual bool ProcPurge();
    static bool CheckOomAdj(int v);

private:
    bool Write(const void *buf, size_t len);

private:
    int socket_;
};

}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_LMKD_CLIENT_H
