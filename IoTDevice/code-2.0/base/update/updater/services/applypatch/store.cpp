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
#include "applypatch/store.h"
#include <algorithm>
#include <cstdio>
#include <fcntl.h>
#include <limits>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <unistd.h>
#include "applypatch/transfer_manager.h"
#include "log/log.h"
#include "utils.h"

using namespace updater::utils;

namespace updater {
int32_t Store::DoFreeSpace(const std::string &directoryPath)
{
    std::vector<std::string> files;
    UPDATER_ERROR_CHECK(GetFilesFromDirectory(directoryPath, files, true) > 0,
                        "Failed to get files for free space", return -1);
    for (const auto &file : files) {
        UPDATER_ERROR_CHECK(DeleteFile(file.c_str()) != -1, "Failed to delete in do free space", continue);
    }
    return 0;
}

int32_t Store::FreeStore(const std::string &dirPath, const std::string &fileName)
{
    UPDATER_CHECK_ONLY_RETURN(!dirPath.empty(), return -1);
    std::string path;
    if (!fileName.empty()) {
        path = dirPath + "/";
    }
    UPDATER_CHECK_ONLY_RETURN(DeleteFile(path.c_str()) == -1, return 0);
    LOG(ERROR) << "Failed to delete " << path;
    return -1;
}

int32_t Store::CreateNewSpace(const std::string &path, bool needClear)
{
    UPDATER_ERROR_CHECK_NOT_RETURN(!path.empty(), "path is empty.");
    std::string dirPath = path + '/';
    struct stat fileStat {};
    LOG(INFO) << "Create dir " << dirPath;
    if (stat(dirPath.c_str(), &fileStat) == -1) {
        UPDATER_ERROR_CHECK(errno == ENOENT, "Create new space, failed to stat", return -1);
        UPDATER_ERROR_CHECK(MkdirRecursive(dirPath, S_DIR_PERMISSION) == 0, "Failed to make store", return -1);
        UPDATER_ERROR_CHECK(chown(dirPath.c_str(), S_USER_PERMISSION, S_USER_PERMISSION) == 0,
                             "Failed to chown store", return -1);
    } else {
        UPDATER_CHECK_ONLY_RETURN(needClear, return 0);
        std::vector<std::string> files;
        UPDATER_CHECK_ONLY_RETURN(GetFilesFromDirectory(dirPath, files) > 0, return -1);
        UPDATER_CHECK_ONLY_RETURN(!files.empty(), return 0);
        std::vector<std::string>::iterator iter = files.begin();
        while (iter != files.end()) {
            if (DeleteFile(*iter) == 0) {
                LOG(INFO) << "Delete " << *iter;
            }
            iter++;
        }
        files.clear();
    }
    return 0;
}

int32_t Store::WriteDataToStore(const std::string &dirPath, const std::string &fileName,
    const std::vector<uint8_t> &buffer, int size)
{
    UPDATER_CHECK_ONLY_RETURN(!dirPath.empty(), return -1);
    std::string pathTmp;
    if (!fileName.empty()) {
        pathTmp = dirPath + "/";
    }
    std::string path = pathTmp + fileName;
    pathTmp = pathTmp + fileName + ".tmp";

    int fd = open(pathTmp.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_OPEN_PERMISSION);
    UPDATER_ERROR_CHECK(fd != -1, "Failed to create store", return -1);
    UPDATER_ERROR_CHECK(fchown(fd, S_USER_PERMISSION, S_USER_PERMISSION) == 0,
        "Failed to chown", close(fd); return -1);
    LOG(INFO) << "Writing " << size << " blocks to " << path;
    if (!WriteFully(fd, const_cast<uint8_t *>(buffer.data()), size)) {
        UPDATER_CHECK_ONLY_RETURN(errno != EIO, close(fd); return 1);
        LOG(ERROR) << "Write to stash failed";
        close(fd);
        return -1;
    }
    UPDATER_ERROR_CHECK(fsync(fd) != -1, "Failed to fsync", close(fd); return -1);
    UPDATER_ERROR_CHECK(rename(pathTmp.c_str(), path.c_str()) != -1,
        "Failed rename store", close(fd); return -1);
    close(fd);
    int fdd = open(dirPath.c_str(), O_RDONLY | O_DIRECTORY);
    UPDATER_ERROR_CHECK(fdd != -1, "Failed to open", return -1);
    close(fdd);
    return 0;
}

int32_t Store::LoadDataFromStore(const std::string &dirPath, const std::string &fileName,
    std::vector<uint8_t> &buffer)
{
    LOG(INFO) << "Store base is " << dirPath << "/" << fileName;
    std::string path = dirPath;
    if (!fileName.empty()) {
        path = path + "/" + fileName;
    }
    struct stat fileStat {};
    UPDATER_WARING_CHECK(stat(path.c_str(), &fileStat) != -1, "Failed to stat", return -1);
    UPDATER_ERROR_CHECK((fileStat.st_size % H_BLOCK_SIZE) == 0, "Not multiple of block size 4096", return -1);
    int fd = open(path.c_str(), O_RDONLY);
    UPDATER_ERROR_CHECK(fd != -1, "Failed to create", return -1);
    buffer.resize(fileStat.st_size);
    UPDATER_ERROR_CHECK(ReadFully(fd, buffer.data(), fileStat.st_size), "Failed to read store data", return -1);
    return 0;
}
} // namespace updater
