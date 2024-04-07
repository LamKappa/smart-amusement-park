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
#include <cstdio>
#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>
#include <set>
#include <thread>

#include "ability_lifecycle_executor.h"
#include "ability_manager_errors.h"
#include "ability_manager_service.h"
#include "app_mgr_service.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "data_uri_utils.h"
#include "event.h"
#include "hilog_wrapper.h"
#include "module_test_dump_util.h"
#include "sa_mgr_client.h"
#include "semaphore_ex.h"
#include "stoperator.h"
#include "system_ability_definition.h"
#include "system_test_ability_util.h"
#include "testConfigParser.h"
#include "uri.h"
namespace {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::MTUtil;
using namespace OHOS::EventFwk;
using Uri = OHOS::Uri;
using namespace OHOS::STtools;
using namespace OHOS::STABUtil;
using namespace testing::ext;

using MAP_STR_STR = std::map<std::string, std::string>;
using VECTOR_STR = std::vector<std::string>;
static const std::string BUNDLE_NAME_BASE = "com.ohos.amsst.AppKitData";
static const std::string ABILITY_NAME_BASE = "AmsStKitDataAbility";
static const std::string HAP_NAME_BASE = "amsKitSystemTestData";

static const int ABILITY_CODE_BASE = 100;
static const long DEFAULT_ID = 100;

const int PAGE_ABILITY_CODE = 1;
const int DATA_ABILITY_CODE = 2;
const int SERVICE_ABILITY_CODE = 3;

const int ABILITY_KIT_PAGE_A_CODE = 130;
const int ABILITY_KIT_SERVICE_A_CODE = 310;
const int ABILITY_KIT_DATA_A1_CODE = 250;
const int LIFECYCLE_CALLBACKS_A1 = 251;
const int LIFECYCLE_OBSERVER_A1 = 252;
const int ABILITY_KIT_DATA_A2_CODE = 260;
const int LIFECYCLE_CALLBACKS_A2 = 261;
const int LIFECYCLE_OBSERVER_A2 = 262;
const int ABILITY_KIT_DATA_A3_CODE = 290;
const int LIFECYCLE_OBSERVER_A3 = 292;

const int ABILITY_KIT_PAGE_B_CODE = 140;
const int ABILITY_KIT_DATA_B_CODE = 270;
const int LIFECYCLE_CALLBACKS_B = 271;
const int LIFECYCLE_OBSERVER_B = 272;

const std::string DEVICE_ID = "device";

const std::string ABILITY_TYPE_PAGE = "0";
const std::string ABILITY_TYPE_SERVICE = "1";
const std::string ABILITY_TYPE_DATA = "2";

const std::string ABILITY_EVENT_NAME = "event_data_ability_callback";
const std::string TEST_EVENT_NAME = "event_data_test_action";

const std::string PAGE_STATE_ONSTART = "onStart";
const std::string PAGE_STATE_ONACTIVE = "OnActive";

const std::string DATA_STATE_ONSTART = "OnStart";
const std::string DATA_STATE_ONSTOP = "OnStop";
const std::string DATA_STATE_ONACTIVE = "OnActive";
const std::string DATA_STATE_ONINACTIVE = "OnInactive";
const std::string DATA_STATE_ONFOREGROUND = "OnForeground";
const std::string DATA_STATE_ONBACKGROUND = "OnBackground";
const std::string DATA_STATE_INSERT = "Insert";
const std::string DATA_STATE_DELETE = "Delete";
const std::string DATA_STATE_UPDATE = "Update";
const std::string DATA_STATE_QUERY = "Query";
const std::string DATA_STATE_GETFILETYPES = "GetFileTypes";
const std::string DATA_STATE_OPENFILE = "OpenFile";

const std::string SERVICE_STATE_ONSTART = "onStart";
const std::string SERVICE_STATE_ONACTIVE = "OnActive";
const std::string SERVICE_STATE_ONCOMMAND = "OnCommand";

const std::string DATA_STATE_CHANGE = "OnStateChanged";
const std::string DATA_STATE_ONNEWWANT = "OnNewWant";

const std::string OPERATOR_INSERT = "Insert";
const std::string OPERATOR_DELETE = "Delete";
const std::string OPERATOR_UPDATE = "Update";
const std::string OPERATOR_QUERY = "Query";
const std::string OPERATOR_GETFILETYPES = "GetFileTypes";
const std::string OPERATOR_OPENFILE = "OpenFile";
const std::string OPERATOR_GETTYPE = "GetType";
const std::string OPERATOR_TEST_LIFECYCLE = "TestLifeCycle";
const std::string OPERATOR_GETLIFECYCLESTATE = "GetLifecycleState";

const std::string DEFAULT_INSERT_RESULT = "1111";
const std::string DEFAULT_DELETE_RESULT = "2222";
const std::string DEFAULT_UPDATE_RESULT = "3333";
const std::string DEFAULT_OPENFILE_RESULT = "4444";
const std::string DEFAULT_GETFILETYPE_RESULT = "filetypes";
const std::string DEFAULT_QUERY_RESULT = "Query";

int g_StLevel = 1;

bool PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    Want want;
    want.SetAction(eventName);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}

class ActsAmsKitDataTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    void ResetSystem() const;
    static bool SubscribeEvent();
    void ReInstallBundle() const;
    void UnInstallBundle() const;
    static sptr<IAppMgr> g_appMs;
    static sptr<IAbilityManager> g_abilityMs;

    static int TestWaitCompleted(Event &event, const std::string &eventName, const int code, const int timeout = 10);
    static void TestCompleted(Event &event, const std::string &eventName, const int code);

    static STtools::Event event;
    static StressTestLevel stLevel_;

    class AppEventSubscriber : public CommonEventSubscriber {
    public:
        explicit AppEventSubscriber(const CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp)
        {
            PageAbilityState_ = {
                {"onStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
                {"OnStop", AbilityLifecycleExecutor::LifecycleState::INITIAL},
                {"OnActive", AbilityLifecycleExecutor::LifecycleState::ACTIVE},
                {"OnInactive", AbilityLifecycleExecutor::LifecycleState::INACTIVE},
                {"OnBackground", AbilityLifecycleExecutor::LifecycleState::BACKGROUND},
            };
            DataAbilityState_ = {
                {"OnStart"},
                {"Insert"},
                {"Delete"},
                {"Update"},
                {"Query"},
                {"GetFileTypes"},
                {"OpenFile"},
            };
            AbilityOperator_ = {
                {"Insert"},
                {"Delete"},
                {"Update"},
                {"Query"},
                {"GetFileTypes"},
                {"OpenFile"},
            };
        };
        ~AppEventSubscriber()
        {
            PageAbilityState_ = {};
            DataAbilityState_ = {};
            AbilityOperator_ = {};
        };
        virtual void OnReceiveEvent(const CommonEventData &data) override;
        std::unordered_map<std::string, int> PageAbilityState_;
        std::set<std::string> DataAbilityState_;
        std::set<std::string> AbilityOperator_;
    };
};

StressTestLevel ActsAmsKitDataTest::stLevel_{};
STtools::Event ActsAmsKitDataTest::event = STtools::Event();

void ActsAmsKitDataTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "OnReceiveEvent: event=" << data.GetWant().GetAction();
    GTEST_LOG_(INFO) << "OnReceiveEvent: data=" << data.GetData();
    GTEST_LOG_(INFO) << "OnReceiveEvent: code=" << data.GetCode();

    std::string eventName = data.GetWant().GetAction();
    if (eventName.compare(ABILITY_EVENT_NAME) == 0) {

        std::string target = data.GetData();
        if (target.find(" ") != target.npos) {
            ActsAmsKitDataTest::TestCompleted(event, target, data.GetCode());
            return;
        }

        if (PAGE_ABILITY_CODE == data.GetCode() / ABILITY_CODE_BASE)  // page ability
        {
            ActsAmsKitDataTest::TestCompleted(event, target, data.GetCode());
            return;
        } else if (DATA_ABILITY_CODE == data.GetCode() / ABILITY_CODE_BASE)  // data ability
        {
            ActsAmsKitDataTest::TestCompleted(event, target, data.GetCode());
            return;
        } else if (SERVICE_ABILITY_CODE == data.GetCode() / ABILITY_CODE_BASE)  // data ability
        {
            ActsAmsKitDataTest::TestCompleted(event, target, data.GetCode());
            return;
        }
    }
}

void ActsAmsKitDataTest::SetUpTestCase(void)
{
    if (!SubscribeEvent()) {
        GTEST_LOG_(INFO) << "SubscribeEvent error";
    }
    TestConfigParser tcp;
    tcp.ParseFromFile4StressTest(STRESS_TEST_CONFIG_FILE_PATH, stLevel_);
    std::cout << "stress test level : "
              << "AMS : " << stLevel_.AMSLevel << " "
              << "BMS : " << stLevel_.BMSLevel << " "
              << "CES : " << stLevel_.CESLevel << std::endl;
    g_StLevel = stLevel_.AMSLevel;
    g_StLevel = 5;
}

void ActsAmsKitDataTest::TearDownTestCase(void)
{}

void ActsAmsKitDataTest::SetUp(void)
{
    ReInstallBundle();
    ResetSystem();
    STAbilityUtil::CleanMsg(event);

    g_abilityMs = STAbilityUtil::GetAbilityManagerService();
    g_appMs = STAbilityUtil::GetAppMgrService();
}

void ActsAmsKitDataTest::TearDown(void)
{
    UnInstallBundle();
    STAbilityUtil::CleanMsg(event);
}

bool ActsAmsKitDataTest::SubscribeEvent()
{
    std::vector<std::string> eventList = {"event_data_ability_callback"};
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    auto subscriber = std::make_shared<AppEventSubscriber>(subscribeInfo);
    return CommonEventManager::SubscribeCommonEvent(subscriber);
}

