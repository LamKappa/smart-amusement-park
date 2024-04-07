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

#define private public
#include "app_mgr_service.h"
#include "app_mgr_service_inner.h"
#undef private

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "semaphore_ex.h"
#include "app_scheduler_host.h"
#include "app_scheduler_proxy.h"
#include "mock_app_mgr_service_inner.h"
#include "mock_app_spawn_socket.h"

using namespace testing::ext;
using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;
using testing::Return;

namespace {

constexpr int COUNT = 1;

}

namespace OHOS {
namespace AppExecFwk {

class TestAppSchedulerImpl : public AppSchedulerHost {
public:
    void ScheduleForegroundApplication() override
    {}
    void ScheduleBackgroundApplication() override
    {}
    void ScheduleTerminateApplication() override
    {}
    void ScheduleShrinkMemory(const int) override
    {}
    void ScheduleLowMemory() override
    {}
    void ScheduleLaunchApplication(const AppLaunchData &) override
    {}
    void ScheduleLaunchAbility(const AbilityInfo &, const sptr<IRemoteObject> &) override
    {}
    void ScheduleCleanAbility(const sptr<IRemoteObject> &) override
    {}
    void ScheduleProfileChanged(const Profile &) override
    {}
    void ScheduleConfigurationUpdated(const Configuration &) override
    {}
    void ScheduleProcessSecurityExit() override
    {}
};

class AppMgrServiceModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    inline static std::shared_ptr<MockAppMgrServiceInner> mockAppMgrServiceInner_;
    inline static std::shared_ptr<AppMgrService> appMgrService_;
    inline static sptr<IRemoteObject> testRemoteObject_;
};

void AppMgrServiceModuleTest::SetUpTestCase()
{
    if (!appMgrService_) {
        appMgrService_ = std::make_shared<AppMgrService>();
    }

    if (!mockAppMgrServiceInner_) {
        mockAppMgrServiceInner_ = std::make_shared<MockAppMgrServiceInner>();
    }

    if (appMgrService_ && mockAppMgrServiceInner_) {
        appMgrService_->appMgrServiceInner_ = mockAppMgrServiceInner_;
        appMgrService_->OnStart();
    }

    if (!testRemoteObject_) {
        testRemoteObject_ = static_cast<IRemoteObject *>(new TestAppSchedulerImpl);
    }
}

void AppMgrServiceModuleTest::TearDownTestCase()
{
    if (testRemoteObject_) {
        testRemoteObject_.clear();
    }

    if (mockAppMgrServiceInner_) {
        mockAppMgrServiceInner_.reset();
    }

    if (appMgrService_) {
        appMgrService_->OnStop();
        int sleepTime = 1;
        sleep(sleepTime);  // Waiting for appMgrService_'s event loop backend thread exited.
        if (appMgrService_->appMgrServiceInner_) {
            appMgrService_->appMgrServiceInner_.reset();
        }
        if (appMgrService_->amsMgrScheduler_) {
            appMgrService_->amsMgrScheduler_.clear();
        }
        appMgrService_.reset();
    }
}

void AppMgrServiceModuleTest::SetUp()
{}

void AppMgrServiceModuleTest::TearDown()
{}

/*
 * Feature: AppMgrService
 * Function: AttachApplication
 * SubFunction: NA
 * FunctionPoints: AppMgrService => AppMgrServiceInner: AttachApplication
 * CaseDescription: Check event loop AttachApplication task post from AppMgrService to AppMgrServiceInner.
 */
HWTEST_F(AppMgrServiceModuleTest, AttachApplication_001, TestSize.Level1)
{
    ASSERT_TRUE(appMgrService_);
    ASSERT_TRUE(mockAppMgrServiceInner_);
    ASSERT_TRUE(testRemoteObject_);

    for (int i = 0; i < COUNT; ++i) {
        EXPECT_CALL(*mockAppMgrServiceInner_, AddAppDeathRecipient(_, _))
            .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner_.get(), &MockAppMgrServiceInner::Post));
        EXPECT_CALL(*mockAppMgrServiceInner_, AttachApplication(_, _))
            .WillOnce(InvokeWithoutArgs(mockAppMgrServiceInner_.get(), &MockAppMgrServiceInner::Post));

