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

static const std::string bundleName1 = "com.ohos.amsst.AppKitAbilityManager";
static const std::string bundleName2 = "com.ohos.amsst.AppKit";
static const std::string bundleName3 = "com.ohos.amsst.appN";

static const std::string thirdAbilityName = "ThirdAbility";
static const std::string sixthAbilityName = "SixthAbility";
static const std::string abilityManagerName = "KitTestAbilityManager";
static const std::string abilityManagerSecondName = "KitTestAbilityManagerSecond";
static const std::string terminatePageAbility = "requ_page_ability_terminate";
static const std::string eventNameAbilityN1 = "resp_st_page_ability_callback";
static const std::string launcherBundleName = "com.ix.launcher";
static const std::string systemUiBundle = "com.ohos.systemui";

std::vector<std::string> bundleNameList = {
    bundleName1,
    bundleName2,
    bundleName3,
};
std::vector<std::string> hapNameList = {
    "amsKitSystemTest",
    "amsSystemTestN",
    "amsKitSTAbilityManager",
};
static const std::string abilityNameBase = "AmsStAbilityN1";
int amsKitSTCode = 0;
constexpr int WAIT_ABILITY_STATUS_OK = 4 * 1000;
constexpr int WAIT_ABILITY_TERMINATE_OK = 2 * 1000;
constexpr int WAIT_LAUNCHER_OK = 25 * 1000;
}  // namespace

class ActsAmsKitTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static bool SubscribeEvent();
    static void ClearSystem();
    void TestSkills(Want want);
    void StartAbilityKitTest(const std::string &abilityName, const std::string &bundleName);
    void TerminateAbility(const std::string &eventName, const std::string &abilityName);
    class AppEventSubscriber : public CommonEventSubscriber {
    public:
        explicit AppEventSubscriber(const CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp){};
        virtual void OnReceiveEvent(const CommonEventData &data) override;
        ~AppEventSubscriber() = default;
    };

    static sptr<IAppMgr> appMs;
    static sptr<IAbilityManager> abilityMs;
    static Event event;
    static Event abilityEvent;
    static StressTestLevel stLevel_;
    static std::shared_ptr<AppEventSubscriber> subscriber_;
};

Event ActsAmsKitTest::event = Event();
Event ActsAmsKitTest::abilityEvent = Event();
sptr<IAppMgr> ActsAmsKitTest::appMs = nullptr;
sptr<IAbilityManager> ActsAmsKitTest::abilityMs = nullptr;
StressTestLevel ActsAmsKitTest::stLevel_{};
std::shared_ptr<ActsAmsKitTest::AppEventSubscriber> ActsAmsKitTest::subscriber_ = nullptr;

void ActsAmsKitTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "OnReceiveEvent: event=" << data.GetWant().GetAction();
    GTEST_LOG_(INFO) << "OnReceiveEvent: data=" << data.GetData();
    GTEST_LOG_(INFO) << "OnReceiveEvent: code=" << data.GetCode();
    STAbilityUtil::Completed(event, data.GetWant().GetAction(), data.GetCode(), data.GetData());
    STAbilityUtil::Completed(abilityEvent, data.GetData(), data.GetCode());
}

void ActsAmsKitTest::SetUpTestCase(void)
{
    ClearSystem();
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

void ActsAmsKitTest::TearDownTestCase(void)
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void ActsAmsKitTest::SetUp(void)
{
    STAbilityUtil::InstallHaps(hapNameList);
}

void ActsAmsKitTest::TearDown(void)
{
    STAbilityUtil::UninstallBundle(bundleNameList);
    STAbilityUtil::CleanMsg(event);
    STAbilityUtil::CleanMsg(abilityEvent);
}

bool ActsAmsKitTest::SubscribeEvent()
{
    const std::vector<std::string> eventList = {
        g_respPageThirdAbilityST,
        g_respPageSixthAbilityST,
        g_respPageManagerAbilityST,
        g_respPageManagerSecondAbilityST,
        g_respPageSixthAbilityLifecycleCallbacks,
        eventNameAbilityN1,
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

void ActsAmsKitTest::ClearSystem()
{
    STAbilityUtil::KillService("appspawn");
    STAbilityUtil::KillService("installs");
    STAbilityUtil::KillService(launcherBundleName);
    STAbilityUtil::KillService(systemUiBundle);
    STAbilityUtil::KillService("foundation");
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_LAUNCHER_OK));
}

void ActsAmsKitTest::StartAbilityKitTest(const std::string &abilityName, const std::string &bundleName)
{
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityName + g_abilityStateOnActive, 0), 0);
}