void ActsAmsKitDataTest::ReInstallBundle() const
{
    std::vector<std::string> bundleNameSuffix = {"A", "B"};
    for (std::string suffix : bundleNameSuffix) {
        STAbilityUtil::Uninstall(BUNDLE_NAME_BASE + suffix);
        STAbilityUtil::Install(HAP_NAME_BASE + suffix);
    }
}

void ActsAmsKitDataTest::UnInstallBundle() const
{
    std::vector<std::string> bundleNameSuffix = {"A", "B"};
    for (std::string suffix : bundleNameSuffix) {
        STAbilityUtil::Uninstall(BUNDLE_NAME_BASE + suffix);
    }
}

sptr<IAppMgr> ActsAmsKitDataTest::g_appMs = nullptr;
sptr<IAbilityManager> ActsAmsKitDataTest::g_abilityMs = nullptr;

void ActsAmsKitDataTest::ResetSystem() const
{
    GTEST_LOG_(INFO) << "ResetSystem";
}

int ActsAmsKitDataTest::TestWaitCompleted(Event &event, const std::string &eventName, const int code, const int timeout)
{
    GTEST_LOG_(INFO) << "TestWaitCompleted : " << eventName << " " << code;
    return STAbilityUtil::WaitCompleted(event, eventName, code, timeout);
}

void ActsAmsKitDataTest::TestCompleted(Event &event, const std::string &eventName, const int code)
{
    GTEST_LOG_(INFO) << "TestCompleted : " << eventName << " " << code;
    return STAbilityUtil::Completed(event, eventName, code);
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_00100
 * @tc.name      : test onabilityactive in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onabilityactive function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_00100, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityActive() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);

        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00100 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_00200
 * @tc.name      : test onabilityactive in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the onabilityactive function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_00200, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityActive() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00200 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_00300
 * @tc.name      : test onabilityactive in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onabilityactive function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_00300, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityActive() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_CALLBACKS_A2, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00300 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_00400
 * @tc.name      : test onabilityactive in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in different app. Verify that the onabilityactive function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_00400, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityActive() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_CALLBACKS_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00400 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_00500
 * @tc.name      : test onabilityactive in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onabilityactive function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_00500, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityActive() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_SERVICE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00500 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_00600
 * @tc.name      : test onabilityactive in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in different app. Verify that the onabilityactive function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_00600, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityActive() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_SERVICE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_CALLBACKS_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00600 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_00700
 * @tc.name      : test onAbilityInactive in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in same app. Verify that the onAbilityInactive function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_00700, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityInactive() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00700 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_00800
 * @tc.name      : test onAbilityInactive in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onAbilityInactive function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_00800, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityInactive()  002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00800 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_00900
 * @tc.name      : test onAbilityInactive in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onAbilityInactive function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_00900, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityInactive() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_CALLBACKS_A2, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_00900 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_01000
 * @tc.name      : test onAbilityInactive in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in different app. Verify that the onAbilityInactive function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_01000, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityInactive() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_CALLBACKS_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01000 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_01100
 * @tc.name      : test onAbilityInactive in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onAbilityInactive function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_01100, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityInactive() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01100 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_01200
 * @tc.name      : test onAbilityInactive in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in different app. Verify that the onAbilityInactive function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_01200, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityInactive() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_CALLBACKS_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01200 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_01300
 * @tc.name      : test onAbilityForeground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onAbilityForeground function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_01300, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityForeground() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01300 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_01400
 * @tc.name      : test onAbilityForeground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the onAbilityForeground function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_01400, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityForeground()  002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01400 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_01500
 * @tc.name      : test onAbilityForeground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onAbilityForeground function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_01500, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityForeground() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_A2, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01500 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_01600
 * @tc.name      : test onAbilityForeground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in different app. Verify that the onAbilityForeground function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_01600, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityForeground() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01600 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_01700
 * @tc.name      : test onAbilityForeground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onAbilityForeground function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_01700, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityForeground() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01700 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_01800
 * @tc.name      : test onAbilityForeground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in different app. Verify that the onAbilityForeground function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_01800, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityForeground() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01800 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_01900
 * @tc.name      : test onAbilityForeground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onAbilityForeground function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_01900, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityForeground() 007
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_01900 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_02000
 * @tc.name      : test onAbilityForeground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the onAbilityForeground function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_02000, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityForeground()  008
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02000 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_02100
 * @tc.name      : test onAbilityForeground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onAbilityForeground function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_02100, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityForeground() 009
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_A2, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_A2, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02100 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_02200
 * @tc.name      : test onAbilityForeground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in different app. Verify that the onAbilityForeground function in
 * ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_02200, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityForeground() 010
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);

    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02200 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_02300
 * @tc.name      : test onAbilityForeground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onAbilityForeground function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_02300, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityForeground() 011
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02300 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_02400
 * @tc.name      : test onAbilityForeground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in different app. Verify that the onAbilityForeground function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_02400, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityForeground() 012
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_CALLBACKS_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02400 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_02500
 * @tc.name      : test onAbilityStop in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onAbilityStop function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_02500, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStop() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02500 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_02600
 * @tc.name      : test onAbilityStop in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the onAbilityStop function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_02600, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStop() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02600 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_02700
 * @tc.name      : test onAbilityStop in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onAbilityStop function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_02700, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStop() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A2_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_CALLBACKS_A2, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02700 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_02800
 * @tc.name      : test onAbilityStop in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in different app. Verify that the onAbilityStop function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_02800, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStop() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_CALLBACKS_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02800 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_02900
 * @tc.name      : test onAbilityStop in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onAbilityStop function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_02900, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStop() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_02900 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_03000
 * @tc.name      : test onAbilityStop in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in different app. Verify that the onAbilityStop function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_03000, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStop() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_CALLBACKS_B, 1), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03000 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_03100
 * @tc.name      : test onAbilitySaveState in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onAbilitySaveState function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_03100, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilitySaveState(PacMap*) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03100 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_03200
 * @tc.name      : test onAbilitySaveState in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the onAbilitySaveState function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_03200, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilitySaveState(PacMap*) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest ams_kit_data_test_032 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_03300
 * @tc.name      : test onAbilitySaveState in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onAbilitySaveState function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_03300, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilitySaveState(PacMap*) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_CALLBACKS_A2, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_CALLBACKS_A2, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03300 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_03400
 * @tc.name      : test onAbilitySaveState in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in different app. Verify that the onAbilitySaveState function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_03400, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilitySaveState(PacMap*) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);

    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_CALLBACKS_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_CALLBACKS_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03400 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_03500
 * @tc.name      : test onAbilitySaveState in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onAbilitySaveState function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_03500, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilitySaveState(PacMap*) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03500 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_03600
 * @tc.name      : test onAbilitySaveState in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in different app. Verify that the onAbilitySaveState function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_03600, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilitySaveState(PacMap*) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_CALLBACKS_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_CALLBACKS_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03600 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_03700
 * @tc.name      : test onAbilityBackground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onAbilityBackground function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_03700, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityBackground() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03700 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_03800
 * @tc.name      : test onAbilityBackground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the onAbilityBackgrounds function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_03800, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityBackground() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03800 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_03900
 * @tc.name      : test onAbilityBackground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in different app. Verify that the onAbilityBackgrounds function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_03900, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityBackground() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_CALLBACKS_A2, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_03900 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_04000
 * @tc.name      : test onAbilityBackground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in different app. Verify that the onAbilityBackgrounds function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_04000, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityBackground() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_CALLBACKS_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04000 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_04100
 * @tc.name      : test onAbilityBackground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in different app. Verify that the onAbilityBackgrounds function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_04100, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityBackground() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_CALLBACKS_A1, 1), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04100 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_04200
 * @tc.name      : test onAbilityBackground in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in different app. Verify that the onAbilityBackgrounds function
 * in ability_lifecycle_callbacks is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_04200, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityBackground() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_CALLBACKS_B, 1), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04200 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_04300
 * @tc.name      : test onAbilityStart in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the onAbilityStart function
 * in ability_lifecycle_callbacks is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_04300, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStart() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04300 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_04400
 * @tc.name      : test onAbilityStart in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the onAbilityStart function
 * in ability_lifecycle_callbacks is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_04400, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStart()  002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04400 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_04500
 * @tc.name      : test onAbilityStart in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in different app. Verify that the onAbilityStart function
 * in ability_lifecycle_callbacks is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_04500, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStart() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A2), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04500 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_04600
 * @tc.name      : test onAbilityStart in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in different app. Verify that the onAbilityStart function
 * in ability_lifecycle_callbacks is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_04600, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStart() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_B), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04600 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_04700
 * @tc.name      : test onAbilityStart in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in different app. Verify that the onAbilityStart function
 * in ability_lifecycle_callbacks is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_04700, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStart() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04700 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_04800
 * @tc.name      : test onAbilityStart in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in different app. Verify that the onAbilityStart function
 * in ability_lifecycle_callbacks is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_04800, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStart() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_B), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04800 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_04900
 * @tc.name      : test onAbilityStart in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the onAbilityStart function
 * in ability_lifecycle_callbacks is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_04900, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStart() 007
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_04900 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_05000
 * @tc.name      : test onAbilityStart in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the onAbilityStart function
 * in ability_lifecycle_callbacks is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_05000, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStart()  008
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_05000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_05000 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_05100
 * @tc.name      : test onAbilityStart in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in different app. Verify that the onAbilityStart function
 * in ability_lifecycle_callbacks is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_05100, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStart() 009
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_05100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A2), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A2, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_05100 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_05200
 * @tc.name      : test onAbilityStart in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by data ability in different app. Verify that the onAbilityStart function
 * in ability_lifecycle_callbacks is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_05200, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStart() 010
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_05200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);

    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_B), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_05200 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_05300
 * @tc.name      : test onAbilityStart in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in different app. Verify that the onAbilityStart function
 * in ability_lifecycle_callbacks is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_05300, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStart() 011
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_05300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_05300 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleCallbacks_05400
 * @tc.name      : test onAbilityStart in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by service ability in different app. Verify that the onAbilityStart function
 * in ability_lifecycle_callbacks is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleCallbacks_05400, Function | MediumTest | Level1)
{
    // ability_lifecycle_callbacks.h	void onAbilityStart() 012
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_05400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_B), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleCallbacks_05400 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00100
 * @tc.name      : test insert in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the insert function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_00100, Function | MediumTest | Level1)
{
    // ability.h	int insert(Uri*, ValuesBucket)   001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00100 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00200
 * @tc.name      : test insert in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the insert function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_00200, Function | MediumTest | Level1)
{
    // ability.h	int insert(Uri*, ValuesBucket) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00200 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00300
 * @tc.name      : test insert in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the insert function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_00300, Function | MediumTest | Level1)
{
    // ability.h	int insert(Uri*, ValuesBucket)  003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A2_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00300 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00400
 * @tc.name      : test insert in ability
 * @tc.desc      : start data ability by data ability in different app. Verify that the insert function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_00400, Function | MediumTest | Level1)
{
    // ability.h	int insert(Uri*, ValuesBucket)  004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00400 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00500
 * @tc.name      : test insert in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the insert function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_00500, Function | MediumTest | Level1)
{
    // ability.h	int insert(Uri*, ValuesBucket)  005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00500 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00600
 * @tc.name      : test insert in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the insert function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_00600, Function | MediumTest | Level1)
{
    // ability.h	int insert(Uri*, ValuesBucket)  006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00600 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00700
 * @tc.name      : test update in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the update function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_00700, Function | MediumTest | Level1)
{
    // ability.h	int update(Uri, ValuesBucket, DataAbilityPredicates)   001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00700 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00800
 * @tc.name      : test update in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the update function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_00800, Function | MediumTest | Level1)
{
    // ability.h	int update(Uri, ValuesBucket, DataAbilityPredicates) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00800 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00900
 * @tc.name      : test update in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the update function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_00900, Function | MediumTest | Level1)
{
    // ability.h	int update(Uri, ValuesBucket, DataAbilityPredicates)  003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_KIT_DATA_A2_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_00900 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01000
 * @tc.name      : test update in ability
 * @tc.desc      : start data ability by data ability in different app. Verify that the update function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_01000, Function | MediumTest | Level1)
{
    // ability.h	int update(Uri, ValuesBucket, DataAbilityPredicates)  004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01000 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01100
 * @tc.name      : test update in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the update function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_01100, Function | MediumTest | Level1)
{
    // ability.h	int update(Uri, ValuesBucket, DataAbilityPredicates)  005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01100 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01200
 * @tc.name      : test update in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the update function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_01200, Function | MediumTest | Level1)
{
    // ability.h	int update(Uri, ValuesBucket, DataAbilityPredicates)  006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01200 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01300
 * @tc.name      : test onStart in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onStart function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_01300, Function | MediumTest | Level1)
{
    // ability.h	void onStart(Intent) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01300 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01400
 * @tc.name      : test onStart in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the onStart function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_01400, Function | MediumTest | Level1)
{
    // ability.h	void onStart(Intent) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01400 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01500
 * @tc.name      : test onStart in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onStart function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_01500, Function | MediumTest | Level1)
{
    // ability.h	void onStart(Intent) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A2_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01500 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01600
 * @tc.name      : test onStart in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the onStart function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_01600, Function | MediumTest | Level1)
{
    // ability.h	void onStart(Intent) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A2_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01600 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01700
 * @tc.name      : test onStart in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onStart function in ability is
 * executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_01700, Function | MediumTest | Level1)
{
    // ability.h	void onStart(Intent) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01700 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01800
 * @tc.name      : test onStart in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the onStart function in ability
 * is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_01800, Function | MediumTest | Level1)
{
    // ability.h	void onStart(Intent) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01800 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01900
 * @tc.name      : test onStart in ability
 * @tc.desc      : start data ability twice by page ability in  the same app. Verify that the onStart function in
 * ability is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_01900, Function | MediumTest | Level1)
{
    // ability.h	void onStart(Intent) 007
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_01900 end";
}