        sptr<IRemoteObject> client;
        appMgrService_->AttachApplication(client);
        mockAppMgrServiceInner_->Wait();
    }
}

/*
 * Feature: AppMgrService
 * Function: ApplicationForegrounded
 * SubFunction: NA
 * FunctionPoints: AppMgrService => AppMgrServiceInner: ApplicationForegrounded
 * CaseDescription: Check event loop ApplicationForegrounded task post from AppMgrService to AppMgrServiceInner.
 */
HWTEST_F(AppMgrServiceModuleTest, ApplicationForegrounded_001, TestSize.Level1)
{
    ASSERT_TRUE(appMgrService_);
    ASSERT_TRUE(mockAppMgrServiceInner_);

    int32_t testRecordId = 123;
    bool testResult = false;
    Semaphore sem(0);

    auto mockHandler = [&](const int32_t recordId) {
        testResult = (recordId == testRecordId);
        sem.Post();
    };

    for (int i = 0; i < COUNT; ++i) {
        testResult = false;

        EXPECT_CALL(*mockAppMgrServiceInner_, ApplicationForegrounded(_)).Times(1).WillOnce(Invoke(mockHandler));

        appMgrService_->ApplicationForegrounded(testRecordId);

        sem.Wait();

        EXPECT_TRUE(testResult);
    }
}

/*
 * Feature: AppMgrService
 * Function: ApplicationBackgrounded
 * SubFunction: NA
 * FunctionPoints: AppMgrService => AppMgrServiceInner: ApplicationBackgrounded
 * CaseDescription: Check event loop ApplicationBackgrounded task post from AppMgrService to AppMgrServiceInner.
 */
HWTEST_F(AppMgrServiceModuleTest, ApplicationBackgrounded_001, TestSize.Level1)
{
    ASSERT_TRUE(appMgrService_);
    ASSERT_TRUE(mockAppMgrServiceInner_);

    int32_t testRecordId = 123;
    bool testResult = false;
    Semaphore sem(0);

    auto mockHandler = [&](const int32_t recordId) {
        testResult = (recordId == testRecordId);
        sem.Post();
    };

    for (int i = 0; i < COUNT; ++i) {
        testResult = false;

        EXPECT_CALL(*mockAppMgrServiceInner_, ApplicationBackgrounded(_)).Times(1).WillOnce(Invoke(mockHandler));

        appMgrService_->ApplicationBackgrounded(testRecordId);

        sem.Wait();

        EXPECT_TRUE(testResult);
    }
}

/*
 * Feature: AppMgrService
 * Function: ApplicationTerminated
 * SubFunction: NA
 * FunctionPoints: AppMgrService => AppMgrServiceInner: ApplicationTerminated
 * CaseDescription: Check event loop ApplicationTerminated task post from AppMgrService to AppMgrServiceInner.
 */
HWTEST_F(AppMgrServiceModuleTest, ApplicationTerminated_001, TestSize.Level1)
{
    ASSERT_TRUE(appMgrService_);
    ASSERT_TRUE(mockAppMgrServiceInner_);

    int32_t testRecordId = 123;
    bool testResult = false;
    Semaphore sem(0);

    auto mockHandler = [&testResult, testRecordId, &sem](const int32_t recordId) {
        testResult = (recordId == testRecordId);
        sem.Post();
    };

    for (int i = 0; i < COUNT; ++i) {
        testResult = false;

        EXPECT_CALL(*mockAppMgrServiceInner_, ApplicationTerminated(_)).Times(1).WillOnce(Invoke(mockHandler));

        appMgrService_->ApplicationTerminated(testRecordId);

        sem.Wait();

        EXPECT_TRUE(testResult);
    }
}

/*
 * Feature: AppMgrService
 * Function: AbilityCleaned
 * SubFunction: NA
 * FunctionPoints: AppMgrService => AppMgrServiceInner: AbilityCleaned
 * CaseDescription: Check event loop AbilityCleaned task post from AppMgrService to AppMgrServiceInner.
 */
