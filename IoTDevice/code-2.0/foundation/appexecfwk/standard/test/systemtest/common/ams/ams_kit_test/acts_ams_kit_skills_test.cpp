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
static const std::string fourthAbilityName = "FourthAbility";
std::vector<std::string> bundleNameList = {
    "com.ohos.amsst.AppKit",
};
std::vector<std::string> hapNameList = {
    "amsKitSystemTest",
};

int amsKitSTCode = 0;
}  // namespace

class ActsAmsKitSkillsTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static bool SubscribeEvent();
    void StartAbilityKitTest(const std::string &abilityName, const std::string &bundleName);
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
    static std::unordered_map<std::string, int> mapState;
    static std::vector<std::string> eventList;
    static StressTestLevel stLevel_;
    static std::shared_ptr<AppEventSubscriber> subscriber_;
};

std::vector<std::string> ActsAmsKitSkillsTest::eventList = {
    g_respPageFourthAbilityST,
};
StressTestLevel ActsAmsKitSkillsTest::stLevel_{};
std::shared_ptr<ActsAmsKitSkillsTest::AppEventSubscriber> ActsAmsKitSkillsTest::subscriber_ = nullptr;

Event ActsAmsKitSkillsTest::event = Event();
Event ActsAmsKitSkillsTest::abilityEvent = Event();
sptr<IAppMgr> ActsAmsKitSkillsTest::appMs = nullptr;
sptr<IAbilityManager> ActsAmsKitSkillsTest::abilityMs = nullptr;
std::unordered_map<std::string, int> ActsAmsKitSkillsTest::mapState = {
    {"OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
    {"OnStop", AbilityLifecycleExecutor::LifecycleState::INITIAL},
    {"OnActive", AbilityLifecycleExecutor::LifecycleState::ACTIVE},
    {"OnInactive", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
    {"OnBackground", AbilityLifecycleExecutor::LifecycleState::BACKGROUND},
    {"OnForeground", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
    {"OnAbilityStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
    {"OnAbilityStop", AbilityLifecycleExecutor::LifecycleState::INITIAL},
    {"OnAbilityActive", AbilityLifecycleExecutor::LifecycleState::ACTIVE},
    {"OnAbilityInactive", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
    {"OnAbilityBackground", AbilityLifecycleExecutor::LifecycleState::BACKGROUND},
    {"OnAbilityForeground", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
};

void ActsAmsKitSkillsTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "OnReceiveEvent: event=" << data.GetWant().GetAction();
    GTEST_LOG_(INFO) << "OnReceiveEvent: data=" << data.GetData();
    GTEST_LOG_(INFO) << "OnReceiveEvent: code=" << data.GetCode();
    STAbilityUtil::Completed(event, data.GetWant().GetAction(), data.GetCode(), data.GetData());
    STAbilityUtil::Completed(abilityEvent, data.GetData(), data.GetCode());

    auto eventName = data.GetWant().GetAction();
    auto iter = std::find(eventList.begin(), eventList.end(), eventName);
    if (iter != eventList.end()) {
        STAbilityUtil::Completed(event, data.GetData(), data.GetCode());
    }
}

void ActsAmsKitSkillsTest::SetUpTestCase(void)
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

void ActsAmsKitSkillsTest::TearDownTestCase(void)
{
    STAbilityUtil::UninstallBundle(bundleNameList);
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void ActsAmsKitSkillsTest::SetUp(void)
{}

void ActsAmsKitSkillsTest::TearDown(void)
{
    STAbilityUtil::CleanMsg(event);
    STAbilityUtil::CleanMsg(abilityEvent);
}

bool ActsAmsKitSkillsTest::SubscribeEvent()
{
    const std::vector<std::string> eventList = {
        g_respPageFourthAbilityST,
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

void ActsAmsKitSkillsTest::StartAbilityKitTest(const std::string &abilityName, const std::string &bundleName)
{
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityName + g_abilityStateOnActive, 0), 0);
}

/**
 * @tc.number    : AMS_Page_Skills_00100
 * @tc.name      : Skills::CountActions
 * @tc.desc      : judge the return value of countactions after calling addaction
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_00100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountActions)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_00100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_00200
 * @tc.name      : Skills::CountActions
 * @tc.desc      : judge the return value of countaction
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_00200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountActions)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_00200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_00300
 * @tc.name      : Skills::CountActions
 * @tc.desc      : after calling addaction many times, judge the return value of countaction
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_00300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountActions)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_00300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_00400
 * @tc.name      : Skills::GetAction
 * @tc.desc      : add a normal string to judge the return value of getaction
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_00400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAction)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_00400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_00500
 * @tc.name      : Skills::GetAction
 * @tc.desc      : judge the return value of getaction, when the index value exceeds
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_00500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAction)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_00500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_00600
 * @tc.name      : Skills::GetAction
 * @tc.desc      : judge the return value of getaction when the index value is negative
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_00600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAction)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_00600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_00700
 * @tc.name      : Skills::GetAction
 * @tc.desc      : judge the return value of getaction when action is null
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_00700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAction)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_00700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_00800
 * @tc.name      : Skills::GetAction
 * @tc.desc      : add a special string to judge the return value of getaction
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_00800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAction)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_00800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_00900
 * @tc.name      : Skills::GetAction
 * @tc.desc      : after calling addaction many times, judge the return value of getaction(index 0)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_00900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAction)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_00900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_01000
 * @tc.name      : Skills::GetAction
 * @tc.desc      : after calling addaction many times, judge the return value of getaction(index 150)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_01000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAction)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_01000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_01100
 * @tc.name      : Skills::HasAction
 * @tc.desc      : after calling addaction many times, judge the return value of hasaction
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_01100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasAction)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_01100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_01200
 * @tc.name      : Skills::HasAction
 * @tc.desc      : judge the return value of hasaction,when action is empty
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_01200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasAction)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_01200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_01300
 * @tc.name      : Skills::HasAction
 * @tc.desc      : add a special string to judge the return value of hasaction
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_01300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasAction)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_01300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_01400
 * @tc.name      : Skills::RemoveAction
 * @tc.desc      : add specialstring. after deleting, judge hasaction
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_01400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAction)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_01400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_01500
 * @tc.name      : Skills::RemoveAction
 * @tc.desc      : add specialstring, after deleting, judge getaction
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_01500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAction)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_01500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_01600
 * @tc.name      : Skills::RemoveAction
 * @tc.desc      : loop add normalstring, after deleting, judge hasaction
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_01600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAction)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_01600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_01700
 * @tc.name      : Skills::RemoveAction
 * @tc.desc      : loop add normalstring, delete specialstring and judge hasaction
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_01700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAction)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_01700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_01800
 * @tc.name      : Skills::RemoveAction
 * @tc.desc      : loop add normalstring, after deleting, judge getaction (the index value exceeds)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_01800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAction)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_01800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_01900
 * @tc.name      : Skills::RemoveAction
 * @tc.desc      : loop add normalstring, delete specialstring and judge getaction
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_01900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAction)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_01900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_02000
 * @tc.name      : Skills::RemoveAction
 * @tc.desc      : add specialstring, delete specialstring, and judge countactions
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_02000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAction)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_02000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_02100
 * @tc.name      : Skills::RemoveAction
 * @tc.desc      : loop add normalstring, delete normalstring, and judge countactions
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_02100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAction)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_02100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_02200
 * @tc.name      : Skills::RemoveAction
 * @tc.desc      : loop add normalstring, delete specialstring, and judge countactions
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_02200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAction)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_02200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_02300
 * @tc.name      : Skills::CountEntities
 * @tc.desc      : judge the return value of countentities after calling addentity
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_02300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountEntities)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_02300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_02400
 * @tc.name      : Skills::CountEntities
 * @tc.desc      : judge the return value of countentities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_02400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountEntities)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_02400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_02500
 * @tc.name      : Skills::CountEntities
 * @tc.desc      : after calling addentity many times, judge the return value of countentities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_02500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountEntities)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_02500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_02600
 * @tc.name      : Skills::GetEntity
 * @tc.desc      : add a normal string to judge the return value of getentity
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_02600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetEntity)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_02600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_02700
 * @tc.name      : Skills::GetEntity
 * @tc.desc      : judge the return value of getentity, when the index value exceeds
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_02700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetEntity)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_02700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_02800
 * @tc.name      : Skills::GetEntity
 * @tc.desc      : judge the return value of getentity when the index value is negative
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_02800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetEntity)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_02800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_02900
 * @tc.name      : Skills::GetEntity
 * @tc.desc      : judge the return value of getentity when entity is null
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_02900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetEntity)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_02900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_03000
 * @tc.name      : Skills::GetEntity
 * @tc.desc      : add a special string to judge the return value of getentity
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_03000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetEntity)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_03000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_03100
 * @tc.name      : Skills::GetEntity
 * @tc.desc      : loop addentity, judge the return value of getentity(index 0)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_03100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetEntity)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_03100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_03200
 * @tc.name      : Skills::GetEntity
 * @tc.desc      : loop addentity, judge the return value of getentity(index 150)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_03200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetEntity)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_03200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_03300
 * @tc.name      : Skills::HasEntity
 * @tc.desc      : loop addentity, judge the return value of hasentity
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_03300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasEntity)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_03300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_03400
 * @tc.name      : Skills::HasEntity
 * @tc.desc      : judge the return value of hasentity,when entity is empty
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_03400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasEntity)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_03400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_03500
 * @tc.name      : Skills::HasEntity
 * @tc.desc      : add a special string to judge the return value of hasentity
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_03500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasEntity)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_03500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_03600
 * @tc.name      : Skills::RemoveEntity
 * @tc.desc      : add specialstring. after deleting, judge hasentity
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_03600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveEntity)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_03600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_03700
 * @tc.name      : Skills::RemoveEntity
 * @tc.desc      : add specialstring, after deleting, judge getentity
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_03700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveEntity)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_03700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_03800
 * @tc.name      : Skills::RemoveEntity
 * @tc.desc      : loop add normalstring, after deleting, judge hasentity
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_03800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveEntity)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_03800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_03900
 * @tc.name      : Skills::RemoveEntity
 * @tc.desc      : loop add normalstring, delete specialstring and judge hasentity
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_03900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveEntity)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_03900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_04000
 * @tc.name      : Skills::RemoveEntity
 * @tc.desc      : loop add normalstring, after deleting, judge getentity (the index value exceeds)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_04000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveEntity)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_04000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_04100
 * @tc.name      : Skills::RemoveEntity
 * @tc.desc      : loop add normalstring, delete specialstring and judge getentity
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_04100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveEntity)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_04100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_04200
 * @tc.name      : Skills::RemoveEntity
 * @tc.desc      : add specialstring, delete specialstring, and judge countentities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_04200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveEntity)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_04200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_04300
 * @tc.name      : Skills::RemoveEntity
 * @tc.desc      : loop add normalstring, delete normalstring, and judge countentities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_04300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveEntity)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_04300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_04400
 * @tc.name      : Skills::RemoveEntity
 * @tc.desc      : loop add normalstring, delete specialstring, and judge countentities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_04400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveEntity)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_04400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_04500
 * @tc.name      : Skills::CountAuthorities
 * @tc.desc      : judge the return value of countauthorities after calling addauthority
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_04500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountAuthorities)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_04500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_04600
 * @tc.name      : Skills::CountAuthorities
 * @tc.desc      : judge the return value of countauthorities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_04600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountAuthorities)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_04600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_04700
 * @tc.name      : Skills::CountAuthorities
 * @tc.desc      : after calling addauthority many times, judge the return value of countauthorities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_04700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountAuthorities)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_04700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_04800
 * @tc.name      : Skills::GetAuthority
 * @tc.desc      : add a normal string to judge the return value of getauthority
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_04800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAuthority)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_04800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_04900
 * @tc.name      : Skills::GetAuthority
 * @tc.desc      : judge the return value of getauthority, when the index value exceeds
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_04900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAuthority)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_04900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_05000
 * @tc.name      : Skills::GetAuthority
 * @tc.desc      : judge the return value of getauthority when the index value is negative
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_05000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAuthority)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_05000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_05100
 * @tc.name      : Skills::GetAuthority
 * @tc.desc      : judge the return value of getauthority when authority is null
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_05100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAuthority)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_05100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_05200
 * @tc.name      : Skills::GetAuthority
 * @tc.desc      : add a special string to judge the return value of getauthority
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_05200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAuthority)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_05200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_05300
 * @tc.name      : Skills::GetAuthority
 * @tc.desc      : loop addauthority, judge the return value of getauthority(index 0)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_05300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAuthority)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_05300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_05400
 * @tc.name      : Skills::GetAuthority
 * @tc.desc      : loop addauthority, judge the return value of getauthority(index 150)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_05400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetAuthority)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_05400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_05500
 * @tc.name      : Skills::HasAuthority
 * @tc.desc      : loop addauthority, judge the return value of hasauthority
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_05500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasAuthority)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_05500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_05600
 * @tc.name      : Skills::HasAuthority
 * @tc.desc      : judge the return value of hasauthority,when authority is empty
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_05600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasAuthority)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_05600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_05700
 * @tc.name      : Skills::HasAuthority
 * @tc.desc      : add a special string to judge the return value of hasauthority
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_05700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasAuthority)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_05700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_05800
 * @tc.name      : Skills::RemoveAuthority
 * @tc.desc      : add specialstring. after deleting, judge hasauthority
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_05800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAuthority)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_05800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_05900
 * @tc.name      : Skills::RemoveAuthority
 * @tc.desc      : add specialstring, after deleting, judge getauthority
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_05900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAuthority)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_05900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_06000
 * @tc.name      : Skills::RemoveAuthority
 * @tc.desc      : loop add normalstring, after deleting, judge hasauthority
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_06000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAuthority)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_06000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_06100
 * @tc.name      : Skills::RemoveAuthority
 * @tc.desc      : loop add normalstring, delete specialstring and judge hasauthority
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_06100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAuthority)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_06100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_06200
 * @tc.name      : Skills::RemoveAuthority
 * @tc.desc      : loop add normalstring, after deleting, judge getauthority (the index value exceeds)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_06200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAuthority)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_06200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_06300
 * @tc.name      : Skills::RemoveAuthority
 * @tc.desc      : loop add normalstring, delete specialstring and judge getauthority
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_06300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAuthority)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_06300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_06400
 * @tc.name      : Skills::RemoveAuthority
 * @tc.desc      : add specialstring, delete specialstring, and judge countauthorities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_06400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAuthority)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_06400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_06500
 * @tc.name      : Skills::RemoveAuthority
 * @tc.desc      : loop add normalstring, delete normalstring, and judge countauthorities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_06500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAuthority)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_06500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_06600
 * @tc.name      : Skills::RemoveAuthority
 * @tc.desc      : loop add normalstring, delete specialstring, and judge countauthorities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_06600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveAuthority)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_06600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_06700
 * @tc.name      : Skills::CountSchemes
 * @tc.desc      : judge the return value of countschemes after calling addscheme
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_06700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountSchemes)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_06700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_06800
 * @tc.name      : Skills::CountSchemes
 * @tc.desc      : judge the return value of countschemes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_06800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountSchemes)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_06800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_06900
 * @tc.name      : Skills::CountSchemes
 * @tc.desc      : after calling addscheme many times, judge the return value of countschemes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_06900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountSchemes)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_06900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_07000
 * @tc.name      : Skills::GetScheme
 * @tc.desc      : add a normal string to judge the return value of getscheme
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_07000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetScheme)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_07000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_07100
 * @tc.name      : Skills::GetScheme
 * @tc.desc      : judge the return value of getscheme, when the index value exceeds
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_07100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetScheme)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_07100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_07200
 * @tc.name      : Skills::GetScheme
 * @tc.desc      : judge the return value of getscheme when the index value is negative
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_07200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetScheme)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_07200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_07300
 * @tc.name      : Skills::GetScheme
 * @tc.desc      : judge the return value of getscheme when scheme is null
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_07300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetScheme)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_07300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_07400
 * @tc.name      : Skills::GetScheme
 * @tc.desc      : add a special string to judge the return value of getscheme
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_07400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetScheme)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_07400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_07500
 * @tc.name      : Skills::GetScheme
 * @tc.desc      : loop addscheme, judge the return value of getscheme(index 0)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_07500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetScheme)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_07500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_07600
 * @tc.name      : Skills::GetScheme
 * @tc.desc      : loop addscheme, judge the return value of getscheme(index 150)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_07600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetScheme)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_07600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_07700
 * @tc.name      : Skills::HasScheme
 * @tc.desc      : loop addscheme, judge the return value of hasscheme
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_07700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasScheme)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_07700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_07800
 * @tc.name      : Skills::HasScheme
 * @tc.desc      : judge the return value of hasscheme,when scheme is empty
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_07800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasScheme)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_07800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_07900
 * @tc.name      : Skills::HasScheme
 * @tc.desc      : add a special string to judge the return value of hasscheme
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_07900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasScheme)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_07900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_08000
 * @tc.name      : Skills::RemoveScheme
 * @tc.desc      : add specialstring. after deleting, judge hasscheme
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_08000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveScheme)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_08000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_08100
 * @tc.name      : Skills::RemoveScheme
 * @tc.desc      : add specialstring, after deleting, judge getscheme
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_08100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveScheme)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_08100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_08200
 * @tc.name      : Skills::RemoveScheme
 * @tc.desc      : loop add normalstring, after deleting, judge hasscheme
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_08200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveScheme)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_08200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_08300
 * @tc.name      : Skills::RemoveScheme
 * @tc.desc      : loop add normalstring, delete specialstring and judge hasscheme
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_08300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveScheme)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_08300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_08400
 * @tc.name      : Skills::RemoveScheme
 * @tc.desc      : loop add normalstring, after deleting, judge getscheme (the index value exceeds)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_08400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveScheme)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_08400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_08500
 * @tc.name      : Skills::RemoveScheme
 * @tc.desc      : loop add normalstring, delete specialstring and judge getscheme
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_08500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveScheme)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_08500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_08600
 * @tc.name      : Skills::RemoveScheme
 * @tc.desc      : add specialstring, delete specialstring, and judge countschemes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_08600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveScheme)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_08600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_08700
 * @tc.name      : Skills::RemoveScheme
 * @tc.desc      : loop add normalstring, delete normalstring, and judge countschemes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_08700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveScheme)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_08700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_08800
 * @tc.name      : Skills::RemoveScheme
 * @tc.desc      : loop add normalstring, delete specialstring, and judge countschemes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_08800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveScheme)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_08800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_08900
 * @tc.name      : Skills::CountSchemeSpecificParts
 * @tc.desc      : judge the return value of countschemespecificparts after calling addschemespecificpart
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_08900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountSchemeSpecificParts)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_08900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_09000
 * @tc.name      : Skills::CountSchemeSpecificParts
 * @tc.desc      : judge the return value of countschemespecificparts
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_09000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountSchemeSpecificParts)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_09000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_09100
 * @tc.name      : Skills::CountSchemeSpecificParts
 * @tc.desc      : after calling addschemespecificpart many times,
 *                 judge the return value of countschemespecificparts
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_09100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountSchemeSpecificParts)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_09100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_09200
 * @tc.name      : Skills::GetSchemeSpecificPart
 * @tc.desc      : add a normal string to judge the return value of getschemespecificpart
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_09200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetSchemeSpecificPart)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_09200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_09300
 * @tc.name      : Skills::GetSchemeSpecificPart
 * @tc.desc      : judge the return value of getschemespecificpart, when the index value exceeds
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_09300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetSchemeSpecificPart)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_09300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_09400
 * @tc.name      : Skills::GetSchemeSpecificPart
 * @tc.desc      : judge the return value of getschemespecificpart when the index value is negative
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_09400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetSchemeSpecificPart)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_09400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_09500
 * @tc.name      : Skills::GetSchemeSpecificPart
 * @tc.desc      : judge the return value of getschemespecificpart when specificpart is null
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_09500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetSchemeSpecificPart)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_09500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_09600
 * @tc.name      : Skills::GetSchemeSpecificPart
 * @tc.desc      : add a special string to judge the return value of getschemespecificpart
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_09600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetSchemeSpecificPart)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_09600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_09700
 * @tc.name      : Skills::GetSchemeSpecificPart
 * @tc.desc      : loop addschemespecificpart, judge the return value of getschemespecificpart(index 0)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_09700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetSchemeSpecificPart)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_09700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_09800
 * @tc.name      : Skills::GetSchemeSpecificPart
 * @tc.desc      : loop addschemespecificpart, judge the return value of getschemespecificpart(index 150)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_09800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetSchemeSpecificPart)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_09800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_09900
 * @tc.name      : Skills::HasSchemeSpecificPart
 * @tc.desc      : loop addschemespecificpart, judge the return value of hasschemespecificpart
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_09900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasSchemeSpecificPart)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_09900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_10000
 * @tc.name      : Skills::HasSchemeSpecificPart
 * @tc.desc      : judge the return value of hasschemespecificpart,when specificpart is empty
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_10000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasSchemeSpecificPart)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_10000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_10100
 * @tc.name      : Skills::HasSchemeSpecificPart
 * @tc.desc      : add a special string to judge the return value of hasschemespecificpart
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_10100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasSchemeSpecificPart)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_10100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_10200
 * @tc.name      : Skills::RemoveSchemeSpecificPart
 * @tc.desc      : add specialstring. after deleting, judge hasschemespecificpart
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_10200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveSchemeSpecificPart)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_10200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_10300
 * @tc.name      : Skills::RemoveSchemeSpecificPart
 * @tc.desc      : add specialstring, after deleting, judge getschemespecificpart
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_10300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveSchemeSpecificPart)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_10300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_10400
 * @tc.name      : Skills::RemoveSchemeSpecificPart
 * @tc.desc      : loop add normalstring, after deleting, judge hasschemespecificpart
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_10400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveSchemeSpecificPart)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_10400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_10500
 * @tc.name      : Skills::RemoveSchemeSpecificPart
 * @tc.desc      : loop add normalstring, delete specialstring and judge hasschemespecificpart
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_10500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveSchemeSpecificPart)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_10500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_10600
 * @tc.name      : Skills::RemoveSchemeSpecificPart
 * @tc.desc      : loop add normalstring, after deleting, judge getschemespecificpart (the index value exceeds)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_10600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveSchemeSpecificPart)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_10600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_10700
 * @tc.name      : Skills::RemoveSchemeSpecificPart
 * @tc.desc      : loop add normalstring, delete specialstring and judge getschemespecificpart
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_10700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveSchemeSpecificPart)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_10700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_10800
 * @tc.name      : Skills::RemoveSchemeSpecificPart
 * @tc.desc      : add specialstring, delete specialstring, and judge countschemespecificparts
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_10800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveSchemeSpecificPart)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_10800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_10900
 * @tc.name      : Skills::RemoveSchemeSpecificPart
 * @tc.desc      : loop add normalstring, delete normalstring, and judge countschemespecificparts
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_10900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveSchemeSpecificPart)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_10900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_11000
 * @tc.name      : Skills::RemoveSchemeSpecificPart
 * @tc.desc      : loop add normalstring, delete specialstring, and judge countschemespecificparts
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_11000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveSchemeSpecificPart)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_11000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_11100
 * @tc.name      : Skills::AddPath_String_CountPaths
 * @tc.desc      : using addpath_string method to add path and judge the return value of countpaths
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_11100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_CountPaths)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_11100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_11200
 * @tc.name      : Skills::AddPath_String_CountPaths
 * @tc.desc      : loop uses addpath_string method to add path and judge the return value of countpaths
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_11200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_CountPaths)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_11200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_11300
 * @tc.name      : Skills::AddPath_String_MatchType_CountPaths
 * @tc.desc      : using addpath_string_matchtype method to add path and judge the return value of countpaths
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_11300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_MatchType_CountPaths)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_11300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_11400
 * @tc.name      : Skills::AddPath_String_MatchType_CountPaths
 * @tc.desc      : loop uses addpath_string_matchtype method to add path and judge the return value of countpaths
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_11400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_MatchType_CountPaths)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_11400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_11500
 * @tc.name      : Skills::AddPath_PatternMatcher_CountPaths
 * @tc.desc      : using addpath_patternmatcher method to add path and judge the return value of countpaths
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_11500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_PatternMatcher_CountPaths)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_11500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_11600
 * @tc.name      : Skills::AddPath_PatternMatcher_CountPaths
 * @tc.desc      : loop uses addpath_patternmatcher method to add path and judge the return value of countpaths
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_11600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_PatternMatcher_CountPaths)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_11600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_11700
 * @tc.name      : Skills::CountPaths
 * @tc.desc      : judge the return value of countpaths when path is empty
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_11700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountPaths)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_11700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_11800
 * @tc.name      : Skills::AddPath_String_GetPath
 * @tc.desc      : add a normal string to judge the return value of getpath(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_11800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_GetPath)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_11800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_11900
 * @tc.name      : Skills::AddPath_String_GetPath
 * @tc.desc      : judge the return value of getpath, when the index value exceeds(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_11900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_GetPath)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_11900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_12000
 * @tc.name      : Skills::AddPath_String_GetPath
 * @tc.desc      : judge the return value of getpath when the index value is negative(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_12000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_GetPath)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_12000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_12100
 * @tc.name      : Skills::AddPath_String_GetPath
 * @tc.desc      : judge the return value of getpath when scheme is null(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_12100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_GetPath)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_12100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_12200
 * @tc.name      : Skills::AddPath_String_GetPath
 * @tc.desc      : add a special string to judge the return value of getpath(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_12200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_GetPath)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_12200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_12300
 * @tc.name      : Skills::AddPath_String_GetPath
 * @tc.desc      : loop addscheme, judge the return value of getpath(index 0)(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_12300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_GetPath)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_12300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_12400
 * @tc.name      : Skills::AddPath_String_MatchType_GetPath
 * @tc.desc      : add a normal string to judge the return value of getpath(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_12400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_MatchType_GetPath)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_12400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_12500
 * @tc.name      : Skills::AddPath_String_MatchType_GetPath
 * @tc.desc      : judge the return value of getpath, when the index value exceeds(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_12500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_MatchType_GetPath)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_12500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_12600
 * @tc.name      : Skills::AddPath_String_MatchType_GetPath
 * @tc.desc      : judge the return value of getpath when the index value is negative(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_12600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_MatchType_GetPath)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_12600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_12700
 * @tc.name      : Skills::AddPath_String_MatchType_GetPath
 * @tc.desc      : judge the return value of getpath when scheme is null(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_12700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_MatchType_GetPath)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_12700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_12800
 * @tc.name      : Skills::AddPath_String_MatchType_GetPath
 * @tc.desc      : add a special string to judge the return value of getpath(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_12800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_MatchType_GetPath)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_12800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_12900
 * @tc.name      : Skills::AddPath_String_MatchType_GetPath
 * @tc.desc      : loop addscheme, judge the return value of getpath(index 0)(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_12900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_MatchType_GetPath)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_12900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_13000
 * @tc.name      : Skills::AddPath_PatternMatcher_GetPath
 * @tc.desc      : add a normal string to judge the return value of getpath(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_13000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_PatternMatcher_GetPath)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_13000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_13100
 * @tc.name      : Skills::AddPath_PatternMatcher_GetPath
 * @tc.desc      : judge the return value of getpath, when the index value exceeds(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_13100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_PatternMatcher_GetPath)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_13100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_13200
 * @tc.name      : Skills::AddPath_PatternMatcher_GetPath
 * @tc.desc      : judge the return value of getpath when the index value is negative(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_13200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_PatternMatcher_GetPath)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_13200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_13300
 * @tc.name      : Skills::AddPath_PatternMatcher_GetPath
 * @tc.desc      : judge the return value of getpath when scheme is null(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_13300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_PatternMatcher_GetPath)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_13300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_13400
 * @tc.name      : Skills::AddPath_PatternMatcher_GetPath
 * @tc.desc      : add a special string to judge the return value of getpath(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_13400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_PatternMatcher_GetPath)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_13400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_13500
 * @tc.name      : Skills::AddPath_PatternMatcher_GetPath
 * @tc.desc      : loop addscheme, judge the return value of getpath(index 0)(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_13500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_PatternMatcher_GetPath)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_13500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_13600
 * @tc.name      : Skills::GetPath
 * @tc.desc      : judge the return value of getpath when path is empty
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_13600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetPath)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_13600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_13700
 * @tc.name      : Skills::AddPath_String_HasPath
 * @tc.desc      : loop addauthority, judge the return value of haspath(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_13700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_HasPath)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_13700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_13800
 * @tc.name      : Skills::AddPath_String_HasPath
 * @tc.desc      : add a special string to judge the return value of haspath(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_13800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_HasPath)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_13800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_13900
 * @tc.name      : Skills::AddPath_String_MatchType_HasPath
 * @tc.desc      : loop addauthority, judge the return value of haspath(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_13900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_MatchType_HasPath)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_13900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_14000
 * @tc.name      : Skills::AddPath_String_MatchType_HasPath
 * @tc.desc      : add a special string to judge the return value of haspath(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_14000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_String_MatchType_HasPath)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_14000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_14100
 * @tc.name      : Skills::AddPath_PatternMatcher_HasPath
 * @tc.desc      : loop addauthority, judge the return value of haspath(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_14100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_PatternMatcher_HasPath)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_14100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_14200
 * @tc.name      : Skills::AddPath_PatternMatcher_HasPath
 * @tc.desc      : add a special string to judge the return value of haspath(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_14200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddPath_PatternMatcher_HasPath)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_14200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_14300
 * @tc.name      : Skills::HasPath
 * @tc.desc      : judge the return value of haspath,when authority is empty
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_14300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasPath)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_14300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_14400
 * @tc.name      : Skills::RemovePath_String
 * @tc.desc      : add specialstring. after deleting, judge haspath(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_14400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_14400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_14500
 * @tc.name      : Skills::RemovePath_String
 * @tc.desc      : add specialstring, after deleting, judge getpath(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_14500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_14500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_14600
 * @tc.name      : Skills::RemovePath_String
 * @tc.desc      : loop add normalstring, after deleting, judge haspath(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_14600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_14600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_14700
 * @tc.name      : Skills::RemovePath_String
 * @tc.desc      : loop add normalstring, delete specialstring and judge haspath(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_14700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_14700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_14800
 * @tc.name      : Skills::RemovePath_String
 * @tc.desc      : loop add normalstring, after deleting, judge getpath (the index value exceeds)(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_14800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_14800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_14900
 * @tc.name      : Skills::RemovePath_String
 * @tc.desc      : loop add normalstring, delete specialstring and judge getpath(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_14900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_14900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_15000
 * @tc.name      : Skills::RemovePath_String
 * @tc.desc      : add specialstring, delete specialstring, and judge countpaths(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_15000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_15000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_15100
 * @tc.name      : Skills::RemovePath_String
 * @tc.desc      : loop add normalstring, delete normalstring, and judge countpaths(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_15100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_15100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_15200
 * @tc.name      : Skills::RemovePath_String
 * @tc.desc      : loop add normalstring, delete specialstring, and judge countpaths(addpath_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_15200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_15200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_15300
 * @tc.name      : Skills::RemovePath_String_MatchType
 * @tc.desc      : add specialstring. after deleting, judge haspath(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_15300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String_MatchType)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_15300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_15400
 * @tc.name      : Skills::RemovePath_String_MatchType
 * @tc.desc      : add specialstring, after deleting, judge getpath(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_15400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String_MatchType)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_15400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_15500
 * @tc.name      : Skills::RemovePath_String_MatchType
 * @tc.desc      : loop add normalstring, after deleting, judge haspath(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_15500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String_MatchType)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_15500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_15600
 * @tc.name      : Skills::RemovePath_String_MatchType
 * @tc.desc      : loop add normalstring, delete specialstring and judge haspath(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_15600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String_MatchType)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_15600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_15700
 * @tc.name      : Skills::RemovePath_String_MatchType
 * @tc.desc      : loop add normalstring, after deleting, judge getpath
 *                 (the index value exceeds)(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_15700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String_MatchType)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_15700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_15800
 * @tc.name      : Skills::RemovePath_String_MatchType
 * @tc.desc      : loop add normalstring, delete specialstring and judge getpath(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_15800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String_MatchType)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_15800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_15900
 * @tc.name      : Skills::RemovePath_String_MatchType
 * @tc.desc      : add specialstring, delete specialstring, and judge countpaths(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_15900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String_MatchType)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_15900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_16000
 * @tc.name      : Skills::RemovePath_String_MatchType
 * @tc.desc      : loop add normalstring, delete normalstring, and judge countpaths(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_16000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String_MatchType)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_16000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_16100
 * @tc.name      : Skills::RemovePath_String_MatchType
 * @tc.desc      : loop add normalstring, delete specialstring, and judge countpaths(addpath_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_16100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_String_MatchType)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_16100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_16200
 * @tc.name      : Skills::RemovePath_PatternMatcher
 * @tc.desc      : add specialstring. after deleting, judge haspath(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_16200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_PatternMatcher)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_16200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_16300
 * @tc.name      : Skills::RemovePath_PatternMatcher
 * @tc.desc      : add specialstring, after deleting, judge getpath(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_16300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_PatternMatcher)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_16300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_16400
 * @tc.name      : Skills::RemovePath_PatternMatcher
 * @tc.desc      : loop add normalstring, after deleting, judge haspath(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_16400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_PatternMatcher)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_16400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_16500
 * @tc.name      : Skills::RemovePath_PatternMatcher
 * @tc.desc      : loop add normalstring, delete specialstring and judge haspath(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_16500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_PatternMatcher)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_16500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_16600
 * @tc.name      : Skills::RemovePath_PatternMatcher
 * @tc.desc      : loop add normalstring, after deleting,
 *                 judge getpath (the index value exceeds)(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_16600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_PatternMatcher)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_16600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_16700
 * @tc.name      : Skills::RemovePath_PatternMatcher
 * @tc.desc      : loop add normalstring, delete specialstring and judge getpath(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_16700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_PatternMatcher)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_16700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_16800
 * @tc.name      : Skills::RemovePath_PatternMatcher
 * @tc.desc      : add specialstring, delete specialstring, and judge countpaths(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_16800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_PatternMatcher)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_16800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_16900
 * @tc.name      : Skills::RemovePath_PatternMatcher
 * @tc.desc      : loop add normalstring, delete normalstring, and judge countpaths(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_16900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_PatternMatcher)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_16900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_17000
 * @tc.name      : Skills::RemovePath_PatternMatcher
 * @tc.desc      : loop add normalstring, delete specialstring, and judge countpaths(addpath_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_17000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_PatternMatcher)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_17000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_17100
 * @tc.name      : Skills::RemovePath_Other
 * @tc.desc      : using addpath_string and addpath_ string_ matchtype two methods to add path,
 *                 after deleting, judge the return value of haspath
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_17100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_Other)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_17100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_17200
 * @tc.name      : Skills::RemovePath_Other
 * @tc.desc      : loop uses addpath_string_ the matchtype method adds a path, and after deleting,
 *                 judges the return value of countpaths
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_17200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_Other)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_17200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_17300
 * @tc.name      : Skills::RemovePath_Other
 * @tc.desc      : using addpath_string and addpath_ string_ matchtype two methods to add path,
 *                 judge the return value of countpaths
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_17300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemovePath_Other)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_17300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_17400
 * @tc.name      : Skills::AddType_String_CountTypes
 * @tc.desc      : using addtype_string method to add type and judge the return value of counttypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_17400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_CountTypes)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_17400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_17500
 * @tc.name      : Skills::AddType_String_CountTypes
 * @tc.desc      : loop uses addtype_string method to add type and judge the return value of counttypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_17500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_CountTypes)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_17500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_17600
 * @tc.name      : Skills::AddType_String_MatchType_CountTypes
 * @tc.desc      : using addtype_string_matchtype method to add type and judge the return value of counttypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_17600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_MatchType_CountTypes)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_17600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_17700
 * @tc.name      : Skills::AddType_String_MatchType_CountTypes
 * @tc.desc      : loop uses addtype_string_matchtype method to add type and judge the return value of counttypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_17700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_MatchType_CountTypes)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_17700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_17800
 * @tc.name      : Skills::AddType_PatternMatcher_CountTypes
 * @tc.desc      : using addtype_patternmatcher method to add type and judge the return value of counttypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_17800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_PatternMatcher_CountTypes)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_17800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_17900
 * @tc.name      : Skills::AddType_PatternMatcher_CountTypes
 * @tc.desc      : loop uses addtype_patternmatcher method to add type and judge the return value of counttypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_17900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_PatternMatcher_CountTypes)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_17900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_18000
 * @tc.name      : Skills::CountTypes
 * @tc.desc      : judge the return value of counttypes when type is empty
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_18000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::CountTypes)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_18000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_18100
 * @tc.name      : Skills::AddType_String_GetType
 * @tc.desc      : add a normal string to judge the return value of gettype(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_18100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_GetType)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_18100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_18200
 * @tc.name      : Skills::AddType_String_GetType
 * @tc.desc      : judge the return value of gettype, when the index value exceeds(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_18200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_GetType)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_18200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_18300
 * @tc.name      : Skills::AddType_String_GetType
 * @tc.desc      : judge the return value of gettype when the index value is negative(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_18300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_GetType)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_18300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_18400
 * @tc.name      : Skills::AddType_String_GetType
 * @tc.desc      : judge the return value of gettype when scheme is null(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_18400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_GetType)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_18400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_18500
 * @tc.name      : Skills::AddType_String_GetType
 * @tc.desc      : add a special string to judge the return value of gettype(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_18500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_GetType)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_18500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_18600
 * @tc.name      : Skills::AddType_String_GetType
 * @tc.desc      : loop addscheme, judge the return value of gettype(index 0)(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_18600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_GetType)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_18600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_18700
 * @tc.name      : Skills::AddType_String_MatchType_GetType
 * @tc.desc      : add a normal string to judge the return value of gettype(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_18700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_MatchType_GetType)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_18700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_18800
 * @tc.name      : Skills::AddType_String_MatchType_GetType
 * @tc.desc      : judge the return value of gettype, when the index value exceeds(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_18800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_MatchType_GetType)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_18800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_18900
 * @tc.name      : Skills::AddType_String_MatchType_GetType
 * @tc.desc      : judge the return value of gettype when the index value is negative(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_18900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_MatchType_GetType)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_18900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_19000
 * @tc.name      : Skills::AddType_String_MatchType_GetType
 * @tc.desc      : judge the return value of gettype when scheme is null(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_19000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_MatchType_GetType)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_19000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_19100
 * @tc.name      : Skills::AddType_String_MatchType_GetType
 * @tc.desc      : add a special string to judge the return value of gettype(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_19100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_MatchType_GetType)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_19100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_19200
 * @tc.name      : Skills::AddType_String_MatchType_GetType
 * @tc.desc      : loop addscheme, judge the return value of gettype(index 0)(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_19200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_MatchType_GetType)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_19200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_19300
 * @tc.name      : Skills::AddType_PatternMatcher_GetType
 * @tc.desc      : add a normal string to judge the return value of gettype(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_19300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_PatternMatcher_GetType)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_19300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_19400
 * @tc.name      : Skills::AddType_PatternMatcher_GetType
 * @tc.desc      : judge the return value of gettype, when the index value exceeds(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_19400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_PatternMatcher_GetType)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_19400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_19500
 * @tc.name      : Skills::AddType_PatternMatcher_GetType
 * @tc.desc      : judge the return value of gettype when the index value is negative(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_19500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_PatternMatcher_GetType)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_19500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_19600
 * @tc.name      : Skills::AddType_PatternMatcher_GetType
 * @tc.desc      : judge the return value of gettype when scheme is null(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_19600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_PatternMatcher_GetType)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_19600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_19700
 * @tc.name      : Skills::AddType_PatternMatcher_GetType
 * @tc.desc      : add a special string to judge the return value of gettype(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_19700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_PatternMatcher_GetType)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_19700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_19800
 * @tc.name      : Skills::AddType_PatternMatcher_GetType
 * @tc.desc      : loop addscheme, judge the return value of gettype(index 0)(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_19800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_PatternMatcher_GetType)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_19800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_19900
 * @tc.name      : Skills::GetType
 * @tc.desc      : judge the return value of gettype when type is empty
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_19900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetType)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_19900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_20000
 * @tc.name      : Skills::AddType_String_HasType
 * @tc.desc      : loop addauthority, judge the return value of hastype(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_20000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_HasType)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_20000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_20100
 * @tc.name      : Skills::AddType_String_HasType
 * @tc.desc      : add a special string to judge the return value of hastype(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_20100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_HasType)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_20100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_20200
 * @tc.name      : Skills::AddType_String_MatchType_HasType
 * @tc.desc      : loop addauthority, judge the return value of hastype(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_20200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_MatchType_HasType)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_20200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_20300
 * @tc.name      : Skills::AddType_String_MatchType_HasType
 * @tc.desc      : add a special string to judge the return value of hastype(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_20300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_String_MatchType_HasType)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_20300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_20400
 * @tc.name      : Skills::AddType_PatternMatcher_HasType
 * @tc.desc      : loop addauthority, judge the return value of hastype(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_20400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_PatternMatcher_HasType)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_20400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_20500
 * @tc.name      : Skills::AddType_PatternMatcher_HasType
 * @tc.desc      : add a special string to judge the return value of hastype(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_20500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddType_PatternMatcher_HasType)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_20500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_20600
 * @tc.name      : Skills::HasType
 * @tc.desc      : judge the return value of hastype,when authority is empty
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_20600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::HasType)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_20600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_20700
 * @tc.name      : Skills::RemoveType_String
 * @tc.desc      : add specialstring. after deleting, judge hastype(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_20700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_20700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_20800
 * @tc.name      : Skills::RemoveType_String
 * @tc.desc      : add specialstring, after deleting, judge gettype(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_20800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_20800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_20900
 * @tc.name      : Skills::RemoveType_String
 * @tc.desc      : loop add normalstring, after deleting, judge hastype(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_20900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_20900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_21000
 * @tc.name      : Skills::RemoveType_String
 * @tc.desc      : loop add normalstring, delete specialstring and judge hastype(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_21000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_21000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_21100
 * @tc.name      : Skills::RemoveType_String
 * @tc.desc      : loop add normalstring, after deleting, judge gettype (the index value exceeds)(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_21100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_21100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_21200
 * @tc.name      : Skills::RemoveType_String
 * @tc.desc      : loop add normalstring, delete specialstring and judge gettype(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_21200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_21200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_21300
 * @tc.name      : Skills::RemoveType_String
 * @tc.desc      : add specialstring, delete specialstring, and judge counttypes(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_21300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_21300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_21400
 * @tc.name      : Skills::RemoveType_String
 * @tc.desc      : loop add normalstring, delete normalstring, and judge counttypes(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_21400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_21400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_21500
 * @tc.name      : Skills::RemoveType_String
 * @tc.desc      : loop add normalstring, delete specialstring, and judge counttypes(addtype_string)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_21500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_21500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_21600
 * @tc.name      : Skills::RemoveType_String_MatchType
 * @tc.desc      : add specialstring. after deleting, judge hastype(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_21600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String_MatchType)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_21600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_21700
 * @tc.name      : Skills::RemoveType_String_MatchType
 * @tc.desc      : add specialstring, after deleting, judge gettype(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_21700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String_MatchType)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_21700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_21800
 * @tc.name      : Skills::RemoveType_String_MatchType
 * @tc.desc      : loop add normalstring, after deleting, judge hastype(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_21800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String_MatchType)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_21800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_21900
 * @tc.name      : Skills::RemoveType_String_MatchType
 * @tc.desc      : loop add normalstring, delete specialstring and
 *                 judge hastype(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_21900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String_MatchType)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_21900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_22000
 * @tc.name      : Skills::RemoveType_String_MatchType
 * @tc.desc      : loop add normalstring, after deleting,
 *                 judge gettype (the index value exceeds)(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_22000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String_MatchType)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_22000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_22100
 * @tc.name      : Skills::RemoveType_String_MatchType
 * @tc.desc      : loop add normalstring, delete specialstring and
 *                 judge gettype(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_22100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String_MatchType)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_22100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_22200
 * @tc.name      : Skills::RemoveType_String_MatchType
 * @tc.desc      : add specialstring, delete specialstring,
 *                 and judge counttypes(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_22200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String_MatchType)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_22200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_22300
 * @tc.name      : Skills::RemoveType_String_MatchType
 * @tc.desc      : loop add normalstring, delete normalstring,
 *                 and judge counttypes(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_22300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String_MatchType)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_22300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_22400
 * @tc.name      : Skills::RemoveType_String_MatchType
 * @tc.desc      : loop add normalstring, delete specialstring,
 *                 and judge counttypes(addtype_string_matchtype)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_22400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_String_MatchType)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_22400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_22500
 * @tc.name      : Skills::RemoveType_PatternMatcher
 * @tc.desc      : add specialstring. after deleting, judge hastype(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_22500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_PatternMatcher)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_22500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_22600
 * @tc.name      : Skills::RemoveType_PatternMatcher
 * @tc.desc      : add specialstring, after deleting, judge gettype(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_22600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_PatternMatcher)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_22600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_22700
 * @tc.name      : Skills::RemoveType_PatternMatcher
 * @tc.desc      : loop add normalstring, after deleting, judge hastype(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_22700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_PatternMatcher)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_22700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_22800
 * @tc.name      : Skills::RemoveType_PatternMatcher
 * @tc.desc      : loop add normalstring, delete specialstring and judge hastype(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_22800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_PatternMatcher)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_22800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_22900
 * @tc.name      : Skills::RemoveType_PatternMatcher
 * @tc.desc      : loop add normalstring, after deleting, judge gettype
 *                 (the index value exceeds)(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_22900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_PatternMatcher)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_22900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_23000
 * @tc.name      : Skills::RemoveType_PatternMatcher
 * @tc.desc      : loop add normalstring, delete specialstring
 *                 and judge gettype(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_23000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_PatternMatcher)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_23000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_23100
 * @tc.name      : Skills::RemoveType_PatternMatcher
 * @tc.desc      : add specialstring, delete specialstring,
 *                 and judge counttypes(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_23100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_PatternMatcher)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_23100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_23200
 * @tc.name      : Skills::RemoveType_PatternMatcher
 * @tc.desc      : loop add normalstring, delete normalstring,
 *                 and judge counttypes(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_23200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_PatternMatcher)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_23200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_23300
 * @tc.name      : Skills::RemoveType_PatternMatcher
 * @tc.desc      : loop add normalstring, delete specialstring,
 *                 and judge counttypes(addtype_patternmatcher)
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_23300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData =
        "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_PatternMatcher)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_23300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_23400
 * @tc.name      : Skills::RemoveType_Other
 * @tc.desc      : using addtype_string and addtype_string_matchtype two methods to add type,
 *                 after deleting, judge the return value of hastype
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_23400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_Other)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_23400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_23500
 * @tc.name      : Skills::RemoveType_Other
 * @tc.desc      : loop uses addtype_string_ the matchtype method adds a type, and after deleting,
 *                 judges the return value of counttypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_23500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_Other)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_23500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_23600
 * @tc.name      : Skills::RemoveType_Other
 * @tc.desc      : using addtype_string and addtype_string_matchtype two methods to add type,
 *                 judge the return value of counttypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_23600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_Other)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_23600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_23700
 * @tc.name      : Skills::RemoveType_Other
 * @tc.desc      : using addtype_string and addtype_string_matchtype two methods to add special type,
 *                 judge the return value of counttypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_23700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_Other)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_23700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_23800
 * @tc.name      : Skills::RemoveType_Other
 * @tc.desc      : using addtype_string and addtype_string_matchtype two methods to add special type,
 *                 judge the return value of hastypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_23800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_Other)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_23800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_23900
 * @tc.name      : Skills::RemoveType_Other
 * @tc.desc      : using addtype_string and addtype_string_matchtype two methods to add special type,
 *                 judge the return value of hastypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_23900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_Other)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_23900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_24000
 * @tc.name      : Skills::RemoveType_Other
 * @tc.desc      : using addtype_string and addtype_string_matchtype two methods to add special type,
 *                 judge the return value of counttypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_24000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_Other)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_24000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_24100
 * @tc.name      : Skills::RemoveType_Other
 * @tc.desc      : using addtype_string and addtype_string_matchtype two methods to add special type,
 *                 judge the return value of counttypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_24100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_Other)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_24100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_24200
 * @tc.name      : Skills::RemoveType_Other
 * @tc.desc      : using addtype_string and addtype_string_matchtype two methods to add special type,
 *                 judge the return value of hastypes
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_24200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::RemoveType_Other)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_24200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_24300
 * @tc.name      : Skills::GetEntities
 * @tc.desc      : add entity to determine the size of getentities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_24300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetEntities)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_24300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_24400
 * @tc.name      : Skills::GetEntities
 * @tc.desc      : repeatedly add entity to determine the size of getentities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_24400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetEntities)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_24400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_24500
 * @tc.name      : Skills::GetEntities
 * @tc.desc      : add entity to determine whether the return value of
 *                 countentities is the same as the size of getentities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_24500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetEntities)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_24500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_24600
 * @tc.name      : Skills::GetEntities
 * @tc.desc      : add entity to determine whether the return value of getentity
 *                 is the same as the data in getentities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_24600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetEntities)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_24600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_24700
 * @tc.name      : Skills::GetEntities
 * @tc.desc      : add specialstring entity to determine whether the return value of
 *                 getentity is the same as the data in getentities
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_24700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetEntities)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_24700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_24800
 * @tc.name      : Skills::GetWantParams
 * @tc.desc      : judge whether the bool type value of setwantpams is the same as that of getwantpams
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_24800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetWantParams)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_24800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_24900
 * @tc.name      : Skills::GetWantParams
 * @tc.desc      : judge whether the loog type value of setwantpams is the same as that of getwantpams
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_24900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetWantParams)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_24900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_25000
 * @tc.name      : Skills::GetWantParams
 * @tc.desc      : judge whether the int type value of setwantpams is the same as that of getwantpams
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_25000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetWantParams)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_25000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_25100
 * @tc.name      : Skills::GetWantParams
 * @tc.desc      : judge whether the string type value(specialstring) of setwantpams
 *                 is the same as that of getwantpams
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_25100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetWantParams)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_25100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_25200
 * @tc.name      : Skills::GetWantParams
 * @tc.desc      : judge whether the string type value(normalstring)of setwantpams
 *                 is the same as that of getwantpams
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_25200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::GetWantParams)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_25200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_25300
 * @tc.name      : Skills::Match
 * @tc.desc      : set the skills object and want object to judge the return value of match
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_25300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Match)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_25300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_25400
 * @tc.name      : Skills::Match
 * @tc.desc      : set the skills object and want object to judge the return value of match
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_25400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Match)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_25400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_25500
 * @tc.name      : Skills::Match
 * @tc.desc      : set the skills object and want object to judge the return value of match
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_25500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Match)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_25500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_25600
 * @tc.name      : Skills::Match
 * @tc.desc      : set the skills object and want object to judge the return value of match
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_25600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Match)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_25600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_25700
 * @tc.name      : Skills::Match
 * @tc.desc      : set the skills object and want object to judge the return value of match
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_25700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Match)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_25700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_25800
 * @tc.name      : Skills::Match
 * @tc.desc      : set the skills object and want object to judge the return value of match
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_25800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Match)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_25800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_25900
 * @tc.name      : Skills::Unmarshalling
 * @tc.desc      : set the skills object data to be empty and the current object is marshaling.
 *                 judge whether the unmatched object is the same as the current object
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_25900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Unmarshalling)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_25900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_26000
 * @tc.name      : Skills::Unmarshalling
 * @tc.desc      : set the skills object data to be normalstring and the current object is marshaling.
 *                 judge whether the unmatched object is the same as the current object
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_26000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Unmarshalling)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_26000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_26100
 * @tc.name      : Skills::Unmarshalling
 * @tc.desc      : set the skills object data to be specialstringg and the current object is marshaling.
 *                 judge whether the unmatched object is the same as the current object
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_26100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Unmarshalling)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_26100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_26200
 * @tc.name      : Skills::Unmarshalling
 * @tc.desc      : set the skills object data to be specialstringg(normalstring,empty)
 *                 and the current object is marshaling.
 *                 judge whether the unmatched object is the same as the current object
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_26200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Unmarshalling)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_26200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_26300
 * @tc.name      : SkillsApi::Skills
 * @tc.desc      : use skills skill; construct skill object in this way,
 *                 and call count method to judge whether the construction is successful
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_26300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Skills)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_26300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_26400
 * @tc.name      : SkillsApi::Skills
 * @tc.desc      : use skills skill; construct skill object in this way,
 *                 and call get method to judge whether the construction is successful
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_26400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Skills)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_26400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_26500
 * @tc.name      : SkillsApi::Skills
 * @tc.desc      : use skills skill; construct skill object in this way,
 *                 and call has method to judge whether the construction is successful
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_26500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Skills)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_26500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_26600
 * @tc.name      : SkillsApi::Skills
 * @tc.desc      : use skills skill; in this way, construct the skill object,
 *                 and call the remove method to judge whether the construction is successful
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_26600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Skills)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_26600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_26700
 * @tc.name      : SkillsApi::Skills_Skills
 * @tc.desc      : use skills copy constructor to create skill object and
 *                 judge whether the copied object is equal to the original object
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_26700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Skills_Skills)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_26700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_26800
 * @tc.name      : SkillsApi::Skills_Skills
 * @tc.desc      : use skills copy constructor to create skill object and
 *                 judge whether the copied object is equal to the original object
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_26800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Skills_Skills)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_26800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_26900
 * @tc.name      : SkillsApi::Skills_Skills
 * @tc.desc      : use skills copy constructor to create skill object and
 *                 judge whether the copied object is equal to the original object
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_26900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Skills_Skills)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_26900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_27000
 * @tc.name      : SkillsApi::Skills_Skills
 * @tc.desc      : use skills copy constructor to create skill object and
 *                 judge whether the copied object is equal to the original object
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_27000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::Skills_Skills)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_27000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_27100
 * @tc.name      : SkillsApi::AddAction
 * @tc.desc      : Add a variety of characters as action to the skills object,
 *                 use the count function to verify whether the addition is successful
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_27100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddAction)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_27100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_27200
 * @tc.name      : SkillsApi::AddEntity
 * @tc.desc      : Add a variety of characters as Entity to the skills object,
 *                 use the count function to verify whether the addition is successful
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_27200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddEntity)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_27200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_27300
 * @tc.name      : SkillsApi::AddAuthority
 * @tc.desc      : Add a variety of characters as Authority to the skills object,
 *                 use the count function to verify whether the addition is successful
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_27300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddAuthority)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_27300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_27400
 * @tc.name      : SkillsApi::AddScheme
 * @tc.desc      : Add a variety of characters as Scheme to the skills object,
 *                 use the count function to verify whether the addition is successful
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_27400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddScheme)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_27400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_Skills_27500
 * @tc.name      : SkillsApi::AddSchemeSpecificPart
 * @tc.desc      : Add a variety of characters as SchemeSpecificPart to the skills object,
 *                 use the count function to verify whether the addition is successful
 */
HWTEST_F(ActsAmsKitSkillsTest, AMS_Page_Skills_27500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(fourthAbilityName, bundleName);
    std::string eventData = "SkillsApi_" + std::to_string(static_cast<int>(SkillsApi::AddSchemeSpecificPart)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageFourthAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageFourthAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageFourthAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result && i > 1) {
            GTEST_LOG_(INFO) << "AMS_Page_Skills_27500 : " << i;
            break;
        }
    }
}
