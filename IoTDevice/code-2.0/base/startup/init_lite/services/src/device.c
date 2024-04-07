/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#define DEFAULT_RW_MODE 0666
#define DEFAULT_NO_AUTHORITY_MODE 0600
#define DEVICE_ID_THIRD 3
#define DEVICE_ID_EIGHTH 8
#define DEVICE_ID_NINTH 9
#define DEVICE_ID_ELEVNTH 11

void MountBasicFs()
{
    if (mount("tmpfs", "/dev", "tmpfs", MS_NOSUID, "mode=0755") != 0) {
        printf("Mount tmpfs failed. %s\n", strerror(errno));
    }
    if (mount("proc", "/proc", "proc", 0, "hidepid=2") != 0) {
        printf("Mount procfs failed. %s\n", strerror(errno));
    }
    if (mount("sysfs", "/sys", "sysfs", 0, NULL) != 0) {
        printf("Mount sysfs failed. %s\n", strerror(errno));
    }
}

void CreateDeviceNode()
{
    if (mknod("/dev/kmsg", S_IFCHR | DEFAULT_NO_AUTHORITY_MODE, makedev(1, DEVICE_ID_ELEVNTH)) != 0) {
        printf("Create /dev/kmsg device node failed. %s\n", strerror(errno));
    }
    if (mknod("/dev/null", S_IFCHR | DEFAULT_RW_MODE, makedev(1, DEVICE_ID_THIRD)) != 0) {
        printf("Create /dev/null device node failed. %s\n", strerror(errno));
    }
    if (mknod("/dev/random", S_IFCHR | DEFAULT_RW_MODE, makedev(1, DEVICE_ID_EIGHTH)) != 0) {
        printf("Create /dev/random device node failed. %s\n", strerror(errno));
    }

    if (mknod("/dev/urandom", S_IFCHR | DEFAULT_RW_MODE, makedev(1, DEVICE_ID_NINTH)) != 0) {
        printf("Create /dev/urandom device node failed. %s\n", strerror(errno));
    }
}
