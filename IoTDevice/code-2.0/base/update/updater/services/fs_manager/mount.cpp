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
#include "fs_manager/mount.h"
#include <cerrno>
#include <map>
#include <string>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include "fs_manager/fstab.h"
#include "fs_manager/fstab_api.h"
#include "log/log.h"
#include "utils.h"

namespace updater {
using updater::utils::SplitString;
static std::string g_defaultUpdaterFstab = "";
static Fstab g_fstab;

static bool IsSupportedFilesystem(const std::string &fsType)
{
    std::vector<std::string> supportedFilesystems = {"ext4", "f2fs", "vfat"};

    bool supported = false;
    for (const auto &fs : supportedFilesystems) {
        if (fsType == fs) {
            supported = true;
            break;
        }
    }
    return supported;
}

static int ExecCommand(std::vector<std::string> cmds)
{
    std::vector<char *> extractedCmds;

    for (const auto &cmd : cmds) {
        extractedCmds.push_back(const_cast<char *>(cmd.c_str()));
    }
    extractedCmds.push_back(nullptr);
    pid_t pid = fork();
    if (pid < 0) {
        LOG(ERROR) << "Fork new process to format failed: " << errno;
        ERROR_CODE(CODE_FORK_FAIL);
        return -1;
    }
    if (pid == 0) {
        execv(extractedCmds[0], extractedCmds.data());
        exit(-1);
    }
    int status;
    waitpid(pid, &status, 0);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        LOG(ERROR) << "Command " << extractedCmds[0] << " failed with status " << WEXITSTATUS(status);
    }
    return WEXITSTATUS(status);
}

static int DoFormat(const std::string &devPath, const std::string &fsType)
{
    std::map<std::string, std::string> fsToolsMap = {
        { "ext4", "/system/bin/mke2fs" },
        { "f2fs", "/system/bin/make_f2fs" },
    };
    int ret = 0;
    auto it = fsToolsMap.find(fsType);
    if (it == fsToolsMap.end()) {
        LOG(ERROR) << "Can not find fs tools for " << fsType;
        return -1;
    }
    auto fsTool = it->second;
    if (fsType == "ext4") {
        constexpr int blockSize = 4096;
        std::vector<std::string> formatCmds;
        formatCmds.push_back(fsTool);
        formatCmds.push_back("-F");
        formatCmds.push_back("-t");
        formatCmds.push_back(fsType);
        formatCmds.push_back("-b");
        formatCmds.push_back(std::to_string(blockSize));
        formatCmds.push_back(devPath);
        ret = ExecCommand(formatCmds);
    } else if (fsType == "f2fs") {
        std::vector<std::string> formatCmds;
        formatCmds.push_back(fsTool);
        formatCmds.push_back(devPath);
        ret = ExecCommand(formatCmds);
    }
    return ret;
}

int FormatPartition(const std::string &path)
{
    struct FstabItem *item = FindFstabItemForPath(g_fstab, path);
    if (item == nullptr) {
        LOG(ERROR) << "Cannot find fstab item for " << path << " to format.";
        return -1;
    }

    if (item->mountPoint == "/") {
        /* Can not format root */
        return 0;
    }

    if (!IsSupportedFilesystem(item->fsType)) {
        LOG(ERROR) << "Try to format " << item->mountPoint << " with unsupported file system type: " << item->fsType;
        return -1;
    }

    // Umount first
    if (UmountForPath(path) != 0) {
        return -1;
    }
    int ret = DoFormat(item->deviceName, item->fsType);
    if (ret != 0) {
        LOG(ERROR) << "Format " << path << " failed";
    }
    return ((ret != 0) ? -1 : 0);
}

static MountStatus GetMountStatusForMountPoint(const std::string &mountPoint)
{
    char buffer[512];
    size_t n;
    constexpr size_t numMountItems = 6;
    const std::string mountFile = "/proc/mounts";
    auto fp = std::unique_ptr<FILE, decltype(&fclose)>(fopen(mountFile.c_str(), "r"), fclose);

    while (fgets(buffer, sizeof(buffer), fp.get()) != nullptr) {
        n = strlen(buffer);
        if (buffer[n - 1] == '\n') {
            buffer[n - 1] = '\0';
        }
        std::string line(buffer);
        std::vector<std::string> mountItems = SplitString(line);
        if (mountItems.size() == numMountItems) {
            // Second item in /proc/mounts is mount point
            if (mountItems[1] == mountPoint) {
                return MountStatus::MOUNT_MOUNTED;
            }
        }
    }

    // Cannot find it from system.
    return MountStatus::MOUNT_UMOUNTED;
}

MountStatus GetMountStatusForPath(const std::string &path)
{
    struct FstabItem *item = FindFstabItemForPath(g_fstab, path);
    if (item == nullptr) {
        return MountStatus::MOUNT_ERROR;
    }
    return GetMountStatusForMountPoint(item->mountPoint);
}

static std::string GetFstabFile()
{
    /* check vendor fstab files from specific directory */
    std::vector<const std::string> specificFstabFiles = {"/vendor/etc/fstab.updater", "/odm/etc/fstab.updater"};
    for (auto& fstabFile : specificFstabFiles) {
        if (access(fstabFile.c_str(), F_OK) == 0) {
            return fstabFile;
        }
    }
    return "";
}

