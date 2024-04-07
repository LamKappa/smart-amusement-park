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

#include "life_cycle_observer_ability.h"
#include "app_log_wrapper.h"
#include "base_ability.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

void LifecycleObserverLifecycleObserver::DoTask()
{
    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::TWO:
            PostTask<LifecycleObserverLifecycleCallbacks>();
            break;
        case CaseIndex::THREE:
            DoWhile<LifecycleObserverLifecycleCallbacks>();
            break;
        case CaseIndex::SIX:
            break;
        default:
            break;
    }
}

void LifecycleObserverLifecycleObserver::OnActive()
{
    DoTask();
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 0, "OnActive");
}

void LifecycleObserverLifecycleObserver::OnBackground()
{
    DoTask();
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 0, "OnBackground");
}

void LifecycleObserverLifecycleObserver::OnForeground(const Want &want)
{
    DoTask();
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 0, "OnForeground");
}

void LifecycleObserverLifecycleObserver::OnInactive()
{
    DoTask();
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 0, "OnInactive");
}

void LifecycleObserverLifecycleObserver::OnStart(const Want &want)
{
    DoTask();
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 0, "OnStart");
}

void LifecycleObserverLifecycleObserver::OnStop()
{
    DoTask();
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 0, "OnStop");
}

void LifecycleObserverLifecycleObserver::OnStateChanged(LifeCycle::Event event, const Want &want)
{
    DoTask();
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 0, "OnStateChanged");
}

void LifecycleObserverLifecycleObserver::OnStateChanged(LifeCycle::Event event)
{
    DoTask();
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 0, "OnStateChanged");
}

void LifecycleObserverLifecycleCallbacks::OnAbilityStart(const std::shared_ptr<Ability> &ability)
{
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ability->GetState(), "OnAbilityStart");
}

void LifecycleObserverLifecycleCallbacks::OnAbilityInactive(const std::shared_ptr<Ability> &ability)
{
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ability->GetState(), "OnAbilityInactive");
}

void LifecycleObserverLifecycleCallbacks::OnAbilityBackground(const std::shared_ptr<Ability> &ability)
{
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ability->GetState(), "OnAbilityBackground");
}

void LifecycleObserverLifecycleCallbacks::OnAbilityForeground(const std::shared_ptr<Ability> &ability)
{
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ability->GetState(), "OnAbilityForeground");
}

void LifecycleObserverLifecycleCallbacks::OnAbilityActive(const std::shared_ptr<Ability> &ability)
{
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ability->GetState(), "OnAbilityActive");
}

void LifecycleObserverLifecycleCallbacks::OnAbilityStop(const std::shared_ptr<Ability> &ability)
{
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ability->GetState(), "OnAbilityStop");
}

void LifecycleObserverLifecycleCallbacks::OnAbilitySaveState(const PacMap &outState)
{}

LifeCycleObserverAbility::~LifeCycleObserverAbility()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void LifeCycleObserverAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("LifeCycleObserverAbility::Init called.");
    BaseAbility::Init(abilityInfo, application, handler, token);

    SubscribeEvent();

    lifecycleCallbacks_ = std::make_shared<LifecycleObserverLifecycleCallbacks>();
    GetApplication()->RegisterAbilityLifecycleCallbacks(lifecycleCallbacks_);
    lifecycleObserver_ = std::make_shared<LifecycleObserverLifecycleObserver>();
    GetLifecycle()->AddObserver(lifecycleObserver_);

    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME,
        BaseAbility::GetLifecycle()->GetLifecycleState(),
        "TestLifeCycleGetLifecycleState");
}

void LifeCycleObserverAbility::StopSelfAbility()
{
    TerminateAbility();
}

bool LifeCycleObserverAbility::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<LifeCycleObserverAbilityEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility = *this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void LifeCycleObserverAbility::OnStart(const Want &want)
{
    sequenceNumber_ = std::stoi(GetNoFromWantInfo(want));
    want_ = want;
    APP_LOGI("LifeCycleObserverAbility::OnStart");

    BaseAbility::OnStart(want);
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, LifeCycleObserverAbility::GetState(), "OnStart");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");

    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::FOUR:
            StopAbility(want);
            break;
        case CaseIndex::FIVE:
            TerminateAbility();
            break;
        default:
            break;
    }
}

void LifeCycleObserverAbility::OnStop()
{
    APP_LOGI("LifeCycleObserverAbility::OnStop");
    BaseAbility::OnStop();
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, LifeCycleObserverAbility::GetState(), "OnStop");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void LifeCycleObserverAbility::OnActive()
{
    APP_LOGI("LifeCycleObserverAbility::OnActive");
    BaseAbility::OnActive();
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, LifeCycleObserverAbility::GetState(), "OnActive");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void LifeCycleObserverAbility::OnInactive()
{
    APP_LOGI("LifeCycleObserverAbility::OnInactive");
    BaseAbility::OnInactive();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, LifeCycleObserverAbility::GetState(), "OnInactive");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void LifeCycleObserverAbility::OnBackground()
{
    APP_LOGI("LifeCycleObserverAbility::OnBackground");
    BaseAbility::OnBackground();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, LifeCycleObserverAbility::GetState(), "OnBackground");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void LifeCycleObserverAbility::OnForeground(const Want &want)
{
    APP_LOGI("LifeCycleObserverAbility::OnForeground");
    BaseAbility::OnBackground();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, LifeCycleObserverAbility::GetState(), "OnForeground");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void LifeCycleObserverAbility::OnCommand(const Want &want, bool restart, int startId)
{
    APP_LOGI("LifeCycleObserverAbility::OnCommand");

    BaseAbility::OnCommand(want, restart, startId);
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, LifeCycleObserverAbility::GetState(), "OnCommand");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

