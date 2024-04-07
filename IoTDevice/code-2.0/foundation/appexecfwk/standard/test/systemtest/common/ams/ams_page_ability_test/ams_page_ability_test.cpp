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

#include <cstdio>
#include <thread>
#include "ability_manager_service.h"
#include "ability_manager_errors.h"
#include "module_test_dump_util.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"
#include "system_test_ability_util.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "app_mgr_service.h"
#include "ability_lifecycle_executor.h"
#include "ability_lifecycle.h"
#include "hilog_wrapper.h"
#include <gtest/gtest.h>

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::MTUtil;
using namespace OHOS::STABUtil;
using namespace OHOS::EventFwk;
namespace {
typedef std::map<std::string, std::string> MAP_STR_STR;
std::vector<std::string> bundleNameSuffix = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "M", "N", "O", "P"};
std::string bundleNameBase = "com.ohos.amsst.app";
std::string abilityNameBase = "AmsStAbility";
std::string hapNameBase = "amsSystemTest";
std::string launcherAbilityName = "LauncherAbility";
std::string launcherBundleName = "com.ix.launcher";
std::string systemUiBundle = "com.ohos.systemui";
std::string terminatePageAbility = "requ_page_ability_terminate";
std::string abilityEventName = "resp_st_page_ability_callback";
std::string pidEventName = "resp_st_page_ability_pid_callback";
const int amsStAbilityO1Code = 10;
const int amsStAbilityP1Code = 11;
static const std::string DUMP_STACK_LIST = "--stack-list";
static const std::string DUMP_STACK = "--stack";
static const std::string DUMP_MISSION = "--mission";
static const std::string DUMP_TOP = "--top";
static const std::string DUMP_ALL = "-a";
static const std::string lifecycleStateUninitialized =
    std::to_string(AbilityLifecycleExecutor::LifecycleState::UNINITIALIZED);
static const std::string lifecycleStateActive = std::to_string(AbilityLifecycleExecutor::LifecycleState::ACTIVE);
static const std::string lifecycleStateBackground =
    std::to_string(AbilityLifecycleExecutor::LifecycleState::BACKGROUND);
static const std::string lifecycleStateInactive = std::to_string(AbilityLifecycleExecutor::LifecycleState::INACTIVE);
static const std::string lifecycleStateInitial = std::to_string(AbilityLifecycleExecutor::LifecycleState::INITIAL);
constexpr int WAIT_TIME = 7 * 1000;
constexpr int WAIT_LAUNCHER_OK = 25 * 1000;
enum AbilityState_Test {
    INITIAL = 0,
    INACTIVE,
    ACTIVE,
    BACKGROUND,
    SUSPENDED,
    INACTIVATING,
    ACTIVATING,
    MOVING_BACKGROUND,
    TERMINATING,
    ALLSUM,
};
static const std::vector<std::string> abilityStateVec = {
    "INITIAL",
    "INACTIVE",
    "ACTIVE",
    "BACKGROUND",
    "SUSPENDED",
    "INACTIVATING",
    "ACTIVATING",
    "MOVING_BACKGROUND",
    "TERMINATING",
};
static const std::string abilityStateOnStart = ":OnStart";
static const std::string abilityStateOnStop = ":OnStop";
static const std::string abilityStateOnActive = ":OnActive";
static const std::string abilityStateOnInactive = ":OnInactive";
static const std::string abilityStateOnBackground = ":OnBackground";
static const std::string abilityStateOnForeground = ":OnForeground";
static const std::string abilityStateOnNewWant = ":OnNewWant";
static const int abilityStateCountOne = 1;
static const int abilityStateCountTwo = 2;
static const int abilityStateCountThree = 3;
}  // namespace

class AmsPageAbilityTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static std::vector<std::string> GetBundleNames(
        const std::string &strBase, const std::vector<std::string> &strSuffixs);
    static void CheckAbilityStateByName(const std::string &abilityName, const std::vector<std::string> &info,
        const std::string &state, const std::string &midState);
    void ExpectAbilityCurrentState(const std::string &abilityName, const AbilityState_Test &currentState,
        const AbilityState_Test &midState = AbilityState_Test::ALLSUM, const std::string &args = (DUMP_STACK + " 1"));
    void ExpectAbilityNumInStack(const std::string &abilityName, int abilityNum);
    void ClearSystem();
    void ShowDump();
    static bool SubscribeEvent();
    static void GetAllStackInfo(MissionStackInfo &missionStackInfo);
    class AppEventSubscriber : public CommonEventSubscriber {
    public:
        explicit AppEventSubscriber(const CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp)
        {}
        virtual ~AppEventSubscriber()
        {}
        virtual void OnReceiveEvent(const CommonEventData &data) override;
    };
    static sptr<IAbilityManager> abilityMs_;
    static sptr<IAppMgr> appMs_;
    static STtools::Event event_;
    static STtools::Event processInfoEvent_;
};

STtools::Event AmsPageAbilityTest::event_ = STtools::Event();
STtools::Event AmsPageAbilityTest::processInfoEvent_ = STtools::Event();
sptr<IAbilityManager> AmsPageAbilityTest::abilityMs_ = nullptr;
sptr<IAppMgr> AmsPageAbilityTest::appMs_ = nullptr;

void AmsPageAbilityTest::ShowDump()
{
    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState("-a", dumpInfo);
    for (const auto &info : dumpInfo) {
        std::cout << info << std::endl;
    }
}

void AmsPageAbilityTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest::SetUpTestCase(void)";
    std::vector<std::string> hapNames = GetBundleNames(hapNameBase, bundleNameSuffix);
    STAbilityUtil::InstallHaps(hapNames);
}

void AmsPageAbilityTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest::TearDownTestCase(void)";
    std::vector<std::string> bundleNames = GetBundleNames(bundleNameBase, bundleNameSuffix);
    STAbilityUtil::UninstallBundle(bundleNames);
}

void AmsPageAbilityTest::SetUp(void)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest::SetUp(void)";
    ClearSystem();
    std::vector<std::string> bundleNames = GetBundleNames(bundleNameBase, bundleNameSuffix);
    STAbilityUtil::KillBundleProcess(bundleNames);
    SubscribeEvent();
}

void AmsPageAbilityTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "void AmsPageAbilityTest::TearDown(void)";
    STAbilityUtil::CleanMsg(event_);
    STAbilityUtil::CleanMsg(processInfoEvent_);
}

std::vector<std::string> AmsPageAbilityTest::GetBundleNames(
    const std::string &strBase, const std::vector<std::string> &strSuffixs)
{
    std::vector<std::string> bundleNames;
    for (auto strSuffix : strSuffixs) {
        bundleNames.push_back(strBase + strSuffix);
    }
    return bundleNames;
}

void AmsPageAbilityTest::CheckAbilityStateByName(const std::string &abilityName, const std::vector<std::string> &info,
    const std::string &state, const std::string &midState)
{
    std::vector<std::string> result;
    MTDumpUtil::GetInstance()->GetAll("AbilityName", info, result);
    auto pos = MTDumpUtil::GetInstance()->GetSpecific(abilityName, result, result.begin());
    // ability exist
    EXPECT_NE(pos, result.end());
    MTDumpUtil::GetInstance()->GetAll("State", info, result);
    EXPECT_TRUE(pos < result.end());
    // ability state
    if (midState != "") {
        bool compareResult = ((*pos == state) || (*pos == midState));
        EXPECT_EQ(1, compareResult);
    } else {
        EXPECT_EQ(*pos, state);
    }
}

void AmsPageAbilityTest::ExpectAbilityCurrentState(const std::string &abilityName,
    const AbilityState_Test &currentState, const AbilityState_Test &midState, const std::string &args)
{
    std::string strCurrentState = abilityStateVec.at(currentState);
    std::string strMidState = "";
    if (midState != AbilityState_Test::ALLSUM) {
        strMidState = abilityStateVec.at(midState);
    }
    std::vector<std::string> dumpInfo;
    if (abilityMs_ != nullptr) {
        abilityMs_->DumpState(args, dumpInfo);
        CheckAbilityStateByName(abilityName, dumpInfo, strCurrentState, strMidState);
    } else {
        HILOG_ERROR("ability manager service(abilityMs_) is nullptr");
    }
}

void AmsPageAbilityTest::ExpectAbilityNumInStack(const std::string &abilityName, int abilityNum)
{
    std::vector<std::string> dumpInfo;
    if (abilityMs_ != nullptr) {
        abilityMs_->DumpState(DUMP_STACK + " 1", dumpInfo);
        std::vector<std::string> result;
        MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
        // only one record in stack
        EXPECT_EQ(abilityNum, std::count(result.begin(), result.end(), abilityName));
    } else {
        HILOG_ERROR("ability manager service(abilityMs_) is nullptr");
    }
}

void AmsPageAbilityTest::ClearSystem()
{
    STAbilityUtil::KillService("appspawn");
    STAbilityUtil::KillService("installs");
    STAbilityUtil::KillService(launcherBundleName);
    STAbilityUtil::KillService(systemUiBundle);
    STAbilityUtil::KillService("foundation");
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_LAUNCHER_OK));
}

bool AmsPageAbilityTest::SubscribeEvent()
{
    std::vector<std::string> eventList = {
        abilityEventName,
        pidEventName,
    };
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    auto subscriber = std::make_shared<AppEventSubscriber>(subscribeInfo);
    return CommonEventManager::SubscribeCommonEvent(subscriber);
}

void AmsPageAbilityTest::GetAllStackInfo(MissionStackInfo &missionStackInfo)
{
    StackInfo stackInfo;
    abilityMs_->GetAllStackInfo(stackInfo);
    for (const auto &stackInfo : stackInfo.missionStackInfos) {
        if (stackInfo.id == 1) {
            missionStackInfo = stackInfo;
            break;
        }
    }
}

void AmsPageAbilityTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "OnReceiveEvent: event=" << data.GetWant().GetAction();
    GTEST_LOG_(INFO) << "OnReceiveEvent: data=" << data.GetData();
    GTEST_LOG_(INFO) << "OnReceiveEvent: code=" << data.GetCode();

    std::string eventName = data.GetWant().GetAction();
    if (eventName == abilityEventName) {
        std::string target = data.GetData();
        STAbilityUtil::Completed(event_, target, data.GetCode());
    }
    if (eventName == pidEventName) {
        STAbilityUtil::Completed(processInfoEvent_, eventName, data.GetCode(), data.GetData());
    }
}

/*
 * @tc.number    : AMS_Page_Ability_0100
 * @tc.name      : start a page ability with launchtype singletop,launchtype[A1:singletop]
 * @tc.desc      : 1.start a page ability
 *                 2.wait for the event sent by the page ability
 *                 3.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0100 start";
    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0100 end";
}

/*
 * @tc.number    : AMS_Page_Ability_0200
 * @tc.name      : start a page ability twice,launchtype[A1:singletop]
 * @tc.desc      : 1.start a page ability
 *                 2.wait for the event sent by the page ability
 *                 3.start page ability again
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0200 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);

    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);
    // only one record in stack
    ExpectAbilityNumInStack(abilityName, 1);
    // ability state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0200 end";
}

/*
 * @tc.number    : AMS_Page_Ability_0300
 * @tc.name      : start different page ability in the same app sequencely,launchtype[A1,A2:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.wait for the event sent by the page ability
 *                 3.start a page ability A2
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0300 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    // start ability in the same app
    std::string bundleName2 = bundleNameBase + "A";
    std::string abilityName2 = abilityNameBase + "A2";
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // ability "A1" in stack
    ExpectAbilityNumInStack(abilityName, 1);
    // ability "A1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // ability "A2" in stack
    ExpectAbilityNumInStack(abilityName2, 1);
    // ability "A2" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0300 end";
}

/*
 * @tc.number    : AMS_Page_Ability_0400
 * @tc.name      : start different page ability in different app sequencely,launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.wait for the event sent by the page ability
 *                 3.start a page ability B1
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0400 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    // start ability in another app
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // ability "A1" in stack
    ExpectAbilityNumInStack(abilityName, 1);
    // ability "A1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // ability "B1" in stack
    ExpectAbilityNumInStack(abilityName2, 1);
    // ability "B1" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0400 end";
}

/*
 * @tc.number    : AMS_Page_Ability_0500
 * @tc.name      : start page ability by another ability in the same app,launchtype[A1,A2:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.start a page ability A2 by A1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0500 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string abilityName2 = abilityNameBase + "A2";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName;
    params["targetAbility"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);

    // ability "A1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // ability "A2" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0500 end";
}

/*
 * @tc.number    : AMS_Page_Ability_0600
 * @tc.name      : start page ability by itself,launchtype[A1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.start itself by A1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_0600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0600 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName;
    params["targetAbility"] = abilityName;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnNewWant, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);

    // only one record in stack
    ExpectAbilityNumInStack(abilityName, 1);
    // ability state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0600 end";
}

/*
 * @tc.number    : AMS_Page_Ability_0700
 * @tc.name      : start page ability by another ability in different app,launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.start a page ability B1 by A1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_0700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0700 start";

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
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    // ability "A1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // ability "B1" state ACTIVE
    ExpectAbilityCurrentState(abilityNameBase + "B1", AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0700 end";
}

/*
 * @tc.number    : AMS_Page_Ability_0800
 * @tc.name      : verify mission stack management,launchtype[C1,C2,C3:singletop]
 * @tc.desc      : 1.start page ability C1,C2,C3,C2 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_0800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0800 start";

    std::string bundleName = bundleNameBase + "C";
    std::string abilityName = abilityNameBase + "C1";
    std::string abilityName2 = abilityNameBase + "C2";
    std::string abilityName3 = abilityNameBase + "C3";
    std::string abilityName4 = abilityNameBase + "C2";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName, params);
    STAbilityUtil::StartAbility(want2, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    STAbilityUtil::StartAbility(want2, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountTwo), 0);

    Want want3 = STAbilityUtil::MakeWant("device", abilityName3, bundleName, params);
    STAbilityUtil::StartAbility(want3, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    Want want4 = STAbilityUtil::MakeWant("device", abilityName4, bundleName, params);
    STAbilityUtil::StartAbility(want4, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName4 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);

    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState(DUMP_STACK + " 1", dumpInfo);

    std::vector<std::string> result;
    // bundle name from top to bottom
    MTDumpUtil::GetInstance()->GetAll("BundleName", dumpInfo, result);
    std::vector<std::string> expectedResult = {
        bundleNameBase + "C",
        bundleNameBase + "C",
        bundleNameBase + "C",
        bundleNameBase + "C",
    };
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));

    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
    // ability name from top to bottom
    expectedResult = {
        abilityNameBase + "C2",
        abilityNameBase + "C3",
        abilityNameBase + "C2",
        abilityNameBase + "C1",
    };
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0800 end";
}

/*
 * @tc.number    : AMS_Page_Ability_0900
 * @tc.name      : verify mission stack management(multiple apps),launchtype[C1,C2,B1:singletop]
 * @tc.desc      : 1.start page ability C1,C2,B1,C2,B1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_0900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0900 start";

    std::string bundleName = bundleNameBase + "C";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName = abilityNameBase + "C1";
    std::string abilityName2 = abilityNameBase + "C2";
    std::string abilityName3 = abilityNameBase + "B1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName, params);
    STAbilityUtil::StartAbility(want2, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    Want want3 = STAbilityUtil::MakeWant("device", abilityName3, bundleName2, params);
    STAbilityUtil::StartAbility(want3, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    STAbilityUtil::StartAbility(want2, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);

    STAbilityUtil::StartAbility(want3, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState(DUMP_STACK + " 1", dumpInfo);

    std::vector<std::string> result;
    // bundle name from top to bottom
    MTDumpUtil::GetInstance()->GetAll("BundleName", dumpInfo, result);
    std::vector<std::string> expectedResult = {
        bundleNameBase + "B",
        bundleNameBase + "C",
        bundleNameBase + "B",
        bundleNameBase + "C",
        bundleNameBase + "C",
    };
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));

    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
    // ability name from top to bottom
    expectedResult = {
        abilityNameBase + "B1",
        abilityNameBase + "C2",
        abilityNameBase + "B1",
        abilityNameBase + "C2",
        abilityNameBase + "C1",
    };
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_0900 end";
}

/*
 * @tc.number    : AMS_Page_Ability_1000
 * @tc.name      : verify mission stack management(multiple apps + ENTITY_HOME),launchtype[C1,C2,B1:singletop]
 * @tc.desc      : 1.start page ability C1,C2,ENTITY_HOME,B1,C2,B1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_1000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1000 start";

    std::string bundleName = bundleNameBase + "C";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName = abilityNameBase + "C1";
    std::string abilityName2 = abilityNameBase + "C2";
    std::string abilityName3 = abilityNameBase + "B1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName, params);
    STAbilityUtil::StartAbility(want2, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    // simulate ENTITY_HOME
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    Want want3 = STAbilityUtil::MakeWant("device", abilityName3, bundleName2, params);
    STAbilityUtil::StartAbility(want3, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);

    STAbilityUtil::StartAbility(want2, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);

    STAbilityUtil::StartAbility(want3, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo);
    EXPECT_TRUE(missionStackInfo.missionRecords.size() == 2);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    // ability in mission #2 from top to bottom
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName3));
    EXPECT_EQ(0, abilityInfos[1].mainName.compare(abilityName2));
    EXPECT_EQ(0, abilityInfos[2].mainName.compare(abilityName3));
    // ability in mission #1 from top to bottom
    abilityInfos = missionStackInfo.missionRecords[1].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName2));
    EXPECT_EQ(0, abilityInfos[1].mainName.compare(abilityName));

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1000 end";
}

/*
 * @tc.number    : AMS_Page_Ability_1100
 * @tc.name      : ability state transition to background when Home event,launchtype[A1:singletop]
 * @tc.desc      : 1.start page ability A1,ENTITY_HOME in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_1100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1100 start";

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

    // ability "A1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1100 end";
}

/*
 * @tc.number    : AMS_Page_Ability_1200
 * @tc.name      : last ability state transition to foreground when stopping top ability
 *                 (same app),launchtype[A1,A2:singletop]
 * @tc.desc      : 1.start page ability A1,A2 in turn
 *                 2.stop ability A2
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_1200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1200 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    // start ability in the same app
    std::string bundleName2 = bundleNameBase + "A";
    std::string abilityName2 = abilityNameBase + "A2";
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // ability "A1" in stack
    ExpectAbilityNumInStack(abilityName, 1);
    // ability "A1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // ability "A2" in stack
    ExpectAbilityNumInStack(abilityName2, 1);
    // ability "A2" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    STAbilityUtil::StopAbility(terminatePageAbility, 0, abilityName2);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);

    // ability "A1" state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // ability "A2" not in stack
    ExpectAbilityNumInStack(abilityName2, 0);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1200 end";
}

/*
 * @tc.number    : AMS_Page_Ability_1300
 * @tc.name      : last ability state transition to foreground when stopping top ability
 *                 (different app),launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start page ability A1,B1 in turn
 *                 2.stop ability B1
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_1300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1300 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    // start ability in another app
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // ability "A1" in stack
    ExpectAbilityNumInStack(abilityName, 1);
    // ability "A1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // ability "B1" in stack
    ExpectAbilityNumInStack(abilityName2, 1);
    // ability "B1" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    STAbilityUtil::StopAbility(terminatePageAbility, 0, abilityName2);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);

    // ability "A1" state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // ability "B1" not in stack
    ExpectAbilityNumInStack(abilityName2, 0);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1300 end";
}

/*
 * @tc.number    : AMS_Page_Ability_1400
 * @tc.name      : ability state transition to foreground when background app restart,launchtype[A1:singletop]
 * @tc.desc      : 1.start page ability A1,ENTITY_HOME,A1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_1400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1400 start";

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
    // ability "A1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    // background app restart
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);
    // ability "A1" state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1400 end";
}

/*
 * @tc.number    : AMS_Page_Ability_1500
 * @tc.name      : start page ability for the first time when system ready,launchtype[D1:singleton]
 * @tc.desc      : 1.start a page ability
 *                 2.wait for the event sent by the page ability
 *                 3.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_1500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1500 start";

    std::string bundleName = bundleNameBase + "D";
    std::string abilityName = abilityNameBase + "D1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1500 end";
}

/*
 * @tc.number    : AMS_Page_Ability_1600
 * @tc.name      : start page ability twice,launchtype[D1:singleton]
 * @tc.desc      : 1.start a page ability
 *                 2.wait for the event sent by the page ability
 *                 3.start page ability again
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_1600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1600 start";

    std::string bundleName = bundleNameBase + "D";
    std::string abilityName = abilityNameBase + "D1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);

    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnNewWant, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);

    ExpectAbilityNumInStack(abilityName, 1);
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1600 end";
}

/*
 * @tc.number    : AMS_Page_Ability_1700
 * @tc.name      : start different page ability in the same app sequencely,launchtype[D1,D2:singleton]
 * @tc.desc      : 1.start a page ability D1
 *                 2.wait for the event sent by the page ability
 *                 3.start a page ability D2
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_1700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1700 start";

    std::string bundleName = bundleNameBase + "D";
    std::string abilityName = abilityNameBase + "D1";
    std::string abilityName2 = abilityNameBase + "D2";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);

    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1700 end";
}

/*
 * @tc.number    : AMS_Page_Ability_1800
 * @tc.name      : start different page ability in different app sequencely,launchtype[D1,E1:singleton]
 * @tc.desc      : 1.start a page ability D1
 *                 2.wait for the event sent by the page ability
 *                 3.start a page ability E1
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_1800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1800 start";

    std::string bundleName = bundleNameBase + "D";
    std::string abilityName = abilityNameBase + "D1";
    std::string bundleName2 = bundleNameBase + "E";
    std::string abilityName2 = abilityNameBase + "E1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    // start ability in another app
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1800 end";
}

/*
 * @tc.number    : AMS_Page_Ability_1900
 * @tc.name      : start page ability by another ability in the same app,launchtype[D1,D2:singleton]
 * @tc.desc      : 1.start a page ability D1
 *                 2.start a page ability D2 by D1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_1900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1900 start";

    std::string bundleName = bundleNameBase + "D";
    std::string abilityName = abilityNameBase + "D1";
    std::string abilityName2 = abilityNameBase + "D2";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName;
    params["targetAbility"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_1900 end";
}

/*
 * @tc.number    : AMS_Page_Ability_2000
 * @tc.name      : start page ability by itself,launchtype[D1:singleton]
 * @tc.desc      : 1.start a page ability D1
 *                 2.start itself by D1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_2000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2000 start";

    std::string bundleName = bundleNameBase + "D";
    std::string abilityName = abilityNameBase + "D1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName;
    params["targetAbility"] = abilityName;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);

    ExpectAbilityNumInStack(abilityName, 1);
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2000 end";
}

/*
 * @tc.number    : AMS_Page_Ability_2100
 * @tc.name      : start page ability by another ability in different app,launchtype[D1,E1:singleton]
 * @tc.desc      : 1.start a page ability D1
 *                 2.start a page ability E1 by D1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_2100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2100 start";

    std::string bundleName = bundleNameBase + "D";
    std::string abilityName = abilityNameBase + "D1";
    std::string bundleName2 = bundleNameBase + "E";
    std::string abilityName2 = abilityNameBase + "E1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName2;
    params["targetAbility"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2100 end";
}

/*
 * @tc.number    : AMS_Page_Ability_2200
 * @tc.name      : terminate page ability by ability itself(one ability),launchtype[A1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.terminate a page ability A1 by A1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_2200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2200 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    MAP_STR_STR params;
    params["shouldReturn"] = abilityName;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStop, abilityStateCountOne), 0);

    // ability "A1" not in stack(terminated)
    ExpectAbilityNumInStack(abilityName, 0);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2200 end";
}

/*
 * @tc.number    : AMS_Page_Ability_2300
 * @tc.name      : terminate page ability by ability itself(same app),launchtype[A1,A2:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.start a page ability A2 by A1 onActive function.
 *                 3.terminate a page ability A2 by A2 onActive function.
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_2300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2300 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string abilityName2 = abilityNameBase + "A2";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName;
    params["targetAbility"] = abilityName2;
    params["shouldReturn"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);

    // ability "A1" state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // ability "A2" not in stack(terminated)
    ExpectAbilityNumInStack(abilityName2, 0);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2300 end";
}

/*
 * @tc.number    : AMS_Page_Ability_2400
 * @tc.name      : terminate page ability by ability itself(multiple app),launchtype[A1,B1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.start a page ability B1 by A1 onActive function.
 *                 3.terminate a page ability B1 by B1 onActive function.
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_2400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2400 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName2 = abilityNameBase + "B1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName2;
    params["targetAbility"] = abilityName2;
    params["shouldReturn"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);

    // ability "A1" state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // ability "B1" not in stack(terminated)
    ExpectAbilityNumInStack(abilityName2, 0);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2400 end";
}

/*
 * @tc.number    : AMS_Page_Ability_2500
 * @tc.name      : terminate previous ability of current ability,launchtype[A1,A2:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.start a page ability A2 by A1 onActive function.
 *                 3.terminate a page ability A1 by A1 onActive function.
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_2500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2500 start";

    std::string bundleName = bundleNameBase + "A";
    std::string abilityName = abilityNameBase + "A1";
    std::string abilityName2 = abilityNameBase + "A2";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName;
    params["targetAbility"] = abilityName2;
    params["shouldReturn"] = abilityName;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStop, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);

    // ability "A1" not in stack(terminated)
    ExpectAbilityNumInStack(abilityName, 0);
    // ability "A2" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2500 end";
}

/*
 * @tc.number    : AMS_Page_Ability_2600
 * @tc.name      : verify mission stack management(singletop(top) + SI),launchtype[I1,I2,I4:singletop,I3:singleton]
 * @tc.desc      : 1.start page ability I1,I2,I3,I4 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_2600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2600 start";
    std::string bundleName1 = bundleNameBase + "I";
    std::string abilityName1 = abilityNameBase + "I1";
    std::string abilityName2 = abilityNameBase + "I2";
    std::string abilityName3 = abilityNameBase + "I3";
    std::string abilityName4 = abilityNameBase + "I4";
    MAP_STR_STR params;
    // Start Ability I1(singletop)
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    // Start Ability I2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability I3(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability I4(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName4, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName4 + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName4 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);

    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState((DUMP_STACK + " 1"), dumpInfo);
    std::vector<std::string> result;

    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
    std::vector<std::string> expectedResult = {abilityName4, abilityName2, abilityName1, abilityName3};
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo);
    EXPECT_TRUE(missionStackInfo.missionRecords.size() == 2);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    // ability in mission from top to bottom
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName4));
    EXPECT_EQ(0, abilityInfos[1].mainName.compare(abilityName2));
    EXPECT_EQ(0, abilityInfos[2].mainName.compare(abilityName1));
    // ability in mission from top to bottom
    abilityInfos = missionStackInfo.missionRecords[1].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName3));

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2600 end";
}

/*
 * @tc.number    : AMS_Page_Ability_2700
 * @tc.name      : after ability back, the status of other page abilities is displayed
 *                 (singletop back),launchtype[I1,I2,I4:singletop,I3:singleton]
 * @tc.desc      : 1.start page ability I1,I2,I3,I4 in turn
 *                 2.page ability I4 back,dump other abilities state
 *                 3.page ability I2 back,dump other abilities state
 *                 4.page ability I1 back,dump other abilities state
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_2700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2700 start";
    std::string bundleName1 = bundleNameBase + "I";
    std::string abilityName1 = abilityNameBase + "I1";
    std::string abilityName2 = abilityNameBase + "I2";
    std::string abilityName3 = abilityNameBase + "I3";
    std::string abilityName4 = abilityNameBase + "I4";
    MAP_STR_STR params;
    // Start Ability I1(singletop)
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    // Start Ability I2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability I3(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability I4(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName4, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName4 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Back Ability I4(singletop)
    params["shouldReturn"] = abilityName4;
    want = STAbilityUtil::MakeWant("device", abilityName4, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName4 + abilityStateOnStop, abilityStateCountOne), 0);

    // Ability I4 back ,Expect that the current Ability I2 is active
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    ExpectAbilityCurrentState(abilityName3, AbilityState_Test::BACKGROUND, AbilityState_Test::ACTIVATING);
    // Back Ability I2(singletop)
    params["shouldReturn"] = abilityName2;
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);

    // Ability I2 back ,Expect that the current Ability I1 is active
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    ExpectAbilityCurrentState(abilityName3, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // Back Ability I1(singletop)
    params["shouldReturn"] = abilityName1;
    want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnStop, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(launcherAbilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING, DUMP_ALL);
    ExpectAbilityCurrentState(abilityName3, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2700 end";
}

/*
 * @tc.number    : AMS_Page_Ability_2800
 * @tc.name      : verify mission stack management(singletop + singleton),launchtype[I1,I2,B1:singletop,I3:singleton]
 * @tc.desc      : 1.start page ability I1,I2,I3,B1,I3 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_2800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2800 start";
    std::string bundleName1 = bundleNameBase + "I";
    std::string abilityName1 = abilityNameBase + "I1";
    std::string abilityName2 = abilityNameBase + "I2";
    std::string abilityName3 = abilityNameBase + "I3";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName5 = abilityNameBase + "B1";
    // Start Ability I1(singletop)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    // Start Ability I2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);
    // Start Ability I3(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability B1(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName5, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability I3(SI) Again
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnBackground, abilityStateCountOne), 0);
    ExpectAbilityNumInStack(abilityName3, 1);
    ExpectAbilityCurrentState(abilityName3, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState((DUMP_STACK + " 1"), dumpInfo);
    std::vector<std::string> result;
    // Get all AbilityNames in the stack
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
    std::vector<std::string> expectedResult = {abilityName3, abilityName5, abilityName2, abilityName1};
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo);
    EXPECT_TRUE(missionStackInfo.missionRecords.size() == 3);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    // ability in mission from top to bottom
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName3));
    // ability in mission from top to bottom
    abilityInfos = missionStackInfo.missionRecords[1].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName5));

    // ability in mission from top to bottom
    abilityInfos = missionStackInfo.missionRecords[2].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName2));
    EXPECT_EQ(0, abilityInfos[1].mainName.compare(abilityName1));

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2800 end";
}

/*
 * @tc.number    : AMS_Page_Ability_2900
 * @tc.name      : after ability back, the status of other page abilities is displayed
 *                 (singleton back),launchtype[I1,I2,B1:singletop,I3:singleton]
 * @tc.desc      : 1.start page ability I1,I2,I3,B1,I3 in turn
 *                 2.ability I3 back
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_2900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2900 start";
    std::string bundleName1 = bundleNameBase + "I";
    std::string abilityName1 = abilityNameBase + "I1";
    std::string abilityName2 = abilityNameBase + "I2";
    std::string abilityName3 = abilityNameBase + "I3";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName5 = abilityNameBase + "B1";
    // Start Ability I1(singletop)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    // Start Ability I2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability I3(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability B1(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName5, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability I3(SI) Again
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityNumInStack(abilityName3, 1);
    ExpectAbilityCurrentState(abilityName3, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // Start Ability I3(SI) Back
    params["shouldReturn"] = abilityName3;
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnStop, abilityStateCountOne), 0);

    // Ability I3 back ,Expect that the current Ability B1 is active
    ExpectAbilityCurrentState(abilityName5, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_2900 end";
}

/*
 * @tc.number    : AMS_Page_Ability_3000
 * @tc.name      : verify mission stack management,After starting singletop singleton and
 *                 ENTITY_HOME,launchtype[I1,I2,B1:singletop,I3:singleton]
 * @tc.desc      : 1.start page ability I1,I2,I3,ENTITY_HOME,B1,I3 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_3000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3000 start";
    std::string bundleName1 = bundleNameBase + "I";
    std::string abilityName1 = abilityNameBase + "I1";
    std::string abilityName2 = abilityNameBase + "I2";
    std::string abilityName3 = abilityNameBase + "I3";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName5 = abilityNameBase + "B1";

    // Start Ability I1(singletop)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    // Start Ability I2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability I3(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    // simulate ENTITY_HOME
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability B1(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName5, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountOne), 0);

    // Start Ability I3 again(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnBackground, abilityStateCountOne), 0);

    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo);
    EXPECT_TRUE(missionStackInfo.missionRecords.size() == 3);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    // ability in mission from top to bottom
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName3));

    // ability in mission from top to bottom
    abilityInfos = missionStackInfo.missionRecords[1].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName5));

    // ability in mission from top to bottom
    abilityInfos = missionStackInfo.missionRecords[2].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName2));
    EXPECT_EQ(0, abilityInfos[1].mainName.compare(abilityName1));

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3000 end";
}

/*
 * @tc.number    : AMS_Page_Ability_3100
 * @tc.name      : after ability back, the status of other page abilities is displayed
 *                 (singleton back),launchtype[I1,I2,B1:singletop,I3:singleton]
 * @tc.desc      : 1.start page ability I1,I2,I3,ENTITY_HOME,B1,I3 in turn
 *                 2.ability I3 back
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_3100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3100 start";
    std::string bundleName1 = bundleNameBase + "I";
    std::string abilityName1 = abilityNameBase + "I1";
    std::string abilityName2 = abilityNameBase + "I2";
    std::string abilityName3 = abilityNameBase + "I3";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName5 = abilityNameBase + "B1";

    // Start Ability I1(singletop)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    // Start Ability I2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability I3(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    // simulate ENTITY_HOME
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);

    // Start Ability B1(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName5, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountOne), 0);

    // Start Ability I3 again(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityNumInStack(abilityName3, 1);
    ExpectAbilityCurrentState(abilityName3, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    // I3(SI) back
    params["shouldReturn"] = abilityName3;
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountTwo), 0);

    // Ability I3 back, B1(singletop) State Is ACTIVE
    ExpectAbilityCurrentState(abilityName5, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // B1(singletop) back
    params["shouldReturn"] = abilityName5;
    want = STAbilityUtil::MakeWant("device", abilityName5, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountThree), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnBackground, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnStop, abilityStateCountOne), 0);

    // Ability B1 back ,Expect that the current launcher is active
    ExpectAbilityCurrentState(launcherAbilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING, DUMP_ALL);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3100 end";
}

/*
 * @tc.number    : AMS_Page_Ability_3200
 * @tc.name      : ability state transition in the same app,launchtype[G1:singleton,G2:singletop]
 * @tc.desc      : 1.start page ability G1,G2 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_3200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3200 start";
    std::string bundleName1 = bundleNameBase + "G";
    std::string abilityName1 = abilityNameBase + "G1";
    std::string abilityName2 = abilityNameBase + "G2";

    // Start Ability G1(SI)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    // Start Ability G2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3200 end";
}

/*
 * @tc.number    : AMS_Page_Ability_3300
 * @tc.name      : ability state transition in the different app,launchtype[G1:singleton,H1:singletop]
 * @tc.desc      : 1.start page ability G1,H1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_3300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3300 start";
    std::string bundleName1 = bundleNameBase + "G";
    std::string abilityName1 = abilityNameBase + "G1";
    std::string bundleName2 = bundleNameBase + "H";
    std::string abilityName2 = abilityNameBase + "H1";

    // Start Ability G1(SI)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    // Start Ability H1(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3300 end";
}

/*
 * @tc.number    : AMS_Page_Ability_3400
 * @tc.name      : ability state transition in the different app,launchtype[G1:singleton,ENTITY_HOME]
 * @tc.desc      : 1.start page ability G1,ENTITY_HOME in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_3400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3400 start";
    std::string bundleName1 = bundleNameBase + "G";
    std::string abilityName1 = abilityNameBase + "G1";

    // Start Ability G1(SI)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    // simulate ENTITY_HOME,G1 go backstage.
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3400 end";
}

/*
 * @tc.number    : AMS_Page_Ability_3500
 * @tc.name      : ability state transition in the same app,launchtype[G1:singleton,G2:singletop]
 * @tc.desc      : 1.start a page ability G1
 *                 2.start a page ability G2 by G1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_3500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3500 start";
    std::string bundleName1 = bundleNameBase + "G";
    std::string abilityName1 = abilityNameBase + "G1";
    std::string abilityName2 = abilityNameBase + "G2";

    // Start Ability G1(SI)
    MAP_STR_STR params;
    params["targetBundle"] = bundleName1;
    params["targetAbility"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3500 end";
}

/*
 * @tc.number    : AMS_Page_Ability_3600
 * @tc.name      : ability state transition in the different app,launchtype[G1:singleton,H1:singletop]
 * @tc.desc      : 1.start a page ability G1
 *                 2.start a page ability H1 by G1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_3600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3600 start";
    std::string bundleName1 = bundleNameBase + "G";
    std::string abilityName1 = abilityNameBase + "G1";
    std::string bundleName2 = bundleNameBase + "H";
    std::string abilityName2 = abilityNameBase + "H1";

    // Start Ability G1(SI)
    MAP_STR_STR params;
    params["targetBundle"] = bundleName2;
    params["targetAbility"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3600 end";
}

/*
 * @tc.number    : AMS_Page_Ability_3700
 * @tc.name      : ability state transition in the same app when ability back,launchtype[G1:singleton,G2:singletop]
 * @tc.desc      : 1.start page ability G1,G2 in turn
 *                 2.ability G2 back
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_3700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3700 start";
    std::string bundleName1 = bundleNameBase + "G";
    std::string abilityName1 = abilityNameBase + "G1";
    std::string abilityName2 = abilityNameBase + "G2";

    // Start Ability G1(SI)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // Start Ability G2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // Back Ability G2(singletop)
    params["shouldReturn"] = abilityName2;
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3700 end";
}

/*
 * @tc.number    : AMS_Page_Ability_3800
 * @tc.name      : ability state transition in the different app when ability back,launchtype[G1:singleton,H1:singletop]
 * @tc.desc      : 1.start page ability G1,H1 in turn
 *                 2.ability H1 back
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_3800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3800 start";
    std::string bundleName1 = bundleNameBase + "G";
    std::string abilityName1 = abilityNameBase + "G1";
    std::string bundleName2 = bundleNameBase + "H";
    std::string abilityName2 = abilityNameBase + "H1";

    // Start Ability G1(SI)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // Start Ability H1(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // Back Ability H1(singletop)
    params["shouldReturn"] = abilityName2;
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);
    // Back Ability H1(singletop), G1(SI) Ability State Is ACTIVE
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3800 end";
}

/*
 * @tc.number    : AMS_Page_Ability_3900
 * @tc.name      : ability state transition when after ability enters
 *                 the background and start it again,launchtype[G1:singleton]
 * @tc.desc      : 1.start page ability G1,ENTITY_HOME,G1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_3900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3900 start";
    std::string bundleName1 = bundleNameBase + "G";
    std::string abilityName1 = abilityNameBase + "G1";

    // Start Ability G1(SI)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    // simulate ENTITY_HOME,G1 go backstage.
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    // Start Ability G1(SI) again
    want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountTwo), 0);

    ExpectAbilityNumInStack(abilityName1, 1);
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_3900 end";
}

/*
 * @tc.number    : AMS_Page_Ability_4000
 * @tc.name      : the transition state when ability terminate,launchtype[G1:singleton]
 * @tc.desc      : 1.start page ability G1
 *                 2.wait for the event sent by the page ability
 *                 3.terminate ability G1
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_4000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4000 start";
    std::string bundleName1 = bundleNameBase + "G";
    std::string abilityName1 = abilityNameBase + "G1";

    // Start Ability G1(SI)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // Terminate Ability G1(SI)
    params["shouldReturn"] = abilityName1;
    want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnNewWant, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnStop, abilityStateCountOne), 0);

    ExpectAbilityNumInStack(abilityName1, 0);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4000 end";
}

/*
 * @tc.number    : AMS_Page_Ability_4100
 * @tc.name      : the transition state when ability terminate(the same app),
 *                 launchtype[G1:singleton,G2:singletop]
 * @tc.desc      : 1.start page ability G1,G2 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.terminate ability G2
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_4100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4100 start";
    std::string bundleName1 = bundleNameBase + "G";
    std::string abilityName1 = abilityNameBase + "G1";
    std::string abilityName2 = abilityNameBase + "G2";

    // Start Ability G1(SI)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // Start Ability G2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // terminate Ability G2(singletop)
    params["shouldReturn"] = abilityName2;
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4100 end";
}

/*
 * @tc.number    : AMS_Page_Ability_4200
 * @tc.name      : the transition state when ability terminate(the different app),
 *                 launchtype[G1:singleton,H1:singletop]
 * @tc.desc      : 1.start page ability G1,H1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.terminate ability H1
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_4200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4200 start";
    std::string bundleName1 = bundleNameBase + "G";
    std::string abilityName1 = abilityNameBase + "G1";
    std::string bundleName2 = bundleNameBase + "H";
    std::string abilityName2 = abilityNameBase + "H1";

    // Start Ability G1(SI)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // Start Ability H1(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    // terminate Ability H1(singletop)
    params["shouldReturn"] = abilityName2;
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnNewWant, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountTwo), 0);

    // terminate Ability H1(singletop), G1(SI) Ability State Is ACTIVE
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4200 end";
}

/*
 * @tc.number    : AMS_Page_Ability_4300
 * @tc.name      : the transition state when ability terminate,
 *                 launchtype[G1:singleton,G2:singletop]
 * @tc.desc      : 1.start page ability G1,G2,G2 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.terminate ability G1
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_4300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4300 start";
    std::string bundleName1 = bundleNameBase + "G";
    std::string abilityName1 = abilityNameBase + "G1";
    std::string abilityName2 = abilityNameBase + "G2";

    // Start Ability G1(SI)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // Start Ability G2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    // Start Ability G2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnNewWant, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountTwo), 0);

    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    // terminate Ability G1(SI)
    params["shouldReturn"] = abilityName1;
    want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnStop, abilityStateCountOne), 0);

    // terminate Ability G1(SI), Launcher Ability State Is ACTIVE
    ExpectAbilityCurrentState(launcherAbilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING, DUMP_ALL);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4300 end";
}

/*
 * @tc.number    : AMS_Page_Ability_4400
 * @tc.name      : start a page ability with launchtype standard,launchtype[N1:standard]
 * @tc.desc      : 1.start a page ability
 *                 2.wait for the event sent by the page ability
 *                 3.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_4400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4400 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4400 end";
}

/*
 * @tc.number    : AMS_Page_Ability_4500
 * @tc.name      : start a page ability twice,launchtype[N1:standard]
 * @tc.desc      : 1.start a page ability
 *                 2.wait for the event sent by the page ability
 *                 3.start page ability again
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 *                 6.dump the stack info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_4500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4500 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);

    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // two record in stack
    ExpectAbilityNumInStack(abilityName, 2);
    // ability state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4500 end";
}

/*
 * @tc.number    : AMS_Page_Ability_4600
 * @tc.name      : start different page ability in the same app sequencely,launchtype[N1,N2:standard]
 * @tc.desc      : 1.start a page ability N1
 *                 2.wait for the event sent by the page ability
 *                 3.start a page ability N2
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 *                 6.dump the stack info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_4600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4600 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    // start ability in the same app
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // ability "N1" in stack
    ExpectAbilityNumInStack(abilityName, 1);
    // ability "N1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // ability "N2" in stack
    ExpectAbilityNumInStack(abilityName2, 1);
    // ability "N2" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4600 end";
}

/*
 * @tc.number    : AMS_Page_Ability_4700
 * @tc.name      : start different page ability in different app sequencely,launchtype[N1,M1:standard]
 * @tc.desc      : 1.start a page ability N1
 *                 2.wait for the event sent by the page ability
 *                 3.start a page ability M1
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 *                 6.dump the stack info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_4700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4700 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    std::string bundleName2 = bundleNameBase + "M";
    std::string abilityName2 = abilityNameBase + "M1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    // start ability in another app
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_, 5 * 1000);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // ability "N1" in stack
    ExpectAbilityNumInStack(abilityName, 1);
    // ability "N1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // ability "M1" in stack
    ExpectAbilityNumInStack(abilityName2, 1);
    // ability "M1" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4700 end";
}

/*
 * @tc.number    : AMS_Page_Ability_4800
 * @tc.name      : start page ability by another ability in the same app,launchtype[N1,N2:standard]
 * @tc.desc      : 1.start a page ability N1
 *                 2.start a page ability N2 by N1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_4800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4800 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName;
    params["targetAbility"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // ability "N1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // ability "N2" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4800 end";
}

/*
 * @tc.number    : AMS_Page_Ability_4900
 * @tc.name      : start page ability by itself,launchtype[N1:standard]
 * @tc.desc      : 1.start a page ability N1
 *                 2.start itself by N1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 *                 5.dump the stack info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_4900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4900 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName;
    params["targetAbility"] = abilityName;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // two record in stack
    ExpectAbilityNumInStack(abilityName, 2);
    // ability state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_4900 end";
}

/*
 * @tc.number    : AMS_Page_Ability_5000
 * @tc.name      : start page ability by another ability in different app,launchtype[A1,M1:singletop]
 * @tc.desc      : 1.start a page ability N1
 *                 2.start a page ability M1 by N1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_5000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5000 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    std::string bundleName2 = bundleNameBase + "M";
    std::string abilityName2 = abilityNameBase + "M1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName2;
    params["targetAbility"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // ability "N1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // ability "M1" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5000 end";
}

/*
 * @tc.number    : AMS_Page_Ability_5100
 * @tc.name      : verify mission stack management,launchtype[N1,N2,N4:singletop]
 * @tc.desc      : 1.start page ability N1,N2,N2,N4,N2 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_5100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5100 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";
    std::string abilityName3 = abilityNameBase + "N4";
    std::string abilityName4 = abilityNameBase + "N2";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName, params);
    STAbilityUtil::StartAbility(want2, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    STAbilityUtil::StartAbility(want2, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    Want want3 = STAbilityUtil::MakeWant("device", abilityName3, bundleName, params);
    STAbilityUtil::StartAbility(want3, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    Want want4 = STAbilityUtil::MakeWant("device", abilityName4, bundleName, params);
    STAbilityUtil::StartAbility(want4, abilityMs_);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);

    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState(DUMP_STACK + " 1", dumpInfo);
    std::vector<std::string> result;
    // bundle name from top to bottom
    MTDumpUtil::GetInstance()->GetAll("BundleName", dumpInfo, result);
    std::vector<std::string> expectedResult = {
        bundleNameBase + "N",
        bundleNameBase + "N",
        bundleNameBase + "N",
        bundleNameBase + "N",
        bundleNameBase + "N",
    };
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));

    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
    // ability name from top to bottom
    expectedResult = {
        abilityNameBase + "N2",
        abilityNameBase + "N4",
        abilityNameBase + "N2",
        abilityNameBase + "N2",
        abilityNameBase + "N1",
    };
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5100 end";
}

/*
 * @tc.number    : AMS_Page_Ability_5200
 * @tc.name      : verify mission stack management(multiple apps),launchtype[N1,N2,M1:standard]
 * @tc.desc      : 1.start page ability N1,N2,M1,N2,M1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_5200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5200 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";
    std::string bundleName2 = bundleNameBase + "M";
    std::string abilityName3 = abilityNameBase + "M1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName, params);
    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    Want want3 = STAbilityUtil::MakeWant("device", abilityName3, bundleName2, params);
    STAbilityUtil::StartAbility(want3, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);

    STAbilityUtil::StartAbility(want3, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState(DUMP_STACK + " 1", dumpInfo);

    std::vector<std::string> result;
    // bundle name from top to bottom
    MTDumpUtil::GetInstance()->GetAll("BundleName", dumpInfo, result);
    std::vector<std::string> expectedResult = {
        bundleNameBase + "M",
        bundleNameBase + "N",
        bundleNameBase + "M",
        bundleNameBase + "N",
        bundleNameBase + "N",
    };
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));

    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
    // ability name from top to bottom
    expectedResult = {
        abilityNameBase + "M1",
        abilityNameBase + "N2",
        abilityNameBase + "M1",
        abilityNameBase + "N2",
        abilityNameBase + "N1",
    };
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5200 end";
}

/*
 * @tc.number    : AMS_Page_Ability_5300
 * @tc.name      : verify mission stack management(multiple apps + ENTITY_HOME),launchtype[N1,N2,M1:standard]
 * @tc.desc      : 1.start page ability N1,N2,ENTITY_HOME,M1,N2,M1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_5300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5300 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";
    std::string bundleName3 = bundleNameBase + "M";
    std::string abilityName3 = abilityNameBase + "M1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    Want want2 = STAbilityUtil::MakeWant("device", abilityName2, bundleName, params);
    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // simulate ENTITY_HOME
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    Want want3 = STAbilityUtil::MakeWant("device", abilityName3, bundleName3, params);
    STAbilityUtil::StartAbility(want3, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);

    STAbilityUtil::StartAbility(want2, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);

    STAbilityUtil::StartAbility(want3, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo);
    EXPECT_TRUE(missionStackInfo.missionRecords.size() == 2);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    // ability in mission #2 from top to bottom
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityNameBase + "M1"));
    EXPECT_EQ(0, abilityInfos[1].mainName.compare(abilityNameBase + "N2"));
    EXPECT_EQ(0, abilityInfos[2].mainName.compare(abilityNameBase + "M1"));
    // ability in mission #1 from top to bottom
    abilityInfos = missionStackInfo.missionRecords[1].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityNameBase + "N2"));
    EXPECT_EQ(0, abilityInfos[1].mainName.compare(abilityNameBase + "N1"));

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5300 end";
}

/*
 * @tc.number    : AMS_Page_Ability_5400
 * @tc.name      : ability state transition to background when Home event,launchtype[N1:standard]
 * @tc.desc      : 1.start page ability N1,ENTITY_HOME in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_5400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5400 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    // simulate ENTITY_HOME
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // ability "N1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5400 end";
}

/*
 * @tc.number    : AMS_Page_Ability_5500
 * @tc.name      : last ability state transition to foreground when stopping top ability
 *                 (same app),launchtype[N1,N2:standard]
 * @tc.desc      : 1.start page ability N1,N2 in turn
 *                 2.stop ability N2
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_5500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5500 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    // start ability in the same app
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // ability "N1" in stack
    ExpectAbilityNumInStack(abilityName, 1);
    // ability "N1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // ability "N2" in stack
    ExpectAbilityNumInStack(abilityName2, 1);
    // ability "N2" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    STAbilityUtil::StopAbility(terminatePageAbility, 0, abilityName2);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnForeground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);

    // ability "N1" state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // ability "N2" not in stack
    ExpectAbilityNumInStack(abilityName2, 0);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5500 end";
}

/*
 * @tc.number    : AMS_Page_Ability_5600
 * @tc.name      : last ability state transition to foreground when stopping top ability
 *                 (different app),launchtype[N1,M1:standard]
 * @tc.desc      : 1.start page ability N1,M1 in turn
 *                 2.stop ability M1
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_5600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5600 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    std::string bundleName2 = bundleNameBase + "M";
    std::string abilityName2 = abilityNameBase + "M1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    // start ability in another app
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // ability "N1" in stack
    ExpectAbilityNumInStack(abilityName, 1);
    // ability "N1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    // ability "M1" in stack
    ExpectAbilityNumInStack(abilityName2, 1);
    // ability "M1" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    STAbilityUtil::StopAbility(terminatePageAbility, 0, abilityName2);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnForeground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);

    // ability "N1" state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // ability "M1" not in stack
    ExpectAbilityNumInStack(abilityName2, 0);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5600 end";
}

/*
 * @tc.number    : AMS_Page_Ability_5700
 * @tc.name      : ability state transition to foreground when background app restart,launchtype[N1:standard]
 * @tc.desc      : 1.start page ability N1,ENTITY_HOME,N1 in turn
 *                 2.wait for the event sent by the page ability
 *                 3.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_5700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5700 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);

    // simulate ENTITY_HOME
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);

    // ability "A1" state BACKGROUND
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);

    // background app restart
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);

    // ability "A1" state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5700 end";
}

/*
 * @tc.number    : AMS_Page_Ability_5800
 * @tc.name      : terminate page ability by ability itself(one ability),launchtype[N1:standard]
 * @tc.desc      : 1.start a page ability N1
 *                 2.terminate a page ability N1 by N1 onActive function.
 *                 3.wait for the event sent by the page ability
 *                 4.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_5800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5800 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    MAP_STR_STR params;
    params["shouldReturn"] = abilityName;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStop, abilityStateCountOne), 0);

    // ability "N1" not in stack(terminated)
    ExpectAbilityNumInStack(abilityName, 0);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5800 end";
}

/*
 * @tc.number    : AMS_Page_Ability_5900
 * @tc.name      : terminate page ability by ability itself(same app),launchtype[N1,N2:standard]
 * @tc.desc      : 1.start a page ability N1
 *                 2.start a page ability N2 by N1 onActive function.
 *                 3.terminate a page ability N2 by N2 onActive function.
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_5900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5900 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName;
    params["targetAbility"] = abilityName2;
    params["shouldReturn"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);

    // ability "N1" state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // ability "N2" not in stack(terminated)
    ExpectAbilityNumInStack(abilityName2, 0);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_5900 end";
}

/*
 * @tc.number    : AMS_Page_Ability_6000
 * @tc.name      : terminate page ability by ability itself(multiple app),launchtype[N1,M1:standard]
 * @tc.desc      : 1.start a page ability N1
 *                 2.start a page ability M1 by N1 onActive function.
 *                 3.terminate a page ability M1 by M1 onActive function.
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_6000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_6000 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    std::string bundleName2 = bundleNameBase + "M";
    std::string abilityName2 = abilityNameBase + "M1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName2;
    params["targetAbility"] = abilityName2;
    params["shouldReturn"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountTwo), 0);

    // ability "N1" state ACTIVE
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // ability "M1" not in stack(terminated)
    ExpectAbilityNumInStack(abilityName2, 0);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_6000 end";
}

/*
 * @tc.number    : AMS_Page_Ability_6100
 * @tc.name      : terminate previous ability of current ability,launchtype[N1,N2:standard]
 * @tc.desc      : 1.start a page ability N1
 *                 2.start a page ability N2 by N1 onActive function.
 *                 3.terminate a page ability N1 by N1 onActive function.
 *                 4.wait for the event sent by the page ability
 *                 5.dump the ability info
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_6100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_6100 start";

    std::string bundleName = bundleNameBase + "N";
    std::string abilityName = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName;
    params["targetAbility"] = abilityName2;
    params["shouldReturn"] = abilityName;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName + abilityStateOnStop, abilityStateCountOne), 0);

    // ability "N1" not in stack(terminated)
    ExpectAbilityNumInStack(abilityName, 0);
    // ability "N2" state ACTIVE
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);

    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_6100 end";
}

/*
 * @tc.number    : AMS_Page_Ability_6200
 * @tc.name      : verify mission stack management(standard + singleton),launchtype[N1,N2,M1:standard,N3:singleton]
 * @tc.desc      : 1.start page ability N1
 *                 2.start page ability N2 by N1 onActive function
 *                 3.start page ability N3 by N2 onActive function
 *                 4.start page ability M1 by N3 onActive function
 *                 5.start page ability N3 by M1 onActive function
 *                 6.wait for the event sent by the page ability
 *                 7.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_6200, Function | MediumTest | Level1)
{
    std::string bundleName1 = bundleNameBase + "N";
    std::string abilityName1 = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";
    std::string abilityName3 = abilityNameBase + "N3";
    std::string bundleName2 = bundleNameBase + "M";
    std::string abilityName5 = abilityNameBase + "M1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName1 + "," + bundleName1 + "," + bundleName2 + "," + bundleName1;
    params["targetAbility"] = abilityName2 + "," + abilityName3 + "," + abilityName5 + "," + abilityName3;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnStart, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnInactive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnForeground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnBackground, abilityStateCountOne), 0);
    ExpectAbilityNumInStack(abilityName3, 1);
    ExpectAbilityCurrentState(abilityName3, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    std::vector<std::string> result;
    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState((DUMP_STACK + " 1"), dumpInfo);
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
    for (auto info : result) {
        GTEST_LOG_(INFO) << info;
    }
    std::vector<std::string> expectedResult = {abilityName3, abilityName5, abilityName2, abilityName1};
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo);
    EXPECT_TRUE(missionStackInfo.missionRecords.size() == 3);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName3));
    abilityInfos = missionStackInfo.missionRecords[1].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName5));
    abilityInfos = missionStackInfo.missionRecords[2].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName2));
    EXPECT_EQ(0, abilityInfos[1].mainName.compare(abilityName1));
}

/*
 * @tc.number    : AMS_Page_Ability_6300
 * @tc.name      : verify mission stack management(standard + singleton),launchtype[N1,N2,M1:standard,N3:singleton]
 * @tc.desc      : 1.start page ability N1
 *                 2.start page ability N2 by N1 onActive function
 *                 3.start page ability N3 by N2 onActive function
 *                 4.start page ability M1 by N3 onActive function
 *                 5.start page ability N3 by M1 onActive function
 *                 6.start page ability N1 by N3 onActive function
 *                 7.wait for the event sent by the page ability
 *                 8.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_6300, Function | MediumTest | Level1)
{
    std::string bundleName1 = bundleNameBase + "N";
    std::string abilityName1 = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";
    std::string abilityName3 = abilityNameBase + "N3";
    std::string bundleName2 = bundleNameBase + "M";
    std::string abilityName5 = abilityNameBase + "M1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName1 + "," + bundleName1;
    params["targetAbility"] = abilityName2 + "," + abilityName3;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    ExpectAbilityNumInStack(abilityName3, 1);
    ExpectAbilityCurrentState(abilityName3, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // simulate ENTITY_HOME
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);
    params["targetBundle"] = bundleName1;
    params["targetAbility"] = abilityName3;
    want = STAbilityUtil::MakeWant("device", abilityName5, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnBackground, abilityStateCountOne), 0);
    params.clear();
    want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountTwo), 0);
    std::vector<std::string> result;
    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState((DUMP_STACK + " 1"), dumpInfo);
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
    std::vector<std::string> expectedResult = {abilityName1, abilityName2, abilityName1, abilityName3, abilityName5};
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo);
    EXPECT_TRUE(missionStackInfo.missionRecords.size() == 3);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName1));
    EXPECT_EQ(0, abilityInfos[1].mainName.compare(abilityName2));
    EXPECT_EQ(0, abilityInfos[2].mainName.compare(abilityName1));
    abilityInfos = missionStackInfo.missionRecords[1].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName3));
    abilityInfos = missionStackInfo.missionRecords[2].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName5));
}

/*
 * @tc.number    : AMS_Page_Ability_6400
 * @tc.name      : verify mission stack management(standard + singleton),launchtype[N1,N2,M1:standard,N3:singleton]
 * @tc.desc      : 1.start page ability N1
 *                 2.start page ability N2 by N1 onActive function
 *                 3.start page ability N3 by N2 onActive function
 *                 4.ENTITY_HOME
 *                 5.start page ability M1 by N3 onActive function
 *                 6.start page ability N3 by M1 onActive function
 *                 7.start page ability N2 by N3 onActive function
 *                 8.wait for the event sent by the page ability
 *                 9.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_6400, Function | MediumTest | Level1)
{
    std::string bundleName1 = bundleNameBase + "N";
    std::string abilityName1 = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";
    std::string abilityName3 = abilityNameBase + "N3";
    std::string bundleName2 = bundleNameBase + "M";
    std::string abilityName5 = abilityNameBase + "M1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName1 + "," + bundleName1;
    params["targetAbility"] = abilityName2 + "," + abilityName3;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    ExpectAbilityNumInStack(abilityName3, 1);
    ExpectAbilityCurrentState(abilityName3, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    // simulate ENTITY_HOME
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);
    params["targetBundle"] = bundleName1;
    params["targetAbility"] = abilityName3;
    want = STAbilityUtil::MakeWant("device", abilityName5, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnBackground, abilityStateCountOne), 0);
    params.clear();
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountTwo), 0);
    std::vector<std::string> result;
    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState((DUMP_STACK + " 1"), dumpInfo);
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
    std::vector<std::string> expectedResult = {abilityName2, abilityName1, abilityName3, abilityName5};
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo);
    EXPECT_TRUE(missionStackInfo.missionRecords.size() == 3);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName2));
    EXPECT_EQ(0, abilityInfos[1].mainName.compare(abilityName1));
    abilityInfos = missionStackInfo.missionRecords[1].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName3));
    abilityInfos = missionStackInfo.missionRecords[2].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName5));
}

/*
 * @tc.number    : AMS_Page_Ability_6500
 * @tc.name      : verify mission stack management(singletop + singleton),launchtype[I1,I2,B1:singletop,I3:singleton]
 * @tc.desc      : 1.start page ability I1
 *                 2.start page ability I2
 *                 3.start page ability I3
 *                 4.ENTITY_HOME
 *                 5.start page ability B1
 *                 6.start page ability I3
 *                 7.start page ability I1
 *                 8.wait for the event sent by the page ability
 *                 9.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_6500, Function | MediumTest | Level1)
{
    std::string bundleName1 = bundleNameBase + "I";
    std::string abilityName1 = abilityNameBase + "I1";
    std::string abilityName2 = abilityNameBase + "I2";
    std::string abilityName3 = abilityNameBase + "I3";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName5 = abilityNameBase + "B1";
    // Start Ability I1(singletop)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    // Start Ability I2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);
    // Start Ability I3(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    // simulate ENTITY_HOME
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);
    // Start Ability B1(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName5, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountOne), 0);
    // Start Ability I3 again(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnBackground, abilityStateCountOne), 0);
    ExpectAbilityNumInStack(abilityName3, 1);
    ExpectAbilityCurrentState(abilityName3, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountTwo), 0);
    std::vector<std::string> result;
    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState((DUMP_STACK + " 1"), dumpInfo);
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
    std::vector<std::string> expectedResult = {abilityName1, abilityName2, abilityName1, abilityName3, abilityName5};
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo);
    EXPECT_TRUE(missionStackInfo.missionRecords.size() == 3);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName1));
    EXPECT_EQ(0, abilityInfos[1].mainName.compare(abilityName2));
    EXPECT_EQ(0, abilityInfos[2].mainName.compare(abilityName1));
    abilityInfos = missionStackInfo.missionRecords[1].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName3));
    abilityInfos = missionStackInfo.missionRecords[2].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName5));
}

/*
 * @tc.number    : AMS_Page_Ability_6600
 * @tc.name      : verify mission stack management(singletop + singleton),launchtype[I1,I2,B1:singletop,I3:singleton]
 * @tc.desc      : 1.start page ability I1
 *                 2.start page ability I2
 *                 3.start page ability I3
 *                 4.ENTITY_HOME
 *                 5.start page ability B1
 *                 6.start page ability I3
 *                 7.start page ability I2
 *                 8.wait for the event sent by the page ability
 *                 9.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_6600, Function | MediumTest | Level1)
{
    std::string bundleName1 = bundleNameBase + "I";
    std::string abilityName1 = abilityNameBase + "I1";
    std::string abilityName2 = abilityNameBase + "I2";
    std::string abilityName3 = abilityNameBase + "I3";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName5 = abilityNameBase + "B1";
    // Start Ability I1(singletop)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    // Start Ability I2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);
    // Start Ability I3(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    // simulate ENTITY_HOME
    Want wantEntity;
    wantEntity.AddEntity(Want::FLAG_HW_HOME_INTENT_FROM_SYSTEM);
    STAbilityUtil::StartAbility(wantEntity, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);
    // Start Ability B1(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName5, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountOne), 0);
    // Start Ability I3 again(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnBackground, abilityStateCountOne), 0);
    ExpectAbilityNumInStack(abilityName3, 1);
    ExpectAbilityCurrentState(abilityName3, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountTwo), 0);
    std::vector<std::string> result;
    std::vector<std::string> dumpInfo;
    abilityMs_->DumpState((DUMP_STACK + " 1"), dumpInfo);
    MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
    std::vector<std::string> expectedResult = {abilityName2, abilityName1, abilityName3, abilityName5};
    EXPECT_TRUE(MTDumpUtil::GetInstance()->CompStrVec(result, expectedResult));
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo);
    EXPECT_TRUE(missionStackInfo.missionRecords.size() == 3);
    auto abilityInfos = missionStackInfo.missionRecords[0].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName2));
    EXPECT_EQ(0, abilityInfos[1].mainName.compare(abilityName1));
    abilityInfos = missionStackInfo.missionRecords[1].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName3));
    abilityInfos = missionStackInfo.missionRecords[2].abilityRecordInfos;
    EXPECT_EQ(0, abilityInfos[0].mainName.compare(abilityName5));
}

/*
 * @tc.number    : AMS_Page_Ability_6700
 * @tc.name      : verify mission stack management(standard + singleton),launchtype[N1,N2,M1:standard,N3:singleton]
 * @tc.desc      : 1.start page ability N1
 *                 2.start page ability N2 by N1 onActive function
 *                 3.start page ability N3 by N2 onActive function
 *                 4.start page ability M1 by N3 onActive function
 *                 5.start page ability N3 by M1 onActive function
 *                 6.back N3,M1,N2,N1 in turn
 *                 7.wait for the event sent by the page ability
 *                 8.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_6700, Function | MediumTest | Level1)
{
    std::string bundleName1 = bundleNameBase + "N";
    std::string abilityName1 = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";
    std::string abilityName3 = abilityNameBase + "N3";
    std::string bundleName2 = bundleNameBase + "M";
    std::string abilityName5 = abilityNameBase + "M1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName1 + "," + bundleName1 + "," + bundleName2;
    params["targetAbility"] = abilityName2 + "," + abilityName3 + "," + abilityName5;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);
    ExpectAbilityNumInStack(abilityName5, 1);
    ExpectAbilityCurrentState(abilityName5, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    int eventCode = 0;
    STAbilityUtil::PublishEvent(terminatePageAbility, eventCode, abilityName5);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnStop, abilityStateCountOne), 0);
    STAbilityUtil::PublishEvent(terminatePageAbility, eventCode, abilityName3);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnStop, abilityStateCountOne), 0);
    STAbilityUtil::PublishEvent(terminatePageAbility, eventCode, abilityName2);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);
    STAbilityUtil::PublishEvent(terminatePageAbility, eventCode, abilityName1);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnStop, abilityStateCountOne), 0);
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo);
    EXPECT_TRUE(missionStackInfo.missionRecords.size() == 0);
    ExpectAbilityCurrentState(launcherAbilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING, DUMP_ALL);
}

/*
 * @tc.number    : AMS_Page_Ability_6800
 * @tc.name      : verify mission stack management(standard + singleton),launchtype[N1,N2,M1:standard,N3:singleton]
 * @tc.desc      : 1.start page ability I1
 *                 2.start page ability I2
 *                 3.start page ability I3
 *                 4.start page ability B1
 *                 5.start page ability I3
 *                 6.back I3,B1,I2,I1 in turn
 *                 7.wait for the event sent by the page ability
 *                 8.dump the stack management
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_6800, Function | MediumTest | Level1)
{
    std::string bundleName1 = bundleNameBase + "I";
    std::string abilityName1 = abilityNameBase + "I1";
    std::string abilityName2 = abilityNameBase + "I2";
    std::string abilityName3 = abilityNameBase + "I3";
    std::string bundleName2 = bundleNameBase + "B";
    std::string abilityName5 = abilityNameBase + "B1";
    // Start Ability I1(singletop)
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    // Start Ability I2(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);
    // Start Ability I3(SI)
    want = STAbilityUtil::MakeWant("device", abilityName3, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);
    // Start Ability B1(singletop)
    want = STAbilityUtil::MakeWant("device", abilityName5, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnBackground, abilityStateCountOne), 0);
    ExpectAbilityNumInStack(abilityName5, 1);
    ExpectAbilityCurrentState(abilityName5, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    int eventCode = 0;
    STAbilityUtil::PublishEvent(terminatePageAbility, eventCode, abilityName5);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName5 + abilityStateOnStop, abilityStateCountOne), 0);
    STAbilityUtil::PublishEvent(terminatePageAbility, eventCode, abilityName3);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName3 + abilityStateOnStop, abilityStateCountOne), 0);
    STAbilityUtil::PublishEvent(terminatePageAbility, eventCode, abilityName2);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountTwo), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);
    STAbilityUtil::PublishEvent(terminatePageAbility, eventCode, abilityName1);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnStop, abilityStateCountOne), 0);
    MissionStackInfo missionStackInfo;
    GetAllStackInfo(missionStackInfo);
    EXPECT_TRUE(missionStackInfo.missionRecords.size() == 0);
    ExpectAbilityCurrentState(launcherAbilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING, DUMP_ALL);
}

/*
 * @tc.number    : AMS_Page_Ability_6900
 * @tc.name      : restart app after kill app process,launchtype[A1,C1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.wait for the event sent by the page ability
 *                 3.determine process status
 *                 4.kill current Application
 *                 5.start page ability C1,A1 in turn
 *                 6.wait for the event sent by the page ability
 *                 7.determine process status and ability status
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_6900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_6900 start";

    std::string bundleName1 = bundleNameBase + "A";
    std::string bundleName2 = bundleNameBase + "C";
    std::string abilityName1 = abilityNameBase + "A1";
    std::string abilityName2 = abilityNameBase + "C1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName1, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName1, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo.state_);

    STAbilityUtil::KillApplication(bundleName1, appMs_, WAIT_TIME);
    STAbilityUtil::CleanMsg(event_);
    params["targetBundle"] = bundleName1;
    params["targetAbility"] = abilityName1;
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName2, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnBackground, abilityStateCountOne), 0);

    AppProcessInfo pInfo1 = STAbilityUtil::GetAppProcessInfoByName(bundleName1, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_FOREGROUND, pInfo1.state_);
    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs_, WAIT_TIME);
    EXPECT_EQ(AppProcessState::APP_STATE_BACKGROUND, pInfo2.state_);
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_6900 end";
}

/*
 * @tc.number    : AMS_Page_Ability_7000
 * @tc.name      : restart app after kill app process,launchtype[A1:singletop]
 * @tc.desc      : 1.start a page ability A1
 *                 2.wait for the event sent by the page ability
 *                 3.determine process status
 *                 4.kill current Application
 *                 5.start a page ability A1
 *                 6.wait for the event sent by the page ability
 *                 7.determine process status and ability status
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_7000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_7000 start";

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
    ExpectAbilityCurrentState(abilityName, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_7000 end";
}

/*
 * @tc.number    : AMS_Page_Ability_7100
 * @tc.name      : different hap same bundleName,launchtype[O1,P1:standard]
 * @tc.desc      : 1.start a page ability O1
 *                 2.wait for the event sent by the page ability
 *                 3.get ability O1 pidinfo
 *                 4.start a page ability P1
 *                 6.wait for the event sent by the page ability
 *                 7.get ability P1 pidinfo
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_7100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_7100 start";
    std::string bundleName = bundleNameBase + "O";
    std::string abilityName1 = abilityNameBase + "O1";
    std::string abilityName2 = abilityNameBase + "P1";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName;
    params["targetAbility"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(processInfoEvent_, pidEventName, amsStAbilityO1Code), 0);
    std::string abilityO1Pid = STAbilityUtil::GetData(processInfoEvent_, pidEventName, amsStAbilityO1Code);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);
    std::string abilityP1Pid = STAbilityUtil::GetData(processInfoEvent_, pidEventName, amsStAbilityP1Code);
    EXPECT_EQ(abilityO1Pid, abilityP1Pid);
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_7100 end";
}

/*
 * @tc.number    : AMS_Page_Ability_7200
 * @tc.name      : different hap same bundleName,launchtype[O1,P1:standard]
 * @tc.desc      : 1.start a page ability O1
 *                 2.wait for the event sent by the page ability
 *                 3.get ability O1 status
 *                 4.start a page ability P1
 *                 6.wait for the event sent by the page ability
 *                 7.get ability O1 status
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_7200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_7200 start";
    std::string bundleName = bundleNameBase + "O";
    std::string abilityName1 = abilityNameBase + "O1";
    std::string abilityName2 = abilityNameBase + "P1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    want = STAbilityUtil::MakeWant("device", abilityName2, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);
    ExpectAbilityCurrentState(abilityName2, AbilityState_Test::ACTIVE, AbilityState_Test::ACTIVATING);
    ExpectAbilityCurrentState(abilityName1, AbilityState_Test::BACKGROUND, AbilityState_Test::MOVING_BACKGROUND);
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_7200 end";
}

/*
 * @tc.number    : AMS_Page_Ability_7300
 * @tc.name      : different hap same bundleName,launchtype[M1:standard]
 * @tc.desc      : 1.start a page ability M1
 *                 2.wait for the event sent by the page ability
 *                 3.terminate ability M1
 *                 4.verify callback path and ability state transition
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_7300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_7300 start";
    std::string bundleName = bundleNameBase + "M";
    std::string abilityName1 = abilityNameBase + "M1";
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    int eventCode = 0;
    STAbilityUtil::PublishEvent(terminatePageAbility, eventCode, abilityName1);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnStop, abilityStateCountOne), 0);
    std::string callBackPact = "InitOnStartOnActiveOnInactiveOnBackgroundOnStop";
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + callBackPact, eventCode), 0);
    std::string abilityStatus = lifecycleStateUninitialized + lifecycleStateInactive + lifecycleStateActive +
                                lifecycleStateInactive + lifecycleStateBackground + lifecycleStateInitial;
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStatus, eventCode), 0);
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_7300 end";
}

/*
 * @tc.number    : AMS_Page_Ability_7400
 * @tc.name      : different hap same bundleName,launchtype[N1,N2:standard]
 * @tc.desc      : 1.start a page ability N1
 *                 2.start page ability N2 by N1 onActive function
 *                 3.terminate ability N2,N1
 *                 4.wait for the event sent by the page ability
 *                 5.verify callback path and ability state transition
 */
