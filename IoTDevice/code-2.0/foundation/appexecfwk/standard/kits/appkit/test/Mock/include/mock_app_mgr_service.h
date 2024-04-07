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

#ifndef FOUNDATION_APPEXECFWK_KITS_APPKIT_TEST_MOCK_INCLUDE_MOCK_APP_MGR_SERVICE_H
#define FOUNDATION_APPEXECFWK_KITS_APPKIT_TEST_MOCK_INCLUDE_MOCK_APP_MGR_SERVICE_H

#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "semaphore_ex.h"
#include "app_scheduler_interface.h"
#include "app_mgr_stub.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
class MockAppMgrService : public AppMgrStub {
public:
    MOCK_METHOD4(LoadAbility,
        void(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
            const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo));
    MOCK_METHOD1(TerminateAbility, void(const sptr<IRemoteObject> &token));
    MOCK_METHOD2(UpdateAbilityState, void(const sptr<IRemoteObject> &token, const AbilityState state));

    virtual void AttachApplication(const sptr<IRemoteObject> &app) override
    {
        GTEST_LOG_(INFO) << "MockAppMgrService::AttachApplication called";
        Attached_ = true;
        EXPECT_TRUE(Attached_);
        Appthread_ = iface_cast<IAppScheduler>(app);
    }

    virtual void ApplicationForegrounded(const int32_t recordId)
    {
        GTEST_LOG_(INFO) << "MockAppMgrService::ApplicationForegrounded called";
        Foregrounded_ = true;
        EXPECT_TRUE(Foregrounded_);
    }

    virtual void ApplicationBackgrounded(const int32_t recordId)
    {
        GTEST_LOG_(INFO) << "MockAppMgrService::ApplicationBackgrounded called";
        Backgrounded_ = true;
        EXPECT_TRUE(Backgrounded_);
    }

    virtual void ApplicationTerminated(const int32_t recordId)
    {
        GTEST_LOG_(INFO) << "MockAppMgrService::ApplicationTerminated called";
        Terminated_ = true;
        EXPECT_TRUE(Terminated_);
    }
    MOCK_METHOD2(CheckPermission, int32_t(const int32_t recordId, const std::string &permission));

    virtual void AbilityCleaned(const sptr<IRemoteObject> &token)
    {
        GTEST_LOG_(INFO) << "MockAppMgrService::AbilityCleaned called";
        Cleaned_ = true;
        EXPECT_TRUE(Cleaned_);
    }

    MOCK_METHOD1(KillApplication, int(const std::string &appName));

    virtual sptr<IAmsMgr> GetAmsMgr() override
    {
        return nullptr;
    };
    virtual void ClearUpApplicationData(const std::string &appName) override{};
    virtual int IsBackgroundRunningRestricted(const std::string &appName) override
    {
        return 0;
    };
    virtual int GetAllRunningProcesses(std::shared_ptr<RunningProcessInfo> &runningProcessInfo) override
    {
        return 0;
    };

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

    void ScheduleTerminateApplication()
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleTerminateApplication();
        }
    }

    void ScheduleLaunchApplication(const AppLaunchData &lanchdata)
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleLaunchApplication(lanchdata);
        }
    }

    void ScheduleForegroundApplication()
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleForegroundApplication();
        }
    }

    void ScheduleBackgroundApplication()
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleBackgroundApplication();
        }
    }

    void ScheduleShrinkMemory(const int32_t level)
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleShrinkMemory(level);
        }
    }

    void ScheduleLowMemory()
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleLowMemory();
        }
    }

    void ScheduleLaunchAbility(const AbilityInfo &abilityinf, const sptr<IRemoteObject> &token)
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleLaunchAbility(abilityinf, token);
        }
    }

    void ScheduleCleanAbility(const sptr<IRemoteObject> &token)
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleCleanAbility(token);
        }
    }

    void ScheduleProfileChanged(const Profile &profile)
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleProfileChanged(profile);
        }
    }

    void ScheduleConfigurationUpdated(const Configuration &config)
    {
        if (Appthread_ != nullptr) {
            Appthread_->ScheduleConfigurationUpdated(config);
        }
    }

    sptr<IAppScheduler> GetAppthread()
    {
        return Appthread_;
    }

    bool IsAttached()
    {
        APP_LOGI("MockAppMgrService::IsAttached Attached_ = %{public}d", Attached_);
        return Attached_;
    }

    bool IsForegrounded()
    {
        APP_LOGI("MockAppMgrService::IsForegrounded Foregrounded_ = %{public}d", Foregrounded_);
        return Foregrounded_;
    }

    bool IsBackgrounded()
    {
        APP_LOGI("MockAppMgrService::IsBackgrounded Backgrounded_ = %{public}d", Backgrounded_);
        return Backgrounded_;
    }

    bool IsTerminated()
    {
        APP_LOGI("MockAppMgrService::IsTerminated Terminated_ = %{public}d", Terminated_);
        return Terminated_;
    }

    void init()
    {
        APP_LOGI("MockAppMgrService::init called");
        Attached_ = false;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

private:
    bool Attached_ = false;
    bool Foregrounded_ = false;
    bool Backgrounded_ = false;
    bool Terminated_ = false;
    bool Cleaned_ = false;
    sptr<IAppScheduler> Appthread_ = nullptr;
    Semaphore sem_;
    std::string data_;
    sptr<IAppStateCallback> callback_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_KITS_APPKIT_TEST_MOCK_INCLUDE_MOCK_APP_MGR_SERVICE_H