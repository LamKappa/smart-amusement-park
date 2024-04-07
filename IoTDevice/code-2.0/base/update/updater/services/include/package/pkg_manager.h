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

#ifndef PKG_MANAGER_H
#define PKG_MANAGER_H

#include <cstdlib>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>
#include "package/package.h"

namespace hpackage {
/**
 * Error code definition
 */
enum {
    PKG_SUCCESS = 0,
    PKG_ERROR_BASE = 100,
    PKG_INVALID_NAME,
    PKG_INVALID_PARAM,
    PKG_INVALID_FILE,
    PKG_INVALID_SIGNATURE,
    PKG_INVALID_PKG_FORMAT,
    PKG_INVALID_ALGORITHM,
    PKG_INVALID_DIGEST,
    PKG_INVALID_STREAM,
    PKG_INVALID_VERSION,
    PKG_INVALID_STATE,
    PKG_INVALID_LZ4,
    PKG_NONE_PERMISSION,
    PKG_NONE_MEMORY,
    PKG_VERIFY_FAIL,
};

/**
 * Package information
 */
struct PkgInfo {
    uint32_t entryCount = 0;
    uint8_t signMethod;
    uint8_t digestMethod;
    uint8_t pkgType;
    uint8_t pkgFlags;
};

/**
 * File information
 */
struct FileInfo {
    uint8_t flags = 0;
    uint8_t digestMethod = 0;
    uint16_t packMethod = 0;
    time_t modifiedTime = 0;
    size_t packedSize = 0;
    size_t unpackedSize = 0;
    size_t headerOffset = 0;
    size_t dataOffset = 0;
    std::string identity;
};

/**
 * Header information of the update package
 */
struct UpgradePkgInfo {
    PkgInfo pkgInfo;
    uint32_t updateFileVersion = 0;
    std::string productUpdateId;
    std::string softwareVersion;
    std::string date;
    std::string time;
};

/**
 * Component information of the update package
 */
struct ComponentInfo {
    FileInfo fileInfo;
    std::string version;
    uint8_t digest[DIGEST_MAX_LEN];
    uint16_t id;
    uint8_t resType;
    uint8_t type;
    uint8_t compFlags;
    size_t originalSize;
};

/**
 * Lz4 file configuration information
 */
struct Lz4FileInfo {
    FileInfo fileInfo;
    int8_t compressionLevel;
    int8_t blockIndependence;
    int8_t contentChecksumFlag;
    int8_t blockSizeID;
    int8_t autoFlush = 1;
};

/**
 * Zip file configuration information
 */
struct ZipFileInfo {
    FileInfo fileInfo;
    int32_t method = -1; // The system automatically uses the default value if the value is -1.
    int32_t level;
    int32_t windowBits;
    int32_t memLevel;
    int32_t strategy;
};

/**
 * buff definition used for parsing
 */
struct PkgBuffer {
    uint8_t *buffer;
    size_t length = 0; // buffer size
    std::vector<uint8_t> data;

    PkgBuffer()
    {
        this->buffer = nullptr;
        this->length = 0;
    }

    PkgBuffer(uint8_t *buffer, size_t bufferSize)
    {
        this->buffer = buffer;
        this->length = bufferSize;
    }

    PkgBuffer(std::vector<uint8_t> &buffer)
    {
        this->buffer = buffer.data();
        this->length = buffer.capacity();
    }

    PkgBuffer(size_t bufferSize)
    {
        data.resize(bufferSize, 0);
        this->buffer = data.data();
        this->length = bufferSize;
    }
};

/**
 * Input and output stream definition
 */
class PkgStream {
public:
    enum {
        PkgStreamType_Read = 0,     // common file reading
        PkgStreamType_Write,        // common file writing (A new file is created and the original content is deleted.)
        PkgStreamType_MemoryMap,    // memory mapping
        PkgStreamType_Process,      // processing while parsing
        PkgStreamType_Buffer,       // buffer
    };

    virtual ~PkgStream() = default;

    /**
     * Read files.
     *
     * @param buff                  buffer to hold the output file content
     * @param start                 start position of reading
     * @param needRead              size of the data to read
     * @param readLen               length of the read data
     * @return                      file reading result
     */
    virtual int32_t Read(const PkgBuffer &data, size_t start, size_t needRead, size_t &readLen) = 0;

    virtual int32_t GetBuffer(PkgBuffer &buffer) const = 0;

    virtual size_t GetFileLength() = 0;
    virtual const std::string GetFileName() const = 0;
    virtual int32_t GetStreamType() const = 0;

    using ExtractFileProcessor = std::function<int(const PkgBuffer &data, size_t size, size_t start,
        bool isFinish, const void *context)>;