HWTEST_F(AppMgrServiceModuleTest, AbilityCleaned_001, TestSize.Level1)
{
    ASSERT_TRUE(appMgrService_);
    ASSERT_TRUE(mockAppMgrServiceInner_);
    ASSERT_TRUE(testRemoteObject_);

    bool testResult = false;
    Semaphore sem(0);

    auto mockHandler = [&testResult, &sem](const sptr<IRemoteObject> &token) {
        testResult = (token == testRemoteObject_);
        sem.Post();
    };

    for (int i = 0; i < COUNT; ++i) {
        testResult = false;

        EXPECT_CALL(*mockAppMgrServiceInner_, AbilityTerminated(_)).Times(1).WillOnce(Invoke(mockHandler));

        appMgrService_->AbilityCleaned(testRemoteObject_);

        sem.Wait();

        EXPECT_TRUE(testResult);
    }
}

/*
 * Feature: AppMgrService
 * Function: ClearUpApplicationData
 * SubFunction: NA
 * FunctionPoints: AppMgrService => AppMgrServiceInner: ClearUpApplicationData
 * CaseDescription: Check event loop ClearUpApplicationData task post from AppMgrService to AppMgrServiceInner.
 */
HWTEST_F(AppMgrServiceModuleTest, ClearUpApplicationData_001, TestSize.Level1)
{
    ASSERT_TRUE(appMgrService_);
    ASSERT_TRUE(mockAppMgrServiceInner_);

    std::string testAppName("testApp");
    bool testResult = false;
    Semaphore sem(0);

    auto mockHandler = [&testResult, testAppName, &sem](const std::string &appName, const int32_t, const pid_t) {
        testResult = (appName == testAppName);
        sem.Post();
    };

    for (int i = 0; i < COUNT; ++i) {
        testResult = false;

        EXPECT_CALL(*mockAppMgrServiceInner_, ClearUpApplicationData(_, _, _)).Times(1).WillOnce(Invoke(mockHandler));

        appMgrService_->ClearUpApplicationData(testAppName);

        sem.Wait();

        EXPECT_TRUE(testResult);
    }
}

/*
 * Feature: AppMgrService
 * Function: IsBackgroundRunningRestricted
 * SubFunction: NA
 * FunctionPoints: AppMgrService => AppMgrServiceInner: IsBackgroundRunningRestricted
 * CaseDescription: Check IsBackgroundRunningRestricted.
 */
HWTEST_F(AppMgrServiceModuleTest, IsBackgroundRunningRestricted_001, TestSize.Level1)
{
    ASSERT_TRUE(appMgrService_);
    ASSERT_TRUE(mockAppMgrServiceInner_);

    std::string testAppName("testApp");
    bool testResult = false;
    Semaphore sem(0);

    auto mockHandler = [&testResult, testAppName, &sem](const std::string &appName) {
        testResult = (appName == testAppName);
        sem.Post();
        return ERR_OK;
    };

    for (int i = 0; i < COUNT; ++i) {
        testResult = false;

        EXPECT_CALL(*mockAppMgrServiceInner_, IsBackgroundRunningRestricted(_)).Times(1).WillOnce(Invoke(mockHandler));

        auto result = appMgrService_->IsBackgroundRunningRestricted(testAppName);

        sem.Wait();

        EXPECT_TRUE(testResult);
        EXPECT_EQ(result, ERR_OK);
    }
}

/*
 * Feature: AppMgrService
 * Function: GetAllRunningProcesses
 * SubFunction: NA
 * FunctionPoints: AppMgrService => AppMgrServiceInner: GetAllRunningProcesses
 * CaseDescription: Check GetAllRunningProcesses.
 */
