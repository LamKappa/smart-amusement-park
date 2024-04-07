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

#include "ability_connect_callback_proxy.h"
#include "mock_ability_connect_callback_stub.h"

using namespace testing::ext;
using namespace testing;
namespace OHOS {
namespace AAFwk {

class AbilityConnectCallBackProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<AbilityConnectionProxy> proxy_;
};

void AbilityConnectCallBackProxyTest::SetUpTestCase(void)
{}
void AbilityConnectCallBackProxyTest::TearDownTestCase(void)
{}
void AbilityConnectCallBackProxyTest::SetUp()
{}
void AbilityConnectCallBackProxyTest::TearDown()
{}

/*
 * Feature: AAFwk
 * Function: AbilityConnectionProxy
 * SubFunction: IPC of client and server
 * FunctionPoints: OnAbilityConnectDone
 * EnvConditions: NA
 * CaseDescription: verify OnAbilityConnectDone IPC between client and server.
 */
HWTEST_F(AbilityConnectCallBackProxyTest, AbilityConnectionCallBack_IPC_001, TestSize.Level1)
{
    sptr<MockAbilityConnectCallback> mockAbilityConnectStub(new MockAbilityConnectCallback());
    sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(mockAbilityConnectStub));
    AppExecFwk::ElementName element;
    sptr<IRemoteObject> remoteObject;
    EXPECT_CALL(*mockAbilityConnectStub, OnAbilityConnectDone(_, _, _))
        .Times(1)
        .WillOnce(InvokeWithoutArgs(mockAbilityConnectStub.GetRefPtr(), &MockAbilityConnectCallback::PostVoid));
    callback->OnAbilityConnectDone(element, remoteObject, 0);
    mockAbilityConnectStub->Wait();
}

/*
 * Feature: AAFwk
 * Function: AbilityConnectionProxy
 * SubFunction: IPC of client and server
 * FunctionPoints: OnAbilityDisconnectDone
 * EnvConditions: NA
 * CaseDescription: verify OnAbilityDisconnectDone IPC between client and server.
 */
HWTEST_F(AbilityConnectCallBackProxyTest, AbilityConnectionCallBack_IPC_002, TestSize.Level1)
{
    sptr<MockAbilityConnectCallback> mockAbilityConnectStub(new MockAbilityConnectCallback());
    sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(mockAbilityConnectStub));
    AppExecFwk::ElementName element;
    EXPECT_CALL(*mockAbilityConnectStub, OnAbilityDisconnectDone(_, _))
        .Times(1)
        .WillOnce(InvokeWithoutArgs(mockAbilityConnectStub.GetRefPtr(), &MockAbilityConnectCallback::PostVoid));
    callback->OnAbilityDisconnectDone(element, 0);
    mockAbilityConnectStub->Wait();
}

/*
 * Feature: AAFwk
 * Function: AbilityConnectionProxy
 * SubFunction: IPC of client and server
 * FunctionPoints: instance
 * EnvConditions: NA
 * CaseDescription: instance is success.
 */
HWTEST_F(AbilityConnectCallBackProxyTest, AbilityConnectionCallBack_IPC_003, TestSize.Level1)
{
    sptr<MockAbilityConnectCallback> mockAbilityConnectStub(new MockAbilityConnectCallback());
    sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(mockAbilityConnectStub));
    EXPECT_NE(callback, nullptr);
}
}  // namespace AAFwk
}  // namespace OHOS