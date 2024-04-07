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
#include "remote_client_manager.h"
#include "app_mgr_service_inner.h"
#undef private

#include <vector>
#include <gtest/gtest.h>
#include "app_launch_data.h"
#include "iremote_object.h"
#include "app_state_callback_proxy.h"
#include "app_log_wrapper.h"
#include "refbase.h"
#include "mock_bundle_manager.h"
#include "mock_ability_token.h"
#include "mock_app_scheduler.h"
#include "mock_app_spawn_client.h"
#include "mock_app_spawn_socket.h"
#include "mock_iapp_state_callback.h"

using namespace testing::ext;
using testing::_;
using testing::Return;
using testing::SetArgReferee;

namespace {

const int32_t ABILITY_NUM = 100;
const int32_t APPLICATION_NUM = 100;
const int32_t INDEX_NUM_100 = 100;
const int32_t INDEX_NUM_MAX = 100;
const std::string TEST_APP_NAME = "test_app_";
const std::string TEST_ABILITY_NAME = "test_ability_";

}  // namespace

namespace OHOS {
namespace AppExecFwk {

struct TestProcessInfo {
    pid_t pid = 0;
    bool isStart = false;
};

// specify process condition
class AmsAppLifeCycleModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::shared_ptr<ApplicationInfo> GetApplicationInfo(const std::string &appName) const;
    std::shared_ptr<AbilityInfo> GetAbilityInfo(const std::string &abilityIndex, const std::string &name,
        const std::string &process, const std::string &applicationName) const;
    void StartAppProcess(const pid_t &pid) const;
    std::shared_ptr<AppRunningRecord> StartProcessAndLoadAbility(const sptr<MockAppScheduler> &mockAppScheduler,
        const sptr<IRemoteObject> &token, const std::shared_ptr<AbilityInfo> &abilityInfo,
        const std::shared_ptr<ApplicationInfo> &appInfo, const TestProcessInfo &testProcessInfo) const;
    void ChangeAbilityStateAfterAppStart(const sptr<MockAppScheduler> &mockAppScheduler, const pid_t &pid) const;
    void ChangeAbilityStateToForegroud(const sptr<MockAppScheduler> &mockAppScheduler,
        const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
        const bool isChange = false) const;
    void ChangeAbilityStateToBackGroud(const sptr<MockAppScheduler> &mockAppScheduler,
        const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
        const bool isChange = false) const;
    void ChangeAbilityStateToTerminate(
        const sptr<MockAppScheduler> &mockAppScheduler, const sptr<IRemoteObject> &token) const;
    void ChangeAppToTerminate(const sptr<MockAppScheduler> &mockAppScheduler,
        const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
        const bool isStop = false) const;
    void CheckState(const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
        const AbilityState abilityState, const ApplicationState appState) const;
    void CheckStateAfterClearAbility(const std::shared_ptr<AppRunningRecord> &appRunningRecord,
        const std::vector<sptr<IRemoteObject>> &tokens, const int32_t &recordId,
        const sptr<MockAppScheduler> &mockAppScheduler) const;
    void CheckStateAfterChangeAbility(const std::shared_ptr<AppRunningRecord> &appRunningRecord,
        const std::vector<sptr<IRemoteObject>> &tokens, const sptr<MockAppScheduler> &mockAppScheduler);
    void CreateAppRecentList(const int32_t appNum);

    sptr<MockAbilityToken> GetAbilityToken();

protected:
    std::unique_ptr<AppMgrServiceInner> serviceInner_ = nullptr;
    sptr<MockAbilityToken> mockToken_ = nullptr;
    sptr<MockAppStateCallback> mockAppStateCallbackStub_ = nullptr;
    std::shared_ptr<AppMgrServiceInner> inner_ = nullptr;
    sptr<BundleMgrService> mockBundleMgr;
};

void AmsAppLifeCycleModuleTest::SetUpTestCase()
{}

void AmsAppLifeCycleModuleTest::TearDownTestCase()
{}

void AmsAppLifeCycleModuleTest::SetUp()
{
    serviceInner_.reset(new (std::nothrow) AppMgrServiceInner());
    mockAppStateCallbackStub_ = new (std::nothrow) MockAppStateCallback();

    inner_ = std::make_shared<AppMgrServiceInner>();

    if (serviceInner_ && mockAppStateCallbackStub_) {
        auto mockAppStateCallbackProxy = iface_cast<IAppStateCallback>(mockAppStateCallbackStub_);
        if (mockAppStateCallbackProxy) {
            serviceInner_->RegisterAppStateCallback(mockAppStateCallbackProxy);
            inner_->RegisterAppStateCallback(mockAppStateCallbackProxy);
        }
    }

    mockBundleMgr = new (std::nothrow) BundleMgrService();
    serviceInner_->SetBundleManager(mockBundleMgr);
}

void AmsAppLifeCycleModuleTest::TearDown()
{
    serviceInner_.reset();
    mockAppStateCallbackStub_.clear();
}

std::shared_ptr<ApplicationInfo> AmsAppLifeCycleModuleTest::GetApplicationInfo(const std::string &appName) const
{
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = appName;
    return appInfo;
}

std::shared_ptr<AbilityInfo> AmsAppLifeCycleModuleTest::GetAbilityInfo(const std::string &abilityIndex,
    const std::string &name, const std::string &process, const std::string &applicationName) const
{
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = name + abilityIndex;
    if (!process.empty()) {
        abilityInfo->process = process;
    }
    abilityInfo->applicationName = applicationName;
    return abilityInfo;
}

std::shared_ptr<AppRunningRecord> AmsAppLifeCycleModuleTest::StartProcessAndLoadAbility(
    const sptr<MockAppScheduler> &mockAppScheduler, const sptr<IRemoteObject> &token,
    const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo,
    const TestProcessInfo &testProcessInfo) const
{
    if (!testProcessInfo.isStart) {
        StartAppProcess(testProcessInfo.pid);
    } else {
        EXPECT_CALL(*mockAppScheduler, ScheduleLaunchAbility(_, _)).Times(1);
    }

    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);

