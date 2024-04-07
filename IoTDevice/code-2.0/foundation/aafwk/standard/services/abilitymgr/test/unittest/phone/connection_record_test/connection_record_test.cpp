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
#include <gmock/gmock.h>

#define private public
#define protected public
#include "ability_record.h"
#include "connection_record.h"
#undef private
#undef protected

#include "ability_manager_errors.h"
#include "ability_connect_callback_stub.h"
#include "ability_scheduler.h"
#include "ability_record_info.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
class AbilityConnectCallbackMock : public AbilityConnectionStub {
public:
    AbilityConnectCallbackMock()
    {}
    virtual ~AbilityConnectCallbackMock()
    {}

    MOCK_METHOD3(OnAbilityConnectDone, void(const ElementName &, const OHOS::sptr<IRemoteObject> &, int));
    MOCK_METHOD2(OnAbilityDisconnectDone, void(const ElementName &, int));
};

class ConnectionRecordTest : public testing::TestWithParam<OHOS::AAFwk::ConnectionState> {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    AbilityRequest GenerateAbilityRequest(const std::string &deviceName, const std::string &abilityName,
        const std::string &appName, const std::string &bundleName);

    std::shared_ptr<ConnectionRecord> connectionRecord_;
    OHOS::sptr<AbilityConnectCallbackMock> callback_;
    std::shared_ptr<AbilityRecord> service_;
};

AbilityRequest ConnectionRecordTest::GenerateAbilityRequest(const std::string &deviceName,
    const std::string &abilityName, const std::string &appName, const std::string &bundleName)
{
    ElementName element(deviceName, bundleName, abilityName);
    Want want;
    want.SetElement(element);

    AbilityInfo abilityInfo;
    abilityInfo.applicationName = appName;
    abilityInfo.bundleName = bundleName;
    abilityInfo.type = AbilityType::SERVICE;
    ApplicationInfo appinfo;
    appinfo.name = appName;

    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = abilityInfo;
    abilityRequest.appInfo = appinfo;

    return abilityRequest;
}

void ConnectionRecordTest::SetUpTestCase(void)
{}
void ConnectionRecordTest::TearDownTestCase(void)
{}

void ConnectionRecordTest::SetUp(void)
{
    callback_ = new AbilityConnectCallbackMock();

    std::string deviceName = "device";
    std::string abilityName = "ServiceAbility";
    std::string appName = "hiservcie";
    std::string bundleName = "com.ix.hiservcie";
    AbilityRequest abilityRequest = GenerateAbilityRequest(deviceName, abilityName, appName, bundleName);
    service_ = AbilityRecord::CreateAbilityRecord(abilityRequest);
    connectionRecord_ = std::make_shared<ConnectionRecord>(service_->GetToken(), service_, callback_);
}

void ConnectionRecordTest::TearDown(void)
{
    connectionRecord_.reset();
}

bool IsExist(const std::string &state)
{
    return std::string::npos != state.find("com.ix.hiservcie");
}

