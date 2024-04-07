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
#include "securec.h"
#include "napi_context.h"
#include "hilog_wrapper.h"
#include "ability_process.h"
#include "napi_error.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

#define PERMISSOIN_EVENT_VERIFYSELFPERMISSION 1
#define PERMISSION_EVENT_REQUEST_PERMISSION 2

namespace OHOS {
namespace AppExecFwk {
CallbackInfo aceCallbackInfoPermission;

static void VerifySelfPermissionExecuteCallback(napi_env env, void *data);
static void RequestPermissionsFromUserExecuteCallback(napi_env env, void *data);
static void RequestPermissionsFromUserExecutePromise(napi_env env, void *data);
static napi_value NAPI_RequestPermissionsFromUserWrap(
    napi_env env, napi_callback_info info, AsyncPermissionCallbackInfo *asyncCallbackInfo);
static napi_value NAPI_VerifySelfPermissionWrap(
    napi_env env, napi_callback_info info, AsyncPermissionCallbackInfo *asyncCallbackInfo);
static napi_value NAPI_VerifySelfPermission(napi_env env, napi_callback_info info);
static napi_value NAPI_RequestPermissionsFromUser(napi_env env, napi_callback_info info);
static napi_value NAPI_OnRequestPermissionsFromUserResultWrap(
    napi_env env, napi_callback_info info, AsyncPermissionCallbackInfo *asyncCallbackInfo);
static napi_value NAPI_OnRequestPermissionsFromUserResult(napi_env env, napi_callback_info info);
static void GetBundleNameExecuteCallback(napi_env env, void *data);
static napi_value NAPI_GetBundleNameWrap(
    napi_env env, napi_callback_info info, AsyncPermissionCallbackInfo *asyncCallbackInfo);
static napi_value NAPI_GetBundleName(napi_env env, napi_callback_info info);

/**
 * @brief Context NAPI module registration.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param exports An empty object via the exports parameter as a convenience.
 *
 * @return The return value from Init is treated as the exports object for the module.
 */
napi_value ContextPermissionInit(napi_env env, napi_value exports)
{
    HILOG_INFO("Context::ContextPermissionInit called.");

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("verifySelfPermission", NAPI_VerifySelfPermission),
        DECLARE_NAPI_FUNCTION("requestPermissionsFromUser", NAPI_RequestPermissionsFromUser),
        DECLARE_NAPI_FUNCTION("onRequestPermissionsFromUserResult", NAPI_OnRequestPermissionsFromUserResult),
        DECLARE_NAPI_FUNCTION("getBundleName", NAPI_GetBundleName),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));
    return exports;
}

void ClearNativeData(ThreadReturnData *data)
{
    if (data != nullptr) {
        data->data_type = NVT_NONE;
        data->int32_value = 0;
        data->bool_value = false;
        data->str_value = "";
    }
}

/**
 * @brief Create asynchronous data.
 *
 * @param env The environment that the Node-API call is invoked under.
 *
 * @return Return a pointer to AsyncPermissionCallbackInfo on success, nullptr on failure
 */
AsyncPermissionCallbackInfo *CreateAsyncPermissionCallbackInfo(napi_env env)
{
    napi_value global = 0;
    NAPI_CALL(env, napi_get_global(env, &global));

    napi_value abilityObj = 0;
    NAPI_CALL(env, napi_get_named_property(env, global, "ability", &abilityObj));

    Ability *ability = nullptr;
    NAPI_CALL(env, napi_get_value_external(env, abilityObj, (void **)&ability));

    AsyncPermissionCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncPermissionCallbackInfo{
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
        .ability = ability,
        .aceCallback = nullptr,
    };
    if (asyncCallbackInfo != nullptr) {
        ClearNativeData(&asyncCallbackInfo->native_data);
    }
    return asyncCallbackInfo;
}