    std::shared_ptr<AppRunningRecord> record =
        serviceInner_->GetAppRunningRecordByProcessName(appInfo->name, abilityInfo->process);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, testProcessInfo.pid);
    }
    return record;
}

void AmsAppLifeCycleModuleTest::StartAppProcess(const pid_t &pid) const
{
    MockAppSpawnClient *mockClientPtr = new (std::nothrow) MockAppSpawnClient();
    EXPECT_TRUE(mockClientPtr);

    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(1).WillOnce(DoAll(SetArgReferee<1>(pid), Return(ERR_OK)));
    EXPECT_CALL(*mockAppStateCallbackStub_, OnAppStateChanged(_)).Times(1);

    serviceInner_->SetAppSpawnClient(std::unique_ptr<MockAppSpawnClient>(mockClientPtr));
}

void AmsAppLifeCycleModuleTest::ChangeAbilityStateAfterAppStart(
    const sptr<MockAppScheduler> &mockAppScheduler, const pid_t &pid) const
{
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchApplication(_)).Times(1);
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchAbility(_, _)).Times(1);

    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    serviceInner_->AttachApplication(pid, client);
}

void AmsAppLifeCycleModuleTest::ChangeAbilityStateToForegroud(const sptr<MockAppScheduler> &mockAppScheduler,
    const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
    const bool isChange) const
{
    if (!isChange) {
        EXPECT_CALL(*mockAppScheduler, ScheduleForegroundApplication()).Times(1);
        EXPECT_CALL(*mockAppStateCallbackStub_, OnAppStateChanged(_)).Times(1);
    }

    serviceInner_->UpdateAbilityState(token, AbilityState::ABILITY_STATE_FOREGROUND);

    if (!isChange) {
        ASSERT_NE(appRunningRecord, nullptr);
        int32_t recordId = appRunningRecord->GetRecordId();
        serviceInner_->ApplicationForegrounded(recordId);
    }
}

void AmsAppLifeCycleModuleTest::ChangeAbilityStateToBackGroud(const sptr<MockAppScheduler> &mockAppScheduler,
    const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
    const bool isChange) const
{
    if (!isChange) {
        EXPECT_CALL(*mockAppScheduler, ScheduleBackgroundApplication()).Times(1);
        EXPECT_CALL(*mockAppStateCallbackStub_, OnAppStateChanged(_)).Times(1);
    }

    serviceInner_->UpdateAbilityState(token, AbilityState::ABILITY_STATE_BACKGROUND);

    if (!isChange) {
        ASSERT_NE(appRunningRecord, nullptr);
        int32_t recordId = appRunningRecord->GetRecordId();
        serviceInner_->ApplicationBackgrounded(recordId);
    }
}

void AmsAppLifeCycleModuleTest::ChangeAppToTerminate(const sptr<MockAppScheduler> &mockAppScheduler,
    const std::shared_ptr<AppRunningRecord> &appRunningRecord, const sptr<IRemoteObject> &token,
    const bool isStop) const
{
    ChangeAbilityStateToTerminate(mockAppScheduler, token);

    if (isStop) {
        EXPECT_CALL(*mockAppScheduler, ScheduleTerminateApplication()).Times(1);
        EXPECT_CALL(*mockAppStateCallbackStub_, OnAppStateChanged(_)).Times(1);
        serviceInner_->AbilityTerminated(token);
        ASSERT_NE(appRunningRecord, nullptr);
        int32_t recordId = appRunningRecord->GetRecordId();
        serviceInner_->ApplicationTerminated(recordId);
    } else {
        serviceInner_->AbilityTerminated(token);
    }
}

void AmsAppLifeCycleModuleTest::ChangeAbilityStateToTerminate(
    const sptr<MockAppScheduler> &mockAppScheduler, const sptr<IRemoteObject> &token) const
{
    EXPECT_CALL(*mockAppScheduler, ScheduleCleanAbility(_)).Times(1);
    serviceInner_->TerminateAbility(token);
}

void AmsAppLifeCycleModuleTest::CheckState(const std::shared_ptr<AppRunningRecord> &appRunningRecord,
    const sptr<IRemoteObject> &token, const AbilityState abilityState, const ApplicationState appState) const
{
    ASSERT_NE(appRunningRecord, nullptr);
    auto abilityRunningRecord = appRunningRecord->GetAbilityRunningRecordByToken(token);
    ApplicationState getAppState = appRunningRecord->GetState();
    EXPECT_EQ(appState, getAppState);
    ASSERT_NE(abilityRunningRecord, nullptr);
    AbilityState getAbilityState = abilityRunningRecord->GetState();
    EXPECT_EQ(abilityState, getAbilityState);
}

