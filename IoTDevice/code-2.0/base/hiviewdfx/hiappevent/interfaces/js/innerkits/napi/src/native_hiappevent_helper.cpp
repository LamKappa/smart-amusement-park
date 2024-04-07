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

#include "native_hiappevent_helper.h"

#include <string>
#include <vector>

#include "hiappevent_base.h"
#include "hiappevent_pack.h"
#include "hiappevent_write.h"
#include "hilog/log.h"

using namespace OHOS::HiviewDFX;
using namespace OHOS::HiviewDFX::ErrorCode;

namespace OHOS {
namespace HiviewDFX {
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, HIAPPEVENT_DOMAIN, "HiAppEvent_NAPI" };
constexpr static int EVENT_NAME_INDEX = 0;
constexpr static int EVENT_TYPE_INDEX = 1;
constexpr static int JSON_OBJECT_INDEX = 2;
constexpr static int CALLBACK_FUNC_PARAM_NUM = 2;
constexpr static int NAPI_VALUE_STRING_LEN = 1024;
constexpr static int SUCCESS_FLAG = 0;

void AddBoolParam2EventPack(napi_env env, const std::string key, const napi_value param,
    std::shared_ptr<AppEventPack>& appEventPack)
{
    bool value = true;
    napi_status status = napi_get_value_bool(env, param, &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "AddBoolParam2EventPack：napi_get_value_bool failed.");
        return;
    }

    AddEventParam(appEventPack, key, value);
}

void AddNumberParam2EventPack(napi_env env, const std::string key, const napi_value param,
    std::shared_ptr<AppEventPack>& appEventPack)
{
    double value;
    napi_status status = napi_get_value_double(env, param, &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "AddNumberParam2EventPack：napi_get_value_double failed.");
        return;
    }

    AddEventParam(appEventPack, key, value);
}

void AddStringParam2EventPack(napi_env env, const std::string key, const napi_value param,
    std::shared_ptr<AppEventPack>& appEventPack)
{
    char value[NAPI_VALUE_STRING_LEN] = {0};
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, param, value, NAPI_VALUE_STRING_LEN - 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "AddStringParam2EventPack：napi_get_value_string_utf8 failed.");
        return;
    }

    std::string valueStr = value;
    AddEventParam(appEventPack, key, valueStr);
}

void AddBoolArrayParam2EventPack(napi_env env, const std::string key, const napi_value arrParam,
    std::shared_ptr<AppEventPack>& appEventPack)
{
    uint32_t len = 0;
    napi_status status = napi_get_array_length(env, arrParam, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "AddBoolArrayParam2EventPack：napi_get_array_length failed.");
        return;
    }

    std::vector<bool> bools;
    napi_value element;
    napi_valuetype type;
    for (uint32_t i = 0; i < len; i++) {
        status = napi_get_element(env, arrParam, i, &element);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "AddBoolArrayParam2EventPack：napi_get_element failed.");
            return;
        }

        status = napi_typeof(env, element, &type);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "AddBoolArrayParam2EventPack：napi get element type failed.");
            return;
        }

        if (type != napi_valuetype::napi_boolean) {
            HiLog::Error(LABEL, "AddBoolArrayParam2EventPack：the array types must all be the same boolean type.");
            return;
        }

        bool value = true;
        napi_status status = napi_get_value_bool(env, element, &value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "AddBoolArrayParam2EventPack：napi_get_value_bool failed.");
            return;
        }
        bools.push_back(value);
    }
    AddEventParam(appEventPack, key, bools);
}

void AddNumberArrayParam2EventPack(napi_env env, const std::string key, const napi_value arrParam,
    std::shared_ptr<AppEventPack>& appEventPack)
{
    uint32_t len = 0;
    napi_status status = napi_get_array_length(env, arrParam, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "AddNumberArrayParam2EventPack：napi_get_array_length failed.");
        return;
    }

    std::vector<double> doubles;
    napi_value element;
    napi_valuetype type;
    for (uint32_t i = 0; i < len; i++) {
        status = napi_get_element(env, arrParam, i, &element);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "AddNumberArrayParam2EventPack：napi_get_element failed.");
            return;
        }

        status = napi_typeof(env, element, &type);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "AddNumberArrayParam2EventPack：napi get element type failed.");
            return;
        }

        if (type != napi_valuetype::napi_number) {
            HiLog::Error(LABEL, "AddNumberArrayParam2EventPack：the array types must all be the same number type.");
            return;
        }

        double value;
        napi_status status = napi_get_value_double(env, element, &value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "AddNumberArrayParam2EventPack：napi_get_value_double failed.");
            return;
        }
        doubles.push_back(value);
    }
    AddEventParam(appEventPack, key, doubles);
}

