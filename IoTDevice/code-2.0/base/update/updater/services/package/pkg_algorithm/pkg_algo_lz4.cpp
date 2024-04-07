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
#include "pkg_algo_lz4.h"
#include "lz4.h"
#include "lz4frame.h"
#include "lz4hc.h"
#include "pkg_stream.h"
#include "pkg_utils.h"
#include "securec.h"

namespace hpackage {
PkgAlgorithmLz4::PkgAlgorithmLz4(const Lz4FileInfo &config) : PkgAlgorithm(),
    compressionLevel_(config.compressionLevel),
    blockIndependence_(config.blockIndependence),
    contentChecksumFlag_(config.contentChecksumFlag),
    blockSizeID_(config.blockSizeID),
    autoFlush_(config.autoFlush)
{
    // blockIndependence_ 0 LZ4F_blockLinked
    // contentChecksumFlag_ 0 disable
    // blockSizeID_ LZ4F_default=0
    if (compressionLevel_ < 1) {
        compressionLevel_ = 2;
    }
    if (compressionLevel_ >= LZ4HC_CLEVEL_MAX) {
        compressionLevel_ = LZ4HC_CLEVEL_MAX;
    }
}

int32_t PkgAlgorithmLz4::AdpLz4Compress(const uint8_t *src, uint8_t *dest,
    uint32_t srcSize, uint32_t dstCapacity) const
{
    if (compressionLevel_ < LZ4HC_CLEVEL_MIN) { // hc 最小是3
        return LZ4_compress_default(reinterpret_cast<const char *>(src), reinterpret_cast<char *>(dest),
            (int32_t)srcSize, (int32_t)dstCapacity);
    }
    return LZ4_compress_HC(reinterpret_cast<const char *>(src), reinterpret_cast<char *>(dest), srcSize, dstCapacity,
        compressionLevel_);
}

int32_t PkgAlgorithmLz4::AdpLz4Decompress(const uint8_t *src, uint8_t *dest, uint32_t srcSize,
    uint32_t dstCapacity) const
{
    return LZ4_decompress_safe(reinterpret_cast<const char *>(src), reinterpret_cast<char *>(dest), srcSize,
        dstCapacity);
}

int32_t PkgAlgorithmBlockLz4::Pack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
    PkgAlgorithmContext &context)
{
    PKG_CHECK(inStream != nullptr && outStream != nullptr, return PKG_INVALID_PARAM, "Param context null!");
    size_t blockSize = GetBlockSizeFromBlockId(blockSizeID_);
    blockSize = (blockSize > LZ4B_BLOCK_SIZE) ? LZ4B_BLOCK_SIZE : blockSize;
    PkgBuffer inBuffer = {blockSize};
    PkgBuffer outBuffer = {LZ4_compressBound(blockSize)};
    PKG_CHECK(inBuffer.buffer != nullptr && outBuffer.buffer != nullptr,
        return PKG_NONE_MEMORY, "Fail to alloc buffer ");

    PKG_LOGI("frameInfo blockSizeID %d compressionLevel_: %d blockIndependence_:%d contentChecksumFlag_:%d %zu",
        static_cast<int32_t>(blockSizeID_), static_cast<int32_t>(compressionLevel_),
        static_cast<int32_t>(blockIndependence_), static_cast<int32_t>(contentChecksumFlag_), blockSize);

    size_t srcOffset = context.srcOffset;
    size_t destOffset = context.destOffset;
    size_t remainSize = context.unpackedSize;
    size_t readLen = 0;
    /* 写包头 */
    WriteLE32(outBuffer.buffer, LZ4B_MAGIC_NUMBER);
    int32_t ret = outStream->Write(outBuffer, sizeof(LZ4B_MAGIC_NUMBER), destOffset);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "Fail write data ");
    destOffset += sizeof(LZ4B_MAGIC_NUMBER);

    while (remainSize > 0) {
        ret = ReadData(inStream, srcOffset, inBuffer, remainSize, readLen);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Fail read data ");

        // Compress Block, reserve 4 bytes to store block size
        int32_t outSize = AdpLz4Compress(inBuffer.buffer,
            outBuffer.buffer + LZ4B_REVERSED_LEN, readLen, outBuffer.length - LZ4B_REVERSED_LEN);
        PKG_CHECK(outSize > 0, break, "Fail to compress data outSize %d ", outSize);

        // Write block to buffer.
        // Buffer format: <block size> + <block contents>
        WriteLE32(outBuffer.buffer, outSize);
        ret = outStream->Write(outBuffer, outSize + LZ4B_REVERSED_LEN, destOffset);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Fail write data ");

        srcOffset += readLen;
        destOffset += outSize + LZ4B_REVERSED_LEN;
    }
    PKG_CHECK(srcOffset - context.srcOffset == context.unpackedSize,
        return ret, "original size error %zu %zu", srcOffset, context.unpackedSize);
    context.packedSize = destOffset - context.destOffset;

    return PKG_SUCCESS;
}

