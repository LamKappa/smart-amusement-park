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

#include "fourth_ability.h"
#include <iostream>
#include <numeric>
#include <sstream>
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;
using namespace OHOS::AAFwk;
namespace {

const std::string normalString = "kitapp.system.test";
// Special string
const std::string specialString = "@#￥#//\\%&*_=+[]^:!~();,.'?3243adsafdf_中文";
const std::string normalPathAndType = "kitapp.system/.test";
const std::string skillspecialTypeStr1 = "special.kitapp.system.test";
// Special string
const std::string skillspecialTypeStr2 = "special.kitapp.system.test/*";
// Special string
const std::string skillspecialTypeStr3 = "/special.kitapp.system.test";
const int cycleCount = 1000;
enum class AddFunctionType { Add_String, Add_String_MatchType, Add_PatternMatcher };
}  // namespace

#define COUNTFUNCTION(objectName, countFunction, expected, code) \
    int actionCount = objectName.countFunction();                \
    bool result = (actionCount == expected);                     \
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result))

#define GETFUNCTION(objectName, getFunction, parameter, expected, code) \
    std::string skillAction = objectName.getFunction(parameter);        \
    bool result = (skillAction == expected);                            \
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result))

#define HASFUNCTION(objectName, hasFunction, parameter, expected, code) \
    bool isHas = objectName.hasFunction(parameter);                     \
    bool result = (isHas == expected);                                  \
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result))

// Add Path and Type Define Function
#define ADDFUNCTION(objeceName, addFunction, stringParameter, addFunctionType) \
    if (AddFunctionType::Add_String == addFunctionType) {                      \
        objeceName.addFunction(stringParameter);                               \
    } else if (AddFunctionType::Add_String_MatchType == addFunctionType) {     \
        skill.addFunction(stringParameter, MatchType::PATTERN_PREFIX);         \
    } else {                                                                   \
        PatternsMatcher pm(stringParameter, MatchType::PATTERN_PREFIX);        \
        skill.addFunction(pm);                                                 \
    }

#define REMOVEFUNCTION(objeceName, removeFunction, stringParameter, addFunctionType) \
    if (AddFunctionType::Add_String == addFunctionType) {                            \
        objeceName.removeFunction(stringParameter);                                  \
    } else if (AddFunctionType::Add_String_MatchType == addFunctionType) {           \
        skill.removeFunction(stringParameter, MatchType::PATTERN_PREFIX);            \
    } else {                                                                         \
        PatternsMatcher pm(stringParameter, MatchType::PATTERN_PREFIX);              \
        skill.removeFunction(pm);                                                    \
    }

// Add and Count ST kit Case(Action,Entity,Authority,Scheme,SchemeSpecificPart)
#define SKILLS_ADD_AND_COUNT_CASE1(addFunction, countFunction, code) \
    Skills skill;                                                    \
    skill.addFunction(normalString);                                 \
    COUNTFUNCTION(skill, countFunction, 1, code)

#define SKILLS_ADD_AND_COUNT_CASE2(countFunction, code) \
    Skills skill;                                       \
    COUNTFUNCTION(skill, countFunction, 0, code)

#define SKILLS_ADD_AND_COUNT_CASE3(addFunction, countFunction, code) \
    Skills skill;                                                    \
    for (int index = 0; index < cycleCount; index++) {               \
        skill.addFunction(normalString);                             \
    }                                                                \
    COUNTFUNCTION(skill, countFunction, 1, code)

// Add and Get ST kit Case(Action,Entity,Authority,Scheme,SchemeSpecificPart)
#define SKILLS_ADD_AND_GET_CASE1(addFunction, getFunction, code) \
    Skills skill;                                                \
    skill.addFunction(normalString);                             \
    GETFUNCTION(skill, getFunction, 0, normalString, code)

#define SKILLS_ADD_AND_GET_CASE2(addFunction, getFunction, code) \
    Skills skill;                                                \
    skill.addFunction(normalString);                             \
    GETFUNCTION(skill, getFunction, 10, std::string(), code)

#define SKILLS_ADD_AND_GET_CASE3(addFunction, getFunction, code) \
    Skills skill;                                                \
    skill.addFunction(normalString);                             \
    GETFUNCTION(skill, getFunction, -10, std::string(), code)

#define SKILLS_ADD_AND_GET_CASE4(getFunction, code) \
    Skills skill;                                   \
    GETFUNCTION(skill, getFunction, 0, std::string(), code)

#define SKILLS_ADD_AND_GET_CASE5(addFunction, getFunction, code) \
    Skills skill;                                                \
    skill.addFunction(specialString);                            \
    GETFUNCTION(skill, getFunction, 0, specialString, code)

#define SKILLS_ADD_AND_GET_CASE6(addFunction, getFunction, code) \
    Skills skill;                                                \
    for (int index = 0; index < cycleCount; index++) {           \
        skill.addFunction(normalString);                         \
    }                                                            \
    GETFUNCTION(skill, getFunction, 0, normalString, code)

#define SKILLS_ADD_AND_GET_CASE7(addFunction, getFunction, code) \
    Skills skill;                                                \
    for (int index = 0; index < cycleCount; index++) {           \
        skill.addFunction(normalString);                         \
    }                                                            \
    GETFUNCTION(skill, getFunction, 150, std::string(), code)

// Add and Has ST kit Case(Action,Entity,Authority,Scheme,SchemeSpecificPart)
#define SKILLS_ADD_AND_HAS_CASE1(addFunction, hasFunction, code) \
    Skills skill;                                                \
    for (int index = 0; index < cycleCount; index++) {           \
        skill.addFunction(normalString);                         \
    }                                                            \
    HASFUNCTION(skill, hasFunction, normalString, true, code)

#define SKILLS_ADD_AND_HAS_CASE2(hasFunction, code) \
    Skills skill;                                   \
    HASFUNCTION(skill, hasFunction, normalString, false, code)

#define SKILLS_ADD_AND_HAS_CASE3(addFunction, hasFunction, code) \
    Skills skill;                                                \
    skill.addFunction(specialString);                            \
    HASFUNCTION(skill, hasFunction, specialString, true, code)

// Add,Remove,Count and Has ST kit Case(Action,Entity,Authority,Scheme,SchemeSpecificPart)
#define SKILLS_ADD_AND_REMOVE_CASE1(addFunction, removeFunction, hasFunction, code) \
    Skills skill;                                                                   \
    skill.addFunction(specialString);                                               \
    skill.removeFunction(specialString);                                            \
    HASFUNCTION(skill, hasFunction, specialString, false, code)

#define SKILLS_ADD_AND_REMOVE_CASE2(addFunction, removeFunction, getFunction, code) \
    Skills skill;                                                                   \
    skill.addFunction(specialString);                                               \
    skill.removeFunction(specialString);                                            \
    GETFUNCTION(skill, getFunction, 0, std::string(), code)

#define SKILLS_ADD_AND_REMOVE_CASE3(addFunction, removeFunction, hasFunction, code) \
    Skills skill;                                                                   \
    for (int index = 0; index < cycleCount; index++) {                              \
        skill.addFunction(normalString);                                            \
    }                                                                               \
    skill.removeFunction(normalString);                                             \
    HASFUNCTION(skill, hasFunction, normalString, false, code)

#define SKILLS_ADD_AND_REMOVE_CASE4(addFunction, removeFunction, hasFunction, code) \
    Skills skill;                                                                   \
    for (int index = 0; index < cycleCount; index++) {                              \
        skill.addFunction(normalString);                                            \
    }                                                                               \
    skill.removeFunction(specialString);                                            \
    HASFUNCTION(skill, hasFunction, normalString, true, code)

#define SKILLS_ADD_AND_REMOVE_CASE5(addFunction, removeFunction, getFunction, code) \
    Skills skill;                                                                   \
    for (int index = 0; index < cycleCount; index++) {                              \
        skill.addFunction(normalString);                                            \
    }                                                                               \
    skill.removeFunction(normalString);                                             \
    GETFUNCTION(skill, getFunction, 150, std::string(), code)

#define SKILLS_ADD_AND_REMOVE_CASE6(addFunction, removeFunction, getFunction, code) \
    Skills skill;                                                                   \
    for (int index = 0; index < cycleCount; index++) {                              \
        skill.addFunction(normalString);                                            \
    }                                                                               \
    skill.removeFunction(specialString);                                            \
    GETFUNCTION(skill, getFunction, 0, normalString, code)

#define SKILLS_ADD_AND_REMOVE_CASE7(addFunction, removeFunction, countFunction, code) \
    Skills skill;                                                                     \
    skill.addFunction(specialString);                                                 \
    skill.removeFunction(specialString);                                              \
    COUNTFUNCTION(skill, countFunction, 0, code)

#define SKILLS_ADD_AND_REMOVE_CASE8(addFunction, removeFunction, countFunction, code) \
    Skills skill;                                                                     \
    for (int index = 0; index < cycleCount; index++) {                                \
        skill.addFunction(normalString);                                              \
    }                                                                                 \
    skill.removeFunction(normalString);                                               \
    COUNTFUNCTION(skill, countFunction, 0, code)

