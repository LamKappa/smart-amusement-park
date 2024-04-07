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

#ifndef LIFE_CYCLE_OBSERVER_ABILITY_H_
#define LIFE_CYCLE_OBSERVER_ABILITY_H_

#include "ability_connect_callback_proxy.h"
#include "ability_connect_callback_stub.h"
#include "ability_lifecycle_observer_interface.h"
#include "ability_loader.h"
#include "base_ability.h"
#include "common_event.h"
#include "common_event_manager.h"

namespace OHOS {
namespace AppExecFwk {
typedef std::map<std::string, std::string> MAP_STR_STR;
class LifeCycleObserverConnectCallback : public AAFwk::AbilityConnectionStub {
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
        if (resultCode == 0) {
            onAbilityConnectDoneCount--;
        }
    }

    static size_t onAbilityConnectDoneCount;
};
size_t LifeCycleObserverConnectCallback::onAbilityConnectDoneCount = 0;

class LifecycleObserverLifecycleCallbacks : public AbilityLifecycleCallbacks {
public:
    LifecycleObserverLifecycleCallbacks() = default;
    virtual ~LifecycleObserverLifecycleCallbacks() = default;

    virtual void OnAbilityStart(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityInactive(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityBackground(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityForeground(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityActive(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityStop(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilitySaveState(const PacMap &outState) override;
};

class LifecycleObserverLifecycleObserver : public ILifecycleObserver {
public:
    LifecycleObserverLifecycleObserver() = default;
    virtual ~LifecycleObserverLifecycleObserver() = default;
    void OnActive() override;
    void OnBackground() override;
    void OnForeground(const Want &want) override;
    void OnInactive() override;
    void OnStart(const Want &want) override;
    void OnStop() override;
    void OnStateChanged(LifeCycle::Event event, const Want &want) override;
    void OnStateChanged(LifeCycle::Event event) override;

    void DoTask();
};

class LifeCycleObserverAbilityEventSubscriber;
class LifeCycleObserverAbility : public BaseAbility {
public:
    ~LifeCycleObserverAbility();

    static int sequenceNumber_;
    void TestConnectAbility();
    void TestStopAbility();
    void TestDisconnectAbility();
    void TestStartAbility();
    void TestTerminateAbility();

    void TestLifeCycleGetLifecycle();
    void TestLifeCycleGetLifecycleState();
    void TestLifeCycleAddObserver();
    void TestLifeCycleDispatchLifecycle();
    void TestLifeCycleDispatchLifecycleOne();
    void TestLifeCycleDispatchLifecycleTwo();
    void TestLifeCycleRemoveObserver();

protected:
    void Init(const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<OHOSApplication> &application,
        std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token) override;
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnForeground(const Want &want) override;
    virtual void OnCommand(const AAFwk::Want &want, bool restart, int startId) override;
    virtual sptr<IRemoteObject> OnConnect(const Want &want) override;
    virtual void OnDisconnect(const Want &want) override;

    bool SubscribeEvent();

private:
    Want want_ = {};
    sptr<LifeCycleObserverConnectCallback> stub_ = {};
    sptr<AAFwk::AbilityConnectionProxy> connCallback_ = {};

    std::shared_ptr<LifecycleObserverLifecycleCallbacks> lifecycleCallbacks_ = {};
    std::shared_ptr<LifecycleObserverLifecycleObserver> lifecycleObserver_ = {};
    std::shared_ptr<LifeCycleObserverAbilityEventSubscriber> subscriber_ = {};
};
int LifeCycleObserverAbility::sequenceNumber_ = 0;

class LifeCycleObserverAbilityEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    LifeCycleObserverAbilityEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp)
        : EventFwk::CommonEventSubscriber(sp)
    {
        mapTestFunc_ = {
            {"ConnectAbility", [this]() { TestConnectAbility(); }},
            {"StopAbility", [this]() { TestStopAbility(); }},
            {"DisconnectAbility", [this]() { TestDisconnectAbility(); }},
            {"StartAbility", [this]() { TestStartAbility(); }},
            {"TerminateAbility", [this]() { TestTerminateAbility(); }},

            {"LifeCycleGetLifecycle", [this]() { TestLifeCycleGetLifecycle(); }},
            {"LifeCycleGetLifecycleState", [this]() { TestLifeCycleGetLifecycleState(); }},
            {"LifeCycleAddObserver", [this]() { TestLifeCycleAddObserver(); }},
            {"LifeCycleDispatchLifecycle", [this]() { TestLifeCycleDispatchLifecycle(); }},
            {"LifeCycleRemoveObserver", [this]() { TestLifeCycleRemoveObserver(); }},
        };
    }
    ~LifeCycleObserverAbilityEventSubscriber() = default;
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data);

    void TestConnectAbility()
    {
        mainAbility.TestConnectAbility();
    }

    void TestStopAbility()
    {
        mainAbility.TestStopAbility();
    }

    void TestDisconnectAbility()
    {
        mainAbility.TestDisconnectAbility();
    }

    void TestStartAbility()
    {
        mainAbility.TestStartAbility();
    }

    void TestTerminateAbility()
    {
        mainAbility.TestTerminateAbility();
    }

    void TestLifeCycleGetLifecycle()
    {
        mainAbility.TestLifeCycleGetLifecycle();
    }

    void TestLifeCycleGetLifecycleState()
    {
        mainAbility.TestLifeCycleGetLifecycleState();
    }

    void TestLifeCycleAddObserver()
    {
        mainAbility.TestLifeCycleAddObserver();
    }

    void TestLifeCycleDispatchLifecycle()
    {
        mainAbility.TestLifeCycleDispatchLifecycle();
    }

    void TestLifeCycleRemoveObserver()
    {
        mainAbility.TestLifeCycleRemoveObserver();
    }

    Want want = {};
    LifeCycleObserverAbility mainAbility = {};
    std::unordered_map<std::string, std::function<void()>> mapTestFunc_ = {};
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // LIFE_CYCLE_OBSERVER_ABILITY_H_