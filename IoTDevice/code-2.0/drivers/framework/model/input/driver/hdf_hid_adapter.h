/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_HID_ADAPTER_H
#define HDF_HID_ADAPTER_H
typedef struct HidInformation {
    uint32_t devType;
    const char *devName;
} HidInfo;

enum HidType {
    HID_TYPE_BEGIN_POS = 33,    /* HID type start position */
    HID_TYPE_MOUSE,             /* Mouse */
    HID_TYPE_KEYBOARD,          /* Keyboard */
    HID_TYPE_UNKNOWN,           /* Unknown input device type */
};

void* HidRegisterHdfInputDev(HidInfo dev);
void HidUnregisterHdfInputDev(const void *inputDev);
void HidReportEvent(const void *inputDev, uint32_t type, uint32_t code, int32_t value);

#endif