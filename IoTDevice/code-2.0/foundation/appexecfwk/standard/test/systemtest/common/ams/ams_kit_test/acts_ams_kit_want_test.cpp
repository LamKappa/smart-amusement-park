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
#include <condition_variable>
#include <cstdio>
#include <gtest/gtest.h>
#include <mutex>
#include <queue>
#include <thread>
#include <gtest/gtest.h>
#include "hilog_wrapper.h"
#include "semaphore_ex.h"
#include "skills.h"
#include "event.h"
#include "ability_lifecycle_executor.h"
#include "ability_lifecycle.h"
#include "ability_manager_service.h"
#include "ability_manager_errors.h"
#include "app_mgr_service.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "module_test_dump_util.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"
#include "system_test_ability_util.h"
#include "kit_test_common_info.h"
#include "testConfigParser.h"

namespace {
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::EventFwk;
using namespace OHOS::MTUtil;
using namespace OHOS::STtools;
using namespace OHOS::STABUtil;
using namespace testing::ext;
using MAP_STR_STR = std::map<std::string, std::string>;

static const std::string bundleName = "com.ohos.amsst.AppKit";
static const std::string secondAbilityName = "SecondAbility";

std::vector<std::string> bundleNameList = {
    "com.ohos.amsst.AppKit",
};
std::vector<std::string> hapNameList = {
    "amsKitSystemTest",
};
constexpr int WAIT_TIME = 1000;
int amsKitSystemTestSecondAbilityCode = 100;
constexpr int delay = 10;
}  // namespace

class ActsAmsKitWantTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static bool SubscribeEvent();
    void StartSecondAbility();
    class AppEventSubscriber : public CommonEventSubscriber {
    public:
        explicit AppEventSubscriber(const CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp){};
        virtual void OnReceiveEvent(const CommonEventData &data) override;
        ~AppEventSubscriber() = default;
    };

    static sptr<IAppMgr> appMs;
    static sptr<IAbilityManager> abilityMs;
    static Event event;
    static std::map<std::string, int> mapState;
    static StressTestLevel stLevel_;
    static std::shared_ptr<AppEventSubscriber> subscriber_;
};

