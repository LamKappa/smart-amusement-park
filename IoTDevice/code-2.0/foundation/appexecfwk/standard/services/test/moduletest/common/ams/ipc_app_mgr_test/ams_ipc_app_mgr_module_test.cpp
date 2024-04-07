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

#include <gtest/gtest.h>

#include <unistd.h>

#include "app_mgr_proxy.h"
#include "app_scheduler_proxy.h"
#include "app_record_id.h"

#include "mock_app_mgr_service.h"
#include "mock_application.h"
#include "mock_ability_token.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using OHOS::iface_cast;
using OHOS::sptr;
using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;

namespace {

const int32_t COUNT = 10000;

}  // namespace

class AmsIpcAppmgrModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AmsIpcAppmgrModuleTest::SetUpTestCase()
{}

void AmsIpcAppmgrModuleTest::TearDownTestCase()
{}

void AmsIpcAppmgrModuleTest::SetUp()
{}

void AmsIpcAppmgrModuleTest::TearDown()
{}

class MockMockAppMgrService : public MockAppMgrService {
public:
    MOCK_METHOD0(GetAmsMgr, sptr<IAmsMgr>());
    MOCK_METHOD1(ClearUpApplicationData, void(const std::string &));
    MOCK_METHOD1(IsBackgroundRunningRestricted, int(const std::string &bundleName));
    MOCK_METHOD1(GetAllRunningProcesses, int(std::shared_ptr<RunningProcessInfo> &));
};

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AppmgrIPCInterface
 * FunctionPoints: test attachApplication API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute attachApplication API 10000 times
 */
HWTEST_F(AmsIpcAppmgrModuleTest, ExcuteAppmgrIPCInterface_001, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
        sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);
        sptr<MockApplication> app(new MockApplication());

        EXPECT_CALL(*mockAppMgr, AttachApplication(_))
            .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
        appMgrClient->AttachApplication(app);
        mockAppMgr->Wait();
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AppmgrIPCInterface
 * FunctionPoints: test applicationForegrounded API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute applicationForegrounded API 10000 times
 */
HWTEST_F(AmsIpcAppmgrModuleTest, ExcuteAppmgrIPCInterface_002, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
        sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);
        auto recordId = AppRecordId::Create();

        EXPECT_CALL(*mockAppMgr, ApplicationForegrounded(_))
            .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
        appMgrClient->ApplicationForegrounded(recordId);
        mockAppMgr->Wait();
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AppmgrIPCInterface
 * FunctionPoints: test applicationBackgrounded API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute applicationBackgrounded API 10000 times
 */
HWTEST_F(AmsIpcAppmgrModuleTest, ExcuteAppmgrIPCInterface_003, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
        sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);
        auto recordId = AppRecordId::Create();

        EXPECT_CALL(*mockAppMgr, ApplicationBackgrounded(_))
            .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
        appMgrClient->ApplicationBackgrounded(recordId);
        mockAppMgr->Wait();
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AppmgrIPCInterface
 * FunctionPoints: test ApplicationTerminated API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute ApplicationTerminated API 10000 times
 */
HWTEST_F(AmsIpcAppmgrModuleTest, ExcuteAppmgrIPCInterface_004, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
        sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);
        auto recordId = AppRecordId::Create();

        EXPECT_CALL(*mockAppMgr, ApplicationTerminated(_))
            .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
        appMgrClient->ApplicationTerminated(recordId);
        mockAppMgr->Wait();
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AppmgrIPCInterface
 * FunctionPoints: test AbilityCleaned API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute AbilityClenaed API 10000 times
 */
HWTEST_F(AmsIpcAppmgrModuleTest, ExcuteAppmgrIPCInterface_005, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
        sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);
        sptr<MockAbilityToken> token = new MockAbilityToken;

        EXPECT_CALL(*mockAppMgr, AbilityCleaned(_))
            .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
        appMgrClient->AbilityCleaned(token);
        mockAppMgr->Wait();
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AppmgrIPCInterface
 * FunctionPoints: test GetAmsMgr API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute KillApplication API 10000 times
 */
