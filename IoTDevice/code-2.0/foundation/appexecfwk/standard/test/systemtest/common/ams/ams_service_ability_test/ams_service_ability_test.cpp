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

#include <algorithm>
#include <cstdio>
#include <functional>
#include <gtest/gtest.h>
#include <thread>
#include "ability_connect_callback_proxy.h"
#include "ability_connect_callback_stub.h"
#include "ability_lifecycle_executor.h"
#include "ability_manager_service.h"
#include "ability_manager_errors.h"
#include "ams_service_ability_test_def.h"
#include "app_mgr_service.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "event.h"
#include "hilog_wrapper.h"
#include "module_test_dump_util.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"
#include "system_test_ability_util.h"

namespace {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::MTUtil;
using namespace OHOS::EventFwk;
using namespace OHOS::STtools;
using namespace OHOS::STABUtil;

std::vector<std::string> bundleNameList = {
    BUNDLE_NAME_BASE + "A",
    BUNDLE_NAME_BASE + "B",
    BUNDLE_NAME_BASE + "C",
    BUNDLE_NAME_BASE + "D",
    BUNDLE_NAME_BASE + "E",
    BUNDLE_NAME_BASE + "F",
    BUNDLE_NAME_BASE + "G",
    BUNDLE_NAME_BASE + "H",
};

std::vector<std::string> hapNameList = {
    HAP_NAME_BASE + "A",
    HAP_NAME_BASE + "B",
    HAP_NAME_BASE + "C",
    HAP_NAME_BASE + "D",
    HAP_NAME_BASE + "E",
    HAP_NAME_BASE + "F",
    HAP_NAME_BASE + "G",
    HAP_NAME_BASE + "H",
};

class AmsServiceAbilityTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    void CheckAbilityStateByName(
        const std::string &abilityName, const std::vector<std::string> &info, const std::string &state) const;
    void ExpectPageAbilityCurrentState(const std::string &abilityName, const std::string &currentState) const;
    void ExpectServiceAbilityCurrentState(const std::string &abilityName, const std::string &currentState) const;
    void ExpectDataAbilityCurrentState(const std::string &abilityName, const std::string &currentState) const;
    void ExpectAbilityNumInStack(const std::string &abilityName, int abilityNum) const;
    AbilityInfo MakeAbilityInfo(
        const std::string &name, const std::string &appName, const std::string &bundleName) const;
    static bool SubscribeEvent();

    void AmsServiceAbilityTest16001() const;
    void AmsServiceAbilityTest16002() const;

    void AmsServiceAbilityTest17001() const;
    void AmsServiceAbilityTest17002() const;

    void AmsServiceAbilityTest18001() const;
    void AmsServiceAbilityTest18002() const;

    void AmsServiceAbilityTest23001() const;
    void AmsServiceAbilityTest23002() const;

    static sptr<IAbilityManager> abilityMs;
    static sptr<IAppMgr> appMs;
    static std::vector<std::string> eventList;
    static STtools::Event event;
    static std::map<std::string, int> mapState;

    // Trim from start
    static inline std::string &Ltrim(std::string &s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) { return !std::isspace(c); }));
        return s;
    }
    // Trim from end
    static inline std::string &Rtrim(std::string &s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); }).base(), s.end());
        return s;
    }

    static inline std::string &Trim(std::string &s)
    {
        return Ltrim(Rtrim(s));
    }

    void ExecuteSystemForResult(const string &cmd, string &result)
    {
        result.clear();
        char bufPs[BUFFER_SIZE];
        FILE *ptr;
        if (!cmd.empty() && (ptr = popen(cmd.c_str(), "r")) != NULL) {
            while (fgets(bufPs, BUFFER_SIZE, ptr) != NULL) {
                result.append(bufPs);
            }
            pclose(ptr);
            ptr = NULL;
        }
    }

    class AbilityConnectCallback : public AbilityConnectionStub {
    public:
        sptr<IRemoteObject> AsObject() override
        {
            return nullptr;
        }
        /**
         * OnAbilityConnectDone, AbilityMs notify caller ability the result of connect.
         *
         * @param element,.service ability's ElementName.
         * @param remoteObject,.the session proxy of service ability.
         * @param resultCode, ERR_OK on success, others on failure.
         */
        void OnAbilityConnectDone(
            const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override
        {
            GTEST_LOG_(INFO) << "AbilityConnectCallback::OnAbilityConnectDone:resultCode = " << resultCode;
            if (resultCode == 0) {
                onAbilityConnectDoneCount++;
            }
        }

        /**
         * OnAbilityDisconnectDone, AbilityMs notify caller ability the result of disconnect.
         *
         * @param element,.service ability's ElementName.
         * @param resultCode, ERR_OK on success, others on failure.
         */
        void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override
        {
            GTEST_LOG_(INFO) << "AbilityConnectCallback::OnAbilityDisconnectDone:resultCode = " << resultCode;
            if (resultCode == 0) {
                onAbilityConnectDoneCount--;
            }
        }

        static size_t onAbilityConnectDoneCount;
    };

    class AppEventSubscriber : public CommonEventSubscriber {
    public:
        explicit AppEventSubscriber(const CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp){};
        ~AppEventSubscriber() = default;
        virtual void OnReceiveEvent(const CommonEventData &data) override;
    };

    static std::shared_ptr<AppEventSubscriber> subscriber;
};
size_t AmsServiceAbilityTest::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
std::shared_ptr<AmsServiceAbilityTest::AppEventSubscriber> AmsServiceAbilityTest::subscriber = nullptr;