/**
 * @tc.number    : AMS_Data_Ability_02000
 * @tc.name      : test onStart in ability
 * @tc.desc      : start data ability twice by page ability in different app. Verify that the onStart function in
 * ability is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_02000, Function | MediumTest | Level1)
{
    // ability.h	void onStart(Intent)  008
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02000 end";
}

/**
 * @tc.number    : AMS_Data_Ability_02100
 * @tc.name      : test onStart in ability
 * @tc.desc      : start data ability twice by data ability in the same app. Verify that the onStart function in ability
 * is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_02100, Function | MediumTest | Level1)
{
    // ability.h	void onStart(Intent) 009
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A2_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A2_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02100 end";
}

/**
 * @tc.number    : AMS_Data_Ability_02200
 * @tc.name      : test onStart in ability
 * @tc.desc      : start data ability twice by data ability in different app. Verify that the onStart function in
 * ability is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_02200, Function | MediumTest | Level1)
{
    // ability.h	void onStart(Intent) 010
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);

    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_B_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02200 end";
}

/**
 * @tc.number    : AMS_Data_Ability_02100
 * @tc.name      : test onStart in ability
 * @tc.desc      : start data ability twice by service ability in the same app. Verify that the onStart function in
 * ability is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_02300, Function | MediumTest | Level1)
{
    // ability.h	void onStart(Intent) 011
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02300 end";
}

/**
 * @tc.number    : AMS_Data_Ability_02200
 * @tc.name      : test onStart in ability
 * @tc.desc      : start data ability twice by service ability in different app. Verify that the onStart function in
 * ability is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_02400, Function | MediumTest | Level1)
{
    // ability.h	void onStart(Intent) 012
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_B_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02400 end";
}

/**
 * @tc.number    : AMS_Data_Ability_02500
 * @tc.name      : test onStop in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onStop function in ability is not
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_02500, Function | MediumTest | Level1)
{
    // ability.h	void onStop() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02500 end";
}

/**
 * @tc.number    : AMS_Data_Ability_02600
 * @tc.name      : test onStop in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the onStop function in ability is
 * not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_02600, Function | MediumTest | Level1)
{
    // ability.h	void onStop() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, ABILITY_KIT_DATA_A1_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02600 end";
}

/**
 * @tc.number    : AMS_Data_Ability_02700
 * @tc.name      : test onStop in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onStop function in ability is not
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_02700, Function | MediumTest | Level1)
{
    // ability.h	void onStop() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, ABILITY_KIT_DATA_A2_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02700 end";
}

/**
 * @tc.number    : AMS_Data_Ability_02800
 * @tc.name      : test onStop in ability
 * @tc.desc      : start data ability by data ability in different app. Verify that the onStop function in ability is
 * not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_02800, Function | MediumTest | Level1)
{
    // ability.h	void onStop() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, ABILITY_KIT_DATA_B_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02800 end";
}

/**
 * @tc.number    : AMS_Data_Ability_02900
 * @tc.name      : test onStop in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onStop function in ability is
 * not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_02900, Function | MediumTest | Level1)
{
    // ability.h	void onStop() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, ABILITY_KIT_DATA_A1_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_02900 end";
}

/**
 * @tc.number    : AMS_Data_Ability_03000
 * @tc.name      : test onStop in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the onStop function in ability is
 * not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_03000, Function | MediumTest | Level1)
{
    // ability.h	void onStop() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, ABILITY_KIT_DATA_B_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03000 end";
}

/**
 * @tc.number    : AMS_Data_Ability_03100
 * @tc.name      : test onNewIntent in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onNewIntent function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_03100, Function | MediumTest | Level1)
{
    // ability.h	void onNewIntent(Intent) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONNEWWANT, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03100 end";
}

/**
 * @tc.number    : AMS_Data_Ability_03200
 * @tc.name      : test onNewIntent in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the onNewIntent function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_03200, Function | MediumTest | Level1)
{
    // ability.h	void onNewIntent(Intent) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONNEWWANT, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03200 end";
}

/**
 * @tc.number    : AMS_Data_Ability_03300
 * @tc.name      : test onNewIntent in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onNewIntent function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_03300, Function | MediumTest | Level1)
{
    // ability.h	void onNewIntent(Intent) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A2_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONNEWWANT, ABILITY_KIT_DATA_A2_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03300 end";
}

/**
 * @tc.number    : AMS_Data_Ability_03400
 * @tc.name      : test onNewIntent in ability
 * @tc.desc      : start data ability by data ability in different app. Verify that the onNewIntent function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_03400, Function | MediumTest | Level1)
{
    // ability.h	void onNewIntent(Intent) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);

    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONNEWWANT, ABILITY_KIT_DATA_B_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03400 end";
}

/**
 * @tc.number    : AMS_Data_Ability_03500
 * @tc.name      : test onNewIntent in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onNewIntent function in
 * ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_03500, Function | MediumTest | Level1)
{
    // ability.h	void onNewIntent(Intent) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONNEWWANT, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03500 end";
}

/**
 * @tc.number    : AMS_Data_Ability_03600
 * @tc.name      : test onNewIntent in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the onNewIntent function in
 * ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_03600, Function | MediumTest | Level1)
{
    // ability.h	void onNewIntent(Intent) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONNEWWANT, ABILITY_KIT_DATA_B_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03600 end";
}

/**
 * @tc.number    : AMS_Data_Ability_03700
 * @tc.name      : test onInactive in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onInactive function in ability is
 * not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_03700, Function | MediumTest | Level1)
{
    // ability.h	void onInactive() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03700 end";
}

/**
 * @tc.number    : AMS_Data_Ability_03800
 * @tc.name      : test onInactive in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the onInactive function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_03800, Function | MediumTest | Level1)
{
    // ability.h	void onInactive() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03800 end";
}

/**
 * @tc.number    : AMS_Data_Ability_03900
 * @tc.name      : test onInactive in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onInactive function in ability is
 * not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_03900, Function | MediumTest | Level1)
{
    // ability.h	void onInactive() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, ABILITY_KIT_DATA_A2_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_03900 end";
}

/**
 * @tc.number    : AMS_Data_Ability_04000
 * @tc.name      : test onInactive in ability
 * @tc.desc      : start data ability by data ability in different app. Verify that the onInactive function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_04000, Function | MediumTest | Level1)
{
    // ability.h	void onInactive() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, ABILITY_KIT_DATA_B_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04000 end";
}

/**
 * @tc.number    : AMS_Data_Ability_04100
 * @tc.name      : test onInactive in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onInactive function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_04100, Function | MediumTest | Level1)
{
    // ability.h	void onInactive() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, ABILITY_KIT_DATA_A1_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04100 end";
}

/**
 * @tc.number    : AMS_Data_Ability_04200
 * @tc.name      : test onInactive in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the onInactive function in
 * ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_04200, Function | MediumTest | Level1)
{
    // ability.h	void onInactive() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, ABILITY_KIT_DATA_B_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04200 end";
}

/**
 * @tc.number    : AMS_Data_Ability_04300
 * @tc.name      : test onActive in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onActive function in ability is
 * not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_04300, Function | MediumTest | Level1)
{
    // ability.h	void onActive() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04300 end";
}

/**
 * @tc.number    : AMS_Data_Ability_04400
 * @tc.name      : test onActive in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the onActive function in ability is
 * not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_04400, Function | MediumTest | Level1)
{
    // ability.h	void onActive() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04400 end";
}

/**
 * @tc.number    : AMS_Data_Ability_04500
 * @tc.name      : test onActive in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onActive function in ability is
 * not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_04500, Function | MediumTest | Level1)
{
    // ability.h	void onActive() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, ABILITY_KIT_DATA_A2_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04500 end";
}

/**
 * @tc.number    : AMS_Data_Ability_04600
 * @tc.name      : test onActive in ability
 * @tc.desc      : start data ability by data ability in different app. Verify that the onActive function in ability is
 * not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_04600, Function | MediumTest | Level1)
{
    // ability.h	void onActive() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, ABILITY_KIT_DATA_B_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04600 end";
}

/**
 * @tc.number    : AMS_Data_Ability_04700
 * @tc.name      : test onActive in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onActive function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_04700, Function | MediumTest | Level1)
{
    // ability.h	void onActive() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, ABILITY_KIT_DATA_A1_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04700 end";
}

/**
 * @tc.number    : AMS_Data_Ability_04800
 * @tc.name      : test onActive in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the onActive function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_04800, Function | MediumTest | Level1)
{
    // ability.h	void onActive() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, ABILITY_KIT_DATA_B_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04800 end";
}

/**
 * @tc.number    : AMS_Data_Ability_04900
 * @tc.name      : test onForeground in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onForeground function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_04900, Function | MediumTest | Level1)
{
    // ability.h	void onForeground(Intent) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_04900 end";
}

/**
 * @tc.number    : AMS_Data_Ability_05000
 * @tc.name      : test onForeground in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the onForeground function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_05000, Function | MediumTest | Level1)
{
    // ability.h	void onForeground(Intent) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05000 end";
}

/**
 * @tc.number    : AMS_Data_Ability_05100
 * @tc.name      : test onForeground in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onForeground function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_05100, Function | MediumTest | Level1)
{
    // ability.h	void onForeground(Intent) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A2_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05100 end";
}

/**
 * @tc.number    : AMS_Data_Ability_05200
 * @tc.name      : test onForeground in ability
 * @tc.desc      : start data ability by data ability in different app. Verify that the onForeground function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_05200, Function | MediumTest | Level1)
{
    // ability.h	void onForeground(Intent) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A2_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05200 end";
}

/**
 * @tc.number    : AMS_Data_Ability_05300
 * @tc.name      : test onForeground in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onForeground function in
 * ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_05300, Function | MediumTest | Level1)
{
    // ability.h	void onForeground(Intent) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A1_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05300 end";
}

/**
 * @tc.number    : AMS_Data_Ability_05400
 * @tc.name      : test onForeground in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the onForeground function in
 * ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_05400, Function | MediumTest | Level1)
{
    // ability.h	void onForeground(Intent) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_B_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05400 end";
}

/**
 * @tc.number    : AMS_Data_Ability_05600
 * @tc.name      : test onForeground in ability
 * @tc.desc      : start data ability twice by page ability in the same app. Verify that the onForeground function in
 * ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_05500, Function | MediumTest | Level1)
{
    // ability.h	void onForeground(Intent) 007
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05500 end";
}

/**
 * @tc.number    : AMS_Data_Ability_05600
 * @tc.name      : test onForeground in ability
 * @tc.desc      : start data ability twice by page ability in different app. Verify that the onForeground function in
 * ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_05600, Function | MediumTest | Level1)
{
    // ability.h	void onForeground(Intent) 008
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05600 end";
}

/**
 * @tc.number    : AMS_Data_Ability_05700
 * @tc.name      : test onForeground in ability
 * @tc.desc      : start data ability twice by data ability in the same app. Verify that the onForeground function in
 * ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_05700, Function | MediumTest | Level1)
{
    // ability.h	void onForeground(Intent) 009
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A2_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A2_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05700 end";
}

/**
 * @tc.number    : AMS_Data_Ability_05800
 * @tc.name      : test onForeground in ability
 * @tc.desc      : start data ability twice by data ability in different app. Verify that the onForeground function in
 * ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_05800, Function | MediumTest | Level1)
{
    // ability.h	void onForeground(Intent) 010
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);

    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A2_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A2_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05800 end";
}

/**
 * @tc.number    : AMS_Data_Ability_05900
 * @tc.name      : test onForeground in ability
 * @tc.desc      : start data ability twice by service ability in the same app. Verify that the onForeground function in
 * ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_05900, Function | MediumTest | Level1)
{
    // ability.h	void onForeground(Intent) 011
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_05900 end";
}

/**
 * @tc.number    : AMS_Data_Ability_06000
 * @tc.name      : test onForeground in ability
 * @tc.desc      : start data ability twice by service ability in different app. Verify that the onForeground function
 * in ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_06000, Function | MediumTest | Level1)
{
    // ability.h	void onForeground(Intent) 012
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_B_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, ABILITY_KIT_DATA_B_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06000 end";
}

/**
 * @tc.number    : AMS_Data_Ability_06100
 * @tc.name      : test onBackground in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onBackground function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_06100, Function | MediumTest | Level1)
{
    // ability.h	void onBackground() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06100 end";
}

/**
 * @tc.number    : AMS_Data_Ability_06200
 * @tc.name      : test onBackground in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the onBackground function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_06200, Function | MediumTest | Level1)
{
    // ability.h	void onBackground() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, ABILITY_KIT_DATA_A1_CODE, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06200 end";
}

/**
 * @tc.number    : AMS_Data_Ability_06300
 * @tc.name      : test onBackground in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onBackground function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_06300, Function | MediumTest | Level1)
{
    // ability.h	void onBackground() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, ABILITY_KIT_DATA_A2_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06300 end";
}

/**
 * @tc.number    : AMS_Data_Ability_06400
 * @tc.name      : test onBackground in ability
 * @tc.desc      : start data ability by data ability in different app. Verify that the onBackground function in ability
 * is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_06400, Function | MediumTest | Level1)
{
    // ability.h	void onBackground() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, ABILITY_KIT_DATA_B_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06400 end";
}

/**
 * @tc.number    : AMS_Data_Ability_06500
 * @tc.name      : test onBackground in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onBackground function in
 * ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_06500, Function | MediumTest | Level1)
{
    // ability.h	void onBackground() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, ABILITY_KIT_DATA_A1_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06500 end";
}

/**
 * @tc.number    : AMS_Data_Ability_06600
 * @tc.name      : test onBackground in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the onBackground function in
 * ability is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_06600, Function | MediumTest | Level1)
{
    // ability.h	void onBackground() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, ABILITY_KIT_DATA_B_CODE, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06600 end";
}

/**
 * @tc.number    : AMS_Data_Ability_06700
 * @tc.name      : test delete in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the delete function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_06700, Function | MediumTest | Level1)
{
    // ability.h	int delete(Uri, DataAbilityPredicates) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_DELETE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06700 end";
}

/**
 * @tc.number    : AMS_Data_Ability_06800
 * @tc.name      : test delete in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the delete function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_06800, Function | MediumTest | Level1)
{
    // ability.h	int delete(Uri, DataAbilityPredicates) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_DELETE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06800 end";
}

/**
 * @tc.number    : AMS_Data_Ability_06900
 * @tc.name      : test delete in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the delete function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_06900, Function | MediumTest | Level1)
{
    // ability.h	int delete(Uri, DataAbilityPredicates) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_DELETE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_KIT_DATA_A2_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_06900 end";
}

/**
 * @tc.number    : AMS_Data_Ability_07000
 * @tc.name      : test delete in ability
 * @tc.desc      : start data ability by data ability in different app. Verify that the delete function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_07000, Function | MediumTest | Level1)
{
    // ability.h	int delete(Uri, DataAbilityPredicates) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_DELETE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07000 end";
}

/**
 * @tc.number    : AMS_Data_Ability_07100
 * @tc.name      : test delete in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the delete function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_07100, Function | MediumTest | Level1)
{
    // ability.h	int delete(Uri, DataAbilityPredicates) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_DELETE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07100 end";
}

/**
 * @tc.number    : AMS_Data_Ability_07200
 * @tc.name      : test delete in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the delete function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_07200, Function | MediumTest | Level1)
{
    // ability.h	int delete(Uri, DataAbilityPredicates) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_DELETE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07200 end";
}

/**
 * @tc.number    : AMS_Data_Ability_07300
 * @tc.name      : test getFileTypes in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the getFileTypes function in ability
 * is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_07300, Function | MediumTest | Level1)
{
    // ability.h	String[] getFileTypes(Uri, String) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_PAGE_A_CODE),
            0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07300 end";
}

/**
 * @tc.number    : AMS_Data_Ability_07400
 * @tc.name      : test getFileTypes in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the getFileTypes function in ability
 * is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_07400, Function | MediumTest | Level1)
{
    // ability.h	String[] getFileTypes(Uri, String) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_PAGE_B_CODE),
            0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07400 end";
}

/**
 * @tc.number    : AMS_Data_Ability_07500
 * @tc.name      : test getFileTypes in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the getFileTypes function in ability
 * is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_07500, Function | MediumTest | Level1)
{
    // ability.h	String[] getFileTypes(Uri, String) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_A2_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(
                      event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_DATA_A1_CODE),
            0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07500 end";
}

/**
 * @tc.number    : AMS_Data_Ability_07600
 * @tc.name      : test getFileTypes in ability
 * @tc.desc      : start data ability by data ability in different app. Verify that the getFileTypes function in ability
 * is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_07600, Function | MediumTest | Level1)
{
    // ability.h	String[] getFileTypes(Uri, String) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(
                      event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_DATA_A1_CODE),
            0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07600 end";
}

/**
 * @tc.number    : AMS_Data_Ability_07700
 * @tc.name      : test getFileTypes in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the getFileTypes function in
 * ability is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_07700, Function | MediumTest | Level1)
{
    // ability.h	String[] getFileTypes(Uri, String) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(
                      event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_SERVICE_A_CODE),
            0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07700 end";
}

/**
 * @tc.number    : AMS_Data_Ability_07800
 * @tc.name      : test getFileTypes in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the getFileTypes function in
 * ability is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_07800, Function | MediumTest | Level1)
{
    // ability.h	String[] getFileTypes(Uri, String) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(
                      event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_SERVICE_A_CODE),
            0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07800 end";
}

/**
 * @tc.number    : AMS_Data_Ability_07900
 * @tc.name      : test openFile in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the openFile function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_07900, Function | MediumTest | Level1)
{
    // ability.h	FileDescriptor openFile(Uri, String) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_OPENFILE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_OPENFILE, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_OPENFILE + " " + DEFAULT_OPENFILE_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_07900 end";
}

/**
 * @tc.number    : AMS_Data_Ability_08000
 * @tc.name      : test openFile in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the openFile function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_08000, Function | MediumTest | Level1)
{
    // ability.h	FileDescriptor openFile(Uri, String) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_OPENFILE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_OPENFILE, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_OPENFILE + " " + DEFAULT_OPENFILE_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08000 end";
}

/**
 * @tc.number    : AMS_Data_Ability_08100
 * @tc.name      : test openFile in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the openFile function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_08100, Function | MediumTest | Level1)
{
    // ability.h	FileDescriptor openFile(Uri, String) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_OPENFILE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_OPENFILE, ABILITY_KIT_DATA_A2_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_OPENFILE + " " + DEFAULT_OPENFILE_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08100 end";
}

/**
 * @tc.number    : AMS_Data_Ability_08200
 * @tc.name      : test openFile in ability
 * @tc.desc      : start data ability by data ability in different app. Verify that the openFile function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_08200, Function | MediumTest | Level1)
{
    // ability.h	FileDescriptor openFile(Uri, String) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_OPENFILE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_OPENFILE, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_OPENFILE + " " + DEFAULT_OPENFILE_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08200 end";
}

/**
 * @tc.number    : AMS_Data_Ability_08300
 * @tc.name      : test openFile in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the openFile function in ability
 * is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_08300, Function | MediumTest | Level1)
{
    // ability.h	FileDescriptor openFile(Uri, String) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_OPENFILE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_OPENFILE, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_OPENFILE + " " + DEFAULT_OPENFILE_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08300 end";
}

/**
 * @tc.number    : AMS_Data_Ability_08400
 * @tc.name      : test openFile in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the openFile function in ability
 * is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_08400, Function | MediumTest | Level1)
{
    // ability.h	FileDescriptor openFile(Uri, String) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_OPENFILE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_OPENFILE, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_OPENFILE + " " + DEFAULT_OPENFILE_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08400 end";
}

/**
 * @tc.number    : AMS_Data_Ability_08500
 * @tc.name      : test query in ability
 * @tc.desc      : start data ability by page ability in the same app. Verify that the query function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_08500, Function | MediumTest | Level1)
{
    // ability.h	ResultSet query(Uri, string[], DataAbilityPredicates) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08500 end";
}

/**
 * @tc.number    : AMS_Data_Ability_08600
 * @tc.name      : test query in ability
 * @tc.desc      : start data ability by page ability in different app. Verify that the query function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_08600, Function | MediumTest | Level1)
{
    // ability.h	ResultSet query(Uri, string[], DataAbilityPredicates) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_QUERY);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_B_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08600 end";
}

/**
 * @tc.number    : AMS_Data_Ability_08700
 * @tc.name      : test query in ability
 * @tc.desc      : start data ability by data ability in the same app. Verify that the query function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_08700, Function | MediumTest | Level1)
{
    // ability.h	ResultSet query(Uri, string[], DataAbilityPredicates) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_QUERY);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A2_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A2_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08700 end";
}

/**
 * @tc.number    : AMS_Data_Ability_08800
 * @tc.name      : test query in ability
 * @tc.desc      : start data ability by data ability in different app. Verify that the query function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_08800, Function | MediumTest | Level1)
{
    // ability.h	ResultSet query(Uri, string[], DataAbilityPredicates) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_QUERY);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_B_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08800 end";
}

/**
 * @tc.number    : AMS_Data_Ability_08900
 * @tc.name      : test query in ability
 * @tc.desc      : start data ability by service ability in the same app. Verify that the query function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_08900, Function | MediumTest | Level1)
{
    // ability.h	ResultSet query(Uri, string[], DataAbilityPredicates) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_08900 end";
}

/**
 * @tc.number    : AMS_Data_Ability_09000
 * @tc.name      : test query in ability
 * @tc.desc      : start data ability by service ability in different app. Verify that the query function in ability is
 * executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_09000, Function | MediumTest | Level1)
{
    // ability.h	ResultSet query(Uri, string[], DataAbilityPredicates) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_09000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_QUERY);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_B_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_09000 end";
}

/**
 * @tc.number    : AMS_Data_Ability_09100
 * @tc.name      : test getLifecycle in ability_lifecycle
 * @tc.desc      : start data ability by page ability in the same app. Verify that the getLifecycle function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_09100, Function | MediumTest | Level1)
{
    // ability.h	Lifecycle getLifecycle() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_09100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_09100 end";
}

/**
 * @tc.number    : AMS_Data_Ability_09200
 * @tc.name      : test getLifecycle in ability_lifecycle
 * @tc.desc      : start data ability by page ability in different app. Verify that the getLifecycle function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_09200, Function | MediumTest | Level1)
{
    // ability.h	Lifecycle getLifecycle() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_09200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_09200 end";
}

/**
 * @tc.number    : AMS_Data_Ability_09300
 * @tc.name      : test getLifecycleState in ability_lifecycle
 * @tc.desc      : start data ability by page ability in the same app. Verify that the getLifecycleState function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_09300, Function | MediumTest | Level1)
{
    // ability_lifecycle.h	Lifecycle.Event getLifecycleState() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_09300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETLIFECYCLESTATE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(
                      event, OPERATOR_GETLIFECYCLESTATE + " " + OPERATOR_GETLIFECYCLESTATE, ABILITY_KIT_DATA_A1_CODE),
            0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_09300 end";
}

/**
 * @tc.number    : AMS_Data_Ability_09400
 * @tc.name      : test getLifecycleState in ability_lifecycle
 * @tc.desc      : start data ability by page ability in different app. Verify that the getLifecycleState function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Ability_09400, Function | MediumTest | Level1)
{
    // ability_lifecycle.h	Lifecycle.Event getLifecycleState() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_09400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETLIFECYCLESTATE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(
                      event, OPERATOR_GETLIFECYCLESTATE + " " + OPERATOR_GETLIFECYCLESTATE, ABILITY_KIT_DATA_A1_CODE),
            0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Ability_09400 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_00100
 * @tc.name      : test delete in data_ability_helper
 * @tc.desc      : start data ability by page ability in the same app. Verify that the delete function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_00100, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int delete(Uri, DataAbilityPredicates) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_DELETE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00100 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_00200
 * @tc.name      : test delete in data_ability_helper
 * @tc.desc      : start data ability by page ability in different app. Verify that the delete function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_00200, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int delete(Uri, DataAbilityPredicates) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_DELETE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00200 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_00300
 * @tc.name      : test delete in data_ability_helper
 * @tc.desc      : start data ability by data ability in the same app. Verify that the delete function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_00300, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int delete(Uri, DataAbilityPredicates) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_DELETE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_KIT_DATA_A2_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00300 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_00400
 * @tc.name      : test delete in data_ability_helper
 * @tc.desc      : start data ability by data ability in different app. Verify that the delete function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_00400, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int delete(Uri, DataAbilityPredicates) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_DELETE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00400 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_00500
 * @tc.name      : test delete in data_ability_helper
 * @tc.desc      : start data ability by service ability in the same app. Verify that the delete function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_00500, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int delete(Uri, DataAbilityPredicates) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_DELETE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00500 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_00600
 * @tc.name      : test delete in data_ability_helper
 * @tc.desc      : start data ability by service ability in different app. Verify that the delete function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_00600, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int delete(Uri, DataAbilityPredicates) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_DELETE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00600 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_00700
 * @tc.name      : test insert in data_ability_helper
 * @tc.desc      : start data ability by page ability in the same app. Verify that the insert function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_00700, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int insert(Uri, ValuesBucket) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00700 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_00800
 * @tc.name      : test insert in data_ability_helper
 * @tc.desc      : start data ability by page ability in different app. Verify that the insert function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_00800, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int insert(Uri, ValuesBucket) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00800 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_00900
 * @tc.name      : test insert in data_ability_helper
 * @tc.desc      : start data ability by data ability in the same app. Verify that the insert function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_00900, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int insert(Uri, ValuesBucket) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A2_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_00900 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_01000
 * @tc.name      : test insert in data_ability_helper
 * @tc.desc      : start data ability by data ability in different app. Verify that the insert function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_01000, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int insert(Uri, ValuesBucket) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01000 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_01100
 * @tc.name      : test insert in data_ability_helper
 * @tc.desc      : start data ability by service ability in the same app. Verify that the insert function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_01100, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int insert(Uri, ValuesBucket) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01100 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_01200
 * @tc.name      : test insert in data_ability_helper
 * @tc.desc      : start data ability by service ability in different app. Verify that the insert function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_01200, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int insert(Uri, ValuesBucket) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01200 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_01300
 * @tc.name      : test update in data_ability_helper
 * @tc.desc      : start data ability by page ability in the same app. Verify that the update function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_01300, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int update(Uri, ValuesBucket, DataAbilityPredicates) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01300 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_01400
 * @tc.name      : test update in data_ability_helper
 * @tc.desc      : start data ability by page ability in different app. Verify that the update function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_01400, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int update(Uri, ValuesBucket, DataAbilityPredicates) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01400 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_01500
 * @tc.name      : test update in data_ability_helper
 * @tc.desc      : start data ability by data ability in the same app. Verify that the update function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_01500, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int update(Uri, ValuesBucket, DataAbilityPredicates) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_KIT_DATA_A2_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01500 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_01600
 * @tc.name      : test update in data_ability_helper
 * @tc.desc      : start data ability by data ability in different app. Verify that the update function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_01600, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int update(Uri, ValuesBucket, DataAbilityPredicates) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01600 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_01700
 * @tc.name      : test update in data_ability_helper
 * @tc.desc      : start data ability by service ability in the same app. Verify that the update function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_01700, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int update(Uri, ValuesBucket, DataAbilityPredicates) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01700 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_01800
 * @tc.name      : test update in data_ability_helper
 * @tc.desc      : start data ability by service ability in different app. Verify that the update function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_01800, Function | MediumTest | Level1)
{
    // data_ability_helper.h	int update(Uri, ValuesBucket, DataAbilityPredicates) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01800 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_01900
 * @tc.name      : test getFileTypes in data_ability_helper
 * @tc.desc      : start data ability by page ability in the same app. Verify that the getFileTypes function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_01900, Function | MediumTest | Level1)
{
    // data_ability_helper.h	String[] getFileTypes(Uri, String) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_PAGE_A_CODE),
            0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_01900 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_02000
 * @tc.name      : test getFileTypes in data_ability_helper
 * @tc.desc      : start data ability by page ability in different app. Verify that the getFileTypes function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_02000, Function | MediumTest | Level1)
{
    // data_ability_helper.h	String[] getFileTypes(Uri, String) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_PAGE_B_CODE),
            0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02000 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_02100
 * @tc.name      : test getFileTypes in data_ability_helper
 * @tc.desc      : start data ability by data ability in the same app. Verify that the getFileTypes function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_02100, Function | MediumTest | Level1)
{
    // data_ability_helper.h	String[] getFileTypes(Uri, String) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_A2_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(
                      event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_DATA_A1_CODE),
            0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02100 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_02200
 * @tc.name      : test getFileTypes in data_ability_helper
 * @tc.desc      : start data ability by data ability in different app. Verify that the getFileTypes function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_02200, Function | MediumTest | Level1)
{
    // data_ability_helper.h	String[] getFileTypes(Uri, String) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(
                      event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_DATA_A1_CODE),
            0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02200 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_02300
 * @tc.name      : test getFileTypes in data_ability_helper
 * @tc.desc      : start data ability by service ability in the same app. Verify that the getFileTypes function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_02300, Function | MediumTest | Level1)
{
    // data_ability_helper.h	String[] getFileTypes(Uri, String) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(
                      event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_SERVICE_A_CODE),
            0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02300 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_02400
 * @tc.name      : test getFileTypes in data_ability_helper
 * @tc.desc      : start data ability by service ability in different app. Verify that the getFileTypes function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_02400, Function | MediumTest | Level1)
{
    // data_ability_helper.h	String[] getFileTypes(Uri, String) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(
                      event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_SERVICE_A_CODE),
            0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02400 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_02500
 * @tc.name      : test getType in data_ability_helper
 * @tc.desc      : start data ability by page ability in the same app. Verify that the getType function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_02500, Function | MediumTest | Level1)
{
    // data_ability_helper.h	String getType(Uri) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETTYPE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_GETTYPE + " ", ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02500 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_02600
 * @tc.name      : test getType in data_ability_helper
 * @tc.desc      : start data ability by page ability in different app. Verify that the getType function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_02600, Function | MediumTest | Level1)
{
    // data_ability_helper.h	String getType(Uri) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETTYPE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_GETTYPE + " ", ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02600 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_02700
 * @tc.name      : test getType in data_ability_helper
 * @tc.desc      : start data ability by data ability in the same app. Verify that the getType function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_02700, Function | MediumTest | Level1)
{
    // data_ability_helper.h	String getType(Uri) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_GETTYPE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_GETTYPE + " ", ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02700 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_02800
 * @tc.name      : test getType in data_ability_helper
 * @tc.desc      : start data ability by data ability in different app. Verify that the getType function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_02800, Function | MediumTest | Level1)
{
    // data_ability_helper.h	String getType(Uri) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_GETTYPE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_GETTYPE + " ", ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02800 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_02900
 * @tc.name      : test getType in data_ability_helper
 * @tc.desc      : start data ability by service ability in the same app. Verify that the getType function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_02900, Function | MediumTest | Level1)
{
    // data_ability_helper.h	String getType(Uri) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETTYPE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_GETTYPE + " ", ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_02900 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_03000
 * @tc.name      : test getType in data_ability_helper
 * @tc.desc      : start data ability by service ability in different app. Verify that the getType function in
 * data_ability_helper isexecuted.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_03000, Function | MediumTest | Level1)
{
    // data_ability_helper.h	String getType(Uri) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_GETTYPE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_GETTYPE + " ", ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03000 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_03100
 * @tc.name      : test query in data_ability_helper
 * @tc.desc      : start data ability by page ability in the same app. Verify that the query function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_03100, Function | MediumTest | Level1)
{
    // data_ability_helper.h	ResultSet query(Uri, string[], DataAbilityPredicates) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03100 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_03200
 * @tc.name      : test query in data_ability_helper
 * @tc.desc      : start data ability by page ability in different app. Verify that the query function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_03200, Function | MediumTest | Level1)
{
    // data_ability_helper.h	ResultSet query(Uri, string[], DataAbilityPredicates) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03200 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_03300
 * @tc.name      : test query in data_ability_helper
 * @tc.desc      : start data ability by data ability in the same app. Verify that the query function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_03300, Function | MediumTest | Level1)
{
    // data_ability_helper.h	ResultSet query(Uri, string[], DataAbilityPredicates) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_QUERY);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A2_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A2_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03300 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_03400
 * @tc.name      : test query in data_ability_helper
 * @tc.desc      : start data ability by data ability in different app. Verify that the query function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_03400, Function | MediumTest | Level1)
{
    // data_ability_helper.h	ResultSet query(Uri, string[], DataAbilityPredicates) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_QUERY);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_B_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03400 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_03500
 * @tc.name      : test query in data_ability_helper
 * @tc.desc      : start data ability by service ability in the same app. Verify that the query function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_03500, Function | MediumTest | Level1)
{
    // data_ability_helper.h	ResultSet query(Uri, string[], DataAbilityPredicates) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03500 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_03600
 * @tc.name      : test query in data_ability_helper
 * @tc.desc      : start data ability by service ability in different app. Verify that the query function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_03600, Function | MediumTest | Level1)
{
    // data_ability_helper.h	ResultSet query(Uri, string[], DataAbilityPredicates) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_QUERY);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_B_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03600 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_03700
 * @tc.name      : test creator in data_ability_helper
 * @tc.desc      : start data ability by page ability in the same app. Verify that the creator function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_03700, Function | MediumTest | Level1)
{
    // data_ability_helper.h	DataAbilityHelper creator(Context, Uri, bool) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_PAGE_A_CODE),
            0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03700 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_03800
 * @tc.name      : test creator in data_ability_helper
 * @tc.desc      : start data ability by page ability in different app. Verify that the creator function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_03800, Function | MediumTest | Level1)
{
    // data_ability_helper.h	DataAbilityHelper creator(Context, Uri, bool) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_PAGE_B_CODE),
            0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03800 end";
}

/**
 * @tc.number    : AMS_DataAbilityHelper_03900
 * @tc.name      : test creator in data_ability_helper
 * @tc.desc      : start data ability by data ability in the same app. Verify that the creator function in
 * data_ability_helper is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataAbilityHelper_03900, Function | MediumTest | Level1)
{
    // data_ability_helper.h	DataAbilityHelper creator(Context, Uri, bool) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_KIT_DATA_A2_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(
                      event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_KIT_DATA_A1_CODE),
            0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataAbilityHelper_03900 end";
}

/**
 * @tc.number    : AMS_Data_Lifecycle_00100
 * @tc.name      : test getLifecycle in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in the same app. Verify that the getLifecycle function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Lifecycle_00100, Function | MediumTest | Level1)
{
    // ability_lifecycle.h	Lifecycle getLifecycle() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00100 end";
}

/**
 * @tc.number    : AMS_Data_Lifecycle_00200
 * @tc.name      : test getLifecycle in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the getLifecycle function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Lifecycle_00200, Function | MediumTest | Level1)
{
    // ability_lifecycle.h	Lifecycle getLifecycle() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_CALLBACKS_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00200 end";
}

/**
 * @tc.number    : AMS_Data_Lifecycle_00300
 * @tc.name      : test addObserver in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the addObserver function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Lifecycle_00300, Function | MediumTest | Level1)
{
    // ability_lifecycle.h	void addObserver(ILifecycleObserver) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00300 end";
}

/**
 * @tc.number    : AMS_Data_Lifecycle_00400
 * @tc.name      : test removeObserver in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the removeObserver function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Lifecycle_00400, Function | MediumTest | Level1)
{
    // ability_lifecycle.h	void removeObserver(ILifecycleObserver) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA3", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A3), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A3_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest ams_kit_data_test_195 end";
}

/**
 * @tc.number    : AMS_Data_Lifecycle_00500
 * @tc.name      : test dispatchLifecycle in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the dispatchLifecycle function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Lifecycle_00500, Function | MediumTest | Level1)
{
    // ability_lifecycle.h	void dispatchLifecycle(Lifecycle.Event, Intent) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_TEST_LIFECYCLE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_OBSERVER_A1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00500 end";
}

/**
 * @tc.number    : AMS_Data_Lifecycle_00600
 * @tc.name      : test dispatchLifecycle in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the dispatchLifecycle function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Lifecycle_00600, Function | MediumTest | Level1)
{
    // ability_lifecycle.h	void dispatchLifecycle(Lifecycle.Event, Intent) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_TEST_LIFECYCLE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_OBSERVER_A1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00600 end";
}

/**
 * @tc.number    : AMS_Data_Lifecycle_00700
 * @tc.name      : test dispatchLifecycle in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the dispatchLifecycle function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Lifecycle_00700, Function | MediumTest | Level1)
{
    // ability_lifecycle.h	void dispatchLifecycle(Lifecycle.Event, Intent) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_TEST_LIFECYCLE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00700 end";
}

/**
 * @tc.number    : AMS_Data_Lifecycle_00800
 * @tc.name      : test dispatchLifecycle in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the dispatchLifecycle function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Lifecycle_00800, Function | MediumTest | Level1)
{
    // ability_lifecycle.h	void dispatchLifecycle(Lifecycle.Event, Intent) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_TEST_LIFECYCLE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_OBSERVER_A1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00800 end";
}

/**
 * @tc.number    : AMS_Data_Lifecycle_00900
 * @tc.name      : test dispatchLifecycle in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the dispatchLifecycle function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Lifecycle_00900, Function | MediumTest | Level1)
{
    // ability_lifecycle.h	void dispatchLifecycle(Lifecycle.Event, Intent) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_TEST_LIFECYCLE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_00900 end";
}

/**
 * @tc.number    : AMS_Data_Lifecycle_01000
 * @tc.name      : test dispatchLifecycle in ability_lifecycle_callbacks
 * @tc.desc      : start data ability by page ability in different app. Verify that the dispatchLifecycle function in
 * ability_lifecycle is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_Lifecycle_01000, Function | MediumTest | Level1)
{
    // ability_lifecycle.h	void dispatchLifecycle(Lifecycle.Event, Intent) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_01000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_TEST_LIFECYCLE);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_OBSERVER_A1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_Lifecycle_01000 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_00100
 * @tc.name      : test onActive in lifecycle_observer
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onActive function in
 * lifecycle_observer is executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_00100, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onActive() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00100 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_00200
 * @tc.name      : test onActive in lifecycle_observer
 * @tc.desc      : start data ability by page ability in different app. Verify that the onActive function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_00200, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onActive() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00200 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_00300
 * @tc.name      : test onActive in lifecycle_observer
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onActive function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_00300, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onActive() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_OBSERVER_A2, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00300s end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_00400
 * @tc.name      : test onActive in lifecycle_observer
 * @tc.desc      : start data ability by data ability in different app. Verify that the onActive function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_00400, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onActive() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_OBSERVER_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00400 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_00500
 * @tc.name      : test onActive in lifecycle_observer
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onActive function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_00500, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onActive() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00500 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_00600
 * @tc.name      : test onActive in lifecycle_observer
 * @tc.desc      : start data ability by service ability in different app. Verify that the onActive function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_00600, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onActive() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONACTIVE, LIFECYCLE_OBSERVER_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00600 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_00700
 * @tc.name      : test onInactive in lifecycle_observer
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onInactive function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_00700, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onInactive() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00700 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_00800
 * @tc.name      : test onInactive in lifecycle_observer
 * @tc.desc      : start data ability by page ability in different app. Verify that the onInactive function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_00800, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onInactive()  002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00800 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_00900
 * @tc.name      : test onInactive in lifecycle_observer
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onInactive function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_00900, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onInactive() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_OBSERVER_A2, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_00900 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_01000
 * @tc.name      : test onInactive in lifecycle_observer
 * @tc.desc      : start data ability by data ability in different app. Verify that the onInactive function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_01000, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onInactive() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_OBSERVER_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01000 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_01100
 * @tc.name      : test onInactive in lifecycle_observer
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onInactive function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_01100, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onInactive() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01100 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_01200
 * @tc.name      : test onInactive in lifecycle_observer
 * @tc.desc      : start data ability by service ability in different app. Verify that the onInactive function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_01200, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onInactive() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONINACTIVE, LIFECYCLE_OBSERVER_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01200 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_01300
 * @tc.name      : test onForeground in lifecycle_observer
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onForeground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_01300, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onForeground(Intent) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01300 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_01400
 * @tc.name      : test onForeground in lifecycle_observer
 * @tc.desc      : start data ability by page ability in different app. Verify that the onForeground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_01400, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onForeground(Intent)  002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01400 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_01500
 * @tc.name      : test onForeground in lifecycle_observer
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onForeground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_01500, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onForeground(Intent) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A2, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01500s end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_01600
 * @tc.name      : test onForeground in lifecycle_observer
 * @tc.desc      : start data ability by data ability in different app. Verify that the onForeground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_01600, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onForeground(Intent) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01600 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_01700
 * @tc.name      : test onForeground in lifecycle_observer
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onForeground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_01700, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onForeground(Intent) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01700 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_01800
 * @tc.name      : test onForeground in lifecycle_observer
 * @tc.desc      : start data ability by service ability in different app. Verify that the onForeground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_01800, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onForeground(Intent) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01800 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_01900
 * @tc.name      : test onForeground in lifecycle_observer
 * @tc.desc      : start data ability twice by page ability in the same app. Verify that the onForeground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_01900, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onForeground(Intent) 007
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_01900 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_02000
 * @tc.name      : test onForeground in lifecycle_observer
 * @tc.desc      : start data ability twice by page ability in different app. Verify that the onForeground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_02000, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onForeground(Intent)  008
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02000 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_02100
 * @tc.name      : test onForeground in lifecycle_observer
 * @tc.desc      : start data ability twice by data ability in the same app. Verify that the onForeground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_02100, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onForeground(Intent) 009
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A2, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A2, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02100 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_02200
 * @tc.name      : test onForeground in lifecycle_observer
 * @tc.desc      : start data ability twice by data ability in different app. Verify that the onForeground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_02200, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onForeground(Intent) 010
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);

    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02200 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_02300
 * @tc.name      : test onForeground in lifecycle_observer
 * @tc.desc      : start data ability twice by service ability in the same app. Verify that the onForeground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_02300, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onForeground(Intent) 011
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02300 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_02400
 * @tc.name      : test onForeground in lifecycle_observer
 * @tc.desc      : start data ability twice by service ability in different app. Verify that the onForeground function
 * in lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_02400, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onForeground(Intent) 012
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONFOREGROUND, LIFECYCLE_OBSERVER_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02400 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_02500
 * @tc.name      : test onStop in lifecycle_observer
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onStop function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_02500, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStop() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_OBSERVER_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02500 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_02600
 * @tc.name      : test onStop in lifecycle_observer
 * @tc.desc      : start data ability by page ability in different app. Verify that the onStop function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_02600, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStop() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02600 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_02700
 * @tc.name      : test onStop in lifecycle_observer
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onStop function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_02700, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStop() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_OBSERVER_A2, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02700 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_02800
 * @tc.name      : test onStop in lifecycle_observer
 * @tc.desc      : start data ability by data ability in different app. Verify that the onStop function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_02800, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStop() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_OBSERVER_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02800 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_02900
 * @tc.name      : test onStop in lifecycle_observer
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onStop function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_02900, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStop() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_02900 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_03000
 * @tc.name      : test onStop in lifecycle_observer
 * @tc.desc      : start data ability by service ability in different app. Verify that the onStop function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_03000, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStop() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTOP, LIFECYCLE_OBSERVER_B, 1), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03000 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_03100
 * @tc.name      : test onStateChanged in lifecycle_observer
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onStateChanged function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_03100, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStateChanged(Lifecycle.Event, Intent) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_OBSERVER_A1, 10), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03100 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_03200
 * @tc.name      : test onStateChanged in lifecycle_observer
 * @tc.desc      : start data ability by page ability in different app. Verify that the onStateChanged function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_03200, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStateChanged(Lifecycle.Event, Intent) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_OBSERVER_A1, 10), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03200 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_03300
 * @tc.name      : test onStateChanged in lifecycle_observer
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onStateChanged function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_03300, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStateChanged(Lifecycle.Event, Intent) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_OBSERVER_A2, 10), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03300 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_03400
 * @tc.name      : test onStateChanged in lifecycle_observer
 * @tc.desc      : start data ability by data ability in different app. Verify that the onStateChanged function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_03400, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStateChanged(Lifecycle.Event, Intent) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);

    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_OBSERVER_B, 10), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03400 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_03500
 * @tc.name      : test onStateChanged in lifecycle_observer
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onStateChanged function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_03500, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStateChanged(Lifecycle.Event, Intent) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_OBSERVER_A1, 10), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03500 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_03600
 * @tc.name      : test onStateChanged in lifecycle_observer
 * @tc.desc      : start data ability by service ability in different app. Verify that the onStateChanged function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_03600, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStateChanged(Lifecycle.Event, Intent) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_CHANGE, LIFECYCLE_OBSERVER_B, 10), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03600 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_03700
 * @tc.name      : test onBackground in lifecycle_observer
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onBackground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_03700, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onBackground() 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_OBSERVER_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03700 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_03800
 * @tc.name      : test onBackground in lifecycle_observer
 * @tc.desc      : start data ability by page ability in different app. Verify that the onBackground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_03800, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onBackground() 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03800 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_03900
 * @tc.name      : test onBackground in lifecycle_observer
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onBackground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_03900, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onBackground() 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_OBSERVER_A2, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_03900 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_04000
 * @tc.name      : test onBackground in lifecycle_observer
 * @tc.desc      : start data ability by data ability in different app. Verify that the onBackground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_04000, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onBackground() 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_OBSERVER_B, 1), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04000 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_04100
 * @tc.name      : test onBackground in lifecycle_observer
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onBackground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_04100, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onBackground() 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_OBSERVER_A1, 1), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04100 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_04200
 * @tc.name      : test onBackground in lifecycle_observer
 * @tc.desc      : start data ability by service ability in different app. Verify that the onBackground function in
 * lifecycle_observer is not executed.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_04200, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onBackground() 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONBACKGROUND, LIFECYCLE_OBSERVER_B, 1), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04200 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_04300
 * @tc.name      : test onStart in lifecycle_observer
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onStart function in
 * lifecycle_observer is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_04300, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStart(Intent) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04300 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_04400
 * @tc.name      : test onStart in lifecycle_observer
 * @tc.desc      : start data ability by page ability in different app. Verify that the onStart function in
 * lifecycle_observer is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_04400, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStart(Intent) 002
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04400 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_04500
 * @tc.name      : test onStart in lifecycle_observer
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onStart function in
 * lifecycle_observer is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_04500, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStart(Intent) 003
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A2), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04500 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_04600
 * @tc.name      : test onStart in lifecycle_observer
 * @tc.desc      : start data ability by data ability in different app. Verify that the onStart function in
 * lifecycle_observer is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_04600, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStart(Intent) 004
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_B), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04600 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_04700
 * @tc.name      : test onStart in lifecycle_observer
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onStart function in
 * lifecycle_observer is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_04700, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStart(Intent) 005
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04700 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_04800
 * @tc.name      : test onStart in lifecycle_observer
 * @tc.desc      : start data ability by service ability in different app. Verify that the onStart function in
 * lifecycle_observer is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_04800, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStart(Intent) 006
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_B), 0);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04800 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_04900
 * @tc.name      : test onStart in lifecycle_observer
 * @tc.desc      : start data ability by page ability in the same app. Verify that the onStart function in
 * lifecycle_observer is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_04900, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStart(Intent) 007
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);

        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_04900 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_05000
 * @tc.name      : test onStart in lifecycle_observer
 * @tc.desc      : start data ability by page ability in different app. Verify that the onStart function in
 * lifecycle_observer is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_05000, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStart(Intent) 008
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_05000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName = ABILITY_NAME_BASE + "PageB";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_B_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_PAGE_B_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_05000 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_05100
 * @tc.name      : test onStart in lifecycle_observer
 * @tc.desc      : start data ability by data ability in the same app. Verify that the onStart function in
 * lifecycle_observer is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_05100, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStart(Intent) 009
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_05100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A2), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A2, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_05100 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_05200
 * @tc.name      : test onStart in lifecycle_observer
 * @tc.desc      : start data ability by data ability in different app. Verify that the onStart function in
 * lifecycle_observer is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_05200, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStart(Intent) 010
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_05200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);

    first->AddChildOperator(second);
    second->AddChildOperator(third);
    second->AddChildOperator(third1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_KIT_PAGE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_KIT_PAGE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_KIT_DATA_A1_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_DATA_A1_CODE, DATA_STATE_QUERY);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_B), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_KIT_PAGE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_05200 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_05300
 * @tc.name      : test onStart in lifecycle_observer
 * @tc.desc      : start data ability by service ability in the same app. Verify that the onStart function in
 * lifecycle_observer is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_05300, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStart(Intent) 011
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_05300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_A1, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_A1_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_05300 end";
}

/**
 * @tc.number    : AMS_Data_LifeCycleObserver_05400
 * @tc.name      : test onStart in lifecycle_observer
 * @tc.desc      : start data ability by service ability in different app. Verify that the onStart function in
 * lifecycle_observer is executed only once.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_Data_LifeCycleObserver_05400, Function | MediumTest | Level1)
{
    // lifecycle_observer.h	void onStart(Intent) 012
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_05400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "ServiceA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    first->AddChildOperator(second1);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        GTEST_LOG_(INFO) << "StartAbility start";
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
        GTEST_LOG_(INFO) << eCode;
        GTEST_LOG_(INFO) << "StartAbility done";
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONSTART, ABILITY_KIT_SERVICE_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, SERVICE_STATE_ONCOMMAND, ABILITY_KIT_SERVICE_A_CODE), 0);
        PublishEvent(TEST_EVENT_NAME, ABILITY_KIT_SERVICE_A_CODE, SERVICE_STATE_ONACTIVE);

        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_B), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);
        EXPECT_NE(TestWaitCompleted(event, DATA_STATE_ONSTART, LIFECYCLE_OBSERVER_B, 1), 0);
        EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_KIT_DATA_B_CODE), 0);

        EXPECT_EQ(
            TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_KIT_SERVICE_A_CODE), 0);
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_Data_LifeCycleObserver_05400 end";
}

/**
 * @tc.number    : AMS_DataUriUtils_0100
 * @tc.name      : test attachId in data_uri_utils
 * @tc.desc      : Verify that the attachId function in data_uri_utils is executed. Verify that the result is right.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataUriUtils_0100, Function | MediumTest | Level1)
{
    // data_uri_utils.h	Uri attachId(Uri, long) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataUriUtils_0100 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        Uri dataAbilityUri("dataability:///" + bundleName + "." + abilityName);

        long Id = DEFAULT_ID;
        dataAbilityUri = DataUriUtils::AttachId(dataAbilityUri, Id);

        EXPECT_TRUE(DataUriUtils::IsAttachedId(dataAbilityUri));

        EXPECT_EQ(Id, DataUriUtils::GetId(dataAbilityUri));
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataUriUtils_0100 end";
}

/**
 * @tc.number    : AMS_DataUriUtils_0200
 * @tc.name      : test getId in data_uri_utils
 * @tc.desc      : Verify that the getId function in data_uri_utils is executed. Verify that the result is right.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataUriUtils_0200, Function | MediumTest | Level1)
{
    // data_uri_utils.h	long getId(Uri) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataUriUtils_0200 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        Uri dataAbilityUri("dataability:///" + bundleName + "." + abilityName);

        const int errorCode = -1;
        EXPECT_EQ(errorCode, DataUriUtils::GetId(dataAbilityUri));

        long Id = DEFAULT_ID;
        dataAbilityUri = DataUriUtils::AttachId(dataAbilityUri, Id);

        EXPECT_TRUE(DataUriUtils::IsAttachedId(dataAbilityUri));

        EXPECT_EQ(Id, DataUriUtils::GetId(dataAbilityUri));
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataUriUtils_0200 end";
}

/**
 * @tc.number    : AMS_DataUriUtils_0300
 * @tc.name      : test deleteId in data_uri_utils
 * @tc.desc      : Verify that the deleteId function in data_uri_utils is executed. Verify that the result is right.
 */
