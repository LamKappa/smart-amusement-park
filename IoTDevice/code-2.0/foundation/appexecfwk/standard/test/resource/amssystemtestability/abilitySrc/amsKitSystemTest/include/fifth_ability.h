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

#ifndef _SECOND_ABILITY_H_
#define _SECOND_ABILITY_H_
#include "ability_loader.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "kit_test_common_info.h"
namespace OHOS {
namespace AppExecFwk {
class FifthEventSubscriber;
class FifthAbility : public Ability {
public:
    FifthAbility()
    {
        InsertSetParamApi();
        InsertHasParamApi();
        InsertIsEmptyApi();
        InsertMarshallingApi();
        InsertSizeApi();
        InsertKeySetApi();
        InsertWantParamsCopyApi();
        InsertRemoveApi();
    }

    void InsertSetParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantParamsSetParamCase1(code); },
            [this](int code) { WantParamsSetParamCase2(code); },
            [this](int code) { WantParamsSetParamCase3(code); },
            [this](int code) { WantParamsSetParamCase4(code); },
            [this](int code) { WantParamsSetParamCase5(code); },
            [this](int code) { WantParamsSetParamCase6(code); },
            [this](int code) { WantParamsSetParamCase7(code); },
            [this](int code) { WantParamsSetParamCase8(code); },
            [this](int code) { WantParamsSetParamCase9(code); },
            [this](int code) { WantParamsSetParamCase10(code); },
            [this](int code) { WantParamsSetParamCase11(code); },
            [this](int code) { WantParamsSetParamCase12(code); },
            [this](int code) { WantParamsSetParamCase13(code); },
            [this](int code) { WantParamsSetParamCase14(code); },
            [this](int code) { WantParamsSetParamCase15(code); },
            [this](int code) { WantParamsSetParamCase16(code); },
            [this](int code) { WantParamsSetParamCase17(code); },
            [this](int code) { WantParamsSetParamCase18(code); },
        };
        mapCase_[(int)WantParamsApi::SetParam] = funs;
    }

    void InsertHasParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantParamsHasParamCase1(code); },
            [this](int code) { WantParamsHasParamCase2(code); },
            [this](int code) { WantParamsHasParamCase3(code); },
            [this](int code) { WantParamsHasParamCase4(code); },
            [this](int code) { WantParamsHasParamCase5(code); },
        };
        mapCase_[(int)WantParamsApi::HasParam] = funs;
    }

    void InsertIsEmptyApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantParamsIsEmptyCase1(code); },
            [this](int code) { WantParamsIsEmptyCase2(code); },
        };
        mapCase_[(int)WantParamsApi::IsEmpty] = funs;
    }

    void InsertMarshallingApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantParamsMarshallingCase1(code); },
            [this](int code) { WantParamsMarshallingCase2(code); },
            [this](int code) { WantParamsMarshallingCase3(code); },
            [this](int code) { WantParamsMarshallingCase4(code); },
            [this](int code) { WantParamsMarshallingCase5(code); },
            [this](int code) { WantParamsMarshallingCase6(code); },
            [this](int code) { WantParamsMarshallingCase7(code); },
            [this](int code) { WantParamsMarshallingCase8(code); },
            [this](int code) { WantParamsMarshallingCase9(code); },
            [this](int code) { WantParamsMarshallingCase10(code); },
            [this](int code) { WantParamsMarshallingCase11(code); },
            [this](int code) { WantParamsMarshallingCase12(code); },
            [this](int code) { WantParamsMarshallingCase13(code); },
            [this](int code) { WantParamsMarshallingCase14(code); },
            [this](int code) { WantParamsMarshallingCase15(code); },
            [this](int code) { WantParamsMarshallingCase16(code); },
            [this](int code) { WantParamsMarshallingCase17(code); },
            [this](int code) { WantParamsMarshallingCase18(code); },
        };
        mapCase_[(int)WantParamsApi::Marshalling] = funs;
    }

    void InsertSizeApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantParamsSizeCase1(code); },
            [this](int code) { WantParamsSizeCase2(code); },
            [this](int code) { WantParamsSizeCase3(code); },
        };
        mapCase_[(int)WantParamsApi::Size] = funs;
    }

    void InsertKeySetApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantParamsKeySetCase1(code); },
            [this](int code) { WantParamsKeySetCase2(code); },
        };
        mapCase_[(int)WantParamsApi::KeySet] = funs;
    }

    void InsertWantParamsCopyApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantParamsCopyCase1(code); },
            [this](int code) { WantParamsCopyCase2(code); },
        };
        mapCase_[(int)WantParamsApi::WantParamsCopy] = funs;
    }

    void InsertRemoveApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantParamsRemoveCase1(code); },
            [this](int code) { WantParamsRemoveCase2(code); },
        };
        mapCase_[(int)WantParamsApi::Remove] = funs;
    }

    virtual ~FifthAbility() = default;
    void SubscribeEvent();
    void TestWantParams(int apiIndex, int caseIndex, int code);
    void WantParamsSetParamCase1(int code);
    void WantParamsSetParamCase2(int code);
    void WantParamsSetParamCase3(int code);
    void WantParamsSetParamCase4(int code);
    void WantParamsSetParamCase5(int code);
    void WantParamsSetParamCase6(int code);
    void WantParamsSetParamCase7(int code);
    void WantParamsSetParamCase8(int code);
    void WantParamsSetParamCase9(int code);
    void WantParamsSetParamCase10(int code);
    void WantParamsSetParamCase11(int code);
    void WantParamsSetParamCase12(int code);
    void WantParamsSetParamCase13(int code);
    void WantParamsSetParamCase14(int code);
    void WantParamsSetParamCase15(int code);
    void WantParamsSetParamCase16(int code);
    void WantParamsSetParamCase17(int code);
    void WantParamsSetParamCase18(int code);
    void WantParamsHasParamCase1(int code);
    void WantParamsHasParamCase2(int code);
    void WantParamsHasParamCase3(int code);
    void WantParamsHasParamCase4(int code);
    void WantParamsHasParamCase5(int code);
    void WantParamsIsEmptyCase1(int code);
    void WantParamsIsEmptyCase2(int code);
    void WantParamsMarshallingCase1(int code);
    void WantParamsMarshallingCase2(int code);
    void WantParamsMarshallingCase3(int code);
    void WantParamsMarshallingCase4(int code);
    void WantParamsMarshallingCase5(int code);
    void WantParamsMarshallingCase6(int code);
    void WantParamsMarshallingCase7(int code);
    void WantParamsMarshallingCase8(int code);
    void WantParamsMarshallingCase9(int code);
    void WantParamsMarshallingCase10(int code);
    void WantParamsMarshallingCase11(int code);
    void WantParamsMarshallingCase12(int code);
    void WantParamsMarshallingCase13(int code);
    void WantParamsMarshallingCase14(int code);
    void WantParamsMarshallingCase15(int code);
    void WantParamsMarshallingCase16(int code);
    void WantParamsMarshallingCase17(int code);
    void WantParamsMarshallingCase18(int code);
    void WantParamsSizeCase1(int code);
    void WantParamsSizeCase2(int code);
    void WantParamsSizeCase3(int code);
    void WantParamsKeySetCase1(int code);
    void WantParamsKeySetCase2(int code);
    void WantParamsCopyCase1(int code);
    void WantParamsCopyCase2(int code);
    void WantParamsRemoveCase1(int code);
    void WantParamsRemoveCase2(int code);

    std::unordered_map<int, std::vector<std::function<void(int)>>> mapCase_;
    std::shared_ptr<FifthEventSubscriber> subscriber;

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

class FifthEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
public:
    FifthEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &sp, FifthAbility *ability)
        : CommonEventSubscriber(sp)
    {
        mapTestFunc_ = {
            {"WantParams",
                [this](int apiIndex, int caseIndex, int code) { TestWantParams(apiIndex, caseIndex, code); }},
        };
        fifthAbility = ability;
    }
    ~FifthEventSubscriber() = default;

    void TestWantParams(int apiIndex, int caseIndex, int code)
    {
        fifthAbility->TestWantParams(apiIndex, caseIndex, code);
    }
    virtual void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data);

    FifthAbility *fifthAbility;
    std::unordered_map<std::string, std::function<void(int, int, int)>> mapTestFunc_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // _SECOND_ABILITY_H_