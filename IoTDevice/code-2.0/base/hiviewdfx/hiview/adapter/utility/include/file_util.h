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

#ifndef UTILITY_FILE_UTIL_H
#define UTILITY_FILE_UTIL_H

#include <string>
#include <vector>

namespace OHOS {
namespace HiviewDFX {
namespace FileUtil {
// file_ex.h
bool LoadStringFromFile(const std::string& filePath, std::string& content);
bool SaveStringToFile(const std::string& filePath, const std::string& content, bool truncated = true);
bool SaveStringToFd(int fd, const std::string& content);
bool LoadBufferFromFile(const std::string& filePath, std::vector<char>& content);
bool SaveBufferToFile(const std::string& filePath, const std::vector<char>& content, bool truncated = true);
bool FileExists(const std::string& fileName);
bool FileExistsA(const std::string& fileName);
// directory_ex.h
std::string ExtractFilePath(const std::string& fileFullName);
std::string ExtractFileName(const std::string& fileFullName);
std::string IncludeTrailingPathDelimiter(const std::string& path);
std::string ExcludeTrailingPathDelimiter(const std::string& path);
void GetDirFiles(const std::string& path, std::vector<std::string>& files);
bool ForceCreateDirectory(const std::string& path);
bool ForceCreateDirectory(const std::string& path, int mode);
void RemoveFolderBeginWith(const std::string &path, const std::string &folderName);
bool ForceRemoveDirectory(const std::string& path, bool isNeedDeleteGivenDirSelf = true);
bool RemoveFile(const std::string& fileName);
uint64_t GetFolderSize(const std::string& path);
uint64_t GetFileSize(const std::string& path);
bool ChangeMode(const std::string& fileName, const mode_t& mode);
bool ChangeModeDirectory(const std::string& path, const mode_t& mode);
bool PathToRealPath(const std::string& path, std::string& realPath);
mode_t Umask(const mode_t& mode);
int Open(const std::string& path, const int flags, const mode_t mode);
void FormatPath2UnixStyle(std::string &path);
void CreateDirWithDefaultPerm(const std::string& path, uid_t aidRoot, uid_t aid_system);
} // namespace FileUtil
} // namespace HiviewDFX
} // namespace OHOS
#endif // UTILITY_FILE_UTIL_H