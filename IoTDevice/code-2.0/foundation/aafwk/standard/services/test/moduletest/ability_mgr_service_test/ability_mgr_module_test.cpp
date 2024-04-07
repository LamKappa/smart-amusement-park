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

#include <thread>
#include <functional>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#define protected public
#include "mock_ability_connect_callback_stub.h"
#include "mock_ability_scheduler.h"
#include "mock_app_mgr_client.h"
#include "mock_bundle_mgr.h"
#include "ability_record_info.h"
#include "ability_manager_errors.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"
#include "ability_manager_service.h"
#include "ability_connect_callback_proxy.h"
#include "ability_config.h"
#undef private
#undef protected

using namespace std::placeholders;
using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using OHOS::iface_cast;
using OHOS::IRemoteObject;
using OHOS::sptr;
using testing::_;
using testing::Invoke;
using testing::Return;

namespace OHOS {
namespace AAFwk {

#if 0
class AbilityMgrModuleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Want CreateWant(const std::string &entity);
    AbilityInfo CreateAbilityInfo(const std::string &name, const std::string &appName, const std::string &bundleName);
    ApplicationInfo CreateAppInfo(const std::string &appName, const std::string &name);
    Want CreateWant(const std::string &abilityName, const std::string &bundleName);
    void waitAMS();
    std::shared_ptr<AbilityRecord> GreatePageAbility(const std::string &abilityName, const std::string &bundleName);
    void MockAbilityTransitionDone(
        bool &testFailed, sptr<IRemoteObject> &dataAbilityToken, sptr<MockAbilityScheduler> &mockDataAbilityScheduler);
    void MockDataAbilityLoadHandlerInner(
        bool &testFailed, sptr<IRemoteObject> &dataAbilityToken, sptr<MockAbilityScheduler> &mockDataAbilityScheduler);
    void MockAbilityBehaviorAnalysisInner(
        bool &testFailed, sptr<IRemoteObject> &dataAbilityToken, std::shared_ptr<AbilityRecord> &topAbility);
    void QueryDataAbilityByUriInner(bool &testFailed);
    void CreateAbilityRequest(const std::string &abilityName, const std::string bundleName, Want &want,
        std::shared_ptr<MissionStack> &curMissionStack, sptr<IRemoteObject> &recordToken);
    void MockServiceAbilityLoadHandlerInner(bool &testResult, const std::string &bundleName,
        const std::string &abilityName, sptr<IRemoteObject> &testToken);
    void CreateServiceRecord(std::shared_ptr<AbilityRecord> &record, Want &want,
        const sptr<AbilityConnectionProxy> &callback1, const sptr<AbilityConnectionProxy> &callback2);
    void CheckTestRecord(std::shared_ptr<AbilityRecord> &record1, std::shared_ptr<AbilityRecord> &record2,
        const sptr<AbilityConnectionProxy> &callback1, const sptr<AbilityConnectionProxy> &callback2);
    void MockLoadHandlerInner(int &testId, sptr<MockAbilityScheduler> &scheduler);

    inline static std::shared_ptr<MockAppMgrClient> mockAppMgrClient_;
    inline static std::shared_ptr<AbilityManagerService> abilityMgrServ_;
    inline static sptr<BundleMgrService> mockBundleMgr_;
    sptr<MockAbilityScheduler> scheduler_;

    static constexpr int TEST_WAIT_TIME = 100000;
};

void AbilityMgrModuleTest::SetUpTestCase(void)
{}

void AbilityMgrModuleTest::TearDownTestCase(void)
{}

void AbilityMgrModuleTest::SetUp(void)
{
    mockBundleMgr_ = new BundleMgrService();
    auto saMgr = OHOS::DelayedSingleton<SaMgrClient>::GetInstance();
    saMgr->RegisterSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, mockBundleMgr_);

    abilityMgrServ_ = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance();
    abilityMgrServ_->OnStart();
    mockAppMgrClient_ = std::make_shared<MockAppMgrClient>();
    waitAMS();
    scheduler_ = new MockAbilityScheduler();
    abilityMgrServ_->appScheduler_->appMgrClient_.reset(mockAppMgrClient_.get());
}

void AbilityMgrModuleTest::TearDown(void)
{
    mockAppMgrClient_.reset();
    abilityMgrServ_->OnStop();
    OHOS::DelayedSingleton<AbilityManagerService>::DestroyInstance();
}

Want AbilityMgrModuleTest::CreateWant(const std::string &entity)
{
    Want want;
    if (!entity.empty()) {
        want.AddEntity(entity);
    }
    return want;
}

AbilityInfo AbilityMgrModuleTest::CreateAbilityInfo(
    const std::string &name, const std::string &appName, const std::string &bundleName)
{
    AbilityInfo abilityInfo;
    abilityInfo.name = name;
    abilityInfo.applicationName = appName;
    abilityInfo.bundleName = bundleName;

    return abilityInfo;
}

ApplicationInfo AbilityMgrModuleTest::CreateAppInfo(const std::string &appName, const std::string &bundleName)
{
    ApplicationInfo appInfo;
    appInfo.name = appName;
    appInfo.bundleName = bundleName;

    return appInfo;
}

Want AbilityMgrModuleTest::CreateWant(const std::string &abilityName, const std::string &bundleName)
{
    ElementName element;
    element.SetDeviceID("devices");
    element.SetAbilityName(abilityName);
    element.SetBundleName(bundleName);
    Want want;
    want.SetElement(element);
    return want;
}

std::shared_ptr<AbilityRecord> AbilityMgrModuleTest::GreatePageAbility(
    const std::string &abilityName, const std::string &bundleName)
{
    Want want = CreateWant(abilityName, bundleName);
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    int testRequestCode = 1;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    waitAMS();

    auto stack = abilityMgrServ_->GetStackManager();
    if (!stack) {
        return nullptr;
    }
    auto topAbility = stack->GetCurrentTopAbility();
    if (!topAbility) {
        return nullptr;
    }
    topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    return topAbility;
}

void AbilityMgrModuleTest::MockAbilityTransitionDone(
    bool &testFailed, sptr<IRemoteObject> &dataAbilityToken, sptr<MockAbilityScheduler> &mockDataAbilityScheduler)
{
    auto mockAbilityTransation = [&testFailed, &dataAbilityToken](
                                     const Want &want, const LifeCycleStateInfo &targetState) {
        testFailed = testFailed || (targetState.state != ABILITY_STATE_ACTIVE);
        std::thread(&AbilityManagerService::AbilityTransitionDone, abilityMgrServ_.get(), dataAbilityToken, ACTIVE)
            .detach();
    };

    EXPECT_CALL(*mockDataAbilityScheduler, ScheduleAbilityTransaction(_, _))
        .Times(1)
        .WillOnce(Invoke(mockAbilityTransation));
}

void AbilityMgrModuleTest::MockDataAbilityLoadHandlerInner(
    bool &testFailed, sptr<IRemoteObject> &dataAbilityToken, sptr<MockAbilityScheduler> &mockDataAbilityScheduler)
{
    // MOCK: data ability load handler

    auto mockLoadAbility = [&testFailed, &dataAbilityToken, &mockDataAbilityScheduler](const sptr<IRemoteObject> &token,
                               const sptr<IRemoteObject> &preToken,
                               const AbilityInfo &abilityInfo,
                               const ApplicationInfo &appInfo) {
        dataAbilityToken = token;
        testFailed = testFailed || (abilityInfo.type != AbilityType::DATA);
        std::thread(&AbilityManagerService::AttachAbilityThread, abilityMgrServ_.get(), mockDataAbilityScheduler, token)
            .detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1).WillOnce(Invoke(mockLoadAbility));

    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _))
        .Times(2)
        .WillOnce(Invoke([&testFailed](const sptr<IRemoteObject> &token, const OHOS::AppExecFwk::AbilityState state) {
            testFailed = testFailed || (state != OHOS::AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND);
            return AppMgrResultCode::RESULT_OK;
        }))
        .WillOnce(Invoke([&testFailed](const sptr<IRemoteObject> &token, const OHOS::AppExecFwk::AbilityState state) {
            testFailed = testFailed || (state != OHOS::AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
            return AppMgrResultCode::RESULT_OK;
        }));
}

void AbilityMgrModuleTest::MockAbilityBehaviorAnalysisInner(
    bool &testFailed, sptr<IRemoteObject> &dataAbilityToken, std::shared_ptr<AbilityRecord> &topAbility)
{
    EXPECT_CALL(*mockAppMgrClient_, AbilityBehaviorAnalysis(_, _, _, _, _))
        .Times(2)
        .WillOnce(Invoke([&testFailed, &dataAbilityToken, &topAbility](const sptr<IRemoteObject> &token,
                             const sptr<IRemoteObject> &preToken,
                             const int32_t visibility,
                             const int32_t perceptibility,
                             const int32_t connectionState) {
            testFailed = testFailed || (token != dataAbilityToken);
            testFailed = testFailed || (Token::GetAbilityRecordByToken(preToken) != topAbility);
            testFailed = testFailed || (visibility != 0);
            testFailed = testFailed || (perceptibility != 0);
            testFailed = testFailed || (connectionState != 1);
            return AppMgrResultCode::RESULT_OK;
        }))
        .WillOnce(Invoke([&testFailed, &dataAbilityToken, &topAbility](const sptr<IRemoteObject> &token,
                             const sptr<IRemoteObject> &preToken,
                             const int32_t visibility,
                             const int32_t perceptibility,
                             const int32_t connectionState) {
            testFailed = testFailed || (token != dataAbilityToken);
            testFailed = testFailed || (Token::GetAbilityRecordByToken(preToken) != topAbility);
            testFailed = testFailed || (visibility != 0);
            testFailed = testFailed || (perceptibility != 0);
            testFailed = testFailed || (connectionState != 0);
            return AppMgrResultCode::RESULT_OK;
        }));
}

void AbilityMgrModuleTest::QueryDataAbilityByUriInner(bool &testFailed)
{
    // MOCK: bms query data ability by uri
    auto mockQueryAbilityInfoByUri = [&testFailed](const std::string &abilityUri, AbilityInfo &abilityInfo) {
        testFailed = testFailed || (abilityUri != "dataability://data.bundle.DataAbility");
        abilityInfo.applicationInfo.name = "com.ix.hiData";
        abilityInfo.applicationInfo.bundleName = "com.ix.hiData";
        abilityInfo.name = "DataAbility";
        abilityInfo.bundleName = "com.ix.hiData";
        abilityInfo.type = AbilityType::DATA;
        return !testFailed;
    };

    EXPECT_CALL(*mockBundleMgr_, QueryAbilityInfoByUri(_, _)).Times(1).WillOnce(Invoke(mockQueryAbilityInfoByUri));
}

void AbilityMgrModuleTest::CreateAbilityRequest(const std::string &abilityName, const std::string bundleName,
    Want &want, std::shared_ptr<MissionStack> &curMissionStack, sptr<IRemoteObject> &recordToken)
{
    Want want2 = CreateWant(abilityName, bundleName);
    AbilityRequest abilityRequest2;
    abilityRequest2.want = want2;
    abilityRequest2.abilityInfo.type = OHOS::AppExecFwk::AbilityType::PAGE;
    abilityRequest2.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    std::shared_ptr<AbilityRecord> abilityRecord2 = AbilityRecord::CreateAbilityRecord(abilityRequest2);
    abilityRecord2->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    mission->AddAbilityRecordToTop(abilityRecord2);
    auto stackManager_ = abilityMgrServ_->GetStackManager();
    curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);
    recordToken = abilityRecord2->GetToken();

    want = CreateWant(abilityName + "_service", bundleName + "_service");
    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName + "_service", bundleName, bundleName + "_service");
    abilityRequest.abilityInfo.type = OHOS::AppExecFwk::AbilityType::SERVICE;
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName + "_service");
    abilityMgrServ_->RemoveAllServiceRecord();
}

void AbilityMgrModuleTest::MockServiceAbilityLoadHandlerInner(
    bool &testResult, const std::string &bundleName, const std::string &abilityName, sptr<IRemoteObject> &testToken)
{
    auto mockHandler = [&testResult, &bundleName, &abilityName, &testToken](const sptr<IRemoteObject> &token,
                           const sptr<IRemoteObject> &preToken,
                           const AbilityInfo &abilityInfo,
                           const ApplicationInfo &appInfo) {
        testToken = token;
        testResult = !!testToken && abilityInfo.bundleName == bundleName && abilityInfo.name == abilityName &&
                     appInfo.bundleName == bundleName;
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1).WillOnce(Invoke(mockHandler));
}

