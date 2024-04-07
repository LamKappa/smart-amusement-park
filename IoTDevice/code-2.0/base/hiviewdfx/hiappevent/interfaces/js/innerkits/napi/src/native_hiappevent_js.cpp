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
#include <cstdio>
#include <string>

#include "hiappevent_base.h"
#include "hiappevent_pack.h"
#include "hiappevent_verify.h"
#include "hilog/log.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_hiappevent_helper.h"

using namespace OHOS::HiviewDFX;
using namespace OHOS::HiviewDFX::ErrorCode;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, HIAPPEVENT_DOMAIN, "HiAppEvent_NAPI" };
constexpr static int EVENT_NAME_INDEX = 0;
constexpr static int EVENT_TYPE_INDEX = 1;
constexpr static int JSON_OBJECT_INDEX = 2;

constexpr static int WRITE_FUNC_MIN_PARAM_NUM = 2;
constexpr static int WRITE_FUNC_MAX_PARAM_NUM = 100;
constexpr static int WRITE_JSON_FUNC_MIN_PARAM_NUM = 2;
constexpr static int WRITE_JSON_FUNC_MAX_PARAM_NUM = 4;
constexpr static int SUCCESS_FLAG = 0;

const static int FAULT_EVENT_TYPE = 1;
const static int STATISTIC_EVENT_TYPE = 2;
const static int SECURITY_EVENT_TYPE = 3;
const static int BEHAVIOR_EVENT_TYPE = 4;
}

static napi_value Write(napi_env env, napi_callback_info info)
{
    HiLog::Debug(LABEL, "Write start to write app event.");

    size_t paramNum = WRITE_FUNC_MAX_PARAM_NUM;
    napi_value params[WRITE_FUNC_MAX_PARAM_NUM] = {0};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramNum, params, &thisArg, &data));

    HiAppEventAsyncContext* asyncContext = new HiAppEventAsyncContext {
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
    };

    int32_t result = SUCCESS_FLAG;
    napi_value promise = nullptr;
    napi_get_undefined(env, &promise);

    if (paramNum >= WRITE_FUNC_MIN_PARAM_NUM && paramNum <= WRITE_FUNC_MAX_PARAM_NUM) {
        if (!CheckWriteParamsType(env, params, sizeof(params) / sizeof(params[0]))) {
            HiLog::Error(LABEL, "event name or event type is invalid parameter type.");
            result = ERROR_INVALID_PARAM_TYPE_JS;
        }

        int paramEndIndex = paramNum;
        napi_valuetype lastParamType;
        napi_typeof(env, params[paramNum - 1], &lastParamType);
        if (lastParamType == napi_valuetype::napi_function) {
            napi_create_reference(env, params[paramNum - 1], 1, &asyncContext->callback);
            paramEndIndex -= 1;
        }

        if (asyncContext->callback == nullptr) {
            napi_create_promise(env, &asyncContext->deferred, &promise);
        }

        if (result == SUCCESS_FLAG) {
            std::shared_ptr<AppEventPack> appEventPack = CreateEventPackFromNapiValue(env, params[EVENT_NAME_INDEX],
                params[EVENT_TYPE_INDEX]);
            if (paramEndIndex > WRITE_FUNC_MIN_PARAM_NUM) {
                int buildRes = BuildAppEventPack(env, params, paramEndIndex, appEventPack);
                if (buildRes != SUCCESS_FLAG) {
                    HiLog::Error(LABEL, "event build AppEventPack failed.");
                    result = buildRes;
                }
            }
            asyncContext->appEventPack = appEventPack;

            int verifyResult = VerifyAppEvent(appEventPack);
            if (verifyResult != SUCCESS_FLAG) {
                HiLog::Error(LABEL, "event verify failed.");
                result = verifyResult;
            }
        }

        asyncContext->result = result;
        AsyncWriteEvent(env, asyncContext);
    } else {
        HiLog::Error(LABEL, "write event failed, invalid number of params.");
    }

    HiLog::Debug(LABEL, "write event end.");
    return promise;
}

