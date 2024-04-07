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

#include "devhost_service_full.h"
#include "devmgr_service_clnt.h"
#include "hdf_device_node.h"
#include "power_state_token.h"
#include "hdf_base.h"
#include "hdf_device_info.h"
#include "hdf_log.h"
#include "osal_message.h"
#include "hdf_device_info_full.h"

#define HDF_LOG_TAG devhost_service_full

static int32_t DevHostServiceFullDispatchMessage(struct HdfMessageTask *task, struct HdfMessage *msg)
{
    struct DevHostServiceFull *hostService =
        HDF_SLIST_CONTAINER_OF(struct HdfMessageTask, task, struct DevHostServiceFull, task);

    int status = HDF_SUCCESS;
    switch (msg->messageId) {
        case DEVHOST_MESSAGE_ADD_DEVICE: {
            struct HdfDeviceInfo *attribute = (struct HdfDeviceInfo *)msg->data[0];
            status = DevHostServiceAddDevice(&hostService->super.super, attribute);
            if (status != HDF_SUCCESS) {
                HDF_LOGE("DevHostServiceAddDevice failed and return %d", status);
            }
            break;
        }
        case DEVHOST_MESSAGE_DEL_DEVICE: {
            struct HdfDeviceInfo *attribute = (struct HdfDeviceInfo *)msg->data[0];
            status = DevHostServiceDelDevice(&hostService->super.super, attribute);
            if (status != HDF_SUCCESS) {
                HDF_LOGE("DevHostServiceDelDevice failed and return %d", status);
            }
            HdfDeviceInfoFullFreeInstance((struct HdfDeviceInfoFull *)attribute);
            break;
        }
        default: {
            HDF_LOGE("DevHostServiceFullDispatchMessage unknown message %d", msg->messageId);
            break;
        }
    }

    return status;
}

static int DevHostServiceFullOpsDevice(
    struct IDevHostService *devHostService, const struct HdfDeviceInfo *attribute, int cmdCode)
{
    if (devHostService == NULL || attribute == NULL) {
        HDF_LOGE("input is null");
        return HDF_FAILURE;
    }
    struct DevHostServiceFull *inst = (struct DevHostServiceFull *)devHostService;
    struct HdfMessageTask *task = &inst->task;
    struct HdfMessage *message = HdfMessageObtain(0);
    if (message == NULL) {
        HDF_LOGE("HdfMessageObtain(0) return null");
        return HDF_ERR_MALLOC_FAIL;
    }

    message->messageId = cmdCode;
    message->data[0] = (void *)attribute;
    return task->SendMessage(task, message, true);
}

static int DevHostServiceFullAddDevice(
    struct IDevHostService *devHostService, const struct HdfDeviceInfo *attribute)
{
    return DevHostServiceFullOpsDevice(devHostService, attribute, DEVHOST_MESSAGE_ADD_DEVICE);
}

static int DevHostServiceFullDelDevice(
    struct IDevHostService *devHostService, const struct HdfDeviceInfo *attribute)
{
    return DevHostServiceFullOpsDevice(devHostService, attribute, DEVHOST_MESSAGE_DEL_DEVICE);
}

static int DispatchSysEventToDevice(struct HdfDevice *device, uint32_t event)
{
    struct HdfSListIterator deviceNodeIt;
    struct HdfDeviceNode *deviceNode = NULL;
    int ret = HDF_SUCCESS;

    HdfSListIteratorInit(&deviceNodeIt, &device->services);
    while (HdfSListIteratorHasNext(&deviceNodeIt)) {
        deviceNode = HDF_SLIST_CONTAINER_OF(struct HdfSListNode, HdfSListIteratorNext(&deviceNodeIt),
            struct HdfDeviceNode, entry);
        if (deviceNode->powerToken != NULL) {
            ret = PowerStateOnSysStateChange(deviceNode->powerToken, event);
        }
    }

    return ret;
}

static int OnSysEventReceived(struct HdfSysEventNotifyNode *self, uint64_t eventClass,
    uint32_t event, const char* content)
{
    (void)(content);
    struct HdfDevice *device = NULL;

    if (self == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    struct DevHostService *hostService = CONTAINER_OF(self, struct DevHostService, sysEventNotifyNode);
    HDF_LOGI("host receive eventClass=%llu, event=%u", (unsigned long long)eventClass, event);

    if (HdfPmIsWakeEvent(event)) {
        DLIST_FOR_EACH_ENTRY_REVERSE(device, &hostService->devices, struct HdfDevice, node) {
            DispatchSysEventToDevice(device, event);
        }
    } else {
        DLIST_FOR_EACH_ENTRY(device, &hostService->devices, struct HdfDevice, node) {
            DispatchSysEventToDevice(device, event);
        }
    }

    return HDF_SUCCESS;
}

static int DevHostServiceFullStartService(struct IDevHostService *service)
{
    struct DevHostService *hostService = (struct DevHostService*)service;
    if (hostService == NULL) {
        HDF_LOGE("Start device service failed, hostService is null");
        return HDF_FAILURE;
    }

    int ret = DevmgrServiceClntAttachDeviceHost(hostService->hostId, service);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    hostService->sysEventNotifyNode.callback = OnSysEventReceived;
    ret = HdfSysEventNotifyRegister(&hostService->sysEventNotifyNode, HDF_SYSEVENT_CLASS_POWER);
    if (ret != HDF_SUCCESS) {
        HDF_LOGW("failed to register power event listener");
    } else {
        HDF_LOGD("host register power event listener success");
    }

    return HDF_SUCCESS;
}

void DevHostServiceFullConstruct(struct DevHostServiceFull *inst)
{
    struct IDevHostService *hostServiceIf = &inst->super.super;
    static struct IHdfMessageHandler handler = {
        .Dispatch = DevHostServiceFullDispatchMessage
    };
    DevHostServiceConstruct(&inst->super);
    hostServiceIf->AddDevice = DevHostServiceFullAddDevice;
    hostServiceIf->DelDevice = DevHostServiceFullDelDevice;
    hostServiceIf->StartService = DevHostServiceFullStartService;
    HdfMessageLooperConstruct(&inst->looper);
    HdfMessageTaskConstruct(&inst->task, &inst->looper, &handler);
}

void DevHostServiceFullDestruct(struct DevHostServiceFull *inst)
{
    if (inst != NULL) {
        DevHostServiceDestruct(&inst->super);
        if (inst->looper.Stop != NULL) {
            inst->looper.Stop(&inst->looper);
        }
    }
}
