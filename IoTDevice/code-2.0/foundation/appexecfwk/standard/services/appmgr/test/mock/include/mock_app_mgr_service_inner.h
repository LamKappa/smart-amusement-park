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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_APPMGR_TEST_MOCK_APP_MGR_SERVICE_INNER_H
#define FOUNDATION_APPEXECFWK_SERVICES_APPMGR_TEST_MOCK_APP_MGR_SERVICE_INNER_H

#include "gmock/gmock.h"
#include "app_log_wrapper.h"

#include "semaphore_ex.h"
#include "app_mgr_service_inner.h"

namespace OHOS {
namespace AppExecFwk {

class MockAppMgrServiceInner : public AppMgrServiceInner {
public:
    MockAppMgrServiceInner() : lock_(0)
    {}
    virtual ~MockAppMgrServiceInner()
    {}

    MOCK_METHOD4(LoadAbility,
        void(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
            const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo));
    MOCK_METHOD2(AttachApplication, void(const pid_t pid, const sptr<IAppScheduler> &app));
    MOCK_METHOD1(ApplicationForegrounded, void(const int32_t recordId));
    MOCK_METHOD1(ApplicationBackgrounded, void(const int32_t recordId));
    MOCK_METHOD1(ApplicationTerminated, void(const int32_t recordId));
    MOCK_METHOD2(UpdateAbilityState, void(const sptr<IRemoteObject> &token, const AbilityState state));
    MOCK_METHOD1(TerminateAbility, void(const sptr<IRemoteObject> &token));
    MOCK_METHOD1(KillApplication, int32_t(const std::string &bundleName));
    MOCK_METHOD1(AbilityTerminated, void(const sptr<IRemoteObject> &token));
    MOCK_METHOD3(ClearUpApplicationData, void(const std::string &, const int32_t, const pid_t));
    MOCK_METHOD1(IsBackgroundRunningRestricted, int32_t(const std::string &));
    MOCK_METHOD1(GetAllRunningProcesses, int32_t(std::shared_ptr<RunningProcessInfo> &));
    MOCK_METHOD1(RegisterAppStateCallback, void(const sptr<IAppStateCallback> &callback));
    MOCK_METHOD0(StopAllProcess, void());
    MOCK_CONST_METHOD0(QueryAppSpawnConnectionState, SpawnConnectionState());
    MOCK_CONST_METHOD2(AddAppDeathRecipient, void(const pid_t pid, const sptr<AppDeathRecipient> &appDeathRecipient));
    MOCK_METHOD1(KillProcessByAbilityToken, void(const sptr<IRemoteObject> &token));

    MOCK_METHOD5(AbilityBehaviorAnalysis,
        void(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken, const int32_t visibility,
            const int32_t perceptibility, const int32_t connectionState));
    MOCK_METHOD2(OptimizerAbilityStateChanged,
        void(const std::shared_ptr<AbilityRunningRecord> &ability, const AbilityState state));

    void Post()
    {
        if (currentCount_ > 1) {
            currentCount_--;
        } else {
            lock_.Post();
            currentCount_ = count_;
        }
    }

    // for mock function return int32_t
    int32_t Post4Int()
    {
        if (currentCount_ > 1) {
            currentCount_--;
        } else {
            lock_.Post();
            currentCount_ = count_;
        }
        return 0;
    }

    void Wait()
    {
        lock_.Wait();
    }

    void SetWaitCount(const int waitCount)
    {
        count_ = waitCount;
        currentCount_ = waitCount;
    }

    int32_t OpenAppSpawnConnection() override
    {
        return 0;
    }

private:
    Semaphore lock_;
    int32_t count_ = 1;
    int32_t currentCount_ = 1;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_APPMGR_TEST_MOCK_APP_MGR_SERVICE_INNER_H
