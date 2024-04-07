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

#include <thread>
#include <chrono>
#include <cstdio>
#include <gtest/gtest.h>
#include "hilog_wrapper.h"
#include "ability_manager_service.h"
#include "ability_manager_errors.h"
#include "app_mgr_service.h"
#include "module_test_dump_util.h"
#include "system_test_ability_util.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"
#include "common_event.h"
#include "common_event_manager.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::MTUtil;
using namespace OHOS::STABUtil;
using namespace OHOS::EventFwk;

namespace {
using MAP_STR_STR = std::map<std::string, std::string>;
std::vector<std::string> bundleNameSuffix = {"A", "B", "C"};
std::string bundleNameBase = "com.ohos.amsst.app";
std::string hapNameBase = "amsSystemTest";
std::string abilityNameBase = "AmsStAbility";
std::string launcherBundle = "com.ix.launcher";
std::string systemUiBundle = "com.ohos.systemui";

static const std::string DUMP_STACK_LIST = "--stack-list";
static const std::string DUMP_STACK = "--stack";
static const std::string DUMP_MISSION = "--mission";
static const std::string DUMP_TOP = "--top";
constexpr int WAIT_TIME = 7 * 1000;
constexpr int WAIT_LAUNCHER_OK = 25 * 1000;
static const std::string abilityStateOnStart = ":OnStart";
static const std::string abilityStateOnStop = ":OnStop";
static const std::string abilityStateOnActive = ":OnActive";
static const std::string abilityStateOnInactive = ":OnInactive";
static const std::string abilityStateOnBackground = ":OnBackground";
static const std::string abilityStateOnForeground = ":OnForeground";
static const std::string abilityStateOnNewWant = ":OnNewWant";
static const int abilityStateCountOne = 1;
static const int abilityStateCountTwo = 2;
}  // namespace
class AppEventSubscriber;

class AmsAppProcessManageTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static std::vector<std::string> GetBundleNames(
        const std::string &strBase, const std::vector<std::string> &strSuffixs);
    static bool SubscribeEvent();
    void ClearSystem();
    void ShowDump();
    class AppEventSubscriber : public CommonEventSubscriber {
    public:
        explicit AppEventSubscriber(const CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp)
        {}
        virtual ~AppEventSubscriber()
        {}
        virtual void OnReceiveEvent(const CommonEventData &data) override;
    };
    static sptr<IAppMgr> appMs_;
    static sptr<IAbilityManager> abilityMs_;
    static STtools::Event event_;
    static std::shared_ptr<AppEventSubscriber> subscriber_;
};

sptr<IAppMgr> AmsAppProcessManageTest::appMs_ = nullptr;
sptr<IAbilityManager> AmsAppProcessManageTest::abilityMs_ = nullptr;
STtools::Event AmsAppProcessManageTest::event_ = STtools::Event();
std::shared_ptr<AmsAppProcessManageTest::AppEventSubscriber> AmsAppProcessManageTest::subscriber_ = nullptr;

void AmsAppProcessManageTest::SetUpTestCase(void)
{
    std::vector<std::string> hapNames = GetBundleNames(hapNameBase, bundleNameSuffix);
    STAbilityUtil::InstallHaps(hapNames);
    GTEST_LOG_(INFO) << "void AmsAppProcessManageTest::SetUpTestCase(void)";
}

void AmsAppProcessManageTest::TearDownTestCase(void)
{
    std::vector<std::string> bundleNames = GetBundleNames(bundleNameBase, bundleNameSuffix);
    STAbilityUtil::UninstallBundle(bundleNames);
    GTEST_LOG_(INFO) << "void AmsAppProcessManageTest::TearDownTestCase(void)";
}

void AmsAppProcessManageTest::SetUp(void)
{
    ClearSystem();
    auto bundleNames = GetBundleNames(bundleNameBase, bundleNameSuffix);
    STAbilityUtil::KillBundleProcess(bundleNames);
    SubscribeEvent();
    GTEST_LOG_(INFO) << "void AmsAppProcessManageTest::SetUp(void)";
}

