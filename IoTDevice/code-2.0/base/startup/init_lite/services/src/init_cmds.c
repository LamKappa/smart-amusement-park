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

#include "init_cmds.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>
#ifndef OHOS_LITE
#include <fcntl.h>
#include <linux/module.h>
#include <sys/syscall.h>
#endif

#include "init_service_manager.h"
#include "securec.h"

#define MODE_LEN 4   // for chmod mode, format 0xxx
#define DEFAULT_DIR_MODE 0755  // mkdir, default mode
#define SPACES_CNT_IN_CMD_MAX 10   // mount, max number of spaces in cmdline
#define SPACES_CNT_IN_CMD_MIN 2    // mount, min number of spaces in cmdline

#define LOADCFG_BUF_SIZE  128  // loadcfg, max buffer for one cmdline
#define LOADCFG_MAX_FILE_LEN 51200  // loadcfg, max file size is 50K
#define LOADCFG_MAX_LOOP 20  // loadcfg, to prevent to be trapped in infite loop
#define OCTAL_TYPE 8  // 8 means octal to decimal
#define MAX_BUFFER 256
static const char *g_supportCfg[] = {
    "/etc/patch.cfg",
    "/patch/fstab.cfg",
};

static const char* g_supportedCmds[] = {
    "start ",
    "mkdir ",
    "chmod ",
    "chown ",
    "mount ",
    "loadcfg ",
    "insmod ",
};

void ParseCmdLine(const char* cmdStr, CmdLine* resCmd)
{
    size_t cmdLineLen = 0;
    if (cmdStr == NULL || resCmd == NULL || (cmdLineLen = strlen(cmdStr)) == 0) {
        return;
    }

    size_t supportCmdCnt = sizeof(g_supportedCmds) / sizeof(g_supportedCmds[0]);
    int foundAndSucceed = 0;
    for (size_t i = 0; i < supportCmdCnt; ++i) {
        size_t curCmdNameLen = strlen(g_supportedCmds[i]);
        if (cmdLineLen > curCmdNameLen && cmdLineLen <= (curCmdNameLen + MAX_CMD_CONTENT_LEN) &&
            strncmp(g_supportedCmds[i], cmdStr, curCmdNameLen) == 0) {
            if (memcpy_s(resCmd->name, MAX_CMD_NAME_LEN, cmdStr, curCmdNameLen) != EOK) {
                break;
            }
            resCmd->name[curCmdNameLen] = '\0';

            const char* cmdContent = cmdStr + curCmdNameLen;
            size_t cmdContentLen = cmdLineLen - curCmdNameLen;
            if (memcpy_s(resCmd->cmdContent, MAX_CMD_CONTENT_LEN, cmdContent, cmdContentLen) != EOK) {
                break;
            }
            resCmd->cmdContent[cmdContentLen] = '\0';
            foundAndSucceed = 1;
            break;
        }
    }

    if (!foundAndSucceed) {
        (void)memset_s(resCmd, sizeof(*resCmd), 0, sizeof(*resCmd));
    }
}

static void DoStart(const char* cmdContent)
{
    StartServiceByName(cmdContent);
}

static void DoMkDir(const char* cmdContent)
{
    mode_t mode = DEFAULT_DIR_MODE;
    if (mkdir(cmdContent, mode) != 0) {
        if (errno != EEXIST) {
            printf("[Init] DoMkDir, failed for %s, err %d.\n", cmdContent, errno);
        }
    }
}

static void DoChmod(const char* cmdContent)
{
    // format: 0xxx /xxx/xxx/xxx
    if (cmdContent[0] != '0' || cmdContent[MODE_LEN] != ' ' || strlen(cmdContent) <= MODE_LEN + 1) {
        printf("[Init] DoChmod, bad format for %s.\n", cmdContent);
        return;
    }

    for (size_t i = 1; i < MODE_LEN; ++i) {
        if (cmdContent[i] > '7' || cmdContent[i] < '0') {
            printf("[Init] DoChmod, bad mode format for %s.\n", cmdContent);
            return;
        }
    }

    const char* pathBeginStr = cmdContent + MODE_LEN + 1;  // after space
    mode_t mode = strtoul(cmdContent, NULL, OCTAL_TYPE);
    if (mode == 0) {
        printf("[Init] DoChmod, strtoul failed for %s, er %d.\n", cmdContent, errno);
        return;
    }

    if (chmod(pathBeginStr, mode) != 0) {
        printf("[Init] DoChmod, failed for %s, err %d.\n", cmdContent, errno);
    }
}

