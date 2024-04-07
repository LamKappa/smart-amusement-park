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
#ifndef _AMS_ST_PAGE_ABILITY_EVENT_H_
#define _AMS_ST_PAGE_ABILITY_EVENT_H_

#include <vector>
#include <string>
#include <memory>
#include "common_event.h"
#include "common_event_manager.h"
#include "ability_loader.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

namespace STEventName {
const std::string g_eventName = "resp_st_page_ability_callback";
const std::string g_pidEventName = "resp_st_page_ability_pid_callback";
const std::string g_abilityStateOnStart = ":OnStart";
const std::string g_abilityStateOnStop = ":OnStop";
const std::string g_abilityStateOnActive = ":OnActive";
const std::string g_abilityStateOnInactive = ":OnInactive";
const std::string g_abilityStateOnBackground = ":OnBackground";
const std::string g_abilityStateOnForeground = ":OnForeground";
const std::string g_abilityStateOnNewWant = ":OnNewWant";
const int eventCode = 0;
const std::vector<std::string> g_eventList = {"requ_page_ability_terminate"};
}  // namespace STEventName

class STPageAbilityEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    STPageAbilityEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp, const std::shared_ptr<Ability> &ability)
        : CommonEventSubscriber(sp), ability_(std::move(ability))
    {}
    ~STPageAbilityEventSubscriber()
    {}
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data) override;

private:
    std::shared_ptr<Ability> ability_;
};

class STPageAbilityEvent {
public:
    STPageAbilityEvent() = default;
    STPageAbilityEvent(const std::string &className);
    ~STPageAbilityEvent() = default;

    bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);
    void SubscribeEvent(std::vector<std::string> eventList, const std::shared_ptr<Ability> &ability);
    void UnsubscribeEvent();
    std::string GetEventDate(const std::string &stateCallbackCount);
    std::string GetCallBackPath(const std::string &callBackPath);
    std::string GetAbilityStatus(const std::string &abilityStatus);

    int GetOnStartCount();
    int GetOnStopCount();
    int GetOnActiveCount();
    int GetOnInactiveCount();
    int GetOnBackgroundCount();
    int GetOnForegroundCount();
    int GetOnNewWantCount();

private:
    std::shared_ptr<STPageAbilityEventSubscriber> subscriber_;
    int onStartCount_ = 0;
    int onStopCount_ = 0;
    int onActiveCount_ = 0;
    int onInactiveCount_ = 0;
    int onBackgroundCount_ = 0;
    int onForegroundCount_ = 0;
    int onNewWantCount_ = 0;
    std::string className_;
    std::string callBackPath_;
    std::string abilityStatus_;
};

}  // namespace AppExecFwk
}  // namespace OHOS

#endif  //_AMS_ST_PAGE_ABILITY_EVENT_H_