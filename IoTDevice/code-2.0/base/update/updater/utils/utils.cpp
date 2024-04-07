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

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <dirent.h>
#include <limits>
#include <string>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include "fs_manager/mount.h"
#include "log/log.h"
#include "misc_info/misc_info.h"
#include "securec.h"

namespace updater {
namespace utils {
constexpr uint32_t MAX_PATH_LEN = 256;
constexpr uint8_t SHIFT_RIGHT_FOUR_BITS = 4;
int32_t DeleteFile(const std::string& filename)
{
    UPDATER_ERROR_CHECK (!filename.empty(), "Invalid filename", return -1);
    if (unlink(filename.c_str()) == -1 && errno != ENOENT) {
        LOG(ERROR) << "unlink " << filename << " failed";
        return -1;
    }
    return 0;
}

int MkdirRecursive(const std::string &pathName, mode_t mode)
{
    size_t slashPos = 0;
    struct stat info {};
    while (true) {
        slashPos = pathName.find_first_of("/", slashPos);
        UPDATER_CHECK_ONLY_RETURN(slashPos != std::string::npos, break);
        if (slashPos == 0) {
            slashPos++;
            continue;
        }
        UPDATER_ERROR_CHECK(slashPos <= MAX_PATH_LEN, "path too long for mkdir", return -1);
        auto subDir = pathName.substr(0, slashPos);
        std::cout << "subDir : " << subDir << std::endl;
        if (stat(subDir.c_str(), &info) != 0) {
            int ret = mkdir(subDir.c_str(), mode);
            UPDATER_CHECK_ONLY_RETURN(!(ret && errno != EEXIST), return ret);
        }
        slashPos++;
    }
    int ret = mkdir(pathName.c_str(), mode);
    UPDATER_CHECK_ONLY_RETURN(!(ret && errno != EEXIST), return ret);
    return 0;
}

int64_t GetFilesFromDirectory(const std::string &path, std::vector<std::string> &files,
    bool isRecursive = false)
{
    struct stat sb {};
    UPDATER_ERROR_CHECK (stat(path.c_str(), &sb) != -1, "Failed to stat", return -1);
    DIR *dirp = opendir(path.c_str());
    struct dirent *dp;
    int64_t totalSize = 0;
    while ((dp = readdir(dirp)) != nullptr) {
        std::string fileName = path + "/" + dp->d_name;
        struct stat st {};
        if (stat(fileName.c_str(), &st) == 0) {
            std::string tmpName = dp->d_name;
            if (tmpName == "." || tmpName == "..") {
                continue;
            }
            if (isRecursive && S_ISDIR(st.st_mode)) {
                totalSize += GetFilesFromDirectory(fileName, files, isRecursive);
            }
            files.push_back(fileName);
            totalSize += st.st_size;
        }
    }
    closedir(dirp);

    return totalSize;
}

std::vector<std::string> SplitString(const std::string &str, const std::string del = " \t")
{
    std::vector<std::string> result;
    size_t found = std::string::npos;
    size_t start = 0;
    while (true) {
        found = str.find_first_of(del, start);
        result.push_back(str.substr(start, found - start));
        if (found == std::string::npos) {
            break;
        }
        start = found + 1;
    }
    return result;
}

std::string Trim(const std::string &str)
{
    if (str.empty()) {
        LOG(ERROR) << "str is empty";
        return str;
    }
    size_t start = 0;
    size_t end = str.size() - 1;
    while (start < str.size()) {
        if (!isspace(str[start])) {
            break;
        }
        start++;
    }
    while (start < end) {
        if (!isspace(str[end])) {
            break;
        }
        end--;
    }
    if (end < start) {
        return "";
    }
    return str.substr(start, end - start + 1);
}

std::string ConvertSha256Hex(const uint8_t* shaDigest, size_t length)
{
    const std::string hexChars = "0123456789abcdef";
    std::string haxSha256 = "";
    unsigned int c;
    for (size_t i = 0; i < length; ++i) {
        auto d = shaDigest[i];
        c = (d >> SHIFT_RIGHT_FOUR_BITS) & 0xf;     // last 4 bits
        haxSha256.push_back(hexChars[c]);
        haxSha256.push_back(hexChars[d & 0xf]);
    }
    return haxSha256;
}

void DoReboot(const std::string& rebootTarget)
{
    LOG(INFO) << ", rebootTarget: " << rebootTarget;
    static const int32_t maxCommandSize = 12;
    LoadFstab();
    auto miscBlockDevice = GetBlockDeviceByMountPoint("/misc");
    struct UpdateMessage msg;
    if (rebootTarget == "updater") {
        bool ret = ReadUpdaterMessage(miscBlockDevice, msg);
        UPDATER_ERROR_CHECK(ret == true, "DoReboot read misc failed", return);
        if (strcmp(msg.command, "boot_updater") != 0) {
            UPDATER_ERROR_CHECK(!memcpy_s(msg.command, maxCommandSize, "boot_updater", maxCommandSize),
                "Memcpy failed", return);
            msg.command[maxCommandSize] = 0;
        }
        ret = WriteUpdaterMessage(miscBlockDevice, msg);
        if (ret != true) {
            LOG(INFO) << "DoReboot: WriteUpdaterMessage boot_updater error";
            return;
        }
    } else {
        UPDATER_ERROR_CHECK(!memset_s(msg.command, MAX_COMMAND_SIZE, 0, MAX_COMMAND_SIZE), "Memset_s failed", return);
        bool ret = WriteUpdaterMessage(miscBlockDevice, msg);
        if (ret != true) {
            LOG(INFO) << "DoReboot: WriteUpdaterMessage empty error";
            return;
        }
    }
#ifndef UPDATER_UT
    syscall(__NR_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART2, rebootTarget.c_str());
#else
    return;
#endif
}

std::string GetCertName()
{
#ifndef UPDATER_UT
    static std::string signingCertName = "/certificate/signing_cert.crt";
#else
    static std::string signingCertName = "/data/updater/src/signing_cert.crt";
#endif
    return signingCertName;
}

bool WriteFully(int fd, const void *data, size_t size)
{
    ssize_t written = 0;
    size_t rest = size;

    auto p = reinterpret_cast<const uint8_t*>(data);
    while (rest > 0) {
        do {
            written = write(fd, p, rest);
        } while (written < 0 && errno == EINTR);

        if (written < 0) {
            return false;
        }
        p += written;
        rest -= written;
    }
    return true;
}

bool ReadFully(int fd, void *data, size_t size)
{
    auto p = reinterpret_cast<uint8_t *>(data);
    size_t remaining = size;
    while (remaining > 0) {
        ssize_t sread = read(fd, p, remaining);
        UPDATER_ERROR_CHECK (sread > 0, "Utils::ReadFully run error", return false);
        p += sread;
        remaining -= sread;
    }
    return true;
}

bool ReadFileToString(int fd, std::string &content)
{
    struct stat sb {};
    if (fstat(fd, &sb) != -1 && sb.st_size > 0) {
        content.resize(sb.st_size);
    }
    ssize_t n;
    size_t remaining = sb.st_size;
    auto p = reinterpret_cast<char *>(content.data());
    while (remaining > 0) {
        n = read(fd, p, remaining);
        UPDATER_CHECK_ONLY_RETURN (n > 0, return false);
        p += n;
        remaining -= n;
    }
    return true;
}

bool WriteStringToFile(int fd, const std::string& content)
{
    const char *p = content.data();
    size_t remaining = content.size();
    while (remaining > 0) {
        ssize_t n = write(fd, p, remaining);
        UPDATER_CHECK_ONLY_RETURN (n != -1, return false);
        p += n;
        remaining -= n;
    }
    return true;
}

std::string GetLocalBoardId()
{
    return "HI3516";
}
} // utils
} // namespace updater
