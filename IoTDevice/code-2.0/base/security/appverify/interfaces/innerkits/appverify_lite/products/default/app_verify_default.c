/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "app_verify_default.h"
#include "app_verify_pub.h"
#include "pms_common.h"
#include "pms_inner.h"
#include "pms_interface_inner.h"
#include "iunknown.h"
#include "samgr_lite.h"


int GetUdidServer(unsigned char *udid, int size)
{
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(PERMISSION_SERVICE, PERM_INNER);
    if (iUnknown == NULL) {
        return INQUIRY_UDID_ERROR;
    }
    PmsInnerApi *interface = NULL;
    iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **) &interface);
    if (interface == NULL || interface->GetDevUdid == NULL) {
        return INQUIRY_UDID_ERROR;
    }
    int ret = interface->GetDevUdid(udid, size);
    return ret;
}

int GetUdidClient(unsigned char *udid, int size)
{
    return RequestDevUdid(udid, size);
}

int GetUdid(unsigned char *udid, int size)
{
    int ret;
    if (APPVERI_IsActsMode() == false) {
        ret = GetUdidServer(udid, size);
    } else {
        ret = GetUdidClient(udid, size);
    }
    return ret;
}

void RegistBaseDefaultFunc(ProductDiff *productFunc)
{
    productFunc->devUdidFunc = GetUdid;
}
