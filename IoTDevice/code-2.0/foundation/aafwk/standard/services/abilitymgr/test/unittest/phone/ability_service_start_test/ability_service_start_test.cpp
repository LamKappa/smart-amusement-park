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
#include "ability_manager_service.h"
#include "system_ability_definition.h"
#include "bundlemgr/mock_bundle_manager.h"
#include "sa_mgr_client.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
namespace {
const std::string NAME_BUNDLE_MGR_SERVICE = "BundleMgrService";
}

class AbilityServiceStartTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

public:
    std::shared_ptr<AbilityManagerService> aams_;
};

void AbilityServiceStartTest::SetUpTestCase()
{}
void AbilityServiceStartTest::TearDownTestCase()
{}
void AbilityServiceStartTest::TearDown()
{}

void AbilityServiceStartTest::SetUp()
{
    OHOS::sptr<OHOS::IRemoteObject> bundleObject = new BundleMgrService();
    OHOS::DelayedSingleton<SaMgrClient>::GetInstance()->RegisterSystemAbility(
        OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);

    aams_ = OHOS::DelayedSingleton<AbilityManagerService>::GetInstance();
}

/*
 * Feature: AbilityManagerService
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: AbilityManager startup & stop
 * EnvConditions: NA
 * CaseDescription: Verify if AbilityManagerService startup & stop successfully.
 */
HWTEST_F(AbilityServiceStartTest, StartUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ability_manager_service_startup_001 start";
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, aams_->QueryServiceState());
    aams_->OnStart();
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, aams_->QueryServiceState());
    aams_->OnStop();
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, aams_->QueryServiceState());
    GTEST_LOG_(INFO) << "ability_manager_service_startup_001 end";
}

/*
 * Feature: AbilityManagerService
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: AbilityManager startup two times
 * EnvConditions: NA
 * CaseDescription: Verify if AbilityManagerService startup & stop successfully.
 */
HWTEST_F(AbilityServiceStartTest, StartUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ability_manager_service_startup_002 start";
    aams_->OnStart();
    aams_->OnStart();
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, aams_->QueryServiceState());
    aams_->OnStop();
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, aams_->QueryServiceState());
    GTEST_LOG_(INFO) << "ability_manager_service_startup_002 end";
}

/*
 * Feature: AbilityManagerService
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: AbilityManager stop
 * EnvConditions: NA
 * CaseDescription: Verify if AbilityManagerService stop successfully.
 */
HWTEST_F(AbilityServiceStartTest, StartUp_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ability_manager_service_startup_003 start";
    aams_->OnStop();
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, aams_->QueryServiceState());
    GTEST_LOG_(INFO) << "ability_manager_service_startup_003 end";
}

/*
 * Feature: AbilityManagerService
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: AbilityManager stop again
 * EnvConditions: NA
 * CaseDescription: Verify if AbilityManagerService stop successfully.
 */
HWTEST_F(AbilityServiceStartTest, StartUp_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ability_manager_service_startup_004 start";
    aams_->OnStart();
    aams_->OnStop();
    aams_->OnStop();
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, aams_->QueryServiceState());
    GTEST_LOG_(INFO) << "ability_manager_service_startup_004 end";
}

/*
 * Feature: AbilityManagerService
 * Function: Service
 * SubFunction: NA
 * FunctionPoints: AbilityManager start & stop 10 times
 * EnvConditions: NA
 * CaseDescription: Verify if AbilityManagerService start & stop successfully.
 */
HWTEST_F(AbilityServiceStartTest, StartUp_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ability_manager_service_startup_005 start";
    for (int i = 0; i < 10; i++) {
        aams_->OnStart();
        auto handler = aams_->GetEventHandler();
        if (handler) {
            handler->RemoveTask("startLauncherAbility");
        }
        GTEST_LOG_(INFO) << "start " << i << "times";
        EXPECT_EQ(ServiceRunningState::STATE_RUNNING, aams_->QueryServiceState());
        aams_->OnStop();
        GTEST_LOG_(INFO) << "stop " << i << "times";
        EXPECT_EQ(ServiceRunningState::STATE_NOT_START, aams_->QueryServiceState());
    }
    GTEST_LOG_(INFO) << "ability_manager_service_startup_005 end";
}
}  // namespace AAFwk
}  // namespace OHOS