static void DoChown(const char* cmdContent)
{
    // format: owner group /xxx/xxx/xxx
    size_t firstSpace = 0;
    size_t secondSpace = 0;
    size_t strLen = strlen(cmdContent);
    for (size_t i = 0; i < strLen; ++i) {
        if (cmdContent[i] == ' ') {
            if (i == 0) {
                printf("[Init] DoChown, bad format for %s.\n", cmdContent);
                return;
            }
            if (firstSpace == 0) {
                firstSpace = i;
            } else {
                secondSpace = i;
                break;
            }
        }
    }

    if (secondSpace <= firstSpace || firstSpace + 1 == secondSpace || secondSpace == strLen - 1) {
        printf("[Init] DoChown, bad format for %s.\n", cmdContent);
        return;
    }

    // only numbers valid
    for (size_t i = 0; i < secondSpace; ++i) {
        if (i != firstSpace && !isdigit(cmdContent[i])) {
            printf("[Init] DoChown, bad format for %s.\n", cmdContent);
            return;
        }
    }

    uid_t owner = strtoul(cmdContent, NULL, 0);
    const char* groupBegin = cmdContent + firstSpace + 1;
    gid_t group = strtoul(groupBegin, NULL, 0);
    const char *path = cmdContent + secondSpace + 1;
    if (chown(path, owner, group) != 0) {
        printf("[Init] DoChown, failed for %s, err %d.\n", cmdContent, errno);
    }
}

static char* CopySubStr(const char* srcStr, size_t startPos, size_t endPos)
{
    if (endPos <= startPos) {
        printf("[Init] DoMount, invalid params<%zu, %zu> for %s.\n", endPos, startPos, srcStr);
        return NULL;
    }

    size_t mallocLen = endPos - startPos + 1;
    char* retStr = (char*)malloc(mallocLen);
    if (retStr == NULL) {
        printf("[Init] DoMount, malloc failed! malloc size %zu, for %s.\n", mallocLen, srcStr);
        return NULL;
    }

    const char* copyStart = srcStr + startPos;
    if (memcpy_s(retStr, mallocLen, copyStart, endPos - startPos) != EOK) {
        printf("[Init] DoMount, memcpy_s failed for %s.\n", srcStr);
        free(retStr);
        return NULL;
    }
    retStr[mallocLen - 1] = '\0';

    // for example, source may be none
    if (strncmp(retStr, "none", strlen("none")) == 0) {
        retStr[0] = '\0';
    }
    return retStr;
}

static int GetMountFlag(unsigned long* mountflags, const char* targetStr)
{
    if (targetStr == NULL) {
        return 0;
    }

    if (strncmp(targetStr, "nodev", strlen("nodev")) == 0) {
        (*mountflags) |= MS_NODEV;
    } else if (strncmp(targetStr, "noexec", strlen("noexec")) == 0) {
        (*mountflags) |= MS_NOEXEC;
    } else if (strncmp(targetStr, "nosuid", strlen("nosuid")) == 0) {
        (*mountflags) |= MS_NOSUID;
    } else if (strncmp(targetStr, "rdonly", strlen("rdonly")) == 0) {
        (*mountflags) |= MS_RDONLY;
    } else {
        return 0;
    }
    return 1;
}

static int CountSpaces(const char* cmdContent, size_t* spaceCnt, size_t* spacePosArr, size_t spacePosArrLen)
{
    *spaceCnt = 0;
    size_t strLen = strlen(cmdContent);
    for (size_t i = 0; i < strLen; ++i) {
        if (cmdContent[i] == ' ') {
            ++(*spaceCnt);
            if ((*spaceCnt) > spacePosArrLen) {
                printf("[Init] DoMount, too many spaces, bad format for %s.\n", cmdContent);
                return 0;
            }
            spacePosArr[(*spaceCnt) - 1] = i;
        }
    }

    if ((*spaceCnt) < SPACES_CNT_IN_CMD_MIN ||           // spaces count should not less than 2(at least 3 items)
        spacePosArr[0] == 0 ||                           // should not start with space
        spacePosArr[(*spaceCnt) - 1] == strLen - 1) {    // should not end with space
        printf("[Init] DoMount, bad format for %s.\n", cmdContent);
        return 0;
    }

    // spaces should not be adjacent
    for (size_t i = 1; i < (*spaceCnt); ++i) {
        if (spacePosArr[i] == spacePosArr[i - 1] + 1) {
            printf("[Init] DoMount, bad format for %s.\n", cmdContent);
            return 0;
        }
    }
    return 1;
}