void AmsAppLifeCycleModuleTest::CheckStateAfterClearAbility(const std::shared_ptr<AppRunningRecord> &appRunningRecord,
    const std::vector<sptr<IRemoteObject>> &tokens, const int32_t &recordId,
    const sptr<MockAppScheduler> &mockAppScheduler) const
{
    unsigned long size = tokens.size();
    for (unsigned long i = 0; i < size; i++) {
        if (i != size - 1) {
            ChangeAppToTerminate(mockAppScheduler, appRunningRecord, tokens[i], false);
            ApplicationState getAppState = appRunningRecord->GetState();
            EXPECT_EQ(ApplicationState::APP_STATE_BACKGROUND, getAppState);
        } else {
            ChangeAppToTerminate(mockAppScheduler, appRunningRecord, tokens[i], true);
            auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId);
            EXPECT_EQ(nullptr, record);
        }
    }
}

void AmsAppLifeCycleModuleTest::CheckStateAfterChangeAbility(const std::shared_ptr<AppRunningRecord> &appRunningRecord,
    const std::vector<sptr<IRemoteObject>> &tokens, const sptr<MockAppScheduler> &mockAppScheduler)
{
    unsigned long size = tokens.size();
    for (unsigned long i = 0; i < size; i++) {
        if (i != size - 1) {
            ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, tokens[i], true);
            CheckState(appRunningRecord,
                tokens[i],
                AbilityState::ABILITY_STATE_BACKGROUND,
                ApplicationState::APP_STATE_FOREGROUND);
        } else {
            ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, tokens[i], false);
            CheckState(appRunningRecord,
                tokens[i],
                AbilityState::ABILITY_STATE_BACKGROUND,
                ApplicationState::APP_STATE_BACKGROUND);
        }
    }
}

void AmsAppLifeCycleModuleTest::CreateAppRecentList(const int32_t appNum)
{
    for (int32_t i = INDEX_NUM_MAX - appNum + 1; i <= INDEX_NUM_MAX; i++) {
        std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
        std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
        appInfo->name = TEST_APP_NAME + std::to_string(i);
        appInfo->bundleName = appInfo->name;
        abilityInfo->name = TEST_ABILITY_NAME + std::to_string(i);
        abilityInfo->applicationName = TEST_APP_NAME + std::to_string(i);
        pid_t pid = i;
        sptr<IRemoteObject> token = new (std::nothrow) MockAbilityToken();
        MockAppSpawnClient *mockedSpawnClient = new (std::nothrow) MockAppSpawnClient();
        EXPECT_TRUE(mockedSpawnClient);
        EXPECT_CALL(*mockedSpawnClient, StartProcess(_, _))
            .Times(1)
            .WillOnce(DoAll(SetArgReferee<1>(pid), Return(ERR_OK)));
        EXPECT_CALL(*mockAppStateCallbackStub_, OnAppStateChanged(_)).Times(1);

        serviceInner_->SetAppSpawnClient(std::unique_ptr<MockAppSpawnClient>(mockedSpawnClient));
        serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    }
    return;
}

sptr<MockAbilityToken> AmsAppLifeCycleModuleTest::GetAbilityToken()
{
    if (mockToken_ != nullptr) {
        return mockToken_;
    }
    mockToken_ = new (std::nothrow) MockAbilityToken();
    return mockToken_;
}