void ActsAmsKitTest::TerminateAbility(const std::string &eventName, const std::string &abilityName)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_STATUS_OK));
    STAbilityUtil::PublishEvent(eventName, 0, "TerminateAbility");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityName + g_abilityStateOnStop, 0), 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_TERMINATE_OK));
}

/**
 * @tc.number    : AMS_Page_AbilityManager_0100
 * @tc.name      : AbilityManager::GetAllRunningProcesses
 * @tc.desc      : 1.Get runningprocessinfo
 *                 2.judge whether the current abilityname is in runningprocessinfo
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_0100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::GetAllRunningProcesses);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_0100 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_0200
 * @tc.name      : AbilityManager::GetAllRunningProcesses
 * @tc.desc      : 1.Get runningprocessinfo
 *                 2.judge whether the current launchability is in runningprocessinfo
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_0200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::GetAllRunningProcesses);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_0200 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_0300
 * @tc.name      : AbilityManager::GetAllRunningProcesses
 * @tc.desc      : Get runningprocessinfo,Judge the process status of launch as background
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_0300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::GetAllRunningProcesses);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_0300 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_0400
 * @tc.name      : AbilityManager::GetAllRunningProcesses
 * @tc.desc      : 1.Get runningprocessinfo
 *                 2.Judge the process status of current ability as foreground
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_0400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::GetAllRunningProcesses);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_0400 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_0500
 * @tc.name      : AbilityManager::GetAllRunningProcesses
 * @tc.desc      : 1.start a new ability
 *                 2.Get runningprocessinfo
 *                 3.judge whether the new abilityname is in runningprocessinfo
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_0500, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::GetAllRunningProcesses);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_4";

        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_0500 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_0600
 * @tc.name      : AbilityManager::GetAllRunningProcesses
 * @tc.desc      : 1.start a new ability
 *                 2.Get runningprocessinfo
 *                 3.judge whether the old abilityname is in runningprocessinfo
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_0600, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::GetAllRunningProcesses);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_5";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_0600 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_0700
 * @tc.name      : AbilityManager::GetAllRunningProcesses
 * @tc.desc      : 1.start a new ability
 *                 2.Get runningprocessinfo
 *                 3.judge whether the launch abilityname is in runningprocessinfo
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_0700, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::GetAllRunningProcesses);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_6";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_0700 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_0800
 * @tc.name      : AbilityManager::GetAllRunningProcesses
 * @tc.desc      : 1.start a new ability
 *                 2.Get runningprocessinfo
 *                 3.Judge the process status of launch as background
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_0800, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::GetAllRunningProcesses);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_7";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_0800 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_0900
 * @tc.name      : AbilityManager::GetAllRunningProcesses
 * @tc.desc      : 1.start a new ability
 *                 2.Get runningprocessinfo
 *                 3.Judge the process status of new ability as foreground
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_0900, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::GetAllRunningProcesses);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_8";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_0900 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_1000
 * @tc.name      : AbilityManager::GetAllRunningProcesses
 * @tc.desc      : 1.start a new ability
 *                 2.Get runningprocessinfo
 *                 3.Judge the process status of old ability as background
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_1000, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::GetAllRunningProcesses);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_9";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_1000 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_1100
 * @tc.name      : AbilityManager::GetAllStackInfo
 * @tc.desc      : 1.Get missionstack info with missionstack ID 1
 *                 2.Judge size of missionrecords in get missionstackinfo
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_1100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::GetAllStackInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_1100 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_1200
 * @tc.name      : AbilityManager::GetAllStackInfo
 * @tc.desc      : 1.Get missionstack info with missionstack ID 1
 *                 2.Judge whether the current ability is at the top of the stack
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_1200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::GetAllStackInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_1200 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_1300
 * @tc.name      : AbilityManager::GetAllStackInfo
 * @tc.desc      : 1.Get missionstack info with missionstack ID 0
 *                 2.Judge size of missionrecords in get missionstackinfo
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_1300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::GetAllStackInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_1300 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_1400
 * @tc.name      : AbilityManager::GetAllStackInfo
 * @tc.desc      : 1.Get missionstack info with missionstack ID 0
 *                 2.Judge whether the launch ability is at the stack
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_1400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::GetAllStackInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_1400 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_1500
 * @tc.name      : AbilityManager::GetAllStackInfo
 * @tc.desc      : 1.Start a new ability
 *                 2.Get missionstack info with missionstack ID 0
 *                 3.Judge size of missionrecords in get missionstackinfo
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_1500, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::GetAllStackInfo);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_4";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_1500 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_1600
 * @tc.name      : AbilityManager::GetAllStackInfo
 * @tc.desc      : 1.Start a new ability
 *                 2.Get missionstack info with missionstack ID 0
 *                 3.Judge whether the launch ability is at the stack
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_1600, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::GetAllStackInfo);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_5";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_1600 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_1700
 * @tc.name      : AbilityManager::GetAllStackInfo
 * @tc.desc      : 1.Start a new ability
 *                 2.Get missionstack info with missionstack ID 1
 *                 3.Judge size of missionrecords in get missionstackinfo
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_1700, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::GetAllStackInfo);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_6";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_1700 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_1800
 * @tc.name      : AbilityManager::GetAllStackInfo
 * @tc.desc      : 1.Start a new ability
 *                 2.Get missionstack info with missionstack ID 1
 *                 3.Judge size of abilityRecordInfos in get missionstackinfo.abilityRecordInfos
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_1800, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::GetAllStackInfo);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_7";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_1800 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_1900
 * @tc.name      : AbilityManager::GetAllStackInfo
 * @tc.desc      : 1.Start a new ability
 *                 2.Get missionstack info with missionstack ID 1
 *                 3.Judge whether the current ability and bottom ability is at the stack
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_1900, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::GetAllStackInfo);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_8";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_1900 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_2000
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : Test the numMax(-1) parameter of the QueryRecentAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_2000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_2000 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_2100
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : Test the flags(-1) parameter of the QueryRecentAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_2100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_2100 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_2200
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : Test the flags(0) parameter of the QueryRecentAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_2200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_2200 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_2300
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : Test the flags(3) parameter of the QueryRecentAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_2300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_2300 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_2400
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : Test the size() of the return value of the queryrecentabilitymissioninfo function
 *                 (parameter:flags(RECENT_WITH_EXCLUDED) )
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_2400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_2400 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_2500
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : Test the size() of the return value of the queryrecentabilitymissioninfo function
 *                 (parameter:flags(RECENT_IGNORE_UNAVAILABLE) )
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_2500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_2500 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_2600
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : Test the member variables contained in the return value of the queryrecentabilitymassioninfo
 *                 function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_2600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_2600 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_2700
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : 1.Strat a new ability
 *                 2.Get the return value of the queryrecentabilitymassioninfo function
 *                 3.Test the baseAbility in test member variable
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_2700, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_7";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_2700 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_2800
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : 1.Strat a new ability
 *                 2.Get the return value of the queryrecentabilitymassioninfo function
 *                 3.Test the size in test member variable
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_2800, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_8";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_2800 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_2900
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : 1.Strat a new ability
 *                 2.Get the return value of the queryrecentabilitymassioninfo function
 *                 3.Test the baseAbility(abilityName and bundleName) in test member variable
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_2900, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_9";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_2900 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_3000
 * @tc.name      : AbilityManager::QueryRunningAbilityMissionInfo
 * @tc.desc      : Test the numMax(-1) parameter of the QueryRunningAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_3000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRunningAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_3000 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_3100
 * @tc.name      : AbilityManager::QueryRunningAbilityMissionInfo
 * @tc.desc      : Test the numMax(0) parameter of the QueryRunningAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_3100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRunningAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_3100 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_3200
 * @tc.name      : AbilityManager::QueryRunningAbilityMissionInfo
 * @tc.desc      : Test the size() of the return value of the QueryRunningAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_3200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRunningAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_3200 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_3300
 * @tc.name      : AbilityManager::QueryRunningAbilityMissionInfo
 * @tc.desc      : 1.Get the return value of the QueryRunningAbilityMissionInfo function
 *                 2.Test the size in test member variable
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_3300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRunningAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_3300 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_3400
 * @tc.name      : AbilityManager::QueryRunningAbilityMissionInfo
 * @tc.desc      : 1.Start a new ability
 *                 2.Get the return value of the QueryRunningAbilityMissionInfo function
 *                 3.Test the baseAbility in test member variable
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_3400, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::QueryRunningAbilityMissionInfo);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_4";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_3400 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_3500
 * @tc.name      : AbilityManager::QueryRunningAbilityMissionInfo
 * @tc.desc      : 1.Start a new ability
 *                 2.Get the return value of the QueryRunningAbilityMissionInfo function
 *                 3.Test the size and id in test member variable
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_3500, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::QueryRunningAbilityMissionInfo);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_5";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_3500 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_3600
 * @tc.name      : AbilityManager::QueryRunningAbilityMissionInfo
 * @tc.desc      : 1.Start a new ability
 *                 2.Get the return value of the QueryRunningAbilityMissionInfo function
 *                 3.Test the baseAbility(bundleName and abilityName) in test member variable
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_3600, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::QueryRunningAbilityMissionInfo);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_6";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_3600 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_3700
 * @tc.name      : IAbilityManager::MoveMissionToTop
 * @tc.desc      : 1.Use movemissiontotop to move the launch to the top
 *                 2.Confirm that OnBackground() of the current ability is called
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_3700, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        StartAbilityKitTest(abilityManagerName, bundleName2);
        int apiIndex = static_cast<int>(AbilityManagerApi::MoveMissionToTop);
        std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_0";
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_3700 : " << i;
            break;
        }
        TerminateAbility(g_requPageManagerSecondAbilityST, abilityManagerSecondName);
        EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityManagerName + g_abilityStateOnActive, 0), 0);
        TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
    }
}

/**
 * @tc.number    : AMS_Page_AbilityManager_3800
 * @tc.name      : IAbilityManager::MoveMissionToTop
 * @tc.desc      : 1.Use movemissiontotop to move the launch to the top
 *                 2.Confirm that OnBackground() of the current ability is called
 *                 3.Get runningprocessinfo in OnBackground()
 *                 4.Judge whether the current process state of launch is foreground
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_3800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::MoveMissionToTop);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_3800 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_3900
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : Test the numMax(INT32_MIN) parameter of the QueryRecentAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_3900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_10";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_3900 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_4000
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : Test the numMax(INT32_MAX) parameter of the QueryRecentAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_4000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_11";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_4000 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_4100
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : Test the flags(INT32_MIN) parameter of the QueryRecentAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_4100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_12";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_4100 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_4200
 * @tc.name      : AbilityManager::QueryRecentAbilityMissionInfo
 * @tc.desc      : Test the flags(INT32_MAX) parameter of the QueryRecentAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_4200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRecentAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_13";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_4200 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_4300
 * @tc.name      : AbilityManager::QueryRunningAbilityMissionInfo
 * @tc.desc      : Test the numMax(INT32_MIN) parameter of the QueryRunningAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_4300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRunningAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_4300 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_AbilityManager_4400
 * @tc.name      : AbilityManager::QueryRunningAbilityMissionInfo
 * @tc.desc      : Test the numMax(INT32_MAX) parameter of the QueryRunningAbilityMissionInfo function
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_AbilityManager_4400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(abilityManagerName, bundleName2);
    int apiIndex = static_cast<int>(AbilityManagerApi::QueryRunningAbilityMissionInfo);
    std::string eventData = "AbilityManagerApi_" + std::to_string(apiIndex) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageManagerAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageManagerAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageManagerAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_AbilityManager_4400 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageManagerAbilityST, abilityManagerName);
}

/**
 * @tc.number    : AMS_Page_Application_0100
 * @tc.name      : OHOSApplicationApi::RegisterAbilityLifecycleCallbacks
 * @tc.desc      : register the abilitylifecyclecallbacks class and count the call
 *                 times of onabilitystart function in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_0100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::RegisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_0100 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_0200
 * @tc.name      : OHOSApplicationApi::RegisterAbilityLifecycleCallbacks
 * @tc.desc      : register the abilitylifecyclecallbacks class and count the call
 *                 times of onabilityinactivefunction in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_0200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::RegisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_0200 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_0300
 * @tc.name      : OHOSApplicationApi::RegisterAbilityLifecycleCallbacks
 * @tc.desc      : register the abilitylifecyclecallbacks class and count the call
 *                 times of onabilitybackground in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_0300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::RegisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_0300 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_0400
 * @tc.name      : OHOSApplicationApi::RegisterAbilityLifecycleCallbacks
 * @tc.desc      : register the abilitylifecyclecallbacks class and count the call
 *                 times of onabilityforeground in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_0400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::RegisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_0400 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_0500
 * @tc.name      : OHOSApplicationApi::RegisterAbilityLifecycleCallbacks
 * @tc.desc      : register the abilitylifecyclecallbacks class and count the call
 *                 times of onabilityactive in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_0500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::RegisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_0500 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_0600
 * @tc.name      : OHOSApplicationApi::RegisterAbilityLifecycleCallbacks
 * @tc.desc      : register the abilitylifecyclecallbacks class and count the call
 *                 times ofonabilitystop in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_0600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::RegisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_0600 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_0700
 * @tc.name      : OHOSApplicationApi::RegisterAbilityLifecycleCallbacks
 * @tc.desc      : loop register the abilitylifecyclecallbacks class(same class) and count the call
 *                 times of onabilitystart in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_0700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::RegisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_0700 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_0800
 * @tc.name      : OHOSApplicationApi::RegisterAbilityLifecycleCallbacks
 * @tc.desc      : loop register the abilitylifecyclecallbacks class(different class) and count the call
 *                 times of onabilitystart in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_0800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::RegisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_0800 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_0900
 * @tc.name      : OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks
 * @tc.desc      : unregister the abilitylifecyclecallbacks class and count the call
 *                 times of onabilitystart function in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_0900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_0900 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_1000
 * @tc.name      : OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks
 * @tc.desc      : unregister the abilitylifecyclecallbacks class and count the call
 *                 times of onabilityinactivefunction in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_1000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_1000 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_1100
 * @tc.name      : OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks
 * @tc.desc      : unregister the abilitylifecyclecallbacks class and count the call
 *                 times of onabilitybackground in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_1100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_1100 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_1200
 * @tc.name      : OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks
 * @tc.desc      : unregister the abilitylifecyclecallbacks class and count the call
 *                 times of onabilityforeground in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_1200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_1200 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_1300
 * @tc.name      : OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks
 * @tc.desc      : unregister the abilitylifecyclecallbacks class and count the call
 *                 times of onabilityactive in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_1300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_1300 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_1400
 * @tc.name      : OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks
 * @tc.desc      : unregister the abilitylifecyclecallbacks class and count the call
 *                 times ofonabilitystop in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_1400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_1400 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_1500
 * @tc.name      : OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks
 * @tc.desc      : loop unregister the abilitylifecyclecallbacks class(same class) and count the call
 *                 times of onabilitystart in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_1500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_1500 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_1600
 * @tc.name      : OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks
 * @tc.desc      : loop unregister the abilitylifecyclecallbacks class(different class) and count the call
 *                 times of onabilitystart in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_1600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::UnregisterAbilityLifecycleCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_1600 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_1700
 * @tc.name      : OHOSApplicationApi::DispatchAbilitySavedState
 * @tc.desc      : register the abilitylifecyclecallbacks class and count the call
 *                 times of onabilitysavestate function in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_1700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::DispatchAbilitySavedState);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_1700 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_1800
 * @tc.name      : OHOSApplicationApi::DispatchAbilitySavedState
 * @tc.desc      : loop register the abilitylifecyclecallbacks class(same class) and count the call
 *                 times of onabilitysavestate in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_1800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::DispatchAbilitySavedState);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_1800 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_1900
 * @tc.name      : OHOSApplicationApi::RegisterElementsCallbacks
 * @tc.desc      : register elementscallbacks class and count the call times of
 *                 onconfigurationupdated function in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_1900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::RegisterElementsCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_1900 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_2000
 * @tc.name      : OHOSApplicationApi::RegisterElementsCallbacks
 * @tc.desc      : register elementscallbacks class and count the call times of
 *                 onmemorylevel function in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_2000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::RegisterElementsCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_2000 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_2100
 * @tc.name      : OHOSApplicationApi::RegisterElementsCallbacks
 * @tc.desc      : loop register elementscallbacks class and count the call times of onconfigurationupdated function
 in
 * the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_2100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::RegisterElementsCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_2100 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_2200
 * @tc.name      : OHOSApplicationApi::RegisterElementsCallbacks
 * @tc.desc      : loop register elementscallbacks class and count the call times of onmemorylevel function in the
 * callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_2200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::RegisterElementsCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_2200 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_2300
 * @tc.name      : OHOSApplicationApi::UnregisterElementsCallbacks
 * @tc.desc      : unregister elementscallbacks class and count the call
 *                 times of onconfigurationupdated function in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_2300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::UnregisterElementsCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_2300 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_2400
 * @tc.name      : OHOSApplicationApi::UnregisterElementsCallbacks
 * @tc.desc      : unregister elementscallbacks class and count the call times of
 *                 onmemorylevel function in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_2400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::UnregisterElementsCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_2400 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_2500
 * @tc.name      : OHOSApplicationApi::UnregisterElementsCallbacks
 * @tc.desc      : loop unregister elementscallbacks class and count the call times of
 *                 onconfigurationupdated function in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_2500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::UnregisterElementsCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_2500 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_2600
 * @tc.name      : OHOSApplicationApi::UnregisterElementsCallbacks
 * @tc.desc      : loop unregister elementscallbacks class and count the call times of
 *                 onmemorylevel function in the callback class
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_2600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(sixthAbilityName, bundleName2);
    int apiIndex = static_cast<int>(OHOSApplicationApi::UnregisterElementsCallbacks);
    std::string eventData = "OHOSApplicationApi_" + std::to_string(apiIndex) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageSixthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageSixthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageSixthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && stLevel_.AMSLevel > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Application_2600 : " << i;
            break;
        }
    }
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_2700
 * @tc.name      : OHOSApplicationApi::Lifecycle callback function
 * @tc.desc      : 1.Control the life cycle of the ability
 *                 2.Application registration capability life cycle callback
 *                 3.The callback function corresponding to the registered class will be called
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_2700, Function | MediumTest | Level1)
{
    MAP_STR_STR params;
    params["targetBundle"] = bundleName2;
    params["targetAbility"] = thirdAbilityName;
    Want want = STAbilityUtil::MakeWant("device", sixthAbilityName, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, sixthAbilityName + g_onAbilityActive, 0), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, sixthAbilityName + g_onAbilityInactive, 0), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, thirdAbilityName + g_onAbilityStart, 0), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, thirdAbilityName + g_onAbilityActive, 0), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, sixthAbilityName + g_onAbilityBackground, 0), 0);
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, sixthAbilityName + g_onAbilityStop, 0), 0);
    TerminateAbility(g_requPageThirdAbilityST, thirdAbilityName);
}

/**
 * @tc.number    : AMS_Page_Application_2800
 * @tc.name      : OHOSApplicationApi::Lifecycle callback function
 * @tc.desc      : 1.Control the life cycle of the ability
 *                 2.Application registration capability life cycle callback
 *                 3.The callback function corresponding to the registered class will be called
 */
HWTEST_F(ActsAmsKitTest, AMS_Page_Application_2800, Function | MediumTest | Level1)
{
    MAP_STR_STR params;
    params["targetBundle"] = bundleName3;
    params["targetAbility"] = abilityNameBase;
    Want want = STAbilityUtil::MakeWant("device", sixthAbilityName, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, sixthAbilityName + g_onAbilityActive, 0), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, sixthAbilityName + g_onAbilityInactive, 0), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, sixthAbilityName + g_onAbilityBackground, 0), 0);
    STAbilityUtil::StopAbility(terminatePageAbility, 0, abilityNameBase);
    int onStopWantCount = 1;
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityNameBase + g_abilityStateOnStop, onStopWantCount), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, sixthAbilityName + g_onAbilityForeground, 0), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, sixthAbilityName + g_onAbilityActive, 0), 0);
    TerminateAbility(g_requPageSixthAbilityST, sixthAbilityName);
}