sptr<IRemoteObject> LifeCycleObserverAbility::OnConnect(const Want &want)
{
    APP_LOGI("LifeCycleObserverAbility::OnConnect");

    sptr<IRemoteObject> ret = BaseAbility::OnConnect(want);
    TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, LifeCycleObserverAbility::GetState(), "OnConnect");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
    return ret;
}

void LifeCycleObserverAbility::OnDisconnect(const Want &want)
{
    APP_LOGI("LifeCycleObserverAbility::OnDisconnect");

    BaseAbility::OnDisconnect(want);
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, LifeCycleObserverAbility::GetState(), "OnDisconnect");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void LifeCycleObserverAbility::TestConnectAbility()
{
    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            stub_ = new (std::nothrow) LifeCycleObserverConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestConnectAbility");
        } break;
        case CaseIndex::TWO: {
            stub_ = new (std::nothrow) LifeCycleObserverConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestConnectAbility");
        } break;
        case CaseIndex::THREE: {
            stub_ = new (std::nothrow) LifeCycleObserverConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestConnectAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                stub_ = new (std::nothrow) LifeCycleObserverConnectCallback();
                connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
                bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
                TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestConnectAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            sptr<LifeCycleObserverConnectCallback> stub(new (std::nothrow) LifeCycleObserverConnectCallback());
            sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
            bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestConnectAbility");
        } break;
        default:
            break;
    }
}

void LifeCycleObserverAbility::TestStopAbility()
{
    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            bool ret = BaseAbility::GetContext()->StopAbility(want_);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            bool ret = BaseAbility::GetContext()->StopAbility(want);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            bool ret = BaseAbility::GetContext()->StopAbility(want);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                bool ret = BaseAbility::GetContext()->StopAbility(want_);
                TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestStopAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            bool ret = BaseAbility::GetContext()->StopAbility(want);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        default:
            break;
    }
}

void LifeCycleObserverAbility::TestDisconnectAbility()
{
    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            stub_ = new (std::nothrow) LifeCycleObserverConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            stub_ = new (std::nothrow) LifeCycleObserverConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            stub_ = new (std::nothrow) LifeCycleObserverConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                stub_ = new (std::nothrow) LifeCycleObserverConnectCallback();
                connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
                bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
                TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
                BaseAbility::GetContext()->DisconnectAbility(connCallback_);
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            stub_ = new (std::nothrow) LifeCycleObserverConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        default:
            break;
    }
}

void LifeCycleObserverAbility::TestStartAbility()
{
    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            BaseAbility::GetContext()->StartAbility(want_, 0);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                BaseAbility::GetContext()->StartAbility(want_, 0);
                TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestStartAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        default:
            break;
    }
}

void LifeCycleObserverAbility::TestTerminateAbility()
{
    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                BaseAbility::GetContext()->TerminateAbility();
                TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestTerminateAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        default:
            break;
    }
}

void LifeCycleObserverAbility::TestLifeCycleGetLifecycle()
{
    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            auto lifecycle = GetLifecycle();
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycle");
        } break;
        case CaseIndex::TWO: {
            for (int i = 0; i < 100; i++) {
                auto lifecycle = GetLifecycle();
            }
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycle");
        } break;
        case CaseIndex::THREE: {
            auto lifecycle = GetLifecycle();
            TestUtils::PublishEvent(
                APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycle");
        } break;
        case CaseIndex::FOUR: {
            auto lifecycle = GetLifecycle();
            lifecycle->AddObserver(nullptr);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycle");
        } break;
        case CaseIndex::FIVE: {
            auto lifecycle = GetLifecycle();
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycle");
        } break;
        case CaseIndex::SIX: {
            auto lifecycle = GetLifecycle();
            lifecycle->RemoveObserver(nullptr);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycle");
        } break;
        default:
            break;
    }
}

void LifeCycleObserverAbility::TestLifeCycleGetLifecycleState()
{
    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            auto lifecycle = GetLifecycle();
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME,
                lifecycle->GetLifecycleState(),
                "TestLifeCycleGetLifecycleState");
        } break;
        case CaseIndex::TWO: {
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                auto lifecycle = GetLifecycle();
            }
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycleState");
        } break;
        case CaseIndex::THREE: {
            auto lifecycle = GetLifecycle();
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME,
                lifecycle->GetLifecycleState(),
                "TestLifeCycleGetLifecycleState");
        } break;
        case CaseIndex::FOUR: {
            auto lifecycle = GetLifecycle();
            lifecycle->AddObserver(nullptr);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycleState");
        } break;
        case CaseIndex::FIVE: {
            auto lifecycle = GetLifecycle();
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycleState");
        } break;
        case CaseIndex::SIX: {
            auto lifecycle = GetLifecycle();
            lifecycle->RemoveObserver(nullptr);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycleState");
        } break;
        default:
            break;
    }
}

