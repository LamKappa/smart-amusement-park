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
#include <queue>
#include <set>
#include <thread>

#include "ability_lifecycle.h"
#include "ability_lifecycle_executor.h"
#include "ability_manager_errors.h"
#include "ability_manager_service.h"
#include "app_mgr_service.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "event.h"
#include "hilog_wrapper.h"
#include "kit_test_common_info.h"
#include "module_test_dump_util.h"
#include "sa_mgr_client.h"
#include "semaphore_ex.h"
#include "skills.h"
#include "stoperator.h"
#include "system_ability_definition.h"
#include "system_test_ability_util.h"
#include "testConfigParser.h"
#include "uri.h"
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
static const string KIT_BUNDLE_NAME = "com.ohos.amsst.AppKit";
static const string KIT_HAP_NAME = "amsKitSystemTest";
static const string FIRST_ABILITY_NAME = "MainAbility";
static const string SECOND_ABILITY_NAME = "SecondAbility";
static const string THIRD_ABILITY_NAME = "MainAbility";
static constexpr int WAIT_TIME = 1;

static const int MAIN_ABILITY_A_CODE = 100;
static const int SECOND_ABILITY_A_CODE = 200;
static const int MAIN_ABILITY_B_CODE = 300;

static string g_eventMessage = "";
int g_StLevel = 1;
class ActsAmsKitATest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static void Reinstall(const std::string &hapName, const std::string &bundleName);
    void ResetSystem() const;
    static bool SubscribeEvent();
    static int TestWaitCompleted(Event &event, const std::string &eventName, const int code, const int timeout = 10);
    static void TestCompleted(Event &event, const std::string &eventName, const int code);

    class AppEventSubscriber : public CommonEventSubscriber {
    public:
        explicit AppEventSubscriber(const CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp){};
        virtual void OnReceiveEvent(const CommonEventData &data) override;
        ~AppEventSubscriber(){};
    };

    static sptr<IAbilityManager> g_abilityMs;
    static Event event;
    static StressTestLevel stLevel_;
};

StressTestLevel ActsAmsKitATest::stLevel_{};
Event ActsAmsKitATest::event = Event();
sptr<IAbilityManager> ActsAmsKitATest::g_abilityMs = nullptr;

void ActsAmsKitATest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "OnReceiveEvent: event=" << data.GetWant().GetAction();
    GTEST_LOG_(INFO) << "OnReceiveEvent: data=" << data.GetData();
    GTEST_LOG_(INFO) << "OnReceiveEvent: code=" << data.GetCode();
    if (data.GetWant().GetAction() == g_EVENT_RESP_FIRST_LIFECYCLE ||
        data.GetWant().GetAction() == g_EVENT_RESP_FIRSTB_LIFECYCLE ||
        data.GetWant().GetAction() == g_EVENT_RESP_SECOND_LIFECYCLE) {
        TestCompleted(event, data.GetData(), data.GetCode());
    } else if (data.GetWant().GetAction() == g_EVENT_RESP_FIRST || data.GetWant().GetAction() == g_EVENT_RESP_FIRSTB ||
               data.GetWant().GetAction() == g_EVENT_RESP_SECOND) {
        g_eventMessage = data.GetData();
        TestCompleted(event, data.GetWant().GetAction(), data.GetCode());
        GTEST_LOG_(INFO) << "OnReceiveEvent: g_eventMessage=" << data.GetData();
    }
}

int ActsAmsKitATest::TestWaitCompleted(Event &event, const std::string &eventName, const int code, const int timeout)
{
    GTEST_LOG_(INFO) << "TestWaitCompleted : " << eventName << " " << code;
    return STAbilityUtil::WaitCompleted(event, eventName, code, timeout);
}

void ActsAmsKitATest::TestCompleted(Event &event, const std::string &eventName, const int code)
{
    GTEST_LOG_(INFO) << "TestCompleted : " << eventName << " " << code;
    return STAbilityUtil::Completed(event, eventName, code);
}

void ActsAmsKitATest::SetUpTestCase(void)
{
    if (!SubscribeEvent()) {
        GTEST_LOG_(INFO) << "SubscribeEvent error";
    }
    Reinstall(KIT_HAP_NAME + "A", KIT_BUNDLE_NAME + "A");
    Reinstall(KIT_HAP_NAME + "B", KIT_BUNDLE_NAME + "B");
    TestConfigParser tcp;
    tcp.ParseFromFile4StressTest(STRESS_TEST_CONFIG_FILE_PATH, stLevel_);
    std::cout << "stress test level : "
              << "AMS : " << stLevel_.AMSLevel << " "
              << "BMS : " << stLevel_.BMSLevel << " "
              << "CES : " << stLevel_.CESLevel << std::endl;
    g_StLevel = stLevel_.AMSLevel;
    g_StLevel = 5;
}

