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

#ifndef _THIRD_ABILITY_H_
#define _THIRD_ABILITY_H_
#include <unordered_map>
#include "ability_loader.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "kit_test_common_info.h"
#include "skills.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using vector_str = std::vector<std::string>;
using vector_conststr = std::vector<const std::string>;
using vector_func = std::vector<std::function<void(int)>>;
class KitTestFourthEventSubscriber;

class FourthAbility : public Ability {
public:
    void SubscribeEvent(const vector_conststr &eventList);
    void SkillsApiStByCode(int apiIndex, int caseIndex, int code);
    bool CompareEntity(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2);
    bool CompareAction(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2);
    bool CompareAuthority(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2);
    bool CompareScheme(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2);
    bool CompareSPath(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2);
    bool CompareSsp(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2);
    bool CompareType(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2);
    bool CompareWantParams(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2);
    bool CompareSkills(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2);
    template <typename T, typename IT, typename M>
    void SkillsGetWantParams(M params, int code)
    {
        AAFwk::Skills skill;
        AAFwk::WantParams wantParams;
        std::string keyStr = "ParamsKey";
        M value = params;
        wantParams.SetParam(keyStr, T::Box(value));
        skill.SetWantParams(wantParams);
        bool result = (value == T::Unbox(IT::Query(skill.GetWantParams().GetParam(keyStr))));
        TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result));
    }

    // AddAction and CountActions ST kit Case
    void SkillsAddActionAndCountActionsCase1(int code);
    void SkillsAddActionAndCountActionsCase2(int code);
    void SkillsAddActionAndCountActionsCase3(int code);
    // AddAction and GetAction ST kit Case
    void SkillsAddActionAndGetActionCase1(int code);
    void SkillsAddActionAndGetActionCase2(int code);
    void SkillsAddActionAndGetActionCase3(int code);
    void SkillsAddActionAndGetActionCase4(int code);
    void SkillsAddActionAndGetActionCase5(int code);
    void SkillsAddActionAndGetActionCase6(int code);
    void SkillsAddActionAndGetActionCase7(int code);
    // AddAction and HasAction ST kit Case
    void SkillsAddActionAndHasActionCase1(int code);
    void SkillsAddActionAndHasActionCase2(int code);
    void SkillsAddActionAndHasActionCase3(int code);
    // AddAction,RemoveAction,CountActions and HasAction ST kit Case
    void SkillsAddActionAndRemoveActionCase1(int code);
    void SkillsAddActionAndRemoveActionCase2(int code);
    void SkillsAddActionAndRemoveActionCase3(int code);
    void SkillsAddActionAndRemoveActionCase4(int code);
    void SkillsAddActionAndRemoveActionCase5(int code);
    void SkillsAddActionAndRemoveActionCase6(int code);
    void SkillsAddActionAndRemoveActionCase7(int code);
    void SkillsAddActionAndRemoveActionCase8(int code);
    void SkillsAddActionAndRemoveActionCase9(int code);

    // AddEntity and CountEntities ST kit Case
    void SkillsAddEntityAndCountEntitiesCase1(int code);
    void SkillsAddEntityAndCountEntitiesCase2(int code);
    void SkillsAddEntityAndCountEntitiesCase3(int code);
    // AddEntity and GetEntity ST kit Case
    void SkillsAddEntityAndGetEntityCase1(int code);
    void SkillsAddEntityAndGetEntityCase2(int code);
    void SkillsAddEntityAndGetEntityCase3(int code);
    void SkillsAddEntityAndGetEntityCase4(int code);
    void SkillsAddEntityAndGetEntityCase5(int code);
    void SkillsAddEntityAndGetEntityCase6(int code);
    void SkillsAddEntityAndGetEntityCase7(int code);
    // AddEntity and HasEntity ST kit Case
    void SkillsAddEntityAndHasEntityCase1(int code);
    void SkillsAddEntityAndHasEntityCase2(int code);
    void SkillsAddEntityAndHasEntityCase3(int code);
    // AddEntity,RemoveEntity,CountEntities and HasEntity ST kit Case
    void SkillsAddEntityAndRemoveEntityCase1(int code);
    void SkillsAddEntityAndRemoveEntityCase2(int code);
    void SkillsAddEntityAndRemoveEntityCase3(int code);
    void SkillsAddEntityAndRemoveEntityCase4(int code);
    void SkillsAddEntityAndRemoveEntityCase5(int code);
    void SkillsAddEntityAndRemoveEntityCase6(int code);
    void SkillsAddEntityAndRemoveEntityCase7(int code);
    void SkillsAddEntityAndRemoveEntityCase8(int code);
    void SkillsAddEntityAndRemoveEntityCase9(int code);

    // AddAuthority and CountAuthorities ST kit Case
    void SkillsAddAuthorityAndCountAuthoritiesCase1(int code);
    void SkillsAddAuthorityAndCountAuthoritiesCase2(int code);
    void SkillsAddAuthorityAndCountAuthoritiesCase3(int code);
    // AddAuthority and GetAuthority ST kit Case
    void SkillsAddAuthorityAndGetAuthorityCase1(int code);
    void SkillsAddAuthorityAndGetAuthorityCase2(int code);
    void SkillsAddAuthorityAndGetAuthorityCase3(int code);
    void SkillsAddAuthorityAndGetAuthorityCase4(int code);
    void SkillsAddAuthorityAndGetAuthorityCase5(int code);
    void SkillsAddAuthorityAndGetAuthorityCase6(int code);
    void SkillsAddAuthorityAndGetAuthorityCase7(int code);
    // AddAuthority and HasAuthority ST kit Case
    void SkillsAddAuthorityAndHasAuthorityCase1(int code);
    void SkillsAddAuthorityAndHasAuthorityCase2(int code);
    void SkillsAddAuthorityAndHasAuthorityCase3(int code);
    // AddAuthority,RemoveAuthority,CountAuthorities and HasAuthority ST kit Case
    void SkillsAddAuthorityAndRemoveAuthorityCase1(int code);
    void SkillsAddAuthorityAndRemoveAuthorityCase2(int code);
    void SkillsAddAuthorityAndRemoveAuthorityCase3(int code);
    void SkillsAddAuthorityAndRemoveAuthorityCase4(int code);
    void SkillsAddAuthorityAndRemoveAuthorityCase5(int code);
    void SkillsAddAuthorityAndRemoveAuthorityCase6(int code);
    void SkillsAddAuthorityAndRemoveAuthorityCase7(int code);
    void SkillsAddAuthorityAndRemoveAuthorityCase8(int code);
    void SkillsAddAuthorityAndRemoveAuthorityCase9(int code);

    // AddScheme and CountSchemes ST kit Case
    void SkillsAddSchemeAndCountSchemesCase1(int code);
    void SkillsAddSchemeAndCountSchemesCase2(int code);
    void SkillsAddSchemeAndCountSchemesCase3(int code);
    // AddScheme and GetScheme ST kit Case
    void SkillsAddSchemeAndGetSchemeCase1(int code);
    void SkillsAddSchemeAndGetSchemeCase2(int code);
    void SkillsAddSchemeAndGetSchemeCase3(int code);
    void SkillsAddSchemeAndGetSchemeCase4(int code);
    void SkillsAddSchemeAndGetSchemeCase5(int code);
    void SkillsAddSchemeAndGetSchemeCase6(int code);
    void SkillsAddSchemeAndGetSchemeCase7(int code);
    // AddScheme and HasScheme ST kit Case
    void SkillsAddSchemeAndHasSchemeCase1(int code);
    void SkillsAddSchemeAndHasSchemeCase2(int code);
    void SkillsAddSchemeAndHasSchemeCase3(int code);
    // AddScheme,RemoveScheme,CountSchemes and HasScheme ST kit Case
    void SkillsAddSchemeAndRemoveSchemeCase1(int code);
    void SkillsAddSchemeAndRemoveSchemeCase2(int code);
    void SkillsAddSchemeAndRemoveSchemeCase3(int code);
    void SkillsAddSchemeAndRemoveSchemeCase4(int code);
    void SkillsAddSchemeAndRemoveSchemeCase5(int code);
    void SkillsAddSchemeAndRemoveSchemeCase6(int code);
    void SkillsAddSchemeAndRemoveSchemeCase7(int code);
    void SkillsAddSchemeAndRemoveSchemeCase8(int code);
    void SkillsAddSchemeAndRemoveSchemeCase9(int code);

    // AddSchemeSpecificPart and CountSchemeSpecificParts ST kit Case
    void SkillsAddSchemeSpecificPartAndCountSchemeSpecificPartsCase1(int code);
    void SkillsAddSchemeSpecificPartAndCountSchemeSpecificPartsCase2(int code);
    void SkillsAddSchemeSpecificPartAndCountSchemeSpecificPartsCase3(int code);
    // AddSchemeSpecificPart and GetSchemeSpecificPart ST kit Case
    void SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase1(int code);
    void SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase2(int code);
    void SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase3(int code);
    void SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase4(int code);
    void SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase5(int code);
    void SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase6(int code);
    void SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase7(int code);
    // AddSchemeSpecificPart and HasSchemeSpecificPart ST kit Case
    void SkillsAddSchemeSpecificPartAndHasSchemeSpecificPartCase1(int code);
    void SkillsAddSchemeSpecificPartAndHasSchemeSpecificPartCase2(int code);
    void SkillsAddSchemeSpecificPartAndHasSchemeSpecificPartCase3(int code);
    // AddSchemeSpecificPart,RemoveSchemeSpecificPart,CountSchemeSpecificParts and HasSchemeSpecificPart ST kit Case
    void SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase1(int code);
    void SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase2(int code);
    void SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase3(int code);
    void SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase4(int code);
    void SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase5(int code);
    void SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase6(int code);
    void SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase7(int code);
    void SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase8(int code);
    void SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase9(int code);

    // AddPath_String and CountPaths ST kit Case
    void SkillsAddPathStringAndCountPathsCase1(int code);
    void SkillsAddPathStringAndCountPathsCase2(int code);
    // AddPath_String_MatchType and CountPaths ST kit Case
    void SkillsAddPathStringMatchTypeAndCountPathsCase1(int code);
    void SkillsAddPathStringMatchTypeAndCountPathsCase2(int code);
    // AddPath_PatternMatcher and CountPaths ST kit Case
    void SkillsAddPathPatternAndCountPathsCase1(int code);
    void SkillsAddPathPatternAndCountPathsCase2(int code);
    // CountPaths ST kit Case
    void SkillsCountPathsCase1(int code);
    // AddPath_String and GetPath ST kit Case
    void SkillsAddPathStringAndGetPathCase1(int code);
    void SkillsAddPathStringAndGetPathCase2(int code);
    void SkillsAddPathStringAndGetPathCase3(int code);
    void SkillsAddPathStringAndGetPathCase4(int code);
    void SkillsAddPathStringAndGetPathCase5(int code);
    void SkillsAddPathStringAndGetPathCase6(int code);
    // AddPath_String_MatchType and GetPath ST kit Case
    void SkillsAddPathStringMatchTypeAndGetPathCase1(int code);
    void SkillsAddPathStringMatchTypeAndGetPathCase2(int code);
    void SkillsAddPathStringMatchTypeAndGetPathCase3(int code);
    void SkillsAddPathStringMatchTypeAndGetPathCase4(int code);
    void SkillsAddPathStringMatchTypeAndGetPathCase5(int code);
    void SkillsAddPathStringMatchTypeAndGetPathCase6(int code);
    // AddPath_PatternMatcher and GetPath ST kit Case
    void SkillsAddPathPatternAndGetPathCase1(int code);
    void SkillsAddPathPatternAndGetPathCase2(int code);
    void SkillsAddPathPatternAndGetPathCase3(int code);
    void SkillsAddPathPatternAndGetPathCase4(int code);
    void SkillsAddPathPatternAndGetPathCase5(int code);
    void SkillsAddPathPatternAndGetPathCase6(int code);
    // GetPath ST kit Case
    void SkillsGetPathCase1(int code);
    // AddPath_String and HasPath ST kit Case
    void SkillsAddPathStringAndHasPathCase1(int code);
    void SkillsAddPathStringAndHasPathCase2(int code);
    // AddPath_String_MatchType and HasPath ST kit Case
    void SkillsAddPathStringMatchTypeAndHasPathCase1(int code);
    void SkillsAddPathStringMatchTypeAndHasPathCase2(int code);
    // AddPath_PatternMatcher and HasPath ST kit Case
    void SkillsAddPathPatternAndHasPathCase1(int code);
    void SkillsAddPathPatternAndHasPathCase2(int code);
    // HasPath ST kit Case
    void SkillsHasPathCase1(int code);
    // AddPath_String and RemovePath ST kit Case
    void SkillsAddPathStringAndRemovePathStringCase1(int code);
    void SkillsAddPathStringAndRemovePathStringCase2(int code);
    void SkillsAddPathStringAndRemovePathStringCase3(int code);
    void SkillsAddPathStringAndRemovePathStringCase4(int code);
    void SkillsAddPathStringAndRemovePathStringCase5(int code);
    void SkillsAddPathStringAndRemovePathStringCase6(int code);
    void SkillsAddPathStringAndRemovePathStringCase7(int code);
    void SkillsAddPathStringAndRemovePathStringCase8(int code);
    void SkillsAddPathStringAndRemovePathStringCase9(int code);
    // AddPath_String_MatchType and RemovePathStringMatchType ST kit Case
    void SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase1(int code);
    void SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase2(int code);
    void SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase3(int code);
    void SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase4(int code);
    void SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase5(int code);
    void SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase6(int code);
    void SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase7(int code);
    void SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase8(int code);
    void SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase9(int code);
    // AddPath_PatternMatcher and RemovePathPatternMatcher ST kit Case
    void SkillsAddPathPatternAndRemovePathPatternMatcherCase1(int code);
    void SkillsAddPathPatternAndRemovePathPatternMatcherCase2(int code);
    void SkillsAddPathPatternAndRemovePathPatternMatcherCase3(int code);
    void SkillsAddPathPatternAndRemovePathPatternMatcherCase4(int code);
    void SkillsAddPathPatternAndRemovePathPatternMatcherCase5(int code);
    void SkillsAddPathPatternAndRemovePathPatternMatcherCase6(int code);
    void SkillsAddPathPatternAndRemovePathPatternMatcherCase7(int code);
    void SkillsAddPathPatternAndRemovePathPatternMatcherCase8(int code);
    void SkillsAddPathPatternAndRemovePathPatternMatcherCase9(int code);
    // AddPath and RemovePath ST kit Case
    void SkillsAddPathPatternAndRemovePathCase1(int code);
    void SkillsAddPathPatternAndRemovePathCase2(int code);
    void SkillsAddPathPatternAndRemovePathCase3(int code);

    // AddType_String and CountTypes ST kit Case
    void SkillsAddTypeStringAndCountTypesCase1(int code);
    void SkillsAddTypeStringAndCountTypesCase2(int code);
    // AddType_String_MatchType and CountTypes ST kit Case
    void SkillsAddTypeStringMatchTypeAndCountTypesCase1(int code);
    void SkillsAddTypeStringMatchTypeAndCountTypesCase2(int code);
    // AddType_PatternMatcher and CountTypes ST kit Case
    void SkillsAddTypePatternAndCountTypesCase1(int code);
    void SkillsAddTypePatternAndCountTypesCase2(int code);
    // CountTypes ST kit Case
    void SkillsCountTypesCase1(int code);
    // AddType_String and GetType ST kit Case
    void SkillsAddTypeStringAndGetTypeCase1(int code);
    void SkillsAddTypeStringAndGetTypeCase2(int code);
    void SkillsAddTypeStringAndGetTypeCase3(int code);
    void SkillsAddTypeStringAndGetTypeCase4(int code);
    void SkillsAddTypeStringAndGetTypeCase5(int code);
    void SkillsAddTypeStringAndGetTypeCase6(int code);
    // AddType_String_MatchType and GetType ST kit Case
    void SkillsAddTypeStringMatchTypeAndGetTypeCase1(int code);
    void SkillsAddTypeStringMatchTypeAndGetTypeCase2(int code);
    void SkillsAddTypeStringMatchTypeAndGetTypeCase3(int code);
    void SkillsAddTypeStringMatchTypeAndGetTypeCase4(int code);
    void SkillsAddTypeStringMatchTypeAndGetTypeCase5(int code);
    void SkillsAddTypeStringMatchTypeAndGetTypeCase6(int code);
    // AddType_PatternMatcher and GetType ST kit Case
    void SkillsAddTypePatternAndGetTypeCase1(int code);
    void SkillsAddTypePatternAndGetTypeCase2(int code);
    void SkillsAddTypePatternAndGetTypeCase3(int code);
    void SkillsAddTypePatternAndGetTypeCase4(int code);
    void SkillsAddTypePatternAndGetTypeCase5(int code);
    void SkillsAddTypePatternAndGetTypeCase6(int code);
    // GetType ST kit Case
    void SkillsGetTypeCase1(int code);
    // AddType_String and HasType ST kit Case
    void SkillsAddTypeStringAndHasTypeCase1(int code);
    void SkillsAddTypeStringAndHasTypeCase2(int code);
    // AddType_String_MatchType and HasType ST kit Case
    void SkillsAddTypeStringMatchTypeAndHasTypeCase1(int code);
    void SkillsAddTypeStringMatchTypeAndHasTypeCase2(int code);
    // AddType_PatternMatcher and HasType ST kit Case
    void SkillsAddTypePatternAndHasTypeCase1(int code);
    void SkillsAddTypePatternAndHasTypeCase2(int code);
    // HasType ST kit Case
    void SkillsHasTypeCase1(int code);
    // AddType_String and RemoveType ST kit Case
    void SkillsAddTypeStringAndRemoveTypeStringCase1(int code);
    void SkillsAddTypeStringAndRemoveTypeStringCase2(int code);
    void SkillsAddTypeStringAndRemoveTypeStringCase3(int code);
    void SkillsAddTypeStringAndRemoveTypeStringCase4(int code);
    void SkillsAddTypeStringAndRemoveTypeStringCase5(int code);
    void SkillsAddTypeStringAndRemoveTypeStringCase6(int code);
    void SkillsAddTypeStringAndRemoveTypeStringCase7(int code);
    void SkillsAddTypeStringAndRemoveTypeStringCase8(int code);
    void SkillsAddTypeStringAndRemoveTypeStringCase9(int code);
    // AddType_String_MatchType and RemoveTypeStringMatchType ST kit Case
    void SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase1(int code);
    void SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase2(int code);
    void SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase3(int code);
    void SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase4(int code);
    void SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase5(int code);
    void SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase6(int code);
    void SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase7(int code);
    void SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase8(int code);
    void SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase9(int code);
    // AddType_PatternMatcher and RemoveTypePatternMatcher ST kit Case
    void SkillsAddTypePatternAndRemoveTypePatternMatcherCase1(int code);
    void SkillsAddTypePatternAndRemoveTypePatternMatcherCase2(int code);
    void SkillsAddTypePatternAndRemoveTypePatternMatcherCase3(int code);
    void SkillsAddTypePatternAndRemoveTypePatternMatcherCase4(int code);
    void SkillsAddTypePatternAndRemoveTypePatternMatcherCase5(int code);
    void SkillsAddTypePatternAndRemoveTypePatternMatcherCase6(int code);
    void SkillsAddTypePatternAndRemoveTypePatternMatcherCase7(int code);
    void SkillsAddTypePatternAndRemoveTypePatternMatcherCase8(int code);
    void SkillsAddTypePatternAndRemoveTypePatternMatcherCase9(int code);
    // AddType and RemoveType ST kit Case
    void SkillsAddTypePatternAndRemoveTypeCase1(int code);
    void SkillsAddTypePatternAndRemoveTypeCase2(int code);
    void SkillsAddTypePatternAndRemoveTypeCase3(int code);
    void SkillsAddTypePatternAndRemoveTypeCase4(int code);
    void SkillsAddTypePatternAndRemoveTypeCase5(int code);
    void SkillsAddTypePatternAndRemoveTypeCase6(int code);
    void SkillsAddTypePatternAndRemoveTypeCase7(int code);
    void SkillsAddTypePatternAndRemoveTypeCase8(int code);
    void SkillsAddTypePatternAndRemoveTypeCase9(int code);

    // AddEntity and GetEntities ST kit Case
    void SkillAddEntityAndGetEntitiesCase1(int code);
    void SkillAddEntityAndGetEntitiesCase2(int code);
    void SkillAddEntityAndGetEntitiesCase3(int code);
    void SkillAddEntityAndGetEntitiesCase4(int code);
    void SkillAddEntityAndGetEntitiesCase5(int code);

    // SetWantParams and GetWantParams ST kit Case
    void SkillSetAndGetWantParamsCase1(int code);
    void SkillSetAndGetWantParamsCase2(int code);
    void SkillSetAndGetWantParamsCase3(int code);
    void SkillSetAndGetWantParamsCase4(int code);
    void SkillSetAndGetWantParamsCase5(int code);

    // Match ST kit Case
    void SkillMatchCase1(int code);
    void SkillMatchCase2(int cade);
    void SkillMatchCase3(int code);
    void SkillMatchCase4(int cade);
    void SkillMatchCase5(int code);
    void SkillMatchCase6(int cade);

    // Unmarshalling And Marshalling ST kit Case
    void SkillUnmarshallingCase1(int code);
    void SkillUnmarshallingCase2(int code);
    void SkillUnmarshallingCase3(int code);
    void SkillUnmarshallingCase4(int code);

    // Skills ST kit Case
    void SkillSkillsCase1(int code);
    void SkillSkillsCase2(int code);
    void SkillSkillsCase3(int code);
    void SkillSkillsCase4(int code);

    // Skills_Skills ST kit Case
    void SkillSkillsSkillsCase1(int code);
    void SkillSkillsSkillsCase2(int code);
    void SkillSkillsSkillsCase3(int code);
    void SkillSkillsSkillsCase4(int code);

    // Skills AddAction ST kit Case
    void SkillAddActionCase1(int code);

    // Skills AddEntity ST kit Case
    void SkillAddEntityCase1(int code);

    // Skills AddAuthority ST kit Case
    void SkillAddAuthorityCase1(int code);

    // Skills AddScheme ST kit Case
    void SkillAddSchemeCase1(int code);

    // Skills AddSchemeSpecificPart ST kit Case
    void SkillAddSchemeSpecificPartCase1(int code);

    std::shared_ptr<KitTestFourthEventSubscriber> subscriber;

