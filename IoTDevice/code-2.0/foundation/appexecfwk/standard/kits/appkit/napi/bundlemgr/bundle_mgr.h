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

#ifndef BUNDLE_MGR_H_
#define BUNDLE_MGR_H_
#include <vector>

#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "hilog_wrapper.h"
#include "ohos/aafwk/content/want.h"
#include "bundle_mgr_interface.h"
#include "application_info.h"

struct AsyncAbilityInfoCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    OHOS::AAFwk::Want want;
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
};

struct AsyncBundleInfoCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::string param;
    OHOS::AppExecFwk::BundleInfo bundleInfo;
};

struct AsyncApplicationInfoCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::string bundleName;
    OHOS::AppExecFwk::ApplicationInfo appInfo;
};

struct AsyncPermissionDefCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::string permissionName;
    OHOS::AppExecFwk::PermissionDef permissionDef;
};

struct AsyncBundleInfosCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::vector<OHOS::AppExecFwk::BundleInfo> bundleInfos;
};

struct AsyncApplicationInfosCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::vector<OHOS::AppExecFwk::ApplicationInfo> appInfos;
};

struct AsyncInstallCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::string param;
    std::string resultMsg;
};

napi_value GetApplicationInfos(napi_env env, napi_callback_info);
napi_value GetApplicationInfo(napi_env env, napi_callback_info info);
napi_value QueryAbilityInfo(napi_env env, napi_callback_info info);
napi_value GetBundleInfos(napi_env env, napi_callback_info info);
napi_value GetBundleInfo(napi_env env, napi_callback_info info);
napi_value GetBundleArchiveInfo(napi_env env, napi_callback_info info);
napi_value GetPermissionDef(napi_env env, napi_callback_info info);
napi_value Install(napi_env env, napi_callback_info info);
napi_value Uninstall(napi_env env, napi_callback_info info);

#endif /* BUNDLE_MGR_H_ */
