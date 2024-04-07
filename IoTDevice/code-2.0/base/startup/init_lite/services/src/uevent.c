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

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/netlink.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/un.h>
#include <unistd.h>
#include "list.h"
#include "securec.h"

#define LINK_NUMBER 4
#define DEFAULT_DIR_MODE 0755
#define DEV_DRM 3
#define DEV_ONCRPC 6
#define DEV_ADSP 4
#define DEV_INPUT 5
#define DEV_MTD 3
#define DEV_SOUND 5
#define DEV_MISC 4
#define DEV_DEFAULT 4
#define DEV_PLAT_FORM 9
#define DEV_USB 4
#define DEV_GRAPHICS 8
#define EVENT_ACTION 7
#define EVENT_DEVPATH 8
#define EVENT_SYSTEM 10
#define EVENT_FIRMWARE 9
#define EVENT_MAJOR 6
#define EVENT_MINOR 6
#define EVENT_PARTN 6
#define EVENT_PART_NAME 9
#define EVENT_DEV_NAME 8
#define EVENT_BLOCK 5
#define EVENT_PLAT_FORM 8
#define TRIGGER_ADDR_SIZE 4
#define BASE_BUFFER_SIZE 1024
#define MAX_BUFFER 256
#define EVENT_MAX_BUFFER 1026
#define MAX_DEV_PATH 96
#define MINORS_GROUPS 128
#define SYS_LINK_NUMBER 2
#define MAX_DEVICE_LEN 64
#define DEFAULT_MODE 0000
#define DEVICE_SKIP 5
#define HANDLE_DEVICE_USB 3
#define DEFAULT_NO_AUTHORITY_MODE 0600

int g_ueventFD = -1;

struct Uevent {
    const char *action;
    const char *path;
    const char *subsystem;
    const char *firmware;
    const char *partitionName;
    const char *deviceName;
    int partitionNum;
    int major;
    int minor;
};

struct PlatformNode {
    char *name;
    char *path;
    size_t pathLen;
    struct ListNode list;
};

static struct ListNode g_platformNames = {
    .next = &g_platformNames,
    .prev = &g_platformNames,
};

const char *g_trigger = "/dev/.trigger_uevent";
static void HandleUevent();

static int UeventFD()
{
    return g_ueventFD;
}

static void DoTrigger(DIR *dir, const char *path, int32_t pathLen)
{
    if (pathLen < 0) {
        return;
    }
    struct dirent *dirent = NULL;
    char ueventPath[MAX_BUFFER];
    if (snprintf_s(ueventPath, sizeof(ueventPath), sizeof(ueventPath) - 1, "%s/uevent", path) == -1) {
        return;
    }
    int fd = open(ueventPath, O_WRONLY);
    if (fd >= 0) {
        write(fd, "add\n", TRIGGER_ADDR_SIZE);
        close(fd);
        HandleUevent();
    }

    while ((dirent = readdir(dir)) != NULL) {
        if (dirent->d_name[0] == '.' || dirent->d_type != DT_DIR) {
            continue;
        }
        char tmpPath[MAX_BUFFER];
        if (snprintf_s(tmpPath, sizeof(tmpPath), sizeof(tmpPath) - 1, "%s/%s", path, dirent->d_name) == -1) {
            continue;
        }

        DIR *dir2 = opendir(tmpPath);
        if (dir2) {
            DoTrigger(dir2, tmpPath, strlen(tmpPath));
            closedir(dir2);
        }
    }
}

void Trigger(const char *sysPath)
{
    DIR *dir = opendir(sysPath);
    if (dir) {
        DoTrigger(dir, sysPath, strlen(sysPath));
        closedir(dir);
    }
}

static void RetriggerUevent()
{
    if (access(g_trigger, F_OK) == 0) {
        printf("Skip trigger uevent, alread done\n");
        return;
    }
    Trigger("/sys/class");
    Trigger("/sys/block");
    Trigger("/sys/devices");
    int fd = open(g_trigger, O_WRONLY | O_CREAT | O_CLOEXEC, DEFAULT_MODE);
    if (fd >= 0) {
        close(fd);
    }
    printf("Re-trigger uevent done\n");
}