void InvokeOnAbilityRequestDone(const sptr<IRemoteObject> &obj, const AbilityState state)
{
    EXPECT_EQ(state, AbilityState::ABILITY_STATE_BACKGROUND);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of ability
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                      foreground
 *                  3.switch ability to background and call ScheduleBackgroundApplication API to enable Application
 *                      background
 *                  4.terminate ability
 *                  5.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_001, TestSize.Level2)
{
    ASSERT_NE(serviceInner_, nullptr);
    pid_t pid = 1024;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld");
    auto appInfo = GetApplicationInfo("com.ohos.test.helloworld");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    ASSERT_TRUE(mockAppScheduler);

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;
    auto appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);
    int32_t recordId = appRunningRecord->GetRecordId();

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    ChangeAppToTerminate(mockAppScheduler, appRunningRecord, token, true);
    auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of abilities
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                    foreground
 *                  3.load other ability, and switch last ability to background and call ScheduleBackgroundApplication
 *                    API to enable Application background
 *                  4.terminate every ability
 *                  5.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_002, TestSize.Level3)
{
    pid_t pid = 1023;
    ASSERT_NE(serviceInner_, nullptr);
    std::shared_ptr<AppRunningRecord> appRunningRecord = nullptr;
    std::vector<sptr<IRemoteObject>> tokens;
    auto abilityInfo = std::make_shared<AbilityInfo>();
    auto appInfo = std::make_shared<ApplicationInfo>();
    sptr<IRemoteObject> token;
    int32_t recordId;
    bool flag = false;
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;

    for (int i = 0; i < ABILITY_NUM; i++) {
        abilityInfo = GetAbilityInfo(std::to_string(i), "MainAbility", "p1", "com.ohos.test.helloworld");
        appInfo = GetApplicationInfo("com.ohos.test.helloworld");
        token = new (std::nothrow) MockAbilityToken();
        tokens.push_back(token);
        testProcessInfo.isStart = flag;
        appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);
        if (!flag) {
            ChangeAbilityStateAfterAppStart(mockAppScheduler, pid);
            recordId = appRunningRecord->GetRecordId();
            flag = true;
            CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
        } else {
            CheckState(
                appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_BACKGROUND);
        }
        ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
        CheckState(
            appRunningRecord, token, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);
        ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token);
        CheckState(
            appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
    }
    auto abilities = appRunningRecord->GetAbilities();
    int size = abilities.size();
    EXPECT_EQ(size, ABILITY_NUM);
    CheckStateAfterClearAbility(appRunningRecord, tokens, recordId, mockAppScheduler);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of abilities at the same time
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start 1000 abilities
 *                  2.switch every ability to foreground
 *                  3.switch every ability to background
 *                  4.terminate every ability
 *                  5.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_003, TestSize.Level3)
{
    pid_t pid = 1025;
    ASSERT_TRUE(serviceInner_);
    std::shared_ptr<AppRunningRecord> appRunningRecord = nullptr;
    std::vector<sptr<IRemoteObject>> tokens;
    auto abilityInfo = std::make_shared<AbilityInfo>();
    auto appInfo = std::make_shared<ApplicationInfo>();
    sptr<IRemoteObject> token;
    int32_t recordId;
    bool flag = false;
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;

    for (int i = 0; i < ABILITY_NUM; i++) {
        abilityInfo = GetAbilityInfo(std::to_string(i), "MainAbility", "p1", "com.ohos.test.helloworld1");
        appInfo = GetApplicationInfo("com.ohos.test.helloworld1");
        token = new (std::nothrow) MockAbilityToken();
        tokens.push_back(token);

        testProcessInfo.isStart = flag;
        appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);
        if (!flag) {
            ChangeAbilityStateAfterAppStart(mockAppScheduler, testProcessInfo.pid);
            recordId = appRunningRecord->GetRecordId();
            flag = true;
            CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
            ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
        } else {
            CheckState(
                appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_FOREGROUND);
            ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token, true);
        }
    }
    CheckStateAfterChangeAbility(appRunningRecord, tokens, mockAppScheduler);
    CheckStateAfterClearAbility(appRunningRecord, tokens, recordId, mockAppScheduler);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of abilities
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                      foreground
 *                  3.switch ability to background and call ScheduleBackgroundApplication API to enable Application
 *                      background
 *                  4.repeat step 2~3 1000 times
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_004, TestSize.Level3)
{
    ASSERT_NE(serviceInner_, nullptr);
    pid_t pid = 1024;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "p3", "com.ohos.test.helloworld");
    auto appInfo = GetApplicationInfo("com.ohos.test.helloworld");

    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;

    auto appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    int count = 1000;
    while (count > 0) {
        ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
        CheckState(
            appRunningRecord, token, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

        ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token);
        CheckState(
            appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
        count--;
    }
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of ability
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                      foreground
 *                  3.switch ability to background
 *                  4.start new ability to foreground
 *                  5.switch ability to background and call ScheduleBackgroundApplication API to enable Application
 *                      background
 *                  6.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_005, TestSize.Level2)
{
    ASSERT_NE(serviceInner_, nullptr);
    pid_t pid = 1024;

    sptr<IRemoteObject> token0 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld");
    auto appInfo = GetApplicationInfo("com.ohos.test.helloworld");

    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;

    auto appRunningRecord =
        StartProcessAndLoadAbility(mockAppScheduler, token0, abilityInfo0, appInfo, testProcessInfo);

    int32_t recordId = appRunningRecord->GetRecordId();

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid);
    CheckState(appRunningRecord, token0, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token0);
    CheckState(
        appRunningRecord, token0, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token0);
    CheckState(
        appRunningRecord, token0, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    sptr<IRemoteObject> token1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo1 = GetAbilityInfo("1", "SubAbility", "p1", "com.ohos.test.helloworld");
    testProcessInfo.isStart = true;
    appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token1, abilityInfo1, appInfo, testProcessInfo);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token1);
    CheckState(
        appRunningRecord, token1, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token1);
    CheckState(
        appRunningRecord, token1, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    ChangeAppToTerminate(mockAppScheduler, appRunningRecord, token0);
    ChangeAppToTerminate(mockAppScheduler, appRunningRecord, token1, true);
    auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of ability
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                      foreground
 *                  3.switch ability to background and call ScheduleBackgroundApplication API to enable Application
 *                      background
 *                  4.through appName kill application
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_006, TestSize.Level2)
{
    ASSERT_NE(serviceInner_, nullptr);

    pid_t pid = fork();
    if (pid == 0) {
        pause();
        exit(0);
    }

    ASSERT_TRUE(pid > 0);
    usleep(50000);

    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld");
    auto appInfo = GetApplicationInfo("com.ohos.test.helloworld");
    appInfo->bundleName = "com.ohos.test.helloworld";
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;

    auto appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);
    int32_t recordId = appRunningRecord->GetRecordId();

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    EXPECT_CALL(*mockAppScheduler, ScheduleProcessSecurityExit()).Times(1);
    int32_t ret = serviceInner_->KillApplication(appInfo->bundleName);
    EXPECT_EQ(ret, 0);
    serviceInner_->OnRemoteDied(mockAppScheduler);  // A faked death recipient.
    auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of ability
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                      foreground
 *                  3.switch ability to background and call ScheduleBackgroundApplication API to enable Application
 *                      background
 *                  4.terminate ability
 *                  5.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_007, TestSize.Level2)
{
    ASSERT_NE(serviceInner_, nullptr);
    pid_t pid = 1024;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld");
    auto appInfo = GetApplicationInfo("com.ohos.test.helloworld");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;

    auto appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);
    int32_t recordId = appRunningRecord->GetRecordId();

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    ChangeAppToTerminate(mockAppScheduler, appRunningRecord, token, true);
    auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of ability
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start app
 *                  2.switch ability to foreground and call ScheduleForegroundApplication API to enable Application
 *                      foreground
 *                  3.switch ability to background and call ScheduleBackgroundApplication API to enable Application
 *                      background
 *                  4.call loadAbility API to start new app
 *                  5.switch ability to foreground and call ScheduleForegroundApplication API to enable new Application
 *                      foreground
 *                  6.switch ability to background and call ScheduleBackgroundApplication API to enable new Application
 *                      background
 *                  7.terminate ability
 *                  8.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_008, TestSize.Level2)
{
    ASSERT_NE(serviceInner_, nullptr);
    pid_t pid_0 = 1024;
    pid_t pid_1 = 2048;
    sptr<IRemoteObject> token_0 = new (std::nothrow) MockAbilityToken();
    sptr<IRemoteObject> token_1 = new (std::nothrow) MockAbilityToken();
    auto abilityInfo_0 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld_0");
    auto appInfo_0 = GetApplicationInfo("com.ohos.test.helloworld_0");
    auto abilityInfo_1 = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld_1");
    auto appInfo_1 = GetApplicationInfo("com.ohos.test.helloworld_1");
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    TestProcessInfo testProcessInfo;

    testProcessInfo.pid = pid_0;
    testProcessInfo.isStart = false;
    auto appRunningRecord_0 =
        StartProcessAndLoadAbility(mockAppScheduler, token_0, abilityInfo_0, appInfo_0, testProcessInfo);

    testProcessInfo.pid = pid_1;
    testProcessInfo.isStart = false;
    auto appRunningRecord_1 =
        StartProcessAndLoadAbility(mockAppScheduler, token_1, abilityInfo_1, appInfo_1, testProcessInfo);
    int32_t recordId_0 = appRunningRecord_0->GetRecordId();
    int32_t recordId_1 = appRunningRecord_1->GetRecordId();

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid_0);
    CheckState(appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord_0, token_0);
    CheckState(
        appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord_0, token_0);
    CheckState(
        appRunningRecord_0, token_0, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    ChangeAbilityStateAfterAppStart(mockAppScheduler, pid_1);
    CheckState(appRunningRecord_1, token_1, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord_1, token_1);
    CheckState(
        appRunningRecord_1, token_1, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord_1, token_1);
    CheckState(
        appRunningRecord_1, token_1, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

    ChangeAppToTerminate(mockAppScheduler, appRunningRecord_0, token_0, true);
    auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId_0);
    EXPECT_EQ(nullptr, record);

    ChangeAppToTerminate(mockAppScheduler, appRunningRecord_1, token_1, true);
    record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId_1);
    EXPECT_EQ(nullptr, record);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test app life cycle change with the start of abilities at the same time
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start 100 app
 *                  2.switch every ability to foreground
 *                  3.switch every ability to background
 *                  4.terminate every ability
 *                  5.call ScheduleTerminateApplication API to make app terminated
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_009, TestSize.Level3)
{
    pid_t pid = 1025;
    ASSERT_NE(serviceInner_, nullptr);
    std::shared_ptr<AppRunningRecord> appRunningRecord = nullptr;
    auto abilityInfo = std::make_shared<AbilityInfo>();
    auto appInfo = std::make_shared<ApplicationInfo>();
    sptr<IRemoteObject> token;
    int32_t recordId;
    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();

    TestProcessInfo testProcessInfo;

    for (int i = 0; i < APPLICATION_NUM; i++) {
        abilityInfo = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.helloworld1");
        appInfo = GetApplicationInfo("com.ohos.test.helloworld1");
        token = new (std::nothrow) MockAbilityToken();
        pid += i;

        testProcessInfo.pid = pid;
        testProcessInfo.isStart = false;
        appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);
        ChangeAbilityStateAfterAppStart(mockAppScheduler, testProcessInfo.pid);
        recordId = appRunningRecord->GetRecordId();
        CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
        ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);

        ChangeAbilityStateToBackGroud(mockAppScheduler, appRunningRecord, token);
        CheckState(
            appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);

        ChangeAppToTerminate(mockAppScheduler, appRunningRecord, token, true);
        auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId);
        EXPECT_EQ(nullptr, record);
    }
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test get and stop all process.
 * EnvConditions: system running normally
 * CaseDescription: 1.call loadAbility API to start 100 app
 *                  2.stop all process
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_010, TestSize.Level3)
{
    pid_t pid = 1025;
    ASSERT_NE(serviceInner_, nullptr);
    std::shared_ptr<AppRunningRecord> appRunningRecord = nullptr;
    int32_t recordId[APPLICATION_NUM];
    sptr<MockAppScheduler> mockAppScheduler[APPLICATION_NUM];

    TestProcessInfo testProcessInfo;
    for (int i = 0; i < APPLICATION_NUM; i++) {
        mockAppScheduler[i] = new MockAppScheduler();

        char index[32];
        int ref = snprintf_s(index, sizeof(index), sizeof(index) - 1, "%d", i);
        ASSERT_TRUE(ref > 0);
        char name[128];
        ref = snprintf_s(name, sizeof(name), sizeof(name) - 1, "com.ohos.test.helloworld%d", i);
        ASSERT_TRUE(ref > 0);
        auto abilityInfo = GetAbilityInfo(index, "MainAbility", index, name);
        auto appInfo = GetApplicationInfo(name);
        auto token = new (std::nothrow) MockAbilityToken();
        pid += i;

        testProcessInfo.pid = pid;
        testProcessInfo.isStart = false;

        appRunningRecord =
            StartProcessAndLoadAbility(mockAppScheduler[i], token, abilityInfo, appInfo, testProcessInfo);
        ASSERT_TRUE(appRunningRecord);

        ChangeAbilityStateAfterAppStart(mockAppScheduler[i], testProcessInfo.pid);

        recordId[i] = appRunningRecord->GetRecordId();

        CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);
        ChangeAbilityStateToForegroud(mockAppScheduler[i], appRunningRecord, token);

        ChangeAbilityStateToBackGroud(mockAppScheduler[i], appRunningRecord, token);
        CheckState(
            appRunningRecord, token, AbilityState::ABILITY_STATE_BACKGROUND, ApplicationState::APP_STATE_BACKGROUND);
        EXPECT_CALL(*mockAppScheduler[i], ScheduleProcessSecurityExit()).Times(1);
    }

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());
    auto allRunningProcessInfo = std::make_shared<RunningProcessInfo>();
    serviceInner_->GetAllRunningProcesses(allRunningProcessInfo);
    EXPECT_EQ(allRunningProcessInfo->appProcessInfos.size(), size_t(APPLICATION_NUM));

    serviceInner_->StopAllProcess();

    for (int i = 0; i < APPLICATION_NUM; i++) {
        serviceInner_->OnRemoteDied(mockAppScheduler[i]);  // A faked death recipient.
        auto record = serviceInner_->GetAppRunningRecordByAppRecordId(recordId[i]);
        EXPECT_EQ(nullptr, record);
    }
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test getrecentapplist and removeappfromrecentlist all process.
 * EnvConditions: system running normally
 * CaseDescription: 1.call getrecentapplist API to get current app list
 *                  2.call removeappfromrecentlist API to remove current app list
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_011, TestSize.Level0)
{
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
    CreateAppRecentList(INDEX_NUM_100);
    EXPECT_EQ(INDEX_NUM_100, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));
    for (int32_t i = INDEX_NUM_MAX; i > 0; i--) {
        std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
        appInfo->name = TEST_APP_NAME + std::to_string(i);
        appInfo->bundleName = appInfo->name;  // specify process condition
        auto appTaskInfo =
            serviceInner_->appProcessManager_->GetAppTaskInfoByProcessName(appInfo->name, appInfo->bundleName);
        serviceInner_->appProcessManager_->RemoveAppFromRecentList(appTaskInfo);
    }
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test getrecentapplist and clearrecentappList all process.
 * EnvConditions: system running normally
 * CaseDescription: 1.call getrecentapplist API to get current app list
 *                  2.call clearrecentapplist API to clear current app list
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_012, TestSize.Level0)
{
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
    CreateAppRecentList(INDEX_NUM_100);
    EXPECT_EQ(INDEX_NUM_100, static_cast<int32_t>(serviceInner_->GetRecentAppList().size()));

    serviceInner_->appProcessManager_->ClearRecentAppList();
    EXPECT_TRUE(serviceInner_->GetRecentAppList().empty());
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test get and stop all process.
 * EnvConditions: system running normally
 * CaseDescription: OnStop
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_013, TestSize.Level3)
{
    ASSERT_TRUE(serviceInner_);
    ASSERT_TRUE(serviceInner_->remoteClientManager_);
    ASSERT_TRUE(serviceInner_->remoteClientManager_->GetSpawnClient());

    auto mockAppSpawnSocket = std::make_shared<MockAppSpawnSocket>();
    ASSERT_TRUE(mockAppSpawnSocket);
    serviceInner_->remoteClientManager_->GetSpawnClient()->SetSocket(mockAppSpawnSocket);

    EXPECT_EQ(serviceInner_->QueryAppSpawnConnectionState(), SpawnConnectionState::STATE_NOT_CONNECT);

    EXPECT_CALL(*mockAppSpawnSocket, OpenAppSpawnConnection()).Times(1).WillOnce(Return(0));

    int ret = serviceInner_->OpenAppSpawnConnection();
    ASSERT_EQ(ret, 0);
    EXPECT_EQ(serviceInner_->QueryAppSpawnConnectionState(), SpawnConnectionState::STATE_CONNECTED);

    EXPECT_CALL(*mockAppSpawnSocket, CloseAppSpawnConnection()).Times(1);

    serviceInner_->OnStop();
    usleep(50000);
    EXPECT_EQ(serviceInner_->QueryAppSpawnConnectionState(), SpawnConnectionState::STATE_NOT_CONNECT);
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: test get app Running Record,When the app is added for the first time
 * EnvConditions: system running normally
 * CaseDescription: GetOrCreateAppRunningRecord
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_014, TestSize.Level0)
{

    pid_t pid = 1000;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;
    auto appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);

    // Because GetOrCreateAppRunningRecord was called in LoadAbility.
    // When the app is added for the first time
    EXPECT_EQ(appRunningRecord->GetName(), "com.ohos.test.special");
    EXPECT_EQ(appRunningRecord->GetProcessName(), "p1");
}