void AddStringArrayParam2EventPack(napi_env env, const std::string key, const napi_value arrParam,
    std::shared_ptr<AppEventPack>& appEventPack)
{
    uint32_t len = 0;
    napi_status status = napi_get_array_length(env, arrParam, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "AddStringArrayParam2EventPack：napi_get_array_length failed.");
        return;
    }

    std::vector<const std::string> strs;
    napi_value element;
    napi_valuetype type;
    for (uint32_t i = 0; i < len; i++) {
        status = napi_get_element(env, arrParam, i, &element);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "AddStringArrayParam2EventPack：napi_get_element failed.");
            return;
        }

        status = napi_typeof(env, element, &type);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "AddStringArrayParam2EventPack：napi get element type failed.");
            return;
        }

        if (type != napi_valuetype::napi_string) {
            HiLog::Error(LABEL, "AddStringArrayParam2EventPack：the array types must all be the same string type.");
            return;
        }

        char value[NAPI_VALUE_STRING_LEN] = {0};
        size_t len = 0;
        napi_status status = napi_get_value_string_utf8(env, element, value, NAPI_VALUE_STRING_LEN - 1, &len);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "AddStringArrayParam2EventPack：napi_get_value_string_utf8 failed.");
            return;
        }
        strs.push_back(value);
    }
    AddEventParam(appEventPack, key, strs);
}

int AddArrayParam2EventPack(napi_env env, const std::string key, const napi_value arrParam,
    std::shared_ptr<AppEventPack>& appEventPack)
{
    uint32_t len = 0;
    napi_status status = napi_get_array_length(env, arrParam, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "AddArrayParam2EventPack：napi_get_array_length failed.");
        return ERROR_UNKNOWN;
    }

    if (len == 0) {
        HiLog::Warn(LABEL, "key=%{public}s array value is empty.", key.c_str());
        AddEventParam(appEventPack, key);
        return SUCCESS_FLAG;
    }

    napi_value value;
    status = napi_get_element(env, arrParam, 0, &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "AddArrayParam2EventPack：napi_get_element failed.");
        return ERROR_UNKNOWN;
    }

    napi_valuetype type;
    status = napi_typeof(env, value, &type);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "AddArrayParam2EventPack：napi get element type failed.");
        return ERROR_UNKNOWN;
    }

    if (type == napi_valuetype::napi_boolean) {
        AddBoolArrayParam2EventPack(env, key, arrParam, appEventPack);
    } else if (type == napi_valuetype::napi_number) {
        AddNumberArrayParam2EventPack(env, key, arrParam, appEventPack);
    } else if (type == napi_valuetype::napi_string) {
        AddStringArrayParam2EventPack(env, key, arrParam, appEventPack);
    } else {
        HiLog::Error(LABEL, "invalid array parameter type of key=%{public}s.", key.c_str());
        return ERROR_INVALID_PARAM_VALUE_LIST_TYPE;
    }

    return SUCCESS_FLAG;
}

int AddParam2EventPack(napi_env env, const std::string key, const napi_value param,
    std::shared_ptr<AppEventPack>& appEventPack)
{
    napi_valuetype type;
    napi_status status = napi_typeof(env, param, &type);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "AddParam2EventPack：napi_typeof failed.");
        return ERROR_UNKNOWN;
    }

    if (type == napi_valuetype::napi_boolean) {
        AddBoolParam2EventPack(env, key, param, appEventPack);
    } else if (type == napi_valuetype::napi_number) {
        AddNumberParam2EventPack(env, key, param, appEventPack);
    } else if (type == napi_valuetype::napi_string) {
        AddStringParam2EventPack(env, key, param, appEventPack);
    } else if (type == napi_valuetype::napi_object) {
        int addArrParamsRes = AddArrayParam2EventPack(env, key, param, appEventPack);
        if (addArrParamsRes != SUCCESS_FLAG) {
            HiLog::Error(LABEL, "key=%{public}s add Array params to AppEventPack failed.", key.c_str());
            return addArrParamsRes;
        }
    } else {
        HiLog::Error(LABEL, "invalid parameter type of key=%{public}s.", key.c_str());
        return ERROR_INVALID_PARAM_VALUE_TYPE;
    }

    return SUCCESS_FLAG;
}
}

