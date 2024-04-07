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

#include "image_patch.h"
#include <memory>
#include <string>
#include <vector>
#include "diffpatch.h"
#include "lz4_adapter.h"
#include "openssl/sha.h"
#include "securec.h"
#include "zip_adapter.h"

using namespace hpackage;

namespace updatepatch {
uint32_t g_tmpFileId = 0;

int32_t NormalImagePatch::ApplyImagePatch(const PatchParam &param, size_t &startOffset)
{
    size_t offset = startOffset;
    PATCH_CHECK((offset + PATCH_NORMAL_MIN_HEADER_LEN) <= param.patchSize, return -1, "Failed to check datalen");

    size_t srcStart = static_cast<size_t>(ReadLE<int64_t>(param.patch + offset));
    offset += sizeof(int64_t);
    size_t srcLen = static_cast<size_t>(ReadLE<int64_t>(param.patch + offset));
    offset += sizeof(int64_t);
    size_t patchOffset = static_cast<size_t>(ReadLE<int64_t>(param.patch + offset));
    offset += sizeof(int64_t);
    PATCH_LOGI("ApplyImagePatch srcStart %zu srcLen %zu patchOffset: %zu", srcStart, srcLen, patchOffset);
    PATCH_CHECK(srcStart + srcLen <= param.oldSize, return -1, "Failed to check datalen");

    PatchBuffer patchInfo = {param.patch, patchOffset, param.patchSize};
    BlockBuffer oldInfo = {param.oldBuff + srcStart, srcLen};
    int32_t ret = UpdatePatch::ApplyBlockPatch(patchInfo, oldInfo, writer_);
    PATCH_CHECK(ret == 0, return -1, "Failed to apply bsdiff patch");
    startOffset = offset;
    return 0;
}

int32_t RowImagePatch::ApplyImagePatch(const PatchParam &param, size_t &startOffset)
{
    size_t offset = startOffset;
    PATCH_CHECK((offset + sizeof(int32_t)) <= param.patchSize, return -1, "Failed to check datalen");
    size_t dataLen = static_cast<size_t>(ReadLE<uint32_t>(param.patch + offset));
    PATCH_CHECK(offset + dataLen <= param.patchSize, return -1, "Failed to check datalen");
    offset += sizeof(uint32_t);

    BlockBuffer data = {param.patch + offset, dataLen};
    int32_t ret = writer_->Write(0, data, dataLen);
    PATCH_CHECK(ret == 0, return -1, "Failed to write chunk");
    PATCH_LOGI("RowImagePatch startOffset %zu dataLen %zu", startOffset, dataLen);
    PATCH_DEBUG("ApplyImagePatch hash %zu %s",  dataLen, GeneraterBufferHash(data).c_str());
    startOffset = offset + dataLen;
    return 0;
}

int32_t CompressedImagePatch::ApplyImagePatch(const PatchParam &param, size_t &startOffset)
{
    size_t offset = startOffset;
    // read header
    PatchHeader header {};
    int32_t ret = ReadHeader(param, header, offset);
    PATCH_CHECK(ret == 0, return -1, "failed to read header");
    PATCH_LOGI("ApplyImagePatch srcStart %zu srcLen %zu patchOffset: %zu expandedLen:%zu %zu",
        header.srcStart, header.srcLength, header.patchOffset, header.expandedLen, header.targetSize);
    PATCH_CHECK(header.srcStart + header.srcLength <= param.oldSize, return -1, "Failed to check patch");

    // decompress old data
    hpackage::PkgManager::StreamPtr stream = nullptr;
    BlockBuffer oldData = { param.oldBuff + header.srcStart, header.srcLength };
    ret = DecompressData(oldData, stream, true, header.expandedLen);
    PATCH_CHECK(ret == 0, return -1, "Failed to decompress data");

    // prepare new data
    std::unique_ptr<hpackage::FileInfo> info = GetFileInfo();
    PATCH_CHECK(info != nullptr, return -1, "Failed to get file info");
    info->packedSize = header.targetSize;
    info->unpackedSize = header.expandedLen;
    std::unique_ptr<CompressedFileRestore> zipWriter = std::make_unique<CompressedFileRestore>(info.get(), writer_);
    PATCH_CHECK(zipWriter != nullptr, return -1, "Failed to create zip writer");
    PATCH_CHECK(zipWriter->Init() == 0, return -1, "Failed to create zip writer");

    // apply patch
    PatchBuffer patchInfo = {param.patch, header.patchOffset, param.patchSize};
    ret = UpdatePatch::ApplyBlockPatch(patchInfo, stream, zipWriter.get());
    PATCH_CHECK(ret == 0, return -1, "Failed to apply bsdiff patch");

    // compress new data
    size_t originalSize = 0;
    size_t compressSize = 0;
    zipWriter->CompressData(originalSize, compressSize);
    PATCH_LOGI("ApplyImagePatch unpackedSize %zu %zu", originalSize, compressSize);
    PATCH_CHECK(originalSize == header.targetSize, return -1, "Failed to apply bsdiff patch");
    startOffset = offset;
    return 0;
}

int32_t CompressedImagePatch::DecompressData(PkgBuffer buffer,
    hpackage::PkgManager::StreamPtr &stream, bool memory, size_t expandedLen) const
{
    PATCH_CHECK(expandedLen > 0, return 0, "Decompress data is null");
    int32_t ret = 0;
    PkgManager* pkgManager = hpackage::PkgManager::GetPackageInstance();
    PATCH_CHECK(pkgManager != nullptr, return -1, "Failed to get pkg manager");

    std::unique_ptr<hpackage::FileInfo> info = GetFileInfo();
    PATCH_CHECK(info != nullptr, return -1, "Failed to get file info");

    info->packedSize = buffer.length;
    info->unpackedSize = expandedLen;
    info->identity = std::to_string(g_tmpFileId++);

    // 申请内存stream，用于解压老文件
    ret = pkgManager->CreatePkgStream(stream, info->identity,
        expandedLen, memory ? PkgStream::PkgStreamType_MemoryMap : PkgStream::PkgStreamType_Write);
    PATCH_CHECK(stream != nullptr, return -1, "Failed to create stream");

    ret = pkgManager->DecompressBuffer(info.get(), buffer, stream);
    PATCH_CHECK(ret == 0, pkgManager->ClosePkgStream(stream); return -1, "Can not decompress buff");

    if (bonusData_.size() == 0) {
        return 0;
    }
    PATCH_CHECK(info->unpackedSize <= (expandedLen - bonusData_.size()), return -1, "Source inflation short");
    if (memory) { // not support for none memory
        PkgBuffer memBuffer;
        ret = stream->GetBuffer(memBuffer);
        PATCH_CHECK(ret == 0, pkgManager->ClosePkgStream(stream); return -1, "Can not get memory buff");
        ret = memcpy_s(memBuffer.buffer + info->unpackedSize,
            expandedLen - info->unpackedSize, bonusData_.data(), bonusData_.size());
    }
    return ret;
}

int32_t ZipImagePatch::ReadHeader(const PatchParam &param, PatchHeader &header, size_t &offset)
{
    PATCH_CHECK((offset + PATCH_DEFLATE_MIN_HEADER_LEN) <= param.patchSize, return -1, "Failed to check datalen");
    header.srcStart = static_cast<size_t>(ReadLE<uint64_t>(param.patch + offset));
    offset += sizeof(uint64_t);
    header.srcLength = static_cast<size_t>(ReadLE<uint64_t>(param.patch + offset));
    offset += sizeof(uint64_t);
    header.patchOffset = static_cast<size_t>(ReadLE<uint64_t>(param.patch + offset));
    offset += sizeof(uint64_t);
    header.expandedLen = static_cast<size_t>(ReadLE<uint64_t>(param.patch + offset));
    offset += sizeof(uint64_t);
    header.targetSize = static_cast<size_t>(ReadLE<uint64_t>(param.patch + offset));
    offset += sizeof(uint64_t);

    level_ = ReadLE<int32_t>(param.patch + offset);
    offset += sizeof(int32_t);
    method_ = ReadLE<int32_t>(param.patch + offset);
    offset += sizeof(int32_t);
    windowBits_ = ReadLE<int32_t>(param.patch + offset);
    offset += sizeof(int32_t);
    memLevel_ = ReadLE<int32_t>(param.patch + offset);
    offset += sizeof(int32_t);
    strategy_ = ReadLE<int32_t>(param.patch + offset);
    offset += sizeof(int32_t);

    PATCH_LOGI("ZipImagePatch::ReadHeader level_:%d method_:%d windowBits_:%d memLevel_:%d strategy_:%d",
        level_, method_, windowBits_, memLevel_, strategy_);
    return 0;
}

std::unique_ptr<hpackage::FileInfo> ZipImagePatch::GetFileInfo() const
{
    hpackage::ZipFileInfo *fileInfo = new ZipFileInfo;
    PATCH_CHECK(fileInfo != nullptr, return nullptr, "Failed to new file info");
    fileInfo->fileInfo.packMethod = PKG_COMPRESS_METHOD_ZIP;
    fileInfo->fileInfo.digestMethod = PKG_DIGEST_TYPE_NONE;
    fileInfo->fileInfo.packedSize = 0;
    fileInfo->fileInfo.unpackedSize = 0;
    fileInfo->fileInfo.identity = std::to_string(g_tmpFileId++);
    fileInfo->level = level_;
    fileInfo->method = method_;
    fileInfo->windowBits = windowBits_;
    fileInfo->memLevel = memLevel_;
    fileInfo->strategy = strategy_;
    return std::unique_ptr<hpackage::FileInfo>((FileInfo *)fileInfo);
}

int32_t GZipImagePatch::ReadHeader(const PatchParam &param, PatchHeader &header, size_t &offset)
{
    int32_t ret = ZipImagePatch::ReadHeader(param, header, offset);
    PATCH_CHECK(ret == 0, return -1, "Failed to read header");
    PATCH_CHECK((offset + sizeof(int32_t) + sizeof(int64_t) + GZIP_HEADER_LEN + GZIP_FOOTER_LEN) <= param.patchSize,
        return -1, "Invalid patch");
    offset += sizeof(int32_t);
    offset += GZIP_HEADER_LEN;
    offset += sizeof(int64_t);
    offset += GZIP_FOOTER_LEN;
    return 0;
}

int32_t Lz4ImagePatch::ReadHeader(const PatchParam &param, PatchHeader &header, size_t &offset)
{
    PATCH_CHECK((offset + PATCH_LZ4_MIN_HEADER_LEN) <= param.patchSize, return -1, "Failed to check datalen");
    header.srcStart = static_cast<size_t>(ReadLE<uint64_t>(param.patch + offset));
    offset += sizeof(uint64_t);
    header.srcLength = static_cast<size_t>(ReadLE<uint64_t>(param.patch + offset));
    offset += sizeof(uint64_t);
    header.patchOffset = static_cast<size_t>(ReadLE<uint64_t>(param.patch + offset));
    offset += sizeof(uint64_t);
    header.expandedLen = static_cast<size_t>(ReadLE<uint64_t>(param.patch + offset));
    offset += sizeof(uint64_t);
    header.targetSize = static_cast<size_t>(ReadLE<uint64_t>(param.patch + offset));
    offset += sizeof(uint64_t);

    compressionLevel_ = static_cast<uint8_t>(ReadLE<int32_t>(param.patch + offset));
    offset += sizeof(int32_t);
    method_ = static_cast<uint8_t>(ReadLE<int32_t>(param.patch + offset));
    offset += sizeof(int32_t);
    blockIndependence_ = static_cast<uint8_t>(ReadLE<int32_t>(param.patch + offset));
    offset += sizeof(int32_t);
    contentChecksumFlag_ = static_cast<uint8_t>(ReadLE<int32_t>(param.patch + offset));
    offset += sizeof(int32_t);
    blockSizeID_ = static_cast<uint8_t>(ReadLE<int32_t>(param.patch + offset));
    offset += sizeof(int32_t);
    autoFlush_ = static_cast<uint8_t>(ReadLE<int32_t>(param.patch + offset));
    offset += sizeof(int32_t);
    PATCH_LOGI("ReadHeader BLOCK_LZ4 level_:%d method_:%d %d contentChecksumFlag_:%d blockSizeID_:%d %d",
        compressionLevel_, method_, blockIndependence_, contentChecksumFlag_, blockSizeID_, autoFlush_);
    return 0;
}

std::unique_ptr<hpackage::FileInfo> Lz4ImagePatch::GetFileInfo() const
{
    hpackage::Lz4FileInfo *fileInfo = new Lz4FileInfo;
    PATCH_CHECK(fileInfo != nullptr, return nullptr, "Failed to new file info");
    fileInfo->fileInfo.packMethod = (method_ == LZ4B_MAGIC) ? PKG_COMPRESS_METHOD_LZ4_BLOCK : PKG_COMPRESS_METHOD_LZ4;
    fileInfo->fileInfo.digestMethod = PKG_DIGEST_TYPE_NONE;
    fileInfo->fileInfo.packedSize = 0;
    fileInfo->fileInfo.unpackedSize = 0;
    fileInfo->fileInfo.identity = std::to_string(g_tmpFileId++);
    fileInfo->compressionLevel = static_cast<uint8_t>(compressionLevel_);
    fileInfo->blockIndependence = static_cast<uint8_t>(blockIndependence_);
    fileInfo->contentChecksumFlag = static_cast<uint8_t>(contentChecksumFlag_);
    fileInfo->blockSizeID = static_cast<uint8_t>(blockSizeID_);
    fileInfo->autoFlush = static_cast<uint8_t>(autoFlush_);
    return std::unique_ptr<hpackage::FileInfo>((FileInfo *)fileInfo);
}

int32_t CompressedFileRestore::Init()
{
    SHA256_Init(&sha256Ctx_);
    if (fileInfo_->packMethod == PKG_COMPRESS_METHOD_ZIP) {
        deflateAdapter_.reset(new ZipAdapter(writer_, 0, fileInfo_));
    } else if (fileInfo_->packMethod == PKG_COMPRESS_METHOD_LZ4) {
        deflateAdapter_.reset(new Lz4FrameAdapter(writer_, 0, fileInfo_));
    } else if (fileInfo_->packMethod == PKG_COMPRESS_METHOD_LZ4_BLOCK) {
        deflateAdapter_.reset(new Lz4BlockAdapter(writer_, 0, fileInfo_));
    }
    PATCH_CHECK(deflateAdapter_ != nullptr, return -1, "Failed to create zip adapter");
    return deflateAdapter_->Open();
}

int32_t CompressedFileRestore::Write(size_t start, const BlockBuffer &buffer, size_t size)
{
    if (size == 0) {
        return 0;
    }
    dataSize_ += size;
    SHA256_Update(&sha256Ctx_, buffer.buffer, size);
    BlockBuffer data = { buffer.buffer, size };
    return deflateAdapter_->WriteData(data);
}

int32_t CompressedFileRestore::CompressData(size_t &originalSize, size_t &compressSize)
{
    int32_t ret = deflateAdapter_->FlushData(compressSize);
    PATCH_CHECK(ret == 0, return -1, "Failed to flush data");
    originalSize = dataSize_;

    std::vector<uint8_t> digest(SHA256_DIGEST_LENGTH);
    SHA256_Final(digest.data(), &sha256Ctx_);
    BlockBuffer buffer = { digest.data(), digest.size() };
    std::string hexDigest = ConvertSha256Hex(buffer);
    PATCH_DEBUG("CompressedFileRestore hash %zu %s ", dataSize_, hexDigest.c_str());
    return 0;
}
} // namespace updater
