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

static const std::string kitPageBundleName = "com.ohos.amsst.page.AppKitA";

static const std::string ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS = "LifecycleCallbacksAbility";
static const std::string ABILTIY_NAME_ABILITY_CONTEXT = "AbilityContextAbility";
static const std::string ABILTIY_NAME_ABILITY = "AbilityAbility";
static const std::string ABILTIY_NAME_ABILITY_LIFE_CYCLE = "AbilityLifeCycleAbility";
static const std::string ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER = "LifeCycleObserverAbility";

static const std::string EVENT_RESP_LIFECYCLE_CALLBACK =
    "resp_com_ohos_amsst_appkit_service_ability_lifecycle_callbacks";
static const std::string EVENT_RESP_LIFECYCLE_OBSERVER = "resp_com_ohos_amsst_appkit_service_lifecycle_observer";
static const std::string EVENT_RESP_FIRST_ABILITY = "resp_com_ohos_amsst_appkit_service_mainabilitydemo";
static const std::string EVENT_RESP_SECOND_ABILITY = "resp_com_ohos_amsst_appkit_service_secondability";
static const std::string EVENT_RESP_KITTEST_COMPLETE = "resp_com_ohos_amsst_appkit_service_kittest_completed";
static const std::string EVENT_REQU_KITTEST = "requ_com_ohos_amsst_appkit_service_kittest";
static const std::string EVENT_RESP_SECOND_ABILITY_CONTEXT =
    "resp_com_ohos_amsst_appkit_service_secondability_ability_context";
static const std::string EVENT_REQU_KITTEST_SECOND = "resp_com_ohos_amsst_appkit_service_secondability_kittest";

static const std::string APP_LIFE_CYCLE_CALL_BACKS_RESP_EVENT_NAME =
    "resp_com_ohos_amsst_service_app_life_cycle_call_backs";
static const std::string APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME =
    "req_com_ohos_amsst_service_app_life_cycle_call_backs";

static const std::string APP_ABILITY_CONTEXT_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_ability_context";
static const std::string APP_ABILITY_CONTEXT_REQ_EVENT_NAME = "req_com_ohos_amsst_service_app_ability_context";

static const std::string APP_ABILITY_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_ability";
static const std::string APP_ABILITY_REQ_EVENT_NAME = "req_com_ohos_amsst_service_app_ability";

static const std::string APP_ABILITY_CONNECTION_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_ability_connection";
static const std::string APP_ABILITY_CONNECTION_REQ_EVENT_NAME = "req_com_ohos_amsst_service_app_ability_connection";

static const std::string APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME = "resp_com_ohos_amsst_service_app_ability_life_cycle";
static const std::string APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME = "req_com_ohos_amsst_service_app_ability_life_cycle";

static const std::string APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME =
    "resp_com_ohos_amsst_service_app_life_cycle_observer";
static const std::string APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME = "req_com_ohos_amsst_service_app_life_cycle_observer";

static const std::string EVENT_RESP_ON_ABILITY_RESULT = "resp_com_ohos_amsst_appkit_mainabilitydemo_onabilityresult";

std::vector<std::string> bundleNameList = {
    "com.ohos.amsst.page.AppKitA",
    "com.ohos.amsst.service.appA",
    "com.ohos.amsst.service.AppKit",
};
std::vector<std::string> hapNameList = {
    "amsKitSystemTestPageA",
    "amsSystemTestServiceA",
    "amsKitSystemTestService",
};
constexpr int WAIT_TIME = 1000;
constexpr int DELAY_TIME = 5;
constexpr int WAIT_ABILITY_OK = 5 * 1000;
}  // namespace

class ActsAmsKitLifecycleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static bool SubscribeEvent();
    class AppEventSubscriber : public CommonEventSubscriber {
    public:
        explicit AppEventSubscriber(const CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp){};
        virtual void OnReceiveEvent(const CommonEventData &data) override;
        ~AppEventSubscriber() = default;
    };

    static sptr<IAppMgr> appMs;
    static sptr<IAbilityManager> abilityMs;
    static Event event;
    static std::unordered_map<std::string, int> mapState;
    static std::vector<std::string> eventList;
    static std::shared_ptr<AppEventSubscriber> subscriber;

    static StressTestLevel stLevel_;
};

StressTestLevel ActsAmsKitLifecycleTest::stLevel_ = {};

std::vector<std::string> ActsAmsKitLifecycleTest::eventList = {
    EVENT_RESP_LIFECYCLE_CALLBACK,
    EVENT_RESP_LIFECYCLE_OBSERVER,
    EVENT_RESP_FIRST_ABILITY,
    EVENT_RESP_KITTEST_COMPLETE,
    EVENT_RESP_SECOND_ABILITY_CONTEXT,
    APP_LIFE_CYCLE_CALL_BACKS_RESP_EVENT_NAME,
    APP_ABILITY_CONTEXT_RESP_EVENT_NAME,
    APP_ABILITY_RESP_EVENT_NAME,
    APP_ABILITY_CONNECTION_RESP_EVENT_NAME,
    APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME,
    APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME,
};

Event ActsAmsKitLifecycleTest::event = Event();
sptr<IAppMgr> ActsAmsKitLifecycleTest::appMs = nullptr;
sptr<IAbilityManager> ActsAmsKitLifecycleTest::abilityMs = nullptr;

std::shared_ptr<ActsAmsKitLifecycleTest::AppEventSubscriber> ActsAmsKitLifecycleTest::subscriber = nullptr;

std::unordered_map<std::string, int> ActsAmsKitLifecycleTest::mapState = {
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

void ActsAmsKitLifecycleTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "OnReceiveEvent: event=" << data.GetWant().GetAction();
    GTEST_LOG_(INFO) << "OnReceiveEvent: data=" << data.GetData();
    GTEST_LOG_(INFO) << "OnReceiveEvent: code=" << data.GetCode();

    auto eventName = data.GetWant().GetAction();
    auto iter = std::find(eventList.begin(), eventList.end(), eventName);
    if (iter != eventList.end()) {
        STAbilityUtil::Completed(event, data.GetData(), data.GetCode());
    }
}

