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

#include "mock_ability_connect_callback.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {

int AbilityConnectCallback::onAbilityConnectDoneCount = 0;
int AbilityConnectCallback::onAbilityDisconnectDoneCount = 0;

void AbilityConnectCallback::OnAbilityConnectDone(const AppExecFwk::ElementName &__attribute__((unused)) element,
    const sptr<IRemoteObject> &__attribute__((unused)) remoteObject, int __attribute__((unused)) resultCode)
{
    HILOG_DEBUG("mock AbilityConnectCallback::OnAbilityConnectDone");
    onAbilityConnectDoneCount++;
}

void AbilityConnectCallback::OnAbilityDisconnectDone(
    const AppExecFwk::ElementName &__attribute__((unused)) element, int __attribute__((unused)) resultCode)
{
    HILOG_DEBUG("mock AbilityConnectCallback::OnAbilityDisConnectDone");
    onAbilityDisconnectDoneCount++;
}

}  // namespace AAFwk
}  // namespace OHOS