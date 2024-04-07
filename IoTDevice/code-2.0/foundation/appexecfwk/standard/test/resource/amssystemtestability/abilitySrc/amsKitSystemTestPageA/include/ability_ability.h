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

#ifndef ABILITY_ABILITY_H_
#define ABILITY_ABILITY_H_

#include "ability_connect_callback_proxy.h"
#include "ability_connect_callback_stub.h"
#include "ability_loader.h"
#include "base_ability.h"
#include "common_event.h"
#include "common_event_manager.h"

namespace OHOS {
namespace AppExecFwk {
typedef std::map<std::string, std::string> MAP_STR_STR;
class AbilityConnectCallback : public AAFwk::AbilityConnectionStub {
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
size_t AbilityConnectCallback::onAbilityConnectDoneCount = 0;

class AbilityAbilityEventSubscriber;
class AbilityAbility : public BaseAbility {
public:
    ~AbilityAbility();

    static std::string sequenceNumber_;
    void TestConnectAbility();
    void TestStopAbility();
    void TestDisconnectAbility();
    void TestStartAbility();
    void TestTerminateAbility();
    void TestStartAbilityWithSetting();
    void TestStartAbilityForResult();

    void TestAbilityStartAbility();
    void TestAbilityConnectAbility();
    void TestAbilityStopAbility();
    void TestAbilityGetLifecycle();
    void TestAbilityDisconnectAbility();

    bool SubscribeEvent();
    void DoTestCase();
    void StopSelfAbility();

protected:
    virtual void Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
        const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
        const sptr<IRemoteObject> &token) override;
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnForeground(const Want &want) override;
    virtual void OnCommand(const AAFwk::Want &want, bool restart, int startId) override;
    virtual sptr<IRemoteObject> OnConnect(const Want &want) override;
    virtual void OnDisconnect(const Want &want) override;
    virtual void OnNewWant(const Want &want) override;
    virtual void OnAbilityResult(int requestCode, int resultCode, const Want &resultData) override;

private:
    Want want_ = {};
    sptr<AbilityConnectCallback> stub_ = {};
    sptr<AAFwk::AbilityConnectionProxy> connCallback_ = {};
    std::shared_ptr<AbilityAbilityEventSubscriber> subscriber_ = {};
};
std::string AbilityAbility::sequenceNumber_ = "0";

class AbilityAbilityEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    AbilityAbilityEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp) : EventFwk::CommonEventSubscriber(sp)
    {
        mapTestFunc_ = {
            {"ConnectAbility", [this]() { TestConnectAbility(); }},
            {"StopAbility", [this]() { TestStopAbility(); }},
            {"DisconnectAbility", [this]() { TestDisconnectAbility(); }},
            {"StartAbility", [this]() { TestStartAbility(); }},
            {"TerminateAbility", [this]() { TestTerminateAbility(); }},
            {"StartAbilityWithSetting", [this]() { TestStartAbilityWithSetting(); }},
            {"StartAbilityForResult", [this]() { TestStartAbilityForResult(); }},

            {"AbilityStartAbility", [this]() { TestAbilityStartAbility(); }},
            {"AbilityConnectAbility", [this]() { TestAbilityConnectAbility(); }},
            {"AbilityStopAbility", [this]() { TestAbilityStopAbility(); }},
            {"AbilityGetLifecycle", [this]() { TestAbilityGetLifecycle(); }},
            {"AbilityDisconnectAbility", [this]() { TestAbilityDisconnectAbility(); }},
            {"StopSelfAbility", [this]() { StopSelfAbility(); }},
        };
    }
    ~AbilityAbilityEventSubscriber() = default;
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

    void TestAbilityStartAbility()
    {
        mainAbility.TestAbilityStartAbility();
    }

    void TestStartAbilityWithSetting()
    {
        mainAbility.TestStartAbilityWithSetting();
    }

    void TestStartAbilityForResult()
    {
        mainAbility.TestStartAbilityForResult();
    }

    void TestAbilityConnectAbility()
    {
        mainAbility.TestAbilityConnectAbility();
    }

    void TestAbilityStopAbility()
    {
        mainAbility.TestAbilityStopAbility();
    }

    void TestAbilityGetLifecycle()
    {
        mainAbility.TestAbilityGetLifecycle();
    }

    void TestAbilityDisconnectAbility()
    {
        mainAbility.TestAbilityDisconnectAbility();
    }

    void StopSelfAbility()
    {
        mainAbility.StopSelfAbility();
    }

    Want want = {};
    AbilityAbility mainAbility = {};
    std::unordered_map<std::string, std::function<void()>> mapTestFunc_ = {};
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // ABILITY_ABILITY_H_