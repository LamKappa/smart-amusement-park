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

#ifndef FEATURE_ABILITY_H
#define FEATURE_ABILITY_H
#include "feature_ability_common.h"
using Want = OHOS::AAFwk::Want;

#define ARGS_MAX_COUNT 10
#define ARGS_ASYNC_COUNT 2
namespace OHOS {
namespace AppExecFwk {
/**
 * @brief FeatureAbility NAPI module registration.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param exports An empty object via the exports parameter as a convenience.
 *
 * @return The return value from Init is treated as the exports object for the module.
 */
napi_value FeatureAbilityInit(napi_env env, napi_value exports);

/**
 * @brief The interface of onAbilityResult provided for ACE to call back to JS.
 *
 * @param requestCode Indicates the request code returned after the ability is started.
 * @param resultCode Indicates the result code returned after the ability is started.
 * @param resultData Indicates the data returned after the ability is started.
 * @param callbackInfo The environment and call back info that the Node-API call is invoked under.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value CallOnAbilityResult(int requestCode, int resultCode, const Want &resultData, CallbackInfo callbackInfo);

/**
 * @brief FeatureAbility NAPI method : startAbility.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_StartAbility(napi_env env, napi_callback_info info);

/**
 * @brief FeatureAbility NAPI method : setResult.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_SetResult(napi_env env, napi_callback_info info);

/**
 * @brief FeatureAbility NAPI method : terminateAbility.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_TerminateAbility(napi_env env, napi_callback_info info);

/**
 * @brief FeatureAbility NAPI method : onAbilityResult.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_OnAbilityResult(napi_env env, napi_callback_info info);

/**
 * @brief Checks whether the main window of this ability has window focus.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_HasWindowFocus(napi_env env, napi_callback_info info);

/**
 * @brief Parse the parameters.
 *
 * @param param Indicates the parameters saved the parse result.
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value UnwrapParam(CallAbilityParam &param, napi_env env, napi_value args);

/**
 * @brief Create asynchronous data.
 *
 * @param env The environment that the Node-API call is invoked under.
 *
 * @return Return a pointer to AsyncCallbackInfo on success, nullptr on failure
 */
AsyncCallbackInfo *CreateAsyncCallbackInfo(napi_env env);

/**
 * @brief HasWindowFocus processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value HasWindowFocusWrap(napi_env env, napi_callback_info info, AsyncCallbackInfo *asyncCallbackInfo);
napi_value HasWindowFocusAsync(
    napi_env env, napi_value *args, size_t argc, const size_t argCount, AsyncCallbackInfo *asyncCallbackInfo);
napi_value HasWindowFocusPromise(napi_env env, AsyncCallbackInfo *asyncCallbackInfo);

/**
 * @brief TerminateAbility processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value TerminateAbilityWrap(napi_env env, napi_callback_info info, AsyncCallbackInfo *asyncCallbackInfo);
napi_value TerminateAbilityAsync(
    napi_env env, napi_value *args, size_t argc, const size_t argCount, AsyncCallbackInfo *asyncCallbackInfo);
napi_value TerminateAbilityPromise(napi_env env, AsyncCallbackInfo *asyncCallbackInfo);

/**
 * @brief SetResult processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value SetResultWrap(napi_env env, napi_callback_info info, AsyncCallbackInfo *asyncCallbackInfo);
napi_value SetResultAsync(
    napi_env env, napi_value *args, size_t argc, const size_t argCount, AsyncCallbackInfo *asyncCallbackInfo);
napi_value SetResultPromise(napi_env env, AsyncCallbackInfo *asyncCallbackInfo);

/**
 * @brief StartAbility processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value StartAbilityWrap(napi_env env, napi_callback_info info, AsyncCallbackInfo *asyncCallbackInfo);
napi_value StartAbilityAsync(
    napi_env env, napi_value *args, size_t argc, const size_t argCount, AsyncCallbackInfo *asyncCallbackInfo);
napi_value StartAbilityPromise(napi_env env, AsyncCallbackInfo *asyncCallbackInfo);

/**
 * @brief OnAbilityResult processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value OnAbilityResultWrap(napi_env env, napi_callback_info info, AsyncCallbackInfo *asyncCallbackInfo);
napi_value OnAbilityResultAsync(
    napi_env env, napi_value *args, size_t argc, const size_t argCount, AsyncCallbackInfo *asyncCallbackInfo);
}  // namespace AppExecFwk
}  // namespace OHOS
#endif /* FEATURE_ABILITY_H */
