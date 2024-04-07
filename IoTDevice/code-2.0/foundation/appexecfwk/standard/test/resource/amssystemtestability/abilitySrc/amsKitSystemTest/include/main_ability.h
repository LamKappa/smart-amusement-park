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

#ifndef _AMS_KIT_SYSTEM_TEST_MAIN_ABILITY_H_
#define _AMS_KIT_SYSTEM_TEST_MAIN_ABILITY_H_
#include "ability.h"
#include "ability_loader.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "kit_test_common_info.h"

namespace OHOS {
namespace AppExecFwk {

class FirstEventSubscriber;
class MainAbility : public Ability {
public:
    MainAbility()
    {
        mapCase_ = {
            {(int)AbilityApi::GetAbilityName,
                {
                    [this](int code) { GetAbilityNameCase1(code); },
                }},
            {(int)AbilityApi::GetAbilityPackage,
                {
                    [this](int code) { GetAbilityPackageCase1(code); },
                }},
            {(int)AbilityApi::GetWant,
                {
                    [this](int code) { GetWantCase1(code); },
                    [this](int code) { GetWantCase2(code); },
                    [this](int code) { GetWantCase3(code); },
                }},
            {(int)AbilityApi::GetWindow,
                {
                    [this](int code) { GetWindowCase1(code); },
                }},
            {(int)AbilityApi::Dump,
                {
                    [this](int code) { DumpCase1(code); },
                }},
            {(int)AbilityApi::SetWant,
                {
                    [this](int code) { SetWantCase1(code); },
                    [this](int code) { SetWantCase2(code); },
                }},
        };
    }
    ~MainAbility() = default;

    void SubscribeEvent();
    void TestAbility(int apiIndex, int caseIndex, int code);
    void GetAbilityNameCase1(int code);
    void GetAbilityPackageCase1(int code);
    void GetWantCase1(int code);
    void GetWantCase2(int code);
    void GetWantCase3(int code);
    void GetWindowCase1(int code);
    void DumpCase1(int code);
    void SetWantCase1(int code);
    void SetWantCase2(int code);

    std::unordered_map<int, std::vector<std::function<void(int)>>> mapCase_;
    std::shared_ptr<FirstEventSubscriber> subscriber;

protected:
    void Init(const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<OHOSApplication> &application,
        std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token) override;
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnForeground(const Want &want) override;
    virtual void OnAbilityResult(int requestCode, int resultCode, const Want &resultData) override;
    virtual void OnBackPressed() override;
    virtual void OnNewWant(const Want &want) override;
};
class FirstEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    FirstEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp, MainAbility *ability) : CommonEventSubscriber(sp)
    {
        mapTestFunc_ = {
            {"Ability", [this](int apiIndex, int caseIndex, int code) { TestAbility(apiIndex, caseIndex, code); }},
        };
        mainAbility = ability;
    }
    ~FirstEventSubscriber() = default;

    void TestAbility(int apiIndex, int caseIndex, int code)
    {
        mainAbility->TestAbility(apiIndex, caseIndex, code);
    }

    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data);

    MainAbility *mainAbility;
    std::unordered_map<std::string, std::function<void(int, int, int)>> mapTestFunc_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // _AMS_KIT_SYSTEM_TEST_MAIN_ABILITY_H_