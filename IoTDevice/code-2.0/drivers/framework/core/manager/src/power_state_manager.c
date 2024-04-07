/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "power_state_manager.h"
#include "hdf_object_manager.h"
#include "power_state_token_clnt.h"

static struct PowerStateTokenClnt *PowerStateManagerGetStateTokenClnt(
    struct PowerStateManager *inst, struct IPowerStateToken *tokenIf)
{
    struct HdfSListIterator it;
    if (inst == NULL) {
        return NULL;
    }
    HdfSListIteratorInit(&it, &inst->tokens);
    while (HdfSListIteratorHasNext(&it)) {
        struct PowerStateTokenClnt *tokenClnt =
            (struct PowerStateTokenClnt *)HdfSListIteratorNext(&it);
        if (tokenClnt->tokenIf == tokenIf)  {
            return tokenClnt;
        }
    }
    return NULL;
}

static void PowerStateManagerAcquireWakeLock(
    struct PowerStateManager *inst, struct IPowerStateToken *tokenIf)
{
    if (inst == NULL) {
        return;
    }
    struct HdfSRef *sref = &inst->wakeRef;
    struct PowerStateTokenClnt *stateTokenClnt = PowerStateManagerGetStateTokenClnt(inst, tokenIf);
    if (stateTokenClnt == NULL) {
        return;
    }
    stateTokenClnt->powerState = POWER_STATE_ACTIVE;
    if (sref->Acquire != NULL) {
        sref->Acquire(sref);
    }
}

static void PowerStateManagerReleaseWakeLock(
    struct PowerStateManager *inst, struct IPowerStateToken *tokenIf)
{
    struct HdfSRef *sref = NULL;
    struct PowerStateTokenClnt *stateTokenClnt = PowerStateManagerGetStateTokenClnt(inst, tokenIf);
    if (inst == NULL || stateTokenClnt == NULL) {
        return;
    }
    stateTokenClnt->powerState = POWER_STATE_INACTIVE;
    sref = &inst->wakeRef;
    if (sref->Release != NULL) {
        sref->Release(sref);
    }
}

static void PowerStateManagerConstruct(struct PowerStateManager *inst)
{
    static struct IHdfSRefListener wakeLockRefListener = {
        .OnFirstAcquire = NULL,
        .OnLastRelease = NULL,
    };

    inst->AcquireWakeLock = PowerStateManagerAcquireWakeLock;
    inst->ReleaseWakeLock = PowerStateManagerReleaseWakeLock;
    HdfSListInit(&inst->tokens);
    HdfSRefConstruct(&inst->wakeRef, &wakeLockRefListener);
}

struct PowerStateManager *PowerStateManagerGetInstance()
{
    static struct PowerStateManager powerStateManager = { 0 };
    if (powerStateManager.AcquireWakeLock == NULL) {
        PowerStateManagerConstruct(&powerStateManager);
    }
    return &powerStateManager;
}
