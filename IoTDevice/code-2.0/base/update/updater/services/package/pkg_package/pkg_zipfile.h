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

#ifndef ZIP_PKG_FILE_H
#define ZIP_PKG_FILE_H

#include <map>
#include "pkg_pkgfile.h"
#include "pkg_utils.h"

namespace hpackage {
// Local file header: descript in APPNOTE-6.3.4
//    local file header signature     4 bytes  (0x04034b50)
//    version needed to extract       2 bytes
//    general purpose bit flag        2 bytes
//    compression method              2 bytes
//    last mod file time              2 bytes
//    last mod file date              2 bytes
//    crc-32                          4 bytes
//    compressed size                 4 bytes
//    uncompressed size               4 bytes
//    file name length                2 bytes
//    extra field length              2 bytes
struct __attribute__((packed)) LocalFileHeader {
    uint32_t signature = 0;
    uint16_t versionNeeded = 0;
    uint16_t flags = 0;
    uint16_t compressionMethod = 0;
    uint16_t modifiedTime = 0;
    uint16_t modifiedDate = 0;
    uint32_t crc = 0;
    uint32_t compressedSize = 0;
    uint32_t uncompressedSize = 0;
    uint16_t nameSize = 0;
    uint16_t extraSize = 0;
};

// central file header
//    Central File header:
//    central file header signature   4 bytes  (0x02014b50)
//    version made by                 2 bytes
//    version needed to extract       2 bytes
//    general purpose bit flag        2 bytes
//    compression method              2 bytes
//    last mod file time              2 bytes
//    last mod file date              2 bytes
//    crc-32                          4 bytes
//    compressed size                 4 bytes
//    uncompressed size               4 bytes
//    file name length                2 bytes
//    extra field length              2 bytes
//    file comment length             2 bytes
//    disk number start               2 bytes
//    internal file attributes        2 bytes
//    external file attributes        4 bytes
//    relative offset of local header 4 bytes
struct __attribute__((packed)) CentralDirEntry {
    uint32_t signature = 0;
    uint16_t versionMade = 0;
    uint16_t versionNeeded = 0;
    uint16_t flags = 0; // general purpose bit flag
    uint16_t compressionMethod = 0;
    uint16_t modifiedTime = 0;
    uint16_t modifiedDate = 0;
    uint32_t crc = 0;
    uint32_t compressedSize = 0;
    uint32_t uncompressedSize = 0;
    uint16_t nameSize = 0;
    uint16_t extraSize = 0;
    uint16_t commentSize = 0;
    uint16_t diskNumStart = 0;
    uint16_t internalAttr = 0;
    uint32_t externalAttr = 0;
    uint32_t localHeaderOffset = 0;
};

struct __attribute__((packed)) EndCentralDir {
    uint32_t signature = 0;
    uint16_t numDisk = 0;
    uint16_t startDiskOfCentralDir = 0;
    uint16_t totalEntriesInThisDisk = 0;
    uint16_t totalEntries = 0;
    uint32_t sizeOfCentralDir = 0;
    uint32_t offset = 0;
    uint16_t commentLen = 0;
};

struct __attribute__((packed)) DataDescriptor {
    uint32_t signature = 0;
    uint32_t crc = 0;
    uint32_t compressedSize = 0;
    uint32_t uncompressedSize = 0;
};

struct __attribute__((packed)) Zip64EndCentralDirRecord {
    uint32_t signature = 0; // 0x06064b50
    uint64_t size = 0;
    uint16_t versionMadeBy = 0;
    uint16_t versionNeedToExtra = 0;
    uint32_t numberOfThisDisk = 0;
    uint32_t startOfthisDisk = 0;
    uint64_t totalEntriesInThisDisk = 0;
    uint64_t totalEntries = 0;
    uint64_t sizeOfCentralDir = 0;
    uint64_t offset = 0;
};

struct __attribute__((packed)) Zip64EndCentralDirLocator {
    uint32_t signature = 0; // 0x07064b50
    uint32_t numberOfDisk = 0;
    uint64_t endOfCentralDirectoryRecord = 0;
    uint32_t totalNumberOfDisks = 0;
};

class ZipFileEntry : public PkgEntry {
public:
    ZipFileEntry(PkgFilePtr pkgFile, uint32_t nodeId) : PkgEntry(pkgFile, nodeId) {}

    ~ZipFileEntry() override {}

    int32_t Init(const PkgManager::FileInfoPtr fileInfo, PkgStreamPtr inStream) override;

    const FileInfo *GetFileInfo() const override
    {
        return &fileInfo_.fileInfo;
    };

    int32_t EncodeHeader(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen) override;

    int32_t Pack(PkgStreamPtr inStream, size_t startOffset, size_t &encodeLen) override;

    int32_t Unpack(PkgStreamPtr outStream) override;

    int32_t DecodeHeader(const PkgBuffer &buffer, size_t headOffset, size_t dataOffset,
        size_t &decodeLen) override;

    int32_t EncodeCentralDirEntry(const PkgStreamPtr stream, size_t startOffset, size_t &encodeLen);

    int32_t DecodeCentralDirEntry(PkgStreamPtr inStream, PkgBuffer &buffer, size_t currentPos,
        size_t &decodeLen);
protected:
    ZipFileInfo fileInfo_ {};
    uint32_t crc32_ {0};
private:
    int32_t DecodeLocalFileHeaderCheck(PkgStreamPtr inStream, const PkgBuffer &data, size_t currentPos);

    int32_t DecodeLocalFileHeader(PkgStreamPtr inStream, const PkgBuffer &data, size_t currentPos,
        size_t &decodeLen);

    int32_t EncodeLocalFileHeader(uint8_t *buffer, size_t bufferLen, bool hasDataDesc, size_t nameLen);

    int32_t EncodeDataDescriptor(const PkgStreamPtr stream, size_t startOffset, uint32_t &encodeLen) const;

    void CombineTimeAndDate(time_t &time, uint16_t modifiedTime, uint16_t modifiedDate) const;
};

class ZipPkgFile : public PkgFile {
public:
    explicit ZipPkgFile(PkgStreamPtr stream) : PkgFile(stream, PkgFile::PKG_TYPE_ZIP)
    {
        pkgInfo_.signMethod = PKG_SIGN_METHOD_RSA;
        pkgInfo_.digestMethod = PKG_DIGEST_TYPE_SHA256;
    }
    ~ZipPkgFile() override {}

    int32_t AddEntry(const PkgManager::FileInfoPtr file, const PkgStreamPtr inStream) override;

    int32_t SavePackage(size_t &offset) override;

    int32_t LoadPackage(std::vector<std::string> &fileNames, VerifyFunction verifier = nullptr) override;
private:
    int32_t LoadPackage(std::vector<std::string> &fileNames, const PkgBuffer &buff,
        uint32_t endDirLen, size_t endDirPos, size_t &readLen);
    int32_t ParseFileEntries(std::vector<std::string> &fileNames, const EndCentralDir &endDir,
        size_t currentPos, size_t fileLen);
private:
    PkgInfo pkgInfo_ {};
    size_t currentOffset_ = 0;
};
} // namespace hpackage
#endif
