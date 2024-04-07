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

#include <gtest/gtest.h>

#include "ability.h"
#include "ability_local_record.h"
#include "ability_handler.h"
#include "ability_info.h"
#include "ability_start_setting.h"
#include "context_deal.h"
#include "mock_page_ability.h"

namespace OHOS {
namespace AppExecFwk {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using OHOS::Parcel;

class AbilityBaseTest : public testing::Test {
public:
    AbilityBaseTest() : ability_(nullptr)
    {}
    ~AbilityBaseTest()
    {}
    std::shared_ptr<Ability> ability_;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AbilityBaseTest::SetUpTestCase(void)
{}

void AbilityBaseTest::TearDownTestCase(void)
{}

void AbilityBaseTest::SetUp(void)
{
    ability_ = std::make_shared<Ability>();
}

void AbilityBaseTest::TearDown(void)
{}

/**
 * @tc.number: AaFwk_Ability_Name_0100
 * @tc.name: GetAbilityName
 * @tc.desc: Verify that the return value of getabilityname is correct.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_Name_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_Name_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = "ability";
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    sptr<IRemoteObject> token = nullptr;

    ability_->Init(abilityInfo, application, handler, token);
    EXPECT_STREQ(abilityInfo->name.c_str(), ability_->GetAbilityName().c_str());

    GTEST_LOG_(INFO) << "AaFwk_Ability_Name_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_GetLifecycle_0100
 * @tc.name: GetLifecycle
 * @tc.desc: Verify that the return value of getlifecycle is not empty.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_GetLifecycle_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_GetLifecycle_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    sptr<IRemoteObject> token = nullptr;

    ability_->Init(abilityInfo, application, handler, token);
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();

    EXPECT_NE(lifeCycle, nullptr);

    GTEST_LOG_(INFO) << "AaFwk_Ability_GetLifecycle_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_GetState_0100
 * @tc.name: GetState
 * @tc.desc: Verify that the return value of getstate is equal to active.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_GetState_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_GetState_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    sptr<IRemoteObject> token = nullptr;

    ability_->Init(abilityInfo, application, handler, token);

    ability_->OnActive();
    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::ACTIVE, state);

    GTEST_LOG_(INFO) << "AaFwk_Ability_GetState_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_GetState_0200
 * @tc.name: GetState
 * @tc.desc: Getstate exception test.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_GetState_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_GetState_0200 start";

    ability_->OnActive();
    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::UNINITIALIZED, state);

    GTEST_LOG_(INFO) << "AaFwk_Ability_GetState_0200 end";
}

/**
 * @tc.number: AaFwk_Ability_Dump_0100
 * @tc.name: Dump
 * @tc.desc: Test dump normal flow.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_Dump_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_Dump_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    sptr<IRemoteObject> token = nullptr;

    ability_->Init(abilityInfo, application, handler, token);

    std::string extra = "";
    ability_->Dump(extra);

    GTEST_LOG_(INFO) << "AaFwk_Ability_Dump_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_OnNewWant_0100
 * @tc.name: OnNewWant
 * @tc.desc: Test whether onnewwant can be called normally.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnNewWant_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnNewWant_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    sptr<IRemoteObject> token = nullptr;

    ability_->Init(abilityInfo, application, handler, token);

    Want want;
    ability_->OnNewWant(want);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnNewWant_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_OnRestoreAbilityState_0100
 * @tc.name: OnRestoreAbilityState
 * @tc.desc: Test whether onnewwant can be called normally.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnRestoreAbilityState_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnRestoreAbilityState_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    sptr<IRemoteObject> token = nullptr;

    ability_->Init(abilityInfo, application, handler, token);

    PacMap inState;
    ability_->OnRestoreAbilityState(inState);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnRestoreAbilityState_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_GetAbilityName_0100
 * @tc.name: GetAbilityName
 * @tc.desc: Verify that the getabilityname return value is correct.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_GetAbilityName_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_GetAbilityName_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    sptr<IRemoteObject> token = nullptr;

    std::string name = "LOL";
    abilityInfo->name = name;
    ability_->Init(abilityInfo, application, handler, token);

    EXPECT_STREQ(ability_->GetAbilityName().c_str(), name.c_str());

    GTEST_LOG_(INFO) << "AaFwk_Ability_GetAbilityName_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_GetApplication_0100
 * @tc.name: GetApplication
 * @tc.desc: Verify that the getapplication return value is correct.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_GetApplication_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_GetApplication_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    sptr<IRemoteObject> token = nullptr;

    ability_->Init(abilityInfo, application, handler, token);
    std::shared_ptr<OHOSApplication> applicationRet = ability_->GetApplication();
    EXPECT_EQ(application, applicationRet);

    GTEST_LOG_(INFO) << "AaFwk_Ability_GetApplication_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_GetApplication_0200
 * @tc.name: GetApplication
 * @tc.desc: Test getapplication exception status.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_GetApplication_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_GetApplication_0200 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    sptr<IRemoteObject> token = nullptr;

    ability_->Init(abilityInfo, nullptr, handler, token);
    std::shared_ptr<OHOSApplication> application = ability_->GetApplication();
    EXPECT_EQ(application, nullptr);

    GTEST_LOG_(INFO) << "AaFwk_Ability_GetApplication_0200 end";
}

/**
 * @tc.number: AaFwk_Ability_OnSaveAbilityState_0100
 * @tc.name: OnSaveAbilityState
 * @tc.desc: Test whether onsaveabilitystate is called normally.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnSaveAbilityState_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnSaveAbilityState_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    std::shared_ptr<OHOSApplication> application = std::make_shared<OHOSApplication>();
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);
    sptr<IRemoteObject> token = nullptr;

    ability_->Init(abilityInfo, application, handler, token);

    PacMap outState;
    ability_->OnSaveAbilityState(outState);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnSaveAbilityState_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_SetWant_GetWant_0100
 * @tc.name: OnSaveAbilityState
 * @tc.desc: Verify that setwant creates the object normally,
 *           and judge whether the return value of getwant is correct.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_SetWant_GetWant_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_SetWant_GetWant_0100 start";

    std::string abilityName = "Ability";
    std::string bundleName = "Bundle";
    AAFwk::Want want;
    want.SetElementName(bundleName, abilityName);
    ability_->SetWant(want);

    EXPECT_STREQ(ability_->GetWant()->GetElement().GetBundleName().c_str(), bundleName.c_str());
    EXPECT_STREQ(ability_->GetWant()->GetElement().GetAbilityName().c_str(), abilityName.c_str());
    GTEST_LOG_(INFO) << "AaFwk_Ability_SetWant_GetWant_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_SetResult_0100
 * @tc.name: SetResult
 * @tc.desc: Test whether setresult is called normally.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_SetResult_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_SetResult_0100 start";

    int resultCode = 0;
    Want want;
    std::string action = "Action";
    want.SetAction(action);

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    AbilityType type = AbilityType::PAGE;
    abilityInfo->type = type;
    std::shared_ptr<OHOSApplication> application = nullptr;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    sptr<IRemoteObject> token = nullptr;
    ability_->Init(abilityInfo, application, handler, token);
    ability_->SetResult(resultCode, want);

    GTEST_LOG_(INFO) << "AaFwk_Ability_SetResult_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_StartAbilityForResult_0100
 * @tc.name: StartAbilityForResult
 * @tc.desc: Test whether startabilityforesult is called normally.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_StartAbilityForResult_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_StartAbilityForResult_0100 start";

    int resultCode = 0;
    Want want;
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    AbilityType type = AbilityType::PAGE;
    abilityInfo->type = type;
    std::shared_ptr<OHOSApplication> application = nullptr;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    sptr<IRemoteObject> token = nullptr;
    ability_->Init(abilityInfo, application, handler, token);
    ability_->StartAbilityForResult(want, resultCode);

    GTEST_LOG_(INFO) << "AaFwk_Ability_StartAbilityForResult_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_StartAbility_0100
 * @tc.name: StartAbility
 * @tc.desc: Test whether startability is called normally.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_StartAbility_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_StartAbility_0100 start";

    Want want;
    ability_->StartAbility(want);

    GTEST_LOG_(INFO) << "AaFwk_Ability_StartAbility_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_TerminateAbility_0100
 * @tc.name: TerminateAbility
 * @tc.desc: Test whether terminateability is called normally.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_TerminateAbility_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_TerminateAbility_0100 start";

    ability_->TerminateAbility();

    GTEST_LOG_(INFO) << "AaFwk_Ability_TerminateAbility_0100 end";
}

#ifdef WMS_COMPILE
HWTEST_F(AbilityBaseTest, AaFwk_Ability_GetWindow_001, Function | MediumTest | Level1)
{
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    AbilityType type = AbilityType::PAGE;
    abilityInfo->type = type;
    std::shared_ptr<OHOSApplication> application = nullptr;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    sptr<IRemoteObject> token = nullptr;
    ability_->Init(abilityInfo, application, handler, token);
}
#endif  // WMS_COMPILE

/**
 * @tc.number: AaFwk_Ability_OnStart_0100
 * @tc.name: OnStart
 * @tc.desc: Test whether OnStart is called normally and verify whether the members are correct.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnStart_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnStart_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    AbilityType type = AbilityType::PAGE;
    abilityInfo->type = type;
    std::shared_ptr<OHOSApplication> application = nullptr;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    sptr<IRemoteObject> token = nullptr;
    ability_->Init(abilityInfo, application, handler, token);

    Want want;
    ability_->OnStart(want);

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();
    LifeCycle::Event lifeCycleState = lifeCycle->GetLifecycleState();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::INACTIVE, state);
    EXPECT_EQ(LifeCycle::Event::ON_START, lifeCycleState);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnStart_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_OnStart_0200
 * @tc.name: OnStart
 * @tc.desc: Test the OnStart exception.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnStart_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnStart_0200 start";

    Want want;
    ability_->OnStart(want);
    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::UNINITIALIZED, state);
    EXPECT_EQ(nullptr, lifeCycle);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnStart_0200 end";
}

/**
 * @tc.number: AaFwk_Ability_OnStop_0100
 * @tc.name: OnStop
 * @tc.desc: Test whether onstop is called normally and verify whether the members are correct.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnStop_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnStop_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    AbilityType type = AbilityType::PAGE;
    abilityInfo->type = type;
    std::shared_ptr<OHOSApplication> application = nullptr;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    sptr<IRemoteObject> token = nullptr;
    ability_->Init(abilityInfo, application, handler, token);

    ability_->OnStop();

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();
    LifeCycle::Event lifeCycleState = lifeCycle->GetLifecycleState();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::INITIAL, state);
    EXPECT_EQ(LifeCycle::Event::ON_STOP, lifeCycleState);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnStop_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_OnStop_0200
 * @tc.name: OnStop
 * @tc.desc: Test the OnStop exception.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnStop_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnStop_0200 start";

    ability_->OnStop();

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::UNINITIALIZED, state);
    EXPECT_EQ(nullptr, lifeCycle);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnStop_0200 end";
}

/**
 * @tc.number: AaFwk_Ability_OnActive_0100
 * @tc.name: OnActive
 * @tc.desc: Test whether onactive is called normally and verify whether the member is correct.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnActive_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnActive_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    AbilityType type = AbilityType::PAGE;
    abilityInfo->type = type;
    std::shared_ptr<OHOSApplication> application = nullptr;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    sptr<IRemoteObject> token = nullptr;
    ability_->Init(abilityInfo, application, handler, token);

    ability_->OnActive();

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();
    LifeCycle::Event lifeCycleState = lifeCycle->GetLifecycleState();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::ACTIVE, state);
    EXPECT_EQ(LifeCycle::Event::ON_ACTIVE, lifeCycleState);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnActive_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_OnActive_0200
 * @tc.name: OnActive
 * @tc.desc: Test the OnActive exception.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnActive_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnActive_0200 start";

    ability_->OnActive();

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::UNINITIALIZED, state);
    EXPECT_EQ(nullptr, lifeCycle);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnActive_0200 end";
}

/**
 * @tc.number: AaFwk_Ability_OnInactive_0100
 * @tc.name: OnInactive
 * @tc.desc: Test whether oninactive is called normally and verify whether the member is correct.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnInactive_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnInactive_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    AbilityType type = AbilityType::PAGE;
    abilityInfo->type = type;
    std::shared_ptr<OHOSApplication> application = nullptr;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    sptr<IRemoteObject> token = nullptr;
    ability_->Init(abilityInfo, application, handler, token);

    ability_->OnInactive();

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();
    LifeCycle::Event lifeCycleState = lifeCycle->GetLifecycleState();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::INACTIVE, state);
    EXPECT_EQ(LifeCycle::Event::ON_INACTIVE, lifeCycleState);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnInactive_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_OnInactive_0200
 * @tc.name: OnInactive
 * @tc.desc: Test the OnInactive exception.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnInactive_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnInactive_0200 start";

    ability_->OnInactive();

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::UNINITIALIZED, state);
    EXPECT_EQ(nullptr, lifeCycle);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnInactive_0200 end";
}

/**
 * @tc.number: AaFwk_Ability_OnForeground_0100
 * @tc.name: OnForeground
 * @tc.desc: Test whether onforegroup is called normally, and verify whether the member is correct.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnForeground_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnForeground_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    AbilityType type = AbilityType::PAGE;
    abilityInfo->type = type;
    std::shared_ptr<OHOSApplication> application = nullptr;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    sptr<IRemoteObject> token = nullptr;
    ability_->Init(abilityInfo, application, handler, token);

    Want want;
    ability_->OnForeground(want);

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();
    LifeCycle::Event lifeCycleState = lifeCycle->GetLifecycleState();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::INACTIVE, state);
    EXPECT_EQ(LifeCycle::Event::ON_FOREGROUND, lifeCycleState);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnForeground_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_OnForeground_0200
 * @tc.name: OnForeground
 * @tc.desc: Test the OnInactive exception.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnForeground_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnForeground_0200 start";

    Want want;
    ability_->OnForeground(want);

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::UNINITIALIZED, state);
    EXPECT_EQ(nullptr, lifeCycle);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnForeground_0200 end";
}

/**
 * @tc.number: AaFwk_Ability_OnBackground_0100
 * @tc.name: OnBackground
 * @tc.desc: Test whether onbackground is called normally and verify whether the members are correct.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnBackground_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnBackground_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    AbilityType type = AbilityType::PAGE;
    abilityInfo->type = type;
    std::shared_ptr<OHOSApplication> application = nullptr;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    sptr<IRemoteObject> token = nullptr;
    ability_->Init(abilityInfo, application, handler, token);

    ability_->OnBackground();

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();
    LifeCycle::Event lifeCycleState = lifeCycle->GetLifecycleState();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::BACKGROUND, state);
    EXPECT_EQ(LifeCycle::Event::ON_BACKGROUND, lifeCycleState);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnBackground_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_OnBackground_0200
 * @tc.name: OnBackground
 * @tc.desc: Test the OnBackground exception.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnBackground_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnBackground_0200 start";

    ability_->OnBackground();

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();
    std::shared_ptr<LifeCycle> lifeCycle = ability_->GetLifecycle();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::UNINITIALIZED, state);
    EXPECT_EQ(nullptr, lifeCycle);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnBackground_0200 end";
}

/**
 * @tc.number: AaFwk_Ability_OnConnect_0100
 * @tc.name: OnConnect
 * @tc.desc: Test whether onconnect is called normally and verify whether the members are correct.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnConnect_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnConnect_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    AbilityType type = AbilityType::PAGE;
    abilityInfo->type = type;
    std::shared_ptr<OHOSApplication> application = nullptr;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    sptr<IRemoteObject> token = nullptr;
    ability_->Init(abilityInfo, application, handler, token);

    Want want;
    ability_->OnConnect(want);

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::ACTIVE, state);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnConnect_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_OnCommond_0100
 * @tc.name: OnCommand
 * @tc.desc: Test whether oncommand is called normally and verify whether the members are correct.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnCommond_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnCommond_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    AbilityType type = AbilityType::PAGE;
    abilityInfo->type = type;
    std::shared_ptr<OHOSApplication> application = nullptr;
    std::shared_ptr<AbilityHandler> handler = nullptr;
    sptr<IRemoteObject> token = nullptr;
    ability_->Init(abilityInfo, application, handler, token);

    Want want;
    ability_->OnCommand(want, false, 0);

    AbilityLifecycleExecutor::LifecycleState state = ability_->GetState();

    EXPECT_EQ(AbilityLifecycleExecutor::LifecycleState::ACTIVE, state);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnCommond_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_OnDisconnect_0100
 * @tc.name: OnDisconnect
 * @tc.desc: Test whether ondisconnect is called normally.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_OnDisconnect_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_OnDisconnect_0100 start";

    Want want;
    ability_->OnDisconnect(want);

    GTEST_LOG_(INFO) << "AaFwk_Ability_OnDisconnect_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_StartAbilitySetting_0100
 * @tc.name: StartAbility
 * @tc.desc: Test whether startability is called normally.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_StartAbilitySetting_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_StartAbilitySetting_0100 start";

    Want want;
    std::shared_ptr<AbilityStartSetting> setting = AbilityStartSetting::GetEmptySetting();

    ability_->StartAbility(want, *setting.get());

    GTEST_LOG_(INFO) << "AaFwk_Ability_StartAbilitySetting_0100 end";
}

/**
 * @tc.number: AaFwk_Ability_StartAbilitySetting_0200
 * @tc.name: StartAbility
 * @tc.desc: Test startability exception status.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_StartAbilitySetting_0200, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_StartAbilitySetting_0200 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->type = AbilityType::PAGE;
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);

    ability_->Init(abilityInfo, nullptr, handler, nullptr);
    Want want;
    std::shared_ptr<AbilityStartSetting> setting = AbilityStartSetting::GetEmptySetting();
    ability_->StartAbility(want, *setting.get());

    GTEST_LOG_(INFO) << "AaFwk_Ability_StartAbilitySetting_0200 end";
}

/**
 * @tc.number: AaFwk_Ability_PostTask_0100
 * @tc.name: PostTask
 * @tc.desc: Test whether posttask is called normally.
 */
HWTEST_F(AbilityBaseTest, AaFwk_Ability_PostTask_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "AaFwk_Ability_PostTask_0100 start";

    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->type = AbilityType::PAGE;
    std::shared_ptr<EventRunner> eventRunner = EventRunner::Create(abilityInfo->name);
    sptr<AbilityThread> abilityThread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    std::shared_ptr<AbilityHandler> handler = std::make_shared<AbilityHandler>(eventRunner, abilityThread);

    ability_->Init(abilityInfo, nullptr, handler, nullptr);
    auto task = []() { GTEST_LOG_(INFO) << "AaFwk_Ability_PostTask_001 task called"; };
    ability_->PostTask(task, 1000);

    GTEST_LOG_(INFO) << "AaFwk_Ability_PostTask_0100 end";
}
}  // namespace AppExecFwk
}  // namespace OHOS