int32_t PkgAlgorithmBlockLz4::Unpack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
    PkgAlgorithmContext &context)
{
    PKG_CHECK(inStream != nullptr && outStream != nullptr, return PKG_INVALID_PARAM, "Param context null!");
    size_t inBuffSize = LZ4_compressBound(LZ4B_BLOCK_SIZE);
    PKG_CHECK(inBuffSize > 0, return PKG_NONE_MEMORY, "BufferSize must > 0");
    PkgBuffer inBuffer(inBuffSize);
    PkgBuffer outBuffer(LZ4B_BLOCK_SIZE);
    PKG_CHECK(inBuffer.buffer != nullptr && outBuffer.buffer != nullptr, return PKG_NONE_MEMORY,
        "Fail to alloc buffer ");

    size_t srcOffset = context.srcOffset;
    size_t destOffset = context.destOffset;
    size_t remainSize = context.packedSize;
    size_t readLen = 0;

    /* Main Loop */
    while (1) {
        /* Block Size */
        inBuffer.length = sizeof(uint32_t);
        int32_t ret = ReadData(inStream, srcOffset, inBuffer, remainSize, readLen);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Fail read data ");
        if (readLen == 0) {
            break;
        }
        uint32_t blockSize = ReadLE32(inBuffer.buffer);
        PKG_CHECK(!(blockSize > LZ4_COMPRESSBOUND(LZ4B_BLOCK_SIZE) || blockSize > inBuffSize), break,
            "Fail to get block size %u  %u", blockSize, LZ4_COMPRESSBOUND(LZ4B_BLOCK_SIZE));
        srcOffset += sizeof(uint32_t);

        /* Read Block */
        inBuffer.length = blockSize;
        ret = ReadData(inStream, srcOffset, inBuffer, remainSize, readLen);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Fail read data ");

        /* Decode Block */
        int32_t decodeSize = AdpLz4Decompress(inBuffer.buffer,
            outBuffer.buffer, readLen, LZ4B_BLOCK_SIZE);
        PKG_CHECK(decodeSize > 0, break, "Fail to decompress");

        /* Write Block */
        ret = outStream->Write(outBuffer, decodeSize, destOffset);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Fail write data ");
        destOffset += decodeSize;
        srcOffset += readLen;
    }
    context.packedSize = srcOffset - context.srcOffset;
    context.unpackedSize = destOffset - context.destOffset;
    return PKG_SUCCESS;
}

