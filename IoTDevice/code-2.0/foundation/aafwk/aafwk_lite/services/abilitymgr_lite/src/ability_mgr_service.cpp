/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "ability_mgr_service.h"

#include "ability_errors.h"
#ifdef APP_PLATFORM_WATCHGT
#include "ability_service.h"
#endif
#include "ability_service_interface.h"
#include "adapter.h"
#include "ohos_init.h"
#include "samgr_lite.h"
#include "util/abilityms_log.h"

namespace OHOS {
#ifndef APP_PLATFORM_WATCHGT
const int STACK_SIZE = 0x800;
#else
const int STACK_SIZE = 0xE00;
#endif
const int QUEUE_SIZE = 20;
const int BYTE_OFFSET = 8;

AbilityMgrService::AbilityMgrService() : Service(), identity_()
{
    this->Service::GetName = AbilityMgrService::GetServiceName;
    this->Service::Initialize = AbilityMgrService::ServiceInitialize;
    this->Service::MessageHandle = AbilityMgrService::ServiceMessageHandle;
    this->Service::GetTaskConfig = AbilityMgrService::GetServiceTaskConfig;
}

static void Init()
{
    SamgrLite *sm = SAMGR_GetInstance();
    CHECK_NULLPTR_RETURN(sm, "AbilityManagerService", "get samgr error");
    BOOL result = sm->RegisterService(AbilityMgrService::GetInstance());
    PRINTI("AbilityManagerService", "ams starts %{public}s", result ? "successfully" : "unsuccessfully");
}
SYSEX_SERVICE_INIT(Init);

const char *AbilityMgrService::GetServiceName(Service *service)
{
    (void)service;
    return AMS_SERVICE;
}

const Identity *AbilityMgrService::GetIdentity()
{
    return &identity_;
}

BOOL AbilityMgrService::ServiceInitialize(Service *service, Identity identity)
{
    if (service == nullptr) {
        return FALSE;
    }
    AbilityMgrService *abilityManagerService = static_cast<AbilityMgrService *>(service);
    abilityManagerService->identity_ = identity;
    return TRUE;
}

BOOL AbilityMgrService::ServiceMessageHandle(Service *service, Request *request)
{
    if (request == nullptr) {
        return FALSE;
    }
#ifdef APP_PLATFORM_WATCHGT
    int ret = ERR_OK;
    if (request->msgId == START_ABILITY) {
        ret = AbilityService::GetInstance().StartAbility(AbilityService::GetInstance().want_);
        AbilityService::GetInstance().CleanWant();
    } else if  (request->msgId == ABILITY_TRANSACTION_DONE) {
        int token = request->msgValue & 0xFF;
        int state = (request->msgValue >> BYTE_OFFSET) & 0xFF;
        ret = AbilityService::GetInstance().SchedulerLifecycleDone(token, state);
    } else if (request->msgId == TERMINATE_ABILITY) {
        ret = AbilityService::GetInstance().TerminateAbility(request->msgValue);
    } else if (request->msgId == TERMINATE_APP) {
        ret = AbilityService::GetInstance().ForceStopBundle(request->msgValue);
    }
    return ret == ERR_OK;
#else
    return TRUE;
#endif
}

TaskConfig AbilityMgrService::GetServiceTaskConfig(Service *service)
{
    TaskConfig config = {LEVEL_HIGH, PRI_NORMAL, STACK_SIZE, QUEUE_SIZE, SINGLE_TASK};
    return config;
}
} // namespace OHOS