/*
 * Feature: Ams
 * Function: AppLifeCycle
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: OnAbilityRequestDone
 */
HWTEST_F(AmsAppLifeCycleModuleTest, StateChange_015, TestSize.Level0)
{
    pid_t pid = 1000;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "p1", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    TestProcessInfo testProcessInfo;
    testProcessInfo.pid = pid;
    testProcessInfo.isStart = false;

    auto appRunningRecord = StartProcessAndLoadAbility(mockAppScheduler, token, abilityInfo, appInfo, testProcessInfo);

    EXPECT_CALL(*mockAppScheduler, ScheduleBackgroundApplication()).Times(1);
    EXPECT_CALL(*mockAppStateCallbackStub_, OnAbilityRequestDone(_, _))
        .Times(1)
        .WillOnce(testing::Invoke(InvokeOnAbilityRequestDone));

    ChangeAbilityStateAfterAppStart(mockAppScheduler, testProcessInfo.pid);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_READY, ApplicationState::APP_STATE_READY);

    ChangeAbilityStateToForegroud(mockAppScheduler, appRunningRecord, token);
    CheckState(appRunningRecord, token, AbilityState::ABILITY_STATE_FOREGROUND, ApplicationState::APP_STATE_FOREGROUND);

    std::weak_ptr<AppMgrServiceInner> inner = inner_;
    appRunningRecord->SetAppMgrServiceInner(inner);

    appRunningRecord->UpdateAbilityState(token, AbilityState::ABILITY_STATE_BACKGROUND);
}