protected:
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnForeground(const Want &want) override;
    virtual void OnNewWant(const Want &want) override;

private:
    std::unordered_map<int, vector_func> mapStKitFunc_ = {
        {static_cast<int>(SkillsApi::CountActions),
            {{[this](int code) { SkillsAddActionAndCountActionsCase1(code); }},
                {[this](int code) { SkillsAddActionAndCountActionsCase2(code); }},
                {[this](int code) { SkillsAddActionAndCountActionsCase3(code); }}}},
        {static_cast<int>(SkillsApi::GetAction),
            {{[this](int code) { SkillsAddActionAndGetActionCase1(code); }},
                {[this](int code) { SkillsAddActionAndGetActionCase2(code); }},
                {[this](int code) { SkillsAddActionAndGetActionCase3(code); }},
                {[this](int code) { SkillsAddActionAndGetActionCase4(code); }},
                {[this](int code) { SkillsAddActionAndGetActionCase5(code); }},
                {[this](int code) { SkillsAddActionAndGetActionCase6(code); }},
                {[this](int code) { SkillsAddActionAndGetActionCase7(code); }}}},
        {static_cast<int>(SkillsApi::HasAction),
            {{[this](int code) { SkillsAddActionAndHasActionCase1(code); }},
                {[this](int code) { SkillsAddActionAndHasActionCase2(code); }},
                {[this](int code) { SkillsAddActionAndHasActionCase3(code); }}}},
        {static_cast<int>(SkillsApi::RemoveAction),
            {{[this](int code) { SkillsAddActionAndRemoveActionCase1(code); }},
                {[this](int code) { SkillsAddActionAndRemoveActionCase2(code); }},
                {[this](int code) { SkillsAddActionAndRemoveActionCase3(code); }},
                {[this](int code) { SkillsAddActionAndRemoveActionCase4(code); }},
                {[this](int code) { SkillsAddActionAndRemoveActionCase5(code); }},
                {[this](int code) { SkillsAddActionAndRemoveActionCase6(code); }},
                {[this](int code) { SkillsAddActionAndRemoveActionCase7(code); }},
                {[this](int code) { SkillsAddActionAndRemoveActionCase8(code); }},
                {[this](int code) { SkillsAddActionAndRemoveActionCase9(code); }}}},
        {static_cast<int>(SkillsApi::CountEntities),
            {{[this](int code) { SkillsAddEntityAndCountEntitiesCase1(code); }},
                {[this](int code) { SkillsAddEntityAndCountEntitiesCase2(code); }},
                {[this](int code) { SkillsAddEntityAndCountEntitiesCase3(code); }}}},
        {static_cast<int>(SkillsApi::GetEntity),
            {{[this](int code) { SkillsAddEntityAndGetEntityCase1(code); }},
                {[this](int code) { SkillsAddEntityAndGetEntityCase2(code); }},
                {[this](int code) { SkillsAddEntityAndGetEntityCase3(code); }},
                {[this](int code) { SkillsAddEntityAndGetEntityCase4(code); }},
                {[this](int code) { SkillsAddEntityAndGetEntityCase5(code); }},
                {[this](int code) { SkillsAddEntityAndGetEntityCase6(code); }},
                {[this](int code) { SkillsAddEntityAndGetEntityCase7(code); }}}},
        {static_cast<int>(SkillsApi::HasEntity),
            {{[this](int code) { SkillsAddEntityAndHasEntityCase1(code); }},
                {[this](int code) { SkillsAddEntityAndHasEntityCase2(code); }},
                {[this](int code) { SkillsAddEntityAndHasEntityCase3(code); }}}},
        {static_cast<int>(SkillsApi::RemoveEntity),
            {{[this](int code) { SkillsAddEntityAndRemoveEntityCase1(code); }},
                {[this](int code) { SkillsAddEntityAndRemoveEntityCase2(code); }},
                {[this](int code) { SkillsAddEntityAndRemoveEntityCase3(code); }},
                {[this](int code) { SkillsAddEntityAndRemoveEntityCase4(code); }},
                {[this](int code) { SkillsAddEntityAndRemoveEntityCase5(code); }},
                {[this](int code) { SkillsAddEntityAndRemoveEntityCase6(code); }},
                {[this](int code) { SkillsAddEntityAndRemoveEntityCase7(code); }},
                {[this](int code) { SkillsAddEntityAndRemoveEntityCase8(code); }},
                {[this](int code) { SkillsAddEntityAndRemoveEntityCase9(code); }}}},
        {static_cast<int>(SkillsApi::CountAuthorities),
            {{[this](int code) { SkillsAddAuthorityAndCountAuthoritiesCase1(code); }},
                {[this](int code) { SkillsAddAuthorityAndCountAuthoritiesCase2(code); }},
                {[this](int code) { SkillsAddAuthorityAndCountAuthoritiesCase3(code); }}}},
        {static_cast<int>(SkillsApi::GetAuthority),
            {{[this](int code) { SkillsAddAuthorityAndGetAuthorityCase1(code); }},
                {[this](int code) { SkillsAddAuthorityAndGetAuthorityCase2(code); }},
                {[this](int code) { SkillsAddAuthorityAndGetAuthorityCase3(code); }},
                {[this](int code) { SkillsAddAuthorityAndGetAuthorityCase4(code); }},
                {[this](int code) { SkillsAddAuthorityAndGetAuthorityCase5(code); }},
                {[this](int code) { SkillsAddAuthorityAndGetAuthorityCase6(code); }},
                {[this](int code) { SkillsAddAuthorityAndGetAuthorityCase7(code); }}}},
        {static_cast<int>(SkillsApi::HasAuthority),
            {{[this](int code) { SkillsAddAuthorityAndHasAuthorityCase1(code); }},
                {[this](int code) { SkillsAddAuthorityAndHasAuthorityCase2(code); }},
                {[this](int code) { SkillsAddAuthorityAndHasAuthorityCase3(code); }}}},
        {static_cast<int>(SkillsApi::RemoveAuthority),
            {{[this](int code) { SkillsAddAuthorityAndRemoveAuthorityCase1(code); }},
                {[this](int code) { SkillsAddAuthorityAndRemoveAuthorityCase2(code); }},
                {[this](int code) { SkillsAddAuthorityAndRemoveAuthorityCase3(code); }},
                {[this](int code) { SkillsAddAuthorityAndRemoveAuthorityCase4(code); }},
                {[this](int code) { SkillsAddAuthorityAndRemoveAuthorityCase5(code); }},
                {[this](int code) { SkillsAddAuthorityAndRemoveAuthorityCase6(code); }},
                {[this](int code) { SkillsAddAuthorityAndRemoveAuthorityCase7(code); }},
                {[this](int code) { SkillsAddAuthorityAndRemoveAuthorityCase8(code); }},
                {[this](int code) { SkillsAddAuthorityAndRemoveAuthorityCase9(code); }}}},
        {static_cast<int>(SkillsApi::CountSchemes),
            {{[this](int code) { SkillsAddSchemeAndCountSchemesCase1(code); }},
                {[this](int code) { SkillsAddSchemeAndCountSchemesCase2(code); }},
                {[this](int code) { SkillsAddSchemeAndCountSchemesCase3(code); }}}},
        {static_cast<int>(SkillsApi::GetScheme),
            {{[this](int code) { SkillsAddSchemeAndGetSchemeCase1(code); }},
                {[this](int code) { SkillsAddSchemeAndGetSchemeCase2(code); }},
                {[this](int code) { SkillsAddSchemeAndGetSchemeCase3(code); }},
                {[this](int code) { SkillsAddSchemeAndGetSchemeCase4(code); }},
                {[this](int code) { SkillsAddSchemeAndGetSchemeCase5(code); }},
                {[this](int code) { SkillsAddSchemeAndGetSchemeCase6(code); }},
                {[this](int code) { SkillsAddSchemeAndGetSchemeCase7(code); }}}},
        {static_cast<int>(SkillsApi::HasScheme),
            {{[this](int code) { SkillsAddSchemeAndHasSchemeCase1(code); }},
                {[this](int code) { SkillsAddSchemeAndHasSchemeCase2(code); }},
                {[this](int code) { SkillsAddSchemeAndHasSchemeCase3(code); }}}},
        {static_cast<int>(SkillsApi::RemoveScheme),
            {{[this](int code) { SkillsAddSchemeAndRemoveSchemeCase1(code); }},
                {[this](int code) { SkillsAddSchemeAndRemoveSchemeCase2(code); }},
                {[this](int code) { SkillsAddSchemeAndRemoveSchemeCase3(code); }},
                {[this](int code) { SkillsAddSchemeAndRemoveSchemeCase4(code); }},
                {[this](int code) { SkillsAddSchemeAndRemoveSchemeCase5(code); }},
                {[this](int code) { SkillsAddSchemeAndRemoveSchemeCase6(code); }},
                {[this](int code) { SkillsAddSchemeAndRemoveSchemeCase7(code); }},
                {[this](int code) { SkillsAddSchemeAndRemoveSchemeCase8(code); }},
                {[this](int code) { SkillsAddSchemeAndRemoveSchemeCase9(code); }}}},
        {static_cast<int>(SkillsApi::CountSchemeSpecificParts),
            {{[this](int code) { SkillsAddSchemeSpecificPartAndCountSchemeSpecificPartsCase1(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndCountSchemeSpecificPartsCase2(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndCountSchemeSpecificPartsCase3(code); }}}},
        {static_cast<int>(SkillsApi::GetSchemeSpecificPart),
            {{[this](int code) { SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase1(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase2(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase3(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase4(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase5(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase6(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase7(code); }}}},
        {static_cast<int>(SkillsApi::HasSchemeSpecificPart),
            {{[this](int code) { SkillsAddSchemeSpecificPartAndHasSchemeSpecificPartCase1(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndHasSchemeSpecificPartCase2(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndHasSchemeSpecificPartCase3(code); }}}},
        {static_cast<int>(SkillsApi::RemoveSchemeSpecificPart),
            {{[this](int code) { SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase1(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase2(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase3(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase4(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase5(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase6(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase7(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase8(code); }},
                {[this](int code) { SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase9(code); }}}},
        {static_cast<int>(SkillsApi::AddPath_String_CountPaths),
            {{[this](int code) { SkillsAddPathStringAndCountPathsCase1(code); }},
                {[this](int code) { SkillsAddPathStringAndCountPathsCase2(code); }}}},
        {static_cast<int>(SkillsApi::AddPath_String_MatchType_CountPaths),
            {{[this](int code) { SkillsAddPathStringMatchTypeAndCountPathsCase1(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndCountPathsCase2(code); }}}},
        {static_cast<int>(SkillsApi::AddPath_PatternMatcher_CountPaths),
            {{[this](int code) { SkillsAddPathPatternAndCountPathsCase1(code); }},
                {[this](int code) { SkillsAddPathPatternAndCountPathsCase2(code); }}}},
        {static_cast<int>(SkillsApi::CountPaths), {{[this](int code) { SkillsCountPathsCase1(code); }}}},
        {static_cast<int>(SkillsApi::AddPath_String_GetPath),
            {{[this](int code) { SkillsAddPathStringAndGetPathCase1(code); }},
                {[this](int code) { SkillsAddPathStringAndGetPathCase2(code); }},
                {[this](int code) { SkillsAddPathStringAndGetPathCase3(code); }},
                {[this](int code) { SkillsAddPathStringAndGetPathCase4(code); }},
                {[this](int code) { SkillsAddPathStringAndGetPathCase5(code); }},
                {[this](int code) { SkillsAddPathStringAndGetPathCase6(code); }}}},
        {static_cast<int>(SkillsApi::AddPath_String_MatchType_GetPath),
            {{[this](int code) { SkillsAddPathStringMatchTypeAndGetPathCase1(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndGetPathCase2(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndGetPathCase3(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndGetPathCase4(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndGetPathCase5(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndGetPathCase6(code); }}}},
        {static_cast<int>(SkillsApi::AddPath_PatternMatcher_GetPath),
            {{[this](int code) { SkillsAddPathPatternAndGetPathCase1(code); }},
                {[this](int code) { SkillsAddPathPatternAndGetPathCase2(code); }},
                {[this](int code) { SkillsAddPathPatternAndGetPathCase3(code); }},
                {[this](int code) { SkillsAddPathPatternAndGetPathCase4(code); }},
                {[this](int code) { SkillsAddPathPatternAndGetPathCase5(code); }},
                {[this](int code) { SkillsAddPathPatternAndGetPathCase6(code); }}}},
        {static_cast<int>(SkillsApi::GetPath), {{[this](int code) { SkillsGetPathCase1(code); }}}},
        {static_cast<int>(SkillsApi::AddPath_String_HasPath),
            {{[this](int code) { SkillsAddPathStringAndHasPathCase1(code); }},
                {[this](int code) { SkillsAddPathStringAndHasPathCase2(code); }}}},
        {static_cast<int>(SkillsApi::AddPath_String_MatchType_HasPath),
            {{[this](int code) { SkillsAddPathStringMatchTypeAndHasPathCase1(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndHasPathCase2(code); }}}},
        {static_cast<int>(SkillsApi::AddPath_PatternMatcher_HasPath),
            {{[this](int code) { SkillsAddPathStringMatchTypeAndHasPathCase1(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndHasPathCase2(code); }}}},
        {static_cast<int>(SkillsApi::HasPath), {{[this](int code) { SkillsHasPathCase1(code); }}}},
        {static_cast<int>(SkillsApi::RemovePath_String),
            {{[this](int code) { SkillsAddPathStringAndRemovePathStringCase1(code); }},
                {[this](int code) { SkillsAddPathStringAndRemovePathStringCase2(code); }},
                {[this](int code) { SkillsAddPathStringAndRemovePathStringCase3(code); }},
                {[this](int code) { SkillsAddPathStringAndRemovePathStringCase4(code); }},
                {[this](int code) { SkillsAddPathStringAndRemovePathStringCase5(code); }},
                {[this](int code) { SkillsAddPathStringAndRemovePathStringCase6(code); }},
                {[this](int code) { SkillsAddPathStringAndRemovePathStringCase7(code); }},
                {[this](int code) { SkillsAddPathStringAndRemovePathStringCase8(code); }},
                {[this](int code) { SkillsAddPathStringAndRemovePathStringCase9(code); }}}},
        {static_cast<int>(SkillsApi::RemovePath_String_MatchType),
            {{[this](int code) { SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase1(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase2(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase3(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase4(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase5(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase6(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase7(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase8(code); }},
                {[this](int code) { SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase9(code); }}}},
        {static_cast<int>(SkillsApi::RemovePath_PatternMatcher),
            {{[this](int code) { SkillsAddPathPatternAndRemovePathPatternMatcherCase1(code); }},
                {[this](int code) { SkillsAddPathPatternAndRemovePathPatternMatcherCase2(code); }},
                {[this](int code) { SkillsAddPathPatternAndRemovePathPatternMatcherCase3(code); }},
                {[this](int code) { SkillsAddPathPatternAndRemovePathPatternMatcherCase4(code); }},
                {[this](int code) { SkillsAddPathPatternAndRemovePathPatternMatcherCase5(code); }},
                {[this](int code) { SkillsAddPathPatternAndRemovePathPatternMatcherCase6(code); }},
                {[this](int code) { SkillsAddPathPatternAndRemovePathPatternMatcherCase7(code); }},
                {[this](int code) { SkillsAddPathPatternAndRemovePathPatternMatcherCase8(code); }},
                {[this](int code) { SkillsAddPathPatternAndRemovePathPatternMatcherCase9(code); }}}},
        {static_cast<int>(SkillsApi::RemovePath_Other),
            {{[this](int code) { SkillsAddPathPatternAndRemovePathCase1(code); }},
                {[this](int code) { SkillsAddPathPatternAndRemovePathCase2(code); }},
                {[this](int code) { SkillsAddPathPatternAndRemovePathCase3(code); }}}},
        {static_cast<int>(SkillsApi::AddType_String_CountTypes),
            {{[this](int code) { SkillsAddTypeStringAndCountTypesCase1(code); }},
                {[this](int code) { SkillsAddTypeStringAndCountTypesCase2(code); }}}},
        {static_cast<int>(SkillsApi::AddType_String_MatchType_CountTypes),
            {{[this](int code) { SkillsAddTypeStringMatchTypeAndCountTypesCase1(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndCountTypesCase2(code); }}}},
        {static_cast<int>(SkillsApi::AddType_PatternMatcher_CountTypes),
            {{[this](int code) { SkillsAddTypePatternAndCountTypesCase1(code); }},
                {[this](int code) { SkillsAddTypePatternAndCountTypesCase2(code); }}}},
        {static_cast<int>(SkillsApi::CountTypes), {{[this](int code) { SkillsCountTypesCase1(code); }}}},
        {static_cast<int>(SkillsApi::AddType_String_GetType),
            {{[this](int code) { SkillsAddTypeStringAndGetTypeCase1(code); }},
                {[this](int code) { SkillsAddTypeStringAndGetTypeCase2(code); }},
                {[this](int code) { SkillsAddTypeStringAndGetTypeCase3(code); }},
                {[this](int code) { SkillsAddTypeStringAndGetTypeCase4(code); }},
                {[this](int code) { SkillsAddTypeStringAndGetTypeCase5(code); }},
                {[this](int code) { SkillsAddTypeStringAndGetTypeCase6(code); }}}},
        {static_cast<int>(SkillsApi::AddType_String_MatchType_GetType),
            {{[this](int code) { SkillsAddTypeStringMatchTypeAndGetTypeCase1(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndGetTypeCase2(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndGetTypeCase3(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndGetTypeCase4(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndGetTypeCase5(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndGetTypeCase6(code); }}}},
        {static_cast<int>(SkillsApi::AddType_PatternMatcher_GetType),
            {{[this](int code) { SkillsAddTypePatternAndGetTypeCase1(code); }},
                {[this](int code) { SkillsAddTypePatternAndGetTypeCase2(code); }},
                {[this](int code) { SkillsAddTypePatternAndGetTypeCase3(code); }},
                {[this](int code) { SkillsAddTypePatternAndGetTypeCase4(code); }},
                {[this](int code) { SkillsAddTypePatternAndGetTypeCase5(code); }},
                {[this](int code) { SkillsAddTypePatternAndGetTypeCase6(code); }}}},
        {static_cast<int>(SkillsApi::GetType), {{[this](int code) { SkillsGetTypeCase1(code); }}}},
        {static_cast<int>(SkillsApi::AddType_String_HasType),
            {{[this](int code) { SkillsAddTypeStringAndHasTypeCase1(code); }},
                {[this](int code) { SkillsAddTypeStringAndHasTypeCase2(code); }}}},
        {static_cast<int>(SkillsApi::AddType_String_MatchType_HasType),
            {{[this](int code) { SkillsAddTypeStringMatchTypeAndHasTypeCase1(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndHasTypeCase2(code); }}}},
        {static_cast<int>(SkillsApi::AddType_PatternMatcher_HasType),
            {{[this](int code) { SkillsAddTypeStringMatchTypeAndHasTypeCase1(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndHasTypeCase2(code); }}}},
        {static_cast<int>(SkillsApi::HasType), {{[this](int code) { SkillsHasTypeCase1(code); }}}},
        {static_cast<int>(SkillsApi::RemoveType_String),
            {{[this](int code) { SkillsAddTypeStringAndRemoveTypeStringCase1(code); }},
                {[this](int code) { SkillsAddTypeStringAndRemoveTypeStringCase2(code); }},
                {[this](int code) { SkillsAddTypeStringAndRemoveTypeStringCase3(code); }},
                {[this](int code) { SkillsAddTypeStringAndRemoveTypeStringCase4(code); }},
                {[this](int code) { SkillsAddTypeStringAndRemoveTypeStringCase5(code); }},
                {[this](int code) { SkillsAddTypeStringAndRemoveTypeStringCase6(code); }},
                {[this](int code) { SkillsAddTypeStringAndRemoveTypeStringCase7(code); }},
                {[this](int code) { SkillsAddTypeStringAndRemoveTypeStringCase8(code); }},
                {[this](int code) { SkillsAddTypeStringAndRemoveTypeStringCase9(code); }}}},
        {static_cast<int>(SkillsApi::RemoveType_String_MatchType),
            {{[this](int code) { SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase1(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase2(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase3(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase4(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase5(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase6(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase7(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase8(code); }},
                {[this](int code) { SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase9(code); }}}},
        {static_cast<int>(SkillsApi::RemoveType_PatternMatcher),
            {{[this](int code) { SkillsAddTypePatternAndRemoveTypePatternMatcherCase1(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypePatternMatcherCase2(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypePatternMatcherCase3(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypePatternMatcherCase4(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypePatternMatcherCase5(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypePatternMatcherCase6(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypePatternMatcherCase7(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypePatternMatcherCase8(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypePatternMatcherCase9(code); }}}},
        {static_cast<int>(SkillsApi::RemoveType_Other),
            {{[this](int code) { SkillsAddTypePatternAndRemoveTypeCase1(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypeCase2(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypeCase3(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypeCase4(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypeCase5(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypeCase6(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypeCase7(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypeCase8(code); }},
                {[this](int code) { SkillsAddTypePatternAndRemoveTypeCase9(code); }}}},
        {static_cast<int>(SkillsApi::GetEntities),
            {{[this](int code) { SkillAddEntityAndGetEntitiesCase1(code); }},
                {[this](int code) { SkillAddEntityAndGetEntitiesCase2(code); }},
                {[this](int code) { SkillAddEntityAndGetEntitiesCase3(code); }},
                {[this](int code) { SkillAddEntityAndGetEntitiesCase4(code); }},
                {[this](int code) { SkillAddEntityAndGetEntitiesCase5(code); }}}},
        {static_cast<int>(SkillsApi::GetWantParams),
            {{[this](int code) { SkillSetAndGetWantParamsCase1(code); }},
                {[this](int code) { SkillSetAndGetWantParamsCase2(code); }},
                {[this](int code) { SkillSetAndGetWantParamsCase3(code); }},
                {[this](int code) { SkillSetAndGetWantParamsCase4(code); }},
                {[this](int code) { SkillSetAndGetWantParamsCase5(code); }}}},
        {static_cast<int>(SkillsApi::Match),
            {{[this](int code) { SkillMatchCase1(code); }},
                {[this](int code) { SkillMatchCase2(code); }},
                {[this](int code) { SkillMatchCase3(code); }},
                {[this](int code) { SkillMatchCase4(code); }},
                {[this](int code) { SkillMatchCase5(code); }},
                {[this](int code) { SkillMatchCase6(code); }}}},
        {static_cast<int>(SkillsApi::Unmarshalling),
            {{[this](int code) { SkillUnmarshallingCase1(code); }},
                {[this](int code) { SkillUnmarshallingCase2(code); }},
                {[this](int code) { SkillUnmarshallingCase3(code); }},
                {[this](int code) { SkillUnmarshallingCase4(code); }}}},
        {static_cast<int>(SkillsApi::Skills),
            {{[this](int code) { SkillSkillsCase1(code); }},
                {[this](int code) { SkillSkillsCase2(code); }},
                {[this](int code) { SkillSkillsCase3(code); }},
                {[this](int code) { SkillSkillsCase4(code); }}}},
        {static_cast<int>(SkillsApi::Skills_Skills),
            {{[this](int code) { SkillSkillsSkillsCase1(code); }},
                {[this](int code) { SkillSkillsSkillsCase2(code); }},
                {[this](int code) { SkillSkillsSkillsCase3(code); }},
                {[this](int code) { SkillSkillsSkillsCase4(code); }}}},
        {static_cast<int>(SkillsApi::AddAction), {{[this](int code) { SkillAddActionCase1(code); }}}},
        {static_cast<int>(SkillsApi::AddEntity), {{[this](int code) { SkillAddEntityCase1(code); }}}},
        {static_cast<int>(SkillsApi::AddAuthority), {{[this](int code) { SkillAddAuthorityCase1(code); }}}},
        {static_cast<int>(SkillsApi::AddScheme), {{[this](int code) { SkillAddSchemeCase1(code); }}}},
        {static_cast<int>(SkillsApi::AddSchemeSpecificPart),
            {{[this](int code) { SkillAddSchemeSpecificPartCase1(code); }}}},
    };
};

class KitTestFourthEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    KitTestFourthEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp, FourthAbility *ability)
        : CommonEventSubscriber(sp)
    {
        mapTestFunc_ = {
            {"SkillsApi",
                [this](int apiIndex, int caseIndex, int code) { SkillsApiStByCode(apiIndex, caseIndex, code); }},
        };
        fourthAbility_ = ability;
    }
    ~KitTestFourthEventSubscriber() = default;
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data) override;

    void SkillsApiStByCode(int apiIndex, int caseIndex, int code);
    void KitTerminateAbility();

    FourthAbility *fourthAbility_;

private:
    std::unordered_map<std::string, std::function<void(int, int, int)>> mapTestFunc_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // _THIRD_ABILITY_H_