HWTEST_F(AmsIpcAppmgrModuleTest, ExcuteAppmgrIPCInterface_006, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockMockAppMgrService> mockMockAppMgr(new MockMockAppMgrService());
        sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockMockAppMgr);

        auto mockHandler = [&]() -> sptr<IAmsMgr> {
            mockMockAppMgr->Post();
            return sptr<IAmsMgr>(nullptr);
        };

        EXPECT_CALL(*mockMockAppMgr, GetAmsMgr()).Times(1).WillOnce(Invoke(mockHandler));

        appMgrClient->GetAmsMgr();

        mockMockAppMgr->Wait();
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AppmgrIPCInterface
 * FunctionPoints: test ClearUpApplicationData API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute ClearUpApplicationData API 10000 times
 */
HWTEST_F(AmsIpcAppmgrModuleTest, ExcuteAppmgrIPCInterface_007, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockMockAppMgrService> mockMockAppMgr(new MockMockAppMgrService());
        sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockMockAppMgr);
        std::string testBundleName("testApp");
        bool testResult = false;

        auto mockHandler = [&](const std::string &name) {
            testResult = (name == testBundleName);
            mockMockAppMgr->Post();
        };

        EXPECT_CALL(*mockMockAppMgr, ClearUpApplicationData(_)).WillOnce(Invoke(mockHandler));

        appMgrClient->ClearUpApplicationData(testBundleName);
        mockMockAppMgr->Wait();

        EXPECT_TRUE(testResult);
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AppmgrIPCInterface
 * FunctionPoints: test IsBackgroundRunningRestricted API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute IsBackgroundRunningRestricted API 10000 times
 */
HWTEST_F(AmsIpcAppmgrModuleTest, ExcuteAppmgrIPCInterface_008, TestSize.Level3)
{
    for (int i = 0; i < COUNT; i++) {
        sptr<MockMockAppMgrService> mockMockAppMgr(new MockMockAppMgrService());
        sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockMockAppMgr);
        std::string testBundleName("testApp");
        bool testResult = false;

        auto mockHandler = [&](const std::string &name) {
            testResult = (name == testBundleName);
            mockMockAppMgr->Post();
            return 0;
        };

        EXPECT_CALL(*mockMockAppMgr, IsBackgroundRunningRestricted(_)).WillOnce(Invoke(mockHandler));

        appMgrClient->IsBackgroundRunningRestricted(testBundleName);
        mockMockAppMgr->Wait();

        EXPECT_TRUE(testResult);
    }
}

/*
 * Feature: ApplicationFramework
 * Function: AppManagerService
 * SubFunction: AppmgrIPCInterface
 * FunctionPoints: test GetAllRunningProcesses API,then check the function whether is good or not
 * EnvConditions: system running normally
 * CaseDescription: excute GetAllRunningProcesses API 10000 times
 */
HWTEST_F(AmsIpcAppmgrModuleTest, ExcuteAppmgrIPCInterface_09, TestSize.Level3)
{
    const std::string testBundleName_1("testApp1");
    pid_t testApp1Pid = 1001;
    const std::string testBundleName_2("testApp2");
    pid_t testApp2Pid = 1002;

    for (int i = 0; i < COUNT; i++) {
        sptr<MockMockAppMgrService> mockMockAppMgr(new MockMockAppMgrService());
        sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockMockAppMgr);
        auto allRunningProcessInfo = std::make_shared<RunningProcessInfo>();

        auto mockHandler = [&](std::shared_ptr<RunningProcessInfo> &result) {
            result->appProcessInfos.clear();

            auto &r1 = result->appProcessInfos.emplace_back();
            r1.processName_ = testBundleName_1;
            r1.pid_ = testApp1Pid;

            auto &r2 = result->appProcessInfos.emplace_back();
            r2.processName_ = testBundleName_2;
            r2.pid_ = testApp2Pid;

            mockMockAppMgr->Post();

            return 0;
        };

        EXPECT_CALL(*mockMockAppMgr, GetAllRunningProcesses(_)).WillOnce(Invoke(mockHandler));

        appMgrClient->GetAllRunningProcesses(allRunningProcessInfo);
        mockMockAppMgr->Wait();

        EXPECT_TRUE(allRunningProcessInfo->appProcessInfos.size() == 2);
        EXPECT_EQ(allRunningProcessInfo->appProcessInfos[0].processName_, testBundleName_1);
        EXPECT_EQ(allRunningProcessInfo->appProcessInfos[0].pid_, testApp1Pid);
        EXPECT_EQ(allRunningProcessInfo->appProcessInfos[1].processName_, testBundleName_2);
        EXPECT_EQ(allRunningProcessInfo->appProcessInfos[1].pid_, testApp2Pid);
    }
}