/*
 * Feature: Ams
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: Ability record update(specify process mode)
 */
HWTEST_F(AmsAppLifeCycleModuleTest, AbilityBehaviorAnalysis_01, TestSize.Level0)
{
    const int32_t formateVaule = 0;
    const int32_t visibility = 1;
    const int32_t perceptibility = 1;
    const int32_t connectionState = 1;

    pid_t pid = 123;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "pNew", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());

    StartAppProcess(pid);
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    std::shared_ptr<AppRunningRecord> record =
        serviceInner_->GetAppRunningRecordByProcessName(appInfo->name, abilityInfo->process);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, pid);
    }

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord->GetVisibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetPerceptibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetConnectionState(), formateVaule);

    serviceInner_->AbilityBehaviorAnalysis(token, nullptr, visibility, perceptibility, connectionState);

    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    auto abilityRecord_1 = appRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord_1->GetToken(), token);
    EXPECT_EQ(abilityRecord_1->GetPreToken(), nullptr);
    EXPECT_EQ(abilityRecord_1->GetVisibility(), visibility);
    EXPECT_EQ(abilityRecord_1->GetPerceptibility(), perceptibility);
    EXPECT_EQ(abilityRecord_1->GetConnectionState(), connectionState);
}

/*
 * Feature: Ams
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: Ability record update(specify process mode and assign visibility)
 */
