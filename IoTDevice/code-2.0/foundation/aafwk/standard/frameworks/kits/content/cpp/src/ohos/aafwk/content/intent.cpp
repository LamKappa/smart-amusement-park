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

#include "ohos/aafwk/content/intent.h"
#include "ohos/aafwk/base/array_wrapper.h"
#include "ohos/aafwk/base/base_object.h"
#include "ohos/aafwk/base/bool_wrapper.h"
#include "ohos/aafwk/base/zchar_wrapper.h"
#include "ohos/aafwk/base/byte_wrapper.h"
#include "ohos/aafwk/base/short_wrapper.h"
#include "ohos/aafwk/base/int_wrapper.h"
#include "ohos/aafwk/base/long_wrapper.h"
#include "ohos/aafwk/base/float_wrapper.h"
#include "ohos/aafwk/base/double_wrapper.h"
#include "ohos/aafwk/base/string_wrapper.h"
#include "ohos/aafwk/base/zchar_wrapper.h"
#include "string_ex.h"

#include <climits>
#include <securec.h>

#include "parcel.h"

using OHOS::AppExecFwk::ElementName;

namespace OHOS {
namespace AAFwk {

const std::string Intent::ACTION_PLAY("action.system.play");

const std::string Intent::ENTITY_HOME("entity.system.home");
const std::string Intent::ENTITY_VIDEO("entity.system.video");

const std::string Intent::OCT_EQUALSTO("075");   // '='
const std::string Intent::OCT_SEMICOLON("073");  // ';'

static constexpr int HEX_STRING_BUF_LEN = 12;
static constexpr int HEX_STRING_LEN = 10;  // "0xffffffff"

Intent::Intent() : flags_(0)
{}

std::string Intent::GetAction() const
{
    return action_;
}

Intent &Intent::SetAction(const std::string &action)
{
    action_ = action;
    return *this;
}

std::string Intent::GetEntity() const
{
    return entity_;
}

Intent &Intent::SetEntity(const std::string &entity)
{
    entity_ = entity;
    return *this;
}

unsigned int Intent::GetFlags() const
{
    return flags_;
}

Intent &Intent::SetFlag(const unsigned int flag)
{
    flags_ = flag;
    return *this;
}

Intent &Intent::AddFlag(const unsigned int flag)
{
    flags_ |= flag;
    return *this;
}

void Intent::RemoveFlags(const unsigned int flag)
{
    flags_ &= ~flag;
}

ElementName Intent::GetElement() const
{
    return element_;
}

Intent &Intent::SetElement(const ElementName &element)
{
    element_ = element;
    return *this;
}

bool Intent::ParseFlag(const std::string &content, Intent &intent)
{
    std::string contentLower = LowerStr(content);
    std::string prefix = "0x";
    if (!contentLower.empty()) {
        if (contentLower.find(prefix) != 0) {
            return false;
        }

        for (std::size_t i = prefix.length(); i < contentLower.length(); i++) {
            if (!isxdigit(contentLower[i])) {
                return false;
            }
        }
        int base = 16;  // hex string
        unsigned int flag = std::stoi(contentLower, nullptr, base);
        intent.SetFlag(flag);
    }
    return true;
}

std::string Intent::Decode(const std::string &str)
{
    std::string decode;

    for (std::size_t i = 0; i < str.length();) {
        if (str[i] == '\\') {
            if (++i >= str.length()) {
                decode += "\\";
                break;
            }
            if (str[i] == '\\') {
                decode += "\\";
                i++;
            } else if (str[i] == '0') {
                if (str.compare(i, OCT_EQUALSTO.length(), OCT_EQUALSTO) == 0) {
                    decode += "=";
                    i += OCT_EQUALSTO.length();
                } else if (str.compare(i, OCT_SEMICOLON.length(), OCT_SEMICOLON) == 0) {
                    decode += ";";
                    i += OCT_SEMICOLON.length();
                } else {
                    decode += "\\" + str.substr(i, 1);
                    i++;
                }
            } else {
                decode += "\\" + str.substr(i, 1);
                i++;
            }
        } else {
            decode += str[i];
            i++;
        }
    }

    return decode;
}

std::string Intent::Encode(const std::string &str)
{
    std::string encode;

    for (std::size_t i = 0; i < str.length(); i++) {
        if (str[i] == '\\') {
            encode += "\\\\";
        } else if (str[i] == '=') {
            encode += "\\" + OCT_EQUALSTO;
        } else if (str[i] == ';') {
            encode += "\\" + OCT_SEMICOLON;
        } else {
            encode += str[i];
        }
    }

    return encode;
}

bool Intent::ParseContent(const std::string &content, std::string &prop, std::string &value)
{
    std::string subString;
    std::size_t pos = content.find("=");
    if (pos != std::string::npos) {
        subString = content.substr(0, pos);
        prop = Decode(subString);
        subString = content.substr(pos + 1, content.length() - pos - 1);
        value = Decode(subString);
        return true;
    }
    return false;
}

bool Intent::ParseUriInternal(const std::string &content, ElementName &element, Intent &intent)
{
    static constexpr int TYPE_TAG_SIZE = 2;

    std::string prop;
    std::string value;

    if (content.empty() || content[0] == '=') {
        return true;
    }

    if (!ParseContent(content, prop, value)) {
        return false;
    }

    if (value.empty()) {
        return true;
    }

    if (prop == "action") {
        intent.SetAction(value);
    } else if (prop == "entity") {
        intent.SetEntity(value);
    } else if (prop == "flag") {
        if (!ParseFlag(value, intent)) {
            return false;
        }
    } else if (prop == "device") {
        element.SetDeviceID(value);
    } else if (prop == "bundle") {
        element.SetBundleName(value);
    } else if (prop == "ability") {
        element.SetAbilityName(value);
    } else if (prop.length() > TYPE_TAG_SIZE) {
        std::string key = prop.substr(TYPE_TAG_SIZE);
        if (prop[0] == String::SIGNATURE && prop[1] == '.') {
            sptr<IString> valueObj = String::Parse(value);
            if (valueObj == nullptr) {
                return false;
            }
            intent.parameters_.SetParam(key, valueObj);
        } else if (prop[0] == Boolean::SIGNATURE && prop[1] == '.') {
            sptr<IBoolean> valueObj = Boolean::Parse(value);
            if (valueObj == nullptr) {
                return false;
            }
            intent.parameters_.SetParam(key, valueObj);
        } else if (prop[0] == Char::SIGNATURE && prop[1] == '.') {
            sptr<IChar> valueObj = Char::Parse(value);
            if (valueObj == nullptr) {
                return false;
            }
            intent.parameters_.SetParam(key, valueObj);
        } else if (prop[0] == Byte::SIGNATURE && prop[1] == '.') {
            sptr<IByte> valueObj = Byte::Parse(value);
            if (valueObj == nullptr) {
                return false;
            }
            intent.parameters_.SetParam(key, valueObj);
        } else if (prop[0] == Short::SIGNATURE && prop[1] == '.') {
            sptr<IShort> valueObj = Short::Parse(value);
            if (valueObj == nullptr) {
                return false;
            }
            intent.parameters_.SetParam(key, valueObj);
        } else if (prop[0] == Integer::SIGNATURE && prop[1] == '.') {
            sptr<IInteger> valueObj = Integer::Parse(value);
            if (valueObj == nullptr) {
                return false;
            }
            intent.parameters_.SetParam(key, valueObj);
        } else if (prop[0] == Long::SIGNATURE && prop[1] == '.') {
            sptr<ILong> valueObj = Long::Parse(value);
            if (valueObj == nullptr) {
                return false;
            }
            intent.parameters_.SetParam(key, valueObj);
        } else if (prop[0] == Float::SIGNATURE && prop[1] == '.') {
            sptr<IFloat> valueObj = Float::Parse(value);
            if (valueObj == nullptr) {
                return false;
            }
            intent.parameters_.SetParam(key, valueObj);
        } else if (prop[0] == Double::SIGNATURE && prop[1] == '.') {
            sptr<IDouble> valueObj = Double::Parse(value);
            if (valueObj == nullptr) {
                return false;
            }
            intent.parameters_.SetParam(key, valueObj);
        } else if (prop[0] == Array::SIGNATURE && prop[1] == '.') {
            sptr<IArray> valueObj = Array::Parse(value);
            if (valueObj == nullptr) {
                return false;
            }
            intent.parameters_.SetParam(key, valueObj);
        }
    }

    return true;
}

Intent *Intent::ParseUri(const std::string &uri)
{
    if (uri.length() <= 0) {
        return nullptr;
    }

    std::string head = "#Intent;";
    std::string end = ";end";

    if (uri.find(head) != 0) {
        return nullptr;
    }

    if (uri.rfind(end) != (uri.length() - end.length())) {
        return nullptr;
    }

    bool ret = true;
    std::string content;
    std::size_t pos;
    std::size_t begin = head.length();
    ElementName element;
    Intent *intent = new Intent();

    pos = uri.find_first_of(";", begin);
    do {
        if (pos != std::string::npos) {
            content = uri.substr(begin, pos - begin);
            ret = ParseUriInternal(content, element, *intent);
            if (!ret) {
                break;
            }
            begin = pos + 1;
            pos = uri.find(";", begin);
            if (pos == std::string::npos) {
                break;
            }
        } else {
            break;
        }
    } while (true);

    if (ret) {
        intent->SetElement(element);
    } else {
        delete intent;
        intent = nullptr;
    }

    return intent;
}

std::string Intent::ToUri()
{
    std::string uriString = "#Intent;";

    if (action_.length() > 0) {
        uriString += "action=" + Encode(action_) + ";";
    }

    if (entity_.length() > 0) {
        uriString += "entity=" + Encode(entity_) + ";";
    }

    if (flags_ != 0) {
        uriString += "flag=";
        char buf[HEX_STRING_BUF_LEN];
        std::size_t len = snprintf_s(buf, sizeof(buf), HEX_STRING_LEN, "0x%08x", flags_);
        if (len == HEX_STRING_LEN) {
            std::string flag = buf;
            uriString += Encode(flag);
            uriString += ";";
        }
    }

    std::string device = element_.GetDeviceID();
    if (device.length() > 0) {
        uriString += "device=" + Encode(device) + ";";
    }

    std::string bundle = element_.GetBundleName();
    if (bundle.length() > 0) {
        uriString += "bundle=" + Encode(bundle) + ";";
    }

    std::string ability = element_.GetAbilityName();
    if (ability.length() > 0) {
        uriString += "ability=" + Encode(ability) + ";";
    }

    auto params = parameters_.GetParams();
    auto iter = params.cbegin();
    while (iter != params.cend()) {
        sptr<IInterface> o = iter->second;
        if (IString::Query(o) != nullptr) {
            uriString += String::SIGNATURE;
        } else if (IBoolean::Query(o) != nullptr) {
            uriString += Boolean::SIGNATURE;
        } else if (IChar::Query(o) != nullptr) {
            uriString += Char::SIGNATURE;
        } else if (IByte::Query(o) != nullptr) {
            uriString += Byte::SIGNATURE;
        } else if (IShort::Query(o) != nullptr) {
            uriString += Short::SIGNATURE;
        } else if (IInteger::Query(o) != nullptr) {
            uriString += Integer::SIGNATURE;
        } else if (ILong::Query(o) != nullptr) {
            uriString += Long::SIGNATURE;
        } else if (IFloat::Query(o) != nullptr) {
            uriString += Float::SIGNATURE;
        } else if (IDouble::Query(o) != nullptr) {
            uriString += Double::SIGNATURE;
        } else if (IArray::Query(o) != nullptr) {
            uriString += Array::SIGNATURE;
        }
        uriString += "." + Encode(iter->first) + "=" + Encode(Object::ToString(*(o.GetRefPtr()))) + ";";
        iter++;
    }

    uriString += "end";

    return uriString;
}

/*
 * Intent format in Parcel. Java side should keep consistent.
 * +----------+----+-----+------+-------+----+-----------+----
 * |  Action  | E1 | Uri | Type | Flags | E2 | Category1 | ...
 * +----------+----+-----+------+-------+----+-----------+----
 * ----+-----------+---------+----+---------+----+------------+
 * ... | CategoryN | Package | E3 | Element | E4 | Parameters |
 * ----+-----------+---------+----+---------+----+------------+
 * E1: If -1, no Uri after it.
 * E2: Category count. If -1, no Category after it.
 * E3: if -1, no element after it.
 * E4: if -1, no parameters after it.
 */
bool Intent::Marshalling(Parcel &parcel) const
{
    // write action
    if (!parcel.WriteString16(Str8ToStr16(action_))) {
        return false;
    }

    // write entity
    if (!parcel.WriteString16(Str8ToStr16(entity_))) {
        return false;
    }

    // write flags
    if (!parcel.WriteUint32(flags_)) {
        return false;
    }

    // write element
    ElementName emptyElement;
    if (element_ == emptyElement) {
        if (!parcel.WriteInt32(VALUE_NULL)) {
            return false;
        }
    } else {
        if (!parcel.WriteInt32(VALUE_OBJECT)) {
            return false;
        }
        if (!parcel.WriteParcelable(&element_)) {
            return false;
        }
    }

    // write parameters
    if (parameters_.GetParams().size() == 0) {
        if (!parcel.WriteInt32(VALUE_NULL)) {
            return false;
        }
    } else {
        if (!parcel.WriteInt32(VALUE_OBJECT)) {
            return false;
        }
        if (!parcel.WriteParcelable(&parameters_)) {
            return false;
        }
    }

    return true;
}

bool Intent::ReadFromParcel(Parcel &parcel)
{
    // read action
    action_ = Str16ToStr8(parcel.ReadString16());

    // read entity
    entity_ = Str16ToStr8(parcel.ReadString16());

    // read flags
    if (!parcel.ReadUint32(flags_)) {
        return false;
    }

    int empty;
    // read element
    empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }

