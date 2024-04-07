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

#ifndef MOCK_FOUNDATION_AAAFWK_INTERFACES_INNERKITS_ABILITY_CONNECT_CALLBACK_STUB_H
#define MOCK_FOUNDATION_AAAFWK_INTERFACES_INNERKITS_ABILITY_CONNECT_CALLBACK_STUB_H

#include "ability_connect_callback_stub.h"
#include "gmock/gmock.h"
#include "semaphore_ex.h"

namespace OHOS {
namespace AAFwk {
class MockAbilityConnectCallbackStub : public AbilityConnectionStub {
public:
    MOCK_METHOD3(OnAbilityConnectDone,
        void(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode));
    MOCK_METHOD2(OnAbilityDisconnectDone, void(const AppExecFwk::ElementName &element, int resultCode));

    void Wait()
    {
        sem_.Wait();
    }

    int Post()
    {
        sem_.Post();
        return 0;
    }

    void PostVoid()
    {
        sem_.Post();
    }

private:
    Semaphore sem_;
};

}  // namespace AAFwk
}  // namespace OHOS

#endif  // MOCK_FOUNDATION_AAAFWK_INTERFACES_INNERKITS_ABILITY_CONNECT_CALLBACK_STUB_H