HWTEST_F(AmsAppLifeCycleModuleTest, AbilityBehaviorAnalysis_02, TestSize.Level0)
{
    const int32_t formateVaule = 0;
    const int32_t visibility = 1;
    const int32_t perceptibility = 0;
    const int32_t connectionState = 0;

    pid_t pid = 123;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "pNew", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());

    StartAppProcess(pid);
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    std::shared_ptr<AppRunningRecord> record =
        serviceInner_->GetAppRunningRecordByProcessName(appInfo->name, abilityInfo->process);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, pid);
    }

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord->GetVisibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetPerceptibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetConnectionState(), formateVaule);

    serviceInner_->AbilityBehaviorAnalysis(token, nullptr, visibility, perceptibility, connectionState);

    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    auto abilityRecord_1 = appRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord_1->GetToken(), token);
    EXPECT_EQ(abilityRecord_1->GetPreToken(), nullptr);
    EXPECT_EQ(abilityRecord_1->GetVisibility(), visibility);
    EXPECT_EQ(abilityRecord_1->GetPerceptibility(), perceptibility);
    EXPECT_EQ(abilityRecord_1->GetConnectionState(), connectionState);
}

/*
 * Feature: Ams
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: Ability record update(specify process mode and assign perceptibility)
 */
HWTEST_F(AmsAppLifeCycleModuleTest, AbilityBehaviorAnalysis_03, TestSize.Level0)
{
    const int32_t formateVaule = 0;
    const int32_t visibility = 0;
    const int32_t perceptibility = 1;
    const int32_t connectionState = 0;

    pid_t pid = 123;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "pNew", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());

    StartAppProcess(pid);
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    std::shared_ptr<AppRunningRecord> record =
        serviceInner_->GetAppRunningRecordByProcessName(appInfo->name, abilityInfo->process);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, pid);
    }

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord->GetVisibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetPerceptibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetConnectionState(), formateVaule);

    serviceInner_->AbilityBehaviorAnalysis(token, nullptr, visibility, perceptibility, connectionState);

    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    auto abilityRecord_1 = appRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord_1->GetToken(), token);
    EXPECT_EQ(abilityRecord_1->GetPreToken(), nullptr);
    EXPECT_EQ(abilityRecord_1->GetVisibility(), visibility);
    EXPECT_EQ(abilityRecord_1->GetPerceptibility(), perceptibility);
    EXPECT_EQ(abilityRecord_1->GetConnectionState(), connectionState);
}