void ActsAmsKitATest::TearDownTestCase(void)
{
    STAbilityUtil::Uninstall(KIT_BUNDLE_NAME + "A");
    STAbilityUtil::Uninstall(KIT_BUNDLE_NAME + "B");
}

static int CODE_ = 0;
void ActsAmsKitATest::SetUp(void)
{
    ResetSystem();
    Reinstall(KIT_HAP_NAME + "A", KIT_BUNDLE_NAME + "A");
    Reinstall(KIT_HAP_NAME + "B", KIT_BUNDLE_NAME + "B");
    STAbilityUtil::CleanMsg(event);

    CODE_++;
}

void ActsAmsKitATest::TearDown(void)
{
    STAbilityUtil::Uninstall(KIT_BUNDLE_NAME + "A");
    STAbilityUtil::Uninstall(KIT_BUNDLE_NAME + "B");
    STAbilityUtil::CleanMsg(event);
}

bool ActsAmsKitATest::SubscribeEvent()
{
    std::vector<std::string> eventList = {g_EVENT_RESP_FIRST_LIFECYCLE,
        g_EVENT_RESP_SECOND_LIFECYCLE,
        g_EVENT_RESP_FIRSTB_LIFECYCLE,
        g_EVENT_RESP_FIRST,
        g_EVENT_RESP_SECOND,
        g_EVENT_RESP_FIRSTB};
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    auto subscriber = std::make_shared<AppEventSubscriber>(subscribeInfo);
    return CommonEventManager::SubscribeCommonEvent(subscriber);
}

void ActsAmsKitATest::Reinstall(const std::string &hapName, const std::string &bundleName)
{
    STAbilityUtil::Uninstall(bundleName);
    STAbilityUtil::Install(hapName);
}