/*
 * Feature: ConnectionRecord
 * Function: SetConnectState and GetConnectState
 * SubFunction: NA
 * FunctionPoints: SetConnectState and GetConnectState
 * EnvConditions:NA
 * CaseDescription: Verify set and get
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_001, TestSize.Level1)
{
    connectionRecord_->SetConnectState(ConnectionState::CONNECTED);
    EXPECT_EQ(connectionRecord_->GetConnectState(), ConnectionState::CONNECTED);
}

/*
 * Feature: ConnectionRecord
 * Function: GetToken
 * SubFunction: NA
 * FunctionPoints: GetToken
 * EnvConditions:NA
 * CaseDescription: Verify that the tokens are equal
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_002, TestSize.Level1)
{
    EXPECT_EQ(connectionRecord_->GetToken().GetRefPtr(), service_->GetToken().GetRefPtr());
}

/*
 * Feature: ConnectionRecord
 * Function: GetAbilityRecord
 * SubFunction: NA
 * FunctionPoints: GetAbilityRecord
 * EnvConditions:NA
 * CaseDescription: Verify that the ability record are equal
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_003, TestSize.Level1)
{
    EXPECT_EQ(connectionRecord_->GetAbilityRecord(), service_);
}

/*
 * Feature: ConnectionRecord
 * Function: GetAbilityConnectCallback
 * SubFunction: NA
 * FunctionPoints: GetAbilityConnectCallback
 * EnvConditions:NA
 * CaseDescription: Verify that the call back are equal
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_004, TestSize.Level1)
{
    EXPECT_EQ(connectionRecord_->GetAbilityConnectCallback(), iface_cast<IAbilityConnection>(callback_));
}

/*
 * Feature: ConnectionRecord
 * Function: DisconnectAbility
 * SubFunction: NA
 * FunctionPoints: DisconnectAbility
 * EnvConditions:NA
 * CaseDescription: 1.Connection state is not connected, DisconnectAbility failed
 * 2.Verify the correct process of disconnectability
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_005, TestSize.Level1)
{
    auto result = connectionRecord_->DisconnectAbility();
    EXPECT_EQ(result, INVALID_CONNECTION_STATE);

    connectionRecord_->SetConnectState(ConnectionState::CONNECTED);

    result = connectionRecord_->DisconnectAbility();
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(connectionRecord_->GetConnectState(), ConnectionState::DISCONNECTED);
}

/*
 * Feature: ConnectionRecord
 * Function: CompleteConnect
 * SubFunction: NA
 * FunctionPoints: CompleteConnect
 * EnvConditions:NA
 * CaseDescription: Verify the correct process of completeconnect
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_006, TestSize.Level1)
{
    EXPECT_CALL(*callback_, OnAbilityConnectDone(::testing::_, ::testing::_, ::testing::_)).Times(1);

    connectionRecord_->CompleteConnect(ERR_OK);
    EXPECT_EQ(connectionRecord_->GetConnectState(), ConnectionState::CONNECTED);
    EXPECT_EQ(service_->GetAbilityState(), AAFwk::AbilityState::ACTIVE);
}

/*
 * Feature: ConnectionRecord
 * Function: CompleteDisconnect
 * SubFunction: NA
 * FunctionPoints: CompleteDisconnect
 * EnvConditions:NA
 * CaseDescription: Verify the correct process of complete disconnect
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_007, TestSize.Level1)
{
    EXPECT_CALL(*callback_, OnAbilityDisconnectDone(::testing::_, ::testing::_)).Times(1);

    connectionRecord_->CompleteDisconnect(ERR_OK);
    EXPECT_EQ(connectionRecord_->GetConnectState(), ConnectionState::DISCONNECTED);
}

/*
 * Feature: ConnectionRecord
 * Function: ScheduleDisconnectAbilityDone
 * SubFunction: NA
 * FunctionPoints: ScheduleDisconnectAbilityDone
 * EnvConditions:NA
 * CaseDescription: 1.Connection state is not DISCONNECTING, Onabilitydisconnectdone is not called
 * 2.Connection state is DISCONNECTING, Call onabilitydisconnect done
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_008, TestSize.Level1)
{
    EXPECT_CALL(*callback_, OnAbilityDisconnectDone(::testing::_, ::testing::_)).Times(1);

    connectionRecord_->SetConnectState(ConnectionState::CONNECTED);
    connectionRecord_->ScheduleDisconnectAbilityDone();

    connectionRecord_->SetConnectState(ConnectionState::DISCONNECTING);
    connectionRecord_->ScheduleDisconnectAbilityDone();
}

/*
 * Feature: ConnectionRecord
 * Function: ScheduleConnectAbilityDone
 * SubFunction: NA
 * FunctionPoints: ScheduleConnectAbilityDone
 * EnvConditions:NA
 * CaseDescription: 1.Connection state is not CONNECTING, OnAbilityConnectDone is not called
 * 2.Connection state is CONNECTING, Call OnAbilityConnectDone done
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_009, TestSize.Level1)
{
    EXPECT_CALL(*callback_, OnAbilityConnectDone(::testing::_, ::testing::_, ::testing::_)).Times(1);

    connectionRecord_->SetConnectState(ConnectionState::DISCONNECTING);
    connectionRecord_->ScheduleConnectAbilityDone();

    connectionRecord_->SetConnectState(ConnectionState::CONNECTING);
    connectionRecord_->ScheduleConnectAbilityDone();
}

/*
 * Feature: ConnectionRecord
 * Function: GetRecordId
 * SubFunction: NA
 * FunctionPoints: GetRecordId
 * EnvConditions:NA
 * CaseDescription: Verify that getrecordids are equal
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_010, TestSize.Level1)
{
    EXPECT_EQ(connectionRecord_->GetRecordId(), 9);
}

/*
 * Feature: ConnectionRecord
 * Function: GetAbilityConnectCallback
 * SubFunction: NA
 * FunctionPoints: GetAbilityConnectCallback
 * EnvConditions:NA
 * CaseDescription: Verify that getabilityconnectcallback is nullptr
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_011, TestSize.Level1)
{
    connectionRecord_->ClearConnCallBack();
    EXPECT_EQ(connectionRecord_->GetAbilityConnectCallback().GetRefPtr(), nullptr);
}

/*
 * Feature: ConnectionRecord
 * Function: ConvertConnectionState
 * SubFunction: NA
 * FunctionPoints: ConvertConnectionState
 * EnvConditions:NA
 * CaseDescription: Verify ConvertConnectionState results
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_012, TestSize.Level1)
{
    auto res = connectionRecord_->ConvertConnectionState(ConnectionState::INIT);
    EXPECT_EQ(res, "INIT");
    res = connectionRecord_->ConvertConnectionState(ConnectionState::CONNECTING);
    EXPECT_EQ(res, "CONNECTING");
    res = connectionRecord_->ConvertConnectionState(ConnectionState::CONNECTED);
    EXPECT_EQ(res, "CONNECTED");
    res = connectionRecord_->ConvertConnectionState(ConnectionState::DISCONNECTING);
    EXPECT_EQ(res, "DISCONNECTING");
    res = connectionRecord_->ConvertConnectionState(ConnectionState::DISCONNECTED);
    EXPECT_EQ(res, "DISCONNECTED");
}

/*
 * Feature: ConnectionRecord
 * Function: Dump
 * SubFunction: NA
 * FunctionPoints: Dump
 * EnvConditions:NA
 * CaseDescription: Verify dump results
 */
HWTEST_F(ConnectionRecordTest, AaFwk_ConnectionRecord_013, TestSize.Level1)
{
    std::vector<std::string> info;
    connectionRecord_->Dump(info);

    for (auto &it : info) {
        GTEST_LOG_(INFO) << it;
    }

    EXPECT_NE(info.end(), std::find_if(info.begin(), info.end(), IsExist));
}
}  // namespace AAFwk
}  // namespace OHOS
