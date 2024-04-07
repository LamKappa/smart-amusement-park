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

#include "ability_connection_ability.h"
#include "app_log_wrapper.h"
#include "base_ability.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

AbilityConnectionAbility::~AbilityConnectionAbility()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void AbilityConnectionAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("AbilityConnectionAbility::Init called.");
    BaseAbility::Init(abilityInfo, application, handler, token);

    SubscribeEvent();
}

bool AbilityConnectionAbility::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(APP_ABILITY_CONNECTION_REQ_EVENT_NAME);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AbilityConnectionAbilityEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility = *this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void AbilityConnectionAbility::OnStart(const Want &want)
{
    want_ = want;
    sequenceNumber_ = GetNoFromWantInfo(want);
    APP_LOGI("AbilityConnectionAbility::OnStart");

    BaseAbility::OnStart(want);
    TestUtils::PublishEvent(
        APP_ABILITY_CONNECTION_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnStart");
}

void AbilityConnectionAbility::OnStop()
{
    APP_LOGI("AbilityConnectionAbility::OnStop");
    BaseAbility::OnStop();
    TestUtils::PublishEvent(
        APP_ABILITY_CONNECTION_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INITIAL, "OnStop");
}

void AbilityConnectionAbility::OnActive()
{
    APP_LOGI("AbilityConnectionAbility::OnActive");
    BaseAbility::OnActive();
    TestUtils::PublishEvent(
        APP_ABILITY_CONNECTION_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnActive");
}

void AbilityConnectionAbility::OnInactive()
{
    APP_LOGI("AbilityConnectionAbility::OnInactive");
    BaseAbility::OnInactive();
    TestUtils::PublishEvent(
        APP_ABILITY_CONNECTION_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnInactive");
}

void AbilityConnectionAbility::OnBackground()
{
    APP_LOGI("AbilityConnectionAbility::OnBackground");
    BaseAbility::OnBackground();
    TestUtils::PublishEvent(
        APP_ABILITY_CONNECTION_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnBackground");
}

void AbilityConnectionAbility::OnForeground(const Want &want)
{
    APP_LOGI("AbilityConnectionAbility::OnForeground");
    BaseAbility::OnBackground();
    TestUtils::PublishEvent(
        APP_ABILITY_CONNECTION_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnForeground");
}

void AbilityConnectionAbility::OnCommand(const Want &want, bool restart, int startId)
{
    APP_LOGI("AbilityConnectionAbility::OnCommand");

    BaseAbility::OnCommand(want, restart, startId);
    TestUtils::PublishEvent(
        APP_ABILITY_CONNECTION_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnCommand");
}

sptr<IRemoteObject> AbilityConnectionAbility::OnConnect(const Want &want)
{
    APP_LOGI("AbilityConnectionAbility::OnConnect");

    sptr<IRemoteObject> ret = BaseAbility::OnConnect(want);
    TestUtils::PublishEvent(
        APP_ABILITY_CONNECTION_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnConnect");
    return ret;
}

void AbilityConnectionAbility::OnDisconnect(const Want &want)
{
    APP_LOGI("AbilityConnectionAbility::OnDisconnect");

    BaseAbility::OnDisconnect(want);
    TestUtils::PublishEvent(
        APP_ABILITY_CONNECTION_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnDisconnect");
}

void AbilityConnectionAbility::TestConnectAbility()
{
    switch ((CaseIndex)std::stoi(AbilityConnectionAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            stub_ = new (std::nothrow) AbilityConnectionConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestConnectAbility");
            sleep(1);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::TWO: {
            stub_ = new (std::nothrow) AbilityConnectionConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestConnectAbility");
            sleep(1);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::THREE: {
            stub_ = new (std::nothrow) AbilityConnectionConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestConnectAbility");
            sleep(1);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                stub_ = new (std::nothrow) AbilityConnectionConnectCallback();
                connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
                bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
                TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestConnectAbility");
                sleep(1);
                BaseAbility::GetContext()->DisconnectAbility(connCallback_);
            }
        } break;
        case CaseIndex::FIVE: {
            sptr<AbilityConnectionConnectCallback> stub(new (std::nothrow) AbilityConnectionConnectCallback());
            sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
            bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestConnectAbility");
            sleep(1);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        default:
            break;
    }
}

void AbilityConnectionAbility::TestStopAbility()
{
    switch ((CaseIndex)std::stoi(AbilityConnectionAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            bool ret = BaseAbility::GetContext()->StopAbility(want_);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            bool ret = BaseAbility::GetContext()->StopAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            bool ret = BaseAbility::GetContext()->StopAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                bool ret = BaseAbility::GetContext()->StopAbility(want_);
                TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestStopAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            bool ret = BaseAbility::GetContext()->StopAbility(want);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestStopAbility");
        } break;
        default:
            break;
    }
}

void AbilityConnectionAbility::TestDisconnectAbility()
{
    switch ((CaseIndex)std::stoi(AbilityConnectionAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            stub_ = new (std::nothrow) AbilityConnectionConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            sleep(1);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            stub_ = new (std::nothrow) AbilityConnectionConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            sleep(1);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            stub_ = new (std::nothrow) AbilityConnectionConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            sleep(1);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                stub_ = new (std::nothrow) AbilityConnectionConnectCallback();
                connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
                bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
                TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
                sleep(1);
                BaseAbility::GetContext()->DisconnectAbility(connCallback_);
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            stub_ = new (std::nothrow) AbilityConnectionConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            sleep(1);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        default:
            break;
    }
}

void AbilityConnectionAbility::TestStartAbility()
{
    switch ((CaseIndex)std::stoi(AbilityConnectionAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            BaseAbility::GetContext()->StartAbility(want_, 0);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                BaseAbility::GetContext()->StartAbility(want_, 0);
                TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, 1, "TestStartAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, 1, "TestStartAbility");
        } break;
        default:
            break;
    }
}

void AbilityConnectionAbility::TestTerminateAbility()
{
    switch ((CaseIndex)std::stoi(AbilityConnectionAbility::sequenceNumber_)) {
        case CaseIndex::ONE: {
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                BaseAbility::GetContext()->TerminateAbility();
                TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, 1, "TestTerminateAbility");
            }
        } break;
        case CaseIndex::FIVE: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKitx", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_ABILITY_CONNECTION_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        default:
            break;
    }
}

void AbilityConnectionAbilityEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI(
        "AbilityConnectionAbilityEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("AbilityConnectionAbilityEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("AbilityConnectionAbilityEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());

    auto eventName = data.GetWant().GetAction();
    if (std::strcmp(eventName.c_str(), APP_ABILITY_CONNECTION_REQ_EVENT_NAME.c_str()) == 0) {
        auto target = data.GetData();
        auto func = mapTestFunc_.find(target);
        if (func != mapTestFunc_.end()) {
            func->second();
        } else {
            APP_LOGI("AbilityConnectionAbilityEventSubscriber::OnReceiveEvent: CommonEventData error(%{public}s)",
                target.c_str());
        }
    }
}
REGISTER_AA(AbilityConnectionAbility)
}  // namespace AppExecFwk
}  // namespace OHOS