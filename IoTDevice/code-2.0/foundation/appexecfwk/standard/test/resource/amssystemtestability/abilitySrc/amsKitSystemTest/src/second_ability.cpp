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

#include "second_ability.h"
#include "ohos/aafwk/base/base_types.h"
#include "app_log_wrapper.h"
#include "uri.h"
#include "ohos/aafwk/content/operation_builder.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using namespace AAFwk;
using namespace OHOS::EventFwk;

constexpr size_t ARRAY_SIZE = 10000;
constexpr int LARGE_STR_LEN = 65534;

bool SecondAbility::CompareWantNoParams(const Want &source, const Want &target)
{
    bool result = source.GetAction() == target.GetAction();
    result = result && source.GetFlags() == target.GetFlags();
    result = result && source.GetType() == target.GetType();
    result = result && source.GetEntities() == target.GetEntities();
    result = result && source.GetElement() == target.GetElement();
    return result;
}

void SecondAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("SecondAbility::Init");
    Ability::Init(abilityInfo, application, handler, token);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, Ability::GetState(), "Init");
}

void SecondAbility::OnStart(const Want &want)
{
    APP_LOGI("SecondAbility::onStart");
    SubscribeEvent();

    Ability::OnStart(want);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, Ability::GetState(), "OnStart");
}

void SecondAbility::OnStop()
{
    APP_LOGI("SecondAbility::onStop");
    Ability::OnStop();
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, Ability::GetState(), "OnStop");
}

void SecondAbility::OnActive()
{
    APP_LOGI("SecondAbility::OnActive");
    Ability::OnActive();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, Ability::GetState(), "OnActive");
}

void SecondAbility::OnInactive()
{
    APP_LOGI("SecondAbility::OnInactive");
    Ability::OnInactive();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, Ability::GetState(), "OnInactive");
}

void SecondAbility::OnBackground()
{
    APP_LOGI("SecondAbility::OnBackground");
    Ability::OnBackground();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, Ability::GetState(), "OnBackground");
}

void SecondAbility::OnForeground(const Want &want)
{
    APP_LOGI("SecondAbility::OnForeground");
    Ability::OnForeground(want);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, Ability::GetState(), "OnForeground");
}

void SecondAbility::OnAbilityResult(int requestCode, int resultCode, const Want &resultData)
{
    APP_LOGI("SecondAbility::OnAbilityResult");
    Ability::OnAbilityResult(requestCode, resultCode, resultData);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, Ability::GetState(), "OnAbilityResult");
}

void SecondAbility::OnBackPressed()
{
    APP_LOGI("SecondAbility::OnBackPressed");
    Ability::OnBackPressed();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, Ability::GetState(), "OnBackPressed");
}

void SecondAbility::OnNewWant(const Want &want)
{
    APP_LOGI("SecondAbility::OnNewWant");
    Ability::OnNewWant(want);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, Ability::GetState(), "OnNewWant");
}

void SecondAbility::SubscribeEvent()
{
    std::vector<std::string> eventList = {
        g_EVENT_REQU_SECOND,
    };
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber = std::make_shared<SecondEventSubscriber>(subscribeInfo, this);
    CommonEventManager::SubscribeCommonEvent(subscriber);
}

void SecondEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("SecondEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("SecondEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("SecondEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (std::strcmp(eventName.c_str(), g_EVENT_REQU_SECOND.c_str()) == 0) {
        auto target = data.GetData();
        auto caseInfo = TestUtils::split(target, "_");
        auto paramMinSize = 3;
        if (caseInfo.size() < static_cast<unsigned int>(paramMinSize)) {
            return;
        }
        if (mapTestFunc_.find(caseInfo[0]) != mapTestFunc_.end()) {
            mapTestFunc_[caseInfo[0]](std::stoi(caseInfo[1]), std::stoi(caseInfo[2]), data.GetCode());
        } else {
            APP_LOGI("OnReceiveEvent: CommonEventData error(%{public}s)", target.c_str());
        }
    }
}

void SecondAbility::TestWant(int apiIndex, int caseIndex, int code)
{
    APP_LOGI("SecondAbility::TestWant");
    if (mapCase_.find(apiIndex) != mapCase_.end()) {
        if (caseIndex < (int)mapCase_[apiIndex].size()) {
            mapCase_[apiIndex][caseIndex](code);
        }
    }
}

