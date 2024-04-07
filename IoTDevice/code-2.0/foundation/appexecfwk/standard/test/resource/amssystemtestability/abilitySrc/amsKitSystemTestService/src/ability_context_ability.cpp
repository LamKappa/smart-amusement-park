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

#include "ability_context_ability.h"
#include "app_log_wrapper.h"
#include "base_ability.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

AbilityContextAbility::~AbilityContextAbility()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void AbilityContextAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("AbilityContextAbility::Init called.");
    BaseAbility::Init(abilityInfo, application, handler, token);

    SubscribeEvent();
}

bool AbilityContextAbility::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(APP_ABILITY_CONTEXT_REQ_EVENT_NAME);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AbilityContextAbilityEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility = *this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void AbilityContextAbility::OnStart(const Want &want)
{
    want_ = want;
    sequenceNumber_ = std::stoi(GetNoFromWantInfo(want));
    APP_LOGI("AbilityContextAbility::OnStart");

    BaseAbility::OnStart(want);
    TestUtils::PublishEvent(
        APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnStart");
}

void AbilityContextAbility::OnStop()
{
    APP_LOGI("AbilityContextAbility::OnStop");
    BaseAbility::OnStop();
    TestUtils::PublishEvent(
        APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INITIAL, "OnStop");
}

void AbilityContextAbility::OnActive()
{
    APP_LOGI("AbilityContextAbility::OnActive");
    BaseAbility::OnActive();
    TestUtils::PublishEvent(
        APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnActive");
}

void AbilityContextAbility::OnInactive()
{
    APP_LOGI("AbilityContextAbility::OnInactive");
    BaseAbility::OnInactive();
    TestUtils::PublishEvent(
        APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnInactive");
}

void AbilityContextAbility::OnBackground()
{
    APP_LOGI("AbilityContextAbility::OnBackground");
    BaseAbility::OnBackground();
    TestUtils::PublishEvent(
        APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnBackground");
}

void AbilityContextAbility::OnForeground(const Want &want)
{
    APP_LOGI("AbilityContextAbility::OnForeground");
    BaseAbility::OnBackground();
    TestUtils::PublishEvent(
        APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnForeground");
}

void AbilityContextAbility::OnCommand(const Want &want, bool restart, int startId)
{
    APP_LOGI("AbilityContextAbility::OnCommand");

    BaseAbility::OnCommand(want, restart, startId);
    TestUtils::PublishEvent(
        APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnCommand");
}

sptr<IRemoteObject> AbilityContextAbility::OnConnect(const Want &want)
{
    APP_LOGI("AbilityContextAbility::OnConnect");

    sptr<IRemoteObject> ret = BaseAbility::OnConnect(want);
    TestUtils::PublishEvent(
        APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnConnect");
    return ret;
}

void AbilityContextAbility::OnDisconnect(const Want &want)
{
    APP_LOGI("AbilityContextAbility::OnDisconnect");

    BaseAbility::OnDisconnect(want);
    TestUtils::PublishEvent(
        APP_ABILITY_CONTEXT_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnDisconnect");
}

void AbilityContextAbility::TestConnectAbility()
{
    switch ((CaseIndex)AbilityContextAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            stub_ = new (std::nothrow) AbilityContextConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestConnectAbility");
            sleep(2);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::TWO: {
            stub_ = new (std::nothrow) AbilityContextConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestConnectAbility");
            sleep(1);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::THREE: {
            stub_ = new (std::nothrow) AbilityContextConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestConnectAbility");
            sleep(1);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                stub_ = new (std::nothrow) AbilityContextConnectCallback();
                connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
                bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
                TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestConnectAbility");
                sleep(1);
                BaseAbility::GetContext()->DisconnectAbility(connCallback_);
            }
        } break;
        case CaseIndex::FIVE: {
            sptr<AbilityContextConnectCallback> stub(new (std::nothrow) AbilityContextConnectCallback());
            sptr<AbilityConnectionProxy> connCallback(new (std::nothrow) AbilityConnectionProxy(stub));
            bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestConnectAbility");
            sleep(1);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        default:
            break;
    }
}

void AbilityContextAbility::TestStopAbility()
{
    switch ((CaseIndex)AbilityContextAbility::sequenceNumber_) {
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
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
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

void AbilityContextAbility::TestDisconnectAbility()
{
    switch ((CaseIndex)AbilityContextAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            stub_ = new (std::nothrow) AbilityContextConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want_, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            stub_ = new (std::nothrow) AbilityContextConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            sleep(2);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            stub_ = new (std::nothrow) AbilityContextConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            sleep(2);
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        case CaseIndex::FOUR: {
            for (int i = 0; i < (int)CaseIndex::THREE; i++) {
                stub_ = new (std::nothrow) AbilityContextConnectCallback();
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
            stub_ = new (std::nothrow) AbilityContextConnectCallback();
            connCallback_ = new (std::nothrow) AbilityConnectionProxy(stub_);
            bool ret = BaseAbility::GetContext()->ConnectAbility(want, connCallback_);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, ret, "TestDisconnectAbility");
            BaseAbility::GetContext()->DisconnectAbility(connCallback_);
        } break;
        default:
            break;
    }
}

void AbilityContextAbility::TestStartAbility()
{
    switch ((CaseIndex)AbilityContextAbility::sequenceNumber_) {
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
            sleep(2);
            BaseAbility::GetContext()->StopAbility(want);
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestStartAbility");
            sleep(2);
            BaseAbility::GetContext()->StopAbility(want);
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

void AbilityContextAbility::TestTerminateAbility()
{
    switch ((CaseIndex)AbilityContextAbility::sequenceNumber_) {
        case CaseIndex::ONE: {
            BaseAbility::GetContext()->TerminateAbility();
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestTerminateAbility");
        } break;
        case CaseIndex::TWO: {
            MAP_STR_STR params;
            Want want =
                TestUtils::MakeWant("device", "LifecycleCallbacksAbility", "com.ohos.amsst.service.AppKit", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestTerminateAbility");
            sleep(2);
            BaseAbility::GetContext()->StopAbility(want);
            BaseAbility::GetContext()->TerminateAbility();
        } break;
        case CaseIndex::THREE: {
            MAP_STR_STR params;
            Want want = TestUtils::MakeWant("device", "AmsStServiceAbilityA1", "com.ohos.amsst.service.appA", params);
            BaseAbility::GetContext()->StartAbility(want, 0);
            TestUtils::PublishEvent(APP_ABILITY_CONTEXT_RESP_EVENT_NAME, 1, "TestTerminateAbility");
            sleep(2);
            BaseAbility::GetContext()->StopAbility(want);
            BaseAbility::GetContext()->TerminateAbility();
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

void AbilityContextAbilityEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI(
        "AbilityContextAbilityEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("AbilityContextAbilityEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("AbilityContextAbilityEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());

    auto eventName = data.GetWant().GetAction();
    if (std::strcmp(eventName.c_str(), APP_ABILITY_CONTEXT_REQ_EVENT_NAME.c_str()) == 0) {
        auto target = data.GetData();
        auto func = mapTestFunc_.find(target);
        if (func != mapTestFunc_.end()) {
            func->second();
        } else {
            APP_LOGI("AbilityContextAbilityEventSubscriber::OnReceiveEvent: CommonEventData error(%{public}s)",
                target.c_str());
        }
    }
}
REGISTER_AA(AbilityContextAbility)
}  // namespace AppExecFwk
}  // namespace OHOS