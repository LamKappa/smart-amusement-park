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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_DEATH_RECIPIENT_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_DEATH_RECIPIENT_H

#include "iremote_object.h"

#include "app_mgr_service_event_handler.h"

namespace OHOS {
namespace AppExecFwk {

class AppMgrServiceInner;

class AppDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject> &remote) override;

    /**
     * @brief Setting event handler instance.
     *
     * @param handler, event handler instance.
     */
    void SetEventHandler(const std::shared_ptr<AMSEventHandler> &handler);

    /**
     * @brief Setting application service internal handler instance.
     *
     * @param serviceInner, application service internal handler instance.
     */
    void SetAppMgrServiceInner(const std::shared_ptr<AppMgrServiceInner> &serviceInner);

private:
    std::weak_ptr<AMSEventHandler> handler_;
    std::weak_ptr<AppMgrServiceInner> appMgrServiceInner_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_INCLUDE_APP_DEATH_RECIPIENT_H