HWTEST_F(AppMgrServiceModuleTest, GetAllRunningProcesses_001, TestSize.Level1)
{
    ASSERT_TRUE(appMgrService_);
    ASSERT_TRUE(mockAppMgrServiceInner_);

    auto testRunningProcessInfo = std::make_shared<RunningProcessInfo>();
    ASSERT_TRUE(testRunningProcessInfo);

    int32_t testPid = 456;
    std::string testProcessName = "testProcess";

    Semaphore sem(0);
    auto mockHandler = [testProcessName, testPid, &sem](std::shared_ptr<RunningProcessInfo> &runningProcessInfo) {
        auto &it = runningProcessInfo->appProcessInfos.emplace_back();
        it.processName_ = testProcessName;
        it.pid_ = testPid;
        sem.Post();
        return ERR_OK;
    };

    for (int i = 0; i < COUNT; ++i) {
        testRunningProcessInfo->appProcessInfos.clear();
        EXPECT_CALL(*mockAppMgrServiceInner_, GetAllRunningProcesses(_)).Times(1).WillOnce(Invoke(mockHandler));

        auto result = appMgrService_->GetAllRunningProcesses(testRunningProcessInfo);
        sem.Wait();

        EXPECT_EQ(testRunningProcessInfo->appProcessInfos.size(), size_t(1));
        EXPECT_EQ(testRunningProcessInfo->appProcessInfos[0].processName_, testProcessName);
        EXPECT_EQ(testRunningProcessInfo->appProcessInfos[0].pid_, testPid);
        EXPECT_EQ(result, ERR_OK);
    }
}

/*
 * Feature: AppMgrService
 * Function: KillApplication
 * SubFunction: NA
 * FunctionPoints: AppMgrService => AppMgrServiceInner: KillApplication
 * CaseDescription: Check KillApplication task post from AppMgrService to AppMgrServiceInner.
 */
HWTEST_F(AppMgrServiceModuleTest, KillApplication_001, TestSize.Level1)
{
    ASSERT_TRUE(appMgrService_);
    ASSERT_TRUE(mockAppMgrServiceInner_);

    std::string testBundleName("testApp");
    bool testResult = false;
    Semaphore sem(0);

    auto mockHandler = [&testResult, testBundleName, &sem](const std::string &bundleName) {
        testResult = (bundleName == testBundleName);
        sem.Post();
        return 0;
    };

    for (int i = 0; i < COUNT; ++i) {
        testResult = false;

        EXPECT_CALL(*mockAppMgrServiceInner_, KillApplication(_)).Times(1).WillOnce(Invoke(mockHandler));

        int ret = appMgrService_->GetAmsMgr()->KillApplication(testBundleName);

        sem.Wait();

        EXPECT_TRUE(testResult);
        EXPECT_EQ(ret, 0);
    }
}

/*
 * Feature: AppMgrService
 * Function: QueryServiceState
 * SubFunction: AppMgrService => AppMgrServiceInner: QueryAppSpawnConnectionState
 * FunctionPoints: NA
 * CaseDescription: Check QueryServiceState.
 */
HWTEST_F(AppMgrServiceModuleTest, QueryServiceState_001, TestSize.Level1)
{
    ASSERT_TRUE(appMgrService_);
    ASSERT_TRUE(mockAppMgrServiceInner_);

    SpawnConnectionState testSpawnConnectionState = SpawnConnectionState::STATE_CONNECTED;
    Semaphore sem(0);

    auto mockHandler = [&]() {
        sem.Post();
        return testSpawnConnectionState;
    };

    for (int i = 0; i < COUNT; ++i) {
        EXPECT_CALL(*mockAppMgrServiceInner_, QueryAppSpawnConnectionState()).Times(1).WillOnce(Invoke(mockHandler));

        auto serviceState = appMgrService_->QueryServiceState();

        sem.Wait();

        EXPECT_EQ(serviceState.connectionState, testSpawnConnectionState);
    }
}

/*
 * Feature: AppMgrService
 * Function: GetAmsMgr
 * SubFunction: NA
 * FunctionPoints: NA
 * CaseDescription: Check GetAmsMgr.
 */
HWTEST_F(AppMgrServiceModuleTest, GetAmsMgr_001, TestSize.Level1)
{
    ASSERT_TRUE(appMgrService_);

    auto amsMgr = appMgrService_->GetAmsMgr();

    EXPECT_TRUE(amsMgr);
}

}  // namespace AppExecFwk
}  // namespace OHOS