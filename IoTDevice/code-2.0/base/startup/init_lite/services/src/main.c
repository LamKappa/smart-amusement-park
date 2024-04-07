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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifdef OHOS_DEBUG
#include <errno.h>
#include <time.h>
#endif // OHOS_DEBUG

#include <unistd.h>

#include "init_adapter.h"
#include "init_read_cfg.h"
#include "init_signal_handler.h"
#ifdef OHOS_LITE
#include "parameter.h"
#endif

#ifndef OHOS_LITE
#include "device.h"
#endif

static const pid_t INIT_PROCESS_PID = 1;

static void PrintSysInfo()
{
#ifdef OHOS_LITE
    const char* sysInfo = GetVersionId();
    if (sysInfo != NULL) {
        printf("[Init] %s\n", sysInfo);
        return;
    }
    printf("[Init] main, GetVersionId failed!\n");
#endif
}

#ifdef OHOS_DEBUG
static long TimeDiffMs(struct timespec* tmBefore, struct timespec* tmAfter)
{
    if (tmBefore != NULL && tmAfter != NULL) {
        long timeUsed = (tmAfter->tv_sec - tmBefore->tv_sec) * 1000 +     // 1 s = 1000 ms
            (tmAfter->tv_nsec - tmBefore->tv_nsec) / 1000000;    // 1 ms = 1000000 ns
        return timeUsed;
    }
    return -1;
}
#endif // OHOS_DEBUG

int main(int argc, char * const argv[])
{
#ifdef OHOS_DEBUG
    struct timespec tmEnter;
    if (clock_gettime(CLOCK_REALTIME, &tmEnter) != 0) {
        printf("[Init] main, enter, get time failed! err %d.\n", errno);
    }
#endif // OHOS_DEBUG

    if (getpid() != INIT_PROCESS_PID) {
        printf("[Init] main, current process id is %d not %d, failed!\n", getpid(), INIT_PROCESS_PID);
        return 0;
    }

    // 1. print system info
    PrintSysInfo();

#ifndef OHOS_LITE
    // 2. Mount basic filesystem and create common device node.
    MountBasicFs();
    CreateDeviceNode();
#endif

    // 3. signal register
    SignalInitModule();

#ifdef OHOS_DEBUG
    struct timespec tmSysInfo;
    if (clock_gettime(CLOCK_REALTIME, &tmSysInfo) != 0) {
        printf("[Init] main, after sysinfo, get time failed! err %d.\n", errno);
    }
#endif // OHOS_DEBUG

    // 4. execute rcs
    ExecuteRcs();

#ifdef OHOS_DEBUG
    struct timespec tmRcs;
    if (clock_gettime(CLOCK_REALTIME, &tmRcs) != 0) {
        printf("[Init] main, after rcs, get time failed! err %d.\n", errno);
    }
#endif // OHOS_DEBUG

    // 5. read configuration file and do jobs
    InitReadCfg();

#ifdef OHOS_DEBUG
    struct timespec tmCfg;
    if (clock_gettime(CLOCK_REALTIME, &tmCfg) != 0) {
        printf("[Init] main, after cfg, get time failed! err %d.\n", errno);
    }
#endif // OHOS_DEBUG

    // 6. keep process alive
#ifdef OHOS_DEBUG
    printf("[Init] main, time used: sigInfo %ld ms, rcs %ld ms, cfg %ld ms.\n", \
        TimeDiffMs(&tmEnter, &tmSysInfo), TimeDiffMs(&tmSysInfo, &tmRcs), TimeDiffMs(&tmRcs, &tmCfg));
#endif

    printf("[Init] main, entering wait.\n");
    while (1) {
        // pause only returns when a signal was caught and the signal-catching function returned.
        // pause only returns -1, no need to process the return value.
        (void)pause();
    }
    return 0;
}
