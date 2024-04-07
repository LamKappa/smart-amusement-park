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
#ifndef PKG_FILE_H
#define PKG_FILE_H

#include <map>
#include "pkg_algorithm.h"
#include "pkg_manager.h"
#include "pkg_utils.h"

namespace hpackage {
class PkgFile;
class PkgEntry;
using PkgEntryPtr = PkgEntry *;
using PkgFilePtr = PkgFile *;

class PkgEntry {
public:
    PkgEntry(PkgFilePtr pkgFile, uint32_t nodeId) : nodeId_(nodeId), pkgFile_(pkgFile) {}

    virtual ~PkgEntry() {}

    virtual int32_t Init(const PkgManager::FileInfoPtr fileInfo, PkgStreamPtr inStream) = 0;

    virtual int32_t EncodeHeader(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen) = 0;

    virtual int32_t Pack(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen) = 0;

    virtual int32_t DecodeHeader(const PkgBuffer &buffer, size_t headerOffset, size_t dataOffset,
        size_t &decodeLen) = 0;

    virtual int32_t Unpack(PkgStreamPtr outStream) = 0;

    virtual const std::string GetFileName() const
    {
        return fileName_;
    };

    virtual const FileInfo *GetFileInfo() const = 0;

    PkgFilePtr GetPkgFile() const
    {
        return pkgFile_;
    }

    uint32_t GetNodeId() const
    {
        return nodeId_;
    }
protected:
    int32_t Init(PkgManager::FileInfoPtr localFileInfo, const PkgManager::FileInfoPtr fileInfo,
        PkgStreamPtr inStream);

protected:
    uint32_t nodeId_ {0};
    PkgFilePtr pkgFile_ {nullptr};
    size_t headerOffset_ {0};
    size_t dataOffset_ {0};
    std::string fileName_ {};
};

class PkgFile {
public:
    enum PkgType {
        PKG_TYPE_NONE = PKG_PACK_TYPE_NONE,
        PKG_TYPE_UPGRADE = PKG_PACK_TYPE_UPGRADE, // 升级包
        PKG_TYPE_ZIP = PKG_PACK_TYPE_ZIP,     // zip压缩包
        PKG_TYPE_LZ4 = PKG_PACK_TYPE_LZ4,     // lz4压缩包
        PKG_TYPE_GZIP = PKG_PACK_TYPE_GZIP,     // gzip压缩包
        PKG_TYPE_MAX
    };
    using VerifyFunction = std::function<int(const PkgManager::PkgInfoPtr info,
        const std::vector<uint8_t> &digest, const std::vector<uint8_t> &signature)>;

public:
    PkgFile(PkgStreamPtr stream, PkgType type) : type_(type), pkgStream_(stream) {}

    virtual ~PkgFile();

    virtual int32_t AddEntry(const PkgManager::FileInfoPtr file, const PkgStreamPtr input) = 0;

    virtual int32_t SavePackage(size_t &signOffset) = 0;

    virtual int32_t ExtractFile(const PkgEntryPtr node, const PkgStreamPtr output);

    virtual int32_t LoadPackage(std::vector<std::string> &fileNames, VerifyFunction verifier = nullptr) = 0;

    PkgEntryPtr FindPkgEntry(const std::string &fileName);

    PkgStreamPtr GetPkgStream() const
    {
        return pkgStream_;
    }

    virtual const PkgInfo *GetPkgInfo() const
    {
        return nullptr;
    }

    PkgType GetPkgType() const
    {
        return type_;
    }

    void SetPkgStream()
    {
        pkgStream_ = nullptr;
    }

    static int32_t ConvertBufferToString(std::string &fileName, const PkgBuffer &buffer);

    static int32_t ConvertStringToBuffer(const std::string &fileName, const PkgBuffer &buffer, size_t &realLen);

    void AddSignData(uint8_t digestMethod, size_t currOffset, size_t &signOffset);
protected:
    PkgEntryPtr AddPkgEntry(const std::string& fileName);
    bool CheckState(std::vector<uint32_t> states, uint32_t state);
protected:
    enum {
        PKG_FILE_STATE_IDLE = 0,
        PKG_FILE_STATE_WORKING, // 打包数据的状态
        PKG_FILE_STATE_CLOSE
    };

    PkgType type_ {};
    PkgStreamPtr pkgStream_ = nullptr;
    uint32_t nodeId_ = 0;
    std::map<uint32_t, PkgEntryPtr> pkgEntryMapId_ {};
    std::multimap<std::string, PkgEntryPtr, std::greater<std::string>> pkgEntryMapFileName_ {};
    uint32_t state_ = PKG_FILE_STATE_IDLE;
};
} // namespace hpackage
#endif
