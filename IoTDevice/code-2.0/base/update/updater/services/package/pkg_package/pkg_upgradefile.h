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
#ifndef UPGRADE_PKG_FILE_H
#define UPGRADE_PKG_FILE_H

#include <map>
#include "pkg_algorithm.h"
#include "pkg_manager.h"
#include "pkg_pkgfile.h"
#include "pkg_utils.h"

namespace hpackage {
struct __attribute__((packed)) PkgTlv {
    uint16_t type;
    uint16_t length;
};

struct __attribute__((packed)) UpgradePkgHeader {
    uint32_t pkgInfoLength; // UpgradePkgTime + UpgradeCompInfo + UPGRADE_RESERVE_LEN
    uint32_t updateFileVersion;
    uint8_t productUpdateId[64];
    uint8_t softwareVersion[64];
};

struct __attribute__((packed)) UpgradePkgTime {
    uint8_t date[16];
    uint8_t time[16];
};

struct __attribute__((packed)) UpgradeCompInfo {
    uint8_t address[16]; // L1 16
    uint16_t id;
    uint8_t resType;
    uint8_t flags;
    uint8_t type;
    uint8_t version[10];
    uint32_t size;
    uint32_t originalSize;
    uint8_t digest[DIGEST_MAX_LEN];
};

class UpgradeFileEntry : public PkgEntry {
public:
    UpgradeFileEntry(PkgFilePtr pkgFile, uint32_t nodeId) : PkgEntry(pkgFile, nodeId) {}

    ~UpgradeFileEntry() override {}

    int32_t Init(const PkgManager::FileInfoPtr fileInfo, PkgStreamPtr inStream) override;

    int32_t EncodeHeader(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen) override;

    int32_t Pack(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen) override;

    int32_t DecodeHeader(const PkgBuffer &buffer, size_t offset, size_t dataOffset,
        size_t &decodeLen) override;

    int32_t Unpack(PkgStreamPtr outStream) override;

    const FileInfo *GetFileInfo() const override
    {
        return &fileInfo_.fileInfo;
    }
private:
    ComponentInfo fileInfo_ {};
};

class UpgradePkgFile : public PkgFile {
public:
    UpgradePkgFile(PkgStreamPtr stream, PkgManager::PkgInfoPtr header) :
        PkgFile(stream, PkgFile::PKG_TYPE_UPGRADE)
    {
        if (header == nullptr || header->entryCount == 0) {
            return;
        }
        UpgradePkgInfo *info = (UpgradePkgInfo *)(header);
        pkgInfo_ = std::move(*info);
    }

    ~UpgradePkgFile() override {}

    int32_t AddEntry(const PkgManager::FileInfoPtr file, const PkgStreamPtr input) override;

    int32_t SavePackage(size_t &offset) override;

    int32_t LoadPackage(std::vector<std::string> &fileNames, VerifyFunction verify = nullptr) override;

    size_t GetUpgradeSignatureLen() const;

    size_t GetDigestLen() const;

    const PkgInfo *GetPkgInfo() const override
    {
        return &pkgInfo_.pkgInfo;
    }
private:
    int16_t GetPackageTlvType();
    int32_t ReadComponents(const PkgBuffer &buffer, size_t &parsedLen,
        DigestAlgorithm::DigestAlgorithmPtr algorithm, std::vector<std::string> &fileNames);

    int32_t ReadUpgradePkgHeader(const PkgBuffer &buffer, size_t &realLen,
        DigestAlgorithm::DigestAlgorithmPtr &algorithm);

private:
    UpgradePkgInfo pkgInfo_ {};
    size_t packedFileSize_ {0};
};
} // namespace hpackage
#endif