static void UeventSockInit()
{
    struct sockaddr_nl addr;
    int buffSize = MAX_BUFFER * BASE_BUFFER_SIZE;
    int on = 1;

    if (memset_s(&addr, sizeof(addr), 0, sizeof(addr)) != 0) {
        return;
    }
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = 0xffffffff;

    int sockfd = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_KOBJECT_UEVENT);
    if (sockfd < 0) {
        printf("Create socket failed. %d\n", errno);
        return;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUFFORCE, &buffSize, sizeof(buffSize));
    setsockopt(sockfd, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));

    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        printf("Bind socket failed. %d\n", errno);
        close(sockfd);
        return;
    }
    g_ueventFD = sockfd;
    fcntl(g_ueventFD, F_SETFD, FD_CLOEXEC);
    fcntl(g_ueventFD, F_SETFL, O_NONBLOCK);
    RetriggerUevent();
    return;
}

ssize_t ReadUevent(int fd, void *buf, size_t len)
{
    struct iovec iov = { buf, len };
    struct sockaddr_nl addr;
    char control[CMSG_SPACE(sizeof(struct ucred))];
    struct msghdr hdr = {
        &addr,
        sizeof(addr),
        &iov,
        1,
        control,
        sizeof(control),
        0,
    };
    ssize_t n = recvmsg(fd, &hdr, 0);
    if (n <= 0) {
        return n;
    }
    struct cmsghdr *cMsg = CMSG_FIRSTHDR(&hdr);
    if (cMsg == NULL || cMsg->cmsg_type != SCM_CREDENTIALS) {
        bzero(buf, len);
        errno = -EIO;
        return n;
    }
    struct ucred *cRed = (struct ucred *)CMSG_DATA(cMsg);
    if (cRed->uid != 0) {
        bzero(buf, len);
        errno = -EIO;
        return n;
    }

    if (addr.nl_groups == 0 || addr.nl_pid != 0) {
    /* ignoring non-kernel or unicast netlink message */
        bzero(buf, len);
        errno = -EIO;
        return n;
    }

    return n;
}

static void InitUevent(struct Uevent *event)
{
    event->action = "";
    event->path = "";
    event->subsystem = "";
    event->firmware = "";
    event->partitionName = "";
    event->deviceName = "";
    event->partitionNum = -1;
}

static void ParseUevent(const char *buf, struct Uevent *event)
{
    InitUevent(event);
    while (*buf) {
        if (strncmp(buf, "ACTION=", EVENT_ACTION) == 0) {
            buf += EVENT_ACTION;
            event->action = buf;
        } else if (strncmp(buf, "DEVPATH=", EVENT_DEVPATH) == 0) {
            buf += EVENT_DEVPATH;
            event->path = buf;
        } else if (strncmp(buf, "SUBSYSTEM=", EVENT_SYSTEM) == 0) {
            buf += EVENT_SYSTEM;
            event->subsystem = buf;
        } else if (strncmp(buf, "FIRMWARE=", EVENT_FIRMWARE) == 0) {
            buf += EVENT_FIRMWARE;
            event->firmware = buf;
        } else if (strncmp(buf, "MAJOR=", EVENT_MAJOR) == 0) {
            buf += EVENT_MAJOR;
            event->major = atoi(buf);
        } else if (strncmp(buf, "MINOR=", EVENT_MINOR) == 0) {
            buf += EVENT_MINOR;
            event->minor = atoi(buf);
        } else if (strncmp(buf, "PARTN=", EVENT_PARTN) == 0) {
            buf += EVENT_PARTN;
            event->partitionNum = atoi(buf);
        } else if (strncmp(buf, "PARTNAME=", EVENT_PART_NAME) == 0) {
            buf += EVENT_PART_NAME;
            event->partitionName = buf;
        } else if (strncmp(buf, "DEVNAME=", EVENT_DEV_NAME) == 0) {
            buf += EVENT_DEV_NAME;
            event->deviceName = buf;
        }
        // Drop reset.
        while (*buf++) {}
    }
}