int32_t PkgAlgorithmLz4::GetPackParam(LZ4F_compressionContext_t &ctx, LZ4F_preferences_t &preferences,
    size_t &inBuffSize, size_t &outBuffSize) const
{
    LZ4F_errorCode_t errorCode = 0;
    errorCode = LZ4F_createCompressionContext(&ctx, LZ4F_VERSION);
    PKG_CHECK(!LZ4F_isError(errorCode), return PKG_NONE_MEMORY,
        "Fail to create compress context %s", LZ4F_getErrorName(errorCode));
    size_t blockSize = GetBlockSizeFromBlockId(blockSizeID_);
    PKG_CHECK(!memset_s(&preferences, sizeof(preferences), 0, sizeof(preferences)),
        return PKG_NONE_MEMORY, "Memset failed");
    preferences.autoFlush = autoFlush_;
    preferences.compressionLevel = compressionLevel_;
    preferences.frameInfo.blockMode = ((blockIndependence_ == 0) ? LZ4F_blockLinked : LZ4F_blockIndependent);
    preferences.frameInfo.blockSizeID = (LZ4F_blockSizeID_t)blockSizeID_;
    preferences.frameInfo.contentChecksumFlag =
        ((contentChecksumFlag_ == 0) ? LZ4F_noContentChecksum : LZ4F_contentChecksumEnabled);

    outBuffSize = LZ4F_compressBound(blockSize, &preferences);
    PKG_CHECK(outBuffSize > 0, return PKG_NONE_MEMORY, "BufferSize must > 0");
    inBuffSize = blockSize;

    PKG_LOGI("frameInfo blockSizeID %d compressionLevel_: %d blockIndependence_:%d contentChecksumFlag_:%d",
        static_cast<int32_t>(blockSizeID_), static_cast<int32_t>(compressionLevel_),
        static_cast<int32_t>(blockIndependence_), static_cast<int32_t>(contentChecksumFlag_));
    PKG_LOGI("blockSize %zu %zu %zu", blockSize, GetBlockSizeFromBlockId(blockSizeID_), outBuffSize);
    return PKG_SUCCESS;
}

/* 打包数据时，会自动生成magic字 */
int32_t PkgAlgorithmLz4::Pack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
    PkgAlgorithmContext &context)
{
    PKG_CHECK(inStream != nullptr && outStream != nullptr, return PKG_INVALID_PARAM, "Param context null!");
    LZ4F_compressionContext_t ctx;
    LZ4F_preferences_t preferences;
    size_t inLength = 0;
    size_t outLength = 0;
    int32_t ret = GetPackParam(ctx, preferences, inLength, outLength);
    PKG_CHECK(ret == PKG_SUCCESS, return PKG_NONE_MEMORY, "Fail to get param for pack");

    PkgBuffer inBuffer(inLength);
    PkgBuffer outBuffer(outLength);
    PKG_CHECK(inBuffer.buffer != nullptr && outBuffer.buffer != nullptr,
        (void)LZ4F_freeCompressionContext(ctx); return PKG_NONE_MEMORY, "Fail to alloc buffer ");
    size_t srcOffset = context.srcOffset;
    size_t destOffset = context.destOffset;
    size_t remainSize = context.unpackedSize;

    /* 写包头 */
    size_t dataLen = LZ4F_compressBegin(ctx, outBuffer.buffer, outBuffer.length, &preferences);
    PKG_CHECK(!LZ4F_isError(dataLen), (void)LZ4F_freeCompressionContext(ctx);
        return PKG_NONE_MEMORY, "Fail to generate header %s", LZ4F_getErrorName(dataLen));
    ret = outStream->Write(outBuffer, dataLen, destOffset);
    PKG_CHECK(ret == PKG_SUCCESS, (void)LZ4F_freeCompressionContext(ctx); return ret, "Fail write data ");

    destOffset += dataLen;
    while (remainSize > 0) {
        ret = ReadData(inStream, srcOffset, inBuffer, remainSize, dataLen);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Fail read data ");

        size_t outSize = LZ4F_compressUpdate(ctx,
            outBuffer.buffer, outBuffer.length, inBuffer.buffer, dataLen, nullptr);
        PKG_CHECK(!LZ4F_isError(outSize), ret = PKG_NONE_MEMORY; break,
            "Fail to compress update %s", LZ4F_getErrorName(outSize));
        ret = outStream->Write(outBuffer, outSize, destOffset);
        PKG_CHECK(ret == PKG_SUCCESS, ret = PKG_NONE_MEMORY; break, "Fail write data ");

        srcOffset += dataLen;
        destOffset += outSize;
    }

    if (ret == PKG_SUCCESS) {
        size_t headerSize = LZ4F_compressEnd(ctx, outBuffer.buffer, outBuffer.length, nullptr);
        PKG_CHECK(!LZ4F_isError(headerSize), (void)LZ4F_freeCompressionContext(ctx);
            return PKG_NONE_MEMORY, "Fail to compress update end %s", LZ4F_getErrorName(headerSize));
        ret = outStream->Write(outBuffer, headerSize, destOffset);
        PKG_CHECK(ret == PKG_SUCCESS, (void)LZ4F_freeCompressionContext(ctx); return ret, "Fail write data ");
        destOffset += headerSize;
    }

    (void)LZ4F_freeCompressionContext(ctx);
    PKG_CHECK(srcOffset - context.srcOffset == context.unpackedSize,
        return ret, "original size error %zu %zu", srcOffset, context.unpackedSize);
    context.packedSize = destOffset - context.destOffset;
    return PKG_SUCCESS;
}