void AmsAppProcessManageTest::TearDown(void)
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
    STAbilityUtil::CleanMsg(event_);
    GTEST_LOG_(INFO) << "void AmsAppProcessManageTest::TearDown(void)";
}

std::vector<std::string> AmsAppProcessManageTest::GetBundleNames(
    const std::string &strBase, const std::vector<std::string> &strSuffixs)
{
    std::vector<std::string> bundleNames;
    for (auto strSuffix : strSuffixs) {
        bundleNames.push_back(strBase + strSuffix);
    }
    return bundleNames;
}

bool AmsAppProcessManageTest::SubscribeEvent()
{
    std::vector<std::string> eventList = {"resp_st_page_ability_callback"};
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void AmsAppProcessManageTest::ClearSystem()
{
    STAbilityUtil::KillService("appspawn");
    STAbilityUtil::KillService("installs");
    STAbilityUtil::KillService(launcherBundle);
    STAbilityUtil::KillService(systemUiBundle);
    STAbilityUtil::KillService("foundation");
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_LAUNCHER_OK));
}

void AmsAppProcessManageTest::ShowDump()
{
    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState("-a", dumpInfo);
    for (const auto &info : dumpInfo) {
        std::cout << info << std::endl;
    }
}

void AmsAppProcessManageTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "OnReceiveEvent: event=" << data.GetWant().GetAction();
    GTEST_LOG_(INFO) << "OnReceiveEvent: data=" << data.GetData();
    GTEST_LOG_(INFO) << "OnReceiveEvent: code=" << data.GetCode();

    std::string eventName = data.GetWant().GetAction();
    if (eventName == "resp_st_page_ability_callback") {
        std::string target = data.GetData();
        STAbilityUtil::Completed(event_, target, data.GetCode());
    }
}