static int MakeDir(const char *path, mode_t mode)
{
    int rc = mkdir(path, mode);
    if (rc < 0 && errno != EEXIST) {
        printf("Create %s failed. %d\n", path, errno);
    }
    return rc;
}

static struct PlatformNode *FindPlatformDevice(const char *path)
{
    size_t pathLen = strlen(path);
    struct ListNode *node = NULL;
    struct PlatformNode *bus = NULL;

    for (node = (&g_platformNames)->prev; node != &g_platformNames; node = node->prev) {
        bus = (struct PlatformNode *)(((char*)(node)) - offsetof(struct PlatformNode, list));
        if ((bus->pathLen < pathLen) && (path[bus->pathLen] == '/') && !strncmp(path, bus->path, bus->pathLen)) {
            return bus;
        }
    }
    return NULL;
}

static void CheckValidPartitionName(char *partitionName)
{
    const char* supportPartition = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-";
    if (!partitionName) {
        return;
    }

    for (size_t i = 0; i < strlen(partitionName); i++) {
        int len = strspn(partitionName, supportPartition);
        partitionName += len;
        i = len;
        if (*partitionName) {
            *partitionName = '_';
        }
    }
}

static char **ParsePlatformBlockDevice(const struct Uevent *uevent)
{
    const char *device;
    char *slash = NULL;
    const char *type;
    char linkPath[MAX_BUFFER];
    int linkNum = 0;
    char *p = NULL;

    struct PlatformNode *pDev = FindPlatformDevice(uevent->path);
    if (!pDev) {
        return NULL;
    }
    device = pDev->name;
    type = "platform";
    char **links = calloc(sizeof(char *), LINK_NUMBER);
    if (!links) {
        return NULL;
    }
    if (snprintf_s(linkPath, sizeof(linkPath), sizeof(linkPath) - 1, "/dev/block/%s/%s", type, device) == -1) {
        free(links);
        return NULL;
    }
    if (uevent->partitionName) {
        p = strdup(uevent->partitionName);
        CheckValidPartitionName(p);
        if (strcmp(uevent->partitionName, p)) {
            printf("Linking partition '%s' as '%s'\n", uevent->partitionName, p);
        }
        if (asprintf(&links[linkNum], "%s/by-name/%s", linkPath, p) > 0) {
            linkNum++;
        } else {
            links[linkNum] = NULL;
        }
        free(p);
    }
    if (uevent->partitionNum >= 0) {
        if (asprintf(&links[linkNum], "%s/by-num/p%d", linkPath, uevent->partitionNum) > 0) {
            linkNum++;
        } else {
            links[linkNum] = NULL;
        }
    }
    slash = strrchr(uevent->path, '/');
    if (asprintf(&links[linkNum], "%s/%s", linkPath, slash + 1) > 0) {
        linkNum++;
    } else {
        links[linkNum] = NULL;
    }
    return links;
}

static void MakeDevice(const char *devpath, const char *path, int block, int major, int minor)
{
    /* Only for super user */
    gid_t gid = 0;
    dev_t dev;
    mode_t mode = DEFAULT_NO_AUTHORITY_MODE;
    mode |= (block ? S_IFBLK : S_IFCHR);
    dev = makedev(major, minor);
    setegid(gid);
    if (mknod(devpath, mode, dev) != 0) {
        if (errno != EEXIST) {
            printf("Make device node[%d, %d] failed. %d\n", major, minor, errno);
        }
    }
}

int MkdirRecursive(const char *pathName, mode_t mode)
{
    char buf[128];
    const char *slash;
    const char *p = pathName;
    struct stat info;

    while ((slash = strchr(p, '/')) != NULL) {
        int width = slash - pathName;
        p = slash + 1;
        if (width < 0) {
            break;
        }
        if (width == 0) {
            continue;
        }
        if ((unsigned int)width > sizeof(buf) - 1) {
            printf("path too long for MkdirRecursive\n");
            return -1;
        }
        if (memcpy_s(buf, width, pathName, width) != 0) {
            return -1;
        }
        buf[width] = 0;
        if (stat(buf, &info) != 0) {
            int ret = MakeDir(buf, mode);
            if (ret && errno != EEXIST) {
                return ret;
            }
        }
    }
    int ret = MakeDir(pathName, mode);
    if (ret && errno != EEXIST) {
        return ret;
    }
    return 0;
}