HWTEST_F(AmsPageAbilityTest, AMS_Page_Ability_7400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_7400 start";
    std::string bundleName = bundleNameBase + "N";
    std::string abilityName1 = abilityNameBase + "N1";
    std::string abilityName2 = abilityNameBase + "N2";
    MAP_STR_STR params;
    params["targetBundle"] = bundleName;
    params["targetAbility"] = abilityName2;
    Want want = STAbilityUtil::MakeWant("device", abilityName1, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnActive, abilityStateCountOne), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnBackground, abilityStateCountOne), 0);
    int eventCode = 0;
    STAbilityUtil::PublishEvent(terminatePageAbility, eventCode, abilityName2);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStateOnStop, abilityStateCountOne), 0);
    STAbilityUtil::PublishEvent(terminatePageAbility, eventCode, abilityName1);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStateOnStop, abilityStateCountOne), 0);
    std::string callBackPactN1 =
        "InitOnStartOnActiveOnInactiveOnBackgroundOnForegroundOnActiveOnInactiveOnBackgroundOnStop";
    std::string callBackPactN2 = "InitOnStartOnActiveOnInactiveOnBackgroundOnStop";

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + callBackPactN1, eventCode), 0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + callBackPactN2, eventCode), 0);
    std::string abilityStatusN1 = lifecycleStateUninitialized + lifecycleStateInactive + lifecycleStateActive +
                                  lifecycleStateInactive + lifecycleStateBackground + lifecycleStateInactive +
                                  lifecycleStateActive + lifecycleStateInactive + lifecycleStateBackground +
                                  lifecycleStateInitial;
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName1 + abilityStatusN1, eventCode), 0);
    std::string abilityStatusN2 = lifecycleStateUninitialized + lifecycleStateInactive + lifecycleStateActive +
                                  lifecycleStateInactive + lifecycleStateBackground + lifecycleStateInitial;
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, abilityName2 + abilityStatusN2, eventCode), 0);
    GTEST_LOG_(INFO) << "AmsPageAbilityTest AMS_Page_Ability_7400 end";
}