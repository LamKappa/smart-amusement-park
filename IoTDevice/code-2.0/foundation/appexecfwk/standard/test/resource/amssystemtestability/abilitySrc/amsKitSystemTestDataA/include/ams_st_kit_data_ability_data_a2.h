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
#ifndef AMS_ST_KIT_DATA_ABILITY_DATA_A2_H
#define AMS_ST_KIT_DATA_ABILITY_DATA_A2_H
#include <string>

#include "ability.h"
#include "ability_lifecycle_callbacks.h"
#include "ability_lifecycle_observer_interface.h"
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

class KitTestDataA2EventSubscriber;
class AmsStKitDataAbilityDataA2 : public Ability {
public:
    void SubscribeEvent(const Want &want);
    bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);
    STtools::Event event;
    void TestLifeCycle();
    ~AmsStKitDataAbilityDataA2();

protected:
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnForeground(const Want &want) override;
    virtual void OnBackground() override;
    virtual void OnNewWant(const Want &want) override;

    void Init(const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<OHOSApplication> &application,
        std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token) override;
    virtual int Insert(const Uri &uri, const ValuesBucket &value) override;
    virtual int Delete(const Uri &uri, const DataAbilityPredicates &predicates) override;
    virtual int Update(const Uri &uri, const ValuesBucket &value, const DataAbilityPredicates &predicates) override;
    virtual std::shared_ptr<ResultSet> Query(
        const Uri &uri, const std::vector<std::string> &columns, const DataAbilityPredicates &predicates) override;
    virtual std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter) override;
    virtual int OpenFile(const Uri &uri, const std::string &mode) override;

private:
    Want originWant_;
    std::shared_ptr<KitTestDataA2EventSubscriber> subscriber_;
};

class AmsStKitDataAbilityDataA2LifecycleCallbacks : public AbilityLifecycleCallbacks {
public:
    AmsStKitDataAbilityDataA2LifecycleCallbacks() = default;
    virtual ~AmsStKitDataAbilityDataA2LifecycleCallbacks() = default;

    virtual void OnAbilityStart(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityInactive(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityBackground(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityForeground(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityActive(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilityStop(const std::shared_ptr<Ability> &ability) override;
    virtual void OnAbilitySaveState(const PacMap &outState) override;
    AmsStKitDataAbilityDataA2 *mainAbility_;
};

class AmsStKitDataAbilityDataA2LifecycleObserver : public ILifecycleObserver {
public:
    AmsStKitDataAbilityDataA2LifecycleObserver() = default;
    virtual ~AmsStKitDataAbilityDataA2LifecycleObserver() = default;
    void OnActive() override;
    void OnBackground() override;
    void OnForeground(const Want &want) override;
    void OnInactive() override;
    void OnStart(const Want &want) override;
    void OnStop() override;
    void OnStateChanged(LifeCycle::Event event, const Want &want) override;
    void OnStateChanged(LifeCycle::Event event) override;
    AmsStKitDataAbilityDataA2 *mainAbility_;
};

class KitTestDataA2EventSubscriber : public CommonEventSubscriber {
public:
    explicit KitTestDataA2EventSubscriber(const CommonEventSubscribeInfo &sp, AmsStKitDataAbilityDataA2 *ability)
        : CommonEventSubscriber(sp)
    {
        mainAbility_ = ability;
    };
    virtual void OnReceiveEvent(const CommonEventData &data);

    void TestPost(const std::string funName = "");
    std::vector<std::string> vectorOperator_;
    AmsStKitDataAbilityDataA2 *mainAbility_;
    ~KitTestDataA2EventSubscriber(){};
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // AMS_ST_KIT_DATA_ABILITY_DATA_A2_H