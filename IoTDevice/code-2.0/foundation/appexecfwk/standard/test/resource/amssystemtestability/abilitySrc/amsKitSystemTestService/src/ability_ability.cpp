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

#include "ability_ability.h"
#include "app_log_wrapper.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

AbilityAbility::~AbilityAbility()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void AbilityAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("AbilityAbility::Init called.");
    BaseAbility::Init(abilityInfo, application, handler, token);

    SubscribeEvent();
}

void AbilityAbility::DoTestCase()
{
    if (!AbilityAbility::sequenceNumber_.empty()) {
        switch ((CaseIndex)std::stoi(AbilityAbility::sequenceNumber_)) {
            case CaseIndex::TWELVE:
                OHOS::AppExecFwk::PostTask<AbilityConnectCallback>();
                break;
            case CaseIndex::THIRTEEN:
                DoWhile<AbilityConnectCallback>();
                break;
            case CaseIndex::FOURTEEN:
                StopAbility(want_);
                break;
            case CaseIndex::FIFTEEN:
                TerminateAbility();
                break;
            default:
                break;
        }
    }
}

bool AbilityAbility::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(APP_ABILITY_REQ_EVENT_NAME);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AbilityAbilityEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility = *this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void AbilityAbility::OnStart(const Want &want)
{
    APP_LOGI("AbilityAbility::OnStart");

    sequenceNumber_ = GetNoFromWantInfo(want);
    APP_LOGI("AbilityAbility::OnStart sequenceNumber_ = %{public}s", sequenceNumber_.c_str());
    want_ = want;
    APP_LOGI("AbilityAbility::OnStart");
    BaseAbility::OnStart(want);
    TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnStart");

    DoTestCase();
}

void AbilityAbility::OnStop()
{
    APP_LOGI("AbilityAbility::OnStop");

    BaseAbility::OnStop();
    TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INITIAL, "OnStop");

    DoTestCase();
}

void AbilityAbility::OnActive()
{
    APP_LOGI("AbilityAbility::OnActive");

    BaseAbility::OnActive();
    TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnActive");

    DoTestCase();
}

