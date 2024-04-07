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
#include <cstring>
#include <vector>
#include "feature_ability.h"
#include "securec.h"
#include "ability_process.h"
#include "want_wrapper.h"
#include "hilog_wrapper.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AppExecFwk {
CallbackInfo g_aceCallbackInfo;

/**
 * @brief FeatureAbility NAPI module registration.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param exports An empty object via the exports parameter as a convenience.
 *
 * @return The return value from Init is treated as the exports object for the module.
 */
napi_value FeatureAbilityInit(napi_env env, napi_value exports)
{
    HILOG_INFO("%{public}s,called", __func__);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("startAbility", NAPI_StartAbility),
        DECLARE_NAPI_FUNCTION("startAbilityForResult", NAPI_StartAbility),
        DECLARE_NAPI_FUNCTION("setResult", NAPI_SetResult),
        DECLARE_NAPI_FUNCTION("terminateAbility", NAPI_TerminateAbility),
        DECLARE_NAPI_FUNCTION("onAbilityResult", NAPI_OnAbilityResult),
        DECLARE_NAPI_FUNCTION("hasWindowFocus", NAPI_HasWindowFocus),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));

    return exports;
}

/**
 * @brief FeatureAbility NAPI method : startAbility.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_StartAbility(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s,called", __func__);
    AsyncCallbackInfo *asyncCallbackInfo = CreateAsyncCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }

    napi_value ret = StartAbilityWrap(env, info, asyncCallbackInfo);
    if (ret == nullptr) {
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        }
    }
    return ret;
}

/**
 * @brief StartAbility processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value StartAbilityWrap(napi_env env, napi_callback_info info, AsyncCallbackInfo *asyncCallbackInfo)
{

    HILOG_INFO("%{public}s,called", __func__);
    size_t argc = 3;
    const size_t argCount = 1;
    const size_t argCountWithAsync = argCount + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    if (argc > argCountWithAsync || argc > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    CallAbilityParam param;
    if (UnwrapParam(param, env, args[0]) == nullptr) {
        HILOG_ERROR("%{public}s, call UnwrapParam failed.", __func__);
        return nullptr;
    }
    asyncCallbackInfo->param = param;
    asyncCallbackInfo->aceCallback = &g_aceCallbackInfo;

    if (argc > argCount) {
        ret = StartAbilityAsync(env, args, argc, argCount, asyncCallbackInfo);
    } else {
        ret = StartAbilityPromise(env, asyncCallbackInfo);
    }

    return ret;
}

napi_value StartAbilityAsync(
    napi_env env, napi_value *args, size_t argc, const size_t argCount, AsyncCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("%{public}s, asyncCallback.", __func__);
    if (args == nullptr || asyncCallbackInfo == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName);

    for (size_t i = argCount, index = 0; i < argc && index < ARGS_ASYNC_COUNT; i++, index++) {
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, args[i], &valuetype);
        if (valuetype == napi_function) {
            napi_create_reference(env, args[i], 1, &asyncCallbackInfo->cbInfo.callback[index]);
        }
    }

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            HILOG_INFO("NAPI_StartAbility, worker pool thread execute.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            if (asyncCallbackInfo != nullptr) {
                AbilityProcess::GetInstance()->StartAbility(
                    asyncCallbackInfo->ability, asyncCallbackInfo->param, *asyncCallbackInfo->aceCallback);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            HILOG_INFO("NAPI_StartAbility, main event thread complete.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value result = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);
            napi_get_null(env, &result);
            napi_get_reference_value(env, asyncCallbackInfo->cbInfo.callback[0], &callback);
            napi_call_function(env, undefined, callback, 1, &result, &callResult);

            if (asyncCallbackInfo->cbInfo.callback[0] != nullptr) {
                napi_delete_reference(env, asyncCallbackInfo->cbInfo.callback[0]);
            }
            if (asyncCallbackInfo->cbInfo.callback[1] != nullptr) {
                napi_delete_reference(env, asyncCallbackInfo->cbInfo.callback[1]);
            }
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
    napi_value result = 0;
    napi_get_null(env, &result);
    return result;
}

napi_value StartAbilityPromise(napi_env env, AsyncCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("%{public}s, promise.", __func__);
    if (asyncCallbackInfo == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName;
    napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName);
    napi_deferred deferred;
    napi_value promise = 0;
    napi_create_promise(env, &deferred, &promise);
    asyncCallbackInfo->deferred = deferred;

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            HILOG_INFO("NAPI_StartAbility, worker pool thread execute.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            if (asyncCallbackInfo != nullptr) {
                AbilityProcess::GetInstance()->StartAbility(
                    asyncCallbackInfo->ability, asyncCallbackInfo->param, *asyncCallbackInfo->aceCallback);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            HILOG_INFO("NAPI_StartAbility,  main event thread complete.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            napi_value result = 0;
            napi_get_null(asyncCallbackInfo->cbInfo.env, &result);
            napi_resolve_deferred(asyncCallbackInfo->cbInfo.env, asyncCallbackInfo->deferred, result);
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
    return promise;
}

/**
 * @brief FeatureAbility NAPI method : setResult.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_SetResult(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s,called", __func__);
    AsyncCallbackInfo *asyncCallbackInfo = CreateAsyncCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }

    napi_value ret = SetResultWrap(env, info, asyncCallbackInfo);
    if (ret == nullptr) {
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        }
    }
    return ret;
}

/**
 * @brief SetResult processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value SetResultWrap(napi_env env, napi_callback_info info, AsyncCallbackInfo *asyncCallbackInfo)
{

    HILOG_INFO("%{public}s,called", __func__);
    size_t argc = 4;
    const size_t argCount = 2;
    const size_t argCountWithAsync = argCount + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    if (argc > argCountWithAsync || argc > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    CallAbilityParam param;
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &param.requestCode));
    HILOG_INFO("FeatureAbility::NAPI_SetResult requestCode=%{public}d.", param.requestCode);
    if (UnwrapWant(param.want, env, args[1]) == nullptr) {
        HILOG_ERROR("%{public}s, call unwrapWant failed.", __func__);
        return nullptr;
    }
    asyncCallbackInfo->param = param;

    if (argc > argCount) {
        ret = SetResultAsync(env, args, argc, argCount, asyncCallbackInfo);
    } else {
        ret = SetResultPromise(env, asyncCallbackInfo);
    }

    return ret;
}

napi_value SetResultAsync(
    napi_env env, napi_value *args, size_t argc, const size_t argCount, AsyncCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("%{public}s, asyncCallback.", __func__);
    if (args == nullptr || asyncCallbackInfo == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName);

    for (size_t i = argCount, index = 0; i < argc && index < ARGS_ASYNC_COUNT; i++, index++) {
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, args[i], &valuetype);
        if (valuetype == napi_function) {
            napi_create_reference(env, args[i], 1, &asyncCallbackInfo->cbInfo.callback[index]);
        }
    }

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            HILOG_INFO("NAPI_SetResult, worker pool thread execute.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            if (asyncCallbackInfo->ability != nullptr) {
                asyncCallbackInfo->ability->SetResult(
                    asyncCallbackInfo->param.requestCode, asyncCallbackInfo->param.want);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            HILOG_INFO("NAPI_SetResult, main event thread complete.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            napi_value result = 0;
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);
            napi_get_null(env, &result);
            napi_get_reference_value(env, asyncCallbackInfo->cbInfo.callback[0], &callback);
            napi_call_function(env, undefined, callback, 1, &result, &callResult);

            if (asyncCallbackInfo->cbInfo.callback[0] != nullptr) {
                napi_delete_reference(env, asyncCallbackInfo->cbInfo.callback[0]);
            }
            if (asyncCallbackInfo->cbInfo.callback[1] != nullptr) {
                napi_delete_reference(env, asyncCallbackInfo->cbInfo.callback[1]);
            }
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
    napi_value result = 0;
    napi_get_null(env, &result);
    return result;
}

napi_value SetResultPromise(napi_env env, AsyncCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("%{public}s, promise.", __func__);
    if (asyncCallbackInfo == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName);
    napi_deferred deferred;
    napi_value promise = 0;
    napi_create_promise(env, &deferred, &promise);
    asyncCallbackInfo->deferred = deferred;

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            HILOG_INFO("NAPI_SetResult, worker pool thread execute.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            if (asyncCallbackInfo->ability != nullptr) {
                asyncCallbackInfo->ability->SetResult(
                    asyncCallbackInfo->param.requestCode, asyncCallbackInfo->param.want);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            HILOG_INFO("NAPI_SetResult,  main event thread complete.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            napi_value result = 0;
            napi_get_null(env, &result);
            napi_resolve_deferred(env, asyncCallbackInfo->deferred, result);
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
    return promise;
}

/**
 * @brief FeatureAbility NAPI method : terminateAbility.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_TerminateAbility(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s,called", __func__);
    AsyncCallbackInfo *asyncCallbackInfo = CreateAsyncCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }

    napi_value ret = TerminateAbilityWrap(env, info, asyncCallbackInfo);
    if (ret == nullptr) {
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        }
    }
    return ret;
}

/**
 * @brief TerminateAbility processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value TerminateAbilityWrap(napi_env env, napi_callback_info info, AsyncCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("%{public}s, asyncCallback.", __func__);
    if (asyncCallbackInfo == nullptr) {
        HILOG_ERROR("%{public}s, asyncCallbackInfo == nullptr.", __func__);
        return nullptr;
    }

    size_t argc = 2;
    const size_t argCount = 0;
    const size_t argCountWithAsync = argCount + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    if (argc > argCountWithAsync || argc > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    if (argc > argCount) {
        ret = TerminateAbilityAsync(env, args, argc, argCount, asyncCallbackInfo);
    } else {
        ret = TerminateAbilityPromise(env, asyncCallbackInfo);
    }

    return ret;
}

napi_value TerminateAbilityAsync(
    napi_env env, napi_value *args, size_t argc, const size_t argCount, AsyncCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("%{public}s, asyncCallback.", __func__);
    if (args == nullptr || asyncCallbackInfo == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName);

    for (size_t i = argCount, index = 0; i < argc && index < ARGS_ASYNC_COUNT; i++, index++) {
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, args[i], &valuetype);
        if (valuetype == napi_function) {
            napi_create_reference(env, args[i], 1, &asyncCallbackInfo->cbInfo.callback[index]);
        }
    }

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            HILOG_INFO("NAPI_TerminateAbility, worker pool thread execute.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            if (asyncCallbackInfo->ability != nullptr) {
                asyncCallbackInfo->ability->TerminateAbility();
            }
        },
        [](napi_env env, napi_status status, void *data) {
            HILOG_INFO("NAPI_TerminateAbility, main event thread complete.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value result = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);
            napi_get_null(env, &result);
            napi_get_reference_value(env, asyncCallbackInfo->cbInfo.callback[0], &callback);
            napi_call_function(env, undefined, callback, 1, &result, &callResult);

            if (asyncCallbackInfo->cbInfo.callback[0] != nullptr) {
                napi_delete_reference(env, asyncCallbackInfo->cbInfo.callback[0]);
            }
            if (asyncCallbackInfo->cbInfo.callback[1] != nullptr) {
                napi_delete_reference(env, asyncCallbackInfo->cbInfo.callback[1]);
            }
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
    napi_value result = 0;
    napi_get_null(env, &result);
    return result;
}

napi_value TerminateAbilityPromise(napi_env env, AsyncCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("%{public}s, promise.", __func__);
    if (asyncCallbackInfo == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName);
    napi_deferred deferred;
    napi_value promise = 0;
    napi_create_promise(env, &deferred, &promise);

    asyncCallbackInfo->deferred = deferred;

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            HILOG_INFO("NAPI_TerminateAbility, worker pool thread execute.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            if (asyncCallbackInfo->ability != nullptr) {
                asyncCallbackInfo->ability->TerminateAbility();
            }
        },
        [](napi_env env, napi_status status, void *data) {
            HILOG_INFO("NAPI_TerminateAbility,  main event thread complete.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            napi_value result = 0;
            napi_get_null(env, &result);
            napi_resolve_deferred(env, asyncCallbackInfo->deferred, result);
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
    return promise;
}

/**
 * @brief Checks whether the main window of this ability has window focus.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_HasWindowFocus(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s,called", __func__);
    AsyncCallbackInfo *asyncCallbackInfo = CreateAsyncCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }

    napi_value ret = HasWindowFocusWrap(env, info, asyncCallbackInfo);
    if (ret == nullptr) {
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        }
    }
    return ret;
}

/**
 * @brief HasWindowFocus processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value HasWindowFocusWrap(napi_env env, napi_callback_info info, AsyncCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("%{public}s, asyncCallback.", __func__);
    if (asyncCallbackInfo == nullptr) {
        HILOG_ERROR("%{public}s, asyncCallbackInfo == nullptr.", __func__);
        return nullptr;
    }

    size_t argc = 2;
    const size_t argCount = 0;
    const size_t argCountWithAsync = argCount + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    if (argc > argCountWithAsync || argc > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    if (argc > argCount) {
        ret = HasWindowFocusAsync(env, args, argc, argCount, asyncCallbackInfo);
    } else {
        ret = HasWindowFocusPromise(env, asyncCallbackInfo);
    }

    return ret;
}

napi_value HasWindowFocusAsync(
    napi_env env, napi_value *args, size_t argc, const size_t argCount, AsyncCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("%{public}s, asyncCallback.", __func__);
    if (args == nullptr || asyncCallbackInfo == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName);
    for (size_t i = argCount, index = 0; i < argc && index < ARGS_ASYNC_COUNT; i++, index++) {
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, args[i], &valuetype);
        if (valuetype == napi_function) {
            napi_create_reference(env, args[i], 1, &asyncCallbackInfo->cbInfo.callback[index]);
        }
    }
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            HILOG_INFO("NAPI_HasWindowFocus, worker pool thread execute.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            if (asyncCallbackInfo->ability != nullptr) {
                asyncCallbackInfo->native_result = asyncCallbackInfo->ability->HasWindowFocus();
            }
        },
        [](napi_env env, napi_status status, void *data) {
            HILOG_INFO("NAPI_HasWindowFocus, main event thread complete.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value result = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);
            napi_get_boolean(env, asyncCallbackInfo->native_result, &result);
            napi_get_reference_value(env, asyncCallbackInfo->cbInfo.callback[0], &callback);
            napi_call_function(env, undefined, callback, 1, &result, &callResult);

            if (asyncCallbackInfo->cbInfo.callback[0] != nullptr) {
                napi_delete_reference(env, asyncCallbackInfo->cbInfo.callback[0]);
            }
            if (asyncCallbackInfo->cbInfo.callback[1] != nullptr) {
                napi_delete_reference(env, asyncCallbackInfo->cbInfo.callback[1]);
            }
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
    napi_value result = 0;
    napi_get_null(env, &result);
    return result;
}

napi_value HasWindowFocusPromise(napi_env env, AsyncCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("%{public}s, promise.", __func__);
    if (asyncCallbackInfo == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName);
    napi_deferred deferred;
    napi_value promise = 0;
    napi_create_promise(env, &deferred, &promise);
    asyncCallbackInfo->deferred = deferred;

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            HILOG_INFO("NAPI_HasWindowFocus, worker pool thread execute.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            if (asyncCallbackInfo->ability != nullptr) {
                asyncCallbackInfo->native_result = asyncCallbackInfo->ability->HasWindowFocus();
            }
        },
        [](napi_env env, napi_status status, void *data) {
            HILOG_INFO("NAPI_HasWindowFocus, main event thread complete.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            napi_value result = 0;
            napi_get_boolean(env, asyncCallbackInfo->native_result, &result);
            napi_resolve_deferred(env, asyncCallbackInfo->deferred, result);
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);

    return promise;
}

/**
 * @brief FeatureAbility NAPI method : onAbilityResult.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_OnAbilityResult(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s,called", __func__);
    AsyncCallbackInfo *asyncCallbackInfo = CreateAsyncCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }

    napi_value ret = OnAbilityResultWrap(env, info, asyncCallbackInfo);
    if (ret == nullptr) {
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        }
    }
    return ret;
}

/**
 * @brief OnAbilityResult processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value OnAbilityResultWrap(napi_env env, napi_callback_info info, AsyncCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("%{public}s,called", __func__);
    size_t argc = 2;
    const size_t argCount = 0;
    const size_t argCountWithAsync = argCount + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    if (argc > argCountWithAsync || argc > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    if (argc > argCount) {
        ret = OnAbilityResultAsync(env, args, argc, argCount, asyncCallbackInfo);
    }

    return ret;
}

napi_value OnAbilityResultAsync(
    napi_env env, napi_value *args, size_t argc, const size_t argCount, AsyncCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("%{public}s, asyncCallback.", __func__);
    if (args == nullptr || asyncCallbackInfo == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    HILOG_INFO("%{public}s, asyncCallback.", __func__);
    napi_value resourceName = 0;
    napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName);

    g_aceCallbackInfo.env = env;
    for (size_t i = argCount, index = 0; i < argc && index < ARGS_ASYNC_COUNT; i++, index++) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, args[i], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        NAPI_CALL(env, napi_create_reference(env, args[i], 1, &g_aceCallbackInfo.callback[index]));
    }

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) { HILOG_INFO("NAPI_OnAbilityResult, worker pool thread execute."); },
        [](napi_env env, napi_status status, void *data) {
            HILOG_INFO("NAPI_OnAbilityResult, main event thread complete.");
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

    napi_value result;
    napi_get_null(env, &result);
    return result;
}

/**
 * @brief The interface of onAbilityResult provided for ACE to call back to JS.
 *
 * @param requestCode Indicates the request code returned after the ability is started.
 * @param resultCode Indicates the result code returned after the ability is started.
 * @param resultData Indicates the data returned after the ability is started.
 * @param cb The environment and call back info that the Node-API call is invoked under.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value CallOnAbilityResult(int requestCode, int resultCode, const Want &resultData, CallbackInfo cb)
{
    HILOG_INFO("%{public}s,called", __func__);
    napi_value result = 0;
    NAPI_CALL(cb.env, napi_create_object(cb.env, &result));
    // create requestCode
    napi_value jsValue = 0;
    NAPI_CALL(cb.env, napi_create_int32(cb.env, requestCode, &jsValue));
    NAPI_CALL(cb.env, napi_set_named_property(cb.env, result, "requestCode", jsValue));
    // create resultCode
    NAPI_CALL(cb.env, napi_create_int32(cb.env, resultCode, &jsValue));
    NAPI_CALL(cb.env, napi_set_named_property(cb.env, result, "resultCode", jsValue));
    // create want
    napi_value jsWant = 0;
    NAPI_CALL(cb.env, napi_create_object(cb.env, &jsWant));
    // create want:action
    napi_value action = 0;
    NAPI_CALL(cb.env, napi_create_string_utf8(cb.env, resultData.GetAction().c_str(), NAPI_AUTO_LENGTH, &action));
    NAPI_CALL(cb.env, napi_set_named_property(cb.env, jsWant, "action", action));
    // create want:entities
    napi_value entity = 0;
    napi_value entities = 0;
    NAPI_CALL(cb.env, napi_create_array(cb.env, &entities));
    if (resultData.GetEntities().empty()) {
        NAPI_CALL(cb.env, napi_create_string_utf8(cb.env, "", NAPI_AUTO_LENGTH, &entity));
        NAPI_CALL(cb.env, napi_set_element(cb.env, entities, 0, entity));
    } else {
        for (size_t idx = 0; idx < resultData.GetEntities().size(); idx++) {
            NAPI_CALL(cb.env,
                napi_create_string_utf8(cb.env, resultData.GetEntities().at(idx).c_str(), NAPI_AUTO_LENGTH, &entity));
            NAPI_CALL(cb.env, napi_set_element(cb.env, entities, idx, entity));
        }
    }
    NAPI_CALL(cb.env, napi_set_named_property(cb.env, jsWant, "entities", entities));
    // create want:type
    napi_value type = 0;
    NAPI_CALL(cb.env, napi_create_string_utf8(cb.env, resultData.GetType().c_str(), NAPI_AUTO_LENGTH, &type));
    NAPI_CALL(cb.env, napi_set_named_property(cb.env, jsWant, "type", type));
    // create want:flags
    napi_value flags = 0;
    NAPI_CALL(cb.env, napi_create_uint32(cb.env, resultData.GetFlags(), &flags));
    NAPI_CALL(cb.env, napi_set_named_property(cb.env, jsWant, "flags", flags));
    // create want:elementName
    napi_value elementName = 0;
    NAPI_CALL(cb.env, napi_create_object(cb.env, &elementName));
    // create want:elementName:deviceId
    napi_value deviceId = 0;
    NAPI_CALL(cb.env,
        napi_create_string_utf8(cb.env, resultData.GetElement().GetDeviceID().c_str(), NAPI_AUTO_LENGTH, &deviceId));
    NAPI_CALL(cb.env, napi_set_named_property(cb.env, elementName, "deviceId", deviceId));
    // create want:elementName:bundleName
    napi_value bundleName = 0;
    NAPI_CALL(cb.env,
        napi_create_string_utf8(
            cb.env, resultData.GetElement().GetBundleName().c_str(), NAPI_AUTO_LENGTH, &bundleName));
    NAPI_CALL(cb.env, napi_set_named_property(cb.env, elementName, "bundleName", bundleName));
    // create want:elementName:abilityName
    napi_value abilityName = 0;
    NAPI_CALL(cb.env,
        napi_create_string_utf8(
            cb.env, resultData.GetElement().GetAbilityName().c_str(), NAPI_AUTO_LENGTH, &abilityName));
    NAPI_CALL(cb.env, napi_set_named_property(cb.env, elementName, "abilityName", abilityName));
    NAPI_CALL(cb.env, napi_set_named_property(cb.env, jsWant, "elementName", elementName));
    NAPI_CALL(cb.env, napi_set_named_property(cb.env, result, "want", jsWant));
    // call CB function
    napi_value callback = 0;
    napi_value undefined = 0;
    NAPI_CALL(cb.env, napi_get_undefined(cb.env, &undefined));
    napi_value callResult = 0;
    NAPI_CALL(cb.env, napi_get_reference_value(cb.env, cb.callback[0], &callback));
    NAPI_CALL(cb.env, napi_call_function(cb.env, undefined, callback, 1, &result, &callResult));
    if (cb.callback[0] != nullptr) {
        NAPI_CALL(cb.env, napi_delete_reference(cb.env, cb.callback[0]));
    }
    if (cb.callback[1] != nullptr) {
        NAPI_CALL(cb.env, napi_delete_reference(cb.env, cb.callback[1]));
    }
    napi_value ret = 0;
    NAPI_CALL(cb.env, napi_create_int32(cb.env, 1, &ret));
    return ret;
}

/**
 * @brief Parse the parameters.
 *
 * @param param Indicates the parameters saved the parse result.
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value UnwrapParam(CallAbilityParam &param, napi_env env, napi_value args)
{
    HILOG_INFO("%{public}s,called", __func__);
    // unwrap the param
    napi_valuetype valueType = napi_undefined;
    // unwrap the param : want object
    UnwrapWant(param.want, env, args);

    // unwrap the param : requestCode (optional)
    napi_value requestCodeProp = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, args, "requestCode", &requestCodeProp));
    NAPI_CALL(env, napi_typeof(env, requestCodeProp, &valueType));
    // there is requestCode in param, set forResultOption = true
    if (valueType == napi_number) {
        NAPI_CALL(env, napi_get_value_int32(env, requestCodeProp, &param.requestCode));
        param.forResultOption = true;
    } else {
        param.forResultOption = false;
    }
    HILOG_INFO("%{public}s, reqCode=%{public}d forResultOption=%{public}d.",
        __func__,
        param.requestCode,
        param.forResultOption);

    // unwrap the param : abilityStartSetting (optional)
    napi_value abilityStartSettingProp = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, args, "abilityStartSetting", &abilityStartSettingProp));
    NAPI_CALL(env, napi_typeof(env, abilityStartSettingProp, &valueType));
    if (valueType == napi_object) {
        param.setting = AbilityStartSetting::GetEmptySetting();
        HILOG_INFO("%{public}s, abilityStartSetting=%{public}p.", __func__, param.setting.get());
    }

    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

/**
 * @brief Create asynchronous data.
 *
 * @param env The environment that the Node-API call is invoked under.
 *
 * @return Return a pointer to AsyncCallbackInfo on success, nullptr on failure
 */
AsyncCallbackInfo *CreateAsyncCallbackInfo(napi_env env)
{
    napi_value global = 0;
    NAPI_CALL(env, napi_get_global(env, &global));

    napi_value abilityObj = 0;
    NAPI_CALL(env, napi_get_named_property(env, global, "ability", &abilityObj));

    Ability *ability = nullptr;
    NAPI_CALL(env, napi_get_value_external(env, abilityObj, (void **)&ability));

    AsyncCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncCallbackInfo{
        .cbInfo.env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
        .ability = ability,
        .native_result = false,
    };
    return asyncCallbackInfo;
}
}  // namespace AppExecFwk
}  // namespace OHOS