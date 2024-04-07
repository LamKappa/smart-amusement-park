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

#include <chrono>  // std::chrono::seconds
#include <thread>

#include "securec.h"

// redefine private and protected since testcase need to invoke and test private function
#define private public
#define protected public
#include "app_mgr_service.h"
#undef private
#undef protected

#include "app_log_wrapper.h"
#include "mock_app_spawn_socket.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using testing::_;
using testing::AtLeast;
using testing::InSequence;
using testing::Invoke;
using testing::Return;

namespace {

const uint32_t CYCLE_NUMBER = 10;
const int32_t PID_VALUE = 13579;
const int32_t CONNECT_RETRY_MAX_TIMES = 15;

}  // namespace

// this function is only used to mock sleep method so mst can run without delay.
int MockSleep([[maybe_unused]] uint32_t seconds)
{
    return 0;
}

class MockedAppSpawnSocket : public AppSpawnSocket {
public:
    MockedAppSpawnSocket()
    {}

    ~MockedAppSpawnSocket()
    {}

    int32_t OpenAppSpawnConnection() override
    {
        APP_LOGI("MockedAppSpawnSocket::OpenAppSpawnConnection ready to openConnection!");
        gHasConnected_ = true;
        if (!gConnectSuccess_) {
            APP_LOGE("MockedAppSpawnSocket::OpenAppSpawnConnection mock case failed to openConnection!");
            return ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED;
        }
        return ERR_OK;
    }

    void CloseAppSpawnConnection() override
    {
        APP_LOGI("MockedAppSpawnSocket::CloseAppSpawnConnection ready to openConnection!");
    }

    int32_t WriteMessage([[maybe_unused]] const void *buf, [[maybe_unused]] int32_t len) override
    {
        if (!gHasConnected_) {
            APP_LOGE("MockedAppSpawnSocket::WriteMessage mock case not openConnection!");
            return ERR_APPEXECFWK_BAD_APPSPAWN_SOCKET;
        }
        if (!gConnectSuccess_) {
            APP_LOGE("MockedAppSpawnSocket::WriteMessage mock case failed to openConnection!");
            return ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED;
        }
        if (!gWriteMessageSuccess_) {
            APP_LOGE("MockedAppSpawnSocket::WriteMessage mock case failed to writeMessage!");
            return ERR_APPEXECFWK_SOCKET_WRITE_FAILED;
        }
        return ERR_OK;
    }

    int32_t ReadMessage(void *buf, [[maybe_unused]] int32_t len) override
    {
        if (!gHasConnected_) {
            APP_LOGE("MockedAppSpawnSocket::ReadMessage mock case not openConnection!");
            return ERR_APPEXECFWK_BAD_APPSPAWN_SOCKET;
        }
        if (!gConnectSuccess_) {
            APP_LOGE("MockedAppSpawnSocket::ReadMessage mock case failed to openConnection!");
            return ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED;
        }
        if (!gReadMessageSuccess_) {
            APP_LOGE("MockedAppSpawnSocket::ReadMessage mock case failed to readMessage!");
            return ERR_APPEXECFWK_SOCKET_READ_FAILED;
        }
        AppSpawnPidMsg msg;
        msg.pid = PID_VALUE;
        if (memcpy_s(buf, sizeof(msg), msg.pidBuf, sizeof(msg)) != EOK) {
            return ERR_APPEXECFWK_ASSEMBLE_START_MSG_FAILED;
        }
        return ERR_OK;
    }

    static bool gConnectSuccess_;
    static bool gHasConnected_;
    static bool gWriteMessageSuccess_;
    static bool gReadMessageSuccess_;
};

bool MockedAppSpawnSocket::gHasConnected_ = false;
bool MockedAppSpawnSocket::gConnectSuccess_ = true;
bool MockedAppSpawnSocket::gWriteMessageSuccess_ = true;
bool MockedAppSpawnSocket::gReadMessageSuccess_ = true;

class MockedAppMgrServiceInner : public AppMgrServiceInner {
public:
    MockedAppMgrServiceInner()
        : socket_(std::make_shared<MockedAppSpawnSocket>()), appSpawnClient_(std::make_unique<AppSpawnClient>())
    {
        appSpawnClient_->SetSocket(socket_);
    }

