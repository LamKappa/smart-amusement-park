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

#ifndef IMAGE_PATCH_H
#define IMAGE_PATCH_H

#include <sys/types.h>
#include "deflate_adapter.h"
#include "diffpatch.h"
#include "openssl/sha.h"
#include "package/pkg_manager.h"
#include "securec.h"

namespace updatepatch {
class ImagePatch {
public:
    explicit ImagePatch(UpdatePatchWriterPtr writer) : writer_(writer) {}
    virtual ~ImagePatch() = default;

    virtual int32_t ApplyImagePatch(const PatchParam &param, size_t &startOffset) = 0;

    template<typename T>
    static T ReadLE(const uint8_t *address)
    {
        T result;
        memcpy_s(&result, sizeof(result), address, sizeof(T));
        return result;
    }
protected:
    UpdatePatchWriterPtr writer_ {nullptr};
};

class RowImagePatch : public ImagePatch {
public:
    explicit RowImagePatch(UpdatePatchWriterPtr writer) : ImagePatch(writer) {}
    ~RowImagePatch() override {}

    int32_t ApplyImagePatch(const PatchParam &param, size_t &startOffset) override;
};

class NormalImagePatch : public ImagePatch {
public:
    explicit NormalImagePatch(UpdatePatchWriterPtr writer) : ImagePatch(writer) {}
    ~NormalImagePatch() override {}

    int32_t ApplyImagePatch(const PatchParam &param, size_t &startOffset)  override;
};

class CompressedImagePatch : public ImagePatch {
public:
    CompressedImagePatch(UpdatePatchWriterPtr writer, const std::vector<uint8_t> &bonusData)
        : ImagePatch(writer), bonusData_(bonusData) {}
    ~CompressedImagePatch() override {}

    int32_t ApplyImagePatch(const PatchParam &param, size_t &startOffset) override;
protected:
    virtual int32_t ReadHeader(const PatchParam &param, PatchHeader &header, size_t &offset) = 0;
    virtual std::unique_ptr<hpackage::FileInfo> GetFileInfo() const = 0;
    int32_t DecompressData(hpackage::PkgBuffer buffer,
        hpackage::PkgManager::StreamPtr &stream, bool memory, size_t expandedLen) const;

    std::vector<uint8_t> bonusData_ {};
};

class ZipImagePatch : public CompressedImagePatch {
public:
    ZipImagePatch(UpdatePatchWriterPtr writer, const std::vector<uint8_t> &bonusData)
        : CompressedImagePatch(writer, bonusData) {}
    ~ZipImagePatch() override {}

protected:
    int32_t ReadHeader(const PatchParam &param, PatchHeader &header, size_t &offset) override;
    std::unique_ptr<hpackage::FileInfo> GetFileInfo() const override;

    int32_t method_ {0};
    int32_t level_ {0};
    int32_t windowBits_ {0};
    int32_t memLevel_ {0};
    int32_t strategy_ {0};
};

class GZipImagePatch : public ZipImagePatch {
public:
    GZipImagePatch(UpdatePatchWriterPtr writer, const std::vector<uint8_t> &bonusData)
        : ZipImagePatch(writer, bonusData) {}
    ~GZipImagePatch() override {}

protected:
    int32_t ReadHeader(const PatchParam &param, PatchHeader &header, size_t &offset) override;
};

class Lz4ImagePatch : public CompressedImagePatch {
public:
    Lz4ImagePatch(UpdatePatchWriterPtr writer, const std::vector<uint8_t> &bonusData)
        : CompressedImagePatch(writer, bonusData) {}
    ~Lz4ImagePatch() override {}

protected:
    int32_t ReadHeader(const PatchParam &param, PatchHeader &header, size_t &offset) override;
    std::unique_ptr<hpackage::FileInfo> GetFileInfo() const override;

    int32_t compressionLevel_ {0};
    int32_t blockIndependence_ {0};
    int32_t contentChecksumFlag_ {0};
    int32_t blockSizeID_ {0};
    int32_t method_ {0};
    int32_t autoFlush_ {1};
};

class CompressedFileRestore : public UpdatePatchWriter {
public:
    CompressedFileRestore(hpackage::PkgManager::FileInfoPtr fileInfo, UpdatePatchWriterPtr writer)
        : UpdatePatchWriter(), fileInfo_(fileInfo), writer_(writer) {}
    ~CompressedFileRestore() override {}

    int32_t Init() override;
    int32_t Write(size_t start, const BlockBuffer &buffer, size_t len) override;
    int32_t Finish() override
    {
        return 0;
    }
    int32_t CompressData(size_t &originalSize, size_t &compressSize);
private:
    std::vector<uint8_t> data_ {};
    size_t dataSize_ {0};
    hpackage::PkgManager::FileInfoPtr fileInfo_ { nullptr };
    UpdatePatchWriterPtr writer_ { nullptr };
    std::unique_ptr<DeflateAdapter> deflateAdapter_ { nullptr };
    SHA256_CTX sha256Ctx_ {};
};
} // namespace updater
#endif  // IMAGE_PATCH_H
