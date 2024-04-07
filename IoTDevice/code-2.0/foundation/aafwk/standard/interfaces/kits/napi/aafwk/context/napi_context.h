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

#ifndef NAPI_CONTEXT_PERMISSION_H_
#define NAPI_CONTEXT_PERMISSION_H_
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "ability.h"
#include "feature_ability_common.h"

#define PERMISSION_C_BUFFER_SIZE 1024 /* Converted to C-style string buffer size */
#define PERMISSION_ARGS_SIZE 10
#define PERMISSION_EVENT_SIZE 2
using Ability = OHOS::AppExecFwk::Ability;

namespace OHOS {
namespace AppExecFwk {
struct CallAbilityPermissionParam {
    std::vector<std::string> permission_list;
    int requestCode = 0;
    int syncOption = false;
};

typedef enum {
    NVT_NONE = 0,
    NVT_INT32,
    NVT_BOOL,
    NVT_STRING,
} TNativeValueType;

typedef struct __ThreadReturnData {
    TNativeValueType data_type;
    int32_t int32_value;
    bool bool_value;
    std::string str_value;
} ThreadReturnData;

struct AsyncPermissionCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    Ability *ability;
    CallAbilityPermissionParam param;
    napi_ref callback[2] = {0};
    ThreadReturnData native_data;
    napi_value result;
    bool run_status;
    int error_code;
    OHOS::AppExecFwk::CallbackInfo *aceCallback;
};

struct AsyncParamEx {
    std::string resource;
    size_t argc;
    size_t startIndex;
    napi_async_execute_callback execute;
    napi_async_complete_callback complete;
};

/**
 * @brief Create asynchronous data.
 *
 * @param env The environment that the Node-API call is invoked under.
 *
 * @return Return a pointer to AsyncPermissionCallbackInfo on success, nullptr on failure
 */
AsyncPermissionCallbackInfo *CreateAsyncPermissionCallbackInfo(napi_env env);

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
    napi_env env, size_t argc, size_t startIndex, napi_value *args, AsyncPermissionCallbackInfo *callback);

/**
 * @brief Parse JS numeric parameters.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param value Return integer value.
 * @param args Parameter list.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value UnwrapParamInt(int *value, napi_env env, napi_value args);

/**
 * @brief Parse JS string parameters.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param value Return stirng value.
 * @param args Parameter list.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value UnwrapParamString(std::vector<std::string> &value, napi_env env, napi_value args);

/**
 * @brief Parse JS array of string parameters.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param value Return array of stirng value.
 * @param args Parameter list.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value UnwrapParamArrayString(std::vector<std::string> &value, napi_env env, napi_value args);

/**
 * @brief The callback at the end of the asynchronous callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void AsyncCompleteCallback(napi_env env, napi_status status, void *data);

/**
 * @brief The callback at the end of the Promise callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void PromiseCompleteCallback(napi_env env, napi_status status, void *data);

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
    napi_env env, napi_value *args, AsyncPermissionCallbackInfo *asyncCallbackInfo, const AsyncParamEx *param);

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
    napi_env env, napi_value *args, AsyncPermissionCallbackInfo *asyncCallbackInfo, const AsyncParamEx *param);

/**
 * @brief Convert local data to JS data.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data The local data.
 * @param value the JS data.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
void NAPI_GetNapiValue(napi_env env, const ThreadReturnData *data, napi_value *value);

/**
 * @brief Context NAPI module registration.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param exports An empty object via the exports parameter as a convenience.
 *
 * @return The return value from Init is treated as the exports object for the module.
 */
napi_value ContextPermissionInit(napi_env env, napi_value exports);

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
    const std::vector<int> &grantResults, OHOS::AppExecFwk::CallbackInfo callbackInfo);
}  // namespace AppExecFwk
}  // namespace OHOS
#endif /* ABILITY_PERMISSION_H_ */
