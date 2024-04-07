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

#include "ability_life_cycle_ability.h"
#include "app_log_wrapper.h"
#include "base_ability.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

AbilityLifeCycleAbility::~AbilityLifeCycleAbility()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void AbilityLifeCycleAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("AbilityLifeCycleAbility::Init called.");
    BaseAbility::Init(abilityInfo, application, handler, token);
    SubscribeEvent();

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void AbilityLifeCycleAbility::StopSelfAbility()
{
    TerminateAbility();
}

bool AbilityLifeCycleAbility::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AbilityLifeCycleAbilityEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility = *this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void AbilityLifeCycleAbility::OnStart(const Want &want)
{
    sequenceNumber_ = std::stoi(GetNoFromWantInfo(want));
    want_ = want;
    APP_LOGI("AbilityLifeCycleAbility::OnStart");

    BaseAbility::OnStart(want);
    TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifeCycleAbility::GetState(), "OnStart");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void AbilityLifeCycleAbility::OnStop()
{
    APP_LOGI("AbilityLifeCycleAbility::OnStop");
    BaseAbility::OnStop();
    TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifeCycleAbility::GetState(), "OnStop");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void AbilityLifeCycleAbility::OnActive()
{
    APP_LOGI("AbilityLifeCycleAbility::OnActive");
    BaseAbility::OnActive();
    TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifeCycleAbility::GetState(), "OnActive");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void AbilityLifeCycleAbility::OnInactive()
{
    APP_LOGI("AbilityLifeCycleAbility::OnInactive");
    BaseAbility::OnInactive();
    TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifeCycleAbility::GetState(), "OnInactive");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void AbilityLifeCycleAbility::OnBackground()
{
    APP_LOGI("AbilityLifeCycleAbility::OnBackground");
    BaseAbility::OnBackground();
    TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifeCycleAbility::GetState(), "OnBackground");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void AbilityLifeCycleAbility::OnForeground(const Want &want)
{
    APP_LOGI("AbilityLifeCycleAbility::OnForeground");
    BaseAbility::OnBackground();
    TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifeCycleAbility::GetState(), "OnForeground");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void AbilityLifeCycleAbility::OnCommand(const Want &want, bool restart, int startId)
{
    APP_LOGI("AbilityLifeCycleAbility::OnCommand");

    BaseAbility::OnCommand(want, restart, startId);
    TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifeCycleAbility::GetState(), "OnCommand");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

sptr<IRemoteObject> AbilityLifeCycleAbility::OnConnect(const Want &want)
{
    APP_LOGI("AbilityLifeCycleAbility::OnConnect");

    sptr<IRemoteObject> ret = BaseAbility::OnConnect(want);
    TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifeCycleAbility::GetState(), "OnConnect");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
    return ret;
}

void AbilityLifeCycleAbility::OnDisconnect(const Want &want)
{
    APP_LOGI("AbilityLifeCycleAbility::OnDisconnect");

    BaseAbility::OnDisconnect(want);
    TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifeCycleAbility::GetState(), "OnDisconnect");

    auto lifecycle = GetLifecycle();
    TestUtils::PublishEvent(
        APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycleState");
}

void AbilityLifeCycleAbility::TestConnectAbility()
{
    switch ((CaseIndex)AbilityLifeCycleAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            stub_ = new (std::nothrow) AbilityLifeCycleConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestConnectAbility");
        } break;
        case CaseIndex::TWO: {
            stub_ = new (std::nothrow) AbilityLifeCycleConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestConnectAbility");
        } break;
        case CaseIndex::THREE: {
            stub_ = new (std::nothrow) AbilityLifeCycleConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestConnectAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                stub_ = new (std::nothrow) AbilityLifeCycleConnectCallback();
                connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
                bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
                TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestConnectAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            sptr<AbilityLifeCycleConnectCallback> stub(new (std::nothrow) AbilityLifeCycleConnectCallback());
            sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
            bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestConnectAbility");
        } break;
        default:
            break;
    }
}