int BuildAppEventPack(napi_env env, const napi_value params[], const int paramEndIndex,
    std::shared_ptr<AppEventPack>& appEventPack)
{
    const int valuePost = 1;
    const int kvNumber = 2;
    const int paramStartIndex = 2;
    std::string keyStr;
    int buildRes = SUCCESS_FLAG;
    for (int index = paramStartIndex; index < paramEndIndex; index++) {
        if (index % kvNumber == valuePost) {
            if (keyStr.empty()) {
                HiLog::Error(LABEL, "the app event param name cannot be empty.");
                buildRes = ERROR_INVALID_PARAM_NAME;
                continue;
            }

            int addParamsRes = AddParam2EventPack(env, keyStr, params[index], appEventPack);
            if (addParamsRes != SUCCESS_FLAG) {
                HiLog::Error(LABEL, "key=%{public}s add params to AppEventPack failed.", keyStr.c_str());
                buildRes = addParamsRes;
                continue;
            }

            keyStr.clear();
        } else {
            napi_valuetype type;
            napi_typeof(env, params[index], &type);

            if (type == napi_valuetype::napi_string) {
                char key[NAPI_VALUE_STRING_LEN] = {0};
                size_t len = 0;
                napi_get_value_string_utf8(env, params[index], key, NAPI_VALUE_STRING_LEN - 1, &len);
                keyStr = key;
            } else {
                HiLog::Error(LABEL, "the key of the app event params must be String.");
                buildRes = ERROR_INVALID_PARAM_KEY_TYPE;
                index++;
                continue;
            }
        }
    }

    return buildRes;
}

int BuildAppEventPackFromObject(napi_env env, const napi_value object, std::shared_ptr<AppEventPack>& appEventPack)
{
    napi_value keyArr = nullptr;
    napi_status status = napi_get_property_names(env, object, &keyArr);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "napi_get_property_names failed.");
        return ERROR_UNKNOWN;
    }

    uint32_t len = 0;
    status = napi_get_array_length(env, keyArr, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "napi_get_array_length failed.");
        return ERROR_UNKNOWN;
    }

    int buildRes = SUCCESS_FLAG;
    for (uint32_t i = 0; i < len; i++) {
        napi_value keyNapiValue = nullptr;
        napi_get_element(env, keyArr, i, &keyNapiValue);

        napi_valuetype valueType;
        napi_typeof(env, keyNapiValue, &valueType);
        if (valueType != napi_valuetype::napi_string) {
            HiLog::Error(LABEL, "the key of the app event params must be String.");
            buildRes = ERROR_INVALID_PARAM_KEY_TYPE;
            continue;
        }

        char key[NAPI_VALUE_STRING_LEN] = {0};
        size_t cValueLength = 0;
        napi_get_value_string_utf8(env, keyNapiValue, key, NAPI_VALUE_STRING_LEN - 1, &cValueLength);

        napi_value value = nullptr;
        napi_get_named_property(env, object, key, &value);

        std::string keyStr = key;
        napi_typeof(env, value, &valueType);
        if (valueType == napi_valuetype::napi_boolean) {
            AddBoolParam2EventPack(env, keyStr, value, appEventPack);
        } else if (valueType == napi_valuetype::napi_number) {
            AddNumberParam2EventPack(env, keyStr, value, appEventPack);
        } else if (valueType == napi_valuetype::napi_string) {
            AddStringParam2EventPack(env, keyStr, value, appEventPack);
        } else if (valueType == napi_valuetype::napi_object) {
            int addArrParamsRes = AddArrayParam2EventPack(env, keyStr, value, appEventPack);
            if (addArrParamsRes != SUCCESS_FLAG) {
                HiLog::Error(LABEL, "key=%{public}s add Array params to AppEventPack failed.", key);
                buildRes = addArrParamsRes;
                continue;
            }
        } else {
            HiLog::Error(LABEL, "invalid param value type of key=%{public}s.", key);
            buildRes = ERROR_INVALID_PARAM_VALUE_TYPE;
            continue;
        }
    }

    return buildRes;
}

