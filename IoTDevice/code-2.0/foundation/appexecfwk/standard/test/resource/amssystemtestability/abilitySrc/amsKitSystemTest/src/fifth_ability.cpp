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

#include "fifth_ability.h"
#include "app_log_wrapper.h"
#include "ohos/aafwk/base/base_types.h"
#include "test_utils.h"

namespace OHOS {
namespace AppExecFwk {
using namespace AAFwk;
using namespace OHOS::EventFwk;
constexpr unsigned loopSize = 10;
void FifthAbility::Init(const std::shared_ptr<AbilityInfo> &abilityInfo,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    APP_LOGI("FifthAbility::Init");
    Ability::Init(abilityInfo, application, handler, token);
}

void FifthAbility::OnStart(const Want &want)
{
    APP_LOGI("FifthAbility::onStart");
    Ability::OnStart(want);
    SubscribeEvent();
}

void FifthAbility::OnStop()
{
    APP_LOGI("FifthAbility::onStop");
    Ability::OnStop();
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, Ability::GetState(), "OnStop");
}

void FifthAbility::OnActive()
{
    APP_LOGI("FifthAbility::OnActive");
    Ability::OnActive();
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, Ability::GetState(), "OnActive");
}

void FifthAbility::OnInactive()
{
    APP_LOGI("FifthAbility::OnInactive");
    Ability::OnInactive();
}

void FifthAbility::OnBackground()
{
    APP_LOGI("FifthAbility::OnBackground");
    Ability::OnBackground();
}

void FifthAbility::OnForeground(const Want &want)
{
    APP_LOGI("FifthAbility::OnForeground");
    Ability::OnForeground(want);
}

void FifthAbility::OnAbilityResult(int requestCode, int resultCode, const Want &resultData)
{
    APP_LOGI("FifthAbility::OnAbilityResult");
    Ability::OnAbilityResult(requestCode, resultCode, resultData);
}

void FifthAbility::OnBackPressed()
{
    APP_LOGI("FifthAbility::OnBackPressed");
    Ability::OnBackPressed();
}

void FifthAbility::OnNewWant(const Want &want)
{
    APP_LOGI("FifthAbility::OnNewWant");
    Ability::OnNewWant(want);
}

void FifthAbility::SubscribeEvent()
{
    std::vector<std::string> eventList = {
        g_EVENT_REQU_FIFTH,
    };
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber = std::make_shared<FifthEventSubscriber>(subscribeInfo, this);
    CommonEventManager::SubscribeCommonEvent(subscriber);
}

void FifthEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("FifthEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("FifthEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("FifthEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (std::strcmp(eventName.c_str(), g_EVENT_REQU_FIFTH.c_str()) == 0) {
        auto target = data.GetData();
        auto handle = 0;
        auto api = 1;
        auto code = 2;
        auto paramMinSize = 3;
        auto caseInfo = TestUtils::split(target, "_");
        if (caseInfo.size() < static_cast<unsigned int>(paramMinSize)) {
            return;
        }
        if (mapTestFunc_.find(caseInfo[handle]) != mapTestFunc_.end()) {
            mapTestFunc_[caseInfo[handle]](std::stoi(caseInfo[api]), std::stoi(caseInfo[code]), data.GetCode());
        } else {
            APP_LOGI("FifthEventSubscriber::OnReceiveEvent: CommonEventData error(%{public}s)", target.c_str());
        }
    }
}

void FifthAbility::TestWantParams(int apiIndex, int caseIndex, int code)
{
    APP_LOGI("FifthAbility::TestWantParams");
    if (mapCase_.find(apiIndex) != mapCase_.end()) {
        if (caseIndex < (int)mapCase_[apiIndex].size()) {
            mapCase_[apiIndex][caseIndex](code);
        }
    }
}

