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
#include "pkg_pkgfile.h"
#include <ctime>
#include <limits>
#include <memory>
#include "pkg_gzipfile.h"
#include "pkg_lz4file.h"
#include "pkg_stream.h"
#include "pkg_upgradefile.h"
#include "pkg_utils.h"
#include "pkg_zipfile.h"

namespace hpackage {
PkgFile::~PkgFile()
{
    auto iter = pkgEntryMapId_.begin();
    while (iter != pkgEntryMapId_.end()) {
        auto entry = iter->second;
        delete entry;
        iter = pkgEntryMapId_.erase(iter);
    }
    pkgEntryMapId_.clear();
    pkgEntryMapFileName_.clear();
    PkgManager::StreamPtr tmpStream = pkgStream_;
    PkgManager::GetPackageInstance()->ClosePkgStream(tmpStream);
}

PkgEntryPtr PkgFile::AddPkgEntry(const std::string &fileName)
{
    uint32_t nodeId = ++nodeId_;
    PkgEntryPtr entry = nullptr;
    switch (type_) {
        case PKG_TYPE_UPGRADE:
            entry = new UpgradeFileEntry(this, nodeId);
            break;
        case PKG_TYPE_ZIP:
            entry = new ZipFileEntry(this, nodeId);
            break;
        case PKG_TYPE_LZ4: {
            entry = new Lz4FileEntry(this, nodeId);
            break;
        }
        case PKG_TYPE_GZIP: {
            entry = new GZipFileEntry(this, nodeId);
            break;
        }
        default:
            return nullptr;
    }
    pkgEntryMapId_.insert(std::pair<uint32_t, PkgEntryPtr>(nodeId, entry));
    pkgEntryMapFileName_.insert(std::pair<std::string, PkgEntryPtr>(fileName, entry));
    return entry;
}

int32_t PkgFile::ExtractFile(const PkgEntryPtr node, PkgStreamPtr output)
{
    PKG_LOGI("ExtractFile %s", output->GetFileName().c_str());
    PKG_CHECK(CheckState({PKG_FILE_STATE_WORKING}, PKG_FILE_STATE_WORKING),
        return PKG_INVALID_STATE, "error state curr %d ", state_);
    auto entry = static_cast<PkgEntryPtr>(node);
    PKG_CHECK(entry != nullptr, return PKG_INVALID_PARAM, "error get entry %s", pkgStream_->GetFileName().c_str());
    return entry->Unpack(output);
}

PkgEntryPtr PkgFile::FindPkgEntry(const std::string &fileName)
{
    PKG_CHECK(CheckState({PKG_FILE_STATE_WORKING}, PKG_FILE_STATE_WORKING), return nullptr,
        "error state curr %d ", state_);
    std::multimap<std::string, PkgEntryPtr>::iterator iter = pkgEntryMapFileName_.find(fileName);
    if (iter != pkgEntryMapFileName_.end()) {
        return (*iter).second;
    }
    return nullptr;
}

bool PkgFile::CheckState(std::vector<uint32_t> states, uint32_t state)
{
    bool ret = false;
    for (auto s : states) {
        if (state_ == s) {
            state_ = state;
            ret = true;
            break;
        }
    }
    return ret;
}

int32_t PkgFile::ConvertBufferToString(std::string &fileName, const PkgBuffer &buffer)
{
    for (uint32_t i = 0; i < buffer.length; ++i) {
        if (buffer.buffer[i] < 32 || buffer.buffer[i] >= 127) { // should be printable character
            break;
        }
        fileName.push_back(buffer.buffer[i]);
    }
    return PKG_SUCCESS;
}

int32_t PkgFile::ConvertStringToBuffer(const std::string &fileName, const PkgBuffer &buffer, size_t &realLen)
{
    PKG_CHECK(buffer.length >= fileName.size(), return PKG_INVALID_PARAM, "Invalid buffer");
    for (uint32_t i = 0; i < fileName.size(); ++i) {
        buffer.buffer[i] = static_cast<uint8_t>(fileName[i]);
        (realLen)++;
    }
    return PKG_SUCCESS;
}

int32_t PkgEntry::Init(PkgManager::FileInfoPtr localFileInfo, const PkgManager::FileInfoPtr fileInfo,
    PkgStreamPtr inStream)
{
    PKG_CHECK(localFileInfo != nullptr && fileInfo != nullptr && inStream != nullptr,
        return PKG_INVALID_PARAM, "Failed to check input param");

    fileName_.assign(inStream->GetFileName());
    localFileInfo->identity.assign(fileInfo->identity);
    localFileInfo->flags = fileInfo->flags;
    localFileInfo->digestMethod = fileInfo->digestMethod;
    localFileInfo->packMethod = fileInfo->packMethod;
    localFileInfo->modifiedTime = fileInfo->modifiedTime;
    localFileInfo->packedSize = fileInfo->packedSize;
    localFileInfo->unpackedSize = fileInfo->unpackedSize;

    // 填充file信息，默认值使用原始文件长度
    if (localFileInfo->unpackedSize == 0) {
        localFileInfo->unpackedSize = inStream->GetFileLength();
    }
    if (localFileInfo->packedSize == 0) {
        localFileInfo->packedSize = inStream->GetFileLength();
    }
    PKG_CHECK(localFileInfo->unpackedSize != 0, return PKG_INVALID_PARAM, "Failed to check unpackedSize = 0");
    if (localFileInfo->modifiedTime == 0) {
        time(&localFileInfo->modifiedTime);
    }
    return PKG_SUCCESS;
}

void PkgFile::AddSignData(uint8_t digestMethod, size_t currOffset, size_t &signOffset)
{
    signOffset = currOffset;
    if (digestMethod == PKG_DIGEST_TYPE_NONE) {
        return;
    } else if (digestMethod == PKG_DIGEST_TYPE_SHA384) {
        signOffset += SIGN_SHA256_LEN;
    }
    std::vector<uint8_t> buffer(SIGN_SHA256_LEN + SIGN_SHA384_LEN, 0);
    int32_t ret = pkgStream_->Write(buffer, buffer.size(), currOffset);
    PKG_CHECK(ret == PKG_SUCCESS, return, "Fail write sign for %s", pkgStream_->GetFileName().c_str());
    pkgStream_->Flush(currOffset + buffer.size());
    PKG_LOGI("SavePackage success file length: %zu signOffset %zu", pkgStream_->GetFileLength(), signOffset);
}
} // namespace hpackage
