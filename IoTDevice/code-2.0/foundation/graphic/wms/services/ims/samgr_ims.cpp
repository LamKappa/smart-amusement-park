/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include <ohos_errno.h>
#include <ohos_init.h>

#include "feature.h"
#include "gfx_utils/graphic_log.h"
#include "input_event_client_proxy.h"
#include "iproxy_client.h"
#include "iproxy_server.h"
#include "iunknown.h"
#include "samgr_lite.h"
#include "service.h"

namespace OHOS {
struct DefaultFeatureApi {
    INHERIT_SERVER_IPROXY;
};

struct IMSService {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(DefaultFeatureApi);
    Identity identity;
};

static const char* GetName(Service* service)
{
    (void)service;
    return IMS_SERVICE_NAME;
}

static BOOL Initialize(Service* service, Identity identity)
{
    IMSService* example = reinterpret_cast<IMSService*>(service);
    example->identity = identity;
    GRAPHIC_LOGI("Initialize(%s)! Identity<%d, %d, %p>", IMS_SERVICE_NAME,
        identity.serviceId, identity.featureId, identity.queueId);
    return TRUE;
}

static BOOL MessageHandle(Service* service, Request* msg)
{
    GRAPHIC_LOGI("MessageHandle(%s)! Request<%d, %d, %p>",
        service->GetName(service), msg->msgId, msg->msgValue, msg->data);
    return FALSE;
}

static TaskConfig GetTaskConfig(Service* service)
{
    (void)service;
    TaskConfig config = {LEVEL_HIGH, PRI_BELOW_NORMAL, 0x800, 20, SHARED_TASK};
    return config;
}

static int32 Invoke(IServerProxy* iProxy, int funcId, void* origin, IpcIo* req, IpcIo* reply)
{
    InputEventClientProxy::GetInstance()->ClientRequestHandle(funcId, origin, req, reply);
    return EC_SUCCESS;
}

static IMSService g_example = {
    .GetName = GetName,
    .Initialize = Initialize,
    .MessageHandle = MessageHandle,
    .GetTaskConfig = GetTaskConfig,
    SERVER_IPROXY_IMPL_BEGIN,
    .Invoke = Invoke,
    IPROXY_END,
};

static void Init(void)
{
    BOOL ret = SAMGR_GetInstance()->RegisterService(reinterpret_cast<Service*>(&g_example));
    if (ret != TRUE) {
        GRAPHIC_LOGE("regist service failed.");
        return;
    }
    ret = SAMGR_GetInstance()->RegisterDefaultFeatureApi(IMS_SERVICE_NAME, GET_IUNKNOWN(g_example));
    if (ret != TRUE) {
        GRAPHIC_LOGE("regist feature failed.");
        return;
    }
}

SYSEX_SERVICE_INIT(Init);
} // namespace OHOS