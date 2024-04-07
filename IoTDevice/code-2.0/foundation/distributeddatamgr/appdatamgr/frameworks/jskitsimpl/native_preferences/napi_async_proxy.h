/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef PREFERENCES_JSKIT_NAPI_ASYNC_PROXY_H
#define PREFERENCES_JSKIT_NAPI_ASYNC_PROXY_H
#include <vector>
#include "securec.h"

#include "napi/native_common.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "common.h"
namespace OHOS {
namespace PreferencesJsKit {
constexpr int MAX_INPUT_COUNT = 10;
constexpr int OK = 0;
constexpr int ERR = -1;

// T inherits AysncContext
template<class T>
class NapiAsyncProxy {
public:
    using InputParser = void (*)(const napi_env&, const napi_value&, T*);
    using NapiAsyncExecute = int (*)(T*);
    using NapiAsyncComplete = int (*)(T*, napi_value&);

    // AsyncContext base
    struct AysncContext {
        napi_env env;
        napi_callback_info info;
        napi_async_work work;
        napi_deferred deferred;
        napi_ref callbackRef;
        NapiAsyncExecute execFunc;
        int execStatus;
        NapiAsyncComplete completeFunc;
        void* boundObj;
    };

public:
    void Init(napi_env env, napi_callback_info info)
    {
        asyncContext = new T();
        if (asyncContext == nullptr) {
            LOG_ERROR("new AsyncContext failed");
            return;
        }
        memset_s(asyncContext, sizeof(T), 0, sizeof(T));
        asyncContext->env = env;
        asyncContext->info = info;
    }

    void ParseInputs(std::vector<InputParser>& parsers)
    {
        if (asyncContext == nullptr) {
            LOG_ERROR("uninitialized AsyncContext");
            return;
        }
        LOG_DEBUG("ParseInputs parsers num: %{public}zu", parsers.size());

        napi_value thisObj = nullptr;
        size_t argc = parsers.size() + 1;
        napi_value args[MAX_INPUT_COUNT] = { 0 };
        napi_get_cb_info(asyncContext->env, asyncContext->info, &argc, args, &thisObj, nullptr);
        LOG_DEBUG("ParseInputs real num:%{public}zu", argc);
        for (size_t i = 0; i < argc && argc <= MAX_INPUT_COUNT; i++) {
            if (i >= parsers.size()) {
                // determines whether the last parameter is a callback function.
                napi_valuetype valueType = napi_undefined;
                napi_typeof(asyncContext->env, args[i], &valueType);
                if (valueType == napi_function) {
                    napi_create_reference(asyncContext->env, args[i], 1, &asyncContext->callbackRef);
                }
                break;
            }
            auto* parserFunction = parsers[i];
            if (parserFunction != nullptr) {
                parserFunction(asyncContext->env, args[i], this->asyncContext);
            }
        }

        napi_unwrap(asyncContext->env, thisObj, &asyncContext->boundObj);
    }

    napi_value DoAsyncWork(std::string resourceName, NapiAsyncExecute execFunc, NapiAsyncComplete completeFunc)
    {
        if (asyncContext == nullptr) {
            LOG_ERROR("uninitialized AsyncContext");
            return nullptr;
        }
        LOG_DEBUG("DoAsyncWork resource name:%{public}s", resourceName.c_str());

        napi_value ret = nullptr;
        if (asyncContext->callbackRef == nullptr) {
            napi_create_promise(asyncContext->env, &asyncContext->deferred, &ret);
        } else {
            napi_get_undefined(asyncContext->env, &ret);
        }

        napi_value resource = nullptr;
        napi_create_string_utf8(asyncContext->env, resourceName.c_str(), NAPI_AUTO_LENGTH, &resource);

        asyncContext->execFunc = execFunc;
        asyncContext->completeFunc = completeFunc;
        napi_create_async_work(
            asyncContext->env,
            nullptr,
            resource,
            [](napi_env env, void* data) {
                LOG_DEBUG("execute callback begin");
                T* asyncContext = (T*)data;
                asyncContext->execStatus = asyncContext->execFunc(asyncContext);
                LOG_DEBUG("execute callback end, ret:%{public}d", asyncContext->execStatus);
            },
            [](napi_env env, napi_status status, void* data) {
                T* asyncContext = (T*)data;
                napi_value output = nullptr;
                LOG_DEBUG("complete callback begin");
                int completeStatus = asyncContext->completeFunc(asyncContext, output);
                LOG_DEBUG("complete callback end, ret:%{public}d", completeStatus);

                napi_value result[RESULT_COUNT] = { 0 };
                if (asyncContext->execStatus == OK && completeStatus == OK) {
                    napi_get_undefined(env, &result[0]);
                    result[1] = output;
                } else {
                    napi_value message = nullptr;
                    napi_create_string_utf8(env, "async call failed", NAPI_AUTO_LENGTH, &message);
                    napi_create_error(env, nullptr, message, &result[0]);
                    napi_get_undefined(env, &result[1]);
                }
                if (asyncContext->deferred) {
                    // promise
                    if (asyncContext->execStatus == OK && completeStatus == OK) {
                        napi_resolve_deferred(env, asyncContext->deferred, result[1]);
                    } else {
                        napi_reject_deferred(env, asyncContext->deferred, result[0]);
                    }
                } else {
                    // callback
                    napi_value callback = nullptr;
                    napi_get_reference_value(env, asyncContext->callbackRef, &callback);
                    napi_value callbackResult = nullptr;
                    napi_status status =
                        napi_call_function(env, nullptr, callback, RESULT_COUNT, result, &callbackResult);
                    napi_delete_reference(env, asyncContext->callbackRef);
                    LOG_DEBUG("callback function execution status: %{public}d", status);
                }
                napi_delete_async_work(env, asyncContext->work);
                delete asyncContext;
            },
            (void*)asyncContext,
            &asyncContext->work);
        napi_queue_async_work(asyncContext->env, asyncContext->work);
        return ret;
    }

private:
    T* asyncContext;
    constexpr static int RESULT_COUNT = 2;
};
}  // namespace PreferencesJsKit
}  // namespace OHOS
#endif