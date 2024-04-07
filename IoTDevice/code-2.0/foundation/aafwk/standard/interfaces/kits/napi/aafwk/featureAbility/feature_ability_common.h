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

#ifndef FEATURE_ABILITY_COMMON_H
#define FEATURE_ABILITY_COMMON_H
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "ability.h"
#include "want.h"

using Want = OHOS::AAFwk::Want;
using Ability = OHOS::AppExecFwk::Ability;
using AbilityStartSetting = OHOS::AppExecFwk::AbilityStartSetting;

namespace OHOS {
namespace AppExecFwk {
class FeatureAbility;

struct CallAbilityParam {
    Want want;
    int requestCode = 0;
    bool forResultOption = false;
    std::shared_ptr<AbilityStartSetting> setting = nullptr;
};

struct CallbackInfo {
    napi_env env;
    napi_ref callback[2] = {0};
};

struct AsyncCallbackInfo {
    CallbackInfo cbInfo;
    napi_async_work asyncWork;
    napi_deferred deferred;
    Ability *ability;
    CallAbilityParam param;
    CallbackInfo *aceCallback;
    bool native_result;
};

static inline std::string NapiValueToStringUtf8(napi_env env, napi_value value)
{
    std::string result = "";
    size_t size = 0;

    if (napi_get_value_string_utf8(env, value, nullptr, 0, &size) != napi_ok) {
        return "";
    }
    result.resize(size + 1);
    if (napi_get_value_string_utf8(env, value, &result[0], result.size(), &size) != napi_ok) {
        return "";
    }
    return result;
}
}  // namespace AppExecFwk
}  // namespace OHOS
#endif /* FEATURE_ABILITY_COMMON_H */
