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

#include "image_diff.h"
#include <iostream>
#include <vector>
#include "diffpatch.h"

using namespace hpackage;

namespace updatepatch {
#define GET_REAL_DATA_LEN(info) (info) ->packedSize + (info)->dataOffset - (info)->headerOffset
constexpr int32_t LZ4F_MAX_BLOCKID = 7;
constexpr int32_t ZIP_MAX_LEVEL = 9;

template<class DataType>
static void WriteToFile(std::ofstream &patchFile, DataType data, size_t dataSize)
{
    patchFile.write(reinterpret_cast<const char*>(&data), dataSize);
}

static size_t GetHeaderSize(const ImageBlock &block)
{
    switch (block.type) {
        case BLOCK_NORMAL:
            return sizeof(uint32_t) + PATCH_NORMAL_MIN_HEADER_LEN;
        case BLOCK_DEFLATE:
            return sizeof(uint32_t) + PATCH_DEFLATE_MIN_HEADER_LEN;
        case BLOCK_RAW:
            return sizeof(uint32_t) + sizeof(uint32_t) + block.newInfo.length;
        case BLOCK_LZ4:
            return sizeof(uint32_t) + PATCH_LZ4_MIN_HEADER_LEN;
        default:
            return 0;
    }
}

int32_t ImageDiff::MakePatch(const std::string &patchName)
{
    PATCH_LOGI("ImageDiff::MakePatch %s limit_:%d", patchName.c_str(), limit_);
    PATCH_CHECK(newParser_ != nullptr && oldParser_ != nullptr, return -1, "Invalid parser");
    BlockBuffer newBuffer;
    BlockBuffer olduffer;
    int32_t ret = newParser_->GetPkgBuffer(newBuffer);
    ret |= oldParser_->GetPkgBuffer(olduffer);
    PATCH_CHECK(ret == 0, return -1, "Failed to get pkgbuffer");

    PATCH_LOGI("ImageDiff::MakePatch newBuffer %zu %zu ", newBuffer.length, olduffer.length);

    if (limit_ == 0 || newBuffer.length <= limit_) {
        ImageBlock block = {
            BLOCK_NORMAL,
            { newBuffer.buffer, 0, newBuffer.length },
            { olduffer.buffer, 0, olduffer.length },
        };
        updateBlocks_.push_back(std::move(block));
    } else {
        PatchBuffer oldInfo = { olduffer.buffer, 0, olduffer.length };
        PatchBuffer newInfo = { newBuffer.buffer, 0, newBuffer.length };
        ret = SplitImage(oldInfo, newInfo);
        PATCH_CHECK(ret == 0, return -1, "Failed to split imgage");
    }
    return DiffImage(patchName);
}

int32_t ImageDiff::SplitImage(const PatchBuffer &oldInfo, const PatchBuffer &newInfo)
{
    size_t blockCount = newInfo.length / limit_ + 1;
    size_t oldBlockSize = oldInfo.length / blockCount;
    size_t newBlockSize = newInfo.length / blockCount;
    int32_t type = (oldInfo.length == 0) ? BLOCK_RAW : BLOCK_NORMAL;
    size_t i = 0;
    while (i < blockCount - 1) {
        ImageBlock block = {
            type,
            { newInfo.buffer, newInfo.start + newBlockSize * i, newBlockSize },
            { oldInfo.buffer, oldInfo.start + oldBlockSize * i, oldBlockSize },
        };
        updateBlocks_.push_back(std::move(block));
        i++;
    }
    ImageBlock block = {
        type,
        { newInfo.buffer, newInfo.start + newBlockSize * i, newInfo.length - (newInfo.start + newBlockSize * i) },
        { oldInfo.buffer, oldInfo.start + oldBlockSize * i, oldInfo.length - (oldInfo.start + oldBlockSize * i) },
    };
    updateBlocks_.push_back(std::move(block));
    return 0;
}

int32_t ImageDiff::WriteHeader(std::ofstream &patchFile, size_t &dataOffset, ImageBlock &block) const
{
    int32_t ret = 0;
    switch (block.type) {
        case BLOCK_NORMAL: {
            std::vector<uint8_t> patchData;
            BlocksDiff diff;
            BlockBuffer newInfo = { block.newInfo.buffer + block.newInfo.start, block.newInfo.length };
            BlockBuffer oldInfo = { block.oldInfo.buffer + block.oldInfo.start, block.oldInfo.length };
            ret = diff.MakePatch(newInfo, oldInfo, patchData);
            PATCH_CHECK(ret == 0, return -1, "Failed to make block patch");

            block.patchData = std::move(patchData);
            PATCH_LOGI("WriteHeader BLOCK_NORMAL patchOffset %zu oldInfo %ld %ld newInfo:%zu %zu",
                static_cast<size_t>(patchFile.tellp()),
                block.oldInfo.start, block.oldInfo.length, block.newInfo.start, block.newInfo.length);
            BlockBuffer patchBuffer = {block.patchData.data(), block.patchData.size()};
            PATCH_LOGI("WriteHeader BLOCK_NORMAL dataOffset:%zu hash %zu %s", dataOffset,
                block.patchData.size(), GeneraterBufferHash(patchBuffer).c_str());

            WriteToFile<int64_t>(patchFile, static_cast<int64_t>(block.oldInfo.start), sizeof(int64_t));
            WriteToFile<int64_t>(patchFile, static_cast<int64_t>(block.oldInfo.length), sizeof(int64_t));
            WriteToFile<int64_t>(patchFile, static_cast<int64_t>(dataOffset), sizeof(int64_t));
            dataOffset += block.patchData.size();
            break;
        }
        case BLOCK_RAW: {
            PATCH_LOGI("WriteHeader BLOCK_ROW patchOffset %zu dataOffset:%zu newInfo:%zu",
                static_cast<size_t>(patchFile.tellp()), dataOffset, block.newInfo.length);
            WriteToFile<int32_t>(patchFile, static_cast<int32_t>(block.newInfo.length), sizeof(int32_t));
            patchFile.write(reinterpret_cast<const char*>(block.newInfo.buffer + block.newInfo.start),
                block.newInfo.length);
            BlockBuffer rawData = { block.newInfo.buffer + block.newInfo.start, block.newInfo.length };
            PATCH_LOGI("WriteHeader BLOCK_ROW hash %zu %s",
                block.newInfo.length, GeneraterBufferHash(rawData).c_str());
            break;
        }
        default:
            break;
    }
    return ret;
}

int32_t ImageDiff::DiffImage(const std::string &patchName)
{
    std::ofstream patchFile(patchName, std::ios::out | std::ios::binary);
    PATCH_CHECK(!patchFile.fail(), return -1, "Failed to open %s", patchName.c_str());

    patchFile.write(IMGDIFF_MAGIC.c_str(), IMGDIFF_MAGIC.size());
    size_t dataOffset = IMGDIFF_MAGIC.size();
    uint32_t size = static_cast<uint32_t>(updateBlocks_.size());
    patchFile.write(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
    dataOffset += sizeof(uint32_t);

    for (size_t index = 0; index < updateBlocks_.size(); index++) {
        dataOffset += GetHeaderSize(updateBlocks_[index]);
    }

    for (size_t index = 0; index < updateBlocks_.size(); index++) {
        PATCH_LOGI("DiffImage [%zu] write header patchOffset %zu dataOffset %zu",
            index, static_cast<size_t>(patchFile.tellp()), dataOffset);
        patchFile.write(reinterpret_cast<const char*>(&updateBlocks_[index].type), sizeof(uint32_t));
        int32_t ret = WriteHeader(patchFile, dataOffset, updateBlocks_[index]);
        PATCH_CHECK(ret == 0, return -1, "Failed to write header");
    }

    for (size_t index = 0; index < updateBlocks_.size(); index++) {
        if (updateBlocks_[index].type != BLOCK_RAW) {
            PATCH_LOGI("DiffImage [%zu] write patch patchOffset %zu length %zu",
                index, static_cast<size_t>(patchFile.tellp()), updateBlocks_[index].patchData.size());
            patchFile.write(reinterpret_cast<const char*>(updateBlocks_[index].patchData.data()),
                updateBlocks_[index].patchData.size());
        }
    }
    PATCH_LOGI("DiffImage success patchOffset %zu %s", static_cast<size_t>(patchFile.tellp()), patchName.c_str());
    patchFile.close();
    return 0;
}

int32_t CompressedImageDiff::MakePatch(const std::string &patchName)
{
    PATCH_DEBUG("CompressedImageDiff::MakePatch %s limit_:%d", patchName.c_str(), limit_);
    PATCH_CHECK(newParser_ != nullptr && oldParser_ != nullptr, return -1, "Invalid parser");
    BlockBuffer newBuffer;
    BlockBuffer oldBuffer;
    int32_t ret = newParser_->GetPkgBuffer(newBuffer);
    ret |= oldParser_->GetPkgBuffer(oldBuffer);
    PATCH_CHECK(ret == 0, return -1, "Failed to get pkgbuffer");

    if (limit_ != 0 && newBuffer.length >= limit_) {
        PatchBuffer oldInfo = { oldBuffer.buffer, 0, oldBuffer.length };
        PatchBuffer newInfo = { newBuffer.buffer, 0, newBuffer.length };
        ret = SplitImage(oldInfo, newInfo);
        PATCH_CHECK(ret == 0, return -1, "Failed to split imgage");
        return DiffImage(patchName);
    }

    size_t oldOffset = 0;
    size_t newOffset = 0;
    for (size_t i = 0; i < newParser_->GetFileIds().size(); i++) {
        PATCH_LOGI("CompressedImageDiff::DiffFile %s oldOffset:%zu newOffset:%zu",
            newParser_->GetFileIds()[i].c_str(), oldOffset, newOffset);
        ret = DiffFile(newParser_->GetFileIds()[i], oldOffset, newOffset);
        PATCH_CHECK(ret == 0, break, "Failed to generate patch");
    }
    if (ret != 0) {
        updateBlocks_.clear();
        return ImageDiff::MakePatch(patchName);
    }
    PATCH_LOGI("MakePatch oldOffset:%zu newOffset:%zu newBuffer %zu oldBuffer: %zu",
        oldOffset, newOffset, newBuffer.length, oldBuffer.length);
    if (newOffset < newBuffer.length) {
        ImageBlock block = {
            BLOCK_RAW,
            { newBuffer.buffer, newOffset, newBuffer.length - newOffset },
            { oldBuffer.buffer, oldOffset, oldBuffer.length - oldOffset },
        };
        updateBlocks_.push_back(std::move(block));
    }
    return DiffImage(patchName);
}

int32_t CompressedImageDiff::DiffFile(const std::string &fileName, size_t &oldOffset, size_t &newOffset)
{
    BlockBuffer orgNewBuffer;
    BlockBuffer orgOldBuffer;
    int32_t ret = newParser_->GetPkgBuffer(orgNewBuffer);
    ret |= oldParser_->GetPkgBuffer(orgOldBuffer);
    PATCH_CHECK(ret == 0, return -1, "Failed to get pkgbuffer");

    std::vector<uint8_t> newBuffer;
    std::vector<uint8_t> oldBuffer;
    ret = newParser_->Extract(fileName, newBuffer);
    const FileInfo *newFileInfo = newParser_->GetFileInfo(fileName);
    PATCH_CHECK(ret == 0 && newFileInfo != nullptr, return -1, "Failed to get new data");
    newOffset += GET_REAL_DATA_LEN(newFileInfo);
    PATCH_CHECK(limit_ == 0 || newFileInfo->unpackedSize < limit_, return PATCH_EXCEED_LIMIT,
        "Exceed limit, so make patch by original file");

    ret = oldParser_->Extract(fileName, oldBuffer);
    if (ret != 0) {
        ImageBlock block = {
            BLOCK_RAW,
            { orgNewBuffer.buffer, newFileInfo->headerOffset, GET_REAL_DATA_LEN(newFileInfo) },
            { orgOldBuffer.buffer, 0, orgOldBuffer.length },
        };
        updateBlocks_.push_back(std::move(block));
        return 0;
    }
    const FileInfo *oldFileInfo = oldParser_->GetFileInfo(fileName);
    PATCH_CHECK(oldFileInfo != nullptr, return -1, "Failed to get file info");
    oldOffset += GET_REAL_DATA_LEN(oldFileInfo);

    BlockBuffer newData = {newBuffer.data(), newFileInfo->unpackedSize};
    ret = TestAndSetConfig(newData, fileName);
    PATCH_CHECK(ret == 0, return -1, "Failed to test zip config");

    if (type_ != BLOCK_LZ4 && newFileInfo->dataOffset > newFileInfo->headerOffset) {
        ImageBlock block = {
            BLOCK_RAW,
            { orgNewBuffer.buffer, newFileInfo->headerOffset, newFileInfo->dataOffset - newFileInfo->headerOffset },
            { orgOldBuffer.buffer, oldFileInfo->headerOffset, oldFileInfo->dataOffset - oldFileInfo->headerOffset },
        };
        updateBlocks_.push_back(std::move(block));
    }

    ImageBlock block = {
        type_,
        { orgNewBuffer.buffer, newFileInfo->dataOffset, newFileInfo->packedSize },
        { orgOldBuffer.buffer, oldFileInfo->dataOffset, oldFileInfo->packedSize },
    };
    block.srcOriginalData = std::move(oldBuffer);
    block.destOriginalData = std::move(newBuffer);
    block.srcOriginalLength = oldFileInfo->unpackedSize;
    block.destOriginalLength = newFileInfo->unpackedSize;
    updateBlocks_.push_back(std::move(block));
    return 0;
}

int32_t ZipImageDiff::WriteHeader(std::ofstream &patchFile, size_t &dataOffset, ImageBlock &block) const
{
    int32_t ret = 0;
    if (block.type == BLOCK_DEFLATE) {
        std::vector<uint8_t> patchData;
        BlocksDiff diff;
        BlockBuffer oldInfo = { block.srcOriginalData.data(), block.srcOriginalLength };
        BlockBuffer newInfo = { block.destOriginalData.data(), block.destOriginalLength };
        ret = diff.MakePatch(newInfo, oldInfo, patchData);
        PATCH_CHECK(ret == 0, return -1, "Failed to make block patch");
        block.patchData = std::move(patchData);

        PATCH_LOGI("WriteHeader BLOCK_DEFLATE patchoffset %zu dataOffset:%zu patchData:%zu",
            static_cast<size_t>(patchFile.tellp()), dataOffset, block.patchData.size());
        PATCH_LOGI("WriteHeader oldInfo start:%zu length:%zu", block.oldInfo.start, block.oldInfo.length);
        PATCH_LOGI("WriteHeader uncompressedLength:%zu %zu", block.srcOriginalLength, block.destOriginalLength);
        PATCH_LOGI("WriteHeader level_:%d method_:%d windowBits_:%d memLevel_:%d strategy_:%d",
            level_, method_, windowBits_, memLevel_, strategy_);
        PATCH_LOGI("WriteHeader BLOCK_DEFLATE decompressed hash %zu %s",
            newInfo.length, GeneraterBufferHash(newInfo).c_str());

        WriteToFile<int64_t>(patchFile, static_cast<int64_t>(block.oldInfo.start), sizeof(int64_t));
        WriteToFile<int64_t>(patchFile, static_cast<int64_t>(block.oldInfo.length), sizeof(int64_t));
        WriteToFile<int64_t>(patchFile, static_cast<int64_t>(dataOffset), sizeof(int64_t));
        WriteToFile<int64_t>(patchFile, static_cast<int64_t>(block.srcOriginalLength), sizeof(int64_t));
        WriteToFile<int64_t>(patchFile, static_cast<int64_t>(block.destOriginalLength), sizeof(int64_t));

        WriteToFile<int32_t>(patchFile, level_, sizeof(int32_t));
        WriteToFile<int32_t>(patchFile, method_, sizeof(int32_t));
        WriteToFile<int32_t>(patchFile, windowBits_, sizeof(int32_t));
        WriteToFile<int32_t>(patchFile, memLevel_, sizeof(int32_t));
        WriteToFile<int32_t>(patchFile, strategy_, sizeof(int32_t));
        dataOffset += block.patchData.size();
    } else {
        ret = ImageDiff::WriteHeader(patchFile, dataOffset, block);
    }
    return ret;
}

int32_t ZipImageDiff::TestAndSetConfig(const BlockBuffer &buffer, const std::string &fileName)
{
    const FileInfo *fileInfo = newParser_->GetFileInfo(fileName);
    PATCH_CHECK(fileInfo != nullptr, return -1, "Failed to get file info");
    ZipFileInfo *info = (ZipFileInfo *)fileInfo;
    method_ = info->method;
    level_ = info->level;
    windowBits_ = info->windowBits;
    memLevel_ = info->memLevel;
    strategy_ = info->strategy;

    BlockBuffer orgNewBuffer;
    int32_t ret = newParser_->GetPkgBuffer(orgNewBuffer);
    PATCH_CHECK(ret == 0, return -1, "Failed to get pkgbuffer");
    ZipFileInfo zipInfo {};
    zipInfo.fileInfo.packMethod = info->fileInfo.packMethod;
    zipInfo.method = info->method;
    zipInfo.level = info->level;
    zipInfo.windowBits = info->windowBits;
    zipInfo.memLevel = info->memLevel;
    zipInfo.strategy = info->strategy;

    PATCH_LOGI("TestAndSetConfig new info %zu %zu %zu %zu",
        fileInfo->unpackedSize, fileInfo->packedSize, fileInfo->dataOffset, fileInfo->headerOffset);
    PATCH_LOGI("TestAndSetConfig level_:%d method_:%d windowBits_:%d memLevel_:%d strategy_:%d",
        level_, method_, windowBits_, memLevel_, strategy_);
    BlockBuffer orgData = {orgNewBuffer.buffer + fileInfo->dataOffset, fileInfo->packedSize};
    PATCH_DEBUG("DiffFile new orignial hash %zu %s", fileInfo->packedSize, GeneraterBufferHash(orgData).c_str());

    std::vector<uint8_t> data;
    for (int32_t i = ZIP_MAX_LEVEL; i >= 0; i--) {
        zipInfo.level = i;
        size_t bufferSize = 0;
        ret = CompressData(&zipInfo.fileInfo, buffer, data, bufferSize);
        PATCH_CHECK(ret == 0, return -1, "Can not Compress buff ");

        if ((bufferSize == fileInfo->packedSize) &&
            memcmp(data.data(), orgNewBuffer.buffer + fileInfo->dataOffset, bufferSize) == 0) {
            level_ = i;
            return 0;
        }
    }
    return -1;
}

int32_t Lz4ImageDiff::WriteHeader(std::ofstream &patchFile, size_t &dataOffset, ImageBlock &block) const
{
    int32_t ret = 0;
    if (block.type == BLOCK_LZ4) {
        std::vector<uint8_t> patchData;
        BlocksDiff diff;
        BlockBuffer oldInfo = { block.srcOriginalData.data(), block.srcOriginalLength };
        BlockBuffer newInfo = { block.destOriginalData.data(), block.destOriginalLength };
        ret = diff.MakePatch(newInfo, oldInfo, patchData);
        PATCH_CHECK(ret == 0, return -1, "Failed to make block patch");
        block.patchData = std::move(patchData);

        PATCH_LOGI("WriteHeader BLOCK_LZ4 patchoffset %zu dataOffset:%zu patchData:%zu",
            static_cast<size_t>(patchFile.tellp()), dataOffset, block.patchData.size());
        PATCH_LOGI("WriteHeader oldInfo start:%zu length:%zu", block.oldInfo.start, block.oldInfo.length);
        PATCH_LOGI("WriteHeader uncompressedLength:%zu %zu", block.srcOriginalLength, block.destOriginalLength);
        PATCH_LOGI("WriteHeader level_:%d method_:%d blockIndependence_:%d contentChecksumFlag_:%d blockSizeID_:%d %d",
            compressionLevel_, method_, blockIndependence_, contentChecksumFlag_, blockSizeID_, autoFlush_);
        PATCH_LOGI("WriteHeader BLOCK_LZ4 decompressed hash %zu %s",
            newInfo.length, GeneraterBufferHash(newInfo).c_str());
        WriteToFile<int64_t>(patchFile, static_cast<int64_t>(block.oldInfo.start), sizeof(int64_t));
        WriteToFile<int64_t>(patchFile, static_cast<int64_t>(block.oldInfo.length), sizeof(int64_t));
        WriteToFile<int64_t>(patchFile, static_cast<int64_t>(dataOffset), sizeof(int64_t));
        WriteToFile<int64_t>(patchFile, static_cast<int64_t>(block.srcOriginalLength), sizeof(int64_t));
        WriteToFile<int64_t>(patchFile, static_cast<int64_t>(block.destOriginalLength), sizeof(int64_t));

        int32_t magic = (method_ == PKG_COMPRESS_METHOD_LZ4_BLOCK) ? LZ4B_MAGIC : LZ4S_MAGIC;
        WriteToFile<int32_t>(patchFile, compressionLevel_, sizeof(int32_t));
        WriteToFile<int32_t>(patchFile, magic, sizeof(int32_t));
        WriteToFile<int32_t>(patchFile, blockIndependence_, sizeof(int32_t));
        WriteToFile<int32_t>(patchFile, contentChecksumFlag_, sizeof(int32_t));
        WriteToFile<int32_t>(patchFile, blockSizeID_, sizeof(int32_t));
        WriteToFile<int32_t>(patchFile, autoFlush_, sizeof(int32_t));
        dataOffset += block.patchData.size();
    } else {
        ret = ImageDiff::WriteHeader(patchFile, dataOffset, block);
    }
    return ret;
}

int32_t Lz4ImageDiff::TestAndSetConfig(const BlockBuffer &buffer, const std::string &fileName)
{
    const FileInfo *fileInfo = newParser_->GetFileInfo(fileName);
    PATCH_CHECK(fileInfo != nullptr, return -1, "Failed to get file info");
    Lz4FileInfo *info = (Lz4FileInfo *)fileInfo;
    method_ = info->fileInfo.packMethod;
    compressionLevel_ = info->compressionLevel;
    blockIndependence_ = info->blockIndependence;
    contentChecksumFlag_ = info->contentChecksumFlag;
    blockSizeID_ = info->blockSizeID;
    autoFlush_ = info->autoFlush;

    BlockBuffer orgNewBuffer;
    int32_t ret = newParser_->GetPkgBuffer(orgNewBuffer);
    PATCH_CHECK(ret == 0, return -1, "Failed to get pkgbuffer");
    Lz4FileInfo lz4Info {};
    lz4Info.fileInfo.packMethod = info->fileInfo.packMethod;
    lz4Info.compressionLevel = info->compressionLevel;
    lz4Info.blockIndependence = info->blockIndependence;
    lz4Info.contentChecksumFlag = info->contentChecksumFlag;
    lz4Info.blockSizeID = info->blockSizeID;
    lz4Info.autoFlush = info->autoFlush;

    PATCH_DEBUG("TestAndSetConfig level_:%d method_:%d blockIndependence_:%d contentChecksumFlag_:%d blockSizeID_:%d",
        compressionLevel_, method_, blockIndependence_, contentChecksumFlag_, blockSizeID_);
    PATCH_DEBUG("DiffFile new info %zu %zu %zu %zu",
        fileInfo->unpackedSize, fileInfo->packedSize, fileInfo->dataOffset, fileInfo->headerOffset);
    BlockBuffer orgData = { orgNewBuffer.buffer, fileInfo->packedSize + sizeof(uint32_t) };
    PATCH_DEBUG("DiffFile new orignial hash %zu %s",
        fileInfo->packedSize + sizeof(uint32_t), GeneraterBufferHash(orgData).c_str());

    std::vector<uint8_t> data;
    for (int32_t i = 0; i <= LZ4F_MAX_BLOCKID; i++) {
        lz4Info.blockSizeID = i;
        size_t bufferSize = 0;
        ret = CompressData(&lz4Info.fileInfo, buffer, data, bufferSize);
        PATCH_CHECK(ret == 0, return -1, "Can not Compress buff ");

        if ((bufferSize == fileInfo->packedSize + sizeof(uint32_t)) &&
            memcmp(data.data(), orgNewBuffer.buffer + fileInfo->headerOffset, bufferSize) == 0) {
            blockSizeID_ = i;
            return 0;
        }
    }
    return -1;
}

int32_t CompressedImageDiff::CompressData(hpackage::PkgManager::FileInfoPtr info,
    const BlockBuffer &buffer, std::vector<uint8_t> &outData, size_t &bufferSize) const
{
    hpackage::PkgManager *pkgManager = hpackage::PkgManager::GetPackageInstance();
    PATCH_CHECK(pkgManager != nullptr, return -1, "Can not get manager ");
    hpackage::PkgManager::StreamPtr stream1 = nullptr;
    pkgManager->CreatePkgStream(stream1, "gzip",
        [&outData, &bufferSize](const PkgBuffer &data,
            size_t size, size_t start, bool isFinish, const void *context) ->int {
            if (isFinish) {
                return 0;
            }
            bufferSize += size;
            if ((start + bufferSize) > outData.size()) {
                outData.resize(IGMDIFF_LIMIT_UNIT * ((start + bufferSize) / IGMDIFF_LIMIT_UNIT + 1));
            }
            return memcpy_s(outData.data() + start, outData.size(), data.buffer, size);
        }, nullptr);
    int32_t ret = pkgManager->CompressBuffer(info, {buffer.buffer, buffer.length}, stream1);
    PATCH_CHECK(ret == 0, return -1, "Can not Compress buff ");
    PATCH_DEBUG("UpdateDiff::MakePatch totalSize: %zu", bufferSize);
    return 0;
}
} // namespace updatepatch