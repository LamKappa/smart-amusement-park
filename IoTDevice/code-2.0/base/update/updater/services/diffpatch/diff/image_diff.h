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

#ifndef IMGAE_DIFF_H
#define IMGAE_DIFF_H
#include <iostream>
#include <memory>
#include <vector>
#include "blocks_diff.h"
#include "update_diff.h"

namespace updatepatch {
struct ImageBlock {
    int32_t type;
    PatchBuffer newInfo;
    PatchBuffer oldInfo;
    size_t srcOriginalLength;
    size_t destOriginalLength;
    std::vector<uint8_t> patchData;
    std::vector<uint8_t> srcOriginalData;
    std::vector<uint8_t> destOriginalData;
};

class ImageDiff {
public:
    ImageDiff() = delete;
    ImageDiff(size_t limit, UpdateDiff::ImageParserPtr newParser, UpdateDiff::ImageParserPtr oldParser)
        : limit_(limit), newParser_(newParser), oldParser_(oldParser)  {}
    virtual ~ImageDiff()
    {
        updateBlocks_.clear();
    }

    virtual int32_t MakePatch(const std::string &patchName);
    virtual int32_t WriteHeader(std::ofstream &patchFile, size_t &offset, ImageBlock &block) const;
protected:
    int32_t SplitImage(const PatchBuffer &oldInfo, const PatchBuffer &newInfo);
    int32_t DiffImage(const std::string &patchName);

    size_t limit_;
    std::vector<ImageBlock> updateBlocks_ {};
    UpdateDiff::ImageParserPtr newParser_ {nullptr};
    UpdateDiff::ImageParserPtr oldParser_ {nullptr};
};

class CompressedImageDiff : public ImageDiff {
public:
    CompressedImageDiff(size_t limit,
        UpdateDiff::ImageParserPtr newParser, UpdateDiff::ImageParserPtr oldParser, int32_t type)
        : ImageDiff(limit, newParser, oldParser), type_(type) {}
    ~CompressedImageDiff() override {}

    int32_t MakePatch(const std::string &patchName) override;
protected:
    virtual int32_t TestAndSetConfig(const BlockBuffer &buffer, const std::string &fileName) = 0;
    int32_t DiffFile(const std::string &fileName, size_t &oldOffset, size_t &newOffset);
    int32_t CompressData(hpackage::PkgManager::FileInfoPtr info,
        const BlockBuffer &buffer, std::vector<uint8_t> &outData, size_t &outSize) const;
    int32_t type_;
};

class ZipImageDiff : public CompressedImageDiff {
public:
    ZipImageDiff(size_t limit, UpdateDiff::ImageParserPtr newParser, UpdateDiff::ImageParserPtr oldParser)
        : CompressedImageDiff(limit, newParser, oldParser, BLOCK_DEFLATE) {}
    ~ZipImageDiff() override {}

    int32_t WriteHeader(std::ofstream &patchFile, size_t &offset, ImageBlock &block) const override;
protected:
    int32_t TestAndSetConfig(const BlockBuffer &buffer, const std::string &fileName) override;

    int32_t method_ = 0;
    int32_t level_ = 0;
    int32_t windowBits_ = 0;
    int32_t memLevel_ = 0;
    int32_t strategy_ = 0;
};

class Lz4ImageDiff : public CompressedImageDiff {
public:
    Lz4ImageDiff(size_t limit, UpdateDiff::ImageParserPtr newParser, UpdateDiff::ImageParserPtr oldParser)
        : CompressedImageDiff(limit, newParser, oldParser, BLOCK_LZ4) {}
    ~Lz4ImageDiff() override {}

    int32_t WriteHeader(std::ofstream &patchFile, size_t &offset, ImageBlock &block) const override;
protected:
    int32_t TestAndSetConfig(const BlockBuffer &buffer, const std::string &fileName) override;
private:
    int32_t method_ {0};
    int32_t compressionLevel_ {0};
    int32_t blockIndependence_ {0};
    int32_t contentChecksumFlag_ {0};
    int32_t blockSizeID_ {0};
    int32_t autoFlush_ {1};
};

class GZipImageDiff : public ZipImageDiff {
public:
    GZipImageDiff(size_t limit, UpdateDiff::ImageParserPtr newParser, UpdateDiff::ImageParserPtr oldParser)
        : ZipImageDiff(limit, newParser, oldParser) {}
    ~GZipImageDiff() override {}
};
} // namespace updatepatch
#endif // IMGAE_DIFF_H