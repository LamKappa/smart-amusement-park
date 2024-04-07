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
#include "app_death_recipient.h"
#include "app_mgr_service_inner.h"
#undef private
#include <gtest/gtest.h>
#include "app_log_wrapper.h"
#include "app_mgr_service_inner.h"
#include "mock_ability_token.h"
#include "mock_app_scheduler.h"
#include "mock_app_spawn_client.h"
#include "iremote_object.h"
#include "mock_bundle_manager.h"

using namespace testing::ext;
using testing::_;
using testing::Return;
using testing::SetArgReferee;

namespace OHOS {
namespace AppExecFwk {

class AppDeathRecipientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

public:
    const std::shared_ptr<AbilityInfo> GetAbilityInfoByIndex(const int32_t index) const;
    const std::shared_ptr<ApplicationInfo> GetApplicationByIndex(const int32_t index) const;
    const std::shared_ptr<AppRunningRecord> GetAppRunningRecordByIndex(const int32_t index) const;
    sptr<IRemoteObject> GetApp(int32_t pid, int size);

public:
    std::shared_ptr<AMSEventHandler> handler_;
    std::shared_ptr<AppMgrServiceInner> appMgrServiceInner_;
    sptr<AppDeathRecipient> appDeathRecipientObject_;
    OHOS::sptr<MockAbilityToken> mockToken_;
    OHOS::sptr<BundleMgrService> mockBundleMgr;
};

static void WaitUntilTaskFinished(std::shared_ptr<AMSEventHandler> handler)
{
    if (!handler) {
        return;
    }

    const uint32_t MAX_RETRY_COUNT = 1000;
    const uint32_t SLEEP_TIME = 1000;
    uint32_t count = 0;
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };
    if (handler->PostTask(f)) {
        while (!taskCalled.load()) {
            ++count;
            // if delay more than 1 second, break
            if (count >= MAX_RETRY_COUNT) {
                break;
            }

            usleep(SLEEP_TIME);
        }
    }
}

void AppDeathRecipientTest::SetUpTestCase()
{}

void AppDeathRecipientTest::TearDownTestCase()
{}

void AppDeathRecipientTest::SetUp()
{

    auto runner = EventRunner::Create("AppDeathRecipientTest");
    appMgrServiceInner_ = std::make_shared<AppMgrServiceInner>();

    handler_ = std::make_shared<AMSEventHandler>(runner, appMgrServiceInner_);

    appDeathRecipientObject_ = new (std::nothrow) AppDeathRecipient();

    mockBundleMgr = new (std::nothrow) BundleMgrService();
    appMgrServiceInner_->SetBundleManager(mockBundleMgr);
}

void AppDeathRecipientTest::TearDown()
{}

const std::shared_ptr<AbilityInfo> AppDeathRecipientTest::GetAbilityInfoByIndex(const int32_t index) const
{
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "AppDeathRecipientTest_ability" + std::to_string(index);
    abilityInfo->applicationName = "AppDeathRecipientTest" + std::to_string(index);
    return abilityInfo;
}

const std::shared_ptr<ApplicationInfo> AppDeathRecipientTest::GetApplicationByIndex(const int32_t index) const
{
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    appInfo->name = "AppDeathRecipientTest" + std::to_string(index);
    appInfo->bundleName = "AppDeathRecipientTest" + std::to_string(index);
    return appInfo;
}

const std::shared_ptr<AppRunningRecord> AppDeathRecipientTest::GetAppRunningRecordByIndex(const int32_t index) const
{
    auto appInfo = GetApplicationByIndex(index);
    auto appRecord = appMgrServiceInner_->GetAppRunningRecordByAppName(appInfo->name);
    EXPECT_NE(nullptr, appRecord);
    return appRecord;
}

sptr<IRemoteObject> AppDeathRecipientTest::GetApp(int32_t pid, int size)
{
    auto abilityInfo = GetAbilityInfoByIndex(pid);
    auto appInfo = GetApplicationByIndex(pid);
    sptr<IRemoteObject> token = new MockAbilityToken();

    std::shared_ptr<MockAppSpawnClient> mockClientPtr = std::make_shared<MockAppSpawnClient>();
    EXPECT_CALL(*mockClientPtr, StartProcess(_, _)).Times(1).WillOnce(DoAll(SetArgReferee<1>(pid), Return(ERR_OK)));
    std::shared_ptr<MockAppSpawnClient> mockClientstr(mockClientPtr);
    appMgrServiceInner_->SetAppSpawnClient(mockClientstr);

    appMgrServiceInner_->LoadAbility(token, nullptr, abilityInfo, appInfo);

    auto appRecord = GetAppRunningRecordByIndex(pid);

    EXPECT_EQ(size, static_cast<int>(appMgrServiceInner_->GetRecentAppList().size()));

    sptr<MockAppScheduler> mockAppScheduler = new (std::nothrow) MockAppScheduler();
    sptr<IAppScheduler> client = iface_cast<IAppScheduler>(mockAppScheduler.GetRefPtr());
    appRecord->SetApplicationClient(client);

    return client->AsObject();
}

/*
 * Feature: Ams
 * Function: SetEventHandler ande SetAppMgrServiceInner.
 * SubFunction: AppDeathRecipient
 * FunctionPoints: initialization
 * EnvConditions: have to an application
 * CaseDescription: How to set parameters
 */

HWTEST_F(AppDeathRecipientTest, AppDeathRecipient_001, TestSize.Level1)
{
    APP_LOGI("AppDeathRecipient_001 start");
    appDeathRecipientObject_->SetEventHandler(handler_);
    EXPECT_TRUE(appDeathRecipientObject_->handler_.lock() != nullptr);

    appDeathRecipientObject_->SetAppMgrServiceInner(appMgrServiceInner_);
    EXPECT_TRUE(appDeathRecipientObject_->appMgrServiceInner_.lock() != nullptr);
    APP_LOGI("AppDeathRecipient_001 end");
}

/*
 * Feature: Ams
 * Function: OnRemoteDied
 * SubFunction: AppDeathRecipient
 * FunctionPoints: Applied death notification
 * EnvConditions: have to an application
 * CaseDescription: Call back the death notification of the notification application
 */
HWTEST_F(AppDeathRecipientTest, AppDeathRecipient_002, TestSize.Level1)
{
    pid_t pid1 = 1024;
    pid_t pid2 = 1025;

    sptr<IRemoteObject> appOne = GetApp(pid1, 1);

    sptr<IRemoteObject> appTwo = GetApp(pid2, 2);

    appDeathRecipientObject_->SetEventHandler(handler_);
    appDeathRecipientObject_->SetAppMgrServiceInner(appMgrServiceInner_);
    appDeathRecipientObject_->OnRemoteDied(appOne);

    WaitUntilTaskFinished(handler_);
    EXPECT_EQ(1, static_cast<int>(appDeathRecipientObject_->appMgrServiceInner_.lock()->GetRecentAppList().size()));

    appDeathRecipientObject_->OnRemoteDied(appTwo);

    WaitUntilTaskFinished(handler_);
    EXPECT_EQ(0, static_cast<int>(appDeathRecipientObject_->appMgrServiceInner_.lock()->GetRecentAppList().size()));
    APP_LOGI("AppDeathRecipient_002 start");
}

}  // namespace AppExecFwk
}  // namespace OHOS