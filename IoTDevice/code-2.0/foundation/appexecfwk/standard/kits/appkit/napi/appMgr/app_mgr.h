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

#ifndef APP_MGR_H
#define APP_MGR_H

#include "napi/native_common.h"
#include <js_native_api.h>
#include "napi/native_node_api.h"

#include "hilog_wrapper.h"
#include "ability_manager_interface.h"

struct AsyncCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::string bundleName;
    int32_t result;
};

#define BUFFER_LENGTH_MAX (128)

napi_value NAPI_KillProcessesByBundleName(napi_env env, napi_callback_info);

#endif  // APP_MGR_H
