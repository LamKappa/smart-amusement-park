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

#ifndef FOUNDATION_AAFWK_SERVICES_ABILITY_TEST_MOCK_APP_MGR_CLIENT_H
#define FOUNDATION_AAFWK_SERVICES_ABILITY_TEST_MOCK_APP_MGR_CLIENT_H

#include "gmock/gmock.h"
#include "app_mgr_client.h"
#include "hilog_wrapper.h"
#include "app_mgr_constants.h"

namespace OHOS {
namespace AAFwk {

using namespace OHOS::AppExecFwk;
class MockAppMgrClient : public AppMgrClient {
public:
    MockAppMgrClient(){};
    virtual ~MockAppMgrClient(){};

    virtual AppMgrResultCode LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
        const AbilityInfo &abilityInfo, const ApplicationInfo &appInfo)
    {
        HILOG_INFO("MockAppMgrClient LoadAbility enter.");
        token_ = token;
        return AppMgrResultCode::RESULT_OK;
    }

    sptr<IRemoteObject> GetToken()
    {
        return token_;
    };

private:
    sptr<IRemoteObject> token_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // FOUNDATION_AAFWK_SERVICES_ABILITY_TEST_MOCK_APP_MGR_CLIENT_H
