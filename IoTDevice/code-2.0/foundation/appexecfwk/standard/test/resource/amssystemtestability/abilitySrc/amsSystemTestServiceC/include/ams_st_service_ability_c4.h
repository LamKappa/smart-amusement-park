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

#ifndef AMS_ST_SERVICE_ABILITY_C4_
#define AMS_ST_SERVICE_ABILITY_C4_
#include <string>
#include "ability_loader.h"
#include "common_event.h"
#include "common_event_manager.h"

namespace OHOS {
namespace AppExecFwk {
class AmsStServiceAbilityC4 : public Ability {
public:
    ~AmsStServiceAbilityC4();

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
    void Clear();
    void GetWantInfo(const Want &want);
    bool SubscribeEvent();
    bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);
    void DisConnectOtherAbility();
    void StopSelfAbility();

    std::string shouldReturn_ = {};
    std::string targetBundle_ = {};
    std::string targetAbility_ = {};

    typedef void (AmsStServiceAbilityC4::*func)();
    static std::map<std::string, func> funcMap_;
    class AppEventSubscriber;
    std::shared_ptr<AppEventSubscriber> subscriber_ = {};

    class AppEventSubscriber : public EventFwk::CommonEventSubscriber {
    public:
        AppEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp){};
        ~AppEventSubscriber() = default;
        virtual void OnReceiveEvent(const EventFwk::CommonEventData &data) override;

        AmsStServiceAbilityC4 *mainAbility_ = nullptr;
    };
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // AMS_ST_SERVICE_ABILITY_C4_