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
#include "app_mgr_client.h"
#undef private
#include <gtest/gtest.h>
#include "ability_info.h"
#include "application_info.h"
#include "app_log_wrapper.h"
#include "iapp_state_callback.h"
#include "running_process_info.h"
#include "mock_ability_token.h"
#include "mock_app_mgr_service.h"
#include "mock_app_service_mgr.h"
#include "mock_iapp_state_callback.h"
#include "mock_ams_mgr_scheduler.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;
using testing::Return;
using testing::SetArgReferee;

class AmsAppMgrClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    sptr<MockAbilityToken> token_;
    sptr<MockAbilityToken> preToken_;
    std::unique_ptr<AppMgrClient> client_;
};

void AmsAppMgrClientTest::SetUpTestCase()
{}

void AmsAppMgrClientTest::TearDownTestCase()
{}

void AmsAppMgrClientTest::SetUp()
{
    client_.reset(new (std::nothrow) AppMgrClient());
    client_->SetServiceManager(std::make_unique<MockAppServiceMgr>());
    token_ = new (std::nothrow) MockAbilityToken();
}

void AmsAppMgrClientTest::TearDown()
{}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient
 * SubFunction: LoadAbility Function
 * FunctionPoints: AppMgrClient LoadAbility interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AppMgrClient invoke LoadAbility works.
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_001, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_001 start");
    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->ConnectAppMgrService());

    AbilityInfo abilityInfo;
    ApplicationInfo appInfo;

    sptr<IAmsMgr> amsMgrScheduler(new MockAmsMgrScheduler());

    EXPECT_CALL(*(static_cast<MockAmsMgrScheduler *>(amsMgrScheduler.GetRefPtr())), LoadAbility(_, _, _, _)).Times(1);

    EXPECT_CALL(*(static_cast<MockAppMgrService *>(client_->remote_.GetRefPtr())), GetAmsMgr())
        .Times(1)
        .WillOnce(Return(amsMgrScheduler));

    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->LoadAbility(token_, preToken_, abilityInfo, appInfo));
    APP_LOGI("ams_app_mgr_client_test_001 end");
}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient
 * SubFunction: LoadAbility Function
 * FunctionPoints: AppMgrClient LoadAbility interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AppMgrClient invoke LoadAbility act normal without connect to service.
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_002, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_002 start");
    AbilityInfo abilityInfo;
    ApplicationInfo appInfo;
    EXPECT_EQ(
        AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED, client_->LoadAbility(token_, preToken_, abilityInfo, appInfo));
    APP_LOGI("ams_app_mgr_client_test_002 end");
}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient
 * SubFunction: TerminateAbility Function
 * FunctionPoints: AppMgrClient TerminateAbility interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AppMgrClient invoke TerminateAbility works.
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_003, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_003 start");
    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->ConnectAppMgrService());

    sptr<IAmsMgr> amsMgrScheduler(new MockAmsMgrScheduler());
    EXPECT_CALL(*(static_cast<MockAppMgrService *>(client_->remote_.GetRefPtr())), GetAmsMgr())
        .Times(1)
        .WillOnce(Return(amsMgrScheduler));
    EXPECT_CALL(*(static_cast<MockAmsMgrScheduler *>(amsMgrScheduler.GetRefPtr())), TerminateAbility(_)).Times(1);

    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->TerminateAbility(token_));
    APP_LOGI("ams_app_mgr_client_test_003 end");
}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient
 * SubFunction: TerminateAbility Function
 * FunctionPoints: AppMgrClient TerminateAbility interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AppMgrClient invoke TerminateAbility act normal without connect to service.
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_004, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_004 start");
    EXPECT_EQ(AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED, client_->TerminateAbility(token_));
    APP_LOGI("ams_app_mgr_client_test_004 end");
}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient
 * SubFunction: UpdateAbilityState Function
 * FunctionPoints: AppMgrClient UpdateAbilityState interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AppMgrClient invoke UpdateAbilityState works.
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_005, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_005 start");
    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->ConnectAppMgrService());

    sptr<IAmsMgr> amsMgrScheduler(new MockAmsMgrScheduler());
    EXPECT_CALL(*(static_cast<MockAmsMgrScheduler *>(amsMgrScheduler.GetRefPtr())), UpdateAbilityState(_, _)).Times(1);
    EXPECT_CALL(*(static_cast<MockAppMgrService *>(client_->remote_.GetRefPtr())), GetAmsMgr())
        .Times(1)
        .WillOnce(Return(amsMgrScheduler));

    AbilityState state = AbilityState::ABILITY_STATE_BEGIN;
    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->UpdateAbilityState(token_, state));
    APP_LOGI("ams_app_mgr_client_test_005 end");
}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient
 * SubFunction: UpdateAbilityState Function
 * FunctionPoints: AppMgrClient UpdateAbilityState interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AppMgrClient invoke UpdateAbilityState act normal without connect to service.
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_006, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_006 start");
    AbilityState state = AbilityState::ABILITY_STATE_BEGIN;
    EXPECT_EQ(AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED, client_->UpdateAbilityState(token_, state));
    APP_LOGI("ams_app_mgr_client_test_006 end");
}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient
 * SubFunction: RegisterAppStateCallback Function
 * FunctionPoints: AppMgrClient RegisterAppStateCallback interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AppMgrClient invoke RegisterAppStateCallback works.
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_007, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_007 start");
    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->ConnectAppMgrService());
    sptr<MockAppStateCallback> mockCallback(new MockAppStateCallback());
    sptr<IAppStateCallback> callback = iface_cast<IAppStateCallback>(mockCallback);

    sptr<IAmsMgr> amsMgrScheduler(new MockAmsMgrScheduler());
    EXPECT_CALL(*(static_cast<MockAppMgrService *>(client_->remote_.GetRefPtr())), GetAmsMgr())
        .Times(1)
        .WillOnce(Return(amsMgrScheduler));

    EXPECT_CALL(*mockCallback, OnAbilityRequestDone(_, _)).Times(1);
    EXPECT_CALL(*mockCallback, OnAppStateChanged(_)).Times(1);

    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->RegisterAppStateCallback(callback));
    APP_LOGI("ams_app_mgr_client_test_007 end");
}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient
 * SubFunction: RegisterAppStateCallback Function
 * FunctionPoints: AppMgrClient RegisterAppStateCallback interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if AppMgrClient invoke RegisterAppStateCallback act normal without connect to service.
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_008, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_008 start");
    sptr<IAppStateCallback> callback;
    EXPECT_EQ(AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED, client_->RegisterAppStateCallback(callback));
    APP_LOGI("ams_app_mgr_client_test_008 end");
}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient::AbilityBehaviorAnalysis
 * SubFunction: RegisterAppStateCallback Function
 * FunctionPoints: AppMgrClient AbilityBehaviorAnalysis interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: ability behavior notification
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_009, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_008 start");
    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->ConnectAppMgrService());
    sptr<IRemoteObject> token;
    sptr<IRemoteObject> preToken;

    sptr<IAmsMgr> amsMgrScheduler(new MockAmsMgrScheduler());
    EXPECT_CALL(
        *(static_cast<MockAmsMgrScheduler *>(amsMgrScheduler.GetRefPtr())), AbilityBehaviorAnalysis(_, _, _, _, _))
        .Times(1);
    EXPECT_CALL(*(static_cast<MockAppMgrService *>(client_->remote_.GetRefPtr())), GetAmsMgr())
        .Times(1)
        .WillOnce(Return(amsMgrScheduler));

    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->AbilityBehaviorAnalysis(token, preToken, 1, 1, 1));
    APP_LOGI("ams_app_mgr_client_test_008 end");
}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient::KillApplication
 * SubFunction: RegisterAppStateCallback Function
 * FunctionPoints: AppMgrClient KillApplication interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Notify app of death
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_010, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_008 start");
    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->ConnectAppMgrService());
    sptr<IRemoteObject> token;
    sptr<IRemoteObject> preToken;

    sptr<IAmsMgr> amsMgrScheduler(new MockAmsMgrScheduler());
    EXPECT_CALL(*(static_cast<MockAmsMgrScheduler *>(amsMgrScheduler.GetRefPtr())), KillApplication(_))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    EXPECT_CALL(*(static_cast<MockAppMgrService *>(client_->remote_.GetRefPtr())), GetAmsMgr())
        .Times(1)
        .WillOnce(Return(amsMgrScheduler));

    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->KillApplication(""));
    APP_LOGI("ams_app_mgr_client_test_008 end");
}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient::KillApplication
 * SubFunction: RegisterAppStateCallback Function
 * FunctionPoints: AppMgrClient KillApplication interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Notify app of death
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_011, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_011 start");
    sptr<IRemoteObject> token;
    sptr<IRemoteObject> preToken;
    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->ConnectAppMgrService());
    sptr<IAmsMgr> amsMgrScheduler(new MockAmsMgrScheduler());
    EXPECT_CALL(*(static_cast<MockAmsMgrScheduler *>(amsMgrScheduler.GetRefPtr())), KillApplication(_))
        .Times(1)
        .WillOnce(Return(ERR_NO_MEMORY));
    EXPECT_CALL(*(static_cast<MockAppMgrService *>(client_->remote_.GetRefPtr())), GetAmsMgr())
        .Times(1)
        .WillOnce(Return(amsMgrScheduler));

    EXPECT_EQ(AppMgrResultCode::ERROR_SERVICE_NOT_READY, client_->KillApplication(""));
    APP_LOGI("ams_app_mgr_client_test_011 end");
}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient::KillProcessByAbilityToken
 * SubFunction: RegisterAppStateCallback Function
 * FunctionPoints: AppMgrClient KillApplication interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Notify app of death
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_012, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_012 start");
    sptr<IRemoteObject> token;
    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->ConnectAppMgrService());
    sptr<IAmsMgr> amsMgrScheduler(new MockAmsMgrScheduler());
    EXPECT_CALL(*(static_cast<MockAmsMgrScheduler *>(amsMgrScheduler.GetRefPtr())), KillProcessByAbilityToken(_))
        .Times(1);

    EXPECT_CALL(*(static_cast<MockAppMgrService *>(client_->remote_.GetRefPtr())), GetAmsMgr())
        .Times(1)
        .WillOnce(Return(amsMgrScheduler));

    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->KillProcessByAbilityToken(token));
    APP_LOGI("ams_app_mgr_client_test_012 end");
}

