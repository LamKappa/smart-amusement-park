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

#ifndef AMS_KIT_SYSTEM_TEST_A_SECOND_ABILITY_H
#define AMS_KIT_SYSTEM_TEST_A_SECOND_ABILITY_H
#include "ability.h"
#include "ability_loader.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "kit_test_common_info.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;
class SecondEventSubscriber;
class SecondAbility : public Ability {
public:
    void SubscribeEvent();
    void TestAbility(int apiIndex, int caseIndex, int code);

    void GetApplicationInfoCase1(int code);
    void GetApplicationInfoCase2(int code);
    void GetApplicationInfoCase3(int code);

    void GetCacheDirCase1(int code);
    void GetCacheDirCase2(int code);
    void GetCacheDirCase3(int code);

    void GetDatabaseDirCase1(int code);
    void GetDatabaseDirCase2(int code);
    void GetDatabaseDirCase3(int code);

    void GetDataDirCase1(int code);
    void GetDataDirCase2(int code);
    void GetDataDirCase3(int code);

    void GetDirCase1(int code);
    void GetDirCase2(int code);
    void GetDirCase3(int code);

    void GetNoBackupFilesDirCase1(int code);

    void GetBundleManagerCase1(int code);

    void VerifyCallingPermissionCase1(int code);

    void VerifyPermissionCase1(int code);

    void VerifySelfPermissionCase1(int code);

    void GetBundleCodePathCase1(int code);
    void GetBundleCodePathCase2(int code);
    void GetBundleCodePathCase3(int code);

    void GetBundleNameCase1(int code);
    void GetBundleNameCase2(int code);
    void GetBundleNameCase3(int code);

    void GetBundleResourcePathCase1(int code);
    void GetBundleResourcePathCase2(int code);
    void GetBundleResourcePathCase3(int code);

    void CanRequestPermissionCase1(int code);

    void GetCallingAbilityCase1(int code);
    void GetCallingAbilityCase2(int code);
    void GetCallingAbilityCase3(int code);

    void GetContextCase1(int code);

    void GetAbilityManagerCase1(int code);

    void GetProcessInfoCase1(int code);
    void GetProcessInfoCase2(int code);
    void GetProcessInfoCase3(int code);

    void GetAppTypeCase1(int code);
    void GetAppTypeCase2(int code);
    void GetAppTypeCase3(int code);

    void GetCallingBundleCase1(int code);
    void GetCallingBundleCase2(int code);
    void GetCallingBundleCase3(int code);

    void StartAbilityCase1(int code);
    void StartAbilityCase2(int code);

    void TerminateAbilityCase1(int code);

    void GetElementNameCase1(int code);
    void GetElementNameCase2(int code);
    void GetElementNameCase3(int code);

    void GetHapModuleInfoCase1(int code);
    void GetHapModuleInfoCase2(int code);
    void GetHapModuleInfoCase3(int code);

    void GetCodeCacheDirCase1(int code);
    void GetCodeCacheDirCase2(int code);
    void GetCodeCacheDirCase3(int code);

    void GetApplicationContextCase1(int code);