/*
 * Feature: Ams
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: Ability record update(specify process mode and assign connectionState)
 */
HWTEST_F(AmsAppLifeCycleModuleTest, AbilityBehaviorAnalysis_04, TestSize.Level0)
{
    const int32_t formateVaule = 0;
    const int32_t visibility = 0;
    const int32_t perceptibility = 0;
    const int32_t connectionState = 1;

    pid_t pid = 123;
    sptr<IRemoteObject> token = GetAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "pNew", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());

    StartAppProcess(pid);
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    std::shared_ptr<AppRunningRecord> record =
        serviceInner_->GetAppRunningRecordByProcessName(appInfo->name, abilityInfo->process);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, pid);
    }

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord->GetVisibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetPerceptibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetConnectionState(), formateVaule);

    serviceInner_->AbilityBehaviorAnalysis(token, nullptr, visibility, perceptibility, connectionState);

    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    auto abilityRecord_1 = appRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord_1->GetToken(), token);
    EXPECT_EQ(abilityRecord_1->GetPreToken(), nullptr);
    EXPECT_EQ(abilityRecord_1->GetVisibility(), visibility);
    EXPECT_EQ(abilityRecord_1->GetPerceptibility(), perceptibility);
    EXPECT_EQ(abilityRecord_1->GetConnectionState(), connectionState);
}

/*
 * Feature: Ams
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: Ability record update(specify process mode and assign proken)
 */
HWTEST_F(AmsAppLifeCycleModuleTest, AbilityBehaviorAnalysis_05, TestSize.Level0)
{
    const int32_t formateVaule = 0;
    const int32_t visibility = 0;
    const int32_t perceptibility = 0;
    const int32_t connectionState = 0;

    pid_t pid = 123;
    sptr<IRemoteObject> token = new (std::nothrow) MockAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "pNew", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());

    StartAppProcess(pid);
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    std::shared_ptr<AppRunningRecord> record =
        serviceInner_->GetAppRunningRecordByProcessName(appInfo->name, abilityInfo->process);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, pid);
    }

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord->GetVisibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetPerceptibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetConnectionState(), formateVaule);

    sptr<IRemoteObject> preToken = new (std::nothrow) MockAbilityToken();
    serviceInner_->AbilityBehaviorAnalysis(token, preToken, visibility, perceptibility, connectionState);

    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    auto abilityRecord_1 = appRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord_1->GetToken(), token);
    EXPECT_EQ(abilityRecord_1->GetPreToken(), preToken);
    EXPECT_EQ(abilityRecord_1->GetVisibility(), visibility);
    EXPECT_EQ(abilityRecord_1->GetPerceptibility(), perceptibility);
    EXPECT_EQ(abilityRecord_1->GetConnectionState(), connectionState);
}

/*
 * Feature: Ams
 * Function: AbilityBehaviorAnalysis
 * SubFunction: NA
 * FunctionPoints: Ability Status change received
 * EnvConditions: system running normally
 * CaseDescription: Ability record update(specify process mode and assign bundleName)
 */
HWTEST_F(AmsAppLifeCycleModuleTest, AbilityBehaviorAnalysis_06, TestSize.Level0)
{
    const int32_t formateVaule = 0;
    const int32_t visibility = 0;
    const int32_t perceptibility = 0;
    const int32_t connectionState = 0;

    pid_t pid = 123;
    sptr<IRemoteObject> token = new (std::nothrow) MockAbilityToken();
    auto abilityInfo = GetAbilityInfo("0", "MainAbility", "pNew", "com.ohos.test.special");
    auto appInfo = GetApplicationInfo("com.ohos.test.special");
    appInfo->bundleName = appInfo->name;
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();

    sptr<BundleMgrService> bundleMgr = new BundleMgrService();
    serviceInner_->SetBundleManager(bundleMgr.GetRefPtr());

    StartAppProcess(pid);
    serviceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    std::shared_ptr<AppRunningRecord> record =
        serviceInner_->GetAppRunningRecordByProcessName(appInfo->name, abilityInfo->process);
    if (record == nullptr) {
        EXPECT_TRUE(false);
    } else {
        pid_t newPid = record->GetPriorityObject()->GetPid();
        EXPECT_EQ(newPid, pid);
    }

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord->GetVisibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetPerceptibility(), formateVaule);
    EXPECT_EQ(abilityRecord->GetConnectionState(), formateVaule);

    sptr<IRemoteObject> preToken = new (std::nothrow) MockAbilityToken();
    serviceInner_->AbilityBehaviorAnalysis(token, preToken, visibility, perceptibility, connectionState);

    auto appRecord = serviceInner_->GetAppRunningRecordByAbilityToken(token);
    auto abilityRecord_1 = appRecord->GetAbilityRunningRecordByToken(token);

    EXPECT_EQ(abilityRecord_1->GetToken(), token);
    EXPECT_EQ(abilityRecord_1->GetPreToken(), preToken);
    EXPECT_EQ(abilityRecord_1->GetVisibility(), visibility);
    EXPECT_EQ(abilityRecord_1->GetPerceptibility(), perceptibility);
    EXPECT_EQ(abilityRecord_1->GetConnectionState(), connectionState);
}

}  // namespace AppExecFwk
}  // namespace OHOS