bool CheckWriteParamsType(napi_env env, const napi_value params[], const size_t len)
{
    if (len < (EVENT_TYPE_INDEX + 1)) {
        HiLog::Error(LABEL, "the length of the params array is less than 2.");
        return false;
    }

    napi_valuetype valueType;
    napi_typeof(env, params[EVENT_NAME_INDEX], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HiLog::Error(LABEL, "the first param must be of type string.");
        return false;
    }

    napi_typeof(env, params[EVENT_TYPE_INDEX], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        HiLog::Error(LABEL, "the second param must be of type number.");
        return false;
    }

    return true;
}

bool CheckWriteJsonParamsType(napi_env env, const napi_value params[], const size_t len)
{
    if (len < (JSON_OBJECT_INDEX + 1)) {
        HiLog::Error(LABEL, "the length of the params array is less than 3.");
        return false;
    }

    napi_valuetype valueType;
    napi_typeof(env, params[EVENT_NAME_INDEX], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HiLog::Error(LABEL, "the first param must be of type string.");
        return false;
    }

    napi_typeof(env, params[EVENT_TYPE_INDEX], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        HiLog::Error(LABEL, "the second param must be of type number.");
        return false;
    }

    napi_typeof(env, params[JSON_OBJECT_INDEX], &valueType);
    if (valueType != napi_valuetype::napi_object) {
        HiLog::Error(LABEL, "the third param must be of type object.");
        return false;
    }

    return true;
}

std::shared_ptr<AppEventPack> CreateEventPackFromNapiValue(napi_env env, napi_value nameValue, napi_value typeValue)
{
    char eventName[NAPI_VALUE_STRING_LEN] = {0};
    size_t cValueLength = 0;
    napi_get_value_string_utf8(env, nameValue, eventName, NAPI_VALUE_STRING_LEN - 1, &cValueLength);

    int32_t eventType = 0;
    napi_get_value_int32(env, typeValue, &eventType);

    HiLog::Debug(LABEL, "write eventName is %{public}s, eventType is %{public}d", eventName, eventType);
    return std::make_shared<AppEventPack>(eventName, eventType);
}

void AsyncWriteEvent(napi_env env, HiAppEventAsyncContext* asyncContext)
{
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSHiAppEventWrite", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            HiAppEventAsyncContext* asyncContext = (HiAppEventAsyncContext*)data;
            if (asyncContext->appEventPack != nullptr && asyncContext->result >= SUCCESS_FLAG) {
                WriterEvent(asyncContext->appEventPack);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            HiAppEventAsyncContext* asyncContext = (HiAppEventAsyncContext*)data;
            napi_value result[CALLBACK_FUNC_PARAM_NUM] = {0};
            if (asyncContext->result == SUCCESS_FLAG) {
                napi_get_undefined(env, &result[0]);
                napi_create_int32(env, asyncContext->result, &result[1]);
            } else {
                napi_value message = nullptr;
                napi_create_string_utf8(env, std::to_string(asyncContext->result).c_str(), NAPI_AUTO_LENGTH, &message);
                napi_create_error(env, nullptr, message, &result[0]);
                napi_get_undefined(env, &result[1]);
            }

            if (asyncContext->deferred) {
                if (asyncContext->result == SUCCESS_FLAG) {
                    napi_resolve_deferred(env, asyncContext->deferred, result[1]);
                } else {
                    napi_reject_deferred(env, asyncContext->deferred, result[0]);
                }
            } else {
                napi_value callback = nullptr;
                napi_get_reference_value(env, asyncContext->callback, &callback);
                napi_value retValue = nullptr;
                napi_call_function(env, nullptr, callback, CALLBACK_FUNC_PARAM_NUM, result, &retValue);
                napi_delete_reference(env, asyncContext->callback);
            }

            napi_delete_async_work(env, asyncContext->asyncWork);
            delete asyncContext;
        },
        (void*)asyncContext, &asyncContext->asyncWork);
    napi_queue_async_work(env, asyncContext->asyncWork);
}
}
}