int32_t PkgAlgorithmLz4::GetUnpackParam(LZ4F_decompressionContext_t &ctx, const PkgStreamPtr inStream,
    size_t &nextToRead, size_t &srcOffset)
{
    LZ4F_errorCode_t errorCode = 0;
    LZ4F_frameInfo_t frameInfo;

    errorCode = LZ4F_createDecompressionContext(&ctx, LZ4F_VERSION);
    PKG_CHECK(!LZ4F_isError(errorCode), return PKG_INVALID_LZ4,
        "Fail to create compress context %s", LZ4F_getErrorName(errorCode));

    PkgBuffer pkgHeader(LZ4S_HEADER_LEN);
    WriteLE32(pkgHeader.buffer, LZ4S_MAGIC_NUMBER);

    /* Decode stream descriptor */
    size_t readLen = 0;
    size_t outBuffSize = 0;
    size_t inBuffSize = 0;
    size_t sizeCheck = sizeof(LZ4B_MAGIC_NUMBER);
    nextToRead = LZ4F_decompress(ctx, nullptr, &outBuffSize, pkgHeader.buffer, &sizeCheck, nullptr);
    PKG_CHECK(!LZ4F_isError(nextToRead), (void)LZ4F_freeDecompressionContext(ctx);
        return PKG_INVALID_LZ4, "Fail to decode frame info %s", LZ4F_getErrorName(nextToRead));
    PKG_CHECK(nextToRead <= pkgHeader.length, (void)LZ4F_freeDecompressionContext(ctx);
        return PKG_INVALID_LZ4, "Invalid pkgHeader.length %d", pkgHeader.length);

    size_t remainSize = LZ4S_HEADER_LEN;
    pkgHeader.length = nextToRead;
    int32_t ret = ReadData(inStream, srcOffset, pkgHeader, remainSize, readLen);
    PKG_CHECK(ret == PKG_SUCCESS, (void)LZ4F_freeDecompressionContext(ctx); return PKG_INVALID_LZ4, "Fail read data ");
    PKG_CHECK(readLen == pkgHeader.length, (void)LZ4F_freeDecompressionContext(ctx); return PKG_INVALID_LZ4,
        "Invalid len %zu %zu", readLen, pkgHeader.length);
    srcOffset += readLen;
    sizeCheck = readLen;
    nextToRead = LZ4F_decompress(ctx, nullptr, &outBuffSize, pkgHeader.buffer, &sizeCheck, nullptr);
    errorCode = LZ4F_getFrameInfo(ctx, &frameInfo, nullptr, &inBuffSize);
    PKG_CHECK(!LZ4F_isError(errorCode), (void)LZ4F_freeDecompressionContext(ctx); return PKG_INVALID_LZ4,
        "Fail to decode frame info %s", LZ4F_getErrorName(errorCode));
    PKG_CHECK(frameInfo.blockSizeID >= 3 && frameInfo.blockSizeID <= 7,
        (void)LZ4F_freeDecompressionContext(ctx); return PKG_INVALID_LZ4, "Invalid block size ID %d",
        frameInfo.blockSizeID);

    blockIndependence_ = frameInfo.blockMode;
    contentChecksumFlag_ = frameInfo.contentChecksumFlag;
    blockSizeID_ = frameInfo.blockSizeID;
    return PKG_SUCCESS;
}

