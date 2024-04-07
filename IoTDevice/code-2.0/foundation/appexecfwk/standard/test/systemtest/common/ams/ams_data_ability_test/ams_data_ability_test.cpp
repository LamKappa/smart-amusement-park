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
#include "ams_data_ability_test_def.h"
#include "app_mgr_service.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "event.h"
#include "hilog_wrapper.h"
#include "module_test_dump_util.h"
#include "sa_mgr_client.h"
#include "semaphore_ex.h"
#include "stoperator.h"
#include "system_ability_definition.h"
#include "system_test_ability_util.h"
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

using MAP_STR_STR = std::map<std::string, std::string>;
using VECTOR_STR = std::vector<std::string>;
static const std::string BUNDLE_NAME_BASE = "com.ohos.amsst.AppData";
static const std::string ABILITY_NAME_BASE = "AmsStDataAbility";
static const std::string HAP_NAME_BASE = "amsDataSystemTest";

static const int ABILITY_CODE_BASE = 100;

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

class AmsDataAbilityTest : public testing::Test {
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
                {"Default"},
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
        };
        virtual void OnReceiveEvent(const CommonEventData &data) override;
        std::unordered_map<std::string, int> PageAbilityState_;
        std::set<std::string> DataAbilityState_;
        std::set<std::string> AbilityOperator_;
    };
};

STtools::Event AmsDataAbilityTest::event = STtools::Event();

void AmsDataAbilityTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "OnReceiveEvent: event=" << data.GetWant().GetAction();
    GTEST_LOG_(INFO) << "OnReceiveEvent: data=" << data.GetData();
    GTEST_LOG_(INFO) << "OnReceiveEvent: code=" << data.GetCode();

    std::string eventName = data.GetWant().GetAction();
    if (eventName.compare(ABILITY_EVENT_NAME) == 0) {

        std::string target = data.GetData();
        if (target.find(" ") != target.npos) {
            AmsDataAbilityTest::TestCompleted(event, target, data.GetCode());
            return;
        }

        if (PAGE_ABILITY_CODE == data.GetCode() / ABILITY_CODE_BASE)  // page ability
        {

            EXPECT_TRUE(PageAbilityState_.find(target) != PageAbilityState_.end());
            AmsDataAbilityTest::TestCompleted(event, target, data.GetCode());
            return;
        } else if (DATA_ABILITY_CODE == data.GetCode() / ABILITY_CODE_BASE)  // data ability
        {
            std::string target = data.GetData();
            EXPECT_TRUE(DataAbilityState_.find(target) != DataAbilityState_.end());
            AmsDataAbilityTest::TestCompleted(event, target, data.GetCode());
            return;
        }
    }
}

void AmsDataAbilityTest::SetUpTestCase(void)
{
    std::vector<std::string> bundleNameSuffix = {"A", "B", "C"};
    for (std::string suffix : bundleNameSuffix) {
        STAbilityUtil::Install(HAP_NAME_BASE + suffix);
    }
    if (!SubscribeEvent()) {
        GTEST_LOG_(INFO) << "SubscribeEvent error";
    }
}

void AmsDataAbilityTest::TearDownTestCase(void)
{}

void AmsDataAbilityTest::SetUp(void)
{
    ReInstallBundle();
    STAbilityUtil::CleanMsg(event);
    ResetSystem();

    g_abilityMs = STAbilityUtil::GetAbilityManagerService();
    g_appMs = STAbilityUtil::GetAppMgrService();
}

void AmsDataAbilityTest::TearDown(void)
{
    UnInstallBundle();
    STAbilityUtil::CleanMsg(event);
}

bool AmsDataAbilityTest::SubscribeEvent()
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

void AmsDataAbilityTest::ReInstallBundle() const
{
    std::vector<std::string> bundleNameSuffix = {"A", "B", "C"};
    for (std::string suffix : bundleNameSuffix) {
        STAbilityUtil::KillService(BUNDLE_NAME_BASE + suffix);
        STAbilityUtil::Install(HAP_NAME_BASE + suffix);
    }
}

void AmsDataAbilityTest::UnInstallBundle() const
{
    std::vector<std::string> bundleNameSuffix = {"A", "B", "C"};
    for (std::string suffix : bundleNameSuffix) {
        STAbilityUtil::KillService(BUNDLE_NAME_BASE + suffix);
        STAbilityUtil::Uninstall(BUNDLE_NAME_BASE + suffix);
    }
}

sptr<IAppMgr> AmsDataAbilityTest::g_appMs = nullptr;
sptr<IAbilityManager> AmsDataAbilityTest::g_abilityMs = nullptr;

void AmsDataAbilityTest::ResetSystem() const
{
    GTEST_LOG_(INFO) << "ResetSystem";
}

int AmsDataAbilityTest::TestWaitCompleted(Event &event, const std::string &eventName, const int code, const int timeout)
{
    GTEST_LOG_(INFO) << "TestWaitCompleted : " << eventName << " " << code;
    return STAbilityUtil::WaitCompleted(event, eventName, code, timeout);
}

