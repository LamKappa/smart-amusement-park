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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_COMMON_TEST_MOCK_MOCK_APP_MGR_SERVICE_H
#define FOUNDATION_APPEXECFWK_SERVICES_COMMON_TEST_MOCK_MOCK_APP_MGR_SERVICE_H

#include "gmock/gmock.h"
#include "semaphore_ex.h"
#include "app_mgr_stub.h"

namespace OHOS {
namespace AppExecFwk {
class MockAppMgrService : public AppMgrStub {
public:
    MOCK_METHOD4(LoadAbility,
        void(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
            const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo));
    MOCK_METHOD1(TerminateAbility, void(const sptr<IRemoteObject> &token));
    MOCK_METHOD2(UpdateAbilityState, void(const sptr<IRemoteObject> &token, const AbilityState state));
    MOCK_METHOD1(AttachApplication, void(const sptr<IRemoteObject> &app));
    MOCK_METHOD1(ApplicationForegrounded, void(const int32_t recordId));
    MOCK_METHOD1(ApplicationBackgrounded, void(const int32_t recordId));
    MOCK_METHOD1(ApplicationTerminated, void(const int32_t recordId));
    MOCK_METHOD2(CheckPermission, int32_t(const int32_t recordId, const std::string &permission));
    MOCK_METHOD1(AbilityCleaned, void(const sptr<IRemoteObject> &token));
    MOCK_METHOD1(KillApplication, int32_t(const std::string &appName));
    MOCK_METHOD1(ClearUpApplicationData, void(const std::string &bundleName));
    MOCK_METHOD1(IsBackgroundRunningRestricted, int(const std::string &bundleName));
    MOCK_METHOD1(GetAllRunningProcesses, int(std::shared_ptr<RunningProcessInfo> &runningProcessInfo));
    MOCK_METHOD0(GetAmsMgr, sptr<IAmsMgr>());

    virtual void RegisterAppStateCallback(const sptr<IAppStateCallback> &callback)
    {
        callback_ = callback;
    }

    int32_t CheckPermissionImpl([[maybe_unused]] const int32_t recordId, const std::string &data)
    {
        data_ = data;
        return 0;
    }

    void KillApplicationImpl(const std::string &data)
    {
        data_ = data;
    }

    const std::string &GetData() const
    {
        return data_;
    }

    void Wait()
    {
        sem_.Wait();
    }

    void Post()
    {
        sem_.Post();
    }

    void UpdateState() const
    {
        if (!callback_) {
            return;
        }
        AppProcessData processData;
        processData.appName = "";
        processData.pid = 1;
        processData.appState = ApplicationState::APP_STATE_BEGIN;
        callback_->OnAppStateChanged(processData);
    }

    void Terminate(const sptr<IRemoteObject> &token) const
    {
        if (!callback_) {
            return;
        }
        AbilityState st = AbilityState::ABILITY_STATE_BEGIN;
        callback_->OnAbilityRequestDone(token, st);
    }

private:
    Semaphore sem_;
    std::string data_;
    sptr<IAppStateCallback> callback_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_COMMON_TEST_MOCK_MOCK_APP_MGR_SERVICE_H