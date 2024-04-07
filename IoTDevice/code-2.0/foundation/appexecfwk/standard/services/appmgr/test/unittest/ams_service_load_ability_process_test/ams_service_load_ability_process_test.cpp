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
#include "app_mgr_service_inner.h"
#undef private

#include <limits>
#include "gtest/gtest.h"
#include "ability_running_record.h"
#include "app_mgr_service.h"
#include "app_running_record.h"
#include "app_log_wrapper.h"
#include "app_record_id.h"
#include "ability_info.h"
#include "application_info.h"
#include "mock_app_scheduler.h"
#include "mock_bundle_manager.h"
#include "mock_ability_token.h"
#include "mock_app_spawn_client.h"

using namespace testing::ext;
using testing::_;
using testing::Return;
using testing::SetArgReferee;
namespace OHOS {
namespace AppExecFwk {
class AmsServiceLoadAbilityProcessTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    static const std::string GetTestAppName()
    {
        return "test_app_name";
    }
    static const std::string GetTestAbilityName()
    {
        return "test_ability_name";
    }

    std::shared_ptr<AppRunningRecord> StartLoadAbility(const sptr<IRemoteObject> &token,
        const sptr<IRemoteObject> &preToken, const std::shared_ptr<AbilityInfo> &abilityInfo,
        const std::shared_ptr<ApplicationInfo> &appInfo, const pid_t newPid) const;