void ActsAmsKitLifecycleTest::SetUpTestCase(void)
{
    TestConfigParser tcp;
    stLevel_.AMSLevel = 1;
    tcp.ParseFromFile4StressTest(STRESS_TEST_CONFIG_FILE_PATH, stLevel_);
    std::cout << "stress test level : "
              << "AMS : " << stLevel_.AMSLevel << " "
              << "BMS : " << stLevel_.BMSLevel << " "
              << "CES : " << stLevel_.CESLevel << std::endl;

    if (!SubscribeEvent()) {
        GTEST_LOG_(INFO) << "SubscribeEvent error";
    }
    usleep(WAIT_TIME);
}

void ActsAmsKitLifecycleTest::TearDownTestCase(void)
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
}

void ActsAmsKitLifecycleTest::SetUp(void)
{
    STAbilityUtil::InstallHaps(hapNameList);
    abilityMs = STAbilityUtil::GetAbilityManagerService();
    appMs = STAbilityUtil::GetAppMgrService();
}

void ActsAmsKitLifecycleTest::TearDown(void)
{
    STAbilityUtil::UninstallBundle(bundleNameList);
    STAbilityUtil::CleanMsg(event);
}

bool ActsAmsKitLifecycleTest::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber = std::make_shared<AppEventSubscriber>(subscribeInfo);
    return CommonEventManager::SubscribeCommonEvent(subscriber);
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_00100
 * @tc.name      : Test the OnAbilityStart event in the AbilityLifecycleCallbacks class
 * @tc.desc      : No processing is done in the OnAbilityStart event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_00100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00100 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00100 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00100 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_00200
 * @tc.name      : Test the OnAbilityStart event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start a task in the OnAbilityStart event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_00200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00200 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00200 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00200 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_00300
 * @tc.name      : Test the OnAbilityStart event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start the while loop in the OnAbilityStart event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_00300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00300 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00300 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00300 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_00400
 * @tc.name      : Test the OnAbilityStart event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_00400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00400 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00400 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00400 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_00500
 * @tc.name      : Test the OnAbilityStart event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_00500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00500 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00500 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00500 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_00600
 * @tc.name      : Test the OnAbilityActive event in the AbilityLifecycleCallbacks class
 * @tc.desc      : No processing is done in the OnAbilityActive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_00600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00600 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00600 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00600 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_00700
 * @tc.name      : Test the OnAbilityActive event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start a task in the OnAbilityActive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_00700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00700 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00700 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00700 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_00800
 * @tc.name      : Test the OnAbilityActive event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start the while loop in the OnAbilityActive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_00800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00800 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00800 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00800 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_00900
 * @tc.name      : Test the OnAbilityActive event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_00900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00900 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00900 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_00900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_00900 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_01000
 * @tc.name      : Test the OnAbilityActive event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_01000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01000 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01000 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01000 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_01100
 * @tc.name      : Test the OnAbilityInactive event in the AbilityLifecycleCallbacks class
 * @tc.desc      : No processing is done in the OnAbilityInactive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_01100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01100 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityInactive", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01100 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01100 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_01200
 * @tc.name      : Test the OnAbilityInactive event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start a task in the OnAbilityInactive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_01200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01200 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityInactive", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01200 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01200 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_01300
 * @tc.name      : Test the OnAbilityInactive event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start the while loop in the OnAbilityInactive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_01300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01300 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityInactive", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01300 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01300 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_01400
 * @tc.name      : Test the OnAbilityInactive event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_01400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01400 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityActive", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityInactive", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01400 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01400 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_01500
 * @tc.name      : Test the OnAbilityInactive event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_01500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01500 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityInactive", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01500 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01500 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_01600
 * @tc.name      : Test the OnAbilityForeground event in the AbilityLifecycleCallbacks class
 * @tc.desc      : No processing is done in the OnAbilityForeground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_01600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01600 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityForeground", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01600 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01600 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_01700
 * @tc.name      : Test the OnAbilityForeground event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start a task in the OnAbilityForeground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_01700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01700 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityForeground", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01700 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01700 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_01800
 * @tc.name      : Test the OnAbilityForeground event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start the while loop in the OnAbilityForeground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_01800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01800 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityForeground", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01800 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01800 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_01900
 * @tc.name      : Test the OnAbilityForeground event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_01900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01900 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityForeground", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01900 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_01900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_01900 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_02000
 * @tc.name      : Test the OnAbilityForeground event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_02000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02000 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityForeground", 0, DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02000 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02000 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_02100
 * @tc.name      : Test the OnAbilityStop event in the AbilityLifecycleCallbacks class
 * @tc.desc      : No processing is done in the OnAbilityStop event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_02100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02100 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityStop", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02100 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02100 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_02200
 * @tc.name      : Test the OnAbilityStop event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start a task in the OnAbilityStop event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_02200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02200 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityStop", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02200 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02200 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_02300
 * @tc.name      : Test the OnAbilityStop event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start the while loop in the OnAbilityStop event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_02300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02300 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityStop", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02300 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02300 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_02400
 * @tc.name      : Test the OnAbilityStop event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_02400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02400 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityStop", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02400 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02400 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_02500
 * @tc.name      : Test the OnAbilityStop event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_02500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02500 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityStop", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02500 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02500 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_02600
 * @tc.name      : Test the OnAbilitySaveState event in the AbilityLifecycleCallbacks class
 * @tc.desc      : No processing is done in the OnAbilitySaveState event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_02600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02600 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilitySaveState", 0, DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02600 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02600 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_02700
 * @tc.name      : Test the OnAbilitySaveState event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start a task in the OnAbilitySaveState event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_02700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02700 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilitySaveState", 0, DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02700 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02700 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_02800
 * @tc.name      : Test the OnAbilitySaveState event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start the while loop in the OnAbilitySaveState event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_02800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02800 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilitySaveState", 0, DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02800 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02800 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_02900
 * @tc.name      : Test the OnAbilitySaveState event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_02900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02900 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilitySaveState", 0, DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02900 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_02900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_02900 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_03000
 * @tc.name      : Test the OnAbilitySaveState event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_03000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_03000 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilitySaveState", 0, DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_03000 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_03000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_03000 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_03100
 * @tc.name      : Test the OnAbilityBackground event in the AbilityLifecycleCallbacks class
 * @tc.desc      : No processing is done in the OnAbilityBackground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_03100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_03100 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityBackground", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_03100 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_03100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_03100 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_03200
 * @tc.name      : Test the OnAbilityBackground event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start a task in the OnAbilityBackground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_03200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_03200 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityBackground", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_03200 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_03200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_03200 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_03300
 * @tc.name      : Test the OnAbilityBackground event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Start the while loop in the OnAbilityBackground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_03300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_03300 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityBackground", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_03300 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_03300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_03300 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_03400
 * @tc.name      : Test the OnAbilityBackground event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_03400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_03400 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityBackground", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_03400 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_03400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_03400 end";
}

