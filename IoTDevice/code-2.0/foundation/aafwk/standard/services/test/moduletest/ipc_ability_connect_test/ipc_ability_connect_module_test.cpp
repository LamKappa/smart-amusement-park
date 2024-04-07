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

#include "gtest/gtest.h"
#include <unistd.h>

#include "mock_ability_connect_callback_stub.h"
#include "ability_connect_callback_proxy.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using OHOS::iface_cast;
using OHOS::sptr;
using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;

namespace OHOS {
namespace AAFwk {
class IpcAbilityConnectModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static constexpr int COUNT = 5;
};

void IpcAbilityConnectModuleTest::SetUpTestCase()
{}

void IpcAbilityConnectModuleTest::TearDownTestCase()
{}

void IpcAbilityConnectModuleTest::SetUp()
{}

void IpcAbilityConnectModuleTest::TearDown()
{}

/*
 * Feature: AAFwk
 * Function: AbilityConnect
 * SubFunction: IPC of client and server
 * FunctionPoints: OnAbilityConnectDone
 * EnvConditions: NA
 * CaseDescription: verify OnAbilityConnectDone IPC between client and server.
 */
HWTEST_F(IpcAbilityConnectModuleTest, AbilityConnectCallBack_IPC_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityConnectModuleTest AbilityConnectCallBack_IPC_001 start";
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityConnectCallbackStub> mockAbilityConnectStub(new MockAbilityConnectCallbackStub());
        sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(mockAbilityConnectStub));

        AppExecFwk::ElementName element;
        sptr<IRemoteObject> remoteObject;
        EXPECT_CALL(*mockAbilityConnectStub, OnAbilityConnectDone(_, _, _))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityConnectStub.GetRefPtr(), &MockAbilityConnectCallbackStub::PostVoid));
        callback->OnAbilityConnectDone(element, remoteObject, 0);

        mockAbilityConnectStub->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityConnectModuleTest AbilityConnectCallBack_IPC_001 end";
}

/*
 * Feature: AAFwk
 * Function: AbilityConnect
 * SubFunction: IPC of client and server
 * FunctionPoints: OnAbilityDisconnectDone
 * EnvConditions: NA
 * CaseDescription: verify OnAbilityDisconnectDone IPC between client and server.
 */
HWTEST_F(IpcAbilityConnectModuleTest, AbilityConnectCallBack_IPC_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IpcAbilityConnectModuleTest AbilityConnectCallBack_IPC_002 start";
    for (int i = 0; i < COUNT; i++) {
        sptr<MockAbilityConnectCallbackStub> mockAbilityConnectStub(new MockAbilityConnectCallbackStub());
        sptr<AbilityConnectionProxy> callback(new AbilityConnectionProxy(mockAbilityConnectStub));

        AppExecFwk::ElementName element;
        EXPECT_CALL(*mockAbilityConnectStub, OnAbilityDisconnectDone(_, _))
            .Times(1)
            .WillOnce(InvokeWithoutArgs(mockAbilityConnectStub.GetRefPtr(), &MockAbilityConnectCallbackStub::PostVoid));
        callback->OnAbilityDisconnectDone(element, 0);

        mockAbilityConnectStub->Wait();
    }

    GTEST_LOG_(INFO) << "IpcAbilityConnectModuleTest AbilityConnectCallBack_IPC_002 end";
}
}  // namespace AAFwk
}  // namespace OHOS