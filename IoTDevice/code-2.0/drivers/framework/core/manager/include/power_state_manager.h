/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef POWER_STATE_MANAGER_H
#define POWER_STATE_MANAGER_H

#include "hdf_sref.h"
#include "hdf_slist.h"
#include "power_state_token_if.h"

struct PowerStateManager {
    struct HdfSRef wakeRef;
    struct HdfSList tokens;
    void (*AcquireWakeLock)(struct PowerStateManager *, struct IPowerStateToken *);
    void (*ReleaseWakeLock)(struct PowerStateManager *, struct IPowerStateToken *);
};

struct PowerStateManager *PowerStateManagerGetInstance(void);

#endif /* POWER_STATE_MANAGER_H */