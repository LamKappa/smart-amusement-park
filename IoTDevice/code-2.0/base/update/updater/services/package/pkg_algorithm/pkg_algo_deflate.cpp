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
#include "pkg_algo_deflate.h"
#include "pkg_stream.h"
#include "pkg_utils.h"
#include "securec.h"
#include "zlib.h"

namespace hpackage {
constexpr uint8_t INFLATE_ERROR_TIMES = 5;
constexpr uint32_t IN_BUFFER_SIZE = 1024 * 32;
constexpr uint32_t OUT_BUFFER_SIZE = 1024 * 4;

int32_t PkgAlgoDeflate::DeflateData(const PkgStreamPtr outStream, z_stream &zstream, int32_t flush,
    PkgBuffer &outBuffer, size_t &destOffset) const
{
    int32_t ret = Z_OK;
    do {
        size_t deflateLen = 0;
        ret = deflate(&zstream, flush);
        PKG_CHECK(ret >= Z_OK, return PKG_NOT_EXIST_ALGORITHM, "deflate finish error ret1 %d", ret);
        deflateLen += outBuffer.length - zstream.avail_out;
        if (deflateLen > 0) {
            int32_t ret1 = outStream->Write(outBuffer, deflateLen, destOffset);
            PKG_CHECK(ret1 == PKG_SUCCESS, break, "error write data deflateLen: %zu", deflateLen);
            destOffset += deflateLen;
            zstream.next_out = outBuffer.buffer;
            zstream.avail_out = outBuffer.length;
        }
        if (flush == Z_NO_FLUSH) {
            break;
        }
    } while (ret == Z_OK && flush == Z_FINISH);
    return PKG_SUCCESS;
}

int32_t PkgAlgoDeflate::Pack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
    PkgAlgorithmContext &context)
{
    DigestAlgorithm::DigestAlgorithmPtr algorithm = PkgAlgorithmFactory::GetDigestAlgorithm(context.digestMethod);
    PKG_CHECK(algorithm != nullptr, return PKG_NOT_EXIST_ALGORITHM, "Can not get digest algor");
    PKG_CHECK(inStream != nullptr && outStream != nullptr, return PKG_INVALID_PARAM, "Param context null!");

    PkgBuffer inBuffer = {};
    PkgBuffer outBuffer = {};
    z_stream zstream;
    int32_t ret = InitStream(zstream, true, inBuffer, outBuffer);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "fail InitStream ");
    size_t remainSize = context.unpackedSize;
    uint32_t crc = 0;
    size_t srcOffset = context.srcOffset;
    size_t destOffset = context.destOffset;
    PkgBuffer crcResult((uint8_t *)&crc, sizeof(crc));

    while ((remainSize > 0) || (zstream.avail_in > 0)) {
        size_t readLen = 0;
        if (zstream.avail_in == 0) {
            ret = ReadData(inStream, srcOffset, inBuffer, remainSize, readLen);
            PKG_CHECK(ret == PKG_SUCCESS, break, "Read data fail!");
            zstream.next_in = reinterpret_cast<unsigned char *>(inBuffer.buffer);
            zstream.avail_in = readLen;
            srcOffset += readLen;
            // Calculate CRC of original file
            algorithm->Calculate(crcResult, inBuffer, readLen);
        }
        ret = DeflateData(outStream, zstream, ((remainSize == 0) ? Z_FINISH : Z_NO_FLUSH), outBuffer, destOffset);
        PKG_CHECK(ret == PKG_SUCCESS, break, "error write data deflateLen: %zu", destOffset);
    }
    ReleaseStream(zstream, true);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "error write data");
    PKG_CHECK(srcOffset == context.unpackedSize, return PKG_INVALID_PKG_FORMAT,
        "original size error %zu %zu", srcOffset, context.unpackedSize);
    context.crc = crc;
    context.packedSize = destOffset - context.destOffset;
    return ret;
}