void AbilityLifeCycleAbility::TestStopAbility()
{
    switch ((CaseIndex)AbilityLifeCycleAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            bool ret = BaseAbility::GetContext()->StopAbility(want_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            bool ret = BaseAbility::GetContext()->StopAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            bool ret = BaseAbility::GetContext()->StopAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < 3; i++) {
                bool ret = BaseAbility::GetContext()->StopAbility(want_);
                TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestStopAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            bool ret = BaseAbility::GetContext()->StopAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        default:
            break;
    }
}

void AbilityLifeCycleAbility::TestDisconnectAbility()
{
    switch ((CaseIndex)AbilityLifeCycleAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            stub_ = new (std::nothrow) AbilityLifeCycleConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            stub_ = new (std::nothrow) AbilityLifeCycleConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            stub_ = new (std::nothrow) AbilityLifeCycleConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                stub_ = new (std::nothrow) AbilityLifeCycleConnectCallback();
                connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
                bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
                TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
                BaseAbility::GetContext()->DisconnectAbility(connCallback_);
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            stub_ = new (std::nothrow) AbilityLifeCycleConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        default:
            break;
    }
}

void AbilityLifeCycleAbility::TestStartAbility()
{
    switch ((CaseIndex)AbilityLifeCycleAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            BaseAbility::GetContext()->StartAbility(want_, 0);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                BaseAbility::GetContext()->StartAbility(want_, 0);
                TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestStartAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        default:
            break;
    }
}

void AbilityLifeCycleAbility::TestTerminateAbility()
{
    switch ((CaseIndex)AbilityLifeCycleAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                BaseAbility::GetContext()->TerminateAbility();
                TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestTerminateAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        default:
            break;
    }
}

void AbilityLifeCycleAbility::TestLifeCycleGetLifecycle()
{
    switch ((CaseIndex)AbilityLifeCycleAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            auto lifecycle = GetLifecycle();
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycle");
        } break;
        case CaseIndex::TWO: {
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                auto lifecycle = GetLifecycle();
            }
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycle");
        } break;
        case CaseIndex::THREE: {
            auto lifecycle = GetLifecycle();
            TestUtils::PublishEvent(
                APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestLifeCycleGetLifecycle");
        } break;
        case CaseIndex::FOUR: {
            auto lifecycle = GetLifecycle();
            lifecycle->AddObserver(nullptr);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycle");
        } break;
        case CaseIndex::FIVE: {
            auto lifecycle = GetLifecycle();
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycle");
        } break;
        case CaseIndex::SIX: {
            auto lifecycle = GetLifecycle();
            lifecycle->RemoveObserver(nullptr);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycle");
        } break;
        default:
            break;
    }
}

void AbilityLifeCycleAbility::TestLifeCycleGetLifecycleState()
{
    switch ((CaseIndex)AbilityLifeCycleAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            auto lifecycle = GetLifecycle();
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME,
                lifecycle->GetLifecycleState(),
                "TestLifeCycleGetLifecycleState");
        } break;
        case CaseIndex::TWO: {
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                auto lifecycle = GetLifecycle();
            }
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycleState");
        } break;
        case CaseIndex::THREE: {
            auto lifecycle = GetLifecycle();
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME,
                lifecycle->GetLifecycleState(),
                "TestLifeCycleGetLifecycleState");
        } break;
        case CaseIndex::FOUR: {
            auto lifecycle = GetLifecycle();
            lifecycle->AddObserver(nullptr);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycleState");
        } break;
        case CaseIndex::FIVE: {
            auto lifecycle = GetLifecycle();
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycleState");
        } break;
        case CaseIndex::SIX: {
            auto lifecycle = GetLifecycle();
            lifecycle->RemoveObserver(nullptr);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleGetLifecycleState");
        } break;
        default:
            break;
    }
}

void AbilityLifecycleLifecycleObserver::OnActive()
{
    TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 0, "OnActive");
}

void AbilityLifecycleLifecycleObserver::OnBackground()
{
    TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 0, "OnBackground");
}

void AbilityLifecycleLifecycleObserver::OnForeground(const Want &want)
{
    TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 0, "OnForeground");
}

void AbilityLifecycleLifecycleObserver::OnInactive()
{
    TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 0, "OnInactive");
}

void AbilityLifecycleLifecycleObserver::OnStart(const Want &want)
{
    TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 0, "OnStart");
}

void AbilityLifecycleLifecycleObserver::OnStop()
{
    TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 0, "OnStop");
}

void AbilityLifecycleLifecycleObserver::OnStateChanged(LifeCycle::Event event, const Want &want)
{
    TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 0, "OnStateChanged");
}

void AbilityLifecycleLifecycleObserver::OnStateChanged(LifeCycle::Event event)
{
    TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 0, "OnStateChanged");
}