    virtual ~MockedAppMgrServiceInner()
    {}
    int32_t OpenAppSpawnConnection() override
    {
        if (appSpawnClient_.get() != nullptr) {
            return appSpawnClient_->OpenConnection();
        }
        return ERR_APPEXECFWK_BAD_APPSPAWN_CLIENT;
    }

    SpawnConnectionState QueryAppSpawnConnectionState() const override
    {
        if (appSpawnClient_) {
            return appSpawnClient_->QueryConnectionState();
        }
        return SpawnConnectionState::STATE_NOT_CONNECT;
    }

    void CloseAppSpawnConnection() const override
    {
        if (appSpawnClient_) {
            appSpawnClient_->CloseConnection();
        }
    }

private:
    std::shared_ptr<AppSpawnSocket> socket_;
    std::unique_ptr<AppSpawnClient> appSpawnClient_;
};

class AmsServiceAppSpawnClientModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AmsServiceAppSpawnClientModuleTest::SetUpTestCase()
{}

void AmsServiceAppSpawnClientModuleTest::TearDownTestCase()
{}

void AmsServiceAppSpawnClientModuleTest::SetUp()
{
    MockedAppSpawnSocket::gHasConnected_ = false;
    MockedAppSpawnSocket::gConnectSuccess_ = true;
    MockedAppSpawnSocket::gWriteMessageSuccess_ = true;
    MockedAppSpawnSocket::gReadMessageSuccess_ = true;
}