static void DoMount(const char* cmdContent)
{
    size_t spaceCnt = 0;
    size_t spacePosArr[SPACES_CNT_IN_CMD_MAX] = {0};
    if (!CountSpaces(cmdContent, &spaceCnt, spacePosArr, SPACES_CNT_IN_CMD_MAX)) {
        return;
    }

    // format: fileSystemType source target mountFlag1 mountFlag2... data
    unsigned long mountflags = 0;
    size_t strLen = strlen(cmdContent);
    size_t indexOffset = 0;
    char* fileSysType = CopySubStr(cmdContent, 0, spacePosArr[indexOffset]);
    if (fileSysType == NULL) {
        return;
    }

    char* source = CopySubStr(cmdContent, spacePosArr[indexOffset] + 1, spacePosArr[indexOffset + 1]);
    if (source == NULL) {
        free(fileSysType);
        return;
    }
    ++indexOffset;

    // maybe only has "filesystype source target", 2 spaces
    size_t targetEndPos = (indexOffset == spaceCnt - 1) ? strLen : spacePosArr[indexOffset + 1];
    char* target = CopySubStr(cmdContent, spacePosArr[indexOffset] + 1, targetEndPos);
    if (target == NULL) {
        free(fileSysType);
        free(source);
        return;
    }
    ++indexOffset;

    // get mountflags, if fail, the rest part of string will be data
    while (indexOffset < spaceCnt) {
        size_t tmpStrEndPos = (indexOffset == spaceCnt - 1) ? strLen : spacePosArr[indexOffset + 1];
        char* tmpStr = CopySubStr(cmdContent, spacePosArr[indexOffset] + 1, tmpStrEndPos);
        int ret = GetMountFlag(&mountflags, tmpStr);
        free(tmpStr);
        tmpStr = NULL;

        // get flag failed, the rest part of string will be data
        if (ret == 0) {
            break;
        }
        ++indexOffset;
    }

    int mountRet;
    if (indexOffset >= spaceCnt) {    // no data
        mountRet = mount(source, target, fileSysType, mountflags, NULL);
    } else {
        const char* dataStr = cmdContent + spacePosArr[indexOffset] + 1;
        mountRet = mount(source, target, fileSysType, mountflags, dataStr);
    }

    if (mountRet != 0) {
        printf("[Init] DoMount, failed for %s, err %d.\n", cmdContent, errno);
    }

    free(fileSysType);
    free(source);
    free(target);
}