// set string param
void FifthAbility::WantParamsSetParamCase1(int code)
{
    APP_LOGI("WantParamsSetParamCase1");
    std::string key = "key";
    std::string value = "value";
    WantParams wParams;
    wParams.SetParam(key, String::Box(value));
    bool result = (String::Unbox(IString::Query(wParams.GetParam(key))) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set bool param
void FifthAbility::WantParamsSetParamCase2(int code)
{
    std::string key = "key";
    bool value = true;
    WantParams wParams;
    wParams.SetParam(key, Boolean::Box(value));
    bool result = (Boolean::Unbox(IBoolean::Query(wParams.GetParam(key))) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set byte param
void FifthAbility::WantParamsSetParamCase3(int code)
{
    std::string key = "key";
    byte value = 9;
    WantParams wParams;
    wParams.SetParam(key, Byte::Box(value));
    bool result = (Byte::Unbox(IByte::Query(wParams.GetParam(key))) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set char param
void FifthAbility::WantParamsSetParamCase4(int code)
{
    std::string key = "key";
    zchar value = 'z';
    WantParams wParams;
    wParams.SetParam(key, Char::Box(value));
    bool result = (Char::Unbox(IChar::Query(wParams.GetParam(key))) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set short param
void FifthAbility::WantParamsSetParamCase5(int code)
{
    std::string key = "key";
    short value = 99;
    WantParams wParams;
    wParams.SetParam(key, Short::Box(value));
    bool result = (Short::Unbox(IShort::Query(wParams.GetParam(key))) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set int param
void FifthAbility::WantParamsSetParamCase6(int code)
{
    std::string key = "key";
    int value = 999;
    WantParams wParams;
    wParams.SetParam(key, Integer::Box(value));
    bool result = (Integer::Unbox(IInteger::Query(wParams.GetParam(key))) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set long param
void FifthAbility::WantParamsSetParamCase7(int code)
{
    std::string key = "key";
    long value = 9999L;
    WantParams wParams;
    wParams.SetParam(key, Long::Box(value));
    bool result = (Long::Unbox(ILong::Query(wParams.GetParam(key))) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set float param
void FifthAbility::WantParamsSetParamCase8(int code)
{
    std::string key = "key";
    float value = 9999.99f;
    WantParams wParams;
    wParams.SetParam(key, Float::Box(value));
    bool result = (Float::Unbox(IFloat::Query(wParams.GetParam(key))) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set double param
void FifthAbility::WantParamsSetParamCase9(int code)
{
    std::string key = "key";
    double value = 99998888.99;
    WantParams wParams;
    wParams.SetParam(key, Double::Box(value));
    bool result = (Double::Unbox(IDouble::Query(wParams.GetParam(key))) == value);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

template <typename T1, typename T2>
static void SetArray(const InterfaceID &id, const std::vector<T1> &value, sptr<IArray> &ao)
{
    typename std::vector<T1>::size_type size = value.size();
    ao = new Array(size, id);
    for (typename std::vector<T1>::size_type i = 0; i < size; i++) {
        ao->Set(i, T2::Box(value[i]));
    }
}

template <typename T1, typename T2, typename T3>
static void FillArray(IArray *ao, std::vector<T1> &array)
{
    auto func = [&](IInterface *object) { array.push_back(T2::Unbox(T3::Query(object))); };
    Array::ForEach(ao, func);
}

// set string array param
void FifthAbility::WantParamsSetParamCase10(int code)
{
    std::string key = "key";
    std::vector<std::string> value = {"aa", "bb", "cc"};

    std::vector<std::string>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IString);
    for (std::vector<std::string>::size_type i = 0; i < size; i++) {
        ao->Set(i, String::Box(value[i]));
    }
    WantParams wParams;
    wParams.SetParam(key, ao);
    std::vector<std::string> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wParams.GetParam(key));
    FillArray<std::string, String, IString>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set bool array param
void FifthAbility::WantParamsSetParamCase11(int code)
{
    std::string key = "key";
    std::vector<bool> value = {false, true, true, false};

    std::vector<bool>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IBoolean);
    SetArray<bool, Boolean>(g_IID_IBoolean, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    std::vector<bool> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wParams.GetParam(key));
    FillArray<bool, Boolean, IBoolean>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set byte array param
void FifthAbility::WantParamsSetParamCase12(int code)
{
    std::string key = "key";
    std::vector<byte> value = {1, 2, 3, 4};

    std::vector<byte>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IByte);
    SetArray<byte, Byte>(g_IID_IByte, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    std::vector<byte> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wParams.GetParam(key));
    FillArray<byte, Byte, IByte>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set char array param
void FifthAbility::WantParamsSetParamCase13(int code)
{
    std::string key = "key";
    std::vector<zchar> value = {'1', '2', '3', '4'};

    std::vector<zchar>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IChar);
    SetArray<zchar, Char>(g_IID_IChar, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    std::vector<zchar> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wParams.GetParam(key));
    FillArray<zchar, Char, IChar>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set short array param
void FifthAbility::WantParamsSetParamCase14(int code)
{
    std::string key = "key";
    std::vector<short> value = {'1', '2', '3', '4'};

    std::vector<short>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IShort);
    SetArray<short, Short>(g_IID_IShort, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    std::vector<short> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wParams.GetParam(key));
    FillArray<short, Short, IShort>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set int array param
void FifthAbility::WantParamsSetParamCase15(int code)
{
    std::string key = "key";
    std::vector<int> value = {10, 20, 30, 40};

    std::vector<int>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IInteger);
    SetArray<int, Integer>(g_IID_IInteger, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    std::vector<int> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wParams.GetParam(key));
    FillArray<int, Integer, IInteger>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set long array param
void FifthAbility::WantParamsSetParamCase16(int code)
{
    std::string key = "key";
    std::vector<long> value = {100L, 200L, 300L, 400L};

    std::vector<long>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_ILong);
    SetArray<long, Long>(g_IID_ILong, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    std::vector<long> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wParams.GetParam(key));
    FillArray<long, Long, ILong>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set float array param
void FifthAbility::WantParamsSetParamCase17(int code)
{
    std::string key = "key";
    std::vector<float> value = {100.1f, 200.1f, 300.1f, 400.1f};

    std::vector<float>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IFloat);
    SetArray<float, Float>(g_IID_IFloat, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    std::vector<float> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wParams.GetParam(key));
    FillArray<float, Float, IFloat>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// set double array param
void FifthAbility::WantParamsSetParamCase18(int code)
{
    std::string key = "key";
    std::vector<double> value = {100.1, 200.1, 300.1, 400.1};

    std::vector<double>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IDouble);
    SetArray<double, Double>(g_IID_IDouble, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    std::vector<double> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wParams.GetParam(key));
    FillArray<double, Double, IDouble>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// empty key on empty WantParams
void FifthAbility::WantParamsHasParamCase1(int code)
{
    std::string key = "";
    WantParams wParams;
    bool result = (wParams.HasParam(key) == false);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// empty key has value
void FifthAbility::WantParamsHasParamCase2(int code)
{
    std::string key = "";
    sptr<IInterface> value = String::Box("value");
    WantParams wParams;
    wParams.SetParam(key, value);
    bool result = (wParams.HasParam(key) == true);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// not empty key on empty WantParams
void FifthAbility::WantParamsHasParamCase3(int code)
{
    std::string key = "key";
    WantParams wParams;
    bool result = (wParams.HasParam(key) == false);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// not empty key has a value
void FifthAbility::WantParamsHasParamCase4(int code)
{
    std::string key = "key";
    sptr<IInterface> value = String::Box("value");
    WantParams wParams;
    wParams.SetParam(key, value);
    bool result = (wParams.HasParam(key) == true);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// key with special character has a value
void FifthAbility::WantParamsHasParamCase5(int code)
{
    std::string key = "~!@#$%^&*()_+`-=:\"<>?;',./";
    sptr<IInterface> value = String::Box("value");
    WantParams wParams;
    wParams.SetParam(key, value);
    bool result = (wParams.HasParam(key) == true);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// empty WantParams
void FifthAbility::WantParamsIsEmptyCase1(int code)
{
    WantParams wParams;
    bool result = (wParams.IsEmpty() == true);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// WantParams with params
void FifthAbility::WantParamsIsEmptyCase2(int code)
{
    std::string key = "key";
    sptr<IInterface> value = String::Box("value");
    WantParams wParams;
    wParams.SetParam(key, value);
    bool result = (wParams.IsEmpty() == false);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall string param
void FifthAbility::WantParamsMarshallingCase1(int code)
{
    std::string key = "key";
    std::string value = "value";
    WantParams wParams;
    wParams.SetParam(key, String::Box(value));
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    bool result = (String::Unbox(IString::Query(wantParamsOut->GetParam(key))) == value);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall bool param
void FifthAbility::WantParamsMarshallingCase2(int code)
{
    std::string key = "key";
    bool value = true;
    WantParams wParams;
    wParams.SetParam(key, Boolean::Box(value));
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    bool result = (Boolean::Unbox(IBoolean::Query(wantParamsOut->GetParam(key))) == value);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall byte param
void FifthAbility::WantParamsMarshallingCase3(int code)
{
    std::string key = "key";
    byte value = 9;
    WantParams wParams;
    wParams.SetParam(key, Byte::Box(value));
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    bool result = (Byte::Unbox(IByte::Query(wantParamsOut->GetParam(key))) == value);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall char param
void FifthAbility::WantParamsMarshallingCase4(int code)
{
    std::string key = "key";
    zchar value = 'z';
    WantParams wParams;
    wParams.SetParam(key, Char::Box(value));
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    bool result = (Char::Unbox(IChar::Query(wantParamsOut->GetParam(key))) == value);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall short param
void FifthAbility::WantParamsMarshallingCase5(int code)
{
    std::string key = "key";
    short value = 99;
    WantParams wParams;
    wParams.SetParam(key, Short::Box(value));
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    bool result = (Short::Unbox(IShort::Query(wantParamsOut->GetParam(key))) == value);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall int param
void FifthAbility::WantParamsMarshallingCase6(int code)
{
    std::string key = "key";
    int value = 999;
    WantParams wParams;
    wParams.SetParam(key, Integer::Box(value));
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    bool result = (Integer::Unbox(IInteger::Query(wantParamsOut->GetParam(key))) == value);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall long param
void FifthAbility::WantParamsMarshallingCase7(int code)
{
    std::string key = "key";
    long value = 9999L;
    WantParams wParams;
    wParams.SetParam(key, Long::Box(value));
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    bool result = (Long::Unbox(ILong::Query(wantParamsOut->GetParam(key))) == value);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall float param
void FifthAbility::WantParamsMarshallingCase8(int code)
{
    std::string key = "key";
    float value = 9999.99f;
    WantParams wParams;
    wParams.SetParam(key, Float::Box(value));
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    bool result = (Float::Unbox(IFloat::Query(wantParamsOut->GetParam(key))) == value);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall double param
void FifthAbility::WantParamsMarshallingCase9(int code)
{
    std::string key = "key";
    double value = 99998888.99;
    WantParams wParams;
    wParams.SetParam(key, Double::Box(value));
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    bool result = (Double::Unbox(IDouble::Query(wantParamsOut->GetParam(key))) == value);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall string array param
void FifthAbility::WantParamsMarshallingCase10(int code)
{
    std::string key = "key";
    std::vector<std::string> value = {"aa", "bb", "cc"};

    std::vector<std::string>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IString);
    for (std::vector<std::string>::size_type i = 0; i < size; i++) {
        ao->Set(i, String::Box(value[i]));
    }
    WantParams wParams;
    wParams.SetParam(key, ao);
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    std::vector<std::string> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wantParamsOut->GetParam(key));
    FillArray<std::string, String, IString>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall bool array param
void FifthAbility::WantParamsMarshallingCase11(int code)
{
    std::string key = "key";
    std::vector<bool> value = {false, true, true, false};

    std::vector<bool>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IBoolean);
    SetArray<bool, Boolean>(g_IID_IBoolean, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    std::vector<bool> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wantParamsOut->GetParam(key));
    FillArray<bool, Boolean, IBoolean>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall byte array param
void FifthAbility::WantParamsMarshallingCase12(int code)
{
    std::string key = "key";
    std::vector<byte> value = {1, 2, 3, 4};

    std::vector<byte>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IByte);
    SetArray<byte, Byte>(g_IID_IByte, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    std::vector<byte> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wantParamsOut->GetParam(key));
    FillArray<byte, Byte, IByte>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall char array param
void FifthAbility::WantParamsMarshallingCase13(int code)
{
    std::string key = "key";
    std::vector<zchar> value = {'1', '2', '3', '4'};

    std::vector<zchar>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IChar);
    SetArray<zchar, Char>(g_IID_IChar, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    std::vector<zchar> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wantParamsOut->GetParam(key));
    FillArray<zchar, Char, IChar>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall short array param
void FifthAbility::WantParamsMarshallingCase14(int code)
{
    std::string key = "key";
    std::vector<short> value = {'1', '2', '3', '4'};

    std::vector<short>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IShort);
    SetArray<short, Short>(g_IID_IShort, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    std::vector<short> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wantParamsOut->GetParam(key));
    FillArray<short, Short, IShort>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall int array param
void FifthAbility::WantParamsMarshallingCase15(int code)
{
    std::string key = "key";
    std::vector<int> value = {10, 20, 30, 40};

    std::vector<int>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IInteger);
    SetArray<int, Integer>(g_IID_IInteger, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    std::vector<int> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wantParamsOut->GetParam(key));
    FillArray<int, Integer, IInteger>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall long array param
void FifthAbility::WantParamsMarshallingCase16(int code)
{
    std::string key = "key";
    std::vector<long> value = {100L, 200L, 300L, 400L};

    std::vector<long>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_ILong);
    SetArray<long, Long>(g_IID_ILong, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    std::vector<long> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wantParamsOut->GetParam(key));
    FillArray<long, Long, ILong>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall float array param
void FifthAbility::WantParamsMarshallingCase17(int code)
{
    std::string key = "key";
    std::vector<float> value = {100.1f, 200.1f, 300.1f, 400.1f};

    std::vector<float>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IFloat);
    SetArray<float, Float>(g_IID_IFloat, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    std::vector<float> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wantParamsOut->GetParam(key));
    FillArray<float, Float, IFloat>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// marshall and unmarshall double array param
void FifthAbility::WantParamsMarshallingCase18(int code)
{
    std::string key = "key";
    std::vector<double> value = {100.1, 200.1, 300.1, 400.1};

    std::vector<double>::size_type size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IDouble);
    SetArray<double, Double>(g_IID_IDouble, value, ao);

    WantParams wParams;
    wParams.SetParam(key, ao);
    Parcel in;
    wParams.Marshalling(in);
    WantParams *wantParamsOut = WantParams::Unmarshalling(in);
    std::vector<double> getParamValue;
    sptr<IArray> getParamAo = IArray::Query(wantParamsOut->GetParam(key));
    FillArray<double, Double, IDouble>(getParamAo, getParamValue);
    bool result = (value == getParamValue);
    if (wantParamsOut) {
        delete wantParamsOut;
    }
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// no any param
void FifthAbility::WantParamsSizeCase1(int code)
{
    WantParams wParams;
    bool result = (wParams.Size() == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// only one param
void FifthAbility::WantParamsSizeCase2(int code)
{
    std::string key = "key";
    sptr<IInterface> value = String::Box("value");
    WantParams wParams;
    wParams.SetParam(key, value);
    bool result = (wParams.Size() == 1);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// multiple params
void FifthAbility::WantParamsSizeCase3(int code)
{
    std::string key = "key";
    sptr<IInterface> value = nullptr;
    WantParams wParams;
    for (unsigned i = 0; i < loopSize; i++) {
        sptr<IInterface> value = String::Box("value" + std::to_string(i));
        wParams.SetParam(key + std::to_string(i), value);
    }
    bool result = (wParams.Size() == loopSize);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// no any key
void FifthAbility::WantParamsKeySetCase1(int code)
{
    WantParams wParams;
    bool result = (wParams.KeySet().size() == 0);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// multiple keys
void FifthAbility::WantParamsKeySetCase2(int code)
{
    std::string key = "key";
    sptr<IInterface> value = nullptr;
    WantParams wParams;
    for (unsigned i = 0; i < loopSize; i++) {
        sptr<IInterface> value = String::Box("value" + std::to_string(i));
        wParams.SetParam(key + std::to_string(i), value);
    }
    bool result = (wParams.KeySet().size() == loopSize);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// copy empty WantParams
void FifthAbility::WantParamsCopyCase1(int code)
{
    WantParams wParams;
    WantParams wParamsCopy(wParams);
    bool result = (wParamsCopy.Size() == wParams.Size());
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// copy WantParams with multiple param
void FifthAbility::WantParamsCopyCase2(int code)
{
    std::string key = "key";
    sptr<IInterface> value = nullptr;
    WantParams wParams;
    for (unsigned i = 0; i < loopSize; i++) {
        sptr<IInterface> value = String::Box("value" + std::to_string(i));
        wParams.SetParam(key + std::to_string(i), value);
    }
    WantParams wParamsCopy(wParams);
    bool result = (wParamsCopy.Size() == wParams.Size());
    result = result && (*(wParamsCopy.KeySet().begin()) == *(wParams.KeySet().begin())) &&
             (*(wParamsCopy.KeySet().rbegin()) == *(wParams.KeySet().rbegin()));
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// remove not existed param
void FifthAbility::WantParamsRemoveCase1(int code)
{
    std::string key = "key";
    sptr<IInterface> value = String::Box("value");
    WantParams wParams;
    bool result = (wParams.GetParam(key) == nullptr);
    wParams.Remove(key);
    result = result && (wParams.GetParam(key) == nullptr);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

// remove existed param
void FifthAbility::WantParamsRemoveCase2(int code)
{
    std::string key = "key";
    sptr<IInterface> value = String::Box("value");
    WantParams wParams;
    wParams.SetParam(key, value);
    bool result = wParams.GetParam(key) != nullptr;
    wParams.Remove(key);
    result = result && (wParams.GetParam(key) == nullptr);
    TestUtils::PublishEvent(g_EVENT_RESP_FIFTH, code, std::to_string(result));
}

REGISTER_AA(FifthAbility)
}  // namespace AppExecFwk
}  // namespace OHOS