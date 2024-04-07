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
#ifndef AMS_ST_KIT_DATA_ABILITY_SERVICE_B_H
#define AMS_ST_KIT_DATA_ABILITY_SERVICE_B_H
#include <string>
#include <vector>

#include "ability.h"
#include "ability_loader.h"
#include "common_event_manager.h"
#include "event.h"
#include "skills.h"
#include "stoperator.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

const std::string abilityEventName = "event_data_ability_callback";
const std::string testEventName = "event_data_test_action";
class KitTestServiceBEventSubscriber;

class AmsStKitDataAbilityServiceB : public Ability {
public:
    void SubscribeEvent(const Want &want);
    bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);
    ~AmsStKitDataAbilityServiceB();

protected:
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnNewWant(const Want &want) override;
    virtual void OnCommand(const AAFwk::Want &want, bool restart, int startId) override;
    virtual sptr<IRemoteObject> OnConnect(const Want &want) override;
    virtual void OnDisconnect(const Want &want) override;

private:
    void GetWantInfo(const Want &want);

    Want originWant_;
    std::shared_ptr<KitTestServiceBEventSubscriber> subscriber_;
};

class KitTestServiceBEventSubscriber : public CommonEventSubscriber {
public:
    KitTestServiceBEventSubscriber(const CommonEventSubscribeInfo &sp, AmsStKitDataAbilityServiceB *ability)
        : CommonEventSubscriber(sp)
    {
        mapTestFunc_ = {{"OnStart", [this]() { TestPost("OnStart"); }},
            {"OnStop", [this]() { TestPost("OnStop"); }},
            {"OnActive", [this]() { TestPost("OnActive"); }},
            {"OnInactive", [this]() { TestPost("OnInactive"); }},
            {"OnBackground", [this]() { TestPost("OnBackground"); }}};
        mainAbility_ = ability;
    };
    virtual void OnReceiveEvent(const CommonEventData &data);

    void TestPost(const std::string funName);
    std::vector<std::string> vectorOperator_;
    AmsStKitDataAbilityServiceB *mainAbility_;
    std::unordered_map<std::string, std::function<void()>> mapTestFunc_;
    ~KitTestServiceBEventSubscriber(){};
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // AMS_ST_KIT_DATA_ABILITY_SERVICE_B_H