int32_t PkgAlgoDeflate::UnpackCalculate(PkgAlgorithmContext &context, const PkgStreamPtr inStream,
    const PkgStreamPtr outStream, DigestAlgorithm::DigestAlgorithmPtr algorithm)
{
    z_stream zstream;
    PkgBuffer inBuffer = {};
    PkgBuffer outBuffer = {};
    int32_t ret = InitStream(zstream, false, inBuffer, outBuffer);
    PKG_CHECK(ret == PKG_SUCCESS, return ret, "fail InitStream ");
    size_t inflateLen = 0;
    uint32_t errorTimes = 0;
    uint32_t crc = 0;

    PkgBuffer crcResult((uint8_t *)&crc, sizeof(crc));
    size_t remainCompressedSize = context.packedSize;
    size_t srcOffset = context.srcOffset;
    size_t destOffset = context.destOffset;
    size_t readLen = 0;
    while ((remainCompressedSize > 0) || (zstream.avail_in > 0)) {
        if (zstream.avail_in == 0) {
            ret = ReadData(inStream, srcOffset, inBuffer, remainCompressedSize, readLen);
            PKG_CHECK(ret == PKG_SUCCESS, break, "Read data fail!");
            zstream.next_in = reinterpret_cast<uint8_t *>(inBuffer.buffer);
            zstream.avail_in = readLen;
            srcOffset += readLen;
        }

        ret = inflate(&zstream, Z_SYNC_FLUSH);
        PKG_CHECK(ret >= Z_OK, break, "fail inflate ret:%d", ret);
        inflateLen = outBuffer.length - zstream.avail_out;
        errorTimes++;
        if (inflateLen > 0) {
            ret = outStream->Write(outBuffer, inflateLen, destOffset);
            PKG_CHECK(ret == PKG_SUCCESS, break, "write data is fail!");
            destOffset += inflateLen;
            zstream.next_out = outBuffer.buffer;
            zstream.avail_out = outBuffer.length;

            errorTimes = 0;
            algorithm->Calculate(crcResult, outBuffer, inflateLen);
        }
        if (ret == Z_STREAM_END) {
            break;
        }
        PKG_CHECK(errorTimes < INFLATE_ERROR_TIMES, break, "unzip inflated data is abnormal!");
    }
    ReleaseStream(zstream, false);
    context.packedSize = context.packedSize - zstream.avail_in - remainCompressedSize;
    context.unpackedSize = destOffset - context.destOffset;
    PKG_CHECK(0 == context.crc || crc == context.crc, return ret, "crc fail %u %u!", crc, context.crc);
    context.crc = crc;
    return (ret == Z_STREAM_END) ? PKG_SUCCESS : ret;
}

int32_t PkgAlgoDeflate::Unpack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
    PkgAlgorithmContext &context)
{
    DigestAlgorithm::DigestAlgorithmPtr algorithm = PkgAlgorithmFactory::GetDigestAlgorithm(context.digestMethod);
    PKG_CHECK(algorithm != nullptr, return PKG_NOT_EXIST_ALGORITHM, "Can not get digest algor");
    PKG_CHECK(inStream != nullptr && outStream != nullptr,
        return PKG_INVALID_PARAM, "Param context null!");

    return UnpackCalculate(context, inStream, outStream, algorithm);
}

int32_t PkgAlgoDeflate::InitStream(z_stream &zstream, bool zip, PkgBuffer &inBuffer, PkgBuffer &outBuffer)
{
    int32_t ret = PKG_SUCCESS;
    // init zlib stream
    PKG_CHECK(!memset_s(&zstream, sizeof(z_stream), 0, sizeof(z_stream)), return PKG_NONE_MEMORY, "memset fail ");
    if (zip) {
        PKG_LOGI("InitStream level_:%d method_:%d windowBits_:%d memLevel_:%d strategy_:%d",
            level_, method_, windowBits_, memLevel_, strategy_);
        ret = deflateInit2(&zstream, level_, method_, windowBits_, memLevel_, strategy_);
        PKG_CHECK(ret == Z_OK, return PKG_NOT_EXIST_ALGORITHM, "fail deflateInit2 ret %d", ret);
        inBuffer.length = IN_BUFFER_SIZE;
        outBuffer.length = OUT_BUFFER_SIZE;
    } else {
        ret = inflateInit2(&zstream, windowBits_);
        PKG_CHECK(ret == Z_OK, return PKG_NOT_EXIST_ALGORITHM, "fail inflateInit2");
        inBuffer.length = IN_BUFFER_SIZE;
        outBuffer.length = OUT_BUFFER_SIZE;
    }

    inBuffer.data.resize(inBuffer.length);
    outBuffer.data.resize(outBuffer.length);
    inBuffer.buffer = reinterpret_cast<uint8_t *>(inBuffer.data.data());
    outBuffer.buffer = reinterpret_cast<uint8_t *>(outBuffer.data.data());
    zstream.next_out = outBuffer.buffer;
    zstream.avail_out = outBuffer.length;
    return PKG_SUCCESS;
}

void PkgAlgoDeflate::ReleaseStream(z_stream &zstream, bool zip) const
{
    int32_t ret = Z_OK;
    if (zip) {
        ret = deflateEnd(&zstream);
        PKG_CHECK(ret == Z_OK, return, "fail deflateEnd %d", ret);
    } else {
        ret = inflateEnd(&zstream);
        PKG_CHECK(ret == Z_OK, return, "fail inflateEnd %d", ret);
    }
}
} // namespace hpackage
