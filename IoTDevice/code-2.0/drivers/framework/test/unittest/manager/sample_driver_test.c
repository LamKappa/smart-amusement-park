/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "sample_driver_test.h"
#include "devsvc_manager_clnt.h"
#include "hdf_log.h"
#include "hdf_device_desc.h"

#define HDF_LOG_TAG sample_driver_test

#ifndef INT32_MAX
#define INT32_MAX 0x7fffffff
#endif

void HdfSampleDriverRelease(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    return;
}

int32_t SampleDriverRegisterDevice(struct HdfSBuf *data)
{
    if (data == NULL) {
        return HDF_FAILURE;
    }

    const char *moduleName = HdfSbufReadString(data);
    if (moduleName == NULL) {
        return HDF_FAILURE;
    }
    const char *serviceName = HdfSbufReadString(data);
    if (serviceName == NULL) {
        return HDF_FAILURE;
    }

    struct HdfDeviceObject *devObj = HdfRegisterDevice(moduleName, serviceName);
    if (devObj == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t SampleDriverUnregisterDevice(struct HdfSBuf *data)
{
    if (data == NULL) {
        return HDF_FAILURE;
    }

    const char *moduleName = HdfSbufReadString(data);
    if (moduleName == NULL) {
        return HDF_FAILURE;
    }
    const char *serviceName = HdfSbufReadString(data);
    if (serviceName == NULL) {
        return HDF_FAILURE;
    }
    HdfUnregisterDevice(moduleName, serviceName);
    return HDF_SUCCESS;
}

int32_t SampleDriverSendEvent(struct HdfDeviceIoClient *client, int id, struct HdfSBuf *data, bool broadcast)
{
    return broadcast ? HdfDeviceSendEvent(client->device, id, data) : HdfDeviceSendEventToClient(client, id, data);
}

int32_t SampleDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret = HDF_SUCCESS;
    if (reply == NULL || client == NULL) {
        return HDF_FAILURE;
    }
    switch (cmdId) {
        case SAMPLE_DRIVER_REGISTER_DEVICE: {
            ret = SampleDriverRegisterDevice(data);
            HdfSbufWriteInt32(reply, ret);
            break;
        }
        case SAMPLE_DRIVER_UNREGISTER_DEVICE:
            ret = SampleDriverUnregisterDevice(data);
            HdfSbufWriteInt32(reply, ret);
            break;
        case SAMPLE_DRIVER_SENDEVENT_SINGLE_DEVICE:
            ret =  SampleDriverSendEvent(client, cmdId, data, false);
            HdfSbufWriteInt32(reply, INT32_MAX);
            break;
        case SAMPLE_DRIVER_SENDEVENT_BROADCAST_DEVICE:
            ret = SampleDriverSendEvent(client, cmdId, data, true);
            HdfSbufWriteInt32(reply, INT32_MAX);
            break;
        default:
            break;
    }

    return ret;
}

int HdfSampleDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGD("%s::enter!, deviceObject=%p", __func__, deviceObject);
    if (deviceObject == NULL) {
        return HDF_FAILURE;
    }
    static struct IDeviceIoService testService = {
        .Dispatch = SampleDriverDispatch,
        .Open = NULL,
        .Release = NULL,
    };
    deviceObject->service = &testService;
    return HDF_SUCCESS;
}

int HdfSampleDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGD("%s::enter!, deviceObject=%p", __func__, deviceObject);
    if (deviceObject == NULL) {
        HDF_LOGE("%s::ptr is null!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s:Init success", __func__);
    return HDF_SUCCESS;
}


struct HdfDriverEntry g_sampleDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "sample_driver",
    .Bind = HdfSampleDriverBind,
    .Init = HdfSampleDriverInit,
    .Release = HdfSampleDriverRelease,
};

HDF_INIT(g_sampleDriverEntry);