/**
 * @tc.number    : AMS_Page_LifeCycleCallbacks_03500
 * @tc.name      : Test the OnAbilityBackground event in the AbilityLifecycleCallbacks class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifeCycleCallbacks_03500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_03500 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_LIFE_CYCLE_CALL_BACKS, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnAbilityStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_CALL_BACKS_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityBackground", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_03500 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifeCycleCallbacks_03500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifeCycleCallbacks_03500 end";
}

/**
 * @tc.number    : AMS_Page_Ability_03600
 * @tc.name      : Test the TerminateAbility method in the AbilityContext class
 * @tc.desc      : Ability to terminate yourself.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_03600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_03600 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_CONTEXT, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_CONTEXT_REQ_EVENT_NAME, 1, "TerminateAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "TestTerminateAbility", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_03600 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_03600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_03600 end";
}

/**
 * @tc.number    : AMS_Page_Ability_03700
 * @tc.name      : Test the TerminateAbility method in the AbilityContext class
 * @tc.desc      : Terminate other services in the same application.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_03700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_03700 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_CONTEXT, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_CONTEXT_REQ_EVENT_NAME, 1, "TerminateAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "TestTerminateAbility", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_03700 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_03700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_03700 end";
}

/**
 * @tc.number    : AMS_Page_Ability_03800
 * @tc.name      : Test the TerminateAbility method in the AbilityContext class
 * @tc.desc      : Terminate services in another application.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_03800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_03800 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_CONTEXT, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_CONTEXT_REQ_EVENT_NAME, 1, "TerminateAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "TestTerminateAbility", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_03800 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_03800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_03800 end";
}

/**
 * @tc.number    : AMS_Page_Ability_03900
 * @tc.name      : Test the TerminateAbility method in the AbilityContext class
 * @tc.desc      : Terminate the same page in the same application multiple times.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_03900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_03900 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_CONTEXT, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_CONTEXT_REQ_EVENT_NAME, 1, "TerminateAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "TestTerminateAbility", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_03900 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_03900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_03900 end";
}

/**
 * @tc.number    : AMS_Page_Ability_04000
 * @tc.name      : Test the TerminateAbility method in the AbilityContext class
 * @tc.desc      : Terminate a non-existent page.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_04000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04000 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_CONTEXT, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_CONTEXT_REQ_EVENT_NAME, 1, "TerminateAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "TestTerminateAbility", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_04000 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_04000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04000 end";
}

/**
 * @tc.number    : AMS_Page_Ability_04100
 * @tc.name      : Test the OnAbilityResult event in the Ability class
 * @tc.desc      : No processing is done in the OnAbilityResult event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_04100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04100 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityForResult");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStartAbilityForResult", 1, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityResult", mapState["OnAbilityResult"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_04100 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_04100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04100 end";
}

/**
 * @tc.number    : AMS_Page_Ability_04200
 * @tc.name      : Test the OnAbilityResult event in the Ability class
 * @tc.desc      : Start a task in the OnAbilityResult event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_04200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04200 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityForResult");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStartAbilityForResult", 1, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityResult", mapState["OnAbilityResult"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_04200 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_04200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04200 end";
}

/**
 * @tc.number    : AMS_Page_Ability_04300
 * @tc.name      : Test the OnAbilityResult event in the Ability class
 * @tc.desc      : Start the while loop in the OnAbilityResult event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_04300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04300 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityForResult");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStartAbilityForResult", 1, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityResult", mapState["OnAbilityResult"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_04300 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_04300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04300 end";
}

/**
 * @tc.number    : AMS_Page_Ability_04400
 * @tc.name      : Test the OnAbilityResult event in the Ability class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_04400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04400 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityForResult");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStartAbilityForResult", 1, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityResult", mapState["OnAbilityResult"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_04400 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_04400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04400 end";
}

/**
 * @tc.number    : AMS_Page_Ability_04500
 * @tc.name      : Test the OnAbilityResult event in the Ability class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_04500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04500 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityForResult");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStartAbilityForResult", 1, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityResult", mapState["OnAbilityResult"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_04500 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_04500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04500 end";
}

/**
 * @tc.number    : AMS_Page_Ability_04600
 * @tc.name      : Test the OnStart event in the Ability class
 * @tc.desc      : No processing is done in the OnStart event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_04600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04600 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        int ret = STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_04600 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_04600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04600 end";
}

/**
 * @tc.number    : AMS_Page_Ability_04700
 * @tc.name      : Test the OnStart event in the Ability class
 * @tc.desc      : Start a task in the OnStart event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_04700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04700 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        int ret = STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_04700 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_04700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04700 end";
}

/**
 * @tc.number    : AMS_Page_Ability_04800
 * @tc.name      : Test the OnStart event in the Ability class
 * @tc.desc      : Start the while loop in the OnStart event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_04800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04800 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        int ret = STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_04800 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_04800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04800 end";
}

/**
 * @tc.number    : AMS_Page_Ability_04900
 * @tc.name      : Test the OnStart event in the Ability class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_04900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04900 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        int ret = STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_04900 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_04900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_04900 end";
}

/**
 * @tc.number    : AMS_Page_Ability_05000
 * @tc.name      : Test the OnStart event in the Ability class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_05000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05000 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        int ret = STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_05000 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_05000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05000 end";
}

/**
 * @tc.number    : AMS_Page_Ability_05100
 * @tc.name      : Test the OnStop event in the Ability class
 * @tc.desc      : No processing is done in the OnStop event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_05100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05100 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStopAbility", 0, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_05100 : " << i;
            break;
        }

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_05100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05100 end";
}

/**
 * @tc.number    : AMS_Page_Ability_05200
 * @tc.name      : Test the OnStop event in the Ability class
 * @tc.desc      : Start a task in the OnStop event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_05200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05200 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStopAbility", 0, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_05200 : " << i;
            break;
        }

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_05200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05200 end";
}

/**
 * @tc.number    : AMS_Page_Ability_05300
 * @tc.name      : Test the OnStop event in the Ability class
 * @tc.desc      : Start the while loop in the OnStop event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_05300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05300 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStopAbility", 0, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_05300 : " << i;
            break;
        }

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_05300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05300 end";
}

/**
 * @tc.number    : AMS_Page_Ability_05400
 * @tc.name      : Test the OnStop event in the Ability class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_05400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05400 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStopAbility", 0, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_05400 : " << i;
            break;
        }

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_05400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05400 end";
}

/**
 * @tc.number    : AMS_Page_Ability_05500
 * @tc.name      : Test the OnStop event in the Ability class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_05500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05500 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStopAbility", 0, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_05500 : " << i;
            break;
        }

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_05500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05500 end";
}

/**
 * @tc.number    : AMS_Page_Ability_05600
 * @tc.name      : Test the OnNewIntent event in the Ability class
 * @tc.desc      : No processing is done in the OnNewIntent event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_05600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05600 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnNewIntent", mapState["OnNewIntent"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_05600 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_05600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05600 end";
}

/**
 * @tc.number    : AMS_Page_Ability_05700
 * @tc.name      : Test the OnNewIntent event in the Ability class
 * @tc.desc      : Start a task in the OnNewIntent event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_05700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05700 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnNewIntent", mapState["OnNewIntent"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_05700 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_05700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05700 end";
}

/**
 * @tc.number    : AMS_Page_Ability_05800
 * @tc.name      : Test the OnNewIntent event in the Ability class
 * @tc.desc      : Start the while loop in the OnNewIntent event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_05800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05800 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnNewIntent", mapState["OnNewIntent"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_05800 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_05800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05800 end";
}

/**
 * @tc.number    : AMS_Page_Ability_05900
 * @tc.name      : Test the OnNewIntent event in the Ability class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_05900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05900 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnNewIntent", mapState["OnNewIntent"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_05900 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_05900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_05900 end";
}

/**
 * @tc.number    : AMS_Page_Ability_06000
 * @tc.name      : Test the OnNewIntent event in the Ability class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_06000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06000 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnNewIntent", mapState["OnNewIntent"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_06000 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_06000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06000 end";
}

/**
 * @tc.number    : AMS_Page_Ability_06100
 * @tc.name      : Test the OnInactive event in the Ability class
 * @tc.desc      : No processing is done in the OnInactive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_06100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06100 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_06100 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_06100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06100 end";
}

/**
 * @tc.number    : AMS_Page_Ability_06200
 * @tc.name      : Test the OnInactive event in the Ability class
 * @tc.desc      : Start a task in the OnInactive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_06200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06200 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_06200 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_06200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06200 end";
}

/**
 * @tc.number    : AMS_Page_Ability_06300
 * @tc.name      : Test the OnInactive event in the Ability class
 * @tc.desc      : Start the while loop in the OnInactive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_06300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06300 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_06300 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_06300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06300 end";
}

/**
 * @tc.number    : AMS_Page_Ability_06400
 * @tc.name      : Test the OnInactive event in the Ability class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_06400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06400 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_06400 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_06400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06400 end";
}

/**
 * @tc.number    : AMS_Page_Ability_06500
 * @tc.name      : Test the OnInactive event in the Ability class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_06500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06500 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_06500 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_06500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06500 end";
}

/**
 * @tc.number    : AMS_Page_Ability_06600
 * @tc.name      : Test the OnActive event in the Ability class
 * @tc.desc      : No processing is done in the OnActive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_06600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06600 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_06600 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_06600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06600 end";
}

/**
 * @tc.number    : AMS_Page_Ability_06700
 * @tc.name      : Test the OnActive event in the Ability class
 * @tc.desc      : Start a task in the OnActive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_06700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06700 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_06700 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_06700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06700 end";
}

/**
 * @tc.number    : AMS_Page_Ability_06800
 * @tc.name      : Test the OnActive event in the Ability class
 * @tc.desc      : Start the while loop in the OnActive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_06800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06800 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_06800 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_06800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06800 end";
}

/**
 * @tc.number    : AMS_Page_Ability_06900
 * @tc.name      : Test the OnActive event in the Ability class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_06900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06900 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_06900 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_06900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_06900 end";
}

/**
 * @tc.number    : AMS_Page_Ability_07000
 * @tc.name      : Test the OnActive event in the Ability class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_07000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07000 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_07000 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_07000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07000 end";
}

/**
 * @tc.number    : AMS_Page_Ability_07100
 * @tc.name      : Test the OnForeground event in the Ability class
 * @tc.desc      : No processing is done in the OnForeground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_07100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07100 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnForeground", mapState["OnForeground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_07100 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_07100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07100 end";
}

/**
 * @tc.number    : AMS_Page_Ability_07200
 * @tc.name      : Test the OnForeground event in the Ability class
 * @tc.desc      : Start a task in the OnForeground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_07200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07200 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnForeground", mapState["OnForeground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_07200 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_07200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07200 end";
}

/**
 * @tc.number    : AMS_Page_Ability_07300
 * @tc.name      : Test the OnForeground event in the Ability class
 * @tc.desc      : Start the while loop in the OnForeground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_07300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07300 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnForeground", mapState["OnForeground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_07300 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_07300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07300 end";
}

/**
 * @tc.number    : AMS_Page_Ability_07400
 * @tc.name      : Test the OnForeground event in the Ability class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_07400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07400 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnForeground", mapState["OnForeground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_07400 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_07400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07400 end";
}

/**
 * @tc.number    : AMS_Page_Ability_07500
 * @tc.name      : Test the OnForeground event in the Ability class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_07500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07500 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnForeground", mapState["OnForeground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_07500 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_07500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07500 end";
}

/**
 * @tc.number    : AMS_Page_Ability_07600
 * @tc.name      : Test the OnBackground event in the Ability class
 * @tc.desc      : No processing is done in the OnBackground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_07600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07600 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_07600 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_07600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07600 end";
}

/**
 * @tc.number    : AMS_Page_Ability_07700
 * @tc.name      : Test the OnBackground event in the Ability class
 * @tc.desc      : Start a task in the OnBackground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_07700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07700 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_07700 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_07700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07700 end";
}

/**
 * @tc.number    : AMS_Page_Ability_07800
 * @tc.name      : Test the OnBackground event in the Ability class
 * @tc.desc      : Start the while loop in the OnBackground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_07800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07800 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_07800 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_07800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07800 end";
}

/**
 * @tc.number    : AMS_Page_Ability_07900
 * @tc.name      : Test the OnBackground event in the Ability class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_07900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07900 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_07900 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_07900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_07900 end";
}

/**
 * @tc.number    : AMS_Page_Ability_08000
 * @tc.name      : Test the OnBackground event in the Ability class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_08000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08000 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_08000 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_08000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08000 end";
}

/**
 * @tc.number    : AMS_Page_Ability_08100
 * @tc.name      : Test the OnAbilityResult event in the Ability class
 * @tc.desc      : No processing is done in the OnAbilityResult event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_08100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08100 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityForResult");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStartAbilityForResult", 1, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityResult", mapState["OnAbilityResult"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_08100 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_08100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08100 end";
}

/**
 * @tc.number    : AMS_Page_Ability_08200
 * @tc.name      : Test the OnAbilityResult event in the Ability class
 * @tc.desc      : Start a task in the OnAbilityResult event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_08200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08200 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityForResult");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStartAbilityForResult", 1, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityResult", mapState["OnAbilityResult"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_08200 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_08200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08200 end";
}

/**
 * @tc.number    : AMS_Page_Ability_08300
 * @tc.name      : Test the OnAbilityResult event in the Ability class
 * @tc.desc      : Start the while loop in the OnAbilityResult event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_08300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08300 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityForResult");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStartAbilityForResult", 1, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityResult", mapState["OnAbilityResult"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_08300 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_08300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08300 end";
}

/**
 * @tc.number    : AMS_Page_Ability_08400
 * @tc.name      : Test the OnAbilityResult event in the Ability class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_08400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08400 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityForResult");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStartAbilityForResult", 1, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityResult", mapState["OnAbilityResult"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_08400 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_08400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08400 end";
}

/**
 * @tc.number    : AMS_Page_Ability_08500
 * @tc.name      : Test the OnAbilityResult event in the Ability class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_08500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08500 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityForResult");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestStartAbilityForResult", 1, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnAbilityResult", mapState["OnAbilityResult"], DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_08500 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_08500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08500 end";
}

/**
 * @tc.number    : AMS_Page_Ability_08600
 * @tc.name      : Test the StartAbility method in the Ability class
 * @tc.desc      : No processing is done in the StartAbility event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_08600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08600 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "AbilityStartAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "TestAbilityStartAbility", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_08600 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_08600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08600 end";
}

/**
 * @tc.number    : AMS_Page_Ability_08700
 * @tc.name      : Test the StartAbility method in the Ability class
 * @tc.desc      : Start a task in the StartAbility event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_08700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08700 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "AbilityStartAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "TestAbilityStartAbility", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_08700 : " << i;
            break;
        }
        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_08700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08700 end";
}

/**
 * @tc.number    : AMS_Page_Ability_08800
 * @tc.name      : Test the StartAbility method in the Ability class
 * @tc.desc      : Start the while loop in the StartAbility event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_08800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08800 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "AbilityStartAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "TestAbilityStartAbility", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_08800 : " << i;
            break;
        }
        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_08800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08800 end";
}

/**
 * @tc.number    : AMS_Page_Ability_08900
 * @tc.name      : Test the StartAbility method in the Ability class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_08900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08900 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "AbilityStartAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "TestAbilityStartAbility", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_08900 : " << i;
            break;
        }
        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_08900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_08900 end";
}

/**
 * @tc.number    : AMS_Page_Ability_09000
 * @tc.name      : Test the StartAbility method in the Ability class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_09000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09000 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "AbilityStartAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "TestAbilityStartAbility", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_09000 : " << i;
            break;
        }
        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_09000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09000 end";
}

/**
 * @tc.number    : AMS_Page_Ability_09100
 * @tc.name      : Test the StartAbility method with AbilityStartSetting in the Ability class
 * @tc.desc      : Ability to start yourself.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_09100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09100 start";

    MAP_STR_STR params;

    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityWithSetting");
        int ret = STAbilityUtil::WaitCompleted(event, "TestStartAbilityWithSetting", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_09100 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_09100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09100 end";
}

/**
 * @tc.number    : AMS_Page_Ability_09200
 * @tc.name      : Test the StartAbility method with AbilityStartSetting in the Ability class
 * @tc.desc      : Start other services in the same application.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_09200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09200 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityWithSetting");
        int ret = STAbilityUtil::WaitCompleted(event, "TestStartAbilityWithSetting", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_09200 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_09200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09200 end";
}

/**
 * @tc.number    : AMS_Page_Ability_09300
 * @tc.name      : Test the StartAbility method with AbilityStartSetting in the Ability class
 * @tc.desc      : Start services in another application.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_09300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09300 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityWithSetting");
        int ret = STAbilityUtil::WaitCompleted(event, "TestStartAbilityWithSetting", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_09300 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_09300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09300 end";
}

/**
 * @tc.number    : AMS_Page_Ability_09400
 * @tc.name      : Test the StartAbility method with AbilityStartSetting in the Ability class
 * @tc.desc      : Start the same page in the same application multiple times.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_09400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09400 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityWithSetting");
        int ret = STAbilityUtil::WaitCompleted(event, "TestStartAbilityWithSetting", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_09400 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_09400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09400 end";
}

/**
 * @tc.number    : AMS_Page_Ability_09500
 * @tc.name      : Test the StartAbility method with AbilityStartSetting in the Ability class
 * @tc.desc      : Start a non-existent page.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_09500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09500 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StartAbilityWithSetting");
        int ret = STAbilityUtil::WaitCompleted(event, "TestStartAbilityWithSetting", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_09500 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_09500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09500 end";
}

/**
 * @tc.number    : AMS_Page_Ability_09600
 * @tc.name      : Test the GetLifecycle method in the Lifecycle class
 * @tc.desc      : Get life cycle object.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_09600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09600 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleGetLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_09600 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_09600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09600 end";
}

/**
 * @tc.number    : AMS_Page_Ability_09700
 * @tc.name      : Test the GetLifecycle method in the Lifecycle class
 * @tc.desc      : Get the lifecycle object multiple times.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_09700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09700 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleGetLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_09700 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_09700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09700 end";
}

/**
 * @tc.number    : AMS_Page_Ability_09800
 * @tc.name      : Test the GetLifecycle method in the Lifecycle class
 * @tc.desc      : Call GetLifecycleState.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_09800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09800 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleGetLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycle", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_09800 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_09800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09800 end";
}

/**
 * @tc.number    : AMS_Page_Ability_09900
 * @tc.name      : Test the GetLifecycle method in the Lifecycle class
 * @tc.desc      : Call AddObserver.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_09900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09900 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleGetLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_09900 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_09900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_09900 end";
}

/**
 * @tc.number    : AMS_Page_Ability_10000
 * @tc.name      : Test the GetLifecycle method in the Lifecycle class
 * @tc.desc      : Call DispatchLifecycle.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_10000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10000 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleGetLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_10000 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_10000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10000 end";
}

/**
 * @tc.number    : AMS_Page_Ability_10100
 * @tc.name      : Test the GetLifecycle method in the Lifecycle class
 * @tc.desc      : Call RemoveObserver.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_10100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10100 start";

    MAP_STR_STR params;
    params["No."] = "6";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleGetLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_10100 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_10100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10100 end";
}

/**
 * @tc.number    : AMS_Page_Ability_10200
 * @tc.name      : Test the GetLifecycleState method in the Lifecycle class
 * @tc.desc      : Called in OnStart function.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_10200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10200 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycleState", 4, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_10200 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_10200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10200 end";
}

/**
 * @tc.number    : AMS_Page_Ability_10300
 * @tc.name      : Test the GetLifecycleState method in the Lifecycle class
 * @tc.desc      : Called in OnActive function.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_10300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10300 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycleState", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_10300 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_10300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10300 end";
}

/**
 * @tc.number    : AMS_Page_Ability_10400
 * @tc.name      : Test the GetLifecycleState method in the Lifecycle class
 * @tc.desc      : Called in OnBackground function.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_10400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10400 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycleState", 3, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_10400 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_10400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10400 end";
}

/**
 * @tc.number    : AMS_Page_Ability_10500
 * @tc.name      : Test the GetLifecycleState method in the Lifecycle class
 * @tc.desc      : Called in Init function.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_10500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10500 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycleState", 6, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_10500 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_10500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10500 end";
}

/**
 * @tc.number    : AMS_Page_Ability_10600
 * @tc.name      : Test the GetLifecycleState method in the Lifecycle class
 * @tc.desc      : Called in OnInactive function.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_10600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10600 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycleState", 6, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_10600 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycleState", 3, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_10600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10600 end";
}

/**
 * @tc.number    : AMS_Page_Ability_10700
 * @tc.name      : Test the GetLifecycleState method in the Lifecycle class
 * @tc.desc      : Called in OnForeground function.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_10700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10700 start";

    MAP_STR_STR params;
    params["No."] = "6";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnForeground", 1, DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycleState", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_10700 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_10700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10700 end";
}

/**
 * @tc.number    : AMS_Page_Ability_10800
 * @tc.name      : Test the GetLifecycleState method in the Lifecycle class
 * @tc.desc      : Called in OnBackground function.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_10800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10800 start";

    MAP_STR_STR params;
    params["No."] = "7";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleGetLifecycleState", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_10800 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_10800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10800 end";
}

/**
 * @tc.number    : AMS_Page_Ability_10900
 * @tc.name      : Test the AddObserver method in the Lifecycle class
 * @tc.desc      : Add an observer object.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_10900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10900 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleAddObserver");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleAddObserver", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_10900 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_10900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_10900 end";
}

/**
 * @tc.number    : AMS_Page_Ability_11000
 * @tc.name      : Test the AddObserver method in the Lifecycle class
 * @tc.desc      : Add an observer object multi times.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_11000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11000 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleAddObserver");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleAddObserver", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_11000 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_11000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11000 end";
}

/**
 * @tc.number    : AMS_Page_Ability_11100
 * @tc.name      : Test the AddObserver method in the Lifecycle class
 * @tc.desc      : Add multi observer objects.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_11100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11100 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleAddObserver");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleAddObserver", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_11100 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_11100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11100 end";
}

/**
 * @tc.number    : AMS_Page_Ability_11200
 * @tc.name      : Test the AddObserver method in the Lifecycle class
 * @tc.desc      : Add multi observer objects.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_11200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11200 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleAddObserver");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleAddObserver", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_11200 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_11200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11200 end";
}

/**
 * @tc.number    : AMS_Page_Ability_11300
 * @tc.name      : Test the AddObserver method in the Lifecycle class
 * @tc.desc      : Add multi observer objects.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_11300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11300 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleAddObserver");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleAddObserver", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_11300 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_11300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11300 end";
}

/**
 * @tc.number    : AMS_Page_Ability_11400
 * @tc.name      : Test the DispatchLifecycle method in the Lifecycle class
 * @tc.desc      : DispatchLifecycle ON_ACTIVE.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_11400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11400 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleDispatchLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleDispatchLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_11400 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_11400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11400 end";
}

/**
 * @tc.number    : AMS_Page_Ability_11500
 * @tc.name      : Test the DispatchLifecycle method in the Lifecycle class
 * @tc.desc      : DispatchLifecycle ON_FOREGROUND.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_11500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11500 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleDispatchLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleDispatchLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_11500 : " << i;
            break;
        }
        EXPECT_EQ(-1, STAbilityUtil::WaitCompleted(event, "OnForeground", 0, DELAY_TIME));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_11500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11500 end";
}

/**
 * @tc.number    : AMS_Page_Ability_11600
 * @tc.name      : Test the DispatchLifecycle method in the Lifecycle class
 * @tc.desc      : DispatchLifecycle ON_BACKGROUND.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_11600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11600 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleDispatchLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleDispatchLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_11600 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", 0, DELAY_TIME));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_11600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11600 end";
}

/**
 * @tc.number    : AMS_Page_Ability_11700
 * @tc.name      : Test the DispatchLifecycle method in the Lifecycle class
 * @tc.desc      : DispatchLifecycle ON_INACTIVE.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_11700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11700 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleDispatchLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleDispatchLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_11700 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", 0, DELAY_TIME));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_11700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11700 end";
}

/**
 * @tc.number    : AMS_Page_Ability_11800
 * @tc.name      : Test the DispatchLifecycle method in the Lifecycle class
 * @tc.desc      : DispatchLifecycle ON_START.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_11800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11800 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleDispatchLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleDispatchLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_11800 : " << i;
            break;
        }
        EXPECT_EQ(-1, STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_11800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11800 end";
}

/**
 * @tc.number    : AMS_Page_Ability_11900
 * @tc.name      : Test the DispatchLifecycle method in the Lifecycle class
 * @tc.desc      : DispatchLifecycle ON_STOP.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_11900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11900 start";

    MAP_STR_STR params;
    params["No."] = "6";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleDispatchLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleDispatchLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_11900 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", 0, DELAY_TIME));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_11900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_11900 end";
}

/**
 * @tc.number    : AMS_Page_Ability_12000
 * @tc.name      : Test the DispatchLifecycle method in the Lifecycle class
 * @tc.desc      : DispatchLifecycle UNDEFINED.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_12000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12000 start";

    MAP_STR_STR params;
    params["No."] = "7";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleDispatchLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleDispatchLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_12000 : " << i;
            break;
        }
        EXPECT_EQ(-1, STAbilityUtil::WaitCompleted(event, "UnDefine", 0, DELAY_TIME));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_12000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12000 end";
}

/**
 * @tc.number    : AMS_Page_Ability_12100
 * @tc.name      : Test the DispatchLifecycle method in the Lifecycle class
 * @tc.desc      : DispatchLifecycle ON_ACTIVE.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_12100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12100 start";

    MAP_STR_STR params;
    params["No."] = "8";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleDispatchLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleDispatchLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_12100 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_12100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12100 end";
}

/**
 * @tc.number    : AMS_Page_Ability_12200
 * @tc.name      : Test the DispatchLifecycle method in the Lifecycle class
 * @tc.desc      : DispatchLifecycle UNDEFINED.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_12200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12200 start";

    MAP_STR_STR params;
    params["No."] = "9";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleDispatchLifecycle");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleDispatchLifecycle", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_12200 : " << i;
            break;
        }
        EXPECT_EQ(-1, STAbilityUtil::WaitCompleted(event, "UnDefine", 0, DELAY_TIME));

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_12200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12200 end";
}

/**
 * @tc.number    : AMS_Page_Ability_12300
 * @tc.name      : Test the RemoveObserver method in the Lifecycle class
 * @tc.desc      : Remove a non-existent observer.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_12300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12300 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleRemoveObserver");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleRemoveObserver", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_12300 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_12300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12300 end";
}

/**
 * @tc.number    : AMS_Page_Ability_12400
 * @tc.name      : Test the RemoveObserver method in the Lifecycle class
 * @tc.desc      : Remove the same observer multiple times.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_12400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12400 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleRemoveObserver");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleRemoveObserver", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_12400 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_12400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12400 end";
}

/**
 * @tc.number    : AMS_Page_Ability_12500
 * @tc.name      : Test the RemoveObserver method in the Lifecycle class
 * @tc.desc      : Remove different observers multiple times.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_12500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12500 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleRemoveObserver");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleRemoveObserver", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_12500 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_12500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12500 end";
}

/**
 * @tc.number    : AMS_Page_Ability_12600
 * @tc.name      : Test the RemoveObserver method in the Lifecycle class
 * @tc.desc      : Remove different observers multiple times.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_12600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12600 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleRemoveObserver");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleRemoveObserver", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_12600 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_12600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12600 end";
}

/**
 * @tc.number    : AMS_Page_Ability_12700
 * @tc.name      : Test the RemoveObserver method in the Lifecycle class
 * @tc.desc      : Remove different observers multiple times.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_Ability_12700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12700 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want = STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // send message
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "LifeCycleRemoveObserver");
        int ret = STAbilityUtil::WaitCompleted(event, "TestLifeCycleRemoveObserver", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_Ability_12700 : " << i;
            break;
        }

        // stop ability
        STAbilityUtil::PublishEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_Ability_12700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_Ability_12700 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_12800
 * @tc.name      : Test the OnActive event in the LifecycleObserver class
 * @tc.desc      : No processing is done in the OnActive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_12800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_12800 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_12800 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_12800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_12800 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_12900
 * @tc.name      : Test the OnActive event in the LifecycleObserver class
 * @tc.desc      : Start a task in the OnActive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_12900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_12900 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_12900 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_12900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_12900 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_13000
 * @tc.name      : Test the OnActive event in the LifecycleObserver class
 * @tc.desc      : Start the while loop in the OnActive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_13000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13000 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13000 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13000 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_13100
 * @tc.name      : Test the OnActive event in the LifecycleObserver class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_13100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13100 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13100 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13100 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_13200
 * @tc.name      : Test the OnActive event in the LifecycleObserver class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_13200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13200 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13200 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13200 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_13300
 * @tc.name      : Test the OnBackground event in the LifecycleObserver class
 * @tc.desc      : No processing is done in the OnBackground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_13300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13300 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13300 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13300 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_13400
 * @tc.name      : Test the OnBackground event in the LifecycleObserver class
 * @tc.desc      : Start a task in the OnBackground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_13400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13400 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13400 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13400 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_13500
 * @tc.name      : Test the OnBackground event in the LifecycleObserver class
 * @tc.desc      : Start the while loop in the OnBackground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_13500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13500 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13500 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13500 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_13600
 * @tc.name      : Test the OnBackground event in the LifecycleObserver class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_13600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13600 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13600 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13600 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_13700
 * @tc.name      : Test the OnBackground event in the LifecycleObserver class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_13700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13700 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs, WAIT_TIME);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13700 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13700 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_13800
 * @tc.name      : Test the OnForeground event in the LifecycleObserver class
 * @tc.desc      : No processing is done in the OnForeground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_13800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13800 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnForeground", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13800 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13800 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_13900
 * @tc.name      : Test the OnForeground event in the LifecycleObserver class
 * @tc.desc      : Start a task in the OnForeground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_13900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13900 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnForeground", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13900 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_13900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_13900 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_14000
 * @tc.name      : Test the OnForeground event in the LifecycleObserver class
 * @tc.desc      : Start the while loop in the OnForeground event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_14000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14000 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnForeground", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14000 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14000 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_14100
 * @tc.name      : Test the OnForeground event in the LifecycleObserver class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_14100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14100 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnForeground", 1, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14100 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14100 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_14200
 * @tc.name      : Test the OnForeground event in the LifecycleObserver class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_14200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14200 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // simulate ENTITY_HOME
        Want wantEntity;
        wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
        eCode = STAbilityUtil::StartAbility(wantEntity, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnForeground", 1, DELAY_TIME);
        EXPECT_EQ(-1, ret);
        if (ret != -1) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14200 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14200 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_14300
 * @tc.name      : Test the OnInactive event in the LifecycleObserver class
 * @tc.desc      : No processing is done in the OnInactive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_14300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14300 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14300 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14300 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_14400
 * @tc.name      : Test the OnInactive event in the LifecycleObserver class
 * @tc.desc      : Start a task in the OnInactive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_14400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14400 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14400 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14400 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_14500
 * @tc.name      : Test the OnInactive event in the LifecycleObserver class
 * @tc.desc      : Start the while loop in the OnInactive event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_14500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14500 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14500 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14500 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_14600
 * @tc.name      : Test the OnInactive event in the LifecycleObserver class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_14600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14600 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14600 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14600 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_14700
 * @tc.name      : Test the OnInactive event in the LifecycleObserver class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_14700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14700 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        int ret = STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14700 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14700 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_14800
 * @tc.name      : Test the OnStart event in the LifecycleObserver class
 * @tc.desc      : No processing is done in the OnStart event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_14800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14800 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14800 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14800 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_14900
 * @tc.name      : Test the OnStart event in the LifecycleObserver class
 * @tc.desc      : Start a task in the OnStart event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_14900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14900 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14900 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_14900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_14900 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_15000
 * @tc.name      : Test the OnStart event in the LifecycleObserver class
 * @tc.desc      : Start the while loop in the OnStart event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_15000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15000 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15000 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15000 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_15100
 * @tc.name      : Test the OnStart event in the LifecycleObserver class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_15100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15100 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15100 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15100 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_15200
 * @tc.name      : Test the OnStart event in the LifecycleObserver class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_15200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15200 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15200 : " << i;
            break;
        }
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15200 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_15300
 * @tc.name      : Test the OnStop event in the LifecycleObserver class
 * @tc.desc      : No processing is done in the OnStop event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_15300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15300 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15300 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15300 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15300 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_15400
 * @tc.name      : Test the OnStop event in the LifecycleObserver class
 * @tc.desc      : Start a task in the OnStop event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_15400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15400 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15400 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15400 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15400 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_15500
 * @tc.name      : Test the OnStop event in the LifecycleObserver class
 * @tc.desc      : Start the while loop in the OnStop event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_15500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15500 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15500 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15500 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15500 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_15600
 * @tc.name      : Test the OnStop event in the LifecycleObserver class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_15600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15600 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15600 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15600 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15600 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_15700
 * @tc.name      : Test the OnStop event in the LifecycleObserver class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_15700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15700 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        int ret = STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15700 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15700 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15700 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_15800
 * @tc.name      : Test the OnStateChanged event in the LifecycleObserver class
 * @tc.desc      : No processing is done in the OnStateChanged event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_15800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15800 start";

    MAP_STR_STR params;
    params["No."] = "1";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));

        int ret = STAbilityUtil::WaitCompleted(event, "OnStateChanged", mapState["OnStateChanged"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15800 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15800 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15800 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_15900
 * @tc.name      : Test the OnStateChanged event in the LifecycleObserver class
 * @tc.desc      : Start a task in the OnStateChanged event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_15900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15900 start";

    MAP_STR_STR params;
    params["No."] = "2";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));

        int ret = STAbilityUtil::WaitCompleted(event, "OnStateChanged", mapState["OnStateChanged"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15900 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_15900 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_15900 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_16000
 * @tc.name      : Test the OnStateChanged event in the LifecycleObserver class
 * @tc.desc      : Start the while loop in the OnStateChanged event.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_16000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_16000 start";

    MAP_STR_STR params;
    params["No."] = "3";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));

        int ret = STAbilityUtil::WaitCompleted(event, "OnStateChanged", mapState["OnStateChanged"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_16000 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_16000 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_16000 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_16100
 * @tc.name      : Test the OnStateChanged event in the LifecycleObserver class
 * @tc.desc      : Stop ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_16100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_16100 start";

    MAP_STR_STR params;
    params["No."] = "4";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));

        int ret = STAbilityUtil::WaitCompleted(event, "OnStateChanged", mapState["OnStateChanged"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_16100 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_16100 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_16100 end";
}

/**
 * @tc.number    : AMS_Page_LifecycleObserver_16200
 * @tc.name      : Test the OnStateChanged event in the LifecycleObserver class
 * @tc.desc      : Terminate ability immediately after starting ability.
 */