void RemoveLink(const char *oldpath, const char *newpath)
{
    char path[MAX_BUFFER];
    ssize_t ret = readlink(newpath, path, sizeof(path) - 1);
    if (ret <= 0) {
        return;
    }
    path[ret] = 0;
    if (!strcmp(path, oldpath)) {
        unlink(newpath);
    }
}

static void MakeLink(const char *oldPath, const char *newPath)
{
    char buf[MAX_BUFFER];
    char *slash = strrchr(newPath, '/');
    if (!slash) {
        return;
    }
    int width = slash - newPath;
    if (width <= 0 || width > (int)sizeof(buf) - 1) {
        return;
    }
    if (memcpy_s(buf, sizeof(buf), newPath, width) != 0) {
        return;
    }
    buf[width] = 0;
    int ret = MkdirRecursive(buf, DEFAULT_DIR_MODE);
    if (ret) {
        printf("Failed to create directory %s: %s (%d)\n", buf, strerror(errno), errno);
    }
    ret = symlink(oldPath, newPath);
    if (ret && errno != EEXIST) {
        printf("Failed to symlink %s to %s: %s (%d)\n", oldPath, newPath, strerror(errno), errno);
    }
}

static void HandleDevice(struct Uevent *event, const char *devpath, int block, char **links)
{
    int i;
    if (!strcmp(event->action, "add")) {
        MakeDevice(devpath, event->path, block, event->major, event->minor);
        if (links) {
            for (i = 0; links[i]; i++) {
                MakeLink(devpath, links[i]);
            }
        }
    }

    if (!strcmp(event->action, "remove")) {
        if (links) {
            for (i = 0; links[i]; i++) {
                RemoveLink(devpath, links[i]);
            }
        }
        unlink(devpath);
    }

    if (links) {
        for (i = 0; links[i]; i++) {
            free(links[i]);
        }
        free(links);
    }
}

static void HandleBlockDevice(struct Uevent *event)
{
    const char *base = "/dev/block";
    char devpath[MAX_DEV_PATH];
    char **links = NULL;

    if (event->major < 0 || event->minor < 0) {
        return;
    }
    const char *name = strrchr(event->path, '/');
    if (name == NULL) {
        return;
    }
    name++;
    if (strlen(name) > MAX_DEVICE_LEN) { // too long
        return;
    }
    if (snprintf_s(devpath, sizeof(devpath), sizeof(devpath) - 1, "%s/%s", base, name) == -1) {
        return;
    }
    MakeDir(base, DEFAULT_DIR_MODE);
    if (!strncmp(event->path, "/devices/", DEV_PLAT_FORM)) {
        links = ParsePlatformBlockDevice(event);
    }
    HandleDevice(event, devpath, 1, links);
}

static void AddPlatformDevice(const char *path)
{
    size_t pathLen = strlen(path);
    const char *name = path;

    if (!strncmp(path, "/devices/", DEV_PLAT_FORM)) {
        name += DEV_PLAT_FORM;
        if (!strncmp(name, "platform/", DEV_PLAT_FORM)) {
            name += DEV_PLAT_FORM;
        }
    }
    printf("adding platform device %s (%s)\n", name, path);
    struct PlatformNode *bus = calloc(1, sizeof(struct PlatformNode));
    if (!bus) {
        return;
    }
    bus->path = strdup(path);
    bus->pathLen = pathLen;
    bus->name = bus->path + (name - path);
    ListAddTail(&g_platformNames, &bus->list);
    return;
}