    SecondAbility()
    {
        mapCase_ = {
            {(int)AbilityContextApi::GetApplicationInfo,
                {
                    [this](int code) { GetApplicationInfoCase1(code); },
                    [this](int code) { GetApplicationInfoCase2(code); },
                    [this](int code) { GetApplicationInfoCase3(code); },
                }},
            {(int)AbilityContextApi::GetCacheDir,
                {
                    [this](int code) { GetCacheDirCase1(code); },
                    [this](int code) { GetCacheDirCase2(code); },
                    [this](int code) { GetCacheDirCase3(code); },
                }},
            {(int)AbilityContextApi::GetCodeCacheDir,
                {
                    [this](int code) { GetCodeCacheDirCase1(code); },
                    [this](int code) { GetCodeCacheDirCase2(code); },
                    [this](int code) { GetCodeCacheDirCase3(code); },
                }},
            {(int)AbilityContextApi::GetDatabaseDir,
                {
                    [this](int code) { GetDatabaseDirCase1(code); },
                    [this](int code) { GetDatabaseDirCase2(code); },
                    [this](int code) { GetDatabaseDirCase3(code); },
                }},
            {(int)AbilityContextApi::GetDataDir,
                {
                    [this](int code) { GetDataDirCase1(code); },
                    [this](int code) { GetDataDirCase2(code); },
                    [this](int code) { GetDataDirCase3(code); },
                }},
            {(int)AbilityContextApi::GetDir,
                {
                    [this](int code) { GetDirCase1(code); },
                    [this](int code) { GetDirCase2(code); },
                    [this](int code) { GetDirCase3(code); },
                }},
            {(int)AbilityContextApi::GetBundleManager,
                {
                    [this](int code) { GetBundleManagerCase1(code); },
                }},
            {(int)AbilityContextApi::GetBundleCodePath,
                {
                    [this](int code) { GetBundleCodePathCase1(code); },
                    [this](int code) { GetBundleCodePathCase2(code); },
                    [this](int code) { GetBundleCodePathCase3(code); },
                }},
            {(int)AbilityContextApi::GetBundleName,
                {
                    [this](int code) { GetBundleNameCase1(code); },
                    [this](int code) { GetBundleNameCase2(code); },
                    [this](int code) { GetBundleNameCase3(code); },
                }},
            {(int)AbilityContextApi::GetBundleResourcePath,
                {
                    [this](int code) { GetBundleResourcePathCase1(code); },
                    [this](int code) { GetBundleResourcePathCase2(code); },
                    [this](int code) { GetBundleResourcePathCase3(code); },
                }},
            {(int)AbilityContextApi::GetApplicationContext,
                {
                    [this](int code) { GetApplicationContextCase1(code); },
                }},
            {(int)AbilityContextApi::GetCallingAbility,
                {
                    [this](int code) { GetCallingAbilityCase1(code); },
                    [this](int code) { GetCallingAbilityCase2(code); },
                    [this](int code) { GetCallingAbilityCase3(code); },
                }},
            {(int)AbilityContextApi::GetContext,
                {
                    [this](int code) { GetContextCase1(code); },
                }},
            {(int)AbilityContextApi::GetAbilityManager,
                {
                    [this](int code) { GetAbilityManagerCase1(code); },
                }},
            {(int)AbilityContextApi::GetProcessInfo,
                {
                    [this](int code) { GetProcessInfoCase1(code); },
                    [this](int code) { GetProcessInfoCase2(code); },
                    [this](int code) { GetProcessInfoCase3(code); },
                }},
            {(int)AbilityContextApi::GetAppType,
                {
                    [this](int code) { GetAppTypeCase1(code); },
                    [this](int code) { GetAppTypeCase2(code); },
                    [this](int code) { GetAppTypeCase3(code); },
                }},
            {(int)AbilityContextApi::GetCallingBundle,
                {
                    [this](int code) { GetCallingBundleCase1(code); },
                    [this](int code) { GetCallingBundleCase2(code); },
                    [this](int code) { GetCallingBundleCase3(code); },
                }},
            {(int)AbilityContextApi::StartAbility_Want_int,
                {
                    [this](int code) { StartAbilityCase1(code); },
                    [this](int code) { StartAbilityCase2(code); },
                }},
            {(int)AbilityContextApi::TerminateAbility,
                {
                    [this](int code) { TerminateAbilityCase1(code); },
                }},
            {(int)AbilityContextApi::GetElementName,
                {
                    [this](int code) { GetElementNameCase1(code); },
                    [this](int code) { GetElementNameCase2(code); },
                    [this](int code) { GetElementNameCase3(code); },
                }},
            {(int)AbilityContextApi::GetHapModuleInfo,
                {
                    [this](int code) { GetHapModuleInfoCase1(code); },
                    [this](int code) { GetHapModuleInfoCase2(code); },
                    [this](int code) { GetHapModuleInfoCase3(code); },
                }},
        };
    }

    std::unordered_map<int, std::vector<std::function<void(int)>>> mapCase_;
    int callingTime = 0;
    ~SecondAbility();

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
    std::shared_ptr<SecondEventSubscriber> subscriber_;
};
class SecondEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    explicit SecondEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp)
    {
        mapTestFunc_ = {
            {"Ability", [this](int apiIndex, int caseIndex, int code) { TestAbility(apiIndex, caseIndex, code); }},
        };
        mainAbility = nullptr;
    }

    void TestAbility(int apiIndex, int caseIndex, int code)
    {
        mainAbility->TestAbility(apiIndex, caseIndex, code);
    }

    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data);

    SecondAbility *mainAbility;
    std::unordered_map<std::string, std::function<void(int, int, int)>> mapTestFunc_;
    ~SecondEventSubscriber() = default;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // AMS_KIT_SYSTEM_TEST_A_MAIN_ABILITY_H