int32_t PkgAlgorithmLz4::Unpack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
    PkgAlgorithmContext &context)
{
    PKG_CHECK(inStream != nullptr && outStream != nullptr, return PKG_INVALID_PARAM, "Param context null!");
    LZ4F_decompressionContext_t ctx;
    LZ4F_errorCode_t errorCode = 0;
    size_t srcOffset = context.srcOffset;
    size_t destOffset = context.destOffset;
    size_t remainSize = context.packedSize;
    size_t nextToRead = 0;
    int32_t ret = GetUnpackParam(ctx, inStream, nextToRead, srcOffset);
    PKG_CHECK(ret == PKG_SUCCESS, return PKG_INVALID_LZ4, "Fail to get param ");

    size_t outBuffSize = GetBlockSizeFromBlockId(blockSizeID_);
    PKG_LOGI("Block size ID %d outBuffSize:%zu", blockSizeID_, outBuffSize);
    size_t inBuffSize = outBuffSize + sizeof(uint32_t);
    PKG_CHECK(inBuffSize > 0 && outBuffSize > 0, return PKG_NONE_MEMORY, "Buffer size must > 0");

    PkgBuffer inBuffer(inBuffSize);
    PkgBuffer outBuffer(outBuffSize);
    PKG_CHECK(inBuffer.buffer != nullptr && outBuffer.buffer != nullptr,
        (void)LZ4F_freeDecompressionContext(ctx); return PKG_NONE_MEMORY, "Fail to alloc buffer ");

    /* Main Loop */
    while (nextToRead != 0) {
        size_t readLen = 0;
        size_t decodedBytes = outBuffSize;
        PKG_CHECK(nextToRead <= inBuffSize, break, "Error next read %zu %zu ", nextToRead, inBuffSize);

        /* Read Block */
        inBuffer.length = nextToRead;
        ret = ReadData(inStream, srcOffset, inBuffer, remainSize, readLen);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Fail read data ");

        /* Decode Block */
        size_t sizeCheck = readLen;
        errorCode = LZ4F_decompress(ctx, outBuffer.buffer,
            &decodedBytes, inBuffer.buffer, &sizeCheck, nullptr);
        PKG_CHECK(!LZ4F_isError(errorCode), (void)LZ4F_freeDecompressionContext(ctx); return PKG_INVALID_LZ4,
            "Fail to decompress %s", LZ4F_getErrorName(errorCode));
        if (decodedBytes == 0) {
            srcOffset += readLen;
            break;
        }
        PKG_CHECK(sizeCheck == nextToRead, break, "Error next read %zu %zu ", nextToRead, sizeCheck);

        /* Write Block */
        ret = outStream->Write(outBuffer, decodedBytes, destOffset);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Fail write data ");
        destOffset += decodedBytes;
        srcOffset += readLen;
        nextToRead = errorCode;
    }
    errorCode = LZ4F_freeDecompressionContext(ctx);
    PKG_CHECK(!LZ4F_isError(errorCode), return PKG_NONE_MEMORY,
        "Fail to free compress context %s", LZ4F_getErrorName(errorCode));
    context.packedSize = srcOffset - context.srcOffset;
    context.unpackedSize = destOffset - context.destOffset;
    return ret;
}

void PkgAlgorithmLz4::UpdateFileInfo(PkgManager::FileInfoPtr info) const
{
    Lz4FileInfo *lz4Info = (Lz4FileInfo *)info;
    lz4Info->fileInfo.packMethod = PKG_COMPRESS_METHOD_LZ4;
    lz4Info->fileInfo.digestMethod = PKG_DIGEST_TYPE_NONE;
    lz4Info->compressionLevel = compressionLevel_;
    lz4Info->blockIndependence = blockIndependence_;
    lz4Info->contentChecksumFlag = contentChecksumFlag_;
    lz4Info->blockSizeID = blockSizeID_;
    lz4Info->autoFlush = autoFlush_;
}
} // namespace hpackage