void AmsServiceAppSpawnClientModuleTest::TearDown()
{}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: Create AppSpawnClient and connect to AppSpawnDaemon
 * EnvConditions: NA
 * CaseDescription: Test if AppMgrService act normal when create appspawnclient and connect to AppSpawnDaemon.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ConnectAppSpawnDaemon_001, TestSize.Level2)
{
    APP_LOGI("ConnectAppSpawnDaemon_001 start");
    std::shared_ptr<AppMgrService> appMgrService = std::make_shared<AppMgrService>();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService->QueryServiceState().connectionState);
    std::shared_ptr<MockedAppMgrServiceInner> amsInner = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService->SetInnerService(amsInner);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        appMgrService->OnStart();
        ASSERT_EQ(SpawnConnectionState::STATE_CONNECTED, appMgrService->QueryServiceState().connectionState);
        ASSERT_EQ(SpawnConnectionState::STATE_CONNECTED, amsInner->QueryAppSpawnConnectionState());
        appMgrService->OnStop();
        EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService->QueryServiceState().connectionState);
        EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, amsInner->QueryAppSpawnConnectionState());
    }
    APP_LOGI("ConnectAppSpawnDaemon_001 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: Create AppSpawnClient and connect to AppSpawnDaemon in AppMgrService_002
 * EnvConditions: NA
 * CaseDescription: Test if AppMgrService act normal when create appspawnclient and failed connect to AppSpawnDaemon.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ConnectAppSpawnDaemon_002, TestSize.Level2)
{
    APP_LOGI("ConnectAppSpawnDaemon_002 start");
    std::shared_ptr<AppMgrService> appMgrService = std::make_shared<AppMgrService>();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService->QueryServiceState().connectionState);
    std::shared_ptr<MockedAppMgrServiceInner> amsInner = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService->SetInnerService(amsInner);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        MockedAppSpawnSocket::gConnectSuccess_ = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        appMgrService->OnStart();
        EXPECT_EQ(SpawnConnectionState::STATE_CONNECT_FAILED, appMgrService->QueryServiceState().connectionState);
        EXPECT_EQ(SpawnConnectionState::STATE_CONNECT_FAILED, amsInner->QueryAppSpawnConnectionState());
        appMgrService->OnStop();
        EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService->QueryServiceState().connectionState);
        EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, amsInner->QueryAppSpawnConnectionState());
        MockedAppSpawnSocket::gConnectSuccess_ = true;
        appMgrService->OnStart();
        ASSERT_EQ(SpawnConnectionState::STATE_CONNECTED, appMgrService->QueryServiceState().connectionState);
        ASSERT_EQ(SpawnConnectionState::STATE_CONNECTED, amsInner->QueryAppSpawnConnectionState());
        appMgrService->OnStop();
        EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService->QueryServiceState().connectionState);
        EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, amsInner->QueryAppSpawnConnectionState());
    }
    APP_LOGI("ConnectAppSpawnDaemon_002 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient connect to AppSpawnDaemon
 * EnvConditions: NA
 * CaseDescription: Test if AppSpawnClient act normal when connect to AppSpawnDaemon twice.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ConnectAppSpawnDaemon_003, TestSize.Level2)
{
    APP_LOGI("ConnectAppSpawnDaemon_003 start");
    std::shared_ptr<AppMgrService> appMgrService = std::make_shared<AppMgrService>();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService->QueryServiceState().connectionState);
    std::shared_ptr<MockedAppMgrServiceInner> amsInner = std::make_shared<MockedAppMgrServiceInner>();
    appMgrService->SetInnerService(amsInner);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        appMgrService->OnStart();
        ASSERT_EQ(SpawnConnectionState::STATE_CONNECTED, appMgrService->QueryServiceState().connectionState);
        ASSERT_EQ(SpawnConnectionState::STATE_CONNECTED, amsInner->QueryAppSpawnConnectionState());
        appMgrService->OnStart();
        EXPECT_EQ(SpawnConnectionState::STATE_CONNECTED, appMgrService->QueryServiceState().connectionState);
        EXPECT_EQ(SpawnConnectionState::STATE_CONNECTED, amsInner->QueryAppSpawnConnectionState());
        appMgrService->OnStop();
        EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService->QueryServiceState().connectionState);
        EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, amsInner->QueryAppSpawnConnectionState());
    }
    APP_LOGI("ConnectAppSpawnDaemon_003 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient connect to AppSpawnDaemon
 * EnvConditions: NA
 * CaseDescription: Test if AppSpawnClient act normal when connect to AppSpawnDaemon successfully.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ConnectAppSpawnDaemon_004, TestSize.Level1)
{
    APP_LOGI("ConnectAppSpawnDaemon_004 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockedAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockedAppSpawnSocket>();
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
        EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
        EXPECT_EQ(SpawnConnectionState::STATE_CONNECTED, appSpawnClient->QueryConnectionState());
        EXPECT_EQ(ERR_OK, mockedAppSpawnSocket->OpenAppSpawnConnection());
        appSpawnClient->CloseConnection();
    }
    APP_LOGI("ConnectAppSpawnDaemon_004 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient request normal process
 * EnvConditions: NA
 * CaseDescription: Test if AppSpawnClient act normal when successfully to send normal request message.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ConnectAppSpawnDaemon_005, TestSize.Level1)
{
    APP_LOGI("ConnectAppSpawnDaemon_005 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockedAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockedAppSpawnSocket>();
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
        AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
        int32_t pid = PID_VALUE;
        int32_t newPid = 0;
        appSpawnClient->StartProcess(params, newPid);
        EXPECT_EQ(pid, newPid);
        EXPECT_EQ(ERR_OK, mockedAppSpawnSocket->OpenAppSpawnConnection());
    }
    appSpawnClient->CloseConnection();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    APP_LOGI("ConnectAppSpawnDaemon_005 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient request normal process
 * EnvConditions: NA
 * CaseDescription: Test if AppSpawnClient act normal when connect daemon successfully but failed
 *                  to start process when try to send abnormal request message with wrong AppName.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ConnectAppSpawnDaemon_006, TestSize.Level1)
{
    APP_LOGI("ConnectAppSpawnDaemon_006 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockedAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockedAppSpawnSocket>();
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    std::string illegalAppName = "01234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                 "01234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                 "01234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                 "0123456789012345";  // The length is 256.
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
        AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", illegalAppName};
        int32_t newPid = 0;
        ErrCode result = appSpawnClient->StartProcess(params, newPid);
        EXPECT_EQ(ERR_APPEXECFWK_ASSEMBLE_START_MSG_FAILED, result);
        EXPECT_EQ(ERR_OK, mockedAppSpawnSocket->OpenAppSpawnConnection());
    }
    appSpawnClient->CloseConnection();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    APP_LOGI("ConnectAppSpawnDaemon_006 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient request normal process
 * EnvConditions: NA
 * CaseDescription: Test if AppSpawnClient act normal when connect daemon successfully but failed
 *                  to start process when try to send abnormal request message with wrong ClsName.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ConnectAppSpawnDaemon_007, TestSize.Level1)
{
    APP_LOGI("ConnectAppSpawnDaemon_007 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockedAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockedAppSpawnSocket>();

    std::string illegalClsName = "01234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                 "01234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                 "01234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                 "0123456789012345";  // The length is 256.
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());

        AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", illegalClsName};
        int32_t newPid = 0;
        ErrCode result = appSpawnClient->StartProcess(params, newPid);
        EXPECT_EQ(ERR_APPEXECFWK_ASSEMBLE_START_MSG_FAILED, result);
        EXPECT_EQ(ERR_OK, mockedAppSpawnSocket->OpenAppSpawnConnection());
    }
    appSpawnClient->CloseConnection();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    APP_LOGI("ConnectAppSpawnDaemon_007 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient request normal process
 * EnvConditions: NA
 * CaseDescription: Test if AppSpawnClient act normal when connect daemon successfully but failed to start
 *                  process when try to send abnormal request message with wrong FuncName.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ConnectAppSpawnDaemon_008, TestSize.Level1)
{
    APP_LOGI("ConnectAppSpawnDaemon_008 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockedAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockedAppSpawnSocket>();

    std::string illegalFuncName = "0123456789012345678901234567890123456789012345678901234567890123456789012345678"
                                  "0123456789012345678901234567890123456789012345678901234567890123456789012345678"
                                  "0123456789012345678901234567890123456789012345678901234567890123456789012345678"
                                  "0123456789012345999";  // The length is 256.
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());

        AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", illegalFuncName};
        int32_t newPid = 0;
        ErrCode result = appSpawnClient->StartProcess(params, newPid);
        EXPECT_EQ(ERR_APPEXECFWK_ASSEMBLE_START_MSG_FAILED, result);
        EXPECT_EQ(ERR_OK, mockedAppSpawnSocket->OpenAppSpawnConnection());
    }
    appSpawnClient->CloseConnection();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    APP_LOGI("ConnectAppSpawnDaemon_008 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient request normal process
 * EnvConditions: NA
 * CaseDescription: Test if AppSpawnClient act normal when connect daemon successfully but failed to start
 *                  process when try to send abnormal request message with wrong argnum.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ConnectAppSpawnDaemon_009, TestSize.Level1)
{
    APP_LOGI("ConnectAppSpawnDaemon_009 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockedAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockedAppSpawnSocket>();
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
        AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};

        int32_t newPid = 0;
        ErrCode result = appSpawnClient->StartProcess(params, newPid);
        EXPECT_EQ(ERR_OK, result);
        EXPECT_EQ(ERR_OK, mockedAppSpawnSocket->OpenAppSpawnConnection());
    }
    appSpawnClient->CloseConnection();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    APP_LOGI("ConnectAppSpawnDaemon_009 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient request normal process
 * EnvConditions: NA
 * CaseDescription: Test if AppSpawnClient act normal when connect daemon successfully but failed to start
 *                  process when try to send abnormal request message with wrong ArgName and ArgNum.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ConnectAppSpawnDaemon_010, TestSize.Level1)
{
    APP_LOGI("ConnectAppSpawnDaemon_010 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockedAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockedAppSpawnSocket>();
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    for (uint32_t i = 0; i < CYCLE_NUMBER; i++) {
        EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
        AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};

        int32_t newPid = 0;
        ErrCode result = appSpawnClient->StartProcess(params, newPid);
        EXPECT_EQ(ERR_OK, result);
        EXPECT_EQ(ERR_OK, mockedAppSpawnSocket->OpenAppSpawnConnection());
    }
    appSpawnClient->CloseConnection();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    APP_LOGI("ConnectAppSpawnDaemon_010 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient request normal process
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Test if AppSpawnClient act normal when failed to connect daemon at first but reconnect successfully.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ReconnectAppSpawnDaemon_001, TestSize.Level0)
{
    APP_LOGI("ReconnectAppSpawnDaemon_001 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};

    int32_t pid = PID_VALUE;
    int32_t newPid = 0;

    EXPECT_CALL(*mockedAppSpawnSocket, OpenAppSpawnConnection())
        .WillOnce(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED))
        .WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*mockedAppSpawnSocket, WriteMessage(_, _)).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*mockedAppSpawnSocket, ReadMessage(_, _))
        .WillRepeatedly(Invoke(mockedAppSpawnSocket.get(), &MockAppSpawnSocket::ReadImpl));

    EXPECT_CALL(*mockedAppSpawnSocket, CloseAppSpawnConnection()).Times(1);
    EXPECT_EQ(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED, mockedAppSpawnSocket->OpenAppSpawnConnection());
    mockedAppSpawnSocket->SetExpectPid(pid);
    auto returnCode = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(pid, newPid);
    EXPECT_EQ(ERR_OK, returnCode);
    APP_LOGI("ReconnectAppSpawnDaemon_001 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient request normal process
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Test if AppSpawnClient act normal when failed to connect daemon but the last reconnect successfully.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ReconnectAppSpawnDaemon_002, TestSize.Level0)
{
    APP_LOGI("ReconnectAppSpawnDaemon_002 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};

    int32_t pid = PID_VALUE;
    int32_t newPid = 0;
    InSequence sequence;

    EXPECT_CALL(*mockedAppSpawnSocket, OpenAppSpawnConnection())
        .Times(CONNECT_RETRY_MAX_TIMES)
        .WillRepeatedly(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED))
        .RetiresOnSaturation();
    EXPECT_CALL(*mockedAppSpawnSocket, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*mockedAppSpawnSocket, WriteMessage(_, _)).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*mockedAppSpawnSocket, ReadMessage(_, _))
        .WillRepeatedly(Invoke(mockedAppSpawnSocket.get(), &MockAppSpawnSocket::ReadImpl));
    EXPECT_CALL(*mockedAppSpawnSocket, CloseAppSpawnConnection()).Times(AtLeast(1));
    mockedAppSpawnSocket->SetExpectPid(pid);
    auto returnCode = appSpawnClient->StartProcessImpl(params, newPid);
    EXPECT_EQ(pid, newPid);
    EXPECT_EQ(ERR_OK, returnCode);
    APP_LOGI("ReconnectAppSpawnDaemon_002 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient request normal process
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Test if AppSpawnClient act normal when failed to connect daemon and still fail to reconnect
 *                  for the max times.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ReconnectAppSpawnDaemon_003, TestSize.Level0)
{
    APP_LOGI("ReconnectAppSpawnDaemon_003 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};

    int32_t pid = PID_VALUE;
    int32_t newPid = 0;
    InSequence sequence;

    EXPECT_CALL(*mockedAppSpawnSocket, OpenAppSpawnConnection())
        .Times(CONNECT_RETRY_MAX_TIMES + 1)
        .WillRepeatedly(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED));
    mockedAppSpawnSocket->SetExpectPid(pid);
    auto returnCode = appSpawnClient->StartProcessImpl(params, newPid);
    EXPECT_NE(pid, newPid);
    EXPECT_EQ(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED, returnCode);
    APP_LOGI("ReconnectAppSpawnDaemon_003 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient request normal process
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Test if AppSpawnClient act normal when failed to start process for ReadMessage,
 *                  but the last is successful.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ReconnectAppSpawnDaemon_004, TestSize.Level0)
{
    APP_LOGI("ReconnectAppSpawnDaemon_004 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    int32_t pid = PID_VALUE;
    int32_t newPid = 0;
    InSequence sequence;

    for (int32_t i = 0; i < CONNECT_RETRY_MAX_TIMES; i++) {
        EXPECT_CALL(*mockedAppSpawnSocket, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*mockedAppSpawnSocket, WriteMessage(_, _))
            .WillRepeatedly(Return(ERR_APPEXECFWK_SOCKET_WRITE_FAILED))
            .RetiresOnSaturation();
        EXPECT_CALL(*mockedAppSpawnSocket, ReadMessage(_, _)).WillRepeatedly(Return(ERR_OK)).RetiresOnSaturation();
        EXPECT_CALL(*mockedAppSpawnSocket, CloseAppSpawnConnection()).Times(1);
    }
    EXPECT_CALL(*mockedAppSpawnSocket, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*mockedAppSpawnSocket, WriteMessage(_, _)).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*mockedAppSpawnSocket, ReadMessage(_, _))
        .WillOnce(Invoke(mockedAppSpawnSocket.get(), &MockAppSpawnSocket::ReadImpl));
    EXPECT_CALL(*mockedAppSpawnSocket, CloseAppSpawnConnection()).Times(1);

    mockedAppSpawnSocket->SetExpectPid(pid);
    auto returnCode = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(pid, newPid);
    EXPECT_EQ(ERR_OK, returnCode);
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    APP_LOGI("ReconnectAppSpawnDaemon_004 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient request normal process
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Test if AppSpawnClient act normal when failed to connect daemon for ReadMessage.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ReconnectAppSpawnDaemon_005, TestSize.Level0)
{
    APP_LOGI("ReconnectAppSpawnDaemon_005 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    int32_t newPid = 0;

    EXPECT_CALL(*mockedAppSpawnSocket, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*mockedAppSpawnSocket, WriteMessage(_, _)).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*mockedAppSpawnSocket, ReadMessage(_, _)).WillRepeatedly(Return(ERR_APPEXECFWK_SOCKET_READ_FAILED));
    EXPECT_CALL(*mockedAppSpawnSocket, CloseAppSpawnConnection()).Times(CONNECT_RETRY_MAX_TIMES + 1);
    EXPECT_EQ(ERR_OK, mockedAppSpawnSocket->OpenAppSpawnConnection());
    auto returnCode = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(ERR_APPEXECFWK_SOCKET_READ_FAILED, returnCode);
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    APP_LOGI("ReconnectAppSpawnDaemon_005 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnClient
 * SubFunction: NA
 * FunctionPoints: AppSpawnClient request normal process
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Test if AppSpawnClient act normal when failed to connect daemon for
 *                  ((CONNECT_RETRY_MAX_TIMES + 1) * (CONNECT_RETRY_MAX_TIMES + 1) - 1) times,
 *                  but the last is successful.
 */
HWTEST_F(AmsServiceAppSpawnClientModuleTest, ReconnectAppSpawnDaemon_006, TestSize.Level0)
{
    APP_LOGI("ReconnectAppSpawnDaemon_006 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> mockedAppSpawnSocket = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(mockedAppSpawnSocket);
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    int32_t pid = PID_VALUE;
    int32_t newPid = 0;
    InSequence sequence;

    EXPECT_CALL(*mockedAppSpawnSocket, OpenAppSpawnConnection())
        .Times((CONNECT_RETRY_MAX_TIMES + 1) * (CONNECT_RETRY_MAX_TIMES + 1) - 1)
        .WillRepeatedly(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED))
        .RetiresOnSaturation();
    EXPECT_CALL(*mockedAppSpawnSocket, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*mockedAppSpawnSocket, WriteMessage(_, _)).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*mockedAppSpawnSocket, ReadMessage(_, _))
        .WillRepeatedly(Invoke(mockedAppSpawnSocket.get(), &MockAppSpawnSocket::ReadImpl));
    EXPECT_CALL(*mockedAppSpawnSocket, CloseAppSpawnConnection()).Times(AtLeast(1));
    mockedAppSpawnSocket->SetExpectPid(pid);
    auto returnCode = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(pid, newPid);
    EXPECT_EQ(ERR_OK, returnCode);
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    APP_LOGI("ReconnectAppSpawnDaemon_006 end");
}