std::vector<std::string> AmsServiceAbilityTest::eventList = {
    "resp_com_ohos_amsst_service_app_a1",
    "resp_com_ohos_amsst_service_app_b2",
    "resp_com_ohos_amsst_service_app_b3",
    "resp_com_ohos_amsst_service_app_c4",
    "resp_com_ohos_amsst_service_app_d1",
    "resp_com_ohos_amsst_service_app_e2",
    "resp_com_ohos_amsst_service_app_f3",
    "resp_com_ohos_amsst_service_app_g1",
    "resp_com_ohos_amsst_service_app_h1",
};

std::map<std::string, int> AmsServiceAbilityTest::mapState = {
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

STtools::Event AmsServiceAbilityTest::event = STtools::Event();
void AmsServiceAbilityTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
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

void AmsServiceAbilityTest::SetUpTestCase(void)
{
    // Subscribe Event
    if (!SubscribeEvent()) {
        GTEST_LOG_(INFO) << "subscribeEvent error";
    }
}

void AmsServiceAbilityTest::TearDownTestCase(void)
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
}

void AmsServiceAbilityTest::SetUp(void)
{
    STAbilityUtil::InstallHaps(hapNameList);
    AbilityConnectCallback::onAbilityConnectDoneCount = 0;
    abilityMs = STAbilityUtil::GetAbilityManagerService();
    appMs = STAbilityUtil::GetAppMgrService();
}

bool AmsServiceAbilityTest::SubscribeEvent()
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

void AmsServiceAbilityTest::TearDown(void)
{
    STAbilityUtil::UninstallBundle(bundleNameList);
    STAbilityUtil::CleanMsg(event);
}

sptr<IAbilityManager> AmsServiceAbilityTest::abilityMs = nullptr;
sptr<IAppMgr> AmsServiceAbilityTest::appMs = nullptr;

void AmsServiceAbilityTest::ExpectPageAbilityCurrentState(
    const std::string &abilityName, const std::string &currentState) const
{
    std::vector<std::string> dumpInfo;
    if (abilityMs != nullptr) {
        abilityMs->DumpState(DUMP_STACK + " 1", dumpInfo);
        CheckAbilityStateByName(abilityName, dumpInfo, currentState);
    } else {
        HILOG_ERROR("ability manager service(abilityMs) is nullptr");
    }
}

void AmsServiceAbilityTest::ExpectServiceAbilityCurrentState(
    const std::string &abilityName, const std::string &currentState) const
{
    std::vector<std::string> dumpInfo;
    if (abilityMs != nullptr) {
        abilityMs->DumpState(DUMP_SERVICE, dumpInfo);
        CheckAbilityStateByName(abilityName, dumpInfo, currentState);
    } else {
        HILOG_ERROR("ability manager service(abilityMs) is nullptr");
    }
}

void AmsServiceAbilityTest::ExpectDataAbilityCurrentState(
    const std::string &abilityName, const std::string &currentState) const
{
    std::vector<std::string> dumpInfo;
    if (abilityMs != nullptr) {
        abilityMs->DumpState(DUMP_DATA, dumpInfo);
        CheckAbilityStateByName(abilityName, dumpInfo, currentState);
    } else {
        HILOG_ERROR("ability manager service(abilityMs) is nullptr");
    }
}

void AmsServiceAbilityTest::ExpectAbilityNumInStack(const std::string &abilityName, int abilityNum) const
{
    std::vector<std::string> dumpInfo;
    if (abilityMs != nullptr) {
        abilityMs->DumpState(DUMP_STACK + " 1", dumpInfo);
        std::vector<std::string> result;
        MTDumpUtil::GetInstance()->GetAll("AbilityName", dumpInfo, result);
        // only one record in stack
        EXPECT_EQ(abilityNum, std::count(result.begin(), result.end(), abilityName));
    } else {
        HILOG_ERROR("ability manager service(abilityMs) is nullptr");
    }
}

void AmsServiceAbilityTest::CheckAbilityStateByName(
    const std::string &abilityName, const std::vector<std::string> &info, const std::string &state) const
{
    std::vector<std::string> result;
    MTDumpUtil::GetInstance()->GetAll("AbilityName", info, result);
    auto pos = MTDumpUtil::GetInstance()->GetSpecific(abilityName, result, result.begin());
    // ability exist
    EXPECT_NE(pos, result.end());
    MTDumpUtil::GetInstance()->GetAll("State", info, result);
    ASSERT_TRUE(pos < result.end());
    // ability state
    EXPECT_EQ(Trim(*pos), state);
}

AbilityInfo AmsServiceAbilityTest::MakeAbilityInfo(
    const std::string &name, const std::string &appName, const std::string &bundleName) const
{
    AbilityInfo abilityInfo;
    abilityInfo.name = name;
    abilityInfo.applicationName = appName;
    abilityInfo.bundleName = bundleName;

    return abilityInfo;
}

