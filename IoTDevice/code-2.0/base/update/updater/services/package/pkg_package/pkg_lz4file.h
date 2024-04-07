/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LZ4_PKG_FILE_H
#define LZ4_PKG_FILE_H

#include "pkg_pkgfile.h"
#include "pkg_utils.h"

namespace hpackage {
class Lz4FileEntry : public PkgEntry {
public:
    Lz4FileEntry(PkgFilePtr pkgFile, uint32_t nodeId) : PkgEntry(pkgFile, nodeId)
    {
        fileInfo_.compressionLevel = 0;
        fileInfo_.blockIndependence = 0;
        fileInfo_.blockSizeID = 0;
        fileInfo_.contentChecksumFlag = 0;
    }

    ~Lz4FileEntry() override {}

    int32_t Init(const PkgManager::FileInfoPtr fileInfo, PkgStreamPtr inStream) override;

    const FileInfo *GetFileInfo() const override
    {
        return &fileInfo_.fileInfo;
    };

    int32_t EncodeHeader(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen) override;

    int32_t Pack(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen) override;

    int32_t Unpack(PkgStreamPtr outStream) override;

    int32_t DecodeHeader(const PkgBuffer &buffer, size_t, size_t, size_t &decodeLen) override;
private:
    Lz4FileInfo fileInfo_ {};
};

class Lz4PkgFile : public PkgFile {
public:
    explicit Lz4PkgFile(PkgStreamPtr stream) : PkgFile(stream, PkgFile::PKG_TYPE_LZ4)
    {
        pkgInfo_.signMethod = PKG_SIGN_METHOD_RSA;
        pkgInfo_.digestMethod = PKG_DIGEST_TYPE_SHA256;
    }
    ~Lz4PkgFile() override {}

    int32_t AddEntry(const PkgManager::FileInfoPtr file, const PkgStreamPtr inStream) override;

    int32_t SavePackage(size_t &offset) override;

    int32_t LoadPackage(std::vector<std::string> &fileNames, VerifyFunction verifier = nullptr) override;
private:
    PkgInfo pkgInfo_ {};
    size_t currentOffset_ {0};
};
} // namespace hpackage
#endif // LZ4_PKG_FILE_H