static void RemovePlatformDevice(const char *path)
{
    struct ListNode *node = NULL;
    struct PlatformNode *bus = NULL;

    for (node = (&g_platformNames)->prev; node != &g_platformNames; node = node->prev) {
        bus = (struct PlatformNode *)(((char*)(node)) - offsetof(struct PlatformNode, list));
        if (!strcmp(path, bus->path)) {
            printf("removing platform device %s\n", bus->name);
            free(bus->path);
            ListRemove(node);
            free(bus);
            return;
        }
    }
}

static void HandlePlatformDevice(const struct Uevent *event)
{
    const char *path = event->path;
    if (strcmp(event->action, "add") == 0) {
        AddPlatformDevice(path);
    } else if (strcmp(event->action, "remove") == 0) {
        RemovePlatformDevice(path);
    }
}

static const char *ParseDeviceName(const struct Uevent *uevent, unsigned int len)
{
    /* if it's not a /dev device, nothing else to do */
    if ((uevent->major < 0) || (uevent->minor < 0)) {
        return NULL;
    }
    /* do we have a name? */
    const char *name = strrchr(uevent->path, '/');
    if (!name) {
        return NULL;
    }
    name++;
    /* too-long names would overrun our buffer */
    if (strlen(name) > len) {
        return NULL;
    }
    return name;
}

static char **GetCharacterDeviceSymlinks(const struct Uevent *uevent)
{
    char *slash = NULL;
    int linkNum = 0;
    int width;

    struct PlatformNode *pDev = FindPlatformDevice(uevent->path);
    if (!pDev) {
        return NULL;
    }
    char **links = calloc(sizeof(char *), SYS_LINK_NUMBER);
    if (!links) {
        return NULL;
    }

    /* skip "/devices/platform/<driver>" */
    const char *parent = strchr(uevent->path + pDev->pathLen, '/');
    if (!*parent) {
        goto err;
    }

    if (strncmp(parent, "/usb", DEV_USB)) {
        goto err;
    }
    /* skip root hub name and device. use device interface */
    if (!*parent) {
        goto err;
    }
    slash = strchr(++parent, '/');
    if (!slash) {
        goto err;
    }
    width = slash - parent;
    if (width <= 0) {
        goto err;
    }

    if (asprintf(&links[linkNum], "/dev/usb/%s%.*s", uevent->subsystem, width, parent) > 0) {
        linkNum++;
    } else {
        links[linkNum] = NULL;
    }
    mkdir("/dev/usb", DEFAULT_DIR_MODE);
    return links;
err:
    free(links);
    return NULL;
}

static int HandleUsbDevice(const struct Uevent *event, char *devpath, int len)
{
    if (event->deviceName) {
        /*
         * create device node provided by kernel if present
         * see drivers/base/core.c
         */
        char *p = devpath;
        if (snprintf_s(devpath, len, len - 1, "/dev/%s", event->deviceName) == -1) {
            return -1;
        }
        /* skip leading /dev/ */
        p += DEVICE_SKIP;
        /* build directories */
        while (*p) {
            if (*p == '/') {
                *p = 0;
                MakeDir(devpath, DEFAULT_DIR_MODE);
                *p = '/';
            }
            p++;
        }
    } else {
        /* This imitates the file system that would be created
         * if we were using devfs instead.
         * Minors are broken up into groups of 128, starting at "001"
         */
        int busId  = event->minor / MINORS_GROUPS + 1;
        int deviceId = event->minor % MINORS_GROUPS + 1;
        /* build directories */
        MakeDir("/dev/bus", DEFAULT_DIR_MODE);
        MakeDir("/dev/bus/usb", DEFAULT_DIR_MODE);
        if (snprintf_s(devpath, len, len - 1, "/dev/bus/usb/%03d", busId) == -1) {
            return -1;
        }
        MakeDir(devpath, DEFAULT_DIR_MODE);
        if (snprintf_s(devpath, len, len - 1, "/dev/bus/usb/%03d/%03d", busId,
            deviceId) == -1) {
            return -1;
        }
    }
    return 0;
}