/*
 * @tc.number    : AMS_App_Process_0100
 * @tc.name      : create app process successfully by starting ability,launchtype[A1:singletop]
 * @tc.desc      : 1.start a page ability
 *                 2.wait for the event sent by the page ability
 *                 3.determine process Info
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0100 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string hapName = hapNameBase + "A";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_);
    EXPECT_TRUE(pInfo.pid_ > 0);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0100 end";
}

/*
 * @tc.number    : AMS_App_Process_0200
 * @tc.name      : create multiple app process successfully by starting ability,launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.start page ability B1 by A1 onActive function
 *                 3.start ENTITY_HOME
 *                 4.wait for the event sent by the page ability
 *                 5.determine process status
 *                 6.start page ability A1
 *                 7.wait for the event sent by the page ability
 *                 8.determine process status
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0200 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    MAP_STR_STR params;
    params["targetBundle"] = bundleNameBase + "B";
    params["targetAbility"] = abilityNameBase + "B1";

    Want want1 = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    MAP_STR_STR params1;
    Want want2 = STAbilityUtil::MakeWant("device", abilityName, bundleName, params1);

    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::CleanMsg(event_);

    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::CleanMsg(event_);
    AppProcessInfo pInfo1 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_TRUE(pInfo1.pid_ > 0);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo1.state_);
    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_, WAIT_TIME);
    EXPECT_TRUE(pInfo2.pid_ > 0);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo2.state_);

    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountTwo), 0);
    pInfo1 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_TRUE(pInfo1.pid_ > 0);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo1.state_);
    pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_, WAIT_TIME);
    EXPECT_TRUE(pInfo2.pid_ > 0);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo2.state_);
    EXPECT_TRUE(pInfo2.pid_ != pInfo1.pid_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0200 end";
}

/*
 * @tc.number    : AMS_App_Process_0300
 * @tc.name      : not create new app process when app process started,launchtype[A1,A2:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.wait for the event sent by the page ability
 *                 3.determine process Info
 *                 4.start a page ability A2
 *                 5.wait for the event sent by the page ability
 *                 6.determine process Info
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0300 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string abilityName2 = abilityNameBase + "A2";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_);
    EXPECT_TRUE(pInfo.pid_ > 0);

    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName, params);
    // start the same abilty again
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_);
    EXPECT_EQ(pInfo.pid_, pInfo2.pid_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0300 end";
}

/*
 * @tc.number    : AMS_App_Process_0400
 * @tc.name      : kill app process that is running,launchtype[A1:singletop]
 * @tc.desc      : 1.start a page ability
 *                 2.wait for the event sent by the page ability
 *                 3.determine process Info
 *                 4.kill current Application
 *                 5.determine process Info and dump stack info
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0400 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_);
    EXPECT_TRUE(pInfo.pid_ > 0);

    STAbilityUtil::KillApplication(bundleName, appMs_, WAIT_TIME);

    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_);
    EXPECT_EQ(0, pInfo2.pid_);

    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState(DUMP_STACK + " 1", dumpInfo);
    std::vector<std::string> result;
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
    auto pos = MTDumpUtil::GetInstance()->GetSpecific(abilityName, result, result.begin());
    EXPECT_NE(pos, result.end());

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0400 end";
}

/*
 * @tc.number    : AMS_App_Process_0500
 * @tc.name      : kill app process that do not exist,launchtype[A1:singletop]
 * @tc.desc      : 1.determine process Info
 *                 2.kill current Application
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0500 start";

    std::string bundleName = bundleNameBase + "A";
    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_);
    EXPECT_TRUE(pInfo.pid_ == 0);

    STAbilityUtil::KillApplication(bundleName, appMs_, WAIT_TIME);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0500 end";
}

/*
 * @tc.number    : AMS_App_Process_0600
 * @tc.name      : create multiple app process successfully by starting ability,launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.wait for the event sent by the page ability
 *                 3.determine process Info
 *                 4.start a page ability B1
 *                 5.wait for the event sent by the page ability
 *                 6.determine process Info
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_0600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0600 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_);
    EXPECT_TRUE(pInfo.pid_ > 0);
    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_);
    EXPECT_TRUE(pInfo2.pid_ > 0);
    EXPECT_TRUE(pInfo2.pid_ != pInfo.pid_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0600 end";
}

/*
 * @tc.number    : AMS_App_Process_0700
 * @tc.name      : kill app process when all abilities of this app is terminated,launchtype[A1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.A1 terminated by A1 OnActive
 *                 3.wait for the event sent by the page ability
 *                 4.determine process Info
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_0700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0700 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    MAP_STR_STR params;
    params["shouldReturn"] = abilityName;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStop, abilityStateCountOne), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(pInfo.pid_, 0);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0700 end";
}

/*
 * @tc.number    : AMS_App_Process_0800
 * @tc.name      : app process state transition to background when HOME key pressed,launchtype[A1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.wait for the event sent by the page ability
 *                 3.start ENTITY_HOME
 *                 4.wait for the event sent by the page ability
 *                 5.determine process status
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_0800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0800 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    // simulate ENTITY_HOME
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo.state_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0800 end";
}

/*
 * @tc.number    : AMS_App_Process_0900
 * @tc.name      : app process state transition to background when another app started,
 *                 launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.wait for the event sent by the page ability
 *                 3.start a page ability B1
 *                 4.wait for the event sent by the page ability
 *                 5.determine process status
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_0900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0900 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo.state_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_0900 end";
}

/*
 * @tc.number    : AMS_App_Process_1000
 * @tc.name      : app process state transition to background when it starts another app,
 *                 launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.start a page ability B1 by A1 OnActive
 *                 3.wait for the event sent by the page ability
 *                 4.determine process status
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_1000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1000 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";

    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";

    MAP_STR_STR params;
    params["targetBundle"] = bundleName2;
    params["targetAbility"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo.state_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1000 end";
}

/*
 * @tc.number    : AMS_App_Process_1100
 * @tc.name      : app process state transition to foreground when restarted,launchtype[A1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.wait for the event sent by the page ability
 *                 3.determine process status
 *                 4.start ENTITY_HOME
 *                 5.wait for the event sent by the page ability
 *                 6.determine process status
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_1100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1100 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    MAP_STR_STR params;
    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(launcherBundle, appMs_);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo.state_);
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo.state_);

    // simulate ENTITY_HOME
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo.state_);
    pInfo = STAbilityUtil::GetAppProcessInfoByName(launcherBundle, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo.state_);

    // restart
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);
    pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo.state_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1100 end";
}

/*
 * @tc.number    : AMS_App_Process_1200
 * @tc.name      : restart app after kill app process,launchtype[A1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.wait for the event sent by the page ability
 *                 3.determine process status
 *                 4.kill current Application
 *                 5.start a page ability A1
 *                 6.wait for the event sent by the page ability
 *                 7.determine process status and determine process info
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_1200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1200 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo.state_);

    STAbilityUtil::KillApplication(bundleName, appMs_, WAIT_TIME);
    STAbilityUtil::CleanMsg(event_);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo2.state_);
    // new process
    EXPECT_NE(pInfo.pid_, pInfo2.pid_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1200 end";
}

/*
 * @tc.number    : AMS_App_Process_1300
 * @tc.name      : kill background app process,launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.wait for the event sent by the page ability
 *                 3.determine process status
 *                 4.kill current Application
 *                 5.start a page ability A1
 *                 6.wait for the event sent by the page ability
 *                 7.determine process status and determine process info
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_1300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1300 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName2;
    params["targetAbility"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo.state_);

    STAbilityUtil::KillApplication(bundleName, appMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);

    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_);
    EXPECT_EQ(0, pInfo2.pid_);
    AppProcessInfo pInfo1 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_);
    EXPECT_EQ(0, pInfo1.pid_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1300 end";
}

/*
 * @tc.number    : AMS_App_Process_1400
 * @tc.name      : verify process management when start abnormal app(onStart exception),launchtype[K1:singletop]
 * @tc.desc      : 1.start a page ability K1
 *                 2.determine process info
 *                 3.get AbilityRecordId
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_1400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1400 start";

    std::string bundleName = bundleNameBase + "K";
    std::string abilityName = "AmsStAbilityErrorK1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_, WAIT_TIME);
    // when app collapse in OnStart, app process should be terminated?
    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_);
    EXPECT_EQ(0, pInfo.pid_);
    // when app collapse in OnStart, ability record should be removed from stack?
    int64_t abilityRecordId;
    STAbilityUtil::GetTopAbilityRecordId(abilityRecordId, abilityMs_);
    EXPECT_EQ(-1, abilityRecordId);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1400 end";
}

/*
 * @tc.number    : AMS_App_Process_1500
 * @tc.name      : create multiple app process successfully by starting ability,launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start page ability A1,B1,A1,B1,A1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.determine process status
 *                 4.start a page ability B1
 *                 5.wait for the event sent by the page ability
 *                 6.determine process status
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_1500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1500 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    MAP_STR_STR params;
    Want want1 = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    STAbilityUtil::CleanMsg(event_);
    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::CleanMsg(event_);
    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::CleanMsg(event_);
    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::CleanMsg(event_);
    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    AppProcessInfo pInfo1 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_TRUE(pInfo1.pid_ > 0);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo1.state_);
    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_, WAIT_TIME);
    EXPECT_TRUE(pInfo2.pid_ > 0);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo2.state_);
    STAbilityUtil::CleanMsg(event_);
    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    pInfo1 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_TRUE(pInfo1.pid_ > 0);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo1.state_);
    pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_, WAIT_TIME);
    EXPECT_TRUE(pInfo2.pid_ > 0);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo2.state_);

    EXPECT_TRUE(pInfo2.pid_ != pInfo1.pid_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1500 end";
}

/*
 * @tc.number    : AMS_App_Process_1600
 * @tc.name      : create multiple app process successfully by starting ability,launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start page ability A1
 *                 2.start page ability B1 by A1 OnActive
 *                 3.wait for the event sent by the page ability
 *                 4.determine process status
 *                 4.start a page ability A1
 *                 5.wait for the event sent by the page ability
 *                 6.determine process status
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_1600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1600 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleNameBase + "B";
    params["targetAbility"] = abilityNameBase + "B1";

    Want want1 = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    params.clear();
    Want want2 = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);

    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::CleanMsg(event_);
    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::CleanMsg(event_);
    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::CleanMsg(event_);
    AppProcessInfo pInfo1 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_TRUE(pInfo1.pid_ > 0);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo1.state_);
    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_, WAIT_TIME);
    EXPECT_TRUE(pInfo2.pid_ > 0);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo2.state_);

    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    pInfo1 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_TRUE(pInfo1.pid_ > 0);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo1.state_);
    pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_, WAIT_TIME);
    EXPECT_TRUE(pInfo2.pid_ > 0);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo2.state_);

    EXPECT_TRUE(pInfo2.pid_ != pInfo1.pid_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1600 end";
}

/*
 * @tc.number    : AMS_App_Process_1700
 * @tc.name      : verify process status,switch the ability status of different bundlename,
 *                 launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start page ability A1,B1,ENTITY_HOME,A1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.determine process status
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_1700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1700 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    MAP_STR_STR params;
    Want want1 = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);

    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);
    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo.state_);
    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo2.state_);
    // new process
    EXPECT_NE(pInfo.pid_, pInfo2.pid_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1700 end";
}

/*
 * @tc.number    : AMS_App_Process_1800
 * @tc.name      : verify process status,switch the ability status of different bundlename,
 *                 launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start page ability A1,ENTITY_HOME,B1,ENTITY_HOME,B1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.determine process status
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_1800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1800 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    MAP_STR_STR params;

    Want want1 = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);

    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountTwo), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo.state_);
    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo2.state_);
    // new process
    EXPECT_NE(pInfo.pid_, pInfo2.pid_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1800 end";
}

/*
 * @tc.number    : AMS_App_Process_1900
 * @tc.name      : verify process status,switch the ability status of different bundlename,
 *                 launchtype[A1,B1,A2:singletop]
 * @tc.desc      : 1.start page ability A1,B1,ENTITY_HOME,A2 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.determine process status
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_1900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1900 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    std::string abilityName3 = abilityNameBase + "A2";
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    MAP_STR_STR params;

    Want want1 = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    Want want3 = STAbilityUtil::MakeWant("device", abilityName3, bundleName, params);

    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(want3, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountTwo), 0);
    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo.state_);
    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo2.state_);
    // new process
    EXPECT_NE(pInfo.pid_, pInfo2.pid_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_1900 end";
}

/*
 * @tc.number    : AMS_App_Process_2000
 * @tc.name      : verify process status,switch the ability status of different bundlename,
 *                 launchtype[A1,B1,A2:singletop]
 * @tc.desc      : 1.start page ability B1,A1,ENTITY_HOME,A2 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.determine process status
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_2000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_2000 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    std::string abilityName3 = abilityNameBase + "A2";
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    MAP_STR_STR params;

    Want want1 = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    Want want3 = STAbilityUtil::MakeWant("device", abilityName3, bundleName, params);

    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(want3, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo.state_);
    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo2.state_);
    // new process
    EXPECT_NE(pInfo.pid_, pInfo2.pid_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_2000 end";
}

/*
 * @tc.number    : AMS_App_Process_2100
 * @tc.name      : verify process status,switch the ability status of different bundlename,
 *                 launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start page ability A1,B1,ENTITY_HOME,A1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.determine process status
 */
HWTEST_F(AmsAppProcessManageTest, AMS_App_Process_2100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_2100 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    MAP_STR_STR params;
    Want want1 = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);

    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(want1, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountTwo), 0);
    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo.state_);
    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo2.state_);
    // new process
    EXPECT_NE(pInfo.pid_, pInfo2.pid_);

    GTEST_LOG_(INFO) << "AmsAppProcessManageTest AMS_App_Process_2100 end";
}