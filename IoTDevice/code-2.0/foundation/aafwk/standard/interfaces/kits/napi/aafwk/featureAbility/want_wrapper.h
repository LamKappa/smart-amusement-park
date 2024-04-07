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
#ifndef WANT_WRAPPER_H
#define WANT_WRAPPER_H
#include "feature_ability_common.h"

using Want = OHOS::AAFwk::Want;
namespace OHOS {
namespace AppExecFwk {
/**
 * @brief Parse the want parameters.
 *
 * @param param Indicates the want parameters saved the parse result.
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value UnwrapWant(Want &param, napi_env env, napi_value args);

/**
 * @brief Parse the elementName parameters.
 *
 * @param param Indicates the elementName parameters saved the parse result.
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value UnwrapElementName(Want &param, napi_env env, napi_value args);

/**
 * @brief Parse the wantParam parameters.
 *
 * @param param Indicates the wantParam parameters saved the parse result.
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value UnwrapWantParam(Want &want, napi_env env, napi_value wantParam);

/**
 * @brief Parse the wantParamArray parameters.
 *
 * @param param Indicates the wantParamArray parameters saved the parse result.
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value UnwrapWantParamArray(Want &want, napi_env env, std::string strProName, napi_value wantParam);
}  // namespace AppExecFwk
}  // namespace OHOS
#endif /* WANT_WRAPPER_H */
