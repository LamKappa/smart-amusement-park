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
#include "pkg_algorithm.h"
#include "pkg_algo_deflate.h"
#include "pkg_algo_lz4.h"
#include "pkg_stream.h"
#include "pkg_utils.h"
#include "securec.h"

namespace hpackage {
constexpr uint32_t MAX_BUFFER_SIZE = 1 << 10;

int32_t PkgAlgorithm::ReadData(const PkgStreamPtr inStream, size_t offset, PkgBuffer &buffer,
    size_t &remainSize, size_t &readLen) const
{
    size_t readBytes = 0;
    size_t remainBytes = (remainSize > buffer.length) ? buffer.length : remainSize;
    if (remainBytes != 0) {
        int32_t ret = inStream->Read(buffer, offset, remainBytes, readBytes);
        PKG_CHECK(ret == PKG_SUCCESS, return ret, "fail deflate write data ");
    }
    remainSize -= readBytes;
    readLen = readBytes;
    return PKG_SUCCESS;
}

int32_t PkgAlgorithm::FinalDigest(DigestAlgorithm::DigestAlgorithmPtr algorithm,
    PkgAlgorithmContext &context, bool check) const
{
    if (context.digestMethod == PKG_DIGEST_TYPE_SHA256) {
        PkgBuffer digest(DIGEST_MAX_LEN);
        algorithm->Final(digest);
        if (check && memcmp(digest.buffer, context.digest, sizeof(digest)) != 0) {
            return PKG_INVALID_DIGEST;
        }
        PKG_CHECK(!memcpy_s(context.digest, sizeof(context.digest), digest.buffer, sizeof(digest)),
            return PKG_NONE_MEMORY, "FinalDigest memcpy failed");
    }
    return PKG_SUCCESS;
}

int32_t PkgAlgorithm::Pack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
    PkgAlgorithmContext &context)
{
    DigestAlgorithm::DigestAlgorithmPtr algorithm = PkgAlgorithmFactory::GetDigestAlgorithm(context.digestMethod);
    PKG_CHECK(algorithm != nullptr, return PKG_NOT_EXIST_ALGORITHM, "Can not get digest algor");
    algorithm->Init();

    PkgBuffer buffer(MAX_BUFFER_SIZE);
    int32_t ret = PKG_SUCCESS;
    size_t srcOffset = context.srcOffset;
    size_t destOffset = context.destOffset;
    size_t remainSize = context.unpackedSize;
    size_t readLen = 0;
    while (remainSize > 0) {
        ret = ReadData(inStream, srcOffset, buffer, remainSize, readLen);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Fail read data ");

        ret = outStream->Write(buffer, readLen, destOffset);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Fail write data ");

        algorithm->Update(buffer, readLen);
        srcOffset += readLen;
        destOffset += readLen;
    }

    FinalDigest(algorithm, context, true);

    PKG_CHECK(srcOffset - context.srcOffset == context.unpackedSize, return ret,
        "original size error %zu %zu", srcOffset, context.unpackedSize);
    context.packedSize = destOffset - context.destOffset;
    return ret;
}

int32_t PkgAlgorithm::Unpack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
    PkgAlgorithmContext &context)
{
    DigestAlgorithm::DigestAlgorithmPtr algorithm = PkgAlgorithmFactory::GetDigestAlgorithm(context.digestMethod);
    PKG_CHECK(algorithm != nullptr, return PKG_NOT_EXIST_ALGORITHM, "Can not get digest algor");
    algorithm->Init();
    PkgBuffer buffer(MAX_BUFFER_SIZE);
    int32_t ret = PKG_SUCCESS;
    size_t srcOffset = context.srcOffset;
    size_t destOffset = context.destOffset;
    size_t remainSize = context.packedSize;
    size_t readLen = 0;
    while (remainSize > 0) {
        ret = ReadData(inStream, srcOffset, buffer, remainSize, readLen);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Fail read data ");

        ret = outStream->Write(buffer, readLen, destOffset);
        PKG_CHECK(ret == PKG_SUCCESS, break, "Fail write data ");

        algorithm->Update(buffer, readLen);
        srcOffset += readLen;
        destOffset += readLen;
    }

    FinalDigest(algorithm, context, true);
    PKG_CHECK(destOffset - context.destOffset == context.packedSize, return ret,
        "original size error %zu %zu", destOffset, context.packedSize);
    context.unpackedSize = srcOffset - context.srcOffset;
    return ret;
}

PkgAlgorithm::PkgAlgorithmPtr PkgAlgorithmFactory::GetAlgorithm(const PkgManager::FileInfoPtr config)
{
    PKG_CHECK(config != nullptr, return nullptr, "Fail to get algorithm ");
    switch (config->packMethod) {
        case PKG_COMPRESS_METHOD_ZIP:
        case PKG_COMPRESS_METHOD_GZIP: {
            PKG_CHECK(config != nullptr, return nullptr, "Invalid param config");
            const ZipFileInfo *info = (ZipFileInfo *)config;
            return std::make_shared<PkgAlgoDeflate>(*info);
        }
        case PKG_COMPRESS_METHOD_NONE:
            return std::make_shared<PkgAlgorithm>();
        case PKG_COMPRESS_METHOD_LZ4: {
            PKG_CHECK(config != nullptr, return nullptr, "Invalid param config");
            const Lz4FileInfo *info = (Lz4FileInfo *)config;
            return std::make_shared<PkgAlgorithmLz4>(*info);
        }
        case PKG_COMPRESS_METHOD_LZ4_BLOCK: {
            PKG_CHECK(config != nullptr, return nullptr, "Invalid param config");
            const Lz4FileInfo *info = (Lz4FileInfo *)config;
            return std::make_shared<PkgAlgorithmBlockLz4>(*info);
        }
        default:
            break;
    }
    return nullptr;
}
} // namespace hpackage
