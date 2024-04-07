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

#include "intl_addon.h"

#include <vector>

#include "hilog/log.h"
#include "node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
#define GET_PARAMS(env, info, num)      \
    size_t argc = num;                  \
    napi_value argv[num];               \
    napi_value thisVar = nullptr;       \
    void *data = nullptr;               \
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data)

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "IntlJs" };
using namespace OHOS::HiviewDFX;

IntlAddon::IntlAddon() : env_(nullptr), wrapper_(nullptr) {}

IntlAddon::~IntlAddon()
{
    napi_delete_reference(env_, wrapper_);
}

void IntlAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (nativeObject == nullptr) {
        return;
    }
    reinterpret_cast<IntlAddon *>(nativeObject)->~IntlAddon();
}

napi_value IntlAddon::InitLocale(napi_env env, napi_value exports)
{
    napi_status status;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_GETTER("language", GetLanguage),
        DECLARE_NAPI_GETTER("baseName", GetBaseName),
        DECLARE_NAPI_GETTER("region", GetRegion),
        DECLARE_NAPI_GETTER("script", GetScript),
    };

    napi_value constructor;
    status = napi_define_class(env, "Locale", NAPI_AUTO_LENGTH, LocaleConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Define class failed when InitLocale");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "Locale", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Set property failed when InitLocale");
        return nullptr;
    }
    return exports;
}

napi_value IntlAddon::InitDateTimeFormat(napi_env env, napi_value exports)
{
    napi_status status;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", Format),
    };

    napi_value constructor;
    status = napi_define_class(env, "DateTimeFormat", NAPI_AUTO_LENGTH, DateTimeFormatConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Define class failed when InitDateTimeFormat");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "DateTimeFormat", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Set property failed when InitDateTimeFormat");
        return nullptr;
    }
    return exports;
}

napi_value IntlAddon::LocaleConstructor(napi_env env, napi_callback_info info)
{
    // Need to get one parameter of a locale in string format to create Locale object.
    GET_PARAMS(env, info, 1);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    size_t len;
    napi_status status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get locale tag length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get locale tag failed");
        return nullptr;
    }

    std::unique_ptr<IntlAddon> obj = std::make_unique<IntlAddon>();
    if (obj == nullptr) {
        HiLog::Error(LABEL, "Create IntlAddon failed");
        return nullptr;
    }

    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), IntlAddon::Destructor, nullptr, &obj->wrapper_);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Wrap IntlAddon failed");
        return nullptr;
    }

    std::string localeTag = buf.data();
    if (!obj->InitLocaleContext(env, info, localeTag)) {
        return nullptr;
    }

    obj.release();

    return thisVar;
}

bool IntlAddon::InitLocaleContext(napi_env env, napi_callback_info info, const std::string localeTag)
{
    napi_value global;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get global failed");
        return false;
    }
    env_ = env;
    locale_ = std::make_unique<LocaleInfo>(localeTag);

    return locale_ != nullptr;
}

napi_value IntlAddon::DateTimeFormatConstructor(napi_env env, napi_callback_info info)
{
    // Need to get one parameter of a locale in string format to create DateTimeFormat object.
    GET_PARAMS(env, info, 1);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    size_t len;
    napi_status status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get locale tag length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get locale tag failed");
        return nullptr;
    }
    std::unique_ptr<IntlAddon> obj = std::make_unique<IntlAddon>();
    if (obj == nullptr) {
        HiLog::Error(LABEL, "Create IntlAddon failed");
        return nullptr;
    }

    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), IntlAddon::Destructor, nullptr, &obj->wrapper_);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Wrap IntlAddon failed");
        return nullptr;
    }

    std::string localeTag = buf.data();
    if (!obj->InitDateTimeFormatContext(env, info, localeTag)) {
        HiLog::Error(LABEL, "Init DateTimeFormat failed");
        return nullptr;
    }

    obj.release();
    return thisVar;
}

bool IntlAddon::InitDateTimeFormatContext(napi_env env, napi_callback_info info, const std::string localeTag)
{
    napi_value global;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get global failed");
        return false;
    }
    env_ = env;
    datefmt_ = std::make_unique<DateTimeFormat>(localeTag);

    return datefmt_ != nullptr;
}

napi_value IntlAddon::Format(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, 1); // Need to get one parameter of a date object to format.
    int64_t year = GetYear(env, argv);
    int64_t month = GetMonth(env, argv);
    int64_t day = GetDay(env, argv);
    int64_t hour = GetHour(env, argv);
    int64_t minute = GetMinute(env, argv);
    int64_t second = GetSecond(env, argv);
    if (year == -1 || month == -1 || day == -1 || hour == -1 || minute == -1 || second == -1) {
        return nullptr;
    }
    IntlAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || obj == nullptr || obj->datefmt_ == nullptr) {
        HiLog::Error(LABEL, "Get DateTimeFormat object failed");
        return nullptr;
    }
    std::string value = obj->datefmt_->Format(year, month, day, hour, minute, second);
    napi_value result;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create format string failed");
        return nullptr;
    }
    return result;
}

int64_t IntlAddon::GetYear(napi_env env, napi_value *argv)
{
    napi_value funcGetDateInfo;
    napi_status status = napi_get_named_property(env, argv[0], "getFullYear", &funcGetDateInfo);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get year property failed");
        return -1;
    }
    napi_value ret_value;
    status = napi_call_function(env, argv[0], funcGetDateInfo, 0, nullptr, &ret_value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get year function failed");
        return -1;
    }
    int64_t year;
    status = napi_get_value_int64(env, ret_value, &year);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get year failed");
        return -1;
    }
    return year;
}

