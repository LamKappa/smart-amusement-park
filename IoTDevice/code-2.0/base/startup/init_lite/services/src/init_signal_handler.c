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

#include "init_signal_handler.h"

#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#ifdef __LINUX__
#include <errno.h>
#include <sys/types.h>
#endif /* __LINUX__ */

#include "init_service_manager.h"


#ifdef __LINUX__
static pid_t  g_waitPid = -1;
static sem_t* g_waitSem = NULL;

void SignalRegWaitSem(pid_t waitPid, sem_t* waitSem)
{
    g_waitPid = waitPid;
    g_waitSem = waitSem;
}

static void CheckWaitPid(pid_t sigPID)
{
    if (g_waitPid == sigPID && g_waitSem != NULL) {
        if (sem_post(g_waitSem) != 0) {
            printf("[Init] CheckWaitPid, sem_post failed, errno %d.\n", errno);
        }
        g_waitPid = -1;
        g_waitSem = NULL;
    }
}
#endif /* __LINUX__ */

static void SigHandler(int sig)
{
    switch (sig) {
        case SIGCHLD: {
            pid_t sigPID;
            int procStat = 0;
            while (1) {
                sigPID = waitpid(-1, &procStat, WNOHANG);
                if (sigPID <= 0) {
                    break;
                }
                printf("[Init] SigHandler, SIGCHLD received, sigPID = %d.\n", sigPID);
#ifdef __LINUX__
                CheckWaitPid(sigPID);
#endif /* __LINUX__ */
                ReapServiceByPID((int)sigPID);
            }
            break;
        }
        case SIGTERM: {
            printf("[Init] SigHandler, SIGTERM received.\n");
            StopAllServices();
            break;
        }
        default:
            printf("[Init] SigHandler, unsupported signal %d.\n", sig);
            break;
    }
}

void SignalInitModule()
{
    struct sigaction act;
    act.sa_handler = SigHandler;
    act.sa_flags   = SA_RESTART;
    (void)sigfillset(&act.sa_mask);

    sigaction(SIGCHLD, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
}

