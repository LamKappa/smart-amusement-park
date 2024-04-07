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
#ifndef PKG_ALGORITHM_LZ4_H
#define PKG_ALGORITHM_LZ4_H

#include "lz4.h"
#include "lz4frame.h"
#include "lz4hc.h"
#include "pkg_algorithm.h"
#include "pkg_stream.h"
#include "pkg_utils.h"

namespace hpackage {
class PkgAlgorithmLz4 : public PkgAlgorithm {
public:
    static const uint32_t LZ4S_MAGIC_NUMBER = 0x184D2204;
    static const uint32_t LZ4S_SKIPPABLE0 = 0x184D2A50;
    static const uint32_t LZ4S_SKIPPABLE_MASK = 0xFFFFFFF0;
    static const uint32_t LZ4B_MAGIC_NUMBER = 0x184C2102;
    static const uint32_t LZ4B_BLOCK_SIZE = 1 << 22; // (4M)
    static const uint32_t LZ4S_HEADER_LEN = 20;

    explicit PkgAlgorithmLz4(const Lz4FileInfo &config);

    ~PkgAlgorithmLz4() override {}

    int32_t Pack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
        PkgAlgorithmContext &context) override;

    int32_t Unpack(const PkgStreamPtr inStream,
        const PkgStreamPtr outStream, PkgAlgorithmContext &context) override;

    void UpdateFileInfo(PkgManager::FileInfoPtr info) const override;

protected:
    int32_t GetBlockSizeFromBlockId(int32_t id) const
    {
        return (1 << (8 + (2 * id)));
    }
    int32_t GetPackParam(LZ4F_compressionContext_t &ctx, LZ4F_preferences_t &preferences,
        size_t &inBuffSize, size_t &outBuffSize) const;
    int32_t GetUnpackParam(LZ4F_decompressionContext_t &ctx,
        const PkgStreamPtr inStream, size_t &nextToRead, size_t &srcOffset);

    int32_t AdpLz4Compress(const uint8_t *src, uint8_t *dst, uint32_t srcSize, uint32_t dstCapacity) const;

    int32_t AdpLz4Decompress(const uint8_t *src, uint8_t *dst, uint32_t srcSize, uint32_t dstCapacity) const;

protected:
    int8_t compressionLevel_ {0};
    int8_t blockIndependence_ {0};
    int8_t contentChecksumFlag_ {0};
    int8_t blockSizeID_ {0};
    int8_t autoFlush_ {1};
};

class PkgAlgorithmBlockLz4 : public PkgAlgorithmLz4 {
public:
    static const uint32_t LZ4B_REVERSED_LEN = 4;
    explicit PkgAlgorithmBlockLz4(const Lz4FileInfo &config) : PkgAlgorithmLz4(config) {}

    ~PkgAlgorithmBlockLz4() override {}

    int32_t Pack(const PkgStreamPtr inStream,
        const PkgStreamPtr outStream, PkgAlgorithmContext &) override;

    int32_t Unpack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
        PkgAlgorithmContext &) override;
};
} // namespace hpackage
#endif