/**
 * @brief Create asynchronous data.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param argc Number of parameters.
 * @param startIndex The position of the first event parameter.
 * @param args Parameter list.
 * @param callback Point to asynchronous processing of data.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value CreateAsyncCallback(
    napi_env env, size_t argc, size_t startIndex, napi_value *args, AsyncPermissionCallbackInfo *callback)
{
    if (args == nullptr) {
        return nullptr;
    }

    size_t argCount = (argc - startIndex > PERMISSION_EVENT_SIZE) ? (startIndex + PERMISSION_EVENT_SIZE) : argc;

    for (size_t i = startIndex; i < argCount; i++) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, args[i], &valueType));
        if (valueType != napi_function) {
            return nullptr;
        }
        NAPI_CALL(env, napi_create_reference(env, args[i], 1, &callback->callback[i - startIndex]));
    }

    napi_value result = 0;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

/**
 * @brief Parse JS string parameters.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param value Return stirng value.
 * @param args Parameter list.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value UnwrapParamString(std::vector<std::string> &value, napi_env env, napi_value args)
{
    HILOG_INFO("Context::UnwrapParamString in.");
    char buf[PERMISSION_C_BUFFER_SIZE] = {0};
    size_t len = 0;

    // unwrap the param : permission
    memset_s(buf, sizeof(buf), 0x0, sizeof(buf));
    NAPI_CALL(env, napi_get_value_string_utf8(env, args, buf, PERMISSION_C_BUFFER_SIZE, &len));
    value.push_back(std::string(buf));
    HILOG_INFO("Context::UnwrapParamString permission=%{public}s.", buf);

    // create result code
    napi_value result = 0;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

/**
 * @brief Parse JS numeric parameters.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param value Return integer value.
 * @param args Parameter list.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value UnwrapParamInt(int *value, napi_env env, napi_value args)
{
    if (value == nullptr) {
        HILOG_INFO("Invalid param(value = nullptr)");
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args, &valueType));
    if (valueType != napi_number) {
        return nullptr;
    }

    int32_t value32 = 0;
    NAPI_CALL(env, napi_get_value_int32(env, args, &value32));
    HILOG_INFO("UnwrapParamInt value=%{public}d", value32);
    *value = value32;
    napi_value result = 0;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

/**
 * @brief Parse JS array of string parameters.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param value Return array of stirng value.
 * @param args Parameter list.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value UnwrapParamArrayString(std::vector<std::string> &value, napi_env env, napi_value args)
{
    value.clear();
    char buf[PERMISSION_C_BUFFER_SIZE] = {0};
    size_t len = 0;
    napi_valuetype valueType = napi_undefined;

    bool isArray = false;
    uint32_t arrayLength = 0;
    napi_value valueArray = 0;

    NAPI_CALL(env, napi_is_array(env, args, &isArray));
    if (!isArray) {
        HILOG_INFO("property type mismatch");
        return nullptr;
    }

    NAPI_CALL(env, napi_get_array_length(env, args, &arrayLength));
    HILOG_INFO("property is array, length=%{public}d", arrayLength);

    for (uint32_t i = 0; i < arrayLength; i++) {
        memset_s(buf, sizeof(buf), 0x0, sizeof(buf));

        NAPI_CALL(env, napi_get_element(env, args, i, &valueArray));
        NAPI_CALL(env, napi_typeof(env, valueArray, &valueType));
        if (valueType != napi_string) {
            return nullptr;
        }

        NAPI_CALL(env, napi_get_value_string_utf8(env, valueArray, buf, PERMISSION_C_BUFFER_SIZE, &len));
        HILOG_INFO("UnwrapParamInt array string value=%{public}s", buf);

        value.emplace_back(std::string(buf));
    }

    napi_value result = 0;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

/**
 * @brief Convert local data to JS data.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data The local data.
 * @param value the JS data.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
void NAPI_GetNapiValue(napi_env env, const ThreadReturnData *data, napi_value *value)
{
    if (data == nullptr || value == nullptr) {
        return;
    }
    switch (data->data_type) {
        case NVT_INT32:
            napi_create_int32(env, data->int32_value, value);
            break;
        case NVT_BOOL:
            napi_create_int32(env, data->bool_value ? 1 : 0, value);
            break;
        case NVT_STRING:
            napi_create_string_utf8(env, data->str_value.c_str(), NAPI_AUTO_LENGTH, value);
            break;
        default:
            napi_get_null(env, value);
            break;
    }
}

/**
 * @brief VerifySelfPermission asynchronous processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void VerifySelfPermissionExecuteCallback(napi_env env, void *data)
{
    HILOG_INFO("Context::NAPI_VerifySelfPermission worker pool thread execute.");

    AsyncPermissionCallbackInfo *asyncCallbackInfo = (AsyncPermissionCallbackInfo *)data;
    if (asyncCallbackInfo != nullptr) {
        int rev = -1;
        if (asyncCallbackInfo->ability != nullptr && asyncCallbackInfo->param.permission_list.size() > 0) {
            rev = asyncCallbackInfo->ability->VerifySelfPermission(asyncCallbackInfo->param.permission_list[0]);
        }

        asyncCallbackInfo->native_data.data_type = NVT_INT32;
        asyncCallbackInfo->native_data.int32_value = rev;

        asyncCallbackInfo->run_status = (rev == 0) ? true : false;
        if (asyncCallbackInfo->run_status == false) {
            asyncCallbackInfo->error_code = NAPI_ERR_NO_PERMISSION;
        }
    }
}

/**
 * @brief The callback at the end of the asynchronous callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void AsyncCompleteCallback(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("main event thread complete.");
    AsyncPermissionCallbackInfo *asyncCallbackInfo = (AsyncPermissionCallbackInfo *)data;
    if (asyncCallbackInfo == nullptr) {
        return;
    }

    const size_t cbCount = 2;
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_get_undefined(env, &undefined);
    napi_value result = 0;
    napi_value callResult = 0;
    NAPI_GetNapiValue(env, &asyncCallbackInfo->native_data, &result);
    if (asyncCallbackInfo->run_status) {
        if (asyncCallbackInfo->callback[0] != nullptr) {
            napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
            napi_call_function(env, undefined, callback, 1, &result, &callResult);

            napi_delete_reference(env, asyncCallbackInfo->callback[0]);
        }
    } else {
        if (asyncCallbackInfo->callback[1] != nullptr) {
            napi_value rev[2] = {nullptr};
            napi_create_int32(env, asyncCallbackInfo->error_code, &rev[0]);
            napi_create_int32(env, asyncCallbackInfo->run_status ? 0 : -1, &rev[1]);
            napi_get_reference_value(env, asyncCallbackInfo->callback[1], &callback);
            napi_call_function(env, undefined, callback, cbCount, rev, &callResult);
            napi_delete_reference(env, asyncCallbackInfo->callback[1]);
        }
    }

    napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
    delete asyncCallbackInfo;
}

/**
 * @brief The callback at the end of the Promise callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void PromiseCompleteCallback(napi_env env, napi_status status, void *data)
{
    AsyncPermissionCallbackInfo *asyncCallbackInfo = (AsyncPermissionCallbackInfo *)data;
    if (asyncCallbackInfo != nullptr) {
        napi_value result = 0;
        NAPI_GetNapiValue(env, &asyncCallbackInfo->native_data, &result);
        napi_resolve_deferred(env, asyncCallbackInfo->deferred, result);
        napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
        delete asyncCallbackInfo;
    }
}

/**
 * @brief Asynchronous callback processing.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param args list of JS of param.
 * @param asyncCallbackInfo Process data asynchronously.
 * @param param other param.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value ExecuteAsyncCallback(
    napi_env env, napi_value *args, AsyncPermissionCallbackInfo *asyncCallbackInfo, const AsyncParamEx *param)
{
    if (param == nullptr) {
        return nullptr;
    }

    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, param->resource.c_str(), NAPI_AUTO_LENGTH, &resourceName));
    if (CreateAsyncCallback(env, param->argc, param->startIndex, args, asyncCallbackInfo) == nullptr) {
        return nullptr;
    }

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            param->execute,
            param->complete,
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));

    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

    // create result code
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));

    return result;
}

/**
 * @brief Asynchronous promise processing.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param args list of JS of param.
 * @param asyncCallbackInfo Process data asynchronously.
 * @param param other param.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value ExecutePromiseCallback(
    napi_env env, napi_value *args, AsyncPermissionCallbackInfo *asyncCallbackInfo, const AsyncParamEx *param)
{
    if (args == nullptr) {
        return nullptr;
    }

    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, param->resource.c_str(), NAPI_AUTO_LENGTH, &resourceName));

    napi_deferred deferred = 0;
    napi_value promise = 0;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    asyncCallbackInfo->deferred = deferred;
    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            param->execute,
            param->complete,
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));

    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    return promise;
}

/**
 * @brief VerifySelfPermission processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value NAPI_VerifySelfPermissionWrap(
    napi_env env, napi_callback_info info, AsyncPermissionCallbackInfo *asyncCallbackInfo)
{
    size_t argc = 3;
    const size_t argCount = 1;
    napi_value args[PERMISSION_ARGS_SIZE] = {nullptr};
    napi_value jsthis = 0;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &jsthis, &data));

    CallAbilityPermissionParam param;
    if (UnwrapParamString(param.permission_list, env, args[0]) == nullptr) {
        return nullptr;
    }
    asyncCallbackInfo->param = param;

    AsyncParamEx asyncParamEx;
    asyncParamEx.argc = argc;

    if (argc > argCount) {
        HILOG_INFO("Context::NAPI_VerifySelfPermission asyncCallback.");
        asyncParamEx.resource = "NAPI_VerifySelfPermission1";
        asyncParamEx.startIndex = PERMISSOIN_EVENT_VERIFYSELFPERMISSION;
        asyncParamEx.execute = VerifySelfPermissionExecuteCallback;
        asyncParamEx.complete = AsyncCompleteCallback;
        return ExecuteAsyncCallback(env, args, asyncCallbackInfo, &asyncParamEx);
    } else {
        HILOG_INFO("Context::NAPI_VerifySelfPermission promise.");
        asyncParamEx.resource = "NAPI_VerifySelfPermission2";
        asyncParamEx.startIndex = 0;
        asyncParamEx.execute = VerifySelfPermissionExecuteCallback;
        asyncParamEx.complete = PromiseCompleteCallback;

        return ExecutePromiseCallback(env, args, asyncCallbackInfo, &asyncParamEx);
    }
}

/**
 * @brief Checks whether the current process has the given permission.
 * You need to call requestPermissionsFromUser(java.lang.std::string[],int) to request a permission only
 * if the current process does not have the specific permission.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the current process has the permission;
 * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
 */