    if (empty == VALUE_OBJECT) {
        auto element = parcel.ReadParcelable<ElementName>();
        if (element != nullptr) {
            element_ = *element;
            delete element;
        } else {
            return false;
        }
    }

    // read parameters
    empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }

    if (empty == VALUE_OBJECT) {
        auto params = parcel.ReadParcelable<IntentParams>();
        if (params != nullptr) {
            parameters_ = *params;
            delete params;
        } else {
            return false;
        }
    }

    return true;
}

Intent *Intent::Unmarshalling(Parcel &parcel)
{
    Intent *intent = new Intent();
    if (intent && !intent->ReadFromParcel(parcel)) {
        delete intent;
        intent = nullptr;
    }
    return intent;
}

bool Intent::HasParameter(const std::string &key) const
{
    return parameters_.HasParam(key);
}

bool Intent::GetBoolParam(const std::string &key, const bool defaultValue)
{
    auto value = parameters_.GetParam(key);
    IBoolean *bo = IBoolean::Query(value);
    if (bo != nullptr) {
        return Boolean::Unbox(bo);
    }
    return defaultValue;
}

Intent &Intent::SetBoolParam(const std::string &key, const bool value)
{
    parameters_.SetParam(key, Boolean::Box(value));
    return *this;
}

std::vector<bool> Intent::GetBoolArrayParam(const std::string &key)
{
    std::vector<bool> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsBooleanArray(ao)) {
        auto func = [&](IInterface *object) { array.push_back(Boolean::Unbox(IBoolean::Query(object))); };
        Array::ForEach(ao, func);
    }
    return array;
}

