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

#include <chrono>
#include <thread>
#include <gtest/gtest.h>

#define private public
#define protected public
#include "data_ability_manager.h"
#include "app_scheduler.h"
#undef private
#undef protected

#include "ability_scheduler_mock.h"
#include "mock_app_mgr_client.h"

using namespace testing::ext;
using namespace testing;
using namespace std::chrono;

namespace OHOS {
namespace AAFwk {
class DataAbilityManagerTest : public testing::TestWithParam<OHOS::AAFwk::AbilityState> {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

protected:
    sptr<AbilitySchedulerMock> abilitySchedulerMock_;
    AbilityRequest abilityRequest_;
    std::shared_ptr<AbilityRecord> abilityRecordClient_;
    OHOS::AAFwk::AbilityState abilityState_;
};

void DataAbilityManagerTest::SetUpTestCase(void)
{}
void DataAbilityManagerTest::TearDownTestCase(void)
{}

void DataAbilityManagerTest::SetUp(void)
{
    if (abilitySchedulerMock_ == nullptr) {
        abilitySchedulerMock_ = new AbilitySchedulerMock();
    }

    abilityRequest_.appInfo.bundleName = "com.test.data_ability";
    abilityRequest_.appInfo.name = "com.test.data_ability";
    abilityRequest_.abilityInfo.name = "DataAbilityHiworld";
    abilityRequest_.abilityInfo.type = AbilityType::DATA;
    abilityRequest_.abilityInfo.bundleName = "com.test.data_ability";
    abilityRequest_.abilityInfo.deviceId = "device";

    if (abilityRecordClient_ == nullptr) {
        OHOS::AppExecFwk::AbilityInfo abilityInfo;
        abilityInfo.name = "DataAbilityClient";
        abilityInfo.type = AbilityType::PAGE;
        abilityInfo.bundleName = "com.test.request";
        abilityInfo.deviceId = "device";
        OHOS::AppExecFwk::ApplicationInfo applicationInfo;
        applicationInfo.bundleName = "com.test.request";
        applicationInfo.name = "com.test.request";
        const Want want;
        abilityRecordClient_ = std::make_shared<AbilityRecord>(want, abilityInfo, applicationInfo);
        abilityRecordClient_->Init();
    }
    abilityState_ = INITIAL;
}

void DataAbilityManagerTest::TearDown(void)
{
    abilitySchedulerMock_.clear();
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Normal Flow
 * FunctionPoints: DataAbilityManager simple flow.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify the DataAbilityManager simple flow.
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_Flow_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_Flow_001 start.");

    std::shared_ptr<DataAbilityManager> dataAbilityManager = std::make_shared<DataAbilityManager>();
    std::unique_ptr<MockAppMgrClient> mockAppMgrClient = std::make_unique<MockAppMgrClient>();

    // mock AppScheduler
    DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_ = std::move(mockAppMgrClient);

    auto func = [this, &dataAbilityManager]() {
        usleep(200 * 1000);  // 200 ms
        sptr<IRemoteObject> tokenAsyn =
            (reinterpret_cast<MockAppMgrClient *>(DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_.get()))
                ->GetToken();
        dataAbilityManager->AttachAbilityThread(abilitySchedulerMock_, tokenAsyn);
        dataAbilityManager->AbilityTransitionDone(tokenAsyn, ACTIVE);
    };

    std::thread(func).detach();
    EXPECT_CALL(*abilitySchedulerMock_, ScheduleAbilityTransaction(_, _)).Times(1);
    EXPECT_NE(dataAbilityManager->Acquire(abilityRequest_, true, abilityRecordClient_->GetToken()), nullptr);

    sptr<IRemoteObject> token =
        (reinterpret_cast<MockAppMgrClient *>(DelayedSingleton<AppScheduler>::GetInstance()->appMgrClient_.get()))
            ->GetToken();
    std::shared_ptr<AbilityRecord> abilityRecord = Token::GetAbilityRecordByToken(token);
    ASSERT_TRUE(abilityRecord);

    // existing ability record
    EXPECT_NE(dataAbilityManager->GetAbilityRecordByToken(token), nullptr);
    EXPECT_NE(dataAbilityManager->GetAbilityRecordByScheduler(abilitySchedulerMock_), nullptr);
    EXPECT_NE(dataAbilityManager->GetAbilityRecordById(abilityRecord->GetRecordId()), nullptr);

    // ability died, clear data ability record
    dataAbilityManager->OnAbilityDied(abilityRecord);

    // ability has released
    EXPECT_EQ(dataAbilityManager->GetAbilityRecordByToken(token), nullptr);
    EXPECT_EQ(dataAbilityManager->GetAbilityRecordByScheduler(abilitySchedulerMock_), nullptr);
    EXPECT_EQ(dataAbilityManager->GetAbilityRecordById(abilityRecord->GetRecordId()), nullptr);

    HILOG_INFO("AaFwk_DataAbilityManager_Flow_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Acquire
 * FunctionPoints: The parameter of function Acquire.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function Acquire parameter is nullptr.
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_Acquire_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_Acquire_001 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    EXPECT_EQ(dataAbilityManager->Acquire(abilityRequest_, true, nullptr), nullptr);

    HILOG_INFO("AaFwk_DataAbilityManager_Acquire_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Acquire
 * FunctionPoints: The parameter of function Acquire.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function Acquire parameter ability type is not data
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_Acquire_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_Acquire_002 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    // page ability type
    abilityRequest_.abilityInfo.type = AbilityType::PAGE;
    EXPECT_EQ(dataAbilityManager->Acquire(abilityRequest_, true, abilityRecordClient_->GetToken()), nullptr);

    HILOG_INFO("AaFwk_DataAbilityManager_Acquire_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Acquire
 * FunctionPoints: The parameter of function Acquire.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function Acquire parameter appinfo bundlename empty
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_Acquire_003, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_Acquire_003 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    // appinfo bundle name empty
    abilityRequest_.appInfo.bundleName = "";
    EXPECT_EQ(dataAbilityManager->Acquire(abilityRequest_, true, abilityRecordClient_->GetToken()), nullptr);

    HILOG_INFO("AaFwk_DataAbilityManager_Acquire_003 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Acquire
 * FunctionPoints: The parameter of function Acquire.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function Acquire parameter ability name empty
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_Acquire_004, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_Acquire_004 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    // ability name empty
    abilityRequest_.abilityInfo.name = "";
    EXPECT_EQ(dataAbilityManager->Acquire(abilityRequest_, true, abilityRecordClient_->GetToken()), nullptr);

    HILOG_INFO("AaFwk_DataAbilityManager_Acquire_004 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Acquire
 * FunctionPoints: The parameter of function Acquire.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function Acquire parameter same bundle name and ability name
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_Acquire_005, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_Acquire_005 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    // same bundle name and ability name
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.name = abilityRequest_.abilityInfo.name;
    abilityInfo.type = AbilityType::PAGE;
    OHOS::AppExecFwk::ApplicationInfo applicationInfo;
    applicationInfo.bundleName = abilityRequest_.appInfo.bundleName;
    applicationInfo.name = abilityRequest_.appInfo.name;
    const Want want;
    std::shared_ptr abilityRecordClient = std::make_shared<AbilityRecord>(want, abilityInfo, applicationInfo);
    abilityRecordClient->Init();

    EXPECT_EQ(dataAbilityManager->Acquire(abilityRequest_, true, abilityRecordClient->GetToken()), nullptr);

    HILOG_INFO("AaFwk_DataAbilityManager_Acquire_005 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Acquire
 * FunctionPoints: The parameter of function Acquire.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function Acquire waitforloaded timeout.
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_Acquire_006, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_Acquire_006 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    EXPECT_EQ(dataAbilityManager->Acquire(abilityRequest_, true, abilityRecordClient_->GetToken()), nullptr);

    HILOG_INFO("AaFwk_DataAbilityManager_Acquire_006 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Release
 * FunctionPoints: The parameter of function Release.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function Release client is nullptr
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_Release_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_Release_001 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    EXPECT_EQ(dataAbilityManager->Release(abilitySchedulerMock_, nullptr), ERR_NULL_OBJECT);

    HILOG_INFO("AaFwk_DataAbilityManager_Release_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Release
 * FunctionPoints: The parameter of function Release.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function Release scheduler is nullptr
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_Release_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_Release_002 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    EXPECT_EQ(dataAbilityManager->Release(nullptr, abilityRecordClient_->GetToken()), ERR_NULL_OBJECT);

    HILOG_INFO("AaFwk_DataAbilityManager_Release_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: Release
 * FunctionPoints: The parameter of function Release.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function Release ability record invalid
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_Release_003, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_Release_003 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    EXPECT_EQ(dataAbilityManager->Release(abilitySchedulerMock_, abilityRecordClient_->GetToken()), ERR_UNKNOWN_OBJECT);

    HILOG_INFO("AaFwk_DataAbilityManager_Release_003 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: AttachAbilityThread
 * FunctionPoints: The parameter of function AttachAbilityThread.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function AttachAbilityThread client is nullptr
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_AttachAbilityThread_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_AttachAbilityThread_001 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    EXPECT_EQ(dataAbilityManager->AttachAbilityThread(abilitySchedulerMock_, nullptr), ERR_NULL_OBJECT);

    HILOG_INFO("AaFwk_DataAbilityManager_AttachAbilityThread_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: AttachAbilityThread
 * FunctionPoints: The parameter of function AttachAbilityThread.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function AttachAbilityThread scheduler is nullptr
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_AttachAbilityThread_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_AttachAbilityThread_002 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    EXPECT_EQ(dataAbilityManager->AttachAbilityThread(nullptr, abilityRecordClient_->GetToken()), ERR_NULL_OBJECT);

    HILOG_INFO("AaFwk_DataAbilityManager_AttachAbilityThread_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: AttachAbilityThread
 * FunctionPoints: The parameter of function AttachAbilityThread.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function AttachAbilityThread ability record invalid
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_AttachAbilityThread_003, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_AttachAbilityThread_003 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    EXPECT_EQ(dataAbilityManager->AttachAbilityThread(abilitySchedulerMock_, abilityRecordClient_->GetToken()),
        ERR_UNKNOWN_OBJECT);

    HILOG_INFO("AaFwk_DataAbilityManager_AttachAbilityThread_003 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: AbilityTransitionDone
 * FunctionPoints: The parameter of function AbilityTransitionDone.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function AbilityTransitionDone token is nullptr
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_AbilityTransitionDone_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_AbilityTransitionDone_001 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    EXPECT_EQ(dataAbilityManager->AbilityTransitionDone(nullptr, INACTIVE), ERR_NULL_OBJECT);

    HILOG_INFO("AaFwk_DataAbilityManager_AbilityTransitionDone_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: AbilityTransitionDone
 * FunctionPoints: The parameter of function AbilityTransitionDone.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function AbilityTransitionDone ability record invalid
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_AbilityTransitionDone_002, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_AbilityTransitionDone_002 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();

    EXPECT_EQ(
        dataAbilityManager->AbilityTransitionDone(abilityRecordClient_->GetToken(), INACTIVE), ERR_UNKNOWN_OBJECT);

    HILOG_INFO("AaFwk_DataAbilityManager_AbilityTransitionDone_002 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: GetAbilityRecordByToken
 * FunctionPoints: The parameter of function GetAbilityRecordByToken.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function GetAbilityRecordByToken token is nullptr.
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_GetAbilityRecordByToken_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_GetAbilityRecordByToken_001 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();
    EXPECT_EQ(dataAbilityManager->GetAbilityRecordByToken(nullptr), nullptr);

    HILOG_INFO("AaFwk_DataAbilityManager_GetAbilityRecordByToken_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: GetAbilityRecordByScheduler
 * FunctionPoints: The parameter of function GetAbilityRecordByScheduler.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function GetAbilityRecordByScheduler token is nullptr.
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_GetAbilityRecordByScheduler_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_GetAbilityRecordByScheduler_001 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();
    EXPECT_EQ(dataAbilityManager->GetAbilityRecordByScheduler(nullptr), nullptr);

    HILOG_INFO("AaFwk_DataAbilityManager_GetAbilityRecordByScheduler_001 end.");
}

/*
 * Feature: AbilityManager
 * Function: DataAbility
 * SubFunction: GetAbilityRecordById
 * FunctionPoints: The parameter of function GetAbilityRecordById.
 * EnvConditions: Can run ohos test framework
 * CaseDescription: Verify function GetAbilityRecordById id is -1.
 */
HWTEST_F(DataAbilityManagerTest, AaFwk_DataAbilityManager_GetAbilityRecordById_001, TestSize.Level0)
{
    HILOG_INFO("AaFwk_DataAbilityManager_GetAbilityRecordById_001 start.");

    std::unique_ptr<DataAbilityManager> dataAbilityManager = std::make_unique<DataAbilityManager>();
    EXPECT_EQ(dataAbilityManager->GetAbilityRecordById(-1), nullptr);

    HILOG_INFO("AaFwk_DataAbilityManager_GetAbilityRecordById_001 end.");
}
}  // namespace AAFwk
}  // namespace OHOS