void ActsAmsKitATest::ResetSystem() const
{
    GTEST_LOG_(INFO) << "ResetSystem";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_00100
 * @tc.name      : test getApplicationInfo in ability_context.h
 * @tc.desc      : Verify that the result of getApplicationInfo function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_00100, Function | MediumTest | Level1)
{
    // ability_context.h	ApplicationInfo* getApplicationInfo() 001
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00100 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetApplicationInfo) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string appInfo = g_eventMessage;

        GTEST_LOG_(INFO) << appInfo;
        EXPECT_EQ(appInfo, KIT_BUNDLE_NAME + "A");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00100 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_00200
 * @tc.name      : test getApplicationInfo in ability_context.h
 * @tc.desc      : Verify that the result of getApplicationInfo function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_00200, Function | MediumTest | Level1)
{
    // ability_context.h	ApplicationInfo* getApplicationInfo() 002
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00200 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetApplicationInfo) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string appInfo1 = g_eventMessage;
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetApplicationInfo) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string appInfo2 = g_eventMessage;

        GTEST_LOG_(INFO) << appInfo1;
        GTEST_LOG_(INFO) << appInfo2;

        EXPECT_EQ(appInfo1, KIT_BUNDLE_NAME + "A");
        EXPECT_EQ(appInfo1, appInfo2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00200 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_00300
 * @tc.name      : test getApplicationInfo in ability_context.h
 * @tc.desc      : Verify that the result of getApplicationInfo function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_00300, Function | MediumTest | Level1)
{
    // ability_context.h	ApplicationInfo* getApplicationInfo() 003
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00300 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetApplicationInfo) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string appInfo1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetApplicationInfo) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string appInfo2 = g_eventMessage;

        GTEST_LOG_(INFO) << appInfo1;
        GTEST_LOG_(INFO) << appInfo2;

        EXPECT_EQ(appInfo1, KIT_BUNDLE_NAME + "A");
        EXPECT_EQ(appInfo2, KIT_BUNDLE_NAME + "B");
        EXPECT_NE(appInfo1, appInfo2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00300 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_00400
 * @tc.name      : test GetCacheDir in ability_context.h
 * @tc.desc      : Verify that the result of GetCacheDir function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_00400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00400 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCacheDir) + "_0");
        TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_);
        string cacheDir = g_eventMessage;

        GTEST_LOG_(INFO) << cacheDir;
        EXPECT_NE(cacheDir, "");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00400 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_00500
 * @tc.name      : test GetCacheDir in ability_context.h
 * @tc.desc      : Verify that the result of GetCacheDir function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_00500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00500 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCacheDir) + "_1");
        TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_);
        string cacheDir1 = g_eventMessage;
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCacheDir) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string cacheDir2 = g_eventMessage;

        GTEST_LOG_(INFO) << cacheDir1;
        GTEST_LOG_(INFO) << cacheDir2;
        EXPECT_NE(cacheDir1, "");
        EXPECT_EQ(cacheDir1, cacheDir2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00500 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_00600
 * @tc.name      : test GetCacheDir in ability_context.h
 * @tc.desc      : Verify that the result of GetCacheDir function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_00600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00600 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCacheDir) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string cacheDir1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCacheDir) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string cacheDir2 = g_eventMessage;

        GTEST_LOG_(INFO) << cacheDir1;
        GTEST_LOG_(INFO) << cacheDir2;

        EXPECT_NE(cacheDir1, "");
        EXPECT_NE(cacheDir2, "");
        EXPECT_NE(cacheDir1, cacheDir2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00600 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_00700
 * @tc.name      : test GetCodeCacheDir in ability_context.h
 * @tc.desc      : Verify that the result of GetCodeCacheDir function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_00700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00700 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCodeCacheDir) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string codeCacheDir = g_eventMessage;

        GTEST_LOG_(INFO) << codeCacheDir;
        EXPECT_NE(codeCacheDir, "");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00700 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_00800
 * @tc.name      : test GetCodeCacheDir in ability_context.h
 * @tc.desc      : Verify that the result of GetCodeCacheDir function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_00800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00800 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCodeCacheDir) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string codeCacheDir1 = g_eventMessage;
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCodeCacheDir) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string codeCacheDir2 = g_eventMessage;

        GTEST_LOG_(INFO) << codeCacheDir1;
        GTEST_LOG_(INFO) << codeCacheDir2;
        EXPECT_NE(codeCacheDir1, "");
        EXPECT_EQ(codeCacheDir1, codeCacheDir2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00800 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_00900
 * @tc.name      : test GetCodeCacheDir in ability_context.h
 * @tc.desc      : Verify that the result of GetCodeCacheDir function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_00900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00900 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCodeCacheDir) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string codeCacheDir1 = g_eventMessage;
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCodeCacheDir) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string codeCacheDir2 = g_eventMessage;

        GTEST_LOG_(INFO) << codeCacheDir1;
        GTEST_LOG_(INFO) << codeCacheDir2;

        EXPECT_NE(codeCacheDir1, "");
        EXPECT_NE(codeCacheDir2, "");
        EXPECT_NE(codeCacheDir1, codeCacheDir2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_00900 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_01000
 * @tc.name      : test GetDatabaseDir in ability_context.h
 * @tc.desc      : Verify that the result of GetDatabaseDir function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_01000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01000 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDatabaseDir) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string dataBaseDir = g_eventMessage;

        GTEST_LOG_(INFO) << dataBaseDir;

        EXPECT_NE(dataBaseDir, "");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01000 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_01100
 * @tc.name      : test GetDatabaseDir in ability_context.h
 * @tc.desc      : Verify that the result of GetDatabaseDir function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_01100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01100 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDatabaseDir) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string dataBaseDir1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDatabaseDir) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string dataBaseDir2 = g_eventMessage;

        GTEST_LOG_(INFO) << dataBaseDir1;
        GTEST_LOG_(INFO) << dataBaseDir2;

        EXPECT_NE(dataBaseDir1, "");
        EXPECT_EQ(dataBaseDir1, dataBaseDir2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01100 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_01200
 * @tc.name      : test GetDatabaseDir in ability_context.h
 * @tc.desc      : Verify that the result of GetDatabaseDir function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_01200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01200 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDatabaseDir) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string dataBaseDir1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDatabaseDir) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string dataBaseDir2 = g_eventMessage;

        GTEST_LOG_(INFO) << dataBaseDir1;
        GTEST_LOG_(INFO) << dataBaseDir2;

        EXPECT_NE(dataBaseDir1, "");
        EXPECT_NE(dataBaseDir2, "");
        EXPECT_NE(dataBaseDir1, dataBaseDir2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01200 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_01300
 * @tc.name      : test GetDataDir in ability_context.h
 * @tc.desc      : Verify that the result of GetDataDir function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_01300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01300 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDataDir) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string dataDir = g_eventMessage;

        GTEST_LOG_(INFO) << dataDir;

        EXPECT_NE(dataDir, "");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01300 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_01400
 * @tc.name      : test GetDataDir in ability_context.h
 * @tc.desc      : Verify that the result of GetDataDir function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_01400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01400 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDataDir) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string dataDir1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDataDir) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string dataDir2 = g_eventMessage;

        GTEST_LOG_(INFO) << dataDir1;
        GTEST_LOG_(INFO) << dataDir2;

        EXPECT_NE(dataDir1, "");
        EXPECT_EQ(dataDir1, dataDir2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01400 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_01500
 * @tc.name      : test GetDataDir in ability_context.h
 * @tc.desc      : Verify that the result of GetDataDir function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_01500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01500 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDataDir) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string dataDir1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDataDir) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string dataDir2 = g_eventMessage;

        GTEST_LOG_(INFO) << dataDir1;
        GTEST_LOG_(INFO) << dataDir2;

        EXPECT_NE(dataDir1, "");
        EXPECT_NE(dataDir2, "");
        EXPECT_NE(dataDir1, dataDir2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01500 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_01600
 * @tc.name      : test GetDir in ability_context.h
 * @tc.desc      : Verify that the result of GetDir function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_01600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01600 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDir) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string dir = g_eventMessage;

        GTEST_LOG_(INFO) << dir;

        EXPECT_NE(dir, "");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01600 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_01700
 * @tc.name      : test GetDir in ability_context.h
 * @tc.desc      : Verify that the result of GetDir function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_01700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01700 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDir) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string dir1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDir) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string dir2 = g_eventMessage;

        GTEST_LOG_(INFO) << dir1;
        GTEST_LOG_(INFO) << dir2;

        EXPECT_NE(dir1, "");
        EXPECT_EQ(dir1, dir2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01700 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_01800
 * @tc.name      : test GetDir in ability_context.h
 * @tc.desc      : Verify that the result of GetDir function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_01800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01800 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDir) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string dir1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetDir) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string dir2 = g_eventMessage;

        GTEST_LOG_(INFO) << dir1;
        GTEST_LOG_(INFO) << dir2;

        EXPECT_NE(dir1, "");
        EXPECT_NE(dir2, "");
        EXPECT_NE(dir1, dir2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01800 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_01900
 * @tc.name      : test GetBundleManager in ability_context.h
 * @tc.desc      : Verify that the result of GetBundleManager function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_01900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01900 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetBundleManager) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string result = g_eventMessage;

        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(result, "1");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_01900 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_02000
 * @tc.name      : test GetBundleCodePath in ability_context.h
 * @tc.desc      : Verify that the result of GetBundleCodePath function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_02000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02000 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetBundleCodePath) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string bundleCodePath = g_eventMessage;

        GTEST_LOG_(INFO) << bundleCodePath;
        EXPECT_NE(bundleCodePath, "");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02000 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_02100
 * @tc.name      : test GetBundleCodePath in ability_context.h
 * @tc.desc      : Verify that the result of GetBundleCodePath function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_02100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02100 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetBundleCodePath) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string bundleCodePath1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetBundleCodePath) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string bundleCodePath2 = g_eventMessage;

        GTEST_LOG_(INFO) << bundleCodePath1;
        GTEST_LOG_(INFO) << bundleCodePath2;

        EXPECT_NE(bundleCodePath1, "");
        EXPECT_EQ(bundleCodePath1, bundleCodePath2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02100 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_02200
 * @tc.name      : test GetBundleCodePath in ability_context.h
 * @tc.desc      : Verify that the result of GetBundleCodePath function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_02200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02200 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetBundleCodePath) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string bundleCodePath1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetBundleCodePath) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string bundleCodePath2 = g_eventMessage;

        GTEST_LOG_(INFO) << bundleCodePath1;
        GTEST_LOG_(INFO) << bundleCodePath2;

        EXPECT_NE(bundleCodePath1, "");
        EXPECT_NE(bundleCodePath2, "");
        EXPECT_NE(bundleCodePath1, bundleCodePath2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02200 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_02300
 * @tc.name      : test GetBundleName in ability_context.h
 * @tc.desc      : Verify that the result of GetBundleName function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_02300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02300 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetBundleName) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string bundleName = g_eventMessage;

        GTEST_LOG_(INFO) << bundleName;
        EXPECT_NE(bundleName, "");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02300 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_02400
 * @tc.name      : test GetBundleName in ability_context.h
 * @tc.desc      : Verify that the result of GetBundleName function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_02400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02400 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetBundleName) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string bundleName1 = g_eventMessage;
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetBundleName) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string bundleName2 = g_eventMessage;

        GTEST_LOG_(INFO) << bundleName1;
        GTEST_LOG_(INFO) << bundleName2;

        EXPECT_NE(bundleName1, "");
        EXPECT_EQ(bundleName1, bundleName2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02400 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_02500
 * @tc.name      : test GetBundleName in ability_context.h
 * @tc.desc      : Verify that the result of GetBundleName function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_02500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02500 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetBundleName) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string bundleName1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetBundleName) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);

        string bundleName2 = g_eventMessage;

        GTEST_LOG_(INFO) << bundleName1;
        GTEST_LOG_(INFO) << bundleName2;

        EXPECT_NE(bundleName1, "");
        EXPECT_NE(bundleName2, "");
        EXPECT_NE(bundleName1, bundleName2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02500 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_02600
 * @tc.name      : test GetBundleResourcePath in ability_context.h
 * @tc.desc      : Verify that the result of GetBundleResourcePath function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_02600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02600 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIRST,
            CODE_,
            "Ability_" + std::to_string((int)AbilityContextApi::GetBundleResourcePath) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string bundleResourcePath = g_eventMessage;

        GTEST_LOG_(INFO) << bundleResourcePath;
        EXPECT_NE(bundleResourcePath, "");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02600 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_02700
 * @tc.name      : test GetBundleResourcePath in ability_context.h
 * @tc.desc      : Verify that the result of GetBundleResourcePath function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_02700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02700 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIRST,
            CODE_,
            "Ability_" + std::to_string((int)AbilityContextApi::GetBundleResourcePath) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string bundleResourcePath1 = g_eventMessage;

        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND,
            CODE_,
            "Ability_" + std::to_string((int)AbilityContextApi::GetBundleResourcePath) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string bundleResourcePath2 = g_eventMessage;

        GTEST_LOG_(INFO) << bundleResourcePath1;
        GTEST_LOG_(INFO) << bundleResourcePath2;

        EXPECT_NE(bundleResourcePath1, "");
        EXPECT_EQ(bundleResourcePath1, bundleResourcePath2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02700 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_02800
 * @tc.name      : test GetBundleResourcePath in ability_context.h
 * @tc.desc      : Verify that the result of GetBundleResourcePath function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_02800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02800 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIRST,
            CODE_,
            "Ability_" + std::to_string((int)AbilityContextApi::GetBundleResourcePath) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string bundleResourcePath1 = g_eventMessage;

        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIRSTB,
            CODE_,
            "Ability_" + std::to_string((int)AbilityContextApi::GetBundleResourcePath) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string bundleResourcePath2 = g_eventMessage;

        GTEST_LOG_(INFO) << bundleResourcePath1;
        GTEST_LOG_(INFO) << bundleResourcePath2;

        EXPECT_NE(bundleResourcePath1, "");
        EXPECT_NE(bundleResourcePath2, "");
        EXPECT_NE(bundleResourcePath1, bundleResourcePath2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02800 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_02900
 * @tc.name      : test GetApplicationContext in ability_context.h
 * @tc.desc      : Verify that the result of GetApplicationContext function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_02900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02900 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIRST,
            CODE_,
            "Ability_" + std::to_string((int)AbilityContextApi::GetApplicationContext) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string result = g_eventMessage;
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(result, "1");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_02900 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_03000
 * @tc.name      : test GetCallingAbility in ability_context.h
 * @tc.desc      : Verify that the result of GetCallingAbility function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_03000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03000 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
    STAbilityUtil::PublishEvent(
        g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingAbility) + "_0");

    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingAbility) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string callingAbility = g_eventMessage;
        GTEST_LOG_(INFO) << callingAbility;
        EXPECT_NE(callingAbility, "");
    }

    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03000 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_03100
 * @tc.name      : test GetCallingAbility in ability_context.h
 * @tc.desc      : Verify that the result of GetCallingAbility function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_03100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03100 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
    STAbilityUtil::PublishEvent(
        g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingAbility) + "_1");

    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);

    // start second ability
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingAbility) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string callingAbility1 = g_eventMessage;
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingAbility) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string callingAbility2 = g_eventMessage;

        GTEST_LOG_(INFO) << callingAbility1;
        GTEST_LOG_(INFO) << callingAbility2;

        EXPECT_NE(callingAbility1, "");
        EXPECT_NE(callingAbility2, "");
        EXPECT_EQ(callingAbility1, callingAbility2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03100 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_03200
 * @tc.name      : test GetCallingAbility in ability_context.h
 * @tc.desc      : Verify that the result of GetCallingAbility function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_03200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03200 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
    STAbilityUtil::PublishEvent(
        g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingAbility) + "_2");
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);
    STAbilityUtil::PublishEvent(
        g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingAbility) + "_2");
    TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);
    STAbilityUtil::PublishEvent(
        g_EVENT_REQU_FIRSTB, CODE_ + 1, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingAbility) + "_2");
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);
    STAbilityUtil::PublishEvent(
        g_EVENT_REQU_SECOND, CODE_ + 1, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingAbility) + "_2");
    TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_ + 1);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingAbility) + "_2");
        TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_);
        string callingAbility1 = g_eventMessage;
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND,
            CODE_ + 1,
            "Ability_" + std::to_string((int)AbilityContextApi::GetCallingAbility) + "_2");
        TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_ + 1);
        string callingAbility2 = g_eventMessage;

        GTEST_LOG_(INFO) << callingAbility1;
        GTEST_LOG_(INFO) << callingAbility2;

        EXPECT_NE(callingAbility1, "");
        EXPECT_NE(callingAbility2, "");
        EXPECT_NE(callingAbility1, callingAbility2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03200 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_03300
 * @tc.name      : test GetContext in ability_context.h
 * @tc.desc      : Verify that the result of GetContext function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_03300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03300 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetContext) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string result = g_eventMessage;

        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(result, "1");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03300 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_03400
 * @tc.name      : test GetAbilityManager in ability_context.h
 * @tc.desc      : Verify that the result of GetAbilityManager function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_03400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03400 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetAbilityManager) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string result = g_eventMessage;

        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(result, "1");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03400 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_03500
 * @tc.name      : test GetProcessInfo in ability_context.h
 * @tc.desc      : Verify that the result of GetProcessInfo function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_03500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03500 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetProcessInfo) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string processInfo = g_eventMessage;

        GTEST_LOG_(INFO) << processInfo;
        EXPECT_NE(processInfo, "");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03500 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_03600
 * @tc.name      : test GetProcessInfo in ability_context.h
 * @tc.desc      : Verify that the result of GetProcessInfo function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_03600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03600 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetProcessInfo) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string processInfo1 = g_eventMessage;
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetProcessInfo) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string processInfo2 = g_eventMessage;

        GTEST_LOG_(INFO) << processInfo1;
        GTEST_LOG_(INFO) << processInfo2;

        EXPECT_NE(processInfo1, "");
        EXPECT_NE(processInfo2, "");
        EXPECT_EQ(processInfo1, processInfo2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03600 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_03700
 * @tc.name      : test GetProcessInfo in ability_context.h
 * @tc.desc      : Verify that the result of GetProcessInfo function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_03700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03700 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetProcessInfo) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string processInfo1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetProcessInfo) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string processInfo2 = g_eventMessage;

        GTEST_LOG_(INFO) << processInfo1;
        GTEST_LOG_(INFO) << processInfo2;

        EXPECT_NE(processInfo1, "");
        EXPECT_NE(processInfo2, "");
        EXPECT_NE(processInfo1, processInfo2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03700 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_03800
 * @tc.name      : test GetAppType in ability_context.h
 * @tc.desc      : Verify that the result of GetAppType function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_03800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03800 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetAppType) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string appType = g_eventMessage;

        GTEST_LOG_(INFO) << appType;
        EXPECT_EQ(appType, "third-party");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03800 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_03900
 * @tc.name      : test GetAppType in ability_context.h
 * @tc.desc      : Verify that the result of GetAppType function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_03900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03900 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetAppType) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string appType1 = g_eventMessage;
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetAppType) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string appType2 = g_eventMessage;

        GTEST_LOG_(INFO) << appType1;
        GTEST_LOG_(INFO) << appType2;
        EXPECT_EQ(appType1, "third-party");
        EXPECT_EQ(appType2, "third-party");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_03900 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_04000
 * @tc.name      : test GetAppType in ability_context.h
 * @tc.desc      : Verify that the result of GetAppType function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_04000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04000 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);
    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetAppType) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string appType1 = g_eventMessage;
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetAppType) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string appType2 = g_eventMessage;

        GTEST_LOG_(INFO) << appType1;
        GTEST_LOG_(INFO) << appType2;
        EXPECT_EQ(appType1, "third-party");
        EXPECT_EQ(appType2, "third-party");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04000 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_04100
 * @tc.name      : test GetCallingBundle in ability_context.h
 * @tc.desc      : Verify that the result of GetCallingBundle function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_04100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04100 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
    STAbilityUtil::PublishEvent(
        g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingBundle) + "_0");

    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingBundle) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string callingBundle = g_eventMessage;

        GTEST_LOG_(INFO) << callingBundle;
        EXPECT_NE(callingBundle, "");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04100 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_04200
 * @tc.name      : test GetCallingBundle in ability_context.h
 * @tc.desc      : Verify that the result of GetCallingBundle function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_04200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04200 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
    STAbilityUtil::PublishEvent(
        g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingBundle) + "_1");

    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingBundle) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string callingBundle1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingBundle) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string callingBundle2 = g_eventMessage;

        GTEST_LOG_(INFO) << callingBundle1;
        GTEST_LOG_(INFO) << callingBundle2;

        EXPECT_NE(callingBundle1, "");
        EXPECT_NE(callingBundle2, "");
        EXPECT_EQ(callingBundle1, callingBundle2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04200 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_04300
 * @tc.name      : test GetCallingBundle in ability_context.h
 * @tc.desc      : Verify that the result of GetCallingBundle function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_04300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04300 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
    STAbilityUtil::PublishEvent(
        g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingBundle) + "_2");
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);
    STAbilityUtil::PublishEvent(
        g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingBundle) + "_2");
    TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);
    STAbilityUtil::PublishEvent(
        g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingBundle) + "_2");
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);
    STAbilityUtil::PublishEvent(
        g_EVENT_REQU_SECOND, CODE_ + 1, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingBundle) + "_2");
    TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_ + 1);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetCallingBundle) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string callingBundle1 = g_eventMessage;
        STAbilityUtil::PublishEvent(g_EVENT_REQU_SECOND,
            CODE_ + 1,
            "Ability_" + std::to_string((int)AbilityContextApi::GetCallingBundle) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_ + 1), 0);
        string callingBundle2 = g_eventMessage;

        GTEST_LOG_(INFO) << callingBundle1;
        GTEST_LOG_(INFO) << callingBundle2;

        EXPECT_NE(callingBundle1, "");
        EXPECT_NE(callingBundle2, "");
        EXPECT_NE(callingBundle1, callingBundle2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04300 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_04400
 * @tc.name      : test StartAbility_Want_int in ability_context.h
 * @tc.desc      : Verify that the result of StartAbility_Want_int function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_04400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04400 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        sleep(WAIT_TIME);
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
        GTEST_LOG_(INFO) << eCode;

        EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIRST,
            CODE_,
            "Ability_" + std::to_string((int)AbilityContextApi::StartAbility_Want_int) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string result = g_eventMessage;

        EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(result, "startAbility");

        Reinstall(KIT_HAP_NAME + "A", KIT_BUNDLE_NAME + "A");
        Reinstall(KIT_HAP_NAME + "B", KIT_BUNDLE_NAME + "B");
        STAbilityUtil::CleanMsg(event);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04400 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_04500
 * @tc.name      : test StartAbility_Want_int in ability_context.h
 * @tc.desc      : Verify that the result of StartAbility_Want_int function in different application is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_04500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04500 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        sleep(WAIT_TIME);
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
        GTEST_LOG_(INFO) << eCode;

        EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
        STAbilityUtil::PublishEvent(g_EVENT_REQU_FIRST,
            CODE_,
            "Ability_" + std::to_string((int)AbilityContextApi::StartAbility_Want_int) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string result = g_eventMessage;

        EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);

        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(result, "startAbility");
        Reinstall(KIT_HAP_NAME + "A", KIT_BUNDLE_NAME + "A");
        Reinstall(KIT_HAP_NAME + "B", KIT_BUNDLE_NAME + "B");
        STAbilityUtil::CleanMsg(event);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04500 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_04600
 * @tc.name      : test TerminateAbility in ability_context.h
 * @tc.desc      : Verify that the result of TerminateAbility function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_04600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04600 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        sleep(WAIT_TIME);
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
        GTEST_LOG_(INFO) << eCode;

        EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::TerminateAbility) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string result = g_eventMessage;

        EXPECT_EQ(TestWaitCompleted(event, "OnStop", MAIN_ABILITY_A_CODE), 0);

        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(result, "TerminateAbility");
        Reinstall(KIT_HAP_NAME + "A", KIT_BUNDLE_NAME + "A");
        Reinstall(KIT_HAP_NAME + "B", KIT_BUNDLE_NAME + "B");
        STAbilityUtil::CleanMsg(event);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04600 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_04700
 * @tc.name      : test GetElementName in ability_context.h
 * @tc.desc      : Verify that the result of GetElementName function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_04700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04700 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetElementName) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string elementName = g_eventMessage;
        GTEST_LOG_(INFO) << elementName;

        EXPECT_NE(elementName, "");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04700 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_04800
 * @tc.name      : test GetElementName in ability_context.h
 * @tc.desc      : Verify that the result of GetElementName function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_04800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04800 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetElementName) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string elementName1 = g_eventMessage;
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetElementName) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string elementName2 = g_eventMessage;

        GTEST_LOG_(INFO) << elementName1;
        GTEST_LOG_(INFO) << elementName2;
        EXPECT_NE(elementName1, "");
        EXPECT_NE(elementName2, "");
        EXPECT_EQ(elementName1, elementName2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04800 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_04900
 * @tc.name      : test GetElementName in ability_context.h
 * @tc.desc      : Verify that the result of GetElementName function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_04900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04900 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetElementName) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string elementName1 = g_eventMessage;
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetElementName) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string elementName2 = g_eventMessage;

        GTEST_LOG_(INFO) << elementName1;
        GTEST_LOG_(INFO) << elementName2;
        EXPECT_NE(elementName1, "");
        EXPECT_NE(elementName2, "");
        EXPECT_NE(elementName1, elementName2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_04900 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_05000
 * @tc.name      : test GetHapModuleInfo in ability_context.h
 * @tc.desc      : Verify that the result of GetHapModuleInfo function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_05000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_05000 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetHapModuleInfo) + "_0");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string hapModuleInfo = g_eventMessage;

        GTEST_LOG_(INFO) << hapModuleInfo;
        EXPECT_NE(hapModuleInfo, "");
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_05000 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_05100
 * @tc.name      : test GetHapModuleInfo in ability_context.h
 * @tc.desc      : Verify that the result of GetHapModuleInfo function in the same application is equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_05100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_05100 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetElementName) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string hapModuleInfo1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_SECOND, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetElementName) + "_1");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_SECOND, CODE_), 0);
        string hapModuleInfo2 = g_eventMessage;

        // -----------------------------EXPECT----------------------------------
        GTEST_LOG_(INFO) << hapModuleInfo1;
        GTEST_LOG_(INFO) << hapModuleInfo2;
        EXPECT_NE(hapModuleInfo1, "");
        EXPECT_NE(hapModuleInfo2, "");
        EXPECT_EQ(hapModuleInfo1, hapModuleInfo2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_05100 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_05200
 * @tc.name      : test GetHapModuleInfo in ability_context.h
 * @tc.desc      : Verify that the result of GetHapModuleInfo function in different application is not equal.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_05200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_05200 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);
    // start first ability
    sleep(WAIT_TIME);
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;

    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

    // start second ability
    want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);
    sleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
    GTEST_LOG_(INFO) << eCode;
    EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRST, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetElementName) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRST, CODE_), 0);
        string hapModuleInfo1 = g_eventMessage;

        STAbilityUtil::PublishEvent(
            g_EVENT_REQU_FIRSTB, CODE_, "Ability_" + std::to_string((int)AbilityContextApi::GetElementName) + "_2");
        EXPECT_EQ(TestWaitCompleted(event, g_EVENT_RESP_FIRSTB, CODE_), 0);
        string hapModuleInfo2 = g_eventMessage;

        GTEST_LOG_(INFO) << hapModuleInfo1;
        GTEST_LOG_(INFO) << hapModuleInfo2;
        EXPECT_NE(hapModuleInfo1, "");
        EXPECT_NE(hapModuleInfo2, "");
        EXPECT_NE(hapModuleInfo1, hapModuleInfo2);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_05200 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_05300
 * @tc.name      : test REGISTER_AA in ability_context.h
 * @tc.desc      : Verify that the result of REGISTER_AA function is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_05300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_05300 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", FIRST_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        sleep(WAIT_TIME);
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
        GTEST_LOG_(INFO) << eCode;

        EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_A_CODE), 0);

        Reinstall(KIT_HAP_NAME + "A", KIT_BUNDLE_NAME + "A");
        Reinstall(KIT_HAP_NAME + "B", KIT_BUNDLE_NAME + "B");
        STAbilityUtil::CleanMsg(event);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_05300 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_05400
 * @tc.name      : test REGISTER_AA in ability_context.h
 * @tc.desc      : Verify that the result of REGISTER_AA function in different ability is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_05400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_05400 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", SECOND_ABILITY_NAME, KIT_BUNDLE_NAME + "A", params);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        sleep(WAIT_TIME);
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
        GTEST_LOG_(INFO) << eCode;

        EXPECT_EQ(TestWaitCompleted(event, "onStart", SECOND_ABILITY_A_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, "OnActive", SECOND_ABILITY_A_CODE), 0);
        Reinstall(KIT_HAP_NAME + "A", KIT_BUNDLE_NAME + "A");
        Reinstall(KIT_HAP_NAME + "B", KIT_BUNDLE_NAME + "B");
        STAbilityUtil::CleanMsg(event);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_05400 end";
}

