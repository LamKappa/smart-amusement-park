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

#define private public
#define protected public
#include "ability_connect_manager.h"
#undef private
#undef protected

#include "ability_manager_errors.h"
#include "ability_scheduler.h"
#include "event_handler.h"
#include "mock_ability_connect_callback.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
template <typename F>
static void WaitUntilTaskCalled(const F &f, const std::shared_ptr<EventHandler> &handler, std::atomic<bool> &taskCalled)
{
    const uint32_t maxRetryCount = 1000;
    const uint32_t sleepTime = 1000;
    uint32_t count = 0;
    if (handler->PostTask(f)) {
        while (!taskCalled.load()) {
            ++count;
            // if delay more than 1 second, break
            if (count >= maxRetryCount) {
                break;
            }
            usleep(sleepTime);
        }
    }
}

static void WaitUntilTaskDone(const std::shared_ptr<EventHandler> &handler)
{
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };
    WaitUntilTaskCalled(f, handler, taskCalled);
}

class AbilityConnectManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    AbilityConnectManager *ConnectManager() const;

    AbilityRequest GenerateAbilityRequest(const std::string &deviceName, const std::string &abilityName,
        const std::string &appName, const std::string &bundleName);

    static constexpr int TEST_WAIT_TIME = 1000000;

protected:
    AbilityRequest abilityRequest_;
    AbilityRequest abilityRequest1_;
    std::shared_ptr<AbilityRecord> serviceRecord_;
    std::shared_ptr<AbilityRecord> serviceRecord1_;
    OHOS::sptr<Token> serviceToken_;
    OHOS::sptr<Token> serviceToken1_;
    OHOS::sptr<IAbilityConnection> callbackA_;
    OHOS::sptr<IAbilityConnection> callbackB_;

private:
    std::shared_ptr<AbilityConnectManager> connectManager_;
};

AbilityRequest AbilityConnectManagerTest::GenerateAbilityRequest(const std::string &deviceName,
    const std::string &abilityName, const std::string &appName, const std::string &bundleName)
{
    ElementName element(deviceName, bundleName, abilityName);
    Want want;
    want.SetElement(element);

    AbilityInfo abilityInfo;
    abilityInfo.applicationName = appName;
    abilityInfo.type = AbilityType::SERVICE;
    abilityInfo.name = abilityName;
    abilityInfo.bundleName = bundleName;
    abilityInfo.deviceId = deviceName;
    ApplicationInfo appinfo;
    appinfo.name = appName;
    abilityInfo.applicationInfo = appinfo;
    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = abilityInfo;
    abilityRequest.appInfo = appinfo;

    return abilityRequest;
}

void AbilityConnectManagerTest::SetUpTestCase(void)
{}
void AbilityConnectManagerTest::TearDownTestCase(void)
{}

void AbilityConnectManagerTest::SetUp(void)
{
    connectManager_ = std::make_unique<AbilityConnectManager>();
    // generate ability request
    std::string deviceName = "device";
    std::string abilityName = "ServiceAbility";
    std::string appName = "hiservcie";
    std::string bundleName = "com.ix.hiservcie";
    abilityRequest_ = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    serviceRecord_ = AbilityRecord::CreateAbilityRecord(abilityRequest_);
    serviceToken_ = serviceRecord_->GetToken();
    std::string deviceName1 = "device";
    std::string abilityName1 = "musicServiceAbility";
    std::string appName1 = "musicservcie";
    std::string bundleName1 = "com.ix.musicservcie";
    abilityRequest1_ = GenerateAbilityRequest(deviceName1, abilityName1, appName1, bundleName1);
    serviceRecord1_ = AbilityRecord::CreateAbilityRecord(abilityRequest1_);
    serviceToken1_ = serviceRecord_->GetToken();
    callbackA_ = new AbilityConnectCallback();
    callbackB_ = new AbilityConnectCallback();
}

void AbilityConnectManagerTest::TearDown(void)
{
    // reset the callback count
    AbilityConnectCallback::onAbilityConnectDoneCount = 0;
    AbilityConnectCallback::onAbilityDisconnectDoneCount = 0;
    serviceRecord_ = nullptr;
}

AbilityConnectManager *AbilityConnectManagerTest::ConnectManager() const
{
    return connectManager_.get();
}