/*
 * Feature: AppMgrService
 * Function: AppMgrClient::KillProcessByAbilityToken
 * SubFunction: RegisterAppStateCallback Function
 * FunctionPoints: AppMgrClient KillApplication interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Notify app of death
 */
HWTEST_F(AmsAppMgrClientTest, AppMgrClient_013, TestSize.Level0)
{
    APP_LOGI("ams_app_mgr_client_test_013 start");
    sptr<IRemoteObject> token;
    EXPECT_EQ(AppMgrResultCode::RESULT_OK, client_->ConnectAppMgrService());
    sptr<IAmsMgr> amsMgrScheduler(new MockAmsMgrScheduler());
    EXPECT_CALL(*(static_cast<MockAmsMgrScheduler *>(amsMgrScheduler.GetRefPtr())), KillProcessByAbilityToken(_))
        .Times(0);

    EXPECT_CALL(*(static_cast<MockAppMgrService *>(client_->remote_.GetRefPtr())), GetAmsMgr())
        .Times(1)
        .WillOnce(Return(nullptr));

    EXPECT_EQ(AppMgrResultCode::ERROR_SERVICE_NOT_CONNECTED, client_->KillProcessByAbilityToken(token));
    APP_LOGI("ams_app_mgr_client_test_013 end");
}