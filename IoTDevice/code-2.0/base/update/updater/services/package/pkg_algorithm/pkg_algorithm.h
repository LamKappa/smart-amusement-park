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
#ifndef PKG_ALGORITHM_H
#define PKG_ALGORITHM_H

#include "pkg_algo_digest.h"
#include "pkg_algo_sign.h"
#include "pkg_stream.h"

namespace hpackage {
struct PkgContextOffset {
    size_t srcOffset;
    size_t destOffset;
    PkgContextOffset(size_t srcOffset, size_t destOffset)
    {
        this->srcOffset = srcOffset;
        this->destOffset = destOffset;
    }
};

struct PkgContextSize {
    size_t packedSize = 0;
    size_t unpackedSize = 0;
    PkgContextSize(size_t packedSize, size_t unpackedSize)
    {
        this->packedSize = packedSize;
        this->unpackedSize = unpackedSize;
    }
};

struct PkgAlgorithmContext {
    size_t srcOffset;
    size_t destOffset;
    size_t packedSize = 0;
    size_t unpackedSize = 0;
    uint32_t crc = 0;
    uint8_t digestMethod = 0;
    uint8_t digest[DIGEST_MAX_LEN];

    PkgAlgorithmContext(PkgContextOffset offset, PkgContextSize size, uint32_t crc, uint8_t digestMethod)
    {
        this->srcOffset = offset.srcOffset;
        this->destOffset = offset.destOffset;
        this->packedSize = size.packedSize;
        this->unpackedSize = size.unpackedSize;
        this->crc = crc;
        this->digestMethod = digestMethod;
    }
};

class PkgAlgorithm {
public:
    using PkgAlgorithmPtr = std::shared_ptr<PkgAlgorithm>;

    PkgAlgorithm() {}

    virtual ~PkgAlgorithm() {}

    virtual int32_t Pack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
        PkgAlgorithmContext &context);
    virtual int32_t Unpack(const PkgStreamPtr inStream, const PkgStreamPtr outStream,
        PkgAlgorithmContext &context);

    virtual void UpdateFileInfo(PkgManager::FileInfoPtr info) const
    {
        (void)info;
    }
protected:
    int32_t FinalDigest(DigestAlgorithm::DigestAlgorithmPtr algorithm,
        PkgAlgorithmContext &context, bool check) const;
    int32_t ReadData(const PkgStreamPtr inStream,
        size_t offset, PkgBuffer &buffer, size_t &size, size_t &readLen) const;
};

class PkgAlgorithmFactory {
public:
    static PkgAlgorithm::PkgAlgorithmPtr GetAlgorithm(const PkgManager::FileInfoPtr config = nullptr);

    static DigestAlgorithm::DigestAlgorithmPtr GetDigestAlgorithm(uint8_t type);

    static SignAlgorithm::SignAlgorithmPtr GetSignAlgorithm(const std::string &path, uint8_t signMethod,
        uint8_t type);

    static SignAlgorithm::SignAlgorithmPtr GetVerifyAlgorithm(const std::string &path, uint8_t type);
};
} // namespace hpackage
#endif
