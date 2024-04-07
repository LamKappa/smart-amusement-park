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
#define protected public
#include "app_mgr_service.h"
#undef private
#undef protected
#include <gtest/gtest.h>
#include "securec.h"
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

// keep same with app_spawn_client.cpp
const int32_t CONNECT_RETRY_MAX_TIMES = 15;

// this function is only used to mock sleep method so ut can run without delay.
int MockSleep([[maybe_unused]] uint32_t seconds)
{
    return 0;
}

class AppMgrServiceInnerMock : public AppMgrServiceInner {
public:
    AppMgrServiceInnerMock()
        : socket_(std::make_shared<MockAppSpawnSocket>()), appSpawnClient_(std::make_unique<AppSpawnClient>())
    {
        appSpawnClient_->SetSocket(socket_);
    }

    ~AppMgrServiceInnerMock()
    {}

    virtual ErrCode OpenAppSpawnConnection() override
    {
        if (appSpawnClient_.get() != nullptr) {
            return appSpawnClient_->OpenConnection();
        }
        return -1;
    }

    virtual SpawnConnectionState QueryAppSpawnConnectionState() const override
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

    const std::shared_ptr<MockAppSpawnSocket> &GetSocket() const
    {
        return socket_;
    }

private:
    std::shared_ptr<MockAppSpawnSocket> socket_;
    std::unique_ptr<AppSpawnClient> appSpawnClient_;
};

class AmsServiceAppSpawnClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AmsServiceAppSpawnClientTest::SetUpTestCase()
{}

void AmsServiceAppSpawnClientTest::TearDownTestCase()
{}

void AmsServiceAppSpawnClientTest::SetUp()
{}