int64_t IntlAddon::GetMonth(napi_env env, napi_value *argv)
{
    napi_value funcGetDateInfo;
    napi_status status = napi_get_named_property(env, argv[0], "getMonth", &funcGetDateInfo);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get month property failed");
        return -1;
    }
    napi_value ret_value;
    status = napi_call_function(env, argv[0], funcGetDateInfo, 0, nullptr, &ret_value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get month function failed");
        return -1;
    }
    int64_t month;
    status = napi_get_value_int64(env, ret_value, &month);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get month failed");
        return -1;
    }
    return month;
}

int64_t IntlAddon::GetDay(napi_env env, napi_value *argv)
{
    napi_value funcGetDateInfo;
    napi_status status = napi_get_named_property(env, argv[0], "getDate", &funcGetDateInfo);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get day property failed");
        return -1;
    }
    napi_value ret_value;
    status = napi_call_function(env, argv[0], funcGetDateInfo, 0, nullptr, &ret_value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get day function failed");
        return -1;
    }
    int64_t day;
    status = napi_get_value_int64(env, ret_value, &day);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get day failed");
        return -1;
    }
    return day;
}

int64_t IntlAddon::GetHour(napi_env env, napi_value *argv)
{
    napi_value funcGetDateInfo;
    napi_status status = napi_get_named_property(env, argv[0], "getHours", &funcGetDateInfo);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get hour property failed");
        return -1;
    }
    napi_value ret_value;
    status = napi_call_function(env, argv[0], funcGetDateInfo, 0, nullptr, &ret_value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get hour function failed");
        return -1;
    }
    int64_t hour;
    status = napi_get_value_int64(env, ret_value, &hour);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get hour failed");
        return -1;
    }
    return hour;
}

int64_t IntlAddon::GetMinute(napi_env env, napi_value *argv)
{
    napi_value funcGetDateInfo;
    napi_status status = napi_get_named_property(env, argv[0], "getMinutes", &funcGetDateInfo);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get minute property failed");
        return -1;
    }
    napi_value ret_value;
    status = napi_call_function(env, argv[0], funcGetDateInfo, 0, nullptr, &ret_value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get minute function failed");
        return -1;
    }
    int64_t minute;
    status = napi_get_value_int64(env, ret_value, &minute);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get minute failed");
        return -1;
    }
    return minute;
}

int64_t IntlAddon::GetSecond(napi_env env, napi_value *argv)
{
    napi_value funcGetDateInfo;
    napi_status status = napi_get_named_property(env, argv[0], "getSeconds", &funcGetDateInfo);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get second property failed");
        return -1;
    }
    napi_value ret_value;
    status = napi_call_function(env, argv[0], funcGetDateInfo, 0, nullptr, &ret_value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get second function failed");
        return -1;
    }
    int64_t second;
    status = napi_get_value_int64(env, ret_value, &second);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get second failed");
        return -1;
    }
    return second;
}

napi_value IntlAddon::GetLanguage(napi_env env, napi_callback_info info)
{
    // No parameters are needed to get the language.
    GET_PARAMS(env, info, 0);

    IntlAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || obj == nullptr || obj->locale_ == nullptr) {
        HiLog::Error(LABEL, "Get Locale object failed");
        return nullptr;
    }
    std::string value = obj->locale_->GetLanguage();

    napi_value result;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create language string failed");
        return nullptr;
    }
    return result;
}

napi_value IntlAddon::GetScript(napi_env env, napi_callback_info info)
{
    // No parameters are needed to get the script.
    GET_PARAMS(env, info, 0);

    IntlAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || obj == nullptr || obj->locale_ == nullptr) {
        HiLog::Error(LABEL, "Get Locale object failed");
        return nullptr;
    }
    std::string value = obj->locale_->GetScript();

    napi_value result;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create script string failed");
        return nullptr;
    }
    return result;
}

napi_value IntlAddon::GetRegion(napi_env env, napi_callback_info info)
{
    // No parameters are needed to get the region.
    GET_PARAMS(env, info, 0);

    IntlAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || obj == nullptr || obj->locale_ == nullptr) {
        HiLog::Error(LABEL, "Get Locale object failed");
        return nullptr;
    }
    std::string value = obj->locale_->GetRegion();

    napi_value result;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create region string failed");
        return nullptr;
    }
    return result;
}

napi_value IntlAddon::GetBaseName(napi_env env, napi_callback_info info)
{
    // No parameters are needed to get the baseName.
    GET_PARAMS(env, info, 0);

    IntlAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || obj == nullptr || obj->locale_ == nullptr) {
        HiLog::Error(LABEL, "Get Locale object failed");
        return nullptr;
    }
    std::string value = obj->locale_->GetBaseName();

    napi_value result;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create base name string failed");
        return nullptr;
    }
    return result;
}

napi_value Init(napi_env env, napi_value exports)
{
    napi_value val = IntlAddon::InitLocale(env, exports);
    return IntlAddon::InitDateTimeFormat(env, val);
}

static napi_module g_intlModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "intl",
    .nm_priv = ((void *)0),
    .reserved = { 0 }
};

extern "C" __attribute__((constructor)) void AbilityRegister()
{
    napi_module_register(&g_intlModule);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS