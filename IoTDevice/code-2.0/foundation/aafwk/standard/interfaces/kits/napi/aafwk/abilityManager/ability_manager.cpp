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

#include "ability_manager.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "app_mgr_interface.h"
#include "ability_manager_interface.h"

static OHOS::sptr<OHOS::AAFwk::IAbilityManager> GetAbilityManagerInstance()
{
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::sptr<OHOS::IRemoteObject> abilityObject =
        systemAbilityManager->GetSystemAbility(OHOS::ABILITY_MGR_SERVICE_ID);
    return OHOS::iface_cast<OHOS::AAFwk::IAbilityManager>(abilityObject);
}

static OHOS::sptr<OHOS::AppExecFwk::IAppMgr> GetAppManagerInstance()
{
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::sptr<OHOS::IRemoteObject> appObject = systemAbilityManager->GetSystemAbility(OHOS::APP_MGR_SERVICE_ID);
    return OHOS::iface_cast<OHOS::AppExecFwk::IAppMgr>(appObject);
}

static void GetRecentMissionsForResult(
    napi_env env, const std::vector<RecentMissionInfo> recentMissionInfos, napi_value result)
{
    int32_t index = 0;
    std::vector<RecentMissionInfo> entities = recentMissionInfos;
    for (const auto &item : entities) {
        napi_value objRecentMissionInfo;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objRecentMissionInfo));

        napi_value id;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, item.id, &id));
        HILOG_INFO("id = [%{public}d]", item.id);
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objRecentMissionInfo, "id", id));

        napi_value runingState;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, item.runingState, &runingState));
        HILOG_INFO("runingState = [%{public}d]", item.runingState);
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objRecentMissionInfo, "runingState", runingState));

        napi_value want;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &want));

        napi_value uri;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.baseWant.GetUriString().c_str(), NAPI_AUTO_LENGTH, &uri));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, want, "uri", uri));

        napi_value type;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.baseWant.GetType().c_str(), NAPI_AUTO_LENGTH, &type));
        HILOG_INFO("type = [%{public}s]", item.baseWant.GetType().c_str());
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, want, "type", type));

        napi_value flags;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, item.baseWant.GetFlags(), &flags));
        HILOG_INFO("flags = [%{public}d]", item.baseWant.GetFlags());
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, want, "flags", flags));

        napi_value action;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.baseWant.GetAction().c_str(), NAPI_AUTO_LENGTH, &action));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, want, "action", action));

        napi_value nWantEntities;
        NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nWantEntities));
        if (item.baseWant.GetEntities().size() > 0) {
            size_t idx = 0;
            std::vector<std::string> wantEntities = item.baseWant.GetEntities();
            for (const auto &entity : wantEntities) {
                napi_value value;
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, entity.c_str(), NAPI_AUTO_LENGTH, &value));
                NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nWantEntities, idx, value));
                idx++;
            }
        }
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, want, "entities", nWantEntities));

        napi_value elementName;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &elementName));

        napi_value deviceId;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(
                env, item.baseWant.GetElement().GetDeviceID().c_str(), NAPI_AUTO_LENGTH, &deviceId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, elementName, "deviceId", deviceId));

        napi_value bundleName;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(
                env, item.baseWant.GetElement().GetBundleName().c_str(), NAPI_AUTO_LENGTH, &bundleName));
        HILOG_INFO("bundleName = [%{public}s]", item.baseWant.GetElement().GetBundleName().c_str());
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, elementName, "bundleName", bundleName));

        napi_value abilityName;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(
                env, item.baseWant.GetElement().GetAbilityName().c_str(), NAPI_AUTO_LENGTH, &abilityName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, elementName, "abilityName", abilityName));
        HILOG_INFO("abilityName = [%{public}s]", item.baseWant.GetElement().GetAbilityName().c_str());
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, want, "elementName", elementName));

        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objRecentMissionInfo, "baseWant", want));

        napi_value baseAbility;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &baseAbility));

        napi_value deviceId1;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.baseAbility.GetDeviceID().c_str(), NAPI_AUTO_LENGTH, &deviceId1));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, baseAbility, "deviceId", deviceId1));

        napi_value bundleName1;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, item.baseAbility.GetBundleName().c_str(), NAPI_AUTO_LENGTH, &bundleName1));
        HILOG_INFO("bundleName1 = [%{public}s]", item.baseAbility.GetBundleName().c_str());
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, baseAbility, "bundleName", bundleName1));

        napi_value abilityName1;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, item.baseAbility.GetAbilityName().c_str(), NAPI_AUTO_LENGTH, &abilityName1));
        HILOG_INFO("abilityName1 = [%{public}s]", item.baseAbility.GetAbilityName().c_str());
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, baseAbility, "abilityName", abilityName1));

        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objRecentMissionInfo, "baseAbility", baseAbility));

        napi_value topAbility;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &topAbility));

        napi_value deviceId2;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.topAbility.GetDeviceID().c_str(), NAPI_AUTO_LENGTH, &deviceId2));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, topAbility, "deviceId", deviceId2));

        napi_value bundleName2;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.topAbility.GetBundleName().c_str(), NAPI_AUTO_LENGTH, &bundleName2));
        HILOG_INFO("bundleName2 = [%{public}s]", item.topAbility.GetBundleName().c_str());
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, topAbility, "bundleName", bundleName2));

        napi_value abilityName2;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, item.topAbility.GetAbilityName().c_str(), NAPI_AUTO_LENGTH, &abilityName2));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, topAbility, "abilityName", abilityName2));
        HILOG_INFO("abilityName2 = [%{public}s]", item.topAbility.GetAbilityName().c_str());
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objRecentMissionInfo, "topAbility", topAbility));

        napi_value size;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, item.size, &size));
        HILOG_INFO("size = [%{public}d]", item.size);
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objRecentMissionInfo, "size", size));

        napi_value missionDescription;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &missionDescription));

        napi_value label;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.missionDescription.label.c_str(), NAPI_AUTO_LENGTH, &label));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, missionDescription, "label", label));

        napi_value iconPath;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.missionDescription.iconPath.c_str(), NAPI_AUTO_LENGTH, &iconPath));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, missionDescription, "iconPath", iconPath));

        NAPI_CALL_RETURN_VOID(
            env, napi_set_named_property(env, objRecentMissionInfo, "missionDescription", missionDescription));

        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, objRecentMissionInfo));
        index++;
    }
}

static void GetAllRunningProcessesForResult(
    napi_env env, const RunningProcessInfo &runningProcessInfo, napi_value result)
{
    int32_t index = 0;

    for (const auto &item : runningProcessInfo.appProcessInfos) {
        napi_value objAppProcessInfo;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objAppProcessInfo));

        napi_value nPid;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, item.pid_, &nPid));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppProcessInfo, "pid", nPid));

        napi_value nProcessName;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.processName_.c_str(), NAPI_AUTO_LENGTH, &nProcessName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppProcessInfo, "processName", nProcessName));

        napi_value nstate;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(item.state_), &nstate));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppProcessInfo, "state", nstate));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, objAppProcessInfo));
        index++;
    }
}

napi_value NAPI_QueryRecentAbilityMissionInfos(napi_env env, napi_callback_info info)
{
    HILOG_INFO("NAPI_QueryRecentAbilityMissionInfos called...");
    size_t argc = 1;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);

    AsyncMissionInfosCallbackInfo *async_callback_info =
        new AsyncMissionInfosCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};
    async_callback_info->maxMissionNum = MAX_MISSION_NUM;
    async_callback_info->queryType = QUERY_RECENT_MISSION_INFO_TYPE;

    if (argc >= 1) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[0], 1, &async_callback_info->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_QueryRecentAbilityMissionInfosCallBack", NAPI_AUTO_LENGTH, &resourceName);

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("queryRecentAbilityMissionInfos called(CallBack Mode)...");
                AsyncMissionInfosCallbackInfo *async_callback_info = (AsyncMissionInfosCallbackInfo *)data;
                HILOG_INFO("maxMissionNum = [%{public}d]", async_callback_info->maxMissionNum);
                HILOG_INFO("queryType = [%{public}d]", async_callback_info->queryType);

                GetAbilityManagerInstance()->GetRecentMissions(async_callback_info->maxMissionNum,
                    async_callback_info->queryType,
                    async_callback_info->recentMissionInfo);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("queryRecentAbilityMissionInfos compeleted(CallBack Mode)...");
                AsyncMissionInfosCallbackInfo *async_callback_info = (AsyncMissionInfosCallbackInfo *)data;

                napi_value result;
                napi_value callback;
                napi_value undefined;

                napi_create_array(env, &result);
                GetRecentMissionsForResult(env, async_callback_info->recentMissionInfo, result);
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, async_callback_info->callback[0], &callback);
                napi_call_function(env, undefined, callback, 1, &result, nullptr);

                if (async_callback_info->callback[0] != nullptr) {
                    napi_delete_reference(env, async_callback_info->callback[0]);
                }
                napi_delete_async_work(env, async_callback_info->asyncWork);
                delete async_callback_info;
            },
            (void *)async_callback_info,
            &async_callback_info->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, async_callback_info->asyncWork));
        return NULL;
    } else {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_QueryRecentAbilityMissionInfosPromise", NAPI_AUTO_LENGTH, &resourceName);

        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        async_callback_info->deferred = deferred;

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("queryRecentAbilityMissionInfos called(Promise Mode)...");
                AsyncMissionInfosCallbackInfo *async_callback_info = (AsyncMissionInfosCallbackInfo *)data;
                HILOG_INFO("maxMissionNum = [%{public}d]", async_callback_info->maxMissionNum);
                HILOG_INFO("queryType = [%{public}d]", async_callback_info->queryType);

                GetAbilityManagerInstance()->GetRecentMissions(async_callback_info->maxMissionNum,
                    async_callback_info->queryType,
                    async_callback_info->recentMissionInfo);
                HILOG_INFO("size = [%{public}d]", async_callback_info->recentMissionInfo.size());
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("queryRecentAbilityMissionInfos compeleted(Promise Mode)...");
                AsyncMissionInfosCallbackInfo *async_callback_info = (AsyncMissionInfosCallbackInfo *)data;
                napi_value result;
                napi_create_array(env, &result);
                GetRecentMissionsForResult(env, async_callback_info->recentMissionInfo, result);
                napi_resolve_deferred(async_callback_info->env, async_callback_info->deferred, result);
                napi_delete_async_work(env, async_callback_info->asyncWork);
                delete async_callback_info;
            },
            (void *)async_callback_info,
            &async_callback_info->asyncWork);
        napi_queue_async_work(env, async_callback_info->asyncWork);
        return promise;
    }
}

napi_value NAPI_QueryRunningAbilityMissionInfos(napi_env env, napi_callback_info info)
{
    HILOG_INFO("NAPI_QueryRunningAbilityMissionInfos called...");
    size_t argc = 1;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);

    AsyncMissionInfosCallbackInfo *async_callback_info =
        new AsyncMissionInfosCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};
    async_callback_info->maxMissionNum = MAX_MISSION_NUM;
    async_callback_info->queryType = QUERY_RECENT_RUNNING_MISSION_INFO_TYPE;

    if (argc >= 1) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[0], 1, &async_callback_info->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_QueryRunningAbilityMissionInfosCallBack", NAPI_AUTO_LENGTH, &resourceName);

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("queryRunningAbilityMissionInfos called(CallBack Mode)...");
                AsyncMissionInfosCallbackInfo *async_callback_info = (AsyncMissionInfosCallbackInfo *)data;
                HILOG_INFO("maxMissionNum = [%{public}d]", async_callback_info->maxMissionNum);
                HILOG_INFO("queryType = [%{public}d]", async_callback_info->queryType);

                GetAbilityManagerInstance()->GetRecentMissions(async_callback_info->maxMissionNum,
                    async_callback_info->queryType,
                    async_callback_info->recentMissionInfo);
                HILOG_INFO("size = [%{public}d]", async_callback_info->recentMissionInfo.size());
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("queryRunningAbilityMissionInfos compeleted(CallBack Mode)...");
                AsyncMissionInfosCallbackInfo *async_callback_info = (AsyncMissionInfosCallbackInfo *)data;

                napi_value result;
                napi_value callback;
                napi_value undefined;

                napi_create_array(env, &result);
                GetRecentMissionsForResult(env, async_callback_info->recentMissionInfo, result);
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, async_callback_info->callback[0], &callback);
                napi_call_function(env, undefined, callback, 1, &result, nullptr);

                if (async_callback_info->callback[0] != nullptr) {
                    napi_delete_reference(env, async_callback_info->callback[0]);
                }
                napi_delete_async_work(env, async_callback_info->asyncWork);
                delete async_callback_info;
            },
            (void *)async_callback_info,
            &async_callback_info->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, async_callback_info->asyncWork));
        return NULL;
    } else {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_QueryRunningAbilityMissionInfosPromise", NAPI_AUTO_LENGTH, &resourceName);

        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        async_callback_info->deferred = deferred;

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("queryRunningAbilityMissionInfos called(Promise Mode)...");
                AsyncMissionInfosCallbackInfo *async_callback_info = (AsyncMissionInfosCallbackInfo *)data;
                HILOG_INFO("maxMissionNum = [%{public}d]", async_callback_info->maxMissionNum);
                HILOG_INFO("queryType = [%{public}d]", async_callback_info->queryType);

                GetAbilityManagerInstance()->GetRecentMissions(async_callback_info->maxMissionNum,
                    async_callback_info->queryType,
                    async_callback_info->recentMissionInfo);
                HILOG_INFO("size = [%{public}d]", async_callback_info->recentMissionInfo.size());
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("queryRunningAbilityMissionInfos compeleted(Promise Mode)...");
                AsyncMissionInfosCallbackInfo *async_callback_info = (AsyncMissionInfosCallbackInfo *)data;
                napi_value result;
                napi_create_array(env, &result);
                GetRecentMissionsForResult(env, async_callback_info->recentMissionInfo, result);
                napi_resolve_deferred(async_callback_info->env, async_callback_info->deferred, result);
                napi_delete_async_work(env, async_callback_info->asyncWork);
                delete async_callback_info;
            },
            (void *)async_callback_info,
            &async_callback_info->asyncWork);
        napi_queue_async_work(env, async_callback_info->asyncWork);
        return promise;
    }
}

napi_value NAPI_GetAllRunningProcesses(napi_env env, napi_callback_info info)
{
    HILOG_INFO("NAPI_GetAllRunningProcesses called...");
    size_t argc = 1;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);

    AsyncCallbackInfo *async_callback_info =
        new AsyncCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};

    if (argc >= 1) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[0], 1, &async_callback_info->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetAllRunningProcessesCallBack", NAPI_AUTO_LENGTH, &resourceName);

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("getAllRunningProcesses called(CallBack Mode)...");
                AsyncCallbackInfo *async_callback_info = (AsyncCallbackInfo *)data;
                std::shared_ptr<OHOS::AppExecFwk::RunningProcessInfo> runningProcessInfo =
                    std::make_shared<OHOS::AppExecFwk::RunningProcessInfo>();
                GetAppManagerInstance()->GetAllRunningProcesses(runningProcessInfo);
                for (size_t idx = 0; idx < runningProcessInfo->appProcessInfos.size(); idx++) {
                    OHOS::AppExecFwk::AppProcessInfo appProcessInfoTmp;
                    appProcessInfoTmp.pid_ = runningProcessInfo->appProcessInfos[idx].pid_;
                    appProcessInfoTmp.processName_ = runningProcessInfo->appProcessInfos[idx].processName_;
                    appProcessInfoTmp.state_ = runningProcessInfo->appProcessInfos[idx].state_;
                    async_callback_info->runningProcessInfo.appProcessInfos.push_back(appProcessInfoTmp);
                }
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("getAllRunningProcesses compeleted(CallBack Mode)...");
                AsyncCallbackInfo *async_callback_info = (AsyncCallbackInfo *)data;
                napi_value result;
                napi_value callback;
                napi_value undefined;
                napi_create_array(env, &result);
                GetAllRunningProcessesForResult(env, async_callback_info->runningProcessInfo, result);
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, async_callback_info->callback[0], &callback);
                napi_call_function(env, undefined, callback, 1, &result, nullptr);

                if (async_callback_info->callback[0] != nullptr) {
                    napi_delete_reference(env, async_callback_info->callback[0]);
                }
                napi_delete_async_work(env, async_callback_info->asyncWork);
                delete async_callback_info;
            },
            (void *)async_callback_info,
            &async_callback_info->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, async_callback_info->asyncWork));
        return NULL;
    } else {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetAllRunningProcessesPromise", NAPI_AUTO_LENGTH, &resourceName);

        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        async_callback_info->deferred = deferred;

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("getAllRunningProcesses called(Promise Mode)...");
                AsyncCallbackInfo *async_callback_info = (AsyncCallbackInfo *)data;
                std::shared_ptr<OHOS::AppExecFwk::RunningProcessInfo> runningProcessInfo =
                    std::make_shared<OHOS::AppExecFwk::RunningProcessInfo>();
                GetAppManagerInstance()->GetAllRunningProcesses(runningProcessInfo);
                for (size_t idx = 0; idx < runningProcessInfo->appProcessInfos.size(); idx++) {
                    OHOS::AppExecFwk::AppProcessInfo appProcessInfoTmp;
                    appProcessInfoTmp.pid_ = runningProcessInfo->appProcessInfos[idx].pid_;
                    appProcessInfoTmp.processName_ = runningProcessInfo->appProcessInfos[idx].processName_;
                    appProcessInfoTmp.state_ = runningProcessInfo->appProcessInfos[idx].state_;
                    async_callback_info->runningProcessInfo.appProcessInfos.push_back(appProcessInfoTmp);
                }
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("getAllRunningProcesses compeleted(Promise Mode)...");
                AsyncCallbackInfo *async_callback_info = (AsyncCallbackInfo *)data;
                napi_value result;
                napi_create_array(env, &result);
                GetAllRunningProcessesForResult(env, async_callback_info->runningProcessInfo, result);
                napi_resolve_deferred(async_callback_info->env, async_callback_info->deferred, result);
                napi_delete_async_work(env, async_callback_info->asyncWork);
                delete async_callback_info;
            },
            (void *)async_callback_info,
            &async_callback_info->asyncWork);
        napi_queue_async_work(env, async_callback_info->asyncWork);
        return promise;
    }
}

napi_value NAPI_RemoveMission(napi_env env, napi_callback_info info)
{
    HILOG_INFO("NAPI_RemoveMission called...");
    size_t argc = 2;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);

    AsyncRemoveMissionCallbackInfo *async_callback_info =
        new AsyncRemoveMissionCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};

    napi_valuetype valuetype0;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype0));
    NAPI_ASSERT(env, valuetype0 == napi_number, "Wrong argument type. Numbers expected.");
    int32_t value0;
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &value0));
    async_callback_info->index = value0;

    if (argc >= 2) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &async_callback_info->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_RemoveMissionCallBack", NAPI_AUTO_LENGTH, &resourceName);

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("removeMission called(CallBack Mode)...");
                AsyncRemoveMissionCallbackInfo *async_callback_info = (AsyncRemoveMissionCallbackInfo *)data;
                async_callback_info->result = GetAbilityManagerInstance()->RemoveMission(async_callback_info->index);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("removeMission compeleted(CallBack Mode)...");
                AsyncRemoveMissionCallbackInfo *async_callback_info = (AsyncRemoveMissionCallbackInfo *)data;

                napi_value result;
                napi_value callback;
                napi_value undefined;

                napi_create_int32(async_callback_info->env, async_callback_info->result, &result);
                napi_get_undefined(env, &undefined);

                napi_get_reference_value(env, async_callback_info->callback[0], &callback);
                napi_call_function(env, undefined, callback, 1, &result, nullptr);

                if (async_callback_info->callback[0] != nullptr) {
                    napi_delete_reference(env, async_callback_info->callback[0]);
                }

                napi_delete_async_work(env, async_callback_info->asyncWork);
                delete async_callback_info;
            },
            (void *)async_callback_info,
            &async_callback_info->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, async_callback_info->asyncWork));
        return NULL;
    } else {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_RemoveMissionPromise", NAPI_AUTO_LENGTH, &resourceName);

        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        async_callback_info->deferred = deferred;

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("removeMission called(Promise Mode)...");
                AsyncRemoveMissionCallbackInfo *async_callback_info = (AsyncRemoveMissionCallbackInfo *)data;
                async_callback_info->result = GetAbilityManagerInstance()->RemoveMission(async_callback_info->index);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("removeMission compeleted(Promise Mode)...");
                AsyncRemoveMissionCallbackInfo *async_callback_info = (AsyncRemoveMissionCallbackInfo *)data;
                napi_value result;
                napi_create_int32(async_callback_info->env, async_callback_info->result, &result);
                napi_resolve_deferred(async_callback_info->env, async_callback_info->deferred, result);
                napi_delete_async_work(env, async_callback_info->asyncWork);
                delete async_callback_info;
            },
            (void *)async_callback_info,
            &async_callback_info->asyncWork);
        napi_queue_async_work(env, async_callback_info->asyncWork);
        return promise;
    }
}

napi_value NAPI_RemoveStack(napi_env env, napi_callback_info info)
{
    HILOG_INFO("NAPI_RemoveStack called...");
    size_t argc = 2;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);

    AsyncRemoveStackCallbackInfo *async_callback_info =
        new AsyncRemoveStackCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};

    napi_valuetype valuetype0;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype0));
    NAPI_ASSERT(env, valuetype0 == napi_number, "Wrong argument type. Numbers expected.");
    int32_t value0;
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &value0));
    async_callback_info->index = value0;

    if (argc >= 2) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &async_callback_info->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_RemoveStackCallBack", NAPI_AUTO_LENGTH, &resourceName);

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("removeStack called(CallBack Mode)...");
                AsyncRemoveStackCallbackInfo *async_callback_info = (AsyncRemoveStackCallbackInfo *)data;
                async_callback_info->result = GetAbilityManagerInstance()->RemoveStack(async_callback_info->index);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("removeStack compeleted(CallBack Mode)...");
                AsyncRemoveStackCallbackInfo *async_callback_info = (AsyncRemoveStackCallbackInfo *)data;

                napi_value result;
                napi_value callback;
                napi_value undefined;

                napi_create_int32(async_callback_info->env, async_callback_info->result, &result);
                napi_get_undefined(env, &undefined);

                napi_get_reference_value(env, async_callback_info->callback[0], &callback);
                napi_call_function(env, undefined, callback, 1, &result, nullptr);

                if (async_callback_info->callback[0] != nullptr) {
                    napi_delete_reference(env, async_callback_info->callback[0]);
                }

                napi_delete_async_work(env, async_callback_info->asyncWork);
                delete async_callback_info;
            },
            (void *)async_callback_info,
            &async_callback_info->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, async_callback_info->asyncWork));
        return NULL;
    } else {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_RemoveStackPromise", NAPI_AUTO_LENGTH, &resourceName);

        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        async_callback_info->deferred = deferred;

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("removeStack called(Promise Mode)...");
                AsyncRemoveStackCallbackInfo *async_callback_info = (AsyncRemoveStackCallbackInfo *)data;
                async_callback_info->result = GetAbilityManagerInstance()->RemoveStack(async_callback_info->index);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("removeStack compeleted(Promise Mode)...");
                AsyncRemoveStackCallbackInfo *async_callback_info = (AsyncRemoveStackCallbackInfo *)data;
                napi_value result;
                napi_create_int32(async_callback_info->env, async_callback_info->result, &result);
                napi_resolve_deferred(async_callback_info->env, async_callback_info->deferred, result);
                napi_delete_async_work(env, async_callback_info->asyncWork);
                delete async_callback_info;
            },
            (void *)async_callback_info,
            &async_callback_info->asyncWork);
        napi_queue_async_work(env, async_callback_info->asyncWork);
        return promise;
    }
}