static void HandleDeviceEvent(struct Uevent *event, char *devpath, int len, const char *base, const char *name)
{
    char **links = NULL;
    links = GetCharacterDeviceSymlinks(event);
    if (!devpath[0]) {
        if (snprintf_s(devpath, len, len - 1, "%s%s", base, name) == -1) {
            printf("[Init] snprintf_s err \n");
            goto err;
        }
    }
    HandleDevice(event, devpath, 0, links);
    return;
err:
    if (links) {
        for (int i = 0; links[i]; i++) {
            free(links[i]);
        }
        free(links);
    }
    return;
}
static void HandleGenericDevice(struct Uevent *event)
{
    char *base = NULL;
    char devpath[MAX_DEV_PATH] = {0};
    const char *name = ParseDeviceName(event, MAX_DEVICE_LEN);
    if (!name) {
        return;
    }
    if (!strncmp(event->subsystem, "usb", HANDLE_DEVICE_USB)) {
        if (!strcmp(event->subsystem, "usb")) {
            if (HandleUsbDevice(event, devpath, MAX_DEV_PATH) == -1) {
                return;
            }
        } else {
            /* ignore other USB events */
            return;
        }
    } else if (!strncmp(event->subsystem, "graphics", DEV_GRAPHICS)) {
        base = "/dev/graphics/";
        MakeDir(base, DEFAULT_DIR_MODE);
    } else if (!strncmp(event->subsystem, "drm", DEV_DRM)) {
        base = "/dev/dri/";
        MakeDir(base, DEFAULT_DIR_MODE);
    } else if (!strncmp(event->subsystem, "oncrpc", DEV_ONCRPC)) {
        base = "/dev/oncrpc/";
        MakeDir(base, DEFAULT_DIR_MODE);
    } else if (!strncmp(event->subsystem, "adsp", DEV_ADSP)) {
        base = "/dev/adsp/";
        MakeDir(base, DEFAULT_DIR_MODE);
    } else if (!strncmp(event->subsystem, "input", DEV_INPUT)) {
        base = "/dev/input/";
        MakeDir(base, DEFAULT_DIR_MODE);
    } else if (!strncmp(event->subsystem, "mtd", DEV_MTD)) {
        base = "/dev/mtd/";
        MakeDir(base, DEFAULT_DIR_MODE);
    } else if (!strncmp(event->subsystem, "sound", DEV_SOUND)) {
        base = "/dev/snd/";
        MakeDir(base, DEFAULT_DIR_MODE);
    } else {
        base = "/dev/";
    }
    HandleDeviceEvent(event, devpath, MAX_DEV_PATH, base, name);
    return;
}

static void HandleDeviceUevent(struct Uevent *event)
{
    if (strcmp(event->action, "add") == 0 || strcmp(event->action, "change") == 0) {
        /* Do nothing for now */
    }
    if (strncmp(event->subsystem, "block", EVENT_BLOCK) == 0) {
        HandleBlockDevice(event);
    } else if (strncmp(event->subsystem, "platform", EVENT_PLAT_FORM) == 0) {
        HandlePlatformDevice(event);
    } else {
        HandleGenericDevice(event);
    }
}

static void HandleUevent()
{
    char buf[EVENT_MAX_BUFFER];
    int ret;
    struct Uevent event;
    while ((ret = ReadUevent(g_ueventFD, buf, BASE_BUFFER_SIZE)) > 0) {
        if (ret >= BASE_BUFFER_SIZE) {
            continue;
        }
        buf[ret] = '\0';
        buf[ret + 1] = '\0';
        ParseUevent(buf, &event);
        HandleDeviceUevent(&event);
    }
}

void UeventInit()
{
    struct pollfd ufd;
    UeventSockInit();
    ufd.events = POLLIN;
    ufd.fd = UeventFD();
    while (1) {
        ufd.revents = 0;
        int ret = poll(&ufd, 1, -1);
        if (ret <= 0) {
            continue;
        }
        if (ufd.revents & POLLIN) {
            HandleUevent();
        }
    }
    return;
}

int main(const int argc, const char **argv)
{
    printf("Uevent demo starting...\n");
    UeventInit();
    return 0;
}