    sptr<MockAbilityToken> GetMockToken() const
    {
        return mock_token_;
    }

protected:
    std::unique_ptr<AppMgrServiceInner> service_;
    sptr<MockAbilityToken> mock_token_;
    sptr<BundleMgrService> bundleMgr_;
};

void AmsServiceLoadAbilityProcessTest::SetUpTestCase()
{}

void AmsServiceLoadAbilityProcessTest::TearDownTestCase()
{}

void AmsServiceLoadAbilityProcessTest::SetUp()
{
    bundleMgr_ = new (std::nothrow) BundleMgrService();
    service_.reset(new (std::nothrow) AppMgrServiceInner());
    mock_token_ = new (std::nothrow) MockAbilityToken();
    if (service_) {
        service_->SetBundleManager(bundleMgr_.GetRefPtr());
    }
}

void AmsServiceLoadAbilityProcessTest::TearDown()
{}

std::shared_ptr<AppRunningRecord> AmsServiceLoadAbilityProcessTest::StartLoadAbility(const sptr<IRemoteObject> &token,
    const sptr<IRemoteObject> &preToken, const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<ApplicationInfo> &appInfo, const pid_t newPid) const
{
    RecordQueryResult result;
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(1).WillOnce(DoAll(SetArgReferee<1>(newPid), Return(ERR_OK)));

    service_->SetAppSpawnClient(mockClientPtr);
    service_->LoadAbility(token, preToken, abilityInfo, appInfo);
    std::shared_ptr<AppRunningRecord> record =
        service_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, abilityInfo->process, 0, result);
    EXPECT_EQ(record->GetPriorityObject()->GetPid(), newPid);
    return record;
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive loadAbility requests.
 * EnvConditions: NA
 * CaseDescription: Normal loadAbility requesets handled inner service.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, LoadAbility_001, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_001 start");
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    sptr<IRemoteObject> token = GetMockToken();
    StartLoadAbility(token, nullptr, abilityInfo, appInfo, PID);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);
    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_001 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive loadAbility requests.
 * EnvConditions: NA
 * CaseDescription: Multiple different loadAbility requesets handled inner service.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, LoadAbility_002, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_002 start");

    EXPECT_TRUE(service_ != nullptr);
    service_->ClearRecentAppList();

    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    appInfo->process = GetTestAppName();

    const pid_t PID = 1234;
    StartLoadAbility(token, nullptr, abilityInfo, appInfo, PID);

    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);

    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);

    sptr<IRemoteObject> token2 = GetMockToken();
    auto abilityInfo2 = std::make_shared<AbilityInfo>();
    abilityInfo2->name = GetTestAbilityName() + "_1";
    abilityInfo2->applicationName = GetTestAppName() + "_1";
    abilityInfo2->process = GetTestAppName() + "_1";

    auto appInfo2 = std::make_shared<ApplicationInfo>();
    appInfo2->name = GetTestAppName() + "_1";
    appInfo2->bundleName = GetTestAppName() + "_1";
    const pid_t PID2 = 2234;

    StartLoadAbility(token2, nullptr, abilityInfo2, appInfo2, PID2);
    const uint32_t EXPECT_MAP_SIZE = 2;
    EXPECT_EQ(recordMap.size(), EXPECT_MAP_SIZE);
    auto record2 = service_->GetAppRunningRecordByAppName(GetTestAppName() + "_1");
    ASSERT_NE(record2, nullptr);
    EXPECT_EQ(record2->GetState(), ApplicationState::APP_STATE_CREATE);
    auto abilityRecord2 = record2->GetAbilityRunningRecordByToken(token2);
    ASSERT_NE(abilityRecord2, nullptr);
    EXPECT_EQ(abilityRecord2->GetState(), AbilityState::ABILITY_STATE_CREATE);
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_002 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive loadAbility requests.
 * EnvConditions: NA
 * CaseDescription: Null abilityId loadAbility requesets handled inner service.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, LoadAbility_003, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_003 start");
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    EXPECT_TRUE(service_ != nullptr);

    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(0);

    service_->SetAppSpawnClient(mockClientPtr);
    service_->LoadAbility(nullptr, nullptr, abilityInfo, appInfo);

    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)0);
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_003 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive loadAbility requests.
 * EnvConditions: NA
 * CaseDescription: Null abilityInfo name loadAbility requesets handled inner service.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, LoadAbility_004, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_004 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "";
    abilityInfo->applicationName = GetTestAppName();
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    EXPECT_TRUE(service_ != nullptr);

    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(0);

    service_->SetAppSpawnClient(mockClientPtr);
    service_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)0);
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_004 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive loadAbility requests.
 * EnvConditions: NA
 * CaseDescription: Null appInfo name loadAbility requesets handled inner service.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, LoadAbility_005, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_005 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = "";
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = "";
    appInfo->bundleName = "";
    EXPECT_TRUE(service_ != nullptr);
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(0);

    service_->SetAppSpawnClient(mockClientPtr);
    service_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)0);
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_005 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive loadAbility requests.
 * EnvConditions: NA
 * CaseDescription: Different name loadAbility requesets handled inner service.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, LoadAbility_006, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_006 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName() + "_1";
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    EXPECT_TRUE(service_ != nullptr);

    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(0);

    service_->SetAppSpawnClient(mockClientPtr);
    service_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)0);
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_006 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive loadAbility requests.
 * EnvConditions: NA
 * CaseDescription: Multiple same loadAbility requesets handled inner service.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, LoadAbility_007, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_007 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    StartLoadAbility(token, nullptr, abilityInfo, appInfo, PID);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);
    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(0);

    service_->SetAppSpawnClient(mockClientPtr);
    service_->LoadAbility(token, nullptr, abilityInfo, appInfo);
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record2 = service_->GetAppRunningRecordByAppName(GetTestAppName());
    EXPECT_EQ(record2, record);
    const auto &abilityMap2 = record2->GetAbilities();
    EXPECT_EQ(abilityMap2.size(), (uint32_t)1);
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_007 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive loadAbility requests.
 * EnvConditions: NA
 * CaseDescription: Multiple different ability with same appName loadAbility requesets handled inner service.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, LoadAbility_008, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_008 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    StartLoadAbility(token, nullptr, abilityInfo, appInfo, PID);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);
    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);
    sptr<IRemoteObject> token2 = new MockAbilityToken();
    sptr<IRemoteObject> preToken = token;
    auto abilityInfo2 = std::make_shared<AbilityInfo>();
    abilityInfo2->name = GetTestAbilityName() + "_1";
    abilityInfo2->applicationName = GetTestAppName();
    const uint32_t EXPECT_MAP_SIZE = 2;
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(0);

    service_->SetAppSpawnClient(mockClientPtr);
    service_->LoadAbility(token2, preToken, abilityInfo2, appInfo);
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record2 = service_->GetAppRunningRecordByAppName(GetTestAppName());
    EXPECT_EQ(record2, record);
    const auto &abilityMap2 = record2->GetAbilities();
    EXPECT_EQ(abilityMap2.size(), EXPECT_MAP_SIZE);
    auto abilityRecord2 = record2->GetAbilityRunningRecordByToken(token2);
    ASSERT_NE(abilityRecord2, nullptr);
    EXPECT_EQ(abilityRecord2->GetState(), AbilityState::ABILITY_STATE_CREATE);
    EXPECT_EQ(abilityRecord2->GetPreToken(), token);
    APP_LOGI("AmsServiceLoadAbilityProcessTest LoadAbility_008 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive loadAbility requests and needd create new process.
 * EnvConditions: NA
 * CaseDescription: Normal loadAbility requesets handled when start process success.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, RequestProcess_001, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest RequestProcess_001 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(1).WillOnce(DoAll(SetArgReferee<1>(PID), Return(ERR_OK)));

    service_->SetAppSpawnClient(mockClientPtr);
    service_->LoadAbility(token, nullptr, abilityInfo, appInfo);

    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetPriorityObject()->GetPid(), PID);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    APP_LOGI("AmsServiceLoadAbilityProcessTest RequestProcess_001 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive loadAbility requests and needd create new process.
 * EnvConditions: NA
 * CaseDescription: Normal loadAbility requesets handled when start process failed.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, RequestProcess_002, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest RequestProcess_002 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    EXPECT_TRUE(service_ != nullptr);
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(1).WillOnce(Return(ERR_APPEXECFWK_INVALID_PID));

    service_->SetAppSpawnClient(mockClientPtr);
    service_->LoadAbility(token, nullptr, abilityInfo, appInfo);

    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)0);
    APP_LOGI("AmsServiceLoadAbilityProcessTest RequestProcess_002 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: the Service save pid to app running record when create new process successfully.
 * EnvConditions: NA
 * CaseDescription: Normal loadAbility and save pid to app running record.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, SavePid_001, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest SavePid_001 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(1).WillOnce(DoAll(SetArgReferee<1>(PID), Return(ERR_OK)));

    service_->SetAppSpawnClient(mockClientPtr);
    service_->LoadAbility(token, nullptr, abilityInfo, appInfo);

    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    EXPECT_EQ(record->GetPriorityObject()->GetPid(), PID);
    APP_LOGI("AmsServiceLoadAbilityProcessTest SavePid_001 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: the Service save pid to app running record when create new process failed.
 * EnvConditions: NA
 * CaseDescription: The service can't save pid to app running record when create new process failed.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, SavePid_002, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest SavePid_002 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    EXPECT_TRUE(service_ != nullptr);
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(1).WillOnce(Return(ERR_APPEXECFWK_INVALID_PID));

    service_->SetAppSpawnClient(mockClientPtr);
    service_->LoadAbility(token, nullptr, abilityInfo, appInfo);

    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    EXPECT_EQ(record, nullptr);
    APP_LOGI("AmsServiceLoadAbilityProcessTest SavePid_002 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive loadAbility requests.
 * EnvConditions: NA
 * CaseDescription: Normal loadAbility requeset with singleton launch mode handled inner service.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, LaunchMode_001, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest LaunchMode_001 start");
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();
    abilityInfo->launchMode = LaunchMode::SINGLETON;
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    sptr<IRemoteObject> token = GetMockToken();
    StartLoadAbility(token, nullptr, abilityInfo, appInfo, PID);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);
    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);
    APP_LOGI("AmsServiceLoadAbilityProcessTest LaunchMode_001 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive loadAbility requests.
 * EnvConditions: NA
 * CaseDescription: Multiple same loadAbility requesets with singleton launch mode and same ability info.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, LaunchMode_002, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest LaunchMode_002 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();
    abilityInfo->launchMode = LaunchMode::SINGLETON;

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    StartLoadAbility(token, nullptr, abilityInfo, appInfo, PID);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);
    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(0);
    sptr<IRemoteObject> token2 = new MockAbilityToken();
    sptr<IRemoteObject> preToken = token;
    service_->SetAppSpawnClient(mockClientPtr);
    service_->LoadAbility(token2, preToken, abilityInfo, appInfo);
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record2 = service_->GetAppRunningRecordByAppName(GetTestAppName());
    EXPECT_EQ(record2, record);
    const auto &abilityMap2 = record2->GetAbilities();
    EXPECT_EQ(abilityMap2.size(), (uint32_t)1);
    APP_LOGI("AmsServiceLoadAbilityProcessTest LaunchMode_002 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive startAbility requests.
 * EnvConditions: NA
 * CaseDescription: startAbility requesets with ability info.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, StartAbility_001, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartAbility_001 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    StartLoadAbility(token, nullptr, abilityInfo, appInfo, PID);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);
    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);
    sptr<IRemoteObject> token2 = new MockAbilityToken();
    auto abilityInfo2 = std::make_shared<AbilityInfo>();
    abilityInfo2->name = GetTestAbilityName() + "_1";
    abilityInfo2->applicationName = GetTestAppName();
    abilityInfo2->process = GetTestAppName();

    record->SetState(ApplicationState::APP_STATE_FOREGROUND);
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    record->SetApplicationClient(client);
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchAbility(_, _)).Times(1);

    service_->StartAbility(token2, token, abilityInfo2, record);
    auto record1 = service_->GetAppRunningRecordByAppName(GetTestAppName());
    const auto &abilityMap1 = record1->GetAbilities();
    EXPECT_EQ(abilityMap1.size(), (uint32_t)2);
    auto abilityrecord1 = record1->GetAbilityRunningRecordByToken(token2);
    ASSERT_NE(abilityrecord1, nullptr);
    EXPECT_EQ(abilityrecord1->GetState(), AbilityState::ABILITY_STATE_READY);
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartAbility_001 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive startAbility requests.
 * EnvConditions: NA
 * CaseDescription: startAbility requesets with not apprecord.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, StartAbility_002, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartAbility_002 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    StartLoadAbility(token, nullptr, abilityInfo, appInfo, PID);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);
    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);
    sptr<IRemoteObject> token2 = new MockAbilityToken();
    auto abilityInfo2 = std::make_shared<AbilityInfo>();
    abilityInfo2->name = GetTestAbilityName() + "_1";
    abilityInfo2->applicationName = GetTestAppName();
    abilityInfo2->process = GetTestAppName();
    record->SetState(ApplicationState::APP_STATE_FOREGROUND);
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    record->SetApplicationClient(client);
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchAbility(_, _)).Times(0);

    service_->StartAbility(token2, token, abilityInfo2, nullptr);
    auto record1 = service_->GetAppRunningRecordByAppName(GetTestAppName());
    const auto &abilityMap1 = record1->GetAbilities();
    EXPECT_EQ(abilityMap1.size(), (uint32_t)1);
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartAbility_002 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive startAbility requests.
 * EnvConditions: NA
 * CaseDescription: startAbility requesets with the same LaunchMode.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, StartAbility_003, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartAbility_003 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();
    abilityInfo->launchMode = LaunchMode::SINGLETON;

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();

    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    StartLoadAbility(token, nullptr, abilityInfo, appInfo, PID);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);
    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);

    sptr<IRemoteObject> token2 = new MockAbilityToken();
    auto abilityInfo2 = std::make_shared<AbilityInfo>();
    abilityInfo2->name = GetTestAbilityName() + "_1";
    abilityInfo2->launchMode = LaunchMode::SINGLETON;
    abilityInfo2->process = GetTestAppName();
    abilityInfo2->applicationName = GetTestAppName();

    record->SetState(ApplicationState::APP_STATE_FOREGROUND);
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    record->SetApplicationClient(client);
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchAbility(_, _)).Times(0);
    service_->StartAbility(token2, token, abilityInfo2, nullptr);
    auto record1 = service_->GetAppRunningRecordByAppName(GetTestAppName());
    const auto &abilityMap1 = record1->GetAbilities();
    EXPECT_EQ(abilityMap1.size(), (uint32_t)1);
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartAbility_003 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive startAbility requests.
 * EnvConditions: NA
 * CaseDescription: startAbility requesets with not token.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, StartAbility_004, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartAbility_004 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    StartLoadAbility(token, nullptr, abilityInfo, appInfo, PID);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);
    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);
    auto abilityInfo2 = std::make_shared<AbilityInfo>();
    abilityInfo2->name = GetTestAbilityName() + "_1";
    abilityInfo2->applicationName = GetTestAppName();
    abilityInfo2->process = GetTestAppName();
    record->SetState(ApplicationState::APP_STATE_FOREGROUND);
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    record->SetApplicationClient(client);
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchAbility(_, _)).Times(0);
    service_->StartAbility(nullptr, token, abilityInfo2, nullptr);

    auto record1 = service_->GetAppRunningRecordByAppName(GetTestAppName());
    const auto &abilityMap1 = record1->GetAbilities();
    EXPECT_EQ(abilityMap1.size(), (uint32_t)1);
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartAbility_004 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive startAbility requests.
 * EnvConditions: NA
 * CaseDescription: startAbility requesets with not preToken.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, StartAbility_005, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartAbility_005 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;

    EXPECT_TRUE(service_ != nullptr);
    StartLoadAbility(token, nullptr, abilityInfo, appInfo, PID);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);
    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);
    sptr<IRemoteObject> token2 = new MockAbilityToken();
    auto abilityInfo2 = std::make_shared<AbilityInfo>();
    abilityInfo2->name = GetTestAbilityName() + "_1";
    abilityInfo2->applicationName = GetTestAppName();
    abilityInfo2->process = GetTestAppName();
    record->SetState(ApplicationState::APP_STATE_FOREGROUND);
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    record->SetApplicationClient(client);
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchAbility(_, _)).Times(1);
    service_->StartAbility(token2, nullptr, abilityInfo2, record);
    auto record1 = service_->GetAppRunningRecordByAppName(GetTestAppName());
    const auto &abilityMap1 = record1->GetAbilities();
    EXPECT_EQ(abilityMap1.size(), (uint32_t)2);
    auto abilityrecord1 = record1->GetAbilityRunningRecordByToken(token2);
    ASSERT_NE(abilityrecord1, nullptr);
    EXPECT_EQ(abilityrecord1->GetState(), AbilityState::ABILITY_STATE_READY);
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartAbility_005 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive startAbility requests.
 * EnvConditions: NA
 * CaseDescription: startAbility requesets with ABILITY_STATE_CREATE.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, StartAbility_006, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartAbility_006 start");
    sptr<IRemoteObject> token = GetMockToken();
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    StartLoadAbility(token, nullptr, abilityInfo, appInfo, PID);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);
    auto abilityRecord = record->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);
    auto abilityInfo2 = std::make_shared<AbilityInfo>();
    abilityInfo2->name = GetTestAbilityName() + "_1";
    abilityInfo2->applicationName = GetTestAppName();
    abilityInfo2->process = GetTestAppName();
    sptr<MockAppScheduler> mockAppScheduler = new MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    record->SetApplicationClient(client);
    EXPECT_CALL(*mockAppScheduler, ScheduleLaunchAbility(_, _)).Times(0);
    service_->StartAbility(nullptr, token, abilityInfo2, nullptr);
    auto record1 = service_->GetAppRunningRecordByAppName(GetTestAppName());
    const auto &abilityMap1 = record1->GetAbilities();
    EXPECT_EQ(abilityMap1.size(), (uint32_t)1);
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartAbility_006 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive StartProcess requests.
 * EnvConditions: NA
 * CaseDescription: Normal StartProcess requesets handled inner service.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, StartProcess001, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartProcess001 start");
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();

    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    sptr<IRemoteObject> token = GetMockToken();
    RecordQueryResult result;
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(1).WillOnce(DoAll(SetArgReferee<1>(PID), Return(ERR_OK)));
    service_->SetAppSpawnClient(mockClientPtr);
    std::shared_ptr<AppRunningRecord> record =
        service_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, GetTestAppName(), 0, result);
    service_->StartProcess(abilityInfo->applicationName, GetTestAppName(), record);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record1 = service_->GetAppRunningRecordByAppName(GetTestAppName());
    EXPECT_EQ(record1->GetPriorityObject()->GetPid(), PID);
    ASSERT_NE(record1, nullptr);
    EXPECT_EQ(record1->GetState(), ApplicationState::APP_STATE_CREATE);
    const auto &abilityMap = record1->GetAbilities();
    EXPECT_EQ(abilityMap.size(), (uint32_t)1);
    auto abilityRecord = record1->GetAbilityRunningRecordByToken(token);
    ASSERT_NE(abilityRecord, nullptr);
    EXPECT_EQ(abilityRecord->GetState(), AbilityState::ABILITY_STATE_CREATE);
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartProcess001 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive StartProcess requests.
 * EnvConditions: NA
 * CaseDescription: Normal StartProcess requesets with not SpawnClient.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, StartProcess002, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartProcess002 start");
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    EXPECT_TRUE(service_ != nullptr);
    sptr<IRemoteObject> token = GetMockToken();
    RecordQueryResult result;
    std::shared_ptr<AppRunningRecord> record =
        service_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, GetTestAppName(), 0, result);
    service_->SetAppSpawnClient(nullptr);
    service_->StartProcess(abilityInfo->applicationName, GetTestAppName(), record);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record1 = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record1, nullptr);
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartProcess002 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive StartProcess requests.
 * EnvConditions: NA
 * CaseDescription: Normal StartProcess requesets with not AppRecord.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, StartProcess003, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartProcess003 start");
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    EXPECT_TRUE(service_ != nullptr);
    sptr<IRemoteObject> token = GetMockToken();
    RecordQueryResult result;
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    service_->SetAppSpawnClient(mockClientPtr);
    std::shared_ptr<AppRunningRecord> record =
        service_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, GetTestAppName(), 0, result);
    service_->StartProcess(abilityInfo->applicationName, GetTestAppName(), nullptr);
    const auto &recordMap = service_->GetRecordMap();
    EXPECT_EQ(recordMap.size(), (uint32_t)1);
    auto record1 = service_->GetAppRunningRecordByAppName(GetTestAppName());
    ASSERT_NE(record1, nullptr);
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartProcess003 end");
}