void LoadFstab()
{
    std::string fstabFile = g_defaultUpdaterFstab;
    if (fstabFile.empty()) {
        fstabFile = GetFstabFile();
        if (fstabFile.empty()) {
            fstabFile = "/etc/fstab.updater";
        }
    }
    // Clear fstab before read fstab file.
    g_fstab.clear();
    if (ReadFstabFromFile(fstabFile, g_fstab) == false) {
        LOG(WARNING) << "Read " << fstabFile << " failed";
        return;
    }

    LOG(DEBUG) << "Updater filesystem config info:";
    for (const auto &item : g_fstab) {
        LOG(DEBUG) << "\tDevice: " << item.deviceName;
        LOG(DEBUG) << "\tMount point : " << item.mountPoint;
        LOG(DEBUG) << "\tFs type : " << item.fsType;
        LOG(DEBUG) << "\tMount options: " << item.mountOptions;
    }
}


void LoadSpecificFstab(const std::string &fstabName)
{
    g_defaultUpdaterFstab = fstabName;
    LoadFstab();
    g_defaultUpdaterFstab = "";
}

int UmountForPath(const std::string& path)
{
    struct FstabItem *item = FindFstabItemForPath(g_fstab, path);
    MountStatus rc;
    if (item == nullptr) {
        LOG(ERROR) << "Cannot find fstab item for " << path << " to umount.";
        return -1;
    }

    LOG(DEBUG) << "Umount for path " << path;
    rc = GetMountStatusForMountPoint(item->mountPoint);
    if (rc == MountStatus::MOUNT_ERROR) {
        return -1;
    } else if (rc == MountStatus::MOUNT_UMOUNTED) {
        return 0;
    } else {
        int ret = umount(item->mountPoint.c_str());
        if (ret == -1) {
            LOG(ERROR) << "Umount " << item->mountPoint << "failed: " << errno;
            return -1;
        }
    }
    return 0;
}

static int Mount(const std::string &source, const std::string &target, const std::string &fsType,
    unsigned long flags, const std::string &data)
{
    struct stat st {};
    int rc;
    if (stat(target.c_str(), &st) != 0 && errno != ENOENT) {
        LOG(ERROR) << "Cannot get " << target << " status: " << errno;
        return -1;
    }
    if ((st.st_mode & S_IFMT) == S_IFLNK) { // link, delete it.
        unlink(target.c_str());
    }
    mkdir(target.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    errno = 0;
    while ((rc = mount(source.c_str(), target.c_str(), fsType.c_str(), flags, data.c_str()) != 0)) {
        if (errno == EAGAIN) {
            LOG(INFO) << "Mount " << source << " to " << target << " failed. try again";
            continue;
        } else {
            break;
        }
    }
    return rc;
}

static int DoMount(const struct FstabItem &item)
{
    unsigned long mountFlags;
    std::string fsSpecificOptions;

    mountFlags = GetMountFlags(item.mountOptions, fsSpecificOptions);
    if (!IsSupportedFilesystem(item.fsType)) {
        LOG(ERROR) << "Unsupported file system \"" << item.fsType << "\"";
        return -1;
    }
    int rc = Mount(item.deviceName, item.mountPoint, item.fsType, mountFlags, fsSpecificOptions);
    if (rc != 0) {
        LOG(ERROR) << "Mount " << item.deviceName << " to " << item.mountPoint << " failed: " << errno;
    } else {
        LOG(INFO) << "Mount " << item.deviceName << " to " << item.mountPoint << " successful";
    }
    return rc;
}

int MountForPath(const std::string &path)
{
    struct FstabItem *item = FindFstabItemForPath(g_fstab, path);
    int ret;
    if (item == nullptr) {
        LOG(ERROR) << "Cannot find fstab item for " << path << " to mount.";
        return -1;
    }

    LOG(DEBUG) << "Mount for path " << path;
    MountStatus rc = GetMountStatusForMountPoint(item->mountPoint);
    if (rc == MountStatus::MOUNT_ERROR) {
        return -1;
    } else if (rc == MountStatus::MOUNT_MOUNTED) {
        std::cout << path << " already mounted\n";
        return 0;
    } else {
        ret = DoMount(*item);
    }
    return ret;
}

struct FstabItem *GetItemForMountPoint(const std::string &mountPoint)
{
    if (g_fstab.empty()) {
        LOG(ERROR) << "Fstab is empty.";
        return nullptr;
    }
    for (auto& item : g_fstab) {
        if (item.mountPoint == mountPoint) {
            return &item;
        }
    }
    return nullptr;
}

int SetupPartitions()
{
    if (g_fstab.empty()) {
        LOG(ERROR) << "Fstab is empty.";
        return -1;
    }
    for (const auto &item : g_fstab) {
        if (item.mountPoint == "/" || item.mountPoint == "/tmp" || item.fsType == "none" ||
            item.mountPoint == "/sdcard") {
            continue;
        }

        if (item.mountPoint == "/data") {
            if (MountForPath(item.mountPoint) != 0) {
                LOG(ERROR) << "Expected partition " << item.mountPoint << " is not mounted.";
                return -1;
            }
            continue;
        }
        if (UmountForPath(item.mountPoint) != 0) {
            LOG(ERROR) << "Umount " << item.mountPoint << " failed";
            return -1;
        }
    }
    return 0;
}

const std::string GetBlockDeviceByMountPoint(const std::string &mountPoint)
{
    std::string blockDevice = "";
    if (!mountPoint.empty()) {
        auto item = FindFstabItemForMountPoint(g_fstab, mountPoint);
        if (item) {
            blockDevice = item->deviceName;
        }
    }
    return blockDevice;
}
} // updater