/*
 * Feature: AbilityConnectManager
 * Function: StartAbility
 * SubFunction: NA
 * FunctionPoints: StartAbility
 * EnvConditions:NA
 * CaseDescription: Verify the normal process of startability
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_001, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto result = ConnectManager()->StartAbility(abilityRequest_);
    EXPECT_EQ(OHOS::ERR_OK, result);
    WaitUntilTaskDone(handler);

    auto elementName = abilityRequest_.want.GetElement().GetURI();
    auto service = ConnectManager()->GetServiceRecordByElementName(elementName);
    EXPECT_NE(service, nullptr);
    EXPECT_EQ(static_cast<int>(ConnectManager()->GetServiceMap().size()), 1);

    service->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    auto result1 = ConnectManager()->StartAbility(abilityRequest_);
    WaitUntilTaskDone(handler);
    EXPECT_EQ(OHOS::ERR_OK, result1);
    EXPECT_EQ(static_cast<int>(ConnectManager()->GetServiceMap().size()), 1);

    service->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVATING);
    auto result2 = ConnectManager()->StartAbility(abilityRequest_);
    WaitUntilTaskDone(handler);
    EXPECT_EQ(OHOS::AAFwk::START_SERVICE_ABILITY_ACTIVING, result2);
    EXPECT_EQ(static_cast<int>(ConnectManager()->GetServiceMap().size()), 1);
}

/*
 * Feature: AbilityConnectManager
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: StartAbility and TerminateAbility
 * EnvConditions:NA
 * CaseDescription: Verify the following:
 * 1.token is nullptr, terminate ability failed
 * 2.token is not nullptr, terminate ability success, and verify the status
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_002, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto result = ConnectManager()->StartAbility(abilityRequest_);
    EXPECT_EQ(OHOS::ERR_OK, result);
    WaitUntilTaskDone(handler);

    OHOS::sptr<OHOS::IRemoteObject> nullToken = nullptr;
    auto result1 = ConnectManager()->TerminateAbility(nullToken);
    EXPECT_EQ(OHOS::ERR_INVALID_VALUE, result1);

    auto elementName = abilityRequest_.want.GetElement().GetURI();
    auto service = ConnectManager()->GetServiceRecordByElementName(elementName);
    EXPECT_NE(service, nullptr);

    auto result2 = ConnectManager()->TerminateAbility(service->GetToken());
    WaitUntilTaskDone(handler);
    EXPECT_EQ(OHOS::ERR_OK, result2);
    EXPECT_EQ(service->GetAbilityState(), TERMINATING);
}

/*
 * Feature: AbilityConnectManager
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: StartAbility and TerminateAbility
 * EnvConditions:NA
 * CaseDescription: Verify ability is terminating, terminate ability success
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_003, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto result = ConnectManager()->StartAbility(abilityRequest_);
    EXPECT_EQ(OHOS::ERR_OK, result);
    WaitUntilTaskDone(handler);

    auto elementName = abilityRequest_.want.GetElement().GetURI();
    auto service = ConnectManager()->GetServiceRecordByElementName(elementName);
    EXPECT_NE(service, nullptr);

    service->SetTerminatingState();
    auto result1 = ConnectManager()->TerminateAbility(service->GetToken());
    WaitUntilTaskDone(handler);
    EXPECT_EQ(OHOS::ERR_OK, result1);
    EXPECT_NE(service->GetAbilityState(), TERMINATING);
}

/*
 * Feature: AbilityConnectManager
 * Function: TerminateAbility
 * SubFunction: NA
 * FunctionPoints: StartAbility and TerminateAbility
 * EnvConditions: NA
 * CaseDescription: Verify service is connected, terminate ability failed
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_004, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto result = ConnectManager()->StartAbility(abilityRequest_);
    EXPECT_EQ(OHOS::ERR_OK, result);
    WaitUntilTaskDone(handler);

    auto result1 = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result1);

    auto elementName = abilityRequest_.want.GetElement().GetURI();
    auto service = ConnectManager()->GetServiceRecordByElementName(elementName);
    EXPECT_NE(service, nullptr);

    auto result2 = ConnectManager()->TerminateAbility(service->GetToken());
    WaitUntilTaskDone(handler);
    EXPECT_EQ(OHOS::AAFwk::TERMINATE_SERVICE_IS_CONNECTED, result2);
    EXPECT_NE(service->GetAbilityState(), TERMINATING);
}

/*
 * Feature: AbilityConnectManager
 * Function: StopServiceAbility
 * SubFunction: NA
 * FunctionPoints: StartAbility and StopServiceAbility
 * EnvConditions: NA
 * CaseDescription: Verify the following:
 * 1.token is nullptr, stop service ability failed
 * 2.token is not nullptr, stop service ability success, and verify the status
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_005, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto result = ConnectManager()->StartAbility(abilityRequest_);
    EXPECT_EQ(OHOS::ERR_OK, result);
    WaitUntilTaskDone(handler);

    auto elementName = abilityRequest_.want.GetElement().GetURI();
    auto service = ConnectManager()->GetServiceRecordByElementName(elementName);
    EXPECT_NE(service, nullptr);

    AbilityRequest otherRequest;
    auto result1 = ConnectManager()->StopServiceAbility(otherRequest);
    WaitUntilTaskDone(handler);
    EXPECT_EQ(OHOS::ERR_INVALID_VALUE, result1);

    auto result2 = ConnectManager()->StopServiceAbility(abilityRequest_);
    WaitUntilTaskDone(handler);
    EXPECT_EQ(OHOS::ERR_OK, result2);
    EXPECT_EQ(service->GetAbilityState(), TERMINATING);
}

/*
 * Feature: AbilityConnectManager
 * Function: StopServiceAbility
 * SubFunction: NA
 * FunctionPoints: StartAbility and StopServiceAbility
 * EnvConditions:NA
 * CaseDescription: Verify ability is terminating, stop service ability success
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_006, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto result = ConnectManager()->StartAbility(abilityRequest_);
    EXPECT_EQ(OHOS::ERR_OK, result);
    WaitUntilTaskDone(handler);

    auto elementName = abilityRequest_.want.GetElement().GetURI();
    auto service = ConnectManager()->GetServiceRecordByElementName(elementName);
    EXPECT_NE(service, nullptr);

    service->SetTerminatingState();
    auto result1 = ConnectManager()->StopServiceAbility(abilityRequest_);
    WaitUntilTaskDone(handler);
    EXPECT_EQ(OHOS::ERR_OK, result1);
    EXPECT_NE(service->GetAbilityState(), TERMINATING);
}

/*
 * Feature: AbilityConnectManager
 * Function: StopServiceAbility
 * SubFunction: NA
 * FunctionPoints: StartAbility and StopServiceAbility
 * EnvConditions: NA
 * CaseDescription: Verify service is connected, stop service ability failed
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_007, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto result = ConnectManager()->StartAbility(abilityRequest_);
    EXPECT_EQ(OHOS::ERR_OK, result);
    WaitUntilTaskDone(handler);

    auto result1 = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result1);

    auto elementName = abilityRequest_.want.GetElement().GetURI();
    auto service = ConnectManager()->GetServiceRecordByElementName(elementName);
    EXPECT_NE(service, nullptr);

    auto result2 = ConnectManager()->StopServiceAbility(abilityRequest_);
    WaitUntilTaskDone(handler);
    EXPECT_EQ(OHOS::AAFwk::TERMINATE_SERVICE_IS_CONNECTED, result2);
    EXPECT_NE(service->GetAbilityState(), TERMINATING);
}

/*
 * Feature: AbilityConnectManager
 * Function: ConnectAbilityLocked
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the scene of service not loaded and callback not bound.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_008, TestSize.Level0)
{
    int result = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result);

    auto connectMap = ConnectManager()->GetConnectMap();
    auto connectRecordList = connectMap.at(callbackA_->AsObject());
    EXPECT_EQ(1, static_cast<int>(connectRecordList.size()));

    auto elementName = abilityRequest_.want.GetElement();
    auto elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    connectRecordList = abilityRecord->GetConnectRecordList();
    EXPECT_EQ(1, static_cast<int>(connectRecordList.size()));
}

/*
 * Feature: AbilityConnectManager
 * Function: ConnectAbilityLocked
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the scene of service load ability's timeout.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_009, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);
    int result = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result);

    auto connectMap = ConnectManager()->GetConnectMap();
    EXPECT_EQ(1, static_cast<int>(connectMap.size()));
    WaitUntilTaskDone(handler);
    usleep(TEST_WAIT_TIME);

    connectMap = ConnectManager()->GetConnectMap();
    EXPECT_EQ(0, static_cast<int>(connectMap.size()));
}

/*
 * Feature: AbilityConnectManager
 * Function: ConnectAbilityLocked
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the scene of service loaded and callback not bound.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_010, TestSize.Level0)
{
    auto result = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result);

    result = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackB_, nullptr);
    EXPECT_EQ(0, result);

    auto connectMap = ConnectManager()->GetConnectMap();
    auto connectRecordList = connectMap.at(callbackA_->AsObject());
    EXPECT_EQ(1, static_cast<int>(connectRecordList.size()));

    connectRecordList = connectMap.at(callbackB_->AsObject());
    EXPECT_EQ(1, static_cast<int>(connectRecordList.size()));

    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    connectRecordList = abilityRecord->GetConnectRecordList();
    EXPECT_EQ(2, static_cast<int>(connectRecordList.size()));
}

/*
 * Feature: AbilityConnectManager
 * Function: ConnectAbilityLocked
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the scene of service connect ability's timeout.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_011, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    int result = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    auto token = abilityRecord->GetToken();

    auto connectMap = ConnectManager()->GetConnectMap();
    EXPECT_EQ(1, static_cast<int>(connectMap.size()));

    auto scheduler = new AbilityScheduler();
    ConnectManager()->AttachAbilityThreadLocked(scheduler, token->AsObject());
    ConnectManager()->OnAbilityRequestDone(
        token->AsObject(), static_cast<int32_t>(AppAbilityState::ABILITY_STATE_FOREGROUND));
    ConnectManager()->AbilityTransitionDone(token->AsObject(), OHOS::AAFwk::AbilityState::INACTIVE);

    WaitUntilTaskDone(handler);
    usleep(TEST_WAIT_TIME);
    connectMap = ConnectManager()->GetConnectMap();
    EXPECT_EQ(0, result);
    EXPECT_EQ(1, static_cast<int>(connectMap.size()));
}

/*
 * Feature: AbilityConnectManager
 * Function: ConnectAbilityLocked
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the scene of service loaded and callback bound.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_012, TestSize.Level0)
{
    auto result = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result);

    result = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result);

    auto connectMap = ConnectManager()->GetConnectMap();
    auto connectRecordList = connectMap.at(callbackA_->AsObject());
    EXPECT_EQ(1, static_cast<int>(connectRecordList.size()));

    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    connectRecordList = abilityRecord->GetConnectRecordList();
    EXPECT_EQ(1, static_cast<int>(connectRecordList.size()));
}

/*
 * Feature: AbilityConnectManager
 * Function: ConnectAbilityLocked
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the scene of service not loaded and callback bound.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_013, TestSize.Level0)
{
    int result = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result);

    std::string deviceNameB = "device";
    std::string abilityNameB = "ServiceAbilityB";
    std::string appNameB = "hiservcieB";
    std::string bundleNameB = "com.ix.hiservcieB";
    auto abilityRequestB = GenerateAbilityRequest(deviceNameB, abilityNameB, appNameB, bundleNameB);
    result = ConnectManager()->ConnectAbilityLocked(abilityRequestB, callbackA_, nullptr);
    EXPECT_EQ(0, result);

    auto connectMap = ConnectManager()->GetConnectMap();
    auto connectRecordList = connectMap.at(callbackA_->AsObject());
    EXPECT_EQ(2, static_cast<int>(connectRecordList.size()));

    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    connectRecordList = abilityRecord->GetConnectRecordList();
    EXPECT_EQ(1, static_cast<int>(connectRecordList.size()));

    auto elementNameB = abilityRequest_.want.GetElement();
    std::string elementNameUriB = elementNameB.GetURI();
    abilityRecord = serviceMap.at(elementNameUriB);
    connectRecordList = abilityRecord->GetConnectRecordList();
    EXPECT_EQ(1, static_cast<int>(connectRecordList.size()));
}

/*
 * Feature: AbilityConnectManager
 * Function: ConnectAbilityLocked
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the scene of service loaded and callback bound, but service and callback was not associated.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_014, TestSize.Level0)
{
    auto result = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result);

    std::string deviceNameB = "device";
    std::string abilityNameB = "ServiceAbilityB";
    std::string appNameB = "hiservcieB";
    std::string bundleNameB = "com.ix.hiservcieB";
    auto abilityRequestB = GenerateAbilityRequest(deviceNameB, abilityNameB, appNameB, bundleNameB);
    result = ConnectManager()->ConnectAbilityLocked(abilityRequestB, callbackB_, nullptr);
    EXPECT_EQ(0, result);

    ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackB_, nullptr);
    auto connectMap = ConnectManager()->GetConnectMap();
    auto connectRecordList = connectMap.at(callbackB_->AsObject());
    EXPECT_EQ(2, static_cast<int>(connectRecordList.size()));

    connectRecordList = connectMap.at(callbackA_->AsObject());
    EXPECT_EQ(1, static_cast<int>(connectRecordList.size()));

    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    connectRecordList = abilityRecord->GetConnectRecordList();
    EXPECT_EQ(2, static_cast<int>(connectRecordList.size()));
}

/*
 * Feature: AbilityConnectManager
 * Function: AttachAbilityThreadLocked
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the AttachAbilityThreadLocked function when the parameter is null.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_015, TestSize.Level0)
{
    auto result = ConnectManager()->AttachAbilityThreadLocked(nullptr, nullptr);
    EXPECT_EQ(OHOS::ERR_INVALID_VALUE, result);
}

/*
 * Feature: AbilityConnectManager
 * Function: ScheduleConnectAbilityDoneLocked
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the ScheduleConnectAbilityDoneLocked function when the parameter is null.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_016, TestSize.Level1)
{
    auto callback = new AbilityConnectCallback();
    auto result = ConnectManager()->ScheduleConnectAbilityDoneLocked(nullptr, callback);
    EXPECT_EQ(OHOS::ERR_INVALID_VALUE, result);
}

/*
 * Feature: AbilityConnectManager
 * Function: ScheduleConnectAbilityDoneLocked
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the ScheduleConnectAbilityDoneLocked function when the state is CONNECTED.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_017, TestSize.Level0)
{
    auto callback = new AbilityConnectCallback();
    ConnectManager()->ConnectAbilityLocked(abilityRequest_, callback, nullptr);

    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    auto token = abilityRecord->GetToken();

    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    ConnectManager()->ScheduleConnectAbilityDoneLocked(token, callback);
    auto abilityRecordB = Token::GetAbilityRecordByToken(token);
    ASSERT_TRUE(abilityRecordB);
    auto connectRecordList = abilityRecordB->GetConnectRecordList();
    int size = connectRecordList.size();
    EXPECT_EQ(1, size);
    if (size != 0) {
        auto connState = (*(connectRecordList.begin()))->GetConnectState();
        EXPECT_EQ(ConnectionState::CONNECTED, connState);
    }
}

/*
 * Feature: AbilityConnectManager
 * Function: ScheduleConnectAbilityDoneLocked
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the input parameters.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Kit_Connect_001, TestSize.Level0)
{
    // start test
    // test1 for serviceToken is null but remoteObject is valid
    OHOS::sptr<OHOS::IRemoteObject> object = new AbilityConnectCallback();
    int ret = ConnectManager()->ScheduleConnectAbilityDoneLocked(nullptr, object);
    EXPECT_EQ(OHOS::ERR_INVALID_VALUE, ret);

    // test2 for both of serviceToken and remoteObject are null
    ret = ConnectManager()->ScheduleConnectAbilityDoneLocked(nullptr, nullptr);
    EXPECT_EQ(OHOS::ERR_INVALID_VALUE, ret);
}

/*
 * Feature: AbilityConnectManager
 * Function: ScheduleConnectAbilityDoneLocked
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the input serviceToken which corresponding ability record doesn't exist.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Kit_Connect_002, TestSize.Level0)
{
    // test for serviceToken's abilityRecord is null
    serviceRecord_ = nullptr;
    int ret = ConnectManager()->ScheduleConnectAbilityDoneLocked(serviceToken_, nullptr);
    EXPECT_EQ(OHOS::ERR_INVALID_VALUE, ret);
}

/*
 * Feature: AbilityConnectManager
 * Function: ScheduleConnectAbilityDoneLocked
 * SubFunction: OnAbilityConnectDone
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the input serviceToken which corresponding connection list is empty.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Kit_Connect_003, TestSize.Level0)
{
    // test for serviceToken's connection list is null
    // start test
    auto callback = new AbilityConnectCallback();
    int ret = ConnectManager()->ScheduleConnectAbilityDoneLocked(serviceToken_, callback);
    EXPECT_EQ(OHOS::AAFwk::INVALID_CONNECTION_STATE, ret);
    auto connList = serviceRecord_->GetConnectRecordList();
    EXPECT_EQ(true, connList.empty());  // the connection list size should be empty
}

/*
 * Feature: AbilityConnectManager
 * Function: ScheduleConnectAbilityDoneLocked
 * SubFunction: OnAbilityConnectDone
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the input serviceToken which corresponding connection list members' state
 * is not CONNECTING or CONNECTED.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Kit_Connect_004, TestSize.Level0)
{
    // test for schedule the service connected done but the corresponding connection state is not CONNECTING
    // generate the first connection record of callbackA_
    auto newConnRecord1 = ConnectionRecord::CreateConnectionRecord(
        serviceToken_, serviceRecord_, callbackA_);  // newConnRecord1's default state is INIT
    serviceRecord_->AddConnectRecordToList(newConnRecord1);
    // generate the second connection record of callbackB_
    auto newConnRecord2 = ConnectionRecord::CreateConnectionRecord(serviceToken_, serviceRecord_, callbackB_);
    newConnRecord2->SetConnectState(ConnectionState::DISCONNECTING);  // newConnRecord2's state is DISCONNECTING
    serviceRecord_->AddConnectRecordToList(newConnRecord2);
    auto connList = serviceRecord_->GetConnectRecordList();
    EXPECT_EQ(2, static_cast<int>(connList.size()));  // the connection list members should be two
    // start test
    auto callback = new AbilityConnectCallback();
    serviceRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    int ret = ConnectManager()->ScheduleConnectAbilityDoneLocked(serviceToken_, callback);
    EXPECT_EQ(OHOS::ERR_OK, ret);  // the result should be OK
    // connection callback should not be called, so check the count
    EXPECT_EQ(0, AbilityConnectCallback::onAbilityConnectDoneCount);
    EXPECT_EQ(0, AbilityConnectCallback::onAbilityDisconnectDoneCount);
}

/*
 * Feature: AbilityConnectManager
 * Function: ScheduleConnectAbilityDoneLocked
 * SubFunction: OnAbilityConnectDone
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the scene : 1.serviceToken's corresponding connection list member's state is CONNECTING.
 * 2.But the connection callback is null.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Kit_Connect_005, TestSize.Level0)
{
    // test for schedule the service connected done but the corresponding connection state is not CONNECTING
    // generate the first connection record of null
    auto newConnRecord1 = ConnectionRecord::CreateConnectionRecord(
        serviceToken_, serviceRecord_, nullptr);  // newConnRecord1's default state is INIT
    serviceRecord_->AddConnectRecordToList(newConnRecord1);
    newConnRecord1->SetConnectState(ConnectionState::CONNECTING);  // newConnRecord1's state is CONNECTING
    auto connList = serviceRecord_->GetConnectRecordList();
    EXPECT_EQ(1, static_cast<int>(connList.size()));  // the connection list members should be zero
    // start test
    auto callback = new AbilityConnectCallback();
    serviceRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    int ret = ConnectManager()->ScheduleConnectAbilityDoneLocked(serviceToken_, callback);
    EXPECT_EQ(OHOS::ERR_OK, ret);  // the result should be OK
    // connection callback should not be called, so check the count
    EXPECT_EQ(0, AbilityConnectCallback::onAbilityConnectDoneCount);
}

/*
 * Feature: AbilityConnectManager
 * Function: ScheduleConnectAbilityDoneLocked
 * SubFunction: OnAbilityConnectDone
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the scene : 1.serviceToken's corresponding connection list member's state is CONNECTED.
 * 2.But the connection callback is null.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Kit_Connect_006, TestSize.Level0)
{
    // test for schedule the service connected done but the corresponding connection state is not CONNECTING
    // generate the first connection record of null
    auto newConnRecord1 = ConnectionRecord::CreateConnectionRecord(
        serviceToken_, serviceRecord_, nullptr);  // newConnRecord1's default state is INIT
    serviceRecord_->AddConnectRecordToList(newConnRecord1);
    newConnRecord1->SetConnectState(ConnectionState::CONNECTED);  // newConnRecord1's state is CONNECTED
    auto connList = serviceRecord_->GetConnectRecordList();
    EXPECT_EQ(1, static_cast<int>(connList.size()));  // the connection list members should be zero
    // start test
    auto callback = new AbilityConnectCallback();
    serviceRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    int ret = ConnectManager()->ScheduleConnectAbilityDoneLocked(serviceToken_, callback);
    EXPECT_EQ(OHOS::ERR_OK, ret);  // the result should be OK
    // connection callback should not be called, so check the count
    EXPECT_EQ(0, AbilityConnectCallback::onAbilityConnectDoneCount);
}

/*
 * Feature: AbilityConnectManager
 * Function: ScheduleConnectAbilityDoneLocked
 * SubFunction: OnAbilityConnectDone
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the scene : 1.serviceToken's corresponding connection list member's state is CONNECTING.
 * 2.But the connection callback is valid.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Kit_Connect_007, TestSize.Level0)
{
    // test for schedule the service connected done but the corresponding connection state is not CONNECTING
    // generate the first connection record of callbackA_
    auto newConnRecord1 = ConnectionRecord::CreateConnectionRecord(
        serviceToken_, serviceRecord_, callbackA_);  // newConnRecord1's default state is INIT
    serviceRecord_->AddConnectRecordToList(newConnRecord1);
    // generate the second connection record of callbackB_
    auto newConnRecord2 = ConnectionRecord::CreateConnectionRecord(serviceToken_, serviceRecord_, callbackB_);
    newConnRecord2->SetConnectState(ConnectionState::CONNECTING);  // newConnRecord2's state is CONNECTING
    serviceRecord_->AddConnectRecordToList(newConnRecord2);
    auto connList = serviceRecord_->GetConnectRecordList();
    EXPECT_EQ(2, static_cast<int>(connList.size()));  // the connection list members should be two
    // start test
    auto callback = new AbilityConnectCallback();
    serviceRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    int ret = ConnectManager()->ScheduleConnectAbilityDoneLocked(serviceToken_, callback);
    EXPECT_EQ(OHOS::ERR_OK, ret);  // the result should be OK
    // connection callback should not be called, so check the count
    EXPECT_EQ(1, AbilityConnectCallback::onAbilityConnectDoneCount);
    EXPECT_EQ(0, AbilityConnectCallback::onAbilityDisconnectDoneCount);
}

/*
 * Feature: AbilityConnectManager
 * Function: ScheduleConnectAbilityDoneLocked
 * SubFunction: OnAbilityConnectDone
 * FunctionPoints: NA
 * EnvConditions:NA
 * CaseDescription: verify the scene : 1.serviceToken's corresponding connection list member's state is CONNECTED.
 * 2.But the connection callback is valid.
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Kit_Connect_008, TestSize.Level0)
{
    // test for schedule the service connected done but the corresponding connection state is not CONNECTING
    // generate the first connection record of callbackA_
    auto newConnRecord1 = ConnectionRecord::CreateConnectionRecord(
        serviceToken_, serviceRecord_, callbackA_);               // newConnRecord1's default state is INIT
    newConnRecord1->SetConnectState(ConnectionState::CONNECTED);  // newConnRecord1's state is CONNECTED
    serviceRecord_->AddConnectRecordToList(newConnRecord1);
    // generate the second connection record of callbackB_
    auto newConnRecord2 = ConnectionRecord::CreateConnectionRecord(serviceToken_, serviceRecord_, callbackB_);
    newConnRecord2->SetConnectState(ConnectionState::CONNECTING);  // newConnRecord2's state is CONNECTING
    serviceRecord_->AddConnectRecordToList(newConnRecord2);
    // generate the third connection record of callbackC
    auto callbackC = new AbilityConnectCallback();
    auto newConnRecord3 = ConnectionRecord::CreateConnectionRecord(serviceToken_, serviceRecord_, callbackC);
    newConnRecord3->SetConnectState(ConnectionState::CONNECTING);  // newConnRecord3's state is CONNECTING
    serviceRecord_->AddConnectRecordToList(newConnRecord3);
    auto connList = serviceRecord_->GetConnectRecordList();
    EXPECT_EQ(3, static_cast<int>(connList.size()));  // the connection list members should be three
    // start test
    auto callback = new AbilityConnectCallback();
    serviceRecord_->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    int ret = ConnectManager()->ScheduleConnectAbilityDoneLocked(serviceToken_, callback);
    EXPECT_EQ(OHOS::ERR_OK, ret);  // the result should be OK
    // connection callback should not be called, so check the count
    EXPECT_EQ(2, AbilityConnectCallback::onAbilityConnectDoneCount);
    EXPECT_EQ(0, AbilityConnectCallback::onAbilityDisconnectDoneCount);
}

/*
 * Feature: AbilityConnectManager
 * Function: DisconnectAbilityLocked
 * SubFunction:
 * FunctionPoints: DisconnectAbilityLocked and ConnectAbilityLocked
 * EnvConditions:NA
 * CaseDescription:Verify the following:
 * 1. Disconnect ability a nonexistent connect, disconnect failed
 * 2. If the current connect ability state is not connected, disconnect fails
 * 3. Verify the success of disconnect ability
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Kit_Disconnect_001, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto callback = new AbilityConnectCallback();
    auto result = ConnectManager()->DisconnectAbilityLocked(callback);
    EXPECT_EQ(result, OHOS::AAFwk::CONNECTION_NOT_EXIST);

    auto result1 = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result1);

    auto result2 = ConnectManager()->DisconnectAbilityLocked(callbackA_);
    EXPECT_EQ(result2, OHOS::AAFwk::INVALID_CONNECTION_STATE);

    auto list = ConnectManager()->GetConnectRecordListByCallback(callbackA_);
    EXPECT_EQ(static_cast<int>(list.size()), 1);

    for (auto &it : list) {
        it->SetConnectState(ConnectionState::CONNECTED);
    }

    auto result3 = ConnectManager()->DisconnectAbilityLocked(callbackA_);
    EXPECT_EQ(result3, OHOS::ERR_OK);
}

/*
 * Feature: AbilityConnectManager
 * Function: DisconnectAbilityLocked
 * SubFunction:
 * FunctionPoints: DisconnectAbilityLocked and ConnectAbilityLocked
 * EnvConditions:NA
 * CaseDescription: Results after verifying the disconnect ability
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Kit_Disconnect_002, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto result = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result);

    auto result1 = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackB_, nullptr);
    EXPECT_EQ(0, result1);

    auto result2 = ConnectManager()->ConnectAbilityLocked(abilityRequest1_, callbackA_, nullptr);
    EXPECT_EQ(0, result2);

    auto result3 = ConnectManager()->ConnectAbilityLocked(abilityRequest1_, callbackB_, nullptr);
    EXPECT_EQ(0, result3);

    auto listA = ConnectManager()->GetConnectRecordListByCallback(callbackA_);
    EXPECT_EQ(static_cast<int>(listA.size()), 2);

    for (auto &it : listA) {
        it->SetConnectState(ConnectionState::CONNECTED);
    }

    auto listB = ConnectManager()->GetConnectRecordListByCallback(callbackB_);
    EXPECT_EQ(static_cast<int>(listB.size()), 2);

    for (auto &it : listB) {
        it->SetConnectState(ConnectionState::CONNECTED);
    }

    auto result5 = ConnectManager()->DisconnectAbilityLocked(callbackA_);
    WaitUntilTaskDone(handler);
    EXPECT_EQ(result5, OHOS::ERR_OK);
    auto serviceMap = ConnectManager()->GetServiceMap();
    EXPECT_EQ(static_cast<int>(serviceMap.size()), 2);

    auto connectMap = ConnectManager()->GetConnectMap();
    EXPECT_EQ(static_cast<int>(connectMap.size()), 1);
    for (auto &it : connectMap) {
        EXPECT_EQ(static_cast<int>(it.second.size()), 2);
    }
}

/*
 * Feature: AbilityConnectManager
 * Function: OnAbilityRequestDone
 * SubFunction: NA
 * FunctionPoints: OnAbilityRequestDone
 * EnvConditions:NA
 * CaseDescription: Verify the ability status, right
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_018, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    auto token = abilityRecord->GetToken();

    auto connectMap = ConnectManager()->GetConnectMap();
    EXPECT_EQ(1, static_cast<int>(connectMap.size()));

    auto scheduler = new AbilityScheduler();
    ConnectManager()->AttachAbilityThreadLocked(scheduler, token->AsObject());

    // invalid parameter, direct return
    OHOS::sptr<OHOS::IRemoteObject> nullToken = nullptr;
    ConnectManager()->OnAbilityRequestDone(
        nullToken, static_cast<int32_t>(OHOS::AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND));

    ConnectManager()->OnAbilityRequestDone(
        token->AsObject(), static_cast<int32_t>(OHOS::AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND));
    WaitUntilTaskDone(handler);
    EXPECT_EQ(abilityRecord->GetAbilityState(), OHOS::AAFwk::AbilityState::INACTIVATING);

    ConnectManager()->OnAbilityRequestDone(
        token->AsObject(), static_cast<int32_t>(OHOS::AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND));
    WaitUntilTaskDone(handler);
    EXPECT_EQ(0, static_cast<int>(ConnectManager()->GetServiceMap().size()));
}

/*
 * Feature: AbilityConnectManager
 * Function: AbilityTransitionDone
 * SubFunction: NA
 * FunctionPoints: AbilityTransitionDone
 * EnvConditions:NA
 * CaseDescription: Verify the abilitytransitiondone process
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_019, TestSize.Level0)
{
    OHOS::sptr<OHOS::IRemoteObject> nullToken = nullptr;
    auto result = ConnectManager()->AbilityTransitionDone(nullToken, OHOS::AAFwk::AbilityState::INACTIVE);
    EXPECT_EQ(result, OHOS::ERR_INVALID_VALUE);

    ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    auto token = abilityRecord->GetToken();

    auto result1 = ConnectManager()->AbilityTransitionDone(token, OHOS::AAFwk::AbilityState::INACTIVE);
    EXPECT_EQ(result1, OHOS::ERR_INVALID_VALUE);

    auto result2 = ConnectManager()->AbilityTransitionDone(token, OHOS::AAFwk::AbilityState::INITIAL);
    EXPECT_EQ(result2, OHOS::ERR_OK);

    auto result3 = ConnectManager()->AbilityTransitionDone(token, OHOS::AAFwk::AbilityState::TERMINATING);
    EXPECT_EQ(result3, OHOS::ERR_INVALID_VALUE);
}

/*
 * Feature: AbilityConnectManager
 * Function: ScheduleDisconnectAbilityDoneLocked
 * SubFunction: NA
 * FunctionPoints: ScheduleDisconnectAbilityDoneLocked
 * EnvConditions:NA
 * CaseDescription: Verify the ScheduleDisconnectAbilityDoneLocked process
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_020, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    OHOS::sptr<OHOS::IRemoteObject> nullToken = nullptr;
    auto result = ConnectManager()->ScheduleDisconnectAbilityDoneLocked(nullToken);
    EXPECT_EQ(result, OHOS::AAFwk::CONNECTION_NOT_EXIST);

    std::shared_ptr<AbilityRecord> ability = nullptr;
    OHOS::sptr<OHOS::IRemoteObject> token1 = new OHOS::AAFwk::Token(ability);
    auto result1 = ConnectManager()->ScheduleDisconnectAbilityDoneLocked(token1);
    EXPECT_EQ(result1, OHOS::AAFwk::CONNECTION_NOT_EXIST);

    ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    auto token = abilityRecord->GetToken();

    auto listA = ConnectManager()->GetConnectRecordListByCallback(callbackA_);
    for (auto &it : listA) {
        it->SetConnectState(ConnectionState::CONNECTED);
    }

    auto result2 = ConnectManager()->DisconnectAbilityLocked(callbackA_);
    WaitUntilTaskDone(handler);
    EXPECT_EQ(result2, OHOS::ERR_OK);

    auto result3 = ConnectManager()->ScheduleDisconnectAbilityDoneLocked(token);
    EXPECT_EQ(result3, OHOS::AAFwk::INVALID_CONNECTION_STATE);

    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    auto result4 = ConnectManager()->ScheduleDisconnectAbilityDoneLocked(token);
    EXPECT_EQ(result4, OHOS::ERR_OK);
}

/*
 * Feature: AbilityConnectManager
 * Function: ScheduleCommandAbilityDoneLocked
 * SubFunction: NA
 * FunctionPoints: ScheduleCommandAbilityDoneLocked
 * EnvConditions:NA
 * CaseDescription: Verify the ScheduleCommandAbilityDoneLocked process
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_021, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    OHOS::sptr<OHOS::IRemoteObject> nullToken = nullptr;
    auto result = ConnectManager()->ScheduleCommandAbilityDoneLocked(nullToken);
    EXPECT_EQ(result, OHOS::ERR_INVALID_VALUE);

    std::shared_ptr<AbilityRecord> ability = nullptr;
    OHOS::sptr<OHOS::IRemoteObject> token1 = new OHOS::AAFwk::Token(ability);
    auto result1 = ConnectManager()->ScheduleCommandAbilityDoneLocked(token1);
    EXPECT_EQ(result1, OHOS::ERR_INVALID_VALUE);

    ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    auto token = abilityRecord->GetToken();

    auto result2 = ConnectManager()->ScheduleCommandAbilityDoneLocked(token);
    EXPECT_EQ(result2, OHOS::AAFwk::INVALID_CONNECTION_STATE);

    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    auto result3 = ConnectManager()->ScheduleCommandAbilityDoneLocked(token);
    EXPECT_EQ(result3, OHOS::ERR_OK);
}

/*
 * Feature: AbilityConnectManager
 * Function: GetServiceRecordByToken
 * SubFunction: NA
 * FunctionPoints: GetServiceRecordByToken
 * EnvConditions:NA
 * CaseDescription: Verify the GetServiceRecordByToken process
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_022, TestSize.Level0)
{
    ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    auto token = abilityRecord->GetToken();

    auto ability = ConnectManager()->GetServiceRecordByToken(token);
    EXPECT_EQ(abilityRecord, ability);

    OHOS::sptr<OHOS::IRemoteObject> nullToken = nullptr;
    auto ability1 = ConnectManager()->GetServiceRecordByToken(nullToken);
    EXPECT_EQ(nullptr, ability1);
}

/*
 * Feature: AbilityConnectManager
 * Function: RemoveAll
 * SubFunction: NA
 * FunctionPoints: RemoveAll
 * EnvConditions:NA
 * CaseDescription: Verify the RemoveAll process
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_023, TestSize.Level0)
{
    ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    auto token = abilityRecord->GetToken();

    auto conMap = ConnectManager()->GetConnectMap();
    EXPECT_EQ(conMap.empty(), false);
    EXPECT_EQ(serviceMap.empty(), false);

    ConnectManager()->RemoveAll();
    conMap = ConnectManager()->GetConnectMap();
    serviceMap = ConnectManager()->GetServiceMap();
    EXPECT_EQ(conMap.empty(), true);
    EXPECT_EQ(serviceMap.empty(), true);
}

/*
 * Feature: AbilityConnectManager
 * Function: OnAbilityDied
 * SubFunction:
 * FunctionPoints: OnAbilityDied
 * EnvConditions:NA
 * CaseDescription: Verify the OnAbilityDied process
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_024, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto result = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result);

    auto result1 = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackB_, nullptr);
    EXPECT_EQ(0, result1);

    auto result2 = ConnectManager()->ConnectAbilityLocked(abilityRequest1_, callbackA_, nullptr);
    EXPECT_EQ(0, result2);

    auto result3 = ConnectManager()->ConnectAbilityLocked(abilityRequest1_, callbackB_, nullptr);
    EXPECT_EQ(0, result3);

    auto listA = ConnectManager()->GetConnectRecordListByCallback(callbackA_);
    EXPECT_EQ(static_cast<int>(listA.size()), 2);

    for (auto &it : listA) {
        it->SetConnectState(ConnectionState::CONNECTED);
    }

    auto listB = ConnectManager()->GetConnectRecordListByCallback(callbackB_);
    EXPECT_EQ(static_cast<int>(listB.size()), 2);

    for (auto &it : listB) {
        it->SetConnectState(ConnectionState::CONNECTED);
    }

    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    auto token = abilityRecord->GetToken();

    ConnectManager()->OnAbilityDied(abilityRecord);
    WaitUntilTaskDone(handler);
    auto list = abilityRecord->GetConnectRecordList();
    EXPECT_EQ(static_cast<int>(list.size()), 0);

    auto elementName1 = abilityRequest1_.want.GetElement();
    std::string elementNameUri1 = elementName1.GetURI();
    serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord1 = serviceMap.at(elementNameUri1);
    auto token1 = abilityRecord1->GetToken();

    ConnectManager()->OnAbilityDied(abilityRecord1);
    WaitUntilTaskDone(handler);
    auto list1 = abilityRecord1->GetConnectRecordList();
    EXPECT_EQ(static_cast<int>(list1.size()), 0);
}

/*
 * Feature: AbilityConnectManager
 * Function: DispatchInactive
 * SubFunction:
 * FunctionPoints: DispatchInactive
 * EnvConditions:NA
 * CaseDescription: Verify the DispatchInactive process
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_025, TestSize.Level0)
{
    std::shared_ptr<AbilityRecord> ability = nullptr;
    auto result = ConnectManager()->DispatchInactive(ability, OHOS::AAFwk::AbilityState::INACTIVATING);
    EXPECT_EQ(result, OHOS::ERR_INVALID_VALUE);

    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto result3 = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result3);

    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    auto token = abilityRecord->GetToken();

    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    auto result1 = ConnectManager()->DispatchInactive(abilityRecord, OHOS::AAFwk::AbilityState::INACTIVATING);
    EXPECT_EQ(result1, OHOS::ERR_INVALID_VALUE);

    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
    auto result2 = ConnectManager()->DispatchInactive(abilityRecord, OHOS::AAFwk::AbilityState::INACTIVATING);
    EXPECT_EQ(result2, OHOS::ERR_OK);
    EXPECT_EQ(abilityRecord->GetAbilityState(), OHOS::AAFwk::AbilityState::INACTIVE);
}

/*
 * Feature: AbilityConnectManager
 * Function: DispatchInactive
 * SubFunction:
 * FunctionPoints: DispatchInactive
 * EnvConditions:NA
 * CaseDescription: Verify the DispatchInactive process
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_026, TestSize.Level0)
{
    std::shared_ptr<AbilityRecord> ability = nullptr;
    auto result = ConnectManager()->DispatchInactive(ability, OHOS::AAFwk::AbilityState::INACTIVATING);
    EXPECT_EQ(result, OHOS::ERR_INVALID_VALUE);

    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto result3 = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    EXPECT_EQ(0, result3);

    auto elementName = abilityRequest_.want.GetElement();
    std::string elementNameUri = elementName.GetURI();
    auto serviceMap = ConnectManager()->GetServiceMap();
    auto abilityRecord = serviceMap.at(elementNameUri);
    auto token = abilityRecord->GetToken();

    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    auto result1 = ConnectManager()->DispatchInactive(abilityRecord, OHOS::AAFwk::AbilityState::INACTIVATING);
    EXPECT_EQ(result1, OHOS::ERR_INVALID_VALUE);

    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVATING);
    auto result2 = ConnectManager()->DispatchInactive(abilityRecord, OHOS::AAFwk::AbilityState::INACTIVATING);
    EXPECT_EQ(result2, OHOS::ERR_OK);
    EXPECT_EQ(abilityRecord->GetAbilityState(), OHOS::AAFwk::AbilityState::INACTIVE);
}

/*
 * Feature: AbilityConnectManager
 * Function: AddConnectDeathRecipient
 * SubFunction:
 * FunctionPoints: AddConnectDeathRecipient
 * EnvConditions:NA
 * CaseDescription: Verify the AddConnectDeathRecipient process
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_027, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    ConnectManager()->AddConnectDeathRecipient(nullptr);
    EXPECT_TRUE(ConnectManager()->recipientMap_.empty());

    ConnectManager()->AddConnectDeathRecipient(callbackA_);
    EXPECT_EQ(static_cast<int>(ConnectManager()->recipientMap_.size()), 1);

    // Add twice, do not add repeatedly
    ConnectManager()->AddConnectDeathRecipient(callbackA_);
    EXPECT_EQ(static_cast<int>(ConnectManager()->recipientMap_.size()), 1);
}

/*
 * Feature: AbilityConnectManager
 * Function: RemoveConnectDeathRecipient
 * SubFunction:
 * FunctionPoints: RemoveConnectDeathRecipient
 * EnvConditions:NA
 * CaseDescription: Verify the RemoveConnectDeathRecipient process
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_028, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    ConnectManager()->AddConnectDeathRecipient(callbackA_);
    EXPECT_EQ(static_cast<int>(ConnectManager()->recipientMap_.size()), 1);

    ConnectManager()->RemoveConnectDeathRecipient(nullptr);
    EXPECT_FALSE(ConnectManager()->recipientMap_.empty());

    ConnectManager()->RemoveConnectDeathRecipient(callbackA_);
    EXPECT_TRUE(ConnectManager()->recipientMap_.empty());
}

/*
 * Feature: AbilityConnectManager
 * Function: OnCallBackDied
 * SubFunction:
 * FunctionPoints: OnCallBackDied
 * EnvConditions:NA
 * CaseDescription: Verify the OnCallBackDied process
 */
