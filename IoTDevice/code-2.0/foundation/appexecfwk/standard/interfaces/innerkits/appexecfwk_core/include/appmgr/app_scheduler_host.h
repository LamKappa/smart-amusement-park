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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_SCHEDULER_HOST_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_SCHEDULER_HOST_H

#include <map>

#include "iremote_object.h"
#include "iremote_stub.h"
#include "nocopyable.h"
#include "app_scheduler_interface.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
class AppSchedulerHost : public IRemoteStub<IAppScheduler> {
public:
    AppSchedulerHost();
    virtual ~AppSchedulerHost();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t HandleScheduleForegroundApplication(MessageParcel &data, MessageParcel &reply);
    int32_t HandleScheduleBackgroundApplication(MessageParcel &data, MessageParcel &reply);
    int32_t HandleScheduleTerminateApplication(MessageParcel &data, MessageParcel &reply);
    int32_t HandleScheduleLowMemory(MessageParcel &data, MessageParcel &reply);
    int32_t HandleScheduleShrinkMemory(MessageParcel &data, MessageParcel &reply);
    int32_t HandleScheduleLaunchAbility(MessageParcel &data, MessageParcel &reply);
    int32_t HandleScheduleCleanAbility(MessageParcel &data, MessageParcel &reply);
    int32_t HandleScheduleLaunchApplication(MessageParcel &data, MessageParcel &reply);
    int32_t HandleScheduleProfileChanged(MessageParcel &data, MessageParcel &reply);
    int32_t HandleScheduleConfigurationUpdated(MessageParcel &data, MessageParcel &reply);
    int32_t HandleScheduleProcessSecurityExit(MessageParcel &data, MessageParcel &reply);

    using AppSchedulerFunc = int32_t (AppSchedulerHost::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, AppSchedulerFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(AppSchedulerHost);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_SCHEDULER_HOST_H