std::map<std::string, int> ActsAmsKitWantTest::mapState = {
    {"OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
    {"OnStop", AbilityLifecycleExecutor::LifecycleState::INITIAL},
    {"OnActive", AbilityLifecycleExecutor::LifecycleState::ACTIVE},
    {"OnCommand", AbilityLifecycleExecutor::LifecycleState::ACTIVE},
    {"OnConnect", AbilityLifecycleExecutor::LifecycleState::ACTIVE},
    {"OnDisConnect", AbilityLifecycleExecutor::LifecycleState::BACKGROUND},
    {"OnInactive", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
    {"OnBackground", AbilityLifecycleExecutor::LifecycleState::BACKGROUND},
    {"OnForeground", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
    {"OnAbilityStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
    {"OnAbilityStop", AbilityLifecycleExecutor::LifecycleState::INITIAL},
    {"OnAbilityActive", AbilityLifecycleExecutor::LifecycleState::ACTIVE},
    {"OnAbilityInactive", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
    {"OnAbilityBackground", AbilityLifecycleExecutor::LifecycleState::BACKGROUND},
    {"OnAbilityForeground", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
    {"OnStateChanged", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
};

StressTestLevel ActsAmsKitWantTest::stLevel_{};
Event ActsAmsKitWantTest::event = Event();
sptr<IAppMgr> ActsAmsKitWantTest::appMs = nullptr;
sptr<IAbilityManager> ActsAmsKitWantTest::abilityMs = nullptr;
std::shared_ptr<ActsAmsKitWantTest::AppEventSubscriber> ActsAmsKitWantTest::subscriber_ = nullptr;

void ActsAmsKitWantTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "OnReceiveEvent: event=" << data.GetWant().GetAction();
    GTEST_LOG_(INFO) << "OnReceiveEvent: data=" << data.GetData();
    GTEST_LOG_(INFO) << "OnReceiveEvent: code=" << data.GetCode();
    STAbilityUtil::Completed(event, data.GetWant().GetAction(), data.GetCode(), data.GetData());
}

void ActsAmsKitWantTest::SetUpTestCase(void)
{
    STAbilityUtil::InstallHaps(hapNameList);
    if (!SubscribeEvent()) {
        GTEST_LOG_(INFO) << "SubscribeEvent error";
    }
    TestConfigParser tcp;
    tcp.ParseFromFile4StressTest(STRESS_TEST_CONFIG_FILE_PATH, stLevel_);
    std::cout << "stress test level : "
              << "AMS : " << stLevel_.AMSLevel << " "
              << "BMS : " << stLevel_.BMSLevel << " "
              << "CES : " << stLevel_.CESLevel << std::endl;
}

void ActsAmsKitWantTest::TearDownTestCase(void)
{
    STAbilityUtil::UninstallBundle(bundleNameList);
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void ActsAmsKitWantTest::SetUp(void)
{}

void ActsAmsKitWantTest::TearDown(void)
{
    STAbilityUtil::CleanMsg(event);
}

bool ActsAmsKitWantTest::SubscribeEvent()
{
    const std::vector<std::string> eventList = {
        g_EVENT_RESP_SECOND,

    };
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void ActsAmsKitWantTest::StartSecondAbility()
{
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", secondAbilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
    EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, mapState["onActive"], delay));
}

/**
 * @tc.number    : AMS_Page_Want_0100
 * @tc.name      : Want copy constructor
 * @tc.desc      : Want object copy
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_0100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::WantCopy) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_0100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_0200
 * @tc.name      : Want copy-assignment
 * @tc.desc      : Want object assignment
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_0200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::WantAssign) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_0200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_0300
 * @tc.name      : Want.AddEntity
 * @tc.desc      : add empty entity
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_0300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::AddEntity) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_0300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_0400
 * @tc.name      : Want.AddEntity
 * @tc.desc      : add entity with special character
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_0400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::AddEntity) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_0400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_0500
 * @tc.name      : Want.AddEntity
 * @tc.desc      : add entity continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_0500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::AddEntity) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_0500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_0600
 * @tc.name      : Want.AddFlags
 * @tc.desc      : add flags minimum
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_0600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::AddFlags) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_0600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_0700
 * @tc.name      : Want.AddFlags
 * @tc.desc      : add flags maximum
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_0700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::AddFlags) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_0700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_0800
 * @tc.name      : Want.AddFlags
 * @tc.desc      : add flags continuouly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_0800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::AddFlags) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_0800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_0900
 * @tc.name      : Want.ClearWant
 * @tc.desc      : clear want normally
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_0900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ClearWant) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_0900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_1000
 * @tc.name      : Want.ClearWant
 * @tc.desc      : clear want continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_1000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ClearWant) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_1000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_1100
 * @tc.name      : Want.CountEntities
 * @tc.desc      : count entity on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_1100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::CountEntities) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_1100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_1200
 * @tc.name      : Want.CountEntities
 * @tc.desc      : count entity after add entity
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_1200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::CountEntities) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_1200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_1300
 * @tc.name      : Want.FormatMimeType
 * @tc.desc      : format normal mime type
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_1300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatMimeType) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_1300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_1400
 * @tc.name      : Want.FormatMimeType
 * @tc.desc      : format uppercase mime type
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_1400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatMimeType) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_1400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_1500
 * @tc.name      : Want.FormatMimeType
 * @tc.desc      : format mime type with special character
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_1500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatMimeType) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_1500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_1600
 * @tc.name      : Want.FormatMimeType
 * @tc.desc      : format mime type with space and tab
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_1600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatMimeType) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_1600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_1700
 * @tc.name      : Want.FormatType
 * @tc.desc      : format normal type
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_1700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatType) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_1700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_1800
 * @tc.name      : Want.FormatType
 * @tc.desc      : format uppercase type
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_1800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatType) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_1800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_1900
 * @tc.name      : Want.FormatType
 * @tc.desc      : format type with special character
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_1900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatType) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_1900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_2000
 * @tc.name      : Want.FormatType
 * @tc.desc      : format type with space, uppercase and tab
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_2000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatType) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_2000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_2100
 * @tc.name      : Want.FormatUri
 * @tc.desc      : format uri string including duplicated property
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_2100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatUri) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_2100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_2200
 * @tc.name      : Want.FormatUri
 * @tc.desc      : format uri string including property of ElementName
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_2200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatUri) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_2200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_2300
 * @tc.name      : Want.FormatUri
 * @tc.desc      : format uri string including parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_2300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatUri) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_2300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_2400
 * @tc.name      : Want.FormatUri
 * @tc.desc      : format return value of ToUri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_2400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatUri) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_2400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_2500
 * @tc.name      : Want.FormatUriAndType
 * @tc.desc      :
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_2500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatUriAndType) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_2500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_2600
 * @tc.name      : Want.FormatUriAndType
 * @tc.desc      :
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_2600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatUriAndType) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_2600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_2700
 * @tc.name      : Want.FormatUriAndType
 * @tc.desc      :
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_2700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatUriAndType) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_2700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_2800
 * @tc.name      : Want.FormatUriAndType
 * @tc.desc      :
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_2800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::FormatUriAndType) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_2800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_2900
 * @tc.name      : Want.GetAction
 * @tc.desc      : get empty action
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_2900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetAction) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_2900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_3000
 * @tc.name      : Want.GetAction
 * @tc.desc      : set and get action
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_3000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetAction) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_3000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_3100
 * @tc.name      : Want.GetAction
 * @tc.desc      : set and get action continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_3100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetAction) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_3100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_3200
 * @tc.name      : Want.GetBundle
 * @tc.desc      : get empty bundle
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_3200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetBundle) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_3200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_3300
 * @tc.name      : Want.GetBundle
 * @tc.desc      : set and get bundle
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_3300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetBundle) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_3300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_3400
 * @tc.name      : Want.GetBundle
 * @tc.desc      : set and get bundle continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_3400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetBundle) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_3400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_3500
 * @tc.name      : Want.GetEntities
 * @tc.desc      : get empty entities
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_3500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetEntities) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_3500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_3600
 * @tc.name      : Want.GetEntities
 * @tc.desc      : add entity and get entities
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_3600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetEntities) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_3600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_3700
 * @tc.name      : Want.GetEntities
 * @tc.desc      : add entity and get entities continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_3700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetEntities) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_3700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_3800
 * @tc.name      : Want.GetElement
 * @tc.desc      : get empty element
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_3800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetElement) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_3800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_3900
 * @tc.name      : Want.GetElement
 * @tc.desc      : set and get element
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_3900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetElement) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_3900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_4000
 * @tc.name      : Want.GetElement
 * @tc.desc      : set and get element continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_4000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetElement) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_4000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_4100
 * @tc.name      : Want.GetUri
 * @tc.desc      : get empty uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_4100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetUri) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_4100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_4200
 * @tc.name      : Want.GetUri
 * @tc.desc      : set and get uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_4200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetUri) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_4200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_4300
 * @tc.name      : Want.GetUri
 * @tc.desc      : set and get uri continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_4300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetUri) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_4300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_4400
 * @tc.name      : Want.GetUriString
 * @tc.desc      : get empty uri string
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_4400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetUriString) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_4400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_4500
 * @tc.name      : Want.GetUriString
 * @tc.desc      : set property and get uri string
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_4500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetUriString) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_4500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_4600
 * @tc.name      : Want.GetUriString
 * @tc.desc      : set uri and get uri string
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_4600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetUriString) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_4600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_4700
 * @tc.name      : Want.GetUriString
 * @tc.desc      : get uri string continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_4700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetUriString) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_4700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_4800
 * @tc.name      : Want.GetFlags
 * @tc.desc      : get empty flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_4800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetFlags) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_4800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_4900
 * @tc.name      : Want.GetFlags
 * @tc.desc      : set and get flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_4900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetFlags) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_4900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_5000
 * @tc.name      : Want.GetFlags
 * @tc.desc      : set and get flags continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_5000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetFlags) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_5000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_5100
 * @tc.name      : Want.GetScheme
 * @tc.desc      :
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_5100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetScheme) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_5100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_5200
 * @tc.name      : Want.GetScheme
 * @tc.desc      :
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_5200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetScheme) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_5200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_5300
 * @tc.name      : Want.GetScheme
 * @tc.desc      :
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_5300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetScheme) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_5300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_5400
 * @tc.name      : Want.GetScheme
 * @tc.desc      :
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_5400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetScheme) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_5400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_5500
 * @tc.name      : Want.GetType
 * @tc.desc      : get empty type
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_5500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetType) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_5500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_5600
 * @tc.name      : Want.GetType
 * @tc.desc      : set and get type
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_5600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetType) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_5600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_5700
 * @tc.name      : Want.GetType
 * @tc.desc      : set and get type continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_5700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetType) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_5700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_5800
 * @tc.name      : Want.HasEntity
 * @tc.desc      : no any entity
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_5800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::HasEntity) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_5800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_5900
 * @tc.name      : Want.HasEntity
 * @tc.desc      : no target entity
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_5900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::HasEntity) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_5900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_6000
 * @tc.name      : Want.HasEntity
 * @tc.desc      : target entity exists
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_6000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::HasEntity) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_6000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_6100
 * @tc.name      : Want.HasEntity
 * @tc.desc      : HasEntity continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_6100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::HasEntity) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_6100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_6200
 * @tc.name      : Want.MakeMainAbility
 * @tc.desc      : MakeMainAbility with empty ElementName
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_6200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::MakeMainAbility) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_6200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_6300
 * @tc.name      : Want.MakeMainAbility
 * @tc.desc      : MakeMainAbility with normal ElementName
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_6300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::MakeMainAbility) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_6300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_6400
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_6400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_6400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_6500
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with action
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_6500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_6500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_6600
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with type
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_6600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_6600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_6700
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_6700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_6700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_6800
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with entity
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_6800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_6800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_6900
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with element
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_6900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_6900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_7000
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with bool parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_7000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_7000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_7100
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with bool array parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_7100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_7100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_7200
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with byte parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_7200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_7200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_7300
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with byte array parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_7300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_9";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_7300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_7400
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with char parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_7400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_10";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_7400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_7500
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with char array parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_7500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_11";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_7500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_7600
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with int parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_7600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_12";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_7600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_7700
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with int array parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_7700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_13";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_7700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_7800
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with double parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_7800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_14";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_7800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_7900
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with double array parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_7900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_15";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_7900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_8000
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with float parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_8000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_16";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_8000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_8100
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with float array parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_8100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_17";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_8100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_8200
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with long parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_8200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_18";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_8200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_8300
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with long array parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_8300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_19";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_8300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_8400
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with short parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_8400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_20";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_8400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_8500
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with short array parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_8500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_21";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_8500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_8600
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with string parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_8600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_22";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_8600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_8700
 * @tc.name      : Want.Marshalling
 * @tc.desc      : marshall and unmarshall Want with string array parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_8700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::Marshalling) + "_23";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_8700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_8800
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse uri that has only key, no equals sign and value
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_8800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_8800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_8900
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse string parameters
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_8900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_8900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_9000
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse uri that has special character
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_9000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_9000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_9100
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse flag that is a hexadecimal starting with "0X"
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_9100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_9100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_9200
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse flag that has no value
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_9200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_9200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_9300
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse uri that has no head "#Want;"
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_9300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_9300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_9400
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse uri that flag type is string
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_9400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_9400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_9500
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse uri that has only all keys and equals sign, no value
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_9500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_9500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_9600
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse empty string
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_9600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_9600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_9700
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse parameter of float array and string array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_9700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_9";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_9700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_9800
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse parameter of float array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_9800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_10";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_9800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_9900
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse parameter of float and string
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_9900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_11";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_9900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_10000
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse action, entity, flags, element
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_10000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_12";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_10000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_10100
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse uri that make empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_10100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_13";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_10100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_10200
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse uri that has only key and euqals sign, no value
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_10200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_14";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_10200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_10300
 * @tc.name      : Want.RemoveEntity
 * @tc.desc      : remove entity from empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_10300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveEntity) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_10300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_10400
 * @tc.name      : Want.RemoveEntity
 * @tc.desc      : add and remove normal entity
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_10400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveEntity) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_10400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_10500
 * @tc.name      : Want.RemoveEntity
 * @tc.desc      : add and remove entity alternatively
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_10500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveEntity) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_10500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_10600
 * @tc.name      : Want.RemoveEntity
 * @tc.desc      : add and remove entity with special character
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_10600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveEntity) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_10600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_10700
 * @tc.name      : Want.RemoveEntity
 * @tc.desc      : add entity repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_10700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveEntity) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_10700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_10800
 * @tc.name      : Want.RemoveFlags
 * @tc.desc      : remove flags from empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_10800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveFlags) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_10800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_10900
 * @tc.name      : Want.RemoveFlags
 * @tc.desc      : add and remove flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_10900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveFlags) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_10900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_11000
 * @tc.name      : Want.RemoveFlags
 * @tc.desc      : set and remove flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_11000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveFlags) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_11000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_11100
 * @tc.name      : Want.RemoveFlags
 * @tc.desc      : set, add and remove flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_11100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveFlags) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_11100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_11200
 * @tc.name      : Want.RemoveFlags
 * @tc.desc      : remove flags repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_11200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveFlags) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_11200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_11300
 * @tc.name      : Want.SetAction
 * @tc.desc      : get empty action
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_11300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetAction) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_11300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_11400
 * @tc.name      : Want.SetAction
 * @tc.desc      : set and get empty action
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_11400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetAction) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_11400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_11500
 * @tc.name      : Want.SetAction
 * @tc.desc      : set and get long action
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_11500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetAction) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_11500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_11600
 * @tc.name      : Want.SetAction
 * @tc.desc      : set and get action with special character
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_11600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetAction) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_11600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_11700
 * @tc.name      : Want.SetAction
 * @tc.desc      : set and get action continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_11700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetAction) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_11700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_11800
 * @tc.name      : Want.SetBundle
 * @tc.desc      : get empty bundle
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_11800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetBundle) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_11800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_11900
 * @tc.name      : Want.SetBundle
 * @tc.desc      : set and get empty bundle
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_11900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetBundle) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_11900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_12000
 * @tc.name      : Want.SetBundle
 * @tc.desc      : set and get long bundle
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_12000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetBundle) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_12000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_12100
 * @tc.name      : Want.SetBundle
 * @tc.desc      : set and get bundle with special character
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_12100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetBundle) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_12100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_12200
 * @tc.name      : Want.SetBundle
 * @tc.desc      : set and get bundle continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_12200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetBundle) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_12200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_12300
 * @tc.name      : Want.SetElement
 * @tc.desc      : get empty element
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_12300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElement) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_12300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_12400
 * @tc.name      : Want.SetElement
 * @tc.desc      : set and get empty element
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_12400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElement) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_12400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_12500
 * @tc.name      : Want.SetElement
 * @tc.desc      : set and get element continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_12500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElement) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_12500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_12600
 * @tc.name      : Want.SetElementName_String_String
 * @tc.desc      : get empty ElementName
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_12600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElementName_String_String) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_12600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_12700
 * @tc.name      : Want.SetElementName_String_String
 * @tc.desc      : set and get empty ElementName
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_12700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElementName_String_String) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_12700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_12800
 * @tc.name      : Want.SetElementName_String_String
 * @tc.desc      : set and get long ElementName
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_12800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElementName_String_String) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_12800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_12900
 * @tc.name      : Want.SetElementName_String_String
 * @tc.desc      : set and get ElementName with special character
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_12900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElementName_String_String) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_12900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_13000
 * @tc.name      : Want.SetElementName_String_String
 * @tc.desc      : set and get ElementName continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_13000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElementName_String_String) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_13000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_13100
 * @tc.name      : Want.SetElementName_String_String_String
 * @tc.desc      : get empty ElementName
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_13100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElementName_String_String_String) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_13100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_13200
 * @tc.name      : Want.SetElementName_String_String_String
 * @tc.desc      : set and get empty ElementName
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_13200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElementName_String_String_String) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_13200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_13300
 * @tc.name      : Want.SetElementName_String_String_String
 * @tc.desc      : set and get long ElementName
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_13300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElementName_String_String_String) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_13300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_13400
 * @tc.name      : Want.SetElementName_String_String_String
 * @tc.desc      : set and get ElementName with special character
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_13400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElementName_String_String_String) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_13400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_13500
 * @tc.name      : Want.SetElementName_String_String_String
 * @tc.desc      : set and get ElementName continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_13500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetElementName_String_String_String) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_13500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_13600
 * @tc.name      : Want.SetFlags
 * @tc.desc      : get empty flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_13600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetFlags) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_13600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_13700
 * @tc.name      : Want.SetFlags
 * @tc.desc      : set max unsigned flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_13700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetFlags) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_13700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_13800
 * @tc.name      : Want.SetFlags
 * @tc.desc      : remove max unsigned flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_13800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetFlags) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_13800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_13900
 * @tc.name      : Want.SetFlags
 * @tc.desc      : set and remove max unsigned flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_13900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetFlags) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_13900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_14000
 * @tc.name      : Want.SetFlags
 * @tc.desc      : add max unsigned flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_14000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetFlags) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_14000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_14100
 * @tc.name      : Want.SetFlags
 * @tc.desc      : add and remove max unsigned flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_14100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetFlags) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_14100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_14200
 * @tc.name      : Want.SetFlags
 * @tc.desc      : set and add flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_14200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetFlags) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_14200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_14300
 * @tc.name      : Want.SetFlags
 * @tc.desc      : add and set flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_14300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetFlags) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_14300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_14400
 * @tc.name      : Want.SetFlags
 * @tc.desc      : add one flags and remove another flags
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_14400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetFlags) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_14400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_14500
 * @tc.name      : Want.SetFlags
 * @tc.desc      : add and remove flags continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_14500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetFlags) + "_9";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_14500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_14600
 * @tc.name      : Want.SetFlags
 * @tc.desc      : set flags continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_14600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetFlags) + "_10";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_14600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_14700
 * @tc.name      : Want.SetType
 * @tc.desc      : set empty type
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_14700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetType) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_14700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_14800
 * @tc.name      : Want.SetType
 * @tc.desc      : set twice and get once
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_14800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetType) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_14800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_14900
 * @tc.name      : Want.SetType
 * @tc.desc      : set type with special character
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_14900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetType) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_14900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_15000
 * @tc.name      : Want.SetType
 * @tc.desc      : set type repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_15000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetType) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_15000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_15100
 * @tc.name      : Want.SetUri
 * @tc.desc      : set empty uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_15100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetUri) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_15100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_15200
 * @tc.name      : Want.SetUri
 * @tc.desc      : set uri with property action
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_15200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetUri) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_15200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_15300
 * @tc.name      : Want.SetUri
 * @tc.desc      : set uri with string parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_15300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetUri) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_15300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_15400
 * @tc.name      : Want.SetUri
 * @tc.desc      : set uri continuously
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_15400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetUri) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_15400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_15500
 * @tc.name      : Want.SetUriAndType
 * @tc.desc      : set empty uri and type
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_15500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetUriAndType) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_15500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_15600
 * @tc.name      : Want.SetUriAndType
 * @tc.desc      : set bad format uri and type
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_15600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetUriAndType) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_15600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_15700
 * @tc.name      : Want.SetUriAndType
 * @tc.desc      : set uri with ElementName and type
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_15700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetUriAndType) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_15700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_15800
 * @tc.name      : Want.SetUriAndType
 * @tc.desc      : set uri and type repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_15800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetUriAndType) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_15800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_15900
 * @tc.name      : Want.ToUri
 * @tc.desc      : empty Want to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_15900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_15900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_16000
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with action to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_16000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_16000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_16100
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with entities to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_16100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_16100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_16200
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with flags to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_16200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_16200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_16300
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with ElementName to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_16300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_16300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_16400
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with string parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_16400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_16400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_16500
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with bool parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_16500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_16500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_16600
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with char parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_16600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_16600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_16700
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with byte parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_16700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_16700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_16800
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with short parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_16800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_9";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_16800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_16900
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with int parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_16900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_10";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_16900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_17000
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with long parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_17000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_11";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_17000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_17100
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with float parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_17100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_12";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_17100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_17200
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with double parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_17200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_13";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_17200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_17300
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with string array parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_17300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_14";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_17300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_17400
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with bool array parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_17400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_15";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_17400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_17500
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with char array parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_17500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_16";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_17500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_17600
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with byte array parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_17600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_17";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_17600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_17700
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with short array parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_17700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_18";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_17700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_17800
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with int array parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_17800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_19";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_17800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_17900
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with long array parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_17900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_20";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_17900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_18000
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with float array parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_18000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_21";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_18000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_18100
 * @tc.name      : Want.ToUri
 * @tc.desc      : Want with double array parameter to uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_18100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ToUri) + "_22";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_18100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_18200
 * @tc.name      : Want.WantParseUri
 * @tc.desc      : empty Want to and from uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_18200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::WantParseUri) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_18200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_18300
 * @tc.name      : Want.WantParseUri
 * @tc.desc      : Want with property to and from uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_18300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::WantParseUri) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_18300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_18400
 * @tc.name      : Want.WantParseUri
 * @tc.desc      : Want with parameter to and from uri
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_18400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::WantParseUri) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_18400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_18500
 * @tc.name      : Want.GetParams
 * @tc.desc      : get params from empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_18500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetParams) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_18500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_18600
 * @tc.name      : Want.GetParams
 * @tc.desc      : set and get params
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_18600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetParams) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_18600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_18700
 * @tc.name      : Want.GetParams
 * @tc.desc      : set and get params repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_18700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetParams) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_18700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_18800
 * @tc.name      : Want.GetByteParam
 * @tc.desc      : get byte on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_18800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetByteParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_18800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_18900
 * @tc.name      : Want.GetByteParam
 * @tc.desc      : get existed byte param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_18900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetByteParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_18900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_19000
 * @tc.name      : Want.GetByteParam
 * @tc.desc      : get byte param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_19000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetByteParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_19000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_19100
 * @tc.name      : Want.GetByteArrayParam
 * @tc.desc      : get byte array on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_19100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetByteArrayParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_19100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_19200
 * @tc.name      : Want.GetByteArrayParam
 * @tc.desc      : get existed byte array param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_19200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetByteArrayParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_19200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_19300
 * @tc.name      : Want.GetByteArrayParam
 * @tc.desc      : get byte array param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_19300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetByteArrayParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_19300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_19400
 * @tc.name      : Want.GetBoolParam
 * @tc.desc      : get bool on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_19400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetBoolParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_19400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_19500
 * @tc.name      : Want.GetBoolParam
 * @tc.desc      : get existed bool param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_19500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetBoolParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_19500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_19600
 * @tc.name      : Want.GetBoolParam
 * @tc.desc      : get bool param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_19600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetBoolParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_19600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_19700
 * @tc.name      : Want.GetBoolArrayParam
 * @tc.desc      : get bool array on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_19700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetBoolArrayParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_19700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_19800
 * @tc.name      : Want.GetBoolArrayParam
 * @tc.desc      : get existed bool array param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_19800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetBoolArrayParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_19800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_19900
 * @tc.name      : Want.GetBoolArrayParam
 * @tc.desc      : get bool array param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_19900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetBoolArrayParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_19900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_20000
 * @tc.name      : Want.GetCharParam
 * @tc.desc      : get char on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_20000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetCharParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_20000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_20100
 * @tc.name      : Want.GetCharParam
 * @tc.desc      : get existed char param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_20100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetCharParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_20100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_20200
 * @tc.name      : Want.GetCharParam
 * @tc.desc      : get char param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_20200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetCharParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_20200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_20300
 * @tc.name      : Want.GetCharArrayParam
 * @tc.desc      : get char array on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_20300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetCharArrayParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_20300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_20400
 * @tc.name      : Want.GetCharArrayParam
 * @tc.desc      : get existed char array param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_20400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetCharArrayParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_20400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_20500
 * @tc.name      : Want.GetCharArrayParam
 * @tc.desc      : get char array param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_20500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetCharArrayParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_20500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_20600
 * @tc.name      : Want.GetIntParam
 * @tc.desc      : get int on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_20600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetIntParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_20600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_20700
 * @tc.name      : Want.GetIntParam
 * @tc.desc      : get existed int param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_20700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetIntParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_20700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_20800
 * @tc.name      : Want.GetIntParam
 * @tc.desc      : get int param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_20800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetIntParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_20800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_20900
 * @tc.name      : Want.GetIntArrayParam
 * @tc.desc      : get int array on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_20900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetIntArrayParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_20900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_21000
 * @tc.name      : Want.GetIntArrayParam
 * @tc.desc      : get existed int array param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_21000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetIntArrayParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_21000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_21100
 * @tc.name      : Want.GetIntArrayParam
 * @tc.desc      : get int array param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_21100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetIntArrayParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_21100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_21200
 * @tc.name      : Want.GetDoubleParam
 * @tc.desc      : get double on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_21200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetDoubleParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_21200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_21300
 * @tc.name      : Want.GetDoubleParam
 * @tc.desc      : get existed double param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_21300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetDoubleParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_21300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_21400
 * @tc.name      : Want.GetDoubleParam
 * @tc.desc      : get double param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_21400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetDoubleParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_21400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_21500
 * @tc.name      : Want.GetDoubleArrayParam
 * @tc.desc      : get double array on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_21500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetDoubleArrayParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_21500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_21600
 * @tc.name      : Want.GetDoubleArrayParam
 * @tc.desc      : get existed double array param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_21600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetDoubleArrayParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_21600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_21700
 * @tc.name      : Want.GetDoubleArrayParam
 * @tc.desc      : get double array param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_21700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetDoubleArrayParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_21700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_21800
 * @tc.name      : Want.GetFloatParam
 * @tc.desc      : get float on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_21800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetFloatParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_21800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_21900
 * @tc.name      : Want.GetFloatParam
 * @tc.desc      : get existed float param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_21900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetFloatParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_21900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_22000
 * @tc.name      : Want.GetFloatParam
 * @tc.desc      : get float param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_22000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetFloatParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_22000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_22100
 * @tc.name      : Want.GetFloatArrayParam
 * @tc.desc      : get float array on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_22100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetFloatArrayParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_22100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_22200
 * @tc.name      : Want.GetFloatArrayParam
 * @tc.desc      : get existed float array param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_22200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetFloatArrayParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_22200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_22300
 * @tc.name      : Want.GetFloatArrayParam
 * @tc.desc      : get float array param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_22300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetFloatArrayParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_22300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_22400
 * @tc.name      : Want.GetLongParam
 * @tc.desc      : get long on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_22400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetLongParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_22400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_22500
 * @tc.name      : Want.GetLongParam
 * @tc.desc      : get existed long param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_22500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetLongParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_22500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_22600
 * @tc.name      : Want.GetLongParam
 * @tc.desc      : get long param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_22600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetLongParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_22600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_22700
 * @tc.name      : Want.GetLongArrayParam
 * @tc.desc      : get long array on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_22700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetLongArrayParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_22700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_22800
 * @tc.name      : Want.GetLongArrayParam
 * @tc.desc      : get existed long array param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_22800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetLongArrayParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_22800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_22900
 * @tc.name      : Want.GetLongArrayParam
 * @tc.desc      : get long array param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_22900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetLongArrayParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_22900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_23000
 * @tc.name      : Want.GetShortParam
 * @tc.desc      : get short on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_23000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetShortParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_23000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_23100
 * @tc.name      : Want.GetShortParam
 * @tc.desc      : get existed short param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_23100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetShortParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_23100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_23200
 * @tc.name      : Want.GetShortParam
 * @tc.desc      : get short param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_23200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetShortParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_23200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_23300
 * @tc.name      : Want.GetShortArrayParam
 * @tc.desc      : get short array on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_23300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetShortArrayParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_23300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_23400
 * @tc.name      : Want.GetShortArrayParam
 * @tc.desc      : get existed short array param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_23400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetShortArrayParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_23400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_23500
 * @tc.name      : Want.GetShortArrayParam
 * @tc.desc      : get short array param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_23500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetShortArrayParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_23500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_23600
 * @tc.name      : Want.GetStringParam
 * @tc.desc      : get string on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_23600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetStringParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_23600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_23700
 * @tc.name      : Want.GetStringParam
 * @tc.desc      : get existed string param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_23700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetStringParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_23700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_23800
 * @tc.name      : Want.GetStringParam
 * @tc.desc      : get string param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_23800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetStringParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_23800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_23900
 * @tc.name      : Want.GetStringArrayParam
 * @tc.desc      : get string array on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_23900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetStringArrayParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_23900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_24000
 * @tc.name      : Want.GetStringArrayParam
 * @tc.desc      : get existed string array param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_24000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetStringArrayParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_24000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_24100
 * @tc.name      : Want.GetStringArrayParam
 * @tc.desc      : get string array param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_24100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetStringArrayParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_24100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_24200
 * @tc.name      : Want.SetParam_byte
 * @tc.desc      : max byte
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_24200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_byte) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_24200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_24300
 * @tc.name      : Want.SetParam_byte
 * @tc.desc      : min byte
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_24300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_byte) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_24300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_24400
 * @tc.name      : Want.SetParam_byte
 * @tc.desc      : different byte value set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_24400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_byte) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_24400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_24500
 * @tc.name      : Want.SetParam_byte
 * @tc.desc      : different byte value set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_24500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_byte) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_24500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_24600
 * @tc.name      : Want.SetParam_byte
 * @tc.desc      : pressure of byte
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_24600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_byte) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_24600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_24700
 * @tc.name      : Want.SetParam_byte_array
 * @tc.desc      : empty byte array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_24700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_byte_array) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_24700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_24800
 * @tc.name      : Want.SetParam_byte_array
 * @tc.desc      : big byte array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_24800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_byte_array) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_24800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_24900
 * @tc.name      : Want.SetParam_byte_array
 * @tc.desc      : different byte array set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_24900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_byte_array) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_24900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_25000
 * @tc.name      : Want.SetParam_byte_array
 * @tc.desc      : different byte array set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_25000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_byte_array) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_25000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_25100
 * @tc.name      : Want.SetParam_byte_array
 * @tc.desc      : pressure of byte array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_25100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_byte_array) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_25100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_25200
 * @tc.name      : Want.SetParam_bool
 * @tc.desc      : bool value false
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_25200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_bool) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_25200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_25300
 * @tc.name      : Want.SetParam_bool
 * @tc.desc      : bool value true
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_25300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_bool) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_25300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_25400
 * @tc.name      : Want.SetParam_bool
 * @tc.desc      : different bool value set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_25400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_bool) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_25400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_25500
 * @tc.name      : Want.SetParam_bool
 * @tc.desc      : different bool value set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_25500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_bool) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_25500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_25600
 * @tc.name      : Want.SetParam_bool
 * @tc.desc      : pressure of bool
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_25600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_bool) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_25600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_25700
 * @tc.name      : Want.SetParam_bool_array
 * @tc.desc      : empty bool array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_25700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_bool_array) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_25700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_25800
 * @tc.name      : Want.SetParam_bool_array
 * @tc.desc      : big bool array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_25800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_bool_array) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_25800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_25900
 * @tc.name      : Want.SetParam_bool_array
 * @tc.desc      : different bool array set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_25900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_bool_array) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_25900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_26000
 * @tc.name      : Want.SetParam_bool_array
 * @tc.desc      : different bool array set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_26000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_bool_array) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_26000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_26100
 * @tc.name      : Want.SetParam_bool_array
 * @tc.desc      : pressure of bool array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_26100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_bool_array) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_26100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_26200
 * @tc.name      : Want.SetParam_char
 * @tc.desc      : empty char
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_26200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_char) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_26200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_26300
 * @tc.name      : Want.SetParam_char
 * @tc.desc      : char default value
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_26300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_char) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_26300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_26400
 * @tc.name      : Want.SetParam_char
 * @tc.desc      : different char value set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_26400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_char) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_26400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_26500
 * @tc.name      : Want.SetParam_char
 * @tc.desc      : different char value set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_26500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_char) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_26500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_26600
 * @tc.name      : Want.SetParam_char
 * @tc.desc      : pressure char
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_26600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_char) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_26600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_26700
 * @tc.name      : Want.SetParam_char_array
 * @tc.desc      : empty char array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_26700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_char_array) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_26700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_26800
 * @tc.name      : Want.SetParam_char_array
 * @tc.desc      : big char array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_26800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_char_array) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_26800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_26900
 * @tc.name      : Want.SetParam_char_array
 * @tc.desc      : different char array set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_26900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_char_array) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_26900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_27000
 * @tc.name      : Want.SetParam_char_array
 * @tc.desc      : different char array set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_27000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_char_array) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_27000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_27100
 * @tc.name      : Want.SetParam_char_array
 * @tc.desc      : pressure char array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_27100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_char_array) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_27100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_27200
 * @tc.name      : Want.SetParam_int
 * @tc.desc      : max int
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_27200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_int) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_27200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_27300
 * @tc.name      : Want.SetParam_int
 * @tc.desc      : min int
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_27300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_int) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_27300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_27400
 * @tc.name      : Want.SetParam_int
 * @tc.desc      : different int value set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_27400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_int) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_27400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_27500
 * @tc.name      : Want.SetParam_int
 * @tc.desc      : different int value set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_27500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_int) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_27500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_27600
 * @tc.name      : Want.SetParam_int
 * @tc.desc      : pressure int
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_27600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_int) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_27600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_27700
 * @tc.name      : Want.SetParam_int_array
 * @tc.desc      : empty int array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_27700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_int_array) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_27700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_27800
 * @tc.name      : Want.SetParam_int_array
 * @tc.desc      : big int array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_27800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_int_array) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_27800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_27900
 * @tc.name      : Want.SetParam_int_array
 * @tc.desc      : different int array set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_27900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_int_array) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_27900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_28000
 * @tc.name      : Want.SetParam_int_array
 * @tc.desc      : different int array set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_28000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_int_array) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_28000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_28100
 * @tc.name      : Want.SetParam_int_array
 * @tc.desc      : pressure byte array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_28100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_int_array) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_28100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_28200
 * @tc.name      : Want.SetParam_double
 * @tc.desc      : max double
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_28200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_double) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_28200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_28300
 * @tc.name      : Want.SetParam_double
 * @tc.desc      : min double
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_28300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_double) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_28300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_28400
 * @tc.name      : Want.SetParam_double
 * @tc.desc      : different double value set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_28400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_double) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_28400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_28500
 * @tc.name      : Want.SetParam_double
 * @tc.desc      : different double value set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_28500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_double) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_28500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_28600
 * @tc.name      : Want.SetParam_double
 * @tc.desc      : pressure double
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_28600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_double) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_28600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_28700
 * @tc.name      : Want.SetParam_double_array
 * @tc.desc      : empty double array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_28700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_double_array) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_28700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_28800
 * @tc.name      : Want.SetParam_double_array
 * @tc.desc      : big double array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_28800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_double_array) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_28800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_28900
 * @tc.name      : Want.SetParam_double_array
 * @tc.desc      : different double array set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_28900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_double_array) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_28900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_29000
 * @tc.name      : Want.SetParam_double_array
 * @tc.desc      : different double array set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_29000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_double_array) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_29000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_29100
 * @tc.name      : Want.SetParam_double_array
 * @tc.desc      : pressure double array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_29100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_double_array) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_29100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_29200
 * @tc.name      : Want.SetParam_float
 * @tc.desc      : max float
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_29200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_float) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_29200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_29300
 * @tc.name      : Want.SetParam_float
 * @tc.desc      : min float
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_29300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_float) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_29300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_29400
 * @tc.name      : Want.SetParam_float
 * @tc.desc      : different float value set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_29400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_float) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_29400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_29500
 * @tc.name      : Want.SetParam_float
 * @tc.desc      : different float value set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_29500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_float) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_29500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_29600
 * @tc.name      : Want.SetParam_float
 * @tc.desc      : pressure float
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_29600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_float) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_29600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_29700
 * @tc.name      : Want.SetParam_float_array
 * @tc.desc      : empty float array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_29700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_float_array) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_29700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_29800
 * @tc.name      : Want.SetParam_float_array
 * @tc.desc      : big float array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_29800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_float_array) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_29800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_29900
 * @tc.name      : Want.SetParam_float_array
 * @tc.desc      : different float array set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_29900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_float_array) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_29900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_30000
 * @tc.name      : Want.SetParam_float_array
 * @tc.desc      : different float array set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_30000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_float_array) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_30000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_30100
 * @tc.name      : Want.SetParam_float_array
 * @tc.desc      : pressure float array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_30100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_float_array) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_30100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_30200
 * @tc.name      : Want.SetParam_long
 * @tc.desc      : max long
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_30200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_long) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_30200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_30300
 * @tc.name      : Want.SetParam_long
 * @tc.desc      : min long
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_30300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_long) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_30300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_30400
 * @tc.name      : Want.SetParam_long
 * @tc.desc      : different long value set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_30400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_long) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_30400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_30500
 * @tc.name      : Want.SetParam_long
 * @tc.desc      : different long value set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_30500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_long) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_30500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_30600
 * @tc.name      : Want.SetParam_long
 * @tc.desc      : pressure long
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_30600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_long) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_30600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_30700
 * @tc.name      : Want.SetParam_long_array
 * @tc.desc      : empty long array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_30700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_long_array) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_30700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_30800
 * @tc.name      : Want.SetParam_long_array
 * @tc.desc      : big long array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_30800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_long_array) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_30800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_30900
 * @tc.name      : Want.SetParam_long_array
 * @tc.desc      : different long array set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_30900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_long_array) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_30900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_31000
 * @tc.name      : Want.SetParam_long_array
 * @tc.desc      : different long array set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_31000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_long_array) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_31000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_31100
 * @tc.name      : Want.SetParam_long_array
 * @tc.desc      : pressure long array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_31100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_long_array) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_31100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_31200
 * @tc.name      : Want.SetParam_short
 * @tc.desc      : max short
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_31200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_short) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_31200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_31300
 * @tc.name      : Want.SetParam_short
 * @tc.desc      : min short
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_31300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_short) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_31300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_31400
 * @tc.name      : Want.SetParam_short
 * @tc.desc      : different short value set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_31400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_short) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_31400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_31500
 * @tc.name      : Want.SetParam_short
 * @tc.desc      : different short value set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_31500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_short) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_31500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_31600
 * @tc.name      : Want.SetParam_short
 * @tc.desc      : pressure short
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_31600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_short) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_31600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_31700
 * @tc.name      : Want.SetParam_short_array
 * @tc.desc      : empty short array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_31700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_short_array) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_31700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_31800
 * @tc.name      : Want.SetParam_short_array
 * @tc.desc      : big short array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_31800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_short_array) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_31800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_31900
 * @tc.name      : Want.SetParam_short_array
 * @tc.desc      : different short array set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_31900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_short_array) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_31900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_32000
 * @tc.name      : Want.SetParam_short_array
 * @tc.desc      : different short array set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_32000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_short_array) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_32000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_32100
 * @tc.name      : Want.SetParam_short_array
 * @tc.desc      : pressure short array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_32100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_short_array) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_32100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_32200
 * @tc.name      : Want.SetParam_string
 * @tc.desc      : empty string
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_32200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_string) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_32200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_32300
 * @tc.name      : Want.SetParam_string
 * @tc.desc      : long string
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_32300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_string) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_32300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_32400
 * @tc.name      : Want.SetParam_string
 * @tc.desc      : different string value set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_32400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_string) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_32400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_32500
 * @tc.name      : Want.SetParam_string
 * @tc.desc      : different string value set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_32500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_string) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_32500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_32600
 * @tc.name      : Want.SetParam_string
 * @tc.desc      : pressure string
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_32600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_string) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_32600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_32700
 * @tc.name      : Want.SetParam_string_array
 * @tc.desc      : empty string array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_32700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_string_array) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_32700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_32800
 * @tc.name      : Want.SetParam_string_array
 * @tc.desc      : big string array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_32800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_string_array) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_32800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_32900
 * @tc.name      : Want.SetParam_string_array
 * @tc.desc      : different string array set on one key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_32900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_string_array) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_32900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_33000
 * @tc.name      : Want.SetParam_string_array
 * @tc.desc      : different string array set on different key
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_33000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_string_array) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_33000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_33100
 * @tc.name      : Want.SetParam_string_array
 * @tc.desc      : pressure string array
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_33100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::SetParam_string_array) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_33100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_33200
 * @tc.name      : Want.HasParameter
 * @tc.desc      : empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_33200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::HasParameter) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_33200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_33300
 * @tc.name      : Want.HasParameter
 * @tc.desc      : parameter not exists
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_33300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::HasParameter) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_33300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_33400
 * @tc.name      : Want.HasParameter
 * @tc.desc      : parameter exits
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_33400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::HasParameter) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_33400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_33500
 * @tc.name      : Want.HasParameter
 * @tc.desc      : after remove parameter
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_33500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::HasParameter) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_33500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_33600
 * @tc.name      : Want.HasParameter
 * @tc.desc      : whether exists parameter repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_33600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::HasParameter) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_33600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_33700
 * @tc.name      : Want.ReplaceParams_WantParams
 * @tc.desc      : replace params on empty Want by empty WantParams
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_33700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ReplaceParams_WantParams) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_33700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_33800
 * @tc.name      : Want.ReplaceParams_WantParams
 * @tc.desc      : set param and then replace it by empty WantParams
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_33800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ReplaceParams_WantParams) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_33800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_33900
 * @tc.name      : Want.ReplaceParams_WantParams
 * @tc.desc      : replace params on empty Want by not empty WantParams
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_33900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ReplaceParams_WantParams) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_33900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_34000
 * @tc.name      : Want.ReplaceParams_WantParams
 * @tc.desc      : replace params twice by different WantParams
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_34000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ReplaceParams_WantParams) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_34000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_34100
 * @tc.name      : Want.ReplaceParams_WantParams
 * @tc.desc      : replace params repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_34100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ReplaceParams_WantParams) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_34100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_34200
 * @tc.name      : Want.ReplaceParams_Want
 * @tc.desc      : replace params on empty Want by another empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_34200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ReplaceParams_Want) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_34200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_34300
 * @tc.name      : Want.ReplaceParams_Want
 * @tc.desc      : replace params on empty Want by another not empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_34300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ReplaceParams_Want) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_34300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_34400
 * @tc.name      : Want.ReplaceParams_Want
 * @tc.desc      : replace params by another empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_34400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ReplaceParams_Want) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_34400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_34500
 * @tc.name      : Want.ReplaceParams_Want
 * @tc.desc      : replace params by another Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_34500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ReplaceParams_Want) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_34500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_34600
 * @tc.name      : Want.ReplaceParams_Want
 * @tc.desc      : replace params repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_34600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ReplaceParams_Want) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_34600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_34700
 * @tc.name      : Want.RemoveParam
 * @tc.desc      : remove param from empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_34700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_34700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_34800
 * @tc.name      : Want.RemoveParam
 * @tc.desc      : remove existed param
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_34800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_34800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_34900
 * @tc.name      : Want.RemoveParam
 * @tc.desc      : set and remove param repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_34900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::RemoveParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_34900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_35000
 * @tc.name      : Want.GetOperation
 * @tc.desc      : get operation from empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_35000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetOperation) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_35000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_35100
 * @tc.name      : Want.GetOperation
 * @tc.desc      : set and get operation
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_35100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetOperation) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_35100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_35200
 * @tc.name      : Want.GetOperation
 * @tc.desc      : get operation repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_35200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::GetOperation) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_35200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_35300
 * @tc.name      : Want.OperationEquals
 * @tc.desc      : compare empty Want operation
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_35300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::OperationEquals) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_35300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_35400
 * @tc.name      : Want.OperationEquals
 * @tc.desc      : compare Want operation not equal
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_35400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::OperationEquals) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_35400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_35500
 * @tc.name      : Want.OperationEquals
 * @tc.desc      : compare Want operation equal
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_35500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::OperationEquals) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_35500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_35600
 * @tc.name      : Want.OperationEquals
 * @tc.desc      : compare Want operation repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_35600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::OperationEquals) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_35600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_35700
 * @tc.name      : Want.CloneOperation
 * @tc.desc      : clone operation on empty Want
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_35700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::CloneOperation) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_35700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_35800
 * @tc.name      : Want.CloneOperation
 * @tc.desc      : clone not empty operation
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_35800, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::CloneOperation) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_35800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_35900
 * @tc.name      : Want.CloneOperation
 * @tc.desc      : clone operation repeatedly
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_35900, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::CloneOperation) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_35900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_36000
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse boolean parameters
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_36000, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_15";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_36000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_36100
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse char parameters
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_36100, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_16";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_36100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_36200
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse byte parameters
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_36200, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_17";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_36200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_36300
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse short parameters
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_36300, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_18";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_36300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_36400
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse short parameters
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_36400, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_19";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_36400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_36500
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse short parameters
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_36500, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_20";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_36500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_36600
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse short parameters
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_36600, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_21";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_36600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Want_36700
 * @tc.name      : Want.ParseUri
 * @tc.desc      : parse short parameters
 */
HWTEST_F(ActsAmsKitWantTest, AMS_Page_Want_36700, Function | MediumTest | Level1)
{
    StartSecondAbility();
    std::string data = "Want_" + std::to_string((int)WantApi::ParseUri) + "_22";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND, ++amsKitSystemTestSecondAbilityCode, data);
        EXPECT_EQ(
            0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_SECOND, amsKitSystemTestSecondAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Want_36700 : " << i;
            break;
        }
    }
}