HWTEST_F(AbilityConnectManagerTest, AAFWK_Connect_Service_029, TestSize.Level0)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    ConnectManager()->SetEventHandler(handler);

    auto result = ConnectManager()->ConnectAbilityLocked(abilityRequest_, callbackA_, nullptr);
    WaitUntilTaskDone(handler);
    EXPECT_EQ(0, result);

    ConnectManager()->OnCallBackDied(nullptr);
    WaitUntilTaskDone(handler);
    auto connectMap = ConnectManager()->GetConnectMap();
    auto connectRecordList = connectMap.at(callbackA_->AsObject());
    EXPECT_EQ(1, static_cast<int>(connectRecordList.size()));
    for (auto &it : connectRecordList) {
        EXPECT_NE(it->GetAbilityConnectCallback(), nullptr);
    }

    ConnectManager()->OnCallBackDied(callbackA_->AsObject());
    WaitUntilTaskDone(handler);
    auto cMap = ConnectManager()->GetConnectMap();
    connectRecordList = connectMap.at(callbackA_->AsObject());
    EXPECT_EQ(1, static_cast<int>(connectMap.size()));
    for (auto &it : connectRecordList) {
        EXPECT_EQ(it->GetAbilityConnectCallback(), nullptr);
    }
}
}  // namespace AAFwk
}  // namespace OHOS