void AmsDataAbilityTest::TestCompleted(Event &event, const std::string &eventName, const int code)
{
    GTEST_LOG_(INFO) << "TestCompleted : " << eventName << " " << code;
    return STAbilityUtil::Completed(event, eventName, code);
}

/**
 * @tc.number    : AMS_Data_Ability_00100
 * @tc.name      : data ability ST 001
 * @tc.desc      : start InsertOperator of data ability by page ability in the same app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_00100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";

    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    GTEST_LOG_(INFO) << "StartAbility start";
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility done";
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00100 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00200
 * @tc.name      : data ability ST 002
 * @tc.desc      : start DeleteOperator of data ability by page ability in the same app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_00200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA", OPERATOR_DELETE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00200 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00300
 * @tc.name      : data ability ST 003
 * @tc.desc      : start UpdateOperator of data ability by page ability in the same app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_00300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00300 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00400
 * @tc.name      : data ability ST 004
 * @tc.desc      : start OpenFileOperator of data ability by page ability in the same app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_00400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA", OPERATOR_OPENFILE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_OPENFILE, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_OPENFILE + " " + DEFAULT_OPENFILE_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00400 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00500
 * @tc.name      : data ability ST 005
 * @tc.desc      : start GetFileTypesOperator of data ability by page ability in the same app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_00500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(
        TestWaitCompleted(event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00500 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00600
 * @tc.name      : data ability ST 006
 * @tc.desc      : start QueryOperator of data ability by page ability in the same app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_00600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA", OPERATOR_QUERY);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_DATA_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_DATA_A_CODE, OPERATOR_QUERY);

    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00600 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00700
 * @tc.name      : data ability ST 007
 * @tc.desc      : start InsertOperator of data ability by page ability in different app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_00700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    GTEST_LOG_(INFO) << "StartAbility start";
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility done";
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00700 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00800
 * @tc.name      : data ability ST 008
 * @tc.desc      : start DeleteOperator of data ability by page ability in different app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_00800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00800 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC1", OPERATOR_DELETE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    GTEST_LOG_(INFO) << "StartAbility start";
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility done";
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00800 end";
}

/**
 * @tc.number    : AMS_Data_Ability_00900
 * @tc.name      : data ability ST 009
 * @tc.desc      : start UpdateOperator of data ability by page ability in different app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_00900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00900 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC1", OPERATOR_UPDATE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    GTEST_LOG_(INFO) << "StartAbility start";
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility done";
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_UPDATE, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_UPDATE + " " + DEFAULT_UPDATE_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_00900 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01000
 * @tc.name      : data ability ST 010
 * @tc.desc      : start OpenFileOperator of data ability by page ability in different app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_01000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01000 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC1", OPERATOR_OPENFILE);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    GTEST_LOG_(INFO) << "StartAbility start";
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility done";
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_OPENFILE, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_OPENFILE + " " + DEFAULT_OPENFILE_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01000 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01100
 * @tc.name      : data ability ST 011
 * @tc.desc      : start GetFileTypesOperator of data ability by page ability in different app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_01100, Function | MediumTest | Level1)
{
    using namespace STtools;
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01100 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first =
        std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName, OPERATOR_DEFAULT);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC1", OPERATOR_GETFILETYPES);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    GTEST_LOG_(INFO) << "StartAbility start";
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility done";
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_GETFILETYPES, ABILITY_DATA_C1_CODE), 0);

    EXPECT_EQ(
        TestWaitCompleted(event, OPERATOR_GETFILETYPES + " " + DEFAULT_GETFILETYPE_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01100 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01200
 * @tc.name      : data ability ST 012
 * @tc.desc      : start QueryOperator of data ability by page ability in different app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_01200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01200 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC1", OPERATOR_QUERY);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    GTEST_LOG_(INFO) << "StartAbility start";
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility done";
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_DATA_C1_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_DATA_C1_CODE, OPERATOR_QUERY);

    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + OPERATOR_QUERY, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01200 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01300
 * @tc.name      : data ability ST 013
 * @tc.desc      : start InsertOperator of data ability by data ability in the same app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_01300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01300 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first =
        std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName, OPERATOR_DEFAULT);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC2", OPERATOR_QUERY);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    GTEST_LOG_(INFO) << "StartAbility start";
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility done";
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_DATA_C1_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_DATA_C1_CODE, DATA_STATE_QUERY);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_C2_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_DATA_C2_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_DATA_C2_CODE, DATA_STATE_QUERY);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_DATA_C1_CODE), 0);

    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01300 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01400
 * @tc.name      : data ability ST 014
 * @tc.desc      : start InsertOperator of data ability by data ability in different app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_01400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01400 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first =
        std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName, OPERATOR_DEFAULT);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    GTEST_LOG_(INFO) << "StartAbility start";
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility done";
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_DATA_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_DATA_A_CODE, DATA_STATE_QUERY);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_DATA_B_CODE), 0);

    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_DATA_A_CODE), 0);

    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01400 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01500
 * @tc.name      : data ability ST 015
 * @tc.desc      : start InsertOperator of data ability by the same data ability.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_01500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01500 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first =
        std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName, OPERATOR_DEFAULT);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC1", OPERATOR_INSERT);
    first->AddChildOperator(second);
    second->AddChildOperator(third);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    GTEST_LOG_(INFO) << "StartAbility start";
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility done";
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_DATA_C1_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_DATA_C1_CODE, DATA_STATE_QUERY);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_DATA_C1_CODE), 0);

    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01500 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01600
 * @tc.name      : data ability ST 016
 * @tc.desc      : start InsertOperator of data ability by two page ability in different app.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_016, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01600 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first =
        std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName, OPERATOR_DEFAULT);
    std::shared_ptr<StOperator> second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);
    GTEST_LOG_(INFO) << "StartAbility1 start";
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility1 done";

    bundleName = BUNDLE_NAME_BASE + "B";
    abilityName = ABILITY_NAME_BASE + "PageB";
    first = std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName, OPERATOR_DEFAULT);
    second = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC2", OPERATOR_INSERT);
    first->AddChildOperator(second);
    vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);
    GTEST_LOG_(INFO) << "StartAbility2 start";
    eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility2 done";

    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_C2_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT, ABILITY_DATA_C2_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_PAGE_A_CODE), 0);

    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_B_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_B_CODE, PAGE_STATE_ONACTIVE);

    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT, ABILITY_DATA_C2_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, OPERATOR_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_PAGE_B_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01600 end";
}

/**
 * @tc.number    : AMS_Data_Ability_01700
 * @tc.name      : data ability ST 017
 * @tc.desc      : start some data ability by some page ability and data ability.
 */
