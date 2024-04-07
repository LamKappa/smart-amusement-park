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

#include "app_spawn_socket.h"
#include <gtest/gtest.h>
#include "securec.h"
#include "app_log_wrapper.h"
#include "mock_client_socket.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using testing::_;
using testing::AtLeast;
using testing::InSequence;
using testing::Invoke;
using testing::Return;

// this function is only used to mock sleep method so ut can run without delay.
int MockSleep([[maybe_unused]] uint32_t seconds)
{
    return 0;
}

class AmsServiceAppSpawnSocketTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AmsServiceAppSpawnSocketTest::SetUpTestCase()
{}

void AmsServiceAppSpawnSocketTest::TearDownTestCase()
{}

void AmsServiceAppSpawnSocketTest::SetUp()
{}

void AmsServiceAppSpawnSocketTest::TearDown()
{}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: OpenAppSpawnConnection
 * FunctionPoints: open connection
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if create client socket fail, open connection error.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_001, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_001 start");

    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_CALL(*mockClientSocket, CreateClient()).WillOnce(Return(-1));
    EXPECT_EQ(ERR_APPEXECFWK_BAD_APPSPAWN_CLIENT, appSpawnSocket->OpenAppSpawnConnection());

    APP_LOGI("ams_service_app_spawn_socket_001 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: OpenAppSpawnConnection
 * FunctionPoints: open connection
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify if connect socket client fail, open connection error.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_002, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_002 start");

    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_CALL(*mockClientSocket, CreateClient()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*mockClientSocket, ConnectSocket()).WillOnce(Return(-1));
    EXPECT_EQ(ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED, appSpawnSocket->OpenAppSpawnConnection());

    APP_LOGI("ams_service_app_spawn_socket_002 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: OpenAppSpawnConnection
 * FunctionPoints: open connection
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function OpenAppSpawnConnection open connection success.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_003, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_003 start");

    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_CALL(*mockClientSocket, CreateClient()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*mockClientSocket, ConnectSocket()).WillOnce(Return(ERR_OK));
    EXPECT_EQ(ERR_OK, appSpawnSocket->OpenAppSpawnConnection());

    APP_LOGI("ams_service_app_spawn_socket_003 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: ReadMessage
 * FunctionPoints: check params
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function ReadMessage can check the invalid buffer point.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_004, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_004 start");

    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();
    std::unique_ptr<uint8_t[]> buff = nullptr;
    int32_t len = 10;

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_EQ(ERR_INVALID_VALUE, appSpawnSocket->ReadMessage(buff.get(), len));

    APP_LOGI("ams_service_app_spawn_socket_004 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: ReadMessage
 * FunctionPoints: check params
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function ReadMessage can check the buffer length is 0.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_005, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_005 start");

    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();
    std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(10);
    int32_t len = 0;

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_EQ(ERR_INVALID_VALUE, appSpawnSocket->ReadMessage(buff.get(), len));

    APP_LOGI("ams_service_app_spawn_socket_005 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: ReadMessage
 * FunctionPoints: check params
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function ReadMessage can check the buffer length < 0.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_006, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_006 start");

    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();
    std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(10);
    int32_t len = -1;

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_EQ(ERR_INVALID_VALUE, appSpawnSocket->ReadMessage(buff.get(), len));

    APP_LOGI("ams_service_app_spawn_socket_006 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: ReadMessage
 * FunctionPoints: read message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function ReadMessage can read the valid message.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_007, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_007 start");

    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();
    std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(10);
    int32_t len = 10;

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_CALL(*mockClientSocket, ReadSocketMessage(_, _)).WillOnce(Return(len));
    EXPECT_EQ(ERR_OK, appSpawnSocket->ReadMessage(buff.get(), len));

    APP_LOGI("ams_service_app_spawn_socket_007 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: ReadMessage
 * FunctionPoints: check params
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function ReadMessage can check the message length.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_008, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_008 start");

    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();
    std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(10);
    int32_t len = 10;

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_CALL(*mockClientSocket, ReadSocketMessage(_, _)).WillOnce(Return(11));
    EXPECT_EQ(ERR_APPEXECFWK_SOCKET_READ_FAILED, appSpawnSocket->ReadMessage(buff.get(), len));

    APP_LOGI("ams_service_app_spawn_socket_008 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: WriteMessage
 * FunctionPoints: check params
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function WriteMessage can check the invalid buffer point.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_009, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_009 start");
    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();
    std::unique_ptr<uint8_t[]> buff = nullptr;
    int32_t len = 10;

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_EQ(ERR_INVALID_VALUE, appSpawnSocket->WriteMessage(buff.get(), len));

    APP_LOGI("ams_service_app_spawn_socket_009 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: WriteMessage
 * FunctionPoints: check params
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function WriteMessage can check the buffer length is 0.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_010, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_010 start");

    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();
    std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(10);
    int32_t len = 0;

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_EQ(ERR_INVALID_VALUE, appSpawnSocket->WriteMessage(buff.get(), len));

    APP_LOGI("ams_service_app_spawn_socket_010 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: WriteMessage
 * FunctionPoints: check params
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function WriteMessage can check the buffer length < 0.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_011, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_011 start");

    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();
    std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(10);
    int32_t len = -1;

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_EQ(ERR_INVALID_VALUE, appSpawnSocket->WriteMessage(buff.get(), len));

    APP_LOGI("ams_service_app_spawn_socket_011 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: WriteMessage
 * FunctionPoints: write message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function WriteMessage can write valid message.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_012, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_012 start");

    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();
    std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(10);
    int32_t len = 10;

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_CALL(*mockClientSocket, WriteSocketMessage(_, _)).WillOnce(Return(len));
    EXPECT_EQ(ERR_OK, appSpawnSocket->WriteMessage(buff.get(), len));

    APP_LOGI("ams_service_app_spawn_socket_012 end");
}

/*
 * Feature: AppMgrService
 * Function: AppSpawnSocket
 * SubFunction: WriteMessage
 * FunctionPoints: write message
 * EnvConditions: mobile that can run ohos test framework
 * CaseDescription: Verify the function WriteMessage can check the message length.
 */
HWTEST(AmsServiceAppSpawnSocketTest, AppSpawnSocket_013, TestSize.Level0)
{
    APP_LOGI("ams_service_app_spawn_socket_013 start");

    std::shared_ptr<OHOS::AppSpawn::MockClientSocket> mockClientSocket =
        std::make_shared<OHOS::AppSpawn::MockClientSocket>();
    std::unique_ptr<AppSpawnSocket> appSpawnSocket = std::make_unique<AppSpawnSocket>();
    std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(10);
    int32_t len = 10;

    appSpawnSocket->SetClientSocket(mockClientSocket);
    EXPECT_CALL(*mockClientSocket, WriteSocketMessage(_, _)).WillOnce(Return(11));
    EXPECT_EQ(ERR_APPEXECFWK_SOCKET_WRITE_FAILED, appSpawnSocket->WriteMessage(buff.get(), len));

    APP_LOGI("ams_service_app_spawn_socket_013 end");
}
