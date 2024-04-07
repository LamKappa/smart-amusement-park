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

#include "init_adapter.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/reboot.h>
#ifdef __LINUX__
#include <linux/securebits.h>
#include "init_signal_handler.h"
#endif

void RebootSystem()
{
    int ret = reboot(RB_AUTOBOOT);
    if (ret != 0) {
        printf("[Init] reboot failed! syscall ret %d, err %d.\n", ret, errno);
    }
}

int KeepCapability()
{
#ifdef __LINUX__
    if (prctl(PR_SET_SECUREBITS, SECBIT_NO_SETUID_FIXUP | SECBIT_NO_SETUID_FIXUP_LOCKED)) {
        printf("[Init] prctl failed\n");
        return -1;
    }
#endif
    return 0;
}

int SetAmbientCapability(int cap)
{
#ifdef __LINUX__
    if (prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, cap, 0, 0)) {
        printf("[Init] prctl PR_CAP_AMBIENT failed\n");
        return -1;
    }
#endif
    return 0;
}

void ExecuteRcs()
{
#if (defined __LINUX__) && (defined NEED_EXEC_RCS_LINUX)
    pid_t retPid = fork();
    if (retPid < 0) {
        printf("[Init] ExecuteRcs, fork failed! err %d.\n", errno);
        return;
    }

    // child process
    if (retPid == 0) {
        printf("[Init] ExecuteRcs, child process id %d.\n", getpid());
        if (execle("/bin/sh", "sh", "/etc/init.d/rcS", NULL, NULL) != 0) {
            printf("[Init] ExecuteRcs, execle failed! err %d.\n", errno);
        }
        _exit(0x7f); // 0x7f: user specified
    }

    // init process
    sem_t sem;
    if (sem_init(&sem, 0, 0) != 0) {
        printf("[Init] ExecuteRcs, sem_init failed, err %d.\n", errno);
        return;
    }
    SignalRegWaitSem(retPid, &sem);

    // wait until rcs process exited
    if (sem_wait(&sem) != 0) {
        printf("[Init] ExecuteRcs, sem_wait failed, err %d.\n", errno);
    }
#endif
}

