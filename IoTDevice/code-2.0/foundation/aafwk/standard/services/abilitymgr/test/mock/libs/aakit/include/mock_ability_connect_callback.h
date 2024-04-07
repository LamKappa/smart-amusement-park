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

#ifndef FOUNDATION_AAAFWK_INTERFACES_INNERKITS_ABILITY_CONNECT_CALLBACK_H
#define FOUNDATION_AAAFWK_INTERFACES_INNERKITS_ABILITY_CONNECT_CALLBACK_H

#include "ability_connect_callback_interface.h"
#include "ability_connect_callback_stub.h"

namespace OHOS {
namespace AAFwk {
class AbilityConnectCallback : public AbilityConnectionStub {
public:
    AbilityConnectCallback(){};
    virtual ~AbilityConnectCallback(){};
    virtual void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;
    virtual void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;
    static int onAbilityConnectDoneCount;
    static int onAbilityDisconnectDoneCount;
};

}  // namespace AAFwk
}  // namespace OHOS

#endif  // FOUNDATION_AAAFWK_INTERFACES_INNERKITS_ABILITY_CONNECT_CALLBACK_H