void LifeCycleObserverAbility::TestLifeCycleAddObserver()
{
    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                std::make_shared<LifecycleObserverLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleAddObserver");
        } break;
        case CaseIndex::TWO: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                std::make_shared<LifecycleObserverLifecycleObserver>();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                lifecycle->AddObserver(lifecycleObserver);
            }
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleAddObserver");
        } break;
        case CaseIndex::THREE: {
            auto lifecycle = GetLifecycle();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                    std::make_shared<LifecycleObserverLifecycleObserver>();
                lifecycle->AddObserver(lifecycleObserver);
            }
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleAddObserver");
        } break;
        case CaseIndex::FOUR: {
            auto lifecycle = GetLifecycle();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                    std::make_shared<LifecycleObserverLifecycleObserver>();
                lifecycle->AddObserver(lifecycleObserver);
                lifecycle->RemoveObserver(lifecycleObserver);
            }
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleAddObserver");
        } break;
        case CaseIndex::FIVE: {
            auto lifecycle = GetLifecycle();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                lifecycle->AddObserver(nullptr);
                lifecycle->RemoveObserver(nullptr);
            }
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleAddObserver");
        } break;
        default:
            break;
    }
}

void LifeCycleObserverAbility::TestLifeCycleDispatchLifecycleOne()
{
    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                std::make_shared<LifecycleObserverLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::TWO: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                std::make_shared<LifecycleObserverLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_BACKGROUND);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::THREE: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                std::make_shared<LifecycleObserverLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_FOREGROUND);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::FOUR: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                std::make_shared<LifecycleObserverLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_INACTIVE);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        default:
            break;
    }
}

void LifeCycleObserverAbility::TestLifeCycleDispatchLifecycleTwo()
{
    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::FIVE: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                std::make_shared<LifecycleObserverLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_START);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::SIX: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                std::make_shared<LifecycleObserverLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_STOP);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::SEVEN: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                std::make_shared<LifecycleObserverLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::UNDEFINED);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::EIGHT: {
            auto lifecycle = GetLifecycle();
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::NINE: {
            auto lifecycle = GetLifecycle();
            lifecycle->DispatchLifecycle(LifeCycle::Event::UNDEFINED);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        default:
            break;
    }
}

void LifeCycleObserverAbility::TestLifeCycleDispatchLifecycle()
{
    TestLifeCycleDispatchLifecycleOne();
    TestLifeCycleDispatchLifecycleTwo();
}

void LifeCycleObserverAbility::TestLifeCycleRemoveObserver()
{
    switch ((CaseIndex)LifeCycleObserverAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                std::make_shared<LifecycleObserverLifecycleObserver>();
            lifecycle->RemoveObserver(lifecycleObserver);
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleRemoveObserver");
        } break;
        case CaseIndex::TWO: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                std::make_shared<LifecycleObserverLifecycleObserver>();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                lifecycle->RemoveObserver(lifecycleObserver);
            }
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleRemoveObserver");
        } break;
        case CaseIndex::THREE: {
            auto lifecycle = GetLifecycle();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                    std::make_shared<LifecycleObserverLifecycleObserver>();
                lifecycle->RemoveObserver(lifecycleObserver);
            }
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleRemoveObserver");
        } break;
        case CaseIndex::FOUR: {
            auto lifecycle = GetLifecycle();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver =
                    std::make_shared<LifecycleObserverLifecycleObserver>();
                lifecycle->AddObserver(lifecycleObserver);
                lifecycle->RemoveObserver(lifecycleObserver);
            }
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleRemoveObserver");
        } break;
        case CaseIndex::FIVE: {
            auto lifecycle = GetLifecycle();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                lifecycle->RemoveObserver(nullptr);
            }
            TestUtils::PublishEvent(APP_LIFE_CYCLE_OBSERVER_RESP_EVENT_NAME, 1, "TestLifeCycleRemoveObserver");
        } break;
        default:
            break;
    }
}

void LifeCycleObserverAbilityEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI(
        "LifeCycleObserverAbilityEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("LifeCycleObserverAbilityEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("LifeCycleObserverAbilityEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());

    auto eventName = data.GetWant().GetAction();
    if (std::strcmp(eventName.c_str(), APP_LIFE_CYCLE_OBSERVER_REQ_EVENT_NAME.c_str()) == 0) {
        auto target = data.GetData();
        auto func = mapTestFunc_.find(target);
        if (func != mapTestFunc_.end()) {
            func->second();
        } else {
            APP_LOGI("LifeCycleObserverAbilityEventSubscriber::OnReceiveEvent: CommonEventData error(%{public}s)",
                target.c_str());
        }
    }
}
REGISTER_AA(LifeCycleObserverAbility)
}  // namespace AppExecFwk
}  // namespace OHOS