void AbilityLifeCycleAbility::TestLifeCycleAddObserver()
{
    switch ((CaseIndex)AbilityLifeCycleAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleAddObserver");
        } break;
        case CaseIndex::TWO: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            for (int i = 0; i < (int)CaseIndex::ONE; i++) {
                lifecycle->AddObserver(lifecycleObserver);
            }
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleAddObserver");
        } break;
        case CaseIndex::THREE: {
            auto lifecycle = GetLifecycle();
            for (int i = 0; i < (int)CaseIndex::ONE; i++) {
                std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                    std::make_shared<AbilityLifecycleLifecycleObserver>();
                lifecycle->AddObserver(lifecycleObserver);
            }
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleAddObserver");
        } break;
        case CaseIndex::FOUR: {
            auto lifecycle = GetLifecycle();
            for (int i = 0; i < (int)CaseIndex::ONE; i++) {
                std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                    std::make_shared<AbilityLifecycleLifecycleObserver>();
                lifecycle->AddObserver(lifecycleObserver);
                lifecycle->RemoveObserver(lifecycleObserver);
            }
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleAddObserver");
        } break;
        case CaseIndex::FIVE: {
            auto lifecycle = GetLifecycle();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                lifecycle->AddObserver(nullptr);
                lifecycle->RemoveObserver(nullptr);
            }
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleAddObserver");
        } break;
        default:
            break;
    }
}

void AbilityLifeCycleAbility::TestLifeCycleDispatchLifecycleOne()
{
    switch ((CaseIndex)AbilityLifeCycleAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::TWO: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_FOREGROUND);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::THREE: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_BACKGROUND);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::FOUR: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_INACTIVE);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        default:
            break;
    }
}

void AbilityLifeCycleAbility::TestLifeCycleDispatchLifecycleTwo()
{
    switch ((CaseIndex)AbilityLifeCycleAbility::sequenceNumber_) {
        case CaseIndex::FIVE: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_START);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::SIX: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_STOP);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::SEVEN: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::UNDEFINED);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::EIGHT: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        case CaseIndex::NINE: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            lifecycle->AddObserver(lifecycleObserver);
            lifecycle->DispatchLifecycle(LifeCycle::Event::UNDEFINED);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleDispatchLifecycle");
        } break;
        default:
            break;
    }
}

void AbilityLifeCycleAbility::TestLifeCycleDispatchLifecycle()
{
    TestLifeCycleDispatchLifecycleOne();
    TestLifeCycleDispatchLifecycleTwo();
}

void AbilityLifeCycleAbility::TestLifeCycleRemoveObserver()
{
    switch ((CaseIndex)AbilityLifeCycleAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            lifecycle->RemoveObserver(lifecycleObserver);
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleRemoveObserver");
        } break;
        case CaseIndex::TWO: {
            auto lifecycle = GetLifecycle();
            std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                std::make_shared<AbilityLifecycleLifecycleObserver>();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                lifecycle->RemoveObserver(lifecycleObserver);
            }
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleRemoveObserver");
        } break;
        case CaseIndex::THREE: {
            auto lifecycle = GetLifecycle();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                    std::make_shared<AbilityLifecycleLifecycleObserver>();
                lifecycle->RemoveObserver(lifecycleObserver);
            }
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleRemoveObserver");
        } break;
        case CaseIndex::FOUR: {
            auto lifecycle = GetLifecycle();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                std::shared_ptr<AbilityLifecycleLifecycleObserver> lifecycleObserver =
                    std::make_shared<AbilityLifecycleLifecycleObserver>();
                lifecycle->AddObserver(lifecycleObserver);
                lifecycle->RemoveObserver(lifecycleObserver);
            }
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleRemoveObserver");
        } break;
        case CaseIndex::FIVE: {
            auto lifecycle = GetLifecycle();
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                lifecycle->RemoveObserver(nullptr);
            }
            TestUtils::PublishEvent(APP_ABILITY_LIFE_CYCLE_RESP_EVENT_NAME, 1, "TestLifeCycleRemoveObserver");
        } break;
        default:
            break;
    }
}

void AbilityLifeCycleAbilityEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI(
        "AbilityLifeCycleAbilityEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("AbilityLifeCycleAbilityEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("AbilityLifeCycleAbilityEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());

    auto eventName = data.GetWant().GetAction();
    if (std::strcmp(eventName.c_str(), APP_ABILITY_LIFE_CYCLE_REQ_EVENT_NAME.c_str()) == 0) {
        auto target = data.GetData();
        auto func = mapTestFunc_.find(target);
        if (func != mapTestFunc_.end()) {
            func->second();
        } else {
            APP_LOGI("AbilityLifeCycleAbilityEventSubscriber::OnReceiveEvent: CommonEventData error(%{public}s)",
                target.c_str());
        }
    }
}
REGISTER_AA(AbilityLifeCycleAbility)
}  // namespace AppExecFwk
}  // namespace OHOS