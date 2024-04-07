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
#include "fs_manager/fstab.h"
#include <cctype>
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>
#include <sys/mount.h>
#include <unistd.h>
#include <vector>
#include "fs_manager/fstab_api.h"
#include "log/log.h"
#include "utils.h"

namespace updater {
using utils::SplitString;

static std::map<std::string, int> g_fsManagerMap = {
    {"check", FS_MANAGER_CHECK},
    {"wait", FS_MANAGER_WAIT},
};

void ParseFstabPerLine(std::string &line, Fstab &fstab, bool &parseError)
{
    const std::string separator = " \t";
    char *restPtr = nullptr;
    struct FstabItem item;
    char *p = nullptr;
    do {
        if ((p = strtok_r(const_cast<char *>(line.c_str()), separator.c_str(), &restPtr)) == nullptr) {
            fprintf(stderr, "Failed to parse block device.\n");
            break;
        }

        item.deviceName = p;
        if ((p = strtok_r(nullptr, separator.c_str(), &restPtr)) == nullptr) {
            fprintf(stderr, "Failed to parse mount point.\n");
            break;
        }

        item.mountPoint = p;
        if ((p = strtok_r(nullptr, separator.c_str(), &restPtr)) == nullptr) {
            fprintf(stderr, "Failed to parse fs type.\n");
            break;
        }
        item.fsType = p;

        if ((p = strtok_r(nullptr, separator.c_str(), &restPtr)) == nullptr) {
            fprintf(stderr, "Failed to parse mount options.\n");
            break;
        }
        item.mountOptions = p;

        if ((p = strtok_r(nullptr, separator.c_str(), &restPtr)) == nullptr) {
            fprintf(stderr, "Failed to parse fs manager flags.\n");
            break;
        }

        std::string fsFlags = p;
        for (const auto& flag : g_fsManagerMap) {
            if (flag.first == p) {
                item.fsManagerFlags |= flag.second;
            }
        }

        fstab.emplace_back(std::move(item));
        parseError = false;
    } while (0);

    return;
}

bool ReadFstabFromFile(const std::string &fstabFile, Fstab &fstab)
{
    char *line = nullptr;
    size_t allocLen = 0;
    ssize_t readLen = 0;
    std::string tmpStr = "";
    Fstab tmpTab;
    auto fp = std::unique_ptr<FILE, decltype(&fclose)>(fopen(fstabFile.c_str(), "r"), fclose);
    if (fp.get() == nullptr) {
        LOG(ERROR) << "fp is nullptr";
        return false;
    }

    while ((readLen = getline(&line, &allocLen, fp.get())) != -1) {
        char *p = nullptr;
        if (line[readLen - 1] == '\n') {
            line[readLen - 1] = '\0';
        }
        p = line;
        while (isspace(*p)) {
            p++;
        }

        if (*p == '\0' || *p == '#') {
            continue;
        }

        bool parseError = true;
        tmpStr = p;
        ParseFstabPerLine(tmpStr, tmpTab, parseError);
        tmpStr.clear();
        if (parseError) {
            free(line);
            return false;
        }
    }

    free(line);
    fstab = std::move(tmpTab);
    return true;
}

struct FstabItem* FindFstabItemForMountPoint(Fstab &fstab, const std::string& mp)
{
    for (auto &item : fstab) {
        if (item.mountPoint == mp) {
            return &item;
        }
    }
    return nullptr;
}

struct FstabItem* FindFstabItemForPath(Fstab &fstab, const std::string &path)
{
    struct FstabItem *item = nullptr;
    std::string tmp(path);
    if (path.empty()) {
        return nullptr;
    }
    while (true) {
        item = FindFstabItemForMountPoint(fstab, tmp);
        if (item != nullptr) {
            return item;
        }
        auto sep = tmp.find_last_of('/');
        if (sep == std::string::npos) {
            return nullptr;
        }
        if (sep == 0) {
            tmp = "/";
            break;
        }
        tmp = tmp.substr(0, sep);
    }
    return nullptr;
}

static bool ParseDefaultMountFlags(const std::string &mountFlag, unsigned long &flags)
{
    std::map<std::string, unsigned long> mountFlags = {
        { "noatime", MS_NOATIME },
        { "noexec", MS_NOEXEC },
        { "nosuid", MS_NOSUID },
        { "nodev", MS_NODEV },
        { "nodiratime", MS_NODIRATIME },
        { "ro", MS_RDONLY },
        { "rw", 0 },
        { "sync", MS_SYNCHRONOUS },
        { "remount", MS_REMOUNT },
        { "bind", MS_BIND },
        { "rec", MS_REC },
        { "unbindable", MS_UNBINDABLE },
        { "private", MS_PRIVATE },
        { "slave", MS_SLAVE },
        { "shared", MS_SHARED },
        { "defaults", 0 },
    };
    for (const auto &flag : mountFlags) {
        if (mountFlag == flag.first) {
            flags = flag.second;
            return true;
        }
    }
    return false;
}

unsigned long GetMountFlags(const std::string &mountOptions, std::string &fsSpecificOptions)
{
    unsigned long flags = 0;
    unsigned long tmpFlag = 0;

    auto options = SplitString(mountOptions, ",");
    fsSpecificOptions.clear();
    for (const auto &option : options) {
        if (ParseDefaultMountFlags(option, tmpFlag)) {
            flags |= tmpFlag;
        } else { // not default mount flags, maybe File system specific.
            fsSpecificOptions.append(option);
            fsSpecificOptions.append(",");
        }
    }
    fsSpecificOptions.pop_back(); // Remove last ','
    LOG(DEBUG) << "File system specific mount options is " << fsSpecificOptions;
    return flags;
}
} // updater
