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

#include "installd/installd_operator.h"

#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "directory_ex.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_extractor.h"

namespace OHOS {
namespace AppExecFwk {
namespace {}  // namespace

bool InstalldOperator::IsExistFile(const std::string &path)
{
    if (path.empty()) {
        return false;
    }

    struct stat buf = {};
    if (stat(path.c_str(), &buf) != 0) {
        return false;
    }
    return S_ISREG(buf.st_mode);
}

bool InstalldOperator::IsExistDir(const std::string &path)
{
    if (path.empty()) {
        return false;
    }

    struct stat buf = {};
    if (stat(path.c_str(), &buf) != 0) {
        return false;
    }
    return S_ISDIR(buf.st_mode);
}

bool InstalldOperator::MkRecursiveDir(const std::string &path, bool isReadByOthers)
{
    if (!OHOS::ForceCreateDirectory(path)) {
        APP_LOGE("mkdir failed");
        return false;
    }
    mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH;
    mode |= (isReadByOthers ? S_IROTH : 0);
    return OHOS::ChangeModeDirectory(path, mode);
}

bool InstalldOperator::DeleteDir(const std::string &path)
{
    if (IsExistFile(path)) {
        return OHOS::RemoveFile(path);
    }
    if (IsExistDir(path)) {
        return OHOS::ForceRemoveDirectory(path);
    }
    return true;
}

bool InstalldOperator::ExtractFiles(const std::string &sourcePath, const std::string &targetPath)
{
    BundleExtractor extractor(sourcePath);
    if (!extractor.Init()) {
        return false;
    }
    std::vector<std::string> entryNames;
    if (!extractor.GetZipFileNames(entryNames)) {
        return false;
    }
    if (entryNames.empty()) {
        return false;
    }

    std::string targetDir = targetPath;
    if (targetPath.back() != Constants::PATH_SEPARATOR[0]) {
        targetDir = targetPath + Constants::PATH_SEPARATOR;
    }
    for (const auto &entryName : entryNames) {
        if (entryName.find("..") != std::string::npos) {
            return false;
        }
        if (entryName.back() == Constants::PATH_SEPARATOR[0]) {
            continue;
        }
        const std::string dir = GetPathDir(entryName);
        std::string filePath = targetDir + dir;
        if (!dir.empty()) {
            if (!MkRecursiveDir(filePath, true)) {
                return false;
            }
        }
        filePath = targetDir + entryName;
        if (!extractor.ExtractFile(entryName, filePath)) {
            return false;
        }
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        if (!OHOS::ChangeModeFile(filePath, mode)) {
            APP_LOGE("change mode failed");
        }
        filePath.clear();
    }
    return true;
}

bool InstalldOperator::RenameDir(const std::string &oldPath, const std::string &newPath)
{
    if (oldPath.empty() || oldPath.size() > PATH_MAX) {
        APP_LOGE("oldpath error");
        return false;
    }
    std::string realOldPath;
    realOldPath.reserve(PATH_MAX);
    realOldPath.resize(PATH_MAX - 1);
    if (realpath(oldPath.c_str(), &(realOldPath[0])) == nullptr) {
        APP_LOGE("realOldPath %{public}s", realOldPath.c_str());
        return false;
    }
    if (!(IsValideCodePath(realOldPath) && IsValideCodePath(newPath))) {
        APP_LOGE("IsValideCodePath failed");
        return false;
    }
    return RenameFile(realOldPath, newPath);
}

std::string InstalldOperator::GetPathDir(const std::string &path)
{
    std::size_t pos = path.rfind(Constants::PATH_SEPARATOR);
    if (pos == std::string::npos) {
        return std::string();
    }
    return path.substr(0, pos + 1);
}

bool InstalldOperator::ChangeFileAttr(const std::string &filePath, const int uid, const int gid)
{
    APP_LOGD("begin to change %{private}s file attribute", filePath.c_str());
    if (chown(filePath.c_str(), uid, gid) != 0) {
        APP_LOGE("fail to change %{private}s ownership", filePath.c_str());
        return false;
    }
    APP_LOGD("change %{private}s file attribute successfully", filePath.c_str());
    return true;
}

bool InstalldOperator::RenameFile(const std::string &oldPath, const std::string &newPath)
{
    if (oldPath.empty() || newPath.empty()) {
        return false;
    }
    if (!DeleteDir(newPath)) {
        return false;
    }
    return rename(oldPath.c_str(), newPath.c_str()) == 0;
}

bool InstalldOperator::IsValidPath(const std::string &rootDir, const std::string &path)
{
    if (rootDir.find(Constants::PATH_SEPARATOR) != 0 ||
        rootDir.rfind(Constants::PATH_SEPARATOR) != (rootDir.size() - 1) || rootDir.find("..") != std::string::npos) {
        return false;
    }
    if (path.find("..") != std::string::npos) {
        return false;
    }
    return path.compare(0, rootDir.size(), rootDir) == 0;
}

bool InstalldOperator::IsValideCodePath(const std::string &codePath)
{
    if (codePath.empty()) {
        return false;
    }
    return IsValidPath(Constants::THIRD_PARTY_APP_INSTALL_PATH + Constants::PATH_SEPARATOR, codePath) ||
           IsValidPath(Constants::SYSTEM_APP_INSTALL_PATH + Constants::PATH_SEPARATOR, codePath);
}

bool InstalldOperator::DeleteFiles(const std::string &dataPath)
{
    std::string subPath;
    bool ret = true;
    DIR *dir = opendir(dataPath.c_str());
    if (dir == nullptr) {
        return false;
    }
    while (true) {
        struct dirent *ptr = readdir(dir);
        if (ptr == nullptr) {
            break;
        }
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        subPath = OHOS::IncludeTrailingPathDelimiter(dataPath) + std::string(ptr->d_name);
        if (ptr->d_type == DT_DIR) {
            ret = OHOS::ForceRemoveDirectory(subPath);
        } else {
            if (access(subPath.c_str(), F_OK) == 0) {
                ret = OHOS::RemoveFile(subPath);
            }
        }
    }
    closedir(dir);
    return ret;
}

bool InstalldOperator::MkOwnerDir(const std::string &path, bool isReadByOthers, const int uid, const int gid)
{
    if (!MkRecursiveDir(path, isReadByOthers)) {
        return false;
    }
    return ChangeFileAttr(path, uid, gid);
}

}  // namespace AppExecFwk
}  // namespace OHOS