/**
 * @tc.number    : AMS_Service_Ability_0100
 * @tc.name      : AMS kit test
 * @tc.desc      : start a service ability
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0100 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    // start ability
    MAP_STR_STR params;
    Want want = STAbilityUtil::STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop ability
    eCode = STAbilityUtil::StopServiceAbility(want);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0100 end";
}

/**
 * @tc.number    : AMS_Service_Ability_0200
 * @tc.name      : AMS kit test
 * @tc.desc      : start multiple times the same service ability in the same app
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0200 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    // start ability
    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);
    for (size_t i = 0; i < TIMES; i++) {
        ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
        EXPECT_EQ(ERR_OK, eCode);
        if (i == 0) {
            EXPECT_EQ(
                STAbilityUtil::WaitCompleted(
                    event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
                0);
        }
        EXPECT_EQ(STAbilityUtil::WaitCompleted(
                      event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
            0);
        usleep(WAIT_TIME);
    }

    // stop ability
    ErrCode eCode1 = STAbilityUtil::StopServiceAbility(want);
    EXPECT_EQ(ERR_OK, eCode1);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0200 end";
}

/**
 * @tc.number    : AMS_Service_Ability_0300
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability by another page ability in the same app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0300 start";

    std::string bundleName2 = BUNDLE_NAME_BASE + "B";
    std::string abilityName2 = ABILITY_NAME_BASE + "B2";
    std::string bundleName3 = BUNDLE_NAME_BASE + "B";
    std::string abilityName3 = ABILITY_NAME_BASE + "B3";

    MAP_STR_STR params;
    params["targetBundle"] = bundleName3;
    params["targetAbility"] = abilityName3;

    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName2, bundleName2, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_ACTIVE, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // ability "B2" state ACTIVE
    ExpectPageAbilityCurrentState(abilityName2, "ACTIVE");

    // ability "B3" state ACTIVE
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_B2, AbilityState_Test::USER_DEFINE, OPERATION_START_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop ability B2
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_B2, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_INACTIVE, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop ability B3
    ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_B3, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0300 end";
}

/**
 * @tc.number    : AMS_Service_Ability_0400
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then stop service ability in the same app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0400 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop ability
    eCode = STAbilityUtil::StopServiceAbility(want);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0400 end";
}

/**
 * @tc.number    : AMS_Service_Ability_0500
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in the same app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0500 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    sptr<AbilityConnectCallback> stub(new (std::nothrow) AbilityConnectCallback());
    sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
    eCode = STAbilityUtil::ConnectAbility(want, connCallback, stub->AsObject());
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    EXPECT_GT(AbilityConnectCallback::onAbilityConnectDoneCount, (size_t)0);

    eCode = STAbilityUtil::DisconnectAbility(connCallback);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);

    // stop ability
    eCode = STAbilityUtil::StopServiceAbility(want);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0500 end";
}

/**
 * @tc.number    : AMS_Service_Ability_0600
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in the same app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_0600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0600 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    sptr<AbilityConnectCallback> stub(new (std::nothrow) AbilityConnectCallback());
    sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
    eCode = STAbilityUtil::ConnectAbility(want, connCallback, stub->AsObject());
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);

    eCode = STAbilityUtil::DisconnectAbility(connCallback);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);

    usleep(WAIT_TIME);
    // stop ability
    eCode = STAbilityUtil::StopServiceAbility(want);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0600 end";
}

/**
 * @tc.number    : AMS_Service_Ability_0700
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in the same app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_0700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0700 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    sptr<AbilityConnectCallback> stub(new (std::nothrow) AbilityConnectCallback());
    sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
    eCode = STAbilityUtil::ConnectAbility(want, connCallback, stub->AsObject());
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // check number of connections
    EXPECT_EQ(AbilityConnectCallback::onAbilityConnectDoneCount, (size_t)1);

    eCode = STAbilityUtil::DisconnectAbility(connCallback);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // check number of connections
    EXPECT_EQ(AbilityConnectCallback::onAbilityConnectDoneCount, (size_t)0);

    // stop ability
    eCode = STAbilityUtil::StopServiceAbility(want);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0700 end";
}

/**
 * @tc.number    : AMS_Service_Ability_0800
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in the same app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_0800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0800 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    sptr<AbilityConnectCallback> stub(new (std::nothrow) AbilityConnectCallback());
    sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
    eCode = STAbilityUtil::ConnectAbility(want, connCallback, stub->AsObject());
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);

    eCode = STAbilityUtil::ConnectAbility(want, connCallback, stub->AsObject());
    EXPECT_EQ(ERR_OK, eCode);
    usleep(WAIT_TIME);
    // check number of connections
    EXPECT_EQ(AbilityConnectCallback::onAbilityConnectDoneCount, (size_t)1);

    eCode = STAbilityUtil::DisconnectAbility(connCallback);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // check number of connections
    EXPECT_EQ(AbilityConnectCallback::onAbilityConnectDoneCount, (size_t)0);

    // stop ability
    eCode = STAbilityUtil::StopServiceAbility(want);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0800 end";
}

/**
 * @tc.number    : AMS_Service_Ability_0900
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in the same app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_0900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0900 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);

    // connect service ability
    sptr<AbilityConnectCallback> stub(new (std::nothrow) AbilityConnectCallback());
    sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
    ErrCode eCode = STAbilityUtil::ConnectAbility(want, connCallback, stub->AsObject());
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // check number of connections
    EXPECT_EQ(AbilityConnectCallback::onAbilityConnectDoneCount, (size_t)1);

    AppProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs);
    EXPECT_TRUE(pInfo.pid_ > 0);

    eCode = STAbilityUtil::DisconnectAbility(connCallback);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // check number of connections
    EXPECT_EQ(AbilityConnectCallback::onAbilityConnectDoneCount, (size_t)0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_0900 end";
}

/**
 * @tc.number    : AMS_Service_Ability_1000
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in the same app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_1000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1000 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);
    sptr<AbilityConnectCallback> stub(new (std::nothrow) AbilityConnectCallback());
    sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));

    for (size_t i = 0; i < TIMES; i++) {
        ErrCode eCode = STAbilityUtil::ConnectAbility(want, connCallback, stub->AsObject());
        EXPECT_EQ(ERR_OK, eCode);
        if (i == 0) {
            EXPECT_EQ(
                STAbilityUtil::WaitCompleted(
                    event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
                0);
            EXPECT_EQ(
                STAbilityUtil::WaitCompleted(
                    event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
                0);
        }
    }
    usleep(WAIT_TIME);
    EXPECT_EQ(AbilityConnectCallback::onAbilityConnectDoneCount, (size_t)1);

    ErrCode eCode1 = STAbilityUtil::DisconnectAbility(connCallback);
    EXPECT_EQ(ERR_OK, eCode1);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // check number of connections
    EXPECT_EQ(AbilityConnectCallback::onAbilityConnectDoneCount, (size_t)0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1000 end";
}

/**
 * @tc.number    : AMS_Service_Ability_1100
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in the same app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_1100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1100 start";

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName2 = ABILITY_NAME_BASE + "B2";
    std::string abilityName3 = ABILITY_NAME_BASE + "B3";

    MAP_STR_STR params;
    params["targetBundleConn"] = bundleName;
    params["targetAbilityConn"] = abilityName3;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName2, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_ACTIVE, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // ability "B2" state ACTIVE
    ExpectPageAbilityCurrentState(abilityName2, "ACTIVE");

    // ability2 connect ability3
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_B2, AbilityState_Test::USER_DEFINE, OPERATION_CONNECT_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);

    // stop ability B3
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_B2, 0, "DisConnectOtherAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop ability B2
    ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_B2, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_INACTIVE, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1100 end";
}

/**
 * @tc.number    : AMS_Service_Ability_1200
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in the same app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_1200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1200 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);

    // connect service ability
    sptr<AbilityConnectCallback> stub(new (std::nothrow) AbilityConnectCallback());
    sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
    ErrCode eCode = STAbilityUtil::ConnectAbility(want, connCallback, stub->AsObject());
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);

    usleep(WAIT_TIME);
    eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);

    usleep(WAIT_TIME);
    eCode = STAbilityUtil::DisconnectAbility(connCallback);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);

    usleep(WAIT_TIME);
    // stop ability
    eCode = STAbilityUtil::StopServiceAbility(want);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1200 end";
}

/**
 * @tc.number    : AMS_Service_Ability_1300
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in the same app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_1300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1300 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);

    // connect service ability
    sptr<AbilityConnectCallback> stub(new (std::nothrow) AbilityConnectCallback());
    sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
    ErrCode eCode = STAbilityUtil::ConnectAbility(want, connCallback, stub->AsObject());
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);

    usleep(WAIT_TIME);
    eCode = STAbilityUtil::DisconnectAbility(connCallback);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1300 end";
}

/**
 * @tc.number    : AMS_Service_Ability_1400
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in the same app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_1400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1400 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);

    // connect service ability
    sptr<AbilityConnectCallback> stub(new (std::nothrow) AbilityConnectCallback());
    sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
    ErrCode eCode = STAbilityUtil::ConnectAbility(want, connCallback, stub->AsObject());
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);

    eCode = STAbilityUtil::StopServiceAbility(want);
    EXPECT_NE(ERR_OK, eCode);

    usleep(WAIT_TIME);
    eCode = STAbilityUtil::DisconnectAbility(connCallback);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1400 end";
}

/**
 * @tc.number    : AMS_Service_Ability_1500
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in another app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_1500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1500 start";

    std::string bundleName = BUNDLE_NAME_BASE + "B";
    std::string abilityName2 = ABILITY_NAME_BASE + "B2";
    std::string abilityName3 = ABILITY_NAME_BASE + "B3";

    MAP_STR_STR params;
    params["targetBundleConn"] = bundleName;
    params["targetAbilityConn"] = abilityName3;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName2, bundleName, params);
    // start ability2
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_ACTIVE, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // ability "B2" state ACTIVE
    ExpectPageAbilityCurrentState(abilityName2, "ACTIVE");

    // ability2 connect ability3
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_B2, AbilityState_Test::USER_DEFINE, OPERATION_CONNECT_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop ability B3
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_B2, 0, "DisConnectOtherAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop ability2
    ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_B2, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_INACTIVE, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1500 end";
}

void AmsServiceAbilityTest::AmsServiceAbilityTest16001() const
{
    std::string bundleName1 = BUNDLE_NAME_BASE + "A";
    std::string bundleName2 = BUNDLE_NAME_BASE + "B";
    std::string abilityName1 = ABILITY_NAME_BASE + "A1";
    std::string abilityName2 = ABILITY_NAME_BASE + "B2";
    std::string abilityName3 = ABILITY_NAME_BASE + "B3";

    MAP_STR_STR params;
    params["targetBundle"] = bundleName2;
    params["targetAbility"] = abilityName3;
    params["nextTargetBundle"] = bundleName1;
    params["nextTargetAbility"] = abilityName1;
    params["nextTargetBundleConn"] = bundleName2;
    params["nextTargetAbilityConn"] = abilityName3;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName2, bundleName2, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    // start page ability2
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_ACTIVE, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // ability "B2" state ACTIVE
    ExpectPageAbilityCurrentState(abilityName2, "ACTIVE");

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_B2, AbilityState_Test::USER_DEFINE, OPERATION_START_OTHER_ABILITY);
    // page ability2 start service ability3
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
}

void AmsServiceAbilityTest::AmsServiceAbilityTest16002() const
{
    std::string bundleName1 = BUNDLE_NAME_BASE + "A";
    std::string bundleName2 = BUNDLE_NAME_BASE + "B";
    std::string abilityName1 = ABILITY_NAME_BASE + "A1";
    std::string abilityName2 = ABILITY_NAME_BASE + "B2";
    std::string abilityName3 = ABILITY_NAME_BASE + "B3";

    MAP_STR_STR params;
    params["targetBundle"] = bundleName2;
    params["targetAbility"] = abilityName3;
    params["nextTargetBundle"] = bundleName1;
    params["nextTargetAbility"] = abilityName1;
    params["nextTargetBundleConn"] = bundleName2;
    params["nextTargetAbilityConn"] = abilityName3;
    // service ability B3 start service ability A1
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_B3, AbilityState_Test::USER_DEFINE, OPERATION_START_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // service ability A1 connect service ability B3
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_A1, AbilityState_Test::USER_DEFINE, OPERATION_CONNECT_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    Want want3 = STAbilityUtil::MakeWant(DEVICE_ID, abilityName3, bundleName2, params);
    ErrCode eCode = STAbilityUtil::StopServiceAbility(want3);
    EXPECT_NE(ERR_OK, eCode);

    // service ability1 disconnect service ability3
    STAbilityUtil::PublishEvent(
        REQ_EVENT_NAME_APP_A1, AbilityState_Test::USER_DEFINE, OPERATION_DISCONNECT_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop abiity B3
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_B3, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop abiity A1
    ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_A1, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop abiity B2
    ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_B2, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_INACTIVE, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
}

/**
 * @tc.number    : AMS_Service_Ability_1600
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in another app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_1600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1600 start";

    AmsServiceAbilityTest16001();
    AmsServiceAbilityTest16002();

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1600 end";
}

void AmsServiceAbilityTest::AmsServiceAbilityTest17001() const
{
    std::string bundleName1 = BUNDLE_NAME_BASE + "A";
    std::string bundleName2 = BUNDLE_NAME_BASE + "B";
    std::string bundleName3 = BUNDLE_NAME_BASE + "C";
    std::string abilityName1 = ABILITY_NAME_BASE + "A1";
    std::string abilityName2 = ABILITY_NAME_BASE + "B2";
    std::string abilityName3 = ABILITY_NAME_BASE + "B3";
    std::string abilityName4 = ABILITY_NAME_BASE + "C4";

    MAP_STR_STR params;
    params["targetBundle"] = bundleName1 + "," + bundleName2 + "," + bundleName3;
    params["targetAbility"] = abilityName1 + "," + abilityName3 + "," + abilityName4;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName2, bundleName2, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    // start page ability2
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_ACTIVE, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // ability "B2" state ACTIVE
    ExpectPageAbilityCurrentState(abilityName2, "ACTIVE");

    // start service ability3
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_B2, AbilityState_Test::USER_DEFINE, OPERATION_START_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
}

void AmsServiceAbilityTest::AmsServiceAbilityTest17002() const
{
    // start service ability4
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // start service ability1
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop abiity B3
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_B3, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop abiity A1
    ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_A1, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop abiity C4
    ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_C4, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop page ability2
    ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_B2, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ONINACTIVE, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
}

/**
 * @tc.number    : AMS_Service_Ability_1700
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in another app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_1700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1700 start";

    AmsServiceAbilityTest17001();
    AmsServiceAbilityTest17002();

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1700 end";
}

void AmsServiceAbilityTest::AmsServiceAbilityTest18001() const
{
    std::string bundleName1 = BUNDLE_NAME_BASE + "A";
    std::string bundleName2 = BUNDLE_NAME_BASE + "B";
    std::string bundleName3 = BUNDLE_NAME_BASE + "C";
    std::string abilityName1 = ABILITY_NAME_BASE + "A1";
    std::string abilityName2 = ABILITY_NAME_BASE + "B2";
    std::string abilityName3 = ABILITY_NAME_BASE + "B3";
    std::string abilityName4 = ABILITY_NAME_BASE + "C4";

    MAP_STR_STR params;
    params["targetBundleConn"] = bundleName1 + "," + bundleName2 + "," + bundleName3;
    params["targetAbilityConn"] = abilityName1 + "," + abilityName3 + "," + abilityName4;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName2, bundleName2, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    // start page ability2
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_ACTIVE, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // ability "B2" state ACTIVE
    ExpectPageAbilityCurrentState(abilityName2, "ACTIVE");

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_B2, AbilityState_Test::USER_DEFINE, OPERATION_CONNECT_OTHER_ABILITY);
    // connect service ability3
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // connect service ability4
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
}

void AmsServiceAbilityTest::AmsServiceAbilityTest18002() const
{
    // connect service ability1
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop abiity A1 B3 C4
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_B2, 0, "DisConnectOtherAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop page ability2
    ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_B2, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_INACTIVE, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
}

/**
 * @tc.number    : AMS_Service_Ability_1800
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in another app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_1800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1800 start";

    AmsServiceAbilityTest18001();
    AmsServiceAbilityTest18002();

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1800 end";
}

/**
 * @tc.number    : AMS_Service_Ability_1900
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in another app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_1900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1900 start";

    std::string bundleName1 = BUNDLE_NAME_BASE + "D";
    std::string bundleName2 = BUNDLE_NAME_BASE + "E";
    std::string bundleName3 = BUNDLE_NAME_BASE + "F";
    std::string abilityName1 = ABILITY_NAME_BASE + "D1";
    std::string abilityName2 = ABILITY_NAME_BASE + "E2";
    std::string abilityName3 = ABILITY_NAME_BASE + "F3";

    MAP_STR_STR params;
    params["targetBundleConn"] = bundleName2;
    params["targetAbilityConn"] = abilityName2;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName1, bundleName1, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    // start page ability1
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_ACTIVE, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // ability "D1" state ACTIVE
    ExpectPageAbilityCurrentState(abilityName1, "ACTIVE");
    usleep(WAIT_TIME);

    // page ability1 connect service ability2
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_D1, AbilityState_Test::USER_DEFINE, OPERATION_CONNECT_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // page ability1 access data ability3<by ability2 api>
    STAbilityUtil::PublishEvent(
        REQ_EVENT_NAME_APP_D1, AbilityState_Test::USER_DEFINE, OPERATION_GET_DATA_BY_DATA_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event, OPERATION_FROM_DATA_ABILITY, 1, DELAY_TIME), 0);

    // stop ability E2
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_D1, 0, "DisConnectOtherAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop ability D1
    ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_D1, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_INACTIVE, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_1900 end";
}

/**
 * @tc.number    : AMS_Service_Ability_2000
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in another app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_2000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_2000 start";

    std::string bundleName1 = BUNDLE_NAME_BASE + "D";
    std::string bundleName2 = BUNDLE_NAME_BASE + "E";
    std::string bundleName3 = BUNDLE_NAME_BASE + "F";
    std::string abilityName1 = ABILITY_NAME_BASE + "D1";
    std::string abilityName2 = ABILITY_NAME_BASE + "E2";
    std::string abilityName3 = ABILITY_NAME_BASE + "F3";

    MAP_STR_STR params;
    Want want = STAbilityUtil::MakeWant(DEVICE_ID, abilityName1, bundleName1, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    // start page ability1
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_ACTIVE, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // ability "D1" state ACTIVE
    ExpectPageAbilityCurrentState(abilityName1, "ACTIVE");

    // page ability1 access data ability3
    STAbilityUtil::PublishEvent(
        REQ_EVENT_NAME_APP_D1, AbilityState_Test::USER_DEFINE, OPERATION_GET_DATA_BY_DATA_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event, OPERATION_FROM_DATA_ABILITY, 1, DELAY_TIME), 0);
    usleep(WAIT_TIME);

    // stop ability D1
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_APP_D1, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_INACTIVE, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_2000 end";
}

/**
 * @tc.number    : AMS_Service_Ability_2100
 * @tc.name      : AMS kit test
 * @tc.desc      : In two different haps, ServiceAbility belongs to the same BundleName, and ServiceAbility belongs
 *                 to the same process id.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_2100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_2100 start";

    std::string bundleName1 = BUNDLE_NAME_BASE + "G";
    std::string bundleName2 = BUNDLE_NAME_BASE + "G";
    std::string abilityName1 = ABILITY_NAME_BASE + "G1";
    std::string abilityName2 = ABILITY_NAME_BASE + "H1";

    // start ability G1
    MAP_STR_STR params1;
    Want want1 = STAbilityUtil::MakeWant(DEVICE_ID, abilityName1, bundleName1, params1);
    ErrCode eCode = STAbilityUtil::StartAbility(want1, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    AppProcessInfo pInfo1 = STAbilityUtil::GetAppProcessInfoByName(bundleName1, appMs);
    EXPECT_TRUE(pInfo1.pid_ > 0);

    // start ability H1
    MAP_STR_STR params2;
    params2["targetBundleConn"] = bundleName1;
    params2["targetAbilityConn"] = abilityName1;
    Want want2 = STAbilityUtil::MakeWant(DEVICE_ID, abilityName2, bundleName2, params2);
    eCode = STAbilityUtil::StartAbility(want2, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs);
    EXPECT_TRUE(pInfo2.pid_ > 0);
    EXPECT_TRUE(pInfo1.pid_ == pInfo2.pid_);

    // stop ability G1
    eCode = STAbilityUtil::StopServiceAbility(want1);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop ability H1
    eCode = STAbilityUtil::StopServiceAbility(want2);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_2100 end";
}

/**
 * @tc.number    : AMS_Service_Ability_2200
 * @tc.name      : AMS kit test
 * @tc.desc      : In two different haps, ServiceAbility belongs to the same BundleName, and ServiceAbility starts
 *                 each other.
 *
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_2200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_2200 start";

    std::string bundleName1 = BUNDLE_NAME_BASE + "G";
    std::string bundleName2 = BUNDLE_NAME_BASE + "G";
    std::string abilityName1 = ABILITY_NAME_BASE + "G1";
    std::string abilityName2 = ABILITY_NAME_BASE + "H1";

    MAP_STR_STR params1;
    params1["targetBundleConn"] = bundleName2;
    params1["targetAbilityConn"] = abilityName2;
    Want want1 = STAbilityUtil::MakeWant(DEVICE_ID, abilityName1, bundleName1, params1);
    ErrCode eCode = STAbilityUtil::StartAbility(want1, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    // start page ability1
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    AppProcessInfo pInfo1 = STAbilityUtil::GetAppProcessInfoByName(bundleName1, appMs);
    EXPECT_TRUE(pInfo1.pid_ > 0);

    // service ability G1 connect service ability H1
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_G1, AbilityState_Test::USER_DEFINE, OPERATION_CONNECT_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // start ability H1
    MAP_STR_STR params2;
    params2["targetBundleConn"] = bundleName1;
    params2["targetAbilityConn"] = abilityName1;
    Want want2 = STAbilityUtil::MakeWant(DEVICE_ID, abilityName2, bundleName2, params2);
    eCode = STAbilityUtil::StartAbility(want2, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        -1);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    AppProcessInfo pInfo2 = STAbilityUtil::GetAppProcessInfoByName(bundleName2, appMs);
    EXPECT_TRUE(pInfo2.pid_ > 0);
    EXPECT_TRUE(pInfo1.pid_ == pInfo2.pid_);

    // service ability H1 connect service ability G1
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_H1, AbilityState_Test::USER_DEFINE, OPERATION_CONNECT_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        -1);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // disconnect G1 from H1
    STAbilityUtil::PublishEvent(
        REQ_EVENT_NAME_APP_H1, AbilityState_Test::USER_DEFINE, OPERATION_DISCONNECT_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // stop ability G1
    eCode = STAbilityUtil::StopServiceAbility(want1);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // disconnect H1 from G1
    STAbilityUtil::PublishEvent(
        REQ_EVENT_NAME_APP_G1, AbilityState_Test::USER_DEFINE, OPERATION_DISCONNECT_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // stop ability H1
    eCode = STAbilityUtil::StopServiceAbility(want2);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_2200 end";
}

void AmsServiceAbilityTest::AmsServiceAbilityTest23001() const
{
    std::string bundleName1 = BUNDLE_NAME_BASE + "A";
    std::string bundleName2 = BUNDLE_NAME_BASE + "B";
    std::string bundleName3 = BUNDLE_NAME_BASE + "E";
    std::string abilityName1 = ABILITY_NAME_BASE + "A1";
    std::string abilityName2 = ABILITY_NAME_BASE + "B2";
    std::string abilityName3 = ABILITY_NAME_BASE + "B3";
    std::string abilityName4 = ABILITY_NAME_BASE + "E2";

    ////////////////////////////////////////////////////////////////////////////////////
    // start page ability A1
    MAP_STR_STR params1;
    params1["targetBundleConn"] = bundleName2;
    params1["targetAbilityConn"] = abilityName3;

    Want want1 = STAbilityUtil::MakeWant(DEVICE_ID, abilityName1, bundleName1, params1);
    ErrCode eCode = STAbilityUtil::StartAbility(want1, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // service ability A1 connect service ability B3
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_A1, AbilityState_Test::USER_DEFINE, OPERATION_CONNECT_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_CONNECT, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // check B3 connect number
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event, "OnAbilityConnectDone", 1, DELAY_TIME), 0);

    ////////////////////////////////////////////////////////////////////////////////////
    // start page ability B2
    MAP_STR_STR params2;
    params2["targetBundleConn"] = bundleName2;
    params2["targetAbilityConn"] = abilityName3;

    Want want2 = STAbilityUtil::MakeWant(DEVICE_ID, abilityName2, bundleName2, params2);
    eCode = STAbilityUtil::StartAbility(want2, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_ACTIVE, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // ability "B2" state ACTIVE
    ExpectPageAbilityCurrentState(abilityName2, "ACTIVE");

    // service ability B2 connect service ability B3
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_B2, AbilityState_Test::USER_DEFINE, OPERATION_CONNECT_OTHER_ABILITY);
    usleep(WAIT_TIME);
    // check B3 connect number
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event, "OnAbilityConnectDone", 1, DELAY_TIME), 0);
}

void AmsServiceAbilityTest::AmsServiceAbilityTest23002() const
{
    std::string bundleName1 = BUNDLE_NAME_BASE + "A";
    std::string bundleName2 = BUNDLE_NAME_BASE + "B";
    std::string bundleName3 = BUNDLE_NAME_BASE + "E";
    std::string abilityName1 = ABILITY_NAME_BASE + "A1";
    std::string abilityName2 = ABILITY_NAME_BASE + "B2";
    std::string abilityName3 = ABILITY_NAME_BASE + "B3";
    std::string abilityName4 = ABILITY_NAME_BASE + "E2";

    ////////////////////////////////////////////////////////////////////////////////////
    // start page ability E2
    MAP_STR_STR params3;
    params3["targetBundleConn"] = bundleName2;
    params3["targetAbilityConn"] = abilityName3;

    Want want3 = STAbilityUtil::MakeWant(DEVICE_ID, abilityName4, bundleName3, params3);
    ErrCode eCode = STAbilityUtil::StartAbility(want3, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, PAGE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);
    usleep(WAIT_TIME);

    // service ability E2 connect service ability B3
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_APP_E2, AbilityState_Test::USER_DEFINE, OPERATION_CONNECT_OTHER_ABILITY);
    usleep(WAIT_TIME);

    // check B3 connect number
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event, "OnAbilityConnectDone", 1, DELAY_TIME), 0);

    ////////////////////////////////////////////////////////////////////////////////////
    // Disconnect A1
    STAbilityUtil::PublishEvent(
        REQ_EVENT_NAME_APP_A1, AbilityState_Test::USER_DEFINE, OPERATION_DISCONNECT_OTHER_ABILITY);
    usleep(WAIT_TIME);
    // check B3 connect number
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event, "OnAbilityDisconnectDone", 0, DELAY_TIME), 0);

    // Disconnect B2
    STAbilityUtil::PublishEvent(
        REQ_EVENT_NAME_APP_B2, AbilityState_Test::USER_DEFINE, OPERATION_DISCONNECT_OTHER_ABILITY);
    usleep(WAIT_TIME);
    // check B3 connect number
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event, "OnAbilityDisconnectDone", 0, DELAY_TIME), 0);

    // Disconnect E2
    STAbilityUtil::PublishEvent(
        REQ_EVENT_NAME_APP_E2, AbilityState_Test::USER_DEFINE, OPERATION_DISCONNECT_OTHER_ABILITY);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_DISCONNECT, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    usleep(WAIT_TIME);
    // check B3 connect number
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event, "OnAbilityDisconnectDone", 0, DELAY_TIME), 0);
}

/**
 * @tc.number    : AMS_Service_Ability_2300
 * @tc.name      : AMS kit test
 * @tc.desc      : start service ability and then connect service ability in another app.
 */