HWTEST_F(ActsAmsKitDataTest, AMS_DataUriUtils_0300, Function | MediumTest | Level1)
{
    // data_uri_utils.h	Uri deleteId(Uri) 001
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataUriUtils_0300 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        Uri dataAbilityUri("dataability:///" + bundleName + "." + abilityName);

        long Id = DEFAULT_ID;
        dataAbilityUri = DataUriUtils::AttachId(dataAbilityUri, Id);
        GTEST_LOG_(INFO) << "DataUriUtils::AttachId(dataAbilityUri, Id)";

        EXPECT_TRUE(DataUriUtils::IsAttachedId(dataAbilityUri));
        GTEST_LOG_(INFO) << "EXPECT_TRUE(DataUriUtils::IsAttachedId(dataAbilityUri))";

        EXPECT_EQ(Id, DataUriUtils::GetId(dataAbilityUri));
        GTEST_LOG_(INFO) << "EXPECT_EQ(Id, DataUriUtils::GetId(dataAbilityUri))";

        dataAbilityUri = DataUriUtils::DeleteId(dataAbilityUri);
        GTEST_LOG_(INFO) << "DataUriUtils::DeleteId(dataAbilityUri)";

        const int errorCode = -1;
        EXPECT_EQ(errorCode, DataUriUtils::GetId(dataAbilityUri));
        GTEST_LOG_(INFO) << "DataUriUtils::GetId(dataAbilityUri))";

        EXPECT_FALSE(DataUriUtils::IsAttachedId(dataAbilityUri));
        GTEST_LOG_(INFO) << "DataUriUtils::IsAttachedId(dataAbilityUri)";
        ReInstallBundle();
    }
    GTEST_LOG_(INFO) << "ActsAmsKitDataTest AMS_DataUriUtils_0300 end";
}
}  // namespace