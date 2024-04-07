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

#ifndef ABILITY_MANAGER_H
#define ABILITY_MANAGER_H

#include <vector>

#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "running_process_info.h"
#include "app_process_info.h"
#include "recent_mission_info.h"

using RunningProcessInfo = OHOS::AppExecFwk::RunningProcessInfo;
using AppProcessInfo = OHOS::AppExecFwk::AppProcessInfo;
using RecentMissionInfo = OHOS::AAFwk::RecentMissionInfo;

#define MAX_MISSION_NUM (65535)
#define QUERY_RECENT_MISSION_INFO_TYPE (1)
#define QUERY_RECENT_RUNNING_MISSION_INFO_TYPE (2)

struct AsyncCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    RunningProcessInfo runningProcessInfo;
};

struct AsyncMissionInfosCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    int32_t maxMissionNum = 0;
    int32_t queryType = 0;
    std::vector<RecentMissionInfo> recentMissionInfo;
};

struct AsyncRemoveMissionCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    int32_t index = -1;
    int32_t result = -1;
};

struct AsyncRemoveStackCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    int32_t index = -1;
    int32_t result = -1;
};

struct AsyncMoveMissionToTopCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    int32_t index = -1;
    int32_t result = -1;
};

napi_value NAPI_GetAllRunningProcesses(napi_env env, napi_callback_info info);
napi_value NAPI_QueryRunningAbilityMissionInfos(napi_env env, napi_callback_info info);
napi_value NAPI_QueryRecentAbilityMissionInfos(napi_env env, napi_callback_info info);
napi_value NAPI_RemoveMission(napi_env env, napi_callback_info info);
napi_value NAPI_RemoveStack(napi_env env, napi_callback_info info);
napi_value NAPI_MoveMissionToTop(napi_env env, napi_callback_info info);

#endif  //  ABILITY_MANAGER_H