HWTEST_F(AmsServiceAbilityTest, AMS_Service_Ability_2300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_2300 start";

    AmsServiceAbilityTest23001();
    AmsServiceAbilityTest23002();

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AMS_Service_Ability_2300 end";
}

/**
 * @tc.number    : AppSpawn_TEST_0100
 * @tc.name      : Check appspawn process information
 * @tc.desc      : The system rc is started, and based on root privileges, the AppSpawn process is created, and
 related
 * resources are loaded.
 */
HWTEST_F(AmsServiceAbilityTest, AppSpawn_TEST_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AppSpawn_TEST_0100 start";

    std::string cmd = "ps -ef |grep appspawn |grep -v grep | awk '{print $1}'";
    std::string result;
    ExecuteSystemForResult(cmd, result);
    EXPECT_EQ(Trim(result).compare("root"), 0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AppSpawn_TEST_0100 end";
}

/**
 * @tc.number    : AppSpawn_TEST_0200
 * @tc.name      : The process starts message monitoring.
 * @tc.desc      : Monitor the message of creating an application process initiated by AMS.
 */
HWTEST_F(AmsServiceAbilityTest, AppSpawn_TEST_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AppSpawn_TEST_0200 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    MAP_STR_STR params;
    Want want = STAbilityUtil::STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);

    // check app process information
    std::string cmd = "ps -ef |grep com.ohos.amsst.service.appA |grep -v grep | awk '{print $2}'";
    std::string result;
    ExecuteSystemForResult(cmd, result);
    EXPECT_FALSE(Trim(result).empty());
    usleep(WAIT_TIME);

    // stop ability
    eCode = STAbilityUtil::StopServiceAbility(want);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AppSpawn_TEST_0200 end";
}

