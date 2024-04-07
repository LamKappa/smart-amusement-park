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

#ifndef BASE_ABILITY_H_
#define BASE_ABILITY_H_
#include "ability.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "event_handler.h"

namespace OHOS {
namespace AppExecFwk {
constexpr int MAX_LOOP = 10;
template<class T, class G>
void SubscribeEvent(const Want &want, const std::vector<std::string> &eventList, const G &ability)
{
    EventFwk::MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    auto subscriber = std::make_shared<T>(subscribeInfo);
    subscriber->want = want;
    subscriber->mainAbility = ability;
    EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber);
}

template<class T>
void DoWhile()
{
    long cnt = 0;
    while (cnt <= MAX_LOOP) {
        cnt++;
    }
}

template<class T>
void PostTask()
{
    auto eventHandler = EventHandler::Current();
    if (!eventHandler) {
        return;
    }
    eventHandler->PostTask([=]() { DoWhile<T>(); });
}

class BaseAbility : public Ability {
public:
    using EventHandlerPtr = std::shared_ptr<EventHandler>;

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

    std::string GetNoFromWantInfo(const Want &want);

private:
    EventHandlerPtr envenHandler_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // BASE_ABILITY_H_
