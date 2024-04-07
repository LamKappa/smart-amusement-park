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
#include "packages_info.h"
#include <cstring>
#include <iostream>
#include "log.h"
#include "pkg_manager.h"
#include "pkg_manager_impl.h"
#include "pkg_pkgfile.h"
#include "pkg_stream.h"
#include "pkg_upgradefile.h"
#include "pkg_utils.h"

using namespace std;
using namespace hpackage;

namespace updater {
vector<string> SplitString(const string &str, const string &pattern)
{
    vector<string> ret;
    if (pattern.empty()) {
        return ret;
    }
    size_t start = 0;
    size_t index = str.find_first_of(pattern, 0);
    while (index != str.npos) {
        if (start != index) {
            string tmpStr = str.substr(start, index - start);
            tmpStr.erase(0, tmpStr.find_first_not_of(" "));
            tmpStr.erase(tmpStr.find_last_not_of(" ") + 1);
            tmpStr.erase(tmpStr.find_last_not_of("\r") + 1);
            ret.push_back(tmpStr);
        }
        start = index + 1;
        index = str.find_first_of(pattern, start);
    }

    if (!str.substr(start).empty()) {
        string tmpStr = str.substr(start);
        tmpStr.erase(0, tmpStr.find_first_not_of(" "));
        tmpStr.erase(tmpStr.find_last_not_of(" ") + 1);
        tmpStr.erase(tmpStr.find_last_not_of("\r") + 1);
        ret.push_back(tmpStr);
    }
    return ret;
}

static PackagesInfoPtr g_packagesInfoInstance = nullptr;
PackagesInfoPtr PackagesInfo::GetPackagesInfoInstance()
{
    if (g_packagesInfoInstance == nullptr) {
        g_packagesInfoInstance = new PackagesInfo();
    }
    return g_packagesInfoInstance;
}

void PackagesInfo::ReleasePackagesInfoInstance(PackagesInfoPtr pkginfomanager)
{
    delete pkginfomanager;
    g_packagesInfoInstance = nullptr;
}

std::vector<std::string> PackagesInfo::GetOTAVersion(hpackage::PkgManager::PkgManagerPtr manager,
    const std::string &versionList, const std::string &versionPath)
{
    vector<string> tmpVersionList;
    PKG_CHECK(manager != nullptr, return tmpVersionList, "manager is null");

    PkgManager::StreamPtr outStream = nullptr;
    const string pattern = "\n";
    const FileInfo *info = manager->GetFileInfo(versionList);
    PKG_CHECK(info != nullptr, return tmpVersionList, "Can not get file info %s", versionList.c_str());
    int32_t ret = manager->CreatePkgStream(outStream, versionList, info->unpackedSize,
        PkgStream::PkgStreamType_MemoryMap);
    PKG_CHECK(outStream, return tmpVersionList, "Create stream fail %s", versionList.c_str());
    ret = manager->ExtractFile(versionList, outStream);
    PKG_CHECK(ret == PKG_SUCCESS, manager->ClosePkgStream(outStream);
        return tmpVersionList, "ExtractFile versionList fail ");
    PkgBuffer data = {};
    ret = outStream->GetBuffer(data);
    PKG_CHECK(ret == PKG_SUCCESS, manager->ClosePkgStream(outStream);
        return tmpVersionList, "ExtractFile fail %s", versionList.c_str());
    std::string str(reinterpret_cast<char*>(data.buffer), data.length);
    tmpVersionList = SplitString(str, pattern);
    manager->ClosePkgStream(outStream);
    return tmpVersionList;
}

std::vector<std::string> PackagesInfo::GetBoardID(hpackage::PkgManager::PkgManagerPtr manager,
    const std::string &boardIdName, const std::string &boardListPath)
{
    vector<string> boardlist;
    PKG_CHECK(manager != nullptr, return boardlist, "manager is null");
    (void)boardListPath;
    PkgManager::StreamPtr outStream = nullptr;
    const FileInfo *info = manager->GetFileInfo(boardIdName);
    PKG_CHECK(info != nullptr, return boardlist, "Can not get file info %s", boardIdName.c_str());
    int32_t ret = manager->CreatePkgStream(outStream, boardIdName, info->unpackedSize,
        PkgStream::PkgStreamType_MemoryMap);
    PKG_CHECK(outStream, return boardlist, "Create stream fail %s", boardIdName.c_str());
    ret = manager->ExtractFile(boardIdName, outStream);
    PkgBuffer data = {};
    ret = outStream->GetBuffer(data);
    PKG_CHECK(ret == PKG_SUCCESS, manager->ClosePkgStream(outStream);
        return boardlist, "ExtractFile fail %s", boardIdName.c_str());
    std::string str(reinterpret_cast<char*>(data.buffer), data.length);
    boardlist = SplitString(str, "\n");
    manager->ClosePkgStream(outStream);
    return boardlist;
}

bool PackagesInfo::IsAllowRollback()
{
    return false;
}
} // namespace updater