/**
 * @tc.number    : AppSpawn_0300
 * @tc.name      : Destroy the process through AppSpawn.
 * @tc.desc      : By monitoring all process information, destroy the zombie process.
 */
HWTEST_F(AmsServiceAbilityTest, AppSpawn_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AppSpawn_0300 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    // start ability A1
    MAP_STR_STR params;
    Want want = STAbilityUtil::STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        0);

    // stop ability
    eCode = STAbilityUtil::StopServiceAbility(want);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_BACKGROUND, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_STOP, AbilityLifecycleExecutor::LifecycleState::INITIAL, DELAY_TIME),
        0);

    // check app process information
    std::string cmd = "ps -ef |grep com.ohos.amsst.service.appA |grep -v grep | awk '{print $2}'";
    std::string result;
    ExecuteSystemForResult(cmd, result);
    EXPECT_TRUE(Trim(result).empty());

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AppSpawn_0300 end";
}

/**
 * @tc.number    : AppSpawn_0400
 * @tc.name      : Recycle zombie processes.
 * @tc.desc      : 1.The application process is suspended.
 *                 2.The appSpawn listener process is suspended and the process is
 * recycled.
 */
HWTEST_F(AmsServiceAbilityTest, AppSpawn_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AppSpawn_0400 start";

    std::string bundleName = BUNDLE_NAME_BASE + "A";
    std::string abilityName = ABILITY_NAME_BASE + "A1";

    // start ability A1. make zombie process
    MAP_STR_STR params;
    params["zombie"] = "zombie";
    Want want = STAbilityUtil::STAbilityUtil::MakeWant(DEVICE_ID, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_START, AbilityLifecycleExecutor::LifecycleState::INACTIVE, DELAY_TIME),
        0);
    EXPECT_EQ(STAbilityUtil::WaitCompleted(
                  event, SERVICE_STATE_ON_COMMAND, AbilityLifecycleExecutor::LifecycleState::ACTIVE, DELAY_TIME),
        -1);

    usleep(WAIT_TIME * 5);
    // check app process information
    std::string cmd = "ps -ef |grep com.ohos.amsst.service.appA |grep -v grep | awk '{print $2}'";
    std::string result;
    ExecuteSystemForResult(cmd, result);
    EXPECT_TRUE(Trim(result).empty());

    // stop ability
    eCode = STAbilityUtil::StopServiceAbility(want);
    EXPECT_NE(ERR_OK, eCode);

    GTEST_LOG_(INFO) << "AmsServiceAbilityTest AppSpawn_0400 end";
}
}  // namespace