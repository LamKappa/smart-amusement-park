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

#ifndef OHOS_AAFWK_ABILITY_CONNECT_CALLBACK_PROXY_H
#define OHOS_AAFWK_ABILITY_CONNECT_CALLBACK_PROXY_H

#include "ability_connect_callback_interface.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace AAFwk {
/**
 * @class AbilityConnectProxy
 * AbilityConnect proxy.
 */
class AbilityConnectionProxy : public IRemoteProxy<IAbilityConnection> {
public:
    explicit AbilityConnectionProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IAbilityConnection>(impl)
    {}

    virtual ~AbilityConnectionProxy()
    {}

    /**
     * OnAbilityConnectDone, AbilityMs notify caller ability the result of connect.
     *
     * @param element,.service ability's ElementName.
     * @param remoteObject,.the session proxy of service ability.
     * @param resultCode, ERR_OK on success, others on failure.
     */
    virtual void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;

    /**
     * OnAbilityDisconnectDone, AbilityMs notify caller ability the result of disconnect.
     *
     * @param element,.service ability's ElementName.
     * @param resultCode, ERR_OK on success, others on failure.
     */
    virtual void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;

private:
    bool WriteInterfaceToken(MessageParcel &data);

private:
    static inline BrokerDelegator<AbilityConnectionProxy> delegator_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_ABILITY_CONNECT_CALLBACK_PROXY_H