void AbilityMgrModuleTest::CreateServiceRecord(std::shared_ptr<AbilityRecord> &record, Want &want,
    const sptr<AbilityConnectionProxy> &callback1, const sptr<AbilityConnectionProxy> &callback2)
{
    record = abilityMgrServ_->connectManager_->GetServiceRecordByElementName(want.GetElement().GetURI());
    ASSERT_TRUE(record);
    ASSERT_TRUE(abilityMgrServ_->connectManager_->IsAbilityConnected(
        record, abilityMgrServ_->GetConnectRecordListByCallback(callback1)));
    ASSERT_TRUE(abilityMgrServ_->connectManager_->IsAbilityConnected(
        record, abilityMgrServ_->GetConnectRecordListByCallback(callback2)));
    EXPECT_EQ((std::size_t)2, record->GetConnectRecordList().size());
    record->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
}

void AbilityMgrModuleTest::CheckTestRecord(std::shared_ptr<AbilityRecord> &record1,
    std::shared_ptr<AbilityRecord> &record2, const sptr<AbilityConnectionProxy> &callback1,
    const sptr<AbilityConnectionProxy> &callback2)
{
    EXPECT_EQ((std::size_t)1, record1->GetConnectRecordList().size());
    EXPECT_EQ((std::size_t)1, record2->GetConnectRecordList().size());
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->GetConnectRecordListByCallback(callback1).size());
    EXPECT_EQ((std::size_t)2, abilityMgrServ_->connectManager_->GetServiceMap().size());

    abilityMgrServ_->DisconnectAbility(callback2);

    abilityMgrServ_->ScheduleDisconnectAbilityDone(record1->GetToken());
    abilityMgrServ_->ScheduleDisconnectAbilityDone(record2->GetToken());
    EXPECT_EQ((std::size_t)0, record1->GetConnectRecordList().size());
    EXPECT_EQ((std::size_t)0, record2->GetConnectRecordList().size());
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->GetConnectRecordListByCallback(callback2).size());

    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(2);
    abilityMgrServ_->AbilityTransitionDone(record1->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INITIAL);
    abilityMgrServ_->AbilityTransitionDone(record2->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INITIAL);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::TERMINATING, record1->GetAbilityState());
    EXPECT_EQ(OHOS::AAFwk::AbilityState::TERMINATING, record2->GetAbilityState());

    EXPECT_CALL(*mockAppMgrClient_, TerminateAbility(_)).Times(2);
    abilityMgrServ_->OnAbilityRequestDone(
        record1->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
    abilityMgrServ_->OnAbilityRequestDone(
        record2->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetServiceMap().size());
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetConnectMap().size());
}

void AbilityMgrModuleTest::MockLoadHandlerInner(int &testId, sptr<MockAbilityScheduler> &scheduler)
{
    auto handler = [&testId](const Want &want, bool restart, int startid) { testId = startid; };
    EXPECT_CALL(*scheduler, ScheduleCommandAbility(_, _, _))
        .Times(3)
        .WillOnce(Invoke(handler))
        .WillOnce(Invoke(handler))
        .WillOnce(Invoke(handler));
}

void AbilityMgrModuleTest::waitAMS()
{
    const uint32_t maxRetryCount = 1000;
    const uint32_t sleepTime = 1000;
    uint32_t count = 0;
    if (!abilityMgrServ_) {
        return;
    }
    auto handler = abilityMgrServ_->GetEventHandler();
    if (!handler) {
        return;
    }
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };
    if (handler->PostTask(f)) {
        while (!taskCalled.load()) {
            ++count;
            if (count >= maxRetryCount) {
                break;
            }
            usleep(sleepTime);
        }
    }
}