/**
 * @tc.number    : AMS_Page_AbilityContext_05500
 * @tc.name      : test REGISTER_AA in ability_context.h
 * @tc.desc      : Verify that the result of REGISTER_AA function in different application is correct.
 */
HWTEST_F(ActsAmsKitATest, AMS_Page_AbilityContext_05500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_05500 start";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", THIRD_ABILITY_NAME, KIT_BUNDLE_NAME + "B", params);

    for (int i = 0; i < g_StLevel; i++) {
        STAbilityUtil::CleanMsg(event);
        sleep(WAIT_TIME);
        ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs, WAIT_TIME);
        GTEST_LOG_(INFO) << eCode;

        EXPECT_EQ(TestWaitCompleted(event, "onStart", MAIN_ABILITY_B_CODE), 0);
        EXPECT_EQ(TestWaitCompleted(event, "OnActive", MAIN_ABILITY_B_CODE), 0);
        Reinstall(KIT_HAP_NAME + "A", KIT_BUNDLE_NAME + "A");
        Reinstall(KIT_HAP_NAME + "B", KIT_BUNDLE_NAME + "B");
        STAbilityUtil::CleanMsg(event);
    }
    GTEST_LOG_(INFO) << "ActsAmsKitATest AMS_Page_AbilityContext_05500 end";
}
}  // namespace