static napi_value WriteJson(napi_env env, napi_callback_info info)
{
    HiLog::Debug(LABEL, "writeJson start to write app event.");

    size_t paramNum = WRITE_JSON_FUNC_MAX_PARAM_NUM;
    napi_value params[WRITE_JSON_FUNC_MAX_PARAM_NUM] = {0};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramNum, params, &thisArg, &data));

    HiAppEventAsyncContext* asyncContext = new HiAppEventAsyncContext {
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
    };

    int32_t result = SUCCESS_FLAG;
    napi_value promise = nullptr;
    napi_get_undefined(env, &promise);

    if (paramNum >= WRITE_JSON_FUNC_MIN_PARAM_NUM && paramNum <= WRITE_JSON_FUNC_MAX_PARAM_NUM) {
        if (!CheckWriteJsonParamsType(env, params, sizeof(params) / sizeof(params[0]))) {
            HiLog::Error(LABEL, "event name or event type is invalid parameter type.");
            result = ERROR_INVALID_PARAM_TYPE_JS;
        }

        napi_valuetype lastParamType;
        napi_typeof(env, params[paramNum - 1], &lastParamType);
        if (lastParamType == napi_valuetype::napi_function) {
            napi_create_reference(env, params[paramNum - 1], 1, &asyncContext->callback);
        }

        if (asyncContext->callback == nullptr) {
            napi_create_promise(env, &asyncContext->deferred, &promise);
        }

        if (result == SUCCESS_FLAG) {
            std::shared_ptr<AppEventPack> appEventPack = CreateEventPackFromNapiValue(env, params[EVENT_NAME_INDEX],
                params[EVENT_TYPE_INDEX]);
            int buildRes = BuildAppEventPackFromObject(env, params[JSON_OBJECT_INDEX], appEventPack);
            if (buildRes != SUCCESS_FLAG) {
                HiLog::Error(LABEL, "event build AppEventPack failed.");
                result = buildRes;
            }
            asyncContext->appEventPack = appEventPack;

            int verifyResult = VerifyAppEvent(appEventPack);
            if (verifyResult != SUCCESS_FLAG) {
                HiLog::Error(LABEL, "event verify failed.");
                result = verifyResult;
            }
        }

        asyncContext->result = result;
        AsyncWriteEvent(env, asyncContext);
    } else {
        HiLog::Error(LABEL, "write event failed, invalid number of params.");
    }

    HiLog::Debug(LABEL, "write event end.");
    return promise;
}

static napi_value EventTypeClassConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value argv = nullptr;
    napi_value thisArg = nullptr;
    void* data = nullptr;

    napi_get_cb_info(env, info, &argc, &argv, &thisArg, &data);

    napi_value global = 0;
    napi_get_global(env, &global);

    return thisArg;
}

static void EventTypeClassInit(napi_env env, napi_value exports)
{
    napi_value faultEvent = nullptr;
    napi_create_int32(env, FAULT_EVENT_TYPE, &faultEvent);
    napi_value statisticEvent = nullptr;
    napi_create_int32(env, STATISTIC_EVENT_TYPE, &statisticEvent);
    napi_value securityEvent = nullptr;
    napi_create_int32(env, SECURITY_EVENT_TYPE, &securityEvent);
    napi_value behaviorEvent = nullptr;
    napi_create_int32(env, BEHAVIOR_EVENT_TYPE, &behaviorEvent);

    napi_property_descriptor descriptors[] = {
        DECLARE_NAPI_STATIC_PROPERTY("FAULT", faultEvent),
        DECLARE_NAPI_STATIC_PROPERTY("STATISTIC", statisticEvent),
        DECLARE_NAPI_STATIC_PROPERTY("SECURITY", securityEvent),
        DECLARE_NAPI_STATIC_PROPERTY("BEHAVIOR", behaviorEvent)
    };

    napi_value result = nullptr;
    napi_define_class(env, "EventType", NAPI_AUTO_LENGTH, EventTypeClassConstructor, nullptr,
        sizeof(descriptors) / sizeof(*descriptors), descriptors, &result);

    napi_set_named_property(env, exports, "EventType", result);
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("write", Write),
        DECLARE_NAPI_FUNCTION("writeJson", WriteJson)
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc));
    EventTypeClassInit(env, exports);

    return exports;
}
EXTERN_C_END

static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = NULL,
    .nm_register_func = Init,
    .nm_modname = "hiappevent",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}