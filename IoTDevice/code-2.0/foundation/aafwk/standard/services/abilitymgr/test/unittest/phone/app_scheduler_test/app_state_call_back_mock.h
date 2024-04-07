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

#ifndef ABILITY_UNITTEST_APP_SCHEDULE_CALL_BACK_MOCK_H
#define ABILITY_UNITTEST_APP_SCHEDULE_CALL_BACK_MOCK_H

#include <gmock/gmock.h>
#include <iremote_object.h>
#include <iremote_stub.h>
#include "app_scheduler.h"

namespace OHOS {
namespace AAFwk {

class AppStateCallbackMock : public AppStateCallback {
public:
    virtual ~AppStateCallbackMock()
    {}

    MOCK_METHOD2(OnAbilityRequestDone, void(const sptr<IRemoteObject> &, const int32_t));
};

}  // namespace AAFwk
}  // namespace OHOS

#endif