void AbilityAbility::OnInactive()
{
    APP_LOGI("AbilityAbility::OnInactive");

    BaseAbility::OnInactive();
    TestUtils::PublishEvent(
        APP_ABILITY_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnInactive");

    DoTestCase();
}

void AbilityAbility::OnBackground()
{
    APP_LOGI("AbilityAbility::OnBackground");

    BaseAbility::OnBackground();
    TestUtils::PublishEvent(
        APP_ABILITY_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnBackground");

    DoTestCase();
}

void AbilityAbility::OnForeground(const Want &want)
{
    APP_LOGI("AbilityAbility::OnForeground");

    BaseAbility::OnBackground();
    TestUtils::PublishEvent(
        APP_ABILITY_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnForeground");

    DoTestCase();
}

void AbilityAbility::OnCommand(const Want &want, bool restart, int startId)
{
    APP_LOGI("AbilityAbility::OnCommand");

    BaseAbility::OnCommand(want, restart, startId);
    TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnCommand");

    DoTestCase();
}

sptr<IRemoteObject> AbilityAbility::OnConnect(const Want &want)
{
    APP_LOGI("AbilityAbility::OnConnect");

    sptr<IRemoteObject> ret = BaseAbility::OnConnect(want);
    TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnConnect");

    DoTestCase();

    return ret;
}

void AbilityAbility::OnDisconnect(const Want &want)
{
    APP_LOGI("AbilityAbility::OnDisconnect");

    BaseAbility::OnDisconnect(want);
    TestUtils::PublishEvent(
        APP_ABILITY_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnDisconnect");

    DoTestCase();
}

void AbilityAbility::OnNewWant(const Want &want)
{
    APP_LOGI("AbilityAbility::OnNewWant");

    BaseAbility::OnNewWant(want);
    TestUtils::PublishEvent(
        APP_ABILITY_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnNewWant");

    DoTestCase();
}

void AbilityAbility::TestConnectAbility()
{
    APP_LOGI("AbilityAbility::OnStart sequenceNumber_ = %{public}s  %{public}d",
        sequenceNumber_.c_str(),
        std::stoi(AbilityAbility::sequenceNumber_));

    switch ((CaseIndex)std::stoi(AbilityAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = ConnectAbility(want_, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestConnectAbility");
            sleep(2);
            DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::TWO: {
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            bool ret = ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestConnectAbility");
            sleep(2);
            DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::THREE: {
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            bool ret = ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestConnectAbility");
            sleep(2);
            DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                stub_ = new (std::nothrow) AbilityConnectCallback();
                connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
                bool ret = ConnectAbility(want_, connCallback_);
                TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestConnectAbility");
                sleep(2);
                DisconnectAbility(connCallback_);
            }
        } break;
        case CaseIndex::FIVE: {
            sptr<AbilityConnectCallback> stub(new (std::nothrow) AbilityConnectCallback());
            sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
            bool ret = ConnectAbility(want_, connCallback);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestConnectAbility");
            sleep(2);
            DisconnectAbility(connCallback);
        } break;
        default:
            break;
    }
}

void AbilityAbility::TestStopAbility()
{
    APP_LOGI("AbilityAbility::OnStart sequenceNumber_ = %{public}s  %{public}d",
        sequenceNumber_.c_str(),
        std::stoi(AbilityAbility::sequenceNumber_));

    switch ((CaseIndex)std::stoi(AbilityAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            bool ret = StopAbility(want_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            bool ret = StopAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            bool ret = StopAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < 3; i++) {
                bool ret = StopAbility(want_);
                TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestStopAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            bool ret = StopAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        default:
            break;
    }
}

void AbilityAbility::TestDisconnectAbility()
{
    APP_LOGI("AbilityAbility::OnStart sequenceNumber_ = %{public}s  %{public}d",
        sequenceNumber_.c_str(),
        std::stoi(AbilityAbility::sequenceNumber_));

    switch ((CaseIndex)std::stoi(AbilityAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = ConnectAbility(want_, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < 3; i++) {
                stub_ = new (std::nothrow) AbilityConnectCallback();
                connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
                bool ret = ConnectAbility(want_, connCallback_);
                TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
                DisconnectAbility(connCallback_);
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            DisconnectAbility(connCallback_);
        } break;
        default:
            break;
    }
}

void AbilityAbility::TestStartAbility()
{
    APP_LOGI("AbilityAbility::OnStart sequenceNumber_ = %{public}s  %{public}d",
        sequenceNumber_.c_str(),
        std::stoi(AbilityAbility::sequenceNumber_));

    switch ((CaseIndex)std::stoi(AbilityAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            StartAbility(want_, 0);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            StartAbility(want, INT_MAX);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            StartAbility(want, INT_MIN);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                StartAbility(want_, 0);
                TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestStartAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            StartAbility(want, 0);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        default:
            break;
    }
}

void AbilityAbility::TestTerminateAbility()
{
    APP_LOGI("AbilityAbility::OnStart sequenceNumber_ = %{public}s  %{public}d",
        sequenceNumber_.c_str(),
        std::stoi(AbilityAbility::sequenceNumber_));

    switch ((CaseIndex)std::stoi(AbilityAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            TerminateAbility();
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            StartAbility(want, 0);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestTerminateAbility");
            sleep(2);
            StopAbility(want);
            TerminateAbility();
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            StartAbility(want, 0);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestTerminateAbility");
            sleep(2);
            StopAbility(want);
            TerminateAbility();
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                TerminateAbility();
                TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestTerminateAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            StartAbility(want, 0);
            sleep(2);
            TerminateAbility();
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        default:
            break;
    }
}

void AbilityAbility::TestAbilityStartAbility()
{
    APP_LOGI("AbilityAbility::OnStart sequenceNumber_ = %{public}s  %{public}d",
        sequenceNumber_.c_str(),
        std::stoi(AbilityAbility::sequenceNumber_));

    switch ((CaseIndex)std::stoi(AbilityAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            StartAbility(want_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestAbilityStartAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            StartAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestAbilityStartAbility");
            sleep(1);
            StopAbility(want);
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            StartAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestAbilityStartAbility");
            sleep(1);
            StopAbility(want);
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                StartAbility(want_);
                TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestAbilityStartAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            StartAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestAbilityStartAbility");
        } break;
        default:
            break;
    }
}

void AbilityAbility::TestAbilityConnectAbility()
{
    APP_LOGI("AbilityAbility::OnStart sequenceNumber_ = %{public}s  %{public}d",
        sequenceNumber_.c_str(),
        std::stoi(AbilityAbility::sequenceNumber_));

    switch ((CaseIndex)std::stoi(AbilityAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = ConnectAbility(want_, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityConnectAbility");
        } break;
        case CaseIndex::TWO: {
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            bool ret = ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityConnectAbility");
            sleep(2);
            DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::THREE: {
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            bool ret = ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityConnectAbility");
            sleep(2);
            DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                stub_ = new (std::nothrow) AbilityConnectCallback();
                connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
                bool ret = ConnectAbility(want_, connCallback_);
                TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityConnectAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            sptr<AbilityConnectCallback> stub(new (std::nothrow) AbilityConnectCallback());
            sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
            bool ret = ConnectAbility(want_, connCallback);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityConnectAbility");
        } break;
        default:
            break;
    }
}

void AbilityAbility::TestAbilityStopAbility()
{
    APP_LOGI("AbilityAbility::OnStart sequenceNumber_ = %{public}s  %{public}d",
        sequenceNumber_.c_str(),
        std::stoi(AbilityAbility::sequenceNumber_));

    switch ((CaseIndex)std::stoi(AbilityAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            bool ret = StopAbility(want_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityStopAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            StartAbility(want);
            sleep(1);
            bool ret = StopAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityStopAbility");
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            StartAbility(want);
            sleep(1);
            bool ret = StopAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityStopAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                bool ret = StopAbility(want_);
                TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityStopAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            StartAbility(want);
            bool ret = StopAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityStopAbility");
        } break;
        default:
            break;
    }
}

void AbilityAbility::TestAbilityGetLifecycle()
{
    APP_LOGI("AbilityAbility::OnStart sequenceNumber_ = %{public}s  %{public}d",
        sequenceNumber_.c_str(),
        std::stoi(AbilityAbility::sequenceNumber_));

    switch ((CaseIndex)std::stoi(AbilityAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            auto lifecycle = GetLifecycle();
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestAbilityGetLifecycle");
        } break;
        case CaseIndex::TWO: {
            for (int i = 0; i < (int)CaseIndex::HANDRED; i++) {
                auto lifecycle = GetLifecycle();
            }
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestAbilityGetLifecycle");
        } break;
        case CaseIndex::THREE: {
            auto lifecycle = GetLifecycle();
            TestUtils::PublishEvent(
                APP_ABILITY_RESP_EVENT_NAME, lifecycle->GetLifecycleState(), "TestAbilityGetLifecycle");
        } break;
        case CaseIndex::FOUR: {
            auto lifecycle = GetLifecycle();
            lifecycle->AddObserver(nullptr);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestAbilityGetLifecycle");
        } break;
        case CaseIndex::FIVE: {
            auto lifecycle = GetLifecycle();
            lifecycle->DispatchLifecycle(LifeCycle::Event::ON_ACTIVE);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestAbilityGetLifecycle");
        } break;
        case CaseIndex::SIX: {
            auto lifecycle = GetLifecycle();
            lifecycle->RemoveObserver(nullptr);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, 1, "TestAbilityGetLifecycle");
        } break;
        default:
            break;
    }
}

void AbilityAbility::TestAbilityDisconnectAbility()
{
    APP_LOGI("AbilityAbility::OnStart sequenceNumber_ = %{public}s  %{public}d",
        sequenceNumber_.c_str(),
        std::stoi(AbilityAbility::sequenceNumber_));

    switch ((CaseIndex)std::stoi(AbilityAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = ConnectAbility(want_, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityDisconnectAbility");
            sleep(1);
            DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityDisconnectAbility");
            sleep(1);
            DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityDisconnectAbility");
            sleep(1);
            DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                stub_ = new (std::nothrow) AbilityConnectCallback();
                connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
                bool ret = ConnectAbility(want_, connCallback_);
                TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityDisconnectAbility");
                sleep(1);
                DisconnectAbility(connCallback_);
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            stub_ = new (std::nothrow) AbilityConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_RESP_EVENT_NAME, ret, "TestAbilityDisconnectAbility");
            sleep(1);
            DisconnectAbility(connCallback_);
        } break;
        default:
            break;
    }
}

void AbilityAbilityEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("AbilityAbilityEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("AbilityAbilityEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("AbilityAbilityEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());

    auto eventName = data.GetWant().GetAction();
    if (std::strcmp(eventName.c_str(), APP_ABILITY_REQ_EVENT_NAME.c_str()) == 0) {
        auto target = data.GetData();
        auto func = mapTestFunc_.find(target);
        if (func != mapTestFunc_.end()) {
            func->second();
        } else {
            APP_LOGI(
                "AbilityAbilityEventSubscriber::OnReceiveEvent: CommonEventData error(%{public}s)", target.c_str());
        }
    }
}
REGISTER_AA(AbilityAbility)
}  // namespace AppExecFwk
}  // namespace OHOS
