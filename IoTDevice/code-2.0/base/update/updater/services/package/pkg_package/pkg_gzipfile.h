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
#ifndef GZIP_PKG_FILE_H
#define GZIP_PKG_FILE_H

#include "pkg_pkgfile.h"
#include "pkg_utils.h"
#include "pkg_zipfile.h"
#include "zlib.h"

namespace hpackage {
struct __attribute__((packed)) GZipHeader {
    uint16_t magic = 0;
    uint8_t method = 0;
    uint8_t flags = 0;
    uint32_t mtime = 0;
    uint8_t xfl = 0;
    uint8_t osFile = 0;
};

struct __attribute__((packed)) GZipExtra {
    uint8_t si1 = 0;
    uint8_t si2 = 0;
    uint16_t xlen = 0;
};

class GZipFileEntry : public ZipFileEntry {
public:
    GZipFileEntry(PkgFilePtr pkgFile, uint32_t nodeId) : ZipFileEntry(pkgFile, nodeId) {}

    ~GZipFileEntry() override {}

    int32_t EncodeHeader(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen) override;

    int32_t Pack(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen) override;

    int32_t Unpack(PkgStreamPtr outStream) override;

    int32_t DecodeHeader(const PkgBuffer &buffer, size_t, size_t, size_t &decodeLen) override;
private:
    void DecodeHeaderCalOffset(uint8_t flags, const PkgBuffer &buffer, size_t &offset,
        std::string &fileName) const;
};

class GZipPkgFile : public PkgFile {
public:
    explicit GZipPkgFile(PkgStreamPtr stream) : PkgFile(stream, PkgFile::PKG_TYPE_GZIP)
    {
        pkgInfo_.signMethod = PKG_SIGN_METHOD_RSA;
        pkgInfo_.digestMethod = PKG_DIGEST_TYPE_SHA256;
    }

    ~GZipPkgFile() override {}

    int32_t AddEntry(const PkgManager::FileInfoPtr file, const PkgStreamPtr inStream) override;

    int32_t SavePackage(size_t &offset) override;

    int32_t LoadPackage(std::vector<std::string> &fileNames, VerifyFunction verifier = nullptr) override;
private:
    PkgInfo pkgInfo_ {};
    size_t currentOffset_ = 0;
};
} // namespace hpackage
#endif // GZIP_PKG_FILE_H