HWTEST_F(AmsDataAbilityTest, AMS_Data_Ability_01700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01700 start";
    STAbilityUtil::CleanMsg(event);

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "PageA";
    std::shared_ptr<StOperator> first =
        std::make_shared<StOperator>(ABILITY_TYPE_PAGE, bundleName, abilityName, OPERATOR_DEFAULT);
    std::shared_ptr<StOperator> second1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC1", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second2 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "B", ABILITY_NAME_BASE + "DataB", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third1 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC1", OPERATOR_DELETE);
    std::shared_ptr<StOperator> second3 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC2", OPERATOR_INSERT);
    std::shared_ptr<StOperator> second4 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "A", ABILITY_NAME_BASE + "DataA", OPERATOR_QUERY);
    std::shared_ptr<StOperator> third2 = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC1", OPERATOR_QUERY);
    std::shared_ptr<StOperator> four = std::make_shared<StOperator>(
        ABILITY_TYPE_DATA, BUNDLE_NAME_BASE + "C", ABILITY_NAME_BASE + "DataC2", OPERATOR_DELETE);
    first->AddChildOperator(second1);
    first->AddChildOperator(second2);
    second2->AddChildOperator(third1);
    first->AddChildOperator(second3);
    first->AddChildOperator(second4);
    second4->AddChildOperator(third2);
    third2->AddChildOperator(four);

    std::vector<string> vectorOperator = STAbilityUtil::SerializationStOperatorToVector(*first);

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, vectorOperator);

    GTEST_LOG_(INFO) << "StartAbility start";
    ErrCode eCode = STAbilityUtil::StartAbility(want, g_abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    GTEST_LOG_(INFO) << "StartAbility done";
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONSTART, ABILITY_PAGE_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, PAGE_STATE_ONACTIVE, ABILITY_PAGE_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_PAGE_A_CODE, PAGE_STATE_ONACTIVE);
    // second1
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_DATA_C1_CODE), 0);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_PAGE_A_CODE), 0);

    // second2
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_DATA_B_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_DATA_B_CODE, DATA_STATE_QUERY);
    // third1
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_DATA_C1_CODE), 0);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_DATA_B_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_PAGE_A_CODE), 0);

    // second3
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_C2_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT, ABILITY_DATA_C2_CODE), 0);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_INSERT + " " + DEFAULT_INSERT_RESULT, ABILITY_PAGE_A_CODE), 0);

    // second4
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_ONSTART, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_DATA_A_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_DATA_A_CODE, DATA_STATE_QUERY);
    // third2
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY, ABILITY_DATA_C1_CODE), 0);
    PublishEvent(TEST_EVENT_NAME, ABILITY_DATA_C1_CODE, DATA_STATE_QUERY);
    // four
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE, ABILITY_DATA_C2_CODE), 0);

    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_DELETE + " " + DEFAULT_DELETE_RESULT, ABILITY_DATA_C1_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_DATA_A_CODE), 0);
    EXPECT_EQ(TestWaitCompleted(event, DATA_STATE_QUERY + " " + DEFAULT_QUERY_RESULT, ABILITY_PAGE_A_CODE), 0);

    GTEST_LOG_(INFO) << "AmsDataAbilityTest AMS_Data_Ability_01700 end";
}
}  // namespace