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

#ifndef OHOS_AAFWK_APPMGR_TEST_SERVICE_H
#define OHOS_AAFWK_APPMGR_TEST_SERVICE_H

#include "ability_manager_service.h"
#include "event_handler.h"
#include "event_runner.h"
#include <map>

namespace OHOS {
namespace AAFwk {
class AppMgrEventHandler : public AppExecFwk::EventHandler {
public:
    AppMgrEventHandler(
        const std::shared_ptr<AppExecFwk::EventRunner> &runner, const std::shared_ptr<AbilityManagerService> &server);
    virtual ~AppMgrEventHandler()
    {}

    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;

private:
    static constexpr int BLOCK_TEST_TIME = 500000; /* Blocked test time is 500ms */
    void ProcessLoadAbility(const AppExecFwk::InnerEvent::Pointer &event);

    void ProcessUpdateAppState(const AppExecFwk::InnerEvent::Pointer &event);

    void ScheduleAbilityTransaction(const AppExecFwk::InnerEvent::Pointer &event);

    void ScheduleConnectAbilityTransaction(const AppExecFwk::InnerEvent::Pointer &event);
    void ScheduleDisconnectAbilityTransaction(const AppExecFwk::InnerEvent::Pointer &event);

    std::shared_ptr<AbilityManagerService> server_;
    std::map<sptr<IAbilityScheduler>, sptr<IRemoteObject>> tokenMap_;
};

class AppManagerTestService : public std::enable_shared_from_this<AppManagerTestService> {
    DECLARE_DELAYED_SINGLETON(AppManagerTestService)
public:
    void Start();
    std::shared_ptr<AppMgrEventHandler> GetEventHandler() const
    {
        return handler_;
    }
    // MSG 0 - 20 simulate appmgr service message
    static constexpr uint32_t LOAD_ABILITY_MSG = 0;
    static constexpr uint32_t UPDATE_APP_STATE_MSG = 1;
    // MSG 20 - 100 simulate app kit message
    static constexpr uint32_t SCHEDULE_ABILITY_MSG = 20;
    static constexpr uint32_t SCHEDULE_CONNECT_MSG = 21;
    static constexpr uint32_t SCHEDULE_DISCONNECT_MSG = 22;
    static constexpr uint32_t SCHEDULE_COMMAND_MSG = 23;

private:
    std::shared_ptr<AppExecFwk::EventRunner> eventLoop_;
    std::shared_ptr<AppMgrEventHandler> handler_;
};

}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_APPMGR_TEST_SERVICE_H