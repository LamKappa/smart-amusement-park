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
static const std::string thirdAbilityName = "ThirdAbility";
static const std::string fifthAbilityName = "FifthAbility";

std::vector<std::string> bundleNameList = {
    "com.ohos.amsst.AppKit",
};
std::vector<std::string> hapNameList = {
    "amsKitSystemTest",
};
constexpr int WAIT_TIME = 1000;

int amsKitSTCode = 0;
int amsKitSystemTestFifthAbilityCode = 0;
const int delay = 10;
}  // namespace

class ActsAmsKitProcessInfoTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static bool SubscribeEvent();
    void TestSkills(Want want);
    void StartAbilityKitTest(const std::string &abilityName, const std::string &bundleName);
    void TerminateAbility(const std::string &eventName, const std::string &abilityName);
    void StartSecondAbility();
    void StartFifthAbility();
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

std::vector<std::string> ActsAmsKitProcessInfoTest::eventList = {
    g_respPageThirdAbilityST,
    g_EVENT_RESP_FIFTH,
};

StressTestLevel ActsAmsKitProcessInfoTest::stLevel_{};
std::shared_ptr<ActsAmsKitProcessInfoTest::AppEventSubscriber> ActsAmsKitProcessInfoTest::subscriber_ = nullptr;

Event ActsAmsKitProcessInfoTest::event = Event();
Event ActsAmsKitProcessInfoTest::abilityEvent = Event();
sptr<IAppMgr> ActsAmsKitProcessInfoTest::appMs = nullptr;
sptr<IAbilityManager> ActsAmsKitProcessInfoTest::abilityMs = nullptr;
std::unordered_map<std::string, int> ActsAmsKitProcessInfoTest::mapState = {
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

void ActsAmsKitProcessInfoTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
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

void ActsAmsKitProcessInfoTest::SetUpTestCase(void)
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

void ActsAmsKitProcessInfoTest::TearDownTestCase(void)
{
    STAbilityUtil::UninstallBundle(bundleNameList);
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void ActsAmsKitProcessInfoTest::SetUp(void)
{}

void ActsAmsKitProcessInfoTest::TearDown(void)
{
    STAbilityUtil::CleanMsg(event);
    STAbilityUtil::CleanMsg(abilityEvent);
}

bool ActsAmsKitProcessInfoTest::SubscribeEvent()
{
    const std::vector<std::string> eventList = {
        g_respPageThirdAbilityST,
        g_EVENT_RESP_FIFTH,
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

void ActsAmsKitProcessInfoTest::StartAbilityKitTest(const std::string &abilityName, const std::string &bundleName)
{
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(abilityEvent, abilityName + g_abilityStateOnActive, 0), 0);
}

void ActsAmsKitProcessInfoTest::StartFifthAbility()
{
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", fifthAbilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
    EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, mapState["onActive"], 0));
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_0100
 * @tc.name      : ProcessInfo::GetPid
 * @tc.desc      : judge whether the current process id is greater than 0
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_0100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::GetPid)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_0100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_0200
 * @tc.name      : ProcessInfo::GetPid
 * @tc.desc      : get the intermediate value of process id
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_0200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::GetPid)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_0200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_0300
 * @tc.name      : ProcessInfo::GetPid
 * @tc.desc      : get the minimum value of process id
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_0300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::GetPid)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_0300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_0400
 * @tc.name      : ProcessInfo::GetPid
 * @tc.desc      : get the maximum value of process id
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_0400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::GetPid)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_0400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_0500
 * @tc.name      : ProcessInfo::GetProcessName
 * @tc.desc      : determine whether the current process name is equal to bundlename
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_0500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::GetProcessName)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_0500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_0600
 * @tc.name      : ProcessInfo::GetProcessName
 * @tc.desc      : get a null value of process name
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_0600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::GetProcessName)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_0600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_0700
 * @tc.name      : ProcessInfo::GetProcessName
 * @tc.desc      : gets the normal character of the process name
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_0700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::GetProcessName)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_0700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_0800
 * @tc.name      : ProcessInfo::GetProcessName
 * @tc.desc      : gets the special character of the process name
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_0800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::GetProcessName)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_0800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_0900
 * @tc.name      : ProcessInfo::Marshalling
 * @tc.desc      : marshaling the current process ,get the processname after marshaling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_0900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Marshalling)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_0900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_1000
 * @tc.name      : ProcessInfo::Marshalling
 * @tc.desc      : marshaling the current process ,get the processid after marshaling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_1000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Marshalling)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_1000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_1100
 * @tc.name      : ProcessInfo::Marshalling
 * @tc.desc      : marshaling the construction processinfo ,get the processname after marshaling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_1100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Marshalling)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_1100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_1200
 * @tc.name      : ProcessInfo::Marshalling
 * @tc.desc      : marshaling the construction processinfo ,get the processid(minimum) after marshaling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_1200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Marshalling)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_1200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_1300
 * @tc.name      : ProcessInfo::Marshalling
 * @tc.desc      : marshaling the current process ,get the processname(null) after marshaling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_1300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Marshalling)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_1300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_1400
 * @tc.name      : ProcessInfo::Marshalling
 * @tc.desc      : marshaling the construction processinfo ,get the processid(maximum) after marshaling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_1400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Marshalling)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_1400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_1500
 * @tc.name      : ProcessInfo::Unmarshalling
 * @tc.desc      : use special string to customize parcel and test processInfo after unmarshalling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_1500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Unmarshalling)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_1500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_1600
 * @tc.name      : ProcessInfo::Unmarshalling
 * @tc.desc      : use normal string to customize parcel and test processInfo after unmarshalling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_1600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Unmarshalling)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_1600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_1700
 * @tc.name      : ProcessInfo::Unmarshalling
 * @tc.desc      : use null string to customize parcel and test processInfo after unmarshalling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_1700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Unmarshalling)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_1700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_1800
 * @tc.name      : ProcessInfo::Unmarshalling
 * @tc.desc      : use 0 to customize parcel and test processInfo after unmarshalling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_1800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Unmarshalling)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_1800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_1900
 * @tc.name      : ProcessInfo::Unmarshalling
 * @tc.desc      : use minimum to customize parcel and test processInfo after unmarshalling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_1900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Unmarshalling)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_1900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_2000
 * @tc.name      : ProcessInfo::Unmarshalling
 * @tc.desc      : use maximum to customize parcel and test processInfo after unmarshalling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_2000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Unmarshalling)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_2000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_2100
 * @tc.name      : ProcessInfo::Unmarshalling
 * @tc.desc      : constructing processInfo with normal strings and minimum values,
 *                 compare the data after marshalling and unmarshalling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_2100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Unmarshalling)) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_2100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_2200
 * @tc.name      : ProcessInfo::Unmarshalling
 * @tc.desc      : constructing processInfo with special strings and maximum values,
 *                 compare the data after marshalling and unmarshalling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_2200, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Unmarshalling)) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_2200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_2300
 * @tc.name      : ProcessInfo::Unmarshalling
 * @tc.desc      : constructing processInfo with special strings and minimum values,
 *                 compare the data after marshalling and unmarshalling
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_2300, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::Unmarshalling)) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_2300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_2400
 * @tc.name      : ProcessInfoApi::ProcessInfo
 * @tc.desc      : use the constructor without parameters, judge determine the PID of the current object
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_2400, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::ProcessInfo)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_2400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_2500
 * @tc.name      : ProcessInfoApi::ProcessInfo
 * @tc.desc      : use the constructor without parameters,
 *                 judge determine the processName of the current object
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_2500, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData = "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::ProcessInfo)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_2500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_2600
 * @tc.name      : ProcessInfo_String_int
 * @tc.desc      : use the constructor with parameters (PID is 0, processname is normal string),
 *                 judge whether PID and processname are the same as the parameters
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_2600, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData =
        "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::ProcessInfo_String_int)) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_2600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_2700
 * @tc.name      : ProcessInfo_String_int
 * @tc.desc      : use the constructor with parameters (PID is mininum, processname is normal string),
 *                 judge whether PID and processname are the same as the parameters
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_2700, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData =
        "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::ProcessInfo_String_int)) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_2700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_2800
 * @tc.name      : ProcessInfo_String_int
 * @tc.desc      : use the constructor with parameters (PID is maxinum, processname is normal string),
 *                 judge whether PID and processname are the same as the parameters
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_2800, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData =
        "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::ProcessInfo_String_int)) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_2800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_2900
 * @tc.name      : ProcessInfo_String_int
 * @tc.desc      : use the constructor with parameters (PID is 0, processname is special string),
 *                 judge whether PID and processname are the same as the parameters
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_2900, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData =
        "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::ProcessInfo_String_int)) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_2900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_3000
 * @tc.name      : ProcessInfo_String_int
 * @tc.desc      : use the constructor with parameters (PID is mininum, processname is special string),
 *                 judge whether PID and processname are the same as the parameters
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_3000, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData =
        "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::ProcessInfo_String_int)) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_3000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_ProcessInfo_3100
 * @tc.name      : ProcessInfo_String_int
 * @tc.desc      : use the constructor with parameters (PID is maxinum, processname is special string),
 *                 judge whether PID and processname are the same as the parameters
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_ProcessInfo_3100, Function | MediumTest | Level1)
{
    StartAbilityKitTest(thirdAbilityName, bundleName);
    std::string eventData =
        "ProcessInfoApi_" + std::to_string(static_cast<int>(ProcessInfoApi::ProcessInfo_String_int)) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_requPageThirdAbilityST, ++amsKitSTCode, eventData);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_respPageThirdAbilityST, amsKitSTCode));
        std::string data = STAbilityUtil::GetData(event, g_respPageThirdAbilityST, amsKitSTCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_ProcessInfo_3100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_0100
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set string param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_0100, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_0100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_0200
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set bool param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_0200, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_0200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_0300
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set byte param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_0300, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_0300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_0400
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set char param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_0400, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_0400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_0500
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set short param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_0500, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_0500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_0600
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set int param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_0600, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_0600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_0700
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set long param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_0700, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_0700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_0800
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set float param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_0800, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_0800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_0900
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set double param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_0900, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_0900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_1000
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set string array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_1000, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_9";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_1000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_1100
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set bool array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_1100, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_10";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_1100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_1200
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set byte array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_1200, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_11";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_1200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_1300
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set char array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_1300, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_12";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_1300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_1400
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set short array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_1400, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_13";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_1400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_1500
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set int array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_1500, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_14";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_1500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_1600
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set long array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_1600, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_15";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_1600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_1700
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set float array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_1700, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_16";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_1700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_1800
 * @tc.name      : WantParams.SetParam
 * @tc.desc      : set double array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_1800, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::SetParam) + "_17";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_1800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_1900
 * @tc.name      : WantParams.HasParam
 * @tc.desc      : empty key on empty WantParams
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_1900, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::HasParam) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_1900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_2000
 * @tc.name      : WantParams.HasParam
 * @tc.desc      : empty key has value
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_2000, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::HasParam) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_2000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_2100
 * @tc.name      : WantParams.HasParam
 * @tc.desc      : not empty key on empty WantParams
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_2100, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::HasParam) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_2100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_2200
 * @tc.name      : WantParams.HasParam
 * @tc.desc      : not empty key has a value
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_2200, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::HasParam) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_2200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_2300
 * @tc.name      : WantParams.HasParam
 * @tc.desc      : key with special character has a value
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_2300, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::HasParam) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_2300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_2400
 * @tc.name      : WantParams.IsEmpty
 * @tc.desc      : empty WantParams
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_2400, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::IsEmpty) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_2400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_2500
 * @tc.name      : WantParams.IsEmpty
 * @tc.desc      : WantParams with params
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_2500, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::IsEmpty) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_2500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_2600
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall string param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_2600, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_2600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_2700
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall bool param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_2700, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_2700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_2800
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall byte param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_2800, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_2800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_2900
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall char param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_2900, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_3";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_2900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_3000
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall short param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_3000, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_4";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_3000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_3100
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall int param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_3100, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_5";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_3100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_3200
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall long param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_3200, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_6";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_3200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_3300
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall float param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_3300, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_7";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_3300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_3400
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall double param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_3400, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_8";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_3400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_3500
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall string array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_3500, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_9";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_3500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_3600
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall bool array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_3600, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_10";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_3600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_3700
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall byte array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_3700, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_11";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_3700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_3800
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall char array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_3800, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_12";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_3800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_3900
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall short array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_3900, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_13";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_3900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_4000
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall int array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_4000, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_14";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_4000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_4100
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall long array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_4100, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_15";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_4100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_4200
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall float array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_4200, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_16";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_4200 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_4300
 * @tc.name      : WantParams.Marshalling
 * @tc.desc      : marshall and unmarshall double array param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_4300, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Marshalling) + "_17";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_4300 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_4400
 * @tc.name      : WantParams.Size
 * @tc.desc      : no any param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_4400, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Size) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_4400 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_4500
 * @tc.name      : WantParams.Size
 * @tc.desc      : only one param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_4500, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Size) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_4500 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_4600
 * @tc.name      : WantParams.Size
 * @tc.desc      : multiple params
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_4600, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Size) + "_2";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_4600 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_4700
 * @tc.name      : WantParams.KeySet
 * @tc.desc      : no any key
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_4700, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::KeySet) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_4700 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_4800
 * @tc.name      : WantParams.KeySet
 * @tc.desc      : multiple keys
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_4800, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::KeySet) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_4800 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_4900
 * @tc.name      : WantParams.WantParamsCopy
 * @tc.desc      : copy empty WantParams
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_4900, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::WantParamsCopy) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_4900 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_5000
 * @tc.name      : WantParams.WantParamsCopy
 * @tc.desc      : copy WantParams with multiple param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_5000, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::WantParamsCopy) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_5000 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_5100
 * @tc.name      : WantParams.Remove
 * @tc.desc      : remove not existed param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_5100, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Remove) + "_0";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_5100 : " << i;
            break;
        }
    }
}

/**
 * @tc.number    : AMS_Page_WantParams_5200
 * @tc.name      : WantParams.Remove
 * @tc.desc      : remove existed param
 */
HWTEST_F(ActsAmsKitProcessInfoTest, AMS_Page_WantParams_5200, Function | MediumTest | Level1)
{
    StartFifthAbility();
    std::string data = "WantParams_" + std::to_string((int)WantParamsApi::Remove) + "_1";
    bool result = false;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIFTH, ++amsKitSystemTestFifthAbilityCode, data);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode, delay));
        data = STAbilityUtil::GetData(event, g_EVENT_RESP_FIFTH, amsKitSystemTestFifthAbilityCode);
        result = data.compare("1") == 0;
        EXPECT_TRUE(result);
        if (!result) {
            GTEST_LOG_(INFO) << "AMS_Page_WantParams_5200 : " << i;
            break;
        }
    }
}