#define SKILLS_ADD_AND_REMOVE_CASE9(addFunction, removeFunction, countFunction, code) \
    Skills skill;                                                                     \
    for (int index = 0; index < cycleCount; index++) {                                \
        skill.addFunction(normalString);                                              \
    }                                                                                 \
    skill.removeFunction(specialString);                                              \
    COUNTFUNCTION(skill, countFunction, 1, code)

// Add and Count ST kit Case(Path,Type)
#define SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE1(addFunction, countFunction, code, addFunctionType) \
    Skills skill;                                                                                 \
    ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                           \
    COUNTFUNCTION(skill, countFunction, 1, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE2(countFunction, code) \
    Skills skill;                                                   \
    COUNTFUNCTION(skill, countFunction, 0, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE3(addFunction, countFunction, code, addFunctionType) \
    Skills skill;                                                                                 \
    for (int index = 0; index < cycleCount; index++) {                                            \
        ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                       \
    }                                                                                             \
    COUNTFUNCTION(skill, countFunction, 1, code)

// Add and Get ST kit Case(Path,Type)
#define SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE1(addFunction, getFunction, code, addFunctionType) \
    Skills skill;                                                                             \
    ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                       \
    GETFUNCTION(skill, getFunction, 0, normalPathAndType, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE2(addFunction, getFunction, code, addFunctionType) \
    Skills skill;                                                                             \
    ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                       \
    GETFUNCTION(skill, getFunction, 10, std::string(), code)

#define SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE3(addFunction, getFunction, code, addFunctionType) \
    Skills skill;                                                                             \
    ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                       \
    GETFUNCTION(skill, getFunction, -10, std::string(), code)

#define SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE4(getFunction, code) \
    Skills skill;                                               \
    GETFUNCTION(skill, getFunction, 0, std::string(), code)

#define SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE5(addFunction, getFunction, code, addFunctionType) \
    Skills skill;                                                                             \
    ADDFUNCTION(skill, addFunction, specialString, addFunctionType)                           \
    GETFUNCTION(skill, getFunction, 0, specialString, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE6(addFunction, getFunction, code, addFunctionType) \
    Skills skill;                                                                             \
    for (int index = 0; index < cycleCount; index++) {                                        \
        ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                   \
    }                                                                                         \
    GETFUNCTION(skill, getFunction, 0, normalPathAndType, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE7(addFunction, getFunction, code, addFunctionType) \
    Skills skill;                                                                             \
    for (int index = 0; index < cycleCount; index++) {                                        \
        ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                   \
    }                                                                                         \
    GETFUNCTION(skill, getFunction, 150, std::string(), code)

// Add and Has ST kit Case(Path,Type)
#define SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE1(addFunction, hasFunction, code, addFunctionType) \
    Skills skill;                                                                             \
    for (int index = 0; index < cycleCount; index++) {                                        \
        ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                   \
    }                                                                                         \
    HASFUNCTION(skill, hasFunction, normalPathAndType, true, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE2(hasFunction, code) \
    Skills skill;                                               \
    HASFUNCTION(skill, hasFunction, normalPathAndType, false, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE3(addFunction, hasFunction, code, addFunctionType) \
    Skills skill;                                                                             \
    ADDFUNCTION(skill, addFunction, specialString, addFunctionType)                           \
    HASFUNCTION(skill, hasFunction, specialString, true, code)

// Add,Remove,Count and Has ST kit Case(Path,Type)
#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE1(addFunction, removeFunction, hasFunction, code, addFunctionType) \
    Skills skill;                                                                                                \
    ADDFUNCTION(skill, addFunction, specialString, addFunctionType)                                              \
    REMOVEFUNCTION(skill, removeFunction, specialString, addFunctionType)                                        \
    HASFUNCTION(skill, hasFunction, specialString, false, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE2(addFunction, removeFunction, getFunction, code, addFunctionType) \
    Skills skill;                                                                                                \
    ADDFUNCTION(skill, addFunction, specialString, addFunctionType)                                              \
    REMOVEFUNCTION(skill, removeFunction, specialString, addFunctionType)                                        \
    GETFUNCTION(skill, getFunction, 0, std::string(), code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE3(addFunction, removeFunction, hasFunction, code, addFunctionType) \
    Skills skill;                                                                                                \
    for (int index = 0; index < cycleCount; index++) {                                                           \
        ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                                      \
    }                                                                                                            \
    REMOVEFUNCTION(skill, removeFunction, normalPathAndType, addFunctionType)                                    \
    HASFUNCTION(skill, hasFunction, normalPathAndType, false, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE4(addFunction, removeFunction, hasFunction, code, addFunctionType) \
    Skills skill;                                                                                                \
    for (int index = 0; index < cycleCount; index++) {                                                           \
        ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                                      \
    }                                                                                                            \
    REMOVEFUNCTION(skill, removeFunction, specialString, addFunctionType)                                        \
    HASFUNCTION(skill, hasFunction, normalPathAndType, true, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE5(addFunction, removeFunction, getFunction, code, addFunctionType) \
    Skills skill;                                                                                                \
    for (int index = 0; index < cycleCount; index++) {                                                           \
        ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                                      \
    }                                                                                                            \
    REMOVEFUNCTION(skill, removeFunction, normalPathAndType, addFunctionType)                                    \
    GETFUNCTION(skill, getFunction, 150, std::string(), code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE6(addFunction, removeFunction, getFunction, code, addFunctionType) \
    Skills skill;                                                                                                \
    for (int index = 0; index < cycleCount; index++) {                                                           \
        ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                                      \
    }                                                                                                            \
    REMOVEFUNCTION(skill, removeFunction, specialString, addFunctionType)                                        \
    GETFUNCTION(skill, getFunction, 0, normalPathAndType, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE7(addFunction, removeFunction, countFunction, code, addFunctionType) \
    Skills skill;                                                                                                  \
    ADDFUNCTION(skill, addFunction, specialString, addFunctionType)                                                \
    REMOVEFUNCTION(skill, removeFunction, specialString, addFunctionType)                                          \
    COUNTFUNCTION(skill, countFunction, 0, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE8(addFunction, removeFunction, countFunction, code, addFunctionType) \
    Skills skill;                                                                                                  \
    for (int index = 0; index < cycleCount; index++) {                                                             \
        ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                                        \
    }                                                                                                              \
    REMOVEFUNCTION(skill, removeFunction, normalPathAndType, addFunctionType)                                      \
    COUNTFUNCTION(skill, countFunction, 0, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE9(addFunction, removeFunction, countFunction, code, addFunctionType) \
    Skills skill;                                                                                                  \
    for (int index = 0; index < cycleCount; index++) {                                                             \
        ADDFUNCTION(skill, addFunction, normalPathAndType, addFunctionType)                                        \
    }                                                                                                              \
    REMOVEFUNCTION(skill, removeFunction, specialString, addFunctionType)                                          \
    COUNTFUNCTION(skill, countFunction, 1, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE10(addFunction, removeFunction, hasFunction, code) \
    Skills skill;                                                                                \
    skill.addFunction(specialString);                                                            \
    skill.addFunction(specialString, MatchType::PATTERN_PREFIX);                                 \
    skill.removeFunction(specialString);                                                         \
    HASFUNCTION(skill, hasFunction, specialString, true, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE11(addFunction, removeFunction, countFunction, code) \
    Skills skill;                                                                                  \
    skill.addFunction(specialString, MatchType::PATTERN_LITERAL);                                  \
    skill.addFunction(specialString, MatchType::PATTERN_PREFIX);                                   \
    skill.addFunction(specialString, MatchType::PATTERN_SIMPLE_GLOB);                              \
    skill.removeFunction(specialString);                                                           \
    COUNTFUNCTION(skill, countFunction, 2, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE12(addFunction, countFunction, code) \
    Skills skill;                                                                  \
    skill.addFunction(specialString);                                              \
    skill.addFunction(specialString, MatchType::PATTERN_LITERAL);                  \
    skill.addFunction(specialString, MatchType::PATTERN_PREFIX);                   \
    skill.addFunction(specialString, MatchType::PATTERN_SIMPLE_GLOB);              \
    COUNTFUNCTION(skill, countFunction, 3, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE13(addFunction, countFunction, code) \
    Skills skill;                                                                  \
    skill.addFunction(skillspecialTypeStr1);                                       \
    skill.addFunction(skillspecialTypeStr1, MatchType::PATTERN_LITERAL);           \
    skill.addFunction(skillspecialTypeStr1, MatchType::PATTERN_PREFIX);            \
    skill.addFunction(skillspecialTypeStr1, MatchType::PATTERN_SIMPLE_GLOB);       \
    COUNTFUNCTION(skill, countFunction, 0, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE14(addFunction, hasFunction, code) \
    Skills skill;                                                                \
    skill.addFunction(skillspecialTypeStr1);                                     \
    skill.addFunction(skillspecialTypeStr1, MatchType::PATTERN_LITERAL);         \
    skill.addFunction(skillspecialTypeStr1, MatchType::PATTERN_PREFIX);          \
    skill.addFunction(skillspecialTypeStr1, MatchType::PATTERN_SIMPLE_GLOB);     \
    HASFUNCTION(skill, hasFunction, skillspecialTypeStr1, false, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE15(addFunction, hasFunction, code) \
    Skills skill;                                                                \
    skill.addFunction(skillspecialTypeStr2);                                     \
    skill.addFunction(skillspecialTypeStr2, MatchType::PATTERN_LITERAL);         \
    skill.addFunction(skillspecialTypeStr2, MatchType::PATTERN_PREFIX);          \
    skill.addFunction(skillspecialTypeStr2, MatchType::PATTERN_SIMPLE_GLOB);     \
    HASFUNCTION(skill, hasFunction, skillspecialTypeStr1, true, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE16(addFunction, countFunction, code) \
    Skills skill;                                                                  \
    skill.addFunction(skillspecialTypeStr2);                                       \
    skill.addFunction(skillspecialTypeStr2, MatchType::PATTERN_LITERAL);           \
    skill.addFunction(skillspecialTypeStr2, MatchType::PATTERN_PREFIX);            \
    skill.addFunction(skillspecialTypeStr2, MatchType::PATTERN_SIMPLE_GLOB);       \
    COUNTFUNCTION(skill, countFunction, 3, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE17(addFunction, countFunction, code) \
    Skills skill;                                                                  \
    skill.addFunction(skillspecialTypeStr3);                                       \
    skill.addFunction(skillspecialTypeStr3, MatchType::PATTERN_LITERAL);           \
    skill.addFunction(skillspecialTypeStr3, MatchType::PATTERN_PREFIX);            \
    skill.addFunction(skillspecialTypeStr3, MatchType::PATTERN_SIMPLE_GLOB);       \
    COUNTFUNCTION(skill, countFunction, 3, code)

#define SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE18(addFunction, hasFunction, code) \
    Skills skill;                                                                \
    skill.addFunction(skillspecialTypeStr3);                                     \
    skill.addFunction(skillspecialTypeStr3, MatchType::PATTERN_LITERAL);         \
    skill.addFunction(skillspecialTypeStr3, MatchType::PATTERN_PREFIX);          \
    skill.addFunction(skillspecialTypeStr3, MatchType::PATTERN_SIMPLE_GLOB);     \
    HASFUNCTION(skill, hasFunction, skillspecialTypeStr3, true, code)

// Skills Add ST Case
#define SKILLS_ADD_CASE1(addFunction, countFunction, code) \
    Skills skill;                                          \
    skill.addFunction(normalString);                       \
    skill.addFunction(specialString);                      \
    skill.addFunction(specialString);                      \
    skill.addFunction(normalPathAndType);                  \
    skill.addFunction("Ams_Kit_ST_Case");                  \
    COUNTFUNCTION(skill, countFunction, 4, code)

void FourthAbility::SubscribeEvent(const vector_conststr &eventList)
{
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber = std::make_shared<KitTestFourthEventSubscriber>(subscribeInfo, this);
    CommonEventManager::SubscribeCommonEvent(subscriber);
}

void FourthAbility::SkillsApiStByCode(int apiIndex, int caseIndex, int code)
{
    APP_LOGI("FourthAbility::SkillsApiStByCode");
    if (mapStKitFunc_.find(apiIndex) != mapStKitFunc_.end() &&
        static_cast<int>(mapStKitFunc_[apiIndex].size()) > caseIndex) {
        mapStKitFunc_[apiIndex][caseIndex](code);
    } else {
        APP_LOGI("SkillsApiStByCode error");
    }
}

bool FourthAbility::CompareEntity(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2)
{
    bool equalEntity = (skills1.CountEntities() == skills2.CountEntities());
    if (equalEntity) {
        int count = 0;
        count = skills1.CountEntities();
        for (int i = 0; i < count; i++) {
            bool result = (skills1.GetEntity(i) == skills2.GetEntity(i));
            if (!result) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool FourthAbility::CompareAction(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2)
{
    bool equalAction = (skills1.CountActions() == skills2.CountActions());
    if (equalAction) {
        int count = 0;
        count = skills1.CountActions();
        for (int i = 0; i < count; i++) {
            bool result = (skills1.GetAction(i) == skills2.GetAction(i));
            if (!result) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool FourthAbility::CompareAuthority(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2)
{
    bool equalAuthority = (skills1.CountAuthorities() == skills2.CountAuthorities());
    if (equalAuthority) {
        int count = 0;
        count = skills1.CountAuthorities();
        for (int i = 0; i < count; i++) {
            bool result = (skills1.GetAuthority(i) == skills2.GetAuthority(i));
            if (!result) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool FourthAbility::CompareScheme(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2)
{
    bool equalScheme = (skills1.CountSchemes() == skills2.CountSchemes());
    if (equalScheme) {
        int count = 0;
        count = skills1.CountSchemes();
        for (int i = 0; i < count; i++) {
            bool result = (skills1.GetScheme(i) == skills2.GetScheme(i));
            if (!result) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool FourthAbility::CompareSPath(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2)
{
    bool equalPath = (skills1.CountPaths() == skills2.CountPaths());
    if (equalPath) {
        int count = 0;
        count = skills1.CountPaths();
        for (int i = 0; i < count; i++) {
            bool result = (skills1.GetPath(i) == skills2.GetPath(i));
            if (!result) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool FourthAbility::CompareSsp(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2)
{
    bool equalSsp = (skills1.CountSchemeSpecificParts() == skills2.CountSchemeSpecificParts());
    if (equalSsp) {
        int count = 0;
        count = skills1.CountSchemeSpecificParts();
        for (int i = 0; i < count; i++) {
            bool result = (skills1.GetSchemeSpecificPart(i) == skills2.GetSchemeSpecificPart(i));
            if (!result) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool FourthAbility::CompareType(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2)
{
    bool equalType = (skills1.CountTypes() == skills2.CountTypes());
    if (equalType) {
        int count = 0;
        count = skills1.CountTypes();
        for (int i = 0; i < count; i++) {
            bool result = (skills1.GetType(i) == skills2.GetType(i));
            if (!result) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool FourthAbility::CompareWantParams(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2)
{
    std::set<std::string> key1;
    std::set<std::string> key2;
    key1 = skills1.GetWantParams().KeySet();
    key2 = skills2.GetWantParams().KeySet();
    bool equalKeyCount = (key1.size() == key2.size());
    if (equalKeyCount) {
        if (key1.size() != 0) {
            std::set<std::string>::iterator iter1 = key1.begin();
            std::set<std::string>::iterator iter2 = key2.begin();
            for (; iter1 != key1.end() && iter2 != key2.end(); iter1++, iter2++) {
                bool result = (*iter1 == *iter2);
                if (!result) {
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

bool FourthAbility::CompareSkills(const AAFwk::Skills &skills1, const AAFwk::Skills &skills2)
{
    return (CompareEntity(skills1, skills2) && CompareAction(skills1, skills2) && CompareAuthority(skills1, skills2) &&
            CompareScheme(skills1, skills2) && CompareSPath(skills1, skills2) && CompareSsp(skills1, skills2) &&
            CompareType(skills1, skills2) && CompareWantParams(skills1, skills2));
}

// Skills API ST Start
// AddAction and CountActions ST kit Case
void FourthAbility::SkillsAddActionAndCountActionsCase1(int code)
{
    SKILLS_ADD_AND_COUNT_CASE1(AddAction, CountActions, code);
}

void FourthAbility::SkillsAddActionAndCountActionsCase2(int code)
{
    SKILLS_ADD_AND_COUNT_CASE2(CountActions, code);
}

void FourthAbility::SkillsAddActionAndCountActionsCase3(int code)
{
    SKILLS_ADD_AND_COUNT_CASE3(AddAction, CountActions, code);
}

// AddAction and GetAction ST kit Case
void FourthAbility::SkillsAddActionAndGetActionCase1(int code)
{
    SKILLS_ADD_AND_GET_CASE1(AddAction, GetAction, code);
}

void FourthAbility::SkillsAddActionAndGetActionCase2(int code)
{
    SKILLS_ADD_AND_GET_CASE2(AddAction, GetAction, code);
}

void FourthAbility::SkillsAddActionAndGetActionCase3(int code)
{
    SKILLS_ADD_AND_GET_CASE3(AddAction, GetAction, code);
}

void FourthAbility::SkillsAddActionAndGetActionCase4(int code)
{
    SKILLS_ADD_AND_GET_CASE4(GetAction, code);
}

void FourthAbility::SkillsAddActionAndGetActionCase5(int code)
{
    SKILLS_ADD_AND_GET_CASE5(AddAction, GetAction, code);
}

void FourthAbility::SkillsAddActionAndGetActionCase6(int code)
{
    SKILLS_ADD_AND_GET_CASE6(AddAction, GetAction, code);
}

void FourthAbility::SkillsAddActionAndGetActionCase7(int code)
{
    SKILLS_ADD_AND_GET_CASE7(AddAction, GetAction, code);
}

// AddAction and HasAction ST kit Case
void FourthAbility::SkillsAddActionAndHasActionCase1(int code)
{
    SKILLS_ADD_AND_HAS_CASE1(AddAction, HasAction, code);
}

void FourthAbility::SkillsAddActionAndHasActionCase2(int code)
{
    SKILLS_ADD_AND_HAS_CASE2(HasAction, code);
}

void FourthAbility::SkillsAddActionAndHasActionCase3(int code)
{
    SKILLS_ADD_AND_HAS_CASE3(AddAction, HasAction, code);
}
// AddAction,RemoveAction,CountActions and HasAction ST kit Case
void FourthAbility::SkillsAddActionAndRemoveActionCase1(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE1(AddAction, RemoveAction, HasAction, code);
}

void FourthAbility::SkillsAddActionAndRemoveActionCase2(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE2(AddAction, RemoveAction, GetAction, code);
}

void FourthAbility::SkillsAddActionAndRemoveActionCase3(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE3(AddAction, RemoveAction, HasAction, code);
}

void FourthAbility::SkillsAddActionAndRemoveActionCase4(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE4(AddAction, RemoveAction, HasAction, code);
}

void FourthAbility::SkillsAddActionAndRemoveActionCase5(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE5(AddAction, RemoveAction, GetAction, code);
}

void FourthAbility::SkillsAddActionAndRemoveActionCase6(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE6(AddAction, RemoveAction, GetAction, code);
}

void FourthAbility::SkillsAddActionAndRemoveActionCase7(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE7(AddAction, RemoveAction, CountActions, code);
}

void FourthAbility::SkillsAddActionAndRemoveActionCase8(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE8(AddAction, RemoveAction, CountActions, code);
}

void FourthAbility::SkillsAddActionAndRemoveActionCase9(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE9(AddAction, RemoveAction, CountActions, code);
}

// AddEntity and CountEntities ST kit Case
void FourthAbility::SkillsAddEntityAndCountEntitiesCase1(int code)
{
    SKILLS_ADD_AND_COUNT_CASE1(AddEntity, CountEntities, code);
}

void FourthAbility::SkillsAddEntityAndCountEntitiesCase2(int code)
{
    SKILLS_ADD_AND_COUNT_CASE2(CountEntities, code);
}

void FourthAbility::SkillsAddEntityAndCountEntitiesCase3(int code)
{
    SKILLS_ADD_AND_COUNT_CASE3(AddEntity, CountEntities, code);
}

// AddEntity and GetEntity ST kit Case
void FourthAbility::SkillsAddEntityAndGetEntityCase1(int code)
{
    SKILLS_ADD_AND_GET_CASE1(AddEntity, GetEntity, code);
}

void FourthAbility::SkillsAddEntityAndGetEntityCase2(int code)
{
    SKILLS_ADD_AND_GET_CASE2(AddEntity, GetEntity, code);
}

void FourthAbility::SkillsAddEntityAndGetEntityCase3(int code)
{
    SKILLS_ADD_AND_GET_CASE3(AddEntity, GetEntity, code);
}

void FourthAbility::SkillsAddEntityAndGetEntityCase4(int code)
{
    SKILLS_ADD_AND_GET_CASE4(GetEntity, code);
}

void FourthAbility::SkillsAddEntityAndGetEntityCase5(int code)
{
    SKILLS_ADD_AND_GET_CASE5(AddEntity, GetEntity, code);
}

void FourthAbility::SkillsAddEntityAndGetEntityCase6(int code)
{
    SKILLS_ADD_AND_GET_CASE6(AddEntity, GetEntity, code);
}

void FourthAbility::SkillsAddEntityAndGetEntityCase7(int code)
{
    SKILLS_ADD_AND_GET_CASE7(AddEntity, GetEntity, code);
}

// AddEntity and HasEntity ST kit Case
void FourthAbility::SkillsAddEntityAndHasEntityCase1(int code)
{
    SKILLS_ADD_AND_HAS_CASE1(AddEntity, HasEntity, code);
}

void FourthAbility::SkillsAddEntityAndHasEntityCase2(int code)
{
    SKILLS_ADD_AND_HAS_CASE2(HasEntity, code);
}

void FourthAbility::SkillsAddEntityAndHasEntityCase3(int code)
{
    SKILLS_ADD_AND_HAS_CASE3(AddEntity, HasEntity, code);
}

// AddEntity,RemoveEntity,CountEntities and HasEntity ST kit Case
void FourthAbility::SkillsAddEntityAndRemoveEntityCase1(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE1(AddEntity, RemoveEntity, HasEntity, code);
}

void FourthAbility::SkillsAddEntityAndRemoveEntityCase2(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE2(AddEntity, RemoveEntity, GetEntity, code);
}

void FourthAbility::SkillsAddEntityAndRemoveEntityCase3(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE3(AddEntity, RemoveEntity, HasEntity, code);
}

void FourthAbility::SkillsAddEntityAndRemoveEntityCase4(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE4(AddEntity, RemoveEntity, HasEntity, code);
}

void FourthAbility::SkillsAddEntityAndRemoveEntityCase5(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE5(AddEntity, RemoveEntity, GetEntity, code);
}

void FourthAbility::SkillsAddEntityAndRemoveEntityCase6(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE6(AddEntity, RemoveEntity, GetEntity, code);
}

void FourthAbility::SkillsAddEntityAndRemoveEntityCase7(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE7(AddEntity, RemoveEntity, CountEntities, code);
}

void FourthAbility::SkillsAddEntityAndRemoveEntityCase8(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE8(AddEntity, RemoveEntity, CountEntities, code);
}

void FourthAbility::SkillsAddEntityAndRemoveEntityCase9(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE9(AddEntity, RemoveEntity, CountEntities, code);
}

// AddAuthority and CountAuthorities ST kit Case
void FourthAbility::SkillsAddAuthorityAndCountAuthoritiesCase1(int code)
{
    SKILLS_ADD_AND_COUNT_CASE1(AddAuthority, CountAuthorities, code);
}

void FourthAbility::SkillsAddAuthorityAndCountAuthoritiesCase2(int code)
{
    SKILLS_ADD_AND_COUNT_CASE2(CountAuthorities, code);
}

void FourthAbility::SkillsAddAuthorityAndCountAuthoritiesCase3(int code)
{
    SKILLS_ADD_AND_COUNT_CASE3(AddAuthority, CountAuthorities, code);
}

// AddAuthority and GetAuthority ST kit Case
void FourthAbility::SkillsAddAuthorityAndGetAuthorityCase1(int code)
{
    SKILLS_ADD_AND_GET_CASE1(AddAuthority, GetAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndGetAuthorityCase2(int code)
{
    SKILLS_ADD_AND_GET_CASE2(AddAuthority, GetAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndGetAuthorityCase3(int code)
{
    SKILLS_ADD_AND_GET_CASE3(AddAuthority, GetAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndGetAuthorityCase4(int code)
{
    SKILLS_ADD_AND_GET_CASE4(GetAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndGetAuthorityCase5(int code)
{
    SKILLS_ADD_AND_GET_CASE5(AddAuthority, GetAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndGetAuthorityCase6(int code)
{
    SKILLS_ADD_AND_GET_CASE6(AddAuthority, GetAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndGetAuthorityCase7(int code)
{
    SKILLS_ADD_AND_GET_CASE7(AddAuthority, GetAuthority, code);
}

// AddAuthority and HasAuthority ST kit Case
void FourthAbility::SkillsAddAuthorityAndHasAuthorityCase1(int code)
{
    SKILLS_ADD_AND_HAS_CASE1(AddAuthority, HasAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndHasAuthorityCase2(int code)
{
    SKILLS_ADD_AND_HAS_CASE2(HasAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndHasAuthorityCase3(int code)
{
    SKILLS_ADD_AND_HAS_CASE3(AddAuthority, HasAuthority, code);
}

// AddAuthority,RemoveAuthority,CountAuthorities and HasAuthority ST kit Case
void FourthAbility::SkillsAddAuthorityAndRemoveAuthorityCase1(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE1(AddAuthority, RemoveAuthority, HasAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndRemoveAuthorityCase2(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE2(AddAuthority, RemoveAuthority, GetAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndRemoveAuthorityCase3(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE3(AddAuthority, RemoveAuthority, HasAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndRemoveAuthorityCase4(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE4(AddAuthority, RemoveAuthority, HasAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndRemoveAuthorityCase5(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE5(AddAuthority, RemoveAuthority, GetAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndRemoveAuthorityCase6(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE6(AddAuthority, RemoveAuthority, GetAuthority, code);
}

void FourthAbility::SkillsAddAuthorityAndRemoveAuthorityCase7(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE7(AddAuthority, RemoveAuthority, CountAuthorities, code);
}

void FourthAbility::SkillsAddAuthorityAndRemoveAuthorityCase8(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE8(AddAuthority, RemoveAuthority, CountAuthorities, code);
}

void FourthAbility::SkillsAddAuthorityAndRemoveAuthorityCase9(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE9(AddAuthority, RemoveAuthority, CountAuthorities, code);
}

// AddScheme and CountSchemes ST kit Case
void FourthAbility::SkillsAddSchemeAndCountSchemesCase1(int code)
{
    SKILLS_ADD_AND_COUNT_CASE1(AddScheme, CountSchemes, code);
}

void FourthAbility::SkillsAddSchemeAndCountSchemesCase2(int code)
{
    SKILLS_ADD_AND_COUNT_CASE2(CountSchemes, code);
}

void FourthAbility::SkillsAddSchemeAndCountSchemesCase3(int code)
{
    SKILLS_ADD_AND_COUNT_CASE3(AddScheme, CountSchemes, code);
}

// AddScheme and GetScheme ST kit Case
void FourthAbility::SkillsAddSchemeAndGetSchemeCase1(int code)
{
    SKILLS_ADD_AND_GET_CASE1(AddScheme, GetScheme, code);
}

void FourthAbility::SkillsAddSchemeAndGetSchemeCase2(int code)
{
    SKILLS_ADD_AND_GET_CASE2(AddScheme, GetScheme, code);
}

void FourthAbility::SkillsAddSchemeAndGetSchemeCase3(int code)
{
    SKILLS_ADD_AND_GET_CASE3(AddScheme, GetScheme, code);
}

void FourthAbility::SkillsAddSchemeAndGetSchemeCase4(int code)
{
    SKILLS_ADD_AND_GET_CASE4(GetScheme, code);
}

void FourthAbility::SkillsAddSchemeAndGetSchemeCase5(int code)
{
    SKILLS_ADD_AND_GET_CASE5(AddScheme, GetScheme, code);
}

void FourthAbility::SkillsAddSchemeAndGetSchemeCase6(int code)
{
    SKILLS_ADD_AND_GET_CASE6(AddScheme, GetScheme, code);
}

void FourthAbility::SkillsAddSchemeAndGetSchemeCase7(int code)
{
    SKILLS_ADD_AND_GET_CASE7(AddScheme, GetScheme, code);
}

// AddScheme and HasScheme ST kit Case
void FourthAbility::SkillsAddSchemeAndHasSchemeCase1(int code)
{
    SKILLS_ADD_AND_HAS_CASE1(AddScheme, HasScheme, code);
}

void FourthAbility::SkillsAddSchemeAndHasSchemeCase2(int code)
{
    SKILLS_ADD_AND_HAS_CASE2(HasScheme, code);
}

void FourthAbility::SkillsAddSchemeAndHasSchemeCase3(int code)
{
    SKILLS_ADD_AND_HAS_CASE3(AddScheme, HasScheme, code);
}

// AddScheme,RemoveScheme,CountSchemes and HasScheme ST kit Case
void FourthAbility::SkillsAddSchemeAndRemoveSchemeCase1(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE1(AddScheme, RemoveScheme, HasScheme, code);
}

void FourthAbility::SkillsAddSchemeAndRemoveSchemeCase2(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE2(AddScheme, RemoveScheme, GetScheme, code);
}

void FourthAbility::SkillsAddSchemeAndRemoveSchemeCase3(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE3(AddScheme, RemoveScheme, HasScheme, code);
}

void FourthAbility::SkillsAddSchemeAndRemoveSchemeCase4(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE4(AddScheme, RemoveScheme, HasScheme, code);
}

void FourthAbility::SkillsAddSchemeAndRemoveSchemeCase5(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE5(AddScheme, RemoveScheme, GetScheme, code);
}

void FourthAbility::SkillsAddSchemeAndRemoveSchemeCase6(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE6(AddScheme, RemoveScheme, GetScheme, code);
}

void FourthAbility::SkillsAddSchemeAndRemoveSchemeCase7(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE7(AddScheme, RemoveScheme, CountSchemes, code);
}

void FourthAbility::SkillsAddSchemeAndRemoveSchemeCase8(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE8(AddScheme, RemoveScheme, CountSchemes, code);
}

void FourthAbility::SkillsAddSchemeAndRemoveSchemeCase9(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE9(AddScheme, RemoveScheme, CountSchemes, code);
}

// AddSchemeSpecificPart and CountSchemeSpecificParts ST kit Case
void FourthAbility::SkillsAddSchemeSpecificPartAndCountSchemeSpecificPartsCase1(int code)
{
    SKILLS_ADD_AND_COUNT_CASE1(AddSchemeSpecificPart, CountSchemeSpecificParts, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndCountSchemeSpecificPartsCase2(int code)
{
    SKILLS_ADD_AND_COUNT_CASE2(CountSchemeSpecificParts, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndCountSchemeSpecificPartsCase3(int code)
{
    SKILLS_ADD_AND_COUNT_CASE3(AddSchemeSpecificPart, CountSchemeSpecificParts, code);
}

// AddSchemeSpecificPart and GetSchemeSpecificPart ST kit Case
void FourthAbility::SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase1(int code)
{
    SKILLS_ADD_AND_GET_CASE1(AddSchemeSpecificPart, GetSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase2(int code)
{
    SKILLS_ADD_AND_GET_CASE2(AddSchemeSpecificPart, GetSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase3(int code)
{
    SKILLS_ADD_AND_GET_CASE3(AddSchemeSpecificPart, GetSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase4(int code)
{
    SKILLS_ADD_AND_GET_CASE4(GetSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase5(int code)
{
    SKILLS_ADD_AND_GET_CASE5(AddSchemeSpecificPart, GetSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase6(int code)
{
    SKILLS_ADD_AND_GET_CASE6(AddSchemeSpecificPart, GetSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndGetSchemeSpecificPartCase7(int code)
{
    SKILLS_ADD_AND_GET_CASE7(AddSchemeSpecificPart, GetSchemeSpecificPart, code);
}

// AddSchemeSpecificPart and HasSchemeSpecificPart ST kit Case
void FourthAbility::SkillsAddSchemeSpecificPartAndHasSchemeSpecificPartCase1(int code)
{
    SKILLS_ADD_AND_HAS_CASE1(AddSchemeSpecificPart, HasSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndHasSchemeSpecificPartCase2(int code)
{
    SKILLS_ADD_AND_HAS_CASE2(HasSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndHasSchemeSpecificPartCase3(int code)
{
    SKILLS_ADD_AND_HAS_CASE3(AddSchemeSpecificPart, HasSchemeSpecificPart, code);
}

// AddSchemeSpecificPart,RemoveSchemeSpecificPart,CountSchemeSpecificParts and HasSchemeSpecificPart ST kit Case
void FourthAbility::SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase1(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE1(AddSchemeSpecificPart, RemoveSchemeSpecificPart, HasSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase2(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE2(AddSchemeSpecificPart, RemoveSchemeSpecificPart, GetSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase3(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE3(AddSchemeSpecificPart, RemoveSchemeSpecificPart, HasSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase4(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE4(AddSchemeSpecificPart, RemoveSchemeSpecificPart, HasSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase5(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE5(AddSchemeSpecificPart, RemoveSchemeSpecificPart, GetSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase6(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE6(AddSchemeSpecificPart, RemoveSchemeSpecificPart, GetSchemeSpecificPart, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase7(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE7(AddSchemeSpecificPart, RemoveSchemeSpecificPart, CountSchemeSpecificParts, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase8(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE8(AddSchemeSpecificPart, RemoveSchemeSpecificPart, CountSchemeSpecificParts, code);
}

void FourthAbility::SkillsAddSchemeSpecificPartAndRemoveSchemeSpecificPartCase9(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE9(AddSchemeSpecificPart, RemoveSchemeSpecificPart, CountSchemeSpecificParts, code);
}

// AddPath_String and CountPaths ST kit Case
void FourthAbility::SkillsAddPathStringAndCountPathsCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE1(AddPath, CountPaths, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndCountPathsCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE3(AddPath, CountPaths, code, AddFunctionType::Add_String);
}

// AddPath_String_MatchType and CountPaths ST kit Case
void FourthAbility::SkillsAddPathStringMatchTypeAndCountPathsCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE1(AddPath, CountPaths, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndCountPathsCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE3(AddPath, CountPaths, code, AddFunctionType::Add_String_MatchType);
}

// AddPath_PatternMatcher and CountPaths ST kit Case
void FourthAbility::SkillsAddPathPatternAndCountPathsCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE1(AddPath, CountPaths, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndCountPathsCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE3(AddPath, CountPaths, code, AddFunctionType::Add_PatternMatcher);
}

// CountPaths ST kit Case
void FourthAbility::SkillsCountPathsCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE2(CountPaths, code);
}

// AddPath_String and GetPath ST kit Case
void FourthAbility::SkillsAddPathStringAndGetPathCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE1(AddPath, GetPath, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndGetPathCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE2(AddPath, GetPath, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndGetPathCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE3(AddPath, GetPath, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndGetPathCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE5(AddPath, GetPath, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndGetPathCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE6(AddPath, GetPath, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndGetPathCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE7(AddPath, GetPath, code, AddFunctionType::Add_String);
}

// AddPath_String_MatchType and GetPath ST kit Case
void FourthAbility::SkillsAddPathStringMatchTypeAndGetPathCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE1(AddPath, GetPath, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndGetPathCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE2(AddPath, GetPath, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndGetPathCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE3(AddPath, GetPath, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndGetPathCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE5(AddPath, GetPath, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndGetPathCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE6(AddPath, GetPath, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndGetPathCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE7(AddPath, GetPath, code, AddFunctionType::Add_String_MatchType);
}

// AddPath_PatternMatcher and GetPath ST kit Case
void FourthAbility::SkillsAddPathPatternAndGetPathCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE1(AddPath, GetPath, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndGetPathCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE2(AddPath, GetPath, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndGetPathCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE3(AddPath, GetPath, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndGetPathCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE5(AddPath, GetPath, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndGetPathCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE6(AddPath, GetPath, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndGetPathCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE7(AddPath, GetPath, code, AddFunctionType::Add_PatternMatcher);
}

// GetPath ST kit Case
void FourthAbility::SkillsGetPathCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE4(GetPath, code);
}

// AddPath_String and HasPath ST kit Case
void FourthAbility::SkillsAddPathStringAndHasPathCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE1(AddPath, HasPath, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndHasPathCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE3(AddPath, HasPath, code, AddFunctionType::Add_String);
}

// AddPath_String_MatchType and HasPath ST kit Case
void FourthAbility::SkillsAddPathStringMatchTypeAndHasPathCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE1(AddPath, HasPath, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndHasPathCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE3(AddPath, HasPath, code, AddFunctionType::Add_String_MatchType);
}

// AddPath_PatternMatcher and HasPath ST kit Case
void FourthAbility::SkillsAddPathPatternAndHasPathCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE1(AddPath, HasPath, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndHasPathCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE3(AddPath, HasPath, code, AddFunctionType::Add_PatternMatcher);
}

// HasPath ST kit Case
void FourthAbility::SkillsHasPathCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE2(HasPath, code);
}

// AddPath_String and RemovePath ST kit Case
void FourthAbility::SkillsAddPathStringAndRemovePathStringCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE1(AddPath, RemovePath, HasPath, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndRemovePathStringCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE2(AddPath, RemovePath, GetPath, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndRemovePathStringCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE3(AddPath, RemovePath, HasPath, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndRemovePathStringCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE4(AddPath, RemovePath, HasPath, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndRemovePathStringCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE5(AddPath, RemovePath, GetPath, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndRemovePathStringCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE6(AddPath, RemovePath, GetPath, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndRemovePathStringCase7(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE7(AddPath, RemovePath, CountPaths, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndRemovePathStringCase8(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE8(AddPath, RemovePath, CountPaths, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddPathStringAndRemovePathStringCase9(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE9(AddPath, RemovePath, CountPaths, code, AddFunctionType::Add_String);
}

// AddPath_String_MatchType and RemovePathStringMatchType ST kit Case
void FourthAbility::SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE1(AddPath, RemovePath, HasPath, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE2(AddPath, RemovePath, GetPath, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE3(AddPath, RemovePath, HasPath, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE4(AddPath, RemovePath, HasPath, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE5(AddPath, RemovePath, GetPath, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE6(AddPath, RemovePath, GetPath, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase7(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE7(
        AddPath, RemovePath, CountPaths, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase8(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE8(
        AddPath, RemovePath, CountPaths, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddPathStringMatchTypeAndRemovePathStringMatchTypeCase9(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE9(
        AddPath, RemovePath, CountPaths, code, AddFunctionType::Add_String_MatchType);
}

// AddPath_PatternMatcher and RemovePathPatternMatcher ST kit Case
void FourthAbility::SkillsAddPathPatternAndRemovePathPatternMatcherCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE1(AddPath, RemovePath, HasPath, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndRemovePathPatternMatcherCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE2(AddPath, RemovePath, GetPath, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndRemovePathPatternMatcherCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE3(AddPath, RemovePath, HasPath, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndRemovePathPatternMatcherCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE4(AddPath, RemovePath, HasPath, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndRemovePathPatternMatcherCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE5(AddPath, RemovePath, GetPath, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndRemovePathPatternMatcherCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE6(AddPath, RemovePath, GetPath, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndRemovePathPatternMatcherCase7(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE7(AddPath, RemovePath, CountPaths, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndRemovePathPatternMatcherCase8(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE8(AddPath, RemovePath, CountPaths, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndRemovePathPatternMatcherCase9(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE9(AddPath, RemovePath, CountPaths, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddPathPatternAndRemovePathCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE10(AddPath, RemovePath, HasPath, code);
}

void FourthAbility::SkillsAddPathPatternAndRemovePathCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE11(AddPath, RemovePath, CountPaths, code);
}

void FourthAbility::SkillsAddPathPatternAndRemovePathCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE12(AddPath, CountPaths, code);
}

// AddType_String and CountTypes ST kit Case
void FourthAbility::SkillsAddTypeStringAndCountTypesCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE1(AddType, CountTypes, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndCountTypesCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE3(AddType, CountTypes, code, AddFunctionType::Add_String);
}

// AddType_String_MatchType and CountTypes ST kit Case
void FourthAbility::SkillsAddTypeStringMatchTypeAndCountTypesCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE1(AddType, CountTypes, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndCountTypesCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE3(AddType, CountTypes, code, AddFunctionType::Add_String_MatchType);
}

// AddType_PatternMatcher and CountTypes ST kit Case
void FourthAbility::SkillsAddTypePatternAndCountTypesCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE1(AddType, CountTypes, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndCountTypesCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE3(AddType, CountTypes, code, AddFunctionType::Add_PatternMatcher);
}

// CountTypes ST kit Case
void FourthAbility::SkillsCountTypesCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_COUNT_CASE2(CountTypes, code);
}

// AddType_String and GetType ST kit Case
void FourthAbility::SkillsAddTypeStringAndGetTypeCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE1(AddType, GetType, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndGetTypeCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE2(AddType, GetType, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndGetTypeCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE3(AddType, GetType, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndGetTypeCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE5(AddType, GetType, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndGetTypeCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE6(AddType, GetType, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndGetTypeCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE7(AddType, GetType, code, AddFunctionType::Add_String);
}

// AddType_String_MatchType and GetType ST kit Case
void FourthAbility::SkillsAddTypeStringMatchTypeAndGetTypeCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE1(AddType, GetType, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndGetTypeCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE2(AddType, GetType, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndGetTypeCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE3(AddType, GetType, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndGetTypeCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE5(AddType, GetType, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndGetTypeCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE6(AddType, GetType, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndGetTypeCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE7(AddType, GetType, code, AddFunctionType::Add_String_MatchType);
}

// AddType_PatternMatcher and GetType ST kit Case
void FourthAbility::SkillsAddTypePatternAndGetTypeCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE1(AddType, GetType, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndGetTypeCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE2(AddType, GetType, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndGetTypeCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE3(AddType, GetType, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndGetTypeCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE5(AddType, GetType, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndGetTypeCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE6(AddType, GetType, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndGetTypeCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE7(AddType, GetType, code, AddFunctionType::Add_PatternMatcher);
}

// GetType ST kit Case
void FourthAbility::SkillsGetTypeCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_GET_CASE4(GetType, code);
}

// AddType_String and HasType ST kit Case
void FourthAbility::SkillsAddTypeStringAndHasTypeCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE1(AddType, HasType, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndHasTypeCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE3(AddType, HasType, code, AddFunctionType::Add_String);
}

// AddType_String_MatchType and HasType ST kit Case
void FourthAbility::SkillsAddTypeStringMatchTypeAndHasTypeCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE1(AddType, HasType, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndHasTypeCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE3(AddType, HasType, code, AddFunctionType::Add_String_MatchType);
}

// AddType_PatternMatcher and HasType ST kit Case
void FourthAbility::SkillsAddTypePatternAndHasTypeCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE1(AddType, HasType, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndHasTypeCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE3(AddType, HasType, code, AddFunctionType::Add_PatternMatcher);
}

// HasType ST kit Case
void FourthAbility::SkillsHasTypeCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_HAS_CASE2(HasType, code);
}

// AddType_String and RemoveType ST kit Case
void FourthAbility::SkillsAddTypeStringAndRemoveTypeStringCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE1(AddType, RemoveType, HasType, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndRemoveTypeStringCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE2(AddType, RemoveType, GetType, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndRemoveTypeStringCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE3(AddType, RemoveType, HasType, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndRemoveTypeStringCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE4(AddType, RemoveType, HasType, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndRemoveTypeStringCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE5(AddType, RemoveType, GetType, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndRemoveTypeStringCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE6(AddType, RemoveType, GetType, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndRemoveTypeStringCase7(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE7(AddType, RemoveType, CountTypes, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndRemoveTypeStringCase8(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE8(AddType, RemoveType, CountTypes, code, AddFunctionType::Add_String);
}

void FourthAbility::SkillsAddTypeStringAndRemoveTypeStringCase9(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE9(AddType, RemoveType, CountTypes, code, AddFunctionType::Add_String);
}

// AddType_String_MatchType and RemoveTypeStringMatchType ST kit Case
void FourthAbility::SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE1(AddType, RemoveType, HasType, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE2(AddType, RemoveType, GetType, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE3(AddType, RemoveType, HasType, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE4(AddType, RemoveType, HasType, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE5(AddType, RemoveType, GetType, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE6(AddType, RemoveType, GetType, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase7(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE7(
        AddType, RemoveType, CountTypes, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase8(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE8(
        AddType, RemoveType, CountTypes, code, AddFunctionType::Add_String_MatchType);
}

void FourthAbility::SkillsAddTypeStringMatchTypeAndRemoveTypeStringMatchTypeCase9(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE9(
        AddType, RemoveType, CountTypes, code, AddFunctionType::Add_String_MatchType);
}

// AddType_PatternMatcher and RemoveTypePatternMatcher ST kit Case
void FourthAbility::SkillsAddTypePatternAndRemoveTypePatternMatcherCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE1(AddType, RemoveType, HasType, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypePatternMatcherCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE2(AddType, RemoveType, GetType, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypePatternMatcherCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE3(AddType, RemoveType, HasType, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypePatternMatcherCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE4(AddType, RemoveType, HasType, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypePatternMatcherCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE5(AddType, RemoveType, GetType, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypePatternMatcherCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE6(AddType, RemoveType, GetType, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypePatternMatcherCase7(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE7(AddType, RemoveType, CountTypes, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypePatternMatcherCase8(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE8(AddType, RemoveType, CountTypes, code, AddFunctionType::Add_PatternMatcher);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypePatternMatcherCase9(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE9(AddType, RemoveType, CountTypes, code, AddFunctionType::Add_PatternMatcher);
}

// AddType and RemoveType ST kit Case
void FourthAbility::SkillsAddTypePatternAndRemoveTypeCase1(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE10(AddType, RemoveType, HasType, code);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypeCase2(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE11(AddType, RemoveType, CountTypes, code);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypeCase3(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE12(AddType, CountTypes, code);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypeCase4(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE13(AddType, CountTypes, code);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypeCase5(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE14(AddType, HasType, code);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypeCase6(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE15(AddType, HasType, code);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypeCase7(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE16(AddType, CountTypes, code);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypeCase8(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE17(AddType, CountTypes, code);
}

void FourthAbility::SkillsAddTypePatternAndRemoveTypeCase9(int code)
{
    SKILLS_ADDFUNCTIONTYPE_AND_REMOVE_CASE18(AddType, HasType, code);
}

// AddEntity and GetEntities ST kit Case
void FourthAbility::SkillAddEntityAndGetEntitiesCase1(int code)
{
    Skills skill;
    skill.AddEntity(normalString);
    bool result = (skill.GetEntities().size() == 1);
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result));
}

void FourthAbility::SkillAddEntityAndGetEntitiesCase2(int code)
{
    Skills skill;
    skill.AddEntity(normalString);
    skill.AddEntity(specialString);
    skill.AddEntity("ams_kit_test");
    skill.AddEntity(normalString);
    skill.AddEntity(specialString);
    bool result = (skill.GetEntities().size() == 3);
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result));
}

void FourthAbility::SkillAddEntityAndGetEntitiesCase3(int code)
{
    Skills skill;
    skill.AddEntity(normalString);
    skill.AddEntity(specialString);
    skill.AddEntity("ams_kit_test");
    skill.AddEntity(normalString);
    skill.AddEntity(specialString);
    bool result = (skill.CountEntities() == static_cast<int>(skill.GetEntities().size()));
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result));
}

void FourthAbility::SkillAddEntityAndGetEntitiesCase4(int code)
{
    Skills skill;
    skill.AddEntity(normalString);
    bool result = (skill.GetEntity(0) == skill.GetEntities().at(0));
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result));
}

void FourthAbility::SkillAddEntityAndGetEntitiesCase5(int code)
{
    Skills skill;
    skill.AddEntity(normalString);
    skill.AddEntity(specialString);
    skill.RemoveEntity(normalString);
    bool result = (specialString == skill.GetEntities().at(0));
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result));
}

// SetWantParams and GetWantParams ST kit Case
void FourthAbility::SkillSetAndGetWantParamsCase1(int code)
{
    SkillsGetWantParams<Boolean, IBoolean, bool>(true, code);
}

void FourthAbility::SkillSetAndGetWantParamsCase2(int code)
{
    SkillsGetWantParams<Long, ILong, long>(12345, code);
}

void FourthAbility::SkillSetAndGetWantParamsCase3(int code)
{
    SkillsGetWantParams<Integer, IInteger, int>(123, code);
}

void FourthAbility::SkillSetAndGetWantParamsCase4(int code)
{
    SkillsGetWantParams<String, IString, std::string>(specialString, code);
}

void FourthAbility::SkillSetAndGetWantParamsCase5(int code)
{
    SkillsGetWantParams<String, IString, std::string>(normalString, code);
}

void FourthAbility::SkillMatchCase1(int code)
{
    std::string strEntity = "entity.system.com";
    Skills skill;
    skill.AddAction(normalString);
    Want want;
    want.SetAction(normalString);
    skill.AddEntity(strEntity);
    want.AddEntity(strEntity);
    bool result = skill.Match(want);
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result));
}

void FourthAbility::SkillMatchCase2(int code)
{
    Skills skill;
    skill.AddAction(normalString);
    Want want;
    want.SetAction("");
    bool result = !skill.Match(want);
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result));
}

void FourthAbility::SkillMatchCase3(int code)
{
    Skills skill;
    std::string strEntity = "entity.system.com";
    skill.AddEntity(strEntity);
    Want want;
    want.AddEntity(strEntity);
    bool result = skill.Match(want);
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result));
}

void FourthAbility::SkillMatchCase4(int code)
{
    Skills skill;
    std::string strEntity = "entity.system.com";
    skill.AddEntity(strEntity);
    Want want;
    want.AddEntity("");
    bool result = !skill.Match(want);
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result));
}

void FourthAbility::SkillMatchCase5(int code)
{
    std::string strEntity = "entity.system.com";
    Skills skill;
    skill.AddAction(specialString);
    Want want;
    want.SetAction(normalString);
    skill.AddEntity(strEntity);
    want.AddEntity(strEntity);
    bool result = !skill.Match(want);
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result));
}

void FourthAbility::SkillMatchCase6(int code)
{
    Skills skill;
    std::string strEntity = "entity.system.com";
    skill.AddEntity(strEntity);
    Want want;
    want.AddEntity(specialString);
    bool result = !skill.Match(want);
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(result));
}

void FourthAbility::SkillUnmarshallingCase1(int code)
{
    Skills skillIn;
    Skills *skillOut = nullptr;
    skillIn.AddEntity("");
    skillIn.AddAction("");
    skillIn.AddAuthority("");
    skillIn.AddScheme("");
    skillIn.AddPath("");
    skillIn.AddSchemeSpecificPart("");
    skillIn.AddType("");
    WantParams wantParams;
    std::string keyStr = "";
    int valueInt = 123;
    wantParams.SetParam(keyStr, Integer::Box(valueInt));
    skillIn.SetWantParams(wantParams);
    Parcel in;
    skillIn.Marshalling(in);
    skillOut = Skills::Unmarshalling(in);
    if (skillOut == nullptr) {
        return;
    }

    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(CompareSkills(skillIn, *skillOut)));
    delete skillOut;
    skillOut = nullptr;
}

void FourthAbility::SkillUnmarshallingCase2(int code)
{
    Skills skillIn;
    Skills *skillOut = nullptr;
    skillIn.AddEntity(normalString);
    skillIn.AddAction(normalString);
    skillIn.AddAuthority(normalString);
    skillIn.AddScheme(normalString);
    skillIn.AddPath(normalString);
    skillIn.AddSchemeSpecificPart(normalString);
    skillIn.AddType(normalPathAndType);
    WantParams wantParams;
    std::string keyStr = normalString;
    int valueInt = 123;
    wantParams.SetParam(keyStr, Integer::Box(valueInt));
    skillIn.SetWantParams(wantParams);
    Parcel in;
    skillIn.Marshalling(in);
    skillOut = Skills::Unmarshalling(in);
    if (skillOut == nullptr) {
        return;
    }

    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(CompareSkills(skillIn, *skillOut)));
    delete skillOut;
    skillOut = nullptr;
}

void FourthAbility::SkillUnmarshallingCase3(int code)
{
    Skills skillIn;
    Skills *skillOut = nullptr;
    skillIn.AddEntity(specialString);
    skillIn.AddAction(specialString);
    skillIn.AddAuthority(specialString);
    skillIn.AddScheme(specialString);
    skillIn.AddPath(specialString);
    skillIn.AddSchemeSpecificPart(specialString);
    skillIn.AddType(skillspecialTypeStr2);
    WantParams wantParams;
    std::string keyStr = specialString;
    int valueInt = 123;
    wantParams.SetParam(keyStr, Integer::Box(valueInt));
    skillIn.SetWantParams(wantParams);
    Parcel in;
    skillIn.Marshalling(in);
    skillOut = Skills::Unmarshalling(in);
    if (skillOut == nullptr) {
        return;
    }

    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(CompareSkills(skillIn, *skillOut)));
    delete skillOut;
    skillOut = nullptr;
}

void FourthAbility::SkillUnmarshallingCase4(int code)
{
    Skills skillIn;
    Skills *skillOut = nullptr;
    skillIn.AddEntity("");
    skillIn.AddAction("");
    skillIn.AddAuthority("");
    skillIn.AddScheme("");
    skillIn.AddPath("");
    skillIn.AddSchemeSpecificPart("");
    skillIn.AddType("");
    skillIn.AddEntity(normalString);
    skillIn.AddAction(normalString);
    skillIn.AddAuthority(normalString);
    skillIn.AddScheme(normalString);
    skillIn.AddPath(normalString);
    skillIn.AddSchemeSpecificPart(normalString);
    skillIn.AddType(normalPathAndType);
    skillIn.AddEntity(specialString);
    skillIn.AddAction(specialString);
    skillIn.AddAuthority(specialString);
    skillIn.AddScheme(specialString);
    skillIn.AddPath(specialString);
    skillIn.AddSchemeSpecificPart(specialString);
    skillIn.AddType(skillspecialTypeStr2);
    WantParams wantParams;
    int valueInt = 123;
    std::string valueString = "kitST";
    wantParams.SetParam(normalString, Integer::Box(valueInt));
    wantParams.SetParam(specialString, String::Box(valueString));
    skillIn.SetWantParams(wantParams);
    Parcel in;
    skillIn.Marshalling(in);
    skillOut = Skills::Unmarshalling(in);
    if (skillOut == nullptr) {
        return;
    }
    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(CompareSkills(skillIn, *skillOut)));
    delete skillOut;
    skillOut = nullptr;
}

// Skills ST kit Case
void FourthAbility::SkillSkillsCase1(int code)
{
    SKILLS_ADD_AND_COUNT_CASE1(AddAction, CountActions, code);
}

void FourthAbility::SkillSkillsCase2(int code)
{
    SKILLS_ADD_AND_GET_CASE1(AddAction, GetAction, code);
}

void FourthAbility::SkillSkillsCase3(int code)
{
    SKILLS_ADD_AND_HAS_CASE1(AddEntity, HasEntity, code);
}

void FourthAbility::SkillSkillsCase4(int code)
{
    SKILLS_ADD_AND_REMOVE_CASE1(AddEntity, RemoveEntity, HasEntity, code);
}

// Skills_Skills ST kit Case
void FourthAbility::SkillSkillsSkillsCase1(int code)
{
    Skills skillIn;
    skillIn.AddEntity("");
    skillIn.AddAction("");
    skillIn.AddAuthority("");
    skillIn.AddScheme("");
    skillIn.AddPath("");
    skillIn.AddSchemeSpecificPart("");
    skillIn.AddType("");
    Skills skillOut(skillIn);

    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(CompareSkills(skillIn, skillOut)));
}

void FourthAbility::SkillSkillsSkillsCase2(int code)
{
    Skills skillIn;
    skillIn.AddEntity(normalString);
    skillIn.AddAction(normalString);
    skillIn.AddAuthority(normalString);
    skillIn.AddScheme(normalString);
    skillIn.AddPath(normalString);
    skillIn.AddSchemeSpecificPart(normalString);
    skillIn.AddType(normalPathAndType);
    Skills skillOut(skillIn);

    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(CompareSkills(skillIn, skillOut)));
}

void FourthAbility::SkillSkillsSkillsCase3(int code)
{
    Skills skillIn;
    skillIn.AddEntity(specialString);
    skillIn.AddAction(specialString);
    skillIn.AddAuthority(specialString);
    skillIn.AddScheme(specialString);
    skillIn.AddPath(specialString);
    skillIn.AddSchemeSpecificPart(specialString);
    skillIn.AddType(skillspecialTypeStr2);
    Skills skillOut(skillIn);

    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(CompareSkills(skillIn, skillOut)));
}

void FourthAbility::SkillSkillsSkillsCase4(int code)
{
    Skills skillIn;
    skillIn.AddEntity("");
    skillIn.AddAction("");
    skillIn.AddAuthority("");
    skillIn.AddScheme("");
    skillIn.AddPath("");
    skillIn.AddSchemeSpecificPart("");
    skillIn.AddType("");
    skillIn.AddEntity(normalString);
    skillIn.AddAction(normalString);
    skillIn.AddAuthority(normalString);
    skillIn.AddScheme(normalString);
    skillIn.AddPath(normalString);
    skillIn.AddSchemeSpecificPart(normalString);
    skillIn.AddType(normalPathAndType);
    skillIn.AddEntity(specialString);
    skillIn.AddAction(specialString);
    skillIn.AddAuthority(specialString);
    skillIn.AddScheme(specialString);
    skillIn.AddPath(specialString);
    skillIn.AddSchemeSpecificPart(specialString);
    skillIn.AddType(skillspecialTypeStr2);
    Skills skillOut(skillIn);

    TestUtils::PublishEvent(g_respPageFourthAbilityST, code, std::to_string(CompareSkills(skillIn, skillOut)));
}

// Skills AddAction ST kit Case
void FourthAbility::SkillAddActionCase1(int code)
{
    SKILLS_ADD_CASE1(AddAction, CountActions, code);
}

// Skills AddEntity ST kit Case
void FourthAbility::SkillAddEntityCase1(int code)
{
    SKILLS_ADD_CASE1(AddEntity, CountEntities, code);
}

// Skills AddAuthority ST kit Case
void FourthAbility::SkillAddAuthorityCase1(int code)
{
    SKILLS_ADD_CASE1(AddAuthority, CountAuthorities, code);
}

// Skills AddScheme ST kit Case
void FourthAbility::SkillAddSchemeCase1(int code)
{
    SKILLS_ADD_CASE1(AddScheme, CountSchemes, code);
}

// Skills AddSchemeSpecificPart ST kit Case
void FourthAbility::SkillAddSchemeSpecificPartCase1(int code)
{
    SKILLS_ADD_CASE1(AddSchemeSpecificPart, CountSchemeSpecificParts, code);
}

// Skills API ST End
void FourthAbility::OnStart(const Want &want)
{
    APP_LOGI("FourthAbility::onStart");
    Ability::OnStart(want);
    SubscribeEvent(g_requPageFourthAbilitySTVector);
    std::string eventData = GetAbilityName() + g_abilityStateOnStart;
    TestUtils::PublishEvent(g_respPageFourthAbilityST, 0, eventData);
}

void FourthAbility::OnStop()
{
    APP_LOGI("FourthAbility::onStop");
    Ability::OnStop();
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    std::string eventData = GetAbilityName() + g_abilityStateOnStop;
    TestUtils::PublishEvent(g_respPageFourthAbilityST, 0, eventData);
}

void FourthAbility::OnActive()
{
    APP_LOGI("FourthAbility::OnActive");
    Ability::OnActive();
    std::string eventData = GetAbilityName() + g_abilityStateOnActive;
    TestUtils::PublishEvent(g_respPageFourthAbilityST, 0, eventData);
}

void FourthAbility::OnInactive()
{
    APP_LOGI("FourthAbility::OnInactive");
    Ability::OnInactive();
    std::string eventData = GetAbilityName() + g_abilityStateOnInactive;
    TestUtils::PublishEvent(g_respPageFourthAbilityST, 0, eventData);
}

void FourthAbility::OnBackground()
{
    APP_LOGI("FourthAbility::OnBackground");
    Ability::OnBackground();
    std::string eventData = GetAbilityName() + g_abilityStateOnBackground;
    TestUtils::PublishEvent(g_respPageFourthAbilityST, 0, eventData);
}

void FourthAbility::OnForeground(const Want &want)
{
    APP_LOGI("FourthAbility::OnForeground");
    Ability::OnForeground(want);
    std::string eventData = GetAbilityName() + g_abilityStateOnForeground;
    TestUtils::PublishEvent(g_respPageFourthAbilityST, 0, eventData);
}

void FourthAbility::OnNewWant(const Want &want)
{
    APP_LOGI("FourthAbility::OnNewWant");
    Ability::OnNewWant(want);
    std::string eventData = GetAbilityName() + g_abilityStateOnNewWant;
    TestUtils::PublishEvent(g_respPageFourthAbilityST, 0, eventData);
}

void KitTestFourthEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("KitTestEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (g_requPageFourthAbilityST == eventName) {
        auto target = data.GetData();
        vector_str splitResult = TestUtils::split(target, "_");
        auto handle = 0;
        auto api = 1;
        auto code = 2;
        auto paramMinSize = 3;
        auto keyMap = splitResult.at(handle);
        if (mapTestFunc_.find(keyMap) != mapTestFunc_.end() &&
            splitResult.size() >= static_cast<unsigned int>(paramMinSize)) {
            auto apiIndex = atoi(splitResult.at(api).c_str());
            auto caseIndex = atoi(splitResult.at(code).c_str());
            mapTestFunc_[keyMap](apiIndex, caseIndex, data.GetCode());
        } else {
            if (keyMap == "TerminateAbility") {
                KitTerminateAbility();
            } else {
                APP_LOGI("OnReceiveEvent: CommonEventData error(%{public}s)", target.c_str());
            }
        }
    }
}

void KitTestFourthEventSubscriber::SkillsApiStByCode(int apiIndex, int caseIndex, int code)
{
    if (fourthAbility_ != nullptr) {
        fourthAbility_->SkillsApiStByCode(apiIndex, caseIndex, code);
    }
}

void KitTestFourthEventSubscriber::KitTerminateAbility()
{
    if (fourthAbility_ != nullptr) {
        fourthAbility_->TerminateAbility();
    }
}

REGISTER_AA(FourthAbility)
}  // namespace AppExecFwk
}  // namespace OHOS