/*
 * Feature: AMS
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: When Service receive StartProcess requests.
 * EnvConditions: NA
 * CaseDescription: Normal StartProcess requesets with StartProcess return fail.
 */
HWTEST_F(AmsServiceLoadAbilityProcessTest, StartProcess004, TestSize.Level0)
{
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartProcess004 start");
    auto abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = GetTestAbilityName();
    abilityInfo->applicationName = GetTestAppName();
    abilityInfo->process = GetTestAppName();
    auto appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = GetTestAppName();
    appInfo->bundleName = GetTestAppName();
    const pid_t PID = 1234;
    EXPECT_TRUE(service_ != nullptr);
    sptr<IRemoteObject> token = GetMockToken();
    RecordQueryResult result;
    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    service_->SetAppSpawnClient(mockClientPtr);
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArgReferee<1>(PID), Return(ERR_TIMED_OUT)));
    std::shared_ptr<AppRunningRecord> record =
        service_->GetOrCreateAppRunningRecord(token, appInfo, abilityInfo, GetTestAppName(), 0, result);
    ASSERT_NE(record, nullptr);
    service_->StartProcess(abilityInfo->applicationName, GetTestAppName(), record);
    auto record1 = service_->GetAppRunningRecordByAppRecordId(record->GetRecordId());
    EXPECT_EQ(record1, nullptr);
    APP_LOGI("AmsServiceLoadAbilityProcessTest StartProcess004 end");
}

}  // namespace AppExecFwk
}  // namespace OHOS