HWTEST_F(ActsAmsKitLifecycleTest, AMS_Page_LifecycleObserver_16200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_16200 start";

    MAP_STR_STR params;
    params["No."] = "5";

    bool result = true;
    for (int i = 1; i <= stLevel_.AMSLevel; i++) {
        Want want =
            STAbilityUtil::MakeWant("device", ABILTIY_NAME_ABILITY_LIFE_CYCLE_OBSERVER, kitPageBundleName, params);
        // start ability
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", mapState["OnStart"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStart", 0, DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnActive", mapState["OnActive"], DELAY_TIME));

        int ret = STAbilityUtil::WaitCompleted(event, "OnStateChanged", mapState["OnStateChanged"], DELAY_TIME);
        EXPECT_EQ(0, ret);
        if (ret != 0) {
            result = false;
            GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_16200 : " << i;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));

        // stop ability
        STAbilityUtil::PublishEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME, 1, "StopSelfAbility");
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnInactive", mapState["OnInactive"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnBackground", mapState["OnBackground"], DELAY_TIME));
        EXPECT_EQ(0, STAbilityUtil::WaitCompleted(event, "OnStop", mapState["OnStop"], DELAY_TIME));
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ABILITY_OK));
    }
    if (result && stLevel_.AMSLevel > 1) {
        GTEST_LOG_(INFO) << "AMS_Page_LifecycleObserver_16200 : " << stLevel_.CESLevel;
    }
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ActsAmsKitLifecycleTest AMS_Page_LifecycleObserver_16200 end";
}