Intent &Intent::SetBoolArrayParam(const std::string &key, const std::vector<bool> &value)
{
    long size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IBoolean);
    for (long i = 0; i < size; i++) {
        ao->Set(i, Boolean::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

Intent &Intent::SetCharParam(const std::string &key, const zchar value)
{
    parameters_.SetParam(key, Char::Box(value));
    return *this;
}

Intent &Intent::SetCharArrayParam(const std::string &key, const std::vector<zchar> &value)
{
    long size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IChar);
    for (long i = 0; i < size; i++) {
        ao->Set(i, Char::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

zchar Intent::GetCharParam(const std::string &key, const zchar defaultValue)
{
    auto value = parameters_.GetParam(key);
    IChar *co = IChar::Query(value);
    if (co != nullptr) {
        return Char::Unbox(co);
    }
    return defaultValue;
}

std::vector<zchar> Intent::GetCharArrayParam(const std::string &key)
{
    std::vector<zchar> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsCharArray(ao)) {
        auto func = [&](IInterface *object) { array.push_back(Char::Unbox(IChar::Query(object))); };
        Array::ForEach(ao, func);
    }
    return array;
}

Intent &Intent::SetByteParam(const std::string &key, const byte value)
{
    parameters_.SetParam(key, Byte::Box(value));
    return *this;
}

Intent &Intent::SetByteArrayParam(const std::string &key, const std::vector<byte> &value)
{
    long size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IByte);
    for (long i = 0; i < size; i++) {
        ao->Set(i, Byte::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

byte Intent::GetByteParam(const std::string &key, const byte defaultValue)
{
    auto value = parameters_.GetParam(key);
    IByte *bo = IByte::Query(value);
    if (bo != nullptr) {
        return Byte::Unbox(bo);
    }
    return defaultValue;
}

std::vector<byte> Intent::GetByteArrayParam(const std::string &key)
{
    std::vector<byte> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsByteArray(ao)) {
        auto func = [&](IInterface *object) { array.push_back(Byte::Unbox(IByte::Query(object))); };
        Array::ForEach(ao, func);
    }
    return array;
}

Intent &Intent::SetShortParam(const std::string &key, const short value)
{
    parameters_.SetParam(key, Short::Box(value));
    return *this;
}

Intent &Intent::SetShortArrayParam(const std::string &key, const std::vector<short> &value)
{
    long size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IShort);
    for (long i = 0; i < size; i++) {
        ao->Set(i, Short::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

short Intent::GetShortParam(const std::string &key, const short defaultValue)
{
    auto value = parameters_.GetParam(key);
    IShort *so = IShort::Query(value);
    if (so != nullptr) {
        return Short::Unbox(so);
    }
    return defaultValue;
}

std::vector<short> Intent::GetShortArrayParam(const std::string &key)
{
    std::vector<short> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsShortArray(ao)) {
        auto func = [&](IInterface *object) { array.push_back(Short::Unbox(IShort::Query(object))); };
        Array::ForEach(ao, func);
    }
    return array;
}

Intent &Intent::SetIntParam(const std::string &key, const int value)
{
    parameters_.SetParam(key, Integer::Box(value));
    return *this;
}

Intent &Intent::SetIntArrayParam(const std::string &key, const std::vector<int> &value)
{
    long size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IInteger);
    for (long i = 0; i < size; i++) {
        ao->Set(i, Integer::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

int Intent::GetIntParam(const std::string &key, const int defaultValue)
{
    auto value = parameters_.GetParam(key);
    IInteger *io = IInteger::Query(value);
    if (io != nullptr) {
        return Integer::Unbox(io);
    }
    return defaultValue;
}

std::vector<int> Intent::GetIntArrayParam(const std::string &key)
{
    std::vector<int> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsIntegerArray(ao)) {
        auto func = [&](IInterface *object) { array.push_back(Integer::Unbox(IInteger::Query(object))); };
        Array::ForEach(ao, func);
    }
    return array;
}

long Intent::GetLongParam(const std::string &key, const long defaultValue)
{
    auto value = parameters_.GetParam(key);
    ILong *lo = ILong::Query(value);
    if (lo != nullptr) {
        return Long::Unbox(lo);
    }
    return defaultValue;
}

Intent &Intent::SetLongParam(const std::string &key, const long value)
{
    parameters_.SetParam(key, Long::Box(value));
    return *this;
}

std::vector<long> Intent::GetLongArrayParam(const std::string &key)
{
    std::vector<long> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsLongArray(ao)) {
        auto func = [&](IInterface *object) { array.push_back(Long::Unbox(ILong::Query(object))); };
        Array::ForEach(ao, func);
    }
    return array;
}

Intent &Intent::SetLongArrayParam(const std::string &key, const std::vector<long> &value)
{
    long size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_ILong);
    for (long i = 0; i < size; i++) {
        ao->Set(i, Long::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

float Intent::GetFloatParam(const std::string &key, const float defaultValue)
{
    auto value = parameters_.GetParam(key);
    IFloat *of = IFloat::Query(value);
    if (of != nullptr) {
        return Float::Unbox(of);
    }
    return defaultValue;
}

Intent &Intent::SetFloatParam(const std::string &key, const float value)
{
    parameters_.SetParam(key, Float::Box(value));
    return *this;
}

std::vector<float> Intent::GetFloatArrayParam(const std::string &key)
{
    std::vector<float> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsFloatArray(ao)) {
        auto func = [&](IInterface *object) { array.push_back(Float::Unbox(IFloat::Query(object))); };
        Array::ForEach(ao, func);
    }
    return array;
}

Intent &Intent::SetFloatArrayParam(const std::string &key, const std::vector<float> &value)
{
    long size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IFloat);
    for (long i = 0; i < size; i++) {
        ao->Set(i, Float::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

double Intent::GetDoubleParam(const std::string &key, const double defaultValue)
{
    auto value = parameters_.GetParam(key);
    IDouble *dbo = IDouble::Query(value);
    if (dbo != nullptr) {
        return Double::Unbox(dbo);
    }
    return defaultValue;
}

Intent &Intent::SetDoubleParam(const std::string &key, const double value)
{
    parameters_.SetParam(key, Double::Box(value));
    return *this;
}

std::vector<double> Intent::GetDoubleArrayParam(const std::string &key)
{
    std::vector<double> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsDoubleArray(ao)) {
        auto func = [&](IInterface *object) { array.push_back(Double::Unbox(IDouble::Query(object))); };
        Array::ForEach(ao, func);
    }
    return array;
}

Intent &Intent::SetDoubleArrayParam(const std::string &key, const std::vector<double> &value)
{
    long size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IDouble);
    for (long i = 0; i < size; i++) {
        ao->Set(i, Double::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

std::string Intent::GetStringParam(const std::string &key)
{
    auto value = parameters_.GetParam(key);
    IString *so = IString::Query(value);
    if (so != nullptr) {
        return String::Unbox(so);
    }
    return std::string();
}

Intent &Intent::SetStringParam(const std::string &key, const std::string &value)
{
    parameters_.SetParam(key, String::Box(value));
    return *this;
}

std::vector<std::string> Intent::GetStringArrayParam(const std::string &key)
{
    std::vector<std::string> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsStringArray(ao)) {
        auto func = [&](IInterface *object) { array.push_back(String::Unbox(IString::Query(object))); };
        Array::ForEach(ao, func);
    }
    return array;
}

Intent &Intent::SetStringArrayParam(const std::string &key, const std::vector<std::string> &value)
{
    long size = value.size();
    sptr<IArray> ao = new Array(size, g_IID_IString);
    for (long i = 0; i < size; i++) {
        ao->Set(i, String::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

}  // namespace AAFwk
}  // namespace OHOS