    int32_t GetBuffer(uint8_t *&buffer, size_t &size)
    {
        PkgBuffer data = {};
        int ret = GetBuffer(data);
        buffer = data.buffer;
        size = data.length;
        return ret;
    }
};

/**
 * Get a singleton PkgManager instance.
 */
class PkgManager {
public:
    using PkgManagerPtr = PkgManager *;
    using FileInfoPtr = FileInfo *;
    using PkgInfoPtr = PkgInfo *;
    using StreamPtr = PkgStream *;
    using VerifyCallback = std::function<void(int32_t result, uint32_t percent)>;

    virtual ~PkgManager() = default;

    static PkgManagerPtr CreatePackageInstance();
    static PkgManagerPtr GetPackageInstance();
    static void ReleasePackageInstance(PkgManagerPtr manager);

    /**
     * Create an update package based on specified parameters.
     *
     * @param path              path of the update package
     * @param header            header, which mainly consists of algorithm information
     * @param files             packed file list
     * @return                  packaging result, with the package saved as the file specified in path
     */
    virtual int32_t CreatePackage(const std::string &path, const std::string &keyName, PkgInfoPtr header,
        std::vector<std::pair<std::string, ZipFileInfo>> &files) = 0;
    virtual int32_t CreatePackage(const std::string &path, const std::string &keyName, PkgInfoPtr header,
        std::vector<std::pair<std::string, ComponentInfo>> &files) = 0;
    virtual int32_t CreatePackage(const std::string &path, const std::string &keyName, PkgInfoPtr header,
        std::vector<std::pair<std::string, Lz4FileInfo>> &files) = 0;
    virtual int32_t CreatePackage(const std::string &path, PkgInfoPtr header,
        std::vector<std::pair<std::string, ComponentInfo>> &files, size_t &offset, std::string &hashValue) = 0;

    /**
     * Verify the signature of the upgrade package.
     *
     * @param packagePath       file name of the update package
     * @param keyPath           file name of the key used for verification
     * @param version           version number of the update package to download
     * @param digest            digest value
     * @param size              digest value size
     * @return                  verification result
     */
    virtual int32_t VerifyPackage(const std::string &packagePath, const std::string &keyPath,
        const std::string &version, const PkgBuffer &digest, VerifyCallback cb) = 0;

    /**
     * Load and parse the update package.
     *
     * @param packagePath       file name of the update package
     * @param fileIds           returned file ID list
     * @param middleTofile      file saving mode during intermediate parsing.
     * @return                  loading and parsing result
     */
    virtual int32_t LoadPackage(const std::string &packagePath, const std::string &keyPath,
        std::vector<std::string> &fileIds) = 0;

    /**
     * Get the information about the update package.
     *
     * @param packagePath   file name of the update package
     * @return              information about the update package
     */
    virtual const PkgInfo *GetPackageInfo(const std::string &packagePath) = 0;

    /**
     * Extract files from the update package, parse the files, and verify the hash value.
     *
     * @param fileId        File ID, which is obtained from the fileIds returned by the LoadPackage function
     * @param output        output of the extracted files
     * @return              read operation result
     */
    virtual int32_t ExtractFile(const std::string &fileId, StreamPtr output) = 0;

    /**
     * Obtain information about the files in the update package.
     *
     * @param fileId        file ID
     * @return              file information
     */
    virtual const FileInfo *GetFileInfo(const std::string &fileId) = 0;

    /**
     * Create a a package stream to output.
     *
     * @param stream        stream for io management
     * @param fileName      file name corresponding to the stream
     * @param size          file size
     * @param type          stream type
     * @return              creation result; false if no access permission
     */
    virtual int32_t CreatePkgStream(StreamPtr &stream, const std::string &fileName, size_t size,
        int32_t type) = 0;

    /**
     * Create a package stream that can be processed while parsing.
     *
     * @param stream        stream used for io management
     * @param fileName      file name corresponding to the stream
     * @param processor     content processor
     * @param context       context for the processor
     * @return              creation result
     */
    virtual int32_t CreatePkgStream(StreamPtr &stream, const std::string &fileName,
        PkgStream::ExtractFileProcessor processor, const void *context) = 0;

    /**
     * Create a package stream that can be processed while parsing.
     *
     * @param stream        stream used for io management
     * @param fileName      file name corresponding to the stream
     * @param buffer        buffer
     * @return              creation result
     */
    virtual int32_t CreatePkgStream(StreamPtr &stream, const std::string &fileName, const PkgBuffer &buffer) = 0;

    /**
     * Close the stream
     *
     * @param stream  stream对象
     */
    virtual void ClosePkgStream(StreamPtr &stream) = 0;

    virtual int32_t DecompressBuffer(FileInfoPtr info, const PkgBuffer &buffer, StreamPtr output) const = 0;
    virtual int32_t CompressBuffer(FileInfoPtr info, const PkgBuffer &buffer, StreamPtr output) const = 0;

    virtual int32_t LoadPackageWithoutUnPack(const std::string &packagePath,
        std::vector<std::string> &fileIds) = 0;

    virtual int32_t ParsePackage(StreamPtr stream, std::vector<std::string> &fileIds, int32_t type) = 0;
};
} // namespace hpackage
#endif // PKG_MANAGER_H