napi_value NAPI_MoveMissionToTop(napi_env env, napi_callback_info info)
{
    HILOG_INFO("NAPI_MoveMissionToTop called...");
    size_t argc = 2;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);

    AsyncMoveMissionToTopCallbackInfo *async_callback_info =
        new AsyncMoveMissionToTopCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};

    napi_valuetype valuetype0;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype0));
    NAPI_ASSERT(env, valuetype0 == napi_number, "Wrong argument type. Numbers expected.");
    int32_t value0;
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &value0));
    async_callback_info->index = value0;

    if (argc >= 2) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &async_callback_info->callback[0]);

        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_MoveMissionToTopCallBack", NAPI_AUTO_LENGTH, &resourceName);

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("moveMissionToTop called(CallBack Mode)...");
                AsyncMoveMissionToTopCallbackInfo *async_callback_info = (AsyncMoveMissionToTopCallbackInfo *)data;
                async_callback_info->result = GetAbilityManagerInstance()->MoveMissionToTop(async_callback_info->index);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("moveMissionToTop compeleted(CallBack Mode)...");
                AsyncMoveMissionToTopCallbackInfo *async_callback_info = (AsyncMoveMissionToTopCallbackInfo *)data;

                napi_value result;
                napi_value callback;
                napi_value undefined;

                napi_create_int32(async_callback_info->env, async_callback_info->result, &result);
                napi_get_undefined(env, &undefined);

                napi_get_reference_value(env, async_callback_info->callback[0], &callback);
                napi_call_function(env, undefined, callback, 1, &result, nullptr);

                if (async_callback_info->callback[0] != nullptr) {
                    napi_delete_reference(env, async_callback_info->callback[0]);
                }

                napi_delete_async_work(env, async_callback_info->asyncWork);
                delete async_callback_info;
            },
            (void *)async_callback_info,
            &async_callback_info->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, async_callback_info->asyncWork));
        return NULL;
    } else {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_MoveMissionToTopPromise", NAPI_AUTO_LENGTH, &resourceName);

        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        async_callback_info->deferred = deferred;

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("moveMissionToTop called(Promise Mode)...");
                AsyncMoveMissionToTopCallbackInfo *async_callback_info = (AsyncMoveMissionToTopCallbackInfo *)data;
                async_callback_info->result = GetAbilityManagerInstance()->MoveMissionToTop(async_callback_info->index);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("moveMissionToTop compeleted(Promise Mode)...");
                AsyncMoveMissionToTopCallbackInfo *async_callback_info = (AsyncMoveMissionToTopCallbackInfo *)data;
                napi_value result;
                napi_create_int32(async_callback_info->env, async_callback_info->result, &result);
                napi_resolve_deferred(async_callback_info->env, async_callback_info->deferred, result);
                napi_delete_async_work(env, async_callback_info->asyncWork);
                delete async_callback_info;
            },
            (void *)async_callback_info,
            &async_callback_info->asyncWork);
        napi_queue_async_work(env, async_callback_info->asyncWork);
        return promise;
    }
}