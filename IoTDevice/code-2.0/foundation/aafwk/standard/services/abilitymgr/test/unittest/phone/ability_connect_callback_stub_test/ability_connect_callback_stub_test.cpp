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
#include "iremote_proxy.h"
#include "mock_ability_connect_callback_stub.h"
#include "ability_connect_callback_proxy.h"

using namespace testing::ext;
using namespace OHOS::AAFwk;
using namespace OHOS;
using namespace testing;

class AbilityConnectionStubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    void WriteInterfaceToken(MessageParcel &data);
};

void AbilityConnectionStubTest::SetUpTestCase(void)
{}
void AbilityConnectionStubTest::TearDownTestCase(void)
{}
void AbilityConnectionStubTest::SetUp()
{}
void AbilityConnectionStubTest::TearDown()
{}

void AbilityConnectionStubTest::WriteInterfaceToken(MessageParcel &data)
{
    data.WriteInterfaceToken(AbilityConnectionProxy::GetDescriptor());
}

/*
 * Feature: AbilityConnectionStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityConnectionStub OnRemoteRequest
 * EnvConditions: ElementName is nullptr
 * CaseDescription: Verify that on remote request is normal and abnormal
 */
HWTEST_F(AbilityConnectionStubTest, AbilityConnectionCallBack_IPC_001, TestSize.Level0)
{
    sptr<MockAbilityConnectCallback> mockAbilityConnectStub(new MockAbilityConnectCallback());

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    sptr<IRemoteObject> remoteObject;
    int32_t reqCode = 10;

    WriteInterfaceToken(data);
    data.WriteParcelable(nullptr);
    data.WriteParcelable(remoteObject);
    data.WriteInt32(reqCode);

    int res = mockAbilityConnectStub->OnRemoteRequest(IAbilityConnection::ON_ABILITY_CONNECT_DONE, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_VALUE);
}

/*
 * Feature: AbilityConnectionStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityConnectionStub OnRemoteRequest
 * EnvConditions: ElementName is not nullptr
 * CaseDescription: Verify that on remote request is normal and abnormal
 */
HWTEST_F(AbilityConnectionStubTest, AbilityConnectionCallBack_IPC_002, TestSize.Level0)
{
    sptr<MockAbilityConnectCallback> mockAbilityConnectStub(new MockAbilityConnectCallback());

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    auto element = std::make_shared<AppExecFwk::ElementName>("bundlename", "appname", "abilityname");
    sptr<IRemoteObject> remoteObject;
    int32_t reqCode = 10;

    WriteInterfaceToken(data);
    data.WriteParcelable(element.get());
    data.WriteParcelable(remoteObject);
    data.WriteInt32(reqCode);

    EXPECT_CALL(*mockAbilityConnectStub, OnAbilityConnectDone(_, _, _)).Times(1);
    int res = mockAbilityConnectStub->OnRemoteRequest(IAbilityConnection::ON_ABILITY_CONNECT_DONE, data, reply, option);
    EXPECT_EQ(res, ERR_OK);
}

/*
 * Feature: AbilityConnectionStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityConnectionStub OnRemoteRequest
 * EnvConditions: ElementName is nullptr
 * CaseDescription: Verify that on remote request is normal and abnormal
 */
HWTEST_F(AbilityConnectionStubTest, AbilityConnectionCallBack_IPC_003, TestSize.Level0)
{
    sptr<MockAbilityConnectCallback> mockAbilityConnectStub(new MockAbilityConnectCallback());

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    sptr<IRemoteObject> remoteObject;
    int32_t reqCode = 10;

    WriteInterfaceToken(data);
    data.WriteParcelable(nullptr);
    data.WriteInt32(reqCode);

    int res =
        mockAbilityConnectStub->OnRemoteRequest(IAbilityConnection::ON_ABILITY_DISCONNECT_DONE, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_VALUE);
}

/*
 * Feature: AbilityConnectionStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityConnectionStub OnRemoteRequest
 * EnvConditions: ElementName is not nullptr
 * CaseDescription: Verify that on remote request is normal and abnormal
 */
HWTEST_F(AbilityConnectionStubTest, AbilityConnectionCallBack_IPC_004, TestSize.Level0)
{
    sptr<MockAbilityConnectCallback> mockAbilityConnectStub(new MockAbilityConnectCallback());

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    auto element = std::make_shared<AppExecFwk::ElementName>("bundlename", "appname", "abilityname");
    sptr<IRemoteObject> remoteObject;
    int32_t reqCode = 10;

    WriteInterfaceToken(data);
    data.WriteParcelable(element.get());
    data.WriteInt32(reqCode);

    EXPECT_CALL(*mockAbilityConnectStub, OnAbilityDisconnectDone(_, _)).Times(1);
    int res =
        mockAbilityConnectStub->OnRemoteRequest(IAbilityConnection::ON_ABILITY_DISCONNECT_DONE, data, reply, option);
    EXPECT_EQ(res, ERR_OK);
}