#ifndef OHOS_LITE
#define OPTIONS_SIZE 128u
static void DoInsmodInternal(const char *fileName, char *secondPtr, char *restPtr, int flags)
{
    char options[OPTIONS_SIZE] = {0};
    if (flags == 0) { //  '-f' option
        if (restPtr != NULL && secondPtr != NULL) { // Reset arugments, combine then all.
            if (snprintf_s(options, sizeof(options), OPTIONS_SIZE -1, "%s %s", secondPtr, restPtr) == -1) {
                return;
            }
        } else if (secondPtr != NULL) {
            if (strncpy_s(options, OPTIONS_SIZE - 1, secondPtr, strlen(secondPtr)) != 0) {
                return;
            }
        }
    } else { // Only restPtr is option
        if (restPtr != NULL) {
            if (strncpy_s(options, OPTIONS_SIZE - 1, restPtr, strlen(restPtr)) != 0) {
                return;
            }
        }
    }
    if (!fileName) {
        return;
    }
    char *realPath = (char *)calloc(MAX_BUFFER, sizeof(char));
    if (realPath == NULL) {
        return;
    }
    realPath = realpath(fileName, realPath);
    int fd = open(realPath, O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
    if (fd < 0) {
        printf("[Init] failed to open %s: %d\n", realPath, errno);
        free(realPath);
        return;
    }
    int rc = syscall(__NR_finit_module, fd, options, flags);
    if (rc == -1) {
        printf("[Init] finit_module for %s failed: %d\n", realPath, errno);
    }
    if (fd >= 0) {
        close(fd);
    }
    free(realPath);
    return;
}

// format insmod <ko name> [-f] [options]
static void DoInsmod(const char *cmdContent)
{
    char *p = NULL;
    char *restPtr = NULL;
    char *fileName = NULL;
    char *line = NULL;
    int flags = 0;

    size_t count = strlen(cmdContent);
    if (count > OPTIONS_SIZE) {
        printf("[Init], options too long, maybe lost some of options\n");
    }
    line = (char *)malloc(count + 1);
    if (line == NULL) {
        printf("[Init] Allocate memory failed.\n");
        return;
    }

    if (memcpy_s(line, count, cmdContent, count) != EOK) {
        printf("[Init] memcpy failed\n");
        free(line);
        return;
    }
    line[count] = '\0';
    do {
        if ((p = strtok_r(line, " ", &restPtr)) == NULL) {
            printf("[Init] debug, cannot get filename\n");
            free(line);
            return;
        }
        fileName = p;
        printf("[Init] debug, fileName is [%s]\n", fileName);
        if ((p = strtok_r(NULL, " ", &restPtr)) == NULL) {
            break;
        }
        if (!strcmp(p, "-f")) {
            flags = MODULE_INIT_IGNORE_VERMAGIC | MODULE_INIT_IGNORE_MODVERSIONS;
        }
    } while (0);
    DoInsmodInternal(fileName, p, restPtr, flags);
    if (line != NULL) {
        free(line);
    }
    return;
}
#endif // OHOS_LITE

static bool CheckValidCfg(const char *path)
{
    size_t cfgCnt = sizeof(g_supportCfg) / sizeof(g_supportCfg[0]);
    struct stat fileStat = {0};

    if (stat(path, &fileStat) != 0 || fileStat.st_size <= 0 || fileStat.st_size > LOADCFG_MAX_FILE_LEN) {
        return false;
    }

    for (size_t i = 0; i < cfgCnt; ++i) {
        if (strcmp(path, g_supportCfg[i]) == 0) {
            return true;
        }
    }

    return false;
}

static void DoLoadCfg(const char *path)
{
    char buf[LOADCFG_BUF_SIZE] = {0};
    FILE *fp = NULL;
    size_t maxLoop = 0;
    CmdLine *cmdLine = NULL;
    int len;

    if (path == NULL) {
        return;
    }

    printf("[Init] DoLoadCfg cfg file %s\n", path);
    if (!CheckValidCfg(path)) {
        printf("[Init] CheckCfg file %s Failed\n", path);
        return;
    }

    fp = fopen(path, "r");
    if (fp == NULL) {
        printf("[Init] open cfg error = %d\n", errno);
        return;
    }

    cmdLine = (CmdLine *)malloc(sizeof(CmdLine));
    if (cmdLine == NULL) {
        printf("[Init] malloc cmdline error");
        fclose(fp);
        return;
    }

    while (fgets(buf, LOADCFG_BUF_SIZE, fp) != NULL && maxLoop < LOADCFG_MAX_LOOP) {
        maxLoop++;
        len = strlen(buf);
        if (len < 1) {
            continue;
        }
        if (buf[len - 1] == '\n') {
            buf[len - 1] = '\0'; // we replace '\n' with '\0'
        }
        (void)memset_s(cmdLine, sizeof(CmdLine), 0, sizeof(CmdLine));
        ParseCmdLine(buf, cmdLine);
        DoCmd(cmdLine);
        (void)memset_s(buf, sizeof(char) * LOADCFG_BUF_SIZE, 0, sizeof(char) * LOADCFG_BUF_SIZE);
    }

    free(cmdLine);
    fclose(fp);
}

void DoCmd(const CmdLine* curCmd)
{
    if (curCmd == NULL) {
        return;
    }

    if (strncmp(curCmd->name, "start ", strlen("start ")) == 0) {
        DoStart(curCmd->cmdContent);
    } else if (strncmp(curCmd->name, "mkdir ", strlen("mkdir ")) == 0) {
        DoMkDir(curCmd->cmdContent);
    } else if (strncmp(curCmd->name, "chmod ", strlen("chmod ")) == 0) {
        DoChmod(curCmd->cmdContent);
    } else if (strncmp(curCmd->name, "chown ", strlen("chown ")) == 0) {
        DoChown(curCmd->cmdContent);
    } else if (strncmp(curCmd->name, "mount ", strlen("mount ")) == 0) {
        DoMount(curCmd->cmdContent);
    } else if (strncmp(curCmd->name, "loadcfg ", strlen("loadcfg ")) == 0) {
        DoLoadCfg(curCmd->cmdContent);
#ifndef OHOS_LITE
    } else if (strncmp(curCmd->name, "insmod ", strlen("insmod ")) == 0) {
        DoInsmod(curCmd->cmdContent);
#endif
    } else {
        printf("[Init] DoCmd, unknown cmd name %s.\n", curCmd->name);
    }
}