napi_value NAPI_VerifySelfPermission(napi_env env, napi_callback_info info)
{
    HILOG_INFO("Context::NAPI_VerifySelfPermission. called.");

    AsyncPermissionCallbackInfo *asyncCallbackInfo = CreateAsyncPermissionCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }

    napi_value rev = NAPI_VerifySelfPermissionWrap(env, info, asyncCallbackInfo);
    if (rev == nullptr) {
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
        napi_value result = 0;
        napi_get_null(env, &result);
        return result;
    }
    return rev;
}

/**
 * @brief RequestPermissionsFromUser asynchronous processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void RequestPermissionsFromUserExecuteCallback(napi_env env, void *data)
{
    HILOG_INFO("Context::NAPI_VerifySelfPermission worker pool thread execute.");
    AsyncPermissionCallbackInfo *asyncCallbackInfo = (AsyncPermissionCallbackInfo *)data;
    if (asyncCallbackInfo != nullptr) {
        AbilityProcess::GetInstance()->RequestPermissionsFromUser(
            asyncCallbackInfo->ability, asyncCallbackInfo->param, *asyncCallbackInfo->aceCallback);
        asyncCallbackInfo->run_status = true;
        asyncCallbackInfo->native_data.data_type = NVT_INT32;
        asyncCallbackInfo->native_data.int32_value = 1;
    }
}

void RequestPermissionsFromUserExecutePromise(napi_env env, void *data)
{
    HILOG_INFO("Context::NAPI_VerifySelfPermission worker pool thread execute.");
    AsyncPermissionCallbackInfo *asyncCallbackInfo = (AsyncPermissionCallbackInfo *)data;
    if (asyncCallbackInfo != nullptr) {
        AbilityProcess::GetInstance()->RequestPermissionsFromUser(
            asyncCallbackInfo->ability, asyncCallbackInfo->param, *asyncCallbackInfo->aceCallback);
        asyncCallbackInfo->run_status = true;
        asyncCallbackInfo->native_data.data_type = NVT_NONE;
    }
}

/**
 * @brief RequestPermissionsFromUser processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value NAPI_RequestPermissionsFromUserWrap(
    napi_env env, napi_callback_info info, AsyncPermissionCallbackInfo *asyncCallbackInfo)
{
    size_t argc = 4;
    const size_t argCount = 2;
    napi_value args[PERMISSION_ARGS_SIZE] = {nullptr};
    napi_value jsthis = 0;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &jsthis, &data));

    CallAbilityPermissionParam param;
    if (UnwrapParamArrayString(param.permission_list, env, args[0]) == nullptr) {
        return nullptr;
    }

    if (UnwrapParamInt(&param.requestCode, env, args[1]) == nullptr) {
        return nullptr;
    }

    asyncCallbackInfo->param = param;
    asyncCallbackInfo->aceCallback = &aceCallbackInfoPermission;
    AsyncParamEx asyncParamEx;
    asyncParamEx.argc = argc;
    if (argc > argCount) {
        HILOG_INFO("Context::NAPI_RequestPermissionsFromUser asyncCallback.");
        asyncParamEx.resource = "NAPI_RequestPermissionsFromUser_Callback";
        asyncParamEx.startIndex = PERMISSION_EVENT_REQUEST_PERMISSION;
        asyncParamEx.execute = RequestPermissionsFromUserExecuteCallback;
        asyncParamEx.complete = AsyncCompleteCallback;
        return ExecuteAsyncCallback(env, args, asyncCallbackInfo, &asyncParamEx);
    } else {
        HILOG_INFO("Context::NAPI_RequestPermissionsFromUser promise.");
        asyncParamEx.resource = "NAPI_RequestPermissionsFromUser_Promise";
        asyncParamEx.startIndex = 0;
        asyncParamEx.execute = RequestPermissionsFromUserExecutePromise;
        asyncParamEx.complete = PromiseCompleteCallback;
        return ExecutePromiseCallback(env, args, asyncCallbackInfo, &asyncParamEx);
    }
}

/**
 * @brief Requests certain permissions from the system.
 * This method is called for permission request. This is an asynchronous method. When it is executed,
 * the Ability.onRequestPermissionsFromUserResult(int, String[], int[]) method will be called back.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 */