// copy constructor
void SecondAbility::WantCopyCase1(int code)
{
    Want want;
    std::string action = "WantCopyCase1";
    want = want.SetAction(action);
    Want wantCopy = Want(want);
    bool result = (wantCopy.GetAction().compare(action) == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// assignment constructor
void SecondAbility::WantAssignCase1(int code)
{
    Want want;
    std::string action = "WantAssignCase1";
    want = want.SetAction(action);
    Want wantAssign = want;
    bool result = (wantAssign.GetAction().compare(action) == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add empty entity
void SecondAbility::WantAddEntityCase1(int code)
{
    std::string empty;
    Want want;
    want.AddEntity("");
    auto entities = want.GetEntities();
    bool result = (entities.size() == 1);
    result = result && (entities.at(0) == empty);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add entity with special character
void SecondAbility::WantAddEntityCase2(int code)
{
    std::string entity = "~!@#$%^&*()_+`-=:\";\'<>?,./\\|\a\b\f\n\r\t\v\0\111\xAB";
    Want want;
    want.AddEntity(entity);
    auto entities = want.GetEntities();
    bool result = (entities.size() == 1);
    result = result && (entities.at(0) == entity);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add entity continuously
void SecondAbility::WantAddEntityCase3(int code)
{
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.AddEntity(std::string(i, 'A'));
    }
    bool result = (want.GetEntities().size() == pressureTimes);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add flags min
void SecondAbility::WantAddFlagsCase1(int code)
{
    unsigned flag = 0;
    Want want;
    want.AddFlags(flag);
    bool result = (want.GetFlags() == flag);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add flags max
void SecondAbility::WantAddFlagsCase2(int code)
{
    Want want;
    want.AddFlags(UINT32_MAX);
    bool result = (want.GetFlags() == UINT32_MAX);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add flags continuously
void SecondAbility::WantAddFlagsCase3(int code)
{
    Want want;
    unsigned int sum = 0;
    for (int i = 0; i < pressureTimes; i++) {
        want.AddFlags(i);
        sum |= static_cast<unsigned int>(i);
    }
    bool result = (want.GetFlags() == sum);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// clear want normally
void SecondAbility::WantClearWantCase1(int code)
{
    std::string empty;
    OHOS::AppExecFwk::ElementName emptyElementName;
    Want want;
    want.SetType("type");
    want.SetAction("action");
    want.SetFlags(1);
    auto elementName = OHOS::AppExecFwk::ElementName("deviceId", "bundleName", "abilityName");
    want.SetElement(elementName);
    AAFwk::WantParams parameters;
    std::string keyString = "keyString";
    parameters.SetParam(keyString, AAFwk::String::Box("valueString"));
    want.SetParams(parameters);
    want.AddEntity("wantEntity");
    Want::ClearWant(&want);
    bool result = (want.GetType() == empty);
    result = result && (want.GetAction() == empty);
    result = result && (want.GetFlags() == 0);
    result = result && (want.GetElement() == emptyElementName);
    result = result && (want.GetStringParam(keyString) == empty);
    result = result && (want.GetEntities().size() == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// clear want continuously
void SecondAbility::WantClearWantCase2(int code)
{
    std::string empty;
    Want want;
    want.SetType("type");
    for (int i = 0; i < pressureTimes; i++) {
        Want::ClearWant(&want);
    }
    bool result = (want.GetType() == empty);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// count entity on empty Want
void SecondAbility::WantCountEntitiesCase1(int code)
{
    Want want;
    bool result = (want.CountEntities() == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// count entity after add entity
void SecondAbility::WantCountEntitiesCase2(int code)
{
    Want want;
    const int entityCount = 3;
    for (int i = 0; i < entityCount; i++) {
        want.AddEntity("wantEntity" + std::to_string(i));
    }
    bool result = (want.CountEntities() == entityCount);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// format normal mime type
void SecondAbility::WantFormatMimeTypeCase1(int code)
{
    std::string mimeType = "WantFormatMimeType;Case1";
    std::string mimeTypeFormat = "wantformatmimetype";
    bool result = mimeTypeFormat.compare(Want::FormatMimeType(mimeType)) == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// format mime type uppercase
void SecondAbility::WantFormatMimeTypeCase2(int code)
{
    std::string mimeType = "WANTFORMATMIMETYPE/CASE1";
    std::string mimeTypeFormat = "wantformatmimetype/case1";
    bool result = mimeTypeFormat.compare(Want::FormatMimeType(mimeType)) == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// format mime type with special character
void SecondAbility::WantFormatMimeTypeCase3(int code)
{
    std::string mimeType = "WantFormatMimeType~!@#$%^&*()_+`-=:\"\\',.<>?;Case1";
    std::string mimeTypeFormat = "wantformatmimetype~!@#$%^&*()_+`-=:\"\\',.<>?";
    bool result = mimeTypeFormat.compare(Want::FormatMimeType(mimeType)) == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}
// format mime type with space and tab
void SecondAbility::WantFormatMimeTypeCase4(int code)
{
    std::string mimeType = "Want Format Mime \tType;Case1";
    std::string mimeTypeFormat = "wantformatmime\ttype";
    bool result = mimeTypeFormat.compare(Want::FormatMimeType(mimeType)) == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// format normal type
void SecondAbility::WantFormatTypeCase1(int code)
{
    std::string mimeType = "WantFormatMimeType;Case1";
    std::string mimeTypeFormat = "wantformatmimetype";
    Want want;
    want = want.FormatType(mimeType);
    bool result = want.GetType().compare(mimeTypeFormat) == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// format type uppercase
void SecondAbility::WantFormatTypeCase2(int code)
{
    std::string mimeType = "WANTFORMATMIMETYPE;CASE1";
    std::string mimeTypeFormat = "wantformatmimetype";
    Want want;
    want = want.FormatType(mimeType);
    bool result = want.GetType().compare(mimeTypeFormat) == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// format type with special character
void SecondAbility::WantFormatTypeCase3(int code)
{
    std::string mimeType = "WantFormatMimeType~!@#$%^&*()_+`-=:\"\\',.<>?;/Case1";
    std::string mimeTypeFormat = "wantformatmimetype~!@#$%^&*()_+`-=:\"\\',.<>?";
    Want want;
    want = want.FormatType(mimeType);
    bool result = want.GetType().compare(mimeTypeFormat) == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// format type with space, upper case and tab
void SecondAbility::WantFormatTypeCase4(int code)
{
    std::string mimeType = "Want Format Mime \tType;Case1";
    std::string mimeTypeFormat = "wantformatmime\ttype";
    Want want;
    want = want.FormatType(mimeType);
    bool result = want.GetType().compare(mimeTypeFormat) == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// format duplicated action
void SecondAbility::WantFormatUriCase1(int code)
{
    std::string uriString = "HTTP://www.baidu.com";
    Uri uri(uriString);
    Want want;
    want = want.FormatUri(uri);
    auto operation = want.GetOperation();
    bool result = (operation.GetUri().ToString().compare("http://www.baidu.com") == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// format ElementName
void SecondAbility::WantFormatUriCase2(int code)
{
    std::string uri = "WWW.baidu.com";
    Want want;
    want = want.FormatUri(uri);
    auto operation = want.GetOperation();
    bool result = (operation.GetUri().ToString().compare("WWW.baidu.com") == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// format string parameter
void SecondAbility::WantFormatUriCase3(int code)
{
    std::string uri = "";
    Want want;
    want = want.FormatUri(uri);
    auto operation = want.GetOperation();
    bool result = (operation.GetUri().ToString().compare("") == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// format return value of ToUri
void SecondAbility::WantFormatUriCase4(int code)
{
    std::string uri = "Http://";
    Want want;
    want = want.FormatUri(uri);
    auto operation = want.GetOperation();
    bool result = (operation.GetUri().ToString().compare("http://") == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

void SecondAbility::WantFormatUriAndTypeCase1(int code)
{
    bool result = true;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}
void SecondAbility::WantFormatUriAndTypeCase2(int code)
{
    bool result = true;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}
void SecondAbility::WantFormatUriAndTypeCase3(int code)
{
    bool result = true;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}
void SecondAbility::WantFormatUriAndTypeCase4(int code)
{
    bool result = true;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get empty action
void SecondAbility::WantGetActionCase1(int code)
{
    std::string empty;
    Want want;
    bool result = want.GetAction() == empty;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get action
void SecondAbility::WantGetActionCase2(int code)
{
    std::string action("action");
    Want want;
    want.SetAction(action);
    bool result = want.GetAction() == action;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get action continuously
void SecondAbility::WantGetActionCase3(int code)
{
    std::string action("action");
    Want want;
    want.SetAction(action);
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        result = result && (want.GetAction() == action);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get empty bundle
void SecondAbility::WantGetBundleCase1(int code)
{
    std::string empty;
    Want want;
    bool result = want.GetBundle() == empty;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get bundle
void SecondAbility::WantGetBundleCase2(int code)
{
    std::string bundle("bundle");
    Want want;
    want.SetBundle(bundle);
    bool result = want.GetBundle() == bundle;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get bundle continuously
void SecondAbility::WantGetBundleCase3(int code)
{
    std::string bundle("bundle");
    Want want;
    want.SetBundle(bundle);
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        result = result && (want.GetBundle() == bundle);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get empty entities
void SecondAbility::WantGetEntitiesCase1(int code)
{
    std::vector<std::string> empty;
    Want want;
    bool result = want.GetEntities() == empty;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add entity and get entities
void SecondAbility::WantGetEntitiesCase2(int code)
{
    std::vector<std::string> entities = {"entity1", "entity2", "entity3"};
    Want want;
    for (const auto &entity : entities) {
        want.AddEntity(entity);
    }
    bool result = want.GetEntities() == entities;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add entity and get entities continuously
void SecondAbility::WantGetEntitiesCase3(int code)
{
    std::vector<std::string> entities = {"entity1", "entity2", "entity3"};
    Want want;
    for (const auto &entity : entities) {
        want.AddEntity(entity);
    }
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        result = result && (want.GetEntities() == entities);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get empty element
void SecondAbility::WantGetElementCase1(int code)
{
    ElementName emptyElement;
    Want want;
    bool result = want.GetElement() == emptyElement;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get element
void SecondAbility::WantGetElementCase2(int code)
{
    ElementName element("deviceId", "bundleName", "abilityName");
    Want want;
    want.SetElement(element);
    bool result = want.GetElement() == element;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get element continuously
void SecondAbility::WantGetElementCase3(int code)
{
    ElementName element("deviceId", "bundleName", "abilityName");
    Want want;
    want.SetElement(element);
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        result = result && (want.GetElement() == element);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get empty uri
void SecondAbility::WantGetUriCase1(int code)
{
    Uri uri("");
    Want want;
    bool result = want.GetUri().Equals(uri);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get uri
void SecondAbility::WantGetUriCase2(int code)
{
    std::string uriString = "#Want;action=action;end";
    Uri uri(uriString);
    Want want;
    want.SetUri(uriString);
    bool result = want.GetUri().Equals(uri);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get uri continuously
void SecondAbility::WantGetUriCase3(int code)
{
    std::string uriString = "#Want;action=action;end";
    Uri uri(uriString);
    Want want;
    want.SetUri(uriString);
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        result = result && (want.GetUri().Equals(uri));
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get empty uri string
void SecondAbility::WantGetUriStringCase1(int code)
{
    std::string emptyUri("");
    Want want;
    bool result = want.GetUriString() == emptyUri;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set property and get uri string
void SecondAbility::WantGetUriStringCase2(int code)
{
    std::string uriString("#Want;action=action;end");
    Want want;
    want.SetAction(uriString);
    bool result = want.GetAction() == uriString;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set uri and get uri string
void SecondAbility::WantGetUriStringCase3(int code)
{
    std::string uriString("#Want;action=action;end");
    Want want;
    want.SetUri(uriString);
    bool result = want.GetUriString() == uriString;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get uri string continuously
void SecondAbility::WantGetUriStringCase4(int code)
{
    std::string uriString("#Want;action=action;end");
    Want want;
    want.SetUri(uriString);
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        result = result && (want.GetUriString() == uriString);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get empty flags
void SecondAbility::WantGetFlagsCase1(int code)
{
    Want want;
    bool result = want.GetFlags() == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get flags
void SecondAbility::WantGetFlagsCase2(int code)
{
    unsigned flags = 128;
    Want want;
    want.SetFlags(flags);
    bool result = want.GetFlags() == flags;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get flags continuously
void SecondAbility::WantGetFlagsCase3(int code)
{
    unsigned flags = 128;
    Want want;
    want.SetFlags(flags);
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        result = result && (want.GetFlags() == flags);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

void SecondAbility::WantGetSchemeCase1(int code)
{
    bool result = true;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}
void SecondAbility::WantGetSchemeCase2(int code)
{
    bool result = true;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}
void SecondAbility::WantGetSchemeCase3(int code)
{
    bool result = true;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}
void SecondAbility::WantGetSchemeCase4(int code)
{
    bool result = true;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get empty type
void SecondAbility::WantGetTypeCase1(int code)
{
    std::string empty;
    Want want;
    bool result = want.GetType() == empty;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get type
void SecondAbility::WantGetTypeCase2(int code)
{
    std::string type = "typeString";
    Want want;
    want.SetType(type);
    bool result = want.GetType() == type;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get type continuously
void SecondAbility::WantGetTypeCase3(int code)
{
    std::string type = "typeString";
    Want want;
    want.SetType(type);
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        result = result && (want.GetType() == type);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// no any entity
void SecondAbility::WantHasEntityCase1(int code)
{
    std::string entity = "entity";
    Want want;
    bool result = !want.HasEntity(entity);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// no target entity
void SecondAbility::WantHasEntityCase2(int code)
{
    std::string entity = "entity";
    std::string entity2 = "entity2";
    Want want;
    want.AddEntity(entity);
    bool result = !want.HasEntity(entity2);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// target entity exists
void SecondAbility::WantHasEntityCase3(int code)
{
    std::string entity = "entity";
    std::string entity2 = "entity2";
    Want want;
    want.AddEntity(entity);
    want.AddEntity(entity2);
    bool result = want.HasEntity(entity);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// HasEntity continuously
void SecondAbility::WantHasEntityCase4(int code)
{
    std::string entity = "entity";
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        std::string tmpEntity = entity + std::to_string(i);
        want.AddEntity(tmpEntity);
        result = result && want.HasEntity(tmpEntity);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// MakeMainAbility with empty ElementName
void SecondAbility::WantMakeMainAbilityCase1(int code)
{
    ElementName element;
    Want *want = Want::MakeMainAbility(element);
    bool result = want != nullptr;
    if (want != nullptr) {
        result = result && want->GetAction() == Want::ACTION_HOME;
        result = result && want->HasEntity(Want::ENTITY_HOME);
        result = result && want->GetElement() == element;
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// MakeMainAbility with normal ElementName
void SecondAbility::WantMakeMainAbilityCase2(int code)
{
    ElementName element("deviceId", "bundleName", "abilityName");
    Want *want = Want::MakeMainAbility(element);
    bool result = want != nullptr;
    if (want != nullptr) {
        result = result && want->GetAction() == Want::ACTION_HOME;
        result = result && want->HasEntity(Want::ENTITY_HOME);
        result = result && want->GetElement() == element;
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall empty Want
void SecondAbility::WantMarshallingCase1(int code)
{
    Want want;
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (CompareWantNoParams(want, *unmarshallWant));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with action
void SecondAbility::WantMarshallingCase2(int code)
{
    Want want;
    want.SetAction("action");
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetAction() == unmarshallWant->GetAction());
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with type
void SecondAbility::WantMarshallingCase3(int code)
{
    Want want;
    want.SetType("type");
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetType() == unmarshallWant->GetType());
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with flags
void SecondAbility::WantMarshallingCase4(int code)
{
    Want want;
    auto flag = 99;
    want.SetFlags(flag);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetFlags() == unmarshallWant->GetFlags());
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with entity
void SecondAbility::WantMarshallingCase5(int code)
{
    Want want;
    want.AddEntity("first entity");
    want.AddEntity("second entity");
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetEntities() == unmarshallWant->GetEntities());
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with element
void SecondAbility::WantMarshallingCase6(int code)
{
    Want want;
    ElementName element("deviceId", "bundleName", "abilityName");
    want.SetElement(element);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetElement() == unmarshallWant->GetElement());
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with bool parameter
void SecondAbility::WantMarshallingCase7(int code)
{
    Want want;
    std::string key = "key";
    bool value = true;
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetBoolParam(key, !value) == unmarshallWant->GetBoolParam(key, !value));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with bool array parameter
void SecondAbility::WantMarshallingCase8(int code)
{
    Want want;
    std::string key = "key";
    std::vector<bool> value = {true, true, false, true};
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetBoolArrayParam(key) == unmarshallWant->GetBoolArrayParam(key));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with byte parameter
void SecondAbility::WantMarshallingCase9(int code)
{
    Want want;
    std::string key = "key";
    byte value = 9;
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetByteParam(key, 0) == unmarshallWant->GetByteParam(key, 1));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with byte array parameter
void SecondAbility::WantMarshallingCase10(int code)
{
    Want want;
    std::string key = "key";
    std::vector<byte> value = {1, 2, 3, 4};
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetByteArrayParam(key) == unmarshallWant->GetByteArrayParam(key));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with char parameter
void SecondAbility::WantMarshallingCase11(int code)
{
    Want want;
    std::string key = "key";
    zchar value = 'a';
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetCharParam(key, 0) == unmarshallWant->GetCharParam(key, 1));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with char array parameter
void SecondAbility::WantMarshallingCase12(int code)
{
    Want want;
    std::string key = "key";
    std::vector<zchar> value = {'a', 'b', 'c', 'd'};
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetCharArrayParam(key) == unmarshallWant->GetCharArrayParam(key));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with int parameter
void SecondAbility::WantMarshallingCase13(int code)
{
    Want want;
    std::string key = "key";
    int value = 99;
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetIntParam(key, 0) == unmarshallWant->GetIntParam(key, 1));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with int array parameter
void SecondAbility::WantMarshallingCase14(int code)
{
    Want want;
    std::string key = "key";
    std::vector<int> value = {99, 999, 9999, 99999};
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetIntArrayParam(key) == unmarshallWant->GetIntArrayParam(key));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with double parameter
void SecondAbility::WantMarshallingCase15(int code)
{
    Want want;
    std::string key = "key";
    double value = 99.9;
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetDoubleParam(key, 0) == unmarshallWant->GetDoubleParam(key, 1));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with double array parameter
void SecondAbility::WantMarshallingCase16(int code)
{
    Want want;
    std::string key = "key";
    std::vector<double> value = {99.9, 999.9, 9999.9, 99999.9};
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetDoubleArrayParam(key) == unmarshallWant->GetDoubleArrayParam(key));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with float parameter
void SecondAbility::WantMarshallingCase17(int code)
{
    Want want;
    std::string key = "key";
    float value = 99.9f;
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetFloatParam(key, 0) == unmarshallWant->GetFloatParam(key, 1));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with float array parameter
void SecondAbility::WantMarshallingCase18(int code)
{
    Want want;
    std::string key = "key";
    std::vector<float> value = {99.9f, 999.9f, 9999.9f, 99999.9f};
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetFloatArrayParam(key) == unmarshallWant->GetFloatArrayParam(key));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with long parameter
void SecondAbility::WantMarshallingCase19(int code)
{
    Want want;
    std::string key = "key";
    long value = 99l;
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetLongParam(key, 0) == unmarshallWant->GetLongParam(key, 1));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with long array parameter
void SecondAbility::WantMarshallingCase20(int code)
{
    Want want;
    std::string key = "key";
    std::vector<long> value = {99l, 999l, 9999l, 99999l};
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetLongArrayParam(key) == unmarshallWant->GetLongArrayParam(key));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with short parameter
void SecondAbility::WantMarshallingCase21(int code)
{
    Want want;
    std::string key = "key";
    short value = 98;
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetShortParam(key, 0) == unmarshallWant->GetShortParam(key, 1));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with short array parameter
void SecondAbility::WantMarshallingCase22(int code)
{
    Want want;
    std::string key = "key";
    std::vector<short> value = {98, 998, 998, 998};
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetShortArrayParam(key) == unmarshallWant->GetShortArrayParam(key));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with string parameter
void SecondAbility::WantMarshallingCase23(int code)
{
    Want want;
    std::string key = "key";
    string value = "value";
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetStringParam(key) == unmarshallWant->GetStringParam(key));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// marshall and unmarshall Want with string array parameter
void SecondAbility::WantMarshallingCase24(int code)
{
    Want want;
    std::string key = "key";
    std::vector<string> value = {"aa", "bb", "cc", "dd"};
    want.SetParam(key, value);
    Parcel parcel;
    bool result = want.Marshalling(parcel);
    Want *unmarshallWant = Want::Unmarshalling(parcel);
    result = result && (unmarshallWant != nullptr);
    if (unmarshallWant) {
        result = result && (want.GetStringArrayParam(key) == unmarshallWant->GetStringArrayParam(key));
        delete unmarshallWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// remove entity from empty Want
void SecondAbility::WantRemoveEntityCase1(int code)
{
    Want want;
    std::string entity("entity");
    bool result = want.HasEntity(entity) == false;
    result = result && (want.CountEntities() == 0);
    want.RemoveEntity(entity);
    result = result && (want.HasEntity(entity) == false);
    result = result && (want.GetEntities().size() == 0);
    result = result && (want.CountEntities() == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add and remove normal entity
void SecondAbility::WantRemoveEntityCase2(int code)
{
    Want want;
    std::string entity("entity");
    want.AddEntity(entity);
    bool result = want.HasEntity(entity) == true;
    result = result && (want.CountEntities() == 1);
    want.RemoveEntity(entity);
    result = result && (want.HasEntity(entity) == false);
    result = result && (want.CountEntities() == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add and remove entity alternative
void SecondAbility::WantRemoveEntityCase3(int code)
{
    Want want;
    std::vector<std::string> entities{"entity1", "entity2", "entity3"};
    bool result = true;
    for (const auto &entity : entities) {
        want.AddEntity(entity);
        result = result && (want.HasEntity(entity) == true);
    }
    result = result && (want.GetEntities() == entities);
    result = result && (want.CountEntities() == (int)entities.size());
    for (const auto &entity : entities) {
        want.RemoveEntity(entity);
        result = result && (want.HasEntity(entity) == false);
    }
    result = result && (want.CountEntities() == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add and remove entity with special character
void SecondAbility::WantRemoveEntityCase4(int code)
{
    std::string entity = "~!@#$%^&*()_+`-={}|[]\\:\";\'<>?,./ \t";
    Want want;
    want.AddEntity(entity);
    bool result = want.HasEntity(entity) == true;
    want.RemoveEntity(entity);
    result = result && (want.HasEntity(entity) == false);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add entity repeatedly
void SecondAbility::WantRemoveEntityCase5(int code)
{
    std::string entity = "entity";
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.AddEntity(entity + std::to_string(i));
    }
    bool result = (want.CountEntities() == pressureTimes);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// remove flags from empty Want
void SecondAbility::WantRemoveFlagsCase1(int code)
{
    unsigned flags = 1;
    Want want;
    bool result = (want.GetFlags() == 0);
    want.RemoveFlags(flags);
    result = result && (want.GetFlags() == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add and remove flags
void SecondAbility::WantRemoveFlagsCase2(int code)
{
    unsigned flags = 1;
    Want want;
    bool result = (want.GetFlags() == 0);
    want.AddFlags(flags);
    result = result && (want.GetFlags() == flags);
    want.RemoveFlags(flags);
    result = result && (want.GetFlags() == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and remove flags
void SecondAbility::WantRemoveFlagsCase3(int code)
{
    unsigned flags = UINT_MAX;
    unsigned removedFlags = 1;
    Want want;
    bool result = (want.GetFlags() == 0);
    want.SetFlags(flags);
    result = result && (want.GetFlags() == flags);
    want.RemoveFlags(removedFlags);
    result = result && (want.GetFlags() == flags - removedFlags);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set, add and remove flags
void SecondAbility::WantRemoveFlagsCase4(int code)
{
    unsigned flags = 32;
    unsigned addFlags = 1024;
    unsigned notExistFlags = 1;
    Want want;
    bool result = (want.GetFlags() == 0);
    want.SetFlags(flags);
    want.AddFlags(addFlags);
    result = result && (want.GetFlags() == flags + addFlags);
    want.RemoveFlags(notExistFlags);
    result = result && (want.GetFlags() == flags + addFlags);
    want.RemoveFlags(addFlags);
    result = result && (want.GetFlags() == flags);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// remove flags repeatedly
void SecondAbility::WantRemoveFlagsCase5(int code)
{
    Want want;
    unsigned flags = UINT_MAX;
    unsigned removedFlags = 0;
    want.SetFlags(flags);
    for (unsigned i = 0; i < pressureTimes; i++) {
        want.RemoveFlags(i);
        removedFlags |= i;
    }
    bool result = (want.GetFlags() == flags - removedFlags);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse uri that has only key, no equals sign and value
void SecondAbility::WantParseUriCase1(int code)
{
    std::string uri = "#Want;action;end";
    Want *want = Want::ParseUri(uri);
    bool result = (want == nullptr);
    if (want) {
        delete want;
    }

    uri = "#Want;entity;end";
    want = Want::ParseUri(uri);
    result = result && (want == nullptr);
    if (want) {
        delete want;
    }

    uri = "#Want;device;end";
    want = Want::ParseUri(uri);
    result = result && (want == nullptr);
    if (want) {
        delete want;
    }

    uri = "#Want;bundle;end";
    want = Want::ParseUri(uri);
    result = result && (want == nullptr);
    if (want) {
        delete want;
    }

    uri = "#Want;ability;end";
    want = Want::ParseUri(uri);
    result = result && (want == nullptr);
    if (want) {
        delete want;
    }

    uri = "#Want;flag;end";
    want = Want::ParseUri(uri);
    result = result && (want == nullptr);
    if (want) {
        delete want;
    }

    uri = "#Want;param;end";
    want = Want::ParseUri(uri);
    result = result && (want == nullptr);
    if (want) {
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse string parameters
void SecondAbility::WantParseUriCase2(int code)
{
    std::string head = "#Want;";
    std::string end = ";end";
    std::string key = "keyString";
    std::string value = "valueString";
    std::string uriKey = AAFwk::String::SIGNATURE + std::string(".") + key + std::string("=");
    std::string uri = head + uriKey + value + end;
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want != nullptr) {
        result = result && (want->GetStringParam(key) == value);
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse uri that has special character
void SecondAbility::WantParseUriCase3(int code)
{
    std::string action = "\\";
    std::string entity = "../../../jj/j=075/./.;;/07507399/\\\\;;--==.com.\a\b\tfoobar.vide\073\\075";
    unsigned int flag = 0x0f0f0f0f;
    std::string key = "\\kkk=.=;";
    std::string value = "==\\\\\\.;\\;\\;\\=\\\073075\\\\075073";

    Want wantOrigin;
    wantOrigin.SetAction(action);
    wantOrigin.AddEntity(entity);
    wantOrigin.AddFlags(flag);
    wantOrigin.SetParam(key, value);
    std::string uri = wantOrigin.ToUri();

    Want *wantNew = Want::ParseUri(uri);
    bool result = (wantNew != nullptr);
    if (wantNew != nullptr) {
        result = result && (wantNew->GetAction().compare(action) == 0);
        for (auto entityItem : wantNew->GetEntities()) {
            result = result && (entityItem.compare(entity) == 0);
        }
        result = result && (wantNew->GetFlags() == flag);
        result = result && (wantNew->GetStringParam(key).compare(value) == 0);
        delete wantNew;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse flag that is a hexadecimal starting with "0X"
void SecondAbility::WantParseUriCase4(int code)
{
    unsigned int flag = 0X12345678;
    std::string uri = "#Want;flag=0X12345678;end";
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want != nullptr) {
        result = result && (want->GetFlags() == flag);
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse flag that has no value
void SecondAbility::WantParseUriCase5(int code)
{
    std::string uri = "#Want;flag=;end";
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want != nullptr) {
        result = result && (want->GetFlags() == static_cast<unsigned int>(0));
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse uri that has no head "#Want;"
void SecondAbility::WantParseUriCase6(int code)
{
    std::string uri = "action=want.action.VIEW;end";
    Want *want = Want::ParseUri(uri);
    bool result = (want == nullptr);
    if (want != nullptr) {
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse uri that flag type is string
void SecondAbility::WantParseUriCase7(int code)
{
    std::string uri = "#Want;action=want.action.VIEW;flag=\"123\";end";
    Want *want = Want::ParseUri(uri);
    bool result = (want == nullptr);
    if (want != nullptr) {
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse uri that has only all keys and equals sign, no value
void SecondAbility::WantParseUriCase8(int code)
{
    std::string empty;
    std::string uri = "#Want;action=;entity=;device=;bundle=;ability=;flag=;end";
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);

    if (want) {
        result = result && (want->GetAction() == empty);
        for (auto entityItem : want->GetEntities()) {
            result = result && (entityItem == empty);
        }
        result = result && (want->GetFlags() == (unsigned int)0);
        OHOS::AppExecFwk::ElementName element;
        result = result && (want->GetElement() == element);
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse empty string
void SecondAbility::WantParseUriCase9(int code)
{
    std::string uri;
    Want *want = Want::ParseUri(uri);
    bool result = (want == nullptr);
    if (want) {
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse parameter of float array and string array
void SecondAbility::WantParseUriCase10(int code)
{
    Want wantOrigin;
    std::string keyFloatArray = "keyFloatArray";
    std::string keyStringArray = "keyStringArray";
    std::vector<float> valueFloatArrayOrigin = {1.1f, 2.1f, 3.1f};
    std::vector<std::string> valueStringArrayOrigin = {"aa", "bb", "cc"};
    wantOrigin.SetParam(keyFloatArray, valueFloatArrayOrigin);
    wantOrigin.SetParam(keyStringArray, valueStringArrayOrigin);

    std::string uri = wantOrigin.ToUri();
    Want *wantNew = Want::ParseUri(uri);
    bool result = (wantNew != nullptr);

    if (wantNew != nullptr) {
        std::vector<float> arrayFloatNew = wantNew->GetFloatArrayParam(keyFloatArray);
        std::vector<float> arrayFloatOld = wantOrigin.GetFloatArrayParam(keyFloatArray);
        result = result && (arrayFloatNew == arrayFloatOld);

        std::vector<std::string> arrayStringNew = wantNew->GetStringArrayParam(keyStringArray);
        std::vector<std::string> arrayStringOld = wantOrigin.GetStringArrayParam(keyStringArray);
        result = result && (arrayStringNew == arrayStringOld);
        delete wantNew;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse parameter of float array
void SecondAbility::WantParseUriCase11(int code)
{
    Want wantOrigin;
    std::string keyFloatArray = "keyFloatArray";
    std::vector<float> valueFloatArrayOrigin = {1.1f, 2.1f, 3.1f};
    wantOrigin.SetParam(keyFloatArray, valueFloatArrayOrigin);

    std::string uri = wantOrigin.ToUri();
    Want *wantNew = Want::ParseUri(uri);
    bool result = (wantNew != nullptr);

    if (wantNew != nullptr) {
        std::vector<float> arrayNew = wantNew->GetFloatArrayParam(keyFloatArray);
        std::vector<float> arrayOld = wantOrigin.GetFloatArrayParam(keyFloatArray);
        result = result && (arrayNew == arrayOld);
        delete wantNew;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse parameter of float and string
void SecondAbility::WantParseUriCase12(int code)
{
    Want wantOrigin;
    std::string keyFloat = "keyFloat";
    std::string keyString = "keyString";
    float valueFloatOrigin = 123.4f;
    std::string valueStringOrigin = "abcd";
    wantOrigin.SetParam(keyFloat, valueFloatOrigin);
    wantOrigin.SetParam(keyString, valueStringOrigin);
    std::string uri = wantOrigin.ToUri();
    Want *wantNew = Want::ParseUri(uri);
    bool result = (wantNew != nullptr);

    if (wantNew) {
        float floatNew = wantNew->GetFloatParam(keyFloat, 0);
        float floatOld = wantOrigin.GetFloatParam(keyFloat, 1);
        result = result && (floatNew == floatOld);

        std::string stringNew = wantNew->GetStringParam(keyString);
        std::string stringOld = wantOrigin.GetStringParam(keyString);
        result = result && (stringNew == stringOld);
        delete wantNew;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse action, entity, flags, element
void SecondAbility::WantParseUriCase13(int code)
{
    std::string action = Want::ACTION_PLAY;
    std::string entity = Want::ENTITY_VIDEO;
    unsigned int flag = 0x0f0f0f0f;
    std::string device = "device1";
    std::string bundle = "bundle1";
    std::string ability = "ability1";
    OHOS::AppExecFwk::ElementName element(device, bundle, ability);

    Want wantOrigin;
    wantOrigin.SetAction(action);
    wantOrigin.AddEntity(entity);
    wantOrigin.AddFlags(flag);
    wantOrigin.SetElement(element);
    std::string uri = wantOrigin.ToUri();
    Want *wantNew = Want::ParseUri(uri);
    bool result = (wantNew != nullptr);

    if (wantNew) {
        result = result && (wantNew->GetAction() == action);
        for (auto entityItem : wantNew->GetEntities()) {
            result = result && (entityItem == entity);
        }
        result = result && (wantNew->GetElement().GetDeviceID() == device);
        result = result && (wantNew->GetElement().GetBundleName() == bundle);
        result = result && (wantNew->GetElement().GetAbilityName() == ability);
        result = result && (wantNew->GetFlags() == flag);
        delete wantNew;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse uri that make empty Want
void SecondAbility::WantParseUriCase14(int code)
{
    // "="
    std::string uri = "#Want;=;end";
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want) {
        delete want;
    }

    // "key="
    uri = "#Want;abc=;end";
    want = Want::ParseUri(uri);
    result = result && (want != nullptr);
    if (want) {
        delete want;
    }

    // "=value"
    uri = "#Want;=abc;end";
    want = Want::ParseUri(uri);
    result = result && (want != nullptr);
    if (want) {
        delete want;
    }

    // "param=value"
    uri = "#Want;xxxx=yyy;end";
    want = Want::ParseUri(uri);
    result = result && (want != nullptr);
    if (want) {
        delete want;
    }

    // empty string
    uri = "#Want;;;;;;end";
    want = Want::ParseUri(uri);
    result = result && (want != nullptr);
    if (want) {
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse uri that has only key and euqals sign, no value
void SecondAbility::WantParseUriCase15(int code)
{
    std::string uri = "#Want;action=;end";
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want) {
        delete want;
    }

    uri = "#Want;entity=;end";
    want = Want::ParseUri(uri);
    result = result && (want != nullptr);
    if (want) {
        delete want;
    }

    uri = "#Want;device=;end";
    want = Want::ParseUri(uri);
    result = result && (want != nullptr);
    if (want) {
        delete want;
    }

    uri = "#Want;bundle=;end";
    want = Want::ParseUri(uri);
    result = result && (want != nullptr);
    if (want) {
        delete want;
    }

    uri = "#Want;ability=;end";
    want = Want::ParseUri(uri);
    result = result && (want != nullptr);
    if (want) {
        delete want;
    }

    uri = "#Want;flag=;end";
    want = Want::ParseUri(uri);
    result = result && (want != nullptr);
    if (want) {
        delete want;
    }

    uri = "#Want;param=;end";
    want = Want::ParseUri(uri);
    result = result && (want != nullptr);
    if (want) {
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse Boolean parameters
void SecondAbility::WantParseUriCase16(int code)
{
    std::string head = "#Want;";
    std::string end = ";end";
    std::string key = "keyBoolean";
    std::string value = "true";
    std::string uriKey = AAFwk::Boolean::SIGNATURE + std::string(".") + key + std::string("=");
    std::string uri = head + uriKey + value + end;
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want != nullptr) {
        result = result && (want->GetBoolParam(key, false) == true);
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse Char parameters
void SecondAbility::WantParseUriCase17(int code)
{
    std::string head = "#Want;";
    std::string end = ";end";
    std::string key = "keyChar";
    AAFwk::zchar zcharValue = 'A';
    std::string uriKey = AAFwk::Char::SIGNATURE + std::string(".") + key + std::string("=");
    std::string uri = head + uriKey + "A" + end;
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want != nullptr) {
        result = result && (want->GetCharParam(key, 'B') == zcharValue);
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse Byte parameters
void SecondAbility::WantParseUriCase18(int code)
{
    std::string head = "#Want;";
    std::string end = ";end";
    std::string key = "keyByte";
    AAFwk::byte byteValue = 8;
    const AAFwk::byte defaultValue = 7;
    std::string uriKey = AAFwk::Byte::SIGNATURE + std::string(".") + key + std::string("=");
    std::string uri = head + uriKey + std::to_string(byteValue) + end;
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want != nullptr) {
        result = result && (want->GetByteParam(key, defaultValue) == byteValue);
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse Short parameters
void SecondAbility::WantParseUriCase19(int code)
{
    std::string head = "#Want;";
    std::string end = ";end";
    std::string key = "keyShort";
    short shortValue = 1;
    const short defaultValue = 2;
    std::string uriKey = AAFwk::Short::SIGNATURE + std::string(".") + key + std::string("=");
    std::string uri = head + uriKey + std::to_string(shortValue) + end;
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want != nullptr) {
        result = result && (want->GetShortParam(key, defaultValue) == shortValue);
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse Integer parameters
void SecondAbility::WantParseUriCase20(int code)
{
    std::string head = "#Want;";
    std::string end = ";end";
    std::string key = "keyInteger";
    int integerValue = 10;
    const short defaultValue = 20;
    std::string uriKey = AAFwk::Integer::SIGNATURE + std::string(".") + key + std::string("=");
    std::string uri = head + uriKey + std::to_string(integerValue) + end;
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want != nullptr) {
        result = result && (want->GetIntParam(key, defaultValue) == integerValue);
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse Float parameters
void SecondAbility::WantParseUriCase21(int code)
{
    std::string head = "#Want;";
    std::string end = ";end";
    std::string key = "keyFloat";
    float floatValue = 1000.9f;
    const float defaultValue = 20.0;
    const float diffValue = 0.000001;
    std::string uriKey = AAFwk::Float::SIGNATURE + std::string(".") + key + std::string("=");
    std::string uri = head + uriKey + std::to_string(floatValue) + end;
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want != nullptr) {
        result = result && (want->GetFloatParam(key, defaultValue) - floatValue <= diffValue);
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse Double parameters
void SecondAbility::WantParseUriCase22(int code)
{
    std::string head = "#Want;";
    std::string end = ";end";
    std::string key = "keyDouble";
    double doubleValue = 10000.99;
    const float defaultValue = 20.0;
    const float diffValue = 0.000001;
    std::string uriKey = AAFwk::Double::SIGNATURE + std::string(".") + key + std::string("=");
    std::string uri = head + uriKey + std::to_string(doubleValue) + end;
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want != nullptr) {
        result = result && (want->GetDoubleParam(key, defaultValue) - doubleValue <= diffValue);
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parse Array parameters
void SecondAbility::WantParseUriCase23(int code)
{
    std::string head = "#Want;";
    std::string end = ";end";
    std::string key = "keyArray";
    std::string value = AAFwk::String::SIGNATURE + std::string("3{aa,bb,cc}");
    std::string uriKey = AAFwk::Array::SIGNATURE + std::string(".") + key + std::string("=");
    std::string uri = head + uriKey + value + end;
    Want *want = Want::ParseUri(uri);
    bool result = (want != nullptr);
    if (want != nullptr) {
        auto parsedValue = want->GetStringArrayParam(key);
        result = result && (parsedValue == (std::vector<std::string>{"aa", "bb", "cc"}));
        delete want;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get empty action
void SecondAbility::WantSetActionCase1(int code)
{
    std::string empty;
    Want want;
    bool result = want.GetAction() == empty;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get empty action
void SecondAbility::WantSetActionCase2(int code)
{
    std::string empty;
    Want want;
    want.SetAction(empty);
    bool result = want.GetAction() == empty;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get long action
void SecondAbility::WantSetActionCase3(int code)
{
    std::string value(LARGE_STR_LEN, 'a');
    Want want;
    want.SetAction(value);
    bool result = want.GetAction() == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get action with special character
void SecondAbility::WantSetActionCase4(int code)
{
    std::string value = "~!@#$%^&*()_+`-={}|[]\\:\";\'<>?,./";
    Want want;
    want.SetAction(value);
    bool result = want.GetAction() == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get action continuously
void SecondAbility::WantSetActionCase5(int code)
{
    std::string value = "value";
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        std::string tmpValue = value + std::to_string(i);
        want.SetAction(tmpValue);
        result = result && want.GetAction() == tmpValue;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get empty bundle
void SecondAbility::WantSetBundleCase1(int code)
{
    std::string empty;
    Want want;
    bool result = want.GetBundle() == empty;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get empty bundle
void SecondAbility::WantSetBundleCase2(int code)
{
    std::string empty;
    Want want;
    want.SetBundle(empty);
    bool result = want.GetBundle() == empty;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get long bundle
void SecondAbility::WantSetBundleCase3(int code)
{
    std::string value(LARGE_STR_LEN, 'a');
    Want want;
    want.SetBundle(value);
    bool result = want.GetBundle() == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get bundle with special character
void SecondAbility::WantSetBundleCase4(int code)
{
    std::string value = "~!@#$%^&*()_+`-={}|[]\\:\";\'<>?,./";
    Want want;
    want.SetBundle(value);
    bool result = want.GetBundle() == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get bundle continuously
void SecondAbility::WantSetBundleCase5(int code)
{
    std::string value = "value";
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        std::string tmpValue = value + std::to_string(i);
        want.SetBundle(tmpValue);
        result = result && want.GetBundle() == tmpValue;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get empty element
void SecondAbility::WantSetElementCase1(int code)
{
    ElementName element;
    Want want;
    bool result = want.GetElement() == element;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get empty element
void SecondAbility::WantSetElementCase2(int code)
{
    ElementName element;
    Want want;
    want.SetElement(element);
    bool result = want.GetElement() == element;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get element continuously
void SecondAbility::WantSetElementCase3(int code)
{
    ElementName element("deviceId", "bundleName", "abilityName");
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        element.SetDeviceID("deviceId" + std::to_string(i));
        want.SetElement(element);
        result = result && want.GetElement() == element;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// SetElementName(string, string) and GetElement
// get empty ElementName
void SecondAbility::WantSetElementNameStringStringCase1(int code)
{
    std::string empty;
    Want want;
    bool result = want.GetElement().GetBundleName() == empty;
    result = result && (want.GetElement().GetAbilityName() == empty);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get empty ElementName
void SecondAbility::WantSetElementNameStringStringCase2(int code)
{
    std::string empty;
    Want want;
    want.SetElementName(empty, empty);
    bool result = want.GetElement().GetBundleName() == empty;
    result = result && (want.GetElement().GetAbilityName() == empty);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get long ElementName
void SecondAbility::WantSetElementNameStringStringCase3(int code)
{
    std::string bundleName(LARGE_STR_LEN, 'a');
    std::string abilityName(LARGE_STR_LEN, 'b');
    Want want;
    want.SetElementName(bundleName, abilityName);
    bool result = want.GetElement().GetBundleName() == bundleName;
    result = result && (want.GetElement().GetAbilityName() == abilityName);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get ElementName including special character
void SecondAbility::WantSetElementNameStringStringCase4(int code)
{
    std::string value = "~!@#$%^&*()_+`-={}|[]\\:\";\'<>?,./";
    Want want;
    want.SetElementName(value, value);
    bool result = want.GetElement().GetBundleName() == value;
    result = result && (want.GetElement().GetAbilityName() == value);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get ElementName continuously
void SecondAbility::WantSetElementNameStringStringCase5(int code)
{
    std::string bundleName;
    std::string abilityName;
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        bundleName = "bundleName" + std::to_string(i);
        abilityName = "abilityName" + std::to_string(i);
        want.SetElementName(bundleName, abilityName);
        result = result && want.GetElement().GetBundleName() == bundleName;
        result = result && (want.GetElement().GetAbilityName() == abilityName);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// SetElementName(string, string, string) and GetElement
// get empty ElementName
void SecondAbility::WantSetElementNameStringStringStringCase1(int code)
{
    std::string empty;
    Want want;
    bool result = want.GetElement().GetBundleName() == empty;
    result = result && (want.GetElement().GetAbilityName() == empty);
    result = result && (want.GetElement().GetDeviceID() == empty);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get empty ElementName
void SecondAbility::WantSetElementNameStringStringStringCase2(int code)
{
    std::string empty;
    Want want;
    want.SetElementName(empty, empty, empty);
    bool result = want.GetElement().GetBundleName() == empty;
    result = result && (want.GetElement().GetAbilityName() == empty);
    result = result && (want.GetElement().GetDeviceID() == empty);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get long ElementName
void SecondAbility::WantSetElementNameStringStringStringCase3(int code)
{
    std::string deviceId(LARGE_STR_LEN, 'a');
    std::string bundleName(LARGE_STR_LEN, 'b');
    std::string abilityName(LARGE_STR_LEN, 'c');
    Want want;
    want.SetElementName(deviceId, bundleName, abilityName);
    bool result = want.GetElement().GetBundleName() == bundleName;
    result = result && (want.GetElement().GetAbilityName() == abilityName);
    result = result && (want.GetElement().GetDeviceID() == deviceId);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get ElementName including special character
void SecondAbility::WantSetElementNameStringStringStringCase4(int code)
{
    std::string value = "~!@#$%^&*()_+`-={}|[]\\:\";\'<>?,./";
    Want want;
    want.SetElementName(value, value, value);
    bool result = want.GetElement().GetBundleName() == value;
    result = result && (want.GetElement().GetAbilityName() == value);
    result = result && (want.GetElement().GetDeviceID() == value);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get ElementName continuously
void SecondAbility::WantSetElementNameStringStringStringCase5(int code)
{
    std::string bundleName;
    std::string abilityName;
    std::string deviceId;
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        bundleName = "bundleName" + std::to_string(i);
        abilityName = "abilityName" + std::to_string(i);
        deviceId = "deviceId" + std::to_string(i);
        want.SetElementName(deviceId, bundleName, abilityName);
        result = result && want.GetElement().GetBundleName() == bundleName;
        result = result && (want.GetElement().GetAbilityName() == abilityName);
        result = result && (want.GetElement().GetDeviceID() == deviceId);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// GetFlags, SetFlags, AddFlags, RemoveFlags
// get empty flags
void SecondAbility::WantSetFlagsCase1(int code)
{
    Want want;
    bool result = want.GetFlags() == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set max unsigned flags
void SecondAbility::WantSetFlagsCase2(int code)
{
    Want want;
    want.SetFlags(UINT_MAX);
    bool result = want.GetFlags() == UINT_MAX;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// remove max unsigned flags
void SecondAbility::WantSetFlagsCase3(int code)
{
    Want want;
    want.RemoveFlags(UINT_MAX);
    bool result = want.GetFlags() == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and remove max unsigned flags
void SecondAbility::WantSetFlagsCase4(int code)
{
    Want want;
    want.SetFlags(UINT_MAX);
    bool result = want.GetFlags() == UINT_MAX;
    want.RemoveFlags(UINT_MAX);
    result = result && (want.GetFlags() == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add max unsigned flags
void SecondAbility::WantSetFlagsCase5(int code)
{
    Want want;
    want.AddFlags(UINT_MAX);
    bool result = want.GetFlags() == UINT_MAX;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add and remove max unsigned flags
void SecondAbility::WantSetFlagsCase6(int code)
{
    Want want;
    want.AddFlags(UINT_MAX);
    bool result = want.GetFlags() == UINT_MAX;
    want.RemoveFlags(UINT_MAX);
    result = result && (want.GetFlags() == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and add flags
void SecondAbility::WantSetFlagsCase7(int code)
{
    unsigned setValue = 2 ^ 7;
    unsigned addValue = 2 ^ 6;
    Want want;
    want.SetFlags(setValue);
    want.AddFlags(addValue);
    bool result = want.GetFlags() == (setValue | addValue);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add and set flags
void SecondAbility::WantSetFlagsCase8(int code)
{
    unsigned setValue = 2 ^ 7;
    unsigned addValue = 2 ^ 6;
    Want want;
    want.AddFlags(addValue);
    want.SetFlags(setValue);
    bool result = want.GetFlags() == setValue;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add one flags and remove another flags
void SecondAbility::WantSetFlagsCase9(int code)
{
    unsigned removeValue = 128;
    unsigned addValue = 64;
    Want want;
    want.AddFlags(addValue);
    want.RemoveFlags(removeValue);
    bool result = want.GetFlags() == addValue;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// add and remove flags continuously
void SecondAbility::WantSetFlagsCase10(int code)
{
    Want want;
    bool result = true;
    unsigned flags = want.GetFlags();
    for (unsigned i = 0; i < pressureTimes; i++) {
        want.AddFlags(i);
        flags |= i;
        result = result && (want.GetFlags() == flags);
    }
    unsigned expectValue = want.GetFlags();
    for (unsigned i = 0; i < pressureTimes; i++) {
        want.RemoveFlags(i);
        expectValue &= ~i;
        result = result && (want.GetFlags() == expectValue);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set flags continuously
void SecondAbility::WantSetFlagsCase11(int code)
{
    Want want;
    bool result = true;
    for (unsigned i = 0; i < pressureTimes; i++) {
        want.SetFlags(i);
        result = result && (want.GetFlags() == i);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set empty type
void SecondAbility::WantSetTypeCase1(int code)
{
    std::string type;
    Want want;
    want.SetType(type);
    bool result = (want.GetType() == type);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set twice and get once
void SecondAbility::WantSetTypeCase2(int code)
{
    std::string type1("type1");
    std::string type2("type2");
    Want want;
    want.SetType(type1);
    want.SetType(type2);
    bool result = (want.GetType() == type2);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set type with special character
void SecondAbility::WantSetTypeCase3(int code)
{
    std::string type = "~!@#$%^&*()_+`-={}|[]\\:\";'<>?,./ \t";
    Want want;
    want.SetType(type);
    bool result = (want.GetType() == type);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set type repeatedly
void SecondAbility::WantSetTypeCase4(int code)
{
    std::string type("type");
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetType(type + std::to_string(i));
    }
    bool result = (want.GetType() == (type + std::to_string(pressureTimes - 1)));
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set empty uri
void SecondAbility::WantSetUriCase1(int code)
{
    std::string uriString;
    Want want;
    want.SetUri(uriString);
    bool result = (want.GetUri().ToString() == uriString);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set uri with property action
void SecondAbility::WantSetUriCase2(int code)
{
    std::string uriString = "#Want;action=action;end";
    Uri uri(uriString);
    Want want;
    want.SetUri(uriString);
    bool result = want.GetUri().Equals(uri);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set uri with string parameter
void SecondAbility::WantSetUriCase3(int code)
{
    std::string strUri = "https://john.doe@www.example.com:123/forum/questions/?tag=networking#top";
    std::string paramString = "a;b:c;d=e;";
    Want want;
    want.SetUri(strUri);
    Want newWant;
    newWant.SetUri(want.GetUri());
    bool result = (newWant.GetUri().ToString() == strUri);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set uri continuously
void SecondAbility::WantSetUriCase4(int code)
{
    std::string head = "#Want;";
    std::string end = ";end";
    std::string propAction = "action=action";
    std::string uriString;
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        uriString = head + propAction + std::to_string(i) + end;
        want.SetUri(uriString);
        result = result && (want.GetUri().ToString() == uriString);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set empty uri and type
void SecondAbility::WantSetUriAndTypeCase1(int code)
{
    Uri empty("");
    std::string type;
    Want want;
    want.SetUriAndType(empty, type);
    bool result = (want.GetUri().ToString() == "");
    result = result && (want.GetType() == type);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set bad format uri and type
void SecondAbility::WantSetUriAndTypeCase2(int code)

{
    std::string empty;
    Uri uri("!@!#:adf;");
    std::string type(LARGE_STR_LEN, 'a');
    Want want;
    want.SetUriAndType(uri, type);
    bool result = (want.GetUri().Equals(uri));
    result = result && (want.GetType() == type);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set uri with ElementName and type
void SecondAbility::WantSetUriAndTypeCase3(int code)
{
    std::string empty;
    Uri uri("#Want;device=device;end");
    std::string type = "type";
    Want want;
    want.SetUriAndType(uri, type);
    bool result = (want.GetUri().Equals(uri));
    result = result && (want.GetType() == type);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set uri and type repeatedly
void SecondAbility::WantSetUriAndTypeCase4(int code)
{
    std::string type = "type";
    std::string uriString = "https://john.doe@www.example.com:123/forum/questions/?tag=networking#top";
    std::string typeValue;
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        typeValue = type + std::to_string(i);
        Uri uri(uriString);
        want.SetUriAndType(uri, typeValue);
        result = result && (want.GetUri().Equals(uri));
        result = result && (want.GetType() == typeValue);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty Want to uri
void SecondAbility::WantToUriCase1(int code)
{
    std::string head = "#Want;";
    std::string end = "end";
    std::string uriString = head + end;
    Want want;
    bool result = (want.ToUri() == uriString);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with action to uri
void SecondAbility::WantToUriCase2(int code)
{
    std::string value = "#Want;action=\"\\b\\\";end";
    Want want;
    want.SetAction(value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetAction() == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with entities to uri
void SecondAbility::WantToUriCase3(int code)
{
    std::string value = "#Want;entity=\"\\b\\\";end";
    Want want;
    want.AddEntity(value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->HasEntity(value));
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with flags to uri
void SecondAbility::WantToUriCase4(int code)
{
    unsigned value = UINT_MAX;
    Want want;
    want.SetFlags(value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetFlags() == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with ElementName to uri
void SecondAbility::WantToUriCase5(int code)
{
    std::string device = "#Want;device=\"\\b\\\";end";
    std::string bundle = "#Want;bundle=\"\\b\\\";end";
    std::string ability = "#Want;ability=\"\\b\\\";end";
    ElementName value(device, bundle, ability);
    Want want;
    want.SetElement(value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetElement() == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with string parameter to uri
void SecondAbility::WantToUriCase6(int code)
{
    std::string key = std::to_string(String::SIGNATURE) + ".#Want;key=\"\\b\\\";end";
    std::string value = "#Want;value=\"\\b\\\";end";
    Want want;
    want.SetParam(key, value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetStringParam(key) == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with bool parameter to uri
void SecondAbility::WantToUriCase7(int code)
{
    std::string key = std::to_string(Boolean::SIGNATURE) + ".#Want;key=\"\\b\\\";end";
    bool value = true;
    Want want;
    want.SetParam(key, value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetBoolParam(key, false) == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with char parameter to uri
void SecondAbility::WantToUriCase8(int code)
{
    std::string key = std::to_string(Char::SIGNATURE) + ".#Want;key=\"\\b\\\";end";
    zchar value = '.';
    Want want;
    want.SetParam(key, value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetCharParam(key, '=') == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with byte parameter to uri
void SecondAbility::WantToUriCase9(int code)
{
    std::string key = std::to_string(Byte::SIGNATURE) + ".#Want;key=\"\\b\\\";end";
    byte value = 9;
    Want want;
    want.SetParam(key, value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetByteParam(key, 0) == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with short parameter to uri
void SecondAbility::WantToUriCase10(int code)
{
    std::string key = std::to_string(Short::SIGNATURE) + ".#Want;key=\"\\b\\\";end";
    short value = 99;
    Want want;
    want.SetParam(key, value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetShortParam(key, 0) == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with int parameter to uri
void SecondAbility::WantToUriCase11(int code)
{
    std::string key = std::to_string(Integer::SIGNATURE) + ".#Want;key=\"\\b\\\";end";
    int value = 999;
    Want want;
    want.SetParam(key, value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetIntParam(key, 0) == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with long parameter to uri
void SecondAbility::WantToUriCase12(int code)
{
    std::string key = std::to_string(Long::SIGNATURE) + ".#Want;key=\"\\b\\\";end";
    long value = 9999l;
    Want want;
    want.SetParam(key, value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetLongParam(key, 0l) == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with float parameter to uri
void SecondAbility::WantToUriCase13(int code)
{
    std::string key = std::to_string(Float::SIGNATURE) + ".#Want;key=\"\\b\\\";end";
    float value = 9999.9f;
    Want want;
    want.SetParam(key, value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetFloatParam(key, 0) == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with double parameter to uri
void SecondAbility::WantToUriCase14(int code)
{
    std::string key = std::to_string(Double::SIGNATURE) + ".#Want;key=\"\\b\\\";end";
    double value = 9999.9;
    Want want;
    want.SetParam(key, value);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetDoubleParam(key, 0) == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with string array parameter to uri
void SecondAbility::WantToUriCase15(int code)
{
    std::string key = std::to_string(Array::SIGNATURE) + ".#Want;key=3{\"\\b\\\";end";
    std::string value = std::to_string(String::SIGNATURE) + "3{#Want;value=\"\\b\\\";end";
    std::vector<std::string> arrayValue = {value, value, value};
    Want want;
    want.SetParam(key, arrayValue);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetStringArrayParam(key) == arrayValue);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with bool array parameter to uri
void SecondAbility::WantToUriCase16(int code)
{
    std::string key = std::to_string(Array::SIGNATURE) + ".#Want;key=3{\"\\b\\\";end";
    std::vector<bool> arrayValue = {true, true, false};
    Want want;
    want.SetParam(key, arrayValue);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetBoolArrayParam(key) == arrayValue);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with char array parameter to uri
void SecondAbility::WantToUriCase17(int code)
{
    std::string key = std::to_string(Array::SIGNATURE) + ".#Want;key=3{\"\\b\\\";end";
    std::vector<zchar> arrayValue = {'.', '=', ';'};
    Want want;
    want.SetParam(key, arrayValue);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetCharArrayParam(key) == arrayValue);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with byte array parameter to uri
void SecondAbility::WantToUriCase18(int code)
{
    std::string key = std::to_string(Array::SIGNATURE) + ".#Want;key=3{\"\\b\\\";end";
    std::vector<byte> arrayValue = {1, 2, 3};
    Want want;
    want.SetParam(key, arrayValue);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetByteArrayParam(key) == arrayValue);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with short array parameter to uri
void SecondAbility::WantToUriCase19(int code)
{
    std::string key = std::to_string(Array::SIGNATURE) + ".#Want;key=3{\"\\b\\\";end";
    std::vector<short> arrayValue = {1, 2, 3};
    Want want;
    want.SetParam(key, arrayValue);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetShortArrayParam(key) == arrayValue);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with int array parameter to uri
void SecondAbility::WantToUriCase20(int code)
{
    std::string key = std::to_string(Array::SIGNATURE) + ".#Want;key=3{\"\\b\\\";end";
    std::vector<int> arrayValue = {1, 2, 3};
    Want want;
    want.SetParam(key, arrayValue);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetIntArrayParam(key) == arrayValue);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with long array parameter to uri
void SecondAbility::WantToUriCase21(int code)
{
    std::string key = std::to_string(Array::SIGNATURE) + ".#Want;key=3{\"\\b\\\";end";
    std::vector<long> arrayValue = {1l, 2l, 3l};
    Want want;
    want.SetParam(key, arrayValue);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetLongArrayParam(key) == arrayValue);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with float array parameter to uri
void SecondAbility::WantToUriCase22(int code)
{
    std::string key = std::to_string(Array::SIGNATURE) + ".#Want;key=3{\"\\b\\\";end";
    std::vector<float> arrayValue = {1, 2, 3};
    Want want;
    want.SetParam(key, arrayValue);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetFloatArrayParam(key) == arrayValue);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with double array parameter to uri
void SecondAbility::WantToUriCase23(int code)
{
    std::string key = std::to_string(Array::SIGNATURE) + ".#Want;key=3{\"\\b\\\";end";
    std::vector<double> arrayValue = {1.1, 2.2, 3.3};
    Want want;
    want.SetParam(key, arrayValue);
    Want *newWant = nullptr;
    newWant = Want::ParseUri(want.ToUri());
    bool result = false;
    if (newWant != nullptr) {
        result = (newWant->GetDoubleArrayParam(key) == arrayValue);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty Want to and from uri
void SecondAbility::WantWantParseUriCase1(int code)
{
    Want want;
    std::string uriString = want.WantToUri(want);
    Want *newWant = Want::WantParseUri(uriString.c_str());
    bool result = (newWant != nullptr);
    std::string empty;
    if (newWant) {
        result = result && (newWant->GetAction() == empty);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with property to and from uri
void SecondAbility::WantWantParseUriCase2(int code)
{
    Want want;
    std::string action = "action";
    want.SetAction(action);
    std::string uriString = want.WantToUri(want);
    Want *newWant = Want::WantParseUri(uriString.c_str());
    bool result = (newWant != nullptr);
    if (newWant) {
        result = result && (newWant->GetAction() == action);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// Want with parameter to and from uri
void SecondAbility::WantWantParseUriCase3(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want want;
    want.SetParam(key, value);
    std::string uriString = want.WantToUri(want);
    Want *newWant = Want::WantParseUri(uriString.c_str());
    bool result = (newWant != nullptr);
    if (newWant) {
        result = result && (newWant->GetStringParam(key) == value);
        delete newWant;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get params from empty Want
void SecondAbility::WantGetParamsCase1(int code)
{
    Want want;
    auto params = want.GetParams();
    bool result = params.IsEmpty();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get params
void SecondAbility::WantGetParamsCase2(int code)
{
    std::string key = "key";
    std::string value = "value";
    WantParams params;
    params.SetParam(key, String::Box(value));
    Want want;
    want.SetParams(params);
    bool result = want.GetParams().HasParam(key);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get params repeatedly
void SecondAbility::WantGetParamsCase3(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        WantParams params;
        std::string tmpKey = key + std::to_string(i);
        params.SetParam(tmpKey, String::Box(value));
        want.SetParams(params);
        result = result && (want.GetParams().HasParam(tmpKey));
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get byte on empty Want
void SecondAbility::WantGetByteParamCase1(int code)
{
    std::string key = "key";
    byte defaultValue = 7;
    Want want;
    bool result = want.GetByteParam(key, defaultValue) == defaultValue;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed byte param
void SecondAbility::WantGetByteParamCase2(int code)
{
    std::string key = "key";
    byte value = 10;
    byte defaultValue = 7;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetByteParam(key, defaultValue) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get byte param repeatedly
void SecondAbility::WantGetByteParamCase3(int code)
{
    std::string key = "key";
    byte value;
    byte defaultValue = 7;
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value = i % 128;
        want.SetParam(key, value);
        result = result && want.GetByteParam(key, defaultValue) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get byte array on empty Want
void SecondAbility::WantGetByteArrayParamCase1(int code)
{
    std::string key = "key";
    Want want;
    bool result = want.GetByteArrayParam(key).size() == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed byte array param
void SecondAbility::WantGetByteArrayParamCase2(int code)
{
    std::string key = "key";
    std::vector<byte> value = {1, 12, 127};
    Want want;
    want.SetParam(key, value);
    bool result = want.GetByteArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get byte array param repeatedly
void SecondAbility::WantGetByteArrayParamCase3(int code)
{
    std::string key = "key";
    std::vector<byte> value = {1, 12, 127};
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value[0] = i % 128;
        want.SetParam(key, value);
        result = result && want.GetByteArrayParam(key) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get bool on empty Want
void SecondAbility::WantGetBoolParamCase1(int code)
{
    std::string key = "key";
    bool defaultValue = true;
    Want want;
    bool result = want.GetBoolParam(key, defaultValue) == defaultValue;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed bool param
void SecondAbility::WantGetBoolParamCase2(int code)
{
    std::string key = "key";
    bool value = true;
    bool defaultValue = false;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetBoolParam(key, defaultValue) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get bool param repeatedly
void SecondAbility::WantGetBoolParamCase3(int code)
{
    std::string key = "key";
    bool value = false;
    bool defaultValue = false;
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value = i % 2 == 0;
        want.SetParam(key, value);
        result = result && want.GetBoolParam(key, defaultValue) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get bool array on empty Want
void SecondAbility::WantGetBoolArrayParamCase1(int code)
{
    std::string key = "key";
    Want want;
    bool result = want.GetBoolArrayParam(key).size() == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed bool array param
void SecondAbility::WantGetBoolArrayParamCase2(int code)
{
    std::string key = "key";
    std::vector<bool> value = {true, true, false};
    Want want;
    want.SetParam(key, value);
    bool result = want.GetBoolArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get bool array param repeatedly
void SecondAbility::WantGetBoolArrayParamCase3(int code)
{
    std::string key = "key";
    std::vector<bool> value = {true, true, false};
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value[0] = !value.at(0);
        want.SetParam(key, value);
        result = result && want.GetBoolArrayParam(key) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get char on empty Want
void SecondAbility::WantGetCharParamCase1(int code)
{
    std::string key = "key";
    zchar defaultValue = 'a';
    Want want;
    bool result = want.GetCharParam(key, defaultValue) == defaultValue;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed char param
void SecondAbility::WantGetCharParamCase2(int code)
{
    std::string key = "key";
    zchar value = 'a';
    zchar defaultValue = 'b';
    Want want;
    want.SetParam(key, value);
    bool result = want.GetCharParam(key, defaultValue) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get char param repeatedly
void SecondAbility::WantGetCharParamCase3(int code)
{
    std::string key = "key";
    zchar value;
    zchar defaultValue = 'a';
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value = (zchar)i;
        want.SetParam(key, value);
        result = result && want.GetCharParam(key, defaultValue) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get char array on empty Want
void SecondAbility::WantGetCharArrayParamCase1(int code)
{
    std::string key = "key";
    Want want;
    bool result = want.GetCharArrayParam(key).size() == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed char array param
void SecondAbility::WantGetCharArrayParamCase2(int code)
{
    std::string key = "key";
    std::vector<zchar> value = {'z', 'a', 'b'};
    Want want;
    want.SetParam(key, value);
    bool result = want.GetCharArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get char array param repeatedly
void SecondAbility::WantGetCharArrayParamCase3(int code)
{
    std::string key = "key";
    std::vector<zchar> value = {'z', 'a', 'b'};
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value[0] = (zchar)i;
        want.SetParam(key, value);
        result = result && want.GetCharArrayParam(key) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get int on empty Want
void SecondAbility::WantGetIntParamCase1(int code)
{
    std::string key = "key";
    int defaultValue = 7;
    Want want;
    bool result = want.GetIntParam(key, defaultValue) == defaultValue;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed int param
void SecondAbility::WantGetIntParamCase2(int code)
{
    std::string key = "key";
    int value = 10;
    int defaultValue = 7;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetIntParam(key, defaultValue) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get int param repeatedly
void SecondAbility::WantGetIntParamCase3(int code)
{
    std::string key = "key";
    int value;
    int defaultValue = 7;
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value = i;
        want.SetParam(key, value);
        result = result && want.GetIntParam(key, defaultValue) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get int array on empty Want
void SecondAbility::WantGetIntArrayParamCase1(int code)
{
    std::string key = "key";
    Want want;
    bool result = want.GetIntArrayParam(key).size() == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed int array param
void SecondAbility::WantGetIntArrayParamCase2(int code)
{
    std::string key = "key";
    std::vector<int> value = {1, 12, 127};
    Want want;
    want.SetParam(key, value);
    bool result = want.GetIntArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get int array param repeatedly
void SecondAbility::WantGetIntArrayParamCase3(int code)
{
    std::string key = "key";
    std::vector<int> value = {1, 12, 127};
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value[0] = i;
        want.SetParam(key, value);
        result = result && want.GetIntArrayParam(key) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get double on empty Want
void SecondAbility::WantGetDoubleParamCase1(int code)
{
    std::string key = "key";
    double defaultValue = 7.99;
    Want want;
    bool result = want.GetDoubleParam(key, defaultValue) == defaultValue;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed double param
void SecondAbility::WantGetDoubleParamCase2(int code)
{
    std::string key = "key";
    double value = 10.99;
    double defaultValue = 7.99;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetDoubleParam(key, defaultValue) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get double param repeatedly
void SecondAbility::WantGetDoubleParamCase3(int code)
{
    std::string key = "key";
    double value;
    double defaultValue = 7.99;
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value = i;
        want.SetParam(key, value);
        result = result && want.GetDoubleParam(key, defaultValue) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get double array on empty Want
void SecondAbility::WantGetDoubleArrayParamCase1(int code)
{
    std::string key = "key";
    Want want;
    bool result = want.GetDoubleArrayParam(key).size() == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed double array param
void SecondAbility::WantGetDoubleArrayParamCase2(int code)
{
    std::string key = "key";
    std::vector<double> value = {1.99, 12.99, 127.99};
    Want want;
    want.SetParam(key, value);
    bool result = want.GetDoubleArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get double array param repeatedly
void SecondAbility::WantGetDoubleArrayParamCase3(int code)
{
    std::string key = "key";
    std::vector<double> value = {1.99, 12.99, 127.99};
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value[0] = i;
        want.SetParam(key, value);
        result = result && want.GetDoubleArrayParam(key) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get float on empty Want
void SecondAbility::WantGetFloatParamCase1(int code)
{
    std::string key = "key";
    float defaultValue = 7.99f;
    Want want;
    bool result = want.GetFloatParam(key, defaultValue) == defaultValue;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed float param
void SecondAbility::WantGetFloatParamCase2(int code)
{
    std::string key = "key";
    float value = 10.99f;
    float defaultValue = 7.99f;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetFloatParam(key, defaultValue) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get float param repeatedly
void SecondAbility::WantGetFloatParamCase3(int code)
{
    std::string key = "key";
    float value;
    float defaultValue = 7.99f;
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value = i;
        want.SetParam(key, value);
        result = result && want.GetFloatParam(key, defaultValue) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get float array on empty Want
void SecondAbility::WantGetFloatArrayParamCase1(int code)
{
    std::string key = "key";
    Want want;
    bool result = want.GetFloatArrayParam(key).size() == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed float array param
void SecondAbility::WantGetFloatArrayParamCase2(int code)
{
    std::string key = "key";
    std::vector<float> value = {1.99f, 12.99f, 127.99f};
    Want want;
    want.SetParam(key, value);
    bool result = want.GetFloatArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get float array param repeatedly
void SecondAbility::WantGetFloatArrayParamCase3(int code)
{
    std::string key = "key";
    std::vector<float> value = {1.99f, 12.99f, 127.99f};
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value[0] = i;
        want.SetParam(key, value);
        result = result && want.GetFloatArrayParam(key) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get long on empty Want
void SecondAbility::WantGetLongParamCase1(int code)
{
    std::string key = "key";
    long defaultValue = 7l;
    Want want;
    bool result = want.GetLongParam(key, defaultValue) == defaultValue;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed long param
void SecondAbility::WantGetLongParamCase2(int code)
{
    std::string key = "key";
    long value = 10l;
    long defaultValue = 7l;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetLongParam(key, defaultValue) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get long param repeatedly
void SecondAbility::WantGetLongParamCase3(int code)
{
    std::string key = "key";
    long value;
    long defaultValue = 7l;
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value = i;
        want.SetParam(key, value);
        result = result && want.GetLongParam(key, defaultValue) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get long array on empty Want
void SecondAbility::WantGetLongArrayParamCase1(int code)
{
    std::string key = "key";
    Want want;
    bool result = want.GetLongArrayParam(key).size() == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed long array param
void SecondAbility::WantGetLongArrayParamCase2(int code)
{
    std::string key = "key";
    std::vector<long> value = {1l, 12l, 127l};
    Want want;
    want.SetParam(key, value);
    bool result = want.GetLongArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get long array param repeatedly
void SecondAbility::WantGetLongArrayParamCase3(int code)
{
    std::string key = "key";
    std::vector<long> value = {1l, 12l, 127l};
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value[0] = i;
        want.SetParam(key, value);
        result = result && want.GetLongArrayParam(key) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get short on empty Want
void SecondAbility::WantGetShortParamCase1(int code)
{
    std::string key = "key";
    short defaultValue = 7;
    Want want;
    bool result = want.GetShortParam(key, defaultValue) == defaultValue;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed short param
void SecondAbility::WantGetShortParamCase2(int code)
{
    std::string key = "key";
    short value = 10l;
    short defaultValue = 7;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetShortParam(key, defaultValue) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get short param repeatedly
void SecondAbility::WantGetShortParamCase3(int code)
{
    std::string key = "key";
    short value;
    short defaultValue = 7;
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value = i;
        want.SetParam(key, value);
        result = result && want.GetShortParam(key, defaultValue) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get short array on empty Want
void SecondAbility::WantGetShortArrayParamCase1(int code)
{
    std::string key = "key";
    Want want;
    bool result = want.GetShortArrayParam(key).size() == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed short array param
void SecondAbility::WantGetShortArrayParamCase2(int code)
{
    std::string key = "key";
    std::vector<short> value = {1, 12, 127};
    Want want;
    want.SetParam(key, value);
    bool result = want.GetShortArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get short array param repeatedly
void SecondAbility::WantGetShortArrayParamCase3(int code)
{
    std::string key = "key";
    std::vector<short> value = {1, 12, 127};
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value[0] = i;
        want.SetParam(key, value);
        result = result && want.GetShortArrayParam(key) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get string on empty Want
void SecondAbility::WantGetStringParamCase1(int code)
{
    std::string key = "key";
    std::string value;
    Want want;
    bool result = want.GetStringParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed string param
void SecondAbility::WantGetStringParamCase2(int code)
{
    std::string key = "key";
    std::string value = "zxc";
    Want want;
    want.SetParam(key, value);
    bool result = want.GetStringParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get string param repeatedly
void SecondAbility::WantGetStringParamCase3(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want want;
    bool result = true;
    std::string tmpValue;
    for (int i = 0; i < pressureTimes; i++) {
        tmpValue = value + std::to_string(i);
        want.SetParam(key, tmpValue);
        result = result && want.GetStringParam(key) == tmpValue;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get string array on empty Want
void SecondAbility::WantGetStringArrayParamCase1(int code)
{
    std::string key = "key";
    Want want;
    bool result = want.GetStringArrayParam(key).size() == 0;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get existed string array param
void SecondAbility::WantGetStringArrayParamCase2(int code)
{
    std::string key = "key";
    std::vector<std::string> value = {"1", "12", "127"};
    Want want;
    want.SetParam(key, value);
    bool result = want.GetStringArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get string array param repeatedly
void SecondAbility::WantGetStringArrayParamCase3(int code)
{
    std::string key = "key";
    std::vector<std::string> value = {"1", "12", "127"};
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        value[0] = std::to_string(i);
        want.SetParam(key, value);
        result = result && want.GetStringArrayParam(key) == value;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// max byte
void SecondAbility::WantSetParamByteCase1(int code)
{
    std::string key = "key";
    byte value = 127;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetByteParam(key, 0) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// min byte
void SecondAbility::WantSetParamByteCase2(int code)
{
    std::string key = "key";
    byte value = -128;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetByteParam(key, 0) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different byte value set on one key
void SecondAbility::WantSetParamByteCase3(int code)
{
    std::string key = "key";
    byte value = 5;
    Want want;
    for (byte i = 0; i <= value; i++) {
        want.SetParam(key, i);
    }
    bool result = want.GetByteParam(key, 0) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different byte value set on different key
void SecondAbility::WantSetParamByteCase4(int code)
{
    std::string key = "key";
    byte value = 5;
    Want want;
    for (byte i = 0; i <= value; i++) {
        want.SetParam(key + std::to_string(i), i);
    }
    bool result = true;
    for (byte i = 0; i <= value; i++) {
        result = result && (want.GetByteParam(key + std::to_string(i), -1) == i);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure of byte
void SecondAbility::WantSetParamByteCase5(int code)
{
    std::string key = "key";
    byte value = 5;
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = want.GetByteParam(key + std::to_string(pressureTimes - 1), 0) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty byte array
void SecondAbility::WantSetParamByteArrayCase1(int code)
{
    std::string key = "key";
    std::vector<byte> value;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetByteArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// big byte array
void SecondAbility::WantSetParamByteArrayCase2(int code)
{
    std::string key = "key";
    byte value = 5;
    std::vector<byte> vec(ARRAY_SIZE, value);
    Want want;
    want.SetParam(key, vec);
    bool result = want.GetByteArrayParam(key) == vec;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different byte array set on one key
void SecondAbility::WantSetParamByteArrayCase3(int code)
{
    std::string key = "key";
    int size = 5;
    std::vector<byte> value(size, size);
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<byte> tmpValue(i, i);
        want.SetParam(key, tmpValue);
    }
    bool result = want.GetByteArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different byte array set on different key
void SecondAbility::WantSetParamByteArrayCase4(int code)
{
    std::string key = "key";
    int size = 5;
    std::vector<byte> value(size, size);
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<byte> tmpValue(i, i);
        want.SetParam(key + std::to_string(i), tmpValue);
    }
    bool result = true;
    for (int i = 1; i <= size; i++) {
        std::vector<byte> tmpValue(i, i);
        result = result && (want.GetByteArrayParam(key + std::to_string(i)) == tmpValue);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure of byte array
void SecondAbility::WantSetParamByteArrayCase5(int code)
{
    std::string key = "key";
    int size = 5;
    std::vector<byte> value(size, size);
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = want.GetByteArrayParam(key + std::to_string(pressureTimes - 1)) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// bool value false
void SecondAbility::WantSetParamBoolCase1(int code)
{
    std::string key = "key";
    bool value = false;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetBoolParam(key, true) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// bool value true
void SecondAbility::WantSetParamBoolCase2(int code)
{
    std::string key = "key";
    bool value = true;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetBoolParam(key, false) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different bool value set on one key
void SecondAbility::WantSetParamBoolCase3(int code)
{
    std::string key = "key";
    int size = 5;
    bool value = true;
    Want want;
    for (int i = 0; i < size; i++) {
        value = !value;
        want.SetParam(key, value);
    }
    bool result = want.GetBoolParam(key, !value) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different bool value set on different key
void SecondAbility::WantSetParamBoolCase4(int code)
{
    std::string key = "key";
    int size = 5;
    bool value = true;
    Want want;
    for (int i = 0; i < size; i++) {
        value = !value;
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = true;
    value = true;
    for (int i = 0; i < size; i++) {
        value = !value;
        result = result && (want.GetBoolParam(key + std::to_string(i), !value) == value);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure of bool
void SecondAbility::WantSetParamBoolCase5(int code)
{
    std::string key = "key";
    bool value = true;
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = (want.GetBoolParam(key + std::to_string(pressureTimes - 1), !value) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty bool array
void SecondAbility::WantSetParamBoolArrayCase1(int code)
{
    std::string key = "key";
    std::vector<bool> value;
    Want want;
    want.SetParam(key, value);
    bool result = (want.GetBoolArrayParam(key) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// big bool array
void SecondAbility::WantSetParamBoolArrayCase2(int code)
{
    std::string key = "key";
    std::vector<bool> value(ARRAY_SIZE, true);
    Want want;
    want.SetParam(key, value);
    bool result = want.GetBoolArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different bool array set on one key
void SecondAbility::WantSetParamBoolArrayCase3(int code)
{
    std::string key = "key";
    int size = 5;
    std::vector<bool> value(size, true);
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<bool> tmpValue(i, true);
        want.SetParam(key, tmpValue);
    }
    bool result = want.GetBoolArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different bool array set on different key
void SecondAbility::WantSetParamBoolArrayCase4(int code)
{
    std::string key = "key";
    int size = 5;
    std::vector<bool> value(size, true);
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<bool> tmpValue(i, true);
        want.SetParam(key + std::to_string(i), tmpValue);
    }
    bool result = true;
    for (int i = 1; i <= size; i++) {
        std::vector<bool> tmpValue(i, true);
        result = result && (want.GetBoolArrayParam(key + std::to_string(i)) == tmpValue);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure of bool array
void SecondAbility::WantSetParamBoolArrayCase5(int code)
{
    std::string key = "key";
    int size = 5;
    std::vector<bool> value(size, true);
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = (want.GetBoolArrayParam(key + std::to_string(pressureTimes - 1)) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty char
void SecondAbility::WantSetParamCharCase1(int code)
{
    std::string key = "key";
    zchar value = ' ';
    Want want;
    want.SetParam(key, value);
    bool result = want.GetCharParam(key, 'z') == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// char default value
void SecondAbility::WantSetParamCharCase2(int code)
{
    std::string key = "key";
    zchar value = '0';
    zchar defaultValue = U'文';
    Want want;
    bool result = (want.GetCharParam(key, defaultValue) == defaultValue);
    want.SetParam(key, value);
    result = result && (want.GetCharParam(key, defaultValue) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different char value set on one key
void SecondAbility::WantSetParamCharCase3(int code)
{
    std::string key = "key";
    std::vector<zchar> value = {'a', 'b', ',', '&', U'中'};
    Want want;
    for (size_t i = 0; i < value.size(); i++) {
        want.SetParam(key, value.at(i));
    }
    bool result = want.GetCharParam(key, '0') == value.back();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different char value set on different key
void SecondAbility::WantSetParamCharCase4(int code)
{
    std::string key = "key";
    std::vector<zchar> value = {'a', 'b', ',', '&', U'中'};
    Want want;
    for (size_t i = 0; i < value.size(); i++) {
        want.SetParam(key + std::to_string(i), value.at(i));
    }
    bool result = true;
    for (size_t i = 0; i < value.size(); i++) {
        result = result && (want.GetCharParam(key + std::to_string(i), '0') == value.at(i));
        APP_LOGI("WantSetParamCharCase4 (%{public}c)", (char)want.GetCharParam(key + std::to_string(i), '0'));
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure char
void SecondAbility::WantSetParamCharCase5(int code)
{
    std::string key = "key";
    zchar value = '5';
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = want.GetCharParam(key + std::to_string(pressureTimes - 1), 0) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty char array
void SecondAbility::WantSetParamCharArrayCase1(int code)
{
    std::string key = "key";
    std::vector<zchar> value;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetCharArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// big char array
void SecondAbility::WantSetParamCharArrayCase2(int code)
{
    std::string key = "key";
    std::vector<zchar> value(ARRAY_SIZE, '5');
    Want want;
    want.SetParam(key, value);
    bool result = want.GetCharArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different char array set on one key
void SecondAbility::WantSetParamCharArrayCase3(int code)
{
    std::string key = "key";
    int size = 5;
    std::vector<zchar> value(size, zchar(size));
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<zchar> tmpValue(i, zchar(i));
        want.SetParam(key, tmpValue);
    }
    bool result = want.GetCharArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different char array set on different key
void SecondAbility::WantSetParamCharArrayCase4(int code)
{
    std::string key = "key";
    int size = 5;
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<zchar> tmpValue(i, zchar(i));
        want.SetParam(key + std::to_string(i), tmpValue);
    }
    bool result = true;
    for (int i = 1; i <= size; i++) {
        std::vector<zchar> tmpValue(i, zchar(i));
        result = result && (want.GetCharArrayParam(key + std::to_string(i)) == tmpValue);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure char array
void SecondAbility::WantSetParamCharArrayCase5(int code)
{
    std::string key = "key";
    int size = 5;
    std::vector<zchar> value(size, 'z');
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = (want.GetCharArrayParam(key + std::to_string(pressureTimes - 1)) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// max int
void SecondAbility::WantSetParamIntCase1(int code)
{
    std::string key = "key";
    Want want;
    want.SetParam(key, INT_MAX);
    bool result = want.GetIntParam(key, 0) == INT_MAX;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// min int
void SecondAbility::WantSetParamIntCase2(int code)
{
    std::string key = "key";
    Want want;
    want.SetParam(key, INT_MIN);
    bool result = want.GetIntParam(key, 0) == INT_MIN;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different int value set on one key
void SecondAbility::WantSetParamIntCase3(int code)
{
    std::string key = "key";
    int value = 5;
    Want want;
    for (int i = 0; i <= value; i++) {
        want.SetParam(key, i);
    }
    bool result = want.GetIntParam(key, 0) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different int value set on different key
void SecondAbility::WantSetParamIntCase4(int code)
{
    std::string key = "key";
    int value = 5;
    Want want;
    for (int i = 0; i <= value; i++) {
        want.SetParam(key + std::to_string(i), i);
    }
    bool result = true;
    for (int i = 0; i <= value; i++) {
        result = result && (want.GetIntParam(key + std::to_string(i), -1) == i);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure int
void SecondAbility::WantSetParamIntCase5(int code)
{
    std::string key = "key";
    int value = 5;
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = want.GetIntParam(key + std::to_string(pressureTimes - 1), 0) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty int array
void SecondAbility::WantSetParamIntArrayCase1(int code)
{
    std::string key = "key";
    std::vector<int> value;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetIntArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// big int array
void SecondAbility::WantSetParamIntArrayCase2(int code)
{
    std::string key = "key";
    std::vector<int> value(ARRAY_SIZE, 5);
    Want want;
    want.SetParam(key, value);
    bool result = want.GetIntArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different int array set on one key
void SecondAbility::WantSetParamIntArrayCase3(int code)
{
    std::string key = "key";
    int size = 5;
    std::vector<int> value(size, size);
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<int> tmpValue(i, i);
        want.SetParam(key, tmpValue);
    }
    bool result = want.GetIntArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different int array set on different key
void SecondAbility::WantSetParamIntArrayCase4(int code)
{
    std::string key = "key";
    int size = 5;
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<int> tmpValue(i, i);
        want.SetParam(key + std::to_string(i), tmpValue);
    }
    bool result = true;
    for (int i = 1; i <= size; i++) {
        std::vector<int> tmpValue(i, i);
        result = result && (want.GetIntArrayParam(key + std::to_string(i)) == tmpValue);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure byte array
void SecondAbility::WantSetParamIntArrayCase5(int code)
{
    std::string key = "key";
    int size = 5;
    std::vector<int> value(size, size);
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = want.GetIntArrayParam(key + std::to_string(pressureTimes - 1)) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// max double
void SecondAbility::WantSetParamDoubleCase1(int code)
{
    std::string key = "key";
    Want want;
    want.SetParam(key, DBL_MAX);
    bool result = want.GetDoubleParam(key, 0) == DBL_MAX;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// min double
void SecondAbility::WantSetParamDoubleCase2(int code)
{
    std::string key = "key";
    Want want;
    want.SetParam(key, DBL_MIN);
    bool result = want.GetDoubleParam(key, 0) == DBL_MIN;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different double value set on one key
void SecondAbility::WantSetParamDoubleCase3(int code)
{
    std::string key = "key";
    double value = 99.9;
    int size = 5;
    Want want;
    for (int i = 0; i < size; i++) {
        want.SetParam(key, i * value);
    }
    bool result = want.GetDoubleParam(key, 0) == value * (size - 1);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different double value set on different key
void SecondAbility::WantSetParamDoubleCase4(int code)
{
    std::string key = "key";
    double value = 99.9;
    int size = 5;
    Want want;
    for (int i = 0; i < size; i++) {
        want.SetParam(key + std::to_string(i), i * value);
    }
    bool result = true;
    for (int i = 0; i < size; i++) {
        result = result && (want.GetDoubleParam(key + std::to_string(i), -1) == i * value);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure double
void SecondAbility::WantSetParamDoubleCase5(int code)
{
    std::string key = "key";
    double value = 99.9;
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = want.GetDoubleParam(key + std::to_string(pressureTimes - 1), 0) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty double array
void SecondAbility::WantSetParamDoubleArrayCase1(int code)
{
    std::string key = "key";
    std::vector<double> value;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetDoubleArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// big double array
void SecondAbility::WantSetParamDoubleArrayCase2(int code)
{
    std::string key = "key";
    double value = 99.9;
    std::vector<double> vec(ARRAY_SIZE, value);
    Want want;
    want.SetParam(key, vec);
    bool result = want.GetDoubleArrayParam(key) == vec;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different double array set on one key
void SecondAbility::WantSetParamDoubleArrayCase3(int code)
{
    std::string key = "key";
    int size = 5;
    double dValue = 99.9;
    std::vector<double> value(size, size * dValue);
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<double> tmpValue(i, i * dValue);
        want.SetParam(key, tmpValue);
    }
    bool result = want.GetDoubleArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different double array set on different key
void SecondAbility::WantSetParamDoubleArrayCase4(int code)
{
    std::string key = "key";
    int size = 5;
    double value = 99.9;
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<double> tmpValue(i, i * value);
        want.SetParam(key + std::to_string(i), tmpValue);
    }
    bool result = true;
    for (int i = 1; i <= size; i++) {
        std::vector<double> tmpValue(i, i * value);
        result = result && (want.GetDoubleArrayParam(key + std::to_string(i)) == tmpValue);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure double array
void SecondAbility::WantSetParamDoubleArrayCase5(int code)
{
    std::string key = "key";
    int size = 5;
    double value = 99.9;
    std::vector<double> vec(size, value);
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), vec);
    }
    bool result = want.GetDoubleArrayParam(key + std::to_string(pressureTimes - 1)) == vec;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// max float
void SecondAbility::WantSetParamFloatCase1(int code)
{
    std::string key = "key";
    Want want;
    want.SetParam(key, FLT_MAX);
    bool result = want.GetFloatParam(key, 0) == FLT_MAX;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// min float
void SecondAbility::WantSetParamFloatCase2(int code)
{
    std::string key = "key";
    Want want;
    want.SetParam(key, FLT_MIN);
    bool result = want.GetFloatParam(key, 0) == FLT_MIN;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different float value set on one key
void SecondAbility::WantSetParamFloatCase3(int code)
{
    std::string key = "key";
    float value = 99.9f;
    int size = 5;
    Want want;
    for (int i = 0; i < size; i++) {
        want.SetParam(key, i * value);
    }
    bool result = want.GetFloatParam(key, 0) == value * (size - 1);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different float value set on different key
void SecondAbility::WantSetParamFloatCase4(int code)
{
    std::string key = "key";
    float value = 99.9f;
    int size = 5;
    Want want;
    for (int i = 0; i < size; i++) {
        want.SetParam(key + std::to_string(i), i * value);
    }
    bool result = true;
    for (int i = 0; i < size; i++) {
        result = result && (want.GetFloatParam(key + std::to_string(i), -1) == i * value);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure float
void SecondAbility::WantSetParamFloatCase5(int code)
{
    std::string key = "key";
    float value = 99.9f;
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = want.GetFloatParam(key + std::to_string(pressureTimes - 1), 0) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty float array
void SecondAbility::WantSetParamFloatArrayCase1(int code)
{
    std::string key = "key";
    std::vector<float> value;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetFloatArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// big float array
void SecondAbility::WantSetParamFloatArrayCase2(int code)
{
    std::string key = "key";
    std::vector<float> value(ARRAY_SIZE, 99.9f);
    Want want;
    want.SetParam(key, value);
    bool result = want.GetFloatArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different float array set on one key
void SecondAbility::WantSetParamFloatArrayCase3(int code)
{
    std::string key = "key";
    int size = 5;
    float fValue = 99.9f;
    std::vector<float> value(size, fValue * size);
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<float> tmpValue(i, i * fValue);
        want.SetParam(key, tmpValue);
    }
    bool result = want.GetFloatArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different float array set on different key
void SecondAbility::WantSetParamFloatArrayCase4(int code)
{
    std::string key = "key";
    int size = 5;
    float value = 99.9f;
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<float> tmpValue(i, i * value);
        want.SetParam(key + std::to_string(i), tmpValue);
    }
    bool result = true;
    for (int i = 1; i <= size; i++) {
        std::vector<float> tmpValue(i, i * value);
        result = result && (want.GetFloatArrayParam(key + std::to_string(i)) == tmpValue);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure float array
void SecondAbility::WantSetParamFloatArrayCase5(int code)
{
    std::string key = "key";
    int size = 5;
    std::vector<float> value(size, 99.9f);
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = want.GetFloatArrayParam(key + std::to_string(pressureTimes - 1)) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// max long
void SecondAbility::WantSetParamLongCase1(int code)
{
    std::string key = "key";
    Want want;
    want.SetParam(key, LONG_MAX);
    bool result = want.GetLongParam(key, 0) == LONG_MAX;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// min long
void SecondAbility::WantSetParamLongCase2(int code)
{
    std::string key = "key";
    Want want;
    want.SetParam(key, LONG_MIN);
    bool result = want.GetLongParam(key, 0) == LONG_MIN;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different long value set on one key
void SecondAbility::WantSetParamLongCase3(int code)
{
    std::string key = "key";
    long value = 999;
    int size = 5;
    Want want;
    for (int i = 0; i < size; i++) {
        want.SetParam(key, i * value);
    }
    bool result = want.GetLongParam(key, 0) == value * (size - 1);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different long value set on different key
void SecondAbility::WantSetParamLongCase4(int code)
{
    std::string key = "key";
    long value = 999;
    int size = 5;
    Want want;
    for (int i = 0; i < size; i++) {
        want.SetParam(key + std::to_string(i), i * value);
    }
    bool result = true;
    for (int i = 0; i < size; i++) {
        result = result && (want.GetLongParam(key + std::to_string(i), -1) == i * value);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure long
void SecondAbility::WantSetParamLongCase5(int code)
{
    std::string key = "key";
    long value = 999;
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = want.GetLongParam(key + std::to_string(pressureTimes - 1), 0) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty long array
void SecondAbility::WantSetParamLongArrayCase1(int code)
{
    std::string key = "key";
    std::vector<long> value;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetLongArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// big long array
void SecondAbility::WantSetParamLongArrayCase2(int code)
{
    std::string key = "key";
    long value = 999;
    std::vector<long> vec(ARRAY_SIZE, value);
    Want want;
    want.SetParam(key, vec);
    bool result = want.GetLongArrayParam(key) == vec;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different long array set on one key
void SecondAbility::WantSetParamLongArrayCase3(int code)
{
    std::string key = "key";
    int size = 5;
    long lValue = 999;
    std::vector<long> value(size, lValue * size);
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<long> tmpValue(i, i * lValue);
        want.SetParam(key, tmpValue);
    }
    bool result = want.GetLongArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different long array set on different key
void SecondAbility::WantSetParamLongArrayCase4(int code)
{
    std::string key = "key";
    int size = 5;
    long value = 999;
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<long> tmpValue(i, i * value);
        want.SetParam(key + std::to_string(i), tmpValue);
    }
    bool result = true;
    for (int i = 1; i <= size; i++) {
        std::vector<long> tmpValue(i, i * value);
        result = result && (want.GetLongArrayParam(key + std::to_string(i)) == tmpValue);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure long array
void SecondAbility::WantSetParamLongArrayCase5(int code)
{
    std::string key = "key";
    int size = 5;
    long value = 999;
    std::vector<long> vec(size, value);
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), vec);
    }
    bool result = want.GetLongArrayParam(key + std::to_string(pressureTimes - 1)) == vec;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// max short
void SecondAbility::WantSetParamShortCase1(int code)
{
    std::string key = "key";
    short value = SHRT_MAX;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetShortParam(key, 0) == SHRT_MAX;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// min short
void SecondAbility::WantSetParamShortCase2(int code)
{
    std::string key = "key";
    short value = SHRT_MIN;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetShortParam(key, 0) == SHRT_MIN;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different short value set on one key
void SecondAbility::WantSetParamShortCase3(int code)
{
    std::string key = "key";
    short value = 999;
    int size = 5;
    Want want;
    for (short i = 0; i < size; i++) {
        want.SetParam(key, static_cast<short>(i * value));
    }
    bool result = want.GetShortParam(key, 0) == value * (size - 1);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different short value set on different key
void SecondAbility::WantSetParamShortCase4(int code)
{
    std::string key = "key";
    short value = 999;
    int size = 5;
    Want want;
    for (short i = 0; i < size; i++) {
        want.SetParam(key + std::to_string(i), static_cast<short>(i * value));
    }
    bool result = true;
    for (short i = 0; i < size; i++) {
        result = result && (want.GetShortParam(key + std::to_string(i), -1) == i * value);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure short
void SecondAbility::WantSetParamShortCase5(int code)
{
    std::string key = "key";
    short value = 999;
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = want.GetShortParam(key + std::to_string(pressureTimes - 1), 0) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty short array
void SecondAbility::WantSetParamShortArrayCase1(int code)
{
    std::string key = "key";
    std::vector<short> value;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetShortArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// big short array
void SecondAbility::WantSetParamShortArrayCase2(int code)
{
    std::string key = "key";
    short value = 999;
    std::vector<short> vec(ARRAY_SIZE, value);
    Want want;
    want.SetParam(key, vec);
    bool result = want.GetShortArrayParam(key) == vec;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different short array set on one key
void SecondAbility::WantSetParamShortArrayCase3(int code)
{
    std::string key = "key";
    int size = 5;
    short sValue = 999;
    std::vector<short> value(size, sValue * size);
    Want want;
    for (short i = 1; i <= size; i++) {
        std::vector<short> tmpValue(i, i * sValue);
        want.SetParam(key, tmpValue);
    }
    bool result = want.GetShortArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different short array set on different key
void SecondAbility::WantSetParamShortArrayCase4(int code)
{
    std::string key = "key";
    int size = 5;
    short value = 999;
    Want want;
    for (short i = 1; i <= size; i++) {
        std::vector<short> tmpValue(i, i * value);
        want.SetParam(key + std::to_string(i), tmpValue);
    }
    bool result = true;
    for (short i = 1; i <= size; i++) {
        std::vector<short> tmpValue(i, i * value);
        result = result && (want.GetShortArrayParam(key + std::to_string(i)) == tmpValue);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure short array
void SecondAbility::WantSetParamShortArrayCase5(int code)
{
    std::string key = "key";
    int size = 5;
    short value = 999;
    std::vector<short> vec(size, value);
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), vec);
    }
    bool result = want.GetShortArrayParam(key + std::to_string(pressureTimes - 1)) == vec;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty string
void SecondAbility::WantSetParamStringCase1(int code)
{
    std::string key = "key";
    std::string value;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetStringParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// long string
void SecondAbility::WantSetParamStringCase2(int code)
{
    std::string key = "key";
    std::string value(LARGE_STR_LEN, 'a');
    Want want;
    want.SetParam(key, value);
    bool result = want.GetStringParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different string value set on one key
void SecondAbility::WantSetParamStringCase3(int code)
{
    std::string key = "key";
    int size = 5;
    std::string value(size, 'a');
    Want want;
    for (int i = 1; i <= size; i++) {
        std::string tmpValue(i, 'a');
        want.SetParam(key, tmpValue);
    }
    bool result = want.GetStringParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different string value set on different key
void SecondAbility::WantSetParamStringCase4(int code)
{
    std::string key = "key";
    int size = 5;
    Want want;
    for (int i = 1; i <= size; i++) {
        std::string tmpValue(i, 'a');
        want.SetParam(key + std::to_string(i), tmpValue);
    }
    bool result = true;
    for (int i = 1; i <= size; i++) {
        std::string tmpValue(i, 'a');
        result = result && (want.GetStringParam(key + std::to_string(i)) == tmpValue);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure string
void SecondAbility::WantSetParamStringCase5(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = want.GetStringParam(key + std::to_string(pressureTimes - 1)) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty string array
void SecondAbility::WantSetParamStringArrayCase1(int code)
{
    std::string key = "key";
    std::vector<std::string> value;
    Want want;
    want.SetParam(key, value);
    bool result = want.GetStringArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// big string array
void SecondAbility::WantSetParamStringArrayCase2(int code)
{
    std::string key = "key";
    std::string strValue = "value";
    std::vector<std::string> value(ARRAY_SIZE, strValue);
    Want want;
    want.SetParam(key, value);
    bool result = want.GetStringArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different string array set on one key
void SecondAbility::WantSetParamStringArrayCase3(int code)
{
    std::string key = "key";
    int size = 5;
    std::string strValue = "value";
    std::vector<std::string> value(size, strValue);
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<std::string> tmpValue(i, strValue);
        want.SetParam(key, tmpValue);
    }
    bool result = want.GetStringArrayParam(key) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// different string array set on different key
void SecondAbility::WantSetParamStringArrayCase4(int code)
{
    std::string key = "key";
    int size = 5;
    std::string value = "value";
    Want want;
    for (int i = 1; i <= size; i++) {
        std::vector<std::string> tmpValue(i, value);
        want.SetParam(key + std::to_string(i), tmpValue);
    }
    bool result = true;
    for (int i = 1; i <= size; i++) {
        std::vector<std::string> tmpValue(i, value);
        result = result && (want.GetStringArrayParam(key + std::to_string(i)) == tmpValue);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// pressure string array
void SecondAbility::WantSetParamStringArrayCase5(int code)
{
    std::string key = "key";
    int size = 5;
    std::string strValue = "value";
    std::vector<std::string> value(size, strValue);
    Want want;
    for (int i = 0; i < pressureTimes; i++) {
        want.SetParam(key + std::to_string(i), value);
    }
    bool result = want.GetStringArrayParam(key + std::to_string(pressureTimes - 1)) == value;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// empty Want
void SecondAbility::WantHasParameterCase1(int code)
{
    std::string key = "key";
    Want want;
    bool result = !want.HasParameter(key);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// no parameter
void SecondAbility::WantHasParameterCase2(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want want;
    want.SetParam("key1", value);
    bool result = !want.HasParameter(key);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// parameter exits
void SecondAbility::WantHasParameterCase3(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want want;
    want.SetParam(key, value);
    bool result = want.HasParameter(key);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// after remove parameter
void SecondAbility::WantHasParameterCase4(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want want;
    want.SetParam(key, value);
    bool result = want.HasParameter(key);
    want.RemoveParam(key);
    result = result && !want.HasParameter(key);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// whether exists parameter repeatedly
void SecondAbility::WantHasParameterCase5(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        std::string tmpKey = key + std::to_string(i);
        want.SetParam(tmpKey, value);
        result = result && want.HasParameter(tmpKey);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// replace params on empty Want by empty WantParams
void SecondAbility::WantReplaceParamsWantParamsCase1(int code)
{
    Want want;
    WantParams wParams;
    want.ReplaceParams(wParams);
    bool result = want.GetParams().IsEmpty();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set param and then replace it by empty WantParams
void SecondAbility::WantReplaceParamsWantParamsCase2(int code)
{
    std::string key = "key";
    std::string value = "value";
    WantParams wParams;
    wParams.SetParam(key, AAFwk::String::Box(value));
    Want want;
    want.SetParams(wParams);
    bool result = want.HasParameter(key);
    WantParams emptyParams;
    want.ReplaceParams(emptyParams);
    result = result && !want.HasParameter(key);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// replace params on empty Want by not empty WantParams
void SecondAbility::WantReplaceParamsWantParamsCase3(int code)
{
    std::string key = "key";
    std::string value = "value";
    WantParams wParams;
    wParams.SetParam(key, AAFwk::String::Box(value));
    Want want;
    want.ReplaceParams(wParams);
    bool result = want.HasParameter(key);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// replace params twice by different WantParams
void SecondAbility::WantReplaceParamsWantParamsCase4(int code)
{
    WantParams wParamsOne;
    wParamsOne.SetParam("key1", AAFwk::String::Box("value1"));
    WantParams wParamsTwo;
    wParamsTwo.SetParam("key2", AAFwk::String::Box("value2"));
    Want want;
    want.ReplaceParams(wParamsOne);
    want.ReplaceParams(wParamsTwo);
    bool result = !want.HasParameter("key1");
    result = result && want.HasParameter("key2");
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// replace params repeatedly
void SecondAbility::WantReplaceParamsWantParamsCase5(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want want;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        WantParams wParams;
        std::string tmpKey = key + std::to_string(i);
        wParams.SetParam(tmpKey, AAFwk::String::Box(value));
        want.ReplaceParams(wParams);
        result = result && want.HasParameter(tmpKey);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// replace params on empty Want by another empty Want
void SecondAbility::WantReplaceParamsWantCase1(int code)
{
    Want wantOne;
    Want wantTwo;
    wantOne.ReplaceParams(wantTwo);
    bool result = wantOne.GetParams().IsEmpty();
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// replace params on empty Want by another not empty Want
void SecondAbility::WantReplaceParamsWantCase2(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want wantOne;
    Want wantTwo;
    wantTwo.SetParam(key, value);
    wantOne.ReplaceParams(wantTwo);
    bool result = wantOne.HasParameter(key);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// replace params by another empty Want
void SecondAbility::WantReplaceParamsWantCase3(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want wantOne;
    Want wantTwo;
    wantOne.SetParam(key, value);
    wantOne.ReplaceParams(wantTwo);
    bool result = !wantOne.HasParameter(key);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// replace params by another Want
void SecondAbility::WantReplaceParamsWantCase4(int code)
{
    std::string value = "value";
    Want wantOne;
    Want wantTwo;
    wantOne.SetParam("key1", value);
    wantTwo.SetParam("key2", value);
    wantOne.ReplaceParams(wantTwo);
    bool result = !wantOne.HasParameter("key1");
    result = result && wantOne.HasParameter("key2");
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// replace params repeatedly
void SecondAbility::WantReplaceParamsWantCase5(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want wantOne;
    Want wantTwo;
    bool result = true;
    std::string tmpKey;
    for (int i = 0; i < pressureTimes; i++) {
        tmpKey = key + std::to_string(i);
        wantTwo.SetParam(tmpKey, value);
        wantOne.ReplaceParams(wantTwo);
        result = result && wantOne.HasParameter(tmpKey);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// remove param from empty Want
void SecondAbility::WantRemoveParamCase1(int code)
{
    std::string key = "key";
    Want want;
    want.RemoveParam(key);
    bool result = !want.HasParameter(key);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// remove existed param
void SecondAbility::WantRemoveParamCase2(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want want;
    want.SetParam(key, value);
    bool result = want.HasParameter(key);
    want.RemoveParam(key);
    result = result && !want.HasParameter(key);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and remove param repeatedly
void SecondAbility::WantRemoveParamCase3(int code)
{
    std::string key = "key";
    std::string value = "value";
    Want want;
    std::string tmpKey;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        tmpKey = key + std::to_string(i);
        want.SetParam(tmpKey, value);
        result = result && want.HasParameter(tmpKey);
        want.RemoveParam(tmpKey);
        result = result && !want.HasParameter(tmpKey);
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get operation from empty Want
void SecondAbility::WantGetOperationCase1(int code)
{
    Want want;
    Operation emptyOperation;
    bool result = want.GetOperation() == emptyOperation;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// set and get operation
void SecondAbility::WantGetOperationCase2(int code)
{
    Want want;
    OperationBuilder opBuilder;
    auto operation = opBuilder.WithAbilityName("abilityName").WithBundleName("bundleName").build();
    want.SetOperation(*operation);
    bool result = want.GetOperation() == *operation;
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// get operation repeatedly
void SecondAbility::WantGetOperationCase3(int code)
{
    Want want;
    OperationBuilder opBuilder;
    std::shared_ptr<Operation> operation;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        operation = opBuilder.WithAbilityName("abilityName" + std::to_string(i)).build();
        want.SetOperation(*operation);
        result = result && want.GetOperation() == *operation;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// compare empty Want operation
void SecondAbility::WantOperationEqualsCase1(int code)
{
    Want wantOne;
    Want wantTwo;
    bool result = wantOne.OperationEquals(wantTwo);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// compare Want operation not equal
void SecondAbility::WantOperationEqualsCase2(int code)
{
    OperationBuilder opBuilder;
    std::vector<std::string> vec = {"entities1", "entities2"};
    auto operation = opBuilder.WithAbilityName("abilityName")
                         .WithBundleName("bundleName")
                         .WithDeviceId("deviceId")
                         .WithAction("action")
                         .WithEntities(vec)
                         .WithFlags(1)
                         .WithUri(Uri("uri"))
                         .build();
    Want wantOne;
    wantOne.SetOperation(*operation);
    OperationBuilder opBuilderTwo;
    auto operationTwo = opBuilderTwo.WithAbilityName("abilityName2")
                            .WithBundleName("bundleName")
                            .WithDeviceId("deviceId")
                            .WithAction("action")
                            .WithEntities(vec)
                            .WithFlags(1)
                            .WithUri(Uri("uri"))
                            .build();
    Want wantTwo;
    wantTwo.SetOperation(*operationTwo);
    bool result = !wantOne.OperationEquals(wantTwo);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// compare Want operation equal
void SecondAbility::WantOperationEqualsCase3(int code)
{
    OperationBuilder opBuilder;
    auto operation = opBuilder.WithAbilityName("abilityName")
                         .WithBundleName("bundleName")
                         .WithDeviceId("deviceId")
                         .WithAction("action")
                         .build();
    Want wantOne;
    wantOne.SetOperation(*operation);
    OperationBuilder opBuilderTwo;
    auto operationTwo = opBuilderTwo.WithAbilityName("abilityName")
                            .WithBundleName("bundleName")
                            .WithDeviceId("deviceId")
                            .WithAction("action")
                            .build();
    Want wantTwo;
    wantTwo.SetOperation(*operationTwo);
    bool result = wantOne.OperationEquals(wantTwo);
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// compare Want operation repeatedly
void SecondAbility::WantOperationEqualsCase4(int code)
{
    OperationBuilder opBuilder;
    auto operation = opBuilder.WithAbilityName("abilityName" + std::to_string(pressureTimes - 1))
                         .WithBundleName("bundleName")
                         .WithDeviceId("deviceId")
                         .WithAction("action")
                         .build();
    Want wantOne;
    wantOne.SetOperation(*operation);
    Want wantTwo;
    std::shared_ptr<Operation> operationTwo;
    OperationBuilder opBuilderTwo;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        operationTwo = opBuilderTwo.WithAbilityName("abilityName" + std::to_string(i))
                           .WithBundleName("bundleName")
                           .WithDeviceId("deviceId")
                           .WithAction("action")
                           .build();
        wantTwo.SetOperation(*operationTwo);
        if (i == pressureTimes - 1) {
            result = result && wantOne.OperationEquals(wantTwo);
        } else {
            result = result && !wantOne.OperationEquals(wantTwo);
        }
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// clone operation on empty Want
void SecondAbility::WantCloneOperationCase1(int code)
{
    Want want;
    Want *wantClone = want.CloneOperation();
    bool result = false;
    if (wantClone) {
        result = wantClone->OperationEquals(want);
        delete wantClone;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// clone not empty operation
void SecondAbility::WantCloneOperationCase2(int code)
{
    OperationBuilder opBuilder;
    auto operation = opBuilder.WithAbilityName("abilityName")
                         .WithBundleName("bundleName")
                         .WithDeviceId("deviceId")
                         .WithAction("action")
                         .build();
    Want want;
    want.SetOperation(*operation);
    Want *wantClone = want.CloneOperation();
    bool result = false;
    if (wantClone != nullptr) {
        result = wantClone->OperationEquals(want);
        delete wantClone;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

// clone operation repeatedly
void SecondAbility::WantCloneOperationCase3(int code)
{
    OperationBuilder opBuilder;
    Want want;
    std::shared_ptr<Operation> operation;
    bool result = true;
    for (int i = 0; i < pressureTimes; i++) {
        operation = opBuilder.WithAbilityName("abilityName" + std::to_string(i))
                        .WithBundleName("bundleName")
                        .WithDeviceId("deviceId")
                        .WithAction("action")
                        .build();
        want.SetOperation(*operation);
        Want *wantClone = want.CloneOperation();
        result = result && wantClone->OperationEquals(want);
        if (wantClone) {
            delete wantClone;
        }
    }
    TestUtils::PublishEvent(g_EVENT_RESP_SECOND, code, std::to_string(result));
}

REGISTER_AA(SecondAbility)
}  // namespace AppExecFwk
}  // namespace OHOS