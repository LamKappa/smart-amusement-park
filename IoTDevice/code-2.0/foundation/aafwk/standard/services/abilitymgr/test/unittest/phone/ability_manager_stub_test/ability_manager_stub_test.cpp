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
#include "ability_manager_stub_impl_mock.h"
#include "ability_scheduler.h"
#include "mock_ability_connect_callback.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace AAFwk {
class AbilityManagerStubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void WriteInterfaceToken(MessageParcel &data);
    sptr<AbilityManagerStubImplMock> stub_;
};

void AbilityManagerStubTest::SetUpTestCase(void)
{}
void AbilityManagerStubTest::TearDownTestCase(void)
{}
void AbilityManagerStubTest::TearDown()
{}

void AbilityManagerStubTest::SetUp()
{
    stub_ = new AbilityManagerStubImplMock();
}

void AbilityManagerStubTest::WriteInterfaceToken(MessageParcel &data)
{
    data.WriteInterfaceToken(AbilityManagerStub::GetDescriptor());
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is START_ABILITY
 * CaseDescription: Verify that on remote request is normal and abnormal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_001, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    Want want;
    WriteInterfaceToken(data);
    want.SetFlags(10);
    data.WriteParcelable(&want);
    data.WriteInt32(1);
    int res = stub_->OnRemoteRequest(IAbilityManager::START_ABILITY, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);

    data.WriteParcelable(nullptr);
    data.WriteInt32(1);
    int res1 = stub_->OnRemoteRequest(IAbilityManager::START_ABILITY, data, reply, option);
    EXPECT_NE(res1, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is TERMINATE_ABILITY
 * CaseDescription: Verify that on remote request is normal and abnormal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_002, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    Want want;
    want.SetFlags(10);
    OHOS::sptr<IRemoteObject> token = nullptr;
    WriteInterfaceToken(data);
    data.WriteParcelable(token);
    data.WriteParcelable(&want);
    int res = stub_->OnRemoteRequest(IAbilityManager::TERMINATE_ABILITY, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is CONNECT_ABILITY
 * CaseDescription: Verify that on remote request is normal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_004, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    Want want;
    want.SetFlags(10);
    sptr<IAbilityConnection> connect = new AbilityConnectCallback();
    sptr<IRemoteObject> callerToken = nullptr;
    WriteInterfaceToken(data);
    data.WriteParcelable(&want);
    data.WriteParcelable(connect->AsObject());
    data.WriteParcelable(callerToken);
    int res = stub_->OnRemoteRequest(IAbilityManager::CONNECT_ABILITY, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: OnRemoteRequest IAbilityManager::CONNECT_ABILITY
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_005, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    Want want;
    want.SetFlags(10);
    sptr<IAbilityConnection> connect = new AbilityConnectCallback();
    sptr<IRemoteObject> callerToken = nullptr;
    WriteInterfaceToken(data);
    data.WriteParcelable(&want);
    data.WriteParcelable(connect->AsObject());
    data.WriteParcelable(callerToken);
    int res = stub_->OnRemoteRequest(IAbilityManager::CONNECT_ABILITY, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: OnRemoteRequest IAbilityManager::CONNECT_ABILITY
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_006, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    sptr<IAbilityConnection> connect = new AbilityConnectCallback();
    sptr<IRemoteObject> callerToken = nullptr;
    WriteInterfaceToken(data);
    data.WriteParcelable(nullptr);
    data.WriteParcelable(connect->AsObject());
    data.WriteParcelable(callerToken);
    int res = stub_->OnRemoteRequest(IAbilityManager::CONNECT_ABILITY, data, reply, option);

    EXPECT_NE(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is DISCONNECT_ABILITY
 * CaseDescription: Verify that on remote request is normal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_007, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    sptr<IAbilityConnection> connect = new AbilityConnectCallback();
    WriteInterfaceToken(data);
    data.WriteParcelable(connect->AsObject());
    int res = stub_->OnRemoteRequest(IAbilityManager::DISCONNECT_ABILITY, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is ATTACH_ABILITY_THREAD
 * CaseDescription: Verify that on remote request is normal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_008, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    sptr<IAbilityScheduler> scheduler = new AbilityScheduler();
    sptr<IRemoteObject> token = nullptr;
    WriteInterfaceToken(data);
    data.WriteParcelable(scheduler->AsObject());
    data.WriteParcelable(token);
    int res = stub_->OnRemoteRequest(IAbilityManager::ATTACH_ABILITY_THREAD, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is ABILITY_TRANSITION_DONE
 * CaseDescription: Verify that on remote request is normal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_009, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    sptr<IRemoteObject> token = nullptr;
    WriteInterfaceToken(data);
    data.WriteParcelable(token);
    data.WriteInt32(1);
    int res = stub_->OnRemoteRequest(IAbilityManager::ABILITY_TRANSITION_DONE, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is CONNECT_ABILITY_DONE
 * CaseDescription: Verify that on remote request is normal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_010, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    sptr<IRemoteObject> token = nullptr;
    sptr<IRemoteObject> remoteObject = nullptr;
    WriteInterfaceToken(data);
    data.WriteParcelable(token);
    data.WriteParcelable(remoteObject);
    int res = stub_->OnRemoteRequest(IAbilityManager::CONNECT_ABILITY_DONE, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is DISCONNECT_ABILITY_DONE
 * CaseDescription: Verify that on remote request is normal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_011, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    sptr<IRemoteObject> token = nullptr;
    WriteInterfaceToken(data);
    data.WriteParcelable(token);
    int res = stub_->OnRemoteRequest(IAbilityManager::DISCONNECT_ABILITY_DONE, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is ADD_WINDOW_INFO
 * CaseDescription: Verify that on remote request is normal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_012, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    sptr<IRemoteObject> token = nullptr;
    WriteInterfaceToken(data);
    data.WriteParcelable(token);
    data.WriteInt32(1);
    int res = stub_->OnRemoteRequest(IAbilityManager::ADD_WINDOW_INFO, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is DUMP_STATE
 * CaseDescription: Verify that on remote request is normal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_013, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    std::string args = "aaa";
    std::vector<std::string> info;
    WriteInterfaceToken(data);
    data.WriteString16(Str8ToStr16(args));
    int res = stub_->OnRemoteRequest(IAbilityManager::DUMP_STATE, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is LIST_STACK_INFO
 * CaseDescription: Verify that on remote request is normal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_014, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WriteInterfaceToken(data);
    int res = stub_->OnRemoteRequest(IAbilityManager::LIST_STACK_INFO, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is TERMINATE_ABILITY_RESULT
 * CaseDescription: Verify that on remote request is normal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_015, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WriteInterfaceToken(data);
    int res = stub_->OnRemoteRequest(IAbilityManager::TERMINATE_ABILITY_RESULT, data, reply, option);

    EXPECT_EQ(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is default
 * CaseDescription: Verify that on remote request is normal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_016, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WriteInterfaceToken(data);
    int res = stub_->OnRemoteRequest(5000, data, reply, option);

    EXPECT_NE(res, NO_ERROR);
}

/*
 * Feature: AbilityManagerService
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: AbilityManagerService OnRemoteRequest
 * EnvConditions: code is GET_RECENT_MISSION
 * CaseDescription: Verify that on remote request is normal
 */
HWTEST_F(AbilityManagerStubTest, AbilityManagerStub_017, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WriteInterfaceToken(data);
    data.WriteInt32(INT_MAX);
    data.WriteInt32(1);
    int res = stub_->OnRemoteRequest(IAbilityManager::GET_RECENT_MISSION, data, reply, option);

    std::vector<RecentMissionInfo> recentList;
    int size = reply.ReadInt32();
    for (int32_t i = 0; i < size; i++) {
        std::unique_ptr<RecentMissionInfo> info(reply.ReadParcelable<RecentMissionInfo>());
        recentList.emplace_back(*info);
    }

    EXPECT_EQ(static_cast<int>(recentList.size()), 1);
    EXPECT_EQ(recentList[0].runingState, -1);
    EXPECT_EQ(recentList[0].missionDescription.label, "label");
    EXPECT_EQ(recentList[0].missionDescription.iconPath, "icon path");
    EXPECT_EQ(recentList[0].baseWant.GetElement().GetAbilityName(), "baseAbility");
    EXPECT_EQ(recentList[0].baseWant.GetElement().GetBundleName(), "baseBundle");
    EXPECT_EQ(recentList[0].baseWant.GetElement().GetDeviceID(), "baseDevice");

    EXPECT_EQ(recentList[0].baseAbility.GetAbilityName(), "baseAbility");
    EXPECT_EQ(recentList[0].baseAbility.GetBundleName(), "baseBundle");
    EXPECT_EQ(recentList[0].baseAbility.GetDeviceID(), "baseDevice");

    EXPECT_EQ(recentList[0].topAbility.GetAbilityName(), "topAbility");
    EXPECT_EQ(recentList[0].topAbility.GetBundleName(), "topBundle");
    EXPECT_EQ(recentList[0].topAbility.GetDeviceID(), "topDevice");

    EXPECT_EQ(res, NO_ERROR);
}
}  // namespace AAFwk
}  // namespace OHOS