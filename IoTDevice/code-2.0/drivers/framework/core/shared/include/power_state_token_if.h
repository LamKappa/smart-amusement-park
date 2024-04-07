/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef POWER_STATE_TOKEN_IF_H
#define POWER_STATE_TOKEN_IF_H

typedef enum {
    POWER_STATE_IDLE,           /* Idle state */
    POWER_STATE_ACTIVE,         /* Activated state */
    POWER_STATE_INACTIVE,       /* Error state */
} HdfPowerState;

struct IPowerStateToken {
    void (*AcquireWakeLock)(struct IPowerStateToken *);
    void (*ReleaseWakeLock)(struct IPowerStateToken *);
};

#endif /* POWER_STATE_TOKEN_IF_H */