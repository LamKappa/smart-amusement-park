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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_MGR_STUB_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_MGR_STUB_H

#include <map>

#include "iremote_stub.h"
#include "nocopyable.h"
#include "string_ex.h"
#include "app_mgr_interface.h"

namespace OHOS {
namespace AppExecFwk {

class AppMgrStub : public IRemoteStub<IAppMgr> {
public:
    AppMgrStub();
    virtual ~AppMgrStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t HandleAttachApplication(MessageParcel &data, MessageParcel &reply);
    int32_t HandleApplicationForegrounded(MessageParcel &data, MessageParcel &reply);
    int32_t HandleApplicationBackgrounded(MessageParcel &data, MessageParcel &reply);
    int32_t HandleApplicationTerminated(MessageParcel &data, MessageParcel &reply);
    int32_t HandleCheckPermission(MessageParcel &data, MessageParcel &reply);
    int32_t HandleAbilityCleaned(MessageParcel &data, MessageParcel &reply);
    int32_t HandleGetAmsMgr(MessageParcel &data, MessageParcel &reply);
    int32_t HandleClearUpApplicationData(MessageParcel &data, MessageParcel &reply);
    int32_t HandleIsBackgroundRunningRestricted(MessageParcel &data, MessageParcel &reply);
    int32_t HandleGetAllRunningProcesses(MessageParcel &data, MessageParcel &reply);

    using AppMgrFunc = int32_t (AppMgrStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, AppMgrFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(AppMgrStub);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_APP_MGR_STUB_H