napi_value NAPI_RequestPermissionsFromUser(napi_env env, napi_callback_info info)
{
    HILOG_INFO("Context::NAPI_RequestPermissionsFromUser. called.");
    AsyncPermissionCallbackInfo *asyncCallbackInfo = CreateAsyncPermissionCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }

    napi_value rev = NAPI_RequestPermissionsFromUserWrap(env, info, asyncCallbackInfo);
    if (rev == nullptr) {
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
        napi_value result = 0;
        napi_get_null(env, &result);
        return result;
    }
    return rev;
}

/**
 * @brief OnRequestPermissionsFromUserResult processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value NAPI_OnRequestPermissionsFromUserResultWrap(
    napi_env env, napi_callback_info info, AsyncPermissionCallbackInfo *asyncCallbackInfo)
{
    size_t argc = 2;
    napi_value args[PERMISSION_ARGS_SIZE] = {nullptr};
    napi_value jsthis = 0;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &jsthis, &data));

    if (argc > 0) {
        HILOG_INFO("Context::NAPI_OnRequestPermissionsFromUserResult");

        napi_value resourceName = 0;
        NAPI_CALL(env,
            napi_create_string_latin1(env, "NAPI_OnRequestPermissionsFromUserResult", NAPI_AUTO_LENGTH, &resourceName));

        aceCallbackInfoPermission.env = env;
        size_t argcCount = (argc > PERMISSION_EVENT_SIZE) ? PERMISSION_EVENT_SIZE : argc;
        for (size_t i = 0; i < argcCount; i++) {
            napi_valuetype valuetype = napi_undefined;
            NAPI_CALL(env, napi_typeof(env, args[i], &valuetype));
            if (valuetype != napi_function) {
                return nullptr;
            }
            napi_create_reference(env, args[i], 1, &aceCallbackInfoPermission.callback[i]);
        }

        NAPI_CALL(env,
            napi_create_async_work(
                env,
                nullptr,
                resourceName,
                [](napi_env env, void *data) {
                    HILOG_INFO("Context::NAPI_OnRequestPermissionsFromUserResult worker pool thread execute.");
                },
                [](napi_env env, napi_status status, void *data) {
                    HILOG_INFO("Conext::NAPI_OnRequestPermissionsFromUserResult main event thread complete.");

                    AsyncPermissionCallbackInfo *asyncCallbackInfo = (AsyncPermissionCallbackInfo *)data;
                    if (asyncCallbackInfo != nullptr) {
                        napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                        delete asyncCallbackInfo;
                    }
                },
                (void *)asyncCallbackInfo,
                &asyncCallbackInfo->asyncWork));

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    }

    // create result code
    napi_value result = 0;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

/**
 * @brief Called back after permissions are requested by using
 * AbilityContext.requestPermissionsFromUser(java.lang.String[],int).
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 */
napi_value NAPI_OnRequestPermissionsFromUserResult(napi_env env, napi_callback_info info)
{
    HILOG_INFO("Context::NAPI_OnRequestPermissionsFromUserResult. called.");

    AsyncPermissionCallbackInfo *asyncCallbackInfo = CreateAsyncPermissionCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }

    napi_value rev = NAPI_OnRequestPermissionsFromUserResultWrap(env, info, asyncCallbackInfo);
    if (rev == nullptr) {
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
    }
    return rev;
}