void AmsServiceAppSpawnClientTest::TearDown()
{}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: Create AppSpawnClient and connect to AppSpawnDaemon in AppMgrService_001
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AppMgrService act normal when create appspawnclient and connect to AppSpawnDaemon
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_001, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_001 start");
    std::shared_ptr<AppMgrService> appMgrService = std::make_shared<AppMgrService>();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService->QueryServiceState().connectionState);
    std::shared_ptr<AppMgrServiceInnerMock> innerService = std::make_shared<AppMgrServiceInnerMock>();
    appMgrService->SetInnerService(innerService);
    EXPECT_CALL(*(innerService->GetSocket()), OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    appMgrService->OnStart();
    EXPECT_EQ(SpawnConnectionState::STATE_CONNECTED, appMgrService->QueryServiceState().connectionState);
    EXPECT_CALL(*(innerService->GetSocket()), CloseAppSpawnConnection()).Times(1);
    appMgrService->OnStop();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService->QueryServiceState().connectionState);
    APP_LOGI("ams_service_app_spawn_client_001 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: Create AppSpawnClient and connect to AppSpawnDaemon in AppMgrService_002
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AppMgrService act normal when create appspawnclient and failed connect to AppSpawnDaemon
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_002, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_002 start");
    std::shared_ptr<AppMgrService> appMgrService = std::make_shared<AppMgrService>();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService->QueryServiceState().connectionState);
    std::shared_ptr<AppMgrServiceInnerMock> innerService = std::make_shared<AppMgrServiceInnerMock>();
    appMgrService->SetInnerService(innerService);
    EXPECT_CALL(*(innerService->GetSocket()), OpenAppSpawnConnection())
        .WillRepeatedly(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED));
    appMgrService->OnStart();
    EXPECT_EQ(SpawnConnectionState::STATE_CONNECT_FAILED, appMgrService->QueryServiceState().connectionState);
    appMgrService->OnStop();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appMgrService->QueryServiceState().connectionState);
    APP_LOGI("ams_service_app_spawn_client_002 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient connect to AppSpawnDaemon_001
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when successfully connect to AppSpawnDaemon
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_003, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_003 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
    EXPECT_EQ(SpawnConnectionState::STATE_CONNECTED, appSpawnClient->QueryConnectionState());
    APP_LOGI("ams_service_app_spawn_client_003 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient connect to AppSpawnDaemon_002
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when failed to connect to AppSpawnDaemon
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_004, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_004 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED));
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    EXPECT_EQ(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED, appSpawnClient->OpenConnection());
    EXPECT_EQ(SpawnConnectionState::STATE_CONNECT_FAILED, appSpawnClient->QueryConnectionState());
    APP_LOGI("ams_service_app_spawn_client_004 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient request new process_001
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when successfully to send fork request
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_005, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_005 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
    EXPECT_CALL(*socketMock, WriteMessage(_, _)).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*socketMock, ReadMessage(_, _)).WillOnce(Invoke(socketMock.get(), &MockAppSpawnSocket::ReadImpl));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(1);
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    pid_t expectPid = 11111;
    pid_t newPid = 0;
    socketMock->SetExpectPid(expectPid);
    appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(expectPid, newPid);
    APP_LOGI("ams_service_app_spawn_client_005 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient request new process_002
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when failed to connect daemon but reconnect success
 *                  when try to send fork request
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_006, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_006 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED));
    EXPECT_EQ(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED, appSpawnClient->OpenConnection());
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*socketMock, WriteMessage(_, _)).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*socketMock, ReadMessage(_, _)).WillOnce(Invoke(socketMock.get(), &MockAppSpawnSocket::ReadImpl));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(1);
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    pid_t expectPid = 11111;
    pid_t newPid = 0;
    socketMock->SetExpectPid(expectPid);
    appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(expectPid, newPid);
    APP_LOGI("ams_service_app_spawn_client_006 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient request new process_003
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when failed to connect daemon and reconnect fail
 *                  when try to send fork request
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_007, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_007 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED));
    EXPECT_EQ(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED, appSpawnClient->OpenConnection());
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    pid_t newPid = 0;
    ErrCode result = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED, result);
    APP_LOGI("ams_service_app_spawn_client_007 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient request new process_004
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when try to send request without connect but reconnect success
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_008, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_008 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*socketMock, WriteMessage(_, _)).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*socketMock, ReadMessage(_, _)).WillOnce(Invoke(socketMock.get(), &MockAppSpawnSocket::ReadImpl));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(1);
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    pid_t expectPid = 11111;
    pid_t newPid = 0;
    socketMock->SetExpectPid(expectPid);
    appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(expectPid, newPid);
    APP_LOGI("ams_service_app_spawn_client_008 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient request new process_005
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when try to send request without connect and reconnect failed
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_009, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_009 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED));
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    pid_t newPid = 0;
    ErrCode result = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED, result);
    APP_LOGI("ams_service_app_spawn_client_009 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient request new process_006
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when failed to send message to AppSpawnDaemon
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_010, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_010 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, WriteMessage(_, _)).WillRepeatedly(Return(ERR_APPEXECFWK_SOCKET_WRITE_FAILED));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(AtLeast(1));
    EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    pid_t newPid = 0;
    ErrCode result = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(ERR_APPEXECFWK_SOCKET_WRITE_FAILED, result);
    APP_LOGI("ams_service_app_spawn_client_010 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient request new process_007
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when failed to read message from AppSpawnDaemon
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_011, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_011 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, WriteMessage(_, _)).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, ReadMessage(_, _)).WillRepeatedly(Return(ERR_APPEXECFWK_SOCKET_READ_FAILED));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(AtLeast(1));
    EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    pid_t newPid = 0;
    ErrCode result = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(ERR_APPEXECFWK_SOCKET_READ_FAILED, result);
    APP_LOGI("ams_service_app_spawn_client_011 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient request new process_008
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when pass invalid param procName(empty)
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_012, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_012 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(AtLeast(1));
    EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "", "soPath"};
    pid_t newPid = 0;
    ErrCode result = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(ERR_APPEXECFWK_ASSEMBLE_START_MSG_FAILED, result);
    APP_LOGI("ams_service_app_spawn_client_012 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient request new process_011
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when pass invalid param soPath(empty)
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_013, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_013 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();

    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(AtLeast(1));
    EXPECT_CALL(*socketMock, WriteMessage(_, _)).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, ReadMessage(_, _)).WillRepeatedly(Return(ERR_OK));

    appSpawnClient->SetSocket(socketMock);

    EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());

    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "", ""};
    pid_t newPid = 0;
    ErrCode result = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(ERR_APPEXECFWK_ASSEMBLE_START_MSG_FAILED, result);
    APP_LOGI("ams_service_app_spawn_client_013 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient request new process_012
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when pass invalid param procName(oversize)
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_014, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_014 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(AtLeast(1));
    EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
    std::string invalidParam = "invalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalid\
                                invalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalid\
                                invalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalid\
                                invalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalid";
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, invalidParam, "soPath"};
    pid_t newPid = 0;
    ErrCode result = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(ERR_APPEXECFWK_ASSEMBLE_START_MSG_FAILED, result);
    APP_LOGI("ams_service_app_spawn_client_014 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient request new process_015
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient act normal when pass invalid param soPath(oversize)
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_015, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_015 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(AtLeast(1));
    EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
    std::string invalidParam = "invalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalid\
                                invalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalid\
                                invalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalid\
                                invalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalidinvalid";
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", invalidParam};
    pid_t newPid = 0;
    ErrCode result = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(ERR_APPEXECFWK_ASSEMBLE_START_MSG_FAILED, result);
    APP_LOGI("ams_service_app_spawn_client_015 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient close connection with AppSpawnDaemon_001
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient CloseConnection act normal after OpenConnection
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_016, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_016 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(AtLeast(1));
    EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
    appSpawnClient->CloseConnection();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    APP_LOGI("ams_service_app_spawn_client_016 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient close connection with AppSpawnDaemon_002
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient CloseConnection act normal without OpenConnection
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_017, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_017 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    appSpawnClient->CloseConnection();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    APP_LOGI("ams_service_app_spawn_client_017 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient close connection with AppSpawnDaemon_003
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient CloseConnection act normal when OpenConnection failed
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_018, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_018 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED));
    EXPECT_EQ(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED, appSpawnClient->OpenConnection());
    appSpawnClient->CloseConnection();
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    APP_LOGI("ams_service_app_spawn_client_018 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: AppSpawnClient
 * FunctionPoints: AppSpawnClient close connection with AppSpawnDaemon_004
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if after close connection with appspawn, appspawnclient startprocess
 *                  will invoke openconnection.
 */
HWTEST_F(AmsServiceAppSpawnClientTest, AppSpawnClient_019, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_client_019 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(1);
    EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
    appSpawnClient->CloseConnection();
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*socketMock, WriteMessage(_, _)).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*socketMock, ReadMessage(_, _)).WillOnce(Invoke(socketMock.get(), &MockAppSpawnSocket::ReadImpl));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(1);
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    pid_t expectPid = 11111;
    pid_t newPid = 0;
    socketMock->SetExpectPid(expectPid);
    ErrCode result = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(expectPid, newPid);
    EXPECT_EQ(ERR_OK, result);
    APP_LOGI("ams_service_app_spawn_client_019 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: ReConnectAppSpawn
 * FunctionPoints: Test AppSpawnClient reconnect functions.
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient first time connection fail, but second time is success.
 */
HWTEST_F(AmsServiceAppSpawnClientTest, ReConnectAppSpawn_001, TestSize.Level0)
{
    APP_LOGI("ams_service_reconnect_app_spawn_001 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection())
        .WillOnce(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED))
        .WillOnce(Return(ERR_OK));
    EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
    EXPECT_EQ(SpawnConnectionState::STATE_CONNECTED, appSpawnClient->QueryConnectionState());
    APP_LOGI("ams_service_reconnect_app_spawn_001 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: ReConnectAppSpawn
 * FunctionPoints: Test AppSpawnClient reconnect functions.
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient first n time connection fail, but last time is success.
 */
HWTEST_F(AmsServiceAppSpawnClientTest, ReConnectAppSpawn_002, TestSize.Level0)
{
    APP_LOGI("ams_service_reconnect_app_spawn_002 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    InSequence seq;
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection())
        .Times(CONNECT_RETRY_MAX_TIMES)
        .WillRepeatedly(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED))
        .RetiresOnSaturation();
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));

    EXPECT_EQ(ERR_OK, appSpawnClient->OpenConnection());
    EXPECT_EQ(SpawnConnectionState::STATE_CONNECTED, appSpawnClient->QueryConnectionState());
    APP_LOGI("ams_service_reconnect_app_spawn_002 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: ReConnectAppSpawn
 * FunctionPoints: Test AppSpawnClient reconnect functions.
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient all (n+1) times connection fail.
 */
HWTEST_F(AmsServiceAppSpawnClientTest, ReConnectAppSpawn_003, TestSize.Level0)
{
    APP_LOGI("ams_service_reconnect_app_spawn_003 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_EQ(SpawnConnectionState::STATE_NOT_CONNECT, appSpawnClient->QueryConnectionState());
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection())
        .Times(CONNECT_RETRY_MAX_TIMES + 1)
        .WillRepeatedly(Return(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED));

    EXPECT_EQ(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED, appSpawnClient->OpenConnection());
    EXPECT_EQ(SpawnConnectionState::STATE_CONNECT_FAILED, appSpawnClient->QueryConnectionState());
    APP_LOGI("ams_service_reconnect_app_spawn_003 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: ReConnectAppSpawn
 * FunctionPoints: Test AppSpawnClient reconnect functions.
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient first time start process fail, but second time is success.
 */
HWTEST_F(AmsServiceAppSpawnClientTest, ReConnectAppSpawn_004, TestSize.Level0)
{
    APP_LOGI("ams_service_reconnect_app_spawn_004 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, WriteMessage(_, _)).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, ReadMessage(_, _))
        .WillOnce(Return(ERR_APPEXECFWK_SOCKET_READ_FAILED))
        .WillOnce(Invoke(socketMock.get(), &MockAppSpawnSocket::ReadImpl));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(AtLeast(1));
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    pid_t expectPid = 11111;
    pid_t newPid = 0;
    socketMock->SetExpectPid(expectPid);
    ErrCode result = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(expectPid, newPid);
    EXPECT_EQ(ERR_OK, result);
    APP_LOGI("ams_service_reconnect_app_spawn_004 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: ReConnectAppSpawn
 * FunctionPoints: Test AppSpawnClient reconnect functions.
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient first n time start process fail, but last time is success.
 */
HWTEST_F(AmsServiceAppSpawnClientTest, ReConnectAppSpawn_005, TestSize.Level0)
{
    APP_LOGI("ams_service_reconnect_app_spawn_005 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    InSequence seq;
    for (int i = 0; i < CONNECT_RETRY_MAX_TIMES; i++) {
        EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*socketMock, WriteMessage(_, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*socketMock, ReadMessage(_, _)).WillOnce(Return(ERR_APPEXECFWK_SOCKET_READ_FAILED));
        EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(1);
    }
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*socketMock, WriteMessage(_, _)).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*socketMock, ReadMessage(_, _)).WillOnce(Invoke(socketMock.get(), &MockAppSpawnSocket::ReadImpl));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(1);
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    pid_t expectPid = 11111;
    pid_t newPid = 0;
    socketMock->SetExpectPid(expectPid);
    ErrCode result = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(expectPid, newPid);
    EXPECT_EQ(ERR_OK, result);
    APP_LOGI("ams_service_reconnect_app_spawn_005 end");
}

/*
 * Feature: AppMgrService
 * Function: Service
 * SubFunction: ReConnectAppSpawn
 * FunctionPoints: Test AppSpawnClient reconnect functions.
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if AppSpawnClient all(n+1) times start process fail.
 */
HWTEST_F(AmsServiceAppSpawnClientTest, ReConnectAppSpawn_006, TestSize.Level0)
{
    APP_LOGI("ams_service_reconnect_app_spawn_006 start");
    std::shared_ptr<AppSpawnClient> appSpawnClient = std::make_shared<AppSpawnClient>();
    std::shared_ptr<MockAppSpawnSocket> socketMock = std::make_shared<MockAppSpawnSocket>();
    appSpawnClient->SetSocket(socketMock);
    EXPECT_CALL(*socketMock, OpenAppSpawnConnection()).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, WriteMessage(_, _)).WillRepeatedly(Return(ERR_OK));
    EXPECT_CALL(*socketMock, ReadMessage(_, _)).WillRepeatedly(Return(ERR_APPEXECFWK_SOCKET_READ_FAILED));
    EXPECT_CALL(*socketMock, CloseAppSpawnConnection()).Times(AtLeast(1));
    AppSpawnStartMsg params = {10001, 10001, {10001, 10002}, "processName", "soPath"};
    pid_t newPid = 0;
    ErrCode result = appSpawnClient->StartProcess(params, newPid);
    EXPECT_EQ(ERR_APPEXECFWK_SOCKET_READ_FAILED, result);
    APP_LOGI("ams_service_reconnect_app_spawn_006 end");
}
