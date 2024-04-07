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

#include "dev_attribute_parcel.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG dev_attribute_parcel

bool DeviceAttributeFullWrite(const struct HdfDeviceInfoFull *attribute, struct HdfSBuf *sbuf)
{
    if (attribute == NULL || sbuf == NULL) {
        return false;
    }

    uint8_t ret = 1;
    ret &= HdfSbufWriteInt32(sbuf, attribute->super.deviceId);
    ret &= HdfSbufWriteInt32(sbuf, attribute->super.hostId);
    ret &= HdfSbufWriteInt32(sbuf, attribute->super.policy);
    ret &= HdfSbufWriteString(sbuf, attribute->super.svcName);
    ret &= HdfSbufWriteString(sbuf, attribute->super.moduleName);
    ret &= HdfSbufWriteString(sbuf, attribute->super.deviceMatchAttr);

    if (ret == 0) {
        HDF_LOGE("Device attribute write parcel failed");
        return false;
    }
    return true;
}

struct HdfDeviceInfoFull *DeviceAttributeFullRead(struct HdfSBuf *sbuf)
{
    if (sbuf == NULL) {
        return NULL;
    }

    struct HdfDeviceInfoFull *attribute = HdfDeviceInfoFullNewInstance();
    if (attribute == NULL) {
        HDF_LOGE("OsalMemCalloc failed, attribute is null");
        return NULL;
    }
    HdfSbufReadUint16(sbuf, &attribute->super.deviceId);
    HdfSbufReadUint16(sbuf, &attribute->super.hostId);
    HdfSbufReadUint16(sbuf, &attribute->super.policy);
    do {
        const char *svcName = HdfSbufReadString(sbuf);
        if (svcName == NULL) {
            HDF_LOGE("Read from parcel failed, svcName is null");
            break;
        }
        attribute->super.svcName = strdup(svcName);
        if (attribute->super.svcName == NULL) {
            HDF_LOGE("Read from parcel failed, strdup svcName fail");
            break;
        }
        const char *moduleName = HdfSbufReadString(sbuf);
        if (moduleName == NULL) {
            HDF_LOGE("Read from parcel failed, driverPath is null");
            break;
        }
        attribute->super.moduleName = strdup(moduleName);
        if (attribute->super.moduleName == NULL) {
            HDF_LOGE("Read from parcel failed, strdup moduleName fail");
            break;
        }

        return attribute;
    } while (0);

    HdfDeviceInfoFullFreeInstance(attribute);
    return NULL;
}