/**
 * @brief The interface of onRequestPermissionsFromUserResult provided for ACE to call back to JS.
 *
 * @param requestCode Indicates the request code returned after the ability is started.
 * @param permissions Indicates list of permission.
 * @param grantResults Indicates List of authorization results.
 * @param callbackInfo The environment and call back info that the Node-API call is invoked under.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value CallOnRequestPermissionsFromUserResult(int requestCode, const std::vector<std::string> &permissions,
    const std::vector<int> &grantResults, CallbackInfo callbackInfo)
{
    HILOG_INFO("Context::CallOnRequestPermissionsFromUserResult called");
    napi_value result = 0;
    NAPI_CALL(callbackInfo.env, napi_create_object(callbackInfo.env, &result));

    // create requestCode
    napi_value jsValue = 0;
    NAPI_CALL(callbackInfo.env, napi_create_int32(callbackInfo.env, requestCode, &jsValue));
    NAPI_CALL(callbackInfo.env, napi_set_named_property(callbackInfo.env, result, "requestCode", jsValue));

    // create permissions
    napi_value perValue = 0;
    napi_value perArray = 0;
    NAPI_CALL(callbackInfo.env, napi_create_array(callbackInfo.env, &perArray));

    for (size_t i = 0; i < permissions.size(); i++) {
        NAPI_CALL(callbackInfo.env,
            napi_create_string_utf8(callbackInfo.env, permissions[i].c_str(), NAPI_AUTO_LENGTH, &perValue));
        NAPI_CALL(callbackInfo.env, napi_set_element(callbackInfo.env, perArray, i, perValue));
    }
    NAPI_CALL(callbackInfo.env, napi_set_named_property(callbackInfo.env, result, "permissions", perArray));

    // create grantResults
    napi_value grantArray;
    NAPI_CALL(callbackInfo.env, napi_create_array(callbackInfo.env, &grantArray));

    for (size_t i = 0; i < grantResults.size(); i++) {
        NAPI_CALL(callbackInfo.env, napi_create_int32(callbackInfo.env, grantResults[i], &perValue));
        NAPI_CALL(callbackInfo.env, napi_set_element(callbackInfo.env, grantArray, i, perValue));
    }
    NAPI_CALL(callbackInfo.env, napi_set_named_property(callbackInfo.env, result, "grantResults", grantArray));

    // call CB function
    napi_value callback = 0;
    napi_value undefined = 0;
    NAPI_CALL(callbackInfo.env, napi_get_undefined(callbackInfo.env, &undefined));

    napi_value callResult = 0;
    NAPI_CALL(callbackInfo.env, napi_get_reference_value(callbackInfo.env, callbackInfo.callback[0], &callback));
    NAPI_CALL(callbackInfo.env, napi_call_function(callbackInfo.env, undefined, callback, 1, &result, &callResult));

    if (callbackInfo.callback[0] != nullptr) {
        NAPI_CALL(callbackInfo.env, napi_delete_reference(callbackInfo.env, callbackInfo.callback[0]));
    }
    if (callbackInfo.callback[1] != nullptr) {
        NAPI_CALL(callbackInfo.env, napi_delete_reference(callbackInfo.env, callbackInfo.callback[1]));
    }

    // create reutrn
    napi_value ret = 0;
    NAPI_CALL(callbackInfo.env, napi_create_int32(callbackInfo.env, 1, &ret));
    return ret;
}

/**
 * @brief GetBundleName asynchronous processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void GetBundleNameExecuteCallback(napi_env env, void *data)
{
    HILOG_INFO("FeatureAbility::NAPI_GetBundleName worker pool thread execute.");
    AsyncPermissionCallbackInfo *asyncCallbackInfo = (AsyncPermissionCallbackInfo *)data;

    if (asyncCallbackInfo->ability != nullptr) {
        asyncCallbackInfo->native_data.data_type = NVT_STRING;
        asyncCallbackInfo->native_data.str_value = asyncCallbackInfo->ability->GetBundleName();
        HILOG_INFO("FeatureAbility::NAPI_GetBundleName bundleName = %{public}s.",
            asyncCallbackInfo->native_data.str_value.c_str());
        asyncCallbackInfo->run_status = true;
    }
}

/**
 * @brief GetBundleName processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value NAPI_GetBundleNameWrap(napi_env env, napi_callback_info info, AsyncPermissionCallbackInfo *asyncCallbackInfo)
{
    size_t argc = 2;
    const size_t argCount = 0;
    napi_value args[PERMISSION_ARGS_SIZE] = {nullptr};
    napi_value jsthis = 0;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &jsthis, &data));

    AsyncParamEx asyncParamEx;
    asyncParamEx.argc = argc;
    if (argc > argCount) {
        HILOG_INFO("Context::NAPI_GetBundleName asyncCallback.");
        asyncParamEx.resource = "NAPI_GetBundleName_Callback";
        asyncParamEx.startIndex = 0;
        asyncParamEx.execute = GetBundleNameExecuteCallback;
        asyncParamEx.complete = AsyncCompleteCallback;
        return ExecuteAsyncCallback(env, args, asyncCallbackInfo, &asyncParamEx);
    } else {
        HILOG_INFO("Context::NAPI_GetBundleName promise.");
        asyncParamEx.resource = "NAPI_GetBundleName_Promise";
        asyncParamEx.startIndex = 0;
        asyncParamEx.execute = GetBundleNameExecuteCallback;
        asyncParamEx.complete = PromiseCompleteCallback;
        return ExecutePromiseCallback(env, args, asyncCallbackInfo, &asyncParamEx);
    }
}

/**
 * @brief Get bundle name.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_GetBundleName(napi_env env, napi_callback_info info)
{
    HILOG_INFO("FeatureAbility::NAPI_GetBundleName. called.");

    AsyncPermissionCallbackInfo *asyncCallbackInfo = CreateAsyncPermissionCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }

    napi_value rev = NAPI_GetBundleNameWrap(env, info, asyncCallbackInfo);
    if (rev == nullptr) {
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
        napi_value result = 0;
        napi_get_null(env, &result);
        return result;
    }

    return rev;
}
}  // namespace AppExecFwk
}  // namespace OHOS