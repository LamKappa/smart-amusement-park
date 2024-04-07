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

#include "display_gralloc.h"
#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <securec.h>
#include "hdf_log.h"
#include "display_type.h"

#define DEFAULT_READ_WRITE_PERMISSIONS   0666
#define MAX_MALLOC_SIZE                  0x10000000UL
#define SHM_MAX_KEY                      10000
#define SHM_START_KEY                    1

static int32_t AllocMem(GrallocBuffer *buffer)
{
    static int32_t key = SHM_START_KEY;
    int32_t shmid;

    if (buffer == NULL) {
        HDF_LOGE("%s: buffer is null", __func__);
        return DISPLAY_NULL_PTR;
    }
    HDF_LOGD("%s: buffer->type = %d, buffer->size = %u", __func__, buffer->type, buffer->size);
    if ((buffer->size > MAX_MALLOC_SIZE) || (buffer->size == 0)) {
        HDF_LOGE("%s: size is invalid, size = %u", __func__, buffer->size);
        return DISPLAY_FAILURE;
    }
    while ((shmid = shmget(key, buffer->size, IPC_CREAT | IPC_EXCL | DEFAULT_READ_WRITE_PERMISSIONS)) < 0) {
        if (errno != EEXIST) {
            HDF_LOGE("%s: fail to alloc shared memory, errno = %d", __func__, errno);
            return DISPLAY_FAILURE;
        }
        key++;
        if (key >= SHM_MAX_KEY) {
            key = SHM_START_KEY;
        }
    }
    void *pBase = shmat(shmid, NULL, 0);
    if (pBase == ((void *)-1)) {
        HDF_LOGE("%s: fail to shmat shared memory, errno = %d", __func__, errno);
        return DISPLAY_FAILURE;
    }
    buffer->virAddr = pBase;
    buffer->hdl.key = key;
    buffer->hdl.shmid = shmid;
    HDF_LOGD("%s: key = %d, shmid = %d", __func__, key, shmid);
    key++;
    if (key >= SHM_MAX_KEY) {
        key = SHM_START_KEY;
    }
    (void)memset_s(buffer->virAddr, buffer->size, 0xff, buffer->size);
    return DISPLAY_SUCCESS;
}

static void FreeMem(GrallocBuffer *buffer)
{
    if (buffer == NULL || buffer->virAddr == NULL) {
        HDF_LOGE("%s: pointer is null", __func__);
        return;
    }
    if (shmdt(buffer->virAddr) == -1) {
        HDF_LOGE("%s: fail to free shared memory, errno = %d", __func__, errno);
    }
    if (shmctl(buffer->hdl.shmid, IPC_RMID, 0) == -1) {
        HDF_LOGE("%s: fail to free shmid, errno = %d", __func__, errno);
    }
}

static void *Mmap(GrallocBuffer *buffer)
{
    int32_t shmid;

    if (buffer == NULL) {
        HDF_LOGE("%s: buffer is null", __func__);
        return NULL;
    }

    HDF_LOGD("%s: buffer->hdl.key = %d, buffer->hdl.shmid = %d", __func__, buffer->hdl.key, buffer->hdl.shmid);
    shmid = shmget(buffer->hdl.key, buffer->size, IPC_EXCL | DEFAULT_READ_WRITE_PERMISSIONS);
    if (shmid < 0) {
        HDF_LOGE("%s: fail to mmap shared memory, errno = %d", __func__, errno);
        return NULL;
    }
    void *pBase = shmat(shmid, NULL, 0);
    if (pBase == ((void *)-1)) {
        HDF_LOGE("%s: fail to shmat share memory, errno = %d", __func__, errno);
        return NULL;
    }
    buffer->hdl.shmid = shmid;
    HDF_LOGD("%s: shmid = %d", __func__, shmid);
    return pBase;
}

static int32_t Unmap(GrallocBuffer *buffer)
{
    if (buffer == NULL || buffer->virAddr == NULL) {
        HDF_LOGE("%s: pointer is null", __func__);
        return DISPLAY_NULL_PTR;
    }
    if (shmdt(buffer->virAddr) == -1) {
        HDF_LOGE("%s: fail to unmap shared memory, errno = %d", __func__, errno);
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

int32_t GrallocInitialize(GrallocFuncs **funcs)
{
    if (funcs == NULL) {
        HDF_LOGE("%s: funcs is null", __func__);
        return DISPLAY_NULL_PTR;
    }
    GrallocFuncs *gFuncs = (GrallocFuncs *)malloc(sizeof(GrallocFuncs));
    if (gFuncs == NULL) {
        HDF_LOGE("%s: gFuncs is null", __func__);
        return DISPLAY_NULL_PTR;
    }
    (void)memset_s(gFuncs, sizeof(GrallocFuncs), 0, sizeof(GrallocFuncs));
    gFuncs->AllocMem = AllocMem;
    gFuncs->FreeMem = FreeMem;
    gFuncs->Mmap = Mmap;
    gFuncs->Unmap = Unmap;
    *funcs = gFuncs;
    HDF_LOGI("%s: gralloc initialize success", __func__);
    return DISPLAY_SUCCESS;
}

int32_t GrallocUninitialize(GrallocFuncs *funcs)
{
    if (funcs == NULL) {
        HDF_LOGE("%s: funcs is null", __func__);
        return DISPLAY_NULL_PTR;
    }
    free(funcs);
    HDF_LOGI("%s: gralloc uninitialize success", __func__);
    return DISPLAY_SUCCESS;
}