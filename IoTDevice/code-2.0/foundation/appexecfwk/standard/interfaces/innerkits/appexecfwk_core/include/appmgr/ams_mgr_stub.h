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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_AMS_MGR_STUB_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_AMS_MGR_STUB_H

#include <map>

#include "iremote_stub.h"
#include "nocopyable.h"
#include "string_ex.h"
#include "ams_mgr_interface.h"

namespace OHOS {
namespace AppExecFwk {

class AmsMgrStub : public IRemoteStub<IAmsMgr> {
public:
    AmsMgrStub();
    virtual ~AmsMgrStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t HandleLoadAbility(MessageParcel &data, MessageParcel &reply);
    int32_t HandleTerminateAbility(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUpdateAbilityState(MessageParcel &data, MessageParcel &reply);
    int32_t HandleRegisterAppStateCallback(MessageParcel &data, MessageParcel &reply);
    int32_t HandleReset(MessageParcel &data, MessageParcel &reply);
    int32_t HandleAbilityBehaviorAnalysis(MessageParcel &data, MessageParcel &reply);
    int32_t HandleKillProcessByAbilityToken(MessageParcel &data, MessageParcel &reply);
    int32_t HandleKillApplication(MessageParcel &data, MessageParcel &reply);

    using AmsMgrFunc = int32_t (AmsMgrStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, AmsMgrFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(AmsMgrStub);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_APPMGR_AMS_MGR_STUB_H