#define SLEEP(milli) std::this_thread::sleep_for(std::chrono::seconds(milli))

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: start ability.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_001, TestSize.Level1)
{
    ASSERT_TRUE(abilityMgrServ_);
    ASSERT_TRUE(mockAppMgrClient_);

    auto stackManager = abilityMgrServ_->GetStackManager();
    ASSERT_TRUE(stackManager);

    std::string abilityName = "MusicAbility";
    std::string bundleName = "com.ix.hiMusic";
    Want want = CreateWant(abilityName, bundleName);
    bool testResult = false;
    sptr<IRemoteObject> testToken;

    auto mockHandler = [&](const sptr<IRemoteObject> &token,
                           const sptr<IRemoteObject> &preToken,
                           const AbilityInfo &abilityInfo,
                           const ApplicationInfo &appInfo) {
        testToken = token;
        testResult = !!testToken && abilityInfo.bundleName == bundleName && abilityInfo.name == abilityName &&
                     appInfo.bundleName == bundleName;
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1).WillOnce(Invoke(mockHandler));

    int testRequestCode = 123;
    abilityMgrServ_->StartAbility(want, testRequestCode);

    EXPECT_TRUE(testResult);
    EXPECT_TRUE(testToken);

    auto testAbilityRecord = stackManager->GetAbilityRecordByToken(testToken);
    ASSERT_TRUE(testAbilityRecord);
    EXPECT_EQ(testAbilityRecord->GetRequestCode(), testRequestCode);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: terminate ability.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_002, TestSize.Level1)
{
    std::string abilityName = "MusicAbility";
    std::string appName = "test_app";
    std::string bundleName = "com.ix.hiMusic";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant("");
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName + "1", appName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(appName, bundleName);

    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);

    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName + "2", appName, bundleName);
    std::shared_ptr<AbilityRecord> abilityRecord2 = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilityRecord2->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(1);
    abilityRecord2->SetScheduler(scheduler);

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(1);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    mission->AddAbilityRecordToTop(abilityRecord);
    mission->AddAbilityRecordToTop(abilityRecord2);
    abilityRecord2->SetMissionRecord(mission);

    auto stackManager_ = abilityMgrServ_->GetStackManager();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);

    int result = abilityMgrServ_->TerminateAbility(abilityRecord2->GetToken(), -1, nullptr);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::INACTIVATING, abilityRecord2->GetAbilityState());
    EXPECT_EQ(1, mission->GetAbilityRecordCount());
    EXPECT_EQ(OHOS::ERR_OK, result);

    mission.reset();
    abilityRecord2->lifecycleDeal_->abilityScheduler_.clear();
    abilityRecord2->scheduler_.clear();
    abilityRecord2.reset();
    scheduler.clear();
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: connect ability.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_003, TestSize.Level1)
{
    std::string abilityName = "MusicAbility";
    std::string bundleName = "com.ix.hiMusic";

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(2);

    Want want2 = CreateWant(abilityName, bundleName);
    AbilityRequest abilityRequest2;
    abilityRequest2.want = want2;
    abilityRequest2.abilityInfo.type = OHOS::AppExecFwk::AbilityType::PAGE;
    abilityRequest2.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    std::shared_ptr<AbilityRecord> abilityRecord2 = AbilityRecord::CreateAbilityRecord(abilityRequest2);
    abilityRecord2->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    mission->AddAbilityRecordToTop(abilityRecord2);
    auto stackManager_ = abilityMgrServ_->GetStackManager();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);

    Want want = CreateWant(abilityName + "_service", bundleName + "_service");
    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName + "_service", bundleName, bundleName + "_service");
    abilityRequest.abilityInfo.type = OHOS::AppExecFwk::AbilityType::SERVICE;
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName + "_service");
    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    abilityMgrServ_->RemoveAllServiceRecord();
    sptr<MockAbilityConnectCallbackStub> stub(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(stub));
    std::shared_ptr<ConnectionRecord> connectRecord =
        ConnectionRecord::CreateConnectionRecord(abilityRecord->GetToken(), abilityRecord, callback);
    ASSERT_TRUE(connectRecord != nullptr);
    connectRecord->SetConnectState(ConnectionState::CONNECTED);
    abilityRecord->AddConnectRecordToList(connectRecord);
    std::list<std::shared_ptr<ConnectionRecord> > connectList;
    connectList.push_back(connectRecord);
    abilityMgrServ_->connectManager_->connectMap_.emplace(callback->AsObject(), connectList);
    abilityMgrServ_->connectManager_->serviceMap_.emplace(abilityRequest.want.GetElement().GetURI(), abilityRecord);

    int result = abilityMgrServ_->ConnectAbility(want, callback, abilityRecord2->GetToken());
    EXPECT_EQ(OHOS::ERR_OK, result);
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->connectMap_.size());
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->serviceMap_.size());

    abilityMgrServ_->RemoveAllServiceRecord();
    curMissionStack->RemoveAll();

    testing::Mock::AllowLeak(stub);
    testing::Mock::AllowLeak(callback);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: OnStart/OnStop
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: OnStart/OnStop
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_004, TestSize.Level1)
{
    ASSERT_TRUE(abilityMgrServ_);
    ASSERT_TRUE(abilityMgrServ_->appScheduler_);
    // It's turned on during initialization, so it's turned off here
    abilityMgrServ_->OnStop();
    static sptr<IAppStateCallback> appStateCallbackHolder;

    auto state = abilityMgrServ_->QueryServiceState();
    EXPECT_EQ(state, ServiceRunningState::STATE_NOT_START);

    EXPECT_CALL(*mockAppMgrClient_, ConnectAppMgrService()).Times(1).WillOnce(Return(AppMgrResultCode::RESULT_OK));

    EXPECT_CALL(*mockAppMgrClient_, RegisterAppStateCallback(_))
        .Times(1)
        .WillOnce(Invoke([&](const sptr<IAppStateCallback> &callback) {
            appStateCallbackHolder = callback;
            return AppMgrResultCode::RESULT_OK;
        }));

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);

    abilityMgrServ_->OnStart();
    waitAMS();

    ASSERT_TRUE(abilityMgrServ_->dataAbilityManager_);
    state = abilityMgrServ_->QueryServiceState();
    EXPECT_EQ(state, ServiceRunningState::STATE_RUNNING);

    auto handler = abilityMgrServ_->GetEventHandler();
    ASSERT_TRUE(handler);
    auto eventRunner = handler->GetEventRunner();
    ASSERT_TRUE(eventRunner);
    EXPECT_TRUE(eventRunner->IsRunning());

    abilityMgrServ_->OnStop();

    state = abilityMgrServ_->QueryServiceState();
    EXPECT_EQ(state, ServiceRunningState::STATE_NOT_START);
    EXPECT_FALSE(abilityMgrServ_->GetEventHandler());
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: AddWindowInfo.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_005, TestSize.Level1)
{
    std::string abilityName = "MusicAbility";
    std::string bundleName = "com.ix.hiMusic";

    Want want = CreateWant(abilityName, bundleName);
    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);
    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVATING);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    mission->AddAbilityRecordToTop(abilityRecord);
    auto stackManager_ = abilityMgrServ_->GetStackManager();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);

    abilityMgrServ_->AddWindowInfo(abilityRecord->GetToken(), 1);
    EXPECT_TRUE(abilityRecord->GetWindowInfo() != nullptr);
    EXPECT_EQ((std::size_t)1, stackManager_->windowTokenToAbilityMap_.size());
    abilityRecord->RemoveWindowInfo();
    stackManager_->windowTokenToAbilityMap_.clear();
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: AttachAbilityThread.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_006, TestSize.Level1)
{
    ASSERT_TRUE(abilityMgrServ_);
    std::string abilityName = "MusicAbility";
    std::string bundleName = "com.ix.hiMusic";

    Want want = CreateWant(abilityName, bundleName);
    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);
    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVATING);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    ASSERT_TRUE(mission);
    mission->AddAbilityRecordToTop(abilityRecord);

    auto stackManager_ = abilityMgrServ_->GetStackManager();
    ASSERT_TRUE(stackManager_);
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);
    sptr<MockAbilityScheduler> scheduler(new MockAbilityScheduler());
    EXPECT_CALL(*scheduler, AsObject()).Times(4);
    abilityRecord->SetScheduler(scheduler);
    auto eventLoop = OHOS::AppExecFwk::EventRunner::Create("NAME_ABILITY_MGR_SERVICE");
    std::shared_ptr<AbilityEventHandler> handler = std::make_shared<AbilityEventHandler>(eventLoop, abilityMgrServ_);
    abilityMgrServ_->handler_ = handler;
    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(1).WillOnce(Return(AppMgrResultCode::RESULT_OK));
    abilityMgrServ_->AttachAbilityThread(scheduler, abilityRecord->GetToken());

    testing::Mock::AllowLeak(scheduler);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: AbilityTransitionDone.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_007, TestSize.Level1)
{
    std::string abilityName = "MusicAbility";
    std::string bundleName = "com.ix.hiMusic";

    Want want = CreateWant(abilityName, bundleName);
    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);
    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    ASSERT_TRUE(abilityRecord);
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVATING);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    ASSERT_TRUE(mission);
    mission->AddAbilityRecordToTop(abilityRecord);
    auto stackManager_ = abilityMgrServ_->GetStackManager();
    ASSERT_TRUE(stackManager_);
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    ASSERT_TRUE(curMissionStack);
    curMissionStack->AddMissionRecordToTop(mission);

    int result = abilityMgrServ_->AbilityTransitionDone(abilityRecord->GetToken(), OHOS::AAFwk::AbilityState::ACTIVE);
    usleep(50 * 1000);
    EXPECT_EQ(OHOS::ERR_OK, result);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::ACTIVE, abilityRecord->GetAbilityState());
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVATING);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: OnAbilityDied
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Test data ability client died.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_008, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ability_mgr_service_test_008  1";
    ASSERT_TRUE(abilityMgrServ_);

    abilityMgrServ_->dataAbilityManager_.reset(new DataAbilityManager());
    ASSERT_TRUE(abilityMgrServ_->dataAbilityManager_);

    // Data ability mock scheduler
    sptr<MockAbilityScheduler> mockDataAbilityScheduler = new MockAbilityScheduler();
    ASSERT_TRUE(mockDataAbilityScheduler);

    auto topAbility = GreatePageAbility("MusicAbility", "com.ix.hiMusic");
    ASSERT_TRUE(topAbility);

    bool testFailed = false;
    sptr<IRemoteObject> dataAbilityToken;

    // All mocks is setup, start test data ability acquiring & releasing...
    OHOS::Uri dataAbilityUri("dataability:///data.bundle.DataAbility");
    auto dataAbility = abilityMgrServ_->AcquireDataAbility(dataAbilityUri, true, topAbility->GetToken());
    EXPECT_TRUE(dataAbility);

    abilityMgrServ_->OnAbilityDied(topAbility);

    EXPECT_FALSE(testFailed);

    abilityMgrServ_->dataAbilityManager_.reset();

    // testing::Mock::AllowLeak(mockDataAbilityScheduler);
    // testing::Mock::AllowLeak(mockBundleMgr_);
    GTEST_LOG_(INFO) << "ability_mgr_service_test_008  1";
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: OnAbilityDied
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Test ability client died.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_010, TestSize.Level3)
{
    ASSERT_TRUE(abilityMgrServ_);

    abilityMgrServ_->dataAbilityManager_.reset(new DataAbilityManager());
    ASSERT_TRUE(abilityMgrServ_->dataAbilityManager_);

    // Data ability mock scheduler
    sptr<MockAbilityScheduler> mockDataAbilityScheduler = new MockAbilityScheduler();
    ASSERT_TRUE(mockDataAbilityScheduler);

    auto topAbility = GreatePageAbility("MusicAbility", "com.ix.music");
    ASSERT_TRUE(topAbility);

    bool testFailed = false;
    sptr<IRemoteObject> dataAbilityToken;

    // MOCK: data ability schedule ability transation handler
    MockAbilityTransitionDone(testFailed, dataAbilityToken, mockDataAbilityScheduler);

    // MOCK: data ability load handler
    MockDataAbilityLoadHandlerInner(testFailed, dataAbilityToken, mockDataAbilityScheduler);

    auto mockAbilityBehaviorAnaysis = [&](const sptr<IRemoteObject> &token,
                                          const sptr<IRemoteObject> &preToken,
                                          const int32_t visibility,
                                          const int32_t perceptibility,
                                          const int32_t connectionState) {
        testFailed = testFailed || (token != dataAbilityToken);
        testFailed = testFailed || (Token::GetAbilityRecordByToken(preToken) != topAbility);
        testFailed = testFailed || (visibility != 0);
        testFailed = testFailed || (perceptibility != 0);
        testFailed = testFailed || (connectionState != 1);
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, AbilityBehaviorAnalysis(_, _, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mockAbilityBehaviorAnaysis));

    // MOCK: bms query data ability by uri
    QueryDataAbilityByUriInner(testFailed);
    // All mocks is setup, start test data ability acquiring & releasing...

    OHOS::Uri dataAbilityUri("dataability:///data.bundle.DataAbility");
    auto dataAbility = abilityMgrServ_->AcquireDataAbility(dataAbilityUri, false, topAbility->GetToken());
    EXPECT_TRUE(dataAbility);

    auto dataAbilityRecord = abilityMgrServ_->dataAbilityManager_->GetAbilityRecordByScheduler(dataAbility);
    ASSERT_TRUE(dataAbilityRecord);

    abilityMgrServ_->OnAbilityDied(dataAbilityRecord);

    EXPECT_FALSE(testFailed);

    abilityMgrServ_->dataAbilityManager_.reset();

    testing::Mock::AllowLeak(mockDataAbilityScheduler);
    testing::Mock::AllowLeak(mockBundleMgr_);
}
#elif 0
/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: connect ability.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_011, TestSize.Level1)
{
    std::string abilityName = "MusicAbility";
    std::string bundleName = "com.ix.hiMusic";
    Want want;
    std::shared_ptr<MissionStack> curMissionStack;
    sptr<IRemoteObject> recordToken;
    CreateAbilityRequest(abilityName, bundleName, want, curMissionStack, recordToken);

    sptr<MockAbilityConnectCallbackStub> stub(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(stub));

    bool testResult = false;
    sptr<IRemoteObject> testToken;
    MockServiceAbilityLoadHandlerInner(testResult, bundleName, abilityName, testToken);

    int result = abilityMgrServ_->ConnectAbility(want, callback, recordToken);
    EXPECT_EQ(testResult, result);
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->connectMap_.size());
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->serviceMap_.size());
    std::shared_ptr<AbilityRecord> record = abilityMgrServ_->connectManager_->GetServiceRecordByToken(testToken);
    ASSERT_TRUE(record);
    ElementName element;
    element.SetAbilityName(abilityName + "_service");
    element.SetBundleName(bundleName + "_service");
    std::shared_ptr<AbilityRecord> record1 =
        abilityMgrServ_->connectManager_->GetServiceRecordByElementName(element.GetURI());
    ASSERT_TRUE(record1);
    std::shared_ptr<ConnectionRecord> connectRecord = record->GetConnectRecordList().front();
    ASSERT_TRUE(connectRecord);

    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(1);
    abilityMgrServ_->AttachAbilityThread(scheduler, record->GetToken());
    ASSERT_TRUE(record->isReady_);

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(1);
    abilityMgrServ_->OnAbilityRequestDone(
        record->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::INACTIVATING, record->GetAbilityState());

    EXPECT_CALL(*scheduler, ScheduleConnectAbility(_)).Times(1);
    abilityMgrServ_->AbilityTransitionDone(record->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INACTIVE);
    ASSERT_TRUE(record->GetConnectingRecord());
    EXPECT_EQ(OHOS::AAFwk::ConnectionState::CONNECTING, connectRecord->GetConnectState());

    EXPECT_CALL(*stub, OnAbilityConnectDone(_, _, _)).Times(1);
    abilityMgrServ_->ScheduleConnectAbilityDone(record->GetToken(), nullptr);

    EXPECT_EQ(OHOS::AAFwk::ConnectionState::CONNECTED, connectRecord->GetConnectState());
    EXPECT_EQ(OHOS::AAFwk::AbilityState::ACTIVE, record->GetAbilityState());

    abilityMgrServ_->RemoveAllServiceRecord();
    curMissionStack->RemoveAll();

    testing::Mock::AllowLeak(scheduler);
    testing::Mock::AllowLeak(stub);
    testing::Mock::AllowLeak(callback);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: disconnect ability.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_012, TestSize.Level1)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);
    sptr<MockAbilityConnectCallbackStub> stub(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(stub));

    abilityMgrServ_->RemoveAllServiceRecord();
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    abilityMgrServ_->ConnectAbility(want, callback, nullptr);
    std::shared_ptr<AbilityRecord> record =
        abilityMgrServ_->connectManager_->GetServiceRecordByElementName(want.GetElement().GetURI());
    ASSERT_TRUE(record);
    std::shared_ptr<ConnectionRecord> connectRecord = record->GetConnectingRecord();
    ASSERT_TRUE(connectRecord);
    connectRecord->SetConnectState(ConnectionState::CONNECTED);
    record->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    record->SetScheduler(scheduler);
    EXPECT_CALL(*scheduler, ScheduleDisconnectAbility(_)).Times(1);
    abilityMgrServ_->DisconnectAbility(callback);
    EXPECT_EQ(OHOS::AAFwk::ConnectionState::DISCONNECTING, connectRecord->GetConnectState());

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_CALL(*stub, OnAbilityDisconnectDone(_, _)).Times(1);
    abilityMgrServ_->ScheduleDisconnectAbilityDone(record->GetToken());
    EXPECT_EQ((std::size_t)0, record->GetConnectRecordList().size());
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetConnectMap().size());

    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(1);
    abilityMgrServ_->AbilityTransitionDone(record->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INITIAL);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::TERMINATING, record->GetAbilityState());

    EXPECT_CALL(*mockAppMgrClient_, TerminateAbility(_)).Times(1);
    abilityMgrServ_->OnAbilityRequestDone(
        record->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetServiceMap().size());

    testing::Mock::AllowLeak(scheduler);
    testing::Mock::AllowLeak(stub);
    testing::Mock::AllowLeak(callback);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: disconnect ability.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_013, TestSize.Level1)
{
    std::string abilityName1 = "MusicAbility_service_1";
    std::string bundleName1 = "com.ix.hiMusic_service_1";
    Want want1 = CreateWant(abilityName1, bundleName1);
    std::string abilityName2 = "MusicAbility_service_2";
    std::string bundleName2 = "com.ix.hiMusic_service_2";
    Want want2 = CreateWant(abilityName2, bundleName2);

    sptr<MockAbilityConnectCallbackStub> stub1(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback1(new AbilityConnectionProxy(stub1));
    sptr<MockAbilityConnectCallbackStub> stub2(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback2(new AbilityConnectionProxy(stub2));

    abilityMgrServ_->RemoveAllServiceRecord();

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(2);
    abilityMgrServ_->ConnectAbility(want1, callback1, nullptr);
    abilityMgrServ_->ConnectAbility(want2, callback1, nullptr);
    abilityMgrServ_->ConnectAbility(want1, callback2, nullptr);
    abilityMgrServ_->ConnectAbility(want2, callback2, nullptr);

    EXPECT_EQ((std::size_t)2, abilityMgrServ_->connectManager_->GetServiceMap().size());
    EXPECT_EQ((std::size_t)2, abilityMgrServ_->connectManager_->GetConnectMap().size());

    std::shared_ptr<AbilityRecord> record1;
    std::shared_ptr<AbilityRecord> record2;
    CreateServiceRecord(record1, want1, callback1, callback2);
    CreateServiceRecord(record2, want2, callback1, callback2);

    for (auto &connectRecord : record1->GetConnectRecordList()) {
        connectRecord->SetConnectState(ConnectionState::CONNECTED);
    }
    for (auto &connectRecord : record2->GetConnectRecordList()) {
        connectRecord->SetConnectState(ConnectionState::CONNECTED);
    }

    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(4);
    record1->SetScheduler(scheduler);
    record2->SetScheduler(scheduler);
    EXPECT_CALL(*stub1, OnAbilityDisconnectDone(_, _)).Times(2);
    abilityMgrServ_->DisconnectAbility(callback1);
    usleep(100);

    EXPECT_CALL(*scheduler, ScheduleDisconnectAbility(_)).Times(2);
    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(2);
    EXPECT_CALL(*stub2, OnAbilityDisconnectDone(_, _)).Times(2);

    CheckTestRecord(record1, record2, callback1, callback2);

    testing::Mock::AllowLeak(stub1);
    testing::Mock::AllowLeak(callback1);
    testing::Mock::AllowLeak(stub2);
    testing::Mock::AllowLeak(callback2);
    testing::Mock::AllowLeak(scheduler);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: start ability (serive ability).
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_014, TestSize.Level1)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);
    bool testResult = false;
    sptr<IRemoteObject> testToken;
    auto mockHandler = [&](const sptr<IRemoteObject> &token,
                           const sptr<IRemoteObject> &preToken,
                           const AbilityInfo &abilityInfo,
                           const ApplicationInfo &appInfo) {
        testToken = token;
        testResult = !!testToken && abilityInfo.bundleName == bundleName && abilityInfo.name == abilityName &&
                     appInfo.bundleName == bundleName;
        return AppMgrResultCode::RESULT_OK;
    };
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1).WillOnce(Invoke(mockHandler));

    int testRequestCode = 123;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    EXPECT_TRUE(testResult);
    EXPECT_TRUE(testToken);

    std::shared_ptr<AbilityRecord> record = abilityMgrServ_->GetServiceRecordByElementName(want.GetElement().GetURI());
    EXPECT_TRUE(record);
    EXPECT_FALSE(record->IsCreateByConnect());

    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(1);
    abilityMgrServ_->AttachAbilityThread(scheduler, record->GetToken());
    ASSERT_TRUE(record->isReady_);

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(1);
    abilityMgrServ_->OnAbilityRequestDone(
        record->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::INACTIVATING, record->GetAbilityState());

    int testId = 0;
    auto handler = [&](const Want &want, bool restart, int startid) { testId = startid; };
    EXPECT_CALL(*scheduler, ScheduleCommandAbility(_, _, _)).Times(1).WillOnce(Invoke(handler));
    abilityMgrServ_->AbilityTransitionDone(record->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INACTIVE);
    EXPECT_EQ(1, testId);

    abilityMgrServ_->ScheduleCommandAbilityDone(record->GetToken());
    EXPECT_EQ(OHOS::AAFwk::AbilityState::ACTIVE, record->GetAbilityState());

    EXPECT_CALL(*scheduler, ScheduleCommandAbility(_, _, _))
        .Times(3)
        .WillOnce(Invoke(handler))
        .WillOnce(Invoke(handler))
        .WillOnce(Invoke(handler));

    for (int i = 0; i < 3; ++i) {
        abilityMgrServ_->StartAbility(want, testRequestCode);
    }
    EXPECT_EQ(4, testId);
    abilityMgrServ_->RemoveAllServiceRecord();
    testing::Mock::AllowLeak(scheduler);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: start ability (serive ability).
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_015, TestSize.Level1)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);
    bool testResult = false;
    sptr<IRemoteObject> testToken;
    MockServiceAbilityLoadHandlerInner(testResult, bundleName, abilityName, testToken);

    abilityMgrServ_->RemoveAllServiceRecord();

    int testRequestCode = 123;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    EXPECT_TRUE(testResult);
    EXPECT_TRUE(testToken);

    std::shared_ptr<AbilityRecord> record = abilityMgrServ_->GetServiceRecordByElementName(want.GetElement().GetURI());
    EXPECT_TRUE(record);
    EXPECT_FALSE(record->IsCreateByConnect());

    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(1);
    abilityMgrServ_->AttachAbilityThread(scheduler, record->GetToken());
    ASSERT_TRUE(record->isReady_);

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(1);
    abilityMgrServ_->OnAbilityRequestDone(
        record->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::INACTIVATING, record->GetAbilityState());

    int testId = 0;
    auto handler = [&](const Want &want, bool restart, int startid) { testId = startid; };
    EXPECT_CALL(*scheduler, ScheduleCommandAbility(_, _, _)).Times(1).WillOnce(Invoke(handler));
    abilityMgrServ_->AbilityTransitionDone(record->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INACTIVE);
    EXPECT_EQ(1, testId);

    abilityMgrServ_->ScheduleCommandAbilityDone(record->GetToken());
    EXPECT_EQ(OHOS::AAFwk::AbilityState::ACTIVE, record->GetAbilityState());

    sptr<MockAbilityConnectCallbackStub> stub(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(stub));
    EXPECT_CALL(*scheduler, ScheduleConnectAbility(_)).Times(1);
    abilityMgrServ_->ConnectAbility(want, callback, nullptr);
    std::shared_ptr<ConnectionRecord> connectRecord = record->GetConnectingRecord();
    ASSERT_TRUE(connectRecord);
    EXPECT_EQ((size_t)1, abilityMgrServ_->GetConnectRecordListByCallback(callback).size());

    EXPECT_CALL(*stub, OnAbilityConnectDone(_, _, _)).Times(1);
    abilityMgrServ_->ScheduleConnectAbilityDone(record->GetToken(), nullptr);
    EXPECT_EQ(ConnectionState::CONNECTED, connectRecord->GetConnectState());

    abilityMgrServ_->RemoveAllServiceRecord();

    testing::Mock::AllowLeak(scheduler);
    testing::Mock::AllowLeak(stub);
    testing::Mock::AllowLeak(callback);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: start ability (serive ability).
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_016, TestSize.Level1)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);

    abilityMgrServ_->RemoveAllServiceRecord();

    sptr<MockAbilityConnectCallbackStub> stub(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(stub));
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    abilityMgrServ_->ConnectAbility(want, callback, nullptr);
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->connectMap_.size());
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->serviceMap_.size());
    std::shared_ptr<AbilityRecord> record =
        abilityMgrServ_->connectManager_->GetServiceRecordByElementName(want.GetElement().GetURI());
    ASSERT_TRUE(record);
    ASSERT_TRUE(record->IsCreateByConnect());

    std::shared_ptr<ConnectionRecord> connectRecord = record->GetConnectRecordList().front();
    ASSERT_TRUE(connectRecord);
    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(1);
    abilityMgrServ_->AttachAbilityThread(scheduler, record->GetToken());
    ASSERT_TRUE(record->isReady_);

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(1);
    abilityMgrServ_->OnAbilityRequestDone(
        record->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::INACTIVATING, record->GetAbilityState());
    EXPECT_CALL(*scheduler, ScheduleConnectAbility(_)).Times(1);
    abilityMgrServ_->AbilityTransitionDone(record->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INACTIVE);
    ASSERT_TRUE(record->GetConnectingRecord());
    EXPECT_EQ(OHOS::AAFwk::ConnectionState::CONNECTING, connectRecord->GetConnectState());
    EXPECT_CALL(*stub, OnAbilityConnectDone(_, _, _)).Times(1);
    abilityMgrServ_->ScheduleConnectAbilityDone(record->GetToken(), nullptr);

    EXPECT_EQ(OHOS::AAFwk::ConnectionState::CONNECTED, connectRecord->GetConnectState());
    EXPECT_EQ(OHOS::AAFwk::AbilityState::ACTIVE, record->GetAbilityState());

    int testId = 0;
    MockLoadHandlerInner(testId, scheduler);

    for (int i = 0; i < 3; ++i) {
        abilityMgrServ_->StartAbility(want, 123);
    }
    EXPECT_EQ(3, testId);
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->GetServiceMap().size());
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->GetConnectMap().size());

    abilityMgrServ_->RemoveAllServiceRecord();

    testing::Mock::AllowLeak(scheduler);
    testing::Mock::AllowLeak(stub);
    testing::Mock::AllowLeak(callback);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: start ability (serive ability).
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_017, TestSize.Level1)
{
    std::string abilityName1 = "MusicAbility_service_1";
    std::string bundleName1 = "com.ix.hiMusic_service_1";
    Want want1 = CreateWant(abilityName1, bundleName1);
    std::string abilityName2 = "MusicAbility_service_2";
    std::string bundleName2 = "com.ix.hiMusic_service_2";
    Want want2 = CreateWant(abilityName2, bundleName2);

    abilityMgrServ_->RemoveAllServiceRecord();
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(2);
    int testRequestCode = 123;
    abilityMgrServ_->StartAbility(want1, testRequestCode);
    abilityMgrServ_->StartAbility(want2, testRequestCode);

    std::shared_ptr<AbilityRecord> record1 =
        abilityMgrServ_->GetServiceRecordByElementName(want1.GetElement().GetURI());
    EXPECT_TRUE(record1);
    EXPECT_FALSE(record1->IsCreateByConnect());

    std::shared_ptr<AbilityRecord> record2 =
        abilityMgrServ_->GetServiceRecordByElementName(want2.GetElement().GetURI());
    EXPECT_TRUE(record2);
    EXPECT_FALSE(record2->IsCreateByConnect());

    abilityMgrServ_->RemoveAllServiceRecord();
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: TerminateAbility  (serive ability).
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_018, TestSize.Level1)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);

    abilityMgrServ_->RemoveAllServiceRecord();
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    int testRequestCode = 123;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    std::shared_ptr<AbilityRecord> record = abilityMgrServ_->GetServiceRecordByElementName(want.GetElement().GetURI());
    EXPECT_TRUE(record);
    EXPECT_FALSE(record->IsCreateByConnect());
    record->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    record->SetScheduler(scheduler);

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(1);
    abilityMgrServ_->TerminateAbility(record->GetToken(), -1);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::TERMINATING, record->GetAbilityState());

    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(1);
    abilityMgrServ_->AbilityTransitionDone(record->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INITIAL);

    EXPECT_CALL(*mockAppMgrClient_, TerminateAbility(_)).Times(1);
    abilityMgrServ_->OnAbilityRequestDone(
        record->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetServiceMap().size());

    abilityMgrServ_->RemoveAllServiceRecord();

    testing::Mock::AllowLeak(scheduler);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: TerminateAbility  (serive ability).
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_019, TestSize.Level1)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);

    abilityMgrServ_->RemoveAllServiceRecord();
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    int testRequestCode = 123;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    std::shared_ptr<AbilityRecord> record = abilityMgrServ_->GetServiceRecordByElementName(want.GetElement().GetURI());
    EXPECT_TRUE(record);
    EXPECT_FALSE(record->IsCreateByConnect());
    record->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    record->SetScheduler(scheduler);

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(1);
    abilityMgrServ_->StopServiceAbility(want);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::TERMINATING, record->GetAbilityState());

    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(1);
    abilityMgrServ_->AbilityTransitionDone(record->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INITIAL);

    EXPECT_CALL(*mockAppMgrClient_, TerminateAbility(_)).Times(1);
    abilityMgrServ_->OnAbilityRequestDone(
        record->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetServiceMap().size());

    abilityMgrServ_->RemoveAllServiceRecord();

    testing::Mock::AllowLeak(scheduler);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: StopServiceAbility  (serive ability).
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_020, TestSize.Level1)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);

    abilityMgrServ_->RemoveAllServiceRecord();
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    int testRequestCode = 123;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    std::shared_ptr<AbilityRecord> record = abilityMgrServ_->GetServiceRecordByElementName(want.GetElement().GetURI());
    EXPECT_TRUE(record);
    EXPECT_FALSE(record->IsCreateByConnect());
    record->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    record->SetScheduler(scheduler);

    sptr<MockAbilityConnectCallbackStub> stub(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(stub));
    EXPECT_CALL(*scheduler, ScheduleConnectAbility(_)).Times(1);
    abilityMgrServ_->ConnectAbility(want, callback, nullptr);
    std::shared_ptr<ConnectionRecord> connectRecord = record->GetConnectingRecord();
    ASSERT_TRUE(connectRecord);
    EXPECT_EQ((size_t)1, abilityMgrServ_->GetConnectRecordListByCallback(callback).size());
    EXPECT_CALL(*stub, OnAbilityConnectDone(_, _, _)).Times(1);
    abilityMgrServ_->ScheduleConnectAbilityDone(record->GetToken(), nullptr);
    EXPECT_EQ(ConnectionState::CONNECTED, connectRecord->GetConnectState());

    int result = abilityMgrServ_->TerminateAbility(record->GetToken(), -1);
    EXPECT_EQ(TERMINATE_SERVICE_IS_CONNECTED, result);

    abilityMgrServ_->RemoveAllServiceRecord();

    testing::Mock::AllowLeak(scheduler);
    testing::Mock::AllowLeak(stub);
    testing::Mock::AllowLeak(callback);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: StopServiceAbility  (serive ability).
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_021, TestSize.Level1)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);

    abilityMgrServ_->RemoveAllServiceRecord();
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    int testRequestCode = 123;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    std::shared_ptr<AbilityRecord> record = abilityMgrServ_->GetServiceRecordByElementName(want.GetElement().GetURI());
    EXPECT_TRUE(record);
    EXPECT_FALSE(record->IsCreateByConnect());
    record->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    record->SetScheduler(scheduler);

    sptr<MockAbilityConnectCallbackStub> stub(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(stub));
    EXPECT_CALL(*scheduler, ScheduleConnectAbility(_)).Times(1);
    abilityMgrServ_->ConnectAbility(want, callback, nullptr);
    std::shared_ptr<ConnectionRecord> connectRecord = record->GetConnectingRecord();
    ASSERT_TRUE(connectRecord);
    EXPECT_EQ((size_t)1, abilityMgrServ_->GetConnectRecordListByCallback(callback).size());
    EXPECT_CALL(*stub, OnAbilityConnectDone(_, _, _)).Times(1);
    abilityMgrServ_->ScheduleConnectAbilityDone(record->GetToken(), nullptr);
    EXPECT_EQ(ConnectionState::CONNECTED, connectRecord->GetConnectState());

    int result = abilityMgrServ_->StopServiceAbility(want);
    EXPECT_EQ(TERMINATE_SERVICE_IS_CONNECTED, result);

    abilityMgrServ_->RemoveAllServiceRecord();

    testing::Mock::AllowLeak(scheduler);
    testing::Mock::AllowLeak(stub);
    testing::Mock::AllowLeak(callback);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: disconnectAbility and stop service ability (serive ability).
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_022, TestSize.Level1)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);

    abilityMgrServ_->RemoveAllServiceRecord();
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    int testRequestCode = 123;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    std::shared_ptr<AbilityRecord> record = abilityMgrServ_->GetServiceRecordByElementName(want.GetElement().GetURI());
    EXPECT_TRUE(record);
    EXPECT_FALSE(record->IsCreateByConnect());
    record->AddStartId();
    record->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    record->SetScheduler(scheduler);

    sptr<MockAbilityConnectCallbackStub> stub(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(stub));
    EXPECT_CALL(*scheduler, ScheduleConnectAbility(_)).Times(1);
    abilityMgrServ_->ConnectAbility(want, callback, nullptr);
    std::shared_ptr<ConnectionRecord> connectRecord = record->GetConnectingRecord();
    ASSERT_TRUE(connectRecord);
    EXPECT_EQ((size_t)1, abilityMgrServ_->GetConnectRecordListByCallback(callback).size());
    EXPECT_CALL(*stub, OnAbilityConnectDone(_, _, _)).Times(1);
    abilityMgrServ_->ScheduleConnectAbilityDone(record->GetToken(), nullptr);
    EXPECT_EQ(ConnectionState::CONNECTED, connectRecord->GetConnectState());

    EXPECT_CALL(*scheduler, ScheduleDisconnectAbility(_)).Times(1);
    abilityMgrServ_->DisconnectAbility(callback);
    EXPECT_EQ(OHOS::AAFwk::ConnectionState::DISCONNECTING, connectRecord->GetConnectState());

    EXPECT_CALL(*stub, OnAbilityDisconnectDone(_, _)).Times(1);
    abilityMgrServ_->ScheduleDisconnectAbilityDone(record->GetToken());
    EXPECT_EQ((std::size_t)0, record->GetConnectRecordList().size());
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetConnectMap().size());
    EXPECT_EQ(OHOS::AAFwk::ConnectionState::DISCONNECTED, connectRecord->GetConnectState());
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->GetServiceMap().size());

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(1);
    abilityMgrServ_->StopServiceAbility(want);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::TERMINATING, record->GetAbilityState());

    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(1);
    abilityMgrServ_->AbilityTransitionDone(record->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INITIAL);
    EXPECT_CALL(*mockAppMgrClient_, TerminateAbility(_)).Times(1);
    abilityMgrServ_->OnAbilityRequestDone(
        record->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetServiceMap().size());

    abilityMgrServ_->RemoveAllServiceRecord();

    testing::Mock::AllowLeak(scheduler);
    testing::Mock::AllowLeak(stub);
    testing::Mock::AllowLeak(callback);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: disconnectAbility and terminate (serive ability).
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_023, TestSize.Level1)
{
    std::string abilityName = "MusicAbilityService";
    std::string bundleName = "com.ix.hiservice";
    Want want = CreateWant(abilityName, bundleName);
    ASSERT_TRUE(abilityMgrServ_);
    abilityMgrServ_->RemoveAllServiceRecord();
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    int testRequestCode = 123;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    std::shared_ptr<AbilityRecord> record = abilityMgrServ_->GetServiceRecordByElementName(want.GetElement().GetURI());
    EXPECT_TRUE(record);
    EXPECT_FALSE(record->IsCreateByConnect());
    record->AddStartId();
    record->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    record->SetScheduler(scheduler);
    sptr<MockAbilityConnectCallbackStub> stub(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(stub));
    EXPECT_CALL(*scheduler, ScheduleConnectAbility(_)).Times(1);
    abilityMgrServ_->ConnectAbility(want, callback, nullptr);
    std::shared_ptr<ConnectionRecord> connectRecord = record->GetConnectingRecord();
    ASSERT_TRUE(connectRecord);
    EXPECT_EQ((size_t)1, abilityMgrServ_->GetConnectRecordListByCallback(callback).size());
    EXPECT_CALL(*stub, OnAbilityConnectDone(_, _, _)).Times(1);
    abilityMgrServ_->ScheduleConnectAbilityDone(record->GetToken(), nullptr);
    EXPECT_EQ(ConnectionState::CONNECTED, connectRecord->GetConnectState());

    EXPECT_CALL(*scheduler, ScheduleDisconnectAbility(_)).Times(1);
    abilityMgrServ_->DisconnectAbility(callback);
    EXPECT_EQ(OHOS::AAFwk::ConnectionState::DISCONNECTING, connectRecord->GetConnectState());

    EXPECT_CALL(*stub, OnAbilityDisconnectDone(_, _)).Times(1);
    abilityMgrServ_->ScheduleDisconnectAbilityDone(record->GetToken());
    EXPECT_EQ((std::size_t)0, record->GetConnectRecordList().size());
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetConnectMap().size());
    EXPECT_EQ(OHOS::AAFwk::ConnectionState::DISCONNECTED, connectRecord->GetConnectState());
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->GetServiceMap().size());

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(1);
    abilityMgrServ_->TerminateAbility(record->GetToken(), -1);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::TERMINATING, record->GetAbilityState());

    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(1);
    abilityMgrServ_->AbilityTransitionDone(record->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INITIAL);
    EXPECT_CALL(*mockAppMgrClient_, TerminateAbility(_)).Times(1);
    abilityMgrServ_->OnAbilityRequestDone(
        record->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetServiceMap().size());

    abilityMgrServ_->RemoveAllServiceRecord();
    testing::Mock::AllowLeak(scheduler);
    testing::Mock::AllowLeak(stub);
    testing::Mock::AllowLeak(callback);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: Mission
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Checking ams mission management.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_024, TestSize.Level3)
{
    constexpr bool DEBUG = false;

    ASSERT_TRUE(abilityMgrServ_);

    struct ScopedLooper {
        std::shared_ptr<AbilityManagerService> ams_;
        std::shared_ptr<AbilityEventHandler> handlerBackup_;
        std::shared_ptr<EventRunner> eventRunner_;

        explicit ScopedLooper(std::shared_ptr<AbilityManagerService> ams) : ams_(ams), handlerBackup_(ams->handler_)
        {
            auto eventRunner = EventRunner::Create("AMSTestEventRunner");
            if (eventRunner) {
                ams_->handler_ = std::make_shared<AbilityEventHandler>(eventRunner, ams);
            }
        }

        ~ScopedLooper()
        {
            ams_->handler_ = handlerBackup_;
        }
    };

    ScopedLooper looper(abilityMgrServ_);
    Semaphore loadDone(0);

    // Test abilities.

    sptr<MockAbilityScheduler> mockAbilitySchedulers[2];
    sptr<IRemoteObject> tokens[2];
    std::shared_ptr<AbilityRecord> abilityRecords[2];

    mockAbilitySchedulers[0] = new MockAbilityScheduler();
    ASSERT_TRUE(mockAbilitySchedulers[0]);

    mockAbilitySchedulers[1] = new MockAbilityScheduler();
    ASSERT_TRUE(mockAbilitySchedulers[1]);

    auto mockAbilityTransaction = [&](int i, const Want &want, const LifeCycleStateInfo &targetState) {
        std::thread(
            [&](sptr<IRemoteObject> token, int state) {
                abilityMgrServ_->AbilityTransitionDone(token, state);
                if (state == ABILITY_STATE_ACTIVE) {
                    loadDone.Post();
                }
            },
            tokens[i],
            targetState.state)
            .detach();
    };

    EXPECT_CALL(*mockAbilitySchedulers[0], ScheduleAbilityTransaction(_, _))
        .Times(4)
        .WillRepeatedly(Invoke(std::bind(mockAbilityTransaction, 0, _1, _2)));

    EXPECT_CALL(*mockAbilitySchedulers[1], ScheduleAbilityTransaction(_, _))
        .Times(6)
        .WillRepeatedly(Invoke(std::bind(mockAbilityTransaction, 1, _1, _2)));

    EXPECT_CALL(*mockAbilitySchedulers[0], AsObject()).Times(1);
    EXPECT_CALL(*mockAbilitySchedulers[1], AsObject()).Times(1);

    testing::Mock::AllowLeak(mockAbilitySchedulers[0]);
    testing::Mock::AllowLeak(mockAbilitySchedulers[1]);

    // MOCK: AppMgrClient
    ASSERT_TRUE(mockAppMgrClient_);
    auto mockLoadAbility = [&](int i,
                               const sptr<IRemoteObject> &token,
                               const sptr<IRemoteObject> &preToken,
                               const AbilityInfo &abilityInfo,
                               const ApplicationInfo &appInfo) {
        abilityRecords[i] = Token::GetAbilityRecordByToken(token);
        tokens[i] = token;
        std::thread([&, i, token] { abilityMgrServ_->AttachAbilityThread(mockAbilitySchedulers[i], token); }).detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(std::bind(mockLoadAbility, 0, _1, _2, _3, _4)))
        .WillOnce(Invoke(std::bind(mockLoadAbility, 1, _1, _2, _3, _4)));

    auto mockUpdateAbilityState = [&](const sptr<IRemoteObject> &token, const AppExecFwk::AbilityState state) {
        std::thread([&, token, state] { abilityMgrServ_->OnAbilityRequestDone(token, int32_t(state)); }).detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(4).WillRepeatedly(Invoke(mockUpdateAbilityState));

    EXPECT_CALL(*mockAppMgrClient_, TerminateAbility).Times(1).WillOnce(Return(AppMgrResultCode::RESULT_OK));

    // All mocks have setup, launch the test...

    int ret;
    std::vector<RecentMissionInfo> missions;
    int missionId1;
    int missionId2;

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Starting the mission1...";
    }

    Want want1;
    want1.SetElementName("test.bundle1", "TestAbility1");
    ret = abilityMgrServ_->StartAbility(want1);
    ASSERT_EQ(ret, ERR_OK);
    loadDone.Wait();

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Checking state...";
    }

    missions.clear();
    ret = abilityMgrServ_->GetRecentMissions(100, RECENT_WITH_EXCLUDED, missions);
    ASSERT_EQ(ret, ERR_OK);
    ASSERT_EQ(missions.size(), size_t(1));
    missionId1 = missions[0].id;

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Starting the mission2...";
    }

    Want want2;
    want2.SetElementName("test.bundle2.singleton", "TestAbility2");
    ret = abilityMgrServ_->StartAbility(want2);
    ASSERT_EQ(ret, ERR_OK);
    loadDone.Wait();

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Checking state...";
    }

    missions.clear();
    ret = abilityMgrServ_->GetRecentMissions(100, RECENT_WITH_EXCLUDED, missions);
    ASSERT_EQ(ret, ERR_OK);
    ASSERT_EQ(missions.size(), size_t(2));
    ASSERT_EQ(missionId1, missions[1].id);
    missionId2 = missions[0].id;
    ASSERT_NE(missionId1, missionId2);

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Moving mission2 to top... (mission2 already on top.)";
    }

    ret = abilityMgrServ_->MoveMissionToTop(missionId2);
    ASSERT_EQ(ret, ERR_OK);

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Checking state...";
    }

    missions.clear();
    ret = abilityMgrServ_->GetRecentMissions(100, RECENT_WITH_EXCLUDED, missions);
    ASSERT_EQ(ret, ERR_OK);
    ASSERT_EQ(missions.size(), size_t(2));
    ASSERT_EQ(missions[0].id, missionId2);
    ASSERT_EQ(missions[1].id, missionId1);

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Moving mission1 to top...";
    }

    ret = abilityMgrServ_->MoveMissionToTop(missionId1);
    ASSERT_EQ(ret, ERR_OK);
    usleep(200 * 1000);

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Checking state...";
    }

    missions.clear();
    ret = abilityMgrServ_->GetRecentMissions(100, RECENT_WITH_EXCLUDED, missions);
    ASSERT_EQ(ret, ERR_OK);
    ASSERT_EQ(missions.size(), size_t(2));
    ASSERT_EQ(missions[0].id, missionId1);
    ASSERT_EQ(missions[1].id, missionId2);

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Removing mission2...";
    }

    ret = abilityMgrServ_->RemoveMission(missionId2);
    ASSERT_EQ(ret, ERR_OK);
    usleep(100 * 1000);

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Checking state...";
    }

    missions.clear();
    ret = abilityMgrServ_->GetRecentMissions(100, RECENT_WITH_EXCLUDED, missions);
    ASSERT_EQ(ret, ERR_OK);
    ASSERT_EQ(missions.size(), size_t(1));

    testing::Mock::AllowLeak(mockAbilitySchedulers[0]);
    testing::Mock::AllowLeak(mockAbilitySchedulers[1]);

    mockAbilitySchedulers[0].clear();
    mockAbilitySchedulers[1].clear();

    abilityRecords[0]->scheduler_.clear();
    abilityRecords[1]->scheduler_.clear();

    abilityRecords[0].reset();
    abilityRecords[1].reset();

    tokens[0].clear();
    tokens[1].clear();

    testing::Mock::AllowLeak(mockAbilitySchedulers[0]);
    testing::Mock::AllowLeak(mockAbilitySchedulers[1]);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: OnAbilityDied
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Test ability death notification.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_025, TestSize.Level2)
{

    //简化这个， 有卡死的情况
    constexpr bool DEBUG = false;

    ASSERT_TRUE(abilityMgrServ_);
    abilityMgrServ_->stackManagers_.clear();
    abilityMgrServ_->SetStackManager(0);

    struct ScopedLooper {
        std::shared_ptr<AbilityManagerService> ams_;
        std::shared_ptr<AbilityEventHandler> handlerBackup_;
        std::shared_ptr<EventRunner> eventRunner_;

        ScopedLooper(std::shared_ptr<AbilityManagerService> ams) : ams_(ams), handlerBackup_(ams->handler_)
        {
            auto eventRunner = EventRunner::Create("AMSTestEventRunner");
            if (eventRunner) {
                ams_->handler_ = std::make_shared<AbilityEventHandler>(eventRunner, ams);
                if (ams_->connectManager_) {
                    ams_->connectManager_->SetEventHandler(ams_->handler_);
                }
            }
        }

        ~ScopedLooper()
        {
            ams_->handler_ = handlerBackup_;
        }
    };

    ScopedLooper looper(abilityMgrServ_);
    Semaphore loadDone(0);

    // Test abilities.

    sptr<MockAbilityScheduler> mockAbilitySchedulers[3];
    sptr<IRemoteObject> tokens[3];
    std::shared_ptr<AbilityRecord> abilityRecords[3];

    mockAbilitySchedulers[0] = new MockAbilityScheduler();
    ASSERT_TRUE(mockAbilitySchedulers[0]);

    mockAbilitySchedulers[1] = new MockAbilityScheduler();
    ASSERT_TRUE(mockAbilitySchedulers[1]);

    mockAbilitySchedulers[2] = new MockAbilityScheduler();
    ASSERT_TRUE(mockAbilitySchedulers[2]);

    auto mockAbilityTransaction = [&](int i, const Want &want, const LifeCycleStateInfo &targetState) {
        std::thread(
            [&](int i, sptr<IRemoteObject> token, int state) {
                abilityMgrServ_->AbilityTransitionDone(token, state);
                switch (i) {
                    case 0:
                        if (state == ABILITY_STATE_ACTIVE) {
                            loadDone.Post();
                        }
                        break;
                    case 1:
                        if (state == ABILITY_STATE_INACTIVE) {
                            loadDone.Post();
                        }
                        break;
                    case 2:
                        break;
                    default:
                        break;
                }
            },
            i,
            tokens[i],
            targetState.state)
            .detach();
    };

    EXPECT_CALL(*mockAbilitySchedulers[0], ScheduleAbilityTransaction(_, _))
        .Times(1)
        .WillRepeatedly(Invoke(std::bind(mockAbilityTransaction, 0, _1, _2)));

    EXPECT_CALL(*mockAbilitySchedulers[1], ScheduleAbilityTransaction(_, _))
        .Times(1)
        .WillRepeatedly(Invoke(std::bind(mockAbilityTransaction, 1, _1, _2)));

    EXPECT_CALL(*mockAbilitySchedulers[2], ScheduleAbilityTransaction(_, _))
        .Times(1)
        .WillRepeatedly(Invoke(std::bind(mockAbilityTransaction, 2, _1, _2)));

    auto mockScheduleCommandAbility = [&](const Want &want, bool restart, int startId) {
        abilityMgrServ_->ScheduleCommandAbilityDone(tokens[1]);
    };

    EXPECT_CALL(*mockAbilitySchedulers[1], ScheduleCommandAbility(_, _, _))
        .Times(1)
        .WillOnce(Invoke(mockScheduleCommandAbility));

    EXPECT_CALL(*mockAbilitySchedulers[0], AsObject()).Times(1);
    EXPECT_CALL(*mockAbilitySchedulers[1], AsObject()).Times(1);
    EXPECT_CALL(*mockAbilitySchedulers[2], AsObject()).Times(1);

    // MOCK: AppMgrClient
    ASSERT_TRUE(mockAppMgrClient_);
    auto mockLoadAbility = [&](int i,
                               const sptr<IRemoteObject> &token,
                               const sptr<IRemoteObject> &preToken,
                               const AbilityInfo &abilityInfo,
                               const ApplicationInfo &appInfo) {
        abilityRecords[i] = Token::GetAbilityRecordByToken(token);
        tokens[i] = token;
        std::thread([&, i, token] {
            abilityMgrServ_->AttachAbilityThread(mockAbilitySchedulers[i], token);
            abilityMgrServ_->OnAbilityRequestDone(token, int32_t(AAFwk::AppAbilityState::ABILITY_STATE_FOREGROUND));
        }).detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _))
        .Times(3)
        .WillOnce(Invoke(std::bind(mockLoadAbility, 0, _1, _2, _3, _4)))
        .WillOnce(Invoke(std::bind(mockLoadAbility, 1, _1, _2, _3, _4)))
        .WillOnce(Invoke(std::bind(mockLoadAbility, 2, _1, _2, _3, _4)));

    auto mockUpdateAbilityState = [&](const sptr<IRemoteObject> &token, const AppExecFwk::AbilityState state) {
        std::thread([&, token, state] { abilityMgrServ_->OnAbilityRequestDone(token, int32_t(state)); }).detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, AbilityBehaviorAnalysis(_, _, _, _, _)).Times(2);
    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(2).WillRepeatedly(Invoke(mockUpdateAbilityState));

    // MOCK: BMS
    auto mockQueryAbilityInfoByUri = [&](const std::string &uri, AbilityInfo &abilityInfo) {
        abilityInfo.type = AppExecFwk::AbilityType::DATA;
        abilityInfo.name = "DataAbility";
        abilityInfo.bundleName = "com.ix.hiData";
        abilityInfo.applicationInfo.name = "data.app";
        abilityInfo.applicationInfo.bundleName = "com.ix.hiData";
        return true;
    };

    EXPECT_CALL(*mockBundleMgr_, QueryAbilityInfoByUri).Times(1).WillOnce(Invoke(mockQueryAbilityInfoByUri));
    // All mocks have setup, launch the test...
    int ret;
    if (DEBUG) {
        GTEST_LOG_(INFO) << "Starting ability1 (PAGE)...";
    }

    Want want1;
    want1.SetElementName("com.ix.hiMusic", "TestPageAbility1");
    ret = abilityMgrServ_->StartAbility(want1);
    ASSERT_EQ(ret, ERR_OK);
    loadDone.Wait();
    ASSERT_TRUE(tokens[0]);

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Starting ability2 (SERVICE)...";
    }

    Want want2;
    want2.SetElementName("com.ix.hiMusic", "TestServiceAbility2");
    ret = abilityMgrServ_->StartAbility(want2);
    ASSERT_EQ(ret, ERR_OK);
    loadDone.Wait();
    ASSERT_TRUE(tokens[1]);

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Starting ability3 (DATA)...";
    }

    abilityMgrServ_->dataAbilityManager_.reset(new DataAbilityManager);
    ASSERT_TRUE(abilityMgrServ_->dataAbilityManager_);

    Want want3;
    want3.SetElementName("com.ix.hiData", "TestDataAbility2");
    auto dataAbility =
        abilityMgrServ_->AcquireDataAbility(Uri("dataability:///data.bundle.DataAbility"), false, tokens[0]);
    ASSERT_TRUE(dataAbility);
    ASSERT_TRUE(tokens[2]);
    dataAbility.clear();

    if (DEBUG) {
        GTEST_LOG_(INFO) << "ability1 died";
    }

    abilityMgrServ_->OnAbilityDied(abilityRecords[0]);
    usleep(100 * 1000);

    if (DEBUG) {
        GTEST_LOG_(INFO) << "ability2 died";
    }

    abilityMgrServ_->OnAbilityDied(abilityRecords[1]);
    usleep(100 * 1000);

    if (DEBUG) {
        GTEST_LOG_(INFO) << "ability3 died";
    }

    abilityMgrServ_->OnAbilityDied(abilityRecords[2]);
    usleep(100 * 1000);

    EXPECT_EQ(abilityRecords[0].use_count(), 2);  // bottom page ability should not destroy.
    EXPECT_EQ(abilityRecords[0]->GetAbilityState(), INITIAL);

    EXPECT_EQ(abilityRecords[1].use_count(), 1);
    EXPECT_EQ(abilityRecords[2].use_count(), 1);

    testing::Mock::AllowLeak(mockAbilitySchedulers[0]);
    testing::Mock::AllowLeak(mockAbilitySchedulers[1]);
    testing::Mock::AllowLeak(mockAbilitySchedulers[2]);

    abilityRecords[0]->scheduler_.clear();
    abilityRecords[1]->scheduler_.clear();
    abilityRecords[2]->scheduler_.clear();

    abilityRecords[0].reset();
    abilityRecords[1].reset();
    abilityRecords[2].reset();

    abilityMgrServ_->stackManagers_.clear();
    abilityMgrServ_->currentStackManager_.reset();

    testing::Mock::AllowLeak(mockBundleMgr_);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: OnAbilityDied
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Test page abilities death notification.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_026, TestSize.Level3)
{
    constexpr bool DEBUG = false;

    ASSERT_TRUE(abilityMgrServ_);
    abilityMgrServ_->stackManagers_.clear();
    abilityMgrServ_->SetStackManager(0);

    struct ScopedLooper {
        std::shared_ptr<AbilityManagerService> ams_;
        std::shared_ptr<AbilityEventHandler> handlerBackup_;
        std::shared_ptr<EventRunner> eventRunner_;

        ScopedLooper(std::shared_ptr<AbilityManagerService> ams) : ams_(ams), handlerBackup_(ams->handler_)
        {
            auto eventRunner = EventRunner::Create("AMSTestEventRunner");
            if (eventRunner) {
                ams_->handler_ = std::make_shared<AbilityEventHandler>(eventRunner, ams);
            }
        }

        ~ScopedLooper()
        {
            ams_->handler_ = handlerBackup_;
        }
    };

    ScopedLooper looper(abilityMgrServ_);
    Semaphore loadDone(0);

    // Test abilities.

    sptr<MockAbilityScheduler> mockAbilitySchedulers[2];
    sptr<IRemoteObject> tokens[2];
    std::shared_ptr<AbilityRecord> abilityRecords[2];

    mockAbilitySchedulers[0] = new MockAbilityScheduler();
    ASSERT_TRUE(mockAbilitySchedulers[0]);

    mockAbilitySchedulers[1] = new MockAbilityScheduler();
    ASSERT_TRUE(mockAbilitySchedulers[1]);

    auto mockAbilityTransaction = [&](int i, const Want &want, const LifeCycleStateInfo &targetState) {
        std::thread(
            [&](int i, sptr<IRemoteObject> token, int state) {
                abilityMgrServ_->AbilityTransitionDone(token, state);
                if (state == ABILITY_STATE_ACTIVE) {
                    loadDone.Post();
                }
            },
            i,
            tokens[i],
            targetState.state)
            .detach();
    };

    EXPECT_CALL(*mockAbilitySchedulers[0], ScheduleAbilityTransaction(_, _))
        .Times(3)
        .WillRepeatedly(Invoke(std::bind(mockAbilityTransaction, 0, _1, _2)));

    EXPECT_CALL(*mockAbilitySchedulers[1], ScheduleAbilityTransaction(_, _))
        .Times(1)
        .WillRepeatedly(Invoke(std::bind(mockAbilityTransaction, 1, _1, _2)));

    EXPECT_CALL(*mockAbilitySchedulers[0], AsObject()).Times(1);
    EXPECT_CALL(*mockAbilitySchedulers[1], AsObject()).Times(1);

    // MOCK: AppMgrClient

    ASSERT_TRUE(mockAppMgrClient_);

    auto mockLoadAbility = [&](int i,
                               const sptr<IRemoteObject> &token,
                               const sptr<IRemoteObject> &preToken,
                               const AbilityInfo &abilityInfo,
                               const ApplicationInfo &appInfo) {
        abilityRecords[i] = Token::GetAbilityRecordByToken(token);
        tokens[i] = token;
        std::thread([&, i, token] {
            abilityMgrServ_->AttachAbilityThread(mockAbilitySchedulers[i], token);
            abilityMgrServ_->OnAbilityRequestDone(token, int32_t(AAFwk::AppAbilityState::ABILITY_STATE_FOREGROUND));
        }).detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(std::bind(mockLoadAbility, 0, _1, _2, _3, _4)))
        .WillOnce(Invoke(std::bind(mockLoadAbility, 1, _1, _2, _3, _4)));

    auto mockUpdateAbilityState = [&](const sptr<IRemoteObject> &token, const AppExecFwk::AbilityState state) {
        std::thread([&, token, state] { abilityMgrServ_->OnAbilityRequestDone(token, int32_t(state)); }).detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(3).WillRepeatedly(Invoke(mockUpdateAbilityState));

    // All mocks have setup, launch the test...

    int ret;
    std::vector<RecentMissionInfo> missions;

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Starting the mission1...";
    }

    Want want1;
    want1.SetElementName("test.bundle1", "TestAbility1");
    ret = abilityMgrServ_->StartAbility(want1);
    ASSERT_EQ(ret, ERR_OK);
    loadDone.Wait();
    ASSERT_TRUE(tokens[0]);
    ASSERT_TRUE(abilityRecords[0]);

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Checking state...";
    }

    missions.clear();
    ret = abilityMgrServ_->GetRecentMissions(100, RECENT_WITH_EXCLUDED, missions);
    ASSERT_EQ(ret, ERR_OK);
    ASSERT_EQ(missions.size(), size_t(1));

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Starting the mission2...";
    }

    Want want2;
    want2.SetElementName("test.bundle2", "TestAbility2");
    ret = abilityMgrServ_->StartAbility(want2);
    ASSERT_EQ(ret, ERR_OK);
    loadDone.Wait();
    ASSERT_TRUE(tokens[1]);
    ASSERT_TRUE(abilityRecords[1]);

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Checking state...";
    }

    missions.clear();
    ret = abilityMgrServ_->GetRecentMissions(100, RECENT_WITH_EXCLUDED, missions);
    ASSERT_EQ(ret, ERR_OK);
    ASSERT_EQ(missions.size(), size_t(1));

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Moving mission2 to top... (mission2 already on top.)";
    }

    // Top ability died.
    abilityMgrServ_->OnAbilityDied(abilityRecords[1]);
    usleep(100 * 1000);
    EXPECT_EQ(abilityRecords[1].use_count(), 1);

    // Test done.

    testing::Mock::AllowLeak(mockAbilitySchedulers[0]);
    testing::Mock::AllowLeak(mockAbilitySchedulers[1]);

    mockAbilitySchedulers[0].clear();
    mockAbilitySchedulers[1].clear();

    abilityRecords[0]->scheduler_.clear();
    abilityRecords[1]->scheduler_.clear();

    abilityRecords[0].reset();
    abilityRecords[1].reset();

    tokens[0].clear();
    tokens[1].clear();

    testing::Mock::AllowLeak(mockAbilitySchedulers[0]);
    testing::Mock::AllowLeak(mockAbilitySchedulers[1]);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: OnAbilityDied
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Test service abilities death notification.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_027, TestSize.Level3)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);

    ASSERT_TRUE(abilityMgrServ_);
    abilityMgrServ_->RemoveAllServiceRecord();
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    int testRequestCode = 123;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    std::shared_ptr<AbilityRecord> record = abilityMgrServ_->GetServiceRecordByElementName(want.GetElement().GetURI());
    EXPECT_TRUE(record);
    EXPECT_FALSE(record->IsCreateByConnect());
    record->AddStartId();
    record->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->GetServiceMap().size());

    abilityMgrServ_->OnAbilityDied(record);
    usleep(100 * 1000);
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetServiceMap().size());
}
#elif 0
/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: OnAbilityDied
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Test service abilities death notification.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_028, TestSize.Level3)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);

    abilityMgrServ_->RemoveAllServiceRecord();

    sptr<MockAbilityConnectCallbackStub> stub(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(stub));
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    abilityMgrServ_->ConnectAbility(want, callback, nullptr);
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->connectMap_.size());
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->serviceMap_.size());
    std::shared_ptr<AbilityRecord> record =
        abilityMgrServ_->connectManager_->GetServiceRecordByElementName(want.GetElement().GetURI());
    ASSERT_TRUE(record);
    ASSERT_TRUE(record->IsCreateByConnect());
    std::shared_ptr<ConnectionRecord> connectRecord = record->GetConnectingRecord();
    ASSERT_TRUE(connectRecord);
    connectRecord->SetConnectState(ConnectionState::CONNECTED);
    record->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    EXPECT_CALL(*stub, OnAbilityDisconnectDone(_, _)).Times(1);
    abilityMgrServ_->OnAbilityDied(record);
    usleep(100 * 1000);

    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetServiceMap().size());
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->connectMap_.size());

    abilityMgrServ_->RemoveAllServiceRecord();
    testing::Mock::AllowLeak(stub);
    testing::Mock::AllowLeak(callback);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: OnCallBackDied
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Test callback death notification.
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_029, TestSize.Level3)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);

    abilityMgrServ_->RemoveAllServiceRecord();

    sptr<MockAbilityConnectCallbackStub> stub(new MockAbilityConnectCallbackStub());
    const sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(stub));
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    abilityMgrServ_->ConnectAbility(want, callback, nullptr);
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->connectMap_.size());
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->serviceMap_.size());
    std::shared_ptr<AbilityRecord> record =
        abilityMgrServ_->connectManager_->GetServiceRecordByElementName(want.GetElement().GetURI());

    sptr<MockAbilityScheduler> scheduler = new MockAbilityScheduler();
    ASSERT_TRUE(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(1);
    abilityMgrServ_->AttachAbilityThread(scheduler, record->GetToken());
    ASSERT_TRUE(record->isReady_);
    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(1);
    abilityMgrServ_->OnAbilityRequestDone(
        record->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::INACTIVATING, record->GetAbilityState());
    EXPECT_CALL(*scheduler, ScheduleConnectAbility(_)).Times(1);
    abilityMgrServ_->AbilityTransitionDone(record->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INACTIVE);
    ASSERT_TRUE(record->GetConnectingRecord());
    EXPECT_CALL(*stub, OnAbilityConnectDone(_, _, _)).Times(1);
    abilityMgrServ_->ScheduleConnectAbilityDone(record->GetToken(), nullptr);

    EXPECT_EQ(OHOS::AAFwk::AbilityState::ACTIVE, record->GetAbilityState());

    EXPECT_CALL(*scheduler, ScheduleDisconnectAbility(_)).Times(1);
    abilityMgrServ_->connectManager_->OnCallBackDied(callback->AsObject());

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(_, _)).Times(1);
    abilityMgrServ_->ScheduleDisconnectAbilityDone(record->GetToken());

    usleep(100);
    EXPECT_EQ((std::size_t)0, record->GetConnectRecordList().size());
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetConnectMap().size());
    EXPECT_EQ((std::size_t)1, abilityMgrServ_->connectManager_->GetServiceMap().size());

    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(1);
    abilityMgrServ_->AbilityTransitionDone(record->GetToken(), AbilityLifeCycleState::ABILITY_STATE_INITIAL);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::TERMINATING, record->GetAbilityState());

    EXPECT_CALL(*mockAppMgrClient_, TerminateAbility(_)).Times(1);
    abilityMgrServ_->OnAbilityRequestDone(
        record->GetToken(), (int32_t)OHOS::AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
    EXPECT_EQ((std::size_t)0, abilityMgrServ_->connectManager_->GetServiceMap().size());

    abilityMgrServ_->RemoveAllServiceRecord();

    testing::Mock::AllowLeak(stub);
    testing::Mock::AllowLeak(callback);
    testing::Mock::AllowLeak(scheduler);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: Mission
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Uninstall
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_030, TestSize.Level3)
{
    constexpr bool DEBUG = false;

    ASSERT_TRUE(abilityMgrServ_);
    abilityMgrServ_->stackManagers_.clear();
    abilityMgrServ_->SetStackManager(0);
    ASSERT_TRUE(abilityMgrServ_->currentStackManager_);

    struct ScopedLooper {
        std::shared_ptr<AbilityManagerService> ams_;
        std::shared_ptr<AbilityEventHandler> handlerBackup_;
        std::shared_ptr<EventRunner> eventRunner_;

        ScopedLooper(std::shared_ptr<AbilityManagerService> ams) : ams_(ams), handlerBackup_(ams->handler_)
        {
            auto eventRunner = EventRunner::Create("AMSTestEventRunner");
            if (eventRunner) {
                ams_->handler_ = std::make_shared<AbilityEventHandler>(eventRunner, ams);
            }
        }

        ~ScopedLooper()
        {
            ams_->handler_ = handlerBackup_;
        }
    };

    ScopedLooper looper(abilityMgrServ_);
    Semaphore loadDone(0);

    // Test abilities.

    sptr<MockAbilityScheduler> mockAbilitySchedulers[2];
    sptr<IRemoteObject> tokens[2];
    std::shared_ptr<AbilityRecord> abilityRecords[2];

    mockAbilitySchedulers[0] = new MockAbilityScheduler();
    ASSERT_TRUE(mockAbilitySchedulers[0]);

    mockAbilitySchedulers[1] = new MockAbilityScheduler();
    ASSERT_TRUE(mockAbilitySchedulers[1]);

    auto mockAbilityTransaction = [&](int i, const Want &want, const LifeCycleStateInfo &targetState) {
        std::thread(
            [&](sptr<IRemoteObject> token, int state) {
                abilityMgrServ_->AbilityTransitionDone(token, state);
                if (state == ABILITY_STATE_ACTIVE) {
                    loadDone.Post();
                }
            },
            tokens[i],
            targetState.state)
            .detach();
    };

    EXPECT_CALL(*mockAbilitySchedulers[0], ScheduleAbilityTransaction(_, _))
        .Times(4)
        .WillRepeatedly(Invoke(std::bind(mockAbilityTransaction, 0, _1, _2)));

    EXPECT_CALL(*mockAbilitySchedulers[1], ScheduleAbilityTransaction(_, _))
        .Times(1)
        .WillRepeatedly(Invoke(std::bind(mockAbilityTransaction, 1, _1, _2)));

    EXPECT_CALL(*mockAbilitySchedulers[0], AsObject()).Times(1);
    EXPECT_CALL(*mockAbilitySchedulers[1], AsObject()).Times(1);

    // MOCK: AppMgrClient

    ASSERT_TRUE(mockAppMgrClient_);

    auto mockLoadAbility = [&](int i,
                               const sptr<IRemoteObject> &token,
                               const sptr<IRemoteObject> &preToken,
                               const AbilityInfo &abilityInfo,
                               const ApplicationInfo &appInfo) {
        abilityRecords[i] = Token::GetAbilityRecordByToken(token);
        tokens[i] = token;
        std::thread([&, i, token] { abilityMgrServ_->AttachAbilityThread(mockAbilitySchedulers[i], token); }).detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(std::bind(mockLoadAbility, 0, _1, _2, _3, _4)))
        .WillOnce(Invoke(std::bind(mockLoadAbility, 1, _1, _2, _3, _4)));

    auto mockUpdateAbilityState = [&](const sptr<IRemoteObject> &token, const AppExecFwk::AbilityState state) {
        std::thread([&, token, state] { abilityMgrServ_->OnAbilityRequestDone(token, int32_t(state)); }).detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(3).WillRepeatedly(Invoke(mockUpdateAbilityState));

    EXPECT_CALL(*mockAppMgrClient_, TerminateAbility(_)).WillOnce(Return(AppMgrResultCode::RESULT_OK));

    auto mockKillApplication = [&](const std::string &bundleName) {
        std::thread([&] { abilityMgrServ_->OnAbilityDied(abilityRecords[1]); }).detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, KillApplication(_)).Times(1).WillOnce(Invoke(mockKillApplication));

    // All mocks have setup, launch the test...

    int ret;

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Starting the ability1...";
    }

    Want want1;
    want1.SetElementName("test.bundle1", "TestAbility1");
    ret = abilityMgrServ_->StartAbility(want1);
    ASSERT_EQ(ret, ERR_OK);
    loadDone.Wait();

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Starting the ability2...";
    }

    Want want2;
    want2.SetElementName("test.bundle2", "TestAbility2");
    ret = abilityMgrServ_->StartAbility(want2);
    ASSERT_EQ(ret, ERR_OK);
    loadDone.Wait();

    ret = abilityMgrServ_->UninstallApp("test.bundle2");
    usleep(100 * 1000);
    ASSERT_EQ(ret, ERR_OK);
    ASSERT_EQ(abilityRecords[1].use_count(), 1);

    // Test done.

    testing::Mock::AllowLeak(mockAbilitySchedulers[0]);
    testing::Mock::AllowLeak(mockAbilitySchedulers[1]);

    mockAbilitySchedulers[0].clear();
    mockAbilitySchedulers[1].clear();

    abilityRecords[0]->scheduler_.clear();
    abilityRecords[1]->scheduler_.clear();

    abilityRecords[0].reset();
    abilityRecords[1].reset();

    tokens[0].clear();
    tokens[1].clear();

    testing::Mock::AllowLeak(mockAbilitySchedulers[0]);
    testing::Mock::AllowLeak(mockAbilitySchedulers[1]);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: Mission
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Uninstall
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_031, TestSize.Level3)
{
    constexpr bool DEBUG = false;

    ASSERT_TRUE(abilityMgrServ_);
    abilityMgrServ_->stackManagers_.clear();
    abilityMgrServ_->SetStackManager(0);
    ASSERT_TRUE(abilityMgrServ_->currentStackManager_);

    struct ScopedLooper {
        std::shared_ptr<AbilityManagerService> ams_;
        std::shared_ptr<AbilityEventHandler> handlerBackup_;
        std::shared_ptr<EventRunner> eventRunner_;

        ScopedLooper(std::shared_ptr<AbilityManagerService> ams) : ams_(ams), handlerBackup_(ams->handler_)
        {
            auto eventRunner = EventRunner::Create("AMSTestEventRunner");
            if (eventRunner) {
                ams_->handler_ = std::make_shared<AbilityEventHandler>(eventRunner, ams);
            }
        }

        ~ScopedLooper()
        {
            ams_->handler_ = handlerBackup_;
        }
    };

    ScopedLooper looper(abilityMgrServ_);
    Semaphore loadDone(0);

    // Test abilities.

    sptr<MockAbilityScheduler> mockAbilitySchedulers[2];
    sptr<IRemoteObject> tokens[2];
    std::shared_ptr<AbilityRecord> abilityRecords[2];

    mockAbilitySchedulers[0] = new MockAbilityScheduler();
    ASSERT_TRUE(mockAbilitySchedulers[0]);

    mockAbilitySchedulers[1] = new MockAbilityScheduler();
    ASSERT_TRUE(mockAbilitySchedulers[1]);

    auto mockAbilityTransaction = [&](int i, const Want &want, const LifeCycleStateInfo &targetState) {
        std::thread(
            [&](sptr<IRemoteObject> token, int state) {
                abilityMgrServ_->AbilityTransitionDone(token, state);
                if (state == ABILITY_STATE_ACTIVE) {
                    loadDone.Post();
                }
            },
            tokens[i],
            targetState.state)
            .detach();
    };

    EXPECT_CALL(*mockAbilitySchedulers[0], ScheduleAbilityTransaction(_, _))
        .Times(3)
        .WillRepeatedly(Invoke(std::bind(mockAbilityTransaction, 0, _1, _2)));

    EXPECT_CALL(*mockAbilitySchedulers[1], ScheduleAbilityTransaction(_, _))
        .Times(2)
        .WillRepeatedly(Invoke(std::bind(mockAbilityTransaction, 1, _1, _2)));

    EXPECT_CALL(*mockAbilitySchedulers[0], AsObject()).Times(1);
    EXPECT_CALL(*mockAbilitySchedulers[1], AsObject()).Times(1);

    // MOCK: AppMgrClient

    ASSERT_TRUE(mockAppMgrClient_);

    auto mockLoadAbility = [&](int i,
                               const sptr<IRemoteObject> &token,
                               const sptr<IRemoteObject> &preToken,
                               const AbilityInfo &abilityInfo,
                               const ApplicationInfo &appInfo) {
        abilityRecords[i] = Token::GetAbilityRecordByToken(token);
        tokens[i] = token;
        std::thread([&, i, token] { abilityMgrServ_->AttachAbilityThread(mockAbilitySchedulers[i], token); }).detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _))
        .Times(2)
        .WillOnce(Invoke(std::bind(mockLoadAbility, 0, _1, _2, _3, _4)))
        .WillOnce(Invoke(std::bind(mockLoadAbility, 1, _1, _2, _3, _4)));

    auto mockUpdateAbilityState = [&](const sptr<IRemoteObject> &token, const AppExecFwk::AbilityState state) {
        std::thread([&, token, state] { abilityMgrServ_->OnAbilityRequestDone(token, int32_t(state)); }).detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, UpdateAbilityState(_, _)).Times(3).WillRepeatedly(Invoke(mockUpdateAbilityState));

    auto mockKillApplication = [&](const std::string &bundleName) {
        std::thread([&] { abilityMgrServ_->OnAbilityDied(abilityRecords[0]); }).detach();
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient_, KillApplication(_)).Times(1).WillOnce(Invoke(mockKillApplication));

    // All mocks have setup, launch the test...

    int ret;

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Starting the ability1...";
    }

    Want want1;
    want1.SetElementName("com.ix.hiMusic", "TestAbility1");
    ret = abilityMgrServ_->StartAbility(want1);
    ASSERT_EQ(ret, ERR_OK);
    loadDone.Wait();

    if (DEBUG) {
        GTEST_LOG_(INFO) << "Starting the ability2...";
    }

    Want want2;
    want2.SetElementName("com.ix.hiMusic", "TestAbility2");
    ret = abilityMgrServ_->StartAbility(want2);
    ASSERT_EQ(ret, ERR_OK);
    loadDone.Wait();

    ret = abilityMgrServ_->UninstallApp("test.bundle1");
    usleep(100 * 1000);
    ASSERT_EQ(ret, ERR_OK);
    ASSERT_EQ(abilityRecords[0].use_count(), 1);

    // Test done.

    testing::Mock::AllowLeak(mockAbilitySchedulers[0]);
    testing::Mock::AllowLeak(mockAbilitySchedulers[1]);

    mockAbilitySchedulers[0].clear();
    mockAbilitySchedulers[1].clear();

    abilityRecords[0]->scheduler_.clear();
    abilityRecords[1]->scheduler_.clear();

    abilityRecords[0].reset();
    abilityRecords[1].reset();

    tokens[0].clear();
    tokens[1].clear();

    testing::Mock::AllowLeak(mockAbilitySchedulers[0]);
    testing::Mock::AllowLeak(mockAbilitySchedulers[1]);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Verify the terminateabilityresult process
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_032, TestSize.Level1)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiMusic_service";
    Want want = CreateWant(abilityName, bundleName);

    ASSERT_TRUE(abilityMgrServ_);
    abilityMgrServ_->RemoveAllServiceRecord();
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    EXPECT_CALL(*mockAppMgrClient_, AbilityBehaviorAnalysis(_, _, _, _, _)).Times(2);
    int testRequestCode = 1;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    waitAMS();
    SLEEP(1);
    auto record = abilityMgrServ_->GetServiceRecordByElementName(want.GetElement().GetURI());
    EXPECT_TRUE(record);
    EXPECT_FALSE(record->IsCreateByConnect());

    record->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    testRequestCode = 2;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    waitAMS();

    record->SetScheduler(scheduler_);
    auto mockAbilityTransation = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        EXPECT_EQ(targetState.state, AbilityLifeCycleState::ABILITY_STATE_INITIAL);
    };

    EXPECT_CALL(*scheduler_, ScheduleAbilityTransaction(_, _)).Times(1).WillOnce(Invoke(mockAbilityTransation));
    abilityMgrServ_->TerminateAbilityResult(record->GetToken(), record->GetStartId());
    waitAMS();
    record = abilityMgrServ_->GetServiceRecordByElementName(want.GetElement().GetURI());
    EXPECT_EQ(record->GetAbilityState(), OHOS::AAFwk::AbilityState::TERMINATING);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Validation provides URI information to the application framework
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_033, TestSize.Level1)
{
    std::string abilityName = "MusicAbility";
    std::string bundleName = "com.ix.hiMusic";
    Want want = CreateWant(abilityName, bundleName);

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    int testRequestCode = 1;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    waitAMS();

    auto stack = abilityMgrServ_->GetStackManager();
    if (!stack) {
        return;
    }
    auto abilityRecord = stack->GetCurrentTopAbility();
    if (!abilityRecord) {
        return;
    }
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    Want radioWant = CreateWant("RadioAbility", "com.ix.radio");
    testRequestCode = 2;
    abilityMgrServ_->StartAbility(radioWant, abilityRecord->GetToken(), testRequestCode);
    waitAMS();

    stack = abilityMgrServ_->GetStackManager();
    auto topAbility = stack->GetCurrentTopAbility();
    auto missionId = topAbility->GetMissionRecord()->GetMissionRecordId();

    topAbility->SetScheduler(scheduler_);
    auto mockAbilityTransation = [&, missionId](const Want &want, const LifeCycleStateInfo &targetState) {
        EXPECT_EQ(targetState.state, AbilityLifeCycleState::ABILITY_STATE_ACTIVE);
        EXPECT_EQ(targetState.caller.abilityName, abilityName);
        EXPECT_EQ(targetState.caller.bundleName, bundleName);
        EXPECT_EQ(targetState.caller.requestCode, testRequestCode);
        EXPECT_EQ(targetState.missionId, missionId);
    };

    EXPECT_CALL(*scheduler_, ScheduleAbilityTransaction(_, _)).Times(1).WillOnce(Invoke(mockAbilityTransation));
    EXPECT_CALL(*mockAppMgrClient_, AbilityBehaviorAnalysis(_, _, _, _, _)).Times(1);
    topAbility->SetIsNewWant(true);
    abilityMgrServ_->OnAbilityRequestDone(topAbility->GetToken(), 2);

    waitAMS();
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Verify page terminateabilitybycaller process
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_034, TestSize.Level1)
{
    std::string abilityName = "MusicAbility";
    std::string bundleName = "com.ix.hiMusic";
    Want want = CreateWant(abilityName, bundleName);

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    int testRequestCode = 1;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    waitAMS();

    auto stack = abilityMgrServ_->GetStackManager();
    if (!stack) {
        return;
    }
    auto topAbility = stack->GetCurrentTopAbility();
    if (!topAbility) {
        return;
    }
    topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    Want radioWant = CreateWant("RadioAbility", "com.ix.radio");
    testRequestCode = 2;
    abilityMgrServ_->StartAbility(radioWant, topAbility->GetToken(), testRequestCode);
    waitAMS();

    GTEST_LOG_(INFO) << "TerminateAbilityByCaller";
    auto result = abilityMgrServ_->TerminateAbilityByCaller(topAbility->GetToken(), testRequestCode);
    waitAMS();
    EXPECT_EQ(result, OHOS::ERR_OK);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Verify service terminateabilitybycaller process
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_035, TestSize.Level1)
{
    std::string abilityName = "MusicAbility_service";
    std::string bundleName = "com.ix.hiworld";
    Want want = CreateWant(abilityName, bundleName);

    ASSERT_TRUE(abilityMgrServ_);
    abilityMgrServ_->RemoveAllServiceRecord();
    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(2);
    EXPECT_CALL(*mockAppMgrClient_, AbilityBehaviorAnalysis(_, _, _, _, _)).Times(2);
    int testRequestCode = 1;
    abilityMgrServ_->StartAbility(want, testRequestCode);
    waitAMS();
    SLEEP(1);
    auto record = abilityMgrServ_->GetServiceRecordByElementName(want.GetElement().GetURI());
    EXPECT_TRUE(record);
    EXPECT_FALSE(record->IsCreateByConnect());

    record->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    testRequestCode = 2;

    Want radioWant = CreateWant("radio_service", "radio_bundle_service");
    testRequestCode = 2;
    abilityMgrServ_->StartAbility(radioWant, record->GetToken(), testRequestCode);
    waitAMS();

    auto result = abilityMgrServ_->TerminateAbilityByCaller(record->GetToken(), testRequestCode);
    waitAMS();
    EXPECT_EQ(result, OHOS::ERR_OK);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Verify dialog startup process
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_036, TestSize.Level1)
{
    std::string abilityName = "LauncherAbility";
    std::string bundleName = "com.ix.hiworld";
    Want want = CreateWant(abilityName, bundleName);

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    int testRequestCode = 1;
    auto result = abilityMgrServ_->StartAbility(want, testRequestCode);
    waitAMS();
    EXPECT_EQ(result, OHOS::ERR_OK);

    auto stack = abilityMgrServ_->GetStackManager();
    if (!stack) {
        return;
    }
    auto topAbility = stack->GetCurrentTopAbility();
    if (!topAbility) {
        return;
    }
    topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    testRequestCode = 2;
    Want dialogWant = CreateWant(AbilityConfig::SYSTEM_DIALOG_NAME, AbilityConfig::SYSTEM_UI_BUNDLE_NAME);
    result = abilityMgrServ_->StartAbility(dialogWant, testRequestCode);
    waitAMS();
    EXPECT_EQ(result, OHOS::ERR_OK);

    topAbility = stack->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    testRequestCode = 3;
    Want launcherWant = CreateWant(abilityName, bundleName);
    result = abilityMgrServ_->StartAbility(launcherWant, testRequestCode);
    waitAMS();
    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/*
 * Feature: AaFwk
 * Function: ability manager service
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Verify the start-up process of systemui and keep alive mechanism
 */
HWTEST_F(AbilityMgrModuleTest, ability_mgr_service_test_037, TestSize.Level1)
{
    Want want = CreateWant(AbilityConfig::SYSTEM_UI_STATUS_BAR, AbilityConfig::SYSTEM_UI_BUNDLE_NAME);

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    int testRequestCode = 1;
    auto result = abilityMgrServ_->StartAbility(want, testRequestCode);
    waitAMS();
    EXPECT_EQ(result, OHOS::ERR_OK);

    auto topAbility = abilityMgrServ_->systemAppManager_->GetCurrentTopAbility();
    abilityMgrServ_->AttachAbilityThread(scheduler_, topAbility->GetToken());

    EXPECT_CALL(*scheduler_, ScheduleAbilityTransaction(_, _)).Times(1);
    abilityMgrServ_->OnAbilityRequestDone(topAbility->GetToken(), 2);

    abilityMgrServ_->AbilityTransitionDone(topAbility->GetToken(), 2);
    waitAMS();
    topAbility = abilityMgrServ_->systemAppManager_->GetCurrentTopAbility();
    EXPECT_EQ(topAbility->GetAbilityState(), OHOS::AAFwk::AbilityState::ACTIVE);

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    Want want1 = CreateWant(AbilityConfig::SYSTEM_UI_NAVIGATION_BAR, AbilityConfig::SYSTEM_UI_BUNDLE_NAME);
    testRequestCode = 2;
    result = abilityMgrServ_->StartAbility(want1, testRequestCode);
    waitAMS();
    EXPECT_EQ(result, OHOS::ERR_OK);

    topAbility = abilityMgrServ_->systemAppManager_->GetCurrentTopAbility();

    EXPECT_CALL(*mockAppMgrClient_, LoadAbility(_, _, _, _)).Times(1);
    abilityMgrServ_->OnAbilityDied(topAbility);
    waitAMS();
}

#endif
